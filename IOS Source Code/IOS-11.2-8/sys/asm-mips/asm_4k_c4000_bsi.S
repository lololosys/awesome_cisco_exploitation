/* $Id: asm_4k_c4000_bsi.S,v 3.2.60.1 1996/08/08 21:06:05 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_c4000_bsi.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000_bsi.S - FDDI Wrappers for Sierra
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000_bsi.S,v $
 * Revision 3.2.60.1  1996/08/08  21:06:05  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:40:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * BSI_MAR_BITS
 */
#define BSI_MAR_OFFSET	0x509
#define BSI_MNR_OFFSET	0x50B

/*
 * fddi_wrapper:
 * FDDI wrapper (Lo IRQ)
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
 # void fddi_wrapper (void)
 #
FRAME(fddi_wrapper,sp,0,s1)
	.set	noreorder

	/*
	 * Read the IACK register. If bit 0 is set, it is a cam
	 * interrupt, else it is a fddi IO interrupt
	 */
	lbu	t0,0(s2)			# read IACK register
	nop					# load delay

	beq	t0,zero,fddi_io			# is it io ?
	lw	s2,LEV2_IDB_A(s0)		# read idb pointer (bd slot)

cam:
	/*
	 * The cam interrupted us
	 */
	jal	cam_interrupt			# call cam interrupt routine
	move	a0,s2				# setup idb param (bd slot)

	j	s1				# return
	nop					# (bd slot)

fddi_io:
	/*
	 * FDDI IO interrupt
	 */
	lbu	t1,BSI_MAR_OFFSET(a1)		# read MAR register
	lbu	t2,BSI_MNR_OFFSET(a1)		# read MNR register
	nop					# load delay
	and	t3,t1,t2			# and t1 and t2
	
	beq	t3,zero,no_interrupt		# is it for real
	nop					# (bd slot)

	move	a0,t1				# setup mar param
	jal	bsi_interrupt			# call bsi interrupt routine
	move	a1,s2				# setup idb param (bd slot)

no_interrupt:
	j	s1				# return
	nop					# (bd slot)

	.set	reorder
ENDFRAME(fddi_wrapper)

/*
 * define some externs so the asm will use gp relative addressing
 */
	.extern r4k_cpu_level,4
	.extern int_count,4
	.extern schedflag,4
	.extern current_count,4
	.extern profile_enabled,4
	.extern wd_save_return,4
	.extern	r4k_4ms_pipe_ticks,4

/*
 * r4k_rt_timer_interrupt:
 * Handle Real Timer 4ms NMI tick.
 */
 
 #
 # void r4k_rt_timer_interrupt (leveltype oldlevel, void *pc)
 #
FRAME(r4k_rt_timer_interrupt,sp,32,ra)
	.set	noreorder

	/*
	 * Save context
	 */
	addiu	sp,sp,-32		# make a stack frame
	sw	ra,16(sp)		# save ra
	sw	a0,20(sp)		# save a0 (interrupted level)
	sw	a1,24(sp)		# save a1 (interrupted pc)

	/*
	 * Update count/compare and de-assert interrupt source
	 */
	mfc0	t0,CP0_COMPARE		# read compare register
	lw	t1,r4k_4ms_pipe_ticks	# get 4ms worth of ticks
	addu	t0,t0,t1		# increment value
	mtc0	t0,CP0_COMPARE		# write new value (de-assert intr)

	/*
	 * We must install level 7 priorities and masks
	 * and de-assert EXL bit (allow reentrancy)
	 */
	li	t4,(LEVEL7 << R4K_LEVEL_SHIFT)	# new level (bits [5..3])
	sw	t4,r4k_cpu_level		# save new level
	mfc0	t3,CP0_SREG			# read status register
	li	t4,~(CAUSE_IP23456|SREG_EXL)	# get new sreg mask
	and	t3,t3,t4			# de-assert EXL and IP[6..2]

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	ori	t3,t3,SREG_IE			# set IE
	mtc0	t3,CP0_SREG			# store status reg
	nop					# let sreg settle

	/*
	 * Call the Real Time timer interrupt handler.
	 */
	jal	rt_timer_int_hand	# call the xdi handler
	nop				# bd slot

	/*
	 * Increment the clock variables.
	 */
	la	t1,r4k_hclock		# get clock address
	ld	t0,0(t1)		# read the clock
	nop				# load delay slot
	daddiu	t0,t0,REFRESHTIME	# increment the clock
	sd	t0,0(t1)		# save the new clock

	/*
	 * Stroke the watchdog. 
	 */
	lw	a0,20(sp)		# restore a0
	la	t1,IO_WATCHDOG_REG	# get register address
	li	t2,REFRFREQUENCY*4	# 16ms
	sh	t2,0(t1)		# stroke the watch dog

	/*
	 * See if we interrupted an interrupt
	 */
	beq	a0,zero,not_interrupt	# did we interrupt an interrupt ?
	lw	t2,int_count		# read int_count (bd slot)

	addiu	t2,t2,1			# increment int_count

	b	idle_out		# jump past process level stuff
	sw	t2,int_count		# save int_count (bd slot)
	
