/* $Id: confreg.h,v 3.2.58.1 1996/05/09 13:54:15 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/confreg.h,v $
 *------------------------------------------------------------------
 * confreg.h -- configuration register definitions (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: confreg.h,v $
 * Revision 3.2.58.1  1996/05/09  13:54:15  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.2.88.1  1996/04/27  06:13:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.78.1  1996/04/08  01:34:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  09:07:11  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.50.1  1996/03/03  21:19:10  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/02  01:07:07  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.32.1  1996/02/29  07:42:10  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  07:42:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:08:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:49  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:44  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:35  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:40  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14  05:22:00  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.1  1995/02/01  10:59:53  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:32  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:47  rahuja
 * IPS ver 10.2 snapshot
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
#define	BOOT_FIELD	0x0F	/* Boot options */
