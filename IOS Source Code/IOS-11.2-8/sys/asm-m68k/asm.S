| $Id: asm.S,v 3.4.12.5 1996/06/24 21:25:17 smackie Exp $
| $Source: /release/112/cvs/Xsys/asm-m68k/asm.S,v $
|------------------------------------------------------------------
| asm.S -- Assembler routines common to the 68000, 68020 and 68040 processors.
|
| March 1989, Kirk Lougheed
|
| Copyright (c) 1986-1996 by cisco Systems, Inc.
| All rights reserved.
| 
| See asm1.s and asm2.s for 68000 (CSC1) and 68020 (CSC2) specific
| routines.
| 
| Registers a0, a1, d0, and d1 are always available.
| Any other registers must be preserved for the caller.
|------------------------------------------------------------------
| /*
| $Log: asm.S,v $
| Revision 3.4.12.5  1996/06/24  21:25:17  smackie
| Fix the assumption that the R4600 and 68K platforms always have a
| "classic" Rom Monitor. (CSCdi61149)
| Branch: California_branch
|
| Revision 3.4.12.4  1996/06/16  21:10:52  vnguyen
| Finally committing Brasil to California_branch
|
| Revision 3.4.12.3  1996/06/12  19:07:55  pst
| CSCdi60173:  Eliminate cisco definition of ntohl in favor of
| swap_xx_bits (partial commit part 1)
| Branch: California_branch
|
| Revision 3.4.12.2  1996/05/09  14:10:28  rbadri
| Branch: California_branch
| LANE, UNI3.1 features
|
| Revision 3.4.12.1.18.1  1996/04/27  06:32:51  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 3.4.12.1.8.1  1996/04/08  01:43:54  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 3.4.12.1  1996/03/18  18:50:43  gstovall
| Branch: California_branch
| Elvis has left the building.  He headed out to California, and took the
| port ready changes with him.
|
| Revision 3.3.12.4  1996/03/16  06:25:44  gstovall
| Branch: DeadKingOnAThrone_branch
| Make the king aware of V111_1_3.
|
| Revision 3.3.12.3  1996/03/09  05:03:37  hampton
| Slim down the kernel of the router.
| Branch: DeadKingOnAThrone_branch
|
| Revision 3.3.12.2  1996/03/07  08:27:47  mdb
| Branch: DeadKingOnAThrone_branch
| cisco and ANSI/POSIX libraries.
|
| Revision 3.3.12.1  1996/02/20  13:32:02  dstine
| Branch: DeadKingOnAThrone_branch
|         Sync from DeadKingOnAThrone_baseline_960122 to
|                   DeadKingOnAThrone_baseline_960213
|
| Revision 3.4  1996/02/15  05:34:03  smackie
| A movql should only be used to move to a data register. Moving to
| an address register or implicit address has to be done via an
| ordinary movl instruction. Not all assemblers are as forgiving as
| the 95q1 release. (CSCdi49069)
|
| Revision 3.3  1995/12/21  16:28:36  hampton
| Switch the 68K version of DELAY() to call the existing usecdelay
| routine which uses the timer chip.  For all other platforms, move a
| routine or two between files.  [CSCdi45965]
|
| Revision 3.2  1995/11/17  08:39:19  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:10  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:05:18  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
| */
|

#define ASMINCLUDE
#include "../../boot/cpu.h"
#include "../os/signal.h"

|
 | set_interrupt_level
 | set interrupt level to specified value, return current contents of status
 | register.
 |
 | ushort set_interrupt_level (int newlevel)
 |

	.text

	.globl _set_interrupt_level

_set_interrupt_level:
	movl	sp@(4),d0	| get new interrupt level
	lslw	#8,d0		| shift new level left 8 bits
	movw	sr,d1		| get current sr
	andw	#0xF8FF,d1	| clear current interrupt level
	orw	d0,d1		| "or" new level into sr bits
	movw	sr,d0		| return in d0 the current copy of sr
	movw	d1,sr		| set new interrupt level
	rts
|
 | 
 | reset_interrupt_level  - reset status register to value returned by 
 |                          {set,raise}_interrupt_level
 |
 | void reset_interrupt_level (ushort oldsr)
 |
	.globl _reset_interrupt_level

