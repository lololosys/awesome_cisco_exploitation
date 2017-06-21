/* $Id: defs_rp1.h,v 3.1 1995/11/09 09:10:05 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/defs_rp1.h,v $
 *------------------------------------------------------------------
 * defs_rp1.h -- monitor definitions
 *
 * June 1992, Gary Kramling
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: defs_rp1.h,v $
 * Revision 3.1  1995/11/09  09:10:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.4  1994/07/09 12:14:20  mdb
 * Sync MAIN to NAC1_postsync_940706
 *
 * Revision 1.1.4.2  1994/03/13  10:40:14  jhayes
 * Branch: Sierra_branch - temporarily restore a timer definition until
 *         68k/r4k timer modularization is completed.
 *
 * Revision 1.1.4.1  1994/02/24  07:58:15  smackie
 * Bring Sierra_branch to 10.0(0.7) patch level
 * Branch: Sierra_branch
 *
 * Revision 1.2  1994/02/18  22:20:17  sthormod
 * CSCdi16289:  16mb of memory not enough for proper operation of the
 * router
 * Change memory sizing to allow for either 16 or 64mb on RP1.
 *
 * Revision 1.1  1993/09/24  16:34:18  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/08/19  03:51:32  hampton
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
 * Revision 1.1  1993/01/30  01:11:44  whp4
 * Add RP1 support.
 *
 *
 * $Endlog$
 */

#define	REFRESHTIME	4		/* time in milliseconds for refresh */
#define	REFRFREQUENCY	(REFRESHTIME*1000)

#define	ERROR_ADDRESS  	error->stack.type7.faddr

#define	BADADDRESS	0xfffffff0	/* non-existent location */

#define BUSERROR	1		/* We support bus errors */

/*
 * location of UART registers
 */

#define UARTDEVBASEADDR 0x11120100

/*
 * location of i8254 registers
 */

#define	CTDEVBASEADDR	0x11120000

/*
 * Where net bootable binaries are loaded into memory
 */

#define INITSTACKP      (ADRSPC_RAM + 0x1000)
                                        /* stack pointer starts (goes down) */
#define LOWCORE0 ((long *) (ADRSPC_RAM + 0x10))
                                        /* start page 0 memory test here */
#define LOWCORE1 ((long *) INITSTACKP)  /* finish page 0 mem test here */

#define MSIZE_16MB   0x1000000

/* FIXME: Remove after reworking if_68_timer.c */

#define ADRSPC_BRUT_TIMER2      0x02120070 /* Timer register 2 (BRUT only) */
