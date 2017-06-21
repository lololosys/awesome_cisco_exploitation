| $Id: start.S,v 1.1.68.1 1996/06/16 21:19:00 vnguyen Exp $
| $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/start.S,v $
|------------------------------------------------------------------
| start.S -- Brasil specific monitor assembly routines.
|
| October 1995, Joe Turner
|
| Copyright (c) 1995-1997 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: start.S,v $
| Revision 1.1.68.1  1996/06/16  21:19:00  vnguyen
| Finally committing Brasil to California_branch
|
| Revision 1.1  1995/12/03  21:18:45  jturner
| Placeholder for Brasil.
|
|------------------------------------------------------------------
| $Endlog$
|

#define ASMINCLUDE
 
#include "pcmap.h"
#include "as5200_pcmap.h"
#include "mon_defs.h"
 
|
| Exception table
|
        .text
        .even
        .globl _evec, _evece
        .globl __start

__start:
_evec:  .long   ADRSPC_BAD_ADDR	| put bad addr here, sp loaded later
	.long	__xstart	| pc
	.long	__buserror	| bus error
	.long	__addresserr	| address error
	.long	__illinstru	| illegal instruction
	.long	__zerodiv	| zero divide
	.long	__chkinstru	| chk instruction
	.long	__trpv		| trapv instruction
	.long	__privviol	| privilege violation
	.long	__trace		| trace
	.long	__l1010em	| line 1010 emulator
	.long	__l1111em	| line 1111 emulator
	.long	__hbreak	| hardware breakpoint
	.long	__unas34	| unassigned, reserved
	.long	__unas38	| unassigned, reserved
	.long	__unintvec	| uninitialized int vector
	.long	__unas40	| unassigned, reserved
	.long	__unas44	| unassigned, reserved
	.long	__unas48	| unassigned, reserved
	.long	__unas4c	| unassigned, reserved
	.long	__unas50	| unassigned, reserved
	.long	__unas54	| unassigned, reserved
	.long	__unas58	| unassigned, reserved
	.long	__unas5c	| unassigned, reserved
	.long	__spurint	| spurious interrupt
	.long	__avect1	| autovector 1
	.long	__avect2	| autovector 2
	.long	__avect3	| autovector 3
	.long	__avect4	| autovector 4 (sid int)
	.long	__avect5	| autovector 5
	.long	__avect6	| autovector 6
	.long	__avect7	| autovector 7
	.long	__trap0		| instruction trap 0
	.long	__trap1		| instruction trap 1 - breakpoint
	.long	__trap2		| instruction trap 2
	.long	__trap3		| instruction trap 3
	.long	__trap4		| instruction trap 4
	.long	__trap5		| instruction trap 5
	.long	__trap6		| instruction trap 6
	.long	__trap7		| instruction trap 7
	.long	__trap8		| instruction trap 8
	.long	__trap9		| instruction trap 9
	.long	__trap10	| instruction trap 10
	.long	__trap11	| instruction trap 11
	.long	__trap12	| instruction trap 12
	.long	__trap13	| instruction trap 13
	.long	__trap14	| instruction trap 14
	.long	__trap15	| instruction trap 15 - emt call
        .long   __reserved      | unassigned reserved (vector 48)
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
        .long   __reserved      | unassigned reserved
 
|
| our peripheral vectored interrupts, vector 64 to 255
|
 
| 64-79
        .long   __xrandomx |__pit_int
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 80-95
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 96-111
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 112-127
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 128-143
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 144-159
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 160-175
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 176-191
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 192-207
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 208-223
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 224-239
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
 
| 240-255
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx
        .long   __xrandomx              | vector 255
_evece:	.long	0			| place holder for the assembler

/*
 * vector to exception handling routine
 *
 * Care must be taken to stack things up as in the 
 * cpu_context structure in src-68/context.h
 */
__buserror:
__addresserr:
__illinstru:
__zerodiv:
__chkinstru:
__trpv:
__privviol:
__l1010em:
__l1111em:
__unas34:
__unas38:
__unintvec:
__unas40:
__unas44:
__unas48:
__unas4c:
__unas50:
__unas54:
__unas58:
__unas5c:
__spurint:
__avect1:
__avect2:
__avect3:
__avect4:
__avect5:
__avect6:
__trap2:
__trap3:
__trap4:
__trap5:
__trap6:
__trap7:
__trap8:
__trap9:
__trap10:
__trap11:
__trap12:
__trap13:
__trap14:
__reserved:
__xrandomx:     | exception stack frame is already on stack
        movml   d0-d7/a0-a7,sp@- | save entire CPU context
        movc    vbr,d1           | get the current contents
        movl    d1,sp@-          | and stash for later
        jsr     _exception
        movl    sp@+,d1          | pick up monitor vbr
        movc    d1,vbr           | and use it
        bra     __retint

