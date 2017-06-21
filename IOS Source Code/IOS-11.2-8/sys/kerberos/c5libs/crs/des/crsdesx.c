/* $Id: crsdesx.c,v 3.2 1995/11/17 17:44:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/des/crsdesx.c,v $
 *------------------------------------------------------------------
 * crsdesx.c - Kerberos DES crypto interface
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crsdesx.c,v $
 * Revision 3.2  1995/11/17  17:44:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original krb_glue.c 3.2 5/11/94*/
/*.original string2key.c 3.2 5/11/94*/
/*.original process_ky.c 3.1 12/29/93*/
/*.original finish_key.c 3.3 3/24/94*/
/*.original init_rkey.c 3.1 12/29/93*/
/*.original fin_rndkey.c 3.2 3/24/94*/
/*.original random_key.c 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1985, 1986, 1987, 1988, 1990, 1991  by the
 * Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * These routines form the basic interface to the DES facilities for
 * encryption and decryption.  These functions are used by both the
 * "standard" calls, and the "raw" calls.
 *
 */


#include <csfc5/header.h>
#include "des0.h"


/*
encrypts "size" bytes at "in", storing result in "out".
"eblock" points to an encrypt block which has been initialized
by process_key().

"out" must be preallocated by the caller to contain sufficient
storage to hold the output; the macro csfC5encryptSize() can
be used to compute this size.

returns: errors
*/

csf_sts csfC5crsDesEncryptF (
	void const *in,
	void *out,
	const csf_size_t len,
	csfC5_crsEBlock *key,
	void *ivec
){
	void *iv;
	csf_sts rsts;

	if (ivec == NULL) {
		iv = csfC5getPtr_data( &key->key->key_data );
	}
	else {
		iv = ivec;
	}

	rsts = csfDESEncryptCBC(
		in,
		out,
		len,
		(csfDES_kSched *) csfC5getPtr_data( &key->eb_data ),
		iv
	);

	return rsts;
}


/*

decrypts "size" bytes at "in", storing result in "out".
"eblock" points to an encrypt block which has been initialized
by process_key().

"out" must be preallocated by the caller to contain sufficient
storage to hold the output; this is guaranteed to be no more than
the input size.

returns: errors

*/

csf_sts csfC5crsDesDecryptF (
	void const *in,
	void *out,
	const csf_size_t size,
	csfC5_crsEBlock *key,
	void *ivec
){
	void *iv;
	csf_sts rsts;

	if (ivec == NULL) {
		iv = csfC5getPtr_data( &key->key->key_data );
	}
	else {
		iv = ivec;
	}

	rsts = csfDESDecryptCBC(
		in,
		out,
		size,
		(csfDES_kSched *) csfC5getPtr_data( &key->eb_data ),
		iv
	);

	return rsts;
}

/*
converts the string pointed to by "data" into an encryption key
of type "keytype".  *keyblock is filled in with the key info;
in particular, keyblock->contents is to be set to allocated storage.
It is the responsibility of the caller to release this storage
when the generated key no longer needed.

The routine may use "salt" to seed or alter the conversion
algorithm.

If the particular function called does not know how to make a
key of type "keytype", an error may be returned.

returns: errors
*/

csf_sts csfC5crsDesStringToKey (
	const csfC5_crsKType keytype,
	csfC5_crsKBlock *keyblock,
	const csfC5_data *data,
	const csfC5_data *salt
){
	csf_sts rsts;

	if (keytype != CSFC5c_CRS_KT_DES) {
		return CSFC5_STS_PROG_KEYTYPE_NOSUPP;
	}

	rsts = csfC5malloc_data( sizeof(csfDES_kBlk), &keyblock->key_data );
	if (rsts) {
		return rsts;
	}

	rsts = csfDESstringTo_kBlk(
		csfC5getPtr_data( data ), csfC5getLen_data( data ),
		csfC5getPtr_data( salt ), csfC5getLen_data( salt ),
		(csfDES_kBlk *) csfC5getPtr_data( &keyblock->key_data )
	);

	if (rsts) {
		csfC5free_data( &keyblock->key_data );
		return rsts;
	}

	keyblock->key_type = CSFC5c_CRS_KT_DES;
	return rsts;
}