_reset_interrupt_level:
	movl	sp@(4),d0	| get old sr
	movw	d0,sr		| reset status register
	rts
|
 | 
 | get_interrupt_level
 | Return current interrupt level.  "level = get_interrupt_level();"
 |
 | ushort get_interrupt_level (void)
 | 
	.globl _get_interrupt_level

_get_interrupt_level:
	moveq   #0,d0		| clear all of d0
	movw	sr,d0		| copy status register to d0
	andw	#0x0700,d0	| flush other bits
	lsrw	#8,d0		| right justify the level
	rts
|
 | raise_interrupt_level - Raise processor level to specified, or keep it where
 | it is if already at that level. Usage: level = raise_interrupt_level(level).
 |
 | ushort raise_interrupt_level (ushort newlevel)
 |

	.globl	_raise_interrupt_level

_raise_interrupt_level:
	movl	sp@(4),d0	| get new interrupt level
	lslw	#8,d0		| shift new level left 8 bits
	movw	sr,d1		| get current sr
	andw	#0x0700,d1	| save only the priority level
	cmpw	d0,d1		| already that high?
	bcs	_set_interrupt_level	| no, raise the level
	movw	sr,d0		| yes, just get the current sr
	rts			| and we are done
|
 | swap_16bit_word
 | Byte swap routines for shorts
 |
 | ushort swap_16bit_word (ushort num)
 |

	.globl	_swap_16bit_word

_swap_16bit_word:
	movl	sp@(4),d0		| get word to be swapped
	rolw	#8,d0			| an 8-bit rotate is a byte swap
	rts


 | swap_32bit_word
 | Byte swap routines for longs - beware: *not* VAX byte/word order!
 |
 | ulong swap_32bit_word (ulong num)
 |
	.globl	_swap_32bit_word

_swap_32bit_word:
	movl	sp@(4),d0		| get long to be swapped
	rolw	#8,d0			| swap bytes 3 and 4 -> 1 2 4 3
	swap	d0			| swap words 1 and 2 -> 4 3 1 2
	rolw	#8,d0			| swap bytes 1 and 2 -> 4 3 2 1
	swap	d0			| swap words 1 and 2 -> 2 1 4 3
	rts				| all done

 | Byte swap routines for longs - this one is VAX byte/word order!
 |
 | ulong vaxorder (ulong num)
 |
 	.globl _vaxorder

_vaxorder:
	movl	sp@(4),d0		| get long to be swapped
	rolw	#8,d0			| swap bytes 3 and 4 -> 1 2 4 3
	swap	d0			| swap words 1 and 2 -> 4 3 1 2
	rolw	#8,d0			| swap bytes 1 and 2 -> 4 3 2 1
	rts				| all done
|
 | ipcrc
 | Return the 1s complement checksum of the word aligned buffer
 | at s, for n bytes.
 |
 | This is the Jacobson algorithm from RFC 1071.
 |
 | ushort ipcrc (ushort *s, int n)
 |	
	.globl	_ipcrc

_ipcrc:
	movl	sp@(4),a0		| get pointer to start of buffer
	movl	sp@(8),d1		| get byte count, leave in d1

	movl	d2,sp@-			| save d2 for scratch use

	movl	d1,d2
	andl	#3,d2			| d2 = count % 4
	bne	vanj_odd

	movl	d1,d2
	lsrl	#6,d1			| count/64 = # loop traversals
	andl	#0x3c,d2		| Then find fractions of a chunk
	negl	d2
	andb	#0xf,cc			| Clear X (extended carry flag)

	moveq	#0,d0			| zero initial checksum
	jmp	pc@(2$-.-2:b,d2)	| Jump into loop

1$:		| Begin inner loop...

	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry

2$:
	dbra	d1,1$			| (NB -- dbra does not affect X)

	movl	d0,d1			| Fold 32 bit sum to 16 bits
	swap	d1			| (NB -- swap does not affect X)
	addxw	d1,d0
	jcc	3$
	addw	#1,d0
3$:
	notw	d0
	andl	#0xffff,d0
	movl	sp@+,d2			| restore d2
	rts

vanj_odd:
 |
 | checksum with partial-longs
 |
