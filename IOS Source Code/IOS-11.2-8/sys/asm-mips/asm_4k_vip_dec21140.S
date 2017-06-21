/* $Id: asm_4k_vip_dec21140.S,v 3.2.56.3 1996/05/13 22:52:46 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_vip_dec21140.S,v $
 *------------------------------------------------------------------
 * asm_4k_vip_dec21140.S - Wrapper for DEC21140 100 Mbit Ethernet PA
 *
 * March 1995, Steve Jacobson
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip_dec21140.S,v $
 * Revision 3.2.56.3  1996/05/13  22:52:46  mbeesley
 * CSCdi57397:  DEC21140 interrupt setting incorrect
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/04/21  00:51:09  getchell
 * CSCdi51219:  Input ignores on FE PAs not correct under high input load
 * Branch: California_branch
 *
 * Revision 3.2.56.1  1996/03/21  22:27:34  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/02/16  03:07:13  getchell
 * CSCdi49146:  optimize svip wrappers for amdp2 and dec21140
 * Changed how dtq and drq were timesharing with the wrappered interfaces.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/10  00:25:44  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  20:25:13  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  08:41:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:07:00  hampton
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

#if defined(SVIP)
# include "asm_4k_vip.h"
# include "asm_4k_svip.h"
#endif	/* SVIP */

#if defined(PREDATOR)
# include "asm_4k_c7100.h"
#endif	/* PREDATOR */


/*
CSR5:
------------------------------------------------------------------------------
| EB | TS | RS |NIS|AIS|  |LNF| FD| AT|RWT|RPS| RU| RI|UNF|  |TJT| TU|TPS| TI|
------------------------------------------------------------------------------

  EB:   Bits 25:23:  Type of system error.  Valid when SE is set
  TS:   Bits 22:20:  Transmit process state
  RS:   Bits 19:17:  Receive process state
  NIS:  Bit 16:  Normal interrupt summary.
                   Summary of TI, TU and RI.
  AIS:  Bit 15:  Abnormal interrupt summary.
                   Summary of TPS, TJT, UNF, RU, RPS, RWT, AT, FD, LNF, SE
  SE:   Bit 13:  System error.  See the EB bits for error type.
  LNF:  Bit 12:  Link Fail (21040 0nly).  10base-T link failure.
  FD:   Bit 11:  Full-Duplex short frame received.
  AT:   Bit 10:  AUI/TP pin (21040 0nly).  SIA AUI/TP pin changed state.
  RWT:  Bit 9:   Receive watchdog timeout.
                   Causes length error in receive descriptor.
  RPS:  Bit 8:   Receive process stopped.
  RU:   Bit 7:   Receive buffer unavailable.
  RI:   Bit 6:   Receive interrupt.
  UNF:  Bit 5:   Transmit underflow.
                   Causes underflow error in transmit descriptor.
  TJT:  Bit 3:   Transmit jabber timeout.
                   Causes transmit jabber timeout error in transmit descriptor.
  TU:   Bit 2:   Transmit buffer unavailable.  Part of NIS summary.
                 Probably want to keep this one masked.
  TPS:  Bit 1:   Transmit process stopped.
  TI:   Bit 0:   Transmit interrupt

*/

/* 
 * DEC FasterNet and DEC 21140 CSR5_BITS: 
 */ 
#define DEC21140_RINT_MASK 	0x0040    	/* Bit 6 */ 
#define DEC21140_TINT_MASK 	0x0021    	/* Bit 0 and bit 5 */ 
#define DEC21140_ERRORS    	0x2000    	/* fatal bit 13 */ 

/* 
 * Offset to the CSR5 register
 */ 
#define DEC21140_PA_CSR5_OFS	40    		/* offset to csr0 */

#if !defined(SVIP)

/*
 * dec21140_eim_wrapper: Does not use posted read hardware
 * 
 * Entry:   Moved onto stack
 *	    Context has been saved
 *	    s0 = &(lev2_jumptable[slot])
 *	    s1 = Return address
 *	    a1 = PA base address
 *	    a2 = Interrupt bits [0..7]
 * 
 * Return:  v0 = return true if more work to do on TX and/or RX rings
 */
 #
 # boolean dec21140_eim_wrapper (void)
 #
FRAME(dec21140_eim_wrapper,sp,0,s1)
	.set	noreorder

	beq	zero,a2,dec21140_done		# is there work ?
	lw	s2,PA_IDB_OFS(s0)		# get idb for interface (bd)

	lw	s0,DEC21140_PA_CSR5_OFS(a1)	# read CSR5
	sw	s0,DEC21140_PA_CSR5_OFS(a1)	# clear interrupt bits

	andi	a1,s0,DEC21140_RINT_MASK	# mask for Rx intr

	beq	a1,zero,dec21140_check_tx	# is Rx bit set ?
	nop					# (bd slot)

	jal	dec21140_rx_interrupt		# call Rx interrupt handler
	move	a0,s2				# load up idb param (bd slot)

dec21140_check_tx:
	andi	a1,s0,DEC21140_TINT_MASK	# check for Tx interrupt
	beq	a1,zero,dec21140_check_err	# is Tx bit set ?
	nop					# (bd slot)

	jal	dec21140_tx_interrupt		# call Tx interrupt handler 
	move	a0,s2				# load up idb param (bd slot)

dec21140_check_err:
	andi	a1,s0,DEC21140_ERRORS		# check for error interrupts
	beq	a1,zero,dec21140_done		# is an Err bit set ?
	move	a0,s0				# copy CSR5 to a0 (bd slot)

	jal	dec21140_error_interrupt	# call Err interrupt handler
	move	a1,s2				# load up idb param (bd slot)

dec21140_done:
	j	s1				# return
	move	v0,zero				# return false

	.set	reorder
ENDFRAME(dec21140_eim_wrapper)

#else  /* SVIP */

#define CSR8_CSR5_OFFSET   24

/*
 * dec21140_eim_wrapper: Uses posted read hardware
 * 
 * Entry:   Moved onto stack
 *	    Context has been saved
 *	    s0 = &(pa_jumptable[port])
 *	    s1 = Return address
 *	    s2 = PA base address  - garbaged by wrapper (not PR)
 *	    a1 = Posted Read indication - garbaged by wrapper
 *	    a2 = Interrupt bits [0..7]
 * 
 * Return:  v0 = return true if more work to do on TX and/or RX rings
 */
 #
 # boolean dec21140_eim_wrapper (void)
 #
FRAME(dec21140_eim_wrapper,sp,0,s1)
	.set	noreorder

	beq     zero,a2,s_dec21140_done           # is there work ?	
	nop

#ifdef  PCI_POSTED_READS
	la	s3,PMA_POSTED_READ_REG0         # addr of PMA PR0
	lw	a1,PA_SREG_OFS(s0)		# get addr for PR
	sw	a1,PMA_PR0_OFFSET(s3)		# start PR 
#endif

	/* Do some useful work while posted read is completing */
	jal     vip_dmaq_consumer
	nop

	lw	a0,PA_IDB_OFS(s0)		# get idb for interface (bd)

#ifdef  PCI_POSTED_READS
	lw	a1,PA_SREG_OFS(s0)		# get addr for PR
	lw	s0,0(s3)			# read CSR5

	/* Start posted read for ignore frame counter  */
	addiu	a1,a1,CSR8_CSR5_OFFSET          # need addr for csr8
	sw	a1,PMA_PR0_OFFSET(s3)		# start PR 
#else
        lw      s0,DEC21140_PA_CSR5_OFS(s2)     # read CSR5
#endif
	sw	s0,DEC21140_PA_CSR5_OFS(s2)	# clear interrupt bits

	andi	a1,s0,DEC21140_RINT_MASK	# mask for Rx intr

	beq	a1,zero,s_dec21140_check_tx	# is Rx bit set ?
	move    s2,a0                           # save idb pointer (bd)

	jal	dec21140_rx_interrupt		# call Rx interrupt handler
	nop					# (bd slot)

#ifdef  PCI_POSTED_READS
	lw	a1,0(s3)			# read CSR8 
	jal	dec21140_update_ignore          # update ignore counter
	move	a0,s2				# load up idb param (bd slot)
#endif	

s_dec21140_check_tx:
	andi	a1,s0,DEC21140_TINT_MASK	# check for Tx interrupt
	beq	a1,zero,s_dec21140_check_err	# is Tx bit set ?
	nop					# (bd slot)

	jal	dec21140_tx_interrupt		# call Tx interrupt handler 
	move	a0,s2				# load up idb param (bd slot)

s_dec21140_check_err:
	andi	a1,s0,DEC21140_ERRORS		# check for error interrupts
	beq	a1,zero,s_dec21140_done		# is an Err bit set ?
	move	a0,s0				# copy CSR5 to a0 (bd slot)

	jal	dec21140_error_interrupt	# call Err interrupt handler
	move	a1,s2				# load up idb param (bd slot)

s_dec21140_done:

	j	s1				# return
	move	v0,zero				# return false

	.set	reorder
ENDFRAME(dec21140_eim_wrapper)

#endif /* SVIP */

/* end of file */




