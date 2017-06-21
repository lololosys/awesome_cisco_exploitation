/* $Id: asm_4k_vip_amdp2.S,v 3.2.54.2 1996/04/23 17:13:20 gshen Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_vip_amdp2.S,v $
 *------------------------------------------------------------------
 * asm_4k_vip_amdp2.S : Wrapper for 8e PCI Port Adaptor
 *
 * April 1995, Dave Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip_amdp2.S,v $
 * Revision 3.2.54.2  1996/04/23  17:13:20  gshen
 * CSCdi55375:  Need to have different address separation for IO and
 * memory based IO
 * Branch: California_branch
 *
 * Two new I/O address separation definitions:
 *
 * #define PCI_DEV_IO_MAPPED_IO_SEPARATION  0x0400 / * 1k * /
 * #define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000 / * 16k * /
 *
 * These changes have been code reviewed.
 *
 * Revision 3.2.54.1  1996/03/21  22:27:29  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/02/16  03:07:12  getchell
 * CSCdi49146:  optimize svip wrappers for amdp2 and dec21140
 * Changed how dtq and drq were timesharing with the wrappered interfaces.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/10  00:25:40  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  20:25:10  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:40:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:06:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

/*
 * Platform include file
 */
#if defined(RVIP)
# include "asm_4k_vip.h"
# include "asm_4k_rvip.h"
#endif	/* RVIP */

#if 	defined(SVIP)
# include "asm_4k_vip.h"
# include "asm_4k_svip.h"
#endif	/* SVIP */

#if defined(PREDATOR)
# include "asm_4k_c7100.h"
#endif	/* PREDATOR */

/*
 * AMDP2 CSR0 bits
 */
#define AMDP2_INTR_MASK		0xff00
#define AMDP2_RINT_MASK		0x0400
#define AMDP2_TINT_MASK		0x0200
#define AMDP2_ERRORS_MASK	0xf900

/*
 * AMDP2 CSR0 offsets
 * Note: Only support memory mapped io
 */
#define AMDP2_0_CSR0_OFS        (0x10)
#define AMDP2_1_CSR0_OFS        ((PCI_DEV_MEM_MAPPED_IO_SEPARATION * 1) + 0x10)
#define AMDP2_2_CSR0_OFS        ((PCI_DEV_MEM_MAPPED_IO_SEPARATION * 2) + 0x10)
#define AMDP2_3_CSR0_OFS        ((PCI_DEV_MEM_MAPPED_IO_SEPARATION * 3) + 0x10)
#define AMDP2_4_CSR0_OFS        ((PCI_DEV_MEM_MAPPED_IO_SEPARATION * 4) + 0x10)
#define AMDP2_5_CSR0_OFS        ((PCI_DEV_MEM_MAPPED_IO_SEPARATION * 5) + 0x10)
#define AMDP2_6_CSR0_OFS        ((PCI_DEV_MEM_MAPPED_IO_SEPARATION * 6) + 0x10)
#define AMDP2_7_CSR0_OFS        ((PCI_DEV_MEM_MAPPED_IO_SEPARATION * 7) + 0x10)

#if !defined(SVIP)

/*
 * amdp2_eim_wrapper: Does not use posted read hardware
 * 
 * Entry:   Moved onto stack
 *	    Context has been saved
 *	    s0 = &(pa_jumptable[slot])
 *	    s1 = Return address
 *	    a0 = PA bay
 *	    a1 = PA base address
 *	    a2 = interrupt bits [0..7]
 */
 #
 # boolean amdp2_eim_wrapper (void)
 #
FRAME(amdp2_eim_wrapper,sp,16,s1)
	.set	noreorder

	bne	a2,zero,amdp2_work		# is there any work ?
	nop					# (bd slot)	

	j	s1				# return
	move	v0,zero				# no more work (bd slot)

amdp2_work:
	addiu	sp,sp,-16			# make a stack frame
	sw	s5,8(sp)			# save s5
	sw	s6,12(sp)			# save s6

	/*
	 * Check if we are doing first half, or second half
	 */
	lw	s2,PA_SCRATCH0_OFS(s0)		# read scratch 0
	sw	s1,4(sp)			# save return address
	
	bne	s2,zero,amdp2_second_half	# do ports 4..7 ?
	li	s2,0x01				# load s2 with 1 (bd slot)

	sw	s2,PA_SCRATCH0_OFS(s0)		# do second half next time
	move	s2,a1				# save base address

	/*
	 * Analyze ports 0..3
	 */
