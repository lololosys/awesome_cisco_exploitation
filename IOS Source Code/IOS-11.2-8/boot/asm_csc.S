| $Id: asm_csc.S,v 3.2.58.1 1996/05/10 23:16:42 gstovall Exp $
| $Source: /release/112/cvs/Xboot/asm_csc.S,v $
|------------------------------------------------------------------
| asm_csc.s -- assembler routines common to the 68000 based CSC1 and
|              the 68020 based CSC2.
|
| June 1989, Chris Shaker
|
| Copyright (c) 1989-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: asm_csc.S,v $
| Revision 3.2.58.1  1996/05/10  23:16:42  gstovall
| CSCdi56792:  Need to remove unused files
| Branch: California_branch
| The beast still exists, finish killing off the CS500, and while I am in
| there, nuke the even crustier CSC1.
|
| Revision 3.2  1995/11/17  07:40:50  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  09:03:56  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  19:11:27  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|

#include "../reason.h"

|
| btouchw, btouchr, stouchw, stouchr
|
| Very carefull byte/short read/write touch routines.  The instruction that 
| might bus error is followed by nops so that our bus error routine can
| continue to be naive, and not worry about fixing up the return
| address.
|

| btouchw(addr, val)
|    unsigned char *addr
|    int val

	.text
	.globl _btouchw
_btouchw:
	movl	sp@(4),a0	| addr
	movl	sp@(8),d0	| value
	movb	d0,a0@		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts

| stouchw(addr, val)
|    unsigned char *addr
|    int val

	.globl _stouchw
_stouchw:
	movl	sp@(4),a0	| addr
	movl	sp@(8),d0	| value
	movw	d0,a0@		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts

| int btouchr(addr)
|    unsigned char *addr

	.globl _btouchr
_btouchr:
	movl	sp@(4),a0	| addr
	movb	a0@,d0		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts

| int stouchr(addr)
|    unsigned char *addr

	.globl _stouchr
_stouchr:
	movl	sp@(4),a0	| addr
	movw	a0@,d0		| this might bus error, so...
	nop			| no matter where we land, we are OK
	nop
	nop
	nop
	nop
	rts
