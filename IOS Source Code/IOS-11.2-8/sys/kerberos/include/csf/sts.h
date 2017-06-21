/* $Id: sts.h,v 3.2 1995/11/17 17:47:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/sts.h,v $
 *------------------------------------------------------------------
 * sts.h - Generic status definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sts.h,v $
 * Revision 3.2  1995/11/17  17:47:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_STS_H_
#define _CSF_STS_H_ "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

/* Globally assigned base values for various systems.
 * Note that some of these live in other directories.
 * We assign everyone a block here. Kluge. Later.
 */
#define CSFASNX_CFG_STSBASE		(CSF_CFG_STSBASE + (256*1))
#define CSFC5_CFG_STSBASE		(CSF_CFG_STSBASE + (256*2))

#ifndef CSF_STS_OK
#define CSF_STS_OK 0
#endif

#ifndef CSF_STS_ENOMEM
#define CSF_STS_ENOMEM ((csf_sts)-1)		/*TBD/JNK*/
#endif

#ifndef _csfSTSerrno
#define _csfSTSerrno( sts ) \
	((csf_sts)(sts))
#endif

typedef struct _csfSTS_table {
	csf_sts sbase;					/* Base value for status codes. */
	csf_sts sfirst;				/* Value of first status code. */
	csf_sts slast;					/* Value of last status code + 1. */
	csf_sts ntext;					/* Number of text string (status) entries. */
	const char * const *text;		/* Pointer to text strings (*text[]). */
	const char *prefix;			/* Prefix string (if any). */
	const struct _csfSTS_table		/* Pointer to subtables (if any). */
		**subtabs;
} csfSTS_table;

#ifndef _csfSTSGX
#define _csfSTSGX( n, s ) s,
#endif

#endif /* _CSF_STS_H_ */
