/* $Id: asm_4k_c4000_tms380.S,v 3.2 1995/11/17 08:40:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-mips/asm_4k_c4000_tms380.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000_tms380.S - TMS380 Wrappers for Sierra
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000_tms380.S,v $
 * Revision 3.2  1995/11/17  08:40:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * TMS380 Defines
 */
#define TMS380_SYSINT_MASK	0x0080		/* Bit 7 */

/*
 * trim_wrapper:
 * TMS380 Wrapper (single R)
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
 # void trim_wrapper (void)
 #
FRAME(trim_wrapper,sp,0,s1)
	.set	noreorder

	lbu	t0,0(s2)			# read IACK register
	move	t1,a1				# get slot base address

	bne	t0,zero,eagle_b			# which eagle wants us ?
	nop


eagle_a:
	lhu	a2,TRIM_A_SIFSTS_OFS(t1)	# read SIFTST
	lw	a0,LEV2_IDB_A(s0)		# read idb param
	andi	t2,a2,TMS380_SYSINT_MASK	# get INT bit

	beq	t2,zero,eagle_a_done		# no interrupt
	addiu	a1,a1,TRIM_A_SIFSTS_OFS		# get SIFSTS address (bd slot)

	jal	tms380_sif_handler		# call interrupt routine
	nop					# (bd slot)

eagle_a_done:
	j	s1				# return
	nop					# (bd slot)


eagle_b:
	lhu	a2,TRIM_B_SIFSTS_OFS(t1)	# read SIFTST
	lw	a0,LEV2_IDB_B(s0)		# read idb param
	andi	t2,a2,TMS380_SYSINT_MASK	# get INT bit

	beq	t2,zero,eagle_b_done		# no interrupt
	lui	t3,TRIM_B_SIFSTS_OFS >> 16	# Upper half offset (bd slot)

	addiu	t3,t3,TRIM_B_SIFSTS_OFS & 0xffff# t3 = status offset

	jal	tms380_sif_handler		# call interrupt routine
	daddu	a1,a1,t3			# get SIFSTS address (bd slot) 

eagle_b_done:
	j	s1				# return
	nop	
	.set	reorder
ENDFRAME(trim_wrapper)

/* End of File */






