| $Id: asm_c4000.S,v 3.2.60.1 1996/03/18 18:50:50 gstovall Exp $
| $Source: /release/111/cvs/Xsys/asm-m68k/asm_c4000.S,v $
|------------------------------------------------------------------
| asm_c4000.S -- wrappers for c4000
|
| 4/92, Steve Southam and Marciano Pitargue
|
| Copyright (c) 1992-1996 by cisco Systems, Inc.
| All rights reserved.
| 
| wrapper routines for interrupt handlers for Dos XX
|------------------------------------------------------------------
| /*
| $Log: asm_c4000.S,v $
| Revision 3.2.60.1  1996/03/18  18:50:50  gstovall
| Branch: California_branch
| Elvis has left the building.  He headed out to California, and took the
| port ready changes with him.
|
| Revision 3.2.26.1  1996/03/09  05:03:44  hampton
| Slim down the kernel of the router.
| Branch: DeadKingOnAThrone_branch
|
| Revision 3.2  1995/11/17  08:39:38  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:26  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:39  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
| */
|

#include "asm_c4000.h"

|
|
| void nim_ichain_jump_exit (void)
|
	.globl _nim_ichain_jump_exit
_nim_ichain_jump_exit:
nim_ichain_jump_exit:
	movl	_l3sp,sp		| restore old stack
	moveml	sp@+,REGISTERS		| restore registers from old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
