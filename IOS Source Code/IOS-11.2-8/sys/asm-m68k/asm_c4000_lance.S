/* $Id: asm_c4000_lance.S,v 3.2 1995/11/17 08:39:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c4000_lance.S,v $
 *------------------------------------------------------------------
 * Assembly wrappers for Lance-based Ethernet NIMs
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c4000_lance.S,v $
| Revision 3.2  1995/11/17  08:39:47  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:34  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:48  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c4000.h"

/*
 * Lance CSR0_BITS:
 */
#define LANCE_RINT_BIT	10
#define LANCE_TINT_BIT	9
#define LANCE_ERRORS    0xF900  /* ERR, BABL, CERR, MISS, MERR, and IDON */
#define LANCE_MASK      0xFF00


|
|
|	_eim_ichain_jump_exit
|
eim_ichain_jump_exit:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception


 |
 |	_eim_slot0a Low Slot IRQ
 |
 | void eim_slot0a (void)
 | void eim_slot0a_jump_entry (void)
 |
	.globl _eim_slot0a
	.globl _eim_slot0a_jump_entry
_eim_slot0a:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_eim_slot0a_jump_entry:
	| ### Got Lance Ethernet Slot 0 interrupt:
	movw	SLOT0_DEIM_A_CSR,d3	| Read CSR0
	movw	d3,SLOT0_DEIM_A_CSR	| Clear interrupt bits
	andw	#LANCE_MASK,d3
	jeq	eim_ichain_jump_exit

	movl	HWIDB_SLOT1A,sp@- 	| Push IDB argument for C routines

	btst	#LANCE_RINT_BIT,d3	| RX interrupt?
	jeq	eim_slot0a_check_TXA	| No, check for TX interrupt
	jsr	_lance_RX_interrupt	| Call interrupt routine
eim_slot0a_check_TXA:
	btst	#LANCE_TINT_BIT,d3	| TX interrupt?
	jeq	eim_slot0a_check_EA	| No, check for error interrupt
	jsr	_lance_TX_interrupt	| Call interrupt routine
eim_slot0a_check_EA:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#LANCE_ERRORS,d3	| Mask off non-error bits
	jeq	eim_slot0a_jump_chain	| No errors, return
	movl	d0,sp@-			| Push CSR0 as long (stack alignment)
	jsr	_lance_E_interrupt	| Call interrupt routine

eim_slot0a_jump_chain:
	movl	_l3stk,sp
        movb    ADRSPC_SLOT_1, d3       | Do not remove this instruction.
	movl	ICHAIN_JUMP_SLOT1A, a0
	jmp	a0@

|
 |
 |	_eim_slot0b Low Slot IRQ  
 |
 | void eim_slot0b (void)
 | void eim_slot0b_jump_entry (void)
 |
	.globl _eim_slot0b
	.globl _eim_slot0b_jump_entry
_eim_slot0b:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_eim_slot0b_jump_entry:
	| ### Got Lance Ethernet Slot 0 interrupt:
	movw	SLOT0_DEIM_B_CSR,d3	| Read CSR0
	movw	d3,SLOT0_DEIM_B_CSR	| Clear interrupt bits
	andw	#LANCE_MASK,d3
	jeq	eim_ichain_jump_exit

	movl	HWIDB_SLOT1B,sp@- 	| Push IDB argument for C routines

	btst	#LANCE_RINT_BIT,d3	| RX interrupt?
	jeq	eim_slot0b_check_TXA	| No, check for TX interrupt
	jsr	_lance_RX_interrupt	| Call interrupt routine
eim_slot0b_check_TXA:
	btst	#LANCE_TINT_BIT,d3	| TX interrupt?
	jeq	eim_slot0b_check_EA	| No, check for error interrupt
	jsr	_lance_TX_interrupt	| Call interrupt routine
eim_slot0b_check_EA:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#LANCE_ERRORS,d3	| Mask off non-error bits
	jeq	eim_slot0b_jump_chain	| No errors, return
	movl	d0,sp@-			| Push CSR0 as long (stack alignment)
	jsr	_lance_E_interrupt	| Call interrupt routine

eim_slot0b_jump_chain:
	movl	_l3stk,sp
        movb    ADRSPC_SLOT_1, d3       | Do not remove this instruction.
	movl	ICHAIN_JUMP_SLOT1B, a0
	jmp	a0@

|
 |
 |	_eim_slot1a Low Slot IRQ  
 |
 | void eim_slot1a (void)
 | void eim_slot1a_jump_entry (void)
 |
	.globl _eim_slot1a
	.globl _eim_slot1a_jump_entry
_eim_slot1a:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_eim_slot1a_jump_entry:
	| ### Got Lance Ethernet Slot 2 interrupt:
	movw	SLOT1_DEIM_A_CSR,d3	| Read CSR0
	movw	d3,SLOT1_DEIM_A_CSR	| Clear interrupt bits
	andw	#LANCE_MASK,d3
	jeq	eim_ichain_jump_exit

	movl	HWIDB_SLOT2A,sp@- 	| Push IDB argument for C routines

	btst	#LANCE_RINT_BIT,d3	| RX interrupt?
	jeq	eim_slot1a_check_TXA	| No, check for TX interrupt
	jsr	_lance_RX_interrupt	| Call interrupt routine
eim_slot1a_check_TXA:
	btst	#LANCE_TINT_BIT,d3	| TX interrupt?
	jeq	eim_slot1a_check_EA	| No, check for error interrupt
	jsr	_lance_TX_interrupt	| Call interrupt routine
eim_slot1a_check_EA:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#LANCE_ERRORS,d3	| Mask off non-error bits
	jeq	eim_slot1a_jump_chain	| No errors, return
	movl	d0,sp@-			| Push CSR0 as long (stack alignment)
	jsr	_lance_E_interrupt	| Call interrupt routine

eim_slot1a_jump_chain:
	movl	_l3stk,sp
        movb    ADRSPC_SLOT_2, d3       | Do not remove this instruction.
	movl	ICHAIN_JUMP_SLOT2A, a0
	jmp	a0@

|
 |
 |	_eim_slot1b Low Slot IRQ  
 |
 | void eim_slot1b (void)
 | void eim_slot1b_jump_entry (void)
 |
	.globl _eim_slot1b
	.globl _eim_slot1b_jump_entry
_eim_slot1b:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_eim_slot1b_jump_entry:
	| ### Got Lance Ethernet Slot 1 interrupt:
	movw	SLOT1_DEIM_B_CSR,d3	| Read CSR0
	movw	d3,SLOT1_DEIM_B_CSR	| Clear interrupt bits
	andw	#LANCE_MASK,d3
	jeq	eim_ichain_jump_exit

	movl	HWIDB_SLOT2B,sp@- 	| Push IDB argument for C routines

	btst	#LANCE_RINT_BIT,d3	| RX interrupt?
	jeq	eim_slot1b_check_TXA	| No, check for TX interrupt
	jsr	_lance_RX_interrupt	| Call interrupt routine
eim_slot1b_check_TXA:
	btst	#LANCE_TINT_BIT,d3	| TX interrupt?
	jeq	eim_slot1b_check_EA	| No, check for error interrupt
	jsr	_lance_TX_interrupt	| Call interrupt routine
eim_slot1b_check_EA:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#LANCE_ERRORS,d3	| Mask off non-error bits
	jeq	eim_slot1b_jump_chain	| No errors, return
	movl	d0,sp@-			| Push CSR0 as long (stack alignment)
	jsr	_lance_E_interrupt	| Call interrupt routine

eim_slot1b_jump_chain:
	movl	_l3stk,sp
        movb    ADRSPC_SLOT_2,d3        | Do not remove this instruction.
	movl	ICHAIN_JUMP_SLOT2B, a0
	jmp	a0@

|
 |
 |	_eim_slot2a Low Slot IRQ  
 |
 | void eim_slot2a (void)
 | void eim_slot2a_jump_entry (void)
 |
	.globl _eim_slot2a
	.globl _eim_slot2a_jump_entry
_eim_slot2a:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_eim_slot2a_jump_entry:
	| ### Got Lance Ethernet Slot 2 interrupt:
	movw	SLOT2_DEIM_A_CSR,d3	| Read CSR0
	movw	d3,SLOT2_DEIM_A_CSR	| Clear interrupt bits
	andw	#LANCE_MASK,d3
	jeq	eim_ichain_jump_exit

	movl	HWIDB_SLOT3A,sp@- 	| Push IDB argument for C routines

	btst	#LANCE_RINT_BIT,d3	| RX interrupt?
	jeq	eim_slot2a_check_TXA	| No, check for TX interrupt
	jsr	_lance_RX_interrupt	| Call interrupt routine
eim_slot2a_check_TXA:
	btst	#LANCE_TINT_BIT,d3	| TX interrupt?
	jeq	eim_slot2a_check_EA	| No, check for error interrupt
	jsr	_lance_TX_interrupt	| Call interrupt routine
eim_slot2a_check_EA:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#LANCE_ERRORS,d3	| Mask off non-error bits
	jeq	eim_slot2a_jump_chain	| No errors, return
	movl	d0,sp@-			| Push CSR0 as long (stack alignment)
	jsr	_lance_E_interrupt	| Call interrupt routine

eim_slot2a_jump_chain:
	movl	_l3stk,sp
        movb    ADRSPC_SLOT_3,d3        | Do not remove this instruction.
	movl	ICHAIN_JUMP_SLOT3A, a0
	jmp	a0@

|
 |
 |	_eim_slot2b Low Slot IRQ  
 |
 | void eim_slot2b (void)
 | void eim_slot2b_jump_entry (void)
 |
	.globl _eim_slot2b
	.globl _eim_slot2b_jump_entry
_eim_slot2b:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_eim_slot2b_jump_entry:
	| ### Got Lance Ethernet Slot 2 interrupt:
	movw	SLOT2_DEIM_B_CSR,d3	| Read CSR0
	movw	d3,SLOT2_DEIM_B_CSR	| Clear interrupt bits
	andw	#LANCE_MASK,d3
	jeq	eim_ichain_jump_exit

	movl	HWIDB_SLOT3B,sp@- 	| Push IDB argument for C routines

	btst	#LANCE_RINT_BIT,d3	| RX interrupt?
	jeq	eim_slot2b_check_TXA	| No, check for TX interrupt
	jsr	_lance_RX_interrupt	| Call interrupt routine
eim_slot2b_check_TXA:
	btst	#LANCE_TINT_BIT,d3	| TX interrupt?
	jeq	eim_slot2b_check_EA	| No, check for error interrupt
	jsr	_lance_TX_interrupt	| Call interrupt routine
eim_slot2b_check_EA:
	movw	d3,d0			| Copy CSR0 into d0
	andw	#LANCE_ERRORS,d3	| Mask off non-error bits
	jeq	eim_slot2b_jump_chain	| No errors, return
	movl	d0,sp@-			| Push CSR0 as long (stack alignment)
	jsr	_lance_E_interrupt	| Call interrupt routine

eim_slot2b_jump_chain:
	movl	_l3stk,sp
        movb    ADRSPC_SLOT_3, d3       | Do not remove this instruction.
	movl	ICHAIN_JUMP_SLOT3B, a0
	jmp	a0@
