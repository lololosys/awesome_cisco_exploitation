/* $Id: sts.h,v 3.2 1995/11/17 17:48:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/sts.h,v $
 *------------------------------------------------------------------
 * sts.h - Generic status definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sts.h,v $
 * Revision 3.2  1995/11/17  17:48:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_STS_H_
#define _CSFC5_STS_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

/*TBD/JNK--this is a kluge. later.*/

#define CSFC5c_STS__base		(CSFC5_CFG_STSBASE + (256*0))
#define CSFC5_STS__base			(CSFC5_CFG_STSBASE + (256*1))
#define CSFC5_STS_CC__base		(CSFC5_CFG_STSBASE + (256*2))
#define CSFC5_STS_RC__base		(CSFC5_CFG_STSBASE + (256*3))
#define CSFC5_STS_KT__base		(CSFC5_CFG_STSBASE + (256*4))
#define CSFC5_STS_CRS__base		(CSFC5_CFG_STSBASE + (256*5))
#define CSFC5_STS_CKS__base		(CSFC5_CFG_STSBASE + (256*6))
#define CSFC5_STS_NET__base		(CSFC5_CFG_STSBASE + (256*7))
#define CSFC5_STS_ASN__base		(CSFC5_CFG_STSBASE + (256*8))

/* Return RFC-defined value-text for given status. */
char * csfC5sts2rfc (
	csf_sts sts,		/* in */
	csf_ui32 *rfcv		/* out */
);

#endif /* _CSFC5_STS_H_ */
