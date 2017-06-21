/* $Id: crsgensq.c,v 3.2 1995/11/17 17:43:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/crs/crsgensq.c,v $
 *------------------------------------------------------------------
 * crsgensq.c - Generates crypto sequence number
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crsgensq.c,v $
 * Revision 3.2  1995/11/17  17:43:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original gen_seqno.c 3.2 6/15/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Routine to automatically generate a starting sequence number.
 * We do this by getting a random key and encrypting the time with it,
 * then return the first 32 bits.
 */

#include <csfc5/header.h>

csf_sts csfC5crsGenSeqNo (
	const csfC5_crsKBlock *key,
	csfC5_seqNo *seqno
){
	void *random_state;
	csfC5_crsEBlock eblock;
	csfC5_crsKBlock subkey;
	csf_sts rsts;
	csf_o *intmp;
	csf_o *outtmp;
	csfC5_usTStamp utnow;
	int esize;


	/* build a random key */
	rsts = csfC5setKType_crsEBlock( &eblock, key->key_type );
	if (rsts) {
		return rsts;
	}
	rsts = csfC5crsInitRanKey( &eblock, key, &random_state );
	if (rsts) {
		return rsts;
	}

	rsts = csfC5crsGenRanKey( &eblock, random_state, &subkey );
	if (rsts) {
		(void) csfC5crsFinishRanKey( &eblock, &random_state );
		/*TBD/JNK--free keyblock?*/
		return rsts;
	}    

	rsts = csfC5crsFinishRanKey( &eblock, &random_state );
	if (rsts) {
		csfC5free_crsKBlock( &subkey );
		return rsts;
	}

	esize = csfC5crsEncryptSize( sizeof(utnow), &eblock );
	intmp = (csf_o *) csfCmalloc( esize );
	if (!intmp) {
		csfC5free_crsKBlock( &subkey );
		return CSF_STS_ENOMEM;
	}

	outtmp = (csf_o *) csfCmalloc( esize );
	if (!outtmp) {
		csfCfree( intmp );
		csfC5free_crsKBlock( &subkey );
		return CSF_STS_ENOMEM;
	}

	rsts = csfC5crsProcessKey( &eblock, &subkey );
	if (rsts) {
		csfC5free_crsKBlock( &subkey );
		csfCfree( intmp );
		csfCfree( outtmp );
		return rsts;
	}

	/* get the current time, so we can encrypt it */
	rsts = csfC5get_usTStamp( &utnow );
	if (rsts) {
		csfC5free_crsKBlock( &subkey );
		csfCfree( intmp );
		csfCfree( outtmp );
		return rsts;
	}

	/* 
	 * turn the time into something we can encrypt 
	 * XXX the following csfCmemcpy won't work if there is a hole in the timev 
	 * structure!
	 */
	(void) csfCmemcpy( intmp, &utnow, sizeof(utnow) );

	/* encrypt it */
	rsts = csfC5crsEncrypt( intmp, outtmp, sizeof(utnow), &eblock, 0 );
	if (rsts) {
		/* encryption failed */
		(void) csfC5crsFinishKey( &eblock );
		csfC5free_crsKBlock( &subkey );
		csfCfree( intmp );
		csfCfree( outtmp );
		return rsts;
	}

	/* copy the random encrypted stuff back to the sequence number param */
	(void) csfCmemcpy( seqno, outtmp, sizeof(csfC5_seqNo) );


	/* all done, clean up */
	(void) csfC5crsFinishKey( &eblock );
	csfC5free_crsKBlock( &subkey );
	csfCfree( intmp );
	csfCfree( outtmp );
	return rsts;
}


#ifdef UNIT_TEST

#include <csfc5/header.h>

main(argc, argv)
int argc;
char **argv;
{
	csf_ui32 i;
	csfC5_crsKBlock k;
	int n = 1;
	int j;

	if (argc > 1) {
		n = atoi( *++argv );
	}

	for (j = 0; j < n; ++j) {
		k.keytype = KEYTYPE_DES;
		k.length = 64;
		k.contents = (csf_o *) csfCmalloc( 64 );
		csfC5crsGenSeqNo( &k, &i );
		printf( "Random sequence number #%d = %d\n", j + 1, (int)i );
	}
}

#endif /* UNIT_TEST */
