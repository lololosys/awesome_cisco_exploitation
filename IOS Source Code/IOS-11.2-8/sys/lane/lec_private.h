/* $Id: lec_private.h,v 3.4.6.13 1996/09/12 18:28:02 michellw Exp $
 * $Source: /release/112/cvs/Xsys/lane/lec_private.h,v $
 *------------------------------------------------------------------
 * lec_private.h: LAN Emulation Client definitions
 *
 * July 1994, Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_private.h,v $
 * Revision 3.4.6.13  1996/09/12  18:28:02  michellw
 * CSCdi61979:  ATM: mismatch of known Vcd cause applications to block
 * Branch: California_branch
 * For lec_delete_nsap_entry(), passed in the pointer to the
 * lec_nsap_info_t to be deleted instead of the pointer to
 * the nsap address.
 *
 * Revision 3.4.6.12  1996/09/09  22:49:28  michellw
 * CSCdi68089:  LANE data direct down; Resolution: backout CSCdi61979
 * Branch: California_branch
 * Backout the fix committed for CSCdi61979
 *
 * Revision 3.4.6.11  1996/08/22  20:32:06  michellw
 * CSCdi61979:  ATM: mismatch of known Vcd cause applications to block
 * Branch: California_branch
 * For loopback VC, there will be two entries in lec_info->nasp_table
 * with the same nsap address. Use callid to uniquely locate the entry.
 *
 * Revision 3.4.6.10  1996/07/30  14:43:00  michellw
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
 * Revision 3.4.6.9  1996/07/29  05:26:59  bbenson
 * CSCdi64313:  Pull out dead code dealing with HSRP MAC addresses.
 * Branch: California_branch
 *
 * Revision 3.4.6.8  1996/07/18  18:00:50  bbenson
 * CSCdi63359:  jittered timer needs to be incorporated in
 * lec_start_arp_age_timer
 * Branch: California_branch
 *
 * Revision 3.4.6.7  1996/06/19  18:58:12  michellw
 * CSCdi60043:  Apply jitter to the LEC timer
 * Branch: California_branch
 * Jitter lec timer up to 20 percent.
 *
 * Revision 3.4.6.6  1996/06/18  01:32:29  cyoung
 * CSCdi60671:  tr lane support for VTP
 * Branch: California_branch
 *
 * Revision 3.4.6.5  1996/06/06  18:39:14  michellw
 * CSCdi59652:  make generic function for creating/deleting LEC
 * Branch: California_branch
 * The functions, lec_create_client() and lec_delete_client(), are
 * created to do the job.
 *
 * Revision 3.4.6.4  1996/05/30  21:33:16  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.4.6.3  1996/05/17  23:26:44  rbadri
 * CSCdi56574:  LANE clients go down/up during 500 station multiping
 * Branch: California_branch
 * Flush requests are to be sent in the context of the client process
 * rather than signalling process. Throttle the number of requests to be
 * sent and be generous about giving up the CPU.
 *
 * Revision 3.4.6.2  1996/05/09  14:33:02  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.6.1.18.1  1996/04/27  07:06:53  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.6.1.8.7  1996/04/26  00:46:12  michellw
 * Branch: LE_Cal_V112_0_2_branch
 * Support two new commands "lane client idle IDLE-TIME" and
 * "clear lane client join-cnt".  Print the LEC uptime and
 * lec join-attempt-count in the "show lan client" display.
 *
 * Revision 3.4.6.1.8.6  1996/04/17  23:14:58  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.4.6.1.8.5  1996/04/16  02:44:57  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.6.1.8.4  1996/04/13  23:13:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * - Don't call tr_lane_start, llc2_start or start_srb_background unless
 *   tr-lane is configured.
 * - First shot at calling lec_set_maxmtu to set the tr-lane MTU.
 * - Remove lane_type from lec_muex_config.  It's not needed.
 * - Only do the tr-lane config of srb when tr-lane is configured.
 *
 * Revision 3.4.6.1.8.3  1996/04/09  02:16:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.4.6.1.8.2  1996/04/08  14:52:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4.6.1.8.1  1996/04/08  01:57:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.6.1  1996/03/18  20:32:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.10.5  1996/04/06  02:52:37  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 * Revision 3.4.10.4  1996/04/03  02:39:40  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * Client state machine is event driven instead of packet driven.
 *
 * Revision 3.4.10.3  1996/03/28  17:34:48  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.4.10.2  1996/03/23  04:54:55  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.10.1  1996/03/22  22:55:18  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4  1996/03/04  19:26:17  rbadri
 * CSCdi50636:  LANE client lacks a Flush timer
 * Add a flush timer to the LE_ARP entry. When it pops, switch to
 * data direct. If the flush response comes back before the timer pops,
 * then stop the timer and use the data direct VC.
 *
 * Revision 3.3  1996/02/28  03:03:43  rbadri
 * CSCdi49394:  CDP can not be enabled on LANE interfaces
 * Enable CDP on ATM interfaces, when LANE is configured and
 * when the LANE client is operational.
 *
 * Revision 3.2.48.3  1996/03/08  01:10:31  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix router/Synergy conflicts.
 *
 * Revision 3.2.48.2  1996/03/05  06:43:25  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/03  21:23:40  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:10:42  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.4  1996/02/16  21:38:27  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Scheduler interface changes for LANE client code.
 *
 * Revision 3.2.14.3  1996/01/24  01:37:01  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.14.2  1996/01/23  23:54:30  cakyol
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
 * Revision 3.2.14.1  1996/01/11  01:46:41  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:58  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:51:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:12:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10.8.1.2.1  1996/02/03  07:07:19  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.10  1995/08/31  17:08:15  bbenson
 * CSCdi39584:  set sdu sizes per connection - part 1
 * - Change of the parm from ulong to ushort.
 *
 * Revision 2.9  1995/08/31  02:08:38  bbenson
 * CSCdi39584:  set sdu sizes per connection - part 1
 * - Staging of the LANE/LEC part of this fix.
 *
 * Revision 2.8.16.2.2.1  1996/01/19  21:56:56  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.8.16.4  1995/12/01  16:01:26  bbenson
 * CSCdi44768:  TOPO change debug message should output on toggle
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Added new debug switch to the client for topo debugging.
 *
 * Revision 2.8.16.3  1995/11/07  01:34:28  bbenson
 * CSCdi43159:  Unresolved LE-ARPs never stop ARPingq
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Removed autodecrement of arp_retry_count.
 *
 * Revision 2.8.16.2  1995/09/27  23:27:05  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * - Further buff and polish of setting ACTIVE SAR state.
 * - Moving Topology change, SNMP, proxy, and flush code into Synergy.
 *
 * Revision 2.8.16.1  1995/09/11  01:56:25  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.8  1995/08/14  19:14:37  bbenson
 * CSCdi35018:  AS should be turned off when 1577 and LANE are on the same
 * AIP.
 *
 * Revision 2.7  1995/08/06  21:59:56  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.6  1995/07/30  01:15:56  bbenson
 * CSCdi37903:  Not necessary to invalidate caches immediately for LANE.
 * - No more calls to reg_invoke_bump_cache_version with an immediate
 *   request.
 * - Also collapsed numerous message pointers in the lec_info_t into a
 *   single union
 *
 * Revision 2.5  1995/07/12  07:28:37  bbenson
 * CSCdi36903:  LEC needs to align show lane output with other components.
 * CSCdi36945:  LEC doesn't handle RELEASE COMPLETE in idle state.
 *
 * Revision 2.4  1995/07/11  05:27:32  bbenson
 * CSCdi36864:  No need to display UPDOWN message periodically.
 * CSCdi36904:  LEC should retransmit JOIN requests C13_max_retry_count
 * times.
 * CSCdi36837:  LEC doesnt recycle when LES/BUS/LECS address changes.
 *
 * Revision 2.3  1995/06/16  06:06:52  bbenson
 * CSCdi35603:  router hangs after displaying NO BLOCK error message
 *  - Remove unnecessary call to reg_invoke_clear_routes.
 *
 * Revision 2.2  1995/06/08  20:31:18  bbenson
 * CSCdi35434: LEC doesnt take the new ELAN name
 * - Bring in fresh copy out of lane_info every time we come up.
 * CSCdi35445: During state LECS_CONNECT abnormally received
 *             RELEASE_COMPLETE
 * - Handle RELEASE_COMPLETE in LECS_GET_ADDR state.
 *
 * Revision 2.1  1995/06/07  21:20:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LEC_PRIVATE_H_
#define _LEC_PRIVATE_H_

#include <string.h>
#include <ciscolib.h>

/*
 * LEC external variables
 */
