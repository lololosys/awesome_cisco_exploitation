/* $Id: cks0.h,v 3.2 1995/11/17 17:49:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/cks/cks0.h,v $
 *------------------------------------------------------------------
 * cks0.h - Checksum private definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cks0.h,v $
 * Revision 3.2  1995/11/17  17:49:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CKS0_H_
#define _CSFC5_CKS0_H_ "%W% %G%"
/*.original encryption.h 3.4 5/16/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1989,1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Checksum system (CKS) private definitions.
 */


/*
 * Checksum tables
 */

typedef struct _csfC5_cksentry {
	csf_sts (*sum_func) (
		void *,			/* in */
		csf_size_t,		/* in_length */
		void *,			/* key/seed */
		csf_size_t,		/* key/seed size */
		csfC5_cksBlock *	/* out_cksum */
	);
	csf_size_t cks_length;	/* length of stuff returned by sum_func */
	unsigned int is_collision_proof: 1;
	unsigned int uses_key: 1;
} csfC5_cksentry;

#endif /* _CSFC5_CKS0_H_ */
