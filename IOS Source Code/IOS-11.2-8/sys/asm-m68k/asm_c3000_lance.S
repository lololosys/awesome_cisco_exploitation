/* $Id: asm_c3000_lance.S,v 3.2 1995/11/17 08:39:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c3000_lance.S,v $
 *------------------------------------------------------------------
 * Assembly wrappers for c3000 lance support
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c3000_lance.S,v $
| Revision 3.2  1995/11/17  08:39:33  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:22  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:34  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c3000.h"

/*
 * Lance CSR0_BITS: 
 */
#define LANCE_RINT_BIT	10
#define LANCE_TINT_BIT	9
#define LANCE_ERRORS	0xF900	/* ERR, BABL, CERR, MISS, MERR, and IDON */

#define E0_CSR	ADRSPC_NETWORK_A
#define E0_RAP	ADRSPC_NETWORK_A+2
#define E1_CSR	ADRSPC_NETWORK_B
#define E1_RAP	ADRSPC_NETWORK_B+2

 |
 |	level4_pan_e0 - Level 4 interrupt wrapper for pan ethernet
 |
 | void level4_pan_e0 (void)
 |
	.globl _level4_pan_e0
_level4_pan_e0:
	| ### Got Lance Ethernet 0 interrupt:
	movw	E0_CSR,d3		| Read CSR0
	movw	d3,E0_CSR		| Clear interrupt bits
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#LANCE_RINT_BIT,d3	| RX interrupt?
	jeq	level4_pan_e0_check_TXA	| No, check for TX interrupt
	jsr	_lance_RX_interrupt	| Call interrupt routine
level4_pan_e0_check_TXA:
	btst	#LANCE_TINT_BIT,d3	| TX interrupt?
	jeq	level4_pan_e0_check_EA	| No, check for error interrupt
	jsr	_lance_TX_interrupt	| Call interrupt routine	
level4_pan_e0_check_EA:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#LANCE_ERRORS,d3	| Mask off non-error bits
	jeq	level4_pan_e0_exit	| No errors, exit
	movl	d0,sp@-			| Push CSR0 as long (stack alignment)
	jsr	_lance_E_interrupt	| Call interrupt routine
	addql	#4,sp			| adjust stack
level4_pan_e0_exit:
 	addql	#4,sp			| adjust stack
	rts				| Bye Bye


 |
 |	level4_pan_e1 - Level 4 interrupt wrapper for pan ethernet
 |
 | void level4_pan_e1 (void)
 |
	.globl _level4_pan_e1
_level4_pan_e1:
	| ### Got Lance Ethernet 1 interrupt:
	movw	E1_CSR,d3		| Read CSR0
	movw	d3,E1_CSR		| Clear interrupt bits
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#LANCE_RINT_BIT,d3	| RX interrupt?
	jeq	level4_pan_e1_check_TXA	| No, check for TX interrupt
	jsr	_lance_RX_interrupt	| Call interrupt routine
level4_pan_e1_check_TXA:
	btst	#LANCE_TINT_BIT,d3	| TX interrupt?
	jeq	level4_pan_e1_check_EA	| No, check for error interrupt
	jsr	_lance_TX_interrupt	| Call interrupt routine	
level4_pan_e1_check_EA:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#LANCE_ERRORS,d3	| Mask off non-error bits
	jeq	level4_pan_e1_exit	| No errors, exit
	movl	d0,sp@-			| Push CSR0 as long (stack alignment)
	jsr	_lance_E_interrupt	| Call interrupt routine
	addql	#4,sp			| adjust stack
level4_pan_e1_exit:
 	addql	#4,sp			| adjust stack
	rts				| Bye Bye