extern int              lec_count;
extern boolean		lec_inputQ_enabled;
extern boolean		lec_booted;
extern ulong		lec_transaction_id;
extern queuetype	lec_infoQ;
extern watched_queue	*lec_inputQ;
extern watched_queue	*lec_bridge_inputQ;
extern watched_queue	*lec_flush_inputQ;
extern watched_queue    *lec_ipc_inputQ;

/*
 * Debugging macros
 */
#define LEC_PACKET_DEBUG	if (lec_packet_debug) lec_dump_control
#define LEC_SIGNAL_DEBUG	if (lec_signal_debug) lec_dump_sig_msg
#define LEC_STATE_DEBUG		if (lec_state_debug) buginf
#define LEC_TOPO_DEBUG		if (lec_topo_debug) buginf

/*
 * Multicast MAC addresses
 */
#define ISMULTICAST(x) (*(x) & 0x01)

/*
 * LAN Emulation Client Constants (ref. atm94-0035R7)
 */

/* Control Time-out */
#define	C7_MINIMUM		(10*ONESEC)
#define C7_DEFAULT		(120*ONESEC)
#define C7_MAXIMUM		(300*ONESEC)

/* Maximum Unknown Frame Count */
#define C10_MINIMUM		0
#define C10_DEFAULT		1
#define C10_MAXIMUM		10

