| $Id: locore.S,v 3.1 1995/11/09 09:09:14 shaker Exp $
| $Source: /swtal/cherf.111/ios/boot/src-68-c3000/locore.S,v $
|------------------------------------------------------------------
| locore.s -- assembler assist routines, tables, and data
| for the 68020 based IGS
|
| August 1986, Greg Satz
|
| Copyright (c) 1986-1994 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: locore.S,v $
| Revision 3.1  1995/11/09  09:09:14  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  19:15:40  hampton
| Bump version numbers from 1.x to 2.x.
|
| Revision 1.3  1994/10/30 21:59:17  abaindur
| CSCdi25240:  Need to support dfb on low end systems
| Flash Load Helper and Dual Flash Bank port.
|
| Revision 1.2  1994/06/28  18:41:13  sjay
| CSCdi20865:  Boot changes to support Brut II: more memory; jump to
| diagnostics
|
| Support for Brut II:
| 1.  Brut II has 8 MB DRAM.  Split it properly into IO & Processor (2/6)
|     by using same code as for Cancun.
| 2.  Support diagnostic code in high ROM by jumping to it if
|     "manufacturing mode" hardware status is pulled low during
|     initialization.
|
| Revision 1.2  1994/06/28  18:41:13  sjay
| CSCdi20865:  Boot changes to support Brut II: more memory; jump to
| diagnostics
|
| Support for Brut II:
| 1.  Brut II has 8 MB DRAM.  Split it properly into IO & Processor (2/6)
|     by using same code as for Cancun.
| 2.  Support diagnostic code in high ROM by jumping to it if
|     "manufacturing mode" hardware status is pulled low during
|     initialization.
|
| Revision 1.1  1993/09/24  16:33:28  widmer
| Rename boot obj directories to include CPU type and product family
|
| Revision 1.6  1993/08/19  03:51:25  hampton
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
| Revision 1.5  1993/07/01  23:10:57  hampton
| Bring modular system to Aries 9.21(0.12) patch level.
|
| CSCdi11671:  Bogus instruction in asm routine setacu()
|
| Revision 1.4  1992/09/23  17:12:52  clev
| Add improved cache command for pan.
| Add support for Bass.
|
| Revision 1.3  1992/08/31  20:38:01  dcoli
| Changes for Brut.
|
| Revision 1.2  1992/05/12  22:20:52  clev
| - Eliminate support for the physical configuration register on Pan.
|
| - Add support for the monitor command "o/r" for Pan and XX.
|
| - Break enable during boot on Pan.
|
| - Flash support added for Pan.
|
| Revision 1.1  1992/03/08  00:12:12  hampton
| Initial conversion from RCS to CVS.
|
|------------------------------------------------------------------
| $Endlog$
|

#define	ASMINCLUDE

#include "../defs.h"
#include "../reason.h"

	.globl _dcacheon
_dcacheon:
	movl #CLR_DCACHE,d0		|clear Dcache (kill any crud in cache)
	movc d0,cacr
	movb _cache,d0
	beq dcacheon1			|Leave both caches off if icache off
	movl #ENABLE_CACHES,d0
dcacheon1:
	movc d0,cacr
	rts

	.globl _dcacheoff
_dcacheoff:
	movb _cache,d0
	beq dcacheoff1			|Icache is off, too.
	movl #ENA_ICACHE,d0
	movc d0,cacr			|disable Dcache, enable Icache
	movl #CLR_DC_EIC,d0		|clear Dcache, enable Icache
	movc d0,cacr
	rts
dcacheoff1:
	movc d0,cacr			|disable both caches
	movl #CLEAR_CACHES,d0
	movc d0,cacr			|clear both caches
	rts

|
| Flicker lights
|
	.globl lights
lights:
lights1:
	movl #ADRSPC_CONTROL,a0		|control register address
	movl #CONTROL_RUNLAMP,d0	|lamp bit
	notw d0				|make mask
	andw d0,a0@			|disable lamp
	movl #0xFFFFFF,d1		|busy wait
lights2:
	dbra d1,lights2
	movl #CONTROL_RUNLAMP,d0	|lamp bit
	orw d0,a0@			|now light it
	movl #0xFFFFFF,d1		|busy wait
lights3:
	dbra d1,lights3
	bra lights1

|
| Bus error handler routines
|
	.text
	.even
	.globl buserror
	.globl _buserror
	.globl _error
	.globl _buserrhandler
	.globl _catchbe
_catchbe:
	andw #0xCEFF,sp@(10)		|clear rerun flags
	rte

_buserror:
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

|
| Refresh (Level 7) Exception for the BRUT project
|
	.globl _refreshe_brut
	.globl _refresh_brut

_refreshe_brut:
	moveml d0-d1/a0-a1,sp@-		|save registers not saved by C code
	movb ADRSPC_TCONTROL,d0		|read timer control (ack)
	addql #REFRESHTIME,_clock	|increment the millisecond clock
	bcc  refreshe0S_brut		|branch if carry clear
	addql #1,_hclock		|we carried, so increment high clock
refreshe0S_brut:
	jsr _refresh_brut		|call C isr for this interrupt
	moveml sp@+,d0-d1/a0-a1		|restore registers
	tstb _abort			|abort seen?
	bne refreshe1S_brut		|only if set
	rte				|otherwise return
refreshe1S_brut:
	cmpb #ABORT_INTERNAL,_abortstate |someone else wants it
	bne refreshe2S_brut		|no, perform normally
	rte				|then return
refreshe2S_brut:
	clrb _abort			|zero abort
	cmpb #ABORT_IGNORE,_abortstate  |should we just ignore it
	bne refreshe3S_brut		|no, hand off to monitor
	rte				|then return
refreshe3S_brut:
	movw #0,sp@-			|padding
	movl #EXC_ABORT,sp@-		|long reason
	jmp enter			|enter monitor

|
| Jump to diagnostic helper for init.c
|
| This routine exists so that init.c can jump to Brut diagnostic code
| without using DRAM (stack) or worrying about register allocation
| by the gcc compiler.
|
        .globl  brut_diag_jump

brut_diag_jump:
        movl    BRUT_DIAG_VECTOR_ADDR,a0  |indirect once     
        movl    a0@,a1                    |indirect twice
        jmp     a1@                       |jump to Brut diagnostics

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

