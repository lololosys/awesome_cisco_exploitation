/* $Id: scrt0_mips_fslib.S,v 3.2 1995/11/17 09:09:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/scrt0_mips_fslib.S,v $
 *------------------------------------------------------------------
 * scrt0_mips_fslib.S
 *
 * February 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Startup module for Monitor standalone programs that expect
 * a system code interface.
 *------------------------------------------------------------------
 * $Log: scrt0_mips_fslib.S,v $
| Revision 3.2  1995/11/17  09:09:03  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  11:33:00  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:33:20  hampton
| Bump version numbers from 1.x to 2.x.
|
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Startup module for Monitor standalone programs that expect
** a system code interface
** Provide an entry point, zero the .bss section, and provide
** an exit routine for the standalone program.
**
** start(int code, struct plb *plb, struct pib *pib)
**
** The argument "code" is for launching a system image
** and is ignored here.
*/
#define ASMINCLUDE
#include "mon_boot.h"
#include "monlib.h"
#include "cpu_mips.h"

	.align 2
	.lcomm save_ra,4
	.lcomm save_sp,4

	.globl mifp
	.comm mifp,4
	
	.globl	main
	.globl  exit

/*
** startup function
**
** int start(code, plb, pib)
*/
FRAME(start,sp,0,ra)
	.set	noreorder
	la	t0,_edata	# zero the .bss section
	la	t1,_end
clrbss:
	sb	zero,0(t0)	# zero a location
	addiu	t0,t0,1		# bump pointer
	blt	t0,t1,clrbss	# loop til done	
	nop

	lw	t0,PIB_MIFP(a2)	# pick up our mifp
	sw	ra,save_ra	# save the return address
	sw	sp,save_sp	# and the stack pointer

	sw	t0,mifp		# set our globl mifp

# The following 2 instructions would give an
# argc,argv program interface
#	lw	a0,PIB_ARGC(a2) # pick up our argc
#	lw	a1,PIB_ARGV(a2) # pick up our argv

	jal	main
	nop

	b	ret_exit
	nop
	.set	reorder
ENDFRAME(start)


/*
** The exit routine
*/

FRAME(exit,sp,0,ra)
	.set	noreorder
	move	v0,a0		# set return value
ret_exit:
	lw	ra,save_ra	# restore our return value
	lw	sp,save_sp	# and our stack pointer
	j	ra		# return
	nop
	.set	reorder
ENDFRAME(exit)

/*
** The monitor system call interface for stand alone programs
**
** Note:
** In the code:
**
**	li	t0,MIF_OPEN
**	b	do_moncall
**	nop
**
** The programmer may be tempted to do the "li" in the branch
** delay slot.  This will work if the immediate data is 16 bits
** but will break with no warning if the data is ever bigger
** than 16 bits.  These wrappers were coded to be safe.
*/
	.globl getdevnum
FRAME(getdevnum,sp,0,ra)
	.set	noreorder
	li	t0,MIF_RA_GETDEVNUM # pick up the offset into jump table
do_moncall:
	lw	t1,mifp		# pick up the jump table pointer
	nop
	addu	t0,t0,t1	# add in the offset
	lw	t1,0(t0)	# pick up routine vector
	nop
	j	t1		# jump to it
	nop
	.set	reorder
ENDFRAME(getdevnum)

	.globl open
FRAME(open,sp,0,ra)
	.set	noreorder
	li	t0,MIF_OPEN
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(open)

	.globl close
FRAME(close,sp,0,ra)
	.set	noreorder
	li	t0,MIF_CLOSE
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(close)

	.globl read
FRAME(read,sp,0,ra)
	.set	noreorder
	li	t0,MIF_READ
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(read)

	.globl raw_read
FRAME(raw_read,sp,0,ra)
	.set	noreorder
	li	t0,MIF_RA_READ
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(raw_read)

	.globl write
FRAME(write,sp,0,ra)
	.set	noreorder
	li	t0,MIF_WRITE
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(write)

	.globl lseek
FRAME(lseek,sp,0,ra)
	.set	noreorder
	li	t0,MIF_LSEEK
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(lseek)

	.globl stat
FRAME(stat,sp,0,ra)
	.set	noreorder
	li	t0,MIF_STAT
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(stat)

	.globl fstat
FRAME(fstat,sp,0,ra)
	.set	noreorder
	li	t0,MIF_FSTAT
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(fstat)

	.globl printf
FRAME(printf,sp,0,ra)
	.set	noreorder
	li	t0,MIF_PRINTF
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(printf)

	.globl sprintf
FRAME(sprintf,sp,0,ra)
	.set	noreorder
	li	t0,MIF_SPRINTF
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(sprintf)

	.globl puts
FRAME(puts,sp,0,ra)
	.set	noreorder
	li	t0,MIF_PUTS
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(puts)

	.globl putchar
FRAME(putchar,sp,0,ra)
	.set	noreorder
	li	t0,MIF_PUTCHAR
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(putchar)

	.globl getchar
FRAME(getchar,sp,0,ra)
	.set	noreorder
	li	t0,MIF_GETCHAR
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(getchar)

	.globl getline
FRAME(getline,sp,0,ra)
	.set	noreorder
	li	t0,MIF_GETLINE
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(getline)

	.globl shcmdline
FRAME(shcmdline,sp,0,ra)
	.set	noreorder
	li	t0,MIF_SHCMDLINE
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(shcmdline)

	.globl getfreememstart
FRAME(getfreememstart,sp,0,ra)
	.set	noreorder
	li	t0,MIF_GETFREEMEMSTART
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(getfreememstart)

	.globl launch
FRAME(launch,sp,0,ra)
	.set	noreorder
	li	t0,MIF_LAUNCH
	b	do_moncall
	nop
	.set	reorder
ENDFRAME(launch)


/*
** Provide the EMT call interface for backward compatibility
**
** emt_call
** ROM monitor emulation trap support
**
** int emt_call(traptype[,arg1[,arg2[,arg3]]])
*/
	.globl emt_call
FRAME(emt_call,sp,0,ra)
	.set	noreorder
	syscall
	nop
	.set	reorder
ENDFRAME(emt_call)
 

/*
** Provide a dummy routine __main for the
** brain dead tool chain.
*/
	.globl __main
FRAME(__main,sp,0,ra)
	.set	noreorder
	j	ra
	nop
	.set	reorder
ENDFRAME(__main)
