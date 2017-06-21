/* $Id: dlsw_peer_externs.h,v 3.6.6.8 1996/09/09 17:40:05 fbordona Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_externs.h,v $
 *------------------------------------------------------------------
 * DLSw Peer Module External Header File
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains external declarations for the DLSw peer module. The
 * DLSw sub-system only needs to include this file to use the DLSw
 * peer modules service routines. This header file includes 
 * typedefs, constants, data structures and function prototypes.
 *------------------------------------------------------------------
 * $Log: dlsw_peer_externs.h,v $
 * Revision 3.6.6.8  1996/09/09  17:40:05  fbordona
 * CSCdi66251:  Filtering at DLSw layer will not work on border router
 * Branch: California_branch
 *
 * Revision 3.6.6.7  1996/09/03  22:25:07  kpatel
 * CSCdi67883:  DLSw lite requires flow control to throttle back end
 *              station traffic
 * Branch: California_branch
 *
 * Revision 3.6.6.6  1996/08/21  22:19:34  akhanna
 * CSCdi48010:  dlsw flow control allow max/min flow control window sizes
 * Branch: California_branch
 *
 * Revision 3.6.6.5  1996/08/09  23:43:34  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.6.6.4  1996/07/23  06:29:51  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/06/18  00:02:06  kpatel
 * CSCdi60179:  dlsw icannotreach lsap is broken
 * Branch: California_branch
 *
 * Revision 3.6.6.2  1996/06/11  15:01:22  ravip
 * CSCdi58441:  xid3 flooding in csna/dlsw environment cause access
 *              router to reboot
 *                  - Carry xid command response bit in the message
 *                  - Use Capex to exchange information about this
 *                    support
 * Branch: California_branch
 *
 * Revision 3.6.6.1  1996/05/17  10:46:21  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.32.5  1996/05/14  03:20:42  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.32.4  1996/04/25  19:37:51  ravip
 * CSCdi54131:  DLSw MIB ciscoDlswTConnOper Table returns 0 for stats
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.32.3  1996/04/25  16:20:15  ravip
 * CSCdi45773:  dlsw icanreach mac-exclusive command not working
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.32.2  1996/04/20  02:08:25  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.32.1  1996/03/17  17:38:18  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6  1996/03/05  18:49:35  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.5  1995/12/14  15:28:31  ravip
 * CSCdi41809:  DLSW MIB ciruit counters and OIDs always set to 0
 *
 * Revision 3.4  1995/12/01  23:40:09  ravip
 * CSCdi41808:  Dlsw Mib does not respond to get-next correctly
 *
 * Revision 3.3  1995/11/30  21:22:58  fbordona
 * CSCdi44759:  DLSw peer-on-demand peers prematurely disconnect.
 *              Fix peer-on-demand crash introduced in 11.1
 *
 * Revision 3.2  1995/11/17  09:03:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:55:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/21  02:05:40  kpatel
 * CSCdi42555:  DLSw+ LLC2 peer connection failure/shutdown does
 *              not clean up cep
 *
 * Revision 2.4  1995/10/20  14:10:41  ravip
 * CSCdi40722: Exclusive reachability option does not filter frames
 *             correctly. Fixed problems with deleting reachability cache.
 *
 * Revision 2.3  1995/06/28  18:51:46  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.2  1995/06/28  09:23:40  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:27:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _DLSW_PEER_EXTERNS_H_
#define _DLSW_PEER_EXTERNS_H_

#include "../tcp/tcp.h"
#include "../ibm/netbios.h"


/*
 * External Global Data
 */
extern boolean dlsw_force_ssp; /* if TRUE, use ssp between ciscos */
extern long    conn_active_connection;
extern long    conn_close_idle;
extern long    conn_close_busy;
extern sys_timestamp dlsw_start_time;


/*
 * Bit Vector Macros
 */
#define CHECK_ON(x,y)      ((x) & (y)) /* returns non-zero if bit(s) set */
#define TURN_ON(x,y)     x=((x) | (y)) /* sets bit(s) in facility x */
#define TURN_OFF(x,y) x=((x) & (~(y))) /* ressets bit(s) in facility x */

