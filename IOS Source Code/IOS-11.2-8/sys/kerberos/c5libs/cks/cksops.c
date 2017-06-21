/* $Id: cksops.c,v 3.2 1995/11/17 17:43:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/cks/cksops.c,v $
 *------------------------------------------------------------------
 * cksops.c - Functions to manipulate Kerberos checksums
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cksops.c,v $
 * Revision 3.2  1995/11/17  17:43:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:17:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original cryptoconf.c 3.2 5/11/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Checksum configuration
 */

#include <csfc5/header.h>
#include <csfc5/cks/cks0.h>


#if (defined( CSFC5_CFG_CRS_DES_CBC_CRC ) && CSFC5_CFG_CRS_DES_CBC_CRC) || \
	(defined( CSFC5_CFG_CKS_CRC32 ) && CSFC5_CFG_CKS_CRC32)
extern csfC5_cksentry				csfC5cksCrc32Entry;
#define CRC32_CKENTRY				&csfC5cksCrc32Entry
#else
#define CRC32_CKENTRY 0
#endif


#if defined( CSFC5_CFG_CKS_RSA_MD4 ) && CSFC5_CFG_CKS_RSA_MD4
extern csfC5_cksentry			csfC5cksRsamd4Entry;
#define MD4_CKENTRY				&csfC5cksRsamd4Entry
extern csfC5_cksentry			csfC5cksRsamd4descbcEntry;
#define MD4_DES_CKENTRY			&csfC5cksRsamd4descbcEntry
#else
#define MD4_CKENTRY 0
#define MD4_DES_CKENTRY 0
#endif


#if defined( CSFC5_CFG_CKS_RSA_MD5 ) && CSFC5_CFG_CKS_RSA_MD5
extern csfC5_cksentry			csfC5cksRsamd5Entry;
#define MD5_CKENTRY				&csfC5cksRsamd5Entry
extern csfC5_cksentry			csfC5cksRsamd5descbcEntry;
#define MD5_DES_CKENTRY			&csfC5cksRsamd5descbcEntry
#else
#define MD5_CKENTRY 0
#define MD5_DES_CKENTRY 0
#endif

#if defined( CSFC5_CFG_CKS_DES_CBC_CKSUM ) && CSFC5_CFG_CKS_DES_CBC_CKSUM
extern csfC5_cksentry			csfC5cksDescbcEntry;
#define DES_CBC_CKENTRY			&csfC5cksDescbcEntry
#else
#define DES_CBC_CKENTRY 0
#endif

csfC5_cksentry *csfC5cksArray0[] = {
	0,
	CRC32_CKENTRY,              /* CKSUMTYPE_CRC32 */
	MD4_CKENTRY,                /* CKSUMTYPE_RSA_MD4 */
	MD4_DES_CKENTRY,            /* CKSUMTYPE_RSA_MD4_DES */
	DES_CBC_CKENTRY,            /* CKSUMTYPE_DESCBC */
	0,                          /* des-mac */
	0,                          /* des-mac-k */
	MD5_CKENTRY,                /* CKSUMTYPE_RSA_MD5 */
	MD5_DES_CKENTRY             /* CKSUMTYPE_RSA_MD5_DES */
};

csf_size_t csfC5cksMax0 =
	sizeof(csfC5cksArray0)/sizeof(csfC5cksArray0[0]);

/*
 * Universal operations.
 */

csf_sts csfC5setSumType_cksBlock (
	csfC5_cksType ckstype,
	csfC5_cksBlock *cksblock
){
	csf_sts rsts;

	csfC5clear_cksBlock( cksblock );
	rsts = csfC5check_cksType( ckstype );
	if (rsts) {
		return rsts;
	}
	rsts = csfC5malloc_data( csfC5size_cksType( ckstype ), &cksblock->cks_data );
	if (rsts) {
		return rsts;
	}
	cksblock->cks_type = ckstype;

	return CSF_STS_OK;
}


csf_sts csfC5cksSumData (
	const csfC5_data *indata,
	const csfC5_cksType ckstype,
	const csfC5_data *seed,
	csfC5_cksBlock *cksblock
){
	csf_sts rsts;

	rsts = csfC5setSumType_cksBlock( ckstype, cksblock );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5cksCalculate(
		ckstype,
		csfC5getPtr_data( indata ),
		csfC5getLen_data( indata ),
		csfC5getPtr_data( seed ),
		csfC5getLen_data( seed ),
		cksblock
	);
	if (rsts) {
		csfC5free_cksBlock( cksblock );
	}

	return rsts;
}


/*
 * The following functions are all used to insulate direct access
 * to the above tables by the rest of the world.
 * The reason for using functions instead of dispatching directly
 * through the tables (as the original code did) is both for space
 * efficiency and for safety: we can ensure we are not jumping off
 * into the weeds.  Presently, these functions are no more than
 * the original macros; consistency checking to be added.
 */


csf_size_t csfC5size_cksType (
	csfC5_cksType cktype
){
	csf_size_t rval;

	rval = csfC5cksArray0[cktype]->cks_length;
	return rval;
}

csf_sts csfC5check_cksType (
	csfC5_cksType cktype
){

	if (
		(cktype > 0) &&
		(cktype <= csfC5cksMax0) &&
		csfC5cksArray0[cktype]
	) {
		return CSF_STS_OK;
	}
	else {
		return CSFC5_STS_PROG_SUMTYPE_NOSUPP;
	}

}

csf_boolean csfC5isCollProof_cksType (
	csfC5_cksType cktype
){
	csf_boolean rval;

	rval = csfC5cksArray0[cktype]->is_collision_proof;
	return rval;
}

csf_boolean csfC5isKeyed_cksType (
	csfC5_cksType cktype
){
	csf_boolean rval;

	rval = csfC5cksArray0[cktype]->uses_key;
	return rval;
}

csf_sts csfC5cksCalculate (
	csfC5_cksType cktype,
	void *in,
	csf_size_t in_len,
	void *seed,
	csf_size_t seed_len,
	csfC5_cksBlock *out
){
	csf_sts rsts;

	rsts = (*csfC5cksArray0[cktype]->sum_func)
		( in, in_len, seed, seed_len, out );
	return rsts;
}
