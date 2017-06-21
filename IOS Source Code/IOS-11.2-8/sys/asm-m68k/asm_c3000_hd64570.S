/* $Id: asm_c3000_hd64570.S,v 3.2 1995/11/17 08:39:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c3000_hd64570.S,v $
 *------------------------------------------------------------------
 * Assembly wrapper support for C3000 hd64570
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c3000_hd64570.S,v $
| Revision 3.2  1995/11/17  08:39:31  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:21  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:31  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c3000.h"

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

#define HD64570_UDRN 7
#define HD64570_OVRN 3

#define HD64570_ISR0 ADRSPC_CRISTAL_NET_C + 17
#define HD64570_ISR1 ADRSPC_CRISTAL_NET_C + 16
#define HD64570_ISR1_ISR0 ADRSPC_CRISTAL_NET_C + 16
#define HD64570_ISR2 ADRSPC_CRISTAL_NET_C + 19
#define HD64570_C0_ST1 ADRSPC_CRISTAL_NET_C + 34
#define HD64570_C1_ST1 ADRSPC_CRISTAL_NET_C + 66
#define HD64570_C0_ST2 ADRSPC_CRISTAL_NET_C + 37
#define HD64570_C1_ST2 ADRSPC_CRISTAL_NET_C + 69


 |
 |	level4_cristal_tnil - Level 4 interrupt wrapper for cristal single
 |	serial on port 0
 |
 | void level4_cristal_tnil (void)
 |
	.globl _level4_cristal_tnil
_level4_cristal_tnil:
	movw	HD64570_ISR1_ISR0,d3	| Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S0_RINT,d3	| RX interrupt?
	jeq	cristal_tnil_check_TX	| no, check for TX interrupt
	jsr	_hd64570_RX_interrupt	| call interrupt routine
cristal_tnil_check_TX:
	btst	#HD64570_S0_TINT,d3	| TX interrupt?
	jeq	cristal_tnil_check_DMIA	| No, check for DMIA
	jsr	_hd64570_TX_interrupt	| Call interrupt routine
cristal_tnil_check_DMIA:
	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_tnil_check_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_tnil_check_err:
	movb	HD64570_C0_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_tnil_exit	| No, clean up and return
	jsr	_hd64570_error_int	| Jump to error handler
cristal_tnil_exit:
	addql	#4,sp			| adjust stack
	rts				| bye bye


 |
 |      level4_cristal_nilt - Level 4 wrapper for cristal dual serial
 |                with a single serial on port 1
 |
        .globl _level4_cristal_nilt
_level4_cristal_nilt:
        movw    HD64570_ISR1_ISR0, d3   | Read HD64570 ISR1_ISR0 register
	movl	a2@(8),sp@-		| Push port 1 IDB for C routine 
        btst    #HD64570_S1_RINT,d3     | RX interrupt?
        jeq     cristal_nilt_check_TX   | no, check for TX interrupt
        jsr     _hd64570_RX_interrupt   | call interrupt routine
cristal_nilt_check_TX:
        btst    #HD64570_S1_TINT,d3     | TX interrupt?
        jeq     cristal_nilt_check_DMIA | No, check for DMIA
        jsr     _hd64570_TX_interrupt   | Call interrupt routine
cristal_nilt_check_DMIA:
        btst    #HD64570_S1_RERR, d3    | Check for RX buffer wrap
        jeq     cristal_nilt_check_err  | No, check for errors
        jsr     _hd64570_DMIA_int       | Jump to handler
cristal_nilt_check_err:
        movb    HD64570_C1_ST1, d1      | Read the ST1 register
        btst    #HD64570_UDRN, d1       | Check for underrun
        jeq     cristal_nilt_exit       | No, clean up and return
        jsr     _hd64570_error_int      | Jump to error handler
cristal_nilt_exit:
        addql   #4,sp                   | adjust stack
        rts

 |
 |	level4_cristal_thdnil - Level 4 interrupt wrapper for cristal single
 |	serial with half-duplex
 |
 | void level4_cristal_thdnil (void)
 |
	.globl _level4_cristal_thdnil
_level4_cristal_thdnil:
	movw	HD64570_ISR1_ISR0,d3	  | Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S0_RINT,d3	| RX interrupt?
	jeq	cristal_thdnil_check_TX	  | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
cristal_thdnil_check_TX:
	btst	#HD64570_S0_TINT,d3	| TX interrupt?
	jeq	cristal_thdnil_check_timer| No, check for timer interrupt
	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
cristal_thdnil_check_timer:
	movb	HD64570_ISR2, d1	| Read ISR2
	andb	#0xF0, d1		| Check interrupt bits
	jeq	cristal_thdnil_check_txint| check tx
	jsr	_hd64570_timer_interrupt| Call interrupt routine
cristal_thdnil_check_txint:
	movb	HD64570_ISR0, d1	| Read ISR0
	btst	#HD64570_S0_TXINT, d1	| Check for interrupt bits
	jeq 	cristal_thdnil_check_DMIA | check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
cristal_thdnil_check_DMIA:
	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_thdnil_check_err  | No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_thdnil_check_err:
	movb	HD64570_C0_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_thdnil_exit	  | No, clean up and return
	jsr	_hd64570_error_int	| Jump to error handler
cristal_thdnil_exit:
	addql	#4,sp			| adjust stack
	rts	


 |
 |      level4_cristal_nilthd - Level 4 wrapper for cristal dual serial
 |                with a single half duplex port on port 1
 |
        .globl _level4_cristal_nilthd
_level4_cristal_nilthd:
        movw    HD64570_ISR1_ISR0, d3        | Read HD64570 ISR1_ISR0 register
	movl	a4@(8),sp@-		     | Push port 1 IDB for C routine 
        btst    #HD64570_S1_RINT,d3          | RX interrupt?
        jeq     cristal_nilthd_check_TX      | no, check for TX interrupt
        jsr     _hd64570_RX_HDX_interrupt    | call interrupt routine
cristal_nilthd_check_TX:
        btst    #HD64570_S1_TINT,d3          | TX interrupt?
        jeq     cristal_nilthd_check_timer   | No, check for timer interrupt
        jsr     _hd64570_TX_HDX_interrupt    | Call interrupt routine
cristal_nilthd_check_timer:
        movb    HD64570_ISR2, d1             | Read ISR2
        andw    #0xF0, d1                    | Check interrupt bits
        jeq     cristal_nilthd_check_txint   | check tx
        jsr     _hd64570_timer_interrupt     | Call interrupt routine
cristal_nilthd_check_txint:
        btst    #HD64570_S1_TXINT, d3        | Check interrupt bits in ISR0
        jeq     cristal_nilthd_check_DMIA    | check for DMIA
        jsr     _hd64570_HDX_txint           | Call interrupt routine
cristal_nilthd_check_DMIA:
        btst    #HD64570_S1_RERR, d3         | Check for RX buffer wrap
        jeq     cristal_nilthd_check_err     | No, check for errors
        jsr     _hd64570_DMIA_int            | Jump to handler
cristal_nilthd_check_err:
        movb    HD64570_C1_ST1, d1           | Read the ST1 register
        btst    #HD64570_UDRN, d1            | Check for underrun
        jeq     cristal_nilthd_exit          | No, clean up and return
        jsr     _hd64570_error_int           | Jump to error handler
cristal_nilthd_exit:
        addql   #4,sp                        | adjust stack
        rts

 |
 |	level4_cristal_tt - Level 4 interrupt wrapper for cristal dual
 |	serial
 |
 | void level4_cristal_tt (void)
 |
	.globl _level4_cristal_tt
_level4_cristal_tt:
	movw	HD64570_ISR1_ISR0,d3	| Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S0_RINT,d3	| RX interrupt?
	jeq	cristal_tt_check_TX0	| no, check for TX interrupt
	jsr	_hd64570_RX_interrupt	| call interrupt routine
cristal_tt_check_TX0:
	btst	#HD64570_S0_TINT,d3	| TX interrupt?
	jeq	cristal_tt_check_DMIA0	| No, check for DMIA
	jsr	_hd64570_TX_interrupt	| Call interrupt routine
cristal_tt_check_DMIA0:
	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_tt_check_c0_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_tt_check_c0_err:
	movb	HD64570_C0_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_tt_check_c1_int	| No, check for C1 interrupt
	jsr	_hd64570_error_int	| Jump to error handler
cristal_tt_check_c1_int:
	addql	#4,sp			| adjust stack
	movl	a2@(8),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S1_RINT,d3	| RX interrupt?
	jeq	cristal_tt_check_TX1	| no, check for TX interrupt
	jsr	_hd64570_RX_interrupt	| call interrupt routine
cristal_tt_check_TX1:
	btst	#HD64570_S1_TINT,d3	| TX interrupt?
	jeq	cristal_tt_check_DMIA1	| No, check for DMIA
	jsr	_hd64570_TX_interrupt	| Call interrupt routine
cristal_tt_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_tt_check_c1_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_tt_check_c1_err:
	movb	HD64570_C1_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_tt_exit		| No, clean up and return
	jsr	_hd64570_error_int	| Jump to error handler
cristal_tt_exit:
	addql	#4,sp			| adjust stack
	rts				| bye bye


 |
 |	level4_cristal_thdthd - Level 4 interrupt wrapper for cristal dual
 |	serial with half-duplex on both serial ports
 |
 | void level4_cristal_thdthd (void)
 |
	.globl _level4_cristal_thdthd
_level4_cristal_thdthd:
	movw	HD64570_ISR1_ISR0,d3	| Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S0_RINT,d3	| RX interrupt?
	jeq	cristal_thdthd_check_TX0	| no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
cristal_thdthd_check_TX0:
	btst	#HD64570_S0_TINT,d3	| TX interrupt?
	jeq	cristal_thdthd_check_txint0| No, check for tx int
	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
cristal_thdthd_check_txint0:
	movb	HD64570_ISR0, d1	| Read ISR0
	btst	#HD64570_S0_TXINT, d1	| Check for interrupt bits
	jeq	cristal_thdthd_check_DMIA0	| No, check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
cristal_thdthd_check_DMIA0:
	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_thdthd_check_c0_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_thdthd_check_c0_err:
	movb	HD64570_C0_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_thdthd_check_c1_int| No, check for C1 interrupt
	jsr	_hd64570_error_int	| Jump to error handler
cristal_thdthd_check_c1_int:
	addql	#4,sp			| adjust stack
	movl	a2@(8),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S1_RINT,d3	| RX interrupt?
	jeq	cristal_thdthd_check_TX1	| no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
cristal_thdthd_check_TX1:
	btst	#HD64570_S1_TINT,d3	| TX interrupt?
	jeq	cristal_thdthd_check_txint1| No, check for tx int
	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
cristal_thdthd_check_txint1:
	movb	HD64570_ISR0, d1	| Read ISR0
	btst	#HD64570_S1_TXINT, d1	| Check for interrupt bits
	jeq	cristal_thdthd_check_DMIA1	| No, check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
cristal_thdthd_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_thdthd_check_c1_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_thdthd_check_c1_err:
	movb	HD64570_C1_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_thdthd_check_timer| No, check for timer interrupts
	jsr	_hd64570_error_int	| Jump to error handler
cristal_thdthd_check_timer:
	movb	HD64570_ISR2, d1	| Read ISR2
	andb	#0xF0, d1		| Check interrupt bits
	jeq	cristal_thdthd_exit	| No, clean up and return
	jsr	_hd64570_timer_interrupt| Call interrupt routine
cristal_thdthd_exit:
	addql	#4,sp			| adjust stack
	rts	


 |
 |	level4_cristal_thdt - Level 4 interrupt wrapper for cristal dual
 |	serial with half-duplex only on the first serial port
 |
 | void level4_cristal_thdt (void)
 |
	.globl _level4_cristal_thdt
_level4_cristal_thdt:
	movw	HD64570_ISR1_ISR0,d3	| Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S0_RINT,d3	| RX interrupt?
	jeq	cristal_thdt_check_TX0	| no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
cristal_thdt_check_TX0:
	btst	#HD64570_S0_TINT,d3	| TX interrupt?
	jeq	cristal_thdt_check_txint0| No, check for tx
	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
cristal_thdt_check_txint0:
	movb	HD64570_ISR0, d1	| Read ISR0
	btst	#HD64570_S0_TXINT, d1	| Check for interrupt bits
	jeq 	cristal_thdt_check_DMIA0| check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
cristal_thdt_check_DMIA0:
	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_thdt_check_c0_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_thdt_check_c0_err:
	movb	HD64570_C0_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_thdt_check_c1_int| No, check for C1 interrupt
	jsr	_hd64570_error_int	| Jump to error handler
cristal_thdt_check_c1_int:
	addql	#4,sp			| adjust stack
	movl	a2@(8),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S1_RINT,d3	| RX interrupt?
	jeq	cristal_thdt_check_TX1	| no, check for TX interrupt
	jsr	_hd64570_RX_interrupt	| call interrupt routine
cristal_thdt_check_TX1:
	btst	#HD64570_S1_TINT,d3	| TX interrupt?
	jeq	cristal_thdt_check_DMIA1| No, check for DMIA
	jsr	_hd64570_TX_interrupt	| Call interrupt routine
cristal_thdt_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_thdt_check_c1_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_thdt_check_c1_err:
	movb	HD64570_C1_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_thdt_check_timer| No, check for timer interrupts
	jsr	_hd64570_error_int	| Jump to error handler
cristal_thdt_check_timer:
	movb	HD64570_ISR2, d1	| Read ISR2
	andb	#0xF0, d1		| Check interrupt bits
	jeq	cristal_thdt_exit	| No, clean up and return
	jsr	_hd64570_timer_interrupt| Call interrupt routine
cristal_thdt_exit:
	addql	#4,sp			| adjust stack
	rts	


 |
 |	level4_cristal_tthd - Level 4 interrupt wrapper for cristal dual
 |	serial with half-duplex only on the second serial port
 |
 | void level4_cristal_tthd (void)
 |
	.globl _level4_cristal_tthd
_level4_cristal_tthd:
	movw	HD64570_ISR1_ISR0,d3	| Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S0_RINT,d3	| RX interrupt?
	jeq	cristal_tthd_check_TX0	| no, check for TX interrupt
	jsr	_hd64570_RX_interrupt	| call interrupt routine
cristal_tthd_check_TX0:
	btst	#HD64570_S0_TINT,d3	| TX interrupt?
	jeq	cristal_tthd_check_DMIA0| No, check for DMIA
	jsr	_hd64570_TX_interrupt	| Call interrupt routine
cristal_tthd_check_DMIA0:
	btst	#HD64570_S0_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_tthd_check_c0_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_tthd_check_c0_err:
	movb	HD64570_C0_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_tthd_check_c1_int| No, check for C1 interrupt
	jsr	_hd64570_error_int	| Jump to error handler
cristal_tthd_check_c1_int:
	addql	#4,sp			| adjust stack
	movl	a2@(8),sp@-		| Push IDB argument for C routines
	btst	#HD64570_S1_RINT,d3	| RX interrupt?
	jeq	cristal_tthd_check_TX1	| no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt| call interrupt routine
cristal_tthd_check_TX1:
	btst	#HD64570_S1_TINT,d3	| TX interrupt?
	jeq	cristal_tthd_check_txint1| No, check for tx
	jsr	_hd64570_TX_HDX_interrupt| Call interrupt routine
cristal_tthd_check_txint1:
	movb	HD64570_ISR0, d1	| Read ISR0
	btst	#HD64570_S1_TXINT, d1	| Check for interrupt bits
	jeq 	cristal_tthd_check_DMIA1| check for DMIA
	jsr	_hd64570_HDX_txint	| Call interrupt routine
cristal_tthd_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	| Check for RX buffer wrap
	jeq	cristal_tthd_check_c1_err	| No, check for errors
	jsr	_hd64570_DMIA_int	| Jump to handler
cristal_tthd_check_c1_err:
	movb	HD64570_C1_ST1, d1	| Read the ST1 register
	btst	#HD64570_UDRN, d1	| Check for underrun
	jeq	cristal_tthd_check_timer| No, check for timer interrupts
	jsr	_hd64570_error_int	| Jump to error handler
cristal_tthd_check_timer:
	movb	HD64570_ISR2, d1	| Read ISR2
	andb	#0xF0, d1		| Check interrupt bits
	jeq	cristal_tthd_exit	| No, clean up and return
	jsr	_hd64570_timer_interrupt| Call interrupt routine
cristal_tthd_exit:
	addql	#4,sp			| adjust stack
	rts	



 |
 |      level4_cristal_bscnil - Level 4 wrapper for cristal dual serial
 |                with a single half duplex BSC port on port 0
 |
        .globl _level4_cristal_bscnil
_level4_cristal_bscnil:
        movw    HD64570_ISR1_ISR0, d3        | Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		     | Push IDB argument for C
        btst    #HD64570_S0_RXRDY,d3         | RX interrupt?
        jeq     cristal_bscnil_check_TX      | no, check for TX interrupt
        jsr     _hd64570_RX_HDX_BSC_interrupt| call interrupt routine
cristal_bscnil_check_TX:
        btst    #HD64570_S0_TINT,d3          | DMA TX interrupt?
        jeq     cristal_bscnil_check_txint   | No, check for tx
        jsr     _hd64570_TX_HDX_interrupt    | Call interrupt routine
cristal_bscnil_check_txint:
        btst    #HD64570_S0_TXINT, d3        | Check interrupt bits in ISR0
        jeq     cristal_bscnil_check_err     | No, check for errors
        jsr     _hd64570_HDX_txint           | Call interrupt routine
cristal_bscnil_check_err:
        movb    HD64570_C0_ST1, d1           | Read the ST1 register
        btst    #HD64570_UDRN, d1            | Check for underrun
        jeq     cristal_bscnil_check_timer   | No, check for timer interrupts
        jsr     _hd64570_error_int           | Jump to error handler
cristal_bscnil_check_timer:
	movb	HD64570_ISR2, d1	     | Read ISR2
        andw    #0xF0, d1                    | Check interrupt bits
        jeq     cristal_bscnil_exit          | No, clean up and return
        jsr     _hd64570_timer_interrupt     | Call interrupt routine
cristal_bscnil_exit:
        addql   #4,sp                        | adjust stack
        rts



 |
 |      level4_cristal_nilbsc - Level 4 wrapper for cristal dual serial
 |                with a single half duplex BSC port on port 1
 |
        .globl _level4_cristal_nilbsc
_level4_cristal_nilbsc:
        movw    HD64570_ISR1_ISR0, d3        | Read HD64570 ISR1_ISR0 register
	movl	a2@(8),sp@-		     | Push port 1 IDB for C routine
        btst    #HD64570_S1_RXRDY,d3         | RX interrupt?
        jeq     cristal_nilbsc_check_TX      | no, check for TX interrupt
        jsr     _hd64570_RX_HDX_BSC_interrupt| call interrupt routine
cristal_nilbsc_check_TX:
        btst    #HD64570_S1_TINT,d3          | TX interrupt?
        jeq     cristal_nilbsc_check_txint   | No, check tx
        jsr     _hd64570_TX_HDX_interrupt    | Call interrupt routine
cristal_nilbsc_check_txint:
        btst    #HD64570_S1_TXINT, d3        | Check interrupt bits in ISR0
        jeq     cristal_nilbsc_check_err     | No, check for errors
        jsr     _hd64570_HDX_txint           | Call interrupt routine
cristal_nilbsc_check_err:
        movb    HD64570_C1_ST1, d1           | Read the ST1 register
        btst    #HD64570_UDRN, d1            | Check for underrun
        jeq     cristal_nilbsc_check_timer   | No, check for timer interrupts
        jsr     _hd64570_error_int           | Jump to error handler
cristal_nilbsc_check_timer:
        movb    HD64570_ISR2, d1             | Read ISR2
        andw    #0xF0, d1                    | Check interrupt bits
        jeq     cristal_nilbsc_exit          | No, clean up and return
        jsr     _hd64570_timer_interrupt     | Call interrupt routine
cristal_nilbsc_exit:
        addql   #4,sp                        | adjust stack
        rts


 |
 |	level4_cristal_bscbsc - Level 4 wrapper for cristal dual serial
 |                with dual half duplex BSC ports.
 |
	.globl _level4_cristal_bscbsc
_level4_cristal_bscbsc:
        movw    HD64570_ISR1_ISR0, d3           | Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		        | Push port 0 IDB arg for C
	btst	#HD64570_S0_RXRDY,d3	        | RX interrupt?
	jeq	cristal_bscbsc_check_TX0        | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt   | call interrupt routine
cristal_bscbsc_check_TX0:
	btst	#HD64570_S0_TINT,d3	        | TX interrupt?
	jeq	cristal_bscbsc_check_txint0     | No, check for tx int
	jsr	_hd64570_TX_HDX_interrupt       | Call interrupt routine
cristal_bscbsc_check_txint0:
	btst	#HD64570_S0_TXINT, d3	        | Check interrupt bits in ISR0
	jeq	cristal_bscbsc_check_c0_err     | No, check for errors
	jsr	_hd64570_HDX_txint	        | Call interrupt routine
cristal_bscbsc_check_c0_err:
        movb    HD64570_C0_ST1, d1              | Read the ST1 register
	btst	#HD64570_UDRN, d1	        | Check for underrun
	jeq	cristal_bscbsc_check_c1_int     | No, check for C1 interrupt
	jsr	_hd64570_error_int	        | Jump to error handler
cristal_bscbsc_check_c1_int:
	addql	#4,sp			        | adjust stack
	movl	a2@(8),sp@-	        	| Push IDB argument for C routines
	btst	#HD64570_S1_RXRDY,d3	        | RX interrupt?
	jeq	cristal_bscbsc_check_TX1        | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt   | call interrupt routine
cristal_bscbsc_check_TX1:
	btst	#HD64570_S1_TINT,d3	        | TX interrupt?
	jeq	cristal_bscbsc_check_txint1     | No, check for tx int
	jsr	_hd64570_TX_HDX_interrupt       | Call interrupt routine
cristal_bscbsc_check_txint1:
	btst	#HD64570_S1_TXINT, d3	        | Check interrupt bits in IRS0 
	jeq	cristal_bscbsc_check_c1_err     | No, check for errors
	jsr	_hd64570_HDX_txint	        | Call interrupt routine
cristal_bscbsc_check_c1_err:
        movb    HD64570_C1_ST1, d1              | Read the ST1 register
	btst	#HD64570_UDRN, d1	        | Check for underrun
	jeq	cristal_bscbsc_check_timer      | No, check for timer interrupts
	jsr	_hd64570_error_int	        | Jump to error handler
cristal_bscbsc_check_timer:
	movb	HD64570_ISR2, d1                | Read ISR2
	andw	#0xF0, d1		        | Check interrupt bits
	jeq	cristal_bscbsc_exit             | No, clean up and return
	jsr	_hd64570_timer_interrupt        | Call interrupt routine
cristal_bscbsc_exit:
	addql	#4,sp			        | adjust stack
	rts	

 |
 |	level4_cristal_bsct - Level 4 wrapper for dual cristal serial
 |      half-duplex BSC on first and full duplex SDLC second port.
 |
	.globl _level4_cristal_bsct
_level4_cristal_bsct:
        movw    HD64570_ISR1_ISR0, d3           | Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		        | Push port 0 IDB for C routines
	btst	#HD64570_S0_RXRDY,d3	        | RX interrupt?
	jeq	cristal_bsct_check_TX0          | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt   | call interrupt routine
cristal_bsct_check_TX0:
	btst	#HD64570_S0_TINT,d3	        | TX interrupt?
	jeq	cristal_bsct_check_txint0       | No, check for tx
	jsr	_hd64570_TX_HDX_interrupt       | Call interrupt routine
cristal_bsct_check_txint0:
	btst	#HD64570_S0_TXINT, d3	        | Check interrupt bits in ISR0
	jeq 	cristal_bsct_check_c0_err       | No, check for errors
	jsr	_hd64570_HDX_txint	        | Call interrupt routine
cristal_bsct_check_c0_err:
        movb    HD64570_C0_ST1, d1              | Read the ST1 register
	btst	#HD64570_UDRN, d1	        | Check for underrun
	jeq	cristal_bsct_check_c1_int       | No, check for C1 interrupt
	jsr	_hd64570_error_int	        | Jump to error handler
cristal_bsct_check_c1_int:
	addql	#4,sp			        | adjust stack
	movl	a2@(8),sp@-		        | Push IDB argument for C routines
	btst	#HD64570_S1_RINT,d3	        | RX interrupt?
	jeq	cristal_bsct_check_TX1          | no, check for TX interrupt
	jsr	_hd64570_RX_interrupt	        | call interrupt routine
cristal_bsct_check_TX1:
	btst	#HD64570_S1_TINT,d3	        | TX interrupt?
	jeq	cristal_bsct_check_DMIA1        | No, check for DMIA
	jsr	_hd64570_TX_interrupt	        | Call interrupt routine
cristal_bsct_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	        | Check for RX buffer wrap
	jeq	cristal_bsct_check_c1_err       | No, check for errors
	jsr	_hd64570_DMIA_int	        | Jump to handler
cristal_bsct_check_c1_err:
        movb    HD64570_C1_ST1, d1              | Read the ST1 register
	btst	#HD64570_UDRN, d1	        | Check for underrun
	jeq	cristal_bsct_check_timer        | No, check for timer interrupts
	jsr	_hd64570_error_int	        | Jump to error handler
cristal_bsct_check_timer:
        movb    HD64570_ISR2, d1                | Read ISR2
	andw	#0xF0, d1		        | Check interrupt bits
	jeq	cristal_bsct_exit	        | No, clean up and return
	jsr	_hd64570_timer_interrupt        | Call interrupt routine
cristal_bsct_exit:
	addql	#4,sp			        | adjust stack
	rts	


 |
 |	level4_cristal_tbsc - Level 4 wrapper for dual cristal serial
 |      full duplex SDLC on first and half-duplex BSC second port.
 |
 	.globl _level4_cristal_tbsc
 _level4_cristal_tbsc:
	movw	HD64570_ISR1_ISR0,d3		| Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		        | Push IDB argument for C routines
 	btst	#HD64570_S0_RINT,d3	        | RX interrupt?
 	jeq	cristal_tbsc_check_TX0          | no, check for TX interrupt
 	jsr	_hd64570_RX_interrupt	        | call interrupt routine
 cristal_tbsc_check_TX0:
 	btst	#HD64570_S0_TINT,d3	        | TX interrupt?
 	jeq	cristal_tbsc_check_DMIA0        | No, check for DMIA
 	jsr	_hd64570_TX_interrupt	        | Call interrupt routine
 cristal_tbsc_check_DMIA0:
 	btst	#HD64570_S0_RERR, d3	        | Check for RX buffer wrap
 	jeq	cristal_tbsc_check_c0_err       | No, check for errors
 	jsr	_hd64570_DMIA_int	        | Jump to handler
 cristal_tbsc_check_c0_err:
        movb    HD64570_C0_ST1, d1              | Read the ST1 register
 	btst	#HD64570_UDRN, d1	        | Check for underrun
 	jeq	cristal_tbsc_check_c1_int       | No, check for C1 interrupt
 	jsr	_hd64570_error_int	        | Jump to error handler
 cristal_tbsc_check_c1_int:
	addql	#4,sp			        | adjust stack
	movl	a2@(8),sp@-		        | Push IDB argument for C routines
 	btst	#HD64570_S1_RXRDY,d3	        | RX interrupt?
 	jeq	cristal_tbsc_check_TX1          | no, check for TX interrupt
 	jsr	_hd64570_RX_HDX_BSC_interrupt   | call interrupt routine
 cristal_tbsc_check_TX1:
 	btst	#HD64570_S1_TINT,d3	        | TX interrupt?
 	jeq	cristal_tbsc_check_txint1       | No, check for tx
 	jsr	_hd64570_TX_HDX_interrupt       | Call interrupt routine
 cristal_tbsc_check_txint1:
 	btst	#HD64570_S1_TXINT, d3	        | Check interrupt bits in ISR0
 	jeq 	cristal_tbsc_check_c1_err       | No, check for errors
 	jsr	_hd64570_HDX_txint	        | Call interrupt routine
 cristal_tbsc_check_c1_err:
        movb    HD64570_C1_ST1, d1              | Read the ST1 register
 	btst	#HD64570_UDRN, d1	        | Check for underrun
 	jeq	cristal_tbsc_check_timer        | No, check for C1 interrupt
 	jsr	_hd64570_error_int	        | Jump to error handler
 cristal_tbsc_check_timer:
        movb    HD64570_ISR2, d1                | Read ISR2
 	andw	#0xF0, d1		        | Check interrupt bits
 	jeq	cristal_tbsc_exit	        | No, clean up and return
 	jsr	_hd64570_timer_interrupt        | Call interrupt routine
 cristal_tbsc_exit:
 	addql	#4,sp			        | adjust stack
 	rts	

 |
 |	level4_cristal_bscthd - Level 4 wrapper for dual cristal serial
 |      half-duplex BSC on first and half-duplex SDLC second port.
 |
	.globl _level4_cristal_bscthd
_level4_cristal_bscthd:
	movw	HD64570_ISR1_ISR0,d3	       | Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		       | Push IDB argument for C routines
	btst	#HD64570_S0_RXRDY,d3	       | RX interrupt?
	jeq	cristal_bscthd_check_TX0       | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt  | call interrupt routine
cristal_bscthd_check_TX0:
	btst	#HD64570_S0_TINT,d3	       | TX interrupt?
	jeq	cristal_bscthd_check_txint0    | No, check for tx
	jsr	_hd64570_TX_HDX_interrupt      | Call interrupt routine
cristal_bscthd_check_txint0:
	btst	#HD64570_S0_TXINT, d3	       | Check interrupt bits in ISR0
	jeq 	cristal_bscthd_check_c0_err    | No, check for errors
	jsr	_hd64570_HDX_txint	       | Call interrupt routine
cristal_bscthd_check_c0_err:
        movb    HD64570_C0_ST1, d1             | Read the ST1 register
	btst	#HD64570_UDRN, d1	       | Check for underrun
	jeq	cristal_bscthd_check_c1_int    | No, check for C1 interrupt
	jsr	_hd64570_error_int	       | Jump to error handler
cristal_bscthd_check_c1_int:
	addql	#4,sp			       | adjust stack
	movl	a2@(8),sp@-		       | Push IDB argument for C routines
	btst	#HD64570_S1_RINT,d3	       | RX interrupt?
	jeq	cristal_bscthd_check_TX1       | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt      | call interrupt routine
cristal_bscthd_check_TX1:
	btst	#HD64570_S1_TINT,d3	       | TX interrupt?
	jeq	cristal_bscthd_check_txint1    | No, check for DMIA
	jsr	_hd64570_TX_HDX_interrupt      | Call interrupt routine
cristal_bscthd_check_txint1:
	btst	#HD64570_S1_TXINT, d3	       | Check for interrupt bits in IRS0 
	jeq	cristal_bscthd_check_DMIA1     | No, check for DMIA
	jsr	_hd64570_HDX_txint	       | Call interrupt routine
cristal_bscthd_check_DMIA1:
	btst	#HD64570_S1_RERR, d3	       | Check for RX buffer wrap
	jeq	cristal_bscthd_check_c1_err    | No, check for errors
	jsr	_hd64570_DMIA_int	       | Jump to handler
cristal_bscthd_check_c1_err:
        movb    HD64570_C1_ST1, d1             | Read the ST1 register
	btst	#HD64570_UDRN, d1	       | Check for underrun
	jeq	cristal_bscthd_check_timer     | No, check for timer interrupts
	jsr	_hd64570_error_int	       | Jump to error handler
cristal_bscthd_check_timer:
        movb    HD64570_ISR2, d1               | Read ISR2
	andw	#0xF0, d1		       | Check interrupt bits
	jeq	cristal_bscthd_exit	       | No, clean up and return
	jsr	_hd64570_timer_interrupt       | Call interrupt routine
cristal_bscthd_exit:
	addql	#4,sp			       | adjust stack
	rts	


 |
 |	level4_cristal_thdbsc - Level 4 wrapper for dual cristal serial
 |      half-duplex SDLC on first and half-duplex BSC second port.
 |
	.globl _level4_cristal_thdbsc
_level4_cristal_thdbsc:
	movw	HD64570_ISR1_ISR0,d3	     | Read HD64570 ISR1_ISR0 register
	movl	a2@(4),sp@-		     | Push IDB argument for C routines
	btst	#HD64570_S0_RINT,d3	     | RX interrupt?
	jeq	cristal_thdbsc_check_TX0     | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_interrupt    | call interrupt routine
cristal_thdbsc_check_TX0:
	btst	#HD64570_S0_TINT,d3	     | TX interrupt?
	jeq	cristal_thdbsc_check_txint0  | No, check for tx
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
cristal_thdbsc_check_txint0:
	btst	#HD64570_S0_TXINT, d3	     | Check interrupt bits in ISR0
	jeq 	cristal_thdbsc_check_DMIA0   | check for DMIA
	jsr	_hd64570_HDX_txint	     | Call interrupt routine
cristal_thdbsc_check_DMIA0:
	btst	#HD64570_S0_RERR, d3	     | Check for RX buffer wrap
	jeq	cristal_thdbsc_check_c0_err  | No, check for errors
	jsr	_hd64570_DMIA_int	     | Jump to handler
cristal_thdbsc_check_c0_err:
        movb    HD64570_C0_ST1, d1           | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	cristal_thdbsc_check_c1_int  | No, check for C1 interrupt
	jsr	_hd64570_error_int	     | Jump to error handler
cristal_thdbsc_check_c1_int:
	addql	#4,sp			     | adjust stack
	movl	a2@(8),sp@-		     | Push IDB argument for C routines
	btst	#HD64570_S1_RXRDY,d3	     | RX interrupt?
	jeq	cristal_thdbsc_check_TX1     | no, check for TX interrupt
	jsr	_hd64570_RX_HDX_BSC_interrupt| call interrupt routine
cristal_thdbsc_check_TX1:
	btst	#HD64570_S1_TINT,d3	     | TX interrupt?
	jeq	cristal_thdbsc_check_txint1  | No, check tx
	jsr	_hd64570_TX_HDX_interrupt    | Call interrupt routine
cristal_thdbsc_check_txint1:
	btst	#HD64570_S1_TXINT, d3	     | Check interrupt bits in ISR0
	jeq 	cristal_thdbsc_check_c1_err  | No, check for errors
	jsr	_hd64570_HDX_txint	     | Call interrupt routine
cristal_thdbsc_check_c1_err:
        movb    HD64570_C1_ST1, d1           | Read the ST1 register
	btst	#HD64570_UDRN, d1	     | Check for underrun
	jeq	cristal_thdbsc_check_timer   | No, check for timer interrupts
	jsr	_hd64570_error_int	     | Jump to error handler
cristal_thdbsc_check_timer:
        movb    HD64570_ISR2, d1             | Read ISR2
	andw	#0xF0, d1		     | Check interrupt bits
	jeq	cristal_thdbsc_exit	     | No, clean up and return
	jsr	_hd64570_timer_interrupt     | Call interrupt routine
cristal_thdbsc_exit:
	addql	#4,sp			     | adjust stack
	rts	






