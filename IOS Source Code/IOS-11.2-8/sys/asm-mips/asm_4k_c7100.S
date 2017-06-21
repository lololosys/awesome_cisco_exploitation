/* $Id: asm_4k_c7100.S,v 3.1.64.3 1996/08/08 21:06:07 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_c7100.S,v $
 *------------------------------------------------------------------
 * asm_4k_c7100.S - Predator specific R4k asm support
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c7100.S,v $
 * Revision 3.1.64.3  1996/08/08  21:06:07  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/05/01  13:27:35  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.1.64.1  1996/03/21  22:27:07  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.6  1996/02/23  19:22:30  mbeesley
 * CSCdi49533:  Error interrupt handler gets incorrect frame pointer
 * Branch: ELC_branch
 *
 * Revision 3.1.2.5  1996/02/19  23:23:27  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.1.2.4  1996/01/26  22:10:19  mbeesley
 * Branch: ELC_branch
 *         o Debug GT64010 timers and add support for counters
 *         o Increase GT64010 cache line size
 *         o Finish support for fine granularity throttling
 *
 * Revision 3.1.2.3  1995/12/13  02:16:36  mbeesley
 * Branch: ELC_branch
 * Reenable GT64010 and Packet SRAM error interrupts. Leave OIR
 * interrupts as defaulted off (C7100_OIR_INTERRUPT).
 *
 * Revision 3.1.2.2  1995/12/12  04:32:51  mbeesley
 * Branch: ELC_branch
 * Add C7100_EXTERNAL_INTERRUPT to control usage of the external
 * error interrupt control. Defautl is off for now (waiting hw fix).
 *
 * Revision 3.1.2.1  1995/11/22  20:24:58  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  21:36:21  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c7100.h"

/*
 * interrupt_jump_table:
 * Used by the interrupt handler to jump to the appropriate routine.
 * The prioritization of interrupts is done by this table.
 * Note this table is in the .text segment, hence read only.
 *
 * 	IBIT		Priority
 *	------------------------------------------------------------
 *	IBIT0		5		sw0
 *	IBIT1		1		sw1 (fake net io interrupt)
 *	IBIT2		1		PA Net IO
 *	IBIT3		3		PA Management interrupt
 *	IBIT4		2		GT64010 DMA interrupt		
 *	IBIT5		4		UART
 *	IBIT6		5		Error interrupt
 *	IBIT7		7		Count / Compare
 */
	.text
	.align 2

	.globl	interrupt_jump_table
 #
 # void ((*interrupt_jump_table)())[64]
 #
	/*
	 * T=Timer, U=Uart, D=DMA, M=PA Management, N=Net IO
	 * E=External
	 */
interrupt_jump_table:			/*             TEUDMN		*/
	.word	c7100_sw_intr		/* IP [7..2] = 000000, SW Int	*/
	.word	c7100_pa_netio_intr	/* IP [7..2] = 000001, Net IO 	*/
	.word	c7100_pa_mng_intr	/* IP [7..2] = 000010, PA Mngt	*/
	.word	c7100_pa_mng_intr	/* IP [7..2] = 000011, PA Mngt	*/
	.word	c7100_dma_intr		/* IP [7..2] = 000100, DMA	*/
	.word	c7100_dma_intr		/* IP [7..2] = 000101, DMA	*/
	.word	c7100_pa_mng_intr	/* IP [7..2] = 000110, PA Mngt	*/
	.word	c7100_pa_mng_intr	/* IP [7..2] = 000111, PA Mngt	*/
	.word	c7100_uart_intr		/* IP [7..2] = 001000, UART 	*/
	.word	c7100_uart_intr		/* IP [7..2] = 001001, UART	*/
	.word	c7100_uart_intr		/* IP [7..2] = 001010, UART	*/
	.word	c7100_uart_intr		/* IP [7..2] = 001011, UART	*/
	.word	c7100_uart_intr		/* IP [7..2] = 001100, UART	*/
	.word	c7100_uart_intr		/* IP [7..2] = 001101, UART 	*/
	.word	c7100_uart_intr		/* IP [7..2] = 001110, UART	*/
	.word	c7100_uart_intr		/* IP [7..2] = 001111, UART 	*/
	.word	c7100_ext_intr		/* IP [7..2] = 010000, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 010001, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 010010, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 010011, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 010100, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 010101, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 010110, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 010111, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 011000, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 011001, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 011010, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 011011, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 011100, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 011101, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 011110, External	*/
	.word	c7100_ext_intr		/* IP [7..2] = 011111, External	*/

	.word	r4k_nmi_intr		/* IP [7..2] = 0x20, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x21, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x22, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x23, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x24, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x24, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x25, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x26, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x27, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x28, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x29, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x2A, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x2B, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x2C, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x2D, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x2E, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x2F, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x30, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x31, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x32, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x33, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x34, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x34, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x35, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x36, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x37, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x38, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x39, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x3A, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x3B, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x3C, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x3D, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x3E, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 0x3F, Timer	*/

	.extern	r4k_cpu_level,4
	.extern r4k_intr_rec_count,4
	.extern c7100_netint_throttle_configured,1
	.extern gt64010_post_coalesce,1

/*
 * Some macros to trim down the file
 */
#define SAVE_T0_T3				\
	sd	t0,SP_OFS(REG_T0)(sp);		\
	sd	t1,SP_OFS(REG_T1)(sp);		\
	sd	t2,SP_OFS(REG_T2)(sp);		\
	sd	t3,SP_OFS(REG_T3)(sp);

#define SAVE_VOLATILE_REGS			\
	sdc1	FPU_R0,SP_OFS(REG_K0)(sp);	\
	sdc1	FPU_R1,SP_OFS(REG_K1)(sp);	\
	sdc1	FPU_R2,SP_OFS(REG_AT)(sp);	\
	dmfc0	t0,CP0_EPC;			\
	mfc0	t1,CP0_SREG;			\
	mflo	t2;				\
	mfhi	t3;				\
	sd	t0,SP_OFS(REG_EPC)(sp);		\
	sw	t1,SP_OFS(REG_SREG)(sp);	\
	sd	t2,SP_OFS(REG_LO)(sp);		\
	sd	t3,SP_OFS(REG_HI)(sp);

#define SAVE_V0_A3				\
	sd	v0,SP_OFS(REG_V0)(sp);		\
	sd	v1,SP_OFS(REG_V1)(sp);		\
	sd 	a0,SP_OFS(REG_A0)(sp);		\
	sd 	a1,SP_OFS(REG_A1)(sp);		\
	sd 	a2,SP_OFS(REG_A2)(sp);		\
	sd 	a3,SP_OFS(REG_A3)(sp);

#define SAVE_T4_RA				\
	sd	t4,SP_OFS(REG_T4)(sp);		\
	sd	t5,SP_OFS(REG_T5)(sp);		\
	sd	t6,SP_OFS(REG_T6)(sp);		\
	sd	t7,SP_OFS(REG_T7)(sp);		\
	sd	t8,SP_OFS(REG_T8)(sp);		\
	sd	ra,SP_OFS(REG_RA)(sp);		\
	sd	t9,SP_OFS(REG_T9)(sp);

#define RESTORE_V0_A3				\
	ld	v0,SP_OFS(REG_V0)(sp);		\
	ld	v1,SP_OFS(REG_V1)(sp);		\
	ld 	a0,SP_OFS(REG_A0)(sp);		\
	ld 	a1,SP_OFS(REG_A1)(sp);		\
	ld 	a2,SP_OFS(REG_A2)(sp);		\
	ld 	a3,SP_OFS(REG_A3)(sp);

#define RESTORE_T1_T3_EPC_LO_HI			\
	ld	t1,SP_OFS(REG_T1)(sp);		\
	ld	t2,SP_OFS(REG_T2)(sp);		\
	ld	t3,SP_OFS(REG_T3)(sp);		\
	ld	t4,SP_OFS(REG_EPC)(sp);		\
	ld	t6,SP_OFS(REG_LO)(sp);		\
	ld	t7,SP_OFS(REG_HI)(sp);		\
	dmtc0	t4,CP0_EPC;			\
	mtlo	t6;				\
	mthi	t7;

#define RESTORE_T4_RA				\
	ld	t4,SP_OFS(REG_T4)(sp);		\
	ld	t5,SP_OFS(REG_T5)(sp);		\
	ld	t6,SP_OFS(REG_T6)(sp);		\
	ld	t7,SP_OFS(REG_T7)(sp);		\
	ld	t8,SP_OFS(REG_T8)(sp);		\
	ld	t9,SP_OFS(REG_T9)(sp);		\
	ld	ra,SP_OFS(REG_RA)(sp);

