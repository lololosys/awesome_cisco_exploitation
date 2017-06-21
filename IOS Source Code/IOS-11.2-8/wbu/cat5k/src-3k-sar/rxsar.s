/* $Id: rxsar.s,v 1.1.4.3 1996/06/18 16:46:14 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/src-3k-sar/rxsar.s,v $
 *-----------------------------------------------------------------
 * rxsar.s --  Receive Reassembly and Command Processing for Rx SAR
 *
 * November 1994, sakumar
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rxsar.s,v $
 * Revision 1.1.4.3  1996/06/18  16:46:14  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/29  18:02:22  sakumar
 * CSCdi58940:  SAR Builds generates assembler warnings
 * Branch: California_branch
 *
 * Added _start symbol to fix warning message from assembler.
 *
 * Revision 1.1.4.1  1996/05/09  15:01:36  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:43  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  03:09:56  sakumar
 * Apply ATM line card 2.1 fixes to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:36  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
| Revision 1.1  1996/03/11  07:44:37  rlowe
| Placeholders for California.
|
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
| Revision 1.47.2.2.4.2.2.1  1996/02/13  16:58:33  sakumar
| The CTRL_BIT on the VC State was being cleared before the CRC check,
| so a CRC error resulted in the ACP buffer being enqueued to the
| BIGA Buffer pool, resulting in the BIGA Buffer pool becoming empty
| since enq_L does not check for overflow.  Now the CTRL_BIT is cleared
| after the CRC check.
|
| Revision 1.47.2.2.4.4  1996/03/18  22:10:08  sakumar
| Defend against invalid AAL5 lengths. AAL5 length should be <= (n-1)*48+40
| where n is the number of cells received in this packet. Also AAL5 length
| should not be 0.
|
| Revision 1.47.2.2.4.3  1996/02/13  16:29:21  sakumar
| The CTRL_BIT on the VC State was being cleared before the CRC check,
| so a CRC error resulted in the ACP buffer being enqueued to the
| BIGA Buffer pool, resulting in the BIGA Buffer pool becoming empty
| since enq_L does not check for overflow.  Now the CTRL_BIT is cleared
| after the CRC check.
|
| Revision 1.47.2.2.4.2  1996/01/23  20:30:21  sakumar
| Added loops to exhaustively service Rx->Tx and Tx->Rx BUS queues.
| CSCdi47326:  BUS locks up after about 2 minutes of heavy traffic
| Branch: ATM21_branch
|
| Revision 1.47.2.2.4.1  1996/01/10  00:28:15  sakumar
| Pulled out workaround for Chip Express SAGE that required RSAR to
| send dummy packet every 20,000 bytes to prevent lockup during
| port-blocking. This problem does not occur in the the later
| LSI SAGE versions.
|
| Revision 1.47.2.2  1995/11/27  18:12:07  sakumar
| Added new RSAR command: VC_MODIFY that used by aalsap_lec_map_bus_vcds(),
| aalsap_change_vcd_clbit() aalsap_bind_vlan_to_vc() and aalsap_bus_map_bus_vcds()
| instead of VC_OPEN.
|
| This fixes a problem in which if the aalsap_change_vcd_clbit() was called
| while the RSAR was re-assembling a FROM_BUS_TO_BIGA VC packet, using VC OPEN
| could corrupt the RSAR's Reassembly TimeOut queue and potentially hang the RSAR.
|
| Revision 1.47.2.1  1995/11/20  23:28:35  sakumar
| Drop incoming OAM cells (since we don't handle OAM in this release).
|
| Revision 1.47  1995/10/20  22:06:37  sakumar
| Fix for bug where the RSAR would hang if a CRC error occurred on a
| FROM_BUS_DEST_BIGA VC.
|
| Revision 1.46  1995/10/16  23:35:51  sakumar
| Added support for:
|     (1) Topology Change
|     (2) LES, BUS, LECS on Catalyst 5000 ATM module with BUS implemented
|         in fast RSAR->TSAR firmware.
|
| Revision 1.45  1995/10/10  17:28:04  sakumar
| Inadvertently disabled IRAM in previous checkin.
|
| Revision 1.44  1995/10/10  16:37:05  sakumar
| Cleaned up RSAR and atmdriver by removing canned Synergy Headers in
| Frame Buffer. The RSAR now dynamically creates the Synergy Header
| from VC Info. Also, in-band IPC does not use the canned Synergy
| Header. This checkin frees up space in the Frame Buffer, which
| is needed for SAR-to-SAR messaging queues.
|
| Revision 1.43  1995/10/04  01:32:07  kchristi
| Fix CVS Endlog directives so that cvs-fix-merge script will work.
|
| Revision 1.42  1995/09/22  23:21:42  sakumar
| RSAR performance changes. (Construct Synergy Header on the fly and
| DMA it to frame buffer; DMA TBD to frame buffer instead of two 4
| word writes).
|
| Revision 1.41  1995/09/12  01:10:45  sakumar
| More RSAR performance improvements: shadow TBDs in SPRAM to reduce
| Frame Buffer accesses during free_BIGA_buffers; data cache enabled.
|
| Revision 1.40  1995/09/07  16:39:53  sakumar
| Took out some debug code.
|
| Revision 1.39  1995/08/31  20:07:50  sakumar
| RTO_Remove_L was being called twice in case of CRC errors.
|
| Revision 1.38  1995/08/27  22:21:22  sakumar
| More RSAR performance enhancements: Since during last cell BIGA processing
| the Frame Buffer is not accessed for read, wait till the last before checking
| whether DMA has completed.
|
| Revision 1.37  1995/08/26  18:50:04  sakumar
| RXSAR performance enhancement:
| 	Step thru BIGA TBDs by using the compile-time known size instead of
|     accessing the Frame Buffer to read the LINK field.
|
| Revision 1.36  1995/08/23  00:01:10  sakumar
| RXSAR Performance Enhancements.
|
| Revision 1.35  1995/08/19  05:22:18  sakumar
| Fixed bug in SAGE workaround in which the RSAR was sending a dummy packet
| after every real packet instead of after 20,000 bytes.
|
| Revision 1.34  1995/08/02  22:49:34  sakumar
| Fixed "cells dropped due to unknown VC" count so it does not include
| cells dropped due to echo suppression.
|
| Revision 1.33  1995/07/25  22:08:51  sakumar
| Code to send Small Packet to Synergy Bus every 20,000 bytes to prevent
| SAGE lockup. Remove this code in LSI version of SAGE since it will be fixed.
|
| Revision 1.32  1995/07/14  23:11:20  sakumar
| Added LED writes to monitor performance.
|
| Revision 1.31  1995/06/22  18:03:21  sakumar
| Change to allow LEC Ctrl packet on DEST_BIGA VC to go to the ACP even
| before BIND call has been made. Also, the LEC ctrl packet is 0xff00
| and not 0xffff as previously coded.
|
| Revision 1.30  1995/06/09  02:03:14  sakumar
| Added frame counters for statistics.
|
| Revision 1.29  1995/06/06  17:54:12  sakumar
| (1) Cleaned up LED code. Now, on exception, the LEDs should show the reason
|     code.
|
| (2) Enabled IRAM and Reassembly Timeout.
|
| Revision 1.28  1995/05/26  01:17:20  sakumar
| Disabled LOOPBACK define.
|
| Revision 1.27  1995/04/25  18:52:47  sakumar
| RXSAR now polls in the Scheduler loop for BIGA buffers to be freed.
| (Better solution -- BIGA interrupts RXSAR after transmitting buffers --
| is available on Rev 2 boards but not implemented in the SAR code yet).
|
| Revision 1.26  1995/04/21  16:43:43  sakumar
| Added support for runtime specification of active VPI/VCI bits.
|
| Revision 1.25  1995/04/12  22:04:51  sakumar
| Changed VCD to VPC and
|         vcd to vpc.
|
| Revision 1.24  1995/04/07  22:45:53  sakumar
| Even if send_to_nmp command fails (due to no BIGA space), the IPC buffer
| is returned to the IPC Pool by the SAR, since the SAR is the only producer
| of elements in this queue.
|
| Revision 1.23  1995/03/31  00:02:04  sakumar
| The following two features have now been tested and work:
| 	(1) Echo Suppression on FROM_BUS Data VC (if LEC matches MY_LEC).
| 	(2) LEC Ctrl Packet (0xffff in first 2 bytes) goes to ACP regardless
| 	    of VCD_DEST_BIGA*.
|
| Revision 1.22  1995/03/29  22:50:38  sakumar
| Changes to support Echo Suppression and CTRL packet in from_BUS_to_BIGA
| VC.
|
| Revision 1.21  1995/03/27  23:16:03  sakumar
| New scheme for ACP->NMP IPC via RXSAR requires only 1 interrupt to ACP.
|
| Revision 1.20  1995/03/25  02:32:59  sakumar
| IPC between the ACP and NMP via RXSAR & BIGA now works.
|
| Revision 1.19  1995/03/22  00:52:11  rlowe
| Now builds and runs without PMON
|
| Revision 1.18  1995/03/17  22:38:13  sakumar
| Now builds two executables, one for PMON and one without PMON.
|
| Revision 1.17  1995/03/17  21:50:12  sakumar
| Chaned copy to IRAM code so that it starts from the Scheduler and some
| of the one-time initialization is no longer copied to IRAM.
|
| Revision 1.16  1995/03/07  21:29:01  sakumar
| added new define LOOPBACK (to swap SA and DA)
|
| Revision 1.15  1995/03/07  21:25:18  sakumar
| Now works with BIGA. Also, rxsar.s does all the initialization and hence
| rxinit.c is not needed.
|
| Revision 1.14  1995/02/15  21:27:52  sakumar
| Enabled IRAM and RTO. Removed (most of) ATMIZER_EMULATOR ifdefs.
|
| Revision 1.13  1995/02/10  23:57:46  sakumar
| Changed error handling scheme for RXSAR. Now if a packet has to be dropped,
| the associated buffer is returned to the appropriate queue. In the case of
| VC-for-ACP, an error status is returned with the buffer. (previously the
| buffer was being re-used for the same VC, but since there is no guarantee
| that new cells will arrive, waiting for Reassembly Timeout to actually
| free the buffer is not such a good idea).
|
| Revision 1.12  1995/01/19  21:39:40  sakumar
| On VC close, the determinination of whether there is a buffer containing
| a partially re-assembled packet (to be freed) is now done by checking
| the pDma field (and not the pRbuf field). This is because on completing
| a packet, only the pDma field is zeroed out. So _if_ the pDma field is
| non-zero, then the pRbuf field points to a buffer that has to be freed.
|
| Revision 1.11  1994/12/16  02:20:10  sakumar
| Now works on actual target. Also, changed CLEAR VC to use only the VCD field,
| and to return currently receiving buffer, if any.
|
| Revision 1.10  1994/12/15  00:10:36  sakumar
| added code to interrupt host during initialization.
|
| Revision 1.9  1994/12/01  23:25:21  sakumar
| Changed to have compile time switches come in from Makefile rather than
| defines in the source files.
|
| Revision 1.8  1994/11/19  02:01:51  sakumar
| Made changes to test code to use atmdriver.c.
|
| Also, fixed code in rxsar.s to set dma field to zero when VC Closed.
|
| The buffer, if any, will be returned by reassembly timeout.
|
| Revision 1.7  1994/11/17  18:49:11  sakumar
| Changed memory carving to static model (declared in spram.h and shram.h
| instead of being allocated on the fly).
|
| Also changed producer/consumer queues to use last pointer instead of last+1
| pointer to detect wraparound condition.
|
| Revision 1.6  1994/11/11  19:42:47  sakumar
| Added BIGA code. Changed structure of test program.
|
| Revision 1.5  1994/11/09  18:06:01  sakumar
| Cleaned up last cell processing and reassembly timeout to:
| 	(1) Use enq deq routines
| 	(2) placeholders for BIGA functionality (to be added in next step)
|
| Revision 1.4  1994/11/08  23:40:10  sakumar
| Changed RTO code to use REGs instead of VCR Variables.
| Fixed a couple RTO bugs (16 bit field instead of 32 bit)
|
| Revision 1.3  1994/11/08  22:26:26  sakumar
| Using t* temp registers instead of s* regs (in preparation for using the
| the (thus saved) s* regs for caching more useful stuff.
|
 * Revision 1.2  1994/11/08  18:13:13  sakumar
 * Cleaned up file header
 *
 * Revision 1.1  1994/11/08  18:06:00  rahuja
 * Added to cvs
 * 
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ENABLE_REASSEMBLY_TIMEOUT
#define IRAM
#ifdef COMMENTED_OUT
#define SAGE1_LOCKUP_WORKAROUND
#define TIMING_LED
#define LOOPBACK
#endif /* COMMENTED_OUT */

