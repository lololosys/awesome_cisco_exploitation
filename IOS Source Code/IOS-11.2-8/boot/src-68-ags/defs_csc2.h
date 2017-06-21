/* $Id: defs_csc2.h,v 3.1 1995/11/09 09:08:30 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-ags/defs_csc2.h,v $
 *------------------------------------------------------------------
 * defs_csc2.h -- monitor definitions
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: defs_csc2.h,v $
 * Revision 3.1  1995/11/09  09:08:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:14:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.3  1994/07/09 11:49:56  mdb
 * Sync MAIN to NAC1_postsync_940706
 *
 * Revision 1.1.4.1  1994/03/13  10:39:46  jhayes
 * Branch: Sierra_branch - temporarily restore a timer definition until
 *         68k/r4k timer modularization is completed.
 *
 * Revision 1.1  1993/09/24  16:33:01  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.4  1993/08/19  03:51:07  hampton
 * Bring modular system to Aries 9.21(0.18) patch level.
 *
 * CSCdi09946:  Protect stack_whyreload() from bad boot_reason values
 * 
 * Doing ROM monitor changes required to support EMT_SET_CACR on all
 * 68020/68030/68040 platforms. Share setcacr() and getcacr() on all of
 * these platforms. Move all defines for bits in CACR registers to the
 * pcmap.h files for each processor so that process level code does not
 * have to have its own definition of the CACR bits.
 * 
 * Add better comments to explain memory sizing code in Merlot (IGS/TR)
 * ROM monitor.
 * 
 * Add new EMT call, EMT_WRITE_BAD_MAIN_MEMORY_PARITY, used by the new
 * 'test crash' menu in the system image. This EMT call writes a 32 bit
 * word of information into main memory with bad parity.
 * 
 * Fix EMT number collision between Clean and 9.1 for new EMT call to make
 * life easier for the 9.17 release.  Also, the numbers for all new EMT
 * calls currently defined in the 9.21 release are now also defined in the
 * 9.1 release as placeholders to prevent such EMT collision problems in
 * the future.
 *
 * Revision 1.3  1993/01/30  01:11:09  whp4
 * Add RP1 support.
 *
 * Revision 1.2  1992/11/23  22:22:08  hampton
 * Fix gcc 2.x warning.
 *
 * Revision 1.1  1992/03/07  23:56:20  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	REFRESHTIME	4		/* time in milliseconds for refresh */
#define	REFRFREQUENCY	(REFRESHTIME*1000)

#define	ERROR_ADDRESS		error->stack.typea.faddr
#define	ERROR_ADDRESS_040	error->stack.type7.faddr

#define	BADADDRESS	0xff00000	/* address of bad location */

#define BUSERROR	1		/* We support bus errors */

/*
 * location of UART registers
 */
#define	UARTDEVBASEADDR	0x02120100

/*
 * location of i8254 registers
 */
#define	CTDEVBASEADDR	0x2120000

/*
 * Where net bootable binaries are loaded into memory
 */

#define INITSTACKP	(ADRSPC_RAM + 0x1000)
					/* stack pointer starts (goes down) */
#define LOWCORE0 ((long *) (ADRSPC_RAM + 0x10))
					/* start page 0 memory test here */
#define LOWCORE1 ((long *) INITSTACKP)	/* finish page 0 mem test here */

/* FIXME: remove after further rework of if_68_timer.c */

#define ADRSPC_BRUT_TIMER2      0x02120070 /* Timer register 2 (BRUT only) */