not_interrupt:
	lw	t0,schedflag		# read schedflag

	beq	t0,zero,task_running	# task is running ?
	lw	t2,current_count	# read current_count (bd slot)

	addiu	t2,t2,1			# increment current_count
	b	idle_out		# branch past process level stuff
	sw	t2,current_count	# save current_count (bd slot)

task_running:
	jal	count_process_tick	# count it against current process
	nop				# (bd slot)

	beq	v0,zero,idle_out	# did the watchdog bark?

	ld	t1,(SP_OFS(REG_EPC)+32)(sp) # get old return address
	la 	t0,watchdog_timed_out	# get handler address
	sw	t1,wd_save_return	# Save current return address
	sd 	t0,(SP_OFS(REG_EPC)+32)(sp) # Fudge exception frame to cause
					    # the process to return to the
					    # watchdog handler

idle_out:
	jal	clock_tick		# run NTP clock
	nop				# (bd slot)

	lw	t0,profile_enabled	# get profile_enabled boolean
	nop				# load delay

	beq	t0,zero,no_profile	# is profiling enabled?
	lw	a1,24(sp)		# param 2 (pc) (bd slot)

	jal	profile_pc		# call profile_pc
	lw	a0,20(sp)		# param 1 (interrupt_state) (bd slot)

no_profile:
	/*
	 * Call the C coded routine to handle the UART break key
	 */
	jal	console_check_break	# call the C code handler
	nop				# (bd slot)

	beq	v0,zero,no_break	# do we need to abort ?
	addiu	a2,sp,32		# get context pointer (bd slot)

	/*
	 * We are going back to the monitor. Lets save all
	 * our remaining context so that the context command of
	 * the monitor does not lie IE s0..s8,gp,zero
	 */
	sd	s0,SP_OFS(REG_S0)(a2)		# save s0
	sd	s1,SP_OFS(REG_S1)(a2)		# save s1
	sd	s2,SP_OFS(REG_S2)(a2)		# save s2
	sd	s3,SP_OFS(REG_S3)(a2)		# save s3
	sd	s4,SP_OFS(REG_S4)(a2)		# save s4
	sd	s5,SP_OFS(REG_S5)(a2)		# save s5
	sd	s6,SP_OFS(REG_S6)(a2)		# save s6
	sd	s7,SP_OFS(REG_S7)(a2)		# save s7
	sd	s8,SP_OFS(REG_S8)(a2)		# save s8
	sd	zero,SP_OFS(REG_ZERO)(a2)	# save zero
	sd	gp,SP_OFS(REG_GP)(a2)		# save gp

	move	a1,zero			# setup param 2
	jal	r4k_return_to_monitor	# return to the monitor
	ori	a0,zero,SIGQUIT		# setup param 1 (bd slot)

	/*
	 * If the user wants to "continue" the image, the monitor
	 * will return us here.
	 */
	addiu	a2,sp,32
	ld	s0,SP_OFS(REG_S0)(a2)		# restore s0
	ld	s1,SP_OFS(REG_S1)(a2)		# restore s1
	ld	s2,SP_OFS(REG_S2)(a2)		# restore s2
	ld	s3,SP_OFS(REG_S3)(a2)		# restore s3
	ld	s4,SP_OFS(REG_S4)(a2)		# restore s4
	ld	s5,SP_OFS(REG_S5)(a2)		# restore s5
	ld	s6,SP_OFS(REG_S6)(a2)		# restore s6
	ld	s7,SP_OFS(REG_S7)(a2)		# restore s7
	ld	s8,SP_OFS(REG_S8)(a2)		# restore s8
	ld	gp,SP_OFS(REG_GP)(a2)		# restore gp

no_break:
	/*
	 * return
	 */
	lw	ra,16(sp)		# restore ra
	addiu	sp,sp,32		# increment stack
	j	ra			# return
	nop				# (bd slot)

	.set	reorder
ENDFRAME(r4k_rt_timer_interrupt)

/* End of file */ 




#Local Variables:
#comment-column: 40
#comment-start: "# "
#comment-end: ""
#comment-multi-line: nil
#comment-start-skip: "# "
#End:


