/* $Id: asm_c4000_hd64570.S,v 3.2 1995/11/17 08:39:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c4000_hd64570.S,v $
 *------------------------------------------------------------------
 * Assembly wrapper for Quad Serial HD64570 NIM 
 *
 * March 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_c4000_hd64570.S,v $
| Revision 3.2  1995/11/17  08:39:45  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:32  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:46  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c4000.h"

/* 4T Interrupt source register bits. */
#define QSIM_MDM_INT_0        0
#define QSIM_MDM_INT_1        1
#define QSIM_MDM_INT_2        2
#define QSIM_MDM_INT_3        3
#define QSIM_SENSE2_0         4
#define QSIM_SENSE2_1         5
#define QSIM_SENSE2_2         6
#define QSIM_SENSE2_3         7
#define QSIM_SCA_0_INT        8
#define QSIM_SCA_1_INT        9

#define QSIM_INT_MASK         0x30F
#define QSIM_INT_CLEAR        0x1000

/* HD64570_ISR1_ISR0 bits. */
#define HD64570_S0_RXRDY     0       /* ISR0 */
#define HD64570_S0_RXINT     2       /* ISR0 */
#define HD64570_S0_TXINT     3       /* ISR0 */
#define HD64570_S1_RXRDY     4       /* ISR0 */
#define HD64570_S1_RXINT     6       /* ISR0 */
#define HD64570_S1_TXINT     7       /* ISR0 */
#define HD64570_S0_RERR      8       /* ISR1 */
#define HD64570_S0_RINT	     9       /* ISR1 */
#define HD64570_S0_TINT	    11       /* ISR1 */
#define HD64570_S1_RERR     12       /* ISR1 */
#define HD64570_S1_RINT	    13       /* ISR1 */
#define HD64570_S1_TINT	    15       /* ISR1 */

/* HD64570_ST1_ST0 bits. */
#define HD64570_IDLE 14
#define HD64570_UDRN 15

/* HD64570_ST3_ST2 bits. */
#define HD64570_OVRN 3

/* Registers to save on 4T interrupts.  */
#define QSIM_REGS        d0-d3/a0-a5


 |      Start of 4T (qsim) interrupt handling code.
 |
 |      4T in Slot 0
 | 
        .globl _qsim_slot0a
_qsim_slot0a:
	moveml	QSIM_REGS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

        movw    QSIM_SLOT0_INT_SRC, d2  | read slot 0 interrupt source register 
        andw    #QSIM_INT_MASK, d2      | check interrupt bits
        jeq     qsim_slot0a_no_int
        
        btst    #QSIM_SCA_0_INT, d2     | check for SCA0 interrupt.
        jeq     qsim_slot0a_check_mp0   | no, check for port 0 modem interrupt
        movl    #QSIM_SLOT0A_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT1A, a3       | pass port 0 idb 
        movl    #HWIDB_SLOT1B, a4       | pass port 1 idb
        movl    QSIM_SLOT0A_WRAPPER,a5 
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot0a_check_mp0:
        btst    #QSIM_MDM_INT_0, d2     | check for port 0 modem interrupt
        jeq     qsim_slot0a_check_mp1   | no, check for port 1 modem interrupt
        movw    QSIM_SLOT0_INT_0, d3    | read port 0 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT0_INT_0    | clear port 0 modem interrupt 
        movl    HWIDB_SLOT1A, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot0a_check_mp1:
        btst    #QSIM_MDM_INT_1, d2     | check for port 1 modem interrupt
        jeq     qsim_slot0a_check_0b
        movw    QSIM_SLOT0_INT_1, d3    | read port 1 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT0_INT_1    | clear port 1 modem interrupt 
        movl    HWIDB_SLOT1B, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
        |
        | Check the B side of slot 0 as well.
        |
qsim_slot0a_check_0b:
        btst    #QSIM_SCA_1_INT, d2     | check for SCA1 interrupt.
        jeq     qsim_slot0a_check_mp2   | no, check for port 2 modem interrupt
        movl    #QSIM_SLOT0B_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT1C, a3       | pass port 2 idb
        movl    #HWIDB_SLOT1D, a4       | pass port 3 idb
        movl    QSIM_SLOT0B_WRAPPER,a5
        jsr     a5@                     | call the wrapper for SCA1
qsim_slot0a_check_mp2:
        btst    #QSIM_MDM_INT_2, d2     | check for port 2 modem interrupt
        jeq     qsim_slot0a_check_mp3   | no, check for port 3 modem interrupt
        movw    QSIM_SLOT0_INT_2, d3    | read port 2 interrupt register
        orw     #QSIM_INT_CLEAR, d3
        movw    d3, QSIM_SLOT0_INT_2    | clear the modem interrupt
        movl    HWIDB_SLOT1C, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot0a_check_mp3:
        btst    #QSIM_MDM_INT_3, d2     | check for port 3 modem interrupt
	jeq	qsim_slot0a_no_int
        movw    QSIM_SLOT0_INT_3, d3    | read port 1 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT0_INT_3    | clear port 3 modem interrupt
        movl    HWIDB_SLOT1D, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot0a_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,QSIM_REGS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
        

	.globl _qsim_slot0b
_qsim_slot0b:
	moveml	QSIM_REGS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

        movw    QSIM_SLOT0_INT_SRC, d2  | read slot 0 interrupt source register 
        andw    #QSIM_INT_MASK, d2      | check interrupt bits
        jeq     qsim_slot0b_no_int

        btst    #QSIM_SCA_1_INT, d2     | check for SCA1 interrupt.
        jeq     qsim_slot0b_check_mp2   | no, check for port 2 modem interrupts
        movl    #QSIM_SLOT0B_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT1C, a3       | pass port 2 idb 
        movl    #HWIDB_SLOT1D, a4       | pass port 3 idb
        movl    QSIM_SLOT0B_WRAPPER,a5 
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot0b_check_mp2:
        btst    #QSIM_MDM_INT_2, d2     | check for port 2 modem interrupt
        jeq     qsim_slot0b_check_mp3   | no, check for port 3 modem interrupt
        movw    QSIM_SLOT0_INT_2, d3    | read port 2 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT0_INT_2    | clear port 2 modem interrupt 
        movl    HWIDB_SLOT1C, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot0b_check_mp3:
        btst    #QSIM_MDM_INT_3, d2     | check for port 3 modem interrupt
        jeq     qsim_slot0b_check_0a
        movw    QSIM_SLOT0_INT_3, d3    | read port 3 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT0_INT_3    | clear port 3 modem interrupt 
        movl    HWIDB_SLOT1D, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
        |
        | Check the A side of slot 0 as well.
        |
