/* $Id: asm_4k_c4000_hd64570.S,v 3.2 1995/11/17 08:40:36 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/asm-mips/asm_4k_c4000_hd64570.S,v $
 *------------------------------------------------------------------
 * asm_4k_c4000_hd64570.S - HD64570 Wrappers for Sierra
 *
 * March 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_c4000_hd64570.S,v $
 * Revision 3.2  1995/11/17  08:40:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:06:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_c4000.h"

/*
 * HD64570 specific defines
 */
#define HD_ISR1_ISR0_OFS      0x110
#define HD_NONE_ISR2_OFS      0x112
#define HD_C0_ST1_ST0_OFS     0x122
#define HD_C1_ST1_ST0_OFS     0x142
#define HD_C0_ST3_ST2_OFS     0x124
#define HD_C1_ST3_ST2_OFS     0x144

/* Interrupt source register bits.  */
#define QSIM_MDM_INT_0       0x0001    /* Bit 0 */
#define QSIM_MDM_INT_1       0x0002    /* Bit 1 */   
#define QSIM_MDM_INT_2       0x0004    /* Bit 2 */
#define QSIM_MDM_INT_3       0x0008    /* Bit 3 */
#define QSIM_SCA_0_INT       0x0100    /* Bit 8 */
#define QSIM_SCA_1_INT       0x0200    /* Bit 9 */

#define QSIM_INT_MASK        0x30F
#define QSIM_INT_CLEAR       0x1000

/* HD64570_ISR1_ISR0 bits. */
#define HD_S0_RXRDY_MASK     0x0001    /* Bit 0, ISR0 */
#define HD_S0_RXINT_MASK     0x0004    /* Bit 2, ISR0 */
#define HD_S0_TXINT_MASK     0x0008    /* Bit 3, ISR0 */
#define HD_S1_RXRDY_MASK     0x0010    /* Bit 4, ISR0 */
#define HD_S1_RXINT_MASK     0x0040    /* Bit 6, ISR0 */
#define HD_S1_TXINT_MASK     0x0080    /* Bit 7, ISR0 */
#define HD_S0_RERR_MASK      0x0100    /* Bit 8, ISR1 */
#define HD_S0_RINT_MASK      0x0200    /* Bit 9, ISR1 */
#define HD_S0_TINT_MASK      0x0800    /* Bit 11, ISR1 */
#define HD_S1_RERR_MASK      0x1000    /* Bit 12, ISR1 */
#define HD_S1_RINT_MASK      0x2000    /* Bit 13, ISR1 */
#define HD_S1_TINT_MASK      0x8000    /* Bit 15, ISR1 */

/* HD64570_ST1_ST0 bits. */
#define HD_ST1_UDRN_MASK     0x8000    /* Bit 15 */ 

