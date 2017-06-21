/* $Id: cksmd4.c,v 3.2 1995/11/17 17:43:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cks/md4/cksmd4.c,v $
 *------------------------------------------------------------------
 * cksmd4.c - Calculates MD4 checksums
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cksmd4.c,v $
 * Revision 3.2  1995/11/17  17:43:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original md4glue.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * MD4 checksum function.
 */

#include <csfc5/header.h>
#include <csfc5/cks/cks0.h>

static csf_sts csfC5cksRsamd4Func (
	void *in,
	csf_size_t in_length,
	void *seed,
	csf_size_t seed_length,
	csfC5_cksBlock *outcksum
){

	csfMD4sum( in, in_length, (csfMD4_cBlk *) outcksum->cks_data.dp );
	outcksum->cks_type = CSFC5c_CKS_T_RSA_MD4;
	outcksum->cks_data.dl = CSFC5c_CKS_T_RSA_MD4_cks;

	return CSF_STS_OK;
}

csfC5_cksentry csfC5cksRsamd4Entry = {
	csfC5cksRsamd4Func,
	CSFC5c_CKS_T_RSA_MD4_cks,
	1,									/* is collision proof */
	0,									/* doesn't use key */
};
