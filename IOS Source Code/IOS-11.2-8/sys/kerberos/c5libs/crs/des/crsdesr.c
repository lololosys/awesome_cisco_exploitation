/* $Id: crsdesr.c,v 3.2 1995/11/17 17:44:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/des/crsdesr.c,v $
 *------------------------------------------------------------------
 * crsdesr.c - Kerberos DES crypto code
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crsdesr.c,v $
 * Revision 3.2  1995/11/17  17:44:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:07  shaker
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
 * encryption and decryption.  These "raw" calls differ from the
 * standard calls in that they do not generate/check either the
 * confounder or the checksum.
 */


#include <csfc5/header.h>
#include "des0.h"

/*
 * Encrypt without confounder or checksum.
 */

csf_sts csfC5crsDesrawEncryptFunc (
	void const *in,
	void *out,
	const csf_size_t size,
	csfC5_crsEBlock *key,
	void *ivec
){
	csf_size_t sumsize;
	csf_sts rsts;

	/* round up to des block size */
	sumsize =  _csfCroundUp( size, sizeof(csfDES_kBlk) );

	/* assemble crypto input into the output area, then encrypt in place. */
	csfCmemzero( out, sumsize );
	csfCmemcpy( out, in, size );

	/* We depend here on the ability of this DES implementation to
	 * encrypt plaintext to ciphertext in-place.
	 */
	rsts = csfC5crsDesEncryptF( out, out, sumsize, key, ivec );

	return rsts;
}


/*
 * Decrypt without confounder or checksum.
 */

csf_sts csfC5crsDesrawDecryptFunc (
	void const *in,
	void *out,
	const csf_size_t size,
	csfC5_crsEBlock *key,
	void *ivec
){
	csf_sts rsts;

	rsts = csfC5crsDesDecryptF( in, out, size, key, ivec );
	return rsts;
}


/*
 * Crypto configuration.
 */

csfC5_crsEntry csfC5crsDesrawEntry = {
	csfC5crsDesrawEncryptFunc,
	csfC5crsDesrawDecryptFunc,
	csfC5crsDesProcessKey,
	csfC5crsDesFinishKey,
	csfC5crsDesStringToKey,
	csfC5crsDesInitRandomKey,
	csfC5crsDesFinishRandomKey,
	csfC5crsDesRandomKey,
	sizeof(csfDES_kBlk),
	0,
	sizeof(csfDES_kBlk),
	CSFC5cx_CRS_ET_DES_CBC_RAW,
	CSFC5c_CRS_KT_DES
};
