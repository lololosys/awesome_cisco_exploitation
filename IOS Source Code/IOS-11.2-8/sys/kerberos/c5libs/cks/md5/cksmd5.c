/* $Id: cksmd5.c,v 3.2 1995/11/17 17:43:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cks/md5/cksmd5.c,v $
 *------------------------------------------------------------------
 * cksmd5.c - Calculates MD5 checksums
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cksmd5.c,v $
 * Revision 3.2  1995/11/17  17:43:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original md5glue.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All rights reserved.
 */

/*
 * MD5 checksum
 */

#include <csfc5/header.h>
#include <csfc5/cks/cks0.h>

static csf_sts csfC5cksRsamd5Func (
	void *in,
	csf_size_t in_length,
	void *seed,
	csf_size_t seed_length,
	csfC5_cksBlock *outcksum
){

	csfMD5sum( in, in_length, (csfMD5_cBlk *) outcksum->cks_data.dp );
	outcksum->cks_type = CSFC5c_CKS_T_RSA_MD5;
	outcksum->cks_data.dl = CSFC5c_CKS_T_RSA_MD5_cks;

	return CSF_STS_OK;
}

csfC5_cksentry csfC5cksRsamd5Entry = {
	csfC5cksRsamd5Func,
	CSFC5c_CKS_T_RSA_MD5_cks,
	1,									/* is collision proof */
	0,									/* doesn't use key */
};
