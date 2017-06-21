| $Id: asm_68_monitor.S,v 3.1.2.1 1996/06/24 21:25:18 smackie Exp $
| $Source: /release/112/cvs/Xsys/asm-m68k/asm_68_monitor.S,v $
|------------------------------------------------------------------
| asm_68_monitor.S - Assembler routines for the Rom Monitor interface
|
| June 1996, Scott Mackie
|
| Copyright (c) 1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| /*
| $Log: asm_68_monitor.S,v $
| Revision 3.1.2.1  1996/06/24  21:25:18  smackie
| Fix the assumption that the R4600 and 68K platforms always have a
| "classic" Rom Monitor. (CSCdi61149)
| Branch: California_branch
|
| Revision 3.1  1996/06/21  18:02:16  smackie
| Add placeholder for EMT cleanup
|
|------------------------------------------------------------------
| $Endlog$
| */
|

 |
 | emt_call
 | ROM monitor emulation trap support
 | 
 | int emt_call (traptype[,arg1[,arg2[,arg3]]])
 | 

	.globl _emt_call

_emt_call:
	link a6,#0
	movl a6@(20),sp@-
	movl a6@(16),sp@-
	movl a6@(12),sp@-
	movl a6@(8),sp@-
	trap #15
 |
 | The next instruction fixes
 | up the stack for us
 |
	unlk a6
	rts

