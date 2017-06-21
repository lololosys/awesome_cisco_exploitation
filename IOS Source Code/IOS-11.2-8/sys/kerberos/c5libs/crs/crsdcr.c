/* $Id: crsdcr.c,v 3.2 1995/11/17 17:43:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/crsdcr.c,v $
 *------------------------------------------------------------------
 * crsdcr.c - Kerberos crypto functions for data
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crsdcr.c,v $
 * Revision 3.2  1995/11/17  17:43:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 */

#include <csfc5/header.h>

csf_sts csfC5crsDecryptData (
	const csfC5_data *in,
	const csfC5_crsKBlock *kblock,
	const csfC5_crsEType etype,
	void *ivec,
	csfC5_data *out
){
	csf_sts rsts;
	csf_size_t ilen;
	csfC5_crsEBlock eblock;

	csfC5clear_data( out );

	rsts = csfC5setEType_crsEBlock( &eblock, etype );
	if (rsts) {
		return rsts;
	}

	ilen = csfC5getLen_data( in );
	rsts = csfC5malloc_data( ilen, out );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5crsProcessKey( &eblock, kblock );
	if (rsts) {
		csfC5free_data( out );
		return rsts;
	}

	rsts = csfC5crsDecrypt(
		csfC5getPtr_data( in ),
		csfC5getPtr_data( out ),
		ilen,
		&eblock,
		ivec
	);
	if (rsts) {
		(void) csfC5crsFinishKey( &eblock );
		csfC5free_data( out );
		return rsts;
	}

	rsts = csfC5crsFinishKey( &eblock );
	if (rsts) {
		csfC5free_data( out );
		return rsts;
	}

	if (ivec) {
		int ebsize;

		ebsize = csfC5blockSize_crsEBlock( &eblock );
		csfCmemcpy(
			ivec,
			_csfCptrInc( csfC5getPtr_data( in ), ilen - ebsize ),
			ebsize
		);
	}

	return rsts;
}


csf_sts csfC5crsEncryptData (
	const csfC5_data *in,
	const csfC5_crsKBlock *kblock,
	const csfC5_crsEType etype,
	void *ivec,
	csfC5_data *out
){
	csf_sts rsts;
	csf_size_t ilen;
	csf_size_t olen;
	csfC5_crsEBlock eblock;

	csfC5clear_data( out );

	rsts = csfC5setEType_crsEBlock( &eblock, etype );
	if (rsts) {
		return rsts;
	}

	ilen = csfC5getLen_data( in );
	olen = csfC5crsEncryptSize( ilen, &eblock );
	rsts = csfC5malloc_data( olen, out );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5crsProcessKey( &eblock, kblock );
	if (rsts) {
		csfC5free_data( out );
		return rsts;
	}

	rsts = csfC5crsEncrypt(
		csfC5getPtr_data( in ),
		csfC5getPtr_data( out ),
		ilen,
		&eblock,
		ivec
	);
	if (rsts) {
		(void) csfC5crsFinishKey( &eblock );
		csfC5free_data( out );
		return rsts;
	}

	rsts = csfC5crsFinishKey( &eblock );
	if (rsts) {
		csfC5free_data( out );
		return rsts;
	}

	if (ivec) {
		int ebsize;

		ebsize = csfC5blockSize_crsEBlock( &eblock );
		csfCmemcpy(
			ivec,
			_csfCptrInc( csfC5getPtr_data( out ), olen - ebsize ),
			ebsize
		);
	}

	return rsts;
}
