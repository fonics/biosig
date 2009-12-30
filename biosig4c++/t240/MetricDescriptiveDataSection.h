/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "FEF-IntermediateDraft"
 * 	found in "../annexb-snacc-122001.asn1"
 */

#ifndef	_MetricDescriptiveDataSection_H_
#define	_MetricDescriptiveDataSection_H_


#include <asn_application.h>

/* Including external dependencies */
#include "Handle.h"
#include "HandleRef.h"
#include "MetricsCode.h"
#include "FEFString.h"
#include "MetricCategory.h"
#include "MetricStatus.h"
#include "MeasurementStatus.h"
#include "UnitsOfMeasurementCode.h"
#include "SimpleColour.h"
#include "PrivateCode.h"
#include "AbsoluteTime.h"
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include "BodySiteCode.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ExtNomenRef;
struct MetricMeasure;
struct MetricCalEntry;

/* MetricDescriptiveDataSection */
typedef struct MetricDescriptiveDataSection {
	Handle_t	 handle;
	HandleRef_t	 devicehandle;
	MetricsCode_t	 metrictype;
	FEFString_t	*labelstring	/* OPTIONAL */;
	MetricCategory_t	*metriccategory	/* DEFAULT 0 */;
	MetricStatus_t	*metricstatus	/* DEFAULT {} */;
	MeasurementStatus_t	*measurementstatus	/* DEFAULT {} */;
	MetricsCode_t	*metricid	/* OPTIONAL */;
	struct ExtNomenRef	*metricidext	/* OPTIONAL */;
	UnitsOfMeasurementCode_t	*unitcode	/* OPTIONAL */;
	FEFString_t	*unitlabelstring	/* OPTIONAL */;
	struct MetricDescriptiveDataSection__vmosourcelist {
		A_SEQUENCE_OF(MetricsCode_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *vmosourcelist;
	struct MetricDescriptiveDataSection__metricsourcelist {
		A_SEQUENCE_OF(HandleRef_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *metricsourcelist;
	struct MetricDescriptiveDataSection__msmtsitelist {
		A_SEQUENCE_OF(BodySiteCode_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *msmtsitelist;
	struct MetricDescriptiveDataSection__msmtsitelistext {
		A_SEQUENCE_OF(struct ExtNomenRef) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *msmtsitelistext;
	struct MetricDescriptiveDataSection__bodysitelist {
		A_SEQUENCE_OF(BodySiteCode_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *bodysitelist;
	struct MetricDescriptiveDataSection__bodysitelistext {
		A_SEQUENCE_OF(struct ExtNomenRef) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *bodysitelistext;
	struct MetricDescriptiveDataSection__metriccalibration {
		A_SEQUENCE_OF(struct MetricCalEntry) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *metriccalibration;
	SimpleColour_t	*colour	/* OPTIONAL */;
	PrivateCode_t	*measuremode	/* OPTIONAL */;
	struct MetricMeasure	*measureperiod	/* OPTIONAL */;
	struct MetricMeasure	*averagingperiod	/* OPTIONAL */;
	AbsoluteTime_t	*starttime	/* OPTIONAL */;
	AbsoluteTime_t	*stoptime	/* OPTIONAL */;
	FEFString_t	*metricinfolabelstring	/* OPTIONAL */;
	struct ExtNomenRef	*substance	/* OPTIONAL */;
	FEFString_t	*substancelabelstring	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} MetricDescriptiveDataSection_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MetricDescriptiveDataSection;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "ExtNomenRef.h"
#include "MetricMeasure.h"
#include "MetricCalEntry.h"

#endif	/* _MetricDescriptiveDataSection_H_ */