/*
 * qsim_wrapper:
 * HD64570 Wrapper (quad serial) 
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
 # void qsim_wrapper (void)
 #
FRAME(qsim_wrapper,sp, 16,s1)
        .set    noreorder
        
        lbu     t0,0(s2)                    # read IACK register
        move    s3,a1                       # get slot base address (ld)

        addiu   sp,sp,-16                   # make a stack frame
        sw      s1,12(sp)                   # save s1
        
        lhu     s2,QSIM_INT_SRC_OFS(a1)     # read intr src register
        sw      s4,8(sp)                    # save s4
        andi    s2,s2,QSIM_INT_MASK         # mask out unwanted bits

        sw      s5,4(sp)                    # save s5 
        bne     t0,zero,qsim_sca_1          # which SCA wants us ?
        sw      s6,0(sp)                    # save s6 (bd)
        

qsim_sca_0:
        addiu   s0,s0,LEV2_IDB_A            # s0 to point to IDB A pointer
        lw      s1,4(s0)                    # read IDB B pointer in s1
        andi    t0,s2,QSIM_SCA_0_INT        # mask for SCA0 interrupt (ld)
        lw      s0,0(s0)                    # read IDB A pointer in s0 

        beq     t0,zero,qsim_sca_0_p0       # SCA0 is interrupting us ?
	nop				    # (bd slot)

        la      t1,qsim_wrapper_table       # get wrapper table address
        sll     a0,a0,3                     # 8 bytes/slot in wrapper table 
        addu    t1,t1,a0                    # get wrapper table entry address
        lw      t2,0(t1)                    # read the wrapper address
        lhu     s5,HD_ISR1_ISR0_OFS(s3)     # read ISR1 and ISR0 register

        jal     s4,t2                       # call the secondary wrapper 
        nop                                 # (bd slot)
        
qsim_sca_0_p0:
        andi    t0,s2,QSIM_MDM_INT_0        # mask for port 0 modem intr
        beq     t0,zero,qsim_sca_0_p1       # port 0 modem intr ?
        andi    s5,s2,QSIM_MDM_INT_1        # mask for port 1 modem intr (bd)  

        lhu     t0,QSIM_P0_INT_OFS(s3)      # read port 0 intr reg  
        nop                                 # load delay
        ori     t0,t0,QSIM_INT_CLEAR        # turn on the clear bit
        sh      t0,QSIM_P0_INT_OFS(s3)      # clear the interrupt 

        jal     hd64570_modem_interrupt     # call modem interrupt
        move    a0,s0                       # load up idb 0 param (bd)
        
qsim_sca_0_p1:
        beq     s5,zero,qsim_sca_0_done     # port 1 modem interrupt ?
        lhu     t0,QSIM_P1_INT_OFS(s3)      # read port 1 intr reg  (bd)

        ori     t0,t0,QSIM_INT_CLEAR        # turn on the clear bit
        sh      t0,QSIM_P1_INT_OFS(s3)      # clear the interrupt 

        jal     hd64570_modem_interrupt     # call modem interrupt
        move    a0,s1                       # load up idb 1 param (bd)

qsim_sca_0_done:       
        lw      s1,12(sp)                   # restore s1
        lw      s4,8(sp)                    # restore s4 
        lw      s5,4(sp)                    # restore s5
        lw      s6,0(sp)                    # restore s6

        j       s1                          # return
        addiu   sp,sp,16                    # remove stack frame (bd)
        
qsim_sca_1:
        addiu   s0,s0,LEV2_IDB_C            # s0 to point to IDB C pointer
        lw      s1,4(s0)                    # read IDB D pointer in s1
        lui     t0,QSIM_PORT23_OFS          # port 2/3 offset (ld)
        lw      s0,0(s0)                    # read IDB C pointer in s0
        addu    s3,s3,t0                    # slot base for 2nd half of 4T

        andi    t0,s2,QSIM_SCA_1_INT        # mask for SCA1 interrupt
        beq     t0,zero,qsim_sca_1_p2       # SCA1 is interrupting us ?
	nop				    # (bd slot)

        la      t1,qsim_wrapper_table       # get wrapper table address
        sll     a0,a0,3                     # 8 bytes/slot in wrapper table 
        addiu   a0,a0,4                     # 2nd entry for this slot
        addu    t1,t1,a0                    # get wrapper table entry address
        lw      t2,0(t1)                    # read the wrapper address
        lhu     s5,HD_ISR1_ISR0_OFS(s3)     # read ISR1 and ISR0 register

        jal     s4,t2                       # call the secondary wrapper 
        nop                                 # (bd slot)
        
qsim_sca_1_p2:
        andi    t0,s2,QSIM_MDM_INT_2        # mask for port 2 modem intr
        beq     t0,zero,qsim_sca_1_p3       # port 2 modem interrupt ?
        andi    s5,s2,QSIM_MDM_INT_3        # mask for port 3 modem intr (bd)  

        lhu     t0,QSIM_P2_INT_OFS(s3)      # read port 2 intr reg  
        nop                                 # load delay
        ori     t0,t0,QSIM_INT_CLEAR        # turn on the clear bit
        sh      t0,QSIM_P2_INT_OFS(s3)      # clear the interrupt 

        jal     hd64570_modem_interrupt     # call modem interrupt
        move    a0,s0                       # load up idb 0 param (bd)
        
qsim_sca_1_p3:
        beq     s5,zero,qsim_sca_1_done     # port 3 modem interrupt ?
        lhu     t0,QSIM_P3_INT_OFS(s3)      # read port 3 intr reg  (bd)

        ori     t0,t0,QSIM_INT_CLEAR        # turn on the clear bit
        sh      t0,QSIM_P3_INT_OFS(s3)      # clear the interrupt 

        jal     hd64570_modem_interrupt     # call modem interrupt
        move    a0,s1                       # load up idb 1 param (bd)

qsim_sca_1_done:       
        lw      s1,12(sp)                   # restore s1
        lw      s4,8(sp)                    # restore s4
        lw      s5,4(sp)                    # restore s5
        lw      s6,0(sp)                    # restore s6

        j       s1                          # return
        addiu   sp,sp,16                    # remove stack frame (bd)

        .set reorder
ENDFRAME(qsim_wrapper)

/*
 * qsim_tt - Seconadry interrupt wrapper for any half
 *           of 4T with dual full duplex ports.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 * 	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_tt (void)
 #
FRAME(qsim_tt,sp, 0,s4)
        .set    noreorder

        andi    t0,s5,HD_S0_RINT_MASK        # mask for port 0 RX intr bit
        beq     t0,zero,qsim_tt_check_TX0    # RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK        # mask for port 0 TX intr (bd) 

        jal     hd64570_RX_interrupt         # call Rx intr routine 
        move    a0,s0                        # load up idb 0 param (bd)

qsim_tt_check_TX0:
        beq     s6,zero,qsim_tt_check_DMIA0  # port 0 TX intr ? 
        nop				     # (bd slot)

        jal     hd64570_TX_interrupt         # call TX intr routine
        move    a0,s0                        # load up idb 0 param (bd)

qsim_tt_check_DMIA0:
        andi    t0,s5,HD_S0_RERR_MASK        # mask for port 0 RX buffer wrap
        beq     t0,zero,qsim_tt_check_c0_err # port 0 RX buffer wrap ?
        nop				     # (bd slot)

        jal     hd64570_DMIA_int             # call buffer wrap handler
        move    a0,s0                        # load up idb 0 param (bd)

qsim_tt_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)     # read port 0 ST1 reg 
        andi    s6,s5,HD_S1_RINT_MASK        # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK       # mask for underrun
        beq     t0,zero,qsim_tt_check_c1_int # underun ?
        nop				     # (bd slot)

        jal     hd64570_error_int            # call error hanlder
        move    a0,s0                        # load up idb0 param (bd)

qsim_tt_check_c1_int:
        beq     s6,zero,qsim_tt_check_TX1    # port 1 RX intr ?
        nop				     # (bd slot)

        jal     hd64570_RX_interrupt	     # call Rx intr routine 
        move    a0,s1                        # load up idb 1 param (bd)

qsim_tt_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK        # mask for pot 1 TX intr
        beq     t0,zero,qsim_tt_check_DMIA1  # port 1 TX intr ?
        andi    s6,s5,HD_S1_RERR_MASK        # mask port 1 RX buffer wrap (bd)

        jal     hd64570_TX_interrupt         # call TX intr routine 
        move    a0,s1                        # load up idb 1 param (bd)

qsim_tt_check_DMIA1:
        beq     s6,zero,qsim_tt_check_c1_err # port 1 RX buffer wrap ?
        nop				     # (bd slot)

        jal     hd64570_DMIA_int             # call buffer wrap handler
        move    a0,s1                        # load up idb 1 param (bd)

qsim_tt_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)     # read port 1 ST1 reg 
        nop				     # load delay
        andi    t0,t0,HD_ST1_UDRN_MASK       # mask for underrun
        beq     t0,zero,qsim_tt_exit         # underun ?
        nop     			     # (bd slot)

        jal     hd64570_error_int            # call error handler
        move    a0,s1                        # load up idb 1 param (bd)

qsim_tt_exit:
        j       s4			     # return
        nop				     # (bd slot)

        .set    reorder
ENDFRAME(qsim_tt)
        
/*
 * qsim_thdthd - Seconadry interrupt wrapper for either half
 *               of 4T with dual half duplex ports.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_thdthd (void)
 #
FRAME(qsim_thdthd,sp, 0,s4)
        .set noreorder

        andi    t0,s5,HD_S0_RINT_MASK            # mask for port 0 RX intr bit
        beq     t0,zero,qsim_thdthd_check_TX0    # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_HDX_interrupt         # call hdx RX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdthd_check_TX0:
        beq     s6,zero,qsim_thdthd_check_txint0 # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdthd_check_txint0:
        andi    t0,s5,HD_S0_TXINT_MASK           # mask for port 0 txint intr
        beq     t0,zero,qsim_thdthd_check_DMIA0  # port 0 txint ?
        andi    s6,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s0                            # load up idb 0 param (bd)
    
qsim_thdthd_check_DMIA0:
        beq     s6,zero,qsim_thdthd_check_c0_err # port 0 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler  
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdthd_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        andi    s6,s5,HD_S1_RINT_MASK            # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thdthd_check_c1_int # port 0 underrun ? 
        nop					 # (bd slot)

        jal     hd64570_error_int                # call error handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdthd_check_c1_int:
        beq     s6,zero,qsim_thdthd_check_TX1    # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_HDX_interrupt         # call hdx RX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thdthd_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_thdthd_check_txint1 # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thdthd_check_txint1:
        beq     s6,zero,qsim_thdthd_check_DMIA1  # port 1 txint intr ?
        nop					 # (bd slot)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thdthd_check_DMIA1:
        andi    t0,s5,HD_S1_RERR_MASK            # mask for RX buffer wrap
        beq     t0,zero,qsim_thdthd_check_c1_err # port 1 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thdthd_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thdthd_check_timer  # port 1 underrun ? 
        andi    s6,s6,0xF0                       # mask for timer intr. (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)
        
qsim_thdthd_check_timer:
        beq     s6,zero,qsim_thdthd_exit         # any timer interrupts ? 
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 0 param 
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 1 param (bd)
        
qsim_thdthd_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder 
ENDFRAME(qsim_thdthd)


/*
 * qsim_thdt - Seconadry interrupt wrapper for either half of 4T with
 *             dual ports, half-duplex on first and full duplex second port.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_thdt (void)
 #
FRAME(qsim_thdt,sp, 0,s4)
        .set    noreorder 

        andi    t0,s5,HD_S0_RINT_MASK            # mask for port 0 RX intr bit
        beq     t0,zero,qsim_thdt_check_TX0      # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_HDX_interrupt         # call hdx RX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdt_check_TX0:
        beq     s6,zero,qsim_thdt_check_txint0   # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdt_check_txint0:
        andi    t0,s5,HD_S0_TXINT_MASK           # mask for port 0 txint intr
        beq     t0,zero,qsim_thdt_check_DMIA0    # port 0 txint ?
        andi    s6,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdt_check_DMIA0:
        beq     s6,zero,qsim_thdt_check_c0_err   # port 0 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdt_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        andi    s6,s5,HD_S1_RINT_MASK            # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thdt_check_c1_int   # port 0 underrun ?
        nop					 # (bd slot)

        jal     hd64570_error_int                # call error handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thdt_check_c1_int:
        beq     s6,zero,qsim_thdt_check_TX1      # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_interrupt             # call hdx RX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thdt_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_thdt_check_DMIA1    # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_interrupt             # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thdt_check_DMIA1:
        andi    t0,s5,HD_S1_RERR_MASK            # mask for RX buffer wrap
        beq     t0,zero,qsim_thdt_check_c1_err   # port 1 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thdt_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thdt_check_timer    # port 1 underrun ?
        andi    s6,s6,0xF0                       # mask for timer intr (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thdt_check_timer:
        beq     s6,zero,qsim_thdt_exit           # any timer interrupts ?
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 1 param (bd)
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 0 param (bd)

qsim_thdt_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder
ENDFRAME(qsim_thdt)

/*
 * qsim_tthd - Seconadry interrupt wrapper for either half of 4T with
 *             dual ports, full duplex on first and half duplex second port.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */
 #
 # void qsim_tthd (void)
 #
