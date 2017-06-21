/* $Id: asm_c3000_mk5025.S,v 3.2 1995/11/17 08:39:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c3000_mk5025.S,v $
 *------------------------------------------------------------------
 * Assembly wrappers for c3000 mk5025 support
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c3000_mk5025.S,v $
| Revision 3.2  1995/11/17  08:39:34  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:24  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:36  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c3000.h"

/*
 * MK5025_CSR0_BITS:
 */
#define MK5025_RINT_BIT	1
#define MK5025_TINT_BIT	2
#define MK5025_ERRORS	0xF8	/* MERR, MISS, ROR, TUR, and PINT */

#define S0_CSR	ADRSPC_NETWORK_B
#define S0_RAP	ADRSPC_NETWORK_B+2
#define S1_CSR	ADRSPC_NETWORK_C
#define S1_RAP	ADRSPC_NETWORK_C+2


 |
 |	level4_pan_t0 - Level 4 interrupt wrapper for pan serial
 |
 | void level4_pan_t0 (void)
 |	
	.globl _level4_pan_t0
_level4_pan_t0:
	| ### Got MK5025 Serial 0 interrupt:
	movw	S0_CSR,d3		| Read CSR0
	movw	d3,S0_CSR		| Clear interrupt bits
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#MK5025_RINT_BIT,d3	| RX interrupt?
	jeq	level4_pan_t0_check_TXB	| no, check for TX interrupt
	jsr	_mk5025_RX_interrupt	| call interrupt routine
level4_pan_t0_check_TXB:
	btst	#MK5025_TINT_BIT,d3	| TX interrupt?
	jeq	level4_pan_t0_check_EB	| No, check for error interrupt
	jsr	_mk5025_TX_interrupt	| Call interrupt routine
level4_pan_t0_check_EB:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#MK5025_ERRORS,d3	| Mask off non-error bits
	jeq	level4_pan_t0_exit	| No errors, clean up and return
	movl	d0,sp@-			| Push CSR0 for Error interrupt
	jsr	_mk5025_E_interrupt	| Call interrupt routine
	addql	#4,sp			| adjust stack
level4_pan_t0_exit:
	addql	#4,sp			| adjust stack
	rts				| bye bye


 |
 |	level4_pan_t1 - Level 4 interrupt wrapper for pan serial
 |
 | void level4_pan_t1 (void)
 |	
	.globl _level4_pan_t1
_level4_pan_t1:
	| ### Got MK5025 Serial 1 interrupt:
	movw	S1_CSR,d3		| Read CSR0
	movw	d3,S1_CSR		| Clear interrupt bits
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#MK5025_RINT_BIT,d3	| RX interrupt?
	jeq	level4_pan_t1_check_TXB	| no, check for TX interrupt
	jsr	_mk5025_RX_interrupt	| call interrupt routine
level4_pan_t1_check_TXB:
	btst	#MK5025_TINT_BIT,d3	| TX interrupt?
	jeq	level4_pan_t1_check_EB	| No, check for error interrupt
	jsr	_mk5025_TX_interrupt	| Call interrupt routine
level4_pan_t1_check_EB:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#MK5025_ERRORS,d3	| Mask off non-error bits
	jeq	level4_pan_t1_exit	| No errors, clean up and return
	movl	d0,sp@-			| Push CSR0 for Error interrupt
	jsr	_mk5025_E_interrupt	| Call interrupt routine
	addql	#4,sp			| adjust stack
level4_pan_t1_exit:
	addql	#4,sp			| adjust stack
	rts				| bye bye

