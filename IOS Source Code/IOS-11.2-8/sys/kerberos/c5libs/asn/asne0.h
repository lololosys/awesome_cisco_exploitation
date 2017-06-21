/* $Id: asne0.h,v 3.2 1995/11/17 17:43:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/asn/asne0.h,v $
 *------------------------------------------------------------------
 * asne0.h - ASN encode definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asne0.h,v $
 * Revision 3.2  1995/11/17  17:43:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_ASNE0_H_
#define _CSFC5_ASNE0_H_ "%W% %G%"

/*
 * Copyright (c) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include "asne1.h"

/* ASNE_FUNCTION -- Encoding function entry-point.  These are the entry points
 * seen by the outside world.  All they do is call "Edriver" with the low-level
 * encoding function.
 */
#if _CSFC5_ASNE_CHECKTYPES
#define _CSFC5_ASNE_FUNCTION( efn, dfn, dt ) \
	csf_sts efn ( \
		const dt val, \
		csfC5_data *data \
	){ \
		dfn( (csfC5_asnE1Ctx *)NULL, val ); \
	}
#else
#define _CSFC5_ASNE_FUNCTION( efn, dfn, dt ) \
	csf_sts efn ( \
		const dt val, \
		csfC5_data *data \
	){ \
		return csfC5asnEDriver( dfn, (const void *)val, data ); \
	}
#endif

csf_sts csfC5asnEDriver (
	csfC5_asnE1F *encoder,
	const void *val,
	csfC5_data *data
);

#endif
