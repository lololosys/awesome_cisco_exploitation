/* $Id: setjmp_mips.S,v 3.2 1995/11/17 18:45:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k/setjmp_mips.S,v $
 *------------------------------------------------------------------
 * setjmp_mips.S
 *
 * April 1993, Michael Beesley
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Set/Long jumps for ORION/R4000 platforms.
 *------------------------------------------------------------------
 * $Log: setjmp_mips.S,v $
| Revision 3.2  1995/11/17  18:45:05  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  13:07:58  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  22:40:05  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */


#define ASMINCLUDE

#include "cpu_mips.h"

/*
 * Note : The MIPS/R4000 Set/Long jumps use the same jmp_buf as 68K platforms
 * allthough it does not require all the elements in this array. The format
 * of the jmp_buf is outlined below
 *
 *      ----------------
 * 	| 	PC	|    jmp_buf[0]
 *  	-----------------
 * 	| 	SP	|           [1]
 * 	-----------------
 *  	| 	S0	|           [2]
 * 	-----------------
 * 	| 	S1	|           [3]
 * 	-----------------
 * 	| 	S2	|           [4]
 * 	-----------------
 * 	| 	...	|           ...
 * 	-----------------
 * 	| 	S8	|           [10]
 * 	-----------------
 *
 *   jmp_buf[11] .. jmp_buf[12] are not used
 *   The jmp_buf will have to be expanded to 64bit longs when
 *   64 bit mode is supported.
 */

	.text
	.align 4
/*
 * int setjmp(ptr)
 *	jmp_buf *ptr;
 *
 * Save the current context at ptr and return 0. ptr is in reg a0
 */
FRAME(setjmp, sp, 0, ra)
	sw	ra,(a0)           # save return address (PC) [0] 
	sw      sp,4(a0)          # save stack pointer       [1]
	sw      s0,8(a0)          # save reg s0              [2]
	sw      s1,12(a0)         # save reg s1              [3]
	sw      s2,16(a0)         # save reg s2              [4]
	sw      s3,20(a0)         # save reg s3              [5]
	sw      s4,24(a0)         # save reg s4              [6]
	sw      s5,28(a0)         # save reg s5              [7]
	sw      s6,32(a0)         # save reg s6              [8]
	sw      s7,36(a0)         # save reg s7              [9]
	sw	s8,40(a0)	  # save reg s8		     [10]
	move    v0,zero           # return 0 
	j       ra
ENDFRAME(setjmp)

/*
 * void longjmp(ptr, v)
 *	jmp_buf *ptr;
 *	int v;
 *
 * Restore the context saved in ptr and return v to caller of setjmp()
 */
FRAME(longjmp, sp, 0, ra)
	lw	ra,(a0)           # load return address (PC) [0] 
	lw      sp,4(a0)          # load stack pointer       [1]
	lw      s0,8(a0)          # load reg s0              [2]
	lw      s1,12(a0)         # load reg s1              [3]
	lw      s2,16(a0)         # load reg s2              [4]
	lw      s3,20(a0)         # load reg s3              [5]
	lw      s4,24(a0)         # load reg s4              [6]
	lw      s5,28(a0)         # load reg s5              [7]
	lw      s6,32(a0)         # load reg s6              [8]
	lw      s7,36(a0)         # load reg s7              [9]
	lw	s8,40(a0)	  # load reg s8		     [10]
	move    v0,a1             # return v 
	j       ra
ENDFRAME(longjmp)

/* End of Module */
