/* $Id: desran.c,v 3.2 1995/11/17 17:46:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/desran.c,v $
 *------------------------------------------------------------------
 * desran.c - DES Engine
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: desran.c,v $
 * Revision 3.2  1995/11/17  17:46:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original new_rn_key.c 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1988,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Originally written by Mark Lillibridge, MIT Project Athena
 */

/*
 * Pseudo-random key generator, using DES encryption to make the
 * pseudo-random cycle as hard to break as DES.
 *
 * This module implements a random number generator facility such that the next
 * number in any random number stream is very hard to predict without knowing
 * the seed for that stream even given the preceeding random numbers.
 */


#include <csf/header.h>

/*
Create a random des key.  csfDES_set_random_number_generater_seed must
be at called least once before this routine is called.

The returned key has correct parity and is guarenteed not to be a weak des key.
*/

void csfDESRANnewKey (
	csfDES_kBlk key,
	csfDESRAN_kSeed *p_seed
){
	do {
		csfDESRANgenBlk( key, p_seed );
		csfDESfixParity_kBlk( key );
	} while (csfDESisWeak_kBlk( key ));
}


/*
Take a secret key possibly shared by a number of servers and use it to
generate a random number stream that is not shared by any of the other servers.
It does this by using the host-dependent information and time (provided by the
caller) to the nearest second.  The resulting stream seed is not useful
information for cracking the secret key. Moreover, this routine keeps no copy
of the secret key.

This routine is used for example, by the kerberos server(s) with the
key in question being the kerberos master key.
*/

void csfDESRANinit (
	const csfDES_kBlk key,
	csfDESRAN_kSeed *p_seed,
	const csfDES_kBlk hseed,
	const csf_ui32 tseed[2]
){
	csfDES_kBlk seed;	/* This must be 64 bits exactly */
	csfDES_kBlk new_key;

	/*
	 * Use a host seed in generating the seed to ensure
	 * that different hosts have different streams:
	 */
	csfCmemzero( seed, sizeof(seed) );
	csfCmemcpy( seed, hseed, sizeof(hseed) );

	/*
	 * Generate a tempory value that depends on the key and host seed
	 * such that it gives no useful information about the key:
	 */
	csfDESRANset( key, p_seed );
	csfDESRANsetSeq( seed, p_seed );
	csfDESRANnewKey( new_key, p_seed );

	/* Use it to select a random stream.
	 */
	csfDESRANset( new_key, p_seed );

	/* Use a time stamp to ensure that a server started later does not reuse
	 * an old stream.
	 */
	csfDESRANsetSeq( (unsigned char *)&tseed, p_seed );

	/* Use the time stamp finally to select the final seed using the
	 * current random number stream:
	 */
	csfDESRANnewKey( new_key, p_seed );
	csfDESRANset( new_key, p_seed );
}


/*
Select a random number stream.  The stream that results is totally
determined by the passed in key. (I.e., calling this routine again with the
same key allows repeating a sequence of random numbers)

Requires: key is a valid des key.  I.e., has correct parity and is not a
weak des key.
*/

void csfDESRANset (
	const csfDES_kBlk key,
	csfDESRAN_kSeed *p_seed
){
	int i;

	/* select the new stream: (note errors are not possible here...) */
	csfDESmake_kSched( key, &p_seed->random_sequence_key );

	/* "seek" to the start of the stream: */
	for (i = 0; i < 8; i++) {
		p_seed->sequence_number[i] = 0;
	}

}


/*
Set the sequence number of the current random number stream.  This routine
may be used to "seek" within the current random number stream.

Note that csfDESRANset resets the sequence number to 0.
*/

void csfDESRANsetSeq (
	const csfDES_kBlk new_sequence_number,
	csfDESRAN_kSeed *p_seed
){
	csfCmemcpy( p_seed->sequence_number, new_sequence_number,
		sizeof(p_seed->sequence_number) );
}


/*
Return the next random number from the current random number stream.
The returned number is 64 bits long.

Requires: csfDESRANset must have been called at least once
before this routine is called.
*/

void csfDESRANgenBlk (
	const csfDES_kBlk block,
	csfDESRAN_kSeed *p_seed
){
	int i;

	/*
	 * Encrypt the sequence number to get the new random block:
	 */
	csfDESCryptECB(
		(csf_ui32 *) p_seed->sequence_number,
		(csf_ui32 *) block,
		&p_seed->random_sequence_key,
		TRUE /*encrypt*/
	);

	/*
	 * Increment the sequence number as an 8 byte unsigned number with wrap:
	 * (using LSB here)
	 */
	for (i = 0; i < 8; i++) {
		p_seed->sequence_number[i] = (p_seed->sequence_number[i] + 1) & 0xff;
		if (p_seed->sequence_number[i]) {
			break;
		}
	}

}
