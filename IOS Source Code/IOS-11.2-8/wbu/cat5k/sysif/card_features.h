/* $Id: card_features.h,v 1.1.4.1 1996/05/09 15:01:44 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/card_features.h,v $
 *-----------------------------------------------------------------
 * card_features.h -- header file for card feature/config lookup table
 *
 * January 1995, Steve Garcia
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: card_features.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:44  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:50  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:33  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:48  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.18  1995/10/04  01:07:24  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.17  1995/09/12  21:21:26  jchiou
 * Changed the card feature index of new Capri card from 9 to 5 and removed the unused 24-port 10-baseT MPR.
 *
 * Revision 1.16  1995/09/08  01:30:40  jkuo
 * Added E_48X10BT_MPR entry for new Capri card.
 *
 * Revision 1.15  1995/09/06  23:16:01  jkuo
 * Added entry for E_12X10X100_MII (10/100) card.
 *
 * Revision 1.14  1995/08/26  01:16:22  sgarcia
 * No function changes.  Just move MAX CARD so it's more obvious it needs to
 * be modified when new card indexes are added.
 *
 * Revision 1.13  1995/08/18  02:05:31  jkuo
 * Changed MAX_CARD & added E_12X100BF.
 *
 * Revision 1.12  1995/06/14  00:16:01  sgarcia
 * For 8051:
 * card_features.h: added MAC_MODE_MASK
 * scp_msgs.h: added SCP_HDR_DATA_START_OFFSET
 *
 * Revision 1.11  1995/04/28  18:27:23  jkuo
 * Start using byte 5 for MAC_SPEED bits (2) for Saint MAC_MODE.
 * These two bits do not apply to SAGE.
 *
 * Revision 1.10  1995/04/20  17:38:30  sgarcia
 * Added table entry for 12 port 10BaseF card.
 *
 * Revision 1.9  1995/04/04  21:23:34  sgarcia
 * Added UNKNOWN_CARD entry for cases when feature_index is unavailable
 * from the serial eeprom.
 *
 * Revision 1.8  1995/03/02  22:01:47  iris
 * add mask for XRAM2; use mask to generate databit (byte6) in cardfeaturetable
 *
 * Revision 1.7  1995/03/02  02:23:58  jkuo
 * Added bit def for XRAM2 exists
 *
 * Revision 1.6  1995/02/07  21:25:06  sgarcia
 * Added media_type to CardFeatureTable.c/h.
 *
 * Revision 1.5  1995/02/04  04:39:04  iris
 * Sorry guys.  utypes.h is not for NMP.  Remove include
 * changed uchar to unsigned char - harder to read but everyone will compile.
 *
 * Revision 1.4  1995/02/04  04:15:35  iris
 * include utypes.h to get rid of compile error due to uchar
 *
 * Revision 1.3  1995/01/23  22:01:53  sgarcia
 * Extern CardFeatureTable.
 *
 * Revision 1.2  1995/01/19  21:56:32  sgarcia
 * Updated to reflect new serial eeprom format and new design for accessing
 * feature data.
 *
 * Revision 1.1  1995/01/18  19:02:22  sgarcia
 * h file for card_features.c
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __card_features_H__
#define __card_features_H__

/*
 * CardFeatureTable
 * 
 * Table format:
 *   Rows indexed by card_feature_index.
 *   Column format:
 *     byte 0: number of MAC addresses allocated, hi byte
 *     byte 1: number of MAC addresses allocated, lo byte
 *     byte 2: number of SAGEs on card
 *     byte 3: number of SAINTs on card
 *     byte 4: number of ports on card
 *     byte 5: bit data, hi byte
 *       bit  7: reserved for expansion
 *       bit  6: unused
 *       bit  5: unused
 *       bit  4: unused
 *       bit  3: unused
 *       bit  2: unused
 *       bit  1: EtherNet Mac Mode MII	 (1 = ON)
 *       bit  0: EtherNet Mac Mode 100Mbs(1 = 100, 0 = 10)
 *     byte 6: bit data, lo byte
 *       bit  7: unused
 *       bit  6: XRAM2 exists		 (1 = TRUE)
 *       bit  5: unused
 *       bit  4: dual port ram exists    (1 = TRUE)
 *       bit  3: CBL exists              (1 = TRUE)
 *       bits 2-0: number of SAINT banks: 3 bits
 *     byte 7: lan type
 */


/*
 * define table size
 */
#define CARD_FEATURE_TABLE_BYTES   8  /* width in bytes of lookup table
				       * Must match CRD_TABLE_WIDTH in
				       * 8051/common/startup2.s */

/*
 * identify table columns
 */
#define NUM_MACS_HI_COL       0
#define NUM_MACS_LO_COL       1
#define NUM_SAGES_COL         2
#define NUM_SAINTS_COL        3
#define NUM_PORTS_COL         4
#define BIT_DATA_HI_COL       5
#define BIT_DATA_LO_COL       6
#define LAN_TYPE_COL          7


/*
 * AND masks to clear unwanted bits from table's bit data
 */
/* hi byte of bit data */
#define EXPAND_TABLE_MASK     0x80
#define MAC_MODE_MASK         0x03
/* lo byte of bit data */
#define DPR_EXISTS_MASK       0x10
#define CBL_EXISTS_MASK       0x08
#define NUM_SAINT_BANKS_MASK  0x07
#define XRAM2_EXISTS_MASK     0x40


/*
 * Indexes into the CardFeatureTable
 */
#define NMP_MCP              0
#define ATM_CARD             1
#define FDDI_CARD            2        /* monterey */
#define E_24X10BT            3        /* 24 port 10-baseT */
#define E_12X100BT           4        /* 12 port 100-baseT */
#define	E_48X10BT_MPR	     5	      /* 48 port 10-baseT 4-segment repeater */
#define E_12X10BF            6        /* 12 port 10-baseF */
#define E_12X100BF           7        /* 12 port 100-baseF */
#define	E_12X10X100_MII	     8	      /* 12 port 10/100 MII */
#define MAX_CARD             9        /* Increment when adding card types.
                                       * Used to error check card type */
#define UNKNOWN_CARD         MAX_CARD /* unknown */


#define INVALID_CARD         255      /* Do not use.  Save for future
                                       * expansion */

/*
 * Defines for lan type field
 */
#define	 TYPE_ETHERNET		0x00
#define	 TYPE_TOKENRING		0x01
#define	 TYPE_FDDI		0x02
#define	 TYPE_ATM		0x03


/*
 * Defines for EtherNet speed field
 */
#define	 MAC_NOT_APPLY		0x00  /* Sage - No config for speed */
#define  MAC_SPEED_10		0x00
#define  MAC_SPEED_100		0x01
#define  MAC_SPEED_MII		0x02


extern const unsigned char CardFeatureTable[MAX_CARD+1][CARD_FEATURE_TABLE_BYTES];

#endif  /* ifndef __card_features_H__ */











