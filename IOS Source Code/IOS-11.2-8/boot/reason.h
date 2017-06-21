/* $Id: reason.h,v 3.3 1995/12/11 03:56:09 dbath Exp $
 * $Source: /vws/aam/cherf.111/ios/boot/reason.h,v $
 *------------------------------------------------------------------
 * reason.h -- reason why system reboots
 *
 * July 1988, Greg Satz
 *
 * Copyright (c) 1988-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reason.h,v $
 * Revision 3.3  1995/12/11  03:56:09  dbath
 * CSCdi45341:  Cant get crash stack on HSA systems.
 * Keep track of maximum reason code, to make it easier to determine
 * if the ROM monitor has left us a reason code, or a pointer to
 * a string.
 *
 * Revision 3.2  1995/11/17  07:41:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Holdover data from last runable image
 */
#define BOOT_STACK_FRAMES	8	/* # of pairs of fp and pc to save */

/*
 * Definition of bootstuff[]:
 */
#define BOOT_COUNT		0	/* Length of bootstuff */
#define	BOOT_REASON		1	/* Reason for last reboot */
#define	BOOT_PC			2	/* PC at last reboot */
#define	BOOT_ADDRESS		3	/* Buserror address */
#define BOOT_STACK_TRACE	4	/* Saved stack trace (fp and pc) */
#define BOOT_STACK_LAST		((BOOT_STACK_FRAMES * 2)-1+BOOT_STACK_TRACE)

#define	BOOTDATA		BOOT_STACK_LAST	/* Length of bootstuff */

/*
 * Reason for last reboot:
 */
#define	EXC_RESET	0	/* Reset switch  */
#define	EXC_ABORT	1	/* Abort switch */
#define	EXC_BREAK	2	/* Breakpoint trap */
#define EXC_EXIT	3	/* Exit trap */
#define	EXC_TRACE	4	/* Trace trap */
#define	EXC_EMT		5	/* Emulator trap */
#define	EXC_BUSERR	6	/* Bus error */
#define	EXC_ADRERR	7	/* Address error */
#define	EXC_WATCHDOG	8	/* Watchdog timeout */
#define	EXC_RELOAD	9	/* Reload requested */
#define	EXC_PARITY	10	/* Parity error */
#define EXC_SHMEM_PE    11      /* shared memory parity error */
#define EXC_UNEXP_INT   12      /* unexpected user definable interrupt */
#define EXC_MAX         12      /* largest reboot reason code */

/*
 * When to give up network boot and boot from roms.
 */

#define	THRESHOLD 5

/*
 * stuff this in bootstuff[BOOT_ADDRESS] to request boot of ROM software
 * actually, stuff ROMBOOT_REQUEST-1 in bootstuff[BOOT_ADDRESS] to 
 * account for increment done by rom monitor before value is checked
 */

#define ROMBOOT_REQUEST (THRESHOLD+2)