qsim_slot0b_check_0a:
        btst    #QSIM_SCA_0_INT, d2     | check for SCA0 interrupt.
        jeq     qsim_slot0b_check_mp0   | no, check for port 0 modem interrupt
        movl    #QSIM_SLOT0A_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT1A, a3       | pass port 0 idb
        movl    #HWIDB_SLOT1B, a4       | pass port 1 idb
        movl    QSIM_SLOT0A_WRAPPER,a5
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot0b_check_mp0:
        btst    #QSIM_MDM_INT_0, d2     | check for port 0 modem interrupt
        jeq     qsim_slot0b_check_mp1   | no, check for port 1 modem interrupt
        movw    QSIM_SLOT0_INT_0, d3    | read port 0 interrupt register
        orw     #QSIM_INT_CLEAR, d3
        movw    d3, QSIM_SLOT0_INT_0    | clear port 0 modem interrupt
        movl    HWIDB_SLOT1A, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot0b_check_mp1:
        btst    #QSIM_MDM_INT_1, d2     | check for port 1 modem interrupt
        jeq     qsim_slot0b_no_int
        movw    QSIM_SLOT0_INT_1, d3    | read port 1 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT0_INT_1    | clear port 1 modem interrupt
        movl    HWIDB_SLOT1B, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot0b_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,QSIM_REGS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
        

 |
 |      4T in Slot 1
 | 
        .globl _qsim_slot1a
_qsim_slot1a:
	moveml	QSIM_REGS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

        movw    QSIM_SLOT1_INT_SRC, d2  | read slot 1 interrupt source register 
        andw    #QSIM_INT_MASK, d2      | check interrupt bits
        jeq     qsim_slot1a_no_int

        btst    #QSIM_SCA_0_INT, d2     | check for SCA0 interrupt.
        jeq     qsim_slot1a_check_mp0   | no, check for port 0 modem interrupt
        movl    #QSIM_SLOT1A_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT2A, a3       | pass port 0 idb 
        movl    #HWIDB_SLOT2B, a4       | pass port 1 idb
        movl    QSIM_SLOT1A_WRAPPER,a5 
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot1a_check_mp0:
        btst    #QSIM_MDM_INT_0, d2     | check for port 0 modem interrupt
        jeq     qsim_slot1a_check_mp1   | no, check for port 1 modem interrupt
        movw    QSIM_SLOT1_INT_0, d3    | read port 0 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT1_INT_0    | clear port 0 modem interrupt 
        movl    HWIDB_SLOT2A, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot1a_check_mp1:
        btst    #QSIM_MDM_INT_1, d2     | check for port 1 modem interrupt
        jeq     qsim_slot1a_check_0b
        movw    QSIM_SLOT1_INT_1, d3    | read port 1 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT1_INT_1    | clear port 1 modem interrupt 
        movl    HWIDB_SLOT2B, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
        |
        | Check the B side of slot 1 as well.
        |
qsim_slot1a_check_0b:
        btst    #QSIM_SCA_1_INT, d2     | check for SCA1 interrupt.
        jeq     qsim_slot1a_check_mp2   | no, check for port 2 modem interrupt
        movl    #QSIM_SLOT1B_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT2C, a3       | pass port 2 idb
        movl    #HWIDB_SLOT2D, a4       | pass port 3 idb
        movl    QSIM_SLOT1B_WRAPPER,a5
        jsr     a5@                     | call the wrapper for SCA1
qsim_slot1a_check_mp2:
        btst    #QSIM_MDM_INT_2, d2     | check for port 2 modem interrupt
        jeq     qsim_slot1a_check_mp3   | no, check for port 3 modem interrupt
        movw    QSIM_SLOT1_INT_2, d3    | read port 2 interrupt register
        orw     #QSIM_INT_CLEAR, d3
        movw    d3, QSIM_SLOT1_INT_2    | clear the modem interrupt
        movl    HWIDB_SLOT2C, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot1a_check_mp3:
        btst    #QSIM_MDM_INT_3, d2     | check for port 3 modem interrupt
	jeq	qsim_slot1a_no_int
        movw    QSIM_SLOT1_INT_3, d3    | read port 3 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT1_INT_3    | clear port 3 modem interrupt
        movl    HWIDB_SLOT2D, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot1a_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,QSIM_REGS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
        

	.globl _qsim_slot1b
_qsim_slot1b:
	moveml	QSIM_REGS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

        movw    QSIM_SLOT1_INT_SRC, d2  | read slot 1 interrupt source register 
        andw    #QSIM_INT_MASK, d2      | check interrupt bits
        jeq     qsim_slot1b_no_int

        btst    #QSIM_SCA_1_INT, d2     | check for SCA1 interrupt.
        jeq     qsim_slot1b_check_mp2   | no, check for port 2 modem interrupts
        movl    #QSIM_SLOT1B_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT2C, a3       | pass port 2 idb 
        movl    #HWIDB_SLOT2D, a4       | pass port 3 idb
        movl    QSIM_SLOT1B_WRAPPER,a5 
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot1b_check_mp2:
        btst    #QSIM_MDM_INT_2, d2     | check for port 2 modem interrupt
        jeq     qsim_slot1b_check_mp3   | no, check for port 3 modem interrupt
        movw    QSIM_SLOT1_INT_2, d3    | read port 2 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT1_INT_2    | clear port 2 modem interrupt 
        movl    HWIDB_SLOT2C, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot1b_check_mp3:
        btst    #QSIM_MDM_INT_3, d2     | check for port 3 modem interrupt
        jeq     qsim_slot1b_check_0a
        movw    QSIM_SLOT1_INT_3, d3    | read port 3 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT1_INT_3    | clear port 3 modem interrupt 
        movl    HWIDB_SLOT2D, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
        |
        | Check the A side of slot 1 as well.
        |
qsim_slot1b_check_0a:
        btst    #QSIM_SCA_0_INT, d2     | check for SCA0 interrupt.
        jeq     qsim_slot1b_check_mp0   | no, check for port 0 modem interrupt
        movl    #QSIM_SLOT1A_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT2A, a3       | pass port 0 idb
        movl    #HWIDB_SLOT2B, a4       | pass port 1 idb
        movl    QSIM_SLOT1A_WRAPPER,a5
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot1b_check_mp0:
        btst    #QSIM_MDM_INT_0, d2     | check for port 0 modem interrupt
        jeq     qsim_slot1b_check_mp1   | no, check for port 1 modem interrupt
        movw    QSIM_SLOT1_INT_0, d3    | read port 0 interrupt register
        orw     #QSIM_INT_CLEAR, d3
        movw    d3, QSIM_SLOT1_INT_0    | clear port 0 modem interrupt
        movl    HWIDB_SLOT2A, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot1b_check_mp1:
        btst    #QSIM_MDM_INT_1, d2     | check for port 1 modem interrupt
        jeq     qsim_slot1b_no_int
        movw    QSIM_SLOT1_INT_1, d3    | read port 1 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT1_INT_1    | clear port 1 modem interrupt
        movl    HWIDB_SLOT2B, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot1b_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,QSIM_REGS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception

 |
 |      4T in Slot 2
 | 
        .globl _qsim_slot2a