amdp2_first_half:

	/*
	 * Save parameter a2, interrupt bits
	 */
	move	s5,a2

	/*
	 * Test for port 0
	 */
	andi	a0,s5,0x01			# check interface 0
	beq	a0,zero,amdp2_port1		# is interface 0 on ?
	srl	s5,s5,1				# shift interrupt bits (bd)

	/*
	 * Process interface 0
	 */
	lw	s1,AMDP2_0_CSR0_OFS(s2)		# read csr0
	lw	s6,PA_IDB_OFS(s0)		# read idb
	sw	s1,AMDP2_0_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,check_0_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_0_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,check_0_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_0_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,amdp2_port1		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

amdp2_port1:
	andi	a0,s5,0x01			# check interface 1
	beq	a0,zero,amdp2_port2		# is interface 1 on ?
	srl	s5,s5,1				# shift interrupt bits (bd)

	/*
	 * Process interface 1
	 */
	lw	s1,AMDP2_1_CSR0_OFS(s2)		# read csr0
	lw	s6,(PA_IDB_OFS+4)(s0)		# read idb
	sw	s1,AMDP2_1_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,check_1_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_1_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,check_1_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_1_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,amdp2_port2		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

amdp2_port2:
	andi	a0,s5,0x01			# check interface 2
	beq	a0,zero,amdp2_port3		# is interface 2 on ?
	srl	s5,s5,1				# shift interrupt bits (bd)

	/*
	 * Process interface 2
	 */
	lw	s1,AMDP2_2_CSR0_OFS(s2)		# read csr0
	lw	s6,(PA_IDB_OFS+8)(s0)		# read idb
	sw	s1,AMDP2_2_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,check_2_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_2_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,check_2_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_2_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,amdp2_port3		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

amdp2_port3:
	andi	a0,s5,0x01			# check interface 3
	beq	a0,zero,amdp2_done1		# is interface 3 on ?
	srl	s5,s5,1				# shift interrupt bits (bd)

	/*
	 * Process interface 3
	 */
	lw	s1,AMDP2_3_CSR0_OFS(s2)		# read csr0
	lw	s6,(PA_IDB_OFS+12)(s0)		# read idb
	sw	s1,AMDP2_3_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,check_3_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_3_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,check_3_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_3_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,amdp2_done1		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

amdp2_done1:
	lw	s1,4(sp)			# restore return address
	lw	s5,8(sp)			# restore s5
	lw	s6,12(sp)			# restore s6
	addiu	sp,sp,16			# remove stack frame

	/*
	 * Return to the interrupt handler
	 */
	j	s1				# return
	move	v0,zero				# no more work to do (bd slot)

	/*
	 * Analyze ports 4..7
	 */
amdp2_second_half:

	/*
	 * Zero out scratch area so we look at ports 0..3 next time.
	 * And shift the interrupt bits by four.
	 */
	sw	zero,PA_SCRATCH0_OFS(s0)	# mark first half next time
	move	s2,a1				# save base address
	srl	s5,a2,4				# shift right 4 bits

	/*
	 * Test for port 4
	 */
	andi	a0,s5,0x01			# check interface 4
	beq	a0,zero,amdp2_port5		# is interface 4 on ?
	srl	s5,s5,1				# shift interrupt bits (bd)

	/*
	 * Process interface 4
	 */
	lw	s1,AMDP2_4_CSR0_OFS(s2)		# read csr0
	lw	s6,(PA_IDB_OFS+16)(s0)		# read idb
	sw	s1,AMDP2_4_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,check_4_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_4_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,check_4_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_4_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,amdp2_port5		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

amdp2_port5:
	andi	a0,s5,0x01			# check interface 5
	beq	a0,zero,amdp2_port6		# is interface 5 on ?
	srl	s5,s5,1				# shift interrupt bits (bd)

	/*
	 * Process interface 5
	 */
	lw	s1,AMDP2_5_CSR0_OFS(s2)		# read csr0
	lw	s6,(PA_IDB_OFS+20)(s0)		# read idb
	sw	s1,AMDP2_5_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,check_5_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_5_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,check_5_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_5_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,amdp2_port6		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

