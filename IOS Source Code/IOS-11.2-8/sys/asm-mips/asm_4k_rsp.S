/* $Id: asm_4k_rsp.S,v 3.2.54.3 1996/08/08 21:06:10 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_rsp.S,v $
 *------------------------------------------------------------------
 * asm_4k_rsp.S - RSP specific R4k asm support
 *
 * Chris Shaker, June 15, 1994
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_rsp.S,v $
 * Revision 3.2.54.3  1996/08/08  21:06:10  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.2.54.2  1996/05/21  22:05:30  mdb
 * The 96q1 assembler warns about using the 'at' register for the sw macro
 * even though it is not really used. Surround the sw instructions with a
 * '.set at' and '.set noat' as a workaround. [CSCdi56015].
 *
 * Revision 3.2.54.1  1996/03/18  18:51:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/09  05:03:51  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.24.1  1996/02/20  13:32:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:40:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_rsp.h"
#include "../src-rsp/rsp_pcmap.h"

	.globl	dcl_spurious_count
	.globl	dcl_timer_count
	.globl	dcl_timer_handler
	.sdata
	.align 2

dcl_spurious_count:
	.word	0
dcl_timer_count:
	.word	0
dcl_timer_handler:
	.word	0

/*
 * interrupt_jump_table:
 * Used by the interrupt handler to jump to the appropriate routine.
 * The prioritization of interrupts is done by this table.
 * Note this table is in the .text segment, hence read only.
 *
 *	line	priority	descr
 *	----	--------	-----
 *	0	2		external interrupt controller
 *	1	5		n/c, spurious
 *	2	6		n/c, spurious
 *	3	4		QA normal priority (network)
 *	4	3		QA high priority (command)
 *	5	1		Count/Compare
 */
	.text
	.align 2

	.globl	interrupt_jump_table
 #
 # void ((*interrupt_jump_table)())[64]
 #
	/* TCNSSD = Timer, Cmd_qa, Net_qa, Spurious, Dcl */
				/*             TCNSSD  */
interrupt_jump_table:		/* precedence= 134562  */
	.word	r4k_spur_intr	/* IP [7..2] = 000000, SW or spurious */
	.word	rsp_dcl_intr	/* IP [7..2] = 000001, */
	.word	r4k_spur_intr	/* IP [7..2] = 000010, */
	.word	rsp_dcl_intr	/* IP [7..2] = 000011, */
	.word	r4k_spur_intr	/* IP [7..2] = 000100, */
	.word	rsp_dcl_intr	/* IP [7..2] = 000101, */
	.word	r4k_spur_intr	/* IP [7..2] = 000110, */
	.word	rsp_dcl_intr	/* IP [7..2] = 000111, */
	.word	rsp_qa_intr	/* IP [7..2] = 001000, */
	.word	rsp_dcl_intr	/* IP [7..2] = 001001, */
	.word	rsp_qa_intr	/* IP [7..2] = 001010, */
	.word	rsp_dcl_intr	/* IP [7..2] = 001011, */
	.word	rsp_qa_intr	/* IP [7..2] = 001100, */
	.word	rsp_dcl_intr	/* IP [7..2] = 001101, */
	.word	rsp_qa_intr	/* IP [7..2] = 001110, */
	.word	rsp_dcl_intr	/* IP [7..2] = 001111, */
	.word	rsp_qa_intr_hi	/* IP [7..2] = 010000, */
	.word	rsp_dcl_intr	/* IP [7..2] = 010001, */
	.word	rsp_qa_intr_hi	/* IP [7..2] = 010010, */
	.word	rsp_dcl_intr	/* IP [7..2] = 010011, */
	.word	rsp_qa_intr_hi	/* IP [7..2] = 010100, */
	.word	rsp_dcl_intr	/* IP [7..2] = 010101, */
	.word	rsp_qa_intr_hi	/* IP [7..2] = 010110, */
	.word	rsp_dcl_intr	/* IP [7..2] = 010111, */
	.word	rsp_qa_intr_hi	/* IP [7..2] = 011000, */
	.word	rsp_dcl_intr	/* IP [7..2] = 011001, */
	.word	rsp_qa_intr_hi	/* IP [7..2] = 011010, */
	.word	rsp_dcl_intr	/* IP [7..2] = 011011, */
	.word	rsp_qa_intr_hi	/* IP [7..2] = 011100, */
	.word	rsp_dcl_intr	/* IP [7..2] = 011101, */
	.word	rsp_qa_intr_hi	/* IP [7..2] = 011110, */
	.word	rsp_dcl_intr	/* IP [7..2] = 011111, */

	.word	r4k_nmi_intr	/* IP [7..2] = 0x20, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x21, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x22, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x23, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x24, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x24, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x25, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x26, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x27, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x28, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x29, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x2A, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x2B, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x2C, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x2D, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x2E, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x2F, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x30, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x31, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x32, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x33, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x34, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x34, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x35, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x36, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x37, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x38, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x39, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x3A, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x3B, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x3C, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x3D, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x3E, Timer	*/
	.word	r4k_nmi_intr	/* IP [7..2] = 0x3F, Timer	*/

