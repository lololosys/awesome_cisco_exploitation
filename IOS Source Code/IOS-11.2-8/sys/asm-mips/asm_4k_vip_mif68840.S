/* $Id: asm_4k_vip_mif68840.S,v 3.1.58.2 1996/04/11 04:39:23 fsunaval Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_vip_mif68840.S,v $
 *------------------------------------------------------------------
 * asm_4k_vip_mif68840.S - Wrapper for 1F PCI Port Adaptor
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip_mif68840.S,v $
 * Revision 3.1.58.2  1996/04/11  04:39:23  fsunaval
 * CSCdi53600:  SW-SVIP: SVIP Fddi optimizations.
 * Branch: California_branch
 * 1.Take advantage of posted read hardware.
 * 2.Take advantage of Management interrupts.
 *
 * Revision 3.1.58.1  1996/03/21  22:27:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.4  1996/02/10  00:25:47  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1.4.3  1996/02/05  06:59:56  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.1.4.2  1996/01/25  12:09:21  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.4.1  1995/12/23  04:39:17  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/11/09  10:55:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/24  20:00:14  fsunaval
 * Placeholder for new development.
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
#endif  /* RVIP */

#if defined(SVIP)
# include "asm_4k_vip.h"
# include "asm_4k_svip.h"
#endif  /* SVIP */

   
#if defined(PREDATOR)
# include "asm_4k_c7100.h"
#endif  /* PREDATOR */

#include "../xdi/motomac.h"

/* 
 * Offset to the SR1 register
 */ 
#define MIF68840_PA_SR1_OFS 0x1004          /* offset to SR1 */

# if !defined(SVIP)

/*
 * mif68840_eim_wrapper:
 * 
 * Entry:   Moved onto stack
 *      Context has been saved
 *      s0 = &(lev2_jumptable[slot])
 *      s1 = Return address
 *      a1 = PA base address
 *      a2 = Interrupt bits [0..7]
 * 
 * Return:  v0 = return true if more work to do on TX and/or RX rings
 */
 #
 # boolean mif68840_eim_wrapper (void)
 #
FRAME(mif68840_eim_wrapper,sp,0,s1)
    .set   noreorder

    beq    zero,a2,mif68840_done            # is there work ?
    lw     s2,PA_IDB_OFS(s0)                # get idb for interface (bd)

    lw     s0,MIF68840_PA_SR1_OFS(a1)       # read SR1
    sw     s0,MIF68840_PA_SR1_OFS(a1)       # clear all interrupt bits

    andi   a1,s0, RECEIVE_COMPLETE_4        # mask for Rx llc intr

    beq    a1,zero,check_tx                 # is Rx llc bit set ?
    nop                                     # (bd slot)

    jal    mif68840_rx_llc_interrupt
    move   a0,s2                            # load up idb param (bd slot)


check_tx:
    andi   a1,s0,RING_CMD_COMPLETE_3        # check for Tx interrupt
    beq    a1,zero,check_smt_err            # is Tx bit set ?
    nop                                     # (bd slot)

    jal mif68840_tx_interrupt               # call Tx interrupt handler
    move    a0,s2                           # load up idb param (bd slot)

check_smt_err:
    li  a1,ERROR_SMT_FRAME                  # check error interrupt/smt 
                                            # frame

    and     a1,a1,s0
    beq a1,zero,mif68840_done               # is error(any)/smt bit set ?
    nop                                     # (bd slot)

    andi    a1,s0,RECEIVE_COMPLETE_5        # check smt frame
    beq a1,zero, check_err                  # is smt bit set ?
    nop

    jal mif68840_rx_smt_interrupt           # call Rx smt interrupt handler
    move    a0,s2                           # load up idb param (bd slot)

    li  a1,ERROR_NO_SMT_FRAME               # check error interrupt
    and     a1,a1,s0
    beq a1,zero,mif68840_done               # is error(any) bit set ?
    nop                                     # (bd slot)

check_err:
    move    a0,s0                           # copy SR1 to a0
    jal     mif68840_error_interrupt        # must be an error interrupt
    move    a1,s2                           # load up idb param (bd slot)

mif68840_done:
    j   s1                                  # return
    move    v0,zero                         # return false

    .set    reorder
ENDFRAME(mif68840_eim_wrapper)


#else  /* SVIP */

/*
 * mif68840_eim_wrapper: Uses posted read hardware
 * 
 * Entry:   Moved onto stack
 *      Context has been saved
 *      s0 = &(pa_jumptable[port])
 *      s1 = Return address
 *      s2 = PA base address  - garbaged by wrapper (not PR)
 *      a1 = Posted Read indication - garbaged by wrapper
 *      a2 = Interrupt bits [0..7]
 * 
 * Return:  v0 = return true if more work to do on TX and/or RX rings
 */
 #
 # boolean mif68840_eim_wrapper (void)
 #
FRAME(mif68840_eim_wrapper,sp,0,s1)
    .set    noreorder

    beq     zero,a2,mif68840_done           # is there work ?   
    nop

#ifdef  PCI_POSTED_READS
    la  s3,PMA_POSTED_READ_REG0             # addr of PMA PR0
    lw  a1,PA_SREG_OFS(s0)                  # get addr for PR
    sw  a1,0(s3)                            # start PR in (bd)
#endif

    /* Do some useful work while posted read is completing */
    jal vip_dmaq_consumer
    nop

    lw  a0,PA_IDB_OFS(s0)                   # get idb for interface

#ifdef  PCI_POSTED_READS
    lw  s0,0(s3)                            # read SR1
#else
    lw  s0, MIF68840_PA_SR1_OFS(s2)         # read SR1
#endif

    sw  s0, MIF68840_PA_SR1_OFS(s2)         # clear interrupt bits


    andi   a1,s0, RECEIVE_COMPLETE_4        # mask for Rx llc intr

    beq    a1,zero,check_tx                 # is Rx llc bit set ?
    move   s2, a0                           # (bd slot)

    jal    mif68840_rx_llc_interrupt
    move   a0,s2                            # load up idb param (bd slot)

check_tx:
    andi   a1,s0,RING_CMD_COMPLETE_3        # check for Tx interrupt
    beq    a1,zero,check_smt_err            # is Tx bit set ?
    nop                                     # (bd slot)

    jal     mif68840_tx_interrupt           # call Tx interrupt handler
    move    a0,s2                           # load up idb param (bd slot)

check_smt_err:
    li      a1,ERROR_SMT_FRAME              # check error interrupt/smt 
                                            # frame

    and     a1,a1,s0
    beq     a1,zero,mif68840_done           # is error(any)/smt bit set ?
    nop                                     # (bd slot)

    andi    a1,s0,RECEIVE_COMPLETE_5        # check smt frame
    beq a1,zero, check_err                  # is smt bit set ?
    nop

    jal mif68840_rx_smt_interrupt           # call Rx smt interrupt handler
    move    a0,s2                           # load up idb param (bd slot)

    li  a1,ERROR_NO_SMT_FRAME               # check error interrupt
    and     a1,a1,s0
    beq a1,zero,mif68840_done               # is error(any) bit set ?
    nop                                     # (bd slot)

check_err:
    move    a0,s0                           # copy SR1 to a0
    jal     mif68840_error_interrupt        # must be an error interrupt
    move    a1,s2                           # load up idb param (bd slot)

mif68840_done:
    j       s1                              # return
    move    v0,zero                         # return false

    .set    reorder
ENDFRAME(mif68840_eim_wrapper)

#endif /* SVIP */

/* end of file */