/* Maximum Unknown Frame Time */
#define C11_MINIMUM		(1*ONESEC)
#define C11_DEFAULT		(1*ONESEC)
#define C11_MAXIMUM		(60*ONESEC)

/* VCC Time-out Period */
#define C12_MINIMUM		(0*ONESEC)
#define C12_DEFAULT		(1200*ONESEC)
#define C12_MAXIMUM		(0*ONESEC)

/* Maximum Retry Count */
#define C13_MINIMUM		0	
#define C13_DEFAULT		1
#define C13_MAXIMUM		2

/* LECID */
#define C14_DEFAULT		0

/* Aging Time */
#define C17_MINIMUM		(10 * ONESEC) 
#define C17_DEFAULT		(300 * ONESEC)
#define C17_MAXIMUM		(300 * ONESEC)

/* Forward Delay Time */
#define C18_MINIMUM		(4 * ONESEC)
#define C18_DEFAULT		(15 * ONESEC)
#define C18_MAXIMUM		(30 * ONESEC)

/* LE-ARP Response Time */
#define C20_MINIMUM		(0 * ONESEC)
#define C20_DEFAULT		(4 * ONESEC)
#define C20_MAXIMUM		(0 * ONESEC)

/* Flush Time */
#define C21_MINIMUM		(1 * ONESEC)
#define C21_DEFAULT		(4 * ONESEC)
#define C21_MAXIMUM		(4 * ONESEC)

/* Path Switching Delay -- not implemented */
#define C22_MINIMUM             (1 * ONESEC)
#define C22_DEFAULT             (6 * ONESEC)
#define C22_MAXIMUM             (8 * ONESEC)

/* Default mcast ave and peak cell rates */
#define C25_DEFAULT		155
#define C26_DEFAULT		155

/* Connect Completion Time */
#define C28_MINIMUM		(1 * ONESEC)	
#define C28_DEFAULT		(4 * ONESEC)
#define C28_MAXIMUM		(60 * ONESEC)

/*
 * IDLE timeout.  This timer wakes the state machine out of the IDLE
 * state.
 */
#define LEC_IDLE_TIMEOUT	(2 * ONESEC)
#define LEC_IDLE_TIMEOUT_MAX	(120 * ONESEC)

#define LEC_JITTER_PCT		20	/* Jitter timers by 20 percent */

/* LEC MIB Owner String MAX length 
 * -- we may want to impose a smaller implementation restriction
 * -- in order to save space in NVRAM
 */
#define MAX_OWNER_STRING_LEN    128    /* from RFC 1271 */

#define MAX_IF_NAME_LEN         20
#define MAX_IF_DESCR_LEN        20

/*
 * LEC Timer Types
 */
typedef enum lec_timer_t_ {
    LEC_TIMER_TYPE_IDLE,
    LEC_TIMER_TYPE_CONFIG,
    LEC_TIMER_TYPE_JOIN,
    LEC_TIMER_TYPE_BUS_ARP,
    LEC_TIMER_TYPE_READY,
    LEC_TIMER_TYPE_ARP_AGE,
    LEC_TIMER_TYPE_ARP_CONTROL,
    LEC_TIMER_TYPE_REG_REQ, 
    LEC_TIMER_TYPE_FLUSH,
} lec_timer_t;

typedef enum lec_call_state_t_ {
    CALL_STATE_UP,
    CALL_STATE_DOWN,
    CALL_STATE_PROCEEDING,
    CALL_STATE_TERMINATING,
} lec_call_state_t;

typedef enum lec_ready_state_t_ {
    CALL_READY,
    CALL_NOT_READY,
} lec_ready_state_t;

typedef struct lec_vc_t_ {
    idbtype		*swidb;
    void		*call_id;
    const blli_type	*blli;
    vcd_t               vcd;
    boolean    		svc;
    lane_vc_type_t      lane_vc_type;
    lec_call_state_t	call_state;
    uint		flags;
    ushort		sdu_size;
    hwaddrtype          nsap;
} lec_vc_t;

/*
 * The LE-ARP struct contains the MAC<->VC translation
 * along with the state of the MAC.
 */
typedef enum lec_mac_state_t_ {
    LEC_MAC_DIRECT,
    LEC_MAC_RESOLVED,
    LEC_MAC_FLUSH,
    LEC_MAC_FLOOD,
    LEC_MAC_REVERIFYING,
} lec_mac_state_t;


typedef enum lec_arp_entry_type_t {
    LEC_LE_ARP_PERMANENT,
    LEC_LE_ARP_LOCAL,
    LEC_LE_ARP_REMOTE
} lec_arp_entry_type_t;

typedef struct lec_le_arp_t_ {
    struct lec_le_arp_t_	*next;
    struct lec_info_t_		*lec_info;
    uchar			mac[STATIONLEN_IEEE48];
    uchar			src_mac[STATIONLEN_IEEE48];
    hwaddrtype			nsap;
    lec_mac_state_t		state;
    void			*cam_entry;
    lec_arp_entry_type_t	entry_type;
    mgd_timer			arp_age_timer;
    mgd_timer			control_timer;
    mgd_timer			flush_timer;
    ushort			arp_retry_count;
    ushort			unknown_count;
    sys_timestamp		unknown_time;
    boolean                     remote_addr_flag_set;  /***/
} lec_le_arp_t;

/* 
 * enum used by LEC MIB to indicate LEC source of LECS address 
 */
typedef enum lec_config_source_t_ {
    LEC_CONFIG_TYPE_ILMI=1,
    LEC_CONFIG_TYPE_KNOWN_ADDR,
    LEC_CONFIG_TYPE_NO_LECS     /* also used for 'not-set-yet' */
} lec_config_source_t;

/*
 * The lec_nsap_info_t structs are hashed into a table which
 * translates NSAP->vcd.  These translations can either be picked up
 * from configuration, or via SVC setups.
 */
typedef struct lec_nsap_info_t_ {
    struct lec_nsap_info_t_	*next;
    lec_vc_t			vc;
    lec_ready_state_t		ready_state;
    ushort			ready_retry_count;
    mgd_timer			ready_timer;
    ushort			lecid;
} lec_nsap_info_t;


/*
 * lec_lecs_addr_t - Holds an LECS ATM address obtained via ILMI
 */
typedef struct lec_lecs_addr_t_ {
    struct lec_lecs_addr_t_	*next;
    hwaddrtype			 atm_addr;
} lec_lecs_addr_t;

/*
 * LEC SNMP info struct
 *
 * This hangs off of the lec_info struct and contains info used by
 * SNMP only. It is not malloced unless SNMP is enabled
 */
typedef struct lec_snmp_info_t_ {

    /* SNMP data:
     * the owner string needs to be saved in the config
     *  -- if it cannot, then set to 'monitor' when created from config 
     */
    char                        lec_owner[MAX_OWNER_STRING_LEN]; /***/    
    lec_config_source_t         lec_config_source;      /*** not done ***/
    boolean                     lec_config_mode_auto;   /*** r/o done ***/
    ushort                      last_fail_resp_code;    /* not working V */
    unsigned                    last_fail_state;        /* in err cond. */
    unsigned                    lec_snmp_index;   /* CLI done,
						   * needs redo for VTP ***/
    boolean                     ifmib_enabled;

    long                        if_index;
    short                       if_max_mtu;        
    uint                        if_phys_addr_len;
    char                        if_name[MAX_IF_NAME_LEN];
    char                        if_descr[MAX_IF_DESCR_LEN];

    /* The LEC MIB requires that counters be maintained for all
     * VCs, since the join phase completed. As SVCs are deleted,
     * their IF-MIB counters are added to the deleted-ifmib totals
     */
    ulong       deleted_in_octets;            /* bits 0..31 */
    ulong       deleted_in_octets_HC;         /* bits 32..63 */
    ulong       deleted_in_ucast_pkts;
    ulong       deleted_in_mcast_pkts;
    ulong       deleted_in_bcast_pkts;
    ulong       deleted_in_discards;
    ulong       deleted_in_errors;
    ulong       deleted_in_unknown_protos;
    ulong       deleted_out_octets;            /* bits 0..31 */
    ulong       deleted_out_octets_HC;         /* bits 32..63 */
    ulong       deleted_out_ucast_pkts;
    ulong       deleted_out_mcast_pkts;
    ulong       deleted_out_bcast_pkts;
    ulong       deleted_out_errors;
    ulong       deleted_out_discards;

    /* a cache of IF-MIB counters is kept to improve performance
     * and help insure the counter values are consistent.
     * The entire set of IF-MIB counters is collected once per
     * PDU, by caching the PDU serial number
     */
    long        cache_serial_num;
    ulong       in_octets;            /* bits 0..31 */
    ulong       in_octets_HC;         /* bits 32..63 */
    ulong       in_ucast_pkts;
    ulong       in_mcast_pkts;
    ulong       in_bcast_pkts;
    ulong       in_discards;
    ulong       in_errors;
    ulong       in_unknown_protos;
    ulong       out_octets;            /* bits 0..31 */
    ulong       out_octets_HC;         /* bits 32..63 */
    ulong       out_ucast_pkts;
    ulong       out_mcast_pkts;
    ulong       out_bcast_pkts;
    ulong       out_errors;
    ulong       out_discards;
} lec_snmp_info_t;

/*
 * max no of additional mac addresses that an LEC
 * can take on because of the HSRP stuff.
 */
#define LEC_HSRP_MACS	256

/*
 * reason to be filled in lec_info->last_fail_reason
 * to describe why an LEC fails to come up.
 */
typedef enum lec_fail_reason_t_ {
    LEC_FAIL_PREFIX_SET = 1,
    LEC_FAIL_CONFIG_CHANGE,
    LEC_FAIL_ACTIVATE,
    LEC_FAIL_DEACTIVATE,
    LEC_FAIL_ILMI_SET_RSP_NEG,
    LEC_FAIL_CONFIG_RSP_NEG,
    LEC_FAIL_JOIN_RSP_NEG,
    LEC_FAIL_REG_RSP_NEG,
    LEC_FAIL_TIMER_CONFIG,
    LEC_FAIL_TIMER_JOIN,
    LEC_FAIL_TIMER_BUS_ARP,
    LEC_FAIL_TIMER_REG_REQ,
    LEC_FAIL_BLLI_CFG,
    LEC_FAIL_BLLI_DIRECT,
    LEC_FAIL_BLLI_DISTRIBUTE,
    LEC_FAIL_BLLI_SEND,
    LEC_FAIL_BLLI_FORWARD,
    LEC_FAIL_ORIG_DISTRIBUTE,
    LEC_FAIL_ORIG_FORWARD,
    LEC_FAIL_FRAME_SIZE_MISMATCH,
    LEC_FAIL_REL_CONFIG,
    LEC_FAIL_REL_CTRL_DIR,
    LEC_FAIL_REL_CTRL_DISTR,
    LEC_FAIL_REL_MCAST_SEND,
    LEC_FAIL_REL_MCAST_FORWARD,
    LEC_FAIL_CTRL_VC_RELEASED,
    LEC_FAIL_SETUP_LECS_VC,
    LEC_FAIL_INIT,
    LEC_FAIL_NOTAPPLY,
} lec_fail_reason_t;

/*
 * LEC info struct
 *
 * This hangs off of the lane_info struct which ties all LANE
 * components to the swidb.
 */
struct lec_info_t_ {
    struct lec_info_t_		*next;
    struct lane_info_t_		*lane_info;
    idbtype			*swidb;
    hwidbtype			*srb_idb;
    hwaddrtype			nsap;
    MaskedAddress		masked_addr;

    /* interface derived mac address for client */
    uchar			normalMac[IEEEBYTES];
    /* the hsrp mac addresses of this client */
    uchar			lecHsrpMacs[LEC_HSRP_MACS];

    lec_id_t			lecid;
    unsigned			state;
    ulong			transaction_id;
    ulong			predicate_vector;
    ushort			control_retrys;

    /*
     * Synergy stuff.
     */
    ushort                      vlanid;
    queuetype                   bridge_lookup_holdQ;


    /*
     * SRB stuff for Token Ring LAN Emulation
     */

    boolean			lec_is_srb;
    boolean			registered_w_les;
    ushort 		        vr_rn;
    ushort 		        srb_bn;
    ushort 		        elan_rn;

    /*
     * These LANE parameters are configurable, but they are overridden
     * depending on the resulting JOIN to the ELAN.
     */
    uchar			actual_lan_type;
    uchar			actual_max_frame_size;
    uchar                       actual_name_size;
    uchar                       actual_elan_name[MAX_ELAN_NAME_LENGTH];

    /*
     * Dequeued messages, and in the case of signaling or the nsap
     * ready timer, parameter blocks passed into the lec state machine.
     */
    union {
	paktype			*control_pak;
	sig_api_msg		*sig_msg;
	lec_nsap_info_t		*nsap_entry;
	lec_le_arp_t		*le_arp_entry;
	ilmiClientReq		*notify;
    } data;

    /*
     * LECS address queue
     */
    lec_lecs_addr_t		*lecs_addrQ;

    /*
     * Control VCCs.  Gotta have'm.
     */
    lec_vc_t			configure;
    lec_vc_t			ctl_direct;
    lec_vc_t			ctl_distribute;
    lec_vc_t			mcast_send;
    lec_vc_t			mcast_forward;

    lec_le_arp_t		*le_arp_table[LANE_NETHASHLEN];
    lec_nsap_info_t		*nsap_table[LANE_NETHASHLEN];
    lec_nsap_info_t		*glare_list;

    boolean			C4_proxy;
    boolean			C19_topology_change;
    ushort			C10_max_unknown_count;
    ushort			C13_max_retry_count;
    ushort			C22_path_switching_delay;
    ushort			C23_local_seg; 
    ulong			C7_control_timeout;
    ulong			C11_max_unknown_time;
    ulong			C12_vcc_timeout;
    ulong			C17_aging_time;	
    ulong			C18_forward_delay_time;
    ulong			C20_arp_response_time;
    ulong			C21_flush_time;
    ulong			C25_mcast_ave_rate;
    ulong			C26_mcast_peak_rate;

    ulong			C28_connect_comp_time;

    /*
     * Time that the LEC is IDLE between attempts at retrying if admin
     * is configured up.  This is recalculated between trys as an
     * exponential backoff.
     */
    ulong			idle_time;
    ulong			config_idle_time;
    ulong			join_attempt_cnt;
    boolean			done_first_idle_timeout;

    /*
     * This timer is used for IDLE/CFG/JOIN/BUS_ARP times,
     * since only one is active at any one time.
     */
    mgd_timer			lec_timer;

    /*
     * The following are master timer blocks that point to multiple
     * leaf timers in the nsap_info cache entries, in the case of the ready
     * timer, and to the le_arp cache entries in the case of the arp timers.
     */
    mgd_timer			lec_ready_timers;
    mgd_timer			lec_arp_timers;

    /*
     * admin_status is the configured up/down LEC status.
     * oper_status is the dynamic up/down state of the LEC.
     */
    lane_status_t		admin_status;
    lane_status_t		oper_status;

    /*
     * LEC Interface Statistics
     *
     * These are defined in the LEC MIB ATMF 94-0737, and are all
     * counter32 type.
     */
    ulong			lecARPrequestsOut;
    ulong			lecARPrequestsIn;
    ulong			lecARPrepliesOut;
    ulong			lecARPrepliesIn;
    ulong                       lecControlFramesOut;
    ulong                       lecControlFramesIn;
    ulong                       lecSVCFailures;

    /* pointer to LEC MIB & IF MIB stuff */
    lec_snmp_info_t             *lec_snmp;

    /* timestamp when the LEC is set to ACTIVE state */
    sys_timestamp		time_when_active;

    /* keep the reason and state when the last failure occurs */
    lec_fail_reason_t		last_fail_reason;
};


/*
 * Inlines and macros
 */
#define FOR_ALL_LEC(lec_info) \
    for ((lec_info) = lec_infoQ.qhead; (lec_info); (lec_info) = (lec_info)->next)

/*
 * lec_stop_timer
 *
 * Stops the client-wide timer, which is used for timer associated
 * with communication with the LES, or controlling global events of
 * the client such as the IDLE timer.
 */
static inline 
void lec_stop_timer (lec_info_t *lec_info)
{
    mgd_timer_stop(&lec_info->lec_timer);
}

/*
 * lec_start_timer
 *
 * Sets both the new type and delay for the global LEC timer.
 */
static inline 
void lec_start_timer (lec_info_t *lec_info, 
		      ulong msecs, 
		      lec_timer_t lec_timer_type)
{
    mgd_timer_start_jittered(&lec_info->lec_timer, msecs, LEC_JITTER_PCT);
    mgd_timer_set_type(&lec_info->lec_timer, lec_timer_type);
}

/*
 * lec_start_arp_age_timer
 *
 * If the topology change flag is set, use the shorter forward delay
 * timer (15 sec), otherwise go with the default (5 minutes).  Only
 * for non-local le-arp entries, of course.
 */
static inline
void lec_start_arp_age_timer (lec_le_arp_t *le_arp_entry)
{
    if (le_arp_entry->entry_type != LEC_LE_ARP_PERMANENT) {
	if (le_arp_entry->lec_info->C19_topology_change &&
	    le_arp_entry->entry_type == LEC_LE_ARP_REMOTE) {
	    mgd_timer_start_jittered(&le_arp_entry->arp_age_timer,
				     le_arp_entry->lec_info->C18_forward_delay_time,
				     LEC_JITTER_PCT);
	} else {
	    mgd_timer_start_jittered(&le_arp_entry->arp_age_timer,
				     le_arp_entry->lec_info->C17_aging_time, 
				     LEC_JITTER_PCT);
	}
    }
}

static inline
void lec_stop_arp_age_timer (lec_le_arp_t *le_arp_entry)
{
    mgd_timer_stop(&le_arp_entry->arp_age_timer);
}

/*
 * lec_start_arp_control_timer
 *
 * Pops when the LE-ARP reply from the peer client gets lost.
 * Defaults to 2 minutes.
 */
static inline
void lec_start_arp_control_timer (lec_le_arp_t *le_arp_entry)
{
    if (le_arp_entry->entry_type != LEC_LE_ARP_PERMANENT) {
	mgd_timer_start(&le_arp_entry->control_timer,
			le_arp_entry->lec_info->C7_control_timeout);
    }
}

static inline
void lec_stop_arp_control_timer (lec_le_arp_t *le_arp_entry)
{
    mgd_timer_stop(&le_arp_entry->control_timer);
}
/*
 * lec_start_flush_timer
 *
 * Pops when the flush response doesn't come back within C21_flush_time
 * Defaults to 4 seconds
 */
static inline
void lec_start_flush_timer (lec_le_arp_t *le_arp_entry)
{

    if (le_arp_entry->entry_type != LEC_LE_ARP_PERMANENT) {
	mgd_timer_start(&le_arp_entry->flush_timer,
			le_arp_entry->lec_info->C22_path_switching_delay);
    }
}

static inline
void lec_stop_flush_timer (lec_le_arp_t *le_arp_entry)
{
    mgd_timer_stop(&le_arp_entry->flush_timer);
}

static inline
void lec_make_hwaddr (hwaddrtype *hwaddr,
		      uchar *address,
		      uchar type,
		      uchar length)
{
    memset(hwaddr, 0, sizeof(hwaddrtype));
    bcopy(address, hwaddr->addr, length);
    hwaddr->type = type;
    hwaddr->length = length;
}

static inline
boolean lec_dest_equal (lane_dest_t *dest1, lane_dest_t *dest2)
{

    if (dest1->tag != dest2->tag) {
	return(FALSE);
    }

    if (bcmp(dest1->addr.mac, dest2->addr.mac, STATIONLEN_IEEE48)) {
	return(FALSE);
    }

    return(TRUE);
}


/*
 * lec_info_from_csb
 *
 * Given a csb pointer, find the interface pointer, then the lane_info
 * pointer to the lec_info block, or NULL.
 */
static inline
lec_info_t *lec_info_from_csb (parseinfo *csb)
{
    idbtype	*swidb;
    lane_info_t	*lane_info;
    lec_info_t	*lec_info;

    swidb = csb->interface;
    if (!swidb) {
	return(NULL);
    }

    lane_info = swidb->lane_info;
    if (!lane_info) {
	return(NULL);
    }

    lec_info = lane_info->lec_info;
    if (!lec_info) {
	return(NULL);
    } else {
	return(lec_info);
    }
}


/*
 * lec_info_from_swidb
 *
 * Given a swidb pointer, find the lane_info pointer and return a
 * pointer to the lec_info block, or NULL.
 */
static inline
lec_info_t *lec_info_from_swidb (idbtype *swidb)
{
    lane_info_t *lane_info;
    lec_info_t *lec_info;

    lane_info = swidb->lane_info;
    if (!lane_info) {
	return(NULL);
    }

    lec_info = lane_info->lec_info;
    if (!lec_info) {
	return(NULL);
    } else {
	return(lec_info);
    }
}


/*
 * lec_info_from_vlanid
 *
 * Given a vlanid, find the lec_info pointer or NULL.
 */
static inline
lec_info_t *lec_info_from_vlanid (ushort vlanid)
{
    lec_info_t *lec_info;

    FOR_ALL_LEC(lec_info) {
	if (lec_info->vlanid == vlanid) {
	    return(lec_info);
	}
    }
    return(NULL);
}

/*
 * Bridging code needs toget the mcast_vcd so that a packet can be flooded
 * on a lane interface. Provide the lookup here. Caller has already checked, if
 * the lane client is enabled on the interface.
 */
static inline
int lane_get_mcast_vcd (idbtype *swidb)
{
    return (swidb->lane_info->lec_info->mcast_send.vcd);
}

/*
 * I have to do this here, unless you want me
 * to include if/network.h in a lot of other
 * files
 */
extern uchar standby_groups[];

/*
 * is this a valid lec mac address ?
 */
static inline boolean lec_mac_valid (lec_info_t *client, uchar *mac)
{
    /* check against the "original" mac address */
    if (LANE_MAC_SAME(client->normalMac, mac)) return TRUE;

    /* check against the hsrp mac addresses */
    if (client->lecHsrpMacs[mac[IEEEBYTES-1]]) {

	/*
	 * Compare the first 4 bytes,
	 * then compare the 5th byte
	 */
	return((GETLONG(standby_groups) == GETLONG(mac)) &&
	       (standby_groups[IEEEBYTES-2] == mac[IEEEBYTES-2]));
    }

    return FALSE;
}

/*
 * Extern function declarations.
 */
extern void		lec_add_lecs_addr(lec_info_t *lec_info, hwaddrtype *addr);
extern boolean          lane_configure_client(idbtype* swidb, 
					      boolean create_lec, 
					      int lane_type, 
					      uchar* elan_name, 
					      int name_size, 
					      ulong vlan_id, 
					      int vr_rn, int srb_bn);
extern void		lec_cfg_lec(parseinfo *csb);
extern void		lec_cfg_lec_nsap(parseinfo *csb);
extern void		lec_cfg_le_arp(parseinfo *csb);
extern void		lec_clear_vc(lec_info_t *lec_info, lec_vc_t *vc);
extern void		lec_create_glare_entry(lec_info_t *lec_info,
					       lec_nsap_info_t *glare_vc);
extern lec_le_arp_t	*lec_create_le_arp_entry(lec_info_t *lec_info,
						 uchar *dest_mac,
						 uchar *src_mac,
						 lec_mac_state_t state,
						 lec_arp_entry_type_t entry_type);
extern lec_nsap_info_t	*lec_create_nsap_entry(lec_info_t *lec_info, hwaddrtype *nsap);
extern void		lec_delete_glare_entry(lec_info_t *lec_info,
					       lec_nsap_info_t *glare_entry);
extern void		lec_delete_nsap_entry(lec_info_t *lec_info,
					      lec_nsap_info_t *nsap_entry);
extern boolean		lec_delete_le_arp_entry(lec_info_t *lec_info, 
						uchar *mac);
extern void		lec_delete_le_arp_nsap(lec_info_t *lec_info, 
					       lec_nsap_info_t *nsap);
extern void 		lec_stop_arp_timers(lec_le_arp_t *le_arp);
extern void		lec_free_lecs_addrQ(lec_info_t *lec_info);
extern void		lec_flush_le_arp_table(lec_info_t *lec_info);
extern void		lec_gen_le_arp(parseinfo *csb);
extern void		lec_gen_lec(parseinfo *csb);
extern void		lec_gen_lec_nsap(parseinfo *csb);
extern void		lec_init(void);
extern lec_info_t	*lec_init_idb(idbtype *swidb);
extern void		lec_init_info_counters(lec_info_t *lec_info);
extern void		lec_input(void);
extern void 		lec_process_flushQ(void);
extern void		lec_load_config(lec_info_t *lec_info);
extern lec_le_arp_t	*lec_lookup_le_arp(lec_info_t *lec_info, uchar *mac);
extern boolean          lec_find_arp_ptr(lec_info_t *lec_info, 
					  lec_le_arp_t *le_arp);
extern lec_nsap_info_t	*lec_lookup_callid(lec_info_t *lec_info, 
					   void *call_id);
extern lec_nsap_info_t	*lec_lookup_glare(lec_info_t *lec_info, void *call_id);
extern lec_nsap_info_t	*lec_lookup_nsap(lec_info_t *lec_info, 
					 hwaddrtype *nsap);
extern lec_nsap_info_t	*lec_lookup_vcd(lec_info_t *lec_info, vcd_t vcd);
extern void		lec_lookup_connection(lec_le_arp_t *le_arp);
extern boolean		lec_start_flush_requests(lec_info_t *lec_info,
						 lec_nsap_info_t *nsap_info);
extern void		lec_make_dest(lec_info_t *lec_info, 
				      uchar const *mac_addr, 
                                      lane_dest_t *lane_dest, ushort tag_type);
extern boolean		lec_muex_config(idbtype *swidb);
extern void		lec_release_all_data(lec_info_t *lec_info);
extern void		lec_release_all_vccs(lec_info_t *lec_info);
extern int		lec_send_ilmi_reg_addr(lec_info_t *lec_info);
extern int		lec_send_ilmi_unreg_addr(lec_info_t *lec_info);
extern void		lec_sig_callback(void *msg);
extern void		lec_start_process(void);
extern void		lec_swif_erase(lane_info_t *lane_info);

extern void 		lec_flush_enqueue(lec_info_t *lec_info,
					  lec_nsap_info_t *lec_nsap_info);
extern void		lec_process_flushQ(void);

extern void		lec_line_statechange(idbtype *swidb, ulong new_state);
extern void		lec_cfg_idle_time(parseinfo *csb);
extern void		lec_gen_idle_time(parseinfo *csb);
extern boolean 		lec_create_client(idbtype *swidb, 
					  lane_caller_t caller_is_cli,
					  int lane_type, uchar *elan_name, 
					  int vr_rn, int srb_bn, 
					  int vlan_id);
extern boolean 		lec_delete_client(idbtype *swidb, 
					  lane_caller_t caller_is_cli);

extern char * lec_pr_fail_reason(lec_fail_reason_t reason);

lec_fail_reason_t lec_get_ctrl_vc_fail(lane_vc_type_t lane_vc_type);

#endif _LEC_PRIVATE_H_
