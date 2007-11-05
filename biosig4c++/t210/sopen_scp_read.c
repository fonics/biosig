/*

    $Id: sopen_scp_read.c,v 1.46 2007-11-05 22:01:44 schloegl Exp $
    Copyright (C) 2005,2006,2007 Alois Schloegl <a.schloegl@ieee.org>
    This function is part of the "BioSig for C/C++" repository 
    (biosig4c++) at http://biosig.sf.net/ 


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */


// 
#define EXPERIMENTAL    // use experimental version of decompressing Huffman, Bimodal, reference beat  
/*
	the experimental version needs a few more thinks: 
	- huffman decoding does not seem to work, yet

	- validation and testing  
*/


#include <stdio.h>             // system includes
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#include "../biosig.h"

#include "structures.h"
static const U_int_S _NUM_SECTION=12U;	//consider first 11 sections of SCP
static bool add_filter=true;             // additional filtering gives better shape, but use with care
int scp_decode(HDRTYPE*, pointer_section*, DATA_DECODE&, DATA_RECORD&, DATA_INFO&, bool&);

// Huffman Tables         	
#define HUFFMAN_H
#ifdef HUFFMAN_H
uint16_t NHT; 	/* number of Huffman tables */
struct table_t {
	uint8_t PrefixLength;
       	uint8_t CodeLength;
       	uint8_t TableModeSwitch;
       	int16_t BaseValue;
       	uint32_t BaseCode;
}; 
struct huffman_t {
      	uint16_t NCT; 	/* number of Code structures in Table #1 */
       	table_t *Table; 
} *Huffman;

struct htree_t {
	htree_t* child0;
	htree_t* child1;
	uint16_t idxTable;
} **HTrees;

table_t DefaultTable[19] = {
	{ 1,  1, 1, 0, 0 },
	{ 3,  3, 1, 1, 1 },
	{ 3,  3, 1,-1, 5 },
	{ 4,  4, 1, 2, 3 },
	{ 4,  4, 1,-2, 11},
	{ 5,  5, 1, 3, 7 },
	{ 5,  5, 1,-3, 23},
	{ 6,  6, 1, 4, 15},
	{ 6,  6, 1,-4, 47},
	{ 7,  7, 1, 5, 31},
	{ 7,  7, 1,-5, 95},
	{ 8,  8, 1, 6, 63},
	{ 8,  8, 1,-6, 191},
	{ 9,  9, 1, 7, 127},
	{ 9,  9, 1,-7, 383},
	{10, 10, 1, 8, 255},
	{10, 10, 1,-8, 767},
	{18, 10, 1, 0, 511},
	{26, 10, 1, 0, 1023}
};

/*
	This structure defines the fields used for "Annotated ECG" 
 */
struct en1064_t {
	char*		test;		/* test field for annotated ECG */
	
	float		diastolicBloodPressure;				
	float		systolicBloodPressure;	
	char*		MedicationDrugs;
	char*		ReferringPhysician;
	char*		LatestConfirmingPhysician;
	char*		Diagnosis;
	uint8_t		EmergencyLevel; /* 0: routine 1-10: increased emergency level */

	float		HeartRate;	
	float		P_wave[2]; 	/* start and end  */
	float		QRS_wave[2]; 	/* start and end  */
	float		T_wave[2]; 	/* start and end  */
	float		P_QRS_T_axes[3];

	/***** SCP only fields *****/
	struct {	
		uint8_t	HUFFMAN;	
		uint8_t	REF_BEAT;
		uint8_t	DIFF;// OBSOLETE
		uint8_t	BIMODAL;// OBSOLETE
	} FLAG;
        struct {
		//uint8_t tag14[41],tag15[41];
	        struct {
			uint16_t INST_NUMBER;		/* tag 14, byte 1-2  */
			uint16_t DEPT_NUMBER;		/* tag 14, byte 3-4  */
			uint16_t DEVICE_ID;		/* tag 14, byte 5-6  */
			uint8_t  DEVICE_TYPE;		/* tag 14, byte 7: 0: Cart, 1: System (or Host)  */
			uint8_t MANUF_CODE;		/* tag 14, byte 8 (MANUF_CODE has to be 255) */
			char*   MOD_DESC;		/* tag 14, byte 9 (MOD_DESC has to be "Cart1") */
			uint8_t VERSION;		/* tag 14, byte 15 (VERSION has to be 20) */
			uint8_t PROT_COMP_LEVEL;	/* tag 14, byte 16 (PROT_COMP_LEVEL has to be 0xA0 => level II) */
			uint8_t LANG_SUPP_CODE;		/* tag 14, byte 17 (LANG_SUPP_CODE has to be 0x00 => Ascii only, latin and 1-byte code) */
			uint8_t ECG_CAP_DEV;		/* tag 14, byte 18 (ECG_CAP_DEV has to be 0xD0 => Acquire, (No Analysis), Print and Store) */
			uint8_t MAINS_FREQ;		/* tag 14, byte 19 (MAINS_FREQ has to be 0: unspecified, 1: 50 Hz, 2: 60Hz) */
			char 	reserved[22]; 		/* char[35-19] reserved; */			
			char* 	ANAL_PROG_REV_NUM;
			char* 	SERIAL_NUMBER_ACQ_DEV;
			char* 	ACQ_DEV_SYS_SW_ID;
			char* 	ACQ_DEV_SCP_SW; 	/* tag 14, byte 38 (SCP_IMPL_SW has to be "OpenECG XML-SCP 1.00") */
			char* 	ACQ_DEV_MANUF;		/* tag 14, byte 38 (ACQ_DEV_MANUF has to be "Manufacturer") */
        	} Tag14, Tag15; 
        } Section1;
        struct {
        } Section2;
        struct {
        	uint8_t NS, flags;
        	struct { 
        		uint32_t start;
        		uint32_t end;
//        		uint8_t  id;
        	} *lead;
        } Section3;
        struct {
        	uint16_t len_ms, fiducial_sample, N;
       		uint32_t SPR;
        	struct { 
        		uint16_t btyp;
        		uint32_t SB;
        		uint32_t fcM;
        		uint32_t SE;
        		uint32_t QB;
        		uint32_t QE;
        	} *beat;
        } Section4;
        struct {
        	size_t   StartPtr;
        	size_t	 Length;
        	uint16_t AVM, dT_us;
        	uint8_t  DIFF; //diff: see FLAG 
        	uint16_t *inlen;
        	int32_t  *datablock;
        } Section5;
        struct {
        	size_t   StartPtr;
        	size_t	 Length;
        	uint16_t AVM, dT_us;
        	uint8_t  DIFF, BIMODAL; //diff, bimodal: see FLAG 
        	uint16_t *inlen;
        	int32_t  *datablock;
        } Section6;
} en1064;

