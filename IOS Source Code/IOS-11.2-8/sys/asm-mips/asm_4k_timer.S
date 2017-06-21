/* $Id: asm_4k_timer.S,v 3.3.6.2 1996/08/08 21:06:18 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_timer.S,v $
 *------------------------------------------------------------------
 *
 * asm_4k_timer.S - R4k asm support for internal count/compare
 *	  	    timer
 *
 * Michael Beesley, October 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_4k_timer.S,v $
 * Revision 3.3.6.2  1996/08/08  21:06:18  mbeesley
 * CSCdi65492:  r4k_cpu_level format can be more efficient. Convert it
 * to a format where bits [5..3] indicate the interrupt level.
 * Branch: California_branch
 *
 * Revision 3.3.6.1  1996/03/21  22:27:23  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:49:56  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/10  00:25:34  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  20:25:08  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/03/04  07:11:13  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP
 *
 * Revision 3.2  1995/11/17  08:40:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:06:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

/*
 * include platform dependant file to define timing constants
 * which depend on CPU clock speed etc.
 */
#if	defined(SIERRA)
# include "asm_4k_c4000.h"
#endif 	/* SIERRA */

#if	defined(RSP)
# include "asm_4k_rsp.h"
# include "../src-rsp/rsp_pcmap.h"
#endif	/* RSP */

#if 	defined(RVIP)
# include "asm_4k_vip.h"
# include "asm_4k_rvip.h"
#endif 	/* RVIP */

#if 	defined(SVIP)
# include "asm_4k_vip.h"
# include "asm_4k_svip.h"
# include "../src-svip/svip_asic.h"
#endif

#if	defined(PREDATOR)
# include "asm_4k_c7100.h"
#endif

/*
 * Of all the above platforms, only one (RSP) has support for a
 * master/slave watchdog.  For all the others, inherit some default
 * settings here that will be sure to cause a crash should we ever
 * get confused and attempt to stroke the m/s watchdog on a platform
 * that does not support it.
 */
#if	!defined(IO_MSDOG_REG)
# define IO_MSDOG_REG		0x0
# define IO_MSDOG_10MSEC	0x0
#endif

/*
 * Variables used by the real time tick.
 *
 * We declare them here allowing us to use 64bit adds to increment. 
 * clockptr and hclockptr need to be initialized to point to 
 * r4k_clock and r4k_hclock respectively.
 *
 * Note : Make sure these stay together and are on an 8byte boundary
 */
	.globl	r4k_hclock
	.globl	r4k_clock

	.sdata
	.align 3

r4k_hclock:
	.word	0
r4k_clock:
	.word	0

	.text
	.align 2

/*
 * define some externs so the asm will use gp relative addressing
 */
	.extern r4k_cpu_level,4
	.extern int_count,4
	.extern schedflag,4
	.extern current_count,4
	.extern profile_enabled,4
	.extern wd_save_return,4
	.extern r4k_4ms_pipe_ticks,4
	.extern r4k_second_wdog,4
	.extern r4k_update_timestamp,4

/*
 * r4k_update_timestamp:
 * Update regular 4ms NMI tick after bus stall
 */
FRAME(r4k_update_timestamp,sp,0,ra)
	.set    noreorder
	ld	t0,r4k_hclock		# read the clock
	daddu	t0,t0,a0		# increment the clock (by four)
	j	ra			# return
	sd	t0,r4k_hclock		# save tr4k_hclock (bd slot)
	.set    reorder
ENDFRAME(r4k_update_timestamp)

/*
 * r4k_4ms_interrupt:
 * Handle regular 4ms NMI tick
 */

 #
 # void r4k_4ms_interrupt (leveltype oldlevel, void *pc)
 #
FRAME(r4k_4ms_interrupt,sp,32,ra)
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

#if !defined(RVIP) && !defined(SVIP)
	/*
	 * Stroke the watchdog. Note, the VIP style cards in the
	 * c7000 and c7500 do not do this.
	 */
	la	t1,IO_WATCHDOG_REG	# get register address
	li	t2,REFRFREQUENCY*4	# 16ms
	sh	t2,0(t1)		# stroke the watch dog

#endif /* RVIP && SVIP */

	/*
	 *  Some 4K platforms (RSP2) have a second hardware watchdog
	 *  timer used to let our peer RSP2 know that we're still
	 *  alive.  If this is such a platform, stroke it too.
	 */
	lw	t0,r4k_second_wdog
	la	t1,IO_MSDOG_REG		# master/slave watchdog register
	beq	t0,zero,no_two_dogs	# is there a 2nd watchdog?
	li	t2,IO_MSDOG_10MSEC	# maxes out at 10 msecs
	sh	t2,0(t1)
	
no_two_dogs:
	/*
	 * We must install NMI_LEVEL priorities and masks
	 * and de-assert EXL bit (allow reentrancy)
	 */
	li	t4,(NMI_LEVEL << R4K_LEVEL_SHIFT) # new level (bits [5..3])
	sw	t4,r4k_cpu_level		# save new level
	mfc0	t3,CP0_SREG			# read status register
	li	t4,NMI_SREG_MASK		# get new sreg mask

	li	t5,~SREG_IE			# get IE mask
	and	t6,t3,t5			# disable IE
	mtc0	t6,CP0_SREG			# write sreg (IE=0)

	and	t3,t3,t4			# de-assert EXL and IP bits
	mtc0	t3,CP0_SREG			# write sreg (IE=0,EXL=0)

	/*
	 * We can end up in an interrupt handler with IE off if the
	 * interrupt occurs at the next instruction after a move to the
	 * status register that turns IE off. So we turn it back on.
	 */
	ori	t3,t3,SREG_IE			# set IE
	mtc0	t3,CP0_SREG			# store status reg

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
	 * Handle incrementing the clock variables
	 */
	ld	t0,r4k_hclock		# read the clock
	nop				# load delay
	daddiu	t0,t0,REFRESHTIME	# increment the clock (by four)

	/*
	 * Call the C coded routine to handle the UART break key
	 */
	jal	console_check_break	# call the C code handler
	sd	t0,r4k_hclock		# save tr4k_hclock (bd slot)

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
ENDFRAME(r4k_4ms_interrupt)

/* End of file */




#Local Variables:
#comment-column: 40
#comment-start: "# "
#comment-end: ""
#comment-multi-line: nil
#comment-start-skip: "# "
#End:


