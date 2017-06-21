| $Id: asm_c3000.S,v 3.2.60.2 1996/04/19 20:23:47 etrehus Exp $
| $Source: /release/112/cvs/Xsys/asm-m68k/asm_c3000.S,v $
|------------------------------------------------------------------
| asm_pan.s -- Assembler routines for the 68020 Based Pancake
|
| October 1989, Chris Shaker
|
| Copyright (c) 1989-1996 by cisco Systems, Inc.
| All rights reserved.
|
| Registers a0, a1, d0, and d1 are always available.
| Any other registers must be preserved for the caller.
|------------------------------------------------------------------
| /*
| $Log: asm_c3000.S,v $
| Revision 3.2.60.2  1996/04/19  20:23:47  etrehus
| CSCdi51928:  3000 restarts after SCHED-2-WATCH error
| Added temporary level 3 interrupt handler if mk5025 is present during
| boot sequence to work around hardware shortcommings, cause the interrupt
| cannot be disabled at the source.
|
| Revision 3.2.60.1  1996/03/18  18:50:48  gstovall
| Branch: California_branch
| Elvis has left the building.  He headed out to California, and took the
| port ready changes with him.
|
| Revision 3.2.26.2  1996/03/09  05:03:41  hampton
| Slim down the kernel of the router.
| Branch: DeadKingOnAThrone_branch
|
| Revision 3.2.26.1  1996/02/20  13:32:10  dstine
| Branch: DeadKingOnAThrone_branch
|         Sync from DeadKingOnAThrone_baseline_960122 to
|                   DeadKingOnAThrone_baseline_960213
|
| Revision 3.2  1995/11/17  08:39:26  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:16  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:25  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
| */
|

#include "asm_c3000.h"
|
 |
 | Hacked for faster network interrupts on Pancake.
 | We gain speed by getting rid of ether_interrupt() and calling the
 | appropriate C routines directly from here.
 |
 | This file contaions new style nifty interrupt wrappers.  There is an outside
 | wrapper for each platform (pan, brut, cristal).  The wrappers check the 
 | interrupt bits to discover which interface caused the interrupt.  They then 
 | index into  pan_int_array and jump to the handler for that interface.  The
 | wrappers do interrupt chaining by looping until no more level4 interrupts
 | are asserted.
 |
 | These wrapper routines will test CSR0 in each network interface
 | to determine which of the RX, TX, or Error interrupt routines from
 | if/if_pan_lance and if/if_pan_mk5025.c are to be called.
 |
 | PLEASE NOTE that it is assumed that the RAP register for every interface
 | is always zero. The C code must ensure that network interrupts are
 | disabled before changing RAP and must restore the RAP to 0 before enabling
 | network interrupts again.
 |
 | Note that we do not clean up the IDB pointer from the interrupt stack
 | after calling the C routines. We do not have to since we are restoring
 | the stack pointer via the moveml before returning.

/* Control register interrupt bits for level 4 interrutps */
#define NETWORK_A_BIT       6    /* Bit number for bit tests chnl A */
#define NETWORK_B_BIT       7    /* Bit number for bit tests chnl B */
#define CRISTAL_NET_A_BIT   0    /* Bit number for bit tests token ring */
#define CRISTAL_NET_B_BIT   1    /* Bit number for tests ether */
#define CRISTAL_NET_C_BIT   2    /* Bit number for tests serial */
#define CRISTAL_NET_D_BIT   3    /* Bit number for tests bri */
#define DEC_NETWORK_C_BIT   5    /* this bit is for the 2nd serial */
                                 /* channel for a DEC box. another */
                                 /* 3 port may use a different bit */
#define CAMPECHE_ETHER_NET_C_BIT 4 /* Campeche Enet Ch C bit */
#define CAMPECHE_TOKEN_NET_C_BIT 5 /* Campeche Token Ring Ch C bit */
/* Mask bits for Campeche interrupt wrappers */
#define CAMPECHE_2R2T_MASK      0x0025
#define CAMPECHE_1R2T1E_MASK    0x0015
#define CAMPECHE_2E2T_MASK      0x0016
/* Control register interrupt bits for level 3 interrutps */
#define DEC_STATE_B_BIT	    2    /* bit num for serial state change chnl b */
#define DEC_STATE_C_BIT	    1    /* bit num for serial state change chnl c */
#define CRISTAL_STATE_C_BIT 10   /* bit num for serial state change chnl c */


|
|       level2_hub - Level 2 interrupt routine for the Galapagos ethernet hub.
|                    This interrupt is cleared when it is serviced.
|
        .globl _level2_hub
