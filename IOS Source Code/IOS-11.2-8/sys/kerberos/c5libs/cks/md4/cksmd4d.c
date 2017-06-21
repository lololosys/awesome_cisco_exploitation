/* $Id: cksmd4d.c,v 3.2 1995/11/17 17:43:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cks/md4/cksmd4d.c,v $
 *------------------------------------------------------------------
 * cksmd4d.c - Calculates MD4 checksums
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cksmd4d.c,v $
 * Revision 3.2  1995/11/17  17:43:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original md4crypto.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * MD4 with DES CBC checksum.
 */

#include <csfc5/header.h>
#include <csfc5/cks/cks0.h>

static csf_sts csfC5cksRsamd4descbcFunc (
	void *in,
	csf_size_t in_length,
	void *seed,
	csf_size_t seed_length,
	csfC5_cksBlock *outcksum
){
	csfMD4_cBlk ckstmp;
	csfDES_kSched schedule;
	csf_sts rsts;

	/* Generate the message digest. */
	csfMD4sum( in, in_length, &ckstmp );
	outcksum->cks_type = CSFC5c_CKS_T_RSA_MD4_DES;
	outcksum->cks_data.dl = CSFC5c_CKS_T_RSA_MD4_DES_cks;

	/*TBD/JNK--the csfDES functions could be a little more
	 * helpful in order to eliminate most of this code. We do
	 * NOT go through the C5 des crypto functions (as the old
	 * code did) because there is no gain in generality, or
	 * reduction in code--and the RFC specs this, so there is
	 * nothing to be gained--and some CPU cycles to be lost.
	 */

	switch (csfDESmake_kSched( seed, &schedule )) {
	case -1:
		rsts = CSFC5_STS_CRS_DES_BAD_KEYPAR;
		goto clean_exit;
	case -2:
		rsts = CSFC5_STS_CRS_DES_WEAK_KEY;
		goto clean_exit;
	default:
		break;
	}

	/* now encrypt it */
	rsts = csfDESEncryptCBC(
		ckstmp,
		outcksum->cks_data.dp,
		CSFC5c_CKS_T_RSA_MD4_DES_cks,
		&schedule,
		seed
	);

clean_exit:
	return rsts;
}


csfC5_cksentry csfC5cksRsamd4descbcEntry = {
	csfC5cksRsamd4descbcFunc,
	CSFC5c_CKS_T_RSA_MD4_DES_cks,
	1,									/* is collision proof */
	1,									/* uses key */
};
