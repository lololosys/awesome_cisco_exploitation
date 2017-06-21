/* $Id: asm_4k_c4000_atmzr.S,v 3.2 1995/11/17 08:40:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-mips/asm_4k_c4000_atmzr.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000_atmzr.S - ATM Wrappers for Sierra
 *
 * Walter Friedrich, January 1995
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000_atmzr.S,v $
| Revision 3.2  1995/11/17  08:40:31  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:55:11  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:06:31  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * atm_wrapper:
 * ATM wrapper (Lo IRQ)
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
 # void atm_wrapper (void)
 #
FRAME(atm_wrapper,sp,0,s1)
	.set	noreorder

	/*
	 * Read the IACK register. If bit 0 is set, it is a cam
	 * interrupt, else it is a fddi IO interrupt
	 */
	lbu	t0,0(s2)			# read IACK register
	nop					# load delay

	lw	a0,LEV2_IDB_A(s0)		# read idb pointer (bd slot)
	nop					# (bd slot)

	jal	atmzr_lo_irq_hand		# call ISR routine
	nop					# (bd slot)

	j	s1				# return
	nop					# (bd slot)

	.set	reorder
ENDFRAME(atm_wrapper)

