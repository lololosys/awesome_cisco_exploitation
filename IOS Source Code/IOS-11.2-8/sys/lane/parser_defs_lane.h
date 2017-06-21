/* $Id: parser_defs_lane.h,v 3.2.58.1 1996/05/09 14:33:28 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/parser_defs_lane.h,v $
 *------------------------------------------------------------------
 * lane/parser_defs_lane.h: LANE parsing specific constants
 *
 * October 1994, Chris Parrott
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_lane.h,v $
 * Revision 3.2.58.1  1996/05/09  14:33:28  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:07:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.5  1996/04/26  00:46:14  michellw
 * Branch: LE_Cal_V112_0_2_branch
 * Support two new commands "lane client idle IDLE-TIME" and
 * "clear lane client join-cnt".  Print the LEC uptime and
 * lec join-attempt-count in the "show lan client" display.
 *
 * Revision 3.2.76.4  1996/04/16  02:45:25  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.76.3  1996/04/08  20:47:11  cakyol
 * sync corrections:
 * - value "LANE_LECS_BIND_NSAP" should not be there.
 * - parser user errors should emit a "%" as the first character.
 * - white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.76.2  1996/04/08  14:52:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.76.1  1996/04/08  01:57:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.2  1996/03/27  22:58:16  cakyol
 * Added some more 11.2 backward compatibility code that I missed.
 * This involves the "name NAME server-atm-address ADDRESS" lane
 * database command.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.64.1  1996/03/22  22:55:46  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.2  1996/03/05  06:43:50  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/03  21:23:46  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.46.1  1996/03/02  01:19:49  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:11:07  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.16.1  1996/02/29  07:49:07  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.3  1996/02/22  09:05:48  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.2  1996/02/06  22:19:52  cakyol
 * cleanup for the last code review:
 * - get rid of "#ifdef PARANOID...#endif".  Leave some in
 *   permanently, take some out completely.
 * - get rid of all "if 0....endif" type constructs.
 * - debug messages speeded up by using variable argument
 *   macro expansion.
 * - most fatal type errors are converted to crashdump with
 *   __FILE__ and __LINE__ printing.
 * - in the LECS, do *NOT* update the table of global LECS
 *   addresses, if any outstanding signalling requests remain.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:47:03  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:07:16  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:51:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3.46.1  1996/02/03  07:07:23  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.3.28.1.2.1  1996/01/19  21:56:58  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.3.28.2  1995/11/07  01:29:42  bbenson
 * CSCdi43138:  Can assign an ELAN to a reserved VLAN
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Disallow VLANs 1001 through 1023 for Synergy.
 *
 * Revision 2.3.28.1  1995/09/11  01:56:27  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.3.12.1  1995/09/02  01:58:30  bbenson
 * Branch: Synalc2_branch
 * Sync in LANE for the new improved Synalc branch.
 *
 * Revision 1.1.2.1.2.2  1995/07/25  06:02:39  bbenson
 * Branch: Synergy_ATMlc_branch
 * - Sync'd LANE code on Synergy_ATMlc_branch up to V110_0_12 on mainline
 *   from the head of the LE_branch (last time it was sync'd).  Rest of
 *   the Synergy branch is left as-is.
 *
 * Revision 1.1.2.1.2.1  1995/06/08  19:27:46  bbenson
 * Branch: Synergy_ATMlc_branch
 * Add EARL ipc.
 *
 * Revision 2.3  1995/07/18  01:09:49  cakyol
 * CSCdi37230:  lane default-atm-address command should be a show command
 * - fixed.  Here is the summary:
 *
 *         show lane default-atm-address
 *                 prints the possible lane components on ALL atm
 *                 interfaces.
 *
 *         show lane default-atm-address interface atm1/0
 *                 prints the possible lane components on the
 *                 specified main-interface.
 *
 *         show lane default-atm-address interface atm1/0.1
 *                 prints the possible lane components on the
 *                 specified sub-interface.
 *
 * Revision 2.2  1995/07/13  01:07:16  cakyol
 * CSCdi37016:  lecs access policy should allow for restricted elans
 * - done.  The main point is that restricted elans cannot now be
 *   joined by name only.  Explicit atm address or mac address
 *   is required.
 *
 * Revision 2.1  1995/06/07  21:20:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef parser_defs_lane_h
#define parser_defs_lane_h

/*******************************************************************
** clear commands 
*/

enum lane_clear_enums {
    CLEAR_LANE_LES_MAC,		/* choose LEC by MAC addr */
    CLEAR_LANE_LES_NSAP,	/* choose LEC by ATM addr */
    CLEAR_LANE_LES_LECID,	/* choose LEC by lecid */
    CLEAR_LANE_LES_ALL,		/* choose all LECs */
    CLEAR_LE_ARP,               /* clear le-arp */
    CLEAR_LANE_LEC_JOIN_CNT_INT,/* clear LEC join counter by interface */
    CLEAR_LANE_LEC_JOIN_CNT_ALL,/* clear all LEC's join counters */
    CLEAR_LANE_LES_RD      /* choose LEC by route descriptor */
};

enum lane_clear_le_arp_mac {
    CLEAR_LE_ARP_NO_MAC,        /* no MAC on command */
    CLEAR_LE_ARP_MAC,           /* specific MAC given */
};

enum lane_clear_le_arp_qual {
    CLEAR_LE_ARP_ALL,           /* no name or int qualifier */
    CLEAR_LE_ARP_BY_INT,        /* qualified by interface   */
    CLEAR_LE_ARP_BY_NAME,       /* qualified by elan name   */
};

