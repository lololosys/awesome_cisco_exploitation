/* $Id: defs_pan.h,v 3.1 1995/11/09 09:09:09 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c3000/defs_pan.h,v $
 *------------------------------------------------------------------
 * defs.h -- monitor definitions
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: defs_pan.h,v $
 * Revision 3.1  1995/11/09  09:09:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1993/12/21 06:25:44  enf
 * Cancun boot support
 *
 * Revision 1.1  1993/09/24  16:33:23  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.5  1993/06/25  05:08:59  smackie
 * Bring modular system to Aries 9.21(0.10) patch level.
 *
 * CSCdi11223:  merge 9.14 low end code into aries
 *
 * Revision 1.4  1993/02/03  04:21:35  gstovall
 * Removed support for the cristal low-end platform.
 *
 * Revision 1.3  1993/01/30  01:11:20  whp4
 * Add RP1 support.
 *
 * Revision 1.2  1992/09/26  00:41:06  gstovall
 * Added support for the serial NVRAM cookie used on cristal.  Copied kph's
 * driver down to the rom monitor level.
 *
 * Revision 1.1  1992/03/07  23:57:35  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	REFRESHTIME	4		/* time in milliseconds for refresh */
#define	UARTFREQUENCY	16
#define	REFRFREQUENCY	(REFRESHTIME*1000)
#define	CPUTYPE		CPU_PAN

#define	ERROR_ADDRESS	error->stack.typea.faddr

#define	BADADDRESS	0x2200000	/* address of bad location */

#define BUSERROR	1		/* We support bus errors */

#define DCACHE		1		/* We support data cache */

#define NO_COOKIE_PROM  0xF1            /* There is no cookie PROM */

#define NO_COOKIE_PROM_CANCUN 0xFF      /* Cancun own version of no_cookie*/
/*
 * Address of 8254 registers
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

/*
 * Serial NVRAM stuff
 */
#define X2444_PORT *(uchar *)0x02110060        /* Data register for X2444 */

#define X2444_CE 0x08           /* CE: Chip enable, also seen as CS */
#define X2444_SK 0x04           /* SK: Serial clock in */
#define X2444_DI 0x02           /* DI: Serial data in  */
#define X2444_DO 0x01           /* DO: Serial data out */




