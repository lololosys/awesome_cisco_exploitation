/* $Id: asm_4k_c4000_lance.S,v 3.2 1995/11/17 08:40:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-mips/asm_4k_c4000_lance.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000_lance.S - Lance Wrappers for Sierra
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000_lance.S,v $
 * Revision 3.2  1995/11/17  08:40:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * Lance CSR0_BITS:
 */
#define LANCE_RINT_MASK	0x0400    /* Bit 10 */
#define LANCE_TINT_MASK	0x0200    /* Bit 9  */
#define LANCE_ERRORS	0xF900    /* ERR,BABL,CERR,MISS,MERR,IDON */
#define LANCE_MASK      0xFF00

/*
 * eim_wrapper:
 * Lance wrapper
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
 # void eim_wrapper (void)
 #
FRAME(eim_wrapper,sp,0,s1)
	.set	noreorder

	lbu	t0,0(s2)			# read IACK register
	move	t1,a1				# get slot base address

	bne	t0,zero,lance_b			# which lance wants us ?
	nop					# (bd slot)

lance_a:
	lhu	s3,DEIM_A_CSR_OFS(t1)		# read CSR0
	lw	s2,LEV2_IDB_A(s0)		# read IDB A
	sh	s3,DEIM_A_CSR_OFS(t1)		# clear interrupt bits
	andi	s3,s3,LANCE_MASK		# mask out unwanted bits
	andi	t3,s3,LANCE_RINT_MASK		# mask for Rx intr.

	beq	t3,zero,check_tx_a		# is Rx bit set ?
	andi	s0,s3,LANCE_TINT_MASK		# check for Tx intr (bd slot)

	jal	lance_RX_interrupt		# call Rx interrupt handler
	move	a0,s2				# load up idb param (bd slot)
	
check_tx_a:
	beq	s0,zero,check_err_a		# is Tx bit set ?
	andi	s0,s3,LANCE_ERRORS		# check for errors bd slot)

	jal	lance_TX_interrupt		# call Tx interrupt handler 
	move	a0,s2				# load up idb param (bd slot)

check_err_a:
	beq	s0,zero,lance_a_done		# is an Err bit set ?
	move	a0,s3				# copy CSR0 to a0

	jal	lance_E_interrupt		# call Err interrupt handler
	move	a1,s2				# load up idb param (bd slot)

lance_a_done:
	j	s1				# return
	nop					# (bd slot)

lance_b:
	lhu	s3,DEIM_B_CSR_OFS(t1)		# read CSR0
	lw	s2,LEV2_IDB_B(s0)		# read IDB B
	sh	s3,DEIM_B_CSR_OFS(t1)		# clear interrupt bits
	andi	s3,s3,LANCE_MASK		# mask out unwanted bits
	andi	t3,s3,LANCE_RINT_MASK		# mask for Rx intr.

	beq	t3,zero,check_tx_b		# is Rx bit set ?
	andi	s0,s3,LANCE_TINT_MASK		# check for Tx intr (bd slot)

	jal	lance_RX_interrupt		# call Rx interrupt handler
	move	a0,s2				# load up idb param (bd slot)
	
check_tx_b:
	beq	s0,zero,check_err_b		# is Tx bit set ?
	andi	s0,s3,LANCE_ERRORS		# check for errors bd slot)

	jal	lance_TX_interrupt		# call Tx interrupt handler 
	move	a0,s2				# load up idb param (bd slot)

check_err_b:
	beq	s0,zero,lance_b_done		# is an Err bit set ?
	move	a0,s3				# copy CSR0 to a0

	jal	lance_E_interrupt		# call Err interrupt handler
	move	a1,s2				# load up idb param (bd slot)

lance_b_done:
	j	s1				# return
	nop					# (bd slot)

	.set	reorder
ENDFRAME(eim_wrapper)

/* End of File */
