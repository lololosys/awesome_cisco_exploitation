/* $Id: ckscrc32.c,v 3.2 1995/11/17 17:43:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cks/crc32/ckscrc32.c,v $
 *------------------------------------------------------------------
 * ckscrc32.c - Calculates CRC32 checksums
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ckscrc32.c,v $
 * Revision 3.2  1995/11/17  17:43:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original crc.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * CRC-32/AUTODIN-II routines
 */

#include <csfc5/header.h>
#include <csfc5/cks/cks0.h>

static csf_sts csfC5cksCrc32Func (
	void *in,
	csf_size_t len,
	void *seed,				/*UNUSED*/
	csf_size_t seed_length,	/*UNUSED*/
	csfC5_cksBlock *cks
){
	csf_ui32 cks32;

	(void) csfCRC32sumAD2( in, len, &cks32 );
	cks->cks_type = CSFC5c_CKS_T_CRC32;
	cks->cks_data.dl = 4;
	csfCmemcpy( cks->cks_data.dp, &cks32, 4 );

	return CSF_STS_OK;
}

/* This function exists solely to allow crypto subsystem access to the
 * standard crc-32 calculaction without exposing the underlying function.
 */
csf_sts csfC5cksCrc32 (
	void *in,
	csf_size_t len,
	csfC5_cksBlock *cks
){
	return csfC5cksCrc32Func( in, len, NULL, 0, cks );
}


csfC5_cksentry csfC5cksCrc32Entry = {
	csfC5cksCrc32Func,
	CSFC5c_CKS_T_CRC32_cks,			/* CRC-32 is 4 octets */
	0,									/* not collision proof */
	0,									/* doesn't use key */
};

