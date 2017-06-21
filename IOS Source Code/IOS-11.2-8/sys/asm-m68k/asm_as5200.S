| $Id: asm_as5200.S,v 3.1.62.1 1996/06/16 21:10:54 vnguyen Exp $
| $Source: /release/112/cvs/Xsys/asm-m68k/asm_as5200.S,v $
|------------------------------------------------------------------
| asm_as5200.S -- Assembler routines for the 68030 AS5200
|
| December 1995, Joe Turner
|
| Copyright (c) 1995-1996 by cisco Systems, Inc.
| All rights reserved.
|
| Registers a0, a1, d0, and d1 are always available.
| Any other registers must be preserved for the caller.
|------------------------------------------------------------------
| $Log: asm_as5200.S,v $
| Revision 3.1.62.1  1996/06/16  21:10:54  vnguyen
| Finally committing Brasil to California_branch
|
| Revision 3.1  1995/12/20  05:55:16  jturner
| Placeholder for new development.
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_as5200.h"

 |
 | Written for faster network interrupt handling.
 | We gain speed by calling the appropriate C routines directly from here.
 |
 | This file contaions our interrupt wrappers. The wrappers check the 
 | interrupt bits to discover which interface caused the interrupt.  They then 
 | index into  pan_int_array and jump to the handler for that interface.  The
 | wrappers do interrupt chaining by looping until no more level4 interrupts
 | are asserted.
 |
 | The interface specific wrapper routines will test CSR0 for each network
 | interface to determine which one of the RX, TX, or Error interrupt routines
 | from the sys/les drivers is to be called.
 |
 | PLEASE NOTE that it is assumed that the RAP register for every interface
 | is always zero. The C code must ensure that network interrupts are
 | disabled before changing RAP and must restore the RAP to 0 before enabling
 | network interrupts again.
 |
 | Note that we do not clean up the IDB pointer from the interrupt stack
 | after calling the C routines. We do not have to since we are restoring
 | the stack pointer via the moveml before returning.

 |
 |	level3 - Level 3 interrupt routine for AS5200 systems.
 |               Level 3 interrupts are caused by a change in the
 |               serial state (CTS, DCD or DSR). The channel that 
 |               interrupted is checked for and the idb pointer is 
 |               pushed onto the stack.
 |
 | void level3 (void)
 |
        .globl _level3_brasil
_level3_brasil:
        moveml  #0xC0C0,sp@-            | save registers
        movl    sp,_l3sp                | save current stack pointer
        movl    _l3stk,sp               | get new stack
	movw    ADRSPC_INTERRUPT,d0     | get the control reg. contents
	btst    #SCR4_LEV3_CH_C_INT_BIT,d0 | test if serial interrupt bit is set
	beq     _level3done             | noone claims interrupt, return
_level3disp:
        movl    _lev3subr,a0            | get routine address
        jsr     a0@                     | dispatch
_level3done:
        movl    _l3sp,sp                | restore old stack
        moveml  sp@+,#0x0303            | restore registers
        addql   #1,_l3cnt               | count interrupt
        rte                             | return from exception

 |
 |      level4_brasil - Level 4 interrupt wrapper for Brasil
 |
 | void level4_brasil (void)
 |
        .globl _level4_brasil
_level4_brasil:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
        movl    sp,_l4sp                | Save current stack pointer
        movl    _l4stk,sp               | Get new stack pointer
level4_brasil_loop_return:
        lea     _pan_int_array,a2       | Load up pointer
        movw    ADRSPC_INTERRUPT,d2     | Read the control register
        andw    #0x000F,d2              | mask bits
        jeq     level4_brasil_exit      | Nothing there?  Check slots.
        btst    #SCR4_LEV4_CH_A_INT_BIT,d2      | Token Ring interrupt bit set?
        jeq     level4_brasil_check_b   | no, check for b
        movl    a2@, a3                 | Set up subroutine ptr
        jbsr    a3@                     | jmp to handler
level4_brasil_check_b:
        addal   #12,a2                  | bump counter
        btst    #SCR4_LEV4_CH_B_INT_BIT,d2      | Ethernet interrupt bit set?
        jeq     level4_brasil_check_c   | no, check for c
        movl    a2@, a3                 | Set up subroutine ptr
        jbsr    a3@                     | jmp to handler
level4_brasil_check_c:
        addal   #12,a2                  | bump counter
        btst    #SCR4_LEV4_CH_D_INT_BIT,d2      | Serial interrupt bit set?
        jeq     level4_brasil_check_d   | no, check for c
        movl    a2@, a3                 | Set up subroutine ptr
        jbsr    a3@                     | jmp to handler
level4_brasil_check_d:
        addal   #12,a2                  | bump counter
        btst    #SCR4_LEV4_CH_C_INT_BIT,d2      | Basic rate interrupt bit set?
        jeq     level4_brasil_loop_return
        movl    a2@, a3                 | Set up subroutine ptr
        jbsr    a3@
        jmp     level4_brasil_loop_return | Try again
level4_brasil_exit:
        movl    _l4sp,sp                | restore old stack
        moveml  sp@+,REGISTERS          | restore registers from old stack
        addql   #1,_l4cnt               | count interrupt
        rte

 |
 |      level4_hi_irq_dispatch wrapper - Level 4 interrupt wrapper
 |		for slot interrupts 
 |
 | void level4_hi_irq_dispatch (void)
 |
	.globl _level4_hi_irq_dispatch
_level4_hi_irq_dispatch:
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	jsr	_hi_irq_dispatcher	| Call interrupt routine
	addql	#4,sp			| adjust stack
	rts				| bye bye

 |
 |	level4_nop - Level 4 interrupt wrapper for a nop
 |
 | void level4_nop (void)
 |
	.globl _level4_nop
_level4_nop:
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	jsr	_as5200_spurious_level4	| Error, we should never get this
	addql	#4,sp			| adjust stack
	rts				| bye bye

 |
 |	level6_nop - Level 6 interrupt wrapper for a nop
 |
 | void level6_nop (void)
 |
	.globl _level6_nop
_level6_nop:
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	jsr	_as5200_spurious_level6	| Error, we should never get this
	addql	#4,sp			| adjust stack
	rts				| bye bye

 |
 |      level6_brasil - Level 6 interrupt for Brasil
 |
 | void level6_brasil (void)
 |
        .globl _level6_brasil
_level6_brasil:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
        movl    sp,_l6sp                | Save current stack pointer
        movl    _l6stk,sp               | Get new stack pointer
        lea     _t1_int_array,a2        | Load up pointer
        movl    a2@, a3                 | Set up subroutine ptr
        jbsr    a3@                     | jmp to handler
        movl    _l6sp,sp                | restore old stack
        moveml  sp@+,REGISTERS          | restore registers from old stack
        addql   #1,_l6cnt               | count interrupt
        rte
