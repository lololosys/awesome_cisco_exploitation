| $Id: setjmp.S,v 3.2 1995/11/17 18:48:25 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/rommon/src-68/setjmp.S,v $
|------------------------------------------------------------------
| setjmp, longjmp for the Moto 68k foamily
|
| November 1994, Rob Clevenger
|
| Copyright (c) 1995 by cisco Systems, Inc.
| All rights reserved.
| 
| Longer description here.
|------------------------------------------------------------------
| $Log: setjmp.S,v $
| Revision 3.2  1995/11/17  18:48:25  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  13:09:56  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  22:42:17  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|

|
| setjmp, longjmp
| 
| 	longjmp(a, v)
| causes a "return(v)" from the
| last call to
| 
| 	setjmp(v)
| by restoring all the registers and
| adjusting the stack
| 
| jmp_buf is set up as:
| 
| 	_________________
| 	| 	pc	| 
| 	-----------------
| 	| 	d2	| 
| 	-----------------
| 	| 	...	| 
| 	-----------------
| 	| 	d7	| 
| 	-----------------
| 	| 	a2	| 
| 	-----------------
| 	| 	...	| 
| 	-----------------
| 	| 	a7	| 
| 	-----------------

#define ASMINCLUDE
	.text
	.even
	.globl _setjmp, _longjmp

_setjmp:
	movl	sp@(4),a0	| pointer to jmp_buf
	movl	sp@,a0@		| pc
	movml	d2-d7/a2-a7,a0@(4)
	clrl	d0		| return 0
	rts

_longjmp:
	movl	sp@(4),a0	| pointer to jmp_buf
	movl	sp@(8),d0	| value returned
	movml	a0@(4),d2-d7/a2-a7 | restore d2-d7, a2-a7
	movl	a0@,sp@ 	| restore pc of call to setjmp to stack
	rts

| end of module
