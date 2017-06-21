/* $Id: asm_c4000_mk5025.S,v 3.2 1995/11/17 08:39:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c4000_mk5025.S,v $
 *------------------------------------------------------------------
 * Assembly wrappers for Dual Serial MK5025 NIM
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c4000_mk5025.S,v $
| Revision 3.2  1995/11/17  08:39:50  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:37  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:52  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c4000.h"

/*
 * MK5025_CSR0_BITS:
 */

#define MK5025_RINT_BIT	1
#define MK5025_TINT_BIT	2
#define MK5025_ERRORS	0xF8	/* MERR, MISS, ROR, TUR, and PINT */


	| ### Start of dsim interrupt wrappers
 | 
 | void dsim_slot0 (void)
 |
	.globl _dsim_slot0
_dsim_slot0:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

	movw	SLOT0_DSIM_VECTOR,d2
	notw	d2
	btst	#DSIM_A_INT,d2
	jeq	dsim_slot0_check_b

	| ### Got MK5025 Slot 0, channel A interrupt:
	
        movw    SLOT0_DSIM_A_CSR,d3    | Read CSR0
        movw    d3,SLOT0_DSIM_A_CSR    | Clear interrupt bits
        movl    HWIDB_SLOT1A,sp@-        | Push IDB argument for C routines

        btst    #MK5025_RINT_BIT,d3     | RX interrupt?
        jeq     dsim_slot0_a_check_TXB  | no, check for TX interrupt
        jsr     _mk5025_RX_interrupt    | call interrupt routine
dsim_slot0_a_check_TXB:
        btst    #MK5025_TINT_BIT,d3     | TX interrupt?
        jeq     dsim_slot0_a_check_EB   | No, check for error interrupt
        jsr     _mk5025_TX_interrupt    | Call interrupt routine
dsim_slot0_a_check_EB:
        movw    d3,d0                   | Copy CSR0 into d0
        andw    #MK5025_ERRORS,d3       | Mask off non-error bits
        jeq     dsim_slot0_check_b      | No errors, clean up and return
        movl    d0,sp@-                 | Push CSR0 for Error interrupt
        jsr     _mk5025_E_interrupt     | Call interrupt routine


dsim_slot0_check_b:
	btst	#DSIM_B_INT,d2
	jeq	dsim_slot0_check_ma

	| ### Got MK5025 Slot 0, channel B interrupt:

        movw    SLOT0_DSIM_B_CSR,d3    | Read CSR0
        movw    d3,SLOT0_DSIM_B_CSR    | Clear interrupt bits
        movl    HWIDB_SLOT1B,sp@-      | Push IDB argument for C routines

        btst    #MK5025_RINT_BIT,d3     | RX interrupt?
        jeq     dsim_slot0_b_check_TXB  | no, check for TX interrupt
        jsr     _mk5025_RX_interrupt    | call interrupt routine
dsim_slot0_b_check_TXB:
        btst    #MK5025_TINT_BIT,d3     | TX interrupt?
        jeq     dsim_slot0_b_check_EB   | No, check for error interrupt
        jsr     _mk5025_TX_interrupt    | Call interrupt routine
dsim_slot0_b_check_EB:
        movw    d3,d0                   | Copy CSR0 into d0
        andw    #MK5025_ERRORS,d3       | Mask off non-error bits
        jeq     dsim_slot0_check_ma     | No errors, clean up and return
        movl    d0,sp@-                 | Push CSR0 for Error interrupt
        jsr     _mk5025_E_interrupt     | Call interrupt routine

dsim_slot0_check_ma:
	btst	#DSIM_MA_INT,d2
	jeq	dsim_slot0_check_mb

	| ### Got Slot 0, channel A Modem state change
        movl    HWIDB_SLOT1A,sp@-       | Push IDB argument for C routines
	jsr	_mk5025_xx_delta_interrupt

dsim_slot0_check_mb:
	btst	#DSIM_MB_INT,d2
	jeq	dsim_slot0_no_int

	| ### Got Slot 0, channel B Modem state change
        movl    HWIDB_SLOT1B,sp@-        | Push IDB argument for C routines
	jsr	_mk5025_xx_delta_interrupt

dsim_slot0_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception

 | 
 | void dsim_slot1 (void)
 |

	.globl _dsim_slot1
_dsim_slot1:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

	movw	SLOT1_DSIM_VECTOR,d2
	notw	d2
	btst	#DSIM_A_INT,d2
	jeq	dsim_slot1_check_b

	| ### Got MK5025 Slot 1, channel A interrupt:
	
        movw    SLOT1_DSIM_A_CSR,d3    | Read CSR0
        movw    d3,SLOT1_DSIM_A_CSR    | Clear interrupt bits
        movl    HWIDB_SLOT2A,sp@-      | Push IDB argument for C routines

        btst    #MK5025_RINT_BIT,d3     | RX interrupt?
        jeq     dsim_slot1_a_check_TXB  | no, check for TX interrupt
        jsr     _mk5025_RX_interrupt    | call interrupt routine
dsim_slot1_a_check_TXB:
        btst    #MK5025_TINT_BIT,d3     | TX interrupt?
        jeq     dsim_slot1_a_check_EB   | No, check for error interrupt
        jsr     _mk5025_TX_interrupt    | Call interrupt routine
dsim_slot1_a_check_EB:
        movw    d3,d0                   | Copy CSR0 into d0
        andw    #MK5025_ERRORS,d3       | Mask off non-error bits
        jeq     dsim_slot1_check_b      | No errors, clean up and return
        movl    d0,sp@-                 | Push CSR0 for Error interrupt
        jsr     _mk5025_E_interrupt     | Call interrupt routine


dsim_slot1_check_b:
	btst	#DSIM_B_INT,d2
	jeq	dsim_slot1_check_ma

	| ### Got MK5025 Slot 1, channel B interrupt:

        movw    SLOT1_DSIM_B_CSR,d3    | Read CSR0
        movw    d3,SLOT1_DSIM_B_CSR    | Clear interrupt bits
        movl    HWIDB_SLOT2B,sp@-      | Push IDB argument for C routines

        btst    #MK5025_RINT_BIT,d3     | RX interrupt?
        jeq     dsim_slot1_b_check_TXB  | no, check for TX interrupt
        jsr     _mk5025_RX_interrupt    | call interrupt routine
dsim_slot1_b_check_TXB:
        btst    #MK5025_TINT_BIT,d3     | TX interrupt?
        jeq     dsim_slot1_b_check_EB   | No, check for error interrupt
        jsr     _mk5025_TX_interrupt    | Call interrupt routine
dsim_slot1_b_check_EB:
        movw    d3,d0                   | Copy CSR0 into d0
        andw    #MK5025_ERRORS,d3       | Mask off non-error bits
        jeq     dsim_slot1_check_ma     | No errors, clean up and return
        movl    d0,sp@-                 | Push CSR0 for Error interrupt
        jsr     _mk5025_E_interrupt     | Call interrupt routine

dsim_slot1_check_ma:
	btst	#DSIM_MA_INT,d2
	jeq	dsim_slot1_check_mb

	| ### Got Slot 1, channel A Modem state change
        movl    HWIDB_SLOT2A,sp@-       | Push IDB argument for C routines
	jsr	_mk5025_xx_delta_interrupt

dsim_slot1_check_mb:
	btst	#DSIM_MB_INT,d2
	jeq	dsim_slot1_no_int

	| ### Got Slot 1, channel B Modem state change
        movl    HWIDB_SLOT2B,sp@-       | Push IDB argument for C routines
	jsr	_mk5025_xx_delta_interrupt

dsim_slot1_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception

 | 
 | void dsim_slot2 (void)
 |

	.globl _dsim_slot2
_dsim_slot2:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

	movw	SLOT2_DSIM_VECTOR,d2
	notw	d2
	btst	#DSIM_A_INT,d2
	jeq	dsim_slot2_check_b

	| ### Got MK5025 Slot 2, channel A interrupt:
	
        movw    SLOT2_DSIM_A_CSR,d3    | Read CSR0
        movw    d3,SLOT2_DSIM_A_CSR    | Clear interrupt bits
        movl    HWIDB_SLOT3A,sp@-      | Push IDB argument for C routines

        btst    #MK5025_RINT_BIT,d3     | RX interrupt?
        jeq     dsim_slot2_a_check_TXB  | no, check for TX interrupt
        jsr     _mk5025_RX_interrupt    | call interrupt routine
dsim_slot2_a_check_TXB:
        btst    #MK5025_TINT_BIT,d3     | TX interrupt?
        jeq     dsim_slot2_a_check_EB   | No, check for error interrupt
        jsr     _mk5025_TX_interrupt    | Call interrupt routine
dsim_slot2_a_check_EB:
        movw    d3,d0                   | Copy CSR0 into d0
        andw    #MK5025_ERRORS,d3       | Mask off non-error bits
        jeq     dsim_slot2_check_b      | No errors, clean up and return
        movl    d0,sp@-                 | Push CSR0 for Error interrupt
        jsr     _mk5025_E_interrupt     | Call interrupt routine


dsim_slot2_check_b:
	btst	#DSIM_B_INT,d2
	jeq	dsim_slot2_check_ma

	| ### Got MK5025 Slot 2, channel B interrupt:

        movw    SLOT2_DSIM_B_CSR,d3    | Read CSR0
        movw    d3,SLOT2_DSIM_B_CSR    | Clear interrupt bits
        movl    HWIDB_SLOT3B,sp@-      | Push IDB argument for C routines

        btst    #MK5025_RINT_BIT,d3     | RX interrupt?
        jeq     dsim_slot2_b_check_TXB  | no, check for TX interrupt
        jsr     _mk5025_RX_interrupt    | call interrupt routine
dsim_slot2_b_check_TXB:
        btst    #MK5025_TINT_BIT,d3     | TX interrupt?
        jeq     dsim_slot2_b_check_EB   | No, check for error interrupt
        jsr     _mk5025_TX_interrupt    | Call interrupt routine
dsim_slot2_b_check_EB:
        movw    d3,d0                   | Copy CSR0 into d0
        andw    #MK5025_ERRORS,d3       | Mask off non-error bits
        jeq     dsim_slot2_check_ma     | No errors, clean up and return
        movl    d0,sp@-                 | Push CSR0 for Error interrupt
        jsr     _mk5025_E_interrupt     | Call interrupt routine

dsim_slot2_check_ma:
	btst	#DSIM_MA_INT,d2
	jeq	dsim_slot2_check_mb

	| ### Got Slot 2, channel A Modem state change
        movl    HWIDB_SLOT3A,sp@-       | Push IDB argument for C routines
	jsr	_mk5025_xx_delta_interrupt

dsim_slot2_check_mb:
	btst	#DSIM_MB_INT,d2
	jeq	dsim_slot2_no_int

	| ### Got Slot 2, channel B Modem state change
        movl    HWIDB_SLOT3B,sp@-       | Push IDB argument for C routines
	jsr	_mk5025_xx_delta_interrupt

dsim_slot2_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