| a0 points to buffer
| d1 is byte-count
| d2 is # extra bytes

	| prime d0 with last partial-long

	movl	a0,a1

	addl	d1,a1			| point past last byte
	subl	d2,a1			| point to last partial-long
	clrl	d0

	cmpb	#2,d2			| 2 extra bytes?
	bne	not2
	movw	a1@,d0			| pick up the last word
	bra	4$

not2:	cmpb	#1,d2			| 1 extra byte?
	bne	not1
	movb	a1@,d0			| pick up the last byte
	asll	#8,d0
	bra	4$

not1:	movw	a1@+,d0			| 3 extra bytes, pick up the last word
	asll	#8,d0
	orb	a1@,d0			|	...and byte
	asll	#8,d0

4$:
	movl	d1,d2
	lsrl	#6,d1			| count/64 = # loop traversals
	andl	#0x3c,d2		| Then find fractions of a chunk
	negl	d2
	andb	#0xf,cc			| Clear X (extended carry flag)

	jmp	pc@(6$-.-2:b,d2)	| Jump into loop

5$:		| Begin inner loop...

	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry
	movl	a0@+,d2			| Fetch 32-bit word
	addxl	d2,d0			|	Add word + previous carry

6$:
	dbra	d1,5$			| (NB -- dbra does not affect X)

	movl	d0,d1			| Fold 32 bit sum to 16 bits
	swap	d1			| (NB -- swap does not affect X)
	addxw	d1,d0
	jcc	7$
	addw	#1,d0
7$:
	notw	d0
	andl	#0xffff,d0
	movl	sp@+,d2			| restore d2
	rts

|
 | ipttl(ptr)
 |
 | Decrement IP TTL field and fixup checksum.
 | Returns TRUE if TTL exceeded, FALSE otherwise
 |
 | boolean ipttl (uchar *ptr)
 |
	.globl	_ipttl

_ipttl:
	movl	sp@(4),a0		| get pointer to start of packet
	cmpb	#1,a0@(8)		| check it
	bls	ipttl4			| jump if bad
	subqb   #1,a0@(8)		| decrement TTL field
	movw	a0@(10),d0		| get IP checksum
	addw	#0x100,d0		| update checksum
	bcc	ipttl2			| any carry?
	addqw	#1,d0			| yes, end-around
ipttl2:	cmpw	#0xFFFF,d0		| result now minus 0?
	bne	ipttl3			| no, ok.
	moveq	#0,d0			| yes, back to plus 0
ipttl3:	movw	d0,a0@(10)		| stash back in header
	moveq   #0,d0			| return FALSE, good TTL
	rts

ipttl4:	moveq   #1,d0			| return TRUE, bad TTL
	rts

|
 | suspend, resume
 |
 | Switch between scheduler and process context.  Since these routines
 | save the process registers (only), they must be in assembler for the
 | C code to be portable.
 |
 | void resume (void)
 | void suspend (void)
 |
	.globl _resume
	.globl _suspend
	.globl 	_schedflag
	
_resume:
	moveml	d2-d7/a2-a6,sp@-	| save scheduler registers a2-a6, d2-d7
	movl	sp,d0			| save sched value of sp
	movl	_forkx,a0		
	movl	a0@(4),a6		| user a6 = forkx->savedsp
	movl	d0,a0@(4)		| forkx->savedsp = sched a6
	moveml	a0@(8),#0x03CFC		| a2-a5, d2-d7 = forkx->reg
	unlk	a6			| fix up user stack
	movl	sp@+,a1			| Pick up PC to return to
	movw	#0,sp@-			| Set a format code of zero
	movl	a1,sp@-			| Set the return PC again
	movw	a0@(48),sp@-		| Set the desired SR
	movl	#0,_schedflag		| No longer in scheduler
	rte				| Now resume

_suspend:
	movl	#1,_schedflag		| Now entering scheduler
	link	a6,#0			| push user a6 on stack
	movl	a6,d0			| copy user a6 to safe place
	movl	_forkx,a0
	movl	a0@(4),sp		| sched sp = forkx->savedsp
	movl	d0,a0@(4)		| forkx->savedsp = user a6
	movw	sr,a0@(48)		| Save the SR value
	moveml	#0x3CFC,a0@(8)		| forkx->regs = user registers
	moveml	sp@+,d2-d7/a2-a6	| Restore registers
	rts				| return in scheduler context

