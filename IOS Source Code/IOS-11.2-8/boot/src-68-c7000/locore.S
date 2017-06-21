| $Id: locore.S,v 3.1 1995/11/09 09:10:19 shaker Exp $
| $Source: /swtal/cherf.111/ios/boot/src-68-c7000/locore.S,v $
|------------------------------------------------------------------
| locore.S -- assembler assist routines, tables, and data
| for the 68040 based RP1
|
| August 1986, Greg Satz
| April 1992, Gary Kramling
|
| Copyright (c) 1986-1993 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: locore.S,v $
| Revision 3.1  1995/11/09  09:10:19  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  19:17:05  hampton
| Bump version numbers from 1.x to 2.x.
|
| Revision 1.1  1993/09/24 16:34:28  widmer
| Rename boot obj directories to include CPU type and product family
|
| Revision 1.1  1993/01/30  01:11:55  whp4
| Add RP1 support.
|
|
| $Endlog$
|

#define	ASMINCLUDE

#include "../defs.h"
#include "../reason.h"
#include "../cpu.h"

|
| Flicker lights
|

	.globl lights

lights:
	movl #ADRSPC_CONTROL,a0		|control register address
	movl #CONTROL_RUNLAMP,d0	|disable running lamp
	notw d0				|create mask
	andw d0,a0@			|light off
lights1:
	movl #CONTROL_LAMP,d0		|lamp bit
	notw d0				|make mask
	andw d0,a0@			|disable lamp
	movl #0x7FFF,d1			|busy wait
lights2:
	dbra d1,lights2
	movl #CONTROL_LAMP,d0		|lamp bit
	orw d0,a0@			|now light it
	movl #0x7FFF,d1			|busy wait
lights3:
	dbra d1,lights3
	bra lights1

|
| Bus error handler routines
|
	.text
	.even
	.globl buserror
	.globl _error
	.globl _buserrhandler
	.globl _catchbe
	.globl _ebuserror
_catchbe:
	rte

_ebuserror:
buserror:
	movl sp,_error			|save pointer to stack frame
	tstl _buserrhandler		|does someone want this
	beq buserror1S			|not if zero
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	movl _buserrhandler,a0		|put address of handler in a0
	jsr a0@				|call handler
	moveml sp@+,d0-d1/a0-a1		|restore registers
 	addl #12, sp@(2)		|68040 needs us to manually bump the
					|pc to recover from a buserror
	rte				|and return

buserror1S:
	movw #0,sp@-			|padding
	movl #EXC_BUSERR,sp@-		|long reason
	jmp enter			|enter monitor

|
| Refresh (Level 7) Exception
|
	.globl	_pf_info
	.globl	refreshe
	.globl _refresh
	.globl _abort
	.globl _abortstate
	.globl _clock
	.globl _hclock

refreshe:
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	movb ADRSPC_TCONTROL,d0		|read timer control (ack)
	addql #REFRESHTIME,_clock	|increment the millisecond clock
	bcc  refreshe0S			|branch if carry clear
	addql #1,_hclock		|we carried, so increment high clock
refreshe0S:
	jsr _refresh			|call main routine
	moveml sp@+,d0-d1/a0-a1		|restore registers
	tstb _abort			|abort seen?
	bne refreshe1S			|only if set
	rte				|otherwise return
refreshe1S:
	cmpb #ABORT_INTERNAL,_abortstate |someone else wants it
	bne refreshe2S			|no, perform normally
	rte				|then return
refreshe2S:
	clrb _abort			|zero abort
	cmpb #ABORT_IGNORE,_abortstate |should we just ignore it
	bne refreshe3S			|no, hand off to monitor
	rte				|then return
refreshe3S:
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
        movl #_pf_info,a0		|grab addr of pf_info->pf_addr
        movl a0@,a0@(4)			|save pf_address for return
        movl #0,a0@			|zero so pwrfail handler isnt called
	moveml sp@+,d0-d1/a0-a1		|restore registers
	movw #0,sp@-			|padding
	movl #EXC_ABORT,sp@-		|long reason
	jmp enter			|enter monitor
