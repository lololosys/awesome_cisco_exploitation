/* $Id: e_r4k.S,v 3.2.58.1 1996/03/21 22:51:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/isdn/e_r4k.S,v $
 *------------------------------------------------------------------
 * e_r4k.S  Low level support routines for MTEX on r4k
 *
 * July 1994, Michael Beesley
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: e_r4k.S,v $
 * Revision 3.2.58.1  1996/03/21  22:51:43  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1996/02/16  03:42:10  mbeesley
 * CSCdi49179:  ISDN subsystem does not compile on Predator
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  17:40:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:15:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#if 	defined(SIERRA)
#include "../asm-mips/asm_4k_c4000.h"
#endif	/* SIERRA */

#if	defined(RSP)
#include "../asm-mips/asm_4k_rsp.h"
#endif

#if	defined(PREDATOR)
#include "../asm-mips/asm_4k_c7100.h"
#endif

/*
 *************************************************************************
 * 
 * MODULE: e_r4k.S
 * DESCRIPTION: Low level support routines for MTEX on r4k cisco boxes
 *
 * GENERAL NOTES
 *
 * This file contains low level routines for running MTEX on a
 * MIPS r4k based cisco system.
 */

/*
 * Externs; allow gas to use gp relative addressing
 */
	.extern forkx, 4

/*
 * _swstack : Switch task stacks for the ISDN MTEX executive
 * Note : On cisco platforms the ISDN executive runs a task
 *	  switcher on top of the cisco scheduler IE looks like one
 *	  task to the scheduler. This should probably be redone at some
 *	  stage to run several scheduler tasks and remove the ISDN executive
 *	  altogether (mjb. 7/11/94)
 */

 #
 # void _swstack (unsigned *old, unsigned *new)
 #
FRAME(_swstack, sp, 0, ra)
	.set	noreorder

	/*
 	 * The task creation code (Cisco_InitTCB()) sets up a stack
	 * frame suitable for _swstack to switch to. Basically save
	 * all permanent registers on the stack, move the stack
	 * and pop of all the permanent registers and return.
	 */

	/*
	 * Make a stack frame on present stack
	 */
	addiu	sp,sp,-40	# 40 bytes for the frame

	/*
	 * Save current stack pointer
	 */
	sw	sp,0(a0)	# save current sp in old

	/*
	 * Save all permanent registers on present stack (s0..s8,ra)
	 */
	sw	ra,0(sp)	# save ra
	sw	s0,4(sp)	# save s0
	sw	s1,8(sp)	# save s1
	sw	s2,12(sp)	# save s2
	sw	s3,16(sp)	# save s3
	sw	s4,20(sp)	# save s4
	sw	s5,24(sp)	# save s5
	sw	s6,28(sp)	# save s6
	sw	s7,32(sp)	# save s7
	sw	s8,36(sp)	# save s8

	/*
 	 * Move onto the 'new' stack
	 */
	lw	sp,0(a1)	# read new stack pointer
	nop			# load delay

	/*
	 * Report the stack change to the scheduler.
	 */
	lw	t0,forkx	# load the current process pointer
	nop			# load delay
	sw	a2,0(t0)	# rewrite the stack pointer.

	/*
	 * Restore the 'new' task register set
	 */	
	lw	ra,0(sp)	# restore ra
	lw	s0,4(sp)	# restore s0
	lw	s1,8(sp)	# restore s1
	lw	s2,12(sp)	# restore s2
	lw	s3,16(sp)	# restore s3
	lw	s4,20(sp)	# restore s4
	lw	s5,24(sp)	# restore s5
	lw	s6,28(sp)	# restore s6
	lw	s7,32(sp)	# restore s7
	lw	s8,36(sp)	# restore s8

	/*
	 * Return on 'new' thread
	 */
	j	ra		# return
	addiu	sp,sp,40	# remove stack frame

	.set	reorder
ENDFRAME(_swstack)

/* End of file */
