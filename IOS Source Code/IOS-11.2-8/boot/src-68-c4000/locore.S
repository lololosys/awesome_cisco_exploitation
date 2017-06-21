| $Id: locore.S,v 3.1 1995/11/09 09:09:39 shaker Exp $
| $Source: /swtal/cherf.111/ios/boot/src-68-c4000/locore.S,v $
|------------------------------------------------------------------
| locore.s -- assembler assist routines for XX
|
| August 1986, Greg Satz
|
| Copyright (c) 1992-1993 by cisco Systems, Inc.
| All rights reserved.
| 
| Some of the exception handler wrappers, etc.  Most of the asm
| stuff has been moved to ../asm.S in the 9.1 release.
|------------------------------------------------------------------
| $Log: locore.S,v $
| Revision 3.1  1995/11/09  09:09:39  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  19:16:12  hampton
| Bump version numbers from 1.x to 2.x.
|
| Revision 1.1  1993/09/24 16:33:56  widmer
| Rename boot obj directories to include CPU type and product family
|
| Revision 1.4  1993/08/19  03:51:41  hampton
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
| Revision 1.3  1993/07/01  23:11:26  hampton
| Bring modular system to Aries 9.21(0.12) patch level.
|
| CSCdi11671:  Bogus instruction in asm routine setacu()
|
| Revision 1.2  1992/04/18  00:00:37  clev
| Added better support for shared memory parity errors and support for
| EMT_ENABLE_DCACHE (and disable).  Also, added support for break enable
| during boot and better decoding of bus errors.  Init was enhanced to
| flash an error number in the run LED should the power-on confidence
| test fail.  Init was also improved to initialize interrupt vectors up
| to 0x200.
|
| Revision 1.1  1992/04/15  02:11:43  clev
| Initial checkin for XX
|
| Revision 1.1  1992/03/09  08:38:06  hampton
| Initial conversion from RCS to CVS.
|
|------------------------------------------------------------------
| $Endlog$
|

#define	ASMINCLUDE

#include "../defs.h"
#include "../reason.h"


|
| included for backwards compatibility
|
	.globl _dcacheon
_dcacheon:
	movl #CACHE_D,d1		|turn on dcache, data burst, and write alloc
	bra chgcacr

	.globl _dcacheoff
_dcacheoff:
	movl #CLR_DCACHE,d1		|clear Dcache (kill any crud in cache)
chgcacr:
	movc cacr,d0			|get the current contents
	andl #CACHE_I,d0		|preserve instruction cache state
	orl d1,d0			|or in new stuff
	movc d0,cacr
	rts

|
| Set the Access Control Unit (ACU) registers
|
| setacu(reg,val)
|	reg - ACU register to be set (AC0 or AC1)
|	val - value to be written to ACU register
|
| Note that any non-zero value for reg will cause AC1 to be
| the target

	.globl _setacu
_setacu:
	|pmove instruction has limited effective address capabilities
	lea  sp@(8),a0		|set up a0 to point to val on stack
	lea  sp@(4),a1		|set up a1 to point to reg on stack
	tstl a1@		|which register?
	bne  setac1
	.long 0xf0110a00	|(pmove ac0,(a1)) save the current contents
	.long 0xf0100800	|(pmove (a0),ac0) write new value
	bra  retacu
setac1:
	.long 0xf0110e00	|(pmove ac1,(a1)) save the current contents
	.long 0xf0100c00	|(pmove (a0),ac1) write new value
retacu:
	movl  a1@,d0		|return the original (current) contents
	rts

|
| Flash lights to indicate error
| Since there may not be a stack the error number is passed in d0
|
	.globl lights
lights:
	movl d0,d1		|get number of flashes
	lsll #1,d1		|double the number (flash on, flash off)
	moveq #2,d3		|number of delimiter loops
	addl d3,d1		|add it into number of flashes
lights1:
	movl #0x60000,d2	|busy wait number
	cmpl d1,d3		|is this a delimit flash?
	ble lights2		|delimit loops do not flash
	eorl #CONTROL_RUNLAMP,ADRSPC_CONTROL |toggle lamp bit in ctl reg
lights2:
	subql #1,d2		|decrement busy wait number
	bne lights2		|loop until zero
	subql #1,d1		|decrement num flashes
	bne lights1		|next flash loop
	bra lights		|do this forever

|
| Bus and Address Error handler routines
|
	.text
	.even
	.globl buserror
	.globl _error
	.globl _buserrhandler
	.globl _catchbe
	.globl _pbuserror
	.globl _parberr
	.globl _plev6hand

_pbuserror:
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	jsr _parberr			|call handler
	moveml sp@+,d0-d1/a0-a1		|restore registers
_catchbe:
	andw #0xCEFF,sp@(10)		|clear rerun flags
	rte

_plev6hand:
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	jsr _lev6phandler		|call handler
	moveml sp@+,d0-d1/a0-a1		|restore registers
	rte

buserror:
	movl sp,_error			|save pointer to stack frame
	tstl _buserrhandler		|does someone want this
	beq buserror1S			|not if zero
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	movl _buserrhandler,a0		|put address of handler in a0
	jsr a0@				|call handler
	moveml sp@+,d0-d1/a0-a1		|restore registers
	andw #0xCEFF,sp@(10)		|clear rerun flags
	rte				|and return
buserror1S:
	andw #0xCEFF,sp@(10)		|clear rerun flags
	movw #0,sp@-			|padding
	movl #EXC_BUSERR,sp@-		|long reason
	jmp enter			|enter monitor

|
| Refresh (Level 7) Exception
|
	.globl _refresh
	.globl _abort
	.globl _abortstate
	.globl _clock
	.globl _hclock
	.globl refreshe

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


	.globl _unexp_int
_unexp_int:
	movl sp,_error			|save pointer to stack frame
	movw #0,sp@-			|padding
	movl #EXC_UNEXP_INT,sp@-	|long reason
	jmp enter			|enter monitor