/* new node in Huffman tree */
htree_t* newNode() {
	htree_t* T  = (htree_t*) malloc(sizeof(htree_t));
	T->child0   = NULL;
	T->child1   = NULL;
	T->idxTable = 0; 
	return(T);
}

/* check Huffman tree */
int checkTree(htree_t *T) {
	int v,v1,v2,v3; 
	
	v1 = (T->child0 == NULL) && (T->child0 == NULL) && (T->idxTable > 0);
	v2 = (T->idxTable == 0) && (T->child0 != NULL) && checkTree(T->child0);
	v3 = (T->idxTable == 0) && (T->child1 != NULL) && checkTree(T->child1);
	v = v1 || v2 || v3;
	if (!v) fprintf(stderr,"Warning: Invalid Node in Huffman Tree: %i %p %p\n",T->idxTable,T->child0,T->child1);
	return(v);
}

/* convert Huffman Table into a Huffman tree */
htree_t* makeTree(huffman_t HT) {
	uint16_t k1,k2;
	htree_t* T = newNode();
	htree_t* node;
	for (k1=0; k1<HT.NCT; k1++) {
		node = T; 
//fprintf(stdout,"%2i\t %2i %2i %2i %4i %5li\n",k1,HT.Table[k1].PrefixLength,HT.Table[k1].CodeLength,HT.Table[k1].TableModeSwitch,HT.Table[k1].BaseValue,HT.Table[k1].BaseCode);
		uint32_t bc = HT.Table[k1].BaseCode;
		for (k2=0; k2<HT.Table[k1].CodeLength; k2++, bc>>=1) {
			if (bc & 0x00000001) {
				if (node->child1==NULL) node->child1 = newNode();
				node = node->child1;
			}
			else {
				if (node->child0==NULL) node->child0 = newNode();
				node = node->child0;
			}
		}
		node->idxTable = k1+1;
	}
	return(T); 
}

/* get rid of Huffman tree */
void freeTree(htree_t* T) {
	if (T->child0 != NULL) freeTree(T->child0);
	if (T->child1 != NULL) freeTree(T->child1);
	free(T);
}

int DecodeHuffman(htree_t *HTrees[], huffman_t *HuffmanTables, uint8_t* indata, size_t inlen, int32_t* outdata, size_t outlen) {
	uint16_t ActualTable = 0; 
	htree_t *node; 
	size_t k1, k2, i;
	uint32_t acc; 
	int8_t dlen,k3,r;

	k1=0, k2=0;
	node = HTrees[ActualTable];
	r = 0; i = 0;
	while ((k1 < inlen*8) && (k2 < outlen)) {
		r = k1 % 8; 
		i = k1 / 8;

		if (!node->idxTable) {
			if (indata[i] & (1<<(7-r))) {
				if (node->child1 != NULL)
					node = node->child1;
				else {
					B4C_ERRMSG = "Empty node in Huffman table! Do not what to!\n";
					B4C_ERRNUM = B4C_DECOMPRESSION_FAILED;					
					return(-1);
				}	
			}	
			else {
				if (node->child0 != NULL)
					node = node->child0;
				else {
					B4C_ERRMSG = "Empty node in Huffman table! Do not what to!\n";
					B4C_ERRNUM = B4C_DECOMPRESSION_FAILED;					
					return(-1);
				}	
			}	
		++k1;
		}

		r = k1 % 8; 
		i = k1 / 8;

		if (node->idxTable) {
			// leaf of tree reached
			table_t TableEntry = HuffmanTables[ActualTable].Table[node->idxTable - 1]; 
			dlen = TableEntry.PrefixLength - TableEntry.CodeLength;
			if (!TableEntry.TableModeSwitch)
				// switch Huffman Code 
				ActualTable = TableEntry.BaseValue;
			else if (dlen) {
				// no compression
				acc = 0;  //(uint32_t)(indata[i]%(1<<r)); 
				for (k3=0; k3*8-r < dlen; k3++)
					acc = (acc<<8)+(uint32_t)indata[i+k3];
				
				outdata[k2] = (acc >> (k3*8 - r - dlen)) & ((1L << dlen) - 1L) ; 
				if (outdata[k2] >= (1<<dlen-1))
					outdata[k2] -= 1<<dlen;
				k1 += dlen; 
				++k2;
			}
			else {
				// lookup Huffman Table 
				outdata[k2++] = TableEntry.BaseValue; 
			}
			// reset node to root
			node = HTrees[ActualTable];
//if (k2<3) fprintf(stdout,"%i %i %i %i %i %li %li %li \n",i,r,k1,k2,node->idxTable,dlen,acc,outdata[k2-1]);
		}
	}
	return(0);
};
#endif 