FRAME(qsim_tthd,sp, 0,s4)
        .set    noreorder

        andi    t0,s5,HD_S0_RINT_MASK            # mask for port 0 RX intr bit
        beq     t0,zero,qsim_tthd_check_TX0      # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_interrupt             # call hdx RX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_tthd_check_TX0:
        beq     s6,zero,qsim_tthd_check_DMIA0    # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_interrupt             # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_tthd_check_DMIA0:
        andi    t0,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)
        beq     t0,zero,qsim_tthd_check_c0_err   # port 0 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_tthd_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        andi    s6,s5,HD_S1_RINT_MASK            # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_tthd_check_c1_int   # underun ?
        nop					 # (bd slot)

        jal     hd64570_error_int                # call error hanlder
        move    a0,s0                            # load up idb0 param (bd)

qsim_tthd_check_c1_int:
        beq     s6,zero,qsim_tthd_check_TX1      # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_HDX_interrupt	 # call RX intr handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_tthd_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_tthd_check_txint1   # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_tthd_check_txint1:
        beq     s6,zero,qsim_tthd_check_DMIA1    # port 1 txint intr ?
        nop					 # (bd slot)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_tthd_check_DMIA1:
        andi    t0,s5,HD_S1_RERR_MASK            # mask for RX buffer wrap
        beq     t0,zero,qsim_tthd_check_c1_err   # port 1 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_tthd_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_tthd_check_timer    # port 1 underrun ?
        andi    s6,s6,0xF0                       # mask for timer intr (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_tthd_check_timer:
        beq     s6,zero,qsim_tthd_exit           # any timer interrupts ?
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 1 param (bd)
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 0 param (bd)