amdp2_port6:
	andi	a0,s5,0x01			# check interface 6
	beq	a0,zero,amdp2_port7		# is interface 6 on ?
	srl	s5,s5,1				# shift interrupt bits (bd)

	/*
	 * Process interface 6
	 */
	lw	s1,AMDP2_6_CSR0_OFS(s2)		# read csr0
	lw	s6,(PA_IDB_OFS+24)(s0)		# read idb
	sw	s1,AMDP2_6_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,check_6_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_6_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,check_6_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_6_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,amdp2_port7		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

amdp2_port7:
	andi	a0,s5,0x01			# check interface 7
	beq	a0,zero,amdp2_done2		# is interface 7 on ?
	srl	s5,s5,1				# shift interrupt bits (bd)

	/*
	 * Process interface 7
	 */
	lw	s1,AMDP2_7_CSR0_OFS(s2)		# read csr0
	lw	s6,(PA_IDB_OFS+28)(s0)		# read idb
	sw	s1,AMDP2_7_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,check_7_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_7_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,check_7_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

check_7_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,amdp2_done2		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

amdp2_done2:
	lw	s1,4(sp)			# restore return address
	lw	s5,8(sp)			# restore s5
	lw	s6,12(sp)			# restore s6
	addiu	sp,sp,16			# remove stack frame

	/*
	 * Return
	 */
	j	s1				# return
	move	v0,zero				# no more work (bd slot)

	.set	reorder
ENDFRAME(amdp2_eim_wrapper)

#else /* SVIP, use posted read hardware */

/*
 * amdp2_eim_wrapper: Uses posted read hardware
 * 
 * Entry:   Moved onto stack
 *	    Context has been saved
 *	    s0 = &(pa_jumptable[slot])
 *	    s1 = Return address
 *	    s2 = PA base address  - garbaged by wrapper (not PR)
 *	    a1 = Posted Read indication - garbaged by wrapper
 *	    a2 = Interrupt bits [0..7]
 *
 * Usage:   s3 = trashed (saved by asm_4k_svip.S)
 *          s5 = saved and used for posted read stuff
 *          s6 = saved and used for idb between rx and tx interrupts
 */
 #
 # boolean amdp2_eim_wrapper (void)
 #
FRAME(amdp2_eim_wrapper,sp,16,s1)
	.set	noreorder

	addiu	sp,sp,-16			# make a stack frame
	sw	s5,8(sp)			# save s5
	sw	s6,12(sp)			# save s6
	sw	s1,4(sp)			# save return address

	/*
	 * Save parameter a2, interrupt bits
	 */
	move	s3,a2

#ifdef  PCI_POSTED_READS
	la	s5,PMA_POSTED_READ_REG0         # addr of PMA PR0
#endif
	/*
	 * Test for port 0
	 */
	andi	a0,s3,0x01			# check interface 0
	beq	a0,zero,s_amdp2_port1		# is interface 0 on ?
	srl	s3,s3,1				# shift interrupt bits (bd)

#ifdef  PCI_POSTED_READS
	lw	a1,PA_SREG_OFS(s0)		# get addr for PR0
	sw	a1,PMA_PR0_OFFSET(s5)		# start PR0
#endif

s_amdp2_work:
	
	/* Do some useful work while posted read is completing */
	jal     vip_dtq_consumer
	nop

	/*
	 * Process interface 0
	 */
#ifdef  PCI_POSTED_READS
        lw      s1,PMA_PR0_OFFSET(s5)           # read CSR0 from PR0
#else
        lw      s1,AMDP2_0_CSR0_OFS(s2)         # read csr0
#endif
	lw	s6,PA_IDB_OFS(s0)		# read idb
	sw	s1,AMDP2_0_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,s_check_0_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_0_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,s_check_0_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_0_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,s_amdp2_port1		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

	/*
	 * Process interface 1
	 */

s_amdp2_port1:

	andi	a0,s3,0x01			# check interface 1
	beq	a0,zero,s_amdp2_port2		# is interface 1 on ?
	srl	s3,s3,1				# shift interrupt bits (bd)

#ifdef  PCI_POSTED_READS
	/*
	 * Start posted read for PR1 
	 */
	lw	a2,PA_SREG_OFS+4(s0)		# get addr for PR1
	sw	a2,PMA_PR1_OFFSET(s5)		# start PR1
