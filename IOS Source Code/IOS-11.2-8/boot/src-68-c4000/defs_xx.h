/* $Id: defs_xx.h,v 3.2 1996/01/03 03:08:58 clev Exp $
 * $Source: /vws/aam/cherf.111/ios/boot/src-68-c4000/defs_xx.h,v $
 *------------------------------------------------------------------
 * defs_xx.h -- monitor definitions specific to XX
 *
 * April 1992, Rob Clevenger
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: defs_xx.h,v $
 * Revision 3.2  1996/01/03  03:08:58  clev
 * CSCdi46211:  Memory sizing broke on 4000 platform
 *
 * Fix old monitor code so it doesn't get optimized out with newer
 * versions of the compiler (gcc.94q2.68k).
 *
 * Revision 3.1  1995/11/09  09:09:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:50  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.3  1993/01/30  01:12:14  whp4
 * Add RP1 support.
 *
 * Revision 1.2  1992/05/06  22:32:54  clev
 * XX cleanup stuff:
 *
 * ../asm.S - fix clear caches in the restart routine
 *
 * ../prom2.c - handle netbooting a system image with a load point different from
 *              firmware system image
 *
 * decodebe.c & printf.c - fix reporting of bus errors and parity errors
 *
 * defs_xx.h - defined BUSERROR for the XX for conditional compiles
 *
 * fldvr.c & rom_tests.c - provide abbreviated flash test for manufacturing
 *
 * init.c - fix the clobbering of bootinfo stuff
 *
 * Revision 1.1  1992/04/15  02:11:35  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	REFRESHTIME	4		/* time in milliseconds for refresh */
#define	UARTFREQUENCY	16
#define	REFRFREQUENCY	(REFRESHTIME*1000)
#define	CPUTYPE		CPU_XX

#define	ERROR_ADDRESS	error->stack.typea.faddr

#define	BADADDRESS	0x2200000	/* address of bad location */

#define BUSERROR 1
#define DCACHE 1

/*
 * location of i8254 registers
 */

#define	CTDEVBASEADDR	0x2120000

#define INITSTACKP	0x1000		/* stack pointer starts (goes down) */
#define LOWCORE0 ((volatile long *) 0x10)	/* start page 0 memory test here */
#define LOWCORE1 ((volatile long *) INITSTACKP)	/* finish page 0 mem test here */
