/* $Id: confreg.h,v 3.2 1995/11/17 07:42:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c3000/confreg.h,v $
 *------------------------------------------------------------------
 * pan/confreg.h -- configuration register definitions
 *
 * Chris Shaker, July, 1990
 *
 * Copyright (c) 1990-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: confreg.h,v $
 * Revision 3.2  1995/11/17  07:42:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:09:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:27  hampton
 * Bump version numbers from 1.x to 2.x.
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