qsim_tthd_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder
ENDFRAME(qsim_tthd)

/*
 * qsim_t0 - Secondary interrupt wrapper for either half of
 *           4T with a single full duplex port on port 0
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_t0 (void)
 #
FRAME(qsim_t0,sp, 0,s4)
        .set    noreorder
         
        andi    t0,s5,HD_S0_RINT_MASK        # mask for port 0 RX intr bit
        beq     t0,zero,qsim_t0_check_TX     # RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK        # mask for port 0 TX intr (bd)

        jal     hd64570_RX_interrupt         # call Rx intr routine
        move    a0,s0                        # load up idb 0 param (bd)

qsim_t0_check_TX:
        beq     s6,zero,qsim_t0_check_DMIA   # port 0 TX intr ?
        nop				     # (bd slot)

        jal     hd64570_TX_interrupt         # call TX intr routine
        move    a0,s0                        # load up idb 0 param (bd)

qsim_t0_check_DMIA:
        andi    t0,s5,HD_S0_RERR_MASK        # mask for port 0 RX buffer wrap
        beq     t0,zero,qsim_t0_check_err    # port 0 RX buffer wrap ?
        nop				     # (bd slot)

        jal     hd64570_DMIA_int             # call buffer wrap handler
        move    a0,s0                        # load up idb 0 param (bd)

qsim_t0_check_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)     # read port 0 ST1 reg
        nop				     # load delay
        andi    t0,t0,HD_ST1_UDRN_MASK       # mask for underrun
        beq     t0,zero,qsim_t0_exit         # underun ?
        nop				     # (bd slot)

        jal     hd64570_error_int            # call error handler
        move    a0,s0                        # load up idb0 param (bd)

qsim_t0_exit:
        j       s4			     # return
        nop				     # (bd slot)
        
        .set    reorder
ENDFRAME(qsim_t0) 
    
/*
 * qsim_0t - Secondary interrupt wrapper for either half of
 *           4T with a single full duplex port on port 1
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_0t (void)
 #
FRAME(qsim_0t,sp, 0,s4)
        .set    noreorder

	andi    t0,s5,HD_S1_RINT_MASK        # mask for port 1 RX intr
        beq     t0,zero,qsim_0t_check_TX     # port 1 RX intr ?
        nop				     # (bd slot)

        jal     hd64570_RX_interrupt
        move    a0,s1                        # load up idb 1 param (bd)

qsim_0t_check_TX:
        andi    t0,s5,HD_S1_TINT_MASK        # mask for pot 1 TX intr
        beq     t0,zero,qsim_0t_check_DMIA   # port 1 TX intr ?
        andi    s6,s5,HD_S1_RERR_MASK        # mask port 1 RX buffer wrap (bd)

        jal     hd64570_TX_interrupt         # call TX intr routine
        move    a0,s1                        # load up idb 1 param (bd)

qsim_0t_check_DMIA:
        beq     s6,zero,qsim_0t_check_err    # port 1 RX buffer wrap ?
        nop				     # (bd slot)

        jal     hd64570_DMIA_int             # call buffer wrap handler
        move    a0,s1                        # load up idb 1 param (bd)

qsim_0t_check_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)     # read port 1 ST1 reg
        nop				     # laod delay
        andi    t0,t0,HD_ST1_UDRN_MASK       # mask for underrun
        beq     t0,zero,qsim_0t_exit         # underun ?
        nop				     # (bd slot)

        jal     hd64570_error_int            # call error hanlder
        move    a0,s1                        # load up idb 1 param (bd)

qsim_0t_exit:
        j       s4			     # return
        nop				     # (bd slot)
        
        .set    reorder
ENDFRAME(qsim_0t) 

/*
 * qsim_thd0 - Secondary interrupt wrapper for either half of
 *             4T with a single half duplex port on port 0
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_thd0 (void)
 #
FRAME(qsim_thd0,sp, 0,s4)
        .set    noreorder

        andi    t0,s5,HD_S0_RINT_MASK            # mask for port 0 RX intr bit
        beq     t0,zero,qsim_thd0_check_TX       # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_HDX_interrupt         # call hdx RX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_check_TX:
        beq     s6,zero,qsim_thd0_check_txint    # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_check_txint:
        andi    t0,s5,HD_S0_TXINT_MASK           # mask for port 0 txint intr
        beq     t0,zero,qsim_thd0_check_DMIA     # port 0 txint
        andi    s6,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_check_DMIA:
        beq     s6,zero,qsim_thd0_check_err      # port 0 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_check_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd0_check_timer    # port 0 underrun ?
        andi    s6,s6,0xF0                       # mask for timer intr (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_check_timer:
        beq     s6,zero,qsim_thd0_exit           # any timer interrupts ?
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 0 param
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 1 param (bd)

qsim_thd0_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder
ENDFRAME(qsim_thd0)
        
/*
 * qsim_0thd - Secondary interrupt wrapper for either half of
 *             4T with a single half duplex port on port 1
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # qsim_0thd (void)
 #
FRAME(qsim_0thd,sp, 0,s4)
        .set    noreorder

        andi    t0,s5,HD_S1_RINT_MASK            # mask for port 1 RX intr (ld)
        beq     t0,zero,qsim_0thd_check_TX       # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_HDX_interrupt	 # call hdx RX intr routine 
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_check_TX:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_0thd_check_txint    # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_check_txint:
        beq     s6,zero,qsim_0thd_check_DMIA     # port 1 txint intr ?
        nop					 # (bd slot)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_check_DMIA:
        andi    t0,s5,HD_S1_RERR_MASK            # mask for RX buffer wrap
        beq     t0,zero,qsim_0thd_check_err      # port 1 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_check_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_0thd_check_timer    # port 1 underrun ?
        andi    s6,s6,0xF0                       # mask for timer intr (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_check_timer:
        beq     s6,zero,qsim_0thd_exit           # any timer interrupts ?
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 0 param
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 1 param (bd)

qsim_0thd_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set reorder
ENDFRAME(qsim_0thd)

/*
 * qsim_thd0_bsc - Secondary interrupt wrapper for either half of
 *             4T with a single half duplex BSC port on port 0
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_thd0_bsc (void)
 #
FRAME(qsim_thd0_bsc,sp, 0,s4)
        .set    noreorder

        andi    t0,s5,HD_S0_RXRDY_MASK           # mask for port 0 RX intr bit
        beq     t0,zero,qsim_thd0_bsc_check_TX   # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_HDX_BSC_interrupt     # call hdx BSC RX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_check_TX:
        beq     s6,zero,qsim_thd0_bsc_check_txint # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_check_txint:
        andi    t0,s5,HD_S0_TXINT_MASK           # mask for port 0 txint intr
        beq     t0,zero,qsim_thd0_bsc_check_err  # 
        andi    s6,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_check_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd0_bsc_check_timer # port 0 underrun ?
        andi    s6,s6,0xF0                       # mask for timer intr (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_check_timer:
        beq     s6,zero,qsim_thd0_bsc_exit           # any timer interrupts ?
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 0 param
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 1 param (bd)

qsim_thd0_bsc_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder
ENDFRAME(qsim_thd0_bsc)
        
/*
 * qsim_0thd_bsc - Secondary interrupt wrapper for either half of
 *             4T with a single half duplex BSC port on port 1
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # qsim_0thd_bsc (void)
 #
FRAME(qsim_0thd_bsc,sp, 0,s4)
        .set    noreorder

        andi    t0,s5,HD_S1_RXRDY_MASK           # mask for port 1 RX intr (ld)
        beq     t0,zero,qsim_0thd_bsc_check_TX   # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_HDX_BSC_interrupt	 # call hdx RX BSC intr routine 
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_bsc_check_TX:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_0thd_bsc_check_txint # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_bsc_check_txint:
        beq     s6,zero,qsim_0thd_bsc_check_err  # port 1 txint intr ?
        nop					 # (bd slot)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_bsc_check_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_0thd_bsc_check_timer # port 1 underrun ?
        andi    s6,s6,0xF0                       # mask for timer intr (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_0thd_bsc_check_timer:
        beq     s6,zero,qsim_0thd_bsc_exit       # any timer interrupts ?
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 0 param
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 1 param (bd)

qsim_0thd_bsc_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set reorder
ENDFRAME(qsim_0thd_bsc)

/*
 * qsim_thd_bsc_thd_bsc - Seconadry interrupt wrapper for either half
 *               of 4T with dual half duplex BSC ports.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_thd_bsc_thd_bsc (void)
 #
FRAME(qsim_thd_bsc_thd_bsc,sp, 0,s4)
        .set noreorder

        andi    t0,s5,HD_S0_RXRDY_MASK           # mask for port 0 RX intr bit
        beq     t0,zero,qsim_thd_bsc_thd_bsc_check_TX0 # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_HDX_BSC_interrupt     # call hdx RX BSC intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd_bsc_thd_bsc_check_TX0:
        beq     s6,zero,qsim_thd_bsc_thd_bsc_check_txint0 # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd_bsc_thd_bsc_check_txint0:
        andi    t0,s5,HD_S0_TXINT_MASK           # mask for port 0 txint intr
        beq     t0,zero,qsim_thd_bsc_thd_bsc_check_c0_err  # port 0 txint ?
        andi    s6,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s0                            # load up idb 0 param (bd)
    
qsim_thd_bsc_thd_bsc_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        andi    s6,s5,HD_S1_RXRDY_MASK           # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd_bsc_thd_bsc_check_c1_int # port 0 underrun ? 
        nop					 # (bd slot)

        jal     hd64570_error_int                # call error handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd_bsc_thd_bsc_check_c1_int:
        beq     s6,zero,qsim_thd_bsc_thd_bsc_check_TX1    # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_HDX_BSC_interrupt     # call hdx RX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd_bsc_thd_bsc_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_thd_bsc_thd_bsc_check_txint1 # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd_bsc_thd_bsc_check_txint1:
        beq     s6,zero,qsim_thd_bsc_thd_bsc_check_c1_err  # port 1 txint intr ?
        nop					 # (bd slot)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd_bsc_thd_bsc_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd_bsc_thd_bsc_check_timer  # port 1 underrun ? 
        andi    s6,s6,0xF0                       # mask for timer intr. (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd_bsc_thd_bsc_check_timer:
        beq     s6,zero,qsim_thd_bsc_thd_bsc_exit         # any timer interrupts ? 
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 0 param 
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 1 param (bd)
        
qsim_thd_bsc_thd_bsc_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder 
ENDFRAME(qsim_thd_bsc_thd_bsc)

/*
 * qsim_thd0_bsc_t - Seconadry interrupt wrapper for either half of 4T with
 *             dual ports, half-duplex BSC on first and full duplex 
 *             SDLC second port.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_thd0_bsc_t (void)
 #
FRAME(qsim_thd0_bsc_t,sp, 0,s4)
        .set    noreorder 

        andi    t0,s5,HD_S0_RXRDY_MASK           # mask for port 0 RX intr bit
        beq     t0,zero,qsim_thd0_bsc_t_check_TX0 # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_HDX_BSC_interrupt     # call hdx RX BSC intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_t_check_TX0:
        beq     s6,zero,qsim_thd0_bsc_t_check_txint0 # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_t_check_txint0:
        andi    t0,s5,HD_S0_TXINT_MASK           # mask for port 0 txint intr
        beq     t0,zero,qsim_thd0_bsc_t_check_c0_err # port 0 txint ?
        andi    s6,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_t_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        andi    s6,s5,HD_S1_RINT_MASK            # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd0_bsc_t_check_c1_int  # port 0 underrun ?
        nop					 # (bd slot)

        jal     hd64570_error_int                # call error handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_t_check_c1_int:
        beq     s6,zero,qsim_thd0_bsc_t_check_TX1      # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_interrupt             # call hdx RX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_bsc_t_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_thd0_bsc_t_check_DMIA1    # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_interrupt             # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_bsc_t_check_DMIA1:
        andi    t0,s5,HD_S1_RERR_MASK            # mask for RX buffer wrap
        beq     t0,zero,qsim_thd0_bsc_t_check_c1_err   # port 1 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_bsc_t_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd0_bsc_t_check_timer    # port 1 underrun ?
        andi    s6,s6,0xF0                       # mask for timer intr (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_bsc_t_check_timer:
        beq     s6,zero,qsim_thd0_bsc_t_exit           # any timer interrupts ?
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 1 param (bd)
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_t_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder
ENDFRAME(qsim_thd0_bsc_t)

/*
 * qsim_t_0thd_bsc - Seconadry interrupt wrapper for either half of 4T with
 *             dual ports, full duplex SDLC on first and half duplex 
 *             BSC second port.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */
 #
 # void qsim_t_0thd_bsc (void)
 #