#endif

	/* Do some useful work while posted read is completing */
	jal     vip_drq_consumer
	nop

#ifdef  PCI_POSTED_READS
        lw      s1,PMA_PR1_OFFSET(s5)           # read CSR0 from PR1 (bd)
#else
        lw      s1,AMDP2_1_CSR0_OFS(s2)         # read csr0
#endif
	lw	s6,(PA_IDB_OFS+4)(s0)		# read idb
	sw	s1,AMDP2_1_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,s_check_1_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_1_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,s_check_1_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_1_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,s_amdp2_port2		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

	/*
	 * Process interface 2
	 */
s_amdp2_port2:

	andi	a0,s3,0x01			# check interface 2
	beq	a0,zero,s_amdp2_port3		# is interface 2 on ?
	srl	s3,s3,1				# shift interrupt bits (bd)

#ifdef  PCI_POSTED_READS
	/*
	 * Start posted read for interface 2
         */
	lw	a1,PA_SREG_OFS+8(s0)		# get addr for PR2
	sw	a1,PMA_PR2_OFFSET(s5)		# start PR2
#endif
	/* Do some useful work while posted read is completing */
	jal     vip_dtq_consumer
	nop


#ifdef  PCI_POSTED_READS
        lw      s1,PMA_PR2_OFFSET(s5)           # read CSR0 from PR2
#else
        lw      s1,AMDP2_2_CSR0_OFS(s2)         # read csr0
#endif
	lw	s6,(PA_IDB_OFS+8)(s0)		# read idb
	sw	s1,AMDP2_2_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,s_check_2_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_2_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,s_check_2_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_2_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,s_amdp2_port3		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

	/*
	 * Process interface 3
	 */
s_amdp2_port3:

	andi	a0,s3,0x01			# check interface 3
	beq	a0,zero,s_amdp2_port4		# is interface 3 on ?
	srl	s3,s3,1				# shift interrupt bits (bd)

#ifdef  PCI_POSTED_READS
	/*
	 * Start posted read for interface 3
         */
	lw	a1,PA_SREG_OFS+12(s0)		# get addr for PR3
	sw	a1,PMA_PR3_OFFSET(s5)		# start PR3
#endif

	/* Do some useful work while posted read is completing */
	jal     vip_drq_consumer
	nop

#ifdef  PCI_POSTED_READS
        lw      s1,PMA_PR3_OFFSET(s5)           # read CSR0 from PR3
#else
        lw      s1,AMDP2_3_CSR0_OFS(s2)         # read csr0
#endif
	lw	s6,(PA_IDB_OFS+12)(s0)		# read idb
	sw	s1,AMDP2_3_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,s_check_3_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_3_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,s_check_3_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_3_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,s_amdp2_port4		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

	/*
	 * Process interface 4
	 */
s_amdp2_port4:

	andi	a0,s3,0x01			# check interface 4
	beq	a0,zero,s_amdp2_port5		# is interface 4 on ?
	srl	s3,s3,1				# shift interrupt bits (bd)

#ifdef  PCI_POSTED_READS
	/*
	 * Start posted read for interface 4
         */
	lw	a1,PA_SREG_OFS+16(s0)		# get addr for PR4
	sw	a1,PMA_PR4_OFFSET(s5)		# start PR4
#endif

	/* Do some useful work while posted read is completing */
        jal     vip_dtq_consumer
	nop

#ifdef  PCI_POSTED_READS
        lw      s1,PMA_PR4_OFFSET(s5)           # read CSR0 from PR4
#else
        lw      s1,AMDP2_4_CSR0_OFS(s2)         # read csr0
#endif
	lw	s6,(PA_IDB_OFS+16)(s0)		# read idb
	sw	s1,AMDP2_4_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,s_check_4_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_4_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,s_check_4_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_4_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,s_amdp2_port5		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)


	/*
	 * Process interface 5
	 */
s_amdp2_port5:

	andi	a0,s3,0x01			# check interface 5
	beq	a0,zero,s_amdp2_port6		# is interface 5 on ?
	srl	s3,s3,1				# shift interrupt bits (bd)

