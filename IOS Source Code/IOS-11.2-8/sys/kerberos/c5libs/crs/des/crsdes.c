/* $Id: crsdes.c,v 3.2 1995/11/17 17:43:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/des/crsdes.c,v $
 *------------------------------------------------------------------
 * crsdes.c - Kerberos DES crypto code
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crsdes.c,v $
 * Revision 3.2  1995/11/17  17:43:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original krb_glue.c 3.2 5/11/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1985, 1986, 1987, 1988, 1990, 1991  by the
 * Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * These routines form the basic interface to the DES facilities for
 * encryption and decryption.  These  calls generate and check the
 * confounder and (crc-32) checksum (unlike the "raw" calls).
 */


#include <csfc5/header.h>
#include "des0.h"

/*
 * Encrypt with confounder and checsum.
 */

csf_sts csfC5crsDesEncryptFunc (
	void const *in,
	void *out,
	const csf_size_t size,
	csfC5_crsEBlock *key,
	void *ivec
){
	csfC5_cksBlock cksum;
	csf_o contents[CSFC5c_CKS_T_CRC32_cks];
	csf_size_t sumsize;
	csf_sts rsts;

#if 0 /*TBD/JNK*/
	if (size < sizeof(csfDES_kBlk)) {
		return CSFC5_STS_BADMSIZE;
	}
#endif

	/*
		caller passes data size, and saves room for the padding.

		format of ciphertext, per RFC is:

		+-----------+----------+-------------+-----+
		|confounder |   check  |   msg-seq   | pad |
		+-----------+----------+-------------+-----+

		our confounder is 8 bytes (one cblock);
		our checksum is CSFC5c_CKS_T_CRC32_cks

	 */

	sumsize =  _csfCroundUp( size + CSFC5c_CKS_T_CRC32_cks +
		sizeof(csfDES_kBlk), sizeof(csfDES_kBlk) );

	/* assemble crypto input into the output area, then encrypt in place. */
	csfCmemzero( out, sumsize );

	/* put in the confounder */
	rsts = csfRANconfounder( sizeof(csfDES_kBlk), out );
	if (rsts) {
		return rsts;
	}

	csfCmemcpy(
		_csfCptrInc( out, sizeof(csfDES_kBlk) + CSFC5c_CKS_T_CRC32_cks ),
		in,
		size
	);

	cksum.cks_data.dp = contents;

	/*TBD/JNK--shared library problem?
	 * This is equivalent to csfC5cksCalculate( CSFC5c_CKS_T_CRC32, ... )
	 * but avoids use of the cryptosystem config table which can not be
	 * referenced here if this object is to be included in a shared library.
	 */
	rsts = csfC5cksCrc32( out, sumsize, &cksum );
	if (rsts) {
		return rsts;
	}

	csfCmemcpy(
		_csfCptrInc( out, sizeof(csfDES_kBlk) ),
		contents,
		CSFC5c_CKS_T_CRC32_cks
	);

	/* We depend here on the ability of this DES implementation to
	 * encrypt plaintext to ciphertext in-place.
	 */
	return csfC5crsDesEncryptF( out, out, sumsize, key, ivec );
}


/*
 * Decrypt and check checksum.
 */

csf_sts csfC5crsDesDecryptFunc (
	void const *in,
	void *out,
	const csf_size_t size,
	csfC5_crsEBlock *key,
	void *ivec
){
	csfC5_cksBlock cksum;
	csf_o contents_prd[CSFC5c_CKS_T_CRC32_cks];
	csf_o contents_get[CSFC5c_CKS_T_CRC32_cks];
	void *p;
	csf_sts rsts;

	if (size < 2 * sizeof(csfDES_kBlk)) {
		return CSFC5_STS_BADMSIZE;
	}

	rsts = csfC5crsDesDecryptF( in, out, size, key, ivec );
	if (rsts) {
		return rsts;
	}

	cksum.cks_data.dp = contents_prd;
	p = _csfCptrInc( out, sizeof(csfDES_kBlk) );
	csfCmemcpy( contents_get, p, CSFC5c_CKS_T_CRC32_cks );
	csfCmemzero( p, CSFC5c_CKS_T_CRC32_cks );

	rsts = csfC5cksCrc32( out, size, &cksum );
	if (rsts) {
		return rsts;
	}

	if (csfCmemcmp( contents_get, contents_prd, CSFC5c_CKS_T_CRC32_cks )) {
		return CSFC5c_STS_APERR_BAD_INTEGRITY;
	}

	csfCmemcpy (
		out,
		_csfCptrInc( out, sizeof(csfDES_kBlk) + CSFC5c_CKS_T_CRC32_cks ),
		size - sizeof(csfDES_kBlk) - CSFC5c_CKS_T_CRC32_cks
	);

	return CSF_STS_OK;
}


/*
 * Crypto configuration.
 */

csfC5_crsEntry csfC5crsDesEntry = {
	csfC5crsDesEncryptFunc,
	csfC5crsDesDecryptFunc,
	csfC5crsDesProcessKey,
	csfC5crsDesFinishKey,
	csfC5crsDesStringToKey,
	csfC5crsDesInitRandomKey,
	csfC5crsDesFinishRandomKey,
	csfC5crsDesRandomKey,
	sizeof(csfDES_kBlk),
	CSFC5c_CKS_T_CRC32_cks + sizeof(csfDES_kBlk),
	sizeof(csfDES_kBlk),
	CSFC5c_CRS_ET_DES_CBC_CRC,
	CSFC5c_CRS_KT_DES
};