rsp_external_intr:
	.word	dcl_spurious_intr	# no interrupt requests (spurious)
	.word	dcl_unused_intr		# 0 - QA low priority (unused)
	.word	dcl_unused_intr		# 1 - QA high priority (unused)
	.word	dcl_intr_stall		# 2 - cbus stall interrupt
	.word	dcl_intr_pcmcia		# 3 - PCMCIA controller
	.word	dcl_intr_console	# 4 - console
	.word	dcl_intr_timer		# 5 - DCL timer (used for throttling)
	.word	dcl_intr_error		# 6 - e.g. cbus parity error
	.word	dcl_unused_intr		# 7 - unused

#define EOI_UNUSED 0x83		/* bitmask of DCL vectors to unused wrapper */

	.extern r4k_cpu_level,4
	.extern l1stk,4
	.extern l1cnt,4
	.extern lev1subr,4

/*
 * handle normal priority level QA interrupts
 */
FRAME(rsp_qa_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 1 stack
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l1stk			# move onto special stack
	lw	k0,l1cnt			# load counter
	
	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l1cnt			# save the counter
	
	sd	v0,SP_OFS(REG_V0)(sp)		# save v0
	sd	v1,SP_OFS(REG_V1)(sp)		# save v1
	sd 	a0,SP_OFS(REG_A0)(sp)		# save a0
	sd 	a1,SP_OFS(REG_A1)(sp)		# save a1
	sd 	a2,SP_OFS(REG_A2)(sp)		# save a2
	sd 	a3,SP_OFS(REG_A3)(sp)		# save a3
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3
	sdc1	FPU_R0,SP_OFS(REG_K0)(sp)	# save copy of k0
	sdc1	FPU_R1,SP_OFS(REG_K1)(sp)	# save copy of k1
	sdc1	FPU_R2,SP_OFS(REG_AT)(sp)	# save copy of AT
	dmfc0	t0,CP0_EPC			# get EPC reg
	mfc0	t1,CP0_SREG			# get Status reg
	mflo	t2				# get LO
	mfhi	t3				# get HI
	sd	t0,SP_OFS(REG_EPC)(sp)		# save EPC reg
	sw	t1,SP_OFS(REG_SREG)(sp)		# save Status reg
	sd	t2,SP_OFS(REG_LO)(sp)		# save LO
	sd	t3,SP_OFS(REG_HI)(sp)		# save HI
	sd	t4,SP_OFS(REG_T4)(sp)		# save t4
	sd	t5,SP_OFS(REG_T5)(sp)		# save t5

	/*
	 * Save the interrupted level and install the new level
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	li	t3,(LEVEL1 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t3,r4k_cpu_level		# save new level

	/*
	 * Save the remaining context
	 */
	sd	t6,SP_OFS(REG_T6)(sp)		# save t6
	sd	t7,SP_OFS(REG_T7)(sp)		# save t7
	sd	t8,SP_OFS(REG_T8)(sp)		# save t8
	sd	t9,SP_OFS(REG_T9)(sp)		# save t9
	sd	ra,SP_OFS(REG_RA)(sp)		# save ra

	/*
	 * We lock out this level of interrupt and negate the EXL
	 * bit. This allows higher priority interrupts to then be
	 * serviced. No need to update external interrupt mask
	 * as it does not change from Levels 0,1,2
	 *
	 * Note : There seems to be a problem with the processor when EXL 
	 * gets de-asserted while IE is asserted and there is another
	 * interrupt pending (the interrupt is taken but the EPC register
	 * does not get re-latched). To work arround this, disable IE,
	 * reset the EXL bit, and then re-enable IE.
	 */
	li	t4,~SREG_IE			# get IE mask
	and	t4,t4,t1			# de-assert IE
	mtc0	t4,CP0_SREG			# move to status register

	addiu	t2,zero,LEVEL1_SREG_MASK	# get sreg mask
	lw	t3,lev1subr			# get handler address
	and	t4,t4,t2			# and sreg copy with mask
	mtc0	t4,CP0_SREG			# EXL=0, IE=0

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t1,t1,t2			# and it into sreg
	ori	t1,t1,SREG_IE			# set IE
	mtc0	t1,CP0_SREG			# write back sreg with EXL=0

	jal	t3				# call handler
	nop					# (bd slot)

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	ld	v0,SP_OFS(REG_V0)(sp)		# restore v0
	ld	v1,SP_OFS(REG_V1)(sp)		# restore v1
	ld 	a0,SP_OFS(REG_A0)(sp)		# restore a0
	ld 	a1,SP_OFS(REG_A1)(sp)		# restore a1
	ld 	a2,SP_OFS(REG_A2)(sp)		# restore a2
	ld 	a3,SP_OFS(REG_A3)(sp)		# restore a3

	lw	t5,SP_OFS(REG_SREG)(sp)		# load Status reg
	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0
	ld	t1,SP_OFS(REG_T1)(sp)		# restore t1
	ld	t2,SP_OFS(REG_T2)(sp)		# restore t2
	ld	t3,SP_OFS(REG_T3)(sp)		# restore t3

	mfc0	t6,CP0_SREG			# read sreg
	li	t4,~SREG_IE			# get IE mask

	and	t6,t6,t4			# disable IE bit
	mtc0	t6,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t7,t5,t4			# disable IE in sreg copy
	mtc0	t7,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t5,CP0_SREG			# restore sreg
	nop					# let sreg settle

	ld	t4,SP_OFS(REG_EPC)(sp)		# load EPC reg
	ld	t6,SP_OFS(REG_LO)(sp)		# load LO
	ld	t7,SP_OFS(REG_HI)(sp)		# load HI

	dmtc0	t4,CP0_EPC			# restore EPC reg
	mtlo	t6				# restore LO
	mthi	t7				# restore HI

	ld	t4,SP_OFS(REG_T4)(sp)		# restore t4
	ld	t5,SP_OFS(REG_T5)(sp)		# restore t5
	ld	t6,SP_OFS(REG_T6)(sp)		# restore t6
	ld	t7,SP_OFS(REG_T7)(sp)		# restore t7
	ld	t8,SP_OFS(REG_T8)(sp)		# restore t8
	ld	t9,SP_OFS(REG_T9)(sp)		# restore t9
	ld	ra,SP_OFS(REG_RA)(sp)		# restore ra

	/*
	 * We need to restore the interrupted level
	 * Note : If incorrect set_interrupt_level/reset_interrupt_level
	 * calls were made in the handler, the interrupt mask will
	 * be corrupted and we would have to re-install it here.
	 */
	sw	k0,r4k_cpu_level		# restore old level
	ld	k1,SP_OFS(REG_K1)(sp)		# restore k1
	ld	k0,SP_OFS(REG_K0)(sp)		# restore k0
	ld	AT,SP_OFS(REG_AT)(sp)		# restore AT

	/*
	 * move back onto the original stack
	 * We dont have to increment the stack pointer
	 */
	ld	sp,SP_OFS(REG_SP)(sp)		# restore old sp

	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

	.set	reorder
	.set	at
ENDFRAME(rsp_qa_intr)


	.extern l2stk,4
	.extern l2cnt,4
	.extern lev2subr,4

/*
 * handle high priority QA interrupts
 */
FRAME(rsp_qa_intr_hi,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 2 stack
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l2stk			# move onto special stack
	lw	k0,l2cnt			# load counter
	
	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l2cnt			# save the counter
	
	sd	v0,SP_OFS(REG_V0)(sp)		# save v0
	sd	v1,SP_OFS(REG_V1)(sp)		# save v1
	sd 	a0,SP_OFS(REG_A0)(sp)		# save a0
	sd 	a1,SP_OFS(REG_A1)(sp)		# save a1
	sd 	a2,SP_OFS(REG_A2)(sp)		# save a2
	sd 	a3,SP_OFS(REG_A3)(sp)		# save a3
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3
	sdc1	FPU_R0,SP_OFS(REG_K0)(sp)	# save copy of k0
	sdc1	FPU_R1,SP_OFS(REG_K1)(sp)	# save copy of k1
	sdc1	FPU_R2,SP_OFS(REG_AT)(sp)	# save copy of AT
	dmfc0	t0,CP0_EPC			# get EPC reg
	mfc0	t1,CP0_SREG			# get Status reg
	mflo	t2				# get LO
	mfhi	t3				# get HI
	sd	t0,SP_OFS(REG_EPC)(sp)		# save EPC reg
	sw	t1,SP_OFS(REG_SREG)(sp)		# save Status reg
	sd	t2,SP_OFS(REG_LO)(sp)		# save LO
	sd	t3,SP_OFS(REG_HI)(sp)		# save HI
	sd	t4,SP_OFS(REG_T4)(sp)		# save t4
	sd	t5,SP_OFS(REG_T5)(sp)		# save t5

	/*
	 * Save the interrupted level and install the new level
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	li	t3,(LEVEL2 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t3,r4k_cpu_level		# save new level

	/*
	 * Save the remaining context
	 */
	sd	t6,SP_OFS(REG_T6)(sp)		# save t6
	sd	t7,SP_OFS(REG_T7)(sp)		# save t7
	sd	t8,SP_OFS(REG_T8)(sp)		# save t8
	sd	t9,SP_OFS(REG_T9)(sp)		# save t9
	sd	ra,SP_OFS(REG_RA)(sp)		# save ra

	/*
	 * We lock out this level of interrupt and negate the EXL
	 * bit. This allows higher priority interrupts to then be
	 * serviced. No need to update external interrupt mask
	 * as it does not change from Levels 0,1,2
	 *
	 * Note : There seems to be a problem with the processor when EXL 
	 * gets de-asserted while IE is asserted and there is another
	 * interrupt pending (the interrupt is taken but the EPC register
	 * does not get re-latched). To work arround this, disable IE,
	 * reset the EXL bit, and then re-enable IE.
	 */
	li	t4,~SREG_IE			# get IE mask
	and	t4,t4,t1			# de-assert IE
	mtc0	t4,CP0_SREG			# move to status register

	addiu	t2,zero,LEVEL2_SREG_MASK	# get sreg mask
	lw	t3,lev2subr			# get handler address
	and	t4,t4,t2			# and sreg copy with mask
	mtc0	t4,CP0_SREG			# EXL=0, IE=0

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t1,t1,t2			# and it into sreg
	ori	t1,t1,SREG_IE			# set IE
	mtc0	t1,CP0_SREG			# write back sreg with EXL=0

	jal	t3				# call handler
	nop					# (bd slot)

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	ld	v0,SP_OFS(REG_V0)(sp)		# restore v0
	ld	v1,SP_OFS(REG_V1)(sp)		# restore v1
	ld 	a0,SP_OFS(REG_A0)(sp)		# restore a0
	ld 	a1,SP_OFS(REG_A1)(sp)		# restore a1
	ld 	a2,SP_OFS(REG_A2)(sp)		# restore a2
	ld 	a3,SP_OFS(REG_A3)(sp)		# restore a3

	lw	t5,SP_OFS(REG_SREG)(sp)		# load Status reg
	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0
	ld	t1,SP_OFS(REG_T1)(sp)		# restore t1
	ld	t2,SP_OFS(REG_T2)(sp)		# restore t2
	ld	t3,SP_OFS(REG_T3)(sp)		# restore t3

	mfc0	t6,CP0_SREG			# read sreg
	li	t4,~SREG_IE			# get IE mask

	and	t6,t6,t4			# disable IE bit
	mtc0	t6,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t7,t5,t4			# disable IE in sreg copy
	mtc0	t7,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t5,CP0_SREG			# restore sreg
	nop					# let sreg settle

	ld	t4,SP_OFS(REG_EPC)(sp)		# load EPC reg
	ld	t6,SP_OFS(REG_LO)(sp)		# load LO
	ld	t7,SP_OFS(REG_HI)(sp)		# load HI

	dmtc0	t4,CP0_EPC			# restore EPC reg
	mtlo	t6				# restore LO
	mthi	t7				# restore HI

	ld	t4,SP_OFS(REG_T4)(sp)		# restore t4
	ld	t5,SP_OFS(REG_T5)(sp)		# restore t5
	ld	t6,SP_OFS(REG_T6)(sp)		# restore t6
	ld	t7,SP_OFS(REG_T7)(sp)		# restore t7
	ld	t8,SP_OFS(REG_T8)(sp)		# restore t8
	ld	t9,SP_OFS(REG_T9)(sp)		# restore t9
	ld	ra,SP_OFS(REG_RA)(sp)		# restore ra

	/*
	 * We need to restore the interrupted level
	 * Note : If incorrect set_interrupt_level/reset_interrupt_level
	 * calls were made in the handler, the interrupt mask will
	 * be corrupted and we would have to re-install it here.
	 */
	sw	k0,r4k_cpu_level		# restore old level
	ld	k1,SP_OFS(REG_K1)(sp)		# restore k1
	ld	k0,SP_OFS(REG_K0)(sp)		# restore k0
	ld	AT,SP_OFS(REG_AT)(sp)		# restore AT

	/*
	 * move back onto the original stack
	 * We dont have to increment the stack pointer
	 */
	ld	sp,SP_OFS(REG_SP)(sp)		# restore old sp

	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

	.set	reorder
	.set	at
ENDFRAME(rsp_qa_intr_hi)

/*
 * rsp_dcl_intr
 * Handle DCL interrupts
 */
FRAME(rsp_dcl_intr,sp,0,ra)
	.set	noreorder
	.set	noat
					# note: dcl_unused_intr expects the
					#   vector table offset in k0
	lbu	k0,ADRSPC_INTVALUE	# read the interrupt vector
	la	k1,rsp_external_intr	# get table address
	addu	k1,k1,k0		# get table entry address
	lw	k1,0(k1)		# read the handler address
	nop				# some R4000 do not interlock !

	j	k1			# jump to the handler
	nop				# (bd slot)

	.set	reorder
	.set	at
ENDFRAME(rsp_dcl_intr)

/*
 * handle phantom interrupt
 */
FRAME(dcl_spurious_intr,sp,0,ra)
	.set	noreorder
	.set	noat
	lw	k1,dcl_spurious_count	# load previous count
	dmfc1	AT,FPU_R2		# restore AT

	addiu	k1,k1,1			# increment the count
	sw	k1,dcl_spurious_count	# store the count back

	dmfc1	k0,FPU_R0		# restore k0
	dmfc1	k1,FPU_R1		# restore k1

	eret				# return
	nop				# does not get executed
	.set	reorder
	.set	at
ENDFRAME(dcl_spurious_intr)

/*
 * handle unused interrupt vectors (crash)
 */
FRAME(dcl_unused_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the present stack. It calls the unused
	 * interrupt driver which will crash the system. Before
	 * calling the handler, install Level 7 priorities.
	 * Note: the vector that got us here is in k0
	 */
	move	a0,k0				# retrieve interrupt vector

	li	t1,(LEVEL7 << R4K_LEVEL_SHIFT)	# get Level 7
	sw	t1,r4k_cpu_level		# set Level 7
	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(2*4*LEVEL7))(t2)		# read new mask value

	la	t0,ADRSPC_INTMASK		# get interrupt mask reg addr
	sb	t1,0(t0)			# write interrupt mask

	li	t3,EOI_UNUSED			# get EOI value
	la	t0,ADRSPC_INTEND		# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	mfc0	a1,CP0_SREG			# read status register
	lw	t3,56(t2)			# read new IP bits for Level 7

	li	t4,~SREG_IE			# get IE mask
	and	t4,a1,t4			# clear IE
	mtc0	t4,CP0_SREG			# write status register
	nop					# let it settle
		
	li	t5,~(CAUSE_IMASK|SREG_EXL)	# get IP[7..0]|SREG_EXL mask
	and 	t5,t4,t5			# clear IP[7..0],EXL
	or	t5,t5,t3			# insert Level 7 IP bits
	mtc0	t5,CP0_SREG			# write status register
	nop					# let it settle

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	ori	t5,t5,SREG_IE			# enable IE
	mtc0	t5,CP0_SREG			# write status register
	nop					# let it settle

	la      t2,ADRSPC_INTMASK		# get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
						# sync the posted writes)

	jal	r4k_unused_handler		# call unused handler
	nop					# (bd slot)

	/*
	 * The unused handler should never return. If it does,
	 * call crashdump(0)
	 */
	jal	crashdump			# call crashdump
	move	a0,zero				# (bd slot)

	.set	reorder
	.set	at
ENDFRAME(dcl_unused_intr)

	.extern l3stk,4
	.extern l3cnt,4
	.extern lev3subr,4

/*
 * Handle a stall interrupt
 */
FRAME(dcl_intr_stall,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 3 stack
	 * It dereferences off lev3subr to the wrapper/driver
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l3stk			# move onto correct stack
	lw	k0,l3cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l3cnt			# save the counter

	/*
	 * Save some working registers
	 */
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL3 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(2*4*LEVEL3))(t2)		# read new mask value
	la	t0,ADRSPC_INTMASK		# get interrupt mask reg addr
	sb	t1,0(t0)			# write interrupt mask
	li	t3,(1<<LEVEL2)			# get EOI value - DCL level
	la	t0,ADRSPC_INTEND		# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	la      t2,ADRSPC_INTMASK		# get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
        	                     		# sync the posted writes)

	/*
	 * Load handler address into AT and call the dispatch routine
	 */
	j	r4k_intr_dispatch		# call dispatcher
	lw	AT,lev3subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(dcl_intr_stall)

/*
 * Handle a DCL timer interrupt
 */
FRAME(dcl_intr_timer,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * share level 3 stack with OIR interrupt handler
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l3stk			# move onto level 1 stack
	lw	k0,dcl_timer_count		# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,dcl_timer_count		# save the counter

	/*
	 * Save some working registers
	 */
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL3 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

 	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(2*4*LEVEL3))(t2)		# read new mask value
	la	t0,ADRSPC_INTMASK		# get interrupt mask reg addr
	sb	t1,0(t0)			# write interrupt mask
	li	t3,(1<<LEVEL5)			# get EOI value - DCL level
	la	t0,ADRSPC_INTEND		# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	la      t2,ADRSPC_INTMASK		# get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
        	                     		# sync the posted writes)

	/*
	 * Load lev5subr into AT and call the dispatch routine
	 */
	j	r4k_intr_dispatch		# call dispatcher
	lw	AT,dcl_timer_handler		# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(dcl_intr_timer)


	.extern l5stk,4
	.extern l5cnt,4
	.extern lev5subr,4

/*
 * Handle a console/aux interrupt
 */
FRAME(dcl_intr_console,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 5 stack
	 * It dereferences off lev5subr to the wrapper/driver
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l5stk			# move onto level 5 stack
	lw	k0,l5cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l5cnt			# save the counter

	/*
	 * Save some working registers
	 */
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL5 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(2*4*LEVEL5))(t2)		# read new mask value
	la	t0,ADRSPC_INTMASK		# get interrupt mask reg addr
	sb	t1,0(t0)			# write interrupt mask
	li	t3,(1<<LEVEL4)			# get EOI value - DCL level
	la	t0,ADRSPC_INTEND		# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	la      t2,ADRSPC_INTMASK		# get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
        	                     		# sync the posted writes)

	/*
	 * Load lev5subr into AT and call the dispatch routine
	 */
	j	r4k_intr_dispatch		# call dispatcher
	lw	AT,lev5subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(dcl_intr_console)


	.extern l4stk,4
	.extern l4cnt,4
	.extern lev4subr,4

/*
 * Handle a PCMCIA interrupt
 */
FRAME(dcl_intr_pcmcia,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 4 stack
	 * It dereferences off lev4subr to the wrapper/driver
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l4stk			# move onto level 5 stack
	lw	k0,l4cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l4cnt			# save the counter

	/*
	 * Save some working registers
	 */
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL4 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(2*4*LEVEL4))(t2)		# read new mask value
	la	t0,ADRSPC_INTMASK		# get interrupt mask reg addr
	sb	t1,0(t0)			# write interrupt mask
	li	t3,(1<<LEVEL3)			# get EOI value - DCL level
	la	t0,ADRSPC_INTEND		# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	la      t2,ADRSPC_INTMASK		# get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
        	                     		# sync the posted writes)

	/*
	 * Load lev4subr into AT and call the dispatch routine
	 */
	j	r4k_intr_dispatch		# call dispatcher
	lw	AT,lev4subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(dcl_intr_pcmcia)


	.extern l6stk,4
	.extern l6cnt,4
	.extern lev6subr,4

/*
 * r4k_lvl6_intr:
 * Handle a Level 6 Interrupt (Errors)
 */
FRAME(dcl_intr_error,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 6 stack
	 * It dereferences off lev6subr to the wrapper/driver
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l6stk			# move onto level 6 stack
	lw	k0,l6cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l6cnt			# save the counter

	/*
	 * Save some working registers
	 */
	sd	t0,SP_OFS(REG_T0)(sp)		# save t0
	sd	t1,SP_OFS(REG_T1)(sp)		# save t1
	sd	t2,SP_OFS(REG_T2)(sp)		# save t2
	sd	t3,SP_OFS(REG_T3)(sp)		# save t3

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL6 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(2*4*LEVEL6))(t2)		# read new mask value
	la	t0,ADRSPC_INTMASK		# get interrupt mask reg addr
	sb	t1,0(t0)			# write interrupt mask
	li	t3,(1<<LEVEL6)			# get EOI value - DCL level
	la	t0,ADRSPC_INTEND		# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	la      t2,ADRSPC_INTMASK		# get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
						# sync the posted writes)

	/*
	 * Load lev6subr into AT and call the dispatch routine
	 */
	j	r4k_intr_dispatch		# call dispatcher
	lw	AT,lev6subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(dcl_intr_error)

/*
 * r4k_intr_dispatch:
 * Save remaining context and call the handler. Re-install the old level,
 * the context and dismiss the interrupt handler. This is a general purpose
 * dispatch for interrupts serviced through the external interrupt controller.
 *
 * Note : t0-t3 have already been saved.
 *	  Stack has been setup with a frame and old sp is stacked
 * 	  Old level is in k0.
 *	  External interrupt controller has been handled
 *	  EXL Bit is still asserted
 *	  AT has address of wrapper/driver routine
 */
FRAME(r4k_intr_dispatch,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Save context
	 */
	sd	v0,SP_OFS(REG_V0)(sp)		# save v0
	sd	v1,SP_OFS(REG_V1)(sp)		# save v1
	sd 	a0,SP_OFS(REG_A0)(sp)		# save a0
	sd 	a1,SP_OFS(REG_A1)(sp)		# save a1
	sd 	a2,SP_OFS(REG_A2)(sp)		# save a2
	sd 	a3,SP_OFS(REG_A3)(sp)		# save a3
	sdc1	FPU_R0,SP_OFS(REG_K0)(sp)	# save copy of k0
	sdc1	FPU_R1,SP_OFS(REG_K1)(sp)	# save copy of k1
	sdc1	FPU_R2,SP_OFS(REG_AT)(sp)	# save copy of AT
	dmfc0	t0,CP0_EPC			# get EPC reg
	mfc0	t1,CP0_SREG			# get Status reg
	mflo	t2				# get LO
	mfhi	t3				# get HI
	sd	t0,SP_OFS(REG_EPC)(sp)		# save EPC reg
	sw	t1,SP_OFS(REG_SREG)(sp)		# save Status reg
	sd	t2,SP_OFS(REG_LO)(sp)		# save LO
	sd	t3,SP_OFS(REG_HI)(sp)		# save HI
	sd	t4,SP_OFS(REG_T4)(sp)		# save t4
	sd	t5,SP_OFS(REG_T5)(sp)		# save t5
	sd	t6,SP_OFS(REG_T6)(sp)		# save t6
	sd	t7,SP_OFS(REG_T7)(sp)		# save t7
	sd	t8,SP_OFS(REG_T8)(sp)		# save t8
	sd	t9,SP_OFS(REG_T9)(sp)		# save t9
	sd	ra,SP_OFS(REG_RA)(sp)		# save ra

	/*
	 * Setup the status register and call the handler
	 * We want to mask IP[2..6] IE DCL, QA hi & low
	 *
	 * Note : There seems to be a problem with the processor when EXL 
	 * gets de-asserted while IE is asserted and there is another
	 * interrupt pending (the interrupt is taken but the EPC register
	 * does not get re-latched). To work arround this, disable IE,
	 * reset the EXL bit, and then re-enable IE.
	 */
	li	t3,~SREG_IE			# get IE mask
	and	t3,t3,t1			# de-assert IE
	mtc0	t3,CP0_SREG			# move to status register

	li	t2,~(CAUSE_IP23456|SREG_EXL)	# get new mask
	and	t3,t3,t2			# and it into sreg copy
	mtc0	t3,CP0_SREG			# clear EXL (IE disabled)

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t1,t1,t2			# and it into sreg
	ori	t1,t1,SREG_IE			# set IE
	mtc0	t1,CP0_SREG			# write back sreg with EXL=0

	jal	AT		# call handler/driver
	move	a0,sp		# setup param 1. This gives error handling
				# interrupts (parity, wberr) a context ptr

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	ld	v0,SP_OFS(REG_V0)(sp)		# restore v0
	ld	v1,SP_OFS(REG_V1)(sp)		# restore v1
	ld 	a0,SP_OFS(REG_A0)(sp)		# restore a0
	ld 	a1,SP_OFS(REG_A1)(sp)		# restore a1
	ld 	a2,SP_OFS(REG_A2)(sp)		# restore a2
	ld 	a3,SP_OFS(REG_A3)(sp)		# restore a3

	lw	t5,SP_OFS(REG_SREG)(sp)		# load Status reg
	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0

	mfc0	t6,CP0_SREG			# read sreg
	li	t4,~SREG_IE			# get IE mask

	and	t6,t6,t4			# disable IE bit
	mtc0	t6,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t7,t5,t4			# disable IE in sreg copy
	mtc0	t7,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t5,CP0_SREG			# restore sreg (EXL=1)
	nop					# let sreg settle

	ld	t1,SP_OFS(REG_T1)(sp)		# restore t1
	ld	t2,SP_OFS(REG_T2)(sp)		# restore t2
	ld	t3,SP_OFS(REG_T3)(sp)		# restore t3
	ld	t4,SP_OFS(REG_EPC)(sp)		# load EPC reg
	ld	t6,SP_OFS(REG_LO)(sp)		# load LO
	ld	t7,SP_OFS(REG_HI)(sp)		# load HI

	dmtc0	t4,CP0_EPC			# restore EPC reg
	mtlo	t6				# restore LO
	mthi	t7				# restore HI

	/*
	 * We need to restore the interrupted level (both the
	 * status register and the external mask)
	 */
	sw	k0,r4k_cpu_level		# restore old level
	la	k1,r4k_level_table		# get table address
	addu	k1,k1,k0			# get address
	lbu	k0,4(k1)			# read intr mask
	la	t9,ADRSPC_INTMASK		# get mask reg addr
	sb	k0,0(t9)			# write new mask

	lbu	t8,0(t9)			# sync the write

	/*
	 * Restore remaining context
	 */
	ld	t4,SP_OFS(REG_T4)(sp)		# restore t4
	ld	t5,SP_OFS(REG_T5)(sp)		# restore t5
	ld	t6,SP_OFS(REG_T6)(sp)		# restore t6
	ld	t7,SP_OFS(REG_T7)(sp)		# restore t7
	ld	t8,SP_OFS(REG_T8)(sp)		# restore t8
	ld	t9,SP_OFS(REG_T9)(sp)		# restore t9
	ld	ra,SP_OFS(REG_RA)(sp)		# restore ra

	ld	k1,SP_OFS(REG_K1)(sp)		# restore k1
	ld	k0,SP_OFS(REG_K0)(sp)		# restore k0
	ld	AT,SP_OFS(REG_AT)(sp)		# restore AT

	/*
	 * move back onto the original stack
	 * We dont have to increment the stack pointer
	 */
	ld	sp,SP_OFS(REG_SP)(sp)		# restore old sp

	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

	.set	reorder
	.set	at
ENDFRAME(r4k_intr_dispatch)

/*
 * Bits in R4600 status/cause regs	Bits in DCL regs
 * -------------------------------	----------------
 *	IP 7	- Internal Timer	DCL bit 7 : n/c
 *	IP 6	- QA high		DCL bit 6 : watch/addr/wr_bus error
 *	IP 5	- QA normal		DCL bit 5 : DCL counters
 *	IP 4	- n/c			DCL bit 4 : DCL DUART, console/AUX
 *	IP 3	- n/c			DCL bit 3 : PCMCIA controller
 *	IP 2 	- DCL			DCL bit 2 : cbus stall
 *	IP 1	- S/W 1			DCL bit 1 : QA high
 *	IP 0	- S/W 0			DCL bit 0 : QA normal
 *
 *			0 : Masked	1 : Masked
 *			1 : Enabled	0 : Enabled
 *
 *	Level		Status Reg	DCL Mask
 *			IP 7..0		Bit 7..0
 *	---------------------------------------------
 *	0		11100111 e7	10000011 83	process level
 *	1		11000111 c7	10000011 83	QA low priority
 *	2		10000111 87	10000011 83	QA high priority
 *	3		10000111 87	10100111 a7	DCL stall & timer
 *	4		10000111 87	10101111 af	DCL pcmcia
 *	5		10000111 87	10111111 bf	DCL console
 *	6		10000111 87	11111111 ff	DCL error
 *	7		10000000 80	11111111 ff	internal count/compare
 *
 * Note : It is the responsibility of each interrupt wrapper to 
 *	  install the approriate "level", and to re-install the
 *	  old level when exiting. A lower level interrupter must
 *	  never be able to interrupt a higher level handler.
 */

 #
 # struct levels {
 #	ulong r4600_statusreg;		/* "1" bits enable interrupts */
 #	uchar dcl_mask;			/* "1" bits mask interrupts */
 #	uchar :24;
 # } level_table[8];
 #
	.globl	r4k_level_table

	.text
	.align	3
r4k_level_table:
	.word	0x0000e700, 0x83000000		# Level 0
	.word	0x0000c700, 0x83000000		# Level 1
	.word	0x00008700, 0x83000000		# Level 2
	.word	0x00008700, 0xa7000000		# Level 3
	.word	0x00008700, 0xaf000000		# Level 4
	.word	0x00008700, 0xbf000000		# Level 5
	.word	0x00008700, 0xff000000		# Level 6
	.word	0x00008000, 0xff000000		# Level 7