_level2_hub:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
        movl    sp,_l2sp                | Save current stack pointer
        movl    _l2stk,sp               | Get new stack pointer
	lea	_pan_level2_int,a2	| Load up pointer
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@			| jmp to handler
        movl    _l2sp,sp                | restore old stack
        moveml  sp@+,REGISTERS          | restore registers from old stack
        addql   #1,_l2cnt               | count interrupt
        rte                             | return from exception


|
|       level2_dummy_rte - Dummy level 2 interrupt routine for Fiji.
|                          It is installed in main().
|                          It is overrided when the PCbus driver installs
|                          the real one.
|
	.globl _level2_dummy_rte
_level2_dummy_rte:
	rte				| back to where we came

|
 |
 |	level3_pan - Level 3 interrupt routine for IGS (pancake) systems.
 |               Level 3 interrupts are caused by a change in the
 |               serial state (CTS, DCD or DSR). The channel that 
 |               interrupted is checked for and the idb pointer is 
 |               pushed onto the stack.
 |
 | void level3_pan (void)
 |
        .globl _level3_pan
_level3_pan:
        moveml  #0xC0C0,sp@-            | save registers
        movl    sp,_l3sp                | save current stack pointer
        movl    _l3stk,sp               | get new stack
        movl    _cookie,a0              | get the cookie address
	cmpb    #7,a0@                  | get the interfaces field from cookie
        bne     _level3e1t              | not equal to 1E2T, go to 1E1T
        movw    ADRSPC_CONTROL,d0       | get the control reg. contents
        btst    #DEC_STATE_C_BIT,d0     | test if chnl c bit is set
        beq     _level3intb             | not set, check chnl c bit
        movl    _network_c_idb,sp@-     | push chnl c idb ptr onto stack
        bra     _level3disp             | jump to call to isr
_level3intb:
        btst    #DEC_STATE_B_BIT,d0     | test if chnl b bit is set
        beq     _level3done             | noone claims interrupt, return
_level3e1t:
        movl    _network_b_idb,sp@-     | push chnl b idb ptr onto stack
_level3disp:
        movl    _lev3subr,a0            | get routine address
        jsr     a0@                     | dispatch
_level3done:
        movl    _l3sp,sp                | restore old stack
        moveml  sp@+,#0x0303            | restore registers
        addql   #1,_l3cnt               | count interrupt
        rte                             | return from exception
|
 |
 |	level3_cristal - Level 3 interrupt routine for 2500 systems.
 |               Level 3 interrupts are caused by a change in the
 |               serial state (CTS, DCD or DSR). The channel that 
 |               interrupted is checked for and the idb pointer is 
 |               pushed onto the stack.
 |
 | void level3_cristal (void)
 |
        .globl _level3_cristal
_level3_cristal:
        moveml  #0xC0C0,sp@-            | save registers
        movl    sp,_l3sp                | save current stack pointer
        movl    _l3stk,sp               | get new stack
        movl    _cookie,a0              | get the cookie address
	movw    ADRSPC_INTERRUPT,d0     | get the control reg. contents
	btst    #CRISTAL_STATE_C_BIT,d0 | test if chnl c bit is set
	beq     _level3done             | noone claims interrupt, return
	bra	_level3disp		| handle it


 |
 |	level4_pan - Level 4 interrupt wrapper for pan
 |
 | void level4_pan (void)
 |
	.globl _level4_pan
_level4_pan:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l4sp		| Save current stack pointer
	movl	_l4stk,sp		| Get new stack pointer
level4_pan_loop_return:
	lea	_pan_int_array,a2	| Load up pointer
	movw	ADRSPC_CONTROL,d2	| Read the control register
	andw	#0x00C0,d2		| mask bits
	jeq	level4_pan_exit		| nothing there, goodbye
	btst	#NETWORK_A_BIT,d2	| Lance A interrupt bit set?
	jeq	level4_pan_check_b	| no, check for b
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@			| jmp to handler
level4_pan_check_b:
	addal	#12,a2			| bump counter
	btst	#NETWORK_B_BIT,d2	| Lance B interrupt bit set?
	jeq	level4_pan_loop_return
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@
	jmp	level4_pan_loop_return	| Try again
level4_pan_exit:
	movl	_l4sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l4cnt		| count interrupt
	rte	


 |
 |	level4_brut - Level 4 interrupt wrapper for brut
 |
 | void level4_brut (void)
 |
	.globl _level4_brut
_level4_brut:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l4sp		| Save current stack pointer
	movl	_l4stk,sp		| Get new stack pointer