|
/*
 * void process_forced_here (ulong routine_to_call)
 *
 * This routine is mainly a stack frame placeholder so that it is obvious how
 * a processes execution thread got from its normal code space to a completely
 * unrelated routine.  The stack will have been modified to transfer execution
 * to this routine either by the routine alter_suspended_process(), or by the
 * watchdog timer.  This routine saves the registers that C doesn't because it
 * might have been inserted into the execution stream at a point where its
 * parent routine wasn't expecting to call a subroutine, and thus may be
 * actively using these registers.
 *
 * This routine is responsible for cleaning its argument off of the stack,
 * because the "calling" routine doesn't know it is there.
 */

	.extern	signal_receive
	.globl _process_forced_here

_process_forced_here:
	subql	#4,sp			| Save space to install return address
	link	a6,#0			| Setup local frame
	moveml	d0-d1/a0-a2,sp@-	| Save registers that C does not

	movl	_forkx,a2		| Get current process structure
	movl	a2@(52),d0		| Get return address and store it on
	movl	d0,a6@(4)		| the stack.  Done first so that stack
					| traces are printed correctly.

	movl	a2@(56),a0		| Execute requested routine
	jsr	a0@

	moveml	sp@+,d0-d1/a0-a2	| Restore registers that C does not
	unlk	a6			| Toss the local frame
	rts				| Return

|
/*
 * void alter_suspended_process (sprocess *proc, ulong new_address)
 *
 * Change the execution address of a process.  This is used when
 * killing a process so the it has a chance to clean up before it is
 * completely destroyed.
 * 
 * This looks really weird because we are inserting a return address
 * between the frame pointer and the current return address.  Since the
 * called routine sets up the frame pointer, the stack needs to look like
 * a routine that called another routine before it set up its own frame.
 * This will all work out as the top frame and the new return address get
 * popped off when this process is rescheduled, and the first thing that
 * happens is that the inserted routine creates its own frame pointer.
 *
 * Note:
 *  	a0@(4) is proc->savedsp
 *
 * WARNING!!!
 *       This routine may only be called on a process that is blocked in
 * a scheduler routine.  This is the only way that the state of the stack
 * may be guaranteed.
 */

	.globl	_alter_suspended_process

_alter_suspended_process:
	link	a6,#0
	movl	a6@(8),a0	| Get pointer to process
	movl	a0@(4),a1	| Get frame pointer from process structure

	movl	a1@(4),d0	| Get old return address
	movl	d0,a0@(52)	| Install in process_structure
	movl	a6@(12),d0	| Get routine address to force
	movl	d0,a0@(56)	| Install in process structure
	movl	#_process_forced_here,d0
	movl	d0,a1@(4)	| Install new return address on stack

	unlk	a6
	rts


|
 | setjmp, longjmp
 | 
 |   int longjmp (jmp_buf *env, int val)
 | 	causes a "return(val)" from the last call to setjmp
 |	by restoring all the registers and adjusting the stack
 | 
 |   int setjmp (jmp_buf *env)
 |	set the jmp_buf up so a later call to longjmp will
 |	return here.
 |
 | jmp_buf is set up as:
 |       _________________
 |       |       pc      |
 |       -----------------
 |       |       d2      |
 |       -----------------
 |       |       ...     |
 |       -----------------
 |       |       d7      |
 |       -----------------
 |       |       a2      |
 |       -----------------
 |       |       ...     |
 |       -----------------
 |       |       a7      |
 |       -----------------
 | 
	.globl _setjmp

_setjmp:
	movl	sp@(4),a0		| pointer to jmp_buf
	movl	sp@,a0@			| pc
	moveml	#0xFCFC,a0@(4)		| d2-d7, a2-a7
	clrl	d0			| return 0
	rts

	.globl	_longjmp
	.globl	_longjmp_restore

| _longjmp is used by C routines to restore context, with parameters
| passed on the stack.

| _longjmp_restore is used by assembly language routines for which
| the state of the stack can not be guaranteed. d0 = value to return
| to the C routine; a0 -> the longjmp structure.

_longjmp:
	movl	sp@(4),a0		| pointer to jmp_buf
	movl	sp@(8),d0		| value to return
_longjmp_restore:
	moveml	a0@(4),#0xFCFC		| d2-d7, a2-a7
	movl	a0@,sp@			| restore pc of call to setjmp
	rts
|
 | "caller" routines
 |
 | Return pc of the subroutines caller.
 | current_pc() give the pc just after
 | the call to current_pc.
 |
 | ulong cpu_caller (void)
 | ulong current_pc (void)
 |
	.globl _cpu_caller

_cpu_caller:
	movl	a6,d0			| get frame register
	btst	#0,d0			| even address???
	bne	callra	        	| no, ca not do long word move
 	movl	a6@(4),d0		| get PC of routines return address
	subql	#8,d0			| back up the PC to callers address
	rts				| return PC in d0

callra:	moveq	#0,d0			| return obviously bogus PC
	rts

	.globl _current_pc
_current_pc:
	movl	sp@,d0			|Get the current PC
	rts



 | caller_frame - Return frame pointer of caller
 |
 | ulong caller_frame (void)
 |
	.globl	_caller_frame

_caller_frame:
	movl	a6,d0			| get callers FP
	rts				| And we are done

|
 | 
 | Interrupt routine dispatches
 | This code replaces the "reentrantSTK" macro
 |
 | void level1 (void)
 | void level2 (void)
 | void level3 (void)
 | void level4 (void)
 | void level5 (void)
 | void level6 (void)
 |
	.globl _level1
_level1:
	movl	sp,_l1sp		| save current stack pointer
	movl	_l1stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	movl	_lev1subr,a0		| get routine address
	jsr	a0@			| dispatch
	moveml	sp@+,#0x0303		| restore registers
	movl	_l1sp,sp		| restore old stack
	addql	#1,_l1cnt		| count interrupt
	rte				| return from exception

	.globl _level2
_level2:
	movl	sp,_l2sp		| save current stack pointer
	movl	_l2stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	movl	_lev2subr,a0		| get routine address
	jsr	a0@			| dispatch
	moveml	sp@+,#0x0303		| restore registers
	movl	_l2sp,sp		| restore old stack
	addql	#1,_l2cnt		| count interrupt
	rte				| return from exception

	.globl _level3
#if defined(CSC2) || defined(RP1)
_level3:
#ifndef RP1
	movw	#0x2400,sr              | Mask level 4 interrupts (TR, etc.)
#endif
	movl	sp,_l3sp		| save current stack pointer
	movl	_l3stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
        movw	#1,_mci_cbus_intflag	| set flag
	movl	_lev3subr,a0		| get routine address
	jsr	a0@			| dispatch
	movl    _lev3ssubr,d0           | get secondary address
	beq     _level3s		| no secondary interrupt address
	movl 	d0,a0			|
	jsr     a0@			| dispatch

_level3s:	
	moveml	sp@+,#0x0303		| restore registers
	movl	_l3sp,sp		| restore old stack
	clrw	_mci_cbus_intflag	| clear flag
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
#else CSC2 || RP1
_level3:
	movl	sp,_l3sp		| save current stack pointer
	movl	_l3stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	movl	_lev3subr,a0		| get routine address
	jsr	a0@			| dispatch
	moveml	sp@+,#0x0303		| restore registers
	movl	_l3sp,sp		| restore old stack
	addql	#1,_l3cnt		| count interrupt
	rte				| return from exception
#endif CSC2 || RP1

	.globl _level4

_level4:
	movl	sp,_l4sp		| save current stack pointer
	movl	_l4stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	movl	_lev4subr,a0		| get routine address
	jsr	a0@			| dispatch
	moveml	sp@+,#0x0303		| restore registers
	movl	_l4sp,sp		| restore old stack
	addql	#1,_l4cnt		| count interrupt
	rte				| return from exception

	.globl _level5
_level5:
	movl	sp,_l5sp		| save current stack pointer
	movl	_l5stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	movl	_lev5subr,a0		| get routine address
	jsr	a0@			| dispatch
	moveml	sp@+,#0x0303		| restore registers
	movl	_l5sp,sp		| restore old stack
	addql	#1,_l5cnt		| count interrupt
	rte				| return from exception

	.globl _level6
