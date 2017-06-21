| $Id: asm_alc.S,v 3.2.60.2 1996/05/21 23:53:40 rlowe Exp $
| $Source: /release/112/cvs/Xsys/asm-m68k/asm_alc.S,v $
|------------------------------------------------------------------
| asm_alc.S -- Assembler routines for 68ec030 based Synergy ATM line card.
| Based on asm_c3000.S -- Assembler routines for the 68020 Based Pancake.
|
| April 13, 1995, Rick Lowe
|
| Copyright (c) 1995-1996 by cisco Systems, Inc.
| All rights reserved.
|
| Registers a0, a1, d0, and d1 are always available.
| Any other registers must be preserved for the caller.
|------------------------------------------------------------------
| /*
| $Log: asm_alc.S,v $
| Revision 3.2.60.2  1996/05/21  23:53:40  rlowe
| Fix typo/bug found during build testing with new toolchain.
| CSCdi58362:  gcc.96q1 compiler found bug in asm_alc.S
| Branch: California_branch
|
| Revision 3.2.60.1  1996/05/09  14:10:30  rbadri
| Branch: California_branch
| LANE, UNI3.1 features
|
| Revision 3.2.90.1  1996/04/27  06:32:52  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 3.2.80.1  1996/04/08  01:43:56  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 3.2.66.2  1996/04/06  00:16:06  rlowe
| Housekeeping cleanup of old ifdefs.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 3.2.66.1  1996/03/22  09:34:41  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 3.2.52.1  1996/03/03  21:22:40  rlowe
| Apply SYNALC content to merge branch.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 3.2.50.1  1996/03/02  01:18:38  rlowe
| Non-sync sync to mainline tag V111_1_0_3.
| Branch: Synalc3_Cal_V111_1_0_3_branch
|
| Revision 3.2.34.1  1996/02/29  07:48:03  rlowe
| First pass non-sync sync for Synergy ATM line card to V111_0_16.
| Branch: Synalc3_Cal_V111_0_16_branch
|
| Revision 3.2  1995/11/17  08:39:20  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:11  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1.62.1  1996/02/03  07:06:08  rlowe
| Non-sync sync of Synalc_catchup_branch to V111_0_5.
| Branch: Synalc_catchup_pretal_branch
|
| Revision 2.1.44.1  1995/09/11  01:55:56  rlowe
| Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
| yielding Synalc2_LE2_V110_1_0_3_branch.
|
| Branch: Synalc2_LE2_V110_1_0_3_branch
|
| Revision 2.1.28.1  1995/08/31  12:46:03  rlowe
| Carryover applicable code from old Synergy_ATMlc_branch (10.3)
| to Synalc2_branch (11.0).
|
| Branch: Synalc2_branch
|
| Revision 2.1  1995/06/07  20:05:20  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
| */
|

#define	ASMINCLUDE				/* SYNALC */
#include "asm_alc.h"				/* SYNALC */
#include "../src-68-alc/alc.h"			/* SYNALC */
|
 |
 | Hacked for faster network interrupts on Pancake.
 | We gain speed by getting rid of ether_interrupt() and calling the
 | appropriate C routines directly from here.
 |
 | This file contaions new style nifty interrupt wrappers.  There is an outside
 | wrapper for each platform (pan, brut, cristal).  The wrappers check the 
 | interrupt bits to discover which interface caused the interrupt.  They then 
 | index into  pan_int_array and jump to the handler for that interface.  The
 | wrappers do interrupt chaining by looping until no more level4 interrupts
 | are asserted.
 |
 | These wrapper routines will test CSR0 in each network interface
 | to determine which of the RX, TX, or Error interrupt routines from
 | if/if_pan_lance and if/if_pan_mk5025.c are to be called.
 |
 | PLEASE NOTE that it is assumed that the RAP register for every interface
 | is always zero. The C code must ensure that network interrupts are
 | disabled before changing RAP and must restore the RAP to 0 before enabling
 | network interrupts again.
 |
 | Note that we do not clean up the IDB pointer from the interrupt stack
 | after calling the C routines. We do not have to since we are restoring
 | the stack pointer via the moveml before returning.


|
|       level2_ALC - Level 2 interrupt routine for SYNALC.
|                    This interrupt is cleared when it is serviced.
|		     Just execute linearly, without loops.  Add later
|		     as an enhancement if desired.  Instead of using
|		     _pan_level2_int or _pan_int_array structures,
|		     have an array of ISR entry points _l2_isr_ALC.
|
        .globl	_level2_ALC,   _level2_ALC_0, _level2_ALC_1
	.globl	_level2_ALC_2, _level2_ALC_3, _level2_ALC_4
	.globl	_level2_ALC_5
_level2_ALC_0:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
	moveq	#(0 * SIZ_INTR_ARRY),d0	| Get array offset to intr descriptor.
	jra	_level2_ALC
_level2_ALC_1:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
	moveq	#(1 * SIZ_INTR_ARRY),d0	| Get array offset to intr descriptor.
	jra	_level2_ALC
_level2_ALC_2:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
	moveq	#(2 * SIZ_INTR_ARRY),d0	| Get array offset to intr descriptor.
	jra	_level2_ALC
_level2_ALC_3:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
	moveq	#(3 * SIZ_INTR_ARRY),d0	| Get array offset to intr descriptor.
	jra	_level2_ALC
_level2_ALC_4:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
	moveq	#(4 * SIZ_INTR_ARRY),d0	| Get array offset to intr descriptor.
	jra	_level2_ALC
_level2_ALC_5:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
	moveq	#(5 * SIZ_INTR_ARRY),d0	| Get array offset to intr descriptor.
	jra	_level2_ALC
	nop				| Just a placeholder.
_level2_ALC:
        movl    sp,_l2sp                | Save current stack pointer
        movl    _l2stk,sp               | Get new stack pointer
	lea	_alc_int_array,a1	| Get ptr to relevant intr descriptor.
	addal	d0,a1			| Need array offset too.
	addql	#(1),a1@(12)		| Count specific interrupt class.
	movl	a1@,a0			| Set up subroutine ptr
	jbsr	a0@			| jmp to handler
        addql   #(1),_l2cnt		| count interrupts at this level.
        movl    _l2sp,sp                | restore old stack
        moveml  sp@+,REGISTERS          | restore registers from old stack
        rte                             | return from exception


 |
 |	level2_nop - Level 2 interrupt wrapper for a nop
 |
 | void level2_nop (void)
 |
	.globl _level2_nop
_level2_nop:
	movl	a2@(4),sp@-		| Push IDB as argument for C routines
	jsr	_alc_spurious_level2	| Error, we should never get this
	addql	#(4),sp			| adjust stack
	rts				| bye bye

