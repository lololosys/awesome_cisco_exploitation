/* $Id: cookie.h,v 3.1.58.1 1996/05/09 13:54:19 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/cookie.h,v $
 *------------------------------------------------------------------
 * cookie.h -- IOS Cookie definition for Synergy ATM line card (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996,97 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cookie.h,v $
 * Revision 3.1.58.1  1996/05/09  13:54:19  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.1  1996/03/22  09:07:14  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:13  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:11  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:13  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:51  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:47  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:38  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:44  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:02  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.1  1995/02/01  10:59:54  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:34  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:48  rahuja
 * IPS ver 10.2 snapshot
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct cookie cookie_t;
struct cookie {							/* byte #  */
    uchar interfaces;           /* Which interfaces does it have?      0   */
    uchar vendor;               /* Which vendors version of the IGS    1   */
    uchar ether0_hw_addr[6];    /* Increment for ethernet 1           2-7  */
    uchar processor;            /* Which processor is this?           8    */
    uchar UNUSED_1[7];          /* Unused (padding)                   9-15 */
    uchar serial[4];            /* BCD packed 8 digit serial number  16-19 */
    uchar UNUSED_2[7];          /* Unused                            20-26 */
    uchar magic[2];             /* Magic number for old cookies      27-28 */
    uchar capabilities[2];      /* capabilities (future stuff)       29-30 */
    uchar version;              /* cookie version number              31    */
};

/*
 * Vendor codes: now in cpu.h
 */


/*
 * Interface codes:
 */
#define INTERFACE_1E1T		1
#define INTERFACE_2E		2
#define INTERFACE_1R1T		3
#define INTERFACE_1R1E		4
#define INTERFACE_2R		5
#define INTERFACE_1E1B		6
#define INTERFACE_1E2T		7
#define INTERFACE_1R2T		8
#define INTERFACE_1E1T1B	9
#define INTERFACE_1R1T1B	10
#define INTERFACE_1E2T1B	11
#define INTERFACE_1R2T1B	12
#define INTERFACE_1R1B		13
#define INTERFACE_1E2T8A	14
#define INTERFACE_1R2T8A	15
/*
 * For Fiji, which is a derivative of Cancun, there will be an ISA
 * bus interface. This interface will be in various combinations with
 * an Ethernet or TR, serial and BRI. To avoid proliferating the no of
 * interface codes, only two new codes will be introduced. These codes
 * will tell us if the board is the Ethernet or TR version with ISA
 * bus and 1 serial interface. The presence of the 2nd serial and
 * BRI will be detected from the daughter card indicator flag in
 * ADRSPC_FIJI_CONTROL. This is being done to allow the daughter card
 * to be field upgradable without having to change the cookie value.
 */
#define INTERFACE_1E1T1I        16    /* I = ISA bus interface */
#define INTERFACE_1R1T1I        17
#define INTERFACE_1R2T1E        18    /* next 3 for campeche */
#define INTERFACE_2R2T          19
#define INTERFACE_2E2T          20
/*
 * Galapagos is hublet derivative of Cancun.  Define two interface
 * codes, and use HUB_MISC_REG to determine if daughtercard is
 * present.
 */
#define INTERFACE_1E2Thub	21
#define INTERFACE_1R2Thub	22
#define INTERFACE_1E2T1Bhub     23    /* cabrillo */ /* SYNALC_ADD */
#define	INTERFACE_1E1Tatmswitch	24		/* for SYNALC */


/*
 * Processor codes:
 */
#define PROCESSOR_PAN		0 /* IGS processor */
#define PROCESSOR_MERLOT	1 /* Token ring IGS processor */
#define PROCESSOR_XX		2 
#define PROCESSOR_BRUT          3
#define PROCESSOR_BASS          4
#define PROCESSOR_CRISTAL       5
#define PROCESSOR_CANCUN        6
#define PROCESSOR_SIERRA        7
#define PROCESSOR_SYNALC        8
#define PROCESSOR_JANEIRO       11
#define PROCESSOR_MAX		10

/*
 * Magic numbers necessary to honor capability[1] bits
 */
#define COOKIE_MAGIC_0          0x5A
#define COOKIE_MAGIC_1          0x69

/*
 * cookie->capabilities[1] bits
 * Features are encoded bitwise in cababilites,
 * In decoding, 0 = include feature; 1 exclude feature:
 *      Bit             Feature
 *      ---             -------
 *       0              Reduced Features a la Bud Lite
 *       1              Bridging
 *       2              TCP IP
 *       3              IPX
 *       4              Decnet
 *       5              Atalk1 and Atalk2
 *       6              Unused
 *       7              Unused
 */
#define REDUCED_FEATURES  0x01 /* Reduced feature set a la Bud Lite */
#define NO_BRIDGING       0x02 /* Bridging only mask */
#define NO_IP             0x04 /* TCP IP only mask */
#define NO_IPX            0x08 /* IPX only mask */
#define NO_DECNET         0x10 /* Decnet only mask */
#define NO_ATALK          0x20 /* Atalk only mask */
 
/*
 * Current cookie capabilites values
 */
#define ENTERPRISE_SW     0x0
#define DESKTOP_SW        0x1
 
/*
 * Cookie decode macro
 * Simple AND returns TRUE if feature is included
 */
#define FEATURE_DECODE(features,mask) ((features & mask) ? TRUE : FALSE)