/*
 * peer_get_vendor_type() return codes
 */
#define PEER_UNKNOWN             0
#define PEER_CISCO               1
#define PEER_NOT_CISCO           2

/*
 * Miscelaneous Constants
 */
#define DLSW_UNKNOWN_PGROUP             0
#define DLSW_MIN_PGROUP                 1
#define DLSW_MAX_PGROUP               255
#define DLSW_DEF_KEEP_INT             (30 * ONESEC)
#define DLSW_KEEP_CNT_THOLD             3
#define MAX_PORTS_IN_LIST              10
#define MAX_RINGS_IN_LIST              10
#define MAX_BGROUPS_IN_LIST            10
#define MAX_SAPS_IN_LIST               10
#define DLSW_DEF_CONNECT_INT          (15 * ONESEC)
#define DLSW_POD_DEF_DISC_DELAY        10   /* def - pods inact disc 10 min */
#define DLSW_POD_MAX_DISC_DELAY        1440 /* number of minutes in 1 day */
#define DLSW_POD_CONN_WAIT             (1 * ONEMIN)
#define DLSW_CONN_WINDOW               (6 * ONESEC)
#define DLSW_MAX_TCP_QLEN             200
#define DLSW_TCP_HI_WATER_PERCENT      50
#define DLSW_TCP_LO_WATER_PERCENT      25

/*
 * Generic congestion levels for all peer types - TCP, LLC2
 */
/* outputq congestion levels */
#define DLSW_PEER_CONG_INCREASE     	0x01 
#define DLSW_PEER_CONG_LOW     		0x02
#define DLSW_PEER_CONG_MEDIUM  		0x04
#define DLSW_PEER_CONG_HIGH    		0x08

/*
 * TCP Connection Modes
 */
#define NORMAL_MODE                1
#define PASSIVE_MODE               2

/*
 * Peer Cost
 */
#define DLSW_UNKNOWN_PCOST         0
#define DLSW_MIN_PCOST             1
#define DLSW_NORM_PCOST            3
#define DLSW_MAX_PCOST             5

/*
 * Initial and Maximum pacing window sizes
 * (flow control)
 */
#define DLSW_FC_INIT_WINDOW  20
#define DLSW_FC_MAX_WINDOW   50

/*
 * peer_to_core() message types
 * Used for receiving frames from remote peers and
 * used to notify core of peer events..
 */
#define PEER_RECV_PAK              0x00
#define PEER_KILL_HANDLE           0x01

/*
 * core_to_peer() message types
 */
#define PEER_PUT                   0x10
#define PEER_PUT_BCAST             0x11
#define PEER_PUT_GROUP             0x12
#define PEER_PUT_POD_RSP           0x13

/*
 * Port List Entry.
 */
struct _dlsw_ring_t;
typedef struct _port_list_entry {
    struct _port_list_entry *next;
    uchar list_num;
    uchar portlist_len;
    idbtype *sw_idb[MAX_PORTS_IN_LIST];
    uchar ringlist_len;
    ushort ring_no[MAX_RINGS_IN_LIST];
    uchar bgrouplist_len;
    ushort bgroup_no[MAX_BGROUPS_IN_LIST];
} portlistentry;

/* right now use the same structure to store ring lists */
typedef struct _port_list_entry ringlistentry;

/* local icanreach mac and name list structures */
typedef struct _macentry_t {
    struct _macentry_t *next;
    uchar macaddr[IEEEBYTES];
    uchar macmask[IEEEBYTES];
} macentry_t;

typedef struct _nbentry_t {
    struct _nbentry_t *next;
    char nbname[NETBIOS_MAX_NAME_LEN + 1];
    char nbmask[NETBIOS_MAX_NAME_LEN]; /* don't care fields are zero */
    boolean wildterm;
    uchar len;
} nbentry_t;


/*
 ***********************************************************************
 * Core to Peer Interface Definition
 ***********************************************************************
 */

