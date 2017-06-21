/* $Id: crsops.c,v 3.2 1995/11/17 17:43:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/crsops.c,v $
 *------------------------------------------------------------------
 * crsops.c - Kerberos crypto subsystem interface
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crsops.c,v $
 * Revision 3.2  1995/11/17  17:43:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:04  shaker
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
 * Cryptosystem configuration & dispatch
 */

#include <csfc5/header.h>
#include <csfc5/crs/crs0.h>


#if defined( CSFC5_CFG_CRS_DES_CBC_CRC ) && CSFC5_CFG_CRS_DES_CBC_CRC
extern csfC5_crsEntry			csfC5crsDesEntry;
#define DES_CBC_CRC_CSENTRY		&csfC5crsDesEntry
#else
#define DES_CBC_CRC_CSENTRY 0
#endif

#if defined( CSFC5_CFG_CRS_DES_RAW_CBC ) && CSFC5_CFG_CRS_DES_RAW_CBC
extern csfC5_crsEntry			csfC5crsDesrawEntry;
#define DES_RAW_CBC_CSENTRY		&csfC5crsDesrawEntry
#else
#define DES_RAW_CBC_CSENTRY 0
#endif

/*TBD/JNK--following warning should ref rfc1510.h*/
/* WARNING:
   make sure the order of entries in these tables matches the #defines in
   <csfc5/crypto.h>
 */

csfC5_crsEntry *csfC5crsEtypeArray0[] = {
	0,                          /* ETYPE_NULL */
	DES_CBC_CRC_CSENTRY,        /* ETYPE_DES_CBC_CRC */
	0,                          /* ETYPE_DES_CBC_MD4 */
	0,                          /* ETYPE_DES_CBC_MD5 */
	DES_RAW_CBC_CSENTRY,        /* ETYPE_DES_RAW_CBC */
};

csf_size_t csfC5crsEtypeMax0 =
	sizeof(csfC5crsEtypeArray0)/sizeof(csfC5crsEtypeArray0[0]) - 1;

csfC5_crsEntry *csfC5crsKtypeArray0[] = {
	0,                          /* KEYTYPE_NULL */
	DES_CBC_CRC_CSENTRY         /* KEYTYPE_DES */
};

csf_size_t csfC5crsKtypeMax0 =
	sizeof(csfC5crsKtypeArray0)/sizeof(csfC5crsKtypeArray0[0]) - 1;


static csfC5_crsEntry * getcrsentry (
	csfC5_crsEType etype
){
	if (
		(etype > 0) &&
		(etype <= (csfC5_crsEType) csfC5crsEtypeMax0) &&
		csfC5crsEtypeArray0[etype]
	) {
		return csfC5crsEtypeArray0[etype];
	}
	else {
		return NULL;
	}
	
}

/*
 * The following functions are all used to insulate direct access
 * to the above tables by the rest of the world.
 *
 * The reason for using functions instead of dispatching directly
 * through the tables (as the original code did) is both for space
 * efficiency and for safety: we can ensure we are not jumping off
 * into the weeds.  Presently, these functions are no more than
 * the original macros; consistency checking to be added.
 */


csf_sts csfC5check_crsEType (
	csfC5_crsEType etype
){

	if (
		(etype > 0) &&
		(etype <= (csfC5_crsEType) csfC5crsEtypeMax0) &&
		csfC5crsEtypeArray0[etype]
	) {
		return CSF_STS_OK;
	}
	else {
		return CSFC5_STS_PROG_ETYPE_NOSUPP;
	}

}

csf_sts csfC5check_crsKType (
	csfC5_crsKType ktype
){

	if (
		(ktype > 0) &&
		(ktype <= csfC5crsKtypeMax0) &&
		csfC5crsKtypeArray0[ktype]
	) {
		return CSF_STS_OK;
	}
	else {
		return CSFC5_STS_PROG_KEYTYPE_NOSUPP;
	}

}

csf_sts csfC5setEType_crsEBlock (
	csfC5_crsEBlock *eblock,
	csfC5_crsEType etype
){
	csf_sts rsts;

	rsts = csfC5check_crsEType( etype );
	if (rsts) {
		return rsts;
	}

	eblock->crstype = etype;
	return CSF_STS_OK;
}

csf_sts csfC5setKType_crsEBlock (
	csfC5_crsEBlock *eblock,
	csfC5_crsKType ktype
){
	csf_sts rsts;

	rsts = csfC5check_crsKType( ktype );
	if (rsts) {
		return rsts;
	}

	eblock->crstype = csfC5crsKtypeArray0[ktype]->etype;
	return CSF_STS_OK;
}

