/* $Id: crs0.h,v 3.2 1995/11/17 17:49:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/crs/crs0.h,v $
 *------------------------------------------------------------------
 * crs0.h - Crypto subsystem private definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crs0.h,v $
 * Revision 3.2  1995/11/17  17:49:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CRS0_H_
#define _CSFC5_CRS0_H_ "%W% %G%"
/*.original encryption.h 3.4 5/16/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1989,1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Crypto system (CRS) private definitions.
 */


/*
 * Crypto tables.
 */

typedef struct _csfC5_crsEntry {

	csf_sts (*crsEncrypt) (
		void const *,	/* in */
		void *,		/* out */
		const csf_size_t,
		csfC5_crsEBlock *,
		void *
	);

	csf_sts (*crsDecrypt) (
		void const *,	/* in */
		void *,		/* out */
		const csf_size_t,
		csfC5_crsEBlock *,
		void *
	);

	csf_sts (*crsProcessKey) (
		csfC5_crsEBlock *,
		const csfC5_crsKBlock *
	);

	csf_sts (*crsFinishKey) (
		csfC5_crsEBlock *
	);

	csf_sts (*crsStringToKey) (
		const csfC5_crsKType,
		csfC5_crsKBlock *,
		const csfC5_data *,
		const csfC5_data *
	);

	csf_sts (*crsInitRanKey) (
		const csfC5_crsKBlock *,
		void **
	);

	csf_sts (*crsFinishRanKey) (
		void **
	);

	csf_sts (*crsGenRanKey) (
		void *,
		csfC5_crsKBlock *
	);

	csf_size_t block_length;
	csf_size_t pad_minimum;	/* needed for cksum size computation */
	csf_size_t keysize;
	csfC5_crsEType etype;		/* encryption type, (protocol number &
									table index) */

	csfC5_crsKType ktype;		/* key type, (protocol number &
									table index) */

} csfC5_crsEntry;

#endif
