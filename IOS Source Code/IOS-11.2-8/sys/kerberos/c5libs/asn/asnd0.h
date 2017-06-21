/* $Id: asnd0.h,v 3.2 1995/11/17 17:42:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asnd0.h,v $
 *------------------------------------------------------------------
 * asnd0.h - Definitions for ASN decode functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asnd0.h,v $
 * Revision 3.2  1995/11/17  17:42:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_ASND0_H_
#define _CSFC5_ASND0_H_ "%W% %G%"

/*
 * Copyright (c) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include "asnd1.h"

#if _CSFC5_ASND_CHECKTYPES
#define _CSFC5_ASND_FUNCTION( fn, dfn, dt ) \
	csf_sts fn ( \
		const csfC5_data *in, \
		dt out \
	){ \
		dfn( (csfC5_asnD1Ctx *)NULL, out ); \
	}
#else
#define _CSFC5_ASND_FUNCTION( fn, dfn, dt ) \
	csf_sts fn ( \
		const csfC5_data *in, \
		dt out \
	){ \
		return csfC5asnDDriver( in, (void *)out, sizeof(*out), dfn ); \
	}
#endif

csf_sts csfC5asnDDriver (
	const csfC5_data *in,
	void *out,
	csf_size_t dts,
	csfC5_asnD1F *decoder
);


#endif
