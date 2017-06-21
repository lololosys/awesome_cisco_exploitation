/* $Id: asm_4k_vip_ibm2692.S,v 3.3.52.2 1996/06/07 02:26:09 hwang Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_vip_ibm2692.S,v $
 *------------------------------------------------------------------
 * asm_4k_vip_ibm2692.S : Wrapper for 4R Port Adaptor
 *
 * July 1995, HC Wang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip_ibm2692.S,v $
 * Revision 3.3.52.2  1996/06/07  02:26:09  hwang
 * CSCdi57022:  Tx at 12kpps Rx at 14kpps with 4r/4r vip2
 * Add call to vip_dmaq_consumer from 4r interrupt wrapper if SVIP is
 * defined.
 * Branch: California_branch
 *
 * Revision 3.3.52.1  1996/03/21  22:27:36  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/10  00:25:46  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/12/06  04:09:04  sthormod
 * Branch: ELC_branch
 * Bring in HC's code review changes.
 *
 * Revision 3.2  1995/11/17  08:41:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/26  19:05:44  mbeesley
 * Add placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#if defined(RVIP)
# include "asm_4k_vip.h"
# include "asm_4k_rvip.h"
#endif	/* RVIP */

#if defined(SVIP)
# include "asm_4k_vip.h"
# include "asm_4k_svip.h"
#endif	/* SVIP */

#if defined(PREDATOR)
# include "asm_4k_c7100.h"
#endif	/* PREDATOR */

#if !defined(SVIP)

/*
 * ibm2692_eim_wrapper:
 * 
 * Entry:   Moved onto stack
 *	    Context has been saved
 *	    s0 = &(pa_jumptable[slot])
 *	    s1 = Return address
 *	    a1 = PA base address
 *          a2 = interrupt bits [0..7]
 * 
 * Return:  v0 = return register
 *               return boolean, true if more work to do on TX and/or RX rings
 */
 #
 # boolean ibm2692_eim_wrapper (void)
 #
FRAME(ibm2692_eim_wrapper,sp,16,s1)
	.set	noreorder
	
        bne     a2,zero,ibm2692_work            # is there any work?
        nop                                     # (bad slot)
        j       s1                              # return
        move    v0,zero                         # no more work (bad slot)

ibm2692_work:
	addiu	sp,sp,-16			# make a stack frame
	sw	s1,4(sp)			# save return address
        sw      s5,8(sp)
        sw      s6,12(sp)
        move	s5,a2                           # save interrupt bits

	lw	s2, PA_SCRATCH0_OFS(s0)		# read scratch 0
	bne	s2,zero,ibm2692_second_half
	li	s2,0x01

	sw	s2,PA_SCRATCH0_OFS(s0)		# do second half next time
	/*
	 * analyze port 0, 1
	 */
ibm2692_first_half:
	/*
	 * do work for port 0, 1
	 */

        andi	a0,s5,0x01			# check interface 0
	beq	a0,zero,ibm2692_port1
	srl	s5,s5,1

	lw	s2,PA_IDB_OFS(s0)		# get idb for interface 0

	/*
	 * Call hermon interrupt handler port 0
	 */
	jal	hermon_interrupt		# call hermon interrupt handler
	move	a0,s2				# load idb in a0 (bd slot)

ibm2692_port1:
	andi	a0,s5,0x01			# check for interface 1
	beq	a0,zero,ibm2692_first_half_done
	srl	s5,s5,1
	lw	s2,(PA_IDB_OFS+4)(s0)		# load idb 2 (bd slot)

	/*
	 * Call hermon interrupt handler port 1
	 */
	jal	hermon_interrupt		# call hermon interrupt handler
	move	a0,s2				# load idb in a0 (bd slot)

ibm2692_first_half_done:
	lw	s1,4(sp)
	lw	s5,8(sp)
	lw	s6,12(sp)
	addiu	sp,sp,16

	/*
	 * return to the interrupt handler
	 */
	j	s1
	move	v0,zero

	/*
	 * analyze port 1, 2
	 */
ibm2692_second_half:
	sw	zero,PA_SCRATCH0_OFS(s0)	# mark first half flag
	srl	s5,s5,2				# skip bits for port 0, 1
	andi	a0,s5,0x01
	beq	a0,zero,ibm2692_port3
	srl	s5,s5,1
	lw	s2,(PA_IDB_OFS+8)(s0)		# load idb 3 (bd slot)

	/*
	 * Call hermon interrupt handler port 2
	 */
	jal	hermon_interrupt		# call hermon interrupt handler
	move	a0,s2				# load idb in a0 (bd slot)

