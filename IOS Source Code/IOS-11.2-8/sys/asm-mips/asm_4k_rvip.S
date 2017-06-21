/* $Id: asm_4k_rvip.S,v 3.3.30.2 1996/08/08 21:06:13 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_rvip.S,v $
 *------------------------------------------------------------------
 * asm_4k_rvip.S : Assembly routines for Checkers
 *
 * January 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_rvip.S,v $
 * Revision 3.3.30.2  1996/08/08  21:06:13  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.3.30.1  1996/03/21  22:27:13  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/02/10  00:25:26  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.3  1996/02/08  06:44:21  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.2  1996/01/25  12:09:19  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.2.2.1  1995/12/23  04:39:14  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.3  1996/01/12  21:53:14  jkuzma
 * CSCdi46358:  VIP1 PA mngmt and DMA interrupt stacks swapped
 *              General debug of PA management interrupts
 *
 * Revision 3.2  1995/11/17  08:40:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:06:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_vip.h"
#include "asm_4k_rvip.h"

/*
 * Include memd support files
 */
#include "../hes/ccb_cmd.h"
#include "../src-vip/vip_memd.h"

/*
 * interrupt_jump_table:
 * Used by the interrupt handler to jump to the appropriate routine.
 * The prioritization of interrupts is done by this table.
 * Note this table is in the .text segment, hence read only.
 *
 * CIP layout
 * 	CPU Line	Priority	Description
 *	----------------------------------------------------------
 *	INT0		4		GDB Debugging
 *	INT1		1		Fake Net IO interrupt
 *	INT2		2,3,4		Nevada (PA NMI, Error, DMA timer,etc)
 *	INT3		2		DMA Engine
 *	INT4		1		PA Net IO
 *	INT5		7		Malu Attention
 *	INT6		6		DBUS
 *	INT7		5		Timer
 *
 * RVIP layout
 *	----------------------------------------------------------
 *	INT0		4		GDB Debugging
 *	INT1		1		Fake Net IO interrupt
 *	INT2		3,4		Nevada (PA NMI, Error, DMA timer,etc)
 *	INT3		1		PA Net IO
 *	INT4		2		DMA Engine
 *	INT5		7		Malu Attention
 *	INT6		6		DBUS
 *	INT7		5		Timer
 */
	.text
	.align 2

	.globl	interrupt_jump_table
	.globl	vip_cbus_intr

 #
 # void ((*interrupt_jump_table)())[64]
 #
interrupt_jump_table:			/*	       765432		*/
	.word	vip_sw_intr		/* IP [7..2] = 000000, S/W Int	*/
	.word	vip_nev_intr		/* IP [7..2] = 000001, Nevada	*/
	.word	vip_pa_lo_intr		/* IP [7..2] = 000010, PA	*/ 
	.word	vip_nev_intr		/* IP [7..2] = 000011, Nevada	*/
	.word	vip_dma_intr		/* IP [7..2] = 000100, DMA 	*/
	.word	vip_nev_intr		/* IP [7..2] = 000101, Nevada 	*/
	.word	vip_dma_intr		/* IP [7..2] = 000110, DMA	*/ 
	.word	vip_nev_intr		/* IP [7..2] = 000111, Nevada	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 001000, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 001001, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 001010, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 001011, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 001100, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 001101, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 001110, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 001111, Cbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 010000, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 010001, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 010010, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 010011, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 010100, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 010101, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 010110, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 010111, Dbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 011000, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 011001, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 011010, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 011011, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 011100, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 011101, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 011110, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 011111, Cbus	*/

	.word	r4k_nmi_intr		/* IP [7..2] = 100000, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100001, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100010, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100011, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100100, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100101, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100110, Timer	*/
	.word	r4k_nmi_intr		/* IP [7..2] = 100111, Timer	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 101000, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 101001, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 101010, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 101011, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 101100, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 101101, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 101110, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 101111, Cbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 110000, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 110001, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 110010, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 110011, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 110100, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 110101, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 110110, Dbus	*/
	.word	vip_dbus_intr		/* IP [7..2] = 110111, Dbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 111000, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 111001, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 111010, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 111011, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 111100, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 111101, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 111110, Cbus	*/
	.word	rvip_cbus_intr		/* IP [7..2] = 111111, Cbus	*/

/*
 * Table driving the external interrupt controller (Nevada)
 */
	.text
	.align 2

vip_external_intr:			# Description		      Level
					# -----------------------------------
	.word	vip_nevada_nop		# no interrupt pending
	.word   vip_pa_nmi_intr		# pa high priority		3
	.word	vip_lvl4_intr		# pci controller (PMAC err)	4
	.word	vip_unused_intr		# not connected			4
	.word	vip_lvl4_intr		# io write error 	    	4
	.word	vip_lvl4_intr		# dram write error 	    	4
	.word	vip_nevada_timer        # dma timer		    	3
	.word	vip_unused_intr		# PCI stall (currently masked)	4
	.word	vip_unused_intr		# not connected			4

/*
 * Some externals to allow gas to do gp addressing
 */
	.extern r4k_cpu_level,4
	.extern int_count,4
	.extern schedflag,4
	.extern current_count,4
	.extern profile_enabled,4

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
 * vip_nev_intr :
 * Handle Nevada interrupts : Bus Errors, PA management
 *
 * FIXME: What about PCI bus errors, Parity errors, PA stalls etc ?
 *
 *  Nevada IRQ		Interrupt
 *  ----------		----------------------------
 *	7		Unused
 *	6		PCI stall (currently masked)
 *	5		Nevada timer
 *	4		DRAM Write bus error
 *	3		IO_Write bus error
 *	2		Unused
 *	1		PMAC error
 *	0		PA Management
 */
 #
 # void vip_nev_intr (void)
 #