_level6:
	movl	sp,_l6sp		| save current stack pointer
	movl	_l6stk,sp		| get new stack
	moveml	#0xC0C0,sp@-		| save registers
	movl	_lev6subr,a0		| get routine address
	jsr	a0@			| dispatch
	moveml	sp@+,#0x0303		| restore registers
	movl	_l6sp,sp		| restore old stack
	addql	#1,_l6cnt		| count interrupt
	rte				| return from exception
|
| exception_handler - Handle non-interrupt exceptions
|
| This function is called when a 68020 exception occurs. It saves all
| of the registers in the exception_registers[] array, extracts the
| exception stack frame, and transfers control to the C exception
| handler for further processing.
|
| void exception_handler (void)
|

|
| Table of number of words in each type of exception frame.
|

exception_size:
	.word	4		| Type 0 (Normal fault)
	.word	4		| Type 1 (Throwaway interrupt frame)
	.word	6		| Type 2 (Instruction-related fault)
	.word	4		| Type 3
	.word	4		| Type 4
	.word	4		| Type 5
	.word	4		| Type 6
	.word	30		| Type 7 (68040 access error)
	.word	29		| Type 8 (68010 bus error)
	.word	10		| Type 9 (68020 coprocessor error)
	.word	16		| Type a (68020 short bus error)
	.word	46		| Type b (68020 long bus error)
	.word	12		| Type c (CPU32 bus error)
	.word	4		| Type d
	.word	4		| Type e
	.word	4		| Type f

.data
.even
exception_stack_guard:
	.long	-1
	.skip	1000		| Space for exception stack
exception_stack:
	.word	0		| A pad word
_exception_registers:
	.skip	72
_exception_frame:
	.skip	104

	.globl	_exception_registers
	.globl	_exception_frame
	.globl	_handle_exception

	.globl	_exception_handler

.text

_exception_handler:
	oriw	#0x700,sr	| Lock out interrupts
        moveml  d0-d7/a0-a6,_exception_registers | save registers

	lea     _exception_registers,a5 | get address of registers     
        movew   sp@,d1          | get status register          
        movew   d1,a5@(66)      | save sr		 		
	movel   sp@(2),a4       | save pc in a4 for later use  
        movel   a4,a5@(68)      | save pc in _exception_registers[]      

|
| figure out how many bytes in the stack frame
|
	movew   sp@(6),d0	| get 68020 exception format	
        movew   d0,d2           | make a copy of format word   
        andiw   #0xf000,d0      | mask off format type         
        rolw    #5,d0           | rotate into the low byte *2  
        lea     exception_size,a1   
        addw    d0,a1           | index into the table         
	movew   a1@,d0          | get number of words in frame 
        movew   d0,d3           | save it                      
	lea	_exception_frame,a1 | Get pointer to frame save area 
	movel	a4,a1@+		| Save the PC			

|
| compute exception number
|
	andl    #0xfff,d2   	| mask off vector offset	
	lsrw    #2,d2   	| divide by 4 to get vect num	
        movel   d2,a1@+         | save it                      

	movew	d0,a1@+		| Save the exception size	
	subql   #1,d0           | predecrement loop counter    

|
| copy the frame
|
save_frame_loop:
	movew  	sp@+,a1@+
	dbf     d0,save_frame_loop
|
| now that the stack has been cleaned,
| save the a7 in use at time of exception
|
        movel   a7,a5@(60)      | save a7                  
	lea	exception_stack,sp | Switch to exception handler stack 
	clrl	d0		| Get a zero
	movel	d0,a6		| Reset the frame pointer
        movel   d2,sp@-		| push exception num           
	jbsr	_handle_exception | Call the handler in C
	|jmp	_return_from_exception
|
| _return_from_exception - Restore frame context and return from exception
|
| The exception handler calls this routine to return back to the thread
| when the exception occured.
|
| void return_from_exception (void)
|

	.globl _return_from_exception

_return_from_exception:
        movel   _exception_registers+60,sp | get new stack pointer         
	lea	_exception_frame,a0 | Get saved frame 
        addql   #8,a0           | skip over pc, vector#              
        movew   a0@+,d0         | get # of words in cpu frame        
        addw    d0,a0           | point to end of data               
        addw    d0,a0           | point to end of data               
        movel   a0,a1                                                   