csf_sts csfC5getEType_crsKType (
	csfC5_crsKType ktype,
	csfC5_crsEType *etype
){
	csf_sts rsts;

	rsts = csfC5check_crsKType( ktype );
	if (rsts) {
		return rsts;
	}

	*etype = csfC5crsKtypeArray0[ktype]->etype;
	return CSF_STS_OK;
}

csf_sts csfC5getKType_crsEType (
	csfC5_crsEType etype,
	csfC5_crsKType *ktype
){
	csf_sts rsts;

	rsts = csfC5check_crsEType( etype );
	if (rsts) {
		return rsts;
	}

	*ktype = csfC5crsEtypeArray0[etype]->ktype;
	return CSF_STS_OK;
}

csf_size_t csfC5blockSize_crsEBlock (
	csfC5_crsEBlock *eblock
){
	csfC5_crsEntry *crs;
	csf_size_t rval;

	crs = getcrsentry( eblock->crstype );
	rval = crs->block_length;
	return rval;
}

csf_size_t csfC5crsEncryptSize (
	csf_size_t length,
	csfC5_crsEBlock *eblock
){
	csfC5_crsEntry *crs;
	csf_size_t rval;

	crs = getcrsentry( eblock->crstype );
	rval = _csfCroundUp( length + crs->pad_minimum, crs->block_length );
	return rval;
}

csf_sts csfC5crsEncrypt (
	void *inptr,
	void *outptr,
	csf_size_t length,
	csfC5_crsEBlock *eblock,
	void *ivec
){
	csfC5_crsEntry *crs;
	csf_sts rsts;

	crs = getcrsentry( eblock->crstype );
	rsts = (*crs->crsEncrypt) ( inptr, outptr, length, eblock, ivec );
	return rsts;
}

csf_sts csfC5crsDecrypt (
	void *inptr,
	void *outptr,
	csf_size_t size,
	csfC5_crsEBlock *eblock,
	void *ivec
){
	csfC5_crsEntry *crs;
	csf_sts rsts;

	crs = getcrsentry( eblock->crstype );
	rsts = (*crs->crsDecrypt) ( inptr, outptr, size, eblock, ivec );
	return rsts;
}

csf_sts csfC5crsStringToKey (
	csfC5_crsEBlock *eblock,
	csfC5_crsKType keytype,
	csfC5_crsKBlock *keyblock,
	const csfC5_data *data,
	const csfC5_data *salt
){
	csfC5_crsEntry *crs;
	csf_sts rsts;

	crs = getcrsentry( eblock->crstype );
	rsts = (*crs->crsStringToKey) ( keytype, keyblock, data, salt );
	return rsts;
}

csf_sts csfC5crsProcessKey (
	csfC5_crsEBlock *eblock,
	const csfC5_crsKBlock *keyblock
){
	csfC5_crsEntry *crs;
	csf_sts rsts;

	crs = getcrsentry( eblock->crstype );
	rsts = (*crs->crsProcessKey) ( eblock, keyblock );
	return rsts;
}

csf_sts csfC5crsFinishKey (
	csfC5_crsEBlock *eblock
){
	csfC5_crsEntry *crs;
	csf_sts rsts;

	crs = getcrsentry( eblock->crstype );
	rsts = (*crs->crsFinishKey) ( eblock );
	return rsts;
}

csf_sts csfC5crsInitRanKey (
	csfC5_crsEBlock *eblock,
	const csfC5_crsKBlock *keyblock,
	void **ptr
){
	csfC5_crsEntry *crs;
	csf_sts rsts;

	crs = getcrsentry( eblock->crstype );
	rsts = (*crs->crsInitRanKey) ( keyblock, ptr );
	return rsts;
}

csf_sts csfC5crsFinishRanKey (
	const csfC5_crsEBlock *eblock,
	void **ptr
){
	csfC5_crsEntry *crs;
	csf_sts rsts;

	crs = getcrsentry( eblock->crstype );
	rsts = (*crs->crsFinishRanKey)
		( ptr );
	return rsts;
}

csf_sts csfC5crsGenRanKey (
	csfC5_crsEBlock *eblock,
	void *ptr,
	csfC5_crsKBlock *keyblock
){
	csfC5_crsEntry *crs;
	csf_sts rsts;

	crs = getcrsentry( eblock->crstype );
	rsts = (*crs->crsGenRanKey) ( ptr, keyblock );
	return rsts;
}