_qsim_slot2a:
	moveml	QSIM_REGS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

        movw    QSIM_SLOT2_INT_SRC, d2  | read slot 2 interrupt source register 
        andw    #QSIM_INT_MASK, d2      | check interrupt bits
        jeq     qsim_slot2a_no_int

        btst    #QSIM_SCA_0_INT, d2     | check for SCA0 interrupt.
        jeq     qsim_slot2a_check_mp0   | no, check for port 0 modem interrupt
        movl    #QSIM_SLOT2A_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT3A, a3       | pass port 0 idb 
        movl    #HWIDB_SLOT3B, a4       | pass port 1 idb
        movl    QSIM_SLOT2A_WRAPPER,a5 
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot2a_check_mp0:
        btst    #QSIM_MDM_INT_0, d2     | check for port 0 modem interrupt
        jeq     qsim_slot2a_check_mp1   | no, check for port 1 modem interrupt
        movw    QSIM_SLOT2_INT_0, d3    | read port 0 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT2_INT_0    | clear port 0 modem interrupt 
        movl    HWIDB_SLOT3A, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot2a_check_mp1:
        btst    #QSIM_MDM_INT_1, d2     | check for port 1 modem interrupt
        jeq     qsim_slot2a_check_0b
        movw    QSIM_SLOT2_INT_1, d3    | read port 1 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT2_INT_1    | clear port 1 modem interrupt 
        movl    HWIDB_SLOT3B, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
        |
        | Check the B side of slot 2 as well.
        |
qsim_slot2a_check_0b:
        btst    #QSIM_SCA_1_INT, d2     | check for SCA1 interrupt.
        jeq     qsim_slot2a_check_mp2   | no, check for port 2 modem interrupt
        movl    #QSIM_SLOT2B_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT3C, a3       | pass port 2 idb
        movl    #HWIDB_SLOT3D, a4       | pass port 3 idb
        movl    QSIM_SLOT2B_WRAPPER,a5
        jsr     a5@                     | call the wrapper for SCA1
qsim_slot2a_check_mp2:
        btst    #QSIM_MDM_INT_2, d2     | check for port 2 modem interrupt
        jeq     qsim_slot2a_check_mp3   | no, check for port 3 modem interrupt
        movw    QSIM_SLOT2_INT_2, d3    | read port 2 interrupt register
        orw     #QSIM_INT_CLEAR, d3
        movw    d3, QSIM_SLOT2_INT_2    | clear the modem interrupt
        movl    HWIDB_SLOT3C, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot2a_check_mp3:
        btst    #QSIM_MDM_INT_3, d2     | check for port 3 modem interrupt
	jeq	qsim_slot2a_no_int
        movw    QSIM_SLOT2_INT_3, d3    | read port 1 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT2_INT_3    | clear port 3 modem interrupt
        movl    HWIDB_SLOT3D, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot2a_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,QSIM_REGS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
        

	.globl _qsim_slot2b
_qsim_slot2b:
	moveml	QSIM_REGS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

        movw    QSIM_SLOT2_INT_SRC, d2  | read slot 2 interrupt source register 
        andw    #QSIM_INT_MASK, d2      | check interrupt bits
        jeq     qsim_slot2b_no_int

        btst    #QSIM_SCA_1_INT, d2     | check for SCA1 interrupt.
        jeq     qsim_slot2b_check_mp2   | no, check for port 2 modem interrupts
        movl    #QSIM_SLOT2B_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT3C, a3       | pass port 2 idb 
        movl    #HWIDB_SLOT3D, a4       | pass port 3 idb
        movl    QSIM_SLOT2B_WRAPPER,a5 
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot2b_check_mp2:
        btst    #QSIM_MDM_INT_2, d2     | check for port 2 modem interrupt
        jeq     qsim_slot2b_check_mp3   | no, check for port 3 modem interrupt
        movw    QSIM_SLOT2_INT_2, d3    | read port 2 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT2_INT_2    | clear port 2 modem interrupt 
        movl    HWIDB_SLOT3C, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot2b_check_mp3:
        btst    #QSIM_MDM_INT_3, d2     | check for port 3 modem interrupt
        jeq     qsim_slot2b_check_0a
        movw    QSIM_SLOT2_INT_3, d3    | read port 3 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT2_INT_3    | clear port 3 modem interrupt 
        movl    HWIDB_SLOT3D, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
        |
        | Check the A side of slot 2 as well.
        |
qsim_slot2b_check_0a:
        btst    #QSIM_SCA_0_INT, d2     | check for SCA0 interrupt.
        jeq     qsim_slot2b_check_mp0   | no, check for port 0 modem interrupt
        movl    #QSIM_SLOT2A_BASE, a2   | pass slot base
        movl    #HWIDB_SLOT3A, a3       | pass port 0 idb
        movl    #HWIDB_SLOT3B, a4       | pass port 1 idb
        movl    QSIM_SLOT2A_WRAPPER,a5
        jsr     a5@                     | call the wrapper for SCA0
qsim_slot2b_check_mp0:
        btst    #QSIM_MDM_INT_0, d2     | check for port 0 modem interrupt
        jeq     qsim_slot2b_check_mp1   | no, check for port 1 modem interrupt
        movw    QSIM_SLOT2_INT_0, d3    | read port 0 interrupt register
        orw     #QSIM_INT_CLEAR, d3
        movw    d3, QSIM_SLOT2_INT_0    | clear port 0 modem interrupt
        movl    HWIDB_SLOT3A, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot2b_check_mp1:
        btst    #QSIM_MDM_INT_1, d2     | check for port 1 modem interrupt
        jeq     qsim_slot2b_no_int
        movw    QSIM_SLOT2_INT_1, d3    | read port 1 interrupt register 
        orw     #QSIM_INT_CLEAR, d3  
        movw    d3, QSIM_SLOT2_INT_1    | clear port 1 modem interrupt
        movl    HWIDB_SLOT3B, sp@-      | push IDB argument for C routines
        jsr     _hd64570_modem_interrupt | call modem interrupt routine
qsim_slot2b_no_int:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,QSIM_REGS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
        


 |
 |      qsim_t0 - Secondary interrupt wrapper for either half of
 |                4T with a single full duplex port on port 0
 |
        .globl  _qsim_t0
_qsim_t0:
        movw    a2@(HD64570_ISR1_ISR0), d3 | Read HD64570 ISR1 register
	movl	a3@,sp@-		| Push IDB argument for C routines
        btst    #HD64570_S0_RINT,d3     | RX interrupt?
        jeq     qsim_t0_check_TX        | no, check for TX interrupt
        jsr     _hd64570_RX_interrupt   | call interrupt routine
qsim_t0_check_TX:
        btst    #HD64570_S0_TINT,d3     | TX interrupt?
        jeq     qsim_t0_check_DMIA      | No, check for DMIA
        jsr     _hd64570_TX_interrupt   | Call interrupt routine
qsim_t0_check_DMIA:
        btst    #HD64570_S0_RERR, d3    | Check for RX buffer wrap
        jeq     qsim_t0_check_err       | No, check for errors
        jsr     _hd64570_DMIA_int       | Jump to handler
qsim_t0_check_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1 | Read the ST1 register
        btst    #HD64570_UDRN, d1       | Check for underrun
        jeq     qsim_t0_exit            | No, clean up and return
        jsr     _hd64570_error_int      | Jump to error handler