/*
does any necessary key preprocessing (such as computing key
schedules for DES).

eblock->crypto_entry must be set by the caller; the other elements
of eblock are to be assigned by this function.
[in particular, eblock->key must be set by this function if the key
is needed in raw form by the encryption routine]

The caller may not move or reallocate "keyblock" before calling
finish_key on "eblock"

returns: errors
 */

csf_sts csfC5crsDesProcessKey (
	csfC5_crsEBlock *eblock,
	const csfC5_crsKBlock *keyblock
){
	csfDES_kSched *ksched = NULL;
	void *kdata = NULL;
	csf_sts rsts = CSF_STS_OK;

	eblock->key = NULL;
	csfC5clear_data( &eblock->eb_data );

	if (csfC5getLen_data( &keyblock->key_data ) != sizeof(csfDES_kBlk)) {
		return CSFC5_STS_BADKEYSIZE;
	}
	kdata = csfC5getPtr_data( &keyblock->key_data );

	rsts = csfC5malloc_data( sizeof(csfDES_kSched), &eblock->eb_data );
	if (rsts) {
		return rsts;
	}
	ksched = (csfDES_kSched *) csfC5getPtr_data( &eblock->eb_data );

	switch (csfDESmake_kSched( kdata, ksched )) {
	case -1:
		rsts = CSFC5_STS_CRS_DES_BAD_KEYPAR;
		break;
	case -2:
		rsts = CSFC5_STS_CRS_DES_WEAK_KEY;
		break;
	default:
		break;
	}

	if (rsts) {
		csfC5free_data( &eblock->eb_data );
		return rsts;
	}

	eblock->key = (csfC5_crsKBlock *) keyblock; /*TBD/JNK--non-const cast*/
	return CSF_STS_OK;

}


/*
 * Does any necessary clean-up on the eblock (such as releasing
 * resources held by eblock->priv).
 * returns: errors
 */

csf_sts csfC5crsDesFinishKey (
	csfC5_crsEBlock *eblock
){
	csfC5free_data( &eblock->eb_data );
	return CSF_STS_OK;
}


/*
 initialize the random key generator using the encryption key,
 "seedblock", and allocating private sequence information, filling
 in "seed" with the haddr of such information.
 "seed" is later passed to the random_key() function to provide
 sequence information.
 */

csf_sts csfC5crsDesInitRandomKey (
	const csfC5_crsKBlock *seedblock,
	void **seed
){
	csfDESRAN_kSeed *p_seed;
	csfDES_kBlk hseed;		/*TBD/JNK--initialize*/
	csf_ui32 tseed[2];		/*TBD/JNK--initialize*/

	if (seedblock->key_type != CSFC5c_CRS_KT_DES) {
		return CSFC5_STS_BADKEYTYPE;
	}

	p_seed = (csfDESRAN_kSeed *)
		csfCcmalloc( sizeof(csfDESRAN_kSeed) );
	if (!p_seed) {
		return CSF_STS_ENOMEM;
	}

	csfDESRANinit( csfC5getPtr_data( &seedblock->key_data ), p_seed,
		hseed, tseed );
	*seed = (void *) p_seed;

	return CSF_STS_OK;
}

/*
 *free any resources held by "seed" and assigned by init_random_key()
 */

csf_sts csfC5crsDesFinishRandomKey (
	void **seed
){
	if (*seed) {
		csfCmemzero( *seed, sizeof(csfDESRAN_kSeed) );
		csfCfree( *seed );
		*seed = NULL;
	}

	return CSF_STS_OK;
}


/*
generate a random encryption key, allocating storage for it and
filling in the keyblock
*/

csf_sts csfC5crsDesRandomKey (
	void *seed,
	csfC5_crsKBlock *keyblock
){
	csfC5_crsKBlock randkey;
	csf_sts rsts;

	csfC5clear_crsKBlock( keyblock );
	rsts = csfC5malloc_data( sizeof(csfDES_kBlk), &randkey.key_data );
	if (rsts) {
		return rsts;
	}
	randkey.key_type = CSFC5c_CRS_KT_DES;
	csfDESRANnewKey(
		csfC5getPtr_data( &randkey.key_data ),
		(csfDESRAN_kSeed *)seed
	);
	*keyblock = randkey;

	return CSF_STS_OK;
}
