/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "FEF-IntermediateDraft"
 * 	found in "../annexb-snacc-122001.asn1"
 */

#include <asn_internal.h>

#include "MedicalDeviceSystemSection.h"

static asn_TYPE_member_t asn_MBR_systemspecification_10[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_SystemSpecEntry,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_systemspecification_tags_10[] = {
	(ASN_TAG_CLASS_APPLICATION | (2437 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_systemspecification_specs_10 = {
	sizeof(struct MedicalDeviceSystemSection__systemspecification),
	offsetof(struct MedicalDeviceSystemSection__systemspecification, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_systemspecification_10 = {
	"systemspecification",
	"systemspecification",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_systemspecification_tags_10,
	sizeof(asn_DEF_systemspecification_tags_10)
		/sizeof(asn_DEF_systemspecification_tags_10[0]) - 1, /* 1 */
	asn_DEF_systemspecification_tags_10,	/* Same as above */
	sizeof(asn_DEF_systemspecification_tags_10)
		/sizeof(asn_DEF_systemspecification_tags_10[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_systemspecification_10,
	1,	/* Single element */
	&asn_SPC_systemspecification_specs_10	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_productionspecification_12[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_ProdSpecEntry,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_productionspecification_tags_12[] = {
	(ASN_TAG_CLASS_APPLICATION | (2349 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_productionspecification_specs_12 = {
	sizeof(struct MedicalDeviceSystemSection__productionspecification),
	offsetof(struct MedicalDeviceSystemSection__productionspecification, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_productionspecification_12 = {
	"productionspecification",
	"productionspecification",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_productionspecification_tags_12,
	sizeof(asn_DEF_productionspecification_tags_12)
		/sizeof(asn_DEF_productionspecification_tags_12[0]) - 1, /* 1 */
	asn_DEF_productionspecification_tags_12,	/* Same as above */
	sizeof(asn_DEF_productionspecification_tags_12)
		/sizeof(asn_DEF_productionspecification_tags_12[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_productionspecification_12,
	1,	/* Single element */
	&asn_SPC_productionspecification_specs_12	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_MedicalDeviceSystemSection_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct MedicalDeviceSystemSection, handle),
		(ASN_TAG_CLASS_APPLICATION | (2337 << 2)),
		0,
		&asn_DEF_Handle,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"handle"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MedicalDeviceSystemSection, systemtype),
		(ASN_TAG_CLASS_APPLICATION | (2438 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DeviceCode,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"systemtype"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MedicalDeviceSystemSection, systemmodel),
		(ASN_TAG_CLASS_APPLICATION | (2344 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SystemModel,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"systemmodel"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MedicalDeviceSystemSection, systemid),
		(ASN_TAG_CLASS_APPLICATION | (2436 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_FEFString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"systemid"
		},
	{ ATF_POINTER, 1, offsetof(struct MedicalDeviceSystemSection, compatibilityid),
		(ASN_TAG_CLASS_APPLICATION | (2336 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"compatibilityid"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MedicalDeviceSystemSection, nomenclatureversion),
		(ASN_TAG_CLASS_APPLICATION | (2376 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"nomenclatureversion"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MedicalDeviceSystemSection, requirednomenclatureversion),
		(ASN_TAG_CLASS_APPLICATION | (6013 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_AbsoluteTime,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"requirednomenclatureversion"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct MedicalDeviceSystemSection, usednomenclatureversion),
		(ASN_TAG_CLASS_APPLICATION | (6014 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_AbsoluteTime,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"usednomenclatureversion"
		},
	{ ATF_POINTER, 10, offsetof(struct MedicalDeviceSystemSection, systemspecification),
		(ASN_TAG_CLASS_APPLICATION | (2437 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_systemspecification_10,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"systemspecification"
		},
	{ ATF_POINTER, 9, offsetof(struct MedicalDeviceSystemSection, productionspecification),
		(ASN_TAG_CLASS_APPLICATION | (2349 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_productionspecification_12,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"productionspecification"
		},
	{ ATF_POINTER, 8, offsetof(struct MedicalDeviceSystemSection, bedlabel),
		(ASN_TAG_CLASS_APPLICATION | (2334 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_FEFString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"bedlabel"
		},
	{ ATF_POINTER, 7, offsetof(struct MedicalDeviceSystemSection, softid),
		(ASN_TAG_CLASS_APPLICATION | (2350 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_FEFString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"softid"
		},
	{ ATF_POINTER, 6, offsetof(struct MedicalDeviceSystemSection, operatingmode),
		(ASN_TAG_CLASS_APPLICATION | (2374 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrivateCode,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"operatingmode"
		},
	{ ATF_POINTER, 5, offsetof(struct MedicalDeviceSystemSection, applicationarea),
		(ASN_TAG_CLASS_APPLICATION | (2317 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ApplicationArea,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"applicationarea"
		},
	{ ATF_POINTER, 4, offsetof(struct MedicalDeviceSystemSection, powerstatus),
		(ASN_TAG_CLASS_APPLICATION | (2389 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PowerStatus,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"powerstatus"
		},
	{ ATF_POINTER, 3, offsetof(struct MedicalDeviceSystemSection, altitude),
		(ASN_TAG_CLASS_APPLICATION | (2316 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"altitude"
		},
	{ ATF_POINTER, 2, offsetof(struct MedicalDeviceSystemSection, linefrequency),
		(ASN_TAG_CLASS_APPLICATION | (2357 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LineFrequency,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"linefrequency"
		},
	{ ATF_POINTER, 1, offsetof(struct MedicalDeviceSystemSection, placeholder),
		(ASN_TAG_CLASS_APPLICATION | (6499 << 2)),
		0,
		&asn_DEF_Placeholder,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"placeholder"
		},
	{ ATF_POINTER, 0, offsetof(struct MedicalDeviceSystemSection, components),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_MedicalDeviceComponents,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"components"
		},
};
static ber_tlv_tag_t asn_DEF_MedicalDeviceSystemSection_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_MedicalDeviceSystemSection_tag2el_1[] = {
    { (ASN_TAG_CLASS_APPLICATION | (2316 << 2)), 15, 0, 0 }, /* altitude at 544 */
    { (ASN_TAG_CLASS_APPLICATION | (2317 << 2)), 13, 0, 0 }, /* applicationarea at 539 */
    { (ASN_TAG_CLASS_APPLICATION | (2334 << 2)), 10, 0, 0 }, /* bedlabel at 529 */
    { (ASN_TAG_CLASS_APPLICATION | (2336 << 2)), 4, 0, 0 }, /* compatibilityid at 501 */
    { (ASN_TAG_CLASS_APPLICATION | (2337 << 2)), 0, 0, 0 }, /* handle at 486 */
    { (ASN_TAG_CLASS_APPLICATION | (2344 << 2)), 2, 0, 0 }, /* systemmodel at 493 */
    { (ASN_TAG_CLASS_APPLICATION | (2349 << 2)), 9, 0, 0 }, /* productionspecification at 525 */
    { (ASN_TAG_CLASS_APPLICATION | (2350 << 2)), 11, 0, 0 }, /* softid at 533 */
    { (ASN_TAG_CLASS_APPLICATION | (2357 << 2)), 16, 0, 0 }, /* linefrequency at 547 */
    { (ASN_TAG_CLASS_APPLICATION | (2374 << 2)), 12, 0, 0 }, /* operatingmode at 537 */
    { (ASN_TAG_CLASS_APPLICATION | (2376 << 2)), 5, 0, 0 }, /* nomenclatureversion at 504 */
    { (ASN_TAG_CLASS_APPLICATION | (2389 << 2)), 14, 0, 0 }, /* powerstatus at 541 */
    { (ASN_TAG_CLASS_APPLICATION | (2436 << 2)), 3, 0, 0 }, /* systemid at 497 */
    { (ASN_TAG_CLASS_APPLICATION | (2437 << 2)), 8, 0, 0 }, /* systemspecification at 520 */
    { (ASN_TAG_CLASS_APPLICATION | (2438 << 2)), 1, 0, 0 }, /* systemtype at 489 */
    { (ASN_TAG_CLASS_APPLICATION | (6013 << 2)), 6, 0, 0 }, /* requirednomenclatureversion at 508 */
    { (ASN_TAG_CLASS_APPLICATION | (6014 << 2)), 7, 0, 0 }, /* usednomenclatureversion at 513 */
    { (ASN_TAG_CLASS_APPLICATION | (6499 << 2)), 17, 0, 0 }, /* placeholder at 550 */
    { (ASN_TAG_CLASS_APPLICATION | (7009 << 2)), 18, 0, 0 }, /* medicaldevice at 596 */
    { (ASN_TAG_CLASS_APPLICATION | (7010 << 2)), 18, 0, 0 }, /* medicaldevices at 600 */
    { (ASN_TAG_CLASS_APPLICATION | (7011 << 2)), 18, 0, 0 } /* subsystems at 606 */
};
static asn_SEQUENCE_specifics_t asn_SPC_MedicalDeviceSystemSection_specs_1 = {
	sizeof(struct MedicalDeviceSystemSection),
	offsetof(struct MedicalDeviceSystemSection, _asn_ctx),
	asn_MAP_MedicalDeviceSystemSection_tag2el_1,
	21,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_MedicalDeviceSystemSection = {
	"MedicalDeviceSystemSection",
	"MedicalDeviceSystemSection",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_MedicalDeviceSystemSection_tags_1,
	sizeof(asn_DEF_MedicalDeviceSystemSection_tags_1)
		/sizeof(asn_DEF_MedicalDeviceSystemSection_tags_1[0]), /* 1 */
	asn_DEF_MedicalDeviceSystemSection_tags_1,	/* Same as above */
	sizeof(asn_DEF_MedicalDeviceSystemSection_tags_1)
		/sizeof(asn_DEF_MedicalDeviceSystemSection_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_MedicalDeviceSystemSection_1,
	19,	/* Elements count */
	&asn_SPC_MedicalDeviceSystemSection_specs_1	/* Additional specs */
};