qsim_t0_exit:
        addql   #4,sp                   | adjust stack
        rts                             | bye bye


 |
 |      qsim_0t - Secondary interrupt wrapper for either half of
 |                4T with a single full duplex port on port 1
 |
        .globl  _qsim_0t
_qsim_0t:
        movw    a2@(HD64570_ISR1_ISR0), d3 | Read HD64570 ISR1 register
	movl	a4@,sp@-		| Push IDB argument for C routines
        btst    #HD64570_S1_RINT,d3     | RX interrupt?
        jeq     qsim_0t_check_TX        | no, check for TX interrupt
        jsr     _hd64570_RX_interrupt   | call interrupt routine
qsim_0t_check_TX:
        btst    #HD64570_S1_TINT,d3     | TX interrupt?
        jeq     qsim_0t_check_DMIA      | No, check for DMIA
        jsr     _hd64570_TX_interrupt   | Call interrupt routine
qsim_0t_check_DMIA:
        btst    #HD64570_S1_RERR, d3    | Check for RX buffer wrap
        jeq     qsim_0t_check_err       | No, check for errors
        jsr     _hd64570_DMIA_int       | Jump to handler
qsim_0t_check_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1 | Read the ST1 register
        btst    #HD64570_UDRN, d1       | Check for underrun
        jeq     qsim_0t_exit            | No, clean up and return
        jsr     _hd64570_error_int      | Jump to error handler
qsim_0t_exit:
        addql   #4,sp                   | adjust stack
        rts                             | bye bye


 |
 |      qsim_thd0 - Secondary interrupt wrapper for either half of
 |                4T with a single half duplex port on port 0
 |
        .globl _qsim_thd0
_qsim_thd0:
        movw    a2@(HD64570_ISR1_ISR0), d3 | Read HD64570 ISR1 register
	movl	a3@,sp@-		| Push IDB argument for C routines
        btst    #HD64570_S0_RINT,d3     | RX interrupt?
        jeq     qsim_thd0_check_TX      | no, check for TX interrupt
        jsr     _hd64570_RX_HDX_interrupt| call interrupt routine
qsim_thd0_check_TX:
        btst    #HD64570_S0_TINT,d3     | TX interrupt?
        jeq     qsim_thd0_check_timer   | No, check for timer interrupt
        jsr     _hd64570_TX_HDX_interrupt| Call interrupt routine
qsim_thd0_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1      | Read ISR2
        andw    #0xF0, d1             | Check interrupt bits
        jeq     qsim_thd0_check_txint   | check tx
        movl    a4@, sp@-               | push IDB arg for port 1
        jsr     _hd64570_timer_interrupt| Call interrupt routine
        addql   #4, sp                  | pop port 1 IDB.
qsim_thd0_check_txint:
        btst    #HD64570_S0_TXINT, d3   | Check for interrupt bits in ISR0
        jeq     qsim_thd0_check_DMIA    | check for DMIA
        jsr     _hd64570_HDX_txint      | Call interrupt routine
qsim_thd0_check_DMIA:
        btst    #HD64570_S0_RERR, d3    | Check for RX buffer wrap
        jeq     qsim_thd0_check_err     | No, check for errors
        jsr     _hd64570_DMIA_int       | Jump to handler
qsim_thd0_check_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1     | Read the ST1 register
        btst    #HD64570_UDRN, d1       | Check for underrun
        jeq     qsim_thd0_exit          | No, clean up and return
        jsr     _hd64570_error_int      | Jump to error handler
qsim_thd0_exit:
        addql   #4,sp                   | adjust stack
        rts


 |
 |      qsim_0thd - Secondary interrupt wrapper for either half of
 |                4T with a single half duplex port on port 1
 |
        .globl _qsim_0thd
_qsim_0thd:
        movw    a2@(HD64570_ISR1_ISR0), d3 | Read HD64570 ISR1 register
	movl	a3@,sp@-		| Push port 0 IDB for C routine
	movl	a4@,sp@-		| Push port 1 IDB for C routine 
        btst    #HD64570_S1_RINT,d3     | RX interrupt?
        jeq     qsim_0thd_check_TX      | no, check for TX interrupt
        jsr     _hd64570_RX_HDX_interrupt| call interrupt routine
qsim_0thd_check_TX:
        btst    #HD64570_S1_TINT,d3     | TX interrupt?
        jeq     qsim_0thd_check_timer   | No, check for timer interrupt
        jsr     _hd64570_TX_HDX_interrupt| Call interrupt routine
qsim_0thd_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1      | Read ISR2
        andw    #0xF0, d1               | Check interrupt bits
        jeq     qsim_0thd_check_txint   | check tx
        jsr     _hd64570_timer_interrupt| Call interrupt routine
qsim_0thd_check_txint:
        btst    #HD64570_S1_TXINT, d3   | Check for interrupt bits in ISR0
        jeq     qsim_0thd_check_DMIA    | check for DMIA
        jsr     _hd64570_HDX_txint      | Call interrupt routine
qsim_0thd_check_DMIA:
        btst    #HD64570_S1_RERR, d3    | Check for RX buffer wrap
        jeq     qsim_0thd_check_err     | No, check for errors
        jsr     _hd64570_DMIA_int       | Jump to handler
qsim_0thd_check_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1     | Read the ST1 register
        btst    #HD64570_UDRN, d1       | Check for underrun
        jeq     qsim_0thd_exit          | No, clean up and return
        jsr     _hd64570_error_int      | Jump to error handler
qsim_0thd_exit:
        addql   #8,sp                   | adjust stack
        rts


 |
 |	qsim_tt - Seconadry interrupt wrapper for any half 
 |                of 4T with dual full duplex ports.
 |
        .globl _qsim_tt
_qsim_tt:
        movw    a2@(HD64570_ISR1_ISR0), d3 | Read HD64570 ISR1 register
        movl    a3@,sp@-                | Push IDB argument for C routines
        btst    #HD64570_S0_RINT,d3     | RX interrupt?
        jeq     qsim_tt_check_TX0       | no, check for TX interrupt
        jsr     _hd64570_RX_interrupt   | call interrupt routine
qsim_tt_check_TX0:
        btst    #HD64570_S0_TINT,d3     | TX interrupt?
        jeq     qsim_tt_check_DMIA0     | No, check for DMIA
        jsr     _hd64570_TX_interrupt   | Call interrupt routine
qsim_tt_check_DMIA0:
        btst    #HD64570_S0_RERR, d3    | Check for RX buffer wrap
        jeq     qsim_tt_check_c0_err    | No, check for errors
        jsr     _hd64570_DMIA_int       | Jump to handler
qsim_tt_check_c0_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1 | Read the ST1 register
        btst    #HD64570_UDRN, d1       | Check for underrun
        jeq     qsim_tt_check_c1_int    | No, check for C1 interrupt
        jsr     _hd64570_error_int      | Jump to error handler
