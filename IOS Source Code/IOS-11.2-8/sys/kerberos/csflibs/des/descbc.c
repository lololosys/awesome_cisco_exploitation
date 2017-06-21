/* $Id: descbc.c,v 3.2 1995/11/17 17:46:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/descbc.c,v $
 *------------------------------------------------------------------
 * descbc.c - DES Engine
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: descbc.c,v $
 * Revision 3.2  1995/11/17  17:46:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original enc_dec.c 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1985, 1986, 1987, 1988, 1990 by the
 * Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * These routines form the library interface to the DES facilities.
 *
 * These routines perform encryption and decryption using the DES
 * private key algorithm, or else a subset of it -- fewer inner loops.
 * (AUTH_DES_ITER defaults to 16, may be less.)
 *
 * Originally written 8/85 by Steve Miller, MIT Project Athena.
 */


#include <csf/header.h>
#include "des0.h"

/*
 * This routine performs DES cipher-block-chaining operation, either
 * encrypting from cleartext to ciphertext, if encrypt != 0 or
 * decrypting from ciphertext to cleartext, if encrypt == 0.
 *
 * The key schedule is passed as an arg, as well as the cleartext or
 * ciphertext.  The cleartext and ciphertext should be in host order.
 *
 * NOTE-- the output is ALWAYS a multiple of 8 bytes long.  If not
 * enough space was provided, your program will get trashed.
 *
 * For encryption, the cleartext string is null padded, at the end, to
 * an integral multiple of eight bytes.
 *
 * For decryption, the ciphertext will be used in integral multiples
 * of 8 bytes, but only the first "len" bytes returned into the
 * cleartext.
 */

csf_sts csfDESEncryptCBC (
	const void *in,			/* >= len bytes of input text */
	void *out,					/* >= len bytes of output text */
	csf_size_t len,			/*  in bytes */
	const csfDES_kSched *key,	/* precomputed key schedule */
	const void *iv				/* 8 bytes of ivec */
){
	register csf_ui32 *input = (csf_ui32 *) in;
	register csf_ui32 *output = (csf_ui32 *) out;
	register csf_ui32 *ivec = (csf_ui32 *) iv;
	csf_ui32 i, j;
	csf_ui32 t_input[2];
	csf_ui32 t_output[2];
	csf_o *t_in_p = (csf_o *) t_input;

	csfCmemcpy( &t_output[0], ivec++, sizeof(t_output[0]) );
	csfCmemcpy( &t_output[1], ivec, sizeof(t_output[1]) );

	for (i = 0; len > 0; i++, len -= 8) {

		/* get input */
		csfCmemcpy( &t_input[0], input++,sizeof(t_input[0]) );
		csfCmemcpy( &t_input[1], input++,sizeof(t_input[1]) );

		/* zero pad */
		if (len < 8) {
			for (j = len; j <= 7; j++) {
				*(t_in_p+j)= 0;
			}
		}

		_csfDBGprint(( "clear",len,t_input[0],t_input[1] ));

		/* do the xor for cbc into the temp */
		t_input[0] ^= t_output[0];
		t_input[1] ^= t_output[1];

		/* encrypt */
		(void) csfDESCryptECB( t_input, t_output, key, TRUE );

		/* copy temp output and save it for cbc */
		csfCmemcpy( output++, &t_output[0], sizeof(t_output[0]) );
		csfCmemcpy( output++, &t_output[1], sizeof(t_output[1]) );

		_csfDBGprint(( "xor'ed",i,t_input[0],t_input[1] ));
		_csfDBGprint(( "cipher",i,t_output[0],t_output[1] ));

	}

	return CSF_STS_OK;

}

csf_sts csfDESDecryptCBC (
	void const *in,			/* >= len bytes of input text */
	void *out,					/* >= len bytes of output text */
	csf_size_t len,			/*  in bytes */
	const csfDES_kSched *key,	/* precomputed key schedule */
	const void *iv				/* 8 bytes of ivec */
){
	register csf_ui32 *input = (csf_ui32 *) in;
	register csf_ui32 *output = (csf_ui32 *) out;
	register csf_ui32 *ivec = (csf_ui32 *) iv;
	csf_ui32 i;
	csf_ui32 t_input[2];
	csf_ui32 t_output[2];
	csf_ui32 xor_0, xor_1;

	csfCmemcpy( &xor_0, ivec++, sizeof(xor_0) );
	csfCmemcpy( &xor_1, ivec, sizeof(xor_1) );

	for (i = 0; len > 0; i++, len -= 8) {

		/* get input */
		csfCmemcpy( &t_input[0], input++, sizeof(t_input[0]) );
		csfCmemcpy( &t_input[1], input++, sizeof(t_input[0]) );

		/* no padding for decrypt */
		_csfDBGprint(( "cipher",i,t_input[0],t_input[1] ));

		/* encrypt */
		(void) csfDESCryptECB( t_input, t_output, key, FALSE );
		_csfDBGprint(( "out pre xor",i,t_output[0],t_output[1] ));

		/* do the xor for cbc into the output */
		t_output[0] ^= xor_0;
		t_output[1] ^= xor_1;

		/* copy temp output */
		csfCmemcpy( output++, &t_output[0], sizeof(t_output[0]) );
		csfCmemcpy( output++, &t_output[1], sizeof(t_output[1]) );

		/* save xor value for next round */
		xor_0 = t_input[0];
		xor_1 = t_input[1];

		_csfDBGprint(( "clear",i,t_output[0],t_output[1] ));

	}

	return CSF_STS_OK;

}
