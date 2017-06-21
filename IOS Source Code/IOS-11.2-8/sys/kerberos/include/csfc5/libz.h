/* $Id: libz.h,v 3.2 1995/11/17 17:48:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/libz.h,v $
 *------------------------------------------------------------------
 * libz.h - Libz definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: libz.h,v $
 * Revision 3.2  1995/11/17  17:48:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_LIBZ_H_
#define _CSFC5_LIBZ_H_   "%W% %G%"
/*.original func-proto.h 3.8 5/22/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef _CSFC5c_STS_HS_
#define _CSFC5c_STS_HS_
#include <csfc5/sts/c5c_sts.hs>
#endif

#ifndef _CSFC5_STS_HS_
#define _CSFC5_STS_HS_
#include <csfc5/sts/c5_sts.hs>
#endif


csf_sts csfC5getNow_tStamp (
	csfC5_tStamp *ts
);

int csfC5cmp_tStamp (
	csfC5_tStamp ts1,
	csfC5_tStamp ts2
);

int csfC5cmpSkewed_tStamp (
	csfC5_tStamp ts1,
	csfC5_tStamp ts2
);

int csfC5cmpSkewedNow_tStamp (
	csfC5_tStamp ts
);


csf_sts csfC5get_usTStamp (
	csfC5_usTStamp *ust
);

csf_sts csfC5getUnique_usTStamp (
	csfC5_usTStamp *ust
);


csf_sts csfC5ssresolve (	/* move ?? krb/ssreslv.c */
	const char *name,		/* Name to resolve. */
	char **typnam,			/* First position of type; NULL if none. */
	csf_size_t *typlen,	/* Length of type string. */
	char **resnam,			/* First position of residual; NULL if none. */
	csf_size_t *reslen		/*  Length of residual string. */
);

csf_sts csfC5getSvcKey (
	char *ktname,
	const csfC5_prnId *prn,
	csfC5_crsKVNo kvno,
	csfC5_crsKBlock *key
);

csf_sts csfC5netHostTo_prnId (	/*TBD/JNK--doesn't belong here*/
	const char *,
	const char *,
	csf_ui32,
	csfC5_prnId *
);

csf_sts csfC5tgtname (
	const csfC5_prnNC *,
	const csfC5_prnNC *,
	csfC5_prnId *
);

#endif