qsim_tt_check_c1_int:
        addql   #4,sp                   | adjust stack
        movl    a4@,sp@-                | Push IDB argument for C routines
        btst    #HD64570_S1_RINT,d3     | RX interrupt?
        jeq     qsim_tt_check_TX1       | no, check for TX interrupt
        jsr     _hd64570_RX_interrupt   | call interrupt routine
qsim_tt_check_TX1:
        btst    #HD64570_S1_TINT,d3     | TX interrupt?
        jeq     qsim_tt_check_DMIA1     | No, check for DMIA
        jsr     _hd64570_TX_interrupt   | Call interrupt routine
qsim_tt_check_DMIA1:
        btst    #HD64570_S1_RERR, d3    | Check for RX buffer wrap
        jeq     qsim_tt_check_c1_err    | No, check for errors
        jsr     _hd64570_DMIA_int       | Jump to handler
qsim_tt_check_c1_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1 | Read the ST1 register
        btst    #HD64570_UDRN, d1       | Check for underrun
        jeq     qsim_tt_exit            | No, clean up and return
        jsr     _hd64570_error_int      | Jump to error handler
qsim_tt_exit:
        addql   #4,sp                   | adjust stack
        rts                             | bye bye


 |
 |	qsim_thdthd - Seconadry interrupt wrapper for any half 
 |                of 4T with dual half duplex ports.
 |
	.globl _qsim_thdthd
_qsim_thdthd:
        movw    a2@(HD64570_ISR1_ISR0), d3 | Read HD64570 ISR1 register
	movl	a3@,sp@-		| Push port 0 IDB arg for C routines
	btst	#HD64570_S0_RINT,d3	| RX interrupt?
	jeq	qsim_thdthd_check_TX0	| no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
qsim_thdthd_check_TX0:
	btst	#HD64570_S0_TINT,d3	| TX interrupt?
	jeq	qsim_thdthd_check_txint0| No, check for tx int
	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
qsim_thdthd_check_txint0:
	btst	#HD64570_S0_TXINT, d3	| Check for interrupt bits in ISR0
	jeq	qsim_thdthd_check_DMIA0	| No, check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
qsim_thdthd_check_DMIA0:
	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
	jeq	qsim_thdthd_check_c0_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
qsim_thdthd_check_c0_err:
	movw	a2@(HD64570_C0_ST1_ST0), d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	qsim_thdthd_check_c1_int| No, check for C1 interrupt
	jsr	_hd64570_error_int	| Jump to error handler
qsim_thdthd_check_c1_int:
	movl	a4@,sp@-    		| Push port 1 IDB arg for C routines
	btst	#HD64570_S1_RINT,d3	| RX interrupt?
	jeq	qsim_thdthd_check_TX1	| no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
qsim_thdthd_check_TX1:
	btst	#HD64570_S1_TINT,d3	| TX interrupt?
	jeq	qsim_thdthd_check_txint1| No, check for tx int
	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
qsim_thdthd_check_txint1:
	btst	#HD64570_S1_TXINT, d3	| Check for interrupt bits in IRS0 
	jeq	qsim_thdthd_check_DMIA1	| No, check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
qsim_thdthd_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	| Check for RX buffer wrap
	jeq	qsim_thdthd_check_c1_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
qsim_thdthd_check_c1_err:
	movw	a2@(HD64570_C1_ST1_ST0), d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	qsim_thdthd_check_timer| No, check for timer interrupts
	jsr	_hd64570_error_int	| Jump to error handler
qsim_thdthd_check_timer:
	movw	a2@(HD64570_NONE_ISR2), d1	| Read ISR2
	andw	#0xF0, d1		| Check interrupt bits
	jeq	qsim_thdthd_exit	| No, clean up and return
	jsr	_hd64570_timer_interrupt| Call interrupt routine
qsim_thdthd_exit:
	addql	#8,sp			| adjust stack
	rts	


 |
 |	qsim_thdt - Seconadry interrupt wrapper for any half 
 |      of 4T with dual ports, half-duplex on first and full duplex 
 |      second port.
 |
	.globl _qsim_thdt
_qsim_thdt:
        movw    a2@(HD64570_ISR1_ISR0), d3 | Read HD64570 ISR1 register
	movl	a3@,sp@-		| Push port 0 IDB arg for C routines
	btst	#HD64570_S0_RINT,d3	| RX interrupt?
	jeq	qsim_thdt_check_TX0	| no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
qsim_thdt_check_TX0:
	btst	#HD64570_S0_TINT,d3	| TX interrupt?
	jeq	qsim_thdt_check_txint0| No, check for tx
	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
qsim_thdt_check_txint0:
	btst	#HD64570_S0_TXINT, d3	| Check for interrupt bits in ISR0
	jeq 	qsim_thdt_check_DMIA0   | check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
qsim_thdt_check_DMIA0:
	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
	jeq	qsim_thdt_check_c0_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
qsim_thdt_check_c0_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1     | Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	qsim_thdt_check_c1_int| No, check for C1 interrupt
	jsr	_hd64570_error_int	| Jump to error handler
qsim_thdt_check_c1_int:
	movl	a4@,sp@-		| Push Port 1 IDB arg for C routines
	btst	#HD64570_S1_RINT,d3	| RX interrupt?
	jeq	qsim_thdt_check_TX1	| no, check for TX interrupt
	jsr	_hd64570_RX_interrupt	| call interrupt routine
qsim_thdt_check_TX1:
	btst	#HD64570_S1_TINT,d3	| TX interrupt?
	jeq	qsim_thdt_check_DMIA1| No, check for DMIA
	jsr	_hd64570_TX_interrupt	| Call interrupt routine
qsim_thdt_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	| Check for RX buffer wrap
	jeq	qsim_thdt_check_c1_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
qsim_thdt_check_c1_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1     | Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	qsim_thdt_check_timer| No, check for timer interrupts
	jsr	_hd64570_error_int	| Jump to error handler
qsim_thdt_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1      | Read ISR2
	andw	#0xF0, d1		| Check interrupt bits
	jeq	qsim_thdt_exit	        | No, clean up and return
	jsr	_hd64570_timer_interrupt| Call interrupt routine