#include "rxsar.h"

/***********************************************************************
*	Coding Conventions:
*		Names:
*			<>_REG for Processor Registers (s0-s8)
*			<>_AREG for ATMIZER Registers
*			<>_s for 32 bit structure fields
*			<>_16s for 16 bit structure fields
*			<>_8s for 8 bit structure fields
*			<>_C for defined constants
*			<>_L for program labels
*		Functions:
*			- Only one level of function calls (there is no stack!)
*			- Return pointer in ra (reg 31)
*			- Arguments in a0, and (if needed) a1. 
*			- Argument value(s) are preserved across function call.
*			- Return in v0
*			- Functions use a0, a1, a2 and v0. Unless the value is an
*			  argument (in a0 or a1), DON'T EXPECT VALUES IN THESE TO 
*			  BE PRESERVED ACROSS FUNCTION CALLS.
*			- s0 - s7, v1, and a3 (defined as <>_REG) are not used by functions
*			- t0 - t9 are not used by functions
*		Other:
*			- Use branches (and not jumps) to code that will live in IRAM
*			- Use register jumps (and not branches) to code that will live 
*			  in Secondary RAM.
***********************************************************************/


    .globl _start
    .ent _start
 
_start:
#ifdef NO_PMON
 
 
###############################################################################
##
##
##
###############################################################################
 
 
#ifdef LR33300
#if 0 /* use IS0 */
CACHECFG = (BIU_IS1|BIU_IS0|BIU_IBLKSZ8|BIU_DS1|BIU_DS0|BIU_DBLKSZ8)
#else
CACHECFG = (BIU_IS1|BIU_IBLKSZ8|BIU_DS1|BIU_DS0|BIU_DBLKSZ8)
#endif
#endif
 
    .text
 
/*-------------------------------------------------------------------*
 * reset_execption()
 *-------------------------------------------------------------------*/
reset_exception_L:
        /* ATMizer must start with a full 32-bit jump */
    la  k0,main
    j   k0
 
    .align  8
/*-------------------------------------------------------------------*
 * utlb_miss_execption()
 *-------------------------------------------------------------------*/
utlb_miss_exception_L:
    la  k0,_exception_L
    li  k1,K1BASE
    or  k0,k1
    j   k0
 
    .align 7
/*-------------------------------------------------------------------*
 * general_exception()
 *-------------------------------------------------------------------*/
general_exception_L:
    la  k0,_exception_L
    li  k1,K1BASE
    or  k0,k1
    j   k0
    .align  9
 

        .end _start
/*-------------------------------------------------------------------*
 *  _exception:
 *-------------------------------------------------------------------*/
    .globl  _exception_L
    .ent _exception_L
_exception_L:
 
        li      t0,0xFF
        li      t1,IOBASE
        .set noreorder
        mfc0    k0,C0_CAUSE
        nop
        .set reorder
        and     k1,k0,CAUSE_EXCMASK
        srl     k1,0x2
        and     k0,k1,0x1
        bne     k0,zero,1f
        sw      t0,LED4_OFFSET(t1)
        b       e1
1:      sw      zero,LED4_OFFSET(t1)
e1:     and     k0,k1,0x2
        bne     k0,zero,2f
        sw      t0,LED3_OFFSET(t1)
        b       e2
2:      sw      zero,LED3_OFFSET(t1)
e2:     and     k0,k1,0x4
        bne     k0,zero,3f
        sw      t0,LED2_OFFSET(t1)
        b       e3
3:      sw      zero,LED2_OFFSET(t1)
e3:     and     k0,k1,0x8
        bne     k0,zero,4f
        sw      t0,LED1_OFFSET(t1)
        b       doom_L
4:      sw      zero,LED1_OFFSET(t1)
 
doom_L:
        b       doom_L
 
    .end _exception_L
 
#endif  /*NO_PMON*/

#ifndef NO_PMON
    .end _start
#endif /* NO NO_PMON */



################################################################################
#  main()
################################################################################


    .globl main
    .ent main
main:

#ifdef NO_PMON
    # force kSeg1 in case control is passed here from Kseg0
    la  t0,1f
    or  t0,K1BASE
    j   t0
1:
 
    # set SR and CAUSE to something sensible
    li  v0,SR_BEV
    .set noreorder
    .set noat
    mtc0    v0,C0_SR
    mtc0    zero,C0_CAUSE
    .set at
    .set reorder
 
BIUCFG = (BIU_BGNT|BIU_NOPAD|BIU_INTP|BIU_IS1|BIU_DS|BIU_DBLKSZ4)
    li  t0,BIUCFG
    sw      t0,M_BIU
SCRCFG = (SCR_RO52|SCR_TO52|SCR_HH0|SCR_HH1|SCR_HH2|SCR_CBS4|SCR_BS32)
    li      t0,SCRCFG
    sw      t0,M_SCR
 
 
    /********************************************
     *  all processor specific setup has been done
     *********************************************/
setUpDone_L:
        /* dummy read to clear intr on ATM line card */
        li      a0,0xA0E80060
        lw      t0,0x0(a0)
 
        /* turn off diag led(code entry ) */
    li  t0,0xff
    li  a0,IOBASE
    sw  t0,LED1_OFFSET(a0)
    sw  t0,LED2_OFFSET(a0)
    sw  t0,LED3_OFFSET(a0)
    sw  t0,LED4_OFFSET(a0)
 
        /* visual delay for LEDs */
        li      t0,0x30000
1:      subu    t0,1
        bne     zero,t0,1b

#endif  /* NO_PMON*/

	# Enable Co-processor condition 0 (for receiving host interrupts)
	#					  condition 1 (Received Cell Indication)
	#					  condition 2 (PRU Timeout)
	#					  condition 3 (DMA Busy)

	# C0_SR is $12 (Processor Status Register)
	mfc0	t1, C0_SR
	or		t1, (SR_CU0|SR_CU1|SR_CU2|SR_CU3)
	mtc0	t1, C0_SR

	
	# Set MSB Register
	la		t0, M_ASUBR
	li		t1, ASUBR_VALUE_C
	sw		t1, (t0)


	# Clear VCR
	la		t0, VCR_ADDR_C
	addu	t1, t0, VCRSIZE_C
1:	sw		zero, (t0)
	addu	t0, 4
	bne		t0, t1, 1b


	# Set the PRU Configuration Register to 0 => Timers use CLK and not 
	# PRU_CLK (Figure 14.8, Pg 14-22)
	# Timer 9 (Used for Reassembly Timetout), is unstalled later
	la		t0, M_CONFIG
	sw		zero, (t0)

	
    # Setup System Control Register (SCR)
    #   - Receive Cell size 52
    #   - Expect HEC on receive
    #   - Check for HEC errors on receive
    #   - Received Cell holder: 16 cells
	#	- Enable the Receiver
	#	- Cache Block Size: 16 bytes
    la	t0, M_SCR
    li	t1, ((3<<SCR_ROSHFT) | SCR_HH1 | SCR_HH2 | SCR_BS16 | SCR_RI | SCR_CBS4)
    sw	t1, (t0)


	# Copy SAR code into IRAM
	bal		iramSize_L		# will return number of words to be copied in v0

	la		t0, M_BIU		# Setup BIU (temporarily -- for IRAM copy)
	li		t1, (BIU_LDSCH | BIU_NOPAD | BIU_IS1|BIU_DBLKSZ4|BIU_DS)
	sw		t1, (t0)
	mfc0	t0, C0_SR		# Get current Processor Status
	or		t1, t0, SR_ISC	# Invalidate cache bit

	la		t3, Scheduler_L	# SOURCE Addr
	move	t4, zero		# DESTINATION Addr (IRAM starts at 0)		

	.set	noreorder
	.set	noat

copy_loop_L:
	lw		t5,	(t3)		# Fetch word from SP
	addi	t3, 4			# increment source pointer
	mtc0	t1,	C0_SR		# enable isolate cache
	sub		v0, 1			# decrement word counter by 1
	sw		t5, (t4)		# store instruction into IRAM
	mtc0	t0, C0_SR		# disable isolate cache
	bne		v0, zero, copy_loop_L
	addi	t4, 4			# increment destination pointer (DELAY slot)

	nop	
	nop
	nop
	
	.set 	reorder
	.set	at

	
/*
 * Initialize Registers and fall through to Rcv Scheduler Loop
 */
	# Setup VCR_REG, VC_TAB_PTR_REG, RTO_HEAD_REG & RTO_TAIL_REG
	li	VCR_REG, VCR_ADDR_C

	li	t0, SAR_SHRAM_START_C			# t0 points to start of frame buffer

