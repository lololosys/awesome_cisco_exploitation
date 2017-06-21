/* $Id: confreg.h,v 3.1 1995/11/09 09:09:29 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/confreg.h,v $
 *------------------------------------------------------------------
 * pan/confreg.h -- configuration register definitions
 *
 * Chris Shaker, July, 1990
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: confreg.h,v $
 * Revision 3.1  1995/11/09  09:09:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1994/07/09 11:50:06  mdb
 * Sync MAIN to NAC1_postsync_940706
 *
 * Revision 1.1.4.1  1994/03/07  21:52:36  mbeesley
 * Branch: Sierra_branch
 * Remove magic_ptr from header file.
 *
 * Revision 1.1  1993/09/24  16:33:43  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.3  1992/08/27  01:39:19  clev
 * Fix the 0x40 bit meaning in the virtual config register to ign_conf.
 * Setting this bit causes the booting system code to ignore the configuration
 * info in NVRAM.  Also changed the description for this bit in the Monitor
 * "o" command.
 *
 * Revision 1.2  1992/06/08  23:55:39  shaker
 * CSCdi05317, "default config reg setting should have NOCONFIG set".
 *
 * Revision 1.1  1992/04/07  19:49:33  clev
 * Initial CVS checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:10  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Configuration options
 * Pancake has a physical 8 bit configuration register and a virtual 16 bit
 * configuration register implemented in NVRAM. The values from the physical
 * register control the bits in the virtual register.
 * These configuration register bits are high when the switches
 * are open.
 */

struct phys_confreg {
    u_int diag:1;		/* 07: diagnostic mode (HIGH) */
    u_int watchdog:1;		/* 06: disable watchdog timer */
    u_int consspeed:2;		/* 05-04: console UART speed code */
    u_int bootfile:4;		/* 03-00: bootfile number */
};

#define	phys_confreg_ptr	((struct phys_confreg *)ADRSPC_STATUS)

/*
 * Magic numbers based upon the above structure
 * If the structure changes these will need to as well.
 */
#define	PHYS_DIAG_MASK			0x80
#define PHYS_WATCHDOG_MASK		0x40
#define PHYS_CONSOLE_MASK		0x30
#define PHYS_CONSOLE_SHIFT		4
#define PHYS_TO_VIRT_CONSOLE_SHIFT	7



/*
 * Virtual 16 bit configuration register implemented in NVRAM:
 */
struct confreg {
    u_int diag:1;		/* 15: diagnostic mode (HIGH) */
    u_int network:1;		/* 14: use net in IP bcast address */
    u_int romboot:1;		/* 13: load rom after netboot fails */
    u_int consspeed:2;		/* 12-11: console UART speed code */
    u_int allzeros:1;		/* 10: use all zero broadcast */
    u_int unused_I:1;		/* 09: unused */
    u_int breakenable:1;	/* 08: break/abort has effect (HIGH) */
    u_int oem:1;		/* 07: oem mode (no cisco copyright) */
    u_int ign_conf:1;		/* 06: ignore the system configuration */
    u_int unused_II:2;		/* 05-04: unused */
    u_int bootfile:4;		/* 03-00: bootfile number */
};

#define	config_ptr		((struct confreg *)ADRSPC_NVRAM_STATUS)
#define config_magic_ptr	((ushort *)ADRSPC_NVRAM_STATUSMAG)
#define CONFREG_MAGIC		0x1315
#define CONFIG_MAGIC		0xf0a5

/*
 * Magic numbers based upon the above structure
 * If the structure changes these will need to as well.
 */
#define CONSOLE_MASK	0x1800
#define CONSOLE_SHIFT	11
#define	DIAG_MASK	0x8000
#define OEM_MASK	0x0080
#define WATCHDOG_MASK	0x0040

/*
 * Speed encodings for console port (Port B is seperate)
 */
#define	BAUDRATE_9600	3
#define	BAUDRATE_4800	2
#define	BAUDRATE_1200	1
#define BAUDRATE_2400	0

/*
 * Bootfile encodings
 */
#define	BOOT_FIELD	0xF	/* Boot options */