FRAME(qsim_t_0thd_bsc,sp, 0,s4)
        .set    noreorder

        andi    t0,s5,HD_S0_RINT_MASK            # mask for port 0 RX intr bit
        beq     t0,zero,qsim_t_0thd_bsc_check_TX0      # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_interrupt             # call RX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_t_0thd_bsc_check_TX0:
        beq     s6,zero,qsim_t_0thd_bsc_check_DMIA0    # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_interrupt             # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_t_0thd_bsc_check_DMIA0:
        andi    t0,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)
        beq     t0,zero,qsim_t_0thd_bsc_check_c0_err   # port 0 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_t_0thd_bsc_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        andi    s6,s5,HD_S1_RXRDY_MASK           # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_t_0thd_bsc_check_c1_int   # underun ?
        nop					 # (bd slot)

        jal     hd64570_error_int                # call error hanlder
        move    a0,s0                            # load up idb0 param (bd)

qsim_t_0thd_bsc_check_c1_int:
        beq     s6,zero,qsim_t_0thd_bsc_check_TX1 # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_HDX_BSC_interrupt	 # call RX hdx BSC intr handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_t_0thd_bsc_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_t_0thd_bsc_check_txint1 # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_t_0thd_bsc_check_txint1:
        beq     s6,zero,qsim_t_0thd_bsc_check_c1_err # port 1 txint intr ?
        nop					 # (bd slot)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_t_0thd_bsc_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_t_0thd_bsc_check_timer  # port 1 underrun ?
        andi    s6,s6,0xF0                       # mask for timer intr (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_t_0thd_bsc_check_timer:
        beq     s6,zero,qsim_t_0thd_bsc_exit     # any timer interrupts ?
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 1 param (bd)
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 0 param (bd)

