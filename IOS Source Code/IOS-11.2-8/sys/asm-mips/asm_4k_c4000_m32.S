/* $Id: asm_4k_c4000_m32.S,v 3.2.60.1 1996/06/16 21:11:05 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_c4000_m32.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000_m32.S - Munich32 Wrappers for Sierra
 *
 * Michael Beesley, July 1994
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000_m32.S,v $
 * Revision 3.2.60.1  1996/06/16  21:11:05  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2  1995/11/17  08:40:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * m32im_wrapper:
 * Munich32 wrapper
 * 
 * Entry :  Moved onto stack
 *	    Context has been saved
 *	    s0 = &(lev2_jumptable[slot])
 *	    s1 = Return address
 *	    s2 = IACK register address
 *	    a0 = Slot number
 *	    a1 = Slot base address
 */

 #
 # void m32im_wrapper (void)
 #
FRAME(m32im_wrapper,sp,0,s1)
	.set	noreorder

	/*
	 * Got Munich32 HDLC controller interrupt
	 */
	lbu	t0,0(s2)		# read IACK reg. (may not be needed)
	li	t1,0x2			# load value 2
	sh	t1,M32_INT_REG_OFS(a1)	# clear interrupt bit (bd slot)
	jal	m32_interrupt		# call m32 interrupt handler
        li      a1,0x0                  # load subunit number

	j	s1			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(m32im_wrapper)


/* End of file */
