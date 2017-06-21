/* $Id: asm_4k_c4000.S,v 3.2.60.1 1996/08/08 21:06:03 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_c4000.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000.S - Sierra specific R4k asm support
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000.S,v $
 * Revision 3.2.60.1  1996/08/08  21:06:03  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:40:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:48:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * interrupt_jump_table:
 * Used by the interrupt handler to jump to the appropriate routine.
 * The prioritization of interrupts is done by this table.
 * Note this table is in the .text segment, hence read only.
 *
 *	Nevada    - INTR 0
 *	NIM 0 Lo  - INTR 1
 *	NIM 1 Lo  - INTR 2
 *	NIM 2 Lo  - INTR 3
 *	NIM Hi    - INTR 4
 *	Timer     - INTR 5 (Count/Compare timer)
 */ 
	.text
	.align 2

	.globl	interrupt_jump_table

 #
 # void ((*interrupt_jump_table)())[64]
 #
interrupt_jump_table:
	.word	r4k_spur_intr	/* IP [7..2] = 0x00, S/W Int	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x01, Nevada	*/
	.word	r4k_intr_lo0	/* IP [7..2] = 0x02, NIM 0 Lo	*/ 
	.word	r4k_io_intr	/* IP [7..2] = 0x03, Nevada	*/
	.word	r4k_intr_lo1	/* IP [7..2] = 0x04, NIM 1 Lo	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x05, Nevada 	*/
	.word	r4k_intr_lo1	/* IP [7..2] = 0x06, NIM 0/1 Lo	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x07, Nevada	*/
	.word	r4k_intr_lo2	/* IP [7..2] = 0x08, NIM 2 Lo	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x09, Nevada	*/
	.word	r4k_intr_lo2	/* IP [7..2] = 0x0A, NIM 0/2 Lo	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x0B, Nevada	*/
	.word	r4k_intr_lo1	/* IP [7..2] = 0x0C, NIM 1/2 Lo	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x0D, Nevada	*/
	.word	r4k_intr_lo0	/* IP [7..2] = 0x0E, NIM 0/1/2Lo*/
	.word	r4k_io_intr	/* IP [7..2] = 0x0F, Nevada	*/
	.word	r4k_intr_hi	/* IP [7..2] = 0x10, NIM Hi	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x11, Nevada	*/
	.word	r4k_intr_hi	/* IP [7..2] = 0x12, NIM Hi	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x13, Nevada	*/
	.word	r4k_intr_hi	/* IP [7..2] = 0x14, NIM Hi	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x15, Nevada	*/
	.word	r4k_intr_hi	/* IP [7..2] = 0x16, NIM Hi	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x17, Nevada	*/
	.word	r4k_intr_hi	/* IP [7..2] = 0x18, NIM Hi	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x19, Nevada	*/
	.word	r4k_intr_hi	/* IP [7..2] = 0x1A, NIM Hi	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x1B, Nevada	*/
	.word	r4k_intr_hi	/* IP [7..2] = 0x1C, NIM Hi	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x1D, Nevada	*/
	.word	r4k_intr_hi	/* IP [7..2] = 0x1E, NIM Hi	*/
	.word	r4k_io_intr	/* IP [7..2] = 0x1F, Nevada	*/

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

	.text
	.align 2

r4k_external_intr:
	.word	r4k_unused_lvl		# no interrupt requests
	.word	r4k_unused_lvl		# level 0
	.word	r4k_unused_lvl		# level 1
	.word	r4k_unused_lvl		# level 2
	.word	r4k_unused_lvl		# level 3
	.word	r4k_lvl4_intr		# console
	.word	r4k_unused_lvl		# external timer
	.word	r4k_lvl6_intr		# parity error
	.word	r4k_lvl7_intr		# write bus error

/*
 * defined some externals to allow gas to use gp relative addressing
 */
	.extern	r4k_cpu_level 4
	.extern l2stk,4
	.extern l2cnt,4
	.extern lev2subr,4
	.extern l3stk,4
	.extern l3cnt,4
	.extern lev3subr,4
	.extern l4stk,4
	.extern l4cnt,4
	.extern lev4subr,4
	.extern l6stk,4
	.extern l6cnt,4
	.extern lev6subr,4

/*
 * Round robin control variable
 */
	.sdata
	.align 2

lvl2_rr_index:
	.word 	0

/*
 * r4k_io_intr :
 * Handle Nevada interrupts : Write Bus Error, Parity Error,
 * 	Timer, Console
 *	
 *	Level		Interrupt
 *	-----		----------------------------
 *	7		Write Bus Error
 *	6		Shared Memory Parity error
 *	5		Timer Interrupt (not used through Nevada)
 *	4		Console port
 *	3		NIM Hi (not used through Nevada)
 *	2		NIM Lo (not used through Nevada)
 *	1		Unused
 *	0		Unused
 */
FRAME(r4k_io_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Note : This could be done faster by jumping
	 * through the intstacks data structure in stacks.c
	 * but changes in the C code will break things, so we
	 * do it this way.
	 *
	 * Read Nevada to get the vector and dereference through
	 * the external jump table
	 */
	lbu	k0,IO_IVECT_REG		# read the vector
	la	k1,r4k_external_intr	# get table address
	addu	k1,k1,k0		# get table entry address
	lw	k1,0(k1)		# read the handler address
	nop				# some R4000 do not interlock !

	j	k1			# jump to the handler
	nop				# (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_io_intr)

/*
 * r4k_lvl4_intr:
 * Handle a Level 4 Interrupt (Console etc)
 */
FRAME(r4k_lvl4_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 4 stack
	 * It dereferences off lev4subr to the wrapper/driver
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l4stk			# move onto level 4 stack
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
	lbu	t1,36(t2)			# read new mask value
	la	t0,IO_IMASK_REG			# get interrupt mask reg addr
	sb	t1,0(t0)			# write nevada mask
	li	t3,(1<<LEVEL4)			# get EOI value
	la	t0,IO_IEOI_REG			# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	la      t2,IO_IMASK_REG                 # get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
        	                     		# sync the posted writes)

	/*
	 * Load lev4subr into AT and call the dispatch routine
	 */
	j	r4k_intr_dispatch		# call dispatcher
	lw	AT,lev4subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_lvl4_intr)

/*
 * r4k_lvl6_intr:
 * Handle a Level 6 Interrupt (Errors)
 */
FRAME(r4k_lvl6_intr,sp,0,ra)
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
	lbu	t1,52(t2)			# read new mask value
	la	t0,IO_IMASK_REG			# get interrupt mask reg addr
	sb	t1,0(t0)			# write nevada mask
	li	t3,(1<<LEVEL6)			# get EOI value
	la	t0,IO_IEOI_REG			# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	la      t2,IO_IMASK_REG                 # get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
						# sync the posted writes)

	/*
	 * Load lev6subr into AT and call the dispatch routine
	 */
	j	r4k_intr_dispatch		# call dispatcher
	lw	AT,lev6subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_lvl6_intr)

/*
 * r4k_lvl7_intr:
 *
 * Handle a Level 7 Interrupt (Write bus error)
 */

FRAME(r4k_lvl7_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This is fatal. If we are not on it already, move onto the 
	 * exception stack, save the context into the context save area 
	 * and dispatch the signal handling routine.
	 *
	 * Note: Allthough this is an interrupt, we treat
	 *       it as a gdb debuggable exception. sp has already
	 * 	 been copied to AT
	 */
	la	k0,exception_stack_guard	# get guard address
	subu	k0,AT,k0			# k0 = AT - k0
	blez	k0,move_stack5			# must we move stacks ?
	nop					# (bd slot)

	la	k0,exception_stack		# get stack address
	subu	k0,k0,AT			# k0 = k0 - AT
	blez	k0,move_stack5			# must we move stacks ?
	nop					# (bd slot)

	b	nomove5				# do not move the stack
	nop					# (bd slot)

move_stack5:
	la	sp,exception_stack		# get new stack pointer

nomove5:
	la	k0,r4k_context_space		# get context space
	sd	AT,SP_OFS(REG_SP)(k0)		# save old sp

	/*
	 * Setup parameters for the dispatcher
	 */
	ori	k1,zero,SIGWBERR		# Write Bus Err signal
	
	j	r4k_sig_dispatch		# call dispatcher
	move	AT,zero				# setup code of zero (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_lvl7_intr)

/*
 * r4k_unused_lvl:
 * Handle a case where Nevada reports an interrupt vector
 * that is unused. We treat this as a gross hardware failure
 * allthough it could be considered a spurious interrupt.
 */
FRAME(r4k_unused_lvl,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the present stack. It calls the unused
	 * interrupt driver which will crash the system. Before
	 * calling the handler, install Level 7 priorities.
	 * Note: Unused interrupts are 0..3 and 5
	 */
	lbu	a0,IO_IVECT_REG			# read interrupt vector

	li	t1,(LEVEL7 << R4K_LEVEL_SHIFT)	# get Level 7
	sw	t1,r4k_cpu_level		# set Level 7
	la	t2,r4k_level_table		# get level table address
	lbu	t1,60(t2)			# read new mask value

	la	t0,IO_IMASK_REG			# get interrupt mask reg addr
	sb	t1,0(t0)			# write nevada mask

	li	t3,0x2f				# get EOI value (Level 0..3,5)
	la	t0,IO_IEOI_REG			# get EOI reg addr
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

	la      t2,IO_IMASK_REG                 # get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
						# sync the posted writes)

	jal	r4k_unused_handler		# call unused handler
	nop					# (bd slot)

	/*
	 * The unsued handler should never return. If it does,
	 * call crashdump(0)
	 */
	jal	crashdump			# call crashdump
	move	a0,zero				# (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_unused_lvl)

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
	 * We want to mask IP[2..6] IE Nevada, NIM Hi and Lo interrupts.
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
	lbu	k0,4(k1)			# read nevada intr mask
	la	t9,IO_IMASK_REG			# get mask reg addr
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
	.set	noat
ENDFRAME(r4k_intr_dispatch)

/*
 * r4k_intr_hi :
 * Handle NIM Hi interrupt from all NIMs. Level 3.
 */
FRAME(r4k_intr_hi,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 3 stack
	 * It calls the fddi wrapper
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l3stk			# move onto level 3 stack
	lw	k0,l3cnt			# load counter
	
	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l3cnt			# save the counter
	
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
	li	t3,(LEVEL3 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
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
	 * serviced. No need to update Nevada as it does not change 
	 * from Levels 0,1,2 to Level 3
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

	addiu	t2,zero,LEVEL3_SREG_MASK	# get sreg mask
	lw	t3,lev3subr			# get handler address
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

	jal	t3				# call level 3 handler
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
	 * calls were made in the handler, the Nevada mask will
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

	.set 	reorder
	.set	noat
ENDFRAME(r4k_intr_hi)

/*
 * r4k_intr_lo0 :
 * Handle NIM Lo interrupt from NIM 0 only
 *
 * Entry : 
 */
FRAME(r4k_intr_lo0,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Move onto level2 stack, save RA in FPU and call 
	 * the context save routine (sp has already been copied to AT)
	 */	
	dmtc1	ra,FPU_RA_REG			# save ra
	sw	zero,lvl2_rr_index		# setup round robin index (0)
	lw	sp,l2stk			# move onto level 2 stack
	la	ra,slot0_entry			# setup return address for
						# save routine
	j	r4k_lvl2_context_save		# call save routine
	nop					# (bd slot)

	.set 	reorder
	.set	noat
ENDFRAME(r4k_intr_lo0)

/*
 * r4k_intr_lo1 :
 * Handle NIM Lo interrupt from NIM 1 only
 */
FRAME(r4k_intr_lo1,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Move onto level2 stack, save RA in FPU and call 
	 * the context save routine (sp has already been copied to AT)
	 */	
	dmtc1	ra,FPU_RA_REG			# save ra
	li	k0,1				# get round robin index (1)
	sw	k0,lvl2_rr_index		# setup round robin index
	la	ra,slot1_entry			# setup return address for
						# save routine
	j	r4k_lvl2_context_save		# call save routine
	lw	sp,l2stk			# move onto level 2 stack (bd)

	.set 	reorder
	.set	noat
ENDFRAME(r4k_intr_lo1)

/*
 * r4k_intr_lo2 :
 * Handle NIM Lo interrupt from NIM 2 only
 */
FRAME(r4k_intr_lo2,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Move onto level2 stack, save RA in FPU and call 
	 * the context save routine (sp has already been copied to AT)
	 */	
	dmtc1	ra,FPU_RA_REG			# save ra
	li	k0,2				# get round robin index (2)
	sw	k0,lvl2_rr_index		# setup round robin index
	la	ra,slot2_entry			# setup return address for
						# save routine
	j	r4k_lvl2_context_save		# call save routine
	lw	sp,l2stk			# move onto level 2 stack (bd)

	.set 	reorder
	.set	noat
ENDFRAME(r4k_intr_lo2)

/*
 * r4k_lvl2_round_robin
 * Round robin scheduler for level 2 (network IO) interrupts
 *
 * When a network IO interrupt occurs, we return from the context save
 * routines to one of the slot entry points. After servicing that
 * interrupt we will loop arround all the remaining interfaces
 * polling for interrupts (from the Cause register) until we arrive
 * back at the entry interface and that interface does not have an 
 * interrupt outstanding.
 */
FRAME(r4k_lvl2_round_robin,sp,0,ra)
	.set	noreorder
	.set	noat

slot0_loop:
	mfc0	s0,CP0_CAUSE			# read the Cause register
	li	s1,CAUSE_IBIT3			# load slot 0 mask
	and	s0,s0,s1			# check IP3 (slot 0 IRQ)

	bne	s0,zero,slot0_entry		# slot 0 have an interrupt ?
	lw	s1,lvl2_rr_index		# read rr index (bd)

	bne	s1,zero,slot1_loop		# the terminating slot ?
	nop					# (bd slot)

	/*
	 * Restore context and dismiss the interrupt
	 */
	j	r4k_lvl2_dismiss		# call general purpose dismiss
	nop					# (bd slot)
	
slot0_entry:
	/*
	 * Setup wrapper parameters and call it
	 * 	s0 - &(lev2_jumptable[slot])
	 * 	s1 - return address
	 * 	s2 - IACK register address
	 * 	a0 - Slot number
	 * 	a1 - Slot base address
	 */
	la	s0,lev2_jumptable		# get jump table address
	lw	t0,LEV2_WRAPPER_OFS(s0)		# get wrapper address
	lw	s2,LEV2_IACK_OFS(s0)		# get IACK reg address
	lw	a1,LEV2_BASE_OFS(s0)		# get slot base address

	jal	s1,t0				# call the wrapper
	move	a0,zero				# get slot no. (bd slot)

slot1_loop:
	mfc0	s0,CP0_CAUSE			# read the Cause register
	li	s1,CAUSE_IBIT4			# load slot 1 mask
	and	s0,s0,s1			# check IP4 (slot 1 IRQ)

	bne	s0,zero,slot1_entry		# slot 1 have an interrupt ?
	lw	s1,lvl2_rr_index		# read rr index (bd)

	li	s0,1				# load slot number
	bne	s1,s0,slot2_loop		# the terminating slot ?
	nop					# (bd slot)

	/*
	 * Restore context and dismiss the interrupt
	 */
	j	r4k_lvl2_dismiss		# call general purpose dismiss
	nop					# (bd slot)

slot1_entry:
	/*
	 * Setup wrapper parameters and call it
	 * 	s0 - &(lev2_jumptable[slot])
	 * 	s1 - return address
	 * 	s2 - IACK register address
	 * 	a0 - Slot number
	 * 	a1 - Slot base address
	 */
	la	s0,lev2_jumptable		# get jump table address
	daddiu	s0,s0,LEV2_TABLE_SIZE		# &(lev2_jumptable[1])
	lw	t0,LEV2_WRAPPER_OFS(s0)		# get wrapper address
	lw	s2,LEV2_IACK_OFS(s0)		# get IACK reg address
	lw	a1,LEV2_BASE_OFS(s0)		# get slot base address

	jal	s1,t0				# call the wrapper
	ori	a0,zero,1			# get slot no. (bd slot)

slot2_loop:
	mfc0	s0,CP0_CAUSE			# read the Cause register
	li	s1,CAUSE_IBIT5			# load slot 2 mask
	and	s0,s0,s1			# check IP5 (slot 2 IRQ)

	bne	s0,zero,slot2_entry		# slot 2 have an interrupt ?
	lw	s1,lvl2_rr_index		# read rr index (bd)

	li	s0,2				# load slot number
	bne	s1,s0,slot0_loop		# the terminating slot ?
	nop					# (bd slot)

	/*
	 * Restore context and dismiss the interrupt
	 */
	j	r4k_lvl2_dismiss		# call general purpose dismiss
	nop					# (bd slot)

slot2_entry:
	/*
	 * Setup wrapper parameters and call it
	 * 	s0 - &(lev2_jumptable[slot])
	 * 	s1 - return address
	 * 	s2 - IACK register address
	 * 	a0 - Slot number
	 * 	a1 - Slot base address
	 */
	la	s0,lev2_jumptable		# get jump table address
	daddiu	s0,s0,(LEV2_TABLE_SIZE*2)	# &(lev2_jumptable[2])
	lw	t0,LEV2_WRAPPER_OFS(s0)		# get wrapper address
	lw	s2,LEV2_IACK_OFS(s0)		# get IACK reg address
	lw	a1,LEV2_BASE_OFS(s0)		# get slot base address

	jal	s1,t0				# call the wrapper
	ori	a0,zero,2			# get slot no. (bd slot)

	j	slot0_loop			# go check slot 0
	nop					# (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_lvl2_round_robin)

/*
 * r4k_lvl2_context_save:
 * Save the standard context for a level 2 interrupt
 *
 * Note: We use a seperate function call to scale down the
 * 	 code, hence averting cache thrashing
 */
FRAME(r4k_lvl2_context_save,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Save context and increment level 2 counter
	 */
	lw	k0,l2cnt			# load counter

	/*
	 * save some context
	 */
	dmtc1	AT,FPU_SP_REG			# save sp
	dmtc1	t0,FPU_T0_REG			# save t0
	dmtc1	t1,FPU_T1_REG			# save t1
	dmtc1	t2,FPU_T2_REG			# save t2
	dmtc1	t3,FPU_T3_REG			# save t3
	dmfc1	t0,FPU_R0			# get copy of k0
	dmfc1	t1,FPU_R1			# get copy of k1
	dmfc1	t2,FPU_R2			# get copy of AT
	dmtc1	t0,FPU_K0_REG			# save k0
	dmtc1	t1,FPU_K1_REG			# save k1
	dmtc1	t2,FPU_AT_REG			# save AT
	dmfc0	t0,CP0_EPC			# get EPC reg
	mfc0	t1,CP0_SREG			# get Status reg
	mflo	t2				# get LO
	mfhi	t3				# get HI
	dmtc1	t0,FPU_EPC_REG			# save EPC reg
	dmtc1	t1,FPU_SREG_REG			# save status reg
	dmtc1	t2,FPU_LO_REG			# save lo
	dmtc1	t3,FPU_HI_REG			# save hi

	/*
	 * increment interrupt counter
	 */
	addiu	k0,k0,1				# increment counter
	sw	k0,l2cnt			# save counter

	/*
	 * save some more context
	 */
	dmtc1	v0,FPU_V0_REG			# save v0
	dmtc1	v1,FPU_V1_REG			# save v1
	dmtc1	a0,FPU_A0_REG			# save a0
	dmtc1	a1,FPU_A1_REG			# save a1
	dmtc1	a2,FPU_A2_REG			# save a2
	dmtc1	a3,FPU_A3_REG			# save a3
	dmtc1	t4,FPU_T4_REG			# save t4
	dmtc1	t5,FPU_T5_REG			# save t5
	dmtc1	t6,FPU_T6_REG			# save t6
	dmtc1	t7,FPU_T7_REG			# save t7
	dmtc1	t8,FPU_T8_REG			# save t8
	dmtc1	t9,FPU_T9_REG			# save t9

	/*
 	 * Install our level
	 */
	lw	k0,r4k_cpu_level		# read old value
	li	t5,(LEVEL2 << R4K_LEVEL_SHIFT)	# get new level (bits [5..3])
	sw	t5,r4k_cpu_level		# register new value

	/*
	 * Note : There seems to be a problem with the processor when EXL 
	 * gets de-asserted while IE is asserted and there is another
	 * interrupt pending (the interrupt is taken but the EPC register
	 * does not get re-latched). To work arround this, disable IE,
	 * reset the EXL bit, and then re-enable IE.
	 */
	li	t3,~SREG_IE			# get IE mask
	and	t3,t3,t1			# de-assert IE
	mtc0	t3,CP0_SREG			# move to status register

	addiu	t5,zero,LEVEL2_SREG_MASK	# get sreg mask
	and	t6,t5,t3			# insert new bits
	mtc0	t6,CP0_SREG			# write new sreg copy

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t6,t5,t1			# and into original sreg
	ori	t6,t6,SREG_IE			# set IE
	mtc0	t6,CP0_SREG			# write sreg (EXL=0,IE=1)
	nop					# let it settle

	dmtc1	s0,FPU_S0_REG			# save s0
	dmtc1	s1,FPU_S1_REG			# save s1
	dmtc1	s2,FPU_S2_REG			# save s2

	j	ra				# return
	dmtc1	s3,FPU_S3_REG			# save s3 (bd slot)

	.set	reorder
	.set	at
ENDFRAME(r4k_lvl2_context_save)

/*
 * r4k_lvl2_dismiss:
 * Restore the context and dismiss a level 2 interrupt
 */
FRAME(r4k_lvl2_dismiss,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Restore context and dismiss a level 2 interrupt
	 *	- k0 has previous level
	 */
	dmfc1	v0,FPU_V0_REG			# restore v0
	dmfc1	v1,FPU_V1_REG			# restore v1
	dmfc1	a0,FPU_A0_REG			# restore a0
	dmfc1	a1,FPU_A1_REG			# restore a1
	dmfc1	a2,FPU_A2_REG			# restore a2
	dmfc1	a3,FPU_A3_REG			# restore a3
	dmfc1	t4,FPU_T4_REG			# restore t4
	dmfc1	t5,FPU_T5_REG			# restore t5
	dmfc1	t6,FPU_T6_REG			# restore t6
	dmfc1	t7,FPU_T7_REG			# restore t7
	dmfc1	t8,FPU_T8_REG			# restore t8
	dmfc1	t9,FPU_T9_REG			# restore t9

	dmfc1	t1,FPU_SREG_REG			# get status reg
	dmfc1	ra,FPU_RA_REG			# restore ra
	dmfc1	s0,FPU_S0_REG			# restore s0
	dmfc1	s1,FPU_S1_REG			# restore s1

	/*
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	mfc0	t2,CP0_SREG			# read sreg
	li	t3,~SREG_IE			# get IE bit mask
	
	and	t2,t2,t3			# disable IE bit
	mtc0	t2,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t3,t3,t1			# disable IE in sreg copy
	mtc0	t3,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t1,CP0_SREG			# restore sreg (EXL=1,IE=1)

	dmfc1	s2,FPU_S2_REG			# restore s2
	dmfc1	s3,FPU_S3_REG			# restore s3

	/*
	 * restore old cpu level
	 */
	sw	k0,r4k_cpu_level		# restore old level

	dmfc1	t0,FPU_EPC_REG			# get EPC reg
	dmfc1	t2,FPU_LO_REG			# get lo
	dmfc1	t3,FPU_HI_REG			# get hi

	dmtc0	t0,CP0_EPC			# restore EPC reg
	mtlo	t2				# restore LO
	mthi	t3				# restore HI

	dmfc1	sp,FPU_SP_REG			# restore sp
	dmfc1	t0,FPU_T0_REG			# restore t0
	dmfc1	t1,FPU_T1_REG			# restore t1
	dmfc1	t2,FPU_T2_REG			# restore t2
	dmfc1	t3,FPU_T3_REG			# restore t3
	dmfc1	k0,FPU_K0_REG			# restore k0
	dmfc1	AT,FPU_AT_REG			# restore AT
	dmfc1	k1,FPU_K1_REG			# restore k1
	
	/*
	 * Dismiss the interrupt
	 */
	eret					# return
	nop					# not executed

	.set	reorder
	.set	at
ENDFRAME(r4k_lvl2_dismiss)

/*
 *	Bit Definitions				
 *	---------------
 *	IP 7	- Internal Timer	Nevada Bit 7 : CPU Write Bus Err
 *	IP 6	- NIM Hi		Nevada Bit 6 : Sh Mem Parity
 *	IP 5	- NIM 2 Lo		Nevada Bit 5 : External Timer
 *	IP 4	- NIM 1 Lo		Nevada Bit 4 : Console
 *	IP 3	- NIM 0 Lo		Nevada Bit 3 : Unused
 *	IP 2 	- Nevada		Nevada Bit 2 : Unused
 *	IP 1	- S/W 1			Nevada Bit 1 : Unused
 *	IP 0	- S/W 0			Nevada Bit 0 : Unused
 *
 *			0 : Masked	1 : Masked
 *			1 : Enabled	0 : Enabled
 *
 *	Level		Status Reg	Nevada Mask
 *			IP 7..0		Bit 7..0
 *	---------------------------------------------
 *	0		11111111	00001111
 *	1		11111111	00001111
 *	2		11000111	00001111
 *	3		10000111	00001111
 *	4		10000111	00011111
 *	5		10000111	00111111
 *	6		10000111	01111111
 *	7		10000000	11111111
 *
 * Note : It is the responsibility of each interrupt wrapper to 
 *	  install the approriate "level", and to re-install the
 *	  old level when exiting. A lower level interrupter must
 *	  never be able to interrupt a higher level handler.
 *
 */

/*
 * Table of interrupt masks for Status register and Nevada Register
 *
 * Note, Nevada mask is inverse polarity IE 0->enabled, 1->disabled.
 * The sw interrupts (0 and 1) are enabled for all levels except
 * level 7.
 */

 #
 # struct levels {
 #	ulong s_reg;
 #	uchar nev_mask;
 #	uchar pad[3];
 # } level_table[8];
 #
	.globl	r4k_level_table

	.text
	.align	3
r4k_level_table:

	.word	0x0000ff00, 0x2f000000		# Level 0
	.word	0x0000ff00, 0x2f000000		# Level 1
	.word	0x0000c700, 0x2f000000		# Level 2
	.word	0x00008700, 0x2f000000		# Level 3
	.word	0x00008300, 0x3f000000		# Level 4
	.word	0x00008300, 0x3f000000		# Level 5
	.word	0x00008300, 0x7f000000		# Level 6
	.word	0x00008000, 0xff000000		# Level 7

/* End of file */