|                                                                       
| copy the stack frame                                                  
|
        subql   #1,d0                                                   

restore_frame_loop:
        movew   a1@-,sp@-                                               
        dbf     d0,restore_frame_loop
	moveml  _exception_registers,d0-d7/a0-a6
	rte  			| pop and go!
|
| _exception_jump - Jump to a routine on the exception thread stack
|
| This routine is used by the exception process to jump to a routine
| on the stack of the process which had the exception.
|
| void exception_jump (void fn(void));
|
	.globl	_exception_jump

_exception_jump:
	movel	sp@(4),a0	| Get address to jump to
	lea	_exception_registers,a1 | Get pointer to saved registers
	movel	a1@(60),sp	| Switch to process stack

	jmp	a0@		| Now continue wherever

|
| _rom_monitor_return - Handle return from old ROM monitor
|
| If we are running with an old ROM monitor, we modify the stack so
| that all exceptions return here. The stack has been set up as
| if we had set up a stack frame via a link instruction, so we must
| unlink the stack before returning

	.globl	__rom_monitor_return

__rom_monitor_return:
	unlk	a6		| Restore the stack
	rts			| Now Actually go back
|
/*
 * watchdog_timed_out    [Not callable from C.]
 *
 * Routine to fudge the stack of a running process.  This routine gets wedged
 * into the execution path when the clock interrupt code determines that the
 * current process has been running for too long.  The clock handler pulls the
 * original return address out of the exception frame and saves it in the
 * variable wd_save_return.  It then puts the address of this routine into the
 * exception frame, so that this code is called immediately upon return from the
 * clock interrupt.
 */
	.data
	.even
wd_save_return:
	.long 0

	.text
	.even
watchdog_timed_out:
	movl	a0,sp@-			| save a register
	movl	_forkx,a0
	movl	wd_save_return,a0@(52)	| Push the original execution address
	movl	#_signal_receive,a0@(56)| Push handler to be called
	movl	#SIGWDOG,a0@(60)	| forkx->pending_signal = SIGWDOG
	movl	sp@+,a0			| restore a register
	jmp	_process_forced_here



|
| Idle/interrupt loop sampler.  Added to intercept the clock interrupts
| to sample the mode where in for accurate CPU utilization nos.  Also
| supports alternate NMI handlers, bumps the system clock, and does profiling.
|
| void exception_handler_clock (void)
|
        .globl  _exception_handler_clock
	.globl	_current_count,	_int_count
	.globl	_nmi_handler, _clock_tick, _count_process_tick
	.globl	_profile_enabled, _profile_pc

_exception_handler_clock:
	moveml  d0-d1/a0-a1,sp@-	|save registers not saved by C code
idle_spl:
	movw	sp@(16),d0		|Get the SR of the trap to get lvl.
	andl	#0xf00,d0		
	tstl	d0			|Spl > 0?
	beq	idle_idle		|No, not intrpt. handler
	addl	#1,_int_count	
	bra	idle_out
idle_idle:
	tstl	_schedflag
	beq	task_running
	addl	#1,_current_count	|We are in the scheduler context
	bra	idle_out
task_running:
	jsr	_count_process_tick	|Count it against the current process
	tstl	d0			|Watchdog stop?
	beq	idle_out
	movel	sp@(18),wd_save_return	|Save current return address
	movel	#watchdog_timed_out,sp@(18)	|Fudge exception frame to cause
						|the process to return to the
						|watchdog handler
idle_out:
	jsr	_clock_tick		|Run the system clock

	tstl	_profile_enabled	|Anything to profile?
	beq	no_profile		|Not this time

	movw	sp@(16),d0		|Get the SR of the trap to get lvl.
	andl	#0xf00,d0		
	movw	sp@(20),sp@-		|Push the interrupt PC onto the stack
	movw	sp@(20),sp@-		|Push the interrupt PC onto the stack
	movl	d0,sp@-			|Push interrupt state onto the stack
	jsr	_profile_pc		|Go do the profiling
	addq	#8,sp			|Pop the stack

no_profile:
	moveml  sp@+,d0-d1/a0-a1	|restore registers
	movel	_nmi_handler,sp@-	|Get NMI handler routine
got_handler:
	rts				|And go there.  The NMI handler ends
					|with a RTE that pops the clock
					|interrupt exception frame.