/*
 * Core to Peer return codes
 *
 * Note: on PEER_PARAMETER_ERROR, the peer module does NOT consumes 
 *       the caller's packet.
 */
typedef enum _core2peer_ret_t {
    PEER_SUCCESS,
    PEER_FILTERED,
    PEER_UNREACHABLE,
    PEER_INVALID_HANDLE,
    PEER_SUCCESS_AND_BUSY,
    PEER_BUSY,
    PEER_NO_CONNECTIONS,
    PEER_PARAMETER_ERROR
} core2peer_ret_t;

/*
 * Peer capabilities filter return codes
 */
#define PEER_PASS                0
#define PEER_SAP_FAIL            1
#define PEER_EXCL_FAIL           2
#define PEER_MAC_FAIL            3
#define PEER_NBNAME_FAIL         4

/*
 * peer setup pod peer return codes
 */
#define PEER_INVALID_PEER        -1
#define PEER_WILL_START           0
#define PEER_STARTED              1


/*
 * Miscellaneous Constants
 */
#define DLSW_MAX_PEER_ID    64 /* used in peer_id_to_str() */

/*
 * peerhandle opaque typedef
 */

typedef ulong peerhandle_t;

/*
 * Peer Put flags
 *
 * PEER_DLX_FRAME  (bit 0): set if DLX frame (if not set, indicates SSP type)
 * PEER_INFO_FRAME (bit 1): set if DLX_INFO_FRAME and SSP_OP_infoframe
 * PEER_SNA_BCAST  (bit 2): set if first CANUREACH_ex for circuit
 * PEER_NETBIOS_UI (bit 3): set if NetBios UI frames
 * PEER_CKT_START  (bit 4): set if CANUREACH_cs frame
 *
 * PEER_CKT_START needed where end station start up sequence is as follows.
 * Cisco APPN router does this.
 *   TEST(P) DSAP 0   --->
 *   TEST(F)         <----
 *   XID(P)  DSAP E4  ---->
 */
#define PEER_SSP_TYPE         0x00 
#define PEER_DLX_TYPE         0x01 /* if set, only send to cisco peers */
#define PEER_INFO_FRAME       0x02 /* if set, prioritize frame */
#define PEER_SNA_BCAST        0x04 /* if set, filter check frame */
#define PEER_NETBIOS_UI       0x08 /* if set, filter check frame */
#define PEER_CKT_START        0x10 /* if set, filter check frame */

/*
 * Peer Manager ONLY constants
 */
#define FILTER_ME         (PEER_SNA_BCAST | PEER_NETBIOS_UI | PEER_CKT_START)
#define PRIORITIZE_ME     (PEER_DLX_TYPE |  PEER_INFO_FRAME)

/*
 * Peer packet categories for prioritization.
 */
typedef enum _peer_pak_cat {
    PEER_BCAST,                /* frames without a peer handle */
    PEER_DIR_CIR_SETUP,        /* circuit setup frames with a peer handle */
    PEER_USER_INFO,            /* i-frame */
    PEER_CIR_ADMIN,            /* post circuit setup fsm admin frames */
    PEER_REACH_ADMIN,          /* periodic reachability - directed verify */
    PEER_CONN_ADMIN,           /* peer keepalives */
    PEER_CONN_SETUP            /* peer connection est - ca exchg */
} peer_pak_cat;

typedef struct _pod_rsp_info_t {
    ipaddrtype dest_ipaddr;
    ushort dest_group;
} pod_rsp_info_t;

/*
 * Peer Put Message notes:
 *
 * 1. peer_put_t requires pak->datagramstart points to the DLX/SSP header.
 * 2. peer_put_t requires that pak->info_start points to the netbios header
 *    if the PEER_NETBIOS_UI flag is set.
 * 3. peer_put_t requires that pak->info_start points to the SNA header
 *    if the PEER_INFO_FRAME flag is set. 
 */