qsim_thdt_exit:
	addql	#8,sp			| adjust stack
	rts	


 |
 |	qsim_tthd - Seconadry interrupt wrapper for any half 
 |      of 4T with dual ports, full duplex on first and half-duplex 
 |      second port.
 |
 	.globl _qsim_tthd
 _qsim_tthd:
        movw    a2@(HD64570_ISR1_ISR0), d3 | Read HD64570 ISR1 register
	movl	a3@,sp@-		| Push port 0 IDB arg for C routines
 	btst	#HD64570_S0_RINT,d3	| RX interrupt?
 	jeq	qsim_tthd_check_TX0	| no, check for TX interrupt
 	jsr	_hd64570_RX_interrupt	| call interrupt routine
 qsim_tthd_check_TX0:
 	btst	#HD64570_S0_TINT,d3	| TX interrupt?
 	jeq	qsim_tthd_check_DMIA0   | No, check for DMIA
 	jsr	_hd64570_TX_interrupt	| Call interrupt routine
 qsim_tthd_check_DMIA0:
 	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
 	jeq	qsim_tthd_check_c0_err	| No, check for errors
 	jsr	_hd64570_DMIA_int	| Jump to handler
 qsim_tthd_check_c0_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1     | Read the ST1 register
 	btst	#HD64570_UDRN, d1	| Check for underrun
 	jeq	qsim_tthd_check_c1_int  | No, check for C1 interrupt
 	jsr	_hd64570_error_int	| Jump to error handler
 qsim_tthd_check_c1_int:
 	movl	a4@,sp@-		| Push Port 1 IDB arg for C routines
 	btst	#HD64570_S1_RINT,d3	| RX interrupt?
 	jeq	qsim_tthd_check_TX1	| no, check for TX interrupt
 	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
 qsim_tthd_check_TX1:
 	btst	#HD64570_S1_TINT,d3	| TX interrupt?
 	jeq	qsim_tthd_check_txint1  | No, check for tx
 	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
 qsim_tthd_check_txint1:
 	btst	#HD64570_S1_TXINT, d3	| Check for interrupt bits in ISR0
 	jeq 	qsim_tthd_check_DMIA1   | check for DMIA
 	jsr	_hd64570_HDX_txint	| Call interrupt routine
 qsim_tthd_check_DMIA1:
 	btst	#HD64570_S1_RERR, d3	| Check for RX buffer wrap
 	jeq	qsim_tthd_check_c1_err	| No, check for errors
 	jsr	_hd64570_DMIA_int	| Jump to handler
 qsim_tthd_check_c1_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1     | Read the ST1 register
 	btst	#HD64570_UDRN, d1	| Check for underrun
 	jeq	qsim_tthd_check_timer   | No, check for timer interrupts
 	jsr	_hd64570_error_int	| Jump to error handler
 qsim_tthd_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1      | Read ISR2
 	andw	#0xF0, d1		| Check interrupt bits
 	jeq	qsim_tthd_exit	        | No, clean up and return
 	jsr	_hd64570_timer_interrupt| Call interrupt routine
 qsim_tthd_exit:
 	addql	#8,sp			| adjust stack
 	rts	


 |
 |      qsim_thd0_bsc - Secondary interrupt wrapper for either half of
 |                4T with a single half duplex BSC port on port 0
 |
        .globl _qsim_thd0_bsc
_qsim_thd0_bsc:
        movw    a2@(HD64570_ISR1_ISR0), d3   | Read HD64570 ISR1 register
	movl	a3@,sp@-		     | Push port 0 IDB for C routine
        btst    #HD64570_S0_RXRDY,d3         | RX interrupt?
        jeq     qsim_thd0_bsc_check_TX       | no, check for TX interrupt
        jsr     _hd64570_RX_HDX_BSC_interrupt| call interrupt routine
qsim_thd0_bsc_check_TX:
        btst    #HD64570_S0_TINT,d3          | DMA TX interrupt?
        jeq     qsim_thd0_bsc_check_timer    | No, check for timer interrupt
        jsr     _hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_thd0_bsc_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1   | Read ISR2
        andw    #0xF0, d1                    | Check interrupt bits
        jeq     qsim_thd0_bsc_check_txint    | check tx
        movl    a4@, sp@-                    | push IDB arg for port 1
        jsr     _hd64570_timer_interrupt     | Call interrupt routine
        addql   #4, sp                       | pop port 1 IDB.
qsim_thd0_bsc_check_txint:
        btst    #HD64570_S0_TXINT, d3        | Check interrupt bits in ISR0
        jeq     qsim_thd0_bsc_check_err      | No, check for errors
        jsr     _hd64570_HDX_txint           | Call interrupt routine
qsim_thd0_bsc_check_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1  | Read the ST1 register
        btst    #HD64570_UDRN, d1            | Check for underrun
        jeq     qsim_thd0_bsc_exit           | We are done
        jsr     _hd64570_error_int           | Jump to error handler
qsim_thd0_bsc_exit:
        addql   #4,sp                        | adjust stack
        rts


 |
 |      qsim_0thd_bsc - Secondary interrupt wrapper for either half of
 |                4T with a single half duplex BSC port on port 1
 |
        .globl _qsim_0thd_bsc
_qsim_0thd_bsc:
        movw    a2@(HD64570_ISR1_ISR0), d3   | Read HD64570 ISR1 register
	movl	a3@,sp@-		     | Push port 0 IDB for C routine
	movl	a4@,sp@-		     | Push port 1 IDB for C routine 
        btst    #HD64570_S1_RXRDY,d3         | RX interrupt?
        jeq     qsim_0thd_bsc_check_TX       | no, check for TX interrupt
        jsr     _hd64570_RX_HDX_BSC_interrupt| call interrupt routine
qsim_0thd_bsc_check_TX:
        btst    #HD64570_S1_TINT,d3          | DMA TX interrupt?
        jeq     qsim_0thd_bsc_check_timer    | No, check for timer interrupt
        jsr     _hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_0thd_bsc_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1   | Read ISR2
        andw    #0xF0, d1                    | Check interrupt bits
        jeq     qsim_0thd_bsc_check_txint    | check tx 
        jsr     _hd64570_timer_interrupt     | Call interrupt routine
qsim_0thd_bsc_check_txint:
        btst    #HD64570_S1_TXINT, d3        | Check interrupt bits in ISR0
        jeq     qsim_0thd_bsc_check_err      | No, check for errors
        jsr     _hd64570_HDX_txint           | Call interrupt routine
qsim_0thd_bsc_check_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1  | Read the ST1 register
        btst    #HD64570_UDRN, d1            | Check for underrun
        jeq     qsim_0thd_bsc_exit           | We are done
        jsr     _hd64570_error_int           | Jump to error handler
qsim_0thd_bsc_exit:
        addql   #8,sp                        | adjust stack
        rts


 |
 |	qsim_thd_bsc_thd_bsc - Seconadry interrupt wrapper for any half 
 |                of 4T with dual half duplex BSC ports.
 |
	.globl _qsim_thd_bsc_thd_bsc
_qsim_thd_bsc_thd_bsc:
        movw    a2@(HD64570_ISR1_ISR0), d3   | Read HD64570 ISR1 register
	movl	a3@,sp@-		     | Push port 0 IDB arg for C
	btst	#HD64570_S0_RXRDY,d3	     | RX interrupt?
	jeq	qsim_thd_bsc_thd_bsc_check_TX0| no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt| call interrupt routine
qsim_thd_bsc_thd_bsc_check_TX0:
	btst	#HD64570_S0_TINT,d3	     | TX interrupt?
	jeq	qsim_thd_bsc_thd_bsc_check_txint0| No, check for tx int
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_thd_bsc_thd_bsc_check_txint0:
	btst	#HD64570_S0_TXINT, d3	     | Check interrupt bits in ISR0
	jeq	qsim_thd_bsc_thd_bsc_check_c0_err| No, check for errors
	jsr	_hd64570_HDX_txint	     | Call interrupt routine
