/* $Id: asm_4k_monitor.S,v 3.1.2.1 1996/06/24 21:25:24 smackie Exp $
 * $Source: /release/112/cvs/Xsys/asm-mips/asm_4k_monitor.S,v $
 * ------------------------------------------------------------------
 * asm_4k_monitor.S - Assembler routines for the Rom Monitor interface
 *
 * June 1996, Scott Mackie
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 * ------------------------------------------------------------------
 *
 * $Log: asm_4k_monitor.S,v $
 * Revision 3.1.2.1  1996/06/24  21:25:24  smackie
 * Fix the assumption that the R4600 and 68K platforms always have a
 * "classic" Rom Monitor. (CSCdi61149)
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/21  18:04:23  smackie
 * Add placeholder for EMT cleanup
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE
#include "target_cisco.h"

/*
 * emt_call 
 * ROM Monitor system call
 * Note : We pass parameters in registers. This limits the total number
 *        to four (a0..a3). Also, the ROM Monitor syscall handler may
 *	  not support branch delay slot, so ensure that the syscall 
 *	  instruction is not in one.
 */

 #
 # int emt_call (int emt_type, [,arg1[,arg2,[arg3]]])
 #
FRAME(emt_call,sp,0,ra)
	.set	noreorder

	syscall			# Take a syscall trap. Arguments are allready
				# in registers a0..a3
	nop			# A bit paranoid !!

	j	ra		# return
	nop			# bd slot

	.set	reorder
ENDFRAME(emt_call)
