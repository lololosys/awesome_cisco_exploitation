/* $Id: asm_c4000_bsi.S,v 3.2 1995/11/17 08:39:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c4000_bsi.S,v $
 *------------------------------------------------------------------
 * Assembly wrappers for Nat Semi BSI/BMAC/PLAYER NIM
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c4000_bsi.S,v $
| Revision 3.2  1995/11/17  08:39:42  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:30  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:44  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c4000.h"

/*
 * BSI_MAR_BITS
 */

#define SLOT1_BSI_MAR ADRSPC_SLOT_1 + 0x509
#define SLOT1_BSI_MNR ADRSPC_SLOT_1 + 0x50B
#define SLOT2_BSI_MAR ADRSPC_SLOT_2 + 0x509
#define SLOT2_BSI_MNR ADRSPC_SLOT_2 + 0x50B
#define SLOT3_BSI_MAR ADRSPC_SLOT_3 + 0x509
#define SLOT3_BSI_MNR ADRSPC_SLOT_3 + 0x50B


|
|
|	_fim_ichain_jump_exit
|
fim_ichain_jump_exit:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception


 |
 |
 |	FDDI Interface module
 |	_fim_slot1 Low Slot IRQ
 |
 | void fim_slot1 (void)
 | void fim_slot1_jump_entry (void)
 |
	.globl _fim_slot1
	.globl _fim_slot1_jump_entry
_fim_slot1:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_fim_slot1_jump_entry:
	movb	SLOT1_BSI_MAR, d3	| Get MAR
	andb	SLOT1_BSI_MNR, d3	| Mask it
	jeq	fim_ichain_jump_exit

	| ### Got bsi FDDI Slot 1 interrupt:
	movl	HWIDB_SLOT1A,sp@- 	| Push IDB argument for C routines
	movl	 d3,sp@- 		| Push MAR argument for C routines
	jsr	_bsi_interrupt	        | Call interrupt routine

fim_slot1_jump_chain:
	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT1A, a0
	jmp	a0@

 |
 |	FDDI Interface module
 |	_fim_slot2 Low Slot IRQ
 |
 | void fim_slot2 (void)
 | void fim_slot2_jump_entry (void)
 |
	.globl _fim_slot2
	.globl _fim_slot2_jump_entry
_fim_slot2:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_fim_slot2_jump_entry:
	movb	SLOT2_BSI_MAR, d3	| Get MAR
	andb	SLOT2_BSI_MNR, d3	| Mask it
	jeq	fim_ichain_jump_exit

	| ### Got bsi FDDI Slot 2 interrupt:
	movl	HWIDB_SLOT2A,sp@- 	| Push IDB argument for C routines
	movl	 d3,sp@- 		| Push MAR argument for C routines
	jsr	_bsi_interrupt	        | Call interrupt routine

fim_slot2_jump_chain:
	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT2A, a0
	jmp	a0@

 |
 |	FDDI Interface module
 |	_fim_slot3 Low Slot IRQ
 |
 | void fim_slot3 (void)
 | void fim_slot3_jump_entry (void)
 |
	.globl _fim_slot3
	.globl _fim_slot3_jump_entry
_fim_slot3:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

_fim_slot3_jump_entry:
	movb	SLOT3_BSI_MAR, d3	| Get MAR
	andb	SLOT3_BSI_MNR, d3	| Mask it
	jeq	fim_ichain_jump_exit

	| ### Got bsi FDDI Slot 3 interrupt:
	movl	HWIDB_SLOT3A,sp@- 	| Push IDB argument for C routines
	movl	 d3,sp@- 		| Push MAR argument for C routines
	jsr	_bsi_interrupt	        | Call interrupt routine

fim_slot3_jump_chain:
	movl	_l3stk,sp
	movl	ICHAIN_JUMP_SLOT3A, a0
	jmp	a0@

|
 |
 |	FDDI Interface module
 |	_cam_slot1 Low Slot IRQ
 |
 | void cam_slot1 (void)
 |
	.globl _cam_slot1
_cam_slot1:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

	| ### Got bsi FDDI Slot 1 interrupt:
	movl	HWIDB_SLOT1A,sp@- 	| Push IDB argument for C routines
	jsr	_cam_interrupt		| Call interrupt routine

	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception

 |
 |	FDDI Interface module
 |	_cam_slot2 Low Slot IRQ
 |
 | void cam_slot2 (void)
 |
	.globl _cam_slot2
_cam_slot2:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

	| ### Got bsi FDDI Slot 2 interrupt:
	movl	HWIDB_SLOT2A,sp@- 	| Push IDB argument for C routines
	jsr	_cam_interrupt		| Call interrupt routine

	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception

 |
 |	FDDI Interface module
 |	_cam_slot3 Low Slot IRQ
 |
 | void cam_slot3 (void)
 |
	.globl _cam_slot3
_cam_slot3:
	moveml	REGISTERS,sp@-		| Save temp registers on process stack
	movl	sp,_l3sp		| Save current stack pointer
	movl	_l3stk,sp		| Get new stack pointer

	| ### Got bsi FDDI Slot 3 interrupt:
	movl	HWIDB_SLOT3A,sp@- 	| Push IDB argument for C routines
	jsr	_cam_interrupt		| Call interrupt routine

	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception

|^L
|
| Refresh (Level 7) Exception, used only if c4000 fddi present
|
| void rt_timer_nmi_wrapper (void)
| void orig_nmi_wrapper (void)
|
#define NMI_REGS        d0-d1/a0-a1

        .globl _rt_timer_nmi_wrapper
        .globl _orig_nmi_wrapper

        .even
        .text
_rt_timer_nmi_wrapper:
        movl    sp,_l7sp                | save current stack pointer
        movl    _l7stk,sp               | get new stack

        moveml  NMI_REGS,sp@-           | save registers not saved by C code
        movb    ADRSPC_TCONTROL,d0      | read timer control (ack)

        jsr     _rt_timer_int_hand      | call real time timers int handler
        addql   #1,_l7cnt               | count interrupt

        moveml  sp@+,NMI_REGS           | restore registers
        movl    _l7sp,sp                | restore old stack

        movl    _orig_nmi_wrapper,sp@-  | push the NMI handler address
        rts                             | go run the original NMI code


