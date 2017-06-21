/* $Id: deskey.c,v 3.2 1995/11/17 17:46:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/deskey.c,v $
 *------------------------------------------------------------------
 * deskey.c - DES Engine
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: deskey.c,v $
 * Revision 3.2  1995/11/17  17:46:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original key_parity.c 3.1 12/29/93*/
/*.original key_sched.c 3.1 12/29/93*/
/*.original weak_key.c 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1985, 1986, 1987, 1988, 1989, 1990 by the
 * Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * These routines check and fix parity of encryption keys for the DES
 * algorithm.
 *
 */

#include <csf/header.h>
#include "des0.h"

/*
 * des_fixup_key_parity: Forces odd parity per byte; parity is bits
 *                       8,16,...64 in des order, implies 0, 8, 16, ...
 *                       vax order.
 */

#include "x_odd.h"			/* Generated odd_parity table */

void csfDESfixParity_kBlk (
	csfDES_kBlk key
){
	csf_size_t i;

	for (i = 0; i < sizeof(csfDES_kBlk); i++) {
		key[i] = odd_parity[ key[i] ];
	}

}


/*
 * des_check_key_parity: returns true iff key has the correct des parity.
 *                       See des_fix_key_parity for the definition of
 *                       correct des parity.
 */

csf_boolean csfDESisGoodParity_kBlk (
	const csfDES_kBlk key
){
	csf_size_t i;

	for (i = 0; i < sizeof(csfDES_kBlk); i++) {
		if (key[i] != odd_parity[ key[i] ]) {
			return FALSE;
		}
	}

	return TRUE;
}


/*
 * This routine computes the DES key schedule given a key.  The
 * permutations and shifts have been done at compile time, resulting
 * in a direct one-step mapping from the input key to the key
 * schedule.
 *
 * Also checks parity and weak keys.
 *
 * Watch out for the subscripts -- most effectively start at 1 instead
 * of at zero.  Maybe some bugs in that area.
 *
 * DON'T change the data types for arrays and such, or it will either
 * break or run slower.  This was optimized for Uvax2.
 *
 * In case the user wants to cache the computed key schedule, it is
 * passed as an arg.  Also implies that caller has explicit control
 * over zeroing both the key schedule and the key.
 *
 * All registers labeled imply Vax using the Ultrix or 4.2bsd compiler.
 *
 * Originally written 6/85 by Steve Miller, MIT Project Athena.
 */

#include "x_kp.h"

typedef char key[64];

static void make_key_sched ( const key, csfDES_kSched * );

int csfDESmake_kSched (
	const csfDES_kBlk k,
	csfDES_kSched *schedule
){
	/* better pass 8 bytes, length not checked here */
	int i, j;
	unsigned int temp;
	char *p_char;
	key k_char;

	i = 8;
	p_char = k_char;

	_csfDBGprint(( "\n\ninput key, left to right = " ));

	if (!csfDESisGoodParity_kBlk( k )) {	/* bad parity --> return -1 */
		return -1;
	}

	do {

		/* get next input key byte */
		_csfDBGprint(( "%02x ", *k & 0xff ));

		temp = (unsigned int) ((unsigned char) *k++);
		j = 8;

		do {
			*p_char++ = (int) temp & 01;
			temp = temp >> 1;
		} while (--j > 0);

	} while (--i > 0);

#ifdef DEBUG
		p_char = k_char;
		_csfDBGprint(( stderr,"\nKey bits, from zero to 63" ));
		for (i = 0; i <= 7; i++) {
			_csfDBGprint(( stderr,"\n\t" ));
			for (j = 0; j <= 7; j++) {
				_csfDBGprint(( stderr, "%d ", *p_char++ ));
			}
		}
	}
#endif

	/* check against weak keys */
	k -= sizeof(csfDES_kBlk);

	if (csfDESisWeak_kBlk( k )) {
		return -2;
	}

	make_key_sched( k_char, schedule );

	/* if key was good, return 0 */
	return 0;
}