|
| mcount - Low level interface for profiling
|
| void mcount (void)
|
| At the start of every routine compiled -p, the following code is placed:
|
|	lea <context>,a0
|	jsr mcount
|
| This places a pointer to the context longword in a0, and the return
| address on the stack.
|
| In order to prevent infinite loops, and to allow the profiling of routines
| which may be used by the profiling handler, we detect if we are called
| from within a handler, count it as a recursive profiling attempt,
| and get out.
|
| If the global _mcount_pc is non-zero, we will call the routine as
| follows:
|
|	(*(mcount_pc))(context, pc, priority, caller_pc)

	.globl	mcount
	.globl	_mcount_handler
	.globl	_mcount_recurse_count
	.globl	_mcount_count

	.data

	.even

_mcount_handler:
	.long	0			|Pointer to counter handler
_mcount_recurse_count:
	.long	0			|Recursion count
_mcount_count:
	.long 	0			|Total call count

	.text

mcount:	addl	#1,_mcount_count	|Count as a call to this routine

	moveal	_mcount_handler, a1	|Get the handler address
	tstl	a1			|Handler installed?
	beq	mcount_exit		|No, nothing to do

	movew	sr,d0			|Get the current PSW
	oriw	#0x0700,sr		|Lock out all interrupts

	movel	d0,d1			|Copy saved PSW
	andl	#0x0700,d1		|Save only priority
	lsrl	#8,d1			|Now get it as a level number

	cmpb	#7,d1			|Are we already running at Level 7?
	beq	mcount_recurse		|Yes, count the event
	movel	d0,sp@-			|No, save the original priority

	movel	a6@(4),sp@-		|Push the callers caller PC
	movel	d1,sp@-			|Push the calling priority
	movel	sp@(12),sp@-		|Push the calling PC
	movel	a0,sp@-			|Push the context pointer
	jsr	(a1)			|Call the handler in C
	addl	#16,sp			|Restore the stack

	movel	sp@+,d0			|Get back the original PSW
	movew	d0,sr			|Restore it

mcount_exit:
	rts				|Return to the original routine

mcount_recurse:
	addl	#1,_mcount_recurse_count |Count the recursion
	rts				|Return to the original routine

|
|
| void (**getvbr(void))(void)
|
| This routine returns the address of the vbr.
|
        .globl _getvbr
_getvbr:
        movc    vbr, d0         | Get vector base register value
        rts
|
|
| void (**setvbr(void (**)(void)))(void)
|
| This routine sets a new vbr, and returns the old one.
|
	.globl	_setvbr

_setvbr:
	movl	sp@(4),d1	| Get new VBR to set
	movc	vbr, d0		| Get current vector table
	movc	d1, vbr		| Set new vector table
	rts			| And return to a brave new world

|
| cache_flush - Flush all caches
|
|	void cache_flush (void)
|
| This routine is used to flush all CPU caches. This is typically used before
| passing control back from the debugger.

	.globl	_cache_flush
	.globl	_cpu_type

_cache_flush:
#if (SAPPHIRE)
	rts				| Nothing to do.
#endif
#if (PAN || BRASIL || XX || SYNALC)
	movec	cacr,d0			| Get current cache control
	oril	#0x0804,d0		| Flush instruction and data cache
	movec	d0,cacr			| Now do the flush
	rts				| all done
#endif
#if (CSC2 || RP1)
	cmpl	#CPU_CSC4, _cpu_type	| Is this a CSC/4?
	beq	cf_68040		| Yes, its a 68040
        cmpl    #CPU_RP1, _cpu_type     | Is this a RP/1?
        beq     cf_68040                | Yes, its a 68040

cf_68020:				| Otherwise do it the 68020/68030 way
	movec	cacr,d0			| Get current cache control
	oril	#0x0804,d0		| Flush instruction and data cache
	movec	d0,cacr			| Now do the flush
	rts				| all done

cf_68040:
	.word	0xf4f8			| cpush bc  ; push data cache, then
                                        | invalidate both caches
	rts				| All done
#endif

|
|Local Variables:
|comment-column: 40
|comment-start: "|"
|comment-end: ""
|comment-multi-line: nil
|comment-start-skip: "|"
|End:
|