level4_brut_loop_return:
	lea	_pan_int_array,a2	| Load up pointer
	movw	ADRSPC_CONTROL,d2	| Read the control register
	andw	#0x00E0,d2		| mask bits
	jeq	level4_brut_exit	| nothing there, goodbye
	btst	#NETWORK_A_BIT,d2	| Lance A interrupt bit set?
	jeq	level4_brut_check_b	| no, check for b
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@			| jmp to handler
level4_brut_check_b:
	addal	#12,a2			| bump counter
	btst	#NETWORK_B_BIT,d2	| Lance B interrupt bit set?
	jeq	level4_brut_check_c
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@
level4_brut_check_c:
	addal	#12,a2			| bump pointer
	btst	#DEC_NETWORK_C_BIT,d2	| serial C interrupt bit set?
	jeq	level4_brut_loop_return
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@
	jmp	level4_brut_loop_return	| Try again
level4_brut_exit:
	movl	_l4sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l4cnt		| count interrupt
	rte


 |
 |	level4_cristal - Level 4 interrupt wrapper for cristal
 |
 | void level4_cristal (void)
 |
	.globl _level4_cristal
_level4_cristal:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l4sp		| Save current stack pointer
	movl	_l4stk,sp		| Get new stack pointer
level4_cristal_loop_return:
	lea	_pan_int_array,a2	| Load up pointer
	movw	ADRSPC_INTERRUPT,d2	| Read the control register
	andw	#0x000F,d2		| mask bits
	jeq	level4_cristal_exit	| nothing there, goodbye
	btst	#CRISTAL_NET_A_BIT,d2	| Token Ring interrupt bit set?
	jeq	level4_cristal_check_b	| no, check for b
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@			| jmp to handler
level4_cristal_check_b:
	addal	#12,a2			| bump counter
	btst	#CRISTAL_NET_B_BIT,d2	| Ethernet interrupt bit set?
	jeq	level4_cristal_check_c	| no, check for c
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@			| jmp to handler
level4_cristal_check_c:
	addal	#12,a2			| bump counter
	btst	#CRISTAL_NET_D_BIT,d2	| Serial interrupt bit set?
	jeq	level4_cristal_check_d	| no, check for d
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@			| jmp to handler
level4_cristal_check_d:
	addal	#12,a2			| bump counter
	btst	#CRISTAL_NET_C_BIT,d2	| Basic rate interrupt bit set?
	jeq	level4_cristal_loop_return
	movl	a2@, a3			| Set up subroutine ptr
	jbsr	a3@
	jmp	level4_cristal_loop_return | Try again
level4_cristal_exit:
	movl	_l4sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l4cnt		| count interrupt
	rte

 |
 |      level4_campeche_2r2t - Level 4 interrupt wrapper
 |              for Campeche 2R2T configuration
 |
 | void level4_campeche_2r2t (void)
 |
	.globl _level4_campeche_2r2t
_level4_campeche_2r2t:
	moveml  REGISTERS,sp@-          | Save temp registers on process stack
	movl    sp,_l4sp                | Save current stack pointer
	movl    _l4stk,sp               | Get new stack pointer
level4_campeche_2r2t_loop_return:
	lea     _pan_int_array,a2       | Load up pointer
	movw    ADRSPC_INTERRUPT,d2     | Read the control register
	andw    #CAMPECHE_2R2T_MASK,d2  | mask bits
	jeq     level4_campeche_2r2t_exit       | nothing there, goodbye
	btst    #CRISTAL_NET_A_BIT,d2   | Primary TRing interrupt bit set?
	jeq     level4_campeche_2r2t_check_b    | no, check for b
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@                     | jmp to handler
level4_campeche_2r2t_check_b:
	addal   #12,a2                  | bump counter
	btst    #CAMPECHE_TOKEN_NET_C_BIT,d2    | 2nd TRing interrupt bit set?
	jeq     level4_campeche_2r2t_check_c    | no, check for c
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@
level4_campeche_2r2t_check_c:
	addal   #24,a2                  | bump counter
	btst    #CRISTAL_NET_C_BIT,d2   | Serial interrupt bit set?
	jeq     level4_campeche_2r2t_loop_return
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@                     | jmp to handler
	jmp     level4_campeche_2r2t_loop_return | Try again
level4_campeche_2r2t_exit:
	movl    _l4sp,sp                | restore old stack
	moveml  sp@+,REGISTERS          | restore registers from old stack
	addql   #1,_l4cnt               | count interrupt
	rte

 |
 |      level4_campeche_2e2t - Level 4 interrupt wrapper
 |              for Campeche 2E2T configuration
 |
 | void level4_campeche_2e2t (void)
 |
	.globl _level4_campeche_2e2t
_level4_campeche_2e2t:
	moveml  REGISTERS,sp@-          | Save temp registers on process stack
	movl    sp,_l4sp                | Save current stack pointer
	movl    _l4stk,sp               | Get new stack pointer
