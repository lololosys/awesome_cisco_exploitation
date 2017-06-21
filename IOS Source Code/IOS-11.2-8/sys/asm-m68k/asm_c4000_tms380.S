/* $Id: asm_c4000_tms380.S,v 3.2 1995/11/17 08:39:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c4000_tms380.S,v $
 *------------------------------------------------------------------
 * Assembly wrappers for TMS380 NIM support
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c4000_tms380.S,v $
| Revision 3.2  1995/11/17  08:39:52  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:39  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:54  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c4000.h"

/*
 * TMS380_SIFSTS_BITS:
 */

/* TMS380_SIFSTS_BITS: */
#define SLOT1a_SIFSTS ADRSPC_SLOT_1 + 0x0406
#define SLOT2a_SIFSTS ADRSPC_SLOT_2 + 0x0406
#define SLOT3a_SIFSTS ADRSPC_SLOT_3 + 0x0406

/* TMS380_SIFSTS_BITS: 2nd i/f on nim */
#define SLOT1b_SIFSTS ADRSPC_SLOT_1 + 0x10406
#define SLOT2b_SIFSTS ADRSPC_SLOT_2 + 0x10406
#define SLOT3b_SIFSTS ADRSPC_SLOT_3 + 0x10406

#define TMS380_SYSINT_BIT 7

 |
 |
 |	_trim_ichain_jump_exit
 |
trim_ichain_jump_exit:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception


 |
 |
 |	_trim_slot1a Low Slot IRQ
 |
 | void trim_slot1a (void)
 | void trim_slot1a_jump_entry (void)
 |
	.globl _trim_slot1a
	.globl _trim_slot1a_jump_entry
_trim_slot1a:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_trim_slot1a_jump_entry:
	movw	SLOT1a_SIFSTS, d3	| Get SIFSTS value
	btst	#TMS380_SYSINT_BIT, d3	| Check SYSTEM_INTERRUPT bit
	jeq	trim_ichain_jump_exit	

        movl	d3, sp@-		| Push value of SIFSTS
        movl	#SLOT1a_SIFSTS, sp@-	| Push address of SIFSTS
	movl	HWIDB_SLOT1A,sp@- 	| Push IDB argument for C routines

	jsr	_tms380_sif_handler	| Call interrupt routine

	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT1A, a0
	jmp	a0@

 |
 |
 |	_trim_slot1b Low Slot IRQ
 |
 | void trim_slot1b (void)
 | void trim_slot1b_jump_entry (void)
 |
	.globl _trim_slot1b
	.globl _trim_slot1b_jump_entry
_trim_slot1b:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_trim_slot1b_jump_entry:
	movw	SLOT1b_SIFSTS, d3	| Get SIFSTS value
	btst	#TMS380_SYSINT_BIT, d3	| Check SYSTEM_INTERRUPT bit
	jeq	trim_ichain_jump_exit	

        movl	d3, sp@-		| Push value of SIFSTS
        movl	#SLOT1b_SIFSTS, sp@-	| Push address of SIFSTS
	movl	HWIDB_SLOT1B,sp@- 	| Push IDB argument for C routines

	jsr	_tms380_sif_handler	| Call interrupt routine

	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT1B, a0
	jmp	a0@

 |
 |	_trim_slot2a Low Slot IRQ
 |
 | void trim_slot2a (void)
 | void trim_slot2a_jump_entry (void)
 |
	.globl _trim_slot2a
	.globl _trim_slot2a_jump_entry
_trim_slot2a:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_trim_slot2a_jump_entry:
	movw	SLOT2a_SIFSTS, d3	| Get SIFSTS value
	btst	#TMS380_SYSINT_BIT, d3	| Check SYSTEM_INTERRUPT bit
	jeq	trim_ichain_jump_exit	

        movl	d3, sp@-		| Push value of SIFSTS
        movl	#SLOT2a_SIFSTS, sp@-	| Push address of SIFSTS
	movl	HWIDB_SLOT2A,sp@- 	| Push IDB argument for C routines

	jsr	_tms380_sif_handler	| Call interrupt routine

	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT2A, a0
	jmp	a0@

 |
 |	_trim_slot2b Low Slot IRQ
 |
 | void trim_slot2b (void)
 | void trim_slot2b_jump_entry (void)
 |
	.globl _trim_slot2b
	.globl _trim_slot2b_jump_entry
_trim_slot2b:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_trim_slot2b_jump_entry:
	movw	SLOT2b_SIFSTS, d3	| Get SIFSTS value
	btst	#TMS380_SYSINT_BIT, d3	| Check SYSTEM_INTERRUPT bit
	jeq	trim_ichain_jump_exit	

        movl	d3, sp@-		| Push value of SIFSTS
        movl	#SLOT2b_SIFSTS, sp@-	| Push address of SIFSTS
	movl	HWIDB_SLOT2B,sp@- 	| Push IDB argument for C routines

	jsr	_tms380_sif_handler	| Call interrupt routine

	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT2B, a0
	jmp	a0@


 |
 |	_trim_slot3a Low Slot IRQ
 |
 | void trim_slot3a (void)
 | void trim_slot3a_jump_entry (void)
 |
	.globl _trim_slot3a
	.globl _trim_slot3a_jump_entry
_trim_slot3a:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_trim_slot3a_jump_entry:
	movw	SLOT3a_SIFSTS, d3	| Get SIFSTS value
	btst	#TMS380_SYSINT_BIT, d3	| Check SYSTEM_INTERRUPT bit
	jeq	trim_ichain_jump_exit	

        movl	d3, sp@-		| Push value of SIFSTS
        movl	#SLOT3a_SIFSTS, sp@-	| Push address of SIFSTS
	movl	HWIDB_SLOT3A,sp@- 	| Push IDB argument for C routines

	jsr	_tms380_sif_handler	| Call interrupt routine

	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT3A, a0
	jmp	a0@

 |
 |	_trim_slot3b Low Slot IRQ
 |
 | void trim_slot3b (void)
 | void trim_slot3b_jump_entry (void)
 |
	.globl _trim_slot3b
	.globl _trim_slot3b_jump_entry
_trim_slot3b:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_trim_slot3b_jump_entry:
	movw	SLOT3b_SIFSTS, d3	| Get SIFSTS value
	btst	#TMS380_SYSINT_BIT, d3	| Check SYSTEM_INTERRUPT bit
	jeq	trim_ichain_jump_exit	

        movl	d3, sp@-		| Push value of SIFSTS
        movl	#SLOT3b_SIFSTS, sp@-	| Push address of SIFSTS
	movl	HWIDB_SLOT3B,sp@- 	| Push IDB argument for C routines

	jsr	_tms380_sif_handler	| Call interrupt routine

	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT3B, a0
	jmp	a0@
