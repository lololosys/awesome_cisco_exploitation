/* $Id: lane.h,v 3.5.28.9 1996/08/12 20:05:15 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane.h,v $
 *------------------------------------------------------------------
 * lane.h: LAN emulation exported definitions
 *
 * September 1994, Brad Benson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane.h,v $
 * Revision 3.5.28.9  1996/08/12  20:05:15  cyoung
 * Add support for more than one LES per ELAN.
 * CSCdi62514:  LES for same ELAN on different physical interfaces missing
 * from MIB
 * Branch: California_branch
 *
 * Revision 3.5.28.8  1996/07/30  18:19:51  bbenson
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * First commit of this ddts omitted registry calls out of network.c into
 * the lane subsys.
 *
 * Revision 3.5.28.7  1996/07/30  14:42:39  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.5.28.6  1996/07/26  00:07:44  hampton
 * Add some accessor routines for various IDB values, and make some
 * existing routines use const values.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.5.28.5  1996/06/06  18:38:58  michellw
 * CSCdi59652:  make generic function for creating/deleting LEC
 * Branch: California_branch
 * The functions, lec_create_client() and lec_delete_client(), are
 * created to do the job.
 *
 * Revision 3.5.28.4  1996/05/30  21:32:30  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.5.28.3  1996/05/17  11:25:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.14.2  1996/05/05  23:29:32  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.14.1  1996/04/03  19:54:13  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.28.2  1996/05/09  14:32:12  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.28.1.18.1  1996/04/27  07:06:06  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.28.1.8.7  1996/04/25  22:12:34  cakyol
 * - rename lecsFinderModuleInit to lecs_finder_init, similar
 *   to all the other LANE module init routines.
 * - call it from the appropriate place (lane.c: lane_init).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.28.1.8.6  1996/04/18  01:53:33  cakyol
 * I forgot to add "reg_add_lec_hsrp_action" to lec.c once I
 * removed it from lane.c, during the initialization cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.28.1.8.5  1996/04/16  02:44:06  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.28.1.8.4  1996/04/15  07:26:55  shj
 * CSCdi52538:  IP multicast packets do not fastswitch to LANE
 * Branch: LE_Cal_V112_0_2_branch
 * Allow BUS path to be added to fast cache if destination is multicast.
 * Suppress fast cache only if BUS path will be replaced later with direct
 * path.
 *
 * Revision 3.5.28.1.8.3  1996/04/12  01:52:35  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Rip C7000 support of Token Ring LANE until ucode is ready
 * - Modify indentation futher to placate the masses
 *
 * Revision 3.5.28.1.8.2  1996/04/08  14:51:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.28.1.8.1  1996/04/08  01:56:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.28.1  1996/03/18  20:32:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.32.4  1996/04/06  02:52:17  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 * Revision 3.5.32.3  1996/04/05  21:28:55  cakyol
 * Combine all the special atm printing routines into a
 * single one (atm_related_printf), saving code space.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.32.2  1996/04/05  03:20:15  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.32.1  1996/03/22  22:53:50  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.18.1  1996/03/05  06:42:34  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.12.2  1996/02/29  22:16:08  cakyol
 * correct some slight merge errors and clean up stuff related
 * to config access, which somehow must not have been totally
 * cleaned before.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.12.1  1996/02/27  21:09:56  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.14.3.2.3  1996/03/23  21:12:06  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 *
 * Revision 3.3.14.3.2.2  1996/03/21  03:26:03  ddecapit
 * Branch: TRlane_branch
 *
 * RSP Support
 * Code Review Comments
 *
 * Revision 3.3.14.3.2.1  1996/03/05  04:46:21  ddecapit
 * Branch: TRlane_branch
 * Token Ring B-LLI for LAN Emulation
 * Fix copyright
 *
 * Revision 3.3.14.3  1996/01/23  23:53:58  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.2  1996/01/19  19:36:52  cakyol
 * - lock a config table to make it accessible by only a
 *   single session at a time (this was bug CSCdi47155
 *   in 11.0 and 11.1)
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.1  1996/01/11  01:46:03  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:06:26  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/20  18:35:53  rbadri
 * CSCdi38662:  Configuring bridging on multiple ATM sub-interfaces causes
 *              hardware reset
 * When a sub-interface is added to the bridge-group, initiate a config
 * change rather than resetting the ATM interface.
 *
 * Revision 3.2  1995/11/17  17:50:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:12:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/17  22:34:52  cakyol
 * CSCdi37207:  auto addresses should be displayed without running the
 * components
 * - fixed.  The interface/subinterface command "lane
 * default-atm-addresses"
 *   now displays all would be auto addresses for all the lane components
 *   on the specific interface.
 *
 * Revision 2.1  1995/06/07  21:18:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LANE_H_
#define _LANE_H_

#include "../if/atm.h"

/* 
 * Defines and constants 
 */
#define 	MAX_ELAN_NAME_LENGTH	32
#define		LANE_MARKER_MASK	0xFF00
#define		LANE_MARKER		0xFF00
#define		LANE_LECID_BYTES	2
#define		LANE_LECID_WORDS	1
#define		LANE_NETHASHLEN		256

/*
 * This struct is allocated per swidb.  Retains LANE information.
 * The lec/les/lecs structs are malloc'ed as-needed to save space.
 */
struct lane_info_t_ 
{
    struct lane_info_t_		*next;
    idbtype			*swidb;

    /*
    ** This data is valid only on the main interface
    ** (subinterface 0).  It is NULL on every other
    ** subinterface.
    **
    ** The reason this is (void *) is becoz its 
    ** type is hidden for other people, noone
    ** who uses lane_info except the lecs finder
    ** process needs to know about it.
    */
    void *lecsFinderContext;

    /*
    ** the "definitive" LECS address that should be
    ** used by ** every ** LANE component.  This has
    ** been determined as a result of the LECS finder
    ** process.  This is valid for ** every ** 
    ** subinterface.  This is where ** every ** LANE 
    ** component should pick up the "valid" LECS address 
    ** from.  It is always prepared and ready to be used.
    */
    hwaddrtype masterLecsAddress;

    /*
    ** the two fields below keep the "MANUALLY"
    ** entered LECS nsap address.  The variable "type"
    ** retains whether it is auto, fixed or other.
    ** It is also used in the nv generation.  These
    ** are valid on ** every ** subinterface.
    */
    int lecs_nsap_type;
    MaskedAddress lecs_nsap;

    /* bus and server addresses */
    MaskedAddress		bus_nsap;
    MaskedAddress		lsv_nsap;

    /*
     * For mac-address-plan commands AND for subinterface 0 (main
     * interface) ONLY.  The "mac_plan_type" values are defined in the
     * parser_defs_lane.h file.
     */
    int 			mac_plan_type;
    uchar 			mac_plan_base [STATIONLEN_IEEE48];

    uchar			type;
    uchar			max_data_frame;
    uchar			name_size;
    uchar			name[MAX_ELAN_NAME_LENGTH+1];

    /* TRUE == structure being destroyed */
    boolean			dead:1;

    /* type set: server command */
    boolean			type_from_les:1;

    /* type set: client command */
    boolean			type_from_lec:1; 

    /* name set: server command */
    boolean			name_from_les:1;

    /* name set: client command */
    boolean			name_from_lec:1; 

    /* TRUE if the client is configured on the subinterface. */
    boolean			lec_enabled:1;

    /* pointers to individual LANE entities */
    struct lec_info_t_		*lec_info;
    struct lsv_info_t_		*lsv_info;
    struct lecs_info_t_		*lecs_info;
    struct lane_vcc_info_t_     *signal_chain;
};

/*
 * Define the different LANE VCC types that are possible.  These are
 * used to identify *each* end of a particular VCC.  Numbers are from
 * Figure 3 in the LANE Client Functional Spec.
 */
typedef enum lane_vc_type_t_ {
    LANE_VC_TYPE_UNKNOWN,        /* 0 means unknown                     */
    LANE_LEC_DIRECT,		 /* 1 LEC Control Direct		*/
    LANE_LEC_DISTRIBUTE,	 /* 2 LEC Control Distribute     	*/
    LANE_LEC_CONFIGURE,		 /* 3 LEC Configure	        	*/
    LANE_LEC_SEND,		 /* 4 LEC Multicast Send		*/
    LANE_LEC_FORWARD,		 /* 5 LEC Multicast Forward     	*/
    LANE_LEC_DATA,		 /* 6 LEC Data		        	*/
    LANE_LES_DIRECT,		 /* 7 LES Control Direct		*/
    LANE_LES_DISTRIBUTE,	 /* 8 LES Control Distribute    	*/
    LANE_BUS_SEND,		 /* 9 BUS Multicast Send         	*/
    LANE_BUS_FORWARD,		 /* 10 BUS Multicast Forward    	*/
    LANE_CONFIGURE,		 /* 11 Configure			*/
    LANE_LEC_TR_DATA,            /* 12 802.5 LEC Data direct            */
    LANE_LEC_TR_SEND,            /* 13 802.5 LEC mcast send             */
    LANE_LEC_TR_FORWARD,         /* 14 802.5 LEC multicast forward      */
    LANE_BUS_TR_SEND,            /* 15 802.5 BUS multicast send         */
    LANE_BUS_TR_FORWARD,         /* 16 802.5 BUS multicast forward      */
    LANE_MAX_VC_TYPE = LANE_BUS_TR_FORWARD,
} lane_vc_type_t;

/*
 * Action codes used by fastswitching
 */
typedef enum {
    LANE_NORMAL,
    LANE_ALREADY_DONE,
    LANE_DO_NOT_ROUTE,
    LANE_TO_PROCESS,
} LANE_FAST_ACTION;

/*
 * Macros used by fastswitching code
 */
#define IS_ETHER_UNICAST(mac) ((mac[0] & 0x1) == 0)

  /* mac's are stored canonically; however they'll come off the line
   * in noncanonical format; the following 2 macros assume you
   * are checking off the line (not in storage)                      */
#define IS_TRING_UNICAST(mac) ((mac[0] & TRING_MAC_MULTICAST) == 0)
 
#define IS_TRING_RI_BCAST(mac,rc_1) (((mac[0] & TR_RII) != 0) &&   \
                                     ((rc_1 & RC_BROADBIT) != 0)) 
 

#define IS_LANE_CTL(marker) ((marker & LANE_MARKER_MASK) == LANE_MARKER)

#define LANE_MAC_DIFF(a, b)                                     \
 (  (GETLONG((uchar *)a)     !=  GETLONG((uchar *)b)) ||        \
   (GETSHORT((uchar *)a + 4) != GETSHORT((uchar *)b + 4)))

#define LANE_MAC_SAME(a, b) \
    ((GETLONG((uchar*)a) == GETLONG((uchar*)b)) && \
     (GETSHORT((uchar*)a+4) == GETSHORT((uchar*)b+4)))

/* Exported typedefs */
typedef ushort lec_id_t;

typedef enum lane_status_t_ {
    LANE_ACTIVE		= 1,
    LANE_INACTIVE	= 2,
} lane_status_t;

typedef enum lane_caller_t_ {
    LANE_CALLER_SNMP	= 1,
    LANE_CALLER_CLI	= 2,
} lane_caller_t;

typedef enum lsv_admin_down_code_t_ {
    LSV_DOWN_SHUTDOWN,		/* shutdown, SNMP admin down */
    LSV_DOWN_NO_LANE,		/* no lane server-bus */
    LSV_DOWN_ERASE		/* no int atm1/0[.5] */
} lsv_admin_down_code_t;

/* globals */
extern queuetype lane_infoQ;

/*
 * LAN Emulation MTU size
 */
#define LANE_MTU_ETHER_1516	1500
#define LANE_MTU_ETHER_4544	4528
#define LANE_MTU_ETHER_9234	9218  /* not supported due to AIP microcode */
#define LANE_MTU_ETHER_18190	18174 /* not supported due to AIP microcode */

#define LANE_MTU_TR_1516	1462
#define LANE_MTU_TR_4544	4490
#define LANE_MTU_TR_9234	9180
#define LANE_MTU_TR_18190	18136 /* not supported due to AIP microcode */

/*
 * Static inline used by other modules to determine if a client is
 * configured on the subinterface.
 */
static inline
boolean lec_enabled (const idbtype *swidb)
{
    if (swidb->lane_info && swidb->lane_info->lec_enabled) {
	return(TRUE);
    }
    return(FALSE);
}

/*
 * Inline to determine if LES/BUS is configured on the sub-interface.
 */
static inline 
boolean les_bus_enabled (const idbtype *swidb)
{
    if (swidb->lane_info && swidb->lane_info->name_from_les) {
	return(TRUE);
    }
    return(FALSE);
}

static inline boolean is_atm_lane (const idbtype *swidb)
{
    return (is_atm(swidb->hwptr) && lec_enabled(swidb));
}

/* 
 * if_lane_exist
 *
 * Checks whether LEC/LES/BUS is configured on this sub-interface.
 */
static inline boolean if_lane_exist (idbtype *swidb)
{
    if (lec_enabled(swidb) || les_bus_enabled(swidb)) {
	return(TRUE);
    }
    return(FALSE);
}

/*
 * Inline to determine if a subinterface is on ATM and LANE is NOT enabled.
 */
static inline boolean  is_atm_nolane (const idbtype *swidb)
{
    return (is_atm(swidb->hwptr) && !lec_enabled(swidb));
}

/* parser and general */
extern char *atm_masked_nsap_short_help(parseinfo *csb, uint flag,
    idbtype *idb);
extern boolean match_masked_atm_nsap(parseinfo *csb, char *buf, 
    int *pi, void *addr_pointer, int in_help, uint flag, 
    idbtype *idb);
extern void lane_interface_commands(parseinfo *parse);
extern lane_info_t *lane_init_idb(idbtype *swidb);
extern void lane_show_commands(parseinfo *parse);
extern boolean is_valid_lane_mtu(idbtype *swidb, ushort mtu);
extern boolean if_lane_exist(idbtype *swidb);
extern unsigned lane_default_mtu(idbtype *swidb);
extern unsigned lane_default_frame_size(idbtype *swidb);

/* lec stuff */
extern boolean lec_enqueue(paktype *pak);
extern boolean lec_find_vc(lane_info_t *lane_info, uchar *dest_mac, 
                           uchar *src_mac, vcd_t *vcd, boolean *cache_ok);
extern void lec_cpcs_lane(paktype *pak);
extern void lec_interface_updown(idbtype*, hwidbtype*);

/* lecs finder stuff */
extern void lecs_finder_init(void);

/* lecs stuff */
extern boolean lecs_init(void);
extern boolean lecs_enqueue(paktype *pak);
extern void *cfgtable_context_exit (parseinfo *csb);
extern void cfgtable_context_enter (parseinfo *csb, void *var);
extern void lecs_interface_updown(idbtype*, hwidbtype*);

/* les stuff */
extern void lsv_init(void);
extern boolean lsv_enqueue(paktype *pak);
extern struct lsv_info_t_ *lsv_create_lesbus(idbtype *, int lan_type,
					     int name_size, uchar *,
					     lane_caller_t, ushort
					     segment_id, ulong lesIndex);
extern boolean lsv_admin_up(struct lsv_info_t_ *);
extern boolean lsv_admin_down(idbtype *, lsv_admin_down_code_t);
extern void lesbus_interface_updown(idbtype*, hwidbtype*);

#endif _LANE_H_

