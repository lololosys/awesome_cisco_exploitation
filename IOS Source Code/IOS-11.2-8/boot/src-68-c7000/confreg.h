/* $Id: confreg.h,v 3.1 1995/11/09 09:09:58 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/confreg.h,v $
 *------------------------------------------------------------------
 * confreg.h -- configuration register definitions
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: confreg.h,v $
 * Revision 3.1  1995/11/09  09:09:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.3  1995/06/07 09:05:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.2.74.1  1995/05/02  23:22:19  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.2  1993/12/16  12:05:38  sthormod
 * ROM monitor support for nvram config compress & s/w config register.
 *
 * Revision 1.1  1993/09/24  16:34:13  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/01/30  01:11:39  whp4
 * Add RP1 support.
 *
 *
 * $Endlog$
 */

/*
 * Bootstrap Configuration options
 * Note that these configuration register bits are high when the jumpers
 * are removed.
 */

struct confreg {
	u_int diag:1;			/* 15: diagnostic mode (HIGH) */
	u_int network:1;		/* 14: use net in IP bcast address */
	u_int romboot:1;		/* 13: load rom after netboot fails */
	u_int consspeed:2;		/* 12-11: console UART speed code */
	u_int allzeros:1;		/* 10: use all zero broadcast */
	u_int bootstrap:1;		/* 09: use secondary bootstrap */
	u_int breakenable:1;		/* 08: break/abort has effect (HIGH) */
	u_int oem:1;			/* 07: oem mode (no cisco copyright) */
	u_int ign_conf:1;		/* 06: ignore the system configuration */
	u_int expert:1;			/* 05: Expert mode for diagnostics */
	u_int fastboot:1;		/* 04: try to boot w/o rboot image */
	u_int bootfile:4;		/* 03-00: bootfile number */
};


struct system_status {
	struct confreg config;		/* 16 bits of configuration */
	u_int mbs4 : 1;			/* Multibus level 4 interrupt state */
	u_int mbs3 : 1;			/* Multibus level 3 interrupt state */
	u_int mbs2 : 1;			/* Multibus level 2 interrupt state */
	u_int mbs1 : 1;			/* Multibus level 1 interrupt state */
	u_int mbc4 : 1;			/* Multibus level 4 control */
	u_int mbc3 : 1;			/* Multibus level 3 control */
	u_int mbc2 : 1;			/* Multibus level 2 control */
	u_int mbc1 : 1;			/* Multibus level 1 control */

	u_char capabilities:5;		/* 5-bit capability mask */
	u_char version:2;		/* Processor version */
	u_char :1;			/* an extra bit */
};

#define	sys_status_reg	((struct system_status *)ADRSPC_STATUS)

/*
 * software configuration register 
 */
#define config_ptr         ((struct confreg *)ADRSPC_NVRAM_STATUS)
#define config_magic_ptr   ((ushort *)ADRSPC_NVRAM_STATUSMAG)
#define CONFREG_MAGIC      0x1315
#define NVCONFIGREG ((unsigned char *)ADRSPC_NVRAM_STATUSMAG)
#define NVREADBYTE(offset) (NVCONFIGREG[offset])

/*
 * Processor version numbers:
 */
#define VER_CSC3	0x01
#define VER_CSC4	0x02
#define VER_RP1		0x03

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
 * Speed encodings for console port (Port B is unused)
 */
#define	BAUDRATE_9600	3
#define	BAUDRATE_4800	2
#define	BAUDRATE_1200	1
#define BAUDRATE_2400	0

/*
 * Bootfile encodings
 */
#define	BOOT_SELF_TEST	0xF	/* don't autoboot, ignore other options */

/*
 * Prototypes
 */
ushort read_nv_short(ushort *);
void   write_nv_short(ushort *, ushort);
