! $Id: unix_sparc.S,v 3.3 1996/02/28 23:11:33 hampton Exp $
! $Source: /release/111/cvs/Xsys/asm-sparc/unix_sparc.S,v $
!------------------------------------------------------------------
! sparc.S -- cisco primitives for the SPARC
!
! 17-December-1991, Kirk Lougheed
!
! Copyright (c) 1991-1996 by cisco Systems, Inc.
! All rights reserved.
!
!------------------------------------------------------------------
! $Log: unix_sparc.S,v $
! Revision 3.3  1996/02/28  23:11:33  hampton
! Fix timer errors in unix images.  [CSCdi45965]
!
! Revision 3.2  1995/11/17  08:41:15  hampton
! Remove old entries from the RCS header logs.
!
! Revision 3.1  1995/11/09  10:55:48  shaker
! Bump version numbers from 2.x to 3.x.
!
! Revision 2.1  1995/06/07  20:07:13  hampton
! Bump version numbers from 1.x to 2.x.
!
!------------------------------------------------------------------
! $Endlog$
!

	.seg "text"

#ifndef __svr4__
#define SYMBOL(s) _##s
#else /* __svr4__ */
#define SYMBOL(s) s		/* Solaris linker doesn't want underscores. */
#endif /* __svr4__ */

/*
 * _resume
 * Switch between scheduler and process context
 * Note: %g1+8 is forkx->regs[0]
 */
        .global SYMBOL(suspend), SYMBOL(resume)

SYMBOL(resume):
	ta 3			/* ask UNIX to flush windows */
	sethi %hi(SYMBOL(forkx)), %g1	/* put address of forkx in %g1 */
	ld [%g1+%lo(SYMBOL(forkx))], %g1 /* %g1 now points to process structre */

	swap [%g1+8], %i0	/* exchange incoming registers */
	swap [%g1+12], %i1
	swap [%g1+16], %i2
	swap [%g1+20], %i3
	swap [%g1+24], %i4
	swap [%g1+28], %i5
	swap [%g1+32], %fp 	/* exchange frame pointers (%fp is %i6) */
	swap [%g1+36], %i7

	swap [%g1+40], %o0	/* exchange outgoing registers */
	swap [%g1+44], %o1
	swap [%g1+48], %o2
	swap [%g1+52], %o3
	swap [%g1+56], %o4
	swap [%g1+60], %o5
	swap [%g1+64], %sp	/* exchange stack pointers (%sp is %o6) */
	swap [%g1+68], %o7	/* exchange return addresses */

	swap [%g1+72], %l0	/* exchange local registers */
	swap [%g1+76], %l1
	swap [%g1+80], %l2
	swap [%g1+84], %l3
	swap [%g1+88], %l4
	swap [%g1+92], %l5
	swap [%g1+96], %l6
	swap [%g1+100], %l7

	sethi %hi(SYMBOL(schedflag)),%g2/* put address of schedflag in %g2 */
	jmpl %o7+8, %g0			/* return in user context */
	st %g0,[%g2+%lo(SYMBOL(schedflag))]	/* clear schedflag (bd slot) */

SYMBOL(suspend):
	ta 3			/* ask UNIX to flush windows */
	sethi %hi(SYMBOL(forkx)), %g1	/* put address of forkx in %g1 */
	ld [%g1+%lo(SYMBOL(forkx))], %g1 /* %g1 now points to process structre */

	sethi %hi(SYMBOL(schedflag)),%g2    /* put address of schedflag in %g2 */
	mov 1,%g3
	st %g3,[%g2+%lo(SYMBOL(schedflag))] /* set schedflag */

	swap [%g1+8], %i0	/* exchange incoming registers */
	swap [%g1+12], %i1
	swap [%g1+16], %i2
	swap [%g1+20], %i3
	swap [%g1+24], %i4
	swap [%g1+28], %i5
	swap [%g1+32], %fp 	/* exchange frame pointers (%fp is %i6) */
	swap [%g1+36], %i7

	swap [%g1+40], %o0	/* exchange outgoing registers */
	swap [%g1+44], %o1
	swap [%g1+48], %o2
	swap [%g1+52], %o3
	swap [%g1+56], %o4
	swap [%g1+60], %o5
	swap [%g1+64], %sp	/* exchange stack pointers (%sp is %o6) */
	swap [%g1+68], %o7	/* exchange return addresses */

	swap [%g1+72], %l0	/* exchange local registers */
	swap [%g1+76], %l1
	swap [%g1+80], %l2
	swap [%g1+84], %l3
	swap [%g1+88], %l4
	swap [%g1+92], %l5
	swap [%g1+96], %l6
	jmpl %o7+8, %g0		/* return in user context */
	swap [%g1+100], %l7