int sopen_SCP_read(HDRTYPE* hdr) {	
/*
	this function is a stub or placeholder and need to be defined in order to be useful. 
	It will be called by the function SOPEN in "biosig.c"

	Input: 
		char* Header	// contains the file content
		
	Output: 
		HDRTYPE *hdr	// defines the HDR structure accoring to "biosig.h"
*/	

	struct pointer_section section[_NUM_SECTION];
	struct DATA_DECODE decode;
	struct DATA_RECORD record;
	struct DATA_INFO textual;
	bool AS_DECODE = 0; 


	uint8_t*	ptr; 	// pointer to memory mapping of the file layout
	uint8_t*	PtrCurSect;	// point to current section 
	uint8_t*	Ptr2datablock=NULL; 	// pointer to data block 
	int32_t* 	data;		// point to rawdata
	int		curSect; 	// current section
	uint32_t 	len; 
	uint16_t 	crc; 
	uint32_t	i,k1,k2; 
	size_t		curSectPos;
	size_t 		sectionStart; 
	int 		NSections = 12;
	uint8_t		tag;
	float 		HighPass=0, LowPass=1.0/0.0, Notch=-1; 	// filter settings
	uint16_t	Cal5=0,Cal6=0;
	uint16_t 	dT_us = 1000; 	// sampling interval in microseconds

	/* 
	   Try direct conversion SCP->HDR to internal data structure
		+ whole data is loaded once, then no further File I/O is needed. 
		- currently Huffman and Bimodal compression is not supported. 
	*/	

	if (hdr->aECG == NULL) {
		hdr->aECG = (aECG_TYPE*)malloc(sizeof(aECG_TYPE));
		hdr->aECG->diastolicBloodPressure=0.0;				 
		hdr->aECG->systolicBloodPressure=0.0;
		hdr->aECG->MedicationDrugs = "\0";
		hdr->aECG->ReferringPhysician="\0";
		hdr->aECG->LatestConfirmingPhysician="\0";
		hdr->aECG->Diagnosis="\0";
		hdr->aECG->EmergencyLevel=0;
		hdr->ID.Technician = "nobody";
	}
	hdr->aECG->Section1.Tag14.VERSION = 0; // acquiring.protocol_revision_number 
	hdr->aECG->Section1.Tag15.VERSION = 0; // analyzing.protocol_revision_number
	hdr->aECG->FLAG.HUFFMAN  = 0; 
	hdr->aECG->FLAG.DIFF     = 0; 
	hdr->aECG->FLAG.REF_BEAT = 0; 
	hdr->aECG->FLAG.BIMODAL  = 0;
	
#ifndef EXPERIMENTAL
	decode.length_BdR0 = NULL; 	
	decode.samples_BdR0= NULL;
	decode.length_Res  = NULL;
	decode.samples_Res = NULL;
	decode.t_Huffman=NULL;
	decode.flag_Huffman=NULL;
	decode.data_lead=NULL;
	decode.data_protected=NULL;
	decode.data_subtraction=NULL;
	decode.length_BdR0=NULL;
	decode.samples_BdR0=NULL;
	decode.Median=NULL;
	decode.length_Res=NULL;
	decode.samples_Res=NULL;
	decode.Residual=NULL;
	decode.Reconstructed=NULL;

	//variables inizialization
	decode.flag_lead.number=0;
	decode.flag_lead.subtraction=0;
	decode.flag_lead.all_simultaneously=0;
	decode.flag_lead.number_simultaneously=0;

	decode.flag_BdR0.length=0;
	decode.flag_BdR0.fcM=0;
	decode.flag_BdR0.AVM=0;
	decode.flag_BdR0.STM=0;
	decode.flag_BdR0.number_samples=0;
	decode.flag_BdR0.encoding=0;

	decode.flag_Res.AVM=0;
	decode.flag_Res.STM=0;
	decode.flag_Res.number=0;
	decode.flag_Res.number_samples=0;
	decode.flag_Res.encoding=0;
	decode.flag_Res.bimodal=0;
	decode.flag_Res.decimation_factor=0;
#endif 
	
	ptr = hdr->AS.Header; 
	hdr->NRec = 1;

	sectionStart = 6;
	PtrCurSect = ptr+sectionStart;	

	/**** SECTION 0 ****/
	len = l_endian_u32(*(uint32_t*)(PtrCurSect+4)); 
	NSections = (len-16)/10;
	section[0].ID	  = 0; 	
	section[0].length = len; 	
	section[0].index  = 6+16;
	for (int K=1; K<NSections; K++)	{
		curSect 	= l_endian_u16(*(uint16_t*)(ptr+6+16+K*10));
		len 		= l_endian_u32(*(uint32_t*)(ptr+6+16+K*10+2));
		sectionStart 	= l_endian_u32(*(uint32_t*)(ptr+6+16+K*10+6))-1;
		if (K < _NUM_SECTION) {
			section[K].ID	  = curSect; 	
			section[K].length = len; 	
			section[K].index  = sectionStart+1;
		}
		 	
	if (VERBOSE_LEVEL>8)
		fprintf(stdout,"SCP Section %i %i len=%i secStart=%i HeaderLength=%i\n",K,curSect,len,sectionStart,hdr->HeadLen);
		
	if (len==0) continue;	 /***** empty section *****/
		
		PtrCurSect = ptr+sectionStart;
		crc 	   = l_endian_u16(*(uint16_t*)(PtrCurSect));
		uint16_t tmpcrc = CRCEvaluate((uint8_t*)(PtrCurSect+2),len-2); 
		if ((crc != 0xffff) && (crc != tmpcrc))
			fprintf(stderr,"Warning SOPEN(SCP-READ): faulty CRC in section %i: crc=%x, %x\n",curSect,crc,tmpcrc);
		if (curSect != l_endian_u16(*(uint16_t*)(PtrCurSect+2)))
			fprintf(stderr,"Warning SOPEN(SCP-READ): Current Section No does not match field in sections (%i %i)\n",curSect,l_endian_u16(*(uint16_t*)(PtrCurSect+2))); 
		if (len != l_endian_u32(*(uint32_t*)(PtrCurSect+4)))
			fprintf(stderr,"Warning SOPEN(SCP-READ): length field in pointer section (%i) does not match length field in sections (%i %i)\n",K,len,l_endian_u32(*(uint32_t*)(PtrCurSect+4))); 

		uint8_t versionSection  = *(ptr+sectionStart+8);
		uint8_t versionProtocol = *(ptr+sectionStart+9);

		curSectPos = 16;
			
		/**** SECTION 0 ****/
		if (curSect==0)  
		{
		}		
				
		/**** SECTION 1 ****/
		else if (curSect==1)  
		{
			struct tm t0,t1;
			t0.tm_year = 0; 
			t0.tm_mon  = 0; 
			t0.tm_mday = 0; 
			t0.tm_hour = 0; 
			t0.tm_min  = 0; 
			t0.tm_sec  = 0; 
			t0.tm_isdst  = -1; // daylight savings time - unknown 
			hdr->T0    = 0; 
			hdr->Patient.Birthday = 0; 
			uint32_t len1; 
 
			while (*(PtrCurSect+curSectPos) < 255) {
				tag = *(PtrCurSect+curSectPos);
				len1 = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+1));
				if (VERBOSE_LEVEL>8)
					fprintf(stdout,"SCP(r): Section 1 Tag %i Len %i\n",tag,len1);

				curSectPos += 3;
				if (curSectPos+len1 > len) {
					fprintf(stdout,"Warning SCP(read): section 1 corrupted (exceeds file length)\n");
			break;
				} 	 
				if (tag==0) {
					if (!hdr->FLAG.ANONYMOUS)
						strncpy(hdr->Patient.Name, (char*)(PtrCurSect+curSectPos),min(len1,MAX_LENGTH_NAME));
				}		
				else if (tag==1) {
//					hdr->Patient.FirstName = (char*)(PtrCurSect+curSectPos);
				}
				else if (tag==2) {
					if (len1>MAX_LENGTH_PID) {
						fprintf(stdout,"Warning SCP(read): length of Patient Id (section1 tag2) exceeds %i>%i\n",len1,MAX_LENGTH_PID); 
					}	
					strncpy(hdr->Patient.Id,(char*)(PtrCurSect+curSectPos),min(len1,MAX_LENGTH_PID));

					if (!strcmp(hdr->Patient.Id,"UNKNOWN")) 
						hdr->Patient.Id[0] = 0;
				}
				else if (tag==3) {
				}
				else if (tag==4) {
				}
				else if (tag==5) {
					t1.tm_year = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos))-1900;
					t1.tm_mon  = *(PtrCurSect+curSectPos+2)-1;
					t1.tm_mday = *(PtrCurSect+curSectPos+3);
					t1.tm_hour = 12; 
					t1.tm_min  =  0; 
					t1.tm_sec  =  0; 
					t1.tm_isdst= -1; // daylight saving time: unknown
//					t1.tm_gmtoff  =  0; 
					hdr->Patient.Birthday = tm_time2gdf_time(&t1);
				}
				else if (tag==6) {
					hdr->Patient.Height = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
				}
				else if (tag==7) {
					hdr->Patient.Weight = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
				}
				else if (tag==8) {
					hdr->Patient.Sex = *(PtrCurSect+curSectPos);
				}
				else if (tag==9) {
				}
				else if (tag==10) {
				}
				else if (tag==11) {
 					hdr->aECG->diastolicBloodPressure = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
				}
				else if (tag==12) {
					hdr->aECG->systolicBloodPressure  = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
				}
				else if (tag==13) {
					hdr->aECG->Diagnosis = (char*)(PtrCurSect+curSectPos);
				}
				else if (tag==14) {
					if (len1>80)
						fprintf(stderr,"Warning SCP(r): length of tag14 %i>40\n",len1);
					memcpy(hdr->ID.Manufacturer._field,(char*)PtrCurSect+curSectPos,min(len1,MAX_LENGTH_MANUF));
					hdr->ID.Manufacturer._field[min(len1,MAX_LENGTH_MANUF)] = 0;
					hdr->ID.Manufacturer.Model = hdr->ID.Manufacturer._field+8;  
					hdr->ID.Manufacturer.Version = hdr->ID.Manufacturer._field+36;  
					int tmp = strlen(hdr->ID.Manufacturer.Version)+1;
					hdr->ID.Manufacturer.SerialNumber = hdr->ID.Manufacturer.Version+tmp;
					tmp += strlen(hdr->ID.Manufacturer.Version+tmp)+1;	// skip SW ID
					tmp += strlen(hdr->ID.Manufacturer.Version+tmp)+1;	// skip SW
					tmp += strlen(hdr->ID.Manufacturer.Version+tmp)+1;	// skip SW
					hdr->ID.Manufacturer.Name = hdr->ID.Manufacturer.Version+tmp;
					
					/* might become obsolete */					
					//memcpy(hdr->aECG->Section1.tag14,PtrCurSect+curSectPos,40);
					//hdr->VERSION = *(PtrCurSect+curSectPos+14)/10.0;	// tag 14, byte 15
					hdr->aECG->Section1.Tag14.INST_NUMBER = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
					hdr->aECG->Section1.Tag14.DEPT_NUMBER = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+2));
					hdr->aECG->Section1.Tag14.DEVICE_ID   = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+4));
					hdr->aECG->Section1.Tag14.DEVICE_TYPE = *(PtrCurSect+curSectPos+ 6);
					hdr->aECG->Section1.Tag14.MANUF_CODE  = *(PtrCurSect+curSectPos+ 7);	// tag 14, byte 7 (MANUF_CODE has to be 255)
					hdr->aECG->Section1.Tag14.MOD_DESC    = (char*)(PtrCurSect+curSectPos+8); 
					hdr->aECG->Section1.Tag14.VERSION     = *(PtrCurSect+curSectPos+14);
					hdr->aECG->Section1.Tag14.PROT_COMP_LEVEL = *(PtrCurSect+curSectPos+15); 	// tag 14, byte 15 (PROT_COMP_LEVEL has to be 0xA0 => level II)
					hdr->aECG->Section1.Tag14.LANG_SUPP_CODE  = *(PtrCurSect+curSectPos+16);	// tag 14, byte 16 (LANG_SUPP_CODE has to be 0x00 => Ascii only, latin and 1-byte code)
					hdr->aECG->Section1.Tag14.ECG_CAP_DEV     = *(PtrCurSect+curSectPos+17);	// tag 14, byte 17 (ECG_CAP_DEV has to be 0xD0 => Acquire, (No Analysis), Print and Store)
					hdr->aECG->Section1.Tag14.MAINS_FREQ      = *(PtrCurSect+curSectPos+18);	// tag 14, byte 18 (MAINS_FREQ has to be 0: unspecified, 1: 50 Hz, 2: 60Hz)

					hdr->aECG->Section1.Tag14.ANAL_PROG_REV_NUM = (char*)(PtrCurSect+curSectPos+36);
					tmp = strlen((char*)(PtrCurSect+curSectPos+36));					
					hdr->aECG->Section1.Tag14.SERIAL_NUMBER_ACQ_DEV = (char*)(PtrCurSect+curSectPos+36+tmp+1);
					tmp += strlen((char*)(PtrCurSect+curSectPos+36+tmp+1));					
					hdr->aECG->Section1.Tag14.ACQ_DEV_SYS_SW_ID = (char*)(PtrCurSect+curSectPos+36+tmp+1);
					tmp += strlen((char*)(PtrCurSect+curSectPos+36+tmp+1));					
					hdr->aECG->Section1.Tag14.ACQ_DEV_SCP_SW = (char*)(PtrCurSect+curSectPos+36+tmp+1); 	// tag 14, byte 38 (SCP_IMPL_SW has to be "OpenECG XML-SCP 1.00")
					tmp += strlen((char*)(PtrCurSect+curSectPos+36+tmp+1));
					hdr->aECG->Section1.Tag14.ACQ_DEV_MANUF  = (char*)(PtrCurSect+curSectPos+36+tmp+1);	// tag 14, byte 38 (ACQ_DEV_MANUF has to be "Manufacturer")
				}
				else if (tag==15) {
					//memcpy(hdr->aECG->Section1.tag15,PtrCurSect+curSectPos,40);
					hdr->aECG->Section1.Tag15.VERSION     = *(PtrCurSect+curSectPos+14);
				}
				else if (tag==16) {
				}
				else if (tag==17) {
				}
				else if (tag==18) {
				}
				else if (tag==19) {
				}
				else if (tag==20) {
					hdr->aECG->ReferringPhysician = (char*)(PtrCurSect+curSectPos);
				}
				else if (tag==21) {
					hdr->aECG->MedicationDrugs = (char*)(PtrCurSect+curSectPos);
				}
				else if (tag==22) {
				}
				else if (tag==23) {
				}
				else if (tag==24) {
					hdr->aECG->EmergencyLevel = *(PtrCurSect+curSectPos);
				}
				else if (tag==25) {
					t0.tm_year = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos))-1900;
					t0.tm_mon  = (*(PtrCurSect+curSectPos+2)) - 1;
					t0.tm_mday = *(PtrCurSect+curSectPos+3);
				}
				else if (tag==26) {
					t0.tm_hour = *(PtrCurSect+curSectPos);
					t0.tm_min  = *(PtrCurSect+curSectPos+1);
					t0.tm_sec  = *(PtrCurSect+curSectPos+2);
				}
				else if (tag==27) {
					HighPass   = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos))/100.0;
				}
				else if (tag==28) {
					LowPass    = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
				}
				else if (tag==29) {
					uint8_t bitmap = *(PtrCurSect+curSectPos);
					if (bitmap==0)
						Notch = NaN;	// undefined 
					else if ((bitmap & 0x03)==0)
						Notch = -1;	// notch off
					else if (bitmap & 0x01)
						Notch = 60.0; 	// notch 60Hz
					else if (bitmap & 0x02)
						Notch = 50.0; 	// notch 50Hz
				}
				else if (tag==30) {
				}
 				else if (tag==31) {
				}
				else if (tag==32) {
				}
				else if (tag==33) {
				}
				else if (tag==34) {
					int16_t tzmin = l_endian_i16(*(int16_t*)(PtrCurSect+curSectPos));
					if (tzmin != 0x7fff)
						if (abs(tzmin)<=780) 
							t0.tm_min += tzmin;
						else 
							fprintf(stderr,"Warning SOPEN(SCP-READ): invalid time zone (Section 1, Tag34)\n");
					//fprintf(stdout,"SOPEN(SCP-READ): tzmin = %i %x \n",tzmin,tzmin);
				}
				else {
				}
				curSectPos += len1;
			}
