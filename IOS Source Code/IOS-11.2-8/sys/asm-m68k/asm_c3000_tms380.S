/* $Id: asm_c3000_tms380.S,v 3.2 1995/11/17 08:39:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c3000_tms380.S,v $
 *------------------------------------------------------------------
 * Assembly wrapper support for c3000 tms380
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c3000_tms380.S,v $
| Revision 3.2  1995/11/17  08:39:36  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:25  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:37  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c3000.h"


 |
 |	level4_pan_r - Level 4 interrupt wrapper for pan token ring
 |
 | void level4_pan_r (void)
 |
	.globl _level4_pan_r
_level4_pan_r:
	| ### Got Tokenring unit 0 interrupt:
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	jsr	_tms380_interrupt	| Call interrupt routine
	addql	#4,sp			| adjust stack
	rts				| bye bye