level4_campeche_2e2t_loop_return:
	lea     _pan_int_array,a2       | Load up pointer
	movw    ADRSPC_INTERRUPT,d2     | Read the control register
	andw    #CAMPECHE_2E2T_MASK,d2  | mask bits
	jeq     level4_campeche_2e2t_exit       | nothing there, goodbye
	btst    #CRISTAL_NET_B_BIT,d2   | Primary Ethernet interrupt bit set?
	jeq     level4_campeche_2e2t_check_b    | no, check for b
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@                     | jmp to handler
level4_campeche_2e2t_check_b:
	addal   #12,a2                  | bump counter
	btst    #CAMPECHE_ETHER_NET_C_BIT,d2    | 2nd Enet interrupt bit set?
	jeq     level4_campeche_2e2t_check_c    | no, check for c
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@
level4_campeche_2e2t_check_c:
	addal   #24,a2                  | bump counter
	btst    #CRISTAL_NET_C_BIT,d2   | Serial interrupt bit set?
	jeq     level4_campeche_2e2t_loop_return
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@                     | jmp to handler
	jmp     level4_campeche_2e2t_loop_return | Try again
level4_campeche_2e2t_exit:
	movl    _l4sp,sp                | restore old stack
	moveml  sp@+,REGISTERS          | restore registers from old stack
	addql   #1,_l4cnt               | count interrupt
	rte

 |
 |      level4_campeche_1r2t1e - Level 4 interrupt wrapper
 |              for Campeche 1R2T1E configuration
 |
 | void level4_campeche_1r2t1e (void)
 |
	.globl _level4_campeche_1r2t1e
_level4_campeche_1r2t1e:
	moveml  REGISTERS,sp@-          | Save temp registers on process stack
	movl    sp,_l4sp                | Save current stack pointer
	movl    _l4stk,sp               | Get new stack pointer
level4_campeche_1r2t1e_loop_return:
	lea     _pan_int_array,a2       | Load up pointer
	movw    ADRSPC_INTERRUPT,d2     | Read the control register
	andw    #CAMPECHE_1R2T1E_MASK,d2        | mask bits
	jeq     level4_campeche_1r2t1e_exit     | nothing there, goodbye
	btst    #CRISTAL_NET_A_BIT,d2   | Token Ring interrupt bit set?
	jeq     level4_campeche_1r2t1e_check_b  | no, check for b
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@                     | jmp to handler
level4_campeche_1r2t1e_check_b:
	addal   #12,a2                  | bump counter
	btst    #CAMPECHE_ETHER_NET_C_BIT,d2    | Ethernet interrupt bit set?
	jeq     level4_campeche_1r2t1e_check_c  | no, check for c
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@
level4_campeche_1r2t1e_check_c:
	addal   #24,a2                  | bump counter
	btst    #CRISTAL_NET_C_BIT,d2   | Serial interrupt bit set?
	jeq     level4_campeche_1r2t1e_loop_return
	movl    a2@, a3                 | Set up subroutine ptr
	jbsr    a3@                     | jmp to handler
	jmp     level4_campeche_1r2t1e_loop_return | Try again
level4_campeche_1r2t1e_exit:
	movl    _l4sp,sp                | restore old stack
	moveml  sp@+,REGISTERS          | restore registers from old stack
	addql   #1,_l4cnt               | count interrupt
	rte

 |
 |	level4_nop - Level 4 interrupt wrapper for a nop
 |
 | void level4_nop (void)
 |
	.globl _level4_nop
_level4_nop:
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	jsr	_c3000_spurious_level4	| Error, we should never get this
	addql	#4,sp			| adjust stack
	rts				| bye bye

|
|     This is a work-around for a poor hardware design on the
|     mk5025 circuitry.  There is no way to disable the level
|     3 interrupts.  The best we can do is receive the interrupt,
|     handle it in a simple way, and then return.
|
      .globl  _no_mk5025_exception_handler
_no_mk5025_exception_handler:
      movl    a0,sp@-                         | save a0.
      movl    #ADRSPC_SERIAL_STATE_B,a0       | load this address into a0.
      movb    #0xFF,a0@                       | clear the interrupt.
      movl    sp@+,a0                         | restore a0.
      rte                                     | return from the exception.

      .globl  _no_mk5025_exception_handler_2
_no_mk5025_exception_handler_2:
      movl    a0,sp@-                         | save a0.
      movl    #ADRSPC_SERIAL_STATE_B,a0       | load this address into a0.
      movb    #0xFF,a0@                       | clear the interrupt.
      movl    #ADRSPC_SERIAL_STATE_C,a0       | load this address into a0.
      movb    #0xFF,a0@                       | clear the interrupt.
      movl    sp@+,a0                         | restore a0.
      rte             