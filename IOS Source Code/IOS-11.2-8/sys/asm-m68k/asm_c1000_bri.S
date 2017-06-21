| $Id: asm_c1000_bri.S,v 3.2 1995/11/17 08:39:24 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c1000_bri.S,v $
|------------------------------------------------------------------
| BRI specfic asm routines for c1000.
|
| January 1995, Eric Fronberg
|
| Copyright (c) 1995 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: asm_c1000_bri.S,v $
| Revision 3.2  1995/11/17  08:39:24  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:15  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:24  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|

#define ASMINCLUDE

#include "../src-36-c1000/pcmap.h"
#include "../src-36/quicc.h"
#include "../src-36/quicc_asm.h"

|
| void quicc_tp3420_interrupt (void)
|
| Note: This wrapper assumes that the D Channel is on SCC3.
|
| Register Usage:
|
| a2 - offset into quicc_intr_array for SCC3 (D-Channel).
| 
	.globl _quicc_tp3420_interrupt
_quicc_tp3420_interrupt:
	moveml	d0-d2/a0-a2, sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer

	lea	_quicc_intr_array, a2	| Get pointer to interrupt array
	addl	#SCC3OFF, a2		| Skip to correct section
	movl	a2@(4), sp@-		| Push idb on stack.
	jbsr    _tp3420_interrupt	| Call Interrupt routine
	addql	#1, _l4cnt		| count interrupt
	movl	_l4sp, sp		| Restore old stack
	moveml  sp@+, d0-d2/a0-a2	| Restore old registers
	rte

|
| void quicc_timer_t3 (void)
|
| Note: This wrapper assumes that the D Channel is on SCC3.
|
| Register Usage:
|
| a2 - offset into quicc_intr_array for SCC3 (D-Channel).
| 
	.globl _quicc_timer_t3
_quicc_timer_t3:
	moveml	d0-d2/a0-a2, sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer

	lea	_quicc_intr_array, a2	| Get pointer to interrupt array
	addl	#SCC3OFF, a2		| Skip to correct section
	movl	a2@(4), sp@-		| Push idb on stack.
	jbsr    _bri_t3_handler		| Call Interrupt routine
	addql	#1, _l4cnt		| count interrupt
	movl	_l4sp, sp		| Restore old stack
	moveml  sp@+, d0-d2/a0-a2	| Restore old registers
	rte

|
| void quicc_timer_t4 (void)
|
| Note: This wrapper assumes that the D Channel is on SCC3.
|
| Register Usage:
|
| a2 - offset into quicc_intr_array for SCC3 (D-Channel).
| 
	.globl _quicc_timer_t4
_quicc_timer_t4:
	moveml	d0-d2/a0-a2, sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer

	lea	_quicc_intr_array, a2	| Get pointer to interrupt array
	addl	#SCC3OFF, a2		| Skip to correct section
	movl	a2@(4), sp@-		| Push idb on stack.
	jbsr    _bri_t4_handler		| Call Interrupt routine
	addql	#1, _l4cnt		| count interrupt
	movl	_l4sp, sp		| Restore old stack
	moveml  sp@+, d0-d2/a0-a2	| Restore old registers
	rte

|
| void quicc_timer_2ms (void)
|
| Note: This wrapper assumes that the D Channel is on SCC3.
|
| Register Usage:
|
| a2 - offset into quicc_intr_array for SCC3 (D-Channel).
| 
	.globl _quicc_timer_2ms
_quicc_timer_2ms:
	moveml	d0-d2/a0-a2, sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer

	lea	_quicc_intr_array, a2	| Get pointer to interrupt array
	addl	#SCC3OFF, a2		| Skip to correct section
	movl	a2@(4), sp@-		| Push idb on stack.
	jbsr    _bri_2ms_handler	| Call Interrupt routine
	addql	#1, _l4cnt		| count interrupt
	movl	_l4sp, sp		| Restore old stack
	moveml  sp@+, d0-d2/a0-a2	| Restore old registers
	rte

#undef ASMINCLUDE