qsim_t_0thd_bsc_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder
ENDFRAME(qsim_t_0thd_bsc)

/*
 * qsim_thd0_bsc_0thd - Seconadry interrupt wrapper for either half
 *               of 4T with dual half duplex ports; half-duplex BSC on 
 *               first and half-duplex SDLC second port.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_thd0_bsc_0thd (void)
 #
FRAME(qsim_thd0_bsc_0thd,sp, 0,s4)
        .set noreorder

        andi    t0,s5,HD_S0_RXRDY_MASK           # mask for port 0 RX intr bit
        beq     t0,zero,qsim_thd0_bsc_0thd_check_TX0 # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_HDX_BSC_interrupt     # call hdx RX BSC intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_0thd_check_TX0:
        beq     s6,zero,qsim_thd0_bsc_0thd_check_txint0 # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_0thd_check_txint0:
        andi    t0,s5,HD_S0_TXINT_MASK           # mask for port 0 txint intr
        beq     t0,zero,qsim_thd0_bsc_0thd_check_c0_err  # port 0 txint ?
        andi    s6,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s0                            # load up idb 0 param (bd)
    
qsim_thd0_bsc_0thd_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        andi    s6,s5,HD_S1_RINT_MASK            # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd0_bsc_0thd_check_c1_int # port 0 underrun ? 
        nop					 # (bd slot)

        jal     hd64570_error_int                # call error handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_bsc_0thd_check_c1_int:
        beq     s6,zero,qsim_thd0_bsc_0thd_check_TX1    # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_HDX_interrupt         # call hdx RX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_bsc_0thd_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_thd0_bsc_0thd_check_txint1 # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_bsc_0thd_check_txint1:
        beq     s6,zero,qsim_thd0_bsc_0thd_check_DMIA1  # port 1 txint intr ?
        nop					 # (bd slot)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_bsc_0thd_check_DMIA1:
        andi    t0,s5,HD_S1_RERR_MASK            # mask for RX buffer wrap
        beq     t0,zero,qsim_thd0_bsc_0thd_check_c1_err # port 1 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_bsc_0thd_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd0_bsc_0thd_check_timer  # port 1 underrun ? 
        andi    s6,s6,0xF0                       # mask for timer intr. (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)
        
qsim_thd0_bsc_0thd_check_timer:
        beq     s6,zero,qsim_thd0_bsc_0thd_exit         # any timer interrupts ? 
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 0 param 
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 1 param (bd)
        
qsim_thd0_bsc_0thd_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder 
ENDFRAME(qsim_thd0_bsc_0thd)

/*
 * qsim_thd0_0thd_bsc - Seconadry interrupt wrapper for either half
 *               of 4T with dual half duplex ports; half-duplex SDLC on 
 *               first and half-duplex BSC second port.
 *
 * Entry :  s0 = idb pointer for port 0 
 *          s1 = idb pointer for port 1
 *          s2 = NIM intr source register 
 *          s3 = Slot base address
 *          s4 = Return address
 *	    s5 = ISR1/ISR0 registers
 *          s6 may be used as scratch registers,
 *          all other s registers must be preserved.
 */

 #
 # void qsim_thd0_0thd_bsc (void)
 #
