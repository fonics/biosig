/*
%
% $Id: biosig.h,v 1.5 2005-09-10 20:39:20 schloegl Exp $
% Copyright (C) 2000,2005 Alois Schloegl <a.schloegl@ieee.org>
% This file is part of the "BioSig for C/C++" repository 
% (biosig4c++) at http://biosig.sf.net/ 
% 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/* 

	Headers for the BioSig4C library
	
	Currently, the GDF-Header is defined. 
   
*/

/****************************************************************************/
/**                                                                        **/
/**                 DEFINITIONS, TYPEDEFS AND MACROS                       **/
/**                                                                        **/
/****************************************************************************/

	// list of file formats 
enum FileFormat {ACQ, BKR, BDF, CNT, DEMG, EDF, FLAC, GDF, MFER, NEX1, PLEXON}; 

enum HANDEDNESS {Unknown=0, Right=1, Left=2, Equal=3}; 
enum GENDER  	{male=1,  female=2};
enum SCALE 	{No=1,    Yes=2,	Corrected=3};

const int GDFTYP_BYTE[] = {1, 1, 1, 2, 2, 4, 4, 8, 8, 4, 8, 0, 0, 0, 0, 0, 4, 8, 16};

typedef char 			int8; 
typedef unsigned char 		byte; 
typedef unsigned char 		uint8; 
typedef short 			int16; 
typedef unsigned short 		uint16; 
typedef long 			int32; 
typedef unsigned long 		uint32;
typedef long long 		int64; 
typedef unsigned long long 	uint64; 


#define min(a,b)                        (((a) < (b)) ? (a) : (b))
#define max(a,b)                        (((a) > (b)) ? (a) : (b))


/*
	Conversion of time formats between Unix and GDF format.

	The default time format in BIOSIG uses a 64-bit fixed point format with 
	reference date 01-Jan-0000 00h00m00s (value=0). 
	One unit indicates the 2^(-32) part of 1 day (ca 20 us). Accordingly, 
	the higher 32 bits count the number of days, the lower 32 bits describe 
	the fraction of a day.  01-Jan-1970 is the day 719529. 

      	time_t t0; 
      	t0 = time(NULL); 
      	T0 = (double)t0/86400.0;	// convert seconds it days since 1970-Jan-01
      	floor(T0) + 719529;		// number of days since 01-Jan-0000
      	floor(ldexp(T0-floor(T0),32));  // fraction x/2^32; one day is 2^32 
      	
      	The following macros define the conversions between the unix time and the 
      	GDF format. 
*/
typedef long long 		time_gdf; // gdf time is represented in 64 bits
#define time_t2time_gdf(t)	((time_gdf)floor(ldexp((t)/86400.0 + 719529, 32)))
#define time_gdf2time_t(t)	((time_t)((ldexp((t),-32) - 719529) * 86400)))


/****************************************************************************/
/**                                                                        **/
/**                     TYPEDEFS AND STRUCTURES                            **/
/**                                                                        **/
/****************************************************************************/

/*
	This structure defines the header for each channel (variable header) 
*/
typedef struct {
	char* 	Label;		// Label of channel 
	char* 	Transducer;	// transducer e.g. EEG: Ag-AgCl electrodes
	char* 	PhysDim;	// physical dimension
	//char* PreFilt;	// pre-filtering

	float 	LowPass;	// lowpass filter
	float 	HighPass;	// high pass
	float 	Notch;		// notch filter
	float 	XYZ[3];		// electrode position
	float 	Impedance;   	// in Ohm
	
	double 	PhysMin;	// physical minimum
	double 	PhysMax;	// physical maximum
	int64 	DigMin;		// digital minimum
	int64 	DigMax;		// digital maximum

	uint16 	GDFTYP;		// data type
	uint32 	SPR;		// samples per record (block)
} CHANNEL_TYPE;


/*
	This structure defines the general (fixed) header  
*/
typedef struct {
	enum FileFormat TYPE; 	// type of file format
	float 	VERSION;	// GDF version number 
	char* 	FileName;
	void*  	buffer; 	// data returned by sread

	uint32 	NS;		// number of channels
	uint32 	SampleRate;	// Sampling rate
	uint32 	SPR;		// samples per block (when different sampling rates are used, this is the LCM(CHANNEL[..].SPR)
	uint8 	IPaddr[6]; 	// IP address of recording device (if applicable)	
	uint32  LOC[4];		// location of recording according to RFC1876
	time_gdf T0; 		// starttime of recording
	uint32 	HeadLen;	// length of header in bytes
	int32  	NRec;		// number of records/blocks -1 indicates length is unknown.	
	uint32 	Dur[2];	// Duration of each block in seconds expressed in the fraction Dur[0]/Dur[1] 

	// Patient specific information 
	struct {
		char*		Name;		// not recommended because of privacy protection 
		char*		Id;		// identification code as used in hospital 
		uint8 		Weight;		// weight in kilograms [kg] 0:unkown, 255: overflow 
		uint8 		Height;		// height in centimeter [cm] 0:unkown, 255: overflow 
		time_gdf 	Birthday; 	// Birthday of Patient
		uint16 		Headsize[3]; 	// circumference, nasion-inion, left-right mastoid in millimeter; 
		enum GENDER 	Sex; 	
		enum HANDEDNESS Handedness;	
		// Patient classification // 
		enum SCALE 	Smoking;
		enum SCALE 	AlcoholAbuse;
		enum SCALE 	DrugAbuse;
		enum SCALE 	Medication;
		struct {
			enum SCALE Visual;
		} Impairment;
	} Patient; 

	// position of electrodes; see also HDR.CHANNEL[k].XYZ
	struct {
		float	REF[3];	// XYZ position of reference electrode
		float	GND[3];	// XYZ position of ground electrode
	} ELEC;

	//	EVENTTABLE 
	struct {
		uint32 	SampleRate;	// for converting POS and DUR into seconds 
		uint32 	N;	// number of events
		uint16 *TYP;	// defined at http://cvs.sourceforge.net/viewcvs.py/biosig/biosig/t200/eventcodes.txt?view=markup
		uint32 *POS;	// starting position [in samples]
		uint32 *DUR;	// duration [in samples]
		uint16 *CHN;	// channel number; 0: all channels 
	} EVENT; 

	struct {	// File specific data 
		FILE* 	FID;		// file handle 
		uint32 	POS;		// current reading/writing position in samples 
		uint8 	OPEN; 		// 0: closed, 1:read, 2: write
	} FILE; 

	//	internal variables (not public) 
	struct {
		char 	PID[81];	// patient identification
		char* 	RID;		// recording identification 
		uint32 	spb;		// total samples per block
		uint32 	bpb;  		// total bytes per block
		uint32 	*bi;
	} AS; 	
	CHANNEL_TYPE *CHANNEL;  
} HDRTYPE;


/****************************************************************************/
/**                                                                        **/
/**                     EXPORTED FUNCTIONS                                 **/
/**                                                                        **/
/****************************************************************************/


HDRTYPE init_default_hdr(HDRTYPE HDR, const unsigned NS, const unsigned N_EVENT);

HDRTYPE sopen(const char* FileName, HDRTYPE HDR, const char* MODE);
HDRTYPE sclose(HDRTYPE HDR);
size_t 	sread(HDRTYPE* hdr, size_t nelem);
size_t	swrite(const void *ptr, size_t nelem, HDRTYPE* hdr);
int	seof(HDRTYPE HDR);
int	srewind(HDRTYPE* hdr);
int 	sseek(HDRTYPE* hdr, uint32 offset, int whence);
int32   stell(HDRTYPE HDR);


/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/