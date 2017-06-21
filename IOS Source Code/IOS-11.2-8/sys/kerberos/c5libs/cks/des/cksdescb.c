/* $Id: cksdescb.c,v 3.2 1995/11/17 17:43:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cks/des/cksdescb.c,v $
 *------------------------------------------------------------------
 * cksdescb.c - Calculates DES checksums
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cksdescb.c,v $
 * Revision 3.2  1995/11/17  17:43:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original cbc_cksum.c 3.2 5/25/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1985, 1986, 1987, 1988, 1990 by the
 * Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Under U.S. law, this software may not be exported outside the US
 * without license from the U.S. Commerce department.
 */

/*
 * These routines form the library interface to the DES facilities.
 */

#include <csfc5/header.h>
#include <csfc5/cks/cks0.h>

/*
 produces cbc cheksum of sequence "in" of the length "in_length"
 with the help of key "key" of size "key_size" (which should be 8);
 fills out csfC5_cksBlock structure.

 caller is responsible for freeing "contents" element in
 csfC5_cksBlock structure.

 returns: errors
*/

static csf_sts csfC5cksDescbcFunc (
	void *in,
	csf_size_t in_length,
	void *key,
	csf_size_t key_size,
	csfC5_cksBlock *cksum
){
	csfDES_kSched schedule;	/* pointer to key schedules */

	/*TBD/JNK--the RFC doesn't define "DES-CBC".  The number assigned to
	 * to "DESCBC" in current implementations is the same as the RFC-specified
	 * "DES-MAC".  However, the checksum size specified for DES-MAC is 16,
	 * whereas the size used in the code ("sizeof(csfDES_kBlk)") is 8. ???
	 */

	if (key_size != sizeof(csfDES_kBlk)) {
		return CSFC5_STS_BADKEYSIZE;
	}

	switch (csfDESmake_kSched( key, &schedule )) {
	case -1:
		return CSFC5_STS_CRS_DES_BAD_KEYPAR;
	case -2:
		return CSFC5_STS_CRS_DES_WEAK_KEY;
	default:
		break;
	}

	cksum->cks_type = CSFC5cx_CKS_T_DESCBC;
	cksum->cks_data.dl = sizeof(csfDES_kBlk);

	csfDESchecksumCBC( in, cksum->cks_data.dp, in_length, &schedule, key );

	return CSF_STS_OK;
}


csfC5_cksentry csfC5cksDescbcEntry = {
	csfC5cksDescbcFunc,
	sizeof(csfDES_kBlk),
	1,									/* is collision proof */
	1,									/* is keyed */
};
