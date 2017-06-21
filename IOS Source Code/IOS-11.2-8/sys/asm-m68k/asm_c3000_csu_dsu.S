| $Id: asm_c3000_csu_dsu.S,v 3.2 1996/03/29 03:47:51 sagarwal Exp $
| $Source: /release/111/cvs/Xsys/asm-m68k/asm_c3000_csu_dsu.S,v $
|------------------------------------------------------------------
| Assembly interrupt wrapper support for integrated CSU/DSU
|
| March 1996, Shalabh Agarwal
|
| Copyright (c) 1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: asm_c3000_csu_dsu.S,v $
| Revision 3.2  1996/03/29  03:47:51  sagarwal
| CSCdi46172:  Add Boa support to 11.1 mainline
|
| Revision 3.1  1996/03/28  19:57:21  sagarwal
| Placeholder files for 2500 Derivative with integrated CSU/DSU
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_c3000.h"

 |
 |      level3_boa_csu_dsu_int - Level 3 vectored interrupt wrapper for the
 |      Boa, integrated CSU/DSU 2500
 |
 | void level3_boa_csu_dsu_int (void)
 |
        .globl _level3_boa_csu_dsu_int
_level3_boa_csu_dsu_int:
        moveml  REGISTERS,sp@-         | Save temp registers on process stack
        movl    sp,_l3sp               | Save process stack pointer
        movl    _l3stk,sp              | Switch context to interrupt level stack
        lea     _csu_dsu_int_array,a2  | Load pointer to array of dsu driver structs
        movl    a2@, sp@-              | Push WAN0 csu_dsu_instance argument
level3_boa_check_wan0:
        movb    ADRSPC_BOA_WAN0_MISC,d2 | Read CSU/DSU Miscellanous reg.
        eorb    #BOA_MISC_ALARM_MASK, d2    | Invert the masks for easier program flow
        btst    #BOA_ALM_MASK_BIT, d2   | Check if ALM interrupts enabled
        jeq     level3_boa_check_wan1   | if masked off, check RDY bit
        btst    #BOA_ALM_STATUS_BIT, d2 | Check if ALM signal HIGH
        jeq     level3_boa_check_wan1   | No? Go check RDY bit
        jsr     _csu_dsu_ALM_int        | Invoke ALM interrupt handler
level3_boa_check_wan1:
        addql   #4, sp                  | Adjust stack pointer back
        movl    a2@(4), sp@-            | Push WAN1 csu_dsu_instance argument
        movb    ADRSPC_BOA_WAN1_MISC, d2    | Read CSU/DSU Miscellanous reg.
        eorb    #BOA_MISC_ALARM_MASK, d2    | Invert the masks for easier program flow
        btst    #BOA_ALM_MASK_BIT, d2   | Check if ALM interrupts enabled
        jeq     level3_boa_exit         | if masked off, check RDY bit
        btst    #BOA_ALM_STATUS_BIT, d2 | Check if ALM signal HIGH
        jeq     level3_boa_exit         | No? Prepare to get out of handler
        jsr     _csu_dsu_ALM_int        | Invoke ALM interrupt handler
level3_boa_exit:
        movl    _l3sp, sp               | Restore process level stack
        moveml  sp@+, REGISTERS         | Restore temp register values
        addql   #1, _l3cnt              | count interrupt
        rte

