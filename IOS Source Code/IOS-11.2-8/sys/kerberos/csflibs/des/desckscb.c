/* $Id: desckscb.c,v 3.2 1995/11/17 17:46:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/desckscb.c,v $
 *------------------------------------------------------------------
 * desckscb.c - DES Engine
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: desckscb.c,v $
 * Revision 3.2  1995/11/17  17:46:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original cksum.c 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1985, 1986, 1987, 1988, 1990 by the
 * Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Under U.S. law, this software may not be exported outside the US
 * without license from the U.S. Commerce department.
 */

/*
 * These routines form the library interface to the DES facilities.
 *      spm     8/85    MIT project athena
 */

#include <csf/header.h>

/*
 * This routine performs DES cipher-block-chaining checksum operation,
 * a.k.a.  Message Authentication Code.  It ALWAYS encrypts from input
 * to a single 64 bit output MAC checksum.
 *
 * The key schedule is passed as an arg, as well as the cleartext or
 * ciphertext. The cleartext and ciphertext should be in host order.
 *
 * NOTE-- the output is ALWAYS 8 bytes long.  If not enough space was
 * provided, your program will get trashed.
 *
 * The input is null padded, at the end (highest addr), to an integral
 * multiple of eight bytes.
 */

void csfDESchecksumCBC (
	const void *in,				/* >= length bytes of inputtext */
	void *out,						/* >= length bytes of outputtext */
	csf_size_t length,				/* in bytes */
	const csfDES_kSched *key,		/* precomputed key schedule */
	const void *iv					/* 8 bytes of ivec */
){
	csf_ui8 *input = (csf_ui8 *) in;
	csf_ui8 *output = (csf_ui8 *) out;
	csf_ui32 t_input[2];
	csf_ui32 t_output[2];

	/*TBD/JNK--very inefficient.  Could elminate multiple memcpy's--and
	 * unroll the loop below--by aligning the input into a temporary
	 * array bigger than just 8 bytes.  A bit of inefficiency was
	 * introduced when the run-time "need for alignment" checks were
	 * removed--but not much; with a reasonable compiler, the memcpy
	 * should take little or no more time than the check did.  (Any loss
	 * was more than made up for by the elimination of the padding check
	 * and loop from the main loop.)
	 */

	/* Get initialization vector.
	 */
	csfCmemcpy( &t_output[0], iv, 8 );	/* align */

	/* Process inputs (in 8 byte chunks).
	 */
	for (; length >= 8; length -= 8) {
		/* align */
		csfCmemcpy( &t_input[0], input, 8 );
		input += 8;
		_csfDBGprint(( "clear", length, t_input[0], t_input[1] ));
		/* xor previous (or ivec) back in */
		t_input[0] ^= t_output[0];
		t_input[1] ^= t_output[1];
		/* encrypt */
		(void) csfDESCryptECB( t_input, t_output, key, TRUE /*encrypt*/ );
		_csfDBGprint(( "xor'ed", i ,t_input[0], t_input[1] ));
		_csfDBGprint(( "cipher", i, t_output[0], t_output[1] ));
	}

	if (length > 0) {
		/* zero pad */
		t_input[0] = 0;
		t_input[1] = 0;
		/* copy remainder */
		csfCmemcpy( &t_input[0], input, length );
		/* and do as above */
		/* xor previous (or ivec) back in */
		t_input[0] ^= t_output[0];
		t_input[1] ^= t_output[1];
		/* encrypt */
		(void) csfDESCryptECB( t_input, t_output, key, TRUE /*encrypt*/ );
		_csfDBGprint(( "xor'ed", i ,t_input[0], t_input[1] ));
		_csfDBGprint(( "cipher", i, t_output[0], t_output[1] ));
	}

	/* copy temp output and save it for checksum */
	csfCmemcpy( output, &t_output[0], 8 );

}
