/* $Id: asm_c3000_bri.S,v 3.2 1995/11/17 08:39:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c3000_bri.S,v $
 *------------------------------------------------------------------
 * Assembly wrapper support for c3000 BRI
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: asm_c3000_bri.S,v $
| Revision 3.2  1995/11/17  08:39:29  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:19  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:29  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "asm_c3000.h"


 |
 |	level4_pan_b - Level 4 interrupt wrapper for pan ISDN basic rate
 |
 | void level4_pan_b (void)
 |
	.globl _level4_pan_b
_level4_pan_b:
	| ### Got Basic Rate interrupt:
	movl	a2@(4),sp@-		| Push IDB argument for C routines
	jsr     _bri_interrupt          | Call Interrupt routine
	addql	#4,sp			| adjust stack
	rts