static void make_key_sched (
	const key Key,
	csfDES_kSched *Schedule
){
	/*
	 * The key has been converted to an array to make this run faster;
	 * on a microvax 2, this routine takes about 3.5ms.  The code and
	 * size of the arrays has been played with to get it as fast as
	 * possible.
	 */
	register int iter = AUTH_DES_ITER;
	register csf_ui32 *k;
	register int *kp;
	register csf_ui32 temp;

	kp = (int *) key_perm;
	k  = (csf_ui32 *) Schedule;

	do {
		/*
		 * create the Key schedule
		 *
		 * put into lsb first order (lsb is bit 0)
		 */

		temp = 0;
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (0));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (1));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (2));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (3));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (4));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (5));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (6));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (7));

		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (8));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (9));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (10));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (11));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (12));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (13));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (14));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (15));

		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (16));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (17));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (18));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (19));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (20));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (21));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (22));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (23));

		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (24));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (25));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (26));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (27));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (28));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (29));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (30));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (31));

		*k++ = temp;
		temp = 0;

		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (0));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (1));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (2));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (3));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (4));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (5));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (6));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (7));

		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (8));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (9));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (10));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (11));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (12));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (13));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (14));
		if ((unsigned) Key[(int) *kp++]) temp |= (((csf_ui32)1)<< (15));

		*k++ = temp;

	} while (--iter > 0);

#ifdef DEBUG
	{
		char *n;
		int q;
		_csfDBGprint(( "\nKey Schedule, left to right" ));
		for (i = 0; i < AUTH_DES_ITER; i++) {
			n = (char *) &Schedule[i];
			_csfDBGprint(( "\n" ));
			for (q = 0; q <= 7; q++) {
				_csfDBGprint(( "%02x ", *n++ & 0xff ));
			}
		}
		_csfDBGprint(( "\n" ));
	}
#endif

}


/*
 * The following are the weak DES keys:
 */

static csfDES_kBlk weak[16] = {
	/* weak keys */
	{0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},
	{0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe},
	{0x1f,0x1f,0x1f,0x1f,0x0e,0x0e,0x0e,0x0e},
	{0xe0,0xe0,0xe0,0xe0,0xf1,0xf1,0xf1,0xf1},

	/* semi-weak */
	{0x01,0xfe,0x01,0xfe,0x01,0xfe,0x01,0xfe},
	{0xfe,0x01,0xfe,0x01,0xfe,0x01,0xfe,0x01},

	{0x1f,0xe0,0x1f,0xe0,0x0e,0xf1,0x0e,0xf1},
	{0xe0,0x1f,0xe0,0x1f,0xf1,0x0e,0xf1,0x0e},

	{0x01,0xe0,0x01,0xe0,0x01,0xf1,0x01,0xf1},
	{0xe0,0x01,0xe0,0x01,0xf1,0x01,0xf1,0x01},

	{0x1f,0xfe,0x1f,0xfe,0x0e,0xfe,0x0e,0xfe},
	{0xfe,0x1f,0xfe,0x1f,0xfe,0x0e,0xfe,0x0e},

	{0x01,0x1f,0x01,0x1f,0x01,0x0e,0x01,0x0e},
	{0x1f,0x01,0x1f,0x01,0x0e,0x01,0x0e,0x01},

	{0xe0,0xfe,0xe0,0xfe,0xf1,0xfe,0xf1,0xfe},
	{0xfe,0xe0,0xfe,0xe0,0xfe,0xf1,0xfe,0xf1}
};

/*
 * csfDESisWeak_kBlk: returns true iff key is a [semi-]weak des key.
 *
 * Requires: key has correct odd parity.
 */

csf_boolean csfDESisWeak_kBlk (
	const csfDES_kBlk key
){
	csfDES_kBlk *weak_p = weak;
	csf_size_t i;

	for (i = 0; i < (sizeof(weak)/sizeof(csfDES_kBlk)); i++) {
		if (!csfCmemcmp( weak_p++, key, sizeof(csfDES_kBlk) )) {
			return TRUE;
		}
	}

	return FALSE;
}
