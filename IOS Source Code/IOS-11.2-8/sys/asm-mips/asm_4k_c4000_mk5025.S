/* $Id: asm_4k_c4000_mk5025.S,v 3.2 1995/11/17 08:40:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-mips/asm_4k_c4000_mk5025.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000_mk5025.S - MK5025 Wrappers for Sierra
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000_mk5025.S,v $
 * Revision 3.2  1995/11/17  08:40:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * MK5025 Defines
 */
#define MK5025_RINT_MASK	0x2	/* Bit 1 */
#define MK5025_TINT_MASK	0x4	/* Bit 2 */
#define MK5025_ERRORS		0xF8	/* MERR, MISS, ROR, TUR and PINT */

#define DSIM_A_INT_MASK		0x01	/* Bit 0 */
#define DSIM_MA_INT_MASK	0x02	/* Bit 1 */
#define DSIM_B_INT_MASK		0x04	/* Bit 2 */
#define DSIM_MB_INT_MASK	0x08	/* Bit 3 */

/*
 * dsim_wrapper:
 * MK5025 Wrapper (dual serial)
 *
 * Entry :  Moved onto stack
 *	    Context has been saved
 *	    s0 = &(lev2_jumptable[slot])
 *	    s1 = Return address
 *	    s2 = IACK register address
 *	    a0 = Slot number
 *	    a1 = Slot base address
 *
 * Note : This wrapper needs to maintain a few more variables
 * 	  then the other wrappers, so we use the stack
 */

 #
 # void dsim_wrapper (void)
 #
FRAME(dsim_wrapper,sp,16,s1)
	.set	noreorder

	lhu	s2,DSIM_VECTOR_OFS(a1)		# read vector register
	addiu	sp,sp,-16			# make a stack frame

	lhu	s2,DSIM_VECTOR_OFS(a1)		# re-read vectore register
	nop					# load delay

	andi	t2,s2,DSIM_A_INT_MASK		# mask bit 0

	bne	t2,zero,dsim_check_b		# check a ?
	move	s3,a1				# copy slot base address (bd)
	
	/*
	 * Got MK5025 Channel A interrupt
	 */
	lhu	t4,DSIM_A_CSR_OFS(s3)		# read CSR0
	lw	a0,LEV2_IDB_A(s0)		# read idb A param

	sh	t4,DSIM_A_CSR_OFS(s3)		# clear interrupt bits
	andi	t5,t4,MK5025_RINT_MASK		# mask Rx intr bit
	
	beq	t5,zero,dsim_check_tx_a		# Rx intr ?
	nop					# (bd slot)

	jal	mk5025_RX_interrupt		# call the driver
	sw	t4,12(sp)			# save t4 (bd slot)

	lw	t4,12(sp)			# restore t4
	nop					# load delay

dsim_check_tx_a:
	andi	t5,t4,MK5025_TINT_MASK		# mask Tx intr bit (bd slot)
	beq	t5,zero,dsim_check_err_a	# Tx interrupt ?
	nop					# (bd slot)

	sw	t4,12(sp)			# save t4
	jal	mk5025_TX_interrupt		# call the driver
	lw	a0,LEV2_IDB_A(s0)		# read idb A param (bd slot)

	lw	t4,12(sp)			# restore t4
	nop					# load delay
	
dsim_check_err_a:
	andi	t5,t4,MK5025_ERRORS		# mask error bits (bd slot)
	beq	t5,zero,dsim_check_b		# Error intr ?
	lw	a1,LEV2_IDB_A(s0)		# read idb A param (bd slot)

	jal	mk5025_E_interrupt		# call the driver
	move	a0,t4				# setup CSR0 param (bd slot)

dsim_check_b:
	andi	t3,s2,DSIM_B_INT_MASK		# mask bit 2
	bne	t3,zero,dsim_check_ma		# check b ?
	nop					# (bd slot)

	/*
	 * Got MK5025 Channel B interrupt
	 */
	lhu	t4,DSIM_B_CSR_OFS(s3)		# read CSR0
	lw	a0,LEV2_IDB_B(s0)		# read idb B param

	sh	t4,DSIM_B_CSR_OFS(s3)		# clear interrupt bits
	andi	t5,t4,MK5025_RINT_MASK		# mask Rx intr bit
	
	beq	t5,zero,dsim_check_tx_b		# Rx intr ?
	nop					# (bd slot)

	jal	mk5025_RX_interrupt		# call the driver
	sw	t4,12(sp)			# save t4 (bd slot)

	lw	t4,12(sp)			# restore t4
	nop					# load delay

dsim_check_tx_b:
	andi	t5,t4,MK5025_TINT_MASK		# mask Tx intr bit (bd slot)
	beq	t5,zero,dsim_check_err_b	# Tx intr ?
	nop					# (bd slot)

	sw	t4,12(sp)			# save t4
	jal	mk5025_TX_interrupt		# call the driver
	lw	a0,LEV2_IDB_B(s0)		# read idb B param (bd slot)

	lw	t4,12(sp)			# restore t4
	nop					# load delay

dsim_check_err_b:
	andi	t5,t4,MK5025_ERRORS		# mask error bits (bd slot)
	beq	t5,zero,dsim_check_ma		# Error intr ?
	lw	a1,LEV2_IDB_B(s0)		# read idb B param (bd slot)

	jal	mk5025_E_interrupt		# call the driver
	move	a0,t4				# setup CSR0 param (bd slot)

dsim_check_ma:
	andi	t3,s2,DSIM_MA_INT_MASK		# mask bit 1
	bne	t3,zero,dsim_check_mb		# check ma ?
	nop					# (bd slot)

	/*
	 * Got a channel A modem state change interrupt
	 */
	jal	mk5025_xx_delta_interrupt	# call the driver
	lw	a0,LEV2_IDB_A(s0)		# setup idb param (bd slot)

dsim_check_mb:
	andi	t3,s2,DSIM_MB_INT_MASK		# mask bit 3
	bne	t3,zero,dsim_exit		# check mb ?
	nop					# (bd slot)

	/*
	 * Got a channel B modem state change interrupt
	 */
	jal	mk5025_xx_delta_interrupt	# call the driver
	lw	a0,LEV2_IDB_B(s0)		# setup idb param (bd slot)

dsim_exit:
	j	s1				# return
	addiu	sp,sp,16			# remove stack frame (bd slot)

	.set	reorder
ENDFRAME(dsim_wrapper)

/* End of File */