//			t0.tm_gmtoff = 60*tzminutes;
			t0.tm_isdst = -1;
			hdr->T0     = tm_time2gdf_time(&t0);
		}

		/**** SECTION 2 ****/
		else if (curSect==2)  {
			hdr->aECG->FLAG.HUFFMAN = 1; 
			en1064.FLAG.HUFFMAN = 1; 

			NHT = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
			curSectPos += 2;
			if (NHT==19999) {
				en1064.FLAG.HUFFMAN = 1; 
				Huffman = (huffman_t*)malloc(sizeof(huffman_t));
				HTrees  = (htree_t**)malloc(sizeof(htree_t*));
				Huffman[0].NCT   = 19;
				Huffman[0].Table = DefaultTable;
				HTrees [0] = makeTree(Huffman[0]);
				if (!checkTree(HTrees[0]))
					fprintf(stderr,"Warning: invalid Huffman Tree\n");
			}
			else {
				en1064.FLAG.HUFFMAN = NHT; 
				Huffman = (huffman_t*)malloc(NHT*sizeof(huffman_t));
				for (k2=0; k2<NHT; k2++) {
					Huffman[k2].NCT   = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
					curSectPos += 2;
					Huffman[k2].Table = (typeof(Huffman[k2].Table))malloc(Huffman[k2].NCT * sizeof(*Huffman[k2].Table));
					HTrees      = (htree_t**)malloc(Huffman[k2].NCT*sizeof(htree_t*));
					for (k1=0; k1<Huffman[k2].NCT; k1++) {
						Huffman[k2].Table[k1].PrefixLength = *(PtrCurSect+curSectPos);  
						Huffman[k2].Table[k1].CodeLength = *(PtrCurSect+curSectPos+1);  
						Huffman[k2].Table[k1].TableModeSwitch = *(PtrCurSect+curSectPos+2);  
						Huffman[k2].Table[k1].BaseValue  = l_endian_i16(*(int16_t*)(PtrCurSect+curSectPos+3));  
						Huffman[k2].Table[k1].BaseCode   = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos+5));  
						curSectPos += 9;
//fprintf(stdout,"HT%i %i\t %i %i %i %i %li\n ",k2,k1,Huffman[k2].Table[k1].PrefixLength,Huffman[k2].Table[k1].CodeLength,Huffman[k2].Table[k1].TableModeSwitch,Huffman[k2].Table[k1].BaseValue,Huffman[k2].Table[k1].BaseCode);
					}
					HTrees[k2] = makeTree(Huffman[k2]);
					if (!checkTree(HTrees[k2]))
						fprintf(stderr,"Warning: invalid Huffman Tree\n");
				}
			}
		}

		/**** SECTION 3 ****/
		else if (curSect==3)  
		{
			hdr->NS = *(PtrCurSect+curSectPos);
			hdr->aECG->FLAG.REF_BEAT = (*(PtrCurSect+curSectPos+1) & 0x01);
			en1064.FLAG.REF_BEAT = (*(PtrCurSect+curSectPos+1) & 0x01);
			en1064.Section3.flags = *(PtrCurSect+curSectPos+1);
			if (en1064.FLAG.REF_BEAT && !section[4].length)
				fprintf(stderr,"Warning (SCP): Reference Beat but no Section 4\n");
			if (!(en1064.Section3.flags & 0x04) || ((en1064.Section3.flags>>3) != hdr->NS))
				fprintf(stderr,"Warning (SCP): channels are not simultaneously recorded! %x %i\n",en1064.Section3.flags,hdr->NS);

			curSectPos += 2;
			hdr->CHANNEL = (CHANNEL_TYPE *) calloc(hdr->NS, sizeof(CHANNEL_TYPE));
			en1064.Section3.lead = (typeof(en1064.Section3.lead))malloc(hdr->NS*sizeof(*en1064.Section3.lead));

			uint32_t startindex0; 
			startindex0 = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos));
			for (i = 0, hdr->SPR=1; i < hdr->NS; i++) {
				en1064.Section3.lead[i].start = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos));
				en1064.Section3.lead[i].end   = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos+4));

				hdr->CHANNEL[i].SPR 	= en1064.Section3.lead[i].end - en1064.Section3.lead[i].start + 1;
				hdr->SPR 		= lcm(hdr->SPR,hdr->CHANNEL[i].SPR);
				hdr->CHANNEL[i].LeadIdCode =  *(PtrCurSect+curSectPos+8);
				hdr->CHANNEL[i].Label[0]= 0;
				hdr->CHANNEL[i].LowPass = LowPass; 
				hdr->CHANNEL[i].HighPass= HighPass; 
				hdr->CHANNEL[i].Notch 	= Notch; 
				curSectPos += 9;

				if (en1064.Section3.lead[i].start != startindex0)
					fprintf(stderr,"Warning SCP(read): starting sample %i of #%i differ to %x in #1\n",en1064.Section3.lead[i].start,*(PtrCurSect+curSectPos+8),startindex0);
			}
		}
		/**** SECTION 4 ****/
		else if (curSect==4)  {
			en1064.Section4.beat	= (typeof(en1064.Section4.beat))malloc(hdr->NS*sizeof(*en1064.Section4.beat));

			en1064.Section4.len_ms	= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
			en1064.Section4.fiducial_sample	= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+2));
			en1064.Section4.N	= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+4));
			en1064.Section4.SPR	= hdr->SPR/4;

			curSectPos += 6;
			for (i=0; i < en1064.Section4.N; i++) {
				en1064.Section4.beat[i].btyp = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
				en1064.Section4.beat[i].SB   = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos+2));
				en1064.Section4.beat[i].fcM  = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos+6));
				en1064.Section4.beat[i].SE   = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos+10));
				curSectPos += 14;
			}	
			for (i=0; i < en1064.Section4.N; i++) {
				en1064.Section4.beat[i].QB   = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos));
				en1064.Section4.beat[i].QE   = l_endian_u32(*(uint32_t*)(PtrCurSect+curSectPos+4));
				curSectPos += 8;
				en1064.Section4.SPR += en1064.Section4.beat[i].QE-en1064.Section4.beat[i].QB-1;
			}	
		}

		/**** SECTION 5 ****/
		else if (curSect==5)  {
			Cal5 			= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
			en1064.Section5.AVM	= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
			en1064.Section5.dT_us	= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+2));
			en1064.Section5.DIFF 	= *(PtrCurSect+curSectPos+4);

			if (en1064.Section5.Length==0) 
				en1064.FLAG.REF_BEAT = 0;
			else {
				en1064.Section5.Length  = 1000L * en1064.Section4.len_ms / en1064.Section5.dT_us; // hdr->SPR;
				en1064.Section5.inlen	= (typeof(en1064.Section5.inlen))malloc(hdr->NS*sizeof(*en1064.Section5.inlen));
				en1064.Section5.datablock = (int32_t*)malloc(4 * hdr->NS * en1064.Section5.Length); 

				Ptr2datablock           = (PtrCurSect+curSectPos+6+2*hdr->NS);
				for (i=0; i < hdr->NS; i++) {
					en1064.Section5.inlen[i] = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+6+2*i));
					if (en1064.FLAG.HUFFMAN) {
						DecodeHuffman(HTrees, Huffman, Ptr2datablock, en1064.Section5.inlen[i], en1064.Section5.datablock + en1064.Section5.Length*i, en1064.Section5.Length);
					}
					else {
						for (k1=0; k1<en1064.Section5.Length; k1++)
							en1064.Section5.datablock[i*en1064.Section5.Length+k1] = l_endian_i16(*(int16_t*)(Ptr2datablock + 2*(i*en1064.Section5.Length + k1)));
					}
					Ptr2datablock += en1064.Section5.inlen[i];
				}	
				size_t ix;
				data = en1064.Section5.datablock;
				if (en1064.Section5.DIFF==1)
					for (k1 = 0; k1 < hdr->NS; k1++)
					for (ix = k1*en1064.Section5.Length+1; ix < (k1+1)*en1064.Section5.Length; ix++)
						data[ix] += data[ix-1];
	
				else if (en1064.Section5.DIFF==2)
					for (k1 = 0; k1 < hdr->NS; k1++)
					for (ix = k1*en1064.Section5.Length+2; ix < (k1+1)*en1064.Section5.Length; ix++)
						data[ix] += 2*data[ix-1] - data[ix-2];
			}
		}
		
		/**** SECTION 6 ****/
		else if (curSect==6)  {

			en1064.Section6.inlen	= (typeof(en1064.Section6.inlen))malloc(hdr->NS*sizeof(*en1064.Section6.inlen));
			uint16_t gdftyp 	= 5;	// int32: internal raw data type   
			hdr->AS.rawdata = (uint8_t*)malloc(4 * hdr->NS * hdr->SPR * hdr->NRec); 
			data = (int32_t*)hdr->AS.rawdata;

			en1064.Section6.AVM	= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
			en1064.Section6.dT_us	= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+2));
			hdr->SampleRate 	= 1e6/en1064.Section6.dT_us;
			en1064.Section6.DIFF 	= *(PtrCurSect+curSectPos+4);
			en1064.FLAG.DIFF 	= *(PtrCurSect+curSectPos+4);
			en1064.Section6.BIMODAL = *(PtrCurSect+curSectPos+5);
			en1064.FLAG.BIMODAL     = *(PtrCurSect+curSectPos+5);

			Cal6 			= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos));
			dT_us	 		= l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+2));
			hdr->SampleRate 	= 1e6/dT_us;
			hdr->aECG->FLAG.DIFF 	= *(PtrCurSect+curSectPos+4);
			hdr->aECG->FLAG.BIMODAL = *(PtrCurSect+curSectPos+5);

			typeof(hdr->SPR) SPR  = ( en1064.FLAG.BIMODAL ? en1064.Section4.SPR : hdr->SPR);  

			Ptr2datablock   = (PtrCurSect+curSectPos + 6 + hdr->NS*2);   // pointer for huffman decoder
			len = 0;  
			size_t ix; 			
			for (i=0; i < hdr->NS; i++) {
				hdr->CHANNEL[i].SPR 	    = hdr->SPR;
				hdr->CHANNEL[i].PhysDimCode = 4275; // PhysDimCode("uV") physical unit "uV"
				hdr->CHANNEL[i].Cal 	    = Cal6 * 1e-3;
				hdr->CHANNEL[i].Off         = 0;
				hdr->CHANNEL[i].OnOff       = 1;    // 1: ON 0:OFF
				hdr->CHANNEL[i].Transducer[0] = '\0';
				hdr->CHANNEL[i].GDFTYP      = gdftyp;  

				// ### these values should represent the true saturation values ###//
				hdr->CHANNEL[i].DigMax      = ldexp(1.0,20)-1;
				hdr->CHANNEL[i].DigMin      = ldexp(-1.0,20);
				hdr->CHANNEL[i].PhysMax     = hdr->CHANNEL[i].DigMax * hdr->CHANNEL[i].Cal;
				hdr->CHANNEL[i].PhysMin     = hdr->CHANNEL[i].DigMin * hdr->CHANNEL[i].Cal;
				
				en1064.Section6.inlen[i]    = l_endian_u16(*(uint16_t*)(PtrCurSect+curSectPos+6+2*i));
				if (en1064.FLAG.HUFFMAN)
					DecodeHuffman(HTrees, Huffman, Ptr2datablock, en1064.Section6.inlen[i], data + i*hdr->SPR, hdr->SPR);
				else {
					for (k1=0, ix = i*hdr->SPR; k1 < SPR; k1++)
						data[ix+k1] = l_endian_i16(*(int16_t*)(Ptr2datablock + 2*k1));
				}
				len += en1064.Section6.inlen[i];
				Ptr2datablock += en1064.Section6.inlen[i]; 

#ifdef EXPERIMENTAL
				if (hdr->aECG->FLAG.BIMODAL || en1064.FLAG.REF_BEAT) { 	
					AS_DECODE = 1; 
					continue; 
				}
#endif 

				if (hdr->aECG->FLAG.DIFF==1) {
					for (ix = i*hdr->SPR+1; ix < i*hdr->SPR + SPR; ix++)
						data[ix] += data[ix-1];
				}		
				else if (hdr->aECG->FLAG.DIFF==2) {
					for (ix = i*hdr->SPR+2; ix < i*hdr->SPR + SPR; ix++)
						data[ix] += 2*data[ix-1] - data[ix-2];
				}

				if (hdr->aECG->FLAG.BIMODAL) {
					ix = i*hdr->SPR;		// memory offset
					k1 = en1064.Section4.SPR-1; 	// SPR of decimated data 
					k2 = hdr->SPR;			// SPR of sample data
					uint32_t k3 = en1064.Section4.N-1; // # of protected zones 
					uint8_t  k4 = 4;		// decimation factor 
					do {
						--k2;
						data[ix + k2] = data[ix+k1];
						if (k2 > en1064.Section4.beat[k3].QE) { // outside protected zone 
							if (--k4==0) {k4=4; --k1;};	
						}
						else {	// inside protected zone 
							--k1;
							if (k2==en1064.Section4.beat[k3].QB) --k3;
						}
					} while (k2>0);
				}
			
				if (en1064.FLAG.REF_BEAT) {
					uint16_t g = 1; 	
					if      (Cal5==0 && Cal6 >1) g = Cal6;
					else if (Cal5 >1 && Cal6==0) g = Cal5;
					else if (Cal5 >1 && Cal6 >1) g = gcd(Cal5,Cal6);

					hdr->CHANNEL[i].Cal = g/Cal6;
					hdr->CHANNEL[i].PhysMax = hdr->CHANNEL[i].DigMax * hdr->CHANNEL[i].Cal;
					hdr->CHANNEL[i].PhysMin = hdr->CHANNEL[i].DigMin * hdr->CHANNEL[i].Cal;

//					for (ix = en1064.Section4.beat[i].SB; ix < en1064.Section4.beat[i].SE; ix++) 
					for (ix = 0; ix < en1064.Section5.Length; ix++) 
					for (k1=0; k1 < en1064.Section4.N; k1++) 
					if (en1064.Section4.beat[i].btyp==0) {
						size_t ix1 = i*hdr->SPR + en1064.Section4.beat[k1].SB - en1064.Section4.beat[k1].fcM + ix;						
						data[ix1]  = data[ix1]*Cal6+en1064.Section5.datablock[i*en1064.Section5.Length+ix]*Cal5;
						data[ix1] /= g; 
					}
				}

			}

			en1064.Section6.datablock = data; 
			hdr->FLAG.SWAP  = 0;

			curSectPos += 6 + 2*hdr->NS + len;
		}

		/**** SECTION 7 ****/
		else if (curSect==7)  {
		}

		/**** SECTION 8 ****/
		else if (curSect==8)  {
		}

		/**** SECTION 9 ****/
		else if (curSect==9)  {
		}

		/**** SECTION 10 ****/
		else if (curSect==10)  {
		}

		/**** SECTION 11 ****/
		else if (curSect==11)  {
		}
		else {
		}
	}	
	hdr->Dur[0] = hdr->SPR * dT_us; 
	hdr->Dur[1] = 1000000L; 

	/* free allocated memory */ 
	if (en1064.FLAG.HUFFMAN) {
		for (k1=0; k1<en1064.FLAG.HUFFMAN; k1++) {
		 	if (NHT!=19999) free(Huffman[k1].Table);
		 	freeTree(HTrees[k1]); 
		} 
		free(Huffman);
		free(HTrees); 
	}	

	if (en1064.Section3.lead != NULL) 	free(en1064.Section3.lead);
//	if (en1064.Section4.beat != NULL) 	free(en1064.Section4.beat);
	if (en1064.Section5.inlen != NULL) 	free(en1064.Section5.inlen);
//	if (en1064.Section5.datablock != NULL) 	free(en1064.Section5.datablock);
	if (en1064.Section6.inlen != NULL) 	free(en1064.Section6.inlen);
//	if (en1064.Section6.datablock != NULL) 	free(en1064.Section6.datablock);

//   	if (!hdr->aECG->FLAG.REF_BEAT && !hdr->aECG->FLAG.BIMODAL) return(0); 


#ifdef EXPERIMENTAL
//		return(0); 
//#else 

//   	if (!hdr->aECG->FLAG.HUFFMAN && !hdr->aECG->FLAG.REF_BEAT && !hdr->aECG->FLAG.BIMODAL) return(0); 
   	if (!hdr->aECG->FLAG.REF_BEAT && !hdr->aECG->FLAG.BIMODAL) return(0); 
    		
//    	if (hdr->aECG->FLAG.HUFFMAN || hdr->aECG->FLAG.REF_BEAT || hdr->aECG->FLAG.BIMODAL) {

/*
---------------------------------------------------------------------------
Copyright (C) 2006  Eugenio Cervesato.
Developed at the Associazione per la Ricerca in Cardiologia - Pordenone - Italy,

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
---------------------------------------------------------------------------
*/

	/* Fall back method: 





		+ implements Huffman, reference beat and Bimodal compression. 
		- uses piece-wise file access
		- defines intermediate data structure
	*/	

	fprintf(stdout, "\nUse SCP_DECODE (Huffman=%i RefBeat=%i Bimodal=%i)\n",hdr->aECG->FLAG.HUFFMAN, hdr->aECG->FLAG.REF_BEAT, hdr->aECG->FLAG.BIMODAL);

	// greatest common divisor of scaling from section 5 and 6
	uint16_t g = 1; 	
	if      (Cal5==0 && Cal6 >1) g = Cal6;
	else if (Cal5 >1 && Cal6==0) g = Cal5;
	else if (Cal5 >1 && Cal6 >1) g = gcd(Cal5,Cal6);
	
	textual.des.acquiring.protocol_revision_number = hdr->aECG->Section1.Tag14.VERSION;
	textual.des.analyzing.protocol_revision_number = hdr->aECG->Section1.Tag15.VERSION; 

	decode.flag_Res.bimodal = (hdr->aECG->Section1.Tag14.VERSION > 10 ? hdr->aECG->FLAG.BIMODAL : 0);  
	decode.Reconstructed    = (int32_t*) hdr->AS.rawdata; 

	if (scp_decode(hdr, section, decode, record, textual, add_filter)) {
		if (g>1)
			for (i=0; i < hdr->NS * hdr->SPR * hdr->NRec; ++i)
				data[i] /= g;
		hdr->FLAG.SWAP  = 0;
	}
	else { 
		B4C_ERRNUM = B4C_CANNOT_OPEN_FILE;
		B4C_ERRMSG = "SCP-DECODE can not read file"; 
		return(0);
	}
	
fprintf(stdout,"\ndata2: ");
for (int k=0;k<8;k++) fprintf(stdout,"%i %i %i\n",data[k*hdr->SPR],data[k*hdr->SPR+1],data[k*hdr->SPR+2]);

	for (i=0; i < hdr->NS; i++) {
		hdr->CHANNEL[i].PhysDimCode = 4275; // PhysDimCode("uV") physical unit "uV" 	
		hdr->CHANNEL[i].Cal 	    = g*1e-3;
		hdr->CHANNEL[i].PhysMax     = hdr->CHANNEL[i].DigMax * hdr->CHANNEL[i].Cal;
		hdr->CHANNEL[i].PhysMin     = hdr->CHANNEL[i].DigMin * hdr->CHANNEL[i].Cal;
	}
	 // end of fall back method 
	return(1);
#endif 
};