/*******************************************************************
** show commands 
*/

/* flags in OBJ(int, 2) on "show lane ..." */

#define SHOW_LANE_BRIEF         0x1
#define SHOW_LANE_INT           0x2
#define SHOW_LANE_NAME          0x4
#define SHOW_LANE_SPECIFIC      0x8

enum lane_show_enums {

    /* for "show lane ENTITY */
    SHOW_LANE_ENTITY,           /* for csb->which */
    SHOW_LANE_ALL,              /* OBJ(int, 1) for "show lane" */
    SHOW_LANE_SERVER,           /* OBJ(int, 1) for "show lane server" */
    SHOW_LANE_BUS,              /* OBJ(int, 1) for "show lane bus" */

    /* show default addresses */
    SHOW_LANE_ADDRESSES,	/* no interface specified */
    SHOW_LANE_ADDR_INT,		/* a specific interface specified */

    /* for the lec */
    SHOW_LANE_LE_ARP,		/* Show entire le-arp table */
    SHOW_LANE_LEC,		/* Show client */

    /* for the lecs */
    SHOW_LANE_LECS,		/* show lecs connexions */
    SHOW_LANE_CFGTBL,		/* show lecs config table */
};

/*******************************************************************
** global config commands 
**
** NOTE: Currently, only the LECS has global configuration.
*/

enum lane_global_config_enums {
    LANE_LECS_CMD_CFGTBL,		/* create config table */

    LANE_LECS_CMD_ELAN_CREATE, 		/* create/modify an elan */
    LANE_LECS_CMD_ELAN_RENAME,		/* rename an elan */
    LANE_LECS_CMD_ELAN_SETR,		/* set restricted status */
    LANE_LECS_CMD_ELAN_SET_TIMEOUT,	/* LES timeout */
    LANE_LECS_CMD_ELAN_SET_SEGMENT, /* local segment id for TR ELAN */

    LANE_LECS_CMD_ADD_MAC,      	/* add a mac addr to elan */
    LANE_LECS_CMD_ADD_NSAP,     	/* add nsap addr to elan */
    LANE_LECS_CMD_DFLT,         	/* set default elan */

    LANE_LECS_SAA_DELETE,		/* delete LES address */

    LANE_LECS_CFGTBL_DISPLAY,   	/* display tables */
    LANE_LECS_NSAP_TEST,
    LANE_LECS_CMD_ADD_RD         /* add route descriptor to ELAN */
};

/*
** change an elan status
*/
typedef enum ElanRestrictedStatus
{
    ELAN_NO_CHANGE,
    ELAN_NOT_RESTRICTED,
    ELAN_RESTRICTED,

} ElanRestrictedStatus;

/*
** lowest possible prio wildcard for an LES address.
** This is used for LES/LECS redundancy & fault tolerance.
*/
#define LOWEST_PRIO	(-1)

/******************************************************************
** Interface specific commands
*/

enum lane_interface_config_enums {

    /* mac plan commands */
    LANE_MACPLAN_SAME,		/* mac plan same, old address */
    LANE_MACPLAN_SAME_NEWADDR,	/* mac plan same, new address specified */
    LANE_MACPLAN_DIFF,		/* mac plan different, old address */
    LANE_MACPLAN_DIFF_NEWADDR,	/* mac plan different, new addr specified */

    /* for the lecs */
    LANE_LECS_BIND_TABLE,	/* bind cfg table to i/f */
    LANE_LECS_BIND_ADDR,	/* generic address bind */
    LANE_LECS_UNBIND_ADDR,	/* unbind ONE address */
    LANE_LECS_UNBIND_ALL_ADDR,	/* unbind ALL addresses */
    LANE_LECS_BIND_FIXED,	/* bind fixed addr to i/f */
    LANE_LECS_BIND_AUTO,	/* bind auto addr to i/f */
    LANE_LECS_GLOBAL_ADDR,	/* add a global LECS address */
    LANE_GET_LECSADDRS_PERIOD,	/* resync to switch, in seconds */

    LANE_CMD_LEC,		/* Activate the LANE Client */
    LANE_CMD_LECS_NSAP,		/* Configure LECS nsap */
    LANE_CMD_LEC_NSAP,		/* Configure LEC nsap */
    LANE_CMD_LEC_SRB,		/* Configure LEC SRB */
    LANE_CMD_BUS_NSAP,		/* Configure BUS nsap */
    LANE_CMD_LES_NSAP,		/* Configure LES nsap */
    LANE_CMD_LES_BUS,		/* Activate LES & BUS */
    LANE_CMD_LE_ARP,		/* Add to the LE-ARP table */
    LANE_CMD_IDLE_TIME,  	/* Set LEC Idle Timer */
};

/*******************************************************************
** Other definitions used with the LANE Commands 
*/
 
/* Emulated LAN types */
#define	LANE_TYPE_ETHERNET	1	/* Ethernet Emulation */
#define	LANE_TYPE_TOKEN_RING    2	/* Token Ring Emulation */

#define	LANE_MAX_LECID		4096	/* Max LEC ID number */
#define LANE_MAX_VLANID		1000	/* Max Synergy VLANID */

#define	LEC_KWD_MATCH_CNT	4	/* "lec-xxxx" must match this many chars */

#define  LANE_MIN_SEGMENTID 	1		/* minimum segment id */
#define  LANE_MAX_SEGMENTID   4095	/* max valid segment id */
#define  LANE_MIN_BRIDGENUM 	1		/* minimum bridge number */
#define  LANE_MAX_BRIDGENUM	15		/* max valid bridge number */
#endif /* parser_defs_lane_h */