/*
 * This is for support of the break/watchpoint mechanism
 * this includes:
 * trap 0 is the system image break key mechanism
 * trap 1 is our monitor software breakpoint mechanism
 * trace is the single step mechanism
 *
 * Care must be taken to stack things up as in the
 * cpu_context structure in src-68/context.h
 * Also, the order that state is saved and restored is
 * important.  Do not change this code without careful
 * consideration.
 */
        .even
        .lcomm brkpt_stk_sav,4          | storage area
        .lcomm sav_sys_state,4
        .text
__trap0:
__trap1:
__trace:
__hbreak:
        movw    _mon_sr_sav,sr          | restore mon status register
        movml   d0-d7/a0-a7,sp@-        | save entire CPU context
        movc    vbr,d1                  | get the current contents
        movl    d1,sp@-                 | and stash for later
        lea     _evec,a0                | use a known good vector base
        movc    a0,vbr                  | with our ISRs
        movl    _sys_state, sav_sys_state  | save the system state
        movl    sp, brkpt_stk_sav       | remember this stack location
        jbsr    _breakpt_isr
        /*
         * we should never return from this but rather to the
         * cont_brkpt routine below
         */

/*
 * continue from a software break point
 * for the cases where we can't just return from breakpt_isr
 */
	.globl _cont_brkpt
_cont_brkpt:
        movl    brkpt_stk_sav, sp          | pick up stack location
        movl    sav_sys_state, _sys_state  | restore this
        movl    sp@+, d1
        movc    d1,vbr                     | restore the vbr
__retint:
	movml	sp@+,d0-d7/a0-a7	   | pop our registers
	rte

/*
 * Level 7 NMI
 */
	.even
	.text
        .globl _clock
        .globl _hclock
 
__avect7:
        moveml d0-d1/a0-a1,sp@-	|save registers not saved by C code
        movb ADRSPC_TCONTROL,d0	|read timer control (ack)
        addql #4,_clock		|increment the msec clock by MON_TIMER_INTERVAL
        bcc  no_clock_carry	|branch if carry clear
        addql #1,_hclock	|we carried, so increment high clock
no_clock_carry:
        jsr _timer_int		|call C isr for this interrupt
        tstl    d0		|timer_int break key detect return code
        moveml sp@+,d0-d1/a0-a1	|restore registers
        bne    __hbreak		|detected a break key
        rte

/*
 * EMT call trap
 */
__trap15:
        jbsr    _emulate
        rte

/*
 * Change interrupt level to value passed on stack and return old level.
 * Expects an integer value - i.e., chgint(5)
 */          
	.even
	.text
	.globl _chgint
_chgint:
        movl    sp@(4),d0       | get new int level
        lsll    #8,d0           | shift left to match int mask in sr
        movw    sr,d1
        andw    #0xf8ff,d1      | clear current int level
        orl     d0,d1           | get new int level
        movw    sr,d0           | get old int level
        andl    #0x0700,d0
        lsrl    #8,d0           | return old int level
        movw    d1,sr           | store new int level
        movw    sr,_mon_sr_sav  | save new level for later
        rts

|
| Flicker lights
|
	.text
        .globl lights
lights:
lights1:
        movl #ADRSPC_CONTROL,a0         |control register address
        movl #CONTROL_RUNLAMP,d0        |lamp bit
        notw d0                         |make mask
        andw d0,a0@                     |disable lamp
        movl #0xFFFFFF,d1               |busy wait
lights2:
        dbra d1,lights2
        movl #CONTROL_RUNLAMP,d0        |lamp bit
        orw d0,a0@                      |now light it
        movl #0xFFFFFF,d1               |busy wait
lights3:
        dbra d1,lights3
        bra lights1

|
| Warm start code
|
 
        .text
        .globl __warm_start 
__warm_start:
        movl	#1000,d0		|loop here until rest of machine
warm_start_loop:
        subql	#1,d0 			|gets warmed up
        bgt	warm_start_loop 	|again?
	movl	#MONITOR_STACK,sp
        movw	#0x2700,sr		|disable interrupts
        movql	#0x7,d0			|cpu space
        movc	d0,sfc			|map source
        movc	d0,dfc			|and dest func code regs

        lea	_evec,a0		|get address of vectors
        movc	a0,vbr			|Set VBR

/*
 * Call our init routine
 */
	movl	#1,a3			|set a3 != EXC_RESET
        jmp	_init			|initialize, setup memory

|
| Startup code
|
 
        .text
        .globl __xstart, initdone