/*
 * __builtin_saveregs
 * Swiped from gcclib2.c.  Need for varargs support on a SPARC
 */
	.global SYMBOL(__builtin_saveregs)

SYMBOL(__builtin_saveregs):
	st %i0,[%fp+68]
	st %i1,[%fp+72]
	st %i2,[%fp+76]
	st %i3,[%fp+80]
	st %i4,[%fp+84]
	retl
	st %i5,[%fp+88]

/*
 * usecdelay
 * 
 * A hardware dependent busy-wait loop.  Since we have no external devices
 * on this "hardware", we don't have to do anything.
 */

	.global SYMBOL(usecdelay)

SYMBOL(usecdelay):
	save %sp, -96, %sp
	ret
	restore

/*
 * caller
 * Return PC of our caller's caller
 * Neither of these routines is implemented correctly. caller() 
 * sometimes returns -1, which is the wrong stack location.
 */

	.global SYMBOL(cpu_caller)

SYMBOL(cpu_caller):
	ld [%sp+60], %o0	/* get PC from stack frame */
	tst %o0
	bge callr0		/* jump if positive, anull next */
	nop
	clr  %o0		/* turn -1 into 0 */
callr0:	jmpl %o7+8, %g0		/* return with value in %o0 */
	nop

/*
 * Some 68K specific stuff that someone else gets to sort out.
 */

	.global SYMBOL(gdb_shutdown), SYMBOL(breakpoint)
	.global SYMBOL(pup_crc1), SYMBOL(pup_qcsum), SYMBOL(vines_crc1)
	.global SYMBOL(vines_qcsum), SYMBOL(xns_crc1), SYMBOL(xns_qcsum)

SYMBOL(breakpoint):
SYMBOL(gdb_shutdown):
SYMBOL(pup_crc1):
SYMBOL(pup_qcsum):
SYMBOL(vines_crc1):
SYMBOL(vines_qcsum):
SYMBOL(xns_qcsum):
	save %sp, -96, %sp
	nop
	ret
	restore

	.global SYMBOL(current_pc)

SYMBOL(current_pc):
	ld [%sp], %o0
	save %sp, -96, %sp
	nop
	ret
	restore

SYMBOL(xns_crc1):
	save %sp, -96, %sp
	set 0xFFFF, %i0		/* return the null checksum always */
	nop
	ret
	restore

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
 */

	.globl	SYMBOL(suspended_process_forced_here)
	.globl	SYMBOL(process_forced_here)
	.globl	SYMBOL(running_process_forced_here)

SYMBOL(suspended_process_forced_here):

	sethi	%hi(SYMBOL(forkx)),%g1		/* put forkx address in %g1 */
	ld	[%g1+%lo(SYMBOL(forkx))],%g1	/* get forkx contents in %g1 */
	b	SYMBOL(process_forced_here)
	ld	[%g1+108],%o7	/* the process executed the "j ra" at
				   the end of "resume" to get here so
				   the ra value is no longer pertinent.
				   Fudge it so that gdb does the right
				   thing when the backtrace command is
				   issued. (bd slot) */

SYMBOL(running_process_forced_here):
	sethi %hi(SYMBOL(forkx)),%g1		/* put forkx address in %g1 */
	ld [%g1+%lo(SYMBOL(forkx))],%g1		/* get forkx contents in %g1 */

SYMBOL(process_forced_here):
	save	%sp,-0x140,%sp
	std	%f30,[%fp-0x08]	/* Save registers */
	std	%f28,[%fp-0x10]
	std	%f26,[%fp-0x18]
	std	%f24,[%fp-0x20]
	std	%f22,[%fp-0x28]
	std	%f20,[%fp-0x30]
	std	%f18,[%fp-0x38]
	std	%f16,[%fp-0x40]
	std	%f14,[%fp-0x48]
	std	%f12,[%fp-0x50]
	std	%f10,[%fp-0x58]
	std	%f8,[%fp-0x60]
	std	%f6,[%fp-0x68]
	std	%f4,[%fp-0x70]
	std	%f2,[%fp-0x78]
	std	%f0,[%fp-0x80]
	std	%g6,[%fp-0x88]
	std	%g4,[%fp-0x90]
	std	%g2,[%fp-0x98]
	std	%g0,[%fp-0xa0]
	std	%i6,[%fp-0xa8]
	std	%i4,[%fp-0xb0]
	std	%i2,[%fp-0xb8]
	std	%i0,[%fp-0xc0]
	mov	%psr,%o0
	st	%o0,[%fp-0xdc]
	mov	%y,%o0
	st	%o0,[%fp-0xe0]

	ld	[%g1+112],%l0	/* load address of inserted routine */
	call	%l0,0		/* and call it */
	nop			/* (bd slot) */

	ld	[%fp-0xe0],%o0	/* restore registers */
	mov	%o0,%y
	ld	[%fp-0xdc],%o0
	mov	%o0,%psr
	ldd	[%fp-0xc0],%i0
	ldd	[%fp-0xb8],%i2
	ldd	[%fp-0xb0],%i4
	ldd	[%fp-0xa8],%i6
	ldd	[%fp-0xa0],%g0
	ldd	[%fp-0x98],%g2
	ldd	[%fp-0x90],%g4
	ldd	[%fp-0x88],%g6
	ldd	[%fp-0x80],%f0
	ldd	[%fp-0x78],%f2
	ldd	[%fp-0x70],%f4
	ldd	[%fp-0x68],%f6
	ldd	[%fp-0x60],%f8
	ldd	[%fp-0x58],%f10
	ldd	[%fp-0x50],%f12
	ldd	[%fp-0x48],%f14
	ldd	[%fp-0x40],%f16
	ldd	[%fp-0x38],%f18
	ldd	[%fp-0x30],%f20
	ldd	[%fp-0x28],%f22
	ldd	[%fp-0x20],%f24
	ldd	[%fp-0x18],%f26
	ldd	[%fp-0x10],%f28
	ldd	[%fp-0x08],%f30
	restore			/* pop the stack */
	ret			/* go back to original routine */
	ld	[%g1+108],%o7	/* restore ra of interrupted routine (bd slot) */

/*
 * void alter_suspended_process (sprocess *proc, ulong new_address)
 *
 * Change the execution address of a process.  This is used when
 * killing a process so the it has a chance to clean up before it is
 * completely destroyed.
 *
 * Note:
 * 	8(t0) is proc->regs[0]
 */

	.globl	SYMBOL(alter_suspended_process)

SYMBOL(alter_suspended_process):
	sethi %hi(SYMBOL(forkx)),%g1	/* put address of forkx in %g1 */
	ld [%g1+%lo(SYMBOL(forkx))],%g1 /* %g1 now points to process structre */

	ld [%g1+4],%l0			/* get stack pointer of process */
	ld [%g1+100],%l1		/* get current return address */

	st %l1,[%g1+108]		/* save original return address */
	st %i1,[%g1+112]		/* save routine to call first */
	
	/* Load address of routine to call */
	sethi %hi(SYMBOL(suspended_process_forced_here)),%l2
	or %l2,%lo(SYMBOL(suspended_process_forced_here)),%l2

	retl				/* return */
	st %l2,[%g1+100]		/* set ra to helper routine (bd slot) */