qsim_thd_bsc_thd_bsc_check_c0_err:
	movw	a2@(HD64570_C0_ST1_ST0), d1  | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_thd_bsc_thd_bsc_check_c1_int | No, check for C1 interrupt
	jsr	_hd64570_error_int	     | Jump to error handler
qsim_thd_bsc_thd_bsc_check_c1_int:
	movl	a4@,sp@-    		     | Push port 1 IDB for C routines
	btst	#HD64570_S1_RXRDY,d3	     | RX interrupt?
	jeq	qsim_thd_bsc_thd_bsc_check_TX1 | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt| call interrupt routine
qsim_thd_bsc_thd_bsc_check_TX1:
	btst	#HD64570_S1_TINT,d3	     | TX interrupt?
	jeq	qsim_thd_bsc_thd_bsc_check_txint1| No, check for tx int
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_thd_bsc_thd_bsc_check_txint1:
	btst	#HD64570_S1_TXINT, d3	     | Check interrupt bits in IRS0 
	jeq	qsim_thd_bsc_thd_bsc_check_c1_err| No, check for errors
	jsr	_hd64570_HDX_txint	     | Call interrupt routine
qsim_thd_bsc_thd_bsc_check_c1_err:
	movw	a2@(HD64570_C1_ST1_ST0), d1  | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_thd_bsc_thd_bsc_check_timer | No,check for timer interrupts
	jsr	_hd64570_error_int	     | Jump to error handler
qsim_thd_bsc_thd_bsc_check_timer:
	movw	a2@(HD64570_NONE_ISR2), d1   | Read ISR2
	andw	#0xF0, d1		     | Check interrupt bits
	jeq	qsim_thd_bsc_thd_bsc_exit    | No, clean up and return
	jsr	_hd64570_timer_interrupt     | Call interrupt routine
qsim_thd_bsc_thd_bsc_exit:
	addql	#8,sp			     | adjust stack
	rts	


 |
 |	qsim_thd0_bsc_t - Seconadry interrupt wrapper for any half 
 |      of 4T with dual ports, half-duplex BSC on first and full duplex 
 |      SDLC second port.
 |
	.globl _qsim_thd0_bsc_t
_qsim_thd0_bsc_t:
        movw    a2@(HD64570_ISR1_ISR0), d3   | Read HD64570 ISR1 register
	movl	a3@,sp@-		     | Push port 0 IDB for C routines
	btst	#HD64570_S0_RXRDY,d3	     | RX interrupt?
	jeq	qsim_thd0_bsc_t_check_TX0    | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt| call interrupt routine
qsim_thd0_bsc_t_check_TX0:
	btst	#HD64570_S0_TINT,d3	     | TX interrupt?
	jeq	qsim_thd0_bsc_t_check_txint0 | No, check for tx
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_thd0_bsc_t_check_txint0:
	btst	#HD64570_S0_TXINT, d3	     | Check interrupt bits in ISR0
	jeq 	qsim_thd0_bsc_t_check_c0_err | No, check for errors
	jsr	_hd64570_HDX_txint	     | Call interrupt routine
qsim_thd0_bsc_t_check_c0_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1  | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_thd0_bsc_t_check_c1_int | No, check for C1 interrupt
	jsr	_hd64570_error_int	     | Jump to error handler
qsim_thd0_bsc_t_check_c1_int:
	movl	a4@,sp@-		     | Push Port 1 IDB for C routines
	btst	#HD64570_S1_RINT,d3	     | RX interrupt?
	jeq	qsim_thd0_bsc_t_check_TX1    | no, check for TX interrupt
	jsr	_hd64570_RX_interrupt	     | call interrupt routine
qsim_thd0_bsc_t_check_TX1:
	btst	#HD64570_S1_TINT,d3	     | TX interrupt?
	jeq	qsim_thd0_bsc_t_check_DMIA1  | No, check for DMIA
	jsr	_hd64570_TX_interrupt	     | Call interrupt routine
qsim_thd0_bsc_t_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	     | Check for RX buffer wrap
	jeq	qsim_thd0_bsc_t_check_c1_err | No, check for errors
	jsr	_hd64570_DMIA_int	     | Jump to handler
qsim_thd0_bsc_t_check_c1_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1  | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_thd0_bsc_t_check_timer  | No, check for timer interrupts
	jsr	_hd64570_error_int	     | Jump to error handler
qsim_thd0_bsc_t_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1   | Read ISR2
	andw	#0xF0, d1		     | Check interrupt bits
	jeq	qsim_thd0_bsc_t_exit	     | No, clean up and return
	jsr	_hd64570_timer_interrupt     | Call interrupt routine
qsim_thd0_bsc_t_exit:
	addql	#8,sp			     | adjust stack
	rts	


 |
 |	qsim_t_0thd_bsc - Seconadry interrupt wrapper for any half 
 |      of 4T with dual ports, full duplex SDLC on first and half-duplex 
 |      BSC second port.
 |
 	.globl _qsim_t_0thd_bsc
 _qsim_t_0thd_bsc:
        movw    a2@(HD64570_ISR1_ISR0), d3   | Read HD64570 ISR1 register
	movl	a3@,sp@-		     | Push port 0 IDB for C routines
 	btst	#HD64570_S0_RINT,d3	     | RX interrupt?
 	jeq	qsim_t_0thd_bsc_check_TX0    | no, check for TX interrupt
 	jsr	_hd64570_RX_interrupt	     | call interrupt routine
 qsim_t_0thd_bsc_check_TX0:
 	btst	#HD64570_S0_TINT,d3	     | TX interrupt?
 	jeq	qsim_t_0thd_bsc_check_DMIA0  | No, check for DMIA
 	jsr	_hd64570_TX_interrupt	     | Call interrupt routine
 qsim_t_0thd_bsc_check_DMIA0:
 	btst	#HD64570_S0_RERR, d3	     | Check for RX buffer wrap
 	jeq	qsim_t_0thd_bsc_check_c0_err | No, check for errors
 	jsr	_hd64570_DMIA_int	     | Jump to handler
 qsim_t_0thd_bsc_check_c0_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1  | Read the ST1 register
 	btst	#HD64570_UDRN, d1	     | Check for underrun
 	jeq	qsim_t_0thd_bsc_check_c1_int | No, check for C1 interrupt
 	jsr	_hd64570_error_int	     | Jump to error handler
 qsim_t_0thd_bsc_check_c1_int:
 	movl	a4@,sp@-		     | Push Port 1 IDB for C routines
 	btst	#HD64570_S1_RXRDY,d3	     | RX interrupt?
 	jeq	qsim_t_0thd_bsc_check_TX1    | no, check for TX interrupt
 	jsr	_hd64570_RX_HDX_BSC_interrupt| call interrupt routine
 qsim_t_0thd_bsc_check_TX1:
 	btst	#HD64570_S1_TINT,d3	     | TX interrupt?
 	jeq	qsim_t_0thd_bsc_check_txint1 | No, check for tx
 	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
 qsim_t_0thd_bsc_check_txint1:
 	btst	#HD64570_S1_TXINT, d3	     | Check interrupt bits in ISR0
 	jeq 	qsim_t_0thd_bsc_check_c1_err | No, check for errors
 	jsr	_hd64570_HDX_txint	     | Call interrupt routine
 qsim_t_0thd_bsc_check_c1_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1  | Read the ST1 register
 	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_t_0thd_bsc_check_timer  | No, check for timer interrupts
 	jsr	_hd64570_error_int	     | Jump to error handler
 qsim_t_0thd_bsc_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1   | Read ISR2
 	andw	#0xF0, d1		     | Check interrupt bits
 	jeq	qsim_t_0thd_bsc_exit	     | No, clean up and return
 	jsr	_hd64570_timer_interrupt     | Call interrupt routine
 qsim_t_0thd_bsc_exit:
 	addql	#8,sp			     | adjust stack
 	rts	


 |
 |	qsim_thd0_bsc_0thd - Seconadry interrupt wrapper for any half 
 |      of 4T with dual ports, half-duplex BSC on first and half-duplex 
 |      SDLC second port.
 |
	.globl _qsim_thd0_bsc_0thd