#ifdef SAGE1_LOCKUP_WORKAROUND
	# Copy pointer to start of Small Packet (in Frame Buffer) 
	lw	t1, shram_pSmallPacket_s(t0)
	sw	t1, VCR_pSmall_Packet_s(VCR_REG)
	sw	zero, VCR_Byte_Count_s(VCR_REG)
#endif /* SAGE1_LOCKUP_WORKAROUND */

	# Copy pointers to SAR-to-SAR queues
	lw	t1, shram_pRxToTxQ_s(t0)
	sw	t1, VCR_pRxToTxQ_s(VCR_REG)
	lw	t1, shram_pTxToRxQ_s(t0)
	sw	t1, VCR_pTxToRxQ_s(VCR_REG)

	li	t0, RX_SAR_SPRAM_START_C		# t0 is pointer to start of shared mem
	lw	VC_TAB_PTR_REG, rxspram_pEntryTable_s(t0)
	move	RTO_HEAD_REG, zero
	move	RTO_TAIL_REG, zero
	
	# Copy ACP Command and Response Queue ptrs to vars in VCR
	lw	t1, rxspram_pAcpCmdq_s(t0)
	sw	t1, VCR_pAcpCmdq_s(VCR_REG)	
	lw	t1, rxspram_pAcpRespq_s(t0)
	sw	t1, VCR_pAcpRespq_s(VCR_REG)	
	lw	t1, rxspram_pRxToAcpDataq_s(t0)
	sw	t1, VCR_pRxToAcpDataq_s(VCR_REG)
	lw	t1, rxspram_pAcpBufPool_s(t0)
	sw	t1, VCR_pAcpBufPool_s(VCR_REG)

	# Copy ACP NMP IPC Pool pointer to VCR variable
	lw	t1, rxspram_pAcpToNnpIpcPool_s(t0)
	sw	t1, VCR_pAcpToNnpIpcPool_s(VCR_REG)

	# Copy pointer to Global Statistics area
	lw  t1, rxspram_pStats_s(t0)
	sw  t1, VCR_pGlobalStats_s(VCR_REG)
    move GL_PKT_COUNT_REG, zero

	# Intialize APU REG that points to start of BIGA Registers
	li	BIGA_BASE_REG, SAR_BIGA32_REGBASE
	
	# Initialize s/w pointers to BIGA TBDs
	lw	BIGA_THEAD_REG, THEAD*16(BIGA_BASE_REG) 
	move	BIGA_TTAIL_REG, BIGA_THEAD_REG

	# XXX Location of shadow TBDs in RX SPRAM
	li SHAD_THEAD_REG, (SAR_SECONDARYPORT_BASE+0x20000)
	# Make it a cacheable address on SP
	and	SHAD_THEAD_REG, ~0xf0000000	
	move SHAD_TTAIL_REG, SHAD_THEAD_REG
	
	# Setup pointer to the Last+1 TBD
	addu	PLP1TBD_REG, BIGA_TTAIL_REG, bigatd_size_s*NUM_BIGA_TDS

    # Copy the BIGA Buffer Queue Descriptor to VCR
    # Since RSAR is both producer and consumer for this queue, we keep it
    # in the VCR. After the ACP set it up, the init values are copied to VCR.
    lw  t1, rxspram_pBigaBufPool_s(t0)
    lw  t2, (t1)
    sw  t2, VCR_BigaBufPool_s(VCR_REG)
    lw  t2, 4(t1)
    sw  t2, VCR_BigaBufPool_s+4(VCR_REG)
    lw  t2, 8(t1)
    sw  t2, VCR_BigaBufPool_s+8(VCR_REG)
    lw  t2, 12(t1)
    sw  t2, VCR_BigaBufPool_s+12(VCR_REG)

	# Init values of number of valid VCI/VPI bits
	li	ACTIVE_VPI_VCI_BITS_REG, (((1<<VCI_BITS_C)-1)<<4)|(((1<<VPI_BITS_C)-1)<<20)|AAL5_HDR_EOM_C
	li	NUM_VPI_SHIFT_BITS_REG, (16-VCI_BITS_C)

	# Initialize TBD packet count register
	li	TBD_pkt_count_REG, 0

	# Setup Effective Address for BIGA Transmit Descriptors DMA
	la	t1, VCR_bigatd_pData_s(VCR_REG)
	and t1, 0xfff
	li	t0, (8<<DMA_BCSHFT)|M_DMACR|DMA_WR	# 8 bytes
	or	t1, t0
	sw	t1, VCR_bigatd_EA_s(VCR_REG)

	# Setup Synergy Header in first 12 bytes of VCR
	sw 	zero, (VCR_REG)
	sw 	zero, 4(VCR_REG)
	sw 	zero, 8(VCR_REG)
	li	t1, 3
	sh  t1, sy_length_16s(VCR_REG)

	# Setup Count Initialization Register for Counter number 9
	li	t0, NUM_TICKS_C					# init value of counter
	sw	t0, CIR_REG9_AREG				# Write value to CIR

	# Unstall timer 9
	li	t0, ~(1<<9)
	sw	t0, STALL_AREG

	# Interrupt from RXSAR notifies host that RXSAR ready
	sw	zero, HOSTINT_AREG
	# fall through to Scheduling Loop

    # Setup BIU Cache Configuration Register
    #   - Enable Load Scheduling
    #   - No Wait State
    #   - Enable I-Cache Set 1 (ONLY IF RUNNING FROM IRAM)
	#   - 4 word Data cache enable
	#   - read priority
    la      t0, M_BIU
    li      t1, (BIU_LDSCH | BIU_NOPAD | BIU_DBLKSZ4 | BIU_DS | BIU_RDPRI)
#ifdef IRAM
    or      t1,  BIU_IS1
    sw      t1, (t0)
    move    t0,  zero           # Go to 0 which is start of code in IRAM
#else /* NOT IRAM */
    sw      t1, (t0)
    la      t0, (Scheduler_L)
#endif /* NOT IRAM */
    j       t0
    nop
    nop
 
    .end main



#################################################################################

/*
 * Receive SAR Scheduling Algorithm:
 *	while(1) {:
 *		if (Cell_Arrived) {
 *			Process_Cell();
 *			if (--count != 0) {
 *				continue;
 *			}
 *		}
 *		count = MAX_CELLS_C;
 *		if (Interrupt) {
 *			Check_for_txd_buffers();	// Buffers that have been tx'd by BIGA
 *			Process_ACP_Command();
 *			continue;
 *		}
 *		if (PRU_Timer_pops) {
 *			Process_Reassembly_Timer_Tick();
 *		}
 *	}
 */


	.globl Scheduler_L
	.ent Scheduler_L
Scheduler_L:

	# Co Processor Condition 1 True => Cell Received
	bc1f	Check_for_txd_buffers_L
	b	Cell_Received_L					# if cell received, process cell

Cell_processing_complete_L:

Check_for_txd_buffers_L:
	# If pkt count exceeds MAX_BUFS_C then reclaim buffers from BIGA
	blt TBD_pkt_count_REG, MAX_BUFS_C, Check_for_interrupt_L
		# Free up BIGA buffers that have been transmitted to the Synergy Bus
		b free_BIGA_bufs_L


# After freeing any BIGA buffers, it returns to label below:
Check_for_interrupt_L:
	bc0f	Check_for_Timer_Tick_L
	
	# Reading Register 3 clears the contents (i.e. interrupt ack)
	la	t1, SAR_INTR_REG_ADDR_C	# Address of Register 3
	lw	t0, (t1)

	# find out who interrupted the RSAR
	and	t1, t0, INTR_OTHER_SAR_BIT_C
	beq	t1, zero, Is_it_ACP_interrupt_L
		# Process interrupt from TXSAR (BUS pkt transmission complete)
		# DO NOT USE t0 in this code since the interrupt status must be
		# preserved
		b	Intr_from_TXSAR_L

Is_it_ACP_interrupt_L:
	and	t1, t0, INTR_BY_ACP_BIT_C
	beq	t1, zero, 1f
		# Process ACP command
		b	ACP_Command_L
1:


Check_for_Timer_Tick_L:
	# Co-Processor Condition 2 True => PRU Timeout
	bc2f	Scheduler_L

	lw	t0, CREDIT_AREG				# "Reading the CGCR clears the CGCR" Pg 7-3 

	b	RTO_Timer_Tick_L			# Process Timer Tick
	
	.end Scheduler_L



#################################################################################

 

	.globl Cell_Received_L
	.ent Cell_Received_L
Cell_Received_L:

# Extract VCI/VPI bits to get pointer VC Info Entry
	# Load Cell Pointer from Current Received Cell Address Register
	lw	t8, RXBUFPTR_AREG				# t8 contains pointer to Cell

	# Extract active VPI/VCI bits from Cell Header
	# at end of this, t4 contains (VPC << 2)
	lw	t9, vcrcell_hdr_s(t8)			# t9 contains Cell Header

	# If OAM bit set in PTI, drop the cell since we do not support OAM yet
	and t5, t9, (1<<3)
	bne t5, zero, drop_cell_L
 
#ifdef NO_PMON
	and t9, ACTIVE_VPI_VCI_BITS_REG		# Get relevant VPI/VCI bits (and AAL5 EOM)
#else
	and t9, (((1<<VCI_BITS_C)-1)<<4)|(((1<<VPI_BITS_C)-1)<< 20)|AAL5_HDR_EOM_C
#endif
	# Extract VCI bits
	and	t4, t9, ABS_VCI_MASK_C
	# Extract VPI bits
	and	t1, t9, ABS_VPI_MASK_C
#ifdef NO_PMON
	srl	t1,	NUM_VPI_SHIFT_BITS_REG		# Shift VPI bits right 
#else
	srl	t1,	(16-VCI_BITS_C)
#endif
	or	t4, t1							# Concatenate VPI-VCI
	srl	t4, 2							# Shift num right (4-2) bits
	
	# VC_map_entry_pointer = Rcv_VCInfo_ptr_array[VPC]
	addu	t0, t4, VC_TAB_PTR_REG		# t0 is ptr to VC Info pointer
	lw	pVC_Info_REG, (t0)				# VC Info Pointer (incoming VC Map entry)
    # The above is an SPRAM access, so meanwhile compute DMA Effective Addr
    # Calculate DMA Control Register Effective Address
    # Default DMA length is 48 bytes. But if last cell, make it 44 bytes
    # since AAL5 CRC itself is not included in CRC calculation.

    and t9, AAL5_HDR_EOM_C              # clear all but PTI bit
 
    la  t5, vcrcell_payload_s(t8)       # Addr of cell data (in VCR)
    and t5, 0xfff                       # Mask everyting but LAC
    # Last cell?
    beq t9, zero, 1f
 
    li  t7, (44<<DMA_BCSHFT)|M_DMACR|DMA_WR # Size = 44, Set RD bit
    b   2f
 