FRAME(qsim_thd0_0thd_bsc,sp, 0,s4)
        .set noreorder

        andi    t0,s5,HD_S0_RINT_MASK            # mask for port 0 RX intr bit
        beq     t0,zero,qsim_thd0_0thd_bsc_check_TX0    # port 0 RX intr ?
        andi    s6,s5,HD_S0_TINT_MASK            # mask for port 0 TX intr (bd)

        jal     hd64570_RX_HDX_interrupt         # call hdx RX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_0thd_bsc_check_TX0:
        beq     s6,zero,qsim_thd0_0thd_bsc_check_txint0 # port 0 TX intr ?
        nop					 # (bd slot)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_0thd_bsc_check_txint0:
        andi    t0,s5,HD_S0_TXINT_MASK           # mask for port 0 txint intr
        beq     t0,zero,qsim_thd0_0thd_bsc_check_DMIA0  # port 0 txint ?
        andi    s6,s5,HD_S0_RERR_MASK            # mask for RX buffer wrap (bd)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s0                            # load up idb 0 param (bd)
    
qsim_thd0_0thd_bsc_check_DMIA0:
        beq     s6,zero,qsim_thd0_0thd_bsc_check_c0_err # port 0 RX buffer wrap ?
        nop					 # (bd slot)

        jal     hd64570_DMIA_int                 # call buffer wrap handler  
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_0thd_bsc_check_c0_err:
        lhu     t0,HD_C0_ST1_ST0_OFS(s3)         # read port 0 ST1 reg
        andi    s6,s5,HD_S1_RXRDY_MASK           # mask for port 1 RX intr (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd0_0thd_bsc_check_c1_int # port 0 underrun ? 
        nop					 # (bd slot)

        jal     hd64570_error_int                # call error handler
        move    a0,s0                            # load up idb 0 param (bd)

