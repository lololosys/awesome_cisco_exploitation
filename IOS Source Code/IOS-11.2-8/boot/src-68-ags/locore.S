| $Id: locore.S,v 3.1 1995/11/09 09:08:35 shaker Exp $
| $Source: /swtal/cherf.111/ios/boot/src-68-ags/locore.S,v $
|------------------------------------------------------------------
| locore.S -- assembler assist routines, tables, and data
| for the 68020 based CSC2
|
| August 1986, Greg Satz
|
| Copyright (c) 1986-1993 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: locore.S,v $
| Revision 3.1  1995/11/09  09:08:35  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  19:14:54  hampton
| Bump version numbers from 1.x to 2.x.
|
| Revision 1.1  1993/09/24 16:33:07  widmer
| Rename boot obj directories to include CPU type and product family
|
| Revision 1.2  1993/08/19  03:51:10  hampton
| Bring modular system to Aries 9.21(0.18) patch level.
|
| CSCdi09946:  Protect stack_whyreload() from bad boot_reason values
| 
| Doing ROM monitor changes required to support EMT_SET_CACR on all
| 68020/68030/68040 platforms. Share setcacr() and getcacr() on all of
| these platforms. Move all defines for bits in CACR registers to the
| pcmap.h files for each processor so that process level code does not
| have to have its own definition of the CACR bits.
| 
| Add better comments to explain memory sizing code in Merlot (IGS/TR)
| ROM monitor.
| 
| Add new EMT call, EMT_WRITE_BAD_MAIN_MEMORY_PARITY, used by the new
| 'test crash' menu in the system image. This EMT call writes a 32 bit
| word of information into main memory with bad parity.
| 
| Fix EMT number collision between Clean and 9.1 for new EMT call to make
| life easier for the 9.17 release.  Also, the numbers for all new EMT
| calls currently defined in the 9.21 release are now also defined in the
| 9.1 release as placeholders to prevent such EMT collision problems in
| the future.
|
| Revision 1.1  1992/03/08  00:12:07  hampton
| Initial conversion from RCS to CVS.
|
|------------------------------------------------------------------
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
	cmpb #CPU_CSC4, _cpu_type	|get CPU type; is this a csc/4?
	beq finish_catchbe		|yes, its a csc/4...
	andw #0xCEFF,sp@(10)		|clear rerun flags in 68020
finish_catchbe:
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
	cmpb #CPU_CSC4, _cpu_type	|get CPU type; is this a csc/4?
	beq fixup_pc			|yes, its a csc/4...
	andw #0xCEFF,sp@(10)		|clear rerun flags
	bra finish_buserr		|return from buserror
fixup_pc:
 	addl #12, sp@(2)		|68040 needs us to manually bump the
					|pc to recover from a buserror
finish_buserr:
	rte				|and return

buserror1S:
	cmpb #CPU_CSC4, _cpu_type	|get CPU type; is this a csc/4?
	beq finish_buserr1S		|yes, its a csc/4...
	andw #0xCEFF,sp@(10)		|clear rerun flags
finish_buserr1S:
	movw #0,sp@-			|padding
	movl #EXC_BUSERR,sp@-		|long reason
	jmp enter			|enter monitor

|
| Refresh (Level 7) Exception
|
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
	movw #0,sp@-			|padding
	movl #EXC_ABORT,sp@-		|long reason
	jmp enter			|enter monitor