FRAME(vip_nev_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * Read Nevada to get the vector and dereference through
	 * the external jump table
	 */
	lbu	k0,IO_IVECT_REG		# read the vector
	la	k1,vip_external_intr	# get table address
	addu	k1,k1,k0		# get table entry address
	lw	k1,0(k1)		# read the handler address

	j	k1			# jump to the handler
	addiu	k1,zero,1		# set k1 to 1 (bd slot)

	.set	reorder
	.set	at
ENDFRAME(vip_nev_intr)

/*
 * vip_pa_nmi_intr:
 * Handle PA management interrupts (Level 3)
 */
 #
 # void vip_pa_nmi_intr (void)
 #
	.extern l3stk,4
	.extern l3cnt,4
	.extern lev3subr,4

FRAME(vip_pa_nmi_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 3 stack
	 * It dereferences off lev3subr to the wrapper/driver
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l3stk			# move onto level 3 stack
	lw	k0,l3cnt			# load counter

	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame on new stack
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l3cnt			# save the counter

	SAVE_T0_T3				/* save t0..t3 */

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL3 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(8*LEVEL3))(t2)		# read new mask value
	la	t0,IO_IMASK_REG			# get interrupt mask reg addr
	sb	t1,0(t0)			# write nevada mask

	/*
	 * From the cpus perspective, this is a level 3 interrupt.
	 * From Nevada's perspective, it is "level 0" (i.e., IRQ0)
	 */
	li	t3,0x1				# get EOI value
	la	t0,IO_IEOI_REG			# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	la      t2,IO_IMASK_REG                 # get interrupt mask reg addr
	lbu     t1,0(t2)                        # read imask register (to
        	                     		# sync the posted writes)

	/*
	 * Load lev3subr into AT and call the dispatch routine
	 * Note : The dispatch lev3subr handler must handle
	 * management interrupts for both port adaptors before returning.
	 */
	j	vip_intr_dispatch		# call dispatcher
	lw	AT,lev3subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(vip_pa_nmi_intr)

/*
 * vip_lvl4_intr:
 * Handle a Level 4 Interrupt (Bus error of some sort)
 */
 #
 # void vip_lvl4_intr (void)
 #
	.extern l4stk,4
	.extern l4cnt,4
	.extern lev4subr,4

FRAME(vip_lvl4_intr,sp,0,ra)
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

	SAVE_T0_T3				/* save t0 .. t3 */

	/*
	 * Save the interrupted level and install new level
	 */
	lw	k0,r4k_cpu_level		# save old level
	li	t1,(LEVEL4 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t1,r4k_cpu_level		# save new level

	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(8*LEVEL4))(t2)		# read new mask value
	la	t0,IO_IMASK_REG			# get interrupt mask reg addr
	sb	t1,0(t0)			# write nevada mask

	/*
	 * Load lev4subr into AT and call the dispatch routine
	 */
	move	k1,sp                           # load k1 with sp
	j	vip_intr_dispatch		# call dispatcher
	lw	AT,lev4subr			# get handler (bd slot)

	.set	reorder
	.set	at
ENDFRAME(vip_lvl4_intr)

/*
 * vip_unused_intr:
 * Handle a case where Nevada reports an interrupt vector
 * that is unused. We treat this as a gross hardware failure
 * allthough it could be considered a spurious interrupt.
 */
FRAME(vip_unused_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the present stack. It calls the unused
	 * interrupt driver which will crash the system. Before
	 * calling the handler, install Level 4 priorities (so we can
	 * be interrupted by the timer, Malu Attention and Dbus).
	 * Note: Unused interrupts are 2,6,7.
	 */
	lbu	a0,IO_IVECT_REG			# read interrupt vector

	li	t1,(LEVEL4 << R4K_LEVEL_SHIFT)	# get Level 4
	sw	t1,r4k_cpu_level		# set Level 4
	la	t2,r4k_level_table		# get level table address
	lbu	t1,(4+(8*LEVEL4))(t2)		# read new mask value

	la	t0,IO_IMASK_REG			# get interrupt mask reg addr
	sb	t1,0(t0)			# write nevada mask

	li	t3,0xc4				# get EOI value (Level 2,6,7)
	la	t0,IO_IEOI_REG			# get EOI reg addr
	sb	t3,0(t0)			# clear the interrupt

	mfc0	a1,CP0_SREG			# read status register
	lw	t3,(8*LEVEL4)(t2)		# read new IP bits for Level 4

	li	t4,~SREG_IE			# get IE mask
	and	t4,a1,t4			# clear IE
	mtc0	t4,CP0_SREG			# write status register
	nop					# let it settle
		
	li	t5,~(CAUSE_IMASK|SREG_EXL)	# get IP[7..0]|SREG_EXL mask
	and 	t5,t4,t5			# clear IP[7..0],EXL
	or	t5,t5,t3			# insert Level 4 IP bits
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
ENDFRAME(vip_unused_intr)

/*
 * vip_intr_dispatch:
 * Dispatch an interrupt handler for interrupts handled thru the
 * external interrupt controller. Save remaining context and call the 
 * handler. Re-install the old level, the context and dismiss the 
 * interrupt handler. This is a general purpose dispatch for interrupts 
 * serviced through the external interrupt controller.
 *
 * Note : t0-t3 have already been saved.
 *	  Stack has been setup with a frame and old sp is stacked
 * 	  Old level is in k0.
 *	  External interrupt controller has been handled
 *	  EXL Bit is still asserted
 *	  AT has address of wrapper/driver routine
 */
 #
 # void vip_intr_dispatch (void)
 #
FRAME(vip_intr_dispatch,sp,0,ra)
	.set	noreorder
	.set	noat

	SAVE_VOLATILE_REGS			/* save volatile stuff */

	/*
	 * Re-enable interrupts so Malu Attention does not wait too long.
	 * We want to mask out PA IO, DMA, SW1
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

	li	t2,LEVEL34_SREG_MASK		# get new mask
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

	/*
	 * Save the remaining context
	 */
	SAVE_V0_A3				/* save v0,v1,a0..a3 */
	SAVE_T4_RA				/* save t4..t9,ra */

	jal	AT				# call handler/driver
	move	a0,k1				# setup param 1. This gives 
						# handlers a context ptr

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	RESTORE_V0_A3				/* restore v0,v1,a0..a3 */

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
	.set	noat
ENDFRAME(vip_intr_dispatch)

/*
 * vip_sw_intr:
 * Handle a software or spurious interrupt
 * 	- k0 has copy of cause register
 */
 #
 # void vip_sw_intr (void)
 #
FRAME(vip_sw_intr, sp, 0, ra)
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
	bne	k1,zero,vip_sw1_intr		# is it sw 1 ?
	nop                                     # (bd slot)
 
	/*
	 * Refer to asm_4k.S if this ever changes
	 */
	mfc0	k1,CP0_SREG			# reload k1 with status reg.
	j	r4k_spurious_callback		# call spurious handler
	nop					# (bd slot)

vip_sw1_intr:
	j	vip_pa_lo_intr			# call net io handler
	nop					# (nd slot)

	.set reorder
	.set at
ENDFRAME(vip_sw_intr)

/*
 * pa_default_wrapper:
 * Default PA inerrupt wrapper for VIP
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

	.extern dma_timer_check,4

/*
 * vip_nevada_nop :
 * Handle a nevada interrupt when the interrupt source goes away
 * and we are left with a null vector.
 * Upon entry:
 *	k0 saved in FPU_R0
 *	k1 saved in FPU_R1
 *	AT saved in FPU_R2
 *	sp saved in AT
 */
FRAME(vip_nevada_nop, sp, 0, ra)
	.set noreorder
	.set noat

	move	sp,AT				# restore sp
	dmfc1	k0,FPU_R0			# restore k0
	dmfc1	k1,FPU_R1			# restore k1
	dmfc1	AT,FPU_R2			# restore AT
	
	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

	.set 	reorder
	.set at
ENDFRAME(vip_nevada_nop)

/*
 * vip_nevada_timer :
 * Handle a Nevada timer interrupt. We basically use this interrupt
 * to guarantee a sw1 interrupt in a reasonable amout of time to
 * limit latency times.
 *
 * Upon entry:
 *	k0 saved in FPU_R0
 *	k1 saved in FPU_R1
 *	AT saved in FPU_R2
 *	sp saved in AT
 */
FRAME(vip_nevada_timer, sp, 0, ra)
	.set noreorder
	.set noat

	la	k0,IO_CTI_REG                   # get CTI reg addr
	sb	zero,0(k0)                      # ack interrupt
	la	k0,IO_IEOI_REG			# get EOI reg addr
	li	k1,0x20				# get EOI value
	sb	k1,0(k0)			# clear the interrupt

	la      k1,IO_IMASK_REG                 # get interrupt mask reg addr
	lbu     k1,0(k1)                        # read imask register (to
        	                     		# sync the posted writes)

	/*
	 * Cause a sw1 interrupt
	 */
	li	k1,CAUSE_IBIT1			# load value
	mtc0	k1,CP0_CAUSE			# assert sw1 interrupt

	/*
	 * Increment the dma_timer_check counter
	 */
	lw	k0,dma_timer_check		# read counter
	addiu	k0,k0,1				# increment counter
	sw	k0,dma_timer_check		# write counter

	add	sp,AT,zero			# restore sp
	dmfc1	k0,FPU_R0			# restore k0
	dmfc1	k1,FPU_R1			# restore k1
	dmfc1	AT,FPU_R2			# restore AT

	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

	.set 	reorder
	.set 	at
ENDFRAME(vip_nevada_timer)

/*
 * vip_pa_lo_intr :
 * Handle a Level 1 interrupt (PA Net IO, MALU Cmd, Nevada Timer)
 * Upon entry:
 *	k0 saved in FPU_R0
 *	k1 saved in FPU_R1
 *	AT saved in FPU_R2
 *	sp saved in AT
 */
	.extern l1stk, 4
	.extern l1cnt, 4

FRAME(vip_pa_lo_intr, sp, 0, ra)
	.set noreorder
	.set noat

	/*
	 * Move onto level 1 stack and save context
	 */
	lw	sp,l1stk			# move onto level 1 stack
	lw	k0,l1cnt			# load counter

        /*
         * Save some registers so we have something to play with
         */
        dmtc1   AT,FPU_SP_REG                   # save sp
        dmtc1   t0,FPU_T0_REG                   # save t0
        dmtc1   t1,FPU_T1_REG                   # save t1
        dmtc1   t2,FPU_T2_REG                   # save t2
        dmtc1   t3,FPU_T3_REG                   # save t3

        /*
         * increment interrupt counter
         */
	addiu	k0,k0,1				# increment counter
	sw	k0,l1cnt			# save counter

	/*
	 * save volatile context
	 */
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
	 * Install our level
	 */
	lw      k0,r4k_cpu_level                # read old value
	li      t2,(LEVEL1 << R4K_LEVEL_SHIFT)  # get new level (bits [5..3])
	sw      t2,r4k_cpu_level                # register new value

	/*
	 * Disable timer interrupts
	 */
	la	t0,IO_CCR_REG			# get timer control register
	sb	zero,0(t0)			# disable timer

	/*
	 * Re-enable interrupts so Malu Attention does not wait too long.
	 * We want to mask out PA IO, DMA Timer, SW1
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

	li      t2,LEVEL1_SREG_MASK             # get sreg mask
	and     t0,t2,t3                        # insert new bits
	mtc0    t0,CP0_SREG                     # write new sreg copy

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and     t0,t2,t1                        # and into original sreg
	ori     t0,t0,SREG_IE                   # set IE
	mtc0    t0,CP0_SREG                     # write sreg (EXL=0,IE=1)
	nop					# let it settle

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
	dmtc1	ra,FPU_RA_REG			# save ra

	dmtc1	s0,FPU_S0_REG			# save s0
	dmtc1	s1,FPU_S1_REG			# save s1
	dmtc1	s2,FPU_S2_REG			# save s2
	dmtc1	s3,FPU_S3_REG			# save s3

level1_play_it_again:

	/*
	 * Clear software interrupts
	 */
	mtc0	zero,CP0_CAUSE			# clear sw int 0/1

        /*
         * Do network level chores (drq/dtq consumer,maluq,etc)
         */
        jal     vip_network_chores              # call the handler
	move	s3,zero				# zero "work to do" (bd slot)

	/*
	 * Call port adaptor zeros wrapper
	 */
	la	s0,pa_jumptable			# get jump table address
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a2,(ADRSPC_PMAC+8)		# read intrp status
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	srl	a2,a2,10			# shift right 10 bits
	andi	a2,a2,0x00ff			# and out everything except
						# PA0s eight net io interrupts

	jal	s1,t0				# call the wrapper
	move	a0,zero				# slot number (bd slot)

        /*
         * Do network level chores (dtq/drq consumer,etc)
         */
        jal     vip_network_chores              # call the handler
	addu	s3,s3,v0			# update "work_to_do"
						# from PA 0s wrapper (bd slot)
	/*
	 * Call port adaptor one wrapper
	 */
	la	s0,pa_jumptable			# get jump table address
	daddiu	s0,s0,PA_TABLE_SIZE		# slot 1s table address
	lw	t0,PA_WRAPPER_OFS(s0)		# get wrapper
	lw	a2,(ADRSPC_PMAC+8)		# read intrp status
	lw	a1,PA_BASE_OFS(s0)		# get PA base address
	andi	a2,a2,0x00ff			# and out everything except
						# PA1s eight net io interrupts

	jal	s1,t0				# call the wrapper
	ori	a0,zero,1			# slot number (bd slot)

        /*
         * Do network level chores (dtq/drq consumer,etc)
         */
        jal     vip_network_chores              # call the handler
	addu	s3,s3,v0			# update "work_to_do"
						# from PA 1s wrapper (bd slot)

	/*
	 * If work to do is non-zero, go to the beginning of the loop
	 */
	bne	s3,zero,level1_play_it_again	# more work to do ?
	nop					# (bd slot)
	
	/*
	 * If PA interrupt still asserted or sw1, loop arround
	 */
	mfc0	s0,CP0_CAUSE			# read cause register
	li	s1,(CAUSE_IBIT3|CAUSE_IBIT1)	# get mask for PA intr,sw1
	and	s0,s0,s1			# check IP4,IP1

	bne	s0,zero,level1_play_it_again    # start all over
	nop					# (bd slot)

vip_level1_dismiss:

	/*
	 * Call the cleanup routine
	 */
	jal	vip_idle_chores			# call idle chore handler
	nop					# (bd slot)

        /*
         * Restore context and dismiss a level 1 interrupt
         *      - k0 has previous level
         */
	dmfc1	v0,FPU_V0_REG			# restore v0
	dmfc1	v1,FPU_V1_REG			# restore v1
	dmfc1	a0,FPU_A0_REG			# restore a0
	dmfc1	a1,FPU_A1_REG			# restore a1
	dmfc1	a2,FPU_A2_REG			# restore a2
	dmfc1	a3,FPU_A3_REG			# restore a3
        dmfc1   t4,FPU_T4_REG                   # restore t4
        dmfc1   t5,FPU_T5_REG                   # restore t5
        dmfc1   t6,FPU_T6_REG                   # restore t6
        dmfc1   t7,FPU_T7_REG                   # restore t7
        dmfc1   t8,FPU_T8_REG                   # restore t8
        dmfc1   t9,FPU_T9_REG                   # restore t9
 
        dmfc1   t1,FPU_SREG_REG                 # get status reg
        dmfc1   ra,FPU_RA_REG                   # restore ra
        dmfc1   s0,FPU_S0_REG                   # restore s0
        dmfc1   s1,FPU_S1_REG                   # restore s1
 
        /*
         * Note: The R4400 processor does not like s/w to change
         * the state of EXL with IE enabled, so disable it during change.
         */
        mfc0    t2,CP0_SREG                     # read sreg
        li      t3,~SREG_IE                     # get IE bit mask
 
        and     t2,t2,t3                        # disable IE bit
        mtc0    t2,CP0_SREG                     # re-write sreg (EXL=0,IE=0)
 
        and     t3,t3,t1                        # disable IE in sreg copy
        mtc0    t3,CP0_SREG                     # re-write sreg (EXL=1,IE=0)
        nop                                     # let sreg settle
 
        mtc0    t1,CP0_SREG                     # restore sreg (EXL=1,IE=1)
 
        dmfc1   s2,FPU_S2_REG                   # restore s2
        dmfc1   s3,FPU_S3_REG                   # restore s3

	/*
	 * We need to restore the interrupted level. There is
	 * no need to restore the external mask as it does not change
	 * between level 0 and level 1.
	 */
	sw	k0,r4k_cpu_level		# restore old level

	/* 
	 * Enable DMA timer 
	 */
	la	t0,IO_CCR_REG                   # DMA timer control register
	li	t1,CCR_CT2_EN                   # control for timer 2
	sb	t1,0(t0)                        # enable timer

	/* Restore volatile registers */
        dmfc1   t0,FPU_EPC_REG                  # get EPC reg
        dmfc1   t2,FPU_LO_REG                   # get lo
        dmfc1   t3,FPU_HI_REG                   # get hi
 
        dmtc0   t0,CP0_EPC                      # restore EPC reg
        mtlo    t2                              # restore LO
        mthi    t3                              # restore HI
 
        dmfc1   sp,FPU_SP_REG                   # restore sp
        dmfc1   t0,FPU_T0_REG                   # restore t0
        dmfc1   t1,FPU_T1_REG                   # restore t1
        dmfc1   t2,FPU_T2_REG                   # restore t2
        dmfc1   t3,FPU_T3_REG                   # restore t3
 
        dmfc1   AT,FPU_AT_REG                   # restore AT
        dmfc1   k0,FPU_K0_REG                   # restore k0
        dmfc1   k1,FPU_K1_REG                   # restore k1
 
	/*
	* Dismiss the interrupt
	*/
	eret                                    # return
	nop                                     # not executed

	.set reorder
	.set at
ENDFRAME(vip_pa_lo_intr)

	.extern dma_working,4
	.extern dma_reqt,4

/*
 * rvip_dma_jumpstart:
 * Jumpstart the dma engine. This is called when something is
 * queued on the request queue and the dma_working flag is FALSE.
 * It is responsible for looking at the dma_reqt status. If it is
 * RVIP_REQUEST_DMA_ENTRY, then start that request.
 */
 #
 # int rvip_dma_jumpstart (void)
 #
FRAME(rvip_dma_jumpstart,sp,0,ra)
	.set noreorder
	.set noat

	/*
	 * Zero the work counter
	 */
	move	v0,zero

loop_next_req:
	/*
	 * See if there is another one
	 */
	lw	t0,dma_reqt			# get reqt pointer
	lhu	t1,DMA_ENTRY_OFS_STATUS(t0)	# read status
	andi	t2,t1,RVIP_REQUEST_DMA_ENTRY	# mask off the request bit

	bne	t2,zero,work_todo		# something to do ?
	sw	zero,dma_working		# clear dma_working (bd slot)

	/*
	 * There is no work left to do. If there was no work
	 * at all, v0 will be zero and our caller will print a message.
	 * Assert sw1 to reduce latency on completed transfers
	 */
	li	t7,CAUSE_IBIT1			# sw 1 interrupt bit

	j	ra				# return
	mtc0	t7,CP0_CAUSE			# assert interrupt (bd slot)

work_todo:
	/*
	 * Increment the work counter
	 */
	addiu	v0,v0,1

	/*
	 * Make sure this one did not have the by-hand bit set
	 */
	andi	t2,t1,RVIP_MANUAL_DMA_ENTRY

	beq	t2,zero,not_manual		# was it done by hand ?
	nop					# (bd slot)

	/*
	 * Bump the dma_reqt and see if there is another request
	 */
	lw	t3,DMA_ENTRY_OFS_NEXT(t0)	# read t0->next

	b	loop_next_req			# try and do another one
	sw	t3,dma_reqt			# save new request head (bd)

not_manual:
	/*
	 * Process the entry. Pointer is in t0. Status is in t1.
	 */
	lhu	t2,DMA_ENTRY_OFS_LENGTH(t0)	# t2 = t0->length

	/* 
	 * Write entry into dma engine. Write pci address,
	 * check direction and write memd addr, length.
	 * Pointer is in t0, status is in t1. Length is in t2.
	 */
	li	t7,0x01				# get constant 1
	sw	t7,dma_working			# set dma_working to TRUE

	lw	t3,DMA_ENTRY_OFS_PCI_ADDR(t0)	# get PCI address	
	la	t7,ADRSPC_DMA			# get DMA reg address

	sw	t3,4(t7)			# write register 1
	andi	t1,t1,RVIP_DMA_FROM_MEMD	# and with RVIP_DMA_FROM_MEMD

	beq	t1,zero,data_to_memd		# which direction ?
	nop					# (bd slot)

	/*
	 * This dma is from memd
	 */
	lw	t3,DMA_ENTRY_OFS_MEMD_ADDR(t0)
	li	t6,(RVIP_DMA_DIRECTION_TX|RVIP_DMA_INT_ENABLE)
	or	t3,t3,t6
	sw	t3,0(t7)			# write register 0

	/*
	 * Finally setup the length and start it. The length is in t2.
	 */
	addiu	t2,t2,-1			# subtract 1 (chip weirdness)
	sll	t2,t2,16			# shift 16
	
	b	dma_jumpstart_done		# jump to return code
	sw	t2,8(t7)			# write register 2 (bd slot)
			
data_to_memd:
	/*
	 * This dma is to memd. Pointer is in t0, pci_addr is written.
	 * Write the memd_addr and the length.
	 */
	lw	t3,DMA_ENTRY_OFS_MEMD_ADDR(t0)
	li	t6,(RVIP_DMA_INT_ENABLE)
	or	t3,t3,t6
	sw	t3,0(t7)			# write register 0

	/*
	 * Finally setup the length and start it. The length is in t2.
	 */
	addiu	t2,t2,-1			# subtract 1 (chip weirdness)
	sll	t2,t2,16			# shift 16
	sw	t2,8(t7)			# write register 2 (bd slot)

dma_jumpstart_done:
	/*
	 * Assert sw1 to reduce latency on completed transfers
	 */
	li	t7,CAUSE_IBIT1			# sw 1 interrupt bit

	j	ra				# return
	mtc0	t7,CP0_CAUSE			# assert interrupt (bd slot)

	.set reorder
	.set at
ENDFRAME(rvip_dma_jumpstart)

/*
 * vip_dma_intr:
 * Handle a Level 2 DMA Interrupt. Note: this handler runs at one level
 * higher than ALL_DISABLE, so it is not allowed touch buffers directly,
 * it is constrained to moving requests from the req queue to the done queue.
 *
 * Upon entry:
 *	k0 saved in FPU_R0
 *	k1 saved in FPU_R1
 *	AT saved in FPU_R2
 *	sp saved in AT
 */
 #
 # void vip_dma_intr (void)
 #
	.extern l2stk,4
	.extern l2cnt,4
	.extern lev2subr,4

FRAME(vip_dma_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 2 stack, and lev2subr vector
	 */
	lw	sp,l2stk			# move onto level 2 stack
	lw	k0,l2cnt			# load counter

        addiu   sp,sp,-R4K_CONTEXT_FRAME        # make a frame on new stack
	addiu	k0,k0,1			# increment counter

	SAVE_T0_T3				# save t0..t3

	/*
	 * Read the DMA error register and make sure all is well
	 */
	la	t0,DMA_ERR_REG			# get DMA error reg addr
	lw	t1,0(t0)			# get error indication
	sw	k0,l2cnt			# save counter
	
	li	t2,0x00020000			# get mask
	and	t2,t2,t1
	
	beq	zero,t2,dma_normal		# is there an error ?
	nop					# (bd slot)

	/* 
         * Got an error indication - get ready for c code
	 */
	j	dma_error
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

dma_normal:

	/*
	 * Mark the dma_reqt entry as RVIP_DONE_DMA_ENTRY. Move dma_reqt
	 * and try and issue another one.
	 */
	lw	k1,dma_reqt			# k1 = dma_reqt
	lhu	t1,DMA_ENTRY_OFS_STATUS(k1)	# read status
	lw	t3,DMA_ENTRY_OFS_NEXT(k1)	# t3 = t0->next

	ori	t1,t1,RVIP_DONE_DMA_ENTRY	# mask it as done
	sh	t1,DMA_ENTRY_OFS_STATUS(k1)	# write the status
	sw	t3,dma_reqt			# save new request head
	sw	zero,dma_working		# mark not busy

dma_loop_next_req:

	/*
	 * See if there is another piece of work at dma_reqt
	 */
	lw	k1,dma_reqt			# get dma_reqt pointer
	lhu	t1,DMA_ENTRY_OFS_STATUS(k1)	# read status
	andi	t2,t1,RVIP_REQUEST_DMA_ENTRY	# mask off the request bit

	bne	t2,zero,dma_work_todo		# something to do ?
	sw	zero,dma_working		# clear dma_working (bd slot)

	/*
	 * There is no work to do.
	 */
	b	dma_fast_fini			# jump to the cleanup code
	nop					# (bd slot)

dma_work_todo:
	/*
	 * Make sure this one did not have the by-hand bit set
	 */
	andi	t2,t1,RVIP_MANUAL_DMA_ENTRY

	beq	t2,zero,dma_not_manual		# was it done by hand ?
	nop					# (bd slot)

	/*
	 * Bump the dma_reqt and see if there is another request
	 */
	lw	t3,DMA_ENTRY_OFS_NEXT(k1)	# read k1->next

	b	dma_loop_next_req		# try and do another one
	sw	t3,dma_reqt			# save new request head (bd)

dma_not_manual:
	/*
	 * Process the entry. Pointer is in k1. Status is in t1.
	 */
	lhu	t2,DMA_ENTRY_OFS_LENGTH(k1)	# t2 = k1->length

	/* 
	 * Write entry into dma engine. Write pci address,
	 * check direction and write memd addr, length.
	 * Pointer is in k1, status is in t1. Length is in t2.
	 */
	li	k0,0x01				# get constant 1
	sw	k0,dma_working			# set dma_working to TRUE

	lw	t3,DMA_ENTRY_OFS_PCI_ADDR(k1)	# get PCI address	
	la	k0,ADRSPC_DMA			# get DMA reg address

	sw	t3,4(k0)			# write register 1
	andi	t1,t1,RVIP_DMA_FROM_MEMD	# and with RVIP_DMA_FROM_MEMD

	beq	t1,zero,dma_to_memd		# which direction ?
	nop					# (bd slot)

	/*
	 * This dma is from memd
	 */
	lw	t3,DMA_ENTRY_OFS_MEMD_ADDR(k1)
	li	t0,(RVIP_DMA_DIRECTION_TX|RVIP_DMA_INT_ENABLE)
	or	t3,t3,t0
	sw	t3,0(k0)			# write register 0

	/*
	 * Finally setup the length and start it. The length is in t2.
	 */
	addiu	t2,t2,-1			# subtract 1 (chip weirdness)
	sll	t2,t2,16			# shift 16
	
	b	dma_fast_fini			# jump to return code
	sw	t2,8(k0)			# write register 2 (bd slot)
			
dma_to_memd:
	/*
	 * This dma is to memd. Pointer is in k1, pci_addr is written.
	 * Write the memd_addr and the length. Length is in t2. DMA register
	 * pointer is in k0.
	 */
	lw	t3,DMA_ENTRY_OFS_MEMD_ADDR(k1)
	li	t0,(RVIP_DMA_INT_ENABLE)
	or	t3,t3,t0
	sw	t3,0(k0)			# write register 0

	/*
	 * Finally setup the length and start it. The length is in t2.
	 */
	addiu	t2,t2,-1			# subtract 1 (chip weirdness)
	sll	t2,t2,16			# shift 16
	sw	t2,8(k0)			# write register 2 (bd slot)

dma_fast_fini:
	/*
	 * Assert sw1 to reduce latency on completed transfers
	 */
	li	t2,CAUSE_IBIT1			# sw 1 interrupt bit
	mtc0	t2,CP0_CAUSE			# assert interrupt

	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0
	ld	t1,SP_OFS(REG_T1)(sp)		# restore t1
	ld	t2,SP_OFS(REG_T2)(sp)		# restore t2
	ld	t3,SP_OFS(REG_T3)(sp)		# restore t3

	move	sp,AT				# restore sp

	dmfc1	k0,FPU_R0			# restore k0
	dmfc1	k1,FPU_R1			# restore k1
	dmfc1	AT,FPU_R2			# restore AT
	
	/*
	 * Dismiss the interrupt
	 */
	eret					# return
	nop					# not executed

dma_error:

	/* 
	 * Call C code handler for errors and cbus stalls
	 * save context normally 
	 */
	SAVE_VOLATILE_REGS

	/*
 	 * Install our level and new nevada mask
	 */
	lw	k0,r4k_cpu_level		# read old value
	li	t2,(LEVEL2 << R4K_LEVEL_SHIFT)	# get new level (bits [5..3])
	sw	t2,r4k_cpu_level		# register new value

	la	t2,r4k_level_table		# get level table address
	lbu	t3,(4+(8*LEVEL2))(t2)		# read new mask value
	la	t0,IO_IMASK_REG			# get interrupt mask reg addr
	sb	t3,0(t0)			# write nevada mask
	lbu     t3,0(t0)                        # read imask register (to
        	                     		# sync the posted writes)

	/*
	 * Note : There seems to be a problem with the processor when EXL 
	 * gets de-asserted while IE is asserted and there is another
	 * interrupt pending (the interrupt is taken but the EPC register
	 * does not get re-latched). To work arround this, disable IE,
	 * reset the EXL bit, and then re-enable IE.
	 */
	li	t3,~SREG_IE			# get IE mask
	and	t3,t3,t1			# de-assert IE
	mtc0	t3,CP0_SREG		# move to status register

	li	t2,LEVEL2_SREG_MASK		# get sreg mask
	and	t0,t2,t3			# insert new bits
	mtc0	t0,CP0_SREG		# write new sreg copy

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t0,t2,t1			# and into original sreg
	ori	t0,t0,SREG_IE		# set IE
	mtc0	t0,CP0_SREG		# write sreg (EXL=0,IE=1)
	nop					    # let it settle

	/*
	 * save some more context
	 */
	SAVE_V0_A3				/* save v0,v1,a0..a3 */
	SAVE_T4_RA				/* save t4..t9,ra */

	lw      AT,lev2subr                     # get handler

	jal	AT				    # call handler
	nop					    # (bd slot)

        /*
         * restore the context. We restore the stacked status register
         * which guarantees that EXL will be asserted through critical
         * section
         *
         * Note: The R4400 processor does not like s/w to change
         * the state of EXL with IE enabled, so disable it during change.
         */
        RESTORE_V0_A3                           /* restore v0,v1,a0..a3 */
 
        lw      t5,SP_OFS(REG_SREG)(sp)         # load Status reg
	ld	t0,SP_OFS(REG_T0)(sp)           # restore t0

        mfc0    t6,CP0_SREG                     # read sreg
        li      t4,~SREG_IE                     # get IE mask
 
        and     t6,t6,t4                        # disable IE bit
        mtc0    t6,CP0_SREG                     # re-write sreg (EXL=0,IE=0)
 
        and     t7,t5,t4                        # disable IE in sreg copy
        mtc0    t7,CP0_SREG                     # re-write sreg (EXL=1,IE=0)
        nop                                     # let sreg settle
 
        mtc0    t5,CP0_SREG                     # restore sreg (EXL=1)
        nop                                     # let sreg settle
 
	RESTORE_T1_T3_EPC_LO_HI			

	/*
	 * restore old cpu level and nevada mask
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
        RESTORE_T4_RA                           /* restore t4..t9, ra */
        RESTORE_K0_K1_AT                        /* restore k0, k1, AT */
 
        /*
         * move back onto the original stack
         * We dont have to increment the stack pointer
         */
        ld      sp,SP_OFS(REG_SP)(sp)           # restore old sp

	/*
	 * Dismiss the interrupt
	 */
	eret					# return
	nop					# not executed

	.set	reorder
	.set	at
ENDFRAME(vip_dma_intr)

/*
 * vip_dbus_intr:
 * Handle a Level 6 Interrupt (Dbus)
 */
 #
 # void vip_dbus_intr (void)
 #
	.extern l6stk,4
	.extern l6cnt,4
	.extern lev6subr,4

FRAME(vip_dbus_intr,sp,0,ra)
	.set	noreorder
	.set	noat

	/*
	 * This routine uses the level 6 stack
	 * Note : sp has already been copied to AT
	 */
	lw	sp,l6stk			# move onto level 6 stack
	lw	k0,l6cnt			# load counter
	
	addiu	sp,sp,-R4K_CONTEXT_FRAME	# make a frame
	sd	AT,SP_OFS(REG_SP)(sp)		# save old sp

	addiu	k0,k0,1				# increment counter
	sw	k0,l6cnt			# save the counter

	SAVE_T0_T3				/* save t0..t3 */

	/*
	 * Save the interrupted level and install the new level
	 */
	lw	k0,r4k_cpu_level		# read interrupted level
	li	t3,(LEVEL6 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t3,r4k_cpu_level		# save new level

	SAVE_VOLATILE_REGS			/* save volatile registers */

	/*
	 * Re-enable interrupts so Malu_Attention can be serviced
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

	li	t2,LEVEL6_SREG_MASK		# get sreg mask
	and	t0,t2,t3			# and sreg copy with mask
	mtc0	t0,CP0_SREG			# EXL=0, IE=0

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	and	t0,t2,t1			# and it into sreg
	ori	t0,t0,SREG_IE			# set IE
	mtc0	t0,CP0_SREG			# write back sreg with EXL=0

	lw	t0,lev6subr			# get handler address

	SAVE_V0_A3				/* save v0,v1,a0..a3 */
	SAVE_T4_RA				/* save t4..t9,ra */

	/*
	 * Call the dbus handler
	 */
	jal	t0				# call level 6 handler
	nop					# (bd slot)

	/*
	 * restore the context. We restore the stacked status register
	 * which guarantees that EXL will be asserted through critical 
	 * section
	 *
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	RESTORE_V0_A3				/* restore v0,v1,a0..a3 */

	lw	t5,SP_OFS(REG_SREG)(sp)		# load Status reg
	ld	t0,SP_OFS(REG_T0)(sp)		# restore t0

	mfc0	t6,CP0_SREG			# read sreg
	li	t4,~SREG_IE			# get IE mask

	and	t6,t6,t4			# disable IE bit
	mtc0	t6,CP0_SREG			# re-write sreg (EXL=0,IE=0)

	and	t7,t5,t4			# disable IE in sreg copy
	mtc0	t7,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let sreg settle

	mtc0	t5,CP0_SREG			# restore sreg
	nop					# let sreg settle

	RESTORE_T1_T3_EPC_LO_HI			/* restore t1..t3,EPC,Lo,Hi */

	/*
	 * restore old cpu level
	 */
	sw	k0,r4k_cpu_level		# restore old level

	RESTORE_T4_RA				/* restore t4..t9,ra */
	RESTORE_K0_K1_AT			/* restore k0,k1,AT */

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
ENDFRAME(vip_dbus_intr)

/*
 * rvip_cbus_intr:
 * Handle a level 7 CBUS command interrupt
 */
	.extern l7cnt,4
	.extern l7stk,4
	.extern lev7subr,4

FRAME(rvip_cbus_intr,sp,96,ra)
	.set	noreorder
	.set	noat

	/*
	 * This interrupt handler runs on the level7 stack
	 */
	lw	sp,l7stk			# move onto level 7 stack
	lw	k0,l7cnt			# load counter

	/*
	 * Make a frame. Note: We deliberately make a frame 
	 * that is at least 32 bytes bigger then we need so
	 * that the create dirty exclusive does not wipe out
	 * other valuable information.
	 */
	addiu	sp,sp,-VIP_CBUS_STACK_SIZE	# make a frame

	/*
	 * Create cache lines - saves cache line fills
	 */
	cache	3*4+1,64(sp)			# avoid loading cache line
	cache	3*4+1,32(sp)			# avoid loading cache line
	cache 	3*4+1,0(sp)			# avoid loading cache line

	addiu	k0,k0,1				# increment counter
	sw	k0,l7cnt			# save the counter

	/*
	 * Save sp,k0,k1,AT,t0..t3 as working registers
	 */
	sd	AT,PUSH_SP_OFS(sp)		# save old sp
	sdc1	FPU_R0,PUSH_K0_OFS(sp)		# save k0 copy
	sdc1	FPU_R1,PUSH_K1_OFS(sp)		# save k1 copy
	sdc1	FPU_R2,PUSH_AT_OFS(sp)		# save AT copy
	sd	t0,PUSH_T0_OFS(sp)		# save t0
	sd	t1,PUSH_T1_OFS(sp)		# save t1
	sd	t2,PUSH_T2_OFS(sp)		# save t2
	sd	t3,PUSH_T3_OFS(sp)		# save t3
	sd	t4,PUSH_T4_OFS(sp)		# save t4

	/*
	 * Save volatile registers (SREG, EPC)
	 */
	sd	ra,PUSH_RA_OFS(sp)		# save ra
	dmfc0	t0,CP0_EPC			# get EPC register
	mfc0	t1,CP0_SREG			# get status register
	sd	t0,PUSH_EPC_OFS(sp)		# save EPC
	sw	t1,PUSH_SREG_OFS(sp)		# save SREG

	/*
	 * Re-enable exceptions (to handle cbus stall)
	 */
	li	t3,~SREG_IE			# get IE mask
	and	t3,t3,t1			# de-assert IE
	mtc0	t3,CP0_SREG			# write to SREG

	li	t2,LEVEL7_SREG_MASK		# get SREG mask for level 7
	and	t2,t2,t3			# clear IE and EXL
	mtc0	t2,CP0_SREG			# set SREG

	la	k1,memd_cntl			# get address of memd_cntl
	lw	t4,MEMD_CNTL_CCB(k1)		# read ccbptr

	lw	t0,CCB_CMDARG0(t4)		# read cmd/arg0

	/*
	 * Clear the interrupt
	 */
	la	t1,ADRSPC_DBUS			# get DBUS register address
	sh	zero,0x18(t1)			# clear interrupt

	jal	vip_cbus_intr
	nop

	/*
	 * Note: The R4400 processor does not like s/w to change
	 * the state of EXL with IE enabled, so disable it during change.
	 */
	lw	t1,PUSH_SREG_OFS(sp)		# load Status reg
	ld	t0,PUSH_EPC_OFS(sp)		# load EPC

	mfc0	t2,CP0_SREG			# read status register
	li	t3,~SREG_IE			# get IE mask
	
	and	t2,t2,t3			# disable IE
	mtc0	t2,CP0_SREG			# write status register
	nop					# let it settle

	and	t3,t3,t1			# disable IE in sreg copy
	mtc0	t3,CP0_SREG			# re-write sreg (EXL=1,IE=0)
	nop					# let it settle

	mtc0	t1,CP0_SREG			# re-write original sreg

	ld	t2,PUSH_T2_OFS(sp)		# restore t2
	ld	t3,PUSH_T3_OFS(sp)		# restore t3
	ld	t4,PUSH_T4_OFS(sp)		# restore t4
	dmtc0	t0,CP0_EPC			# restore EPC

	ld	t1,PUSH_T1_OFS(sp)		# restore t1
	ld	t0,PUSH_T0_OFS(sp)		# restore t0

	/*
	 * Restore k0,k1,AT,ra,sp
	 */
	ld	k1,PUSH_K1_OFS(sp)		# restore k1
	ld	k0,PUSH_K0_OFS(sp)		# restore k0
	ld	AT,PUSH_AT_OFS(sp)		# restore AT
	ld	ra,PUSH_RA_OFS(sp)              # restore ra
	ld	sp,PUSH_SP_OFS(sp)		# restore old sp
	/*
	 * dismiss the interrupt
	 */
	eret					# return
	nop					# does not get executed

	.set	reorder
	.set	at
ENDFRAME(rvip_cbus_intr)

 #
 # boolean pending_cbus_dbus_intrpt (void)
 #
FRAME(pending_cbus_dbus_intrpt,sp,0,ra)
        .set    noreorder
 
        mfc0    v0,CP0_CAUSE    # v0 = CP0 Cause
	li	a0,CAUSE_IBIT6|CAUSE_IBIT5
        j       ra		# return
        and     v0,v0,a0	# (bd slot)
 
        .set    reorder
ENDFRAME(pending_cbus_dbus_intrpt)


#ifdef BROKEN_CHECKERS_ROM

/*
 * set_checkers_memsize:
 * Program the Nevada to realize that there is 8 meg of DRAM.
 * This is a hack work-arround for bugs in the checkers rom
 * where by this register gets set incorrectly.
 *
 * Notes: Cannot use virtual addresses, as the virtual address
 *        map may not be set up yet.
 */
FRAME(set_checkers_memsize,sp,0,ra)
	.set	noreorder

	/*
	 * Disable interrupts
	 */
	mfc0	t8,CP0_SREG		# read status register
	li	t7,~SREG_IE		# get IE mask
	and	t7,t8,t7		# disable IE
	mtc0	t7,CP0_SREG		# write back status register
	nop				# let it settle

	/*
	 * Get nev->cfgb address and read it
	 */
	la	t0,PHY_TO_KSEG1(PHY_ADRSPC_NEV_ASIC)
	lhu	t1,0x208(t0)

	/*
	 * We need to align to a cache line boundary
	 * so that *no* dram activity is happening
	 * while we change the nevada register
	 */
	.align	5

	ori	t1,t1,0x2		# insert DRAM size bits
	sh	t1,0x208(t0)		# write it back
	nop				# let it settle

	lhu	t0,0x208(t0)		# read it back to sync the write
	nop				# load delay

	j	ra			# return
	mtc0	t8,CP0_SREG		# restore original sreg (bd slot)

	.set	reorder
ENDFRAME(set_checkers_memsize)

#endif /* BROKEN_CHECKERS_ROM */

/*
 * Table of interrupt masks for Status register and Nevada Register
 *
 * Note, Nevada mask is inverse polarity IE 0->enabled, 1->disabled.
 * The sw interrupts 0 is enabled for all levels except
 * level 7. sw interrupt 1 is only enabled for level 0.
 */
 #
 # struct levels {
 #	ulong s_reg;
 #	uchar nev_mask;
 #	uchar pad[3];
 # } r4k_level_table[8];
 #
	.globl	r4k_level_table

	.text
	.align	3

r4k_level_table:

/*
 *	Bit Definitions				
 *	---------------
 *	IP 7	- Internal Timer	Nevada Bit 7 : Unused
 *	IP 6	- Dbus			Nevada Bit 6 : PCI Present/Stall
 *	IP 5	- Cbus			Nevada Bit 5 : External Timer
 *	IP 4	- DMA			Nevada Bit 4 : DRAM Write error
 *	IP 3	- PA LO			Nevada Bit 3 : IO Write error
 *	IP 2 	- Nevada		Nevada Bit 2 : Unused
 *	IP 1	- Fake N/W Int		Nevada Bit 1 : PMAC
 *	IP 0	- GDB			Nevada Bit 0 : PCI management
 *
 * Software levels:
 *	1	PA Lo, Fake N/W Int
 *	2	DMA
 *	3	Nevada PA management, Nevada timer
 *	4	Nevada Bus Errors, GDB
 *	5	Orion Timer
 *	6	Dbus
 *	7	Cbus
 *
 *			0 : Masked	1 : Masked
 *			1 : Enabled	0 : Enabled
 *
 *	Level		Status Reg	Nevada Mask
 *			IP 7..0		Bit 7..0
 *	---------------------------------------------
 *                      76543210        76543210
 *	0		11111111	11000100
 *	1		11110101	11000100
 *	2		11100101	11000100
 *	3		11100101	11100101
 *	4		11100000	11111111
 *	5		01100000	11111111
 *	6		00100000	11111111
 *	7		00000000	11111111
 */
	.word	0x0000ff00, 0xc4000000		# Level 0
	.word	0x0000f500, 0xc4000000		# Level 1
	.word	0x0000e500, 0xc4000000		# Level 2
	.word	0x0000e500, 0xe5000000		# Level 3
	.word	0x0000e000, 0xff000000		# Level 4
	.word	0x00006000, 0xff000000		# Level 5
	.word	0x00002000, 0xff000000		# Level 6
	.word	0x00000000, 0xff000000		# Level 7

/* end of file */
