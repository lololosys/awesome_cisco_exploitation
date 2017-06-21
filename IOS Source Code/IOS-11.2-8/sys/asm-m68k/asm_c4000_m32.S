/* $Id: asm_c4000_m32.S,v 3.3.12.1 1996/06/16 21:10:59 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/asm-m68k/asm_c4000_m32.S,v $
 *------------------------------------------------------------------
 * Assembly wrappers for the MCN NIM which has a Munich32 
 * HDLC controller on board. 
 *
 * December 1993, Michael Hubenthal
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c4000_m32.S,v $
| Revision 3.3.12.1  1996/06/16  21:10:59  vnguyen
| Finally committing Brasil to California_branch
|
| Revision 3.3  1996/02/15  05:34:05  smackie
| A movql should only be used to move to a data register. Moving to
| an address register or implicit address has to be done via an
| ordinary movl instruction. Not all assemblers are as forgiving as
| the 95q1 release. (CSCdi49069)
|
| Revision 3.2  1995/11/17  08:39:48  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:36  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:50  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c4000.h"


| 
|  define the address of the Munich32 interrupt
|  status and clear register for each slot
|  
#define SLOT0_M32_INT_REG ADRSPC_SLOT_1 + 0x44
#define SLOT1_M32_INT_REG ADRSPC_SLOT_2 + 0x44
#define SLOT2_M32_INT_REG ADRSPC_SLOT_3 + 0x44

|------------------------------------------------------ 
|  interrupt wrappers for the Munich32 HDLC controller.
|  one wrapper per slot.
|------------------------------------------------------

|
|       _m32im_slot0 Low Slot IRQ
|
        .globl _m32im_slot0
_m32im_slot0:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
        movl    sp,_l3sp                | Save current stack pointer
        movl    _l3stk,sp               | Get new stack pointer

        | Got Munich32 HDLC controller Slot 0 interrupt:
        movw    #2,SLOT0_M32_INT_REG    | Clear interrupt bit
        movl   #0,sp@-                  | Push the subunit number on the stack 
        movl   #0,sp@-                  | Push the slot number on the stack 

        jsr     _m32_interrupt          | jump to the m32 interrupt routine

        movl    _l3sp,sp                | restore old stack
        moveml  sp@+,REGISTERS          | restore registers from old stack
        addql   #1,_l3cnt               | count interrupt
        rte                             | return from exception



|
|       _m32im_slot1 Low Slot IRQ
|
        .globl _m32im_slot1
_m32im_slot1:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
        movl    sp,_l3sp                | Save current stack pointer
        movl    _l3stk,sp               | Get new stack pointer

        | Got Munich32 HDLC controller Slot 1 interrupt:
        movw    #2,SLOT1_M32_INT_REG    | Clear interrupt bit
        movl   #0,sp@-                  | Push the subunit number on the stack 
        movl   #1,sp@-                  | Push the slot number on the stack 

        jsr     _m32_interrupt          | jump to the m32 interrupt routine

        movl    _l3sp,sp                | restore old stack
        moveml  sp@+,REGISTERS          | restore registers from old stack
        addql   #1,_l3cnt               | count interrupt
        rte                             | return from exception


|
|       _m32im_slot2 Low Slot IRQ
|
        .globl _m32im_slot2
_m32im_slot2:
        moveml  REGISTERS,sp@-          | Save temp registers on process stack
        movl    sp,_l3sp                | Save current stack pointer
        movl    _l3stk,sp               | Get new stack pointer

        | Got Munich32 HDLC controller Slot 2 interrupt:
        movw    #2,SLOT2_M32_INT_REG    | Clear interrupt bit
        movl   #0,sp@-                  | Push the subunit number on the stack 
        movl   #2,sp@-                  | Push the slot number on the stack 

        jsr     _m32_interrupt          | jump to the m32 interrupt routine

        movl    _l3sp,sp                | restore old stack
        moveml  sp@+,REGISTERS          | restore registers from old stack
        addql   #1,_l3cnt               | count interrupt
        rte                             | return from exception

