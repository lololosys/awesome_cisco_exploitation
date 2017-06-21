/* $Id: desecb.c,v 3.2 1995/11/17 17:46:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/desecb.c,v $
 *------------------------------------------------------------------
 * desecb.c - DES Engine
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: desecb.c,v $
 * Revision 3.2  1995/11/17  17:46:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original des.c 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright 1985, 1986, 1987, 1988, 1990 by the
 * Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Under U.S. law, this software may not be exported outside the US
 * without license from the U.S. Commerce department.
 */

/*
 * These routines perform encryption and decryption using the DES
 * private key algorithm, or else a subset of it-- fewer inner loops.
 * (AUTH_DES_ITER defaults to 16, may be less.)
 *
 * The key schedule is passed as an arg, as well as the cleartext or
 * ciphertext.
 *
 * All registers labeled imply Vax using the Ultrix or 4.2bsd
 * compiler.
 *
 *      NOTE:  bit and byte numbering:
 *                      DES algorithm is defined in terms of bits of L
 *                      followed by bits of R.
 *              bit 0  ==> lsb of L
 *              bit 63 ==> msb of R
 *
 * Always work in register pairs, FROM L1,R1 TO L2,R2 to make
 * bookkeeping easier.
 *
 * originally written by Steve Miller, MIT Project Athena
 */


#include <csf/header.h>
#include "des0.h"
#include "x_s.h"
#include "x_p.h"

#if CSF_CFG0_M_WORDSIZE == 32
/* these are for 32 bit machines */

typedef struct {
	unsigned b0:6;
	unsigned b1:6;
	unsigned b2:6;
	unsigned b3:6;
	unsigned b4:6;
	unsigned b5:2;
} sbox_in_a;

typedef struct {
	unsigned b5:4;
	unsigned b6:6;
	unsigned b7:6;
} sbox_in_b;

typedef struct {
	unsigned b0:4;
	unsigned b1:4;
	unsigned b2:4;
	unsigned b3:4;
	unsigned b4:4;
	unsigned b5:4;
	unsigned b6:4;
	unsigned b7:4;
} sbox_out;

#endif   /* WORDSIZE == 32 */


#if CSF_CFG0_M_WORDSIZE == 16
/* for sixteen bit machines */

typedef struct {
	unsigned b0:6;
	unsigned b1:6;
	unsigned b2:4;
}       sbox_in_16_a;

typedef struct {
	unsigned b2:2;
	unsigned b3:6;
	unsigned b4:6;
	unsigned b5:2;
}       sbox_in_16_b;

typedef struct {
	unsigned b5:4;
	unsigned b6:6;
	unsigned b7:6;
}       sbox_in_16_c;

typedef struct {
	unsigned b0:4;
	unsigned b1:4;
	unsigned b2:4;
	unsigned b3:4;
	unsigned b4:4;
	unsigned b5:4;
	unsigned b6:4;
	unsigned b7:4;
}       sbox_out;

#endif  /* WORDSIZE == 16 */


#ifdef DEBUG
#define DBG_PRINT( s ) \
   _csfDBGprint( s, i, L1&0xffff, (L1>>16)&0xffff, R1&0xffff, (R1>>16)&0xffff )
#else
#define DBG_PRINT( s )
#endif

void csfDESCryptECB (
	const csf_ui32 *clear,			/* Input (clear-text). */
	csf_ui32 *cipher,				/* Output (cipher-text). */
	const csfDES_kSched *schedule,	/* Precomputed key schedule. */
	csf_boolean encrypt			/* TRUE encrypt; else decrypt */
){

	/* better pass 8 bytes, length not checked here */
	register csf_ui32 R1, L1;
	register csf_ui32 R2, L2;
	long i;
#if CSF_CFG0_M_WORDSIZE == 16
	sbox_in_16_a S_in_16_a;
	sbox_in_16_b S_in_16_b;
	sbox_in_16_c S_in_16_c;
	csf_ui16 *S_in_a_16_p = (csf_ui16 *) &S_in_16_a;
	csf_ui16 *S_in_b_16_p = (csf_ui16 *) &S_in_16_b;
	csf_ui16 *S_in_c_16_p = (csf_ui16 *) &S_in_16_c;
#endif
	csf_ui32 P_temp;
	register csf_o *P_temp_p = (csf_o *) & P_temp;
#if CSF_CFG0_M_WORDSIZE == 16
	sbox_out S_out;
	csf_ui32 *S_out_p = (csf_ui32 *) &S_out;
#endif
	csf_ui32 R_save, L_save;
#ifdef DEBUG
	csf_ui32 dbg_tmp[2];
#endif

	/*
	 * Use L1,R1 and L2,R2 as two sets of "64-bit" registers always
	 * work from L1,R1 input to L2,R2 output; initialize the cleartext
	 * into registers.
	 */
	/*
	 * If the alignment is wrong, the programmer really screwed up --
	 * we aren't even getting the right data type.  His problem.  Keep
	 * this code for debugging.
	 */
	/* Make sure schedule is ok */
	_csfDBGassert( ((long)schedule) & (CSF_CFG0_M_RALIGN-1) );
	csfCmemcpy( &L_save, clear++, sizeof(L_save) );
	csfCmemcpy( &R_save, clear, sizeof(R_save) );
	L1 = L_save;
	R1 = R_save;

#ifdef DEBUG
	if (csfDES_debug & 2) {
		printf( "All values printed from low byte (bit 0)" );
		printf( " --> high byte (bit 63)\n" );
		i = 0;
		dbg_tmp[0] = L1;
		dbg_tmp[1] = R1;
		printf( "iter = %2d	before IP\n\t\tL1 R1 = ", i );
		csfDES_kBlk_print_file( dbg_tmp, stdout );
	}

	DBG_PRINT( "before IP" );
#endif

	/*   IP_start:*/

	/* all the Initial Permutation code is in the include file */
#include "x_ipc.h"
	/* reset input to L1,R1 */
	L1 = L2;
	R1 = R2;

	/* iterate through the inner loop */
	for (i = 0; i <= (AUTH_DES_ITER-1); i++) {

#ifdef DEBUG
		if (csfDES_debug & 2) {
			dbg_tmp[0] = L1;
			dbg_tmp[1] = R1;
			printf( "iter = %2d	start loop\n\t\tL1 R1 = ", i );
			csfDES_kBlk_print_file( dbg_tmp, stdout );
			DBG_PRINT( "start loop" );
		}
#endif

		R_save = R1;
		L_save = L1;

		/*   E_start:*/
		/* apply the E permutation from R1 to L2, R2 */
		/* Bill's fast E */
		L2 = (R1 << 1);
		if (R1 & (((csf_ui32)1)<<31))
			L2 |= ((csf_ui32)1)<<0;
		L2 &= 077;
		L2 |= (R1 <<3) & 07700;
		L2 |= (R1 <<5) & 0770000;
		L2 |= (R1 <<7) & 077000000;
		L2 |= (R1 <<9) & 07700000000;
		L2 |= (R1 <<11) & 030000000000;

		/* now from right to right */

		R2 = ((R1 >> 17) & 0176000);
		if (R1 & (((csf_ui32)1)<<0)) R2 |= ((csf_ui32)1)<<15;

		R2 |= ((R1 >> 21) & 017);
		R2 |= ((R1 >> 19) & 01760);

		/* reset input to L1,R1 */
		L1 = L2;
		R1 = R2;

#ifdef DEBUG
		if (csfDES_debug & 2) {
			dbg_tmp[0] = L1;
			dbg_tmp[1] = R1;
			DBG_PRINT("after e");
			printf( "iter = %2d	after e\n\t\tL1 R1 = ", i );
			csfDES_kBlk_print_file( dbg_tmp, stdout );
		}
#endif

		/*   XOR_start:*/
		/*
		 * XOR with the key schedule, "schedule"
		 *
		 * If this is an encryption operation, use schedule[i],
		 * otherwise use schedule [AUTH_DES_ITER-i-1]
		 *
		 * First XOR left half.
		 */
		if (encrypt) {
			L1 ^= *(((csf_ui32 *) &(schedule->kb[i])) + 0);
			/* now right half */
			R1 ^= *(((csf_ui32 *) &(schedule->kb[i])) + 1);
		}
		else {
			L1 ^= *(((csf_ui32 *) &(schedule->kb[AUTH_DES_ITER-i-1])) +0);
			/* now right half */
			R1 ^= *(((csf_ui32 *) &(schedule->kb[AUTH_DES_ITER-i-1])) +1);
		}

		/* dont have to reset input to L1, R1 */

#ifdef DEBUG
		if (csfDES_debug & 2) {
			dbg_tmp[0] = L1;
			dbg_tmp[1] = R1;
			DBG_PRINT( "after xor" );
			printf( "iter = %2d	after xor\n\t\tL1 R1 =", i );
			csfDES_kBlk_print_file( dbg_tmp, stdout );
		}
#endif

		/*   S_start:*/
		/* apply the S selection from L1, R1 to R2 */

#ifdef notdef
#include "x_sc.h"
#endif

		/* S operations , cant use registers for bit field stuff */
		/* from S_in to S_out */

#if CSF_CFG0_M_WORDSIZE == 16
		*S_in_a_16_p = L1&0xffff;
		*S_in_b_16_p = (L1>>16)&0xffff;
		*S_in_c_16_p = R1&0xffff;
		(*(csf_ui32 *) &S_out) =
			(unsigned) S_adj[0][S_in_16_a.b0];
		S_out.b1 = (unsigned) S_adj[1][S_in_16_a.b1];
		/* b2 spans two words */
		S_out.b2 = (unsigned)
			S_adj[2][(unsigned) S_in_16_a.b2
			+ (((unsigned) S_in_16_b.b2) << 4)];
		S_out.b3 = (unsigned) S_adj[3][S_in_16_b.b3];
		S_out.b4 = (unsigned) S_adj[4][S_in_16_b.b4];
		/* b5 spans both parts */
		S_out.b5 = (unsigned)
			S_adj[5][(unsigned) S_in_16_b.b5
			+ (((unsigned) S_in_16_c.b5) << 2)];
		S_out.b6 = (unsigned) S_adj[6][S_in_16_c.b6];
		S_out.b7 = (unsigned) S_adj[7][S_in_16_c.b7];
		R1 = *S_out_p;
#else
		/* is a 32 bit sys */
		R2 =  (unsigned) S_adj[0][L1 & 077];
		L2 = (unsigned) S_adj[1][(L1 >> 6) & 077];
		R2 |= (L2 <<4 );
		L2 = (unsigned) S_adj[2][(L1 >> 12) & 077];
		R2 |= (L2 <<8);
		L2 = (unsigned) S_adj[3][(L1 >> 18) & 077];
		R2 |= (L2 <<12);
		L2 = (unsigned) S_adj[4][(L1 >> 24) & 077];
		R2 |= (L2 <<16);
		/* b5 spans both parts */
		L2 = (unsigned)
			S_adj[5][(unsigned) ((L1 >>30) & 03) + ((R1 & 017) << 2)];
		R2 |= (L2 << 20);
		L2 = (unsigned) S_adj[6][(R1 >> 4) & 077];
		R2 |= (L2 <<24);
		L2 = (unsigned) S_adj[7][(R1 >> 10) & 077];
		R1 = R2 | (L2 <<28);
		/* reset input to L1, R1 */
#endif

#ifdef DEBUG
		if (csfDES_debug & 2) {
			dbg_tmp[0] = L1;
			dbg_tmp[1] = R1;
			DBG_PRINT( "after s" );
			printf( "iter = %2d	after s\n\t\tL1 R1 = ", i );
			csfDES_kBlk_print_file( dbg_tmp, stdout );
		}
#endif

		/*   P_start:*/
		/* and then the p permutation from R1 into R2 */
#include "x_pc.h"
		/* reset the input to L1, R1 */
		R1 = R2;

#ifdef DEBUG
		if (csfDES_debug & 2) {
			dbg_tmp[0] = L1;
			dbg_tmp[1] = R1;
			DBG_PRINT("after p");
			printf( "iter = %2d	after p\n\t\tL1 R1 = ", i );
			csfDES_kBlk_print_file( dbg_tmp, stdout );
		}
#endif

		/* R1 is the output value from the f() */
		/* move R[iter] to L[iter+1] */
		/*   XOR_2_start:*/
		L1 = R_save;
		/* xor with left */
		R1 = L_save ^ R1;
		/* reset the input */
	}

	/* flip left and right before final permutation */
	L2 = R1;                    /* flip */
	R2 = L1;
	/* reset the input */
	L1 = L2;
	R1 = R2;

#ifdef DEBUG
	if (csfDES_debug & 2) {
		dbg_tmp[0] = L1;
		dbg_tmp[1] = R1;
		DBG_PRINT( "before FP" );
		printf( "iter = %2d	before FP\n\t\tL1 R1 = ", i );
		csfDES_kBlk_print_file( dbg_tmp, stdout );
	}

#endif

	/*FP_start:*/
	/* do the final permutation from L1R1 to L2R2 */
	/* all the fp code is in the include file */
#include "x_fpc.h"

	/* copy the output to the ciphertext string;
	 * can be same as cleartext
	 */

	L_save = L2;
	R_save = R2;
	csfCmemcpy( cipher++, &L_save, sizeof(L_save) );
	csfCmemcpy( cipher, &R_save, sizeof(R_save) );

#ifdef DEBUG
	if (csfDES_debug & 2) {
		L1 = L2;
		R1 = R2;
		dbg_tmp[0] = L1;
		dbg_tmp[1] = R1;
		DBG_PRINT( "done" );
		printf( "iter = %2d	done\n\t\tL1 R1 = ", i );
		csfDES_kBlk_print_file( dbg_tmp, stdout );
	}
#endif

}