typedef struct _peer_put_t {
    uchar type;
    union {
        peerhandle_t peer_handle; 
	ushort peer_group;           /* valid for PEER_PUT_GROUP */ 
        pod_rsp_info_t pod_info;     /* valid for PEER_PUT_POD_RSP */
    } dest_u;
    paktype *pak;                    /* datagramstart -> ssp/dlx header */
    struct _dlsw_ring_t *lan_port;   /* lan input port, NULL=don't filter */
    peer_pak_cat pak_cat;
    uchar flags;
    uchar *dmac;
    uchar *smac;
    uchar dsap;
    uchar ssap;
} peer_put_t;

#define put_handle  p_put.dest_u.peer_handle
#define put_group   p_put.dest_u.peer_group
#define pod_dipaddr p_put.dest_u.pod_info.dest_ipaddr
#define pod_dgroup  p_put.dest_u.pod_info.dest_group

typedef struct _pod_conn_t {
    uchar type;
    ipaddrtype ipaddr;
} pod_conn_t;


/*
 ***********************************************************************
 * Peer to Core Interface Definition
 ***********************************************************************
 */

/*
 * peer_to_core() message types
 * Used for receiving frames from remote peers and
 * used to notify core of peer events..
 */
#define PEER_RECV_PAK              0x00
#define PEER_KILL_HANDLE           0x01
#define PEER_FLOW_CTRL_INDICATION  0x02
#define PEER_DEL_CACHE             0x03

/*
 * Peer to Core return codes
 */
typedef enum _peer2core_ret_t {
    CORE_SUCCESS,
    CORE_PARAMETER_ERROR
} peer2core_ret_t;

typedef struct _peer_recv_t {
    uchar type;
    uchar header_type;        /* PEER_DLX_TYPE or PEER_SSP_TYPE */
    peerhandle_t peer_handle; /* sending peer */
    paktype *pak;             /* datagram_start points to ssp/dlx hdr */
} peer_recv_t;

#define sender_handle sender_u.peer_handle
#define sender_ipaddr sender_u.ipaddr

typedef struct _peer_kill_handle_t {
    uchar type;
    peerhandle_t peer_handle;
} peer_kill_handle_t;

typedef struct _peer_del_cache_t {
     uchar type;
     peerhandle_t peer_handle;
} peer_del_cache_t;

typedef struct _peer_new_cap_t {
    uchar type;
    peerhandle_t peer_handle;
} peer_new_cap_t;

typedef struct _peer_flow_ind_t {
    uchar type;
    peerhandle_t peer_handle;
    ulong cong_level;         /* tcp congestion level - see tcpdriver.h */
} peer_flow_ind_t;

/*
 ***********************************************************************
 * General Peer/Core Interface Message Definition
 ***********************************************************************
 */
 
/*
 * Union for peer to core communications
 * Used as parameter in core_to_peer() and peer_to_core().
 */

typedef union _peer_msg_u {
    uchar type;
    peer_put_t p_put; 
    pod_conn_t p_pod_conn;
    peer_recv_t p_recv; 
    peer_kill_handle_t p_kill_handle; 
    peer_new_cap_t p_new_cap; 
    peer_del_cache_t p_del_cache;
    peer_flow_ind_t p_flow_ind; 
} peer_msg_u;

/*
 ***********************************************************************
 * Peer/CSM Filter Interface Definition
 ***********************************************************************
 */

/*
 * peer_to_csm_filter() message types
 */
#define PEER_CSM_ACCEPT_CHECK     0x20

typedef struct _csm_accept_t {
    uchar type;
    void  *csm_header_start;
} csm_accept_t;

/*
 * Union for peer to core communications
 * Used as parameter in core_to_peer() and peer_to_core().
 */
typedef union _peer_filter_msg_u {
    uchar type;
    csm_accept_t csm_acpt; /* returns TRUE if CSM wants this frame */
} peer_filter_msg_u;


/*
 ***********************************************************************
 * Peer External Function Prototypes 
 ***********************************************************************
 */

/*
 * Parser Function Prototypes
 */
extern void dlsw_parser_init (subsystype *);
extern void dlsw_peer_init (void);
extern void dlsw_peer_background (void);

/*
 * Configuration Command Function Prototypes
 */
extern void dlsw_start(void);
extern void dlsw_disable_command (parseinfo *);
extern void dlsw_nornr_command (parseinfo *);
extern void dlsw_peer_command (parseinfo *);
extern void peer_debug_cmd (boolean, peerhandle_t, parseinfo *, boolean);
extern void show_remote_stats (peerhandle_t);
extern void show_remote_capabilities (peerhandle_t);
extern void print_capabilities (peerhandle_t);
extern void dlsw_show_fast_cache (void);

/*
 * Peer Data Base External Function Prototypes
 */
extern boolean dlsw_sna_spoof_and_seg (void);
extern char *peer_handle_to_int (peerhandle_t);
extern uchar peer_handle_to_xport (peerhandle_t);
extern ipaddrtype peer_handle_to_ip (peerhandle_t);
extern char *peer_handle_to_str (peerhandle_t, char *);
extern ipaddrtype peer_get_my_ipaddr (void);
extern macentry_t *get_local_mac_reach_qhead(void);
extern nbentry_t *get_local_name_reach_qhead(void);
extern macentry_t *peer_get_mac_reach_qhead(peerhandle_t);
extern nbentry_t *peer_get_name_reach_qhead(peerhandle_t);
extern boolean peer_ciscos (void);
extern boolean peer_non_ciscos (void);
extern uchar peer_get_box_largest_frame (void);
extern uchar peer_get_largest_frame (peerhandle_t);
extern ulong peer_get_init_pac_window (peerhandle_t);
extern boolean supported_sap_check (peerhandle_t, uchar, uchar);
extern peerhandle_t peer_ip_to_handle (ipaddrtype);
extern peerhandle_t peer_hwidb_to_handle (hwidbtype *);
extern peerhandle_t peer_swidb_dlci_to_handle(idbtype *, ushort);
extern boolean dlsw_peer_active (ulong sport, ulong dport, ipaddrtype fhost);
extern uchar peer_get_vendor_type (peerhandle_t);
extern boolean peer_pass_port_filter (peerhandle_t, struct _dlsw_ring_t *);
extern uchar peer_get_cost (peerhandle_t);
extern boolean is_fast_peer (peerhandle_t);
extern boolean peer_get_xid_op_support (peerhandle_t peer_handle);
boolean dlsw_is_peer_being_removed (peerhandle_t);
boolean pod_pass_mac_acl (peerhandle_t, char *);
boolean pod_pass_nb_acl (peerhandle_t, netbios_header *);


/*
 * Peer Transport Manager External Function Prototypes
 */
extern peer2core_ret_t peer_to_core (peer_msg_u *peer_msg);
extern core2peer_ret_t core_to_peer (peer_msg_u *peer_msg);
extern void peer_add_circuit (peerhandle_t);
extern void peer_delete_circuit (peerhandle_t);
extern void peer_input (peerentry *, paktype *, boolean);
extern int peer_new_ckt (peerhandle_t);

/*
 * Peer/CSM Filter Function Prototypes
 */
extern boolean peer_to_csm_filter (peer_filter_msg_u *filter_msg);

/*
 * Exclusivity filter function prototypes wrt peers/local
 */
extern dlsw_is_local_mac_excl (void);
extern dlsw_is_local_nb_excl (void);
extern dlsw_is_peer_mac_excl (peerhandle_t);
extern dlsw_is_peer_nb_excl (peerhandle_t);

/*
 * MIB function protypes
 */
extern void peer_update_out_stat (peerentry *peer, paktype *pak);
extern void peer_update_in_stat (peerentry *peer, paktype *pak);

/*
 * Debug output functions
 */
extern char *csm_stat (int);
extern char *csm_error (int);
extern char *csm_nbcom (int);
extern char *csm_prim (int);

/*
 * Added for Frame-Relay Support.
 */
#define DLSW_FR_SAP               0x04
#define FR_REEXPLORE              5
#define FR_DISCONN  1
#define FR_OPEN     2
#define FR_CONN     3

#endif