1:  li  t7, (48<<DMA_BCSHFT)|M_DMACR|DMA_WR # Size = 48, Set RD bit
 
2:  or  t5, t7                          # t5 contains Effective Addr

	lw	t6, imap_pDma_s(pVC_Info_REG)	# t6 addr to DMA cell to
	lw	t7, imap_crc32_s(pVC_Info_REG)	# Get Partial CRC32
	
	# Has DMA buffer been setup for this packet? If so, then this cell is 
	# not the first cell of a new packet, skip to buffer_setup.
	bne	t6, zero, buffer_setup_done_L		
	
	# No buffer present, so either first cell of new packet or VC not configured
	# First check VC State
	lw	t0, imap_vpc_state_s(pVC_Info_REG)

# First Cell of Packet; which queue to get buffer from?
	bne	t0, VPC_DEST_BIGA, 9f 
	# If first two bytes of first cell payload are 0xff00, send packet to ACP 
2:	lhu	t1, vcrcell_payload_s(t8)
	la	a0, VCR_BigaBufPool_s(VCR_REG)
	li	t2, Rbuf_data_s							# DMA offset is 10
	bne t1, 0xff00, get_buffer_L
		# LEC Ctrl pkt on data VC, send it to ACP
		or	t0, VPC_CTRL_BIT					# Set CTRL bit in VC State
		sw	t0, imap_vpc_state_s(pVC_Info_REG)
		lw	a0, VCR_pAcpBufPool_s(VCR_REG)
		li	t2, bufh_payload_s
		b get_buffer_L

9:	bne	t0, VPC_RCV_FOR_BUS, 9f
	la	a0, VCR_BigaBufPool_s(VCR_REG)			# Use same Buffer Pool as BIGA
	li	t2, bufh_payload_s						# start pkt at normal offset
	b	get_buffer_L

9:	bne	t0, VPC_DEST_ACP, 9f
	lw	a0, VCR_pAcpBufPool_s(VCR_REG)
	li	t2, bufh_payload_s
	b get_buffer_L

9:	bne	t0, VPC_DEST_BIGA_FROM_BUS, 9f
	# If cell lecid == my_lecid, then drop packet (Echo Suppression)
	lhu	t1, vcrcell_payload_s(t8)				# Received LECID
	lhu	t2, imap_lecid_16s(pVC_Info_REG)		# My LECID
	beq	t1, t2, 1f								# Echo suppress if same
		# If not Echo Suppression, then same as VPC_DEST_BIGA
		b 2b
	# Echo Suppresion: set bit in VC state, drop cell
1:	or	t0, VPC_ECHO_SUPP_BIT
	sw	t0, imap_vpc_state_s(pVC_Info_REG)
	# now check if first cell is also last cell
	b	8f

9:	bne	t0, (VPC_DEST_BIGA_FROM_BUS|VPC_ECHO_SUPP_BIT), 9f 
	# Echo Suppresion bit set, so if LAST cell, clear bit
8:	beq	t9, zero, 1f
		# Last cell, so clear VPC_ECHO_SUPP_BIT from VC State	
		and	t0, ~VPC_ECHO_SUPP_BIT
		sw	t0, imap_vpc_state_s(pVC_Info_REG)
1:	b drop_cell_L

9:	bne	t0, (VPC_DEST_BIGA | VPC_WT_TL_BIND_BIT), 9f
	# Bind has not been done, but we still need to check for 0xff00 
	# in the first two bytes. If so, we send it to the ACP, otherwise
	# if the packet is a data packet, we will drop it since no BIND.
	lhu	t1, vcrcell_payload_s(t8)
	bne t1, 0xff00, 1f
		# LEC Ctrl pkt on data VC, send it to ACP
		or	t0, VPC_CTRL_BIT					# Set CTRL bit in VC State
		sw	t0, imap_vpc_state_s(pVC_Info_REG)
		lw	a0, VCR_pAcpBufPool_s(VCR_REG)
		li	t2, bufh_payload_s
		b get_buffer_L
1:	b drop_cell_L

9:	bne t0, VPC_SNAP_PVC_TO_BIGA, 9f
	la	a0, VCR_BigaBufPool_s(VCR_REG)
    # In order to remove the SNAP Header, set DMA offset to 
    # Synergy Header size (12) - LLC SNAP Header size (10) = 2
	li	t2, 2							        # DMA offset is 2

    # If PID (7th and 8th byte) is 1 then LAN FCS included, if 7 not included
    # For PVCs, we use the LECID field to indicate LAN FCS included
    lh  t1, vcrcell_payload_s+6(t8)
    bne t1, 1, 1f
        sh t1, imap_lecid_16s(pVC_Info_REG)
	    b   get_buffer_L
1:  sh zero, imap_lecid_16s(pVC_Info_REG)
	b   get_buffer_L

	# Increment unknown vc stat
9:	lw	t8, VCR_pGlobalStats_s(VCR_REG)	# addr of Global stat buffer
	lw	t9, rxgl_cell_unknownvc_err_s(t8)
	addu	t9, 1
	sw	t9, rxgl_cell_unknownvc_err_s(t8)

	b drop_cell_L

get_buffer_L:
	# t2 contains offset to start DMA into buffer
	# Dequeue buffer from Free Buffer Pool
	bal	deq_L							# on ret v0 will contain buffer pointer
	
	# if buffer pointer (v0) is null => no buffers left in pool
	bne	v0, zero, 1f					# drop cell if no buffers avail
		sw	zero, RXACK_AREG				# ack ACI

		# Increment statistic
    	lw  t8, VCR_pGlobalStats_s(VCR_REG) # addr of Global stat buffer
    	lw  t9, rxgl_cell_no_buf_err_s(t8)
    	addu    t9, 1
    	sw  t9, rxgl_cell_no_buf_err_s(t8)

		# Since no buffers available, try to reclaim some buffers from BIGA
		b free_BIGA_bufs_L
1:

	sw	v0, imap_pRbuf_s(pVC_Info_REG)	# Store buffer pointer in VC Info Table

	sh	zero, imap_rlen_16s(pVC_Info_REG) # Initialize bytes received

	li	t7, 0xffffffff					# Initialize CRC value (in t7)

	addu t6, v0, t2 					# load destination DMA address in t6

buffer_setup_done_L: # Destination DMA buffer has now been setup
	# On Entry:
	# 		t4: VPC*4
	# 		t5: DMAC Register Effective Addr 
	# 		t6: addr to be DMA'd to
	# 		t7: Partial CRC
	#		t8: points at received cell in VCR
	#		t9: Cell Header
	#		pVC_Info_REG: points to Rcv Map entry for VC

	# DMA cell payload to shared memory

	# First load CRC register
	sw	t7, CRC32_AREG					# Load CRC Register

	# Start DMA
	sw	t6, (t5)						# Write to DMAC starts DMA

# OK, now that DMA is in progress, we can do a few things

	# load previous byte received count
	lhu	t7, imap_rlen_16s(pVC_Info_REG)	# t7 contains bytes recieved

    # if first cell, then add this VC to Re-Assembly Timeout List
    # we figure out first cell because t7 (bytes received) is zero
	bne t7, zero, 1f
		bal RTO_add_L                   # Add VC to reassembly timeout list

1:

	# Was current cell the last cell of packet (bit 0 set in PTI)?
	bne	t9, zero, last_cell_L

	# increment count of bytes received
	addu	t7, 48

	# not last cell, so calculate next DMA destination address
	addu	t6, 48

	# IMPORTANT: the following code assumes rx buffer size is a multiple of 48
	# not last cell; so will next cell cause buffer overflow? if so, drop packet

	blt	t7, RX_BUFSZ_C, 1f
	li	t0, RCV_PKT_STAT_BUF_OVFL_ERROR_C	# drop_pkt expects status in t0
	b	drop_packet_L

1:
	# store back updated VC info into VC table
	sw	t6, imap_pDma_s(pVC_Info_REG)
	sh	t7, imap_rlen_16s(pVC_Info_REG)
	
# OK, let us now wait for DMA to complete
	1:	bc3t	1b

	# We're done with the cell contents, so Ack ACI that cell processed
	sw	zero, RXACK_AREG				# Writes to Received Cell Indicator Reg

	# Store Partial CRC result back to VC Info 
	lw	t0, CRC32_AREG
	sw	t0, imap_crc32_s(pVC_Info_REG)

	b	Cell_processing_complete_L		# return to scheduling loop



last_cell_L:
	# On Entry:
	#		t4: contains VPC*4
	#		t8: points at received cell in VCR
	#		t7: bytes received so far, not counting the last cell
	#		pVC_Info_REG: points to Rcv Map entry for VC

	# Get packet length (from AAL5 trailer)
    lhu t2, aal5tail_len_16s(t8)

    # AAL5 Length Sanity Check
    addu t7, 40  # max payload of last cell is 40; AAL5 len shd be <= t7
    bgt t2, t7, drop_bad_len_packet_L
    beq t2, zero, drop_bad_len_packet_L


	# Load VC State in t1
	lw	t1, imap_vpc_state_s(pVC_Info_REG)

	# Get CRC from Cell contents
    lw  t0, aal5tail_crc_s(t8)


	# Reset Dma addr field to zero (since we'll give up the buffer)
	sw	zero, imap_pDma_s(pVC_Info_REG)
	sw	zero, imap_pDma_s(pVC_Info_REG)		# DON'T remove this!

	not t0                         # the LSI code does this, not sure why!!

	# Who to give this packet to? (BIGA, ACP or TXSAR?)
	bne	t1, VPC_DEST_BIGA, 9f
packet_for_BIGA_L:
#DESTINATION BIGA

	# Packet goes to BIGA (to be Xmited on Synergy Bus)
	# On Entry:
	#		t0: has AAL5 CRC
	#		t2: has AAL5 length
	#		pVC_Info_REG: points to Rcv Map entry for VC
	#		Need to wait for last cell DMA to complete, check CRC and ACK ACI

# Setup Synergy Header
	lhu t1, imap_vlan_cl_16s(pVC_Info_REG)
	lhu t3, imap_cid_vpc_16s(pVC_Info_REG)
	sh	t1, sy_vlanid_cl_s16(VCR_REG)
	sh	t3, sy_index_16s(VCR_REG)


# Setup TBD for Data Buffer
	# Get pointer to start of packet buffer (including Synergy Header)
	lw	t5, imap_pRbuf_s(pVC_Info_REG)	# t5 has pointer to buffer
	# Since LEC bytes removed, but Synergy Header added, increment by 10
	addu	t2, 10
	# Set length and control bits
	or	t6, t2, (CTL_QC_ASYNC1)
# Write to TBD2 field in VCR
	# First field is buffer pointer
	sw	t5,	VCR_bigatd_pData_s(VCR_REG)
	# Second field is length
	sw	t6, VCR_bigatd_control_s(VCR_REG)

#ifdef LOOPBACK
# XXX For loopback testing, swap DA and SA
    move    a0, t5
    bal     swap_DA_SA_L
#endif

# get next TBD
	addu BIGA_TTAIL_REG, bigatd_size_s	
	addu SHAD_TTAIL_REG, 4	
	# Check for wraparound; does it point to the last plus 1 TBD?
	bne	BIGA_TTAIL_REG, PLP1TBD_REG, 1f
		# Wraparound, so go to beginning TBD
		subu BIGA_TTAIL_REG, bigatd_size_s*NUM_BIGA_TDS
		subu SHAD_TTAIL_REG, 4*NUM_BIGA_TDS
1:

# Queue overflow? If so, drop packet.
	bne	BIGA_TTAIL_REG, BIGA_THEAD_REG, 1f
		li	t0, RCV_PKT_STAT_BIGA_Q_OVFL_ERROR_C # drop_pkt expects status in t0
		b	drop_packet_L

1:
    # Get address of TBD in Frame Buffer into t8
	subu t8, BIGA_TTAIL_REG, 8
	
	# Encode buffer type in address field
	or  t6, t5, BIGA_BUFFER_C
	# Store address and buffer type in shadow SPRAM
	sw	t6, (SHAD_TTAIL_REG)

	# Remove from Reassembly Timeout List
    bal RTO_remove_L


# Now we have finished constructing the Synergy Header and TBD in the VCR 
# so DMA to Frame Buf

	# But first we have to wait for the last cell DMA to complete
1:  bc3t    1b
    sw  zero, RXACK_AREG 	# ack ACI
    lw  t6, CRC32_AREG      # Load CRC value as calculated by ATMizer

	# First DMA Synergy Header
	li	t4, (12<<DMA_BCSHFT)|M_DMACR|DMA_WR  # EA of 1st 12 bytes in VCR
	sw	t5, (t4)

	# increment received packet counters per VC, as well as global
    lw  t3, imap_pVCStat_s(pVC_Info_REG)# addr of per-vc stat buffer
    addu    GL_PKT_COUNT_REG, 1
    lw  t9, rx_packet_input_s(t3)       # number of packets rcvd on this VC


    # Check CRC 
    beq t0, t6, 1f
		li	t0, RCV_PKT_STAT_CRC_ERROR_C	# drop_pkt expects status in t0
		b	drop_packet_no_wait_for_DMA_L

1:
    lw  t4, VCR_pGlobalStats_s(VCR_REG) # addr of Global stat buffer

	# DMA Transmit Buffer Descriptor
	lw	t7, VCR_bigatd_EA_s(VCR_REG)

    addu    t9, 1

	sw	t8, (t7)		# Start DMA of TBD

    sw  GL_PKT_COUNT_REG, rxgl_packet_input_s(t4)       # write back
 
    sw  t9, rx_packet_input_s(t3)       # write back

	# Wait for Sy Hdr and TBD DMAs to complete
1:  bc3t    1b

	# Everything OK, so update BIGA TTAIL register
	sw	BIGA_TTAIL_REG, TTAIL*16(BIGA_BASE_REG)

	addu TBD_pkt_count_REG, 1

#ifdef SAGE1_LOCKUP_WORKAROUND
    # After sending 20,000 bytes on the Synergy Backplane, we send a
    # Small Packet which is ignored by the Bus. However, this prevents
    # the Packet Buffer receive area from filling up and then locking up
    # the SAGE. This code will be disabled in the later fixed versions of SAGE.
 
    lw  t0, VCR_Byte_Count_s(VCR_REG)
    addu    t2, 12      # include synergy header length of 12 bytes
    addu    t0, t2
   
    # If we have received 20,000 bytes, then send a Small Packet (pre-formatted
    # in ios_atmdriver.c) to Synergy bus.
    blt t0, 20000, 2f
        # Send Small Packet
        move t8, BIGA_TTAIL_REG
        move t9, SHAD_TTAIL_REG
        addu t8, bigatd_size_s
        addu t9, 4
        # Check for wraparound; does it point to the last plus 1 TBD?
        bne t8, PLP1TBD_REG, 1f
            # Wraparound, so go to beginning TBD
            subu t8, bigatd_size_s*NUM_BIGA_TDS
            subu t9, 4*NUM_BIGA_TDS
1:
        # Skip if queue overflow (we'll send Small Packet next time)
        beq t8, BIGA_THEAD_REG, 2f
 
        move BIGA_TTAIL_REG, t8
        move SHAD_TTAIL_REG, t9
 
        # Get pointer to packet
        lw  t1, VCR_pSmall_Packet_s(VCR_REG)
 
        # Setup length (72 bytes) and other control fields
        li  t3, (CTL_QC_ASYNC1 | 72)
        sw  t1, -8(BIGA_TTAIL_REG)
        sw  t3, -4(BIGA_TTAIL_REG)
 
        # Encode buffer type in address field
        or  t1, SYN_HDR_BUFFER_C
        # Store address and buffer type in shadow SPRAM
        sw  t1, (SHAD_TTAIL_REG)
 
        sw  BIGA_TTAIL_REG, TTAIL*16(BIGA_BASE_REG)
 
        # Reset counter, since we have sent Small Packet
        move t0, zero
 
2:
        sw  t0, VCR_Byte_Count_s(VCR_REG)
#endif /* SAGE1_LOCKUP_WORKAROUND */

	b	Cell_processing_complete_L		# return to scheduling loop

	# DEST_BIGA_FROM_BUS handled the same way as DEST_BIGA
9:	beq	t1, VPC_DEST_BIGA_FROM_BUS, packet_for_BIGA_L

	# VPC_SNAP_PVC_TO_BIGA handled the same way as DEST_BIGA
9:	bne	t1, VPC_SNAP_PVC_TO_BIGA, 9f
    # subtract 8 bytes of SNAP header from length; the remaining
    # two bytes will be removed above
    subu t2, 8 
    # for PVC, we use the lecid field to indicate whether or not 
    # the Ethernet FCS (4 bytes) has been included
    lh	t3, imap_lecid_16s(pVC_Info_REG)	
    beq t3, zero, packet_for_BIGA_L
        subu t2, 4  # packet len included Ethernet FCS so take it out
    b packet_for_BIGA_L


	# Incoming packet for BUS so send it to TXSAR for transmit
9:	bne	t1, VPC_RCV_FOR_BUS, 9f
packet_for_BUS_L:
    # On Entry:
    #       t0: has AAL5 CRC
	#		t2: has AAL5 length
    #       pVC_Info_REG: points to Rcv Map entry for VC
    #       Need to wait for DMA to complete, check CRC and ACK ACI
 
    # Remove from Reassembly Timeout List
    bal RTO_remove_L
 
    # increment received packet counters per VC, as well as global
    lw  t3, imap_pVCStat_s(pVC_Info_REG)# addr of per-vc stat buffer
    lw  t9, rx_packet_input_s(t3)       # number of packets rcvd on this VC
 
    addu    GL_PKT_COUNT_REG, 1
    lw  t8, VCR_pGlobalStats_s(VCR_REG) # addr of Global stat buffer
    sw  GL_PKT_COUNT_REG, rxgl_packet_input_s(t8)       # write back
 
    addu    t9, 1
    sw  t9, rx_packet_input_s(t3)       # write back
 
    or  t2, (RCV_PKT_STAT_OK_C<<16)     # Insert Status
 
    # Get pointer to start of packet buffer
    lw  a1, imap_pRbuf_s(pVC_Info_REG)  # a1 has pointer to buffer
 
    # Fill in VPC, length and status into packet header
	lhu	t4, imap_cid_vpc_16s(pVC_Info_REG)	# get BUSs OUTGOING vpc
	and t4, 0x3ff						# VPC can be at most 10 bits
    sh  t4, bufh_vpc_16s(a1)            # VPC number
    sw  t2, bufh_status_16s(a1)         # status and length
 
    # Get pointer to queue on to which this packet is sent
    lw  a0, VCR_pRxToTxQ_s(VCR_REG)		# Rxsar to Txsar Queue
 
1:  bc3t    1b                          # wait for DMA to complete
 
    # ack ACI
    sw  zero, RXACK_AREG
 
    # Check CRC
    lw  t9, CRC32_AREG
    beq t0, t9, 1f
    li  t0, RCV_PKT_STAT_CRC_ERROR_C    # drop_pkt expects status in t0
    b   drop_packet_no_wait_for_DMA_L
 
1:
    # Enqueue Received buffer in RXSAR to TXSAR queue
    bal enq_L
 
    # Interrupt TXSAR to tell it that it has a packet
	li	t0, SAR_INTR_REG_ADDR_C
	li	t1, INTR_OTHER_SAR_BIT_C
    sw  t1, (t0)

	b	Cell_processing_complete_L		# return to scheduling loop


9:	and	t3, t1, VPC_CTRL_BIT
	beq	t3, zero, 9f
		# LEC Control packet on data VC, so send it to ACP
		b	packet_for_ACP_L

9:	bne	t1, VPC_DEST_ACP, 9f
packet_for_ACP_L:
	# On Entry:
	#		t0: has AAL5 CRC
	#		t2: contains bytes-received (from AAL5 Trailer)
	#		t4: contains VPC
	#		pVC_Info_REG: points to Rcv Map entry for VC
	#		Need to wait for DMA to complete, check CRC and ACK ACI

	# Remove from Reassembly Timeout List
    bal RTO_remove_L

	# calculate VPC (t4 contained VPC*4, so divide by 4)
    srl t4, 2

	# increment received packet counters per VC, as well as global
    lw  t3, imap_pVCStat_s(pVC_Info_REG)# addr of per-vc stat buffer
    lw  t9, rx_packet_input_s(t3)       # number of packets rcvd on this VC
 
    addu    GL_PKT_COUNT_REG, 1
    lw  t8, VCR_pGlobalStats_s(VCR_REG) # addr of Global stat buffer
    sw  GL_PKT_COUNT_REG, rxgl_packet_input_s(t8)       # write back
 
    addu    t9, 1
    sw  t9, rx_packet_input_s(t3)       # write back

	or	t2, (RCV_PKT_STAT_OK_C<<16)		# Insert Status

	# Get pointer to start of packet buffer
	lw	a1, imap_pRbuf_s(pVC_Info_REG)	# a1 has pointer to buffer

	# Fill in VPC, length and status into packet header
	sh	t4, bufh_vpc_16s(a1)			# VPC number
	sw	t2, bufh_status_16s(a1)			# status and length

	# Get receive queue ptr
	lw	a0, VCR_pRxToAcpDataq_s(VCR_REG)

1:	bc3t	1b							# wait for DMA to complete

	# ack ACI
	sw	zero, RXACK_AREG

    # Check CRC 
    lw  t9, CRC32_AREG
    beq t0, t9, 1f
	li	t0, RCV_PKT_STAT_CRC_ERROR_C	# drop_pkt expects status in t0
	b	drop_packet_no_wait_for_DMA_L

1:
	# Enqueue Received buffer in ACP Receive queue
	bal	enq_L

	# Interrupt ACP to tell it that it has a packet
	sw	zero, HOSTINT_AREG

	# But first, clear VPC_CTRL_BIT in state variable
	and	t1, ~VPC_CTRL_BIT
	sw	t1, imap_vpc_state_s(pVC_Info_REG)

	b	Cell_processing_complete_L		# return to scheduling loop




drop_bad_len_packet_L:
    li t0, RCV_PKT_STAT_LEN_MISMATCH_ERROR_C
    b drop_packet_L


9:
drop_packet_L:
	# NOTE: Packets dropped due to Reassembly Timeout are handled elsewhere
	#
	# On Entry:
	#		pVC_Info_REG: points to Rcv Map entry for VC
	#				  t0: contains error status

	# DMA may already in progress, even though we are going to drop the packet,
    # so wait for DMA to complete

1:  bc3t    1b
 
    # Ack ACI
    sw      zero, RXACK_AREG

	# Remove from Reassembly Timeout List if we haven't already done so 
	bal	RTO_remove_L

drop_packet_no_wait_for_DMA_L:

	# Get pointer to buffer to be returned
	lw  a1, imap_pRbuf_s(pVC_Info_REG)    	# a1 points at buffer to be freed

	# Since buffer will be released, reset DMA Addr field
	sw	zero, imap_pDma_s(pVC_Info_REG)

	# Load VC State in t1
	lw	t1, imap_vpc_state_s(pVC_Info_REG)

	# Which queue to return buffer to?
	bne	t1, VPC_DEST_BIGA, 9f

3:
# DESTINATION BIGA
	# If DEST_BIGA, restore BIGA_TTAIL_REG to original value
	lw  t9, TTAIL*16(BIGA_BASE_REG)
	move t8, PLP1TBD_REG
	subu t8, bigatd_size_s*NUM_BIGA_TDS
restore_tail_loop_L:
	# loop to restore both BIGA_TTAIL_REG and SHAD_TTAIL_REG
	beq	t9, BIGA_TTAIL_REG, 2f
		bne BIGA_TTAIL_REG, t8, 1f
			addu BIGA_TTAIL_REG, bigatd_size_s*(NUM_BIGA_TDS-1)
			addu SHAD_TTAIL_REG, 4*(NUM_BIGA_TDS-1)
			b restore_tail_loop_L
1:
		subu BIGA_TTAIL_REG, bigatd_size_s
		subu SHAD_TTAIL_REG, 4
	b	restore_tail_loop_L

2:
	
	# This VC's buffers normally go to BIGA - return buffer to BIGA Buffer Pool
	la	a0, VCR_BigaBufPool_s(VCR_REG)		# a0 points at queue descriptor
	bal	enq_L
	b	increment_stats_L

	# DEST_BIGA_FROM_BUS and VPC_SNAP_PVC_TO_BIGA handled the same 
    # way as DEST_BIGA
9:	beq	t1, VPC_DEST_BIGA_FROM_BUS, 3b
	beq	t1, VPC_SNAP_PVC_TO_BIGA, 3b

9:	and	t3, t1, VPC_CTRL_BIT
	beq	t3, zero, 9f
		# LEC Control packet on data VC, so send it to ACP
		# But first, clear VPC_CTRL_BIT in state variable
		and	t1, ~VPC_CTRL_BIT
		sw	t1, imap_vpc_state_s(pVC_Info_REG)
		b	rtn_drop_pkt_buf_to_ACP_L

9:	bne t1, VPC_DEST_ACP, 9f
# DESTINATION ACP
rtn_drop_pkt_buf_to_ACP_L:
	# First insert buffer status into Buffer header
	sh	t0,	bufh_status_16s(a1)
	
	# This VC's buffers normally go to ACP - return buffer to ACP Buffer Pool
	lw	a0, VCR_pRxToAcpDataq_s(VCR_REG)	# a0 points at queue descriptor
	bal	enq_L

	# Interrupt ACP to tell it that it has a packet
	sw	zero, HOSTINT_AREG

	b	increment_stats_L

9:	bne	t1, VPC_RCV_FOR_BUS, 9f
	la	a0, VCR_BigaBufPool_s(VCR_REG)		# a0 points at queue descriptor
	bal	enq_L
	b	increment_stats_L

9:
increment_stats_L:
	# increment dropped packet counter

	lw	t8, VCR_pGlobalStats_s(VCR_REG)	# addr of Global stat buffer

	bne	t0,	RCV_PKT_STAT_CRC_ERROR_C, 1f
	lw	t9, rxgl_packet_crc_err_s(t8)
	addu	t9, 1
	sw	t9, rxgl_packet_crc_err_s(t8)
	b	Cell_processing_complete_L

1:	bne	t0,	RCV_PKT_STAT_BUF_OVFL_ERROR_C, 1f
	lw	t9, rxgl_packet_bufovl_err_s(t8)
	addu	t9, 1
	sw	t9, rxgl_packet_bufovl_err_s(t8)
	b	Cell_processing_complete_L

1:	bne	t0,	RCV_PKT_STAT_BIGA_Q_OVFL_ERROR_C, 1f
	lw	t9, rxgl_packet_bigaovl_err_s(t8)
	addu	t9, 1
	sw	t9, rxgl_packet_bigaovl_err_s(t8)
	b	Cell_processing_complete_L

1:	bne	t0,	RCV_PKT_STAT_LEN_MISMATCH_ERROR_C, 1f
    lw t9, rxgl_packet_bad_len_err_s(t8)
    addu t9, 1
    sw t9, rxgl_packet_bad_len_err_s(t8)

	# return to scheduling loop
1:	b	Cell_processing_complete_L


drop_cell_L:
	# No entry requirements:

	sw	zero, RXACK_AREG				# ack ACI

	b	Cell_processing_complete_L		# return to scheduling loop

	.end Cell_Received_L



#################################################################################


/* 
 * Code to support ReAssembly Timeout
 * NOTE: RTO_add_L() and RTO_remove_L() are FUNCTIONS (i.e. return address in ra)
 *       but RTO_Timer_Tick_L is NOT a Function.
 */
	.globl	RTO_add_L
	.ent	RTO_add_L
# FUNCTION to add a VC to the Reassembly Timeout Linked List
# Arguments:
#	pVC_Info_REG: register to contain pointer to VC Info struct
# Return:
#	none
#
RTO_add_L:

	# Set next and previous pointers for this VC
	sw	zero, imap_pRTO_next_s(pVC_Info_REG)	
	sw	RTO_TAIL_REG, imap_pRTO_prev_s(pVC_Info_REG)

	# Set timeout time
	lw	a0, VCR_time_s(VCR_REG)			# System time
	addu	a0, RTO_INTERVAL_C			# timeout time
	sw	a0, imap_RTO_time_s(pVC_Info_REG)

	# Is reassembly timeout list currently empty?
	beq	RTO_TAIL_REG, zero, 1f
	# List not empty
	sw	pVC_Info_REG, imap_pRTO_next_s(RTO_TAIL_REG)
	move	RTO_TAIL_REG, pVC_Info_REG
	j	ra

	# List Currently Empty
1:	move	RTO_TAIL_REG, pVC_Info_REG
	move	RTO_HEAD_REG, pVC_Info_REG
	j	ra
	.end	RTO_add_L


#################################################################################


	.globl RTO_remove_L
	.ent   RTO_remove_L
# FUNCTION to remove a VC from the Reassembly Timeout Linked List
# Arguments:
#	pVC_Info_REG: register to contain pointer to VC Info struct
# Return:
#	none
#
RTO_remove_L:
	lw	a0, imap_pRTO_prev_s(pVC_Info_REG)	# a0 is pointer to previous VC
	lw	a1, imap_pRTO_next_s(pVC_Info_REG)	# a1 is pointer to next VC
	
	# Is previous NULL?
	beq	a0, zero, 1f
	# Previous not NULL, so update its next pointer
	sw	a1, imap_pRTO_next_s(a0)
	b	2f

1:	# previous NULL, so we are removing current head 
	# new head is next VC
	move	RTO_HEAD_REG, a1

2:	# Is next NULL?
	beq	a1, zero, 1f
	# Next not NULL, so update its prev pointer
	sw	a0, imap_pRTO_prev_s(a1)
	j	ra								# return

1:	# next NULL, so we are removing current tail
	# new tail is previous VC
	move	RTO_TAIL_REG, a0

	j	ra								# return
	.end   RTO_remove_L


#################################################################################

	.globl	RTO_Timer_Tick_L
	.ent	RTO_Timer_Tick_L
RTO_Timer_Tick_L:

	# Increment system "time"
	lw	t0, VCR_time_s(VCR_REG)
	addu	t0, 1
	sw	t0, VCR_time_s(VCR_REG)

Timeout_check_L:
	# Get Head VC 
	move	pVC_Info_REG, RTO_HEAD_REG
	
	# List Empty?
	bne	pVC_Info_REG, zero, 1f
	# List empty so return to Scheduling Loop
	b	Scheduler_L

1:	# List not empty, check if head has timed out
	lw	t1, imap_RTO_time_s(pVC_Info_REG)	# t1 has timeout time

#ifdef ENABLE_REASSEMBLY_TIMEOUT
	# Timed out?
	bge	t0, t1, timed_out_L
#endif	/* ENABLE_REASSEMBLY_TIMEOUT */

	# Didn't time out, so return (NOTE: This is an ordered list, so if 
	# head didn't timeout, neither have the others later on the list)
	b	Scheduler_L

timed_out_L:
	# Reassembly Timeout

	# Take out VC from Reassembly Timeout Queue
	bal	RTO_remove_L					# Will update RTO_HEAD_REG

	# Since buffer will be released, reset DMA Addr field
	sw	zero, imap_pDma_s(pVC_Info_REG)

    # Load VC State in t1
    lw  t1, imap_vpc_state_s(pVC_Info_REG)

	# Increment RTO Error count
	lw	t8, VCR_pGlobalStats_s(VCR_REG)	# addr of Global stat buffer
	lw	t9, rxgl_packet_rto_err_s(t8)
	addu	t9, 1
	sw	t9, rxgl_packet_rto_err_s(t8)

	# Which queue to put back timed out buffer on?
	bne	t1, VPC_DEST_BIGA, 9f
# Destination BIGA

	# Buffers on this timed out VC normally go to BIGA; reuse buffer
	# Get pointer to buffer
1:	lw	a1, imap_pRbuf_s(pVC_Info_REG)  # a1 has ptr to buffer 

	# Return buffer to BIGA Buffer Queue
	la	a0, VCR_BigaBufPool_s(VCR_REG)	# a0 points at queue descriptor
	bal	enq_L

	# Now go back and check if new head has timed out too
	b	Timeout_check_L

	# DEST_BIGA_FROM_BUS handled the same way as DEST_BIGA
9:	beq	t1, VPC_DEST_BIGA_FROM_BUS, 1b

	# VPC_SNAP_PVC_TO_BIGA handled the same way as DEST_BIGA
9:	beq	t1, VPC_SNAP_PVC_TO_BIGA, 1b

	# VPC_RCV_FOR_BUS handled the same way as DEST_BIGA
9:	beq	t1, VPC_RCV_FOR_BUS, 1b

9:	and	t3, t1, VPC_CTRL_BIT
	beq	t3, zero, 9f
		# LEC Control packet on data VC, so return buffer to ACP
		# But first, clear VPC_CTRL_BIT in state variable
		and	t1, ~VPC_CTRL_BIT
		sw	t1, imap_vpc_state_s(pVC_Info_REG)
		b	rtn_rto_buf_to_ACP_L

9:	bne	t1, VPC_DEST_ACP, 9f
# Destination ACP
rtn_rto_buf_to_ACP_L:
	# Buffers on this timed out VC normally go to ACP; return buffer
	# to ACP, with reassembly timeout indication

	# Get pointer to start of packet buffer
	lw	a1, imap_pRbuf_s(pVC_Info_REG)	# a1 has ptr to buffer

	# Fill in status
	li	t2, RCV_PKT_STAT_TIMEOUT_ERROR_C
	sh	t2, bufh_status_16s(a1)

	# Put buffer onto ACP receive queue
	lw	a0, VCR_pRxToAcpDataq_s(VCR_REG)
	bal	enq_L

	# Interrupt ACP to tell it that it has a packet
	sw	zero, HOSTINT_AREG

9:
	# Now go back and check if new head has timed out too
	b	Timeout_check_L

	.end	RTO_Timer_Tick_L



#################################################################################

	.globl	free_BIGA_bufs_L
	.ent	free_BIGA_bufs_L

free_BIGA_bufs_L:
#ifdef TIMING_LED
	li	t9, IOBASE
#endif TIMING_LED
	# On entry, exhaustively free up all buffers that BIGA has transmitted

	# Read new head pointer from BIGA Register
	# t0 has current head
	lw	t0, THEAD*16(BIGA_BASE_REG) 

	# While we are loading the HEAD value, load constants into regs
	li	t4, SYN_HDR_BUFFER_C
	li	t5, BIGA_BUFFER_C
	li	t6, BUF_TYPE_MASK_C
	li	t7, ~BUF_TYPE_MASK_C

	# Get pointer to BIGA Buffer Queue descriptor
	la	a0, VCR_BigaBufPool_s(VCR_REG)

free_loop_L:
#ifdef TIMING_LED
	sw	zero, LED2_OFFSET(t9)
#endif TIMING_LED
	# If no buffers to free, check to see if command from ACP
	beq	BIGA_THEAD_REG, t0, Check_for_interrupt_L

	# Increment HEAD pointer to next TBD
	addu SHAD_THEAD_REG, 4	
	# # Get buffer pointer and buffer type from Shadow SPRAM
    # assuming there will be no wraparound.
	lw	a1, (SHAD_THEAD_REG)

	addu BIGA_THEAD_REG, bigatd_size_s
	# Check for wraparound 
	bne BIGA_THEAD_REG, PLP1TBD_REG, 1f
       	# Wraparound, so go to beginning TBD
       	subu BIGA_THEAD_REG, bigatd_size_s*NUM_BIGA_TDS
		subu SHAD_THEAD_REG, 4*NUM_BIGA_TDS	
        # Whoops wraparound. So we will have to reload a1
        lw  a1, (SHAD_THEAD_REG)
1:
	
	# Get buffer type from bits 27 to 24 of a1
	and	t1, a1, t6

	# If TBD points to Synergy header, skip it
	beq	t1, t4, free_loop_L

	# Decrement TBD_pkt_count_REG
	subu TBD_pkt_count_REG, 1
 
	# Clear bits 27 to 24 from address
	and	a1, t7

	bne t1, t5, 9f
biga_buffer_return_L:
    # enqueue freed buffer on BIGA Buffer Pool Queue
    # INLINING THE ENQ FUNCTION FOR PERFORMANCE
 
    lw  v0, qdesc_pProducer_s(a0)
    lw  a2, qdesc_pLast_s(a0)
 
    sw  a1, (v0)                        # write element address to producer

    # Check for wraparound
    beq v0, a2, 1f
        # Increment producer index
        addu    v0, 4
        sw  v0, qdesc_pProducer_s(a0)
        b   free_loop_L
 
1:  lw  v0, qdesc_pFirst_s(a0)
    sw  v0, qdesc_pProducer_s(a0)
    b   free_loop_L

9:	bne t1, ACP_BUFFER_C, 9f
acp_buffer_return_L:
	# Get ACP to NMP IPC Pool pointer
	lw	a0, VCR_pAcpToNnpIpcPool_s(VCR_REG)

	# buffer start address is (data address - buffer_header_len)
	subu	a1, bufh_payload_s

	# Return buffer to ACP
	bal	enq_L

	# Restore a0 to point to BIGA Buffer Queue Descriptor
	la	a0, VCR_BigaBufPool_s(VCR_REG)
9: b	free_loop_L
	.end	free_BIGA_bufs_L


#################################################################################
	.globl ACP_Command_L
	.ent ACP_Command_L
ACP_Command_L:
	# On entry, exhaust all commands before returning

	# Get pointer to ACP Command Queue Descriptor into t0
	lw	t0, VCR_pAcpCmdq_s(VCR_REG)	
	lw	t1, qdesc_pConsumer_s(t0)		# t1 now has cmd_q consumer index
	lw	t2, qdesc_pProducer_s(t0)		# t2 now has cmd_q producer index

	# Get pointer to ACP Response Queue Descriptor into t9
	lw	t9, VCR_pAcpRespq_s(VCR_REG)
	lw	t8, qdesc_pProducer_s(t9)		# t8 contains resp q producer index

command_loop_L:
	# If Command queue non-empty, then process command, else return
	bne	t1, t2, 1f
	# Store back Command Q consumer index value
	sw	t1, qdesc_pConsumer_s(t0)
	# Store back Response Q producer index value
	sw	t8, qdesc_pProducer_s(t9)
	# Interrupt ACP to let it know that Commands have been processed
	sw      zero, HOSTINT_AREG
	# return to Scheduler Loop after command queue empty
	b	Scheduler_L		

# Process Command
1: 	# Get command queue element pointer
	lw	t7, (t1)						# t7 contains ptr to command buffer

	# Get Command opcode
	lw	t4, cmd_hdr_s(t7)				# Host MUST set status to 0	
	and	t3, t4, CMDCODE_MASK_C			# t3 now has command code
	# Handle Command ... (Do not use t0, t1, t2, t3, t4, t7, t8 and t9)
    # VC MODIFY (for bind, map_bus_vcs, change_cl_bit etc.)
	# VC OPEN
	# VC CLOSE
	# RESTART

9:	bne     t3, RXSAR_MODIFY_VPCMAP_C, 9f
# VC MODIFY
    lw pVC_Info_REG, vpcp_pVcMapEntry_s(t7)		# Get ptr to VC Info
 	lw	t6, vpcp_vcstate_s(t7)					# Load New state
	lw	t5, imap_vpc_state_s(pVC_Info_REG)		# Get current state
    and t5, ~(VPC_WT_TL_BIND_BIT | 0xff)		# Clear state bits set by host
	or  t5, t6                                   
	sw	t5, imap_vpc_state_s(pVC_Info_REG)		# Store new state
    # The rest is the same as VC OPEN
    b store_VC_parms_L

9:	bne 	t3, RXSAR_SET_VPCMAP_C, 9f
# VC OPEN
	lw	pVC_Info_REG, vpcp_pVcMapEntry_s(t7)
	# No need to check if this VC slot is already active; done in driver
	# lw	t5, imap_vpc_state_s(pVC_Info_REG)
	# beq	t5, VPC_INACTIVE, 8f
	# or	t4, SAR_CMD_FAIL_C
	# b	command_done_L
	
	# Init DMA address field to zero
	sw	zero, imap_pDma_s(pVC_Info_REG)

	# Copy stuff from command buffer to VC Info Table Element
8: 	lw	t5, vpcp_vcstate_s(t7)
	sw	t5, imap_vpc_state_s(pVC_Info_REG)

store_VC_parms_L:
	lhu	t5, vpcp_lecid_16s(t7)
	sh	t5, imap_lecid_16s(pVC_Info_REG)

	lhu	t5, vpcp_vlan_cl_16s(t7)
	sh	t5, imap_vlan_cl_16s(pVC_Info_REG)

	lhu	t5, vpcp_cid_vpc_16s(t7)
	sh	t5, imap_cid_vpc_16s(pVC_Info_REG)

	lw	t5, vpcp_pRVCStat_s(t7)
	sw	t5, imap_pVCStat_s(pVC_Info_REG)

	or	t4, SAR_CMD_SUCCESS_C
	b	command_done_L

9:	bne t3, RXSAR_CLEAR_VPCMAP_C, 9f
# VC CLOSE

	lhu	t5, vpcp_vpc_16s(t7)		# t5 contains VPC number
	sll	t5,	2						# multiply by 4
	addu	t5, VC_TAB_PTR_REG		# t5 now points to VC Info pointer
	lw	pVC_Info_REG, (t5)

	# Any receive buffers? If so, return to Host (with Error Status)
	lw	t6, imap_pDma_s(pVC_Info_REG)
	beq	t6, zero, 1f

	# Since there is a buffer attached, VC must be on RTO Queue, so take it out
	bal	RTO_remove_L

	# a1 to contain address of buffer (containing partial packet) to be freed
	lw	a1, imap_pRbuf_s(pVC_Info_REG)

	# Which queue to return buffer to?
	# Load VC State in t6
	lw	t6, imap_vpc_state_s(pVC_Info_REG)

	bne	t6, VPC_DEST_BIGA, 8f
	# Buffer was destined for BIGA, so put it back in BigaBufPool
