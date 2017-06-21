| $Id: asm_c1000.S,v 3.2.60.1 1996/03/18 18:50:46 gstovall Exp $
| $Source: /release/111/cvs/Xsys/asm-m68k/asm_c1000.S,v $
|------------------------------------------------------------------
| asm_c1000.s - Assembler routines for the 68360 based C1000
|
| November 1994, David Coli
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: asm_c1000.S,v $
| Revision 3.2.60.1  1996/03/18  18:50:46  gstovall
| Branch: California_branch
| Elvis has left the building.  He headed out to California, and took the
| port ready changes with him.
|
| Revision 3.2.26.2  1996/03/09  05:03:39  hampton
| Slim down the kernel of the router.
| Branch: DeadKingOnAThrone_branch
|
| Revision 3.2.26.1  1996/02/20  13:32:08  dstine
| Branch: DeadKingOnAThrone_branch
|         Sync from DeadKingOnAThrone_baseline_960122 to
|                   DeadKingOnAThrone_baseline_960213
|
| Revision 3.2  1995/11/17  08:39:23  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:14  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:22  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|

#define ASMINCLUDE

#include "target_cisco.h"

	.globl _c1000_pit_isr
	.globl _c1000_clock
	.globl _c1000_hclock
	.globl	_c1000_pit_interrupt
	.globl	_exception_handler

_c1000_pit_isr:
	moveml	d0-d1/a0-a1,sp@-	| save registers not saved by C code
	movl	sp, _l7sp		| save current stack pointer
	movl	_l7stk,sp		| get new stack
	addql	#REFRESHTIME,_c1000_clock | increment the millisecond clock
	bcc	no_carry		| branch if carry clear
	addql	#1,_c1000_hclock	| we carried, so increment high clock
no_carry:
	jsr	_c1000_pit_interrupt	| Call the handler in C
	movl	_l7sp,sp		| restore old stack
	tstl	d0			| Should we force a break?
	beq	no_force_break		| No, not this time
	movw	sp@(20),d0		| Get the interrupted PS
	andiw	#0x700,d0		| Isolate the interrupted level
	cmpw	#0x700,d0		| Are we already at level 7?
	beq	no_force_break		| Yes, ignore the break now
	moveml sp@+,d0-d1/a0-a1		| restore registers
	jmp	_exception_handler	| Continue in exception handler

no_force_break:
	moveml	sp@+,d0-d1/a0-a1	| Restore registers
	rte

|
| smc_int - Interrupt wrapper for SMC console
|

	.globl	_smc_int
	.globl	_smc_console_interrupt

_smc_int:
#ifdef INTERRUPT_DEBUG
	.word	0x4afa		| Hit the BDM
#endif /* INTERRUPT_DEBUG */
	movl	sp,_l4sp	| Save working stack pointer
	movl	_l4stk,sp	| Switch to interrupt stack
	moveml	#0xc0c0,sp@-	| Save working registers
	jsr	_smc_console_interrupt | Dispatch
	moveml	sp@+,#0x0303	| Restore working registers
	movl	_l4sp,sp	| Switch back to the proper stack
	addql	#1,_l4cnt	| Count the interrupt
	rte			| Return from exception
