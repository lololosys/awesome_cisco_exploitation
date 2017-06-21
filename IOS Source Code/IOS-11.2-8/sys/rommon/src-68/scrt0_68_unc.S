/* $Id: scrt0_68_unc.S,v 3.2.58.1 1996/03/21 23:33:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-68/scrt0_68_unc.S,v $
 *------------------------------------------------------------------
 * scrt0_68_unc.S
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Startup module for Monitor standalone programs that expect
 * a system code interface.
 *------------------------------------------------------------------
 * $Log: scrt0_68_unc.S,v $
| Revision 3.2.58.1  1996/03/21  23:33:30  gstovall
| Branch: California_branch
| The ELC_branch now knows the joy of California dreaming.
|
| Revision 3.2.2.1  1995/12/01  04:24:47  mbeesley
| Branch: ELC_branch
| Reorganize Rom monitor header files to make it easier to port
| to new platforms.
|
| Revision 3.2  1995/11/17  18:48:23  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  13:09:55  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  22:42:15  hampton
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

	.globl _mifp
	.comm _mifp,4
	.lcomm sp_sav,4
	.lcomm ra_sav,4
	
	.globl	main
	.globl  exit

	.text
	.even
	.globl start
start:
	movl	#_edata,a0	| start by zeroing the bss section
	movl	#_end,a1
	movql	#0,d0
zero_bss:
	movl	d0,a0@+
	cmpl	a0,a1
	bgt	zero_bss

	| set our global monitor interface pointer (mifp)
	movl	sp@(12),a0	| pick up pib
	movl	a0@(PIB_MIFP),_mifp

	movl sp,sp_sav		| save our SP
	movl sp@+,ra_sav	| and our return address
	/*
	** Note - a little slight of hand above
	** By saving RA and backing up the SP we can call
	** main with the same args wthout copying them.
	*/

| The following instructions would give an
| argc,argv program interface
|	movl	a0@(PIB_ARGC),sp@- | push argc
|	movl	a0@(PIB_ARGV),sp@- | push argv

	jbsr	_main
	bra	ret_exit        | main return val is in d0


/*
** The exit routine
*/

	.even
	.globl _exit
_exit:
	movl	sp@(4),d0	| pick up return value
ret_exit:
	movl	sp_sav,sp       | restore our SP
	movl	ra_sav,sp@	| and our RA
	rts

/*
** The monitor system call interface for stand alone programs
*/
	.even
	.globl _getdevnum
_getdevnum:
	movl	#MIF_RA_GETDEVNUM,d0 | pick up the offset into jump table
do_moncall:
	movl	_mifp,a0	| pick up the jump table pointer
	addl	d0,a0		| add in the offset
	movl	a0@,a1		| pick up routine vector
	jmp	a1@		| jump to it


	.even
	.globl _open
_open:
	movl	#MIF_OPEN,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _close
_close:
	movl	#MIF_CLOSE,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _read
_read:
	movl	#MIF_READ,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _raw_read
_raw_read:
	movl	#MIF_RA_READ,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _write
_write:
	movl	#MIF_WRITE,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _lseek
_lseek:
	movl	#MIF_LSEEK,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _stat
_stat:
	movl	#MIF_STAT,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _fstat
_fstat:
	movl	#MIF_FSTAT,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _printf
_printf:
	movl	#MIF_PRINTF,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _sprintf
_sprintf:
	movl	#MIF_SPRINTF,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _puts
_puts:
	movl	#MIF_PUTS,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _putchar
_putchar:
	movl	#MIF_PUTCHAR,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _getchar
_getchar:
	movl	#MIF_GETCHAR,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _getline
_getline:
	movl	#MIF_GETLINE,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _shcmdline
_shcmdline:
	movl	#MIF_SHCMDLINE,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _getfreememstart
_getfreememstart:
	movl	#MIF_GETFREEMEMSTART,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _launch
_launch:
	movl	#MIF_LAUNCH,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _conin
_conin:
	movl	#MIF_CONIN,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _bzero
_bzero:
	movl	#MIF_BZERO,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _bcopy
_bcopy:
	movl	#MIF_BCOPY,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _strlen
_strlen:
	movl	#MIF_STRLEN,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _strcpy
_strcpy:
	movl	#MIF_STRCPY,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _getnum
_getnum:
	movl	#MIF_GETNUM,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _strcmp
_strcmp:
	movl	#MIF_STRCMP,d0	| pick up the offset into jump table
	bra	do_moncall

	.even
	.globl _strchr
_strchr:
	movl	#MIF_STRCHR,d0	| pick up the offset into jump table
	bra	do_moncall


/*
** Provide the EMT call interface for backward compatibility
**
** emt_call
** ROM monitor emulation trap support
**
** int _emt_call(traptype[,arg1[,arg2[,arg3]]])
** 
** This is the way the system code does it.
*/

	.even
	.globl _emt_call

_emt_call:
	link a6,#0
	movl a6@(20),sp@-   | copy 4 args on stack
	movl a6@(16),sp@-
	movl a6@(12),sp@-
	movl a6@(8),sp@-

	trap #15
 |
 | The next instruction fixes
 | up the stack for us
 |
	unlk a6
	rts


/*
** Provide a dummy routine __main for the
** brain dead tool chain.
*/
	.even
	.globl ___main
___main:
	rts

/* end of module */