7:	la	a0, VCR_BigaBufPool_s(VCR_REG)
	bal	enq_L
	# Skip forward
	b	1f

	# DEST_BIGA_FROM_BUS handled the same way as DEST_BIGA
8:	beq	t6, VPC_DEST_BIGA_FROM_BUS, 7b

	# VPC_SNAP_PVC_TO_BIGA handled the same way as DEST_BIGA
8:	beq	t6, VPC_SNAP_PVC_TO_BIGA, 7b

	# VPC_RCV_FOR_BUS handled the same way as DEST_BIGA since we use Biga Bufs
8:	beq	t6, VPC_RCV_FOR_BUS, 7b

8:	and	t5, t6, VPC_CTRL_BIT
	beq	t5, zero, 8f
		# LEC Control packet on data VC, so return buffer to ACP
		b	ret_close_vc_buf_to_ACP_L

8:	bne	t6, VPC_DEST_ACP, 8f
    # Buffers on this VC normally go to ACP; return buffer
    # to ACP, with error indication
ret_close_vc_buf_to_ACP_L:
	# Fill in status
	li	t5, RCV_PKT_STAT_ERROR_C 
	sh	t5, bufh_status_16s(a1)

	# Put buffer onto ACP receive queue
	lw	a0, VCR_pRxToAcpDataq_s(VCR_REG)
	bal	enq_L

	# Interrupt ACP to tell it that it has a packet
	sw	zero, HOSTINT_AREG

	# Skip forward
	b	1f
	
8:
1: # Init DMA address field to zero
	sw	zero, imap_pDma_s(pVC_Info_REG)
	
	# Set VC State to VPC_INACTIVE
	li	t5, VPC_INACTIVE
	sw	t5, imap_vpc_state_s(pVC_Info_REG)
	or	t4, SAR_CMD_SUCCESS_C
	b	command_done_L
	

9:	bne 	t3, RXSAR_RESTART_C, 9f
restart_L:
# RESTART
	# ... TBD ...
	or	t4, SAR_CMD_SUCCESS_C
	b	command_done_L 

9:	bne		t3, RXSAR_SEND_TO_NMP_C, 9f
# SEND PACKET TO NMP
send_to_nmp_L:
	
	# Setup Synergy Header in VCR
	lhu t5, rx_nmp_vlan_cl_16s(t7)
    lhu t6, rx_nmp_cid_vpc_16s(t7)
    sh  t5, sy_vlanid_cl_s16(VCR_REG)
    sh  t6, sy_index_16s(VCR_REG)

	# Setup BIGA for Data Buffer
	addu BIGA_TTAIL_REG, bigatd_size_s
	addu a2, SHAD_TTAIL_REG, 4	
    # Check for wraparound; does it point to the last plus 1 TBD?
    bne BIGA_TTAIL_REG, PLP1TBD_REG, 1f
        # Wraparound, so go to beginning TBD
        subu BIGA_TTAIL_REG, bigatd_size_s*NUM_BIGA_TDS
		subu a2, 4*NUM_BIGA_TDS
1:
	bne	BIGA_TTAIL_REG, BIGA_THEAD_REG, 1f	# Queue overflow?
		# No space on BIGA, return error
		or	t4, SAR_CMD_FAIL_C
		lw	BIGA_TTAIL_REG, TTAIL*16(BIGA_BASE_REG)		# Restore old tail
		# Return IPC buffer
		lw	a0, VCR_pAcpToNnpIpcPool_s(VCR_REG)
		lw	a1, rx_nmp_pbuf_s(t7)			# Buffer pointer
		bal	enq_L							# Return IPC Buffer
		b	command_done_L

1:	# Get pointer to start of packet buffer
	lw	t5, rx_nmp_pbuf_s(t7)
	la	t5, bufh_payload_s(t5)				# t5 now points to start of data

	# Now copy Synergy Header from VCR to the first 12 bytes of the buffer
	lw	t6, (VCR_REG)
	sw	t6, (t5)
	lw	t6, 4(VCR_REG)
	sw	t6, 4(t5)
	lw	t6, 8(VCR_REG)
	sw	t6, 8(t5)

	lw	t6, rx_nmp_len_s(t7)				# length of packet
	addu t6, 12								# adjust for synergy Header length

	sw	t5, -8(BIGA_TTAIL_REG)				# write data pointer
	and	t6, 0xffff							# clear upper bits (no MORE bit)
	or	t6, (CTL_QC_ASYNC1)					# User defined bits and ASYNC1 bit
	sw	t6, -4(BIGA_TTAIL_REG)				# write length, User Def bits etc.

	# Encode buffer type in address field and store it in shadow SPRAM
	or  t5, ACP_BUFFER_C
	# Store address and buffer type in shadow SPRAM
	move SHAD_TTAIL_REG, a2
	sw	t5, (SHAD_TTAIL_REG)

	# Send to BIGA
	sw	BIGA_TTAIL_REG, TTAIL*16(BIGA_BASE_REG)
	addu TBD_pkt_count_REG, 1
	
	or t4, SAR_CMD_SUCCESS_C				# Fill in status
	b	command_done_L
	
9:	bne 	t3, RXSAR_SET_VPI_VCI_BITS_C, 9f
set_vpi_vci_bits_L:
	lw	ACTIVE_VPI_VCI_BITS_REG, rx_active_vpi_vci_mask_s(t7)
	or	ACTIVE_VPI_VCI_BITS_REG, AAL5_HDR_EOM_C
	lw	NUM_VPI_SHIFT_BITS_REG, rx_num_vpi_shift_bits_s(t7)
	or	t4, SAR_CMD_SUCCESS_C
	b	command_done_L 

9: # next command goes here

# Done Processing Command
command_done_L:
	# Put in Response Status
	sw	t4, cmd_hdr_s(t7)

	# The Command Buffer is now the Response Buffer, store it in Resp Q
	sw	t7, (t8)

	# check for wrap around (Resp Q Producer)
	lw	t3, qdesc_pLast_s(t9)
	bne	t8, t3, 1f
	lw	t8, qdesc_pFirst_s(t9)			# wrap around	
	b	2f

	# increment Response Queue Producer Index
1:	addu	t8, 4

2:	# check for wrap around (Command Q Consumer)
    lw	t3, qdesc_pLast_s(t0)  
    bne	t1, t3, 1f
    lw	t1, qdesc_pFirst_s(t0)    		# wrap around 
	b	command_loop_L

1:	# increment Command Queue Consumer Index
	addu    t1, 4 
	b	command_loop_L
	.end ACP_Command_L


#################################################################################


	.globl	enq_L
	.ent	enq_L
# FUNCTION to Enqueue to a producer-consumer queue
# Arguments:
# 	a0: Queue Descriptor
# 	a1: Element to be queued
# Return:
#	none
#
# NOTE1: Not always called! (In perf critical places, is manually "inlined")
# NOTE2: Does not check for queue overflow since that should never happen.
#        q size should be preconfigured to be > maximum elements carved out.
#
enq_L:	
	lw	v0, qdesc_pProducer_s(a0)		# v0 has producer pointer
	sw	a1, (v0)						# write element address to producer

	# Check for wraparound
	lw	a2, qdesc_pLast_s(a0)
	bne	v0, a2, 1f

	lw	v0, qdesc_pFirst_s(a0)
	b	2f

	# Increment producer index
1:	addu	v0, 4

2:	sw	v0, qdesc_pProducer_s(a0)
	j	ra
	.end	enq_L


#################################################################################


	.globl	deq_L
	.ent	deq_L
# FUNCTION to Dequeue from a producer-consumer queue
# Arguments:
#	a0:	Queue Descriptor
# Return:
#	v0: Pointer to dequeued element; zero if queue empty
#
# NOTE: Not always called! (In perf critical places, is manually "inlined")
#
deq_L:
	lw	a1, qdesc_pConsumer_s(a0)		# a1 has consumer index
	lw	a2, qdesc_pProducer_s(a0)		# a2 has producer index
	
	# Queue empty?
	bne	a1, a2, 1f
	# Yes, queue empty, so return
	move	v0, zero
	j	ra

1:	# Queue not empty, get element
	lw	v0, (a1)		# v0 has pointer to deq'd element

	# Check for wraparound
	lw	a2, qdesc_pLast_s(a0)
	bne	a1, a2, 1f
	lw	a1, qdesc_pFirst_s(a0)
	b	2f

	# Increment Consumer Index
1:	addu	a1, 4

2:	sw	a1, qdesc_pConsumer_s(a0)
	j	ra
	.end	deq_L

#ifdef LOOPBACK
	.globl	swap_DA_SA_L
	.ent	swap_DA_SA_L
swap_DA_SA_L:
	addu	a1, a0, 5

1:	
	lbu	a2, (a1)
	lbu	v0, 6(a1)
	sb	a2, 6(a1)
	sb	v0, (a1)

	subu	a1, 1
	bne		a1, a0, 1b
	j		ra

	.end	swap_DA_SA_L
#endif /* LOOPBACK */

################################################################################
	.globl	Intr_from_TXSAR_L
	.ent	Intr_from_TXSAR_L
Intr_from_TXSAR_L:
    # Process interrupt from TXSAR (BUS pkt transmission complete)
    # DO NOT USE t0 in this code since the interrupt status must be
    # preserved

	# TXSAR returns the RXSARs buffers. RSAR puts em back on its BIGA q
	lw	a0, VCR_pTxToRxQ_s(VCR_REG)
	bal	deq_L	

	beq v0, zero, txsar_intr_exit_L 		# No buffer on queue so EXIT

	# Enqueue returned buffer to BIGA buffer pool
	la	a0, VCR_BigaBufPool_s(VCR_REG)		# a0 points at queue descriptor
	move a1, v0								# enq expects buffer ptr in a1
	bal	enq_L

	b Intr_from_TXSAR_L						# service queue exhaustively
 
txsar_intr_exit_L:
	b	Is_it_ACP_interrupt_L
	.end	Intr_from_TXSAR_L

 
#################################################################################


/*
 * THIS FUNCTION HAS TO COME AFTER THE LAST FUNCTION TO BE INCLUDED IN IRAM
 *
 * iramSize_L()
 * return the size (in words) of the code to be copied to
 * iram.
 */

	.globl iramSize_L
	.ent iramSize_L
iramSize_L:     la      v0,iramSize_L
	la	t0,Scheduler_L
	subu	v0,t0
	srl	v0,2
	j	ra
	.end iramSize_L
