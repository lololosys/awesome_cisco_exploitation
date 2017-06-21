| $Id: quicc_asm.S,v 3.2.62.1 1996/03/18 21:56:36 gstovall Exp $
| $Source: /release/111/cvs/Xsys/src-36/quicc_asm.S,v $
|------------------------------------------------------------------
| Simple assembler routines for the QUICC chip.
|
| November 1994, Andrew G. Harvey
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: quicc_asm.S,v $
| Revision 3.2.62.1  1996/03/18  21:56:36  gstovall
| Branch: California_branch
| Elvis has left the building.  He headed out to California, and took the
| port ready changes with him.
|
| Revision 3.2.26.2  1996/03/09  05:11:16  hampton
| Slim down the kernel of the router.
| Branch: DeadKingOnAThrone_branch
|
| Revision 3.2.26.1  1996/02/20  17:48:56  dstine
| Branch: DeadKingOnAThrone_branch
|           Sync from DeadKingOnAThrone_baseline_960122 to
|                     DeadKingOnAThrone_baseline_960213
|
| Revision 3.2  1995/11/17  18:39:21  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  13:19:14  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  22:51:56  hampton
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
| SCC interrupt structure.
|
|	typedef quicc_interrupt {
|		ushort *event;
|		hwidbtype *idb;
|		ulong mask;
|		ulong *cisr;
|	} quicc_interrupt_t;
|
|	quicc_interrupt_t quicc_intr_array[4];
|
| void quicc_scc_interrupt (void)
|
| a2 must be set before this routine is called
|
	.globl _quicc_scc_interrupt
_quicc_scc_interrupt:
	movl	a2@+, a3		| Get event register pointer
	movw	a3@, d2			| Get event register
	movw	d2, a3@			| Clear event register
	movw	d2, sp@-		| Push event register on stack
	clrw	sp@-			| Align Stack
	movl	a2@+, sp@-		| Push idb on stack.

	movw	d2, d0			| Get cached events
	andw	#0xD, d0		| Check for receive interrupts
	jeq	checktransmit		| If clean, skip to transmit
	jbsr	_quicc_scc_rx_intr	| Jump to receive handler

checktransmit:
	movw    d2, d0 			| Restore SCCE value
	andw	#0x12, d0		| Check for transmit interrupt
	jeq	checkmisc		| Check for all other interrupts
	jbsr	_quicc_scc_tx_intr	| Jump to transmit handler

checkmisc:
	movw    d2, d0 			| Restore SCCE value
	andw	#0xFCE0, d0		| Any bits still set
	jeq	cleanup			| If none cleanup
	jbsr	_quicc_scc_misc_intr	| Jump to misc handler

cleanup:
	movl	a2@+, d0		| Get CISR mask
	movl	a2@, a3			| Cet CISR pointer
	movl    d0, a3@			| Clear CISR (in service bit)

	addql	#1, _l4cnt		| Count interrupt
	movl	_l4sp, sp		| Restore old stack
	moveml  sp@+, d0-d2/a0-a4	| Restore old registers
	rte

|
| void quicc_scc1_interrupt (void)
|
	.globl _quicc_scc1_interrupt
_quicc_scc1_interrupt:
	moveml	d0-d2/a0-a4,sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer

	lea	_quicc_intr_array, a2	| Get pointer to interrupt array
	jmp	_quicc_scc_interrupt	| Jump to common code

|
| void quicc_scc2_interrupt (void)
|
	.globl _quicc_scc2_interrupt
_quicc_scc2_interrupt:
	moveml	d0-d2/a0-a4,sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer

	lea	_quicc_intr_array, a2	| Get pointer to interrupt array
	addl	#SCC2OFF, a2		| Skip to correct section
	jmp	_quicc_scc_interrupt	| Jump to common code

|
| void quicc_scc3_interrupt (void)
|
	.globl _quicc_scc3_interrupt
_quicc_scc3_interrupt:
	moveml	d0-d2/a0-a4,sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer

	lea	_quicc_intr_array, a2	| Get pointer to interrupt array
	addl	#SCC3OFF, a2		| Skip to correct section
	jmp	_quicc_scc_interrupt	| Jump to common code

|
| void quicc_scc4_interrupt (void)
|
	.globl _quicc_scc4_interrupt
_quicc_scc4_interrupt:
	moveml	d0-d2/a0-a4,sp@-	| Save temp registers on process stack
	movl	sp, _l4sp		| Save current stack pointer
	movl	_l4stk, sp		| Get new stack pointer

	lea	_quicc_intr_array, a2	| Get pointer to interrupt array
	addl	#SCC4OFF, a2		| Skip to correct section
	jmp	_quicc_scc_interrupt	| Jump to common code

#undef ASMINCLUDE