_qsim_thd0_bsc_0thd:
        movw    a2@(HD64570_ISR1_ISR0), d3   | Read HD64570 ISR1 register
	movl	a3@,sp@-		     | Push port 0 IDB for C routines
	btst	#HD64570_S0_RXRDY,d3	     | RX interrupt?
	jeq	qsim_thd0_bsc_0thd_check_TX0 | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt| call interrupt routine
qsim_thd0_bsc_0thd_check_TX0:
	btst	#HD64570_S0_TINT,d3	     | TX interrupt?
	jeq	qsim_thd0_bsc_0thd_check_txint0 | No, check for tx
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_thd0_bsc_0thd_check_txint0:
	btst	#HD64570_S0_TXINT, d3	     | Check interrupt bits in ISR0
	jeq 	qsim_thd0_bsc_0thd_check_c0_err | No, check for errors
	jsr	_hd64570_HDX_txint	     | Call interrupt routine
qsim_thd0_bsc_0thd_check_c0_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1  | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_thd0_bsc_0thd_check_c1_int | No, check for C1 interrupt
	jsr	_hd64570_error_int	     | Jump to error handler
qsim_thd0_bsc_0thd_check_c1_int:
	movl	a4@,sp@-		     | Push Port 1 IDB for C routines
	btst	#HD64570_S1_RINT,d3	     | RX interrupt?
	jeq	qsim_thd0_bsc_0thd_check_TX1 | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt    | call interrupt routine
qsim_thd0_bsc_0thd_check_TX1:
	btst	#HD64570_S1_TINT,d3	     | TX interrupt?
	jeq	qsim_thd0_bsc_0thd_check_txint1| No, check for DMIA
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_thd0_bsc_0thd_check_txint1:
	btst	#HD64570_S1_TXINT, d3	| Check for interrupt bits in IRS0 
	jeq	qsim_thd0_bsc_0thd_check_DMIA1	| No, check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
qsim_thd0_bsc_0thd_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	     | Check for RX buffer wrap
	jeq	qsim_thd0_bsc_0thd_check_c1_err	| No, check for errors
	jsr	_hd64570_DMIA_int	     | Jump to handler
qsim_thd0_bsc_0thd_check_c1_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1  | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_thd0_bsc_0thd_check_timer| No, check for timer interrupts
	jsr	_hd64570_error_int	     | Jump to error handler
qsim_thd0_bsc_0thd_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1   | Read ISR2
	andw	#0xF0, d1		     | Check interrupt bits
	jeq	qsim_thd0_bsc_0thd_exit	     | No, clean up and return
	jsr	_hd64570_timer_interrupt     | Call interrupt routine
qsim_thd0_bsc_0thd_exit:
	addql	#8,sp			     | adjust stack
	rts	


 |
 |	qsim_thd0_0thd_bsc - Seconadry interrupt wrapper for any half 
 |      of 4T with dual ports, half-duplex SDLC on first and half-duplex 
 |      BSC second port.
 |
	.globl _qsim_thd0_0thd_bsc
_qsim_thd0_0thd_bsc:
        movw    a2@(HD64570_ISR1_ISR0), d3   | Read HD64570 ISR1 register
	movl	a3@,sp@-		     | Push port 0 IDB for C routines
	btst	#HD64570_S0_RINT,d3	     | RX interrupt?
	jeq	qsim_thd0_0thd_bsc_check_TX0 | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt    | call interrupt routine
qsim_thd0_0thd_bsc_check_TX0:
	btst	#HD64570_S0_TINT,d3	     | TX interrupt?
	jeq	qsim_thd0_0thd_bsc_check_txint0| No, check for tx
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_thd0_0thd_bsc_check_txint0:
	btst	#HD64570_S0_TXINT, d3	     | Check interrupt bits in ISR0
	jeq 	qsim_thd0_0thd_bsc_check_DMIA0 | check for DMIA
	jsr	_hd64570_HDX_txint	     | Call interrupt routine
qsim_thd0_0thd_bsc_check_DMIA0:
	btst	#HD64570_S0_RERR, d3	     | Check for RX buffer wrap
	jeq	qsim_thd0_0thd_bsc_check_c0_err	| No, check for errors
	jsr	_hd64570_DMIA_int	     | Jump to handler
qsim_thd0_0thd_bsc_check_c0_err:
        movw    a2@(HD64570_C0_ST1_ST0), d1  | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_thd0_0thd_bsc_check_c1_int| No, check for C1 interrupt
	jsr	_hd64570_error_int	     | Jump to error handler
qsim_thd0_0thd_bsc_check_c1_int:
	movl	a4@,sp@-		     | Push Port 1 IDB for C routines
	btst	#HD64570_S1_RXRDY,d3	     | RX interrupt?
	jeq	qsim_thd0_0thd_bsc_check_TX1 | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt | call interrupt routine
qsim_thd0_0thd_bsc_check_TX1:
	btst	#HD64570_S1_TINT,d3	     | TX interrupt?
	jeq	qsim_thd0_0thd_bsc_check_txint1| No, check for DMIA
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
qsim_thd0_0thd_bsc_check_txint1:
 	btst	#HD64570_S1_TXINT, d3	     | Check interrupt bits in ISR0
 	jeq 	qsim_thd0_0thd_bsc_check_c1_err  | No, check for errors
 	jsr	_hd64570_HDX_txint	     | Call interrupt routine
qsim_thd0_0thd_bsc_check_c1_err:
        movw    a2@(HD64570_C1_ST1_ST0), d1  | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	qsim_thd0_0thd_bsc_check_timer | No, check for timer interrupts
	jsr	_hd64570_error_int	     | Jump to error handler
qsim_thd0_0thd_bsc_check_timer:
        movw    a2@(HD64570_NONE_ISR2), d1   | Read ISR2
	andw	#0xF0, d1		     | Check interrupt bits
	jeq	qsim_thd0_0thd_bsc_exit	     | No, clean up and return
	jsr	_hd64570_timer_interrupt     | Call interrupt routine
qsim_thd0_0thd_bsc_exit:
	addql	#8,sp			     | adjust stack
	rts	