qsim_thd0_0thd_bsc_check_c1_int:
        beq     s6,zero,qsim_thd0_0thd_bsc_check_TX1 # port 1 RX intr ?
        nop					 # (bd slot)

        jal     hd64570_RX_HDX_BSC_interrupt     # call hdx RX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_0thd_bsc_check_TX1:
        andi    t0,s5,HD_S1_TINT_MASK            # mask for port 1 TX intr
        beq     t0,zero,qsim_thd0_0thd_bsc_check_txint1 # port 1 TX intr ?
        andi    s6,s5,HD_S1_TXINT_MASK           # mask for port 1 txint (bd)

        jal     hd64570_TX_HDX_interrupt         # call hdx TX intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_0thd_bsc_check_txint1:
        beq     s6,zero,qsim_thd0_0thd_bsc_check_c1_err  # port 1 txint intr ?
        nop					 # (bd slot)

        jal     hd64570_HDX_txint                # call txint intr routine
        move    a0,s1                            # load up idb 1 param (bd)

qsim_thd0_0thd_bsc_check_c1_err:
        lhu     t0,HD_C1_ST1_ST0_OFS(s3)         # read port 1 ST1 reg
        lhu     s6,HD_NONE_ISR2_OFS(s3)          # read ISR2 reg (ld)
        andi    t0,t0,HD_ST1_UDRN_MASK           # mask for underrun
        beq     t0,zero,qsim_thd0_0thd_bsc_check_timer  # port 1 underrun ? 
        andi    s6,s6,0xF0                       # mask for timer intr. (bd)

        jal     hd64570_error_int                # call error handler
        move    a0,s1                            # load up idb 1 param (bd)
        
qsim_thd0_0thd_bsc_check_timer:
        beq     s6,zero,qsim_thd0_0thd_bsc_exit         # any timer interrupts ? 
        nop					 # (bd slot)

        move    a0,s1                            # load up idb 0 param 
        jal     hd64570_timer_interrupt          # call timer interrupt routine
        move    a1,s0                            # load up idb 1 param (bd)
        
qsim_thd0_0thd_bsc_exit:
        j       s4				 # return
        nop					 # (bd slot)

        .set    reorder 
ENDFRAME(qsim_thd0_0thd_bsc)