#ifdef  PCI_POSTED_READS
	/*
	 * Start posted read for interface 5
         */
	lw	a2,PA_SREG_OFS+20(s0)		# get addr for PR5
	sw	a2,PMA_PR5_OFFSET(s5)		# start PR5
#endif	

	/* Do some useful work while posted read is completing */
        jal     vip_drq_consumer
	nop

#ifdef  PCI_POSTED_READS
        lw      s1,PMA_PR5_OFFSET(s5)           # read CSR0 from PR5
#else
        lw      s1,AMDP2_5_CSR0_OFS(s2)         # read csr0
#endif
	lw	s6,(PA_IDB_OFS+20)(s0)		# read idb
	sw	s1,AMDP2_5_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,s_check_5_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_5_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,s_check_5_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_5_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,s_amdp2_port6		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

	/*
	 * Process interface 6
	 */
s_amdp2_port6:

	andi	a0,s3,0x01			# check interface 6
	beq	a0,zero,s_amdp2_port7		# is interface 6 on ?
	srl	s3,s3,1				# shift interrupt bits (bd)

#ifdef  PCI_POSTED_READS
	/*
	 * Start posted read for interface 6
         */
	lw	a1,PA_SREG_OFS+24(s0)		# get addr for PR6
	sw	a1,PMA_PR6_OFFSET(s5)		# start PR6
#endif

	/* Do some useful work while posted read is completing */
        jal     vip_dtq_consumer
	nop

#ifdef  PCI_POSTED_READS
        lw      s1,PMA_PR6_OFFSET(s5)           # read CSR0 from PR6
#else
        lw      s1,AMDP2_6_CSR0_OFS(s2)         # read csr0
#endif
	lw	s6,(PA_IDB_OFS+24)(s0)		# read idb
	sw	s1,AMDP2_6_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,s_check_6_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_6_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,s_check_6_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_6_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,s_amdp2_port7		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

	/*
	 * Process interface 7
	 */
s_amdp2_port7:

	andi	a0,s3,0x01			# check interface 7
	beq	a0,zero,s_amdp2_done		# is interface 7 on ?
	srl	s3,s3,1				# shift interrupt bits (bd)

#ifdef  PCI_POSTED_READS
	/*
	 * Start posted read for interface 7
         */
	lw	a2,PA_SREG_OFS+28(s0)		# get addr for PR7
	sw	a2,PMA_PR7_OFFSET(s5)		# start PR7
#endif	

	/* Do some useful work while posted read is completing */
        jal     vip_drq_consumer
	nop

#ifdef  PCI_POSTED_READS
        lw      s1,PMA_PR7_OFFSET(s5)           # read CSR0 from PR7
#else
        lw      s1,AMDP2_7_CSR0_OFS(s2)         # read csr0
#endif
	lw	s6,(PA_IDB_OFS+28)(s0)		# read idb
	sw	s1,AMDP2_7_CSR0_OFS(s2)		# clear interrupt bits
	andi	s1,s1,AMDP2_INTR_MASK		# get just the intr. bits

	andi	t0,s1,AMDP2_RINT_MASK		# check for rx interrupt
	beq	t0,zero,s_check_7_tx		# is rx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_rx_interrupt		# call rx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_7_tx:
	andi	t0,s1,AMDP2_TINT_MASK		# check for tx interrupt
	beq	t0,zero,s_check_7_errors		# is tx interrupt set ?
	nop					# (bd slot)

	jal	amdp2_tx_interrupt		# call tx interrupt handler
	move	a0,s6				# load up idb (bd slot)

s_check_7_errors:
	andi	t0,s1,AMDP2_ERRORS_MASK		# check for error intrs
	beq	t0,zero,s_amdp2_done		# is error intr set ?
	nop					# (bd slot)

	move	a0,s1				# load up CSR0
	jal	amdp2_error_interrupt		# call error intr. handler
	move	a1,s6				# load up idb (bd slot)

s_amdp2_done:
	lw	s1,4(sp)			# restore return address
	lw	s5,8(sp)			# restore s5
	lw	s6,12(sp)			# restore s6
	addiu	sp,sp,16			# remove stack frame

	/*
	 * Return
	 */
	j	s1				# return
	move	v0,zero				# no more work (bd slot)

	.set	reorder
ENDFRAME(amdp2_eim_wrapper)

#endif /* SVIP */

/* end of file */