ibm2692_port3:
	andi	a0,s5,0x01
	beq	a0,zero,ibm2692_second_half_done
	nop
	lw	s2,(PA_IDB_OFS+12)(s0)

	/*
	 * Call hermon interrupt handler port 3
	 */
	jal	hermon_interrupt		# call hermon interrupt handler
	move	a0,s2				# store idb3 in a0 (bd slot)

ibm2692_second_half_done:
        lw	s5,8(sp)			# restore s5
	lw	s6,12(sp)			# restore s6
	lw	s1,4(sp)			# restore return address
	addiu	sp,sp,16			# remove stack frame

	j	s1				# return
	move	v0,zero				# return FALSE

	.set	reorder
ENDFRAME(ibm2692_eim_wrapper)

#else

/*
 * ibm2692_eim_wrapper:
 * 
 * Entry:   Moved onto stack
 *	    Context has been saved
 *	    s0 = &(pa_jumptable[slot])
 *	    s1 = Return address
 *	    a1 = PA base address
 *          a2 = interrupt bits [0..7]
 * 
 * Return:  v0 = return register
 *               return boolean, true if more work to do on TX and/or RX rings
 */
 #
 # boolean ibm2692_eim_wrapper (void)
 #
FRAME(ibm2692_eim_wrapper,sp,16,s1)
	.set	noreorder
	
        bne     a2,zero,ibm2692_work            # is there any work?
        nop                                     # (bad slot)
        j       s1                              # return
        move    v0,zero                         # no more work (bad slot)

ibm2692_work:
	addiu	sp,sp,-16			# make a stack frame
	sw	s1,4(sp)			# save return address
        sw      s5,8(sp)
        sw      s6,12(sp)
        move	s5,a2                           # save interrupt bits

	lw	s2, PA_SCRATCH0_OFS(s0)		# read scratch 0
	bne	s2,zero,ibm2692_second_half
	li	s2,0x01

	sw	s2,PA_SCRATCH0_OFS(s0)		# do second half next time
	/*
	 * analyze port 0, 1
	 */
ibm2692_first_half:
	/*
	 * do work for port 0, 1
	 */

        andi	a0,s5,0x01			# check interface 0
	beq	a0,zero,ibm2692_port1
	srl	s5,s5,1

	lw	s2,PA_IDB_OFS(s0)		# get idb for interface 0

	/*
	 * Call hermon interrupt handler port 0
	 */
	jal	hermon_interrupt		# call hermon interrupt handler
	move	a0,s2				# load idb in a0 (bd slot)

ibm2692_port1:
	andi	a0,s5,0x01			# check for interface 1
	beq	a0,zero,ibm2692_first_half_done
	srl	s5,s5,1
	lw	s2,(PA_IDB_OFS+4)(s0)		# load idb 2 (bd slot)

	/*
	 * Call hermon interrupt handler port 1
	 */
	jal	hermon_interrupt		# call hermon interrupt handler
	move	a0,s2				# load idb in a0 (bd slot)

ibm2692_first_half_done:
	jal	vip_dmaq_consumer
	nop
	lw	s1,4(sp)
	lw	s5,8(sp)
	lw	s6,12(sp)
	addiu	sp,sp,16

	/*
	 * return to the interrupt handler
	 */
	j	s1
	move	v0,zero

	/*
	 * analyze port 1, 2
	 */
ibm2692_second_half:
	sw	zero,PA_SCRATCH0_OFS(s0)	# mark first half flag
	srl	s5,s5,2				# skip bits for port 0, 1
	andi	a0,s5,0x01
	beq	a0,zero,ibm2692_port3
	srl	s5,s5,1
	lw	s2,(PA_IDB_OFS+8)(s0)		# load idb 3 (bd slot)

	/*
	 * Call hermon interrupt handler port 2
	 */
	jal	hermon_interrupt		# call hermon interrupt handler
	move	a0,s2				# load idb in a0 (bd slot)

ibm2692_port3:
	andi	a0,s5,0x01
	beq	a0,zero,ibm2692_second_half_done
	nop
	lw	s2,(PA_IDB_OFS+12)(s0)

	/*
	 * Call hermon interrupt handler port 3
	 */
	jal	hermon_interrupt		# call hermon interrupt handler
	move	a0,s2				# store idb3 in a0 (bd slot)

ibm2692_second_half_done:
	jal	vip_dmaq_consumer
	nop
        lw	s5,8(sp)			# restore s5
	lw	s6,12(sp)			# restore s6
	lw	s1,4(sp)			# restore return address
	addiu	sp,sp,16			# remove stack frame

	j	s1				# return
	move	v0,zero				# return FALSE

	.set	reorder
ENDFRAME(ibm2692_eim_wrapper)

#endif

/* end of file */