#define RESTORE_K0_K1_AT			\
	ld	k1,SP_OFS(REG_K1)(sp);		\
	ld	k0,SP_OFS(REG_K0)(sp);		\
	ld	AT,SP_OFS(REG_AT)(sp);


/*
 * pa_default_wrapper:
 * Default PA interrupt wrapper for C7100
 *
 * Entry : s2 = scratch register
 *	   s1 = return address
 *	   s0 = &pa_jumptable[pa_number]
 *	   a0 = pa number
 *
 * Exit  : return TRUE if we need to be called again before
 *         the net IO loop dismisses the interrupt
 */
 #
 # int pa_default_wrapper(void)
 #
FRAME(pa_default_wrapper,sp,0,ra)
	.set	noreorder

	/*
	 * In the future this wrapper should verify that there is
	 * no outstanding interrupts for the particular PA.
	 * For the time being, we simply return FALSE
	 */
	j	s1			# return
	move	v0,zero			# return FALSE (bd slot)

	.set 	reorder
ENDFRAME(pa_default_wrapper)

/*
 * c7100_sw_intr:
 * Handle a software or spurious interrupt. SW0 should bring us to
 * GDB. SW1 should bring us to pa_netio.
 */
 #
 # void c7100_sw_intr(void)
 #
FRAME(c7100_sw_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine checks to see it sw 1 is asserted. If so
	 * it calls the network IO interrupt handling code after
	 * de-asserting sw 1. If sw 1 is not asserted, call the
	 * spurious interrupt handling code in asm_4k.S (as we either
	 * took a sw 0 (GDB) or a spurious interrupt
	 * Note : sp has already been copied to AT
	 */
	mfc0	k1,CP0_SREG			# re-read status register
	nop					# load delay

	andi	k1,k1,CAUSE_IMASK		# get interrupt enable bits
	and	k1,k1,k0			# get asserted interrupt bits

	andi	k1,k1,CAUSE_IBIT1		# mask sw interrupt 1
	bne	k1,zero,c7100_sw1_intr		# is it sw 1 ?
	nop                                     # (bd slot)
 
	/*
	 * Refer to asm_4k.S if this ever changes
	 */
	mfc0	k1,CP0_SREG			# reload k1 with status reg.
	j	r4k_spurious_callback		# call spurious handler
	nop					# (bd slot)

c7100_sw1_intr:
	j	c7100_pa_netio_intr		# call net io handler
	nop					# (bd slot)

	.set reorder
	.set at
ENDFRAME(c7100_sw_intr)

	.extern l1stk,4
	.extern l1cnt,4

/*
 * c7100_pa_netio_intr:
 * Handle normal priority net IO interrupts
 */
 #
 # void c7100_pa_netio_intr(void);
 #
FRAME(c7100_pa_netio_intr,sp,0,ra)
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

	SAVE_T0_T3				/* Save to..t3 */
	SAVE_VOLATILE_REGS			/* Save k0,k1,AT,EPC,
						   SREG,Lo,Hi */
	SAVE_V0_A3				/* Save v0,v1,a0..a3 */
	SAVE_T4_RA				/* Save t4..t9,ra */

	/*
	 * Save s0..s3
	 */
	sd	s0,SP_OFS(REG_S0)(sp)		# save s0
	sd	s1,SP_OFS(REG_S1)(sp)		# save s1
	sd	s2,SP_OFS(REG_S2)(sp)		# save s2
	sd	s3,SP_OFS(REG_S3)(sp)		# save s3

	/*
	 * Save the interrupted level and install the new level
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	li	t3,(LEVEL1 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t3,r4k_cpu_level		# save new level

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

	li	t2,LEVEL1_SREG_MASK		# get sreg mask
	and	t4,t4,t2			# and sreg copy with mask
	mtc0	t4,CP0_SREG			# EXL=0, IE=0

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t1,t1,t2			# and it into sreg
	ori	t1,t1,SREG_IE			# set IE

	/*
	 * If throttling is configured, set it up
	 */
	lbu	t2,c7100_netint_throttle_configured

	beq	t2,zero,level1_play_it_again	# nope, hop on by
	mtc0	t1,CP0_SREG			# write back sreg, EXL=0 (bd)

	jal	c7100_netint_throttle_setup	# setup throttling
	nop					# (bd slot)
	
level1_play_it_again:

	/*
	 * Clear software interrupts and check the post coalesce
	 * queue for anything to do.
	 */
	lbu	s0,gt64010_post_coalesce	# read post-coalesce flag
	mtc0	zero,CP0_CAUSE			# clear sw int 0/1

	beq	s0,zero,bay0			# nothing to do ?
	move	s3,zero				# clear work to do (bd slot)

	jal	gt64010_post_coalesce_dispatch	# call dispatcher
	nop

bay0:
	/*
	 * Call port adaptor zeros wrapper
	 */
	la	s0,pa_jumptable			# get jump table address
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	
	/*
	 * Read interrupt status bits for PA bay 0
	 */	
	lbu	a2,ADRSPC_BAY0_ISTAT_REG

	jal	s1,t0				# call the wrapper
	move	a0,zero				# slot number (bd slot)

bay1:
	/*
	 * Call port adaptor one's wrapper
	 */
	addu	s3,s3,v0			# update "work_to_do"

	la	s0,pa_jumptable			# get jump table address
	daddiu	s0,s0,PA_TABLE_SIZE		# bay 1 table
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	
	/*
	 * Read interrupt status bits for PA bay 1
	 */	
	lbu	a2,ADRSPC_BAY1_ISTAT_REG

	jal	s1,t0				# call the wrapper
	ori	a0,zero,1			# slot number (bd slot)

bay2:
	/*
	 * Call port adaptor two's wrapper
	 */
	addu	s3,s3,v0			# update "work_to_do"

	la	s0,pa_jumptable			# get jump table address
	daddiu	s0,s0,(PA_TABLE_SIZE*2)		# bay 2 table
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	
	/*
	 * Read interrupt status bits for PA bay 2
	 */	
	lbu	a2,ADRSPC_BAY2_ISTAT_REG

	jal	s1,t0				# call the wrapper
	ori	a0,zero,2			# slot number (bd slot)

bay3:
	/*
	 * Call port adaptor three's wrapper
	 */
	addu	s3,s3,v0			# update "work_to_do"

	la	s0,pa_jumptable			# get jump table address
	daddiu	s0,s0,(PA_TABLE_SIZE*3)		# bay 3 table
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	
	/*
	 * Read interrupt status bits for PA bay 3
	 */	
	lbu	a2,ADRSPC_BAY3_ISTAT_REG

	jal	s1,t0				# call the wrapper
	ori	a0,zero,3			# slot number (bd slot)

	/*
	 * Check for work on the post coalesce queue
	 */
	lbu	s0,gt64010_post_coalesce	# read post-coalesce flag
	addu	s3,s3,v0			# update "work_to_do" bay 3

	beq	s0,zero,bay4			# nothing to do ?
	nop					# (bd slot)

	jal	gt64010_post_coalesce_dispatch	# call dispatcher
	nop

bay4:
	/*
	 * Call port adaptor four's wrapper
	 */
	la	s0,pa_jumptable			# get jump table address
	daddiu	s0,s0,(PA_TABLE_SIZE*4)		# bay 4 table
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	
	/*
	 * Read interrupt status bits for PA bay 4
	 */	
	lbu	a2,ADRSPC_BAY4_ISTAT_REG

	jal	s1,t0				# call the wrapper
	ori	a0,zero,4			# slot number (bd slot)

bay5:
	/*
	 * Call port adaptor fives's wrapper
	 */
	addu	s3,s3,v0			# update "work_to_do"

	la	s0,pa_jumptable			# get jump table address
	daddiu	s0,s0,(PA_TABLE_SIZE*5)		# bay 5 table
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	
	/*
	 * Read interrupt status bits for PA bay 5
	 */	
	lbu	a2,ADRSPC_BAY5_ISTAT_REG

	jal	s1,t0				# call the wrapper
	ori	a0,zero,5			# slot number (bd slot)

bay6:
	/*
	 * Call port adaptor six's wrapper
	 */
	addu	s3,s3,v0			# update "work_to_do"

	la	s0,pa_jumptable			# get jump table address
	daddiu	s0,s0,(PA_TABLE_SIZE*6)		# bay 5 table
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	
	/*
	 * Read interrupt status bits for PA bay 6
	 */	
	lbu	a2,ADRSPC_BAY6_ISTAT_REG

	jal	s1,t0				# call the wrapper
	ori	a0,zero,6			# slot number (bd slot)

	addu	s3,s3,v0			# update "work_to_do"

	/*
	 * If work to do is non-zero, go to the beginning of the loop
	 */
	bne	s3,zero,level1_play_it_again	# more work to do ?
	li	s1,(CAUSE_IBIT2|CAUSE_IBIT1)	# get mask for PA intr,sw1 (bd)

	/*
	 * If throttling is enabled, call the check routine
	 */
	lbu	s3,c7100_netint_throttle_configured

	beq	s3,zero,no_throttling		# no throttling, jump on by
	mfc0	s0,CP0_CAUSE			# read cause register (bd slot)

	jal	c7100_netint_throttle_check	# call the throttle checker
	nop					# (bd slot)

	mfc0	s0,CP0_CAUSE			# read cause register (bd slot)

no_throttling:	
	/*
	 * If PA interrupt still asserted or sw1, loop arround
	 */
	and	s0,s0,s1			# check IBIT2, IBIT1

	bne	s0,zero,level1_play_it_again    # start all over
	nop					# (bd slot)

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	RESTORE_V0_A3				/* Restore v0..v1,a0..a3 */

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

	RESTORE_T1_T3_EPC_LO_HI			/* restore T1..T3,EPC,Lo,Hi */

	/*
	 * Restore s0..s3
	 */
	ld	s0,SP_OFS(REG_S0)(sp)		# restore s0
	ld	s1,SP_OFS(REG_S1)(sp)		# restore s1
	ld	s2,SP_OFS(REG_S2)(sp)		# restore s2
	ld	s3,SP_OFS(REG_S3)(sp)		# restore s3
	
	/*
	 * We need to restore the interrupted level
	 * Note : If incorrect set_interrupt_level/reset_interrupt_level
	 * calls were made in the handler, the interrupt mask will
	 * be corrupted and we would have to re-install it here.
	 * Also decrement the interrupt recursion count
	 */
	lw	t4,r4k_intr_rec_count		# read counter
	sw	k0,r4k_cpu_level		# restore old level
	addiu	t4,t4,-1			# decrement counter
	sw	t4,r4k_intr_rec_count		# save counter
	
	/*
	 * Restore remaining context
	 */
	RESTORE_T4_RA				/* restore t4..t9, ra */
	RESTORE_K0_K1_AT			/* restore k0, k1, AT */

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
ENDFRAME(c7100_pa_netio_intr)

	.extern l2stk,4
	.extern l2cnt,4
	.extern lev2subr,4

/*
 * c7100_dma_intr:
 * Handle DMA interrupts from the gt64010
 */
 #
 # void c7100_dma_intr(void);
 #
FRAME(c7100_dma_intr,sp,0,ra)
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

	SAVE_T0_T3				/* Save to..t3 */
	SAVE_VOLATILE_REGS			/* Save k0,k1,AT,EPC,
						   SREG,Lo,Hi */
	SAVE_V0_A3				/* Save v0,v1,a0..a3 */
	SAVE_T4_RA				/* Save t4..t9,ra */

	/*
	 * Save the interrupted level and install the new level
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	li	t3,(LEVEL2 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t3,r4k_cpu_level		# save new level

	/*
	 * We lock out this level of interrupt and negate the EXL
	 * bit. This allows higher priority interrupts to then be
	 * serviced. No need to update external interrupt mask
	 * as it does not change from Levels 0,1,2 and 3
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

	li	t2,LEVEL2_SREG_MASK		# get sreg mask
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
	RESTORE_V0_A3				/* Restore v0..v1,a0..a3 */

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

	RESTORE_T1_T3_EPC_LO_HI			/* restore T1..T3,EPC,Lo,Hi */

	/*
	 * We need to restore the interrupted level
	 * Note : If incorrect set_interrupt_level/reset_interrupt_level
	 * calls were made in the handler, the interrupt mask will
	 * be corrupted and we would have to re-install it here.
	 * Also decrement the interrupt recursion count
	 */
	lw	t4,r4k_intr_rec_count		# read counter
	sw	k0,r4k_cpu_level		# restore old level
	addiu	t4,t4,-1			# decrement counter
	sw	t4,r4k_intr_rec_count		# save counter

	/*
	 * Restore remaining context
	 */
	RESTORE_T4_RA				/* restore t4..t9, ra */
	RESTORE_K0_K1_AT			/* restore k0, k1, AT */

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
ENDFRAME(c7100_dma_intr)

	.extern l3stk,4
	.extern l3cnt,4
	.extern lev3subr,4
/*
 * c7100_pa_mng_intr:
 * Handle high priority PA management interrupts
 */
 #
 # void c7100_pa_mng_intr(void)
 #
FRAME(c7100_pa_mng_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 3 stack
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l3stk			# move onto special stack
	lw	k0,l3cnt			# load counter
	
	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l3cnt			# save the counter

	SAVE_T0_T3				/* Save to..t3 */
	SAVE_VOLATILE_REGS			/* Save k0,k1,AT,EPC,
						   SREG,Lo,Hi */
	SAVE_V0_A3				/* Save v0,v1,a0..a3 */
	SAVE_T4_RA				/* Save t4..t9,ra */

	/*
	 * Save the interrupted level and install the new level
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	li	t3,(LEVEL3 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t3,r4k_cpu_level		# save new level

	/*
	 * We lock out this level of interrupt and negate the EXL
	 * bit. This allows higher priority interrupts to then be
	 * serviced. No need to update external interrupt mask
	 * as it does not change from Levels 0,1,2 and 3
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

	li	t2,LEVEL3_SREG_MASK		# get sreg mask
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
	RESTORE_V0_A3				/* Restore v0..v1,a0..a3 */

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

	RESTORE_T1_T3_EPC_LO_HI			/* restore T1..T3,EPC,Lo,Hi */

	/*
	 * We need to restore the interrupted level
	 * Note : If incorrect set_interrupt_level/reset_interrupt_level
	 * calls were made in the handler, the interrupt mask will
	 * be corrupted and we would have to re-install it here.
	 * Also decrement the interrupt recursion count
	 */
	lw	t4,r4k_intr_rec_count		# read counter
	sw	k0,r4k_cpu_level		# restore old level
	addiu	t4,t4,-1			# decrement counter
	sw	t4,r4k_intr_rec_count		# save counter

	/*
	 * Restore remaining context
	 */
	RESTORE_T4_RA				/* restore t4..t9, ra */
	RESTORE_K0_K1_AT			/* restore k0, k1, AT */

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
ENDFRAME(c7100_pa_mng_intr)

	.extern l4stk,4
	.extern l4cnt,4
	.extern lev4subr,4
/*
 * c7100_uart_intr:
 * Handle UART interrupts
 */
 #
 # void c7100_uart_intr(void)
 #
FRAME(c7100_uart_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 4 stack
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l4stk			# move onto special stack
	lw	k0,l4cnt			# load counter
	
	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l4cnt			# save the counter

	SAVE_T0_T3				/* Save to..t3 */
	SAVE_VOLATILE_REGS			/* Save k0,k1,AT,EPC,
						   SREG,Lo,Hi */
	SAVE_V0_A3				/* Save v0,v1,a0..a3 */
	SAVE_T4_RA				/* Save t4..t9,ra */

	/*
	 * Save the interrupted level and install the new level
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	li	t3,(LEVEL4 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t3,r4k_cpu_level		# save new level

	/*
	 * We lock out this level of interrupt and negate the EXL
	 * bit. This allows higher priority interrupts to then be
	 * serviced. No need to update external interrupt mask
	 * as it does not change from Levels 0,1,2 and 3
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

	li	t2,LEVEL4_SREG_MASK		# get sreg mask
	lw	t3,lev4subr			# get handler address
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
	RESTORE_V0_A3				/* Restore v0..v1,a0..a3 */

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

	RESTORE_T1_T3_EPC_LO_HI			/* restore T1..T3,EPC,Lo,Hi */

	/*
	 * We need to restore the interrupted level
	 * Note : If incorrect set_interrupt_level/reset_interrupt_level
	 * calls were made in the handler, the interrupt mask will
	 * be corrupted and we would have to re-install it here.
	 * Also decrement the interrupt recursion count
	 */
	lw	t4,r4k_intr_rec_count		# read counter
	sw	k0,r4k_cpu_level		# restore old level
	addiu	t4,t4,-1			# decrement counter
	sw	t4,r4k_intr_rec_count		# save counter

	/*
	 * Restore remaining context
	 */
	RESTORE_T4_RA				/* restore t4..t9, ra */
	RESTORE_K0_K1_AT			/* restore k0, k1, AT */

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
ENDFRAME(c7100_uart_intr)

	.extern l5stk,4
	.extern l5cnt,4
	.extern lev5subr,4
/*
 * c7100_ext_intr:
 * Handle interrupts from the xternal interrupt controller
 */
 #
 # void c7100_ext_intr(void)
 #
FRAME(c7100_ext_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 5 stack
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l5stk			# move onto special stack
	lw	k0,l5cnt			# load counter
	
	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l5cnt			# save the counter

	SAVE_T0_T3				/* Save to..t3 */
	SAVE_VOLATILE_REGS			/* Save k0,k1,AT,EPC,
						   SREG,Lo,Hi */
	SAVE_V0_A3				/* Save v0,v1,a0..a3 */
	SAVE_T4_RA				/* Save t4..t9,ra */

	/*
	 * Save the interrupted level and install the new level
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	li	t3,(LEVEL5 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t3,r4k_cpu_level		# save new level

	/*
	 * We lock out this level of interrupt and negate the EXL
	 * bit. This allows higher priority interrupts to then be
	 * serviced. No need to update external interrupt mask
	 * as it does not change from Levels 0,1,2 and 3
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

	li	t2,LEVEL5_SREG_MASK		# get sreg mask
	lw	t3,lev5subr			# get handler address
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
	move	a0,sp				# setup context param (bd)

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	RESTORE_V0_A3				/* Restore v0..v1,a0..a3 */

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

	RESTORE_T1_T3_EPC_LO_HI			/* restore T1..T3,EPC,Lo,Hi */

	/*
	 * We need to restore the interrupted level
	 * Note : If incorrect set_interrupt_level/reset_interrupt_level
	 * calls were made in the handler, the interrupt mask will
	 * be corrupted and we would have to re-install it here.
	 * Also decrement the interrupt recursion count
	 */
	lw	t4,r4k_intr_rec_count		# read counter
	sw	k0,r4k_cpu_level		# restore old level
	addiu	t4,t4,-1			# decrement counter
	sw	t4,r4k_intr_rec_count		# save counter

	/*
	 * Restore remaining context
	 */
	RESTORE_T4_RA				/* restore t4..t9, ra */
	RESTORE_K0_K1_AT			/* restore k0, k1, AT */

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
ENDFRAME(c7100_ext_intr)

/*
 *
 *			0 : Masked	0 : Masked
 *			1 : Enabled	1 : Enabled
 *
 *	Level		Status Reg	External Mask
 *			IP 7..0		Bit 7..0
 *	---------------------------------------------
 *	0		11111111	11111111
 *	1		11111001	11111111
 *	2		11101001	11111111
 *	3		11100001	11111111
 *	4		11000001	11111111
 *	5		10000000	11111111
 *	6		10000000	11111111
 *	7		10000000	11111111
 *
 * Note : It is the responsibility of each interrupt wrapper to 
 *	  install the approriate "level", and to re-install the
 *	  old level when exiting. A lower level interrupter must
 *	  never be able to interrupt a higher level handler.
 */

/*
 * Table of interrupt masks for Status register and External Register
 *
 * Note, External mask is positive polarity IE 1->enabled, 0->disabled.
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

	/*	
	 * These settings have the OIR interrupts enabled.
	 */
	.word	0x0000ff00, 0xff000000		# Level 0
	.word	0x0000f900, 0xff000000		# Level 1
	.word	0x0000e900, 0xff000000		# Level 2
	.word	0x0000e100, 0xff000000		# Level 3
	.word	0x0000c100, 0xff000000		# Level 4
	.word	0x00008000, 0xff000000		# Level 5
	.word	0x00008000, 0xff000000		# Level 6
	.word	0x00008000, 0xff000000		# Level 7

/* end of file */