__xstart:
        movl	#1000,d0		|loop here until rest of machine
loop:   subql	#1,d0 			|gets warmed up
        bgt	loop 			|again?
	movl	#MONITOR_STACK,sp
        movw	#0x2700,sr		|disable interrupts
        movql	#0x7,d0			|cpu space
        movc	d0,sfc			|map source
        movc	d0,dfc			|and dest func code regs

        lea	_evec,a0		|get address of vectors
        movc	a0,vbr			|Set VBR

/*
 * Call our init routine
 */
	movl	#0,a3			|clear a3 to signal EXC_RESET
	movl	#ADRSPC_BRASIL_TDM_PLL, a0 | Set PLL free running clock
	movw	#0x0027,a0@		|on cold start only.
        jmp	_init			|initialize, setup memory, etc.
initdone:				|(init returns here)

/*
 * Note this monitor does not use main.o
 * All initialization is done in init() and the monitor
 * is invoked from brkpt_isr() (trap #1)
 */
        movc    vbr,d1			| and initialize
        movl    d1,_mon_vbr_sav 	| this
        movw    sr,_mon_sr_sav		| and this for later use
        clrl    sp@-			| zero first location on stack
        movml   sp@,d1-d7/a0-a6		| zero all regs except sp and d0
        trap    #1			| this gets us into the monitor
        bra     initdone		| d0 is still valid from _init

|
| Enable/Disable caches on 68020 and 68030 processors:
|
        .globl _setcacr
_setcacr:
        movc cacr,d0                    |return the current contents
        movl sp@(4),d1                  |get the new cacr value from stack
        movc d1,cacr                    |put it into the cacr
        rts
 
|
| return the current contents of the CACR
|
        .globl _getcacr
_getcacr:
        movc cacr,d0                    |return the current contents
        rts

/*
 * for user reset command
 */
	.globl _do_reset
_do_reset:
	jbsr	_timer_disable		| disable the timer
	movw	#0x2700,sr		| disable interrupts
        jsr	_flush			| empty UART
        clrl d0
        movc d0,cacr
        movw #CLEAR_CACHES,d0
        movc d0,cacr                    |clear caches
	reset				| reset the peripherals
	jmp	__warm_start		| now start over


/*
 * set the vector base register (VBR) to the value passed
 * on the stack
 * return the current contents
 */
	.globl _setvbr
_setvbr:
        movl sp@(4),d1
        movc vbr,d0                     | return the current contents
        movc d1,vbr
        movl d1,_mon_vbr_sav            | remember this
	rts

	.globl _getvbr
_getvbr:
	movc vbr,d0			|return the       current contents
	rts

/*
 * Storage for the monitor context for launch
 */
        .lcomm ret_addr,4
        .lcomm save_regs,48
 
/*
 * launch : Launch a loaded image
 * void launch (code, plb, pib, entry)
 */
        .globl  _sys_state
        .globl  _sys_dirty
        .text
        .even
        .globl _launch
_launch:
        movl    #save_regs+48,a0        | addr of one long past end
        movml   d2-d7/a2-a7,a0@-        | save monitor context
                                        | sierra also saves the status reg
 
        movl    #ADRSPC_RAM,d1          | new VBR value
        movc    vbr,d0                  | get the current contents
        movc    d1,vbr
        movl    d0,_mon_vbr_sav         | and save the old
 
        movw    sr,_mon_sr_sav          | save our status register
        movl    sp@(16),a0              | pick up our entry point
 
        movl    #SYSTEM_RUNNING,_sys_state
        movb    #1, _sys_dirty          | set dirty flag
 
        /*
        ** sierra changes to the system SP here
        ** We allow the system to run on the monitor stack
        ** but to avoid having to copy our params on the
        ** stack again, we save our RA and back up the stack.
        */
        movl    sp@+,ret_addr           | save our return address too
 
        jbsr    a0@                     | invoke system code in RAM
 
        /*
        ** save the return code from the system image (in d0)
        ** if we make calls here
        */
        movl    d0,sp@-                 | save return code
        movw    #0x2700,sr              | disable interrupts
        jbsr    _reset_io               | quiet all of the net ifaces
        jbsr    _re_init_con            | re-initialize the monitor console
        movw    _mon_sr_sav,sr          | restore our status register
        movl    _mon_vbr_sav,d1         | restore monitor VBR
        movc    d1,vbr
        movl    sp@+,d0                 | restore our return code
 
        movl    #save_regs,a0           | addr of start
        movml   a0@+,d2-d7/a2-a7        | restore monitor context
        movl    ret_addr,sp@            | replace our return address
 
        movl    #MONITOR_RUNNING,_sys_state
        rts

/* End of Module */
