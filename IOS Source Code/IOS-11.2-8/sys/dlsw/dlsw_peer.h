/* $Id: dlsw_peer.h,v 3.18.4.8 1996/09/09 19:08:31 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer.h,v $
 *------------------------------------------------------------------
 * DLSw Peer Module Internal Header File
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains declarations for the DLSw peer module. This includes 
 * typedefs, constants, data structures and function prototypes.
 *------------------------------------------------------------------
 * $Log: dlsw_peer.h,v $
 * Revision 3.18.4.8  1996/09/09  19:08:31  akhanna
 * CSCdi65724:  Cannot stop keepalives on promiscous peer
 * Branch: California_branch
 *
 * Revision 3.18.4.7  1996/08/28  12:40:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.18.4.6  1996/08/21  22:19:01  akhanna
 * CSCdi48010:  dlsw flow control allow max/min flow control window sizes
 * Branch: California_branch
 *
 * Revision 3.18.4.5  1996/07/31  18:19:48  akhanna
 * CSCdi61887:  dlsw icanreach saps
 * Branch: California_branch
 *
 * Revision 3.18.4.4  1996/07/09  18:08:23  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.18.4.3  1996/06/27  15:13:32  fbordona
 * CSCdi61278:  DLSw: System restarted by bus error at PC 0xD0D0D0D
 *              action_b(). Do not free peers if blocked on TCP open.
 *              Connect timer not set properly for priority peers.
 * Branch: California_branch
 *
 * Revision 3.18.4.2  1996/06/11  15:01:14  ravip
 * CSCdi58441:  xid3 flooding in csna/dlsw environment cause access
 *              router to reboot
 *                  - Carry xid command response bit in the message
 *                  - Use Capex to exchange information about this
 *                    support
 * Branch: California_branch
 *
 * Revision 3.18.4.1  1996/05/17  10:45:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.15.2.7  1996/05/14  03:20:22  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.6  1996/04/25  19:37:38  ravip
 * CSCdi54131:  DLSw MIB ciscoDlswTConnOper Table returns 0 for stats
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.5  1996/04/24  18:35:15  fbordona
 * CSCdi55588:  DLSw: Use managed timers for connecting dynamic peers
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.4  1996/04/23  20:13:49  fbordona
 * CSCdi55437:  DLSw peer may take several minutes to connect after reload
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.3  1996/04/20  02:08:07  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.2  1996/03/29  02:13:38  fbordona
 * CSCdi52957:  DLSW: Can not use lsap-output-list greater than 255
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.1  1996/03/17  17:38:05  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.18  1996/03/08  22:31:36  mzallocc
 * CSCdi49393:  Router crash on get for ciscoDlswTConn*
 *
 * Revision 3.17  1996/03/05  18:49:23  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.16  1996/02/27  21:36:19  fbordona
 * CSCdi49949:  DLSw peer-on-demand peers ignore tcp-queue-max.
 *              Improve "show dlsw peers" output.
 *
 * Revision 3.15  1996/02/26  20:56:22  ioakley
 * CSCdi49687: Correct problem with tcp-queue-max parameter for
 *             the global "dlsw remote-peer" config command.
 *
 * Revision 3.14  1996/02/23  17:32:39  hampton
 * Clean up platform specific DLSW defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.13  1996/02/20  22:08:09  fbordona
 * CSCdi47801:  SEgV exception, PC0x60637DFC tcpdriver_fhost.
 *              Use the tcpencaps handle now passed in the tcp
 *              closef vector (CSCdi48380) to determine if the
 *              DLSw read or write pipe has been closed.
 *
 * Revision 3.12  1996/02/08  20:10:27  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.11  1996/02/07  19:09:18  fbordona
 * CSCdi46958:  Router crashes when DLSw peer is reloaded.
 *
 * Revision 3.10  1996/01/04  21:39:33  fbordona
 * CSCdi46273:  DLSw peers cycle CONNECT to DISC after shut no shut on
 *              serial.
 *
 * Revision 3.9  1995/12/21  00:36:07  kpatel
 * CSCdi45362:  DLSw: Netbios call doesnt go thru for DLSw over
 *              FrameRelay
 *
 * Revision 3.8  1995/12/14  15:27:18  ravip
 * CSCdi41809:  DLSW MIB ciruit counters and OIDs always set to 0
 *
 * Revision 3.7  1995/12/01  23:39:25  ravip
 * CSCdi41808:  Dlsw Mib does not respond to get-next correctly
 *
 * Revision 3.6  1995/11/30  21:22:47  fbordona
 * CSCdi44759:  DLSw peer-on-demand peers prematurely disconnect.
 *              Fix peer-on-demand crash introduced in 11.1
 *
 * Revision 3.5  1995/11/29  22:06:24  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.4  1995/11/29  19:45:20  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.3  1995/11/20  23:25:45  fbordona
 * CSCdi42215:  dlsw backup peers broken
 *
 * Revision 3.2  1995/11/17  09:03:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:55:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/20  14:09:47  ravip
 * CSCdi40722: Exclusive reachability option does not filter frames
 *             correctly. Fixed problems with deleting reachability cache.
 *
 * Revision 2.6  1995/10/09  20:18:39  rnaderi
 * CSCdi41641:  Add sub-interface option to the parser for DLSw+/FR
 *
 * Revision 2.5  1995/08/10  18:22:35  fbordona
 * CSCdi38575:  DLSw promiscuous peer doesnt learn remote config
 *
 * Revision 2.4  1995/08/07  05:31:25  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.3  1995/07/26  19:12:13  hampton
 * Convert DLSW to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37708]
 *
 * Revision 2.2  1995/06/28 18:50:22  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.1  1995/06/07  20:27:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _DLSW_PEER_H_
#define _DLSW_PEER_H_

#include "logger.h"			/* Required for next include */
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "../ibm/netbios.h"

/*
 ********************************************************************
 * Constants
 ********************************************************************
 */

/*
 * Peer Debug Macro
 */
#define PDEBUG      if (peer->debug) buginf
#define PDEBUG_FSEQ if (peer->debug_fst_seq) buginf
#define PDEBUG_FPAK if (peer->debug_fast_pak) buginf
#define PDEBUG_FERR if (peer->debug_fast_err||peer->debug_fast_pak) buginf

/*
 * DLSw Standard Port Definitions
 */
#define DLSW_RD_PORT            2065
#define DLSW_WR_PORT            2067

/*
 * TCP Connection Constants.
 */
#define TCP_DEFAULT_RETRY_TIME   (10 * ONESEC)
#define TCP_PASSIVE_OPEN_WINDOW  (3 * ONESEC)
#define TCP_RETRY_THOLD_1          4    /* 4 retries then int is 30 secs */
#define TCP_RETRY_THOLD_2          8    /* 8 retries then int is 90 secs */
#define TCP_RETRY_THOLD_3         12    /* 12 retries then int is 15 mins */
#define DLSW_PRIORITY_COUNT        3

/*
 * Fast Switching Constants
 */
#define DLSW_FCT_HASH_SIZE       64     /* must be power of 2 */
#define DLSW_FCT_HASH_MASK       (DLSW_FCT_HASH_SIZE - 1)
#define DLSW_FCT_CLEAN_INT       ONEMIN
#define DLSW_FCT_LIFE            (5 * ONEMIN)

#define DLSW_FST_SEQNUM_MAX   32767
#define DLSW_FST_HALF_SEQ_RANGE   (DLSW_FST_SEQNUM_MAX / 2)
#define DLSW_FST_ORDER_WIN        (DLSW_FST_SEQNUM_MAX / 2)
#define DLSW_FST_OUT_OF_WIN_CNT 100

/*
 * Peer Transport Connection States
 */
typedef enum _peerstatetype {
    PEER_DISC,                /* read and write pipe closed */ 
    PEER_WAIT_READ,           /* wait for tcp read pipe to open */
    PEER_WAIT_CAP,            /* waiting for a cap exchg response */
    PEER_CONN,                /* both pipes opened */ 
    PEER_XPORT_BUSY           /* WAN transport is congested */ 
} peerstatetype;


/*
 * Peer Transport Events
 */
typedef enum _peereventtype {
    OPEN_CONNECTION,    
    CLOSE_CONNECTION,
    READ_PIPE_OPENED,
    READ_OPEN_TIMEOUT,
    CAP_RCVD,
    CAP_RSP_RCVD,
    CAP_TIMEOUT,
    CLOSE_READ_PIPE,
    CLOSE_WRITE_PIPE,
    BUSY_XPORT,
    UNBUSY_XPORT
} peereventtype;

typedef enum rd_state_t {
    RD_INIT,
    RD_HEADER, 
    RD_DATA 
} rd_state_t;

typedef enum _border_peer_ret_t {
    BEST_BP,
    BACKUP_BP,
    NOT_BP
} border_peer_ret_t;

typedef enum _peer_t {
    CONF_PEER,         /* configured by user */
    PROM_PEER,         /* promiscuous peer - partner knows about us */
    DEMAND_PEER,       /* learned of eachother thru border peers */
    DYNAMIC_PEER,       /* configured by user for on demand activation */
    PEER_MAX_TYPE
} peer_t;

typedef enum _peer_status_t {
    PEER_DEAD = 0xABCDEF00,
    PEER_NOT_READY,
    PEER_READY,
} peer_status_t;

/*
 * Misc. constants
 */
#define SAP_CV_BYTES             16
#define MIN_SAP_IN_BYTE        0x80
#define MAX_SAP_IN_BYTE        0x01
#define ODD_SAP_INDICATOR      0x01
#define FST_RSRV_PEERS            2
#define PEER_AUTOMORE_LINES      10
#define PEER_CONN_TIMER           1
#define PEER_KEEP_TIMER           2
#define PEER_MAX_PAK          18000

/*
 ********************************************************************
 * Structures
 ********************************************************************
 */

/*
 * DLSw Peer Capabilities
 * 
 * This information is learned via a Capabilities Exchange.
 * It is maintained seperate from configured information, which
 * resides in a peerentry.
 */
typedef struct _peercap_t {
    boolean cap_sent;
    boolean cap_known;
    boolean cap_ack;

    /* DLSw standard capabilities */
    uchar vendor_id[SNAP_OUIBYTES];
    uchar pad;
    uchar version_number;
    uchar release_number;
    ushort initial_pacing_window;
    char *version_string;
    uchar supp_saps[SAP_CV_BYTES];
    boolean num_tcp_sessions;
    boolean loop_prevent_support;
    boolean mac_exclusive;
    boolean nb_exclusive;
    queuetype macQ;
    queuetype nbnameQ;
 
    /* cisco capabilities */
    ushort cisco_version;
    boolean local_ack;
    boolean priority;
    ushort group;
    ushort cost;
    ushort fr_dlci;
    ushort pad1;
    boolean border;
    boolean pod_enabled;
    boolean spoof_and_seg;
    boolean xid_op_support;
    peer_t peer_type;
} peercap_t;


STRUCTURE_REF(fast_cache_t);
typedef boolean (*fast_wan2lan_type)(hwidbtype *, fast_cache_t *, paktype *);
typedef boolean (*fast_hes_wan2lan_type)(hwidbtype *, fast_cache_t *,
					 trrif_t *, uchar *, int);

/*
 * Fast Switching Cache Entry
 */
struct fast_cache_t_ {
    struct fast_cache_t_ *next;
    ulong local_id;
    ulong remote_id;
    peerhandle_t peer_handle;
    sys_timestamp last_used;
    hwidbtype *lanhwidb;
    fast_wan2lan_type fast_wan2lan;
    fast_hes_wan2lan_type fast_hes_wan2lan;
    uchar lmac[IEEEBYTES];   /* always the address of the LAN side */
    uchar rmac[IEEEBYTES];   /* always the address of the WAN side */
    uchar lsap;              /* always the sap on the LAN side */
    uchar rsap;              /* always the sap on the WAN side */
    uchar riflen;
    uchar rif[SRB_MAXRIF]; /* includes RC - router to LAN direction */
    boolean change_flag;
};

typedef int     (*fast_lan2wan_type)(hwidbtype *, fast_cache_t *, paktype *);
typedef int (*fast_les_lan2wan_type)(hwidbtype *, fast_cache_t *, paktype *, ulong);
typedef int (*fast_hes_lan2wan_type)(hwidbtype *, fast_cache_t *, uchar *, ulong);

/*
 * Remote Peer Entry
 */
struct peerentry_ {
    peerentry  *next;
    peer_status_t peer_status;          /* used to validate peer pointers */
    portlistentry *ple;
    queuetype *qptr;
    ipaddrtype ipaddr;
    boolean remove;                     /* remove peer if not connected */
    peer_t peer_type;                   /* conf, promiscuous, dynamic, pod */
    peerstatetype conn_state;
    boolean debug;
    boolean debug_fast_err;
    boolean debug_fast_pak;
    boolean debug_fst_seq;
    uchar cost;
    uchar xport_type;                   /* tcp, fst, or hdlc */
    uchar largest_frame;
    uchar largest_frame_def;
    peercap_t cap;

    /*
     * Connection and Keepalive Fields
     */
    ulong keepalive_int;         /* keepalive interval in clock ticks */
    ulong keepalive_count;       /* number of outstanding keepalives */
    sys_timestamp last_non_peer_traffic; /* does not count keepalives */
    sys_timestamp zero_ckt_time;
    boolean took_def_keep;       /* keep interval not sepcified in config */
    mgd_timer conn_timer;
    mgd_timer keep_timer;
    ushort conn_retries;
    ulong tcp_timeout;          /* tcp max retransmission time */

    /*
     * TCP Encapsulation Fields
     */
    ushort conn_mode;            /* normal or pasive */
    tcp_encaps *tcp_rd_t;
    tcp_encaps *tcp_wr_t;
    boolean active_open_wait;
    boolean tcp_fini_pending;
    boolean close_write_pending; /* wait for partner to close our write pipe */
    boolean connect_pod;
    boolean connect_dynam;       /* dynam peer scheduled to connect */
    queuetype drpQ;              /* dynamic peers start queue */
    ulong pod_inactivity;        /* time before pod is disconnected */
    ssp_hdr_start_t ssp_start;
    ushort port;
    ushort max_tcp_qlen;
    ushort tcp_explorer_hiwater;
    boolean local_ack;
    boolean priority;
    /* no sending by passive side while priority peers connect */
    boolean wait_pri_connect;
    peerentry *priority_peers[DLSW_PRIORITY_COUNT + 1];
    peerentry *priority_top;
    peerentry *primary_peer;
    peerentry *backup_peer;
    boolean backup_connect;    /* indicates when backup should conn */
    int backup_linger;         /* linger time for backup peers */
    sys_timestamp linger_timer;  /* linger timer for peer disconnect */
    int dynamic_inactivity;    /* inactivity timer(secs) for dynamic
				  peers */
    int no_llc;                 /* for tear down of dynamic peers */
    sys_timestamp conn_start_time;
    sys_timestamp uptime;
    sys_timestamp last_passive_open; /* see dlsw_peer_tcp.c for usage */
    ulong num_ckts;

    /*
     * Fields used to read TCP byte stream
     */
    rd_state_t rd_state;
    uchar *target;
    ulong sizeleft;
    paktype *pak;

    /*
     * FST Encapsulation Fields
     */
    iphdrtype *ip_header;               /* holds copy of IP header */
    ulong out_of_window_cnt;
    ulong ip_int_seqnumin;              /* # of last rcvd frame, from intrpt */
    ulong ip_int_seqnumout;             /* # of next sent frame, from intrpt */
    ushort ipcache_hash;                /* hash for ipaddress */
    fast_lan2wan_type fast_lan2wan;
    fast_les_lan2wan_type fast_les_lan2wan;
    fast_hes_lan2wan_type fast_hes_lan2wan;

    /*
     * Direct Encapsulation Fields
     */
    hwidbtype *outidb;                  /* direct interface idb */
    idbtype *swidb;                     /* direct interface idb */
    uchar macaddr[IEEEBYTES];           /* if a mac address is needed. */
    peerentry *idbnext;                 /* if more than one on this cable */
    ushort pad;

    /* 
     * LLC2 Encapsulation Fields 
     */
    uchar fr_state;                     /* state of frame relay connection */
    ushort fr_dlci;                     /* DLCI used for this peer */
    uint u_cep_id;                      /* User id - for validation */

    uint p_cep_id;                      /* CLSI provider ids */
    uint p_sap_id;

    /* 
     * Peer Output Filtering Fields
     * (lsap, netbios host-name, byte-pattern, mac access list and
     * exclusive dest mac) 
     */
    ulong dlsw_lsap_out;
    int dlsw_dmac_outlist;          /* dest mac acc list for dynamic peers */
    uchar dlsw_dmac_out[IEEEBYTES]; /* exclusive dmac for dynamic peers */
    netbios_acclisttype *netbios_h_oacf;
    netbios_acclisttype *netbios_b_oacf;

    /*
     * Peer Accounting
     */
    ulong drops;
    ulong pkts_rx;                      /* num pkts incoming */
    ulong pkts_tx;                      /* num pkts outgoing */

    /* stats for MIB */
    ulong disc_active_circuits;
    ulong in_data_packets;
    ulong out_data_packets;
    ulong in_data_octets;
    ulong out_data_octets;
    ulong in_cntl_packets;
    ulong out_cntl_packets;
    ulong cur_ex_sents;
    ulong icr_ex_rcvds;
    ulong cur_ex_rcvds;
    ulong icr_ex_sents;
    ulong nq_ex_sents;
    ulong nr_ex_rcvds;
    ulong nq_ex_rcvds;
    ulong nr_ex_sents;
    ulong circuit_creates;
    ulong config_opens;

    /*
     * Peer Transport Control Vectors.
     */
    core2peer_ret_t (*sender)(peerentry *, paktype *, 
                              boolean, peer_pak_cat, peer_put_t *);
    void    (*abort)(peerentry *);
    boolean (*connect)(peerentry *);
    boolean    change_flag;
    uint             peer_index;    /* for mib */
    sys_timestamp    config_tstamp;
    sys_timestamp    connect_tstamp;
    sys_timestamp    disc_tstamp;
};

/*
 * Local Peer for a DLSw Router
 */
typedef struct _localpeer_t {
    ipaddrtype ipaddr;
    ulong keepalive_int;         /* keepalive interval in clock ticks */
    ushort largest_frame;
    ushort conn_mode;            /* normal or pasive */
    ushort max_pacing_window;  /* maximum pacing window (flow control) */
    peerentry *border_peer;
    peerentry pod_defaults;
    peerentry prompeer_defaults;
    peercap_t cap;
    uchar reach_flag;           /* flag to differentiate between the configuration
                                   information entered using 'icanreach' and
                                   'icannotreach' */
    boolean icr_mac_excl_rem;   /* icanreach mac exclusive on remote ONLY */
    boolean icr_nb_excl_rem;    /* icanrech netbios exclusive on remote ONLY */
} localpeer_t;

typedef struct _hdr_info_t {
    uchar *dmac;
    uchar *ssap;
    uchar *dsap;
    netbios_header *nbh;
} hdr_info_t;

/*
 * Union for re-creating dsap/ssap pair from ssp/dlx headers
 */
typedef union _peer_lsap_u {
    ushort lsap;
    uchar sap[2];
} peer_lsap_u;


/*
 ********************************************************************
 * Peer Module Scope Externs
 ********************************************************************
 */

extern queuetype port_listQ;
extern boolean peer_debug_all;
extern localpeer_t *lpeer;
extern queuetype conn_dlx_peerQ[DLSW_MAX_PGROUP];
extern queuetype conn_dlsw_peerQ;
extern queuetype wait_conn_peerQ;
extern queuetype direct_inputQ;
extern queuetype disc_peerQ;
extern queuetype dlsw_peer_llc2Q;
extern int dlsw_peer_pid;
extern queuetype fast_hash_table[DLSW_FCT_HASH_SIZE];


/*
 ********************************************************************
 * Peer Module Function Prototypes
 ********************************************************************
 */

/*
 * Peer Internal Init Function Prototypes
 */
extern void peer_tcp_init(void);
extern void peer_fst_init(void);
extern void peer_direct_init(void);
extern void peer_llc2_init(peerentry *, idbtype *, boolean);
extern forktype dlsw_peer_process (void);

/*
 * Peer Transport Manager Function Prototypes
 */
extern boolean connect_priority_peers (peerentry *);
extern boolean peer_init_remote_xport (peerentry *, idbtype *, ulong);
extern core2peer_ret_t peer_put_pak (peerentry *, peer_put_t *);
extern void dlsw_peer_fsm (peerentry *peer, peereventtype event);

extern core2peer_ret_t dlsw_tcpd_sender (peerentry *, paktype *, 
                                     boolean, peer_pak_cat, peer_put_t *);
extern void    dlsw_tcpd_abort (peerentry *);
extern boolean dlsw_tcpd_connect (peerentry *peer);
extern void    dlsw_tcpd_listen (tcbtype *);

extern core2peer_ret_t dlsw_fst_sender (peerentry *, paktype *, 
                                     boolean, peer_pak_cat, peer_put_t *);
extern void    dlsw_fst_abort (peerentry *);
extern boolean dlsw_fst_connect (peerentry *);

extern core2peer_ret_t dlsw_if_sender (peerentry *, paktype *, 
                                     boolean, peer_pak_cat, peer_put_t *);
extern void    dlsw_if_abort (peerentry *);
extern boolean dlsw_if_connect (peerentry *);

extern void receive_direct_input (paktype *);


/*
 * Peer Data Base Function Prototypes
 */
extern peerentry *get_conn_peer_by_ip (ipaddrtype ipaddr);
extern peerentry *get_pod_peer_by_ip (ipaddrtype ipaddr);
extern boolean valid_peer (peerentry *);
extern boolean peer_ready_to_send (peerentry *);
extern boolean peer_create_local (ipaddrtype, uchar, ushort, boolean,
                                  ushort, ulong, uchar, boolean, boolean,
				  ushort, ushort);
extern void init_pod_defaults (void);
extern void init_prompeer_defaults (void);
extern void peer_remove_local (void);
extern peerentry *create_peer (boolean, ushort, ipaddrtype, peer_t,
                               portlistentry *, uchar, boolean, ushort,
                               ushort, ushort, hwidbtype *, uchar *,
			       idbtype *, boolean, uchar);
extern peerentry *peer_create_remote (ushort fr_dlci, ipaddrtype, peer_t, 
                                 portlistentry *,
                                 uchar, boolean, boolean, ushort, 
                                 ushort, long, ulong, 
                                 netbios_acclisttype *, netbios_acclisttype *, 
                                 ushort, hwidbtype *, uchar *,
				 peerentry *, ulong, int, int, int,
                                 int, uchar *, idbtype *swidb, uchar);
extern peerentry *new_remote_peer (ushort fr_dlci, ipaddrtype, peer_t, 
                            portlistentry *, uchar, 
                            boolean, boolean, ushort, ushort, long, ulong,
                            netbios_acclisttype *, netbios_acclisttype *,
                            ushort, hwidbtype *, uchar *, peerentry *,
			    ulong, int, int, int, int, uchar *,
				   idbtype *swidb, uchar);
extern void peer_remove_remote (peerentry *);
extern void peer_remove_all_remote (void);
extern void peer_nuke (peerentry *);
extern portlistentry *find_port_list (uchar list_num);
extern void remove_port_list (portlistentry *, uchar);
extern void remove_ring_list (portlistentry *, uchar);
extern void remove_bgroup_list (portlistentry *, uchar);
extern void dlsw_peer_newstate (peerentry *, peerstatetype);
extern char *peer_str (peerentry *, char *);
extern peerentry *find_priority_peer (ipaddrtype, ushort); 
extern peerentry *peer_using_port_list (uchar);
extern void update_peer_cap (peerentry *);

/*
 * Peer Manager DLX Peer Group / Border Peer Function Prototypes
 */
extern border_peer_ret_t add_border_peer_info (peerentry *);
extern void delete_border_peer_info (peerentry *);
extern void recv_member_to_border (peerentry *, paktype *);
extern void recv_border_to_member (peerentry *, paktype *);
extern void recv_border_to_border (peerentry *, paktype *);
extern void recv_bp_rsp (peerentry *, paktype *);
extern core2peer_ret_t send_peer_group (peer_msg_u *);
extern core2peer_ret_t send_bp_resp (peer_msg_u *);
extern core2peer_ret_t do_bcast (peer_put_t *);

/*
 * Peer Configuration Function Prototypes
 */
extern void peer_build_netbios_mask (nbentry_t *);

/*
 * Peer Connection Process Function Prototypes
 */
extern char *state_to_string (peerstatetype state);
extern char *event_to_string (peereventtype event);
extern ulong peer_next_conn_interval (peerentry *peer);

/*
 * Peer Executive Command Function Prototypes
 */
void dlsw_show_peer (parseinfo *);

/*
 * Peer Filter External Function Prototypes
 */
extern ulong peer_cap_filter (peerentry *, uchar, uchar);
extern boolean peer_out_filter (peerentry *, uchar, uchar,
				netbios_header *, uchar *dmac);

/*
 * DLSw Fast Cache Function Prototypes
 */
extern void dlsw_fct_house_keeping (void);
extern void dlsw_fct_clear_peer (peerhandle_t);
extern void dlsw_fct_clear_cache (void);
extern fast_cache_t *dlsw_tr2fct (uchar *macs, ushort *lsap);

/*
 * Peer Manager inline functions
 */

extern const char peer_get_pri_by_cat_msg[];
static inline int peer_get_pri_by_cat (peer_pak_cat category)
{
    switch (category) {
      case PEER_DIR_CIR_SETUP:
      case PEER_CIR_ADMIN:
      case PEER_CONN_ADMIN:
      case PEER_CONN_SETUP:
        return(PRIORITY_HIGH);

      case PEER_USER_INFO:
      case PEER_REACH_ADMIN:
        return(PRIORITY_NORMAL);

      case PEER_BCAST:
        return(PRIORITY_LOW);

      default:
        errmsg(PEER_BADVALUE, peer_get_pri_by_cat_msg, "category", category);
        return(PRIORITY_LOW);
    }
}

/*
 * Inline Functions
 */
extern const char DLSW_Revcd_seq_d_expected_d[];
static inline boolean dlsw_fst_acceptor (peerentry *peer, ushort seq_num_in)
{
    boolean out_of_window;

    /*
     * To save cycles, check normal case first.
     */
    if (seq_num_in == peer->ip_int_seqnumin) {
        peer->out_of_window_cnt = 0;
        if (peer->ip_int_seqnumin >= DLSW_FST_SEQNUM_MAX)
            peer->ip_int_seqnumin = 0;
        else
            peer->ip_int_seqnumin++;
        return(TRUE);
    }

    /*
     * We've received an unexpected sequence number.
     */
    out_of_window = FALSE;
    if (peer->ip_int_seqnumin <= DLSW_FST_HALF_SEQ_RANGE) {
        if ((seq_num_in < peer->ip_int_seqnumin) ||
            (seq_num_in > (peer->ip_int_seqnumin + DLSW_FST_ORDER_WIN))) {
            out_of_window = TRUE;
        }
    } else if ((seq_num_in < peer->ip_int_seqnumin) &&
            (seq_num_in > (peer->ip_int_seqnumin - DLSW_FST_ORDER_WIN))) {
        out_of_window = TRUE;
    }

    if (!out_of_window) {
        if (peer->debug_fst_seq) {
            if (seq_num_in != peer->ip_int_seqnumin)
                buginf(DLSW_Revcd_seq_d_expected_d, seq_num_in,
                       peer->ipaddr, peer->ip_int_seqnumin);
        }

        /*
         * Never trust a zero sequence number if we don't expect it.
         * This is because zero is used for processed level switched paks.
         * This is not a band-aid for a bug, just a little paranoia.
         */
        peer->out_of_window_cnt = 0;
        if (seq_num_in >= DLSW_FST_SEQNUM_MAX)
            peer->ip_int_seqnumin = 0;
        else if (seq_num_in != 0)
            peer->ip_int_seqnumin = seq_num_in + 1;

        return(TRUE);
    }

    PDEBUG_FSEQ(DLSW_Revcd_seq_d_expected_d, seq_num_in,
           peer->ipaddr, peer->ip_int_seqnumin);

    /*
     * Received seq num is not (expected + 16k) mod 32k. This
     * means it's likely a delayed packet that has finally arrived. 
     * Increment the out_of_window_cnt.
     *
     * If we have received more than DLSW_FST_OUT_OF_WIN_CNT paks in
     * a row that have been in the "delayed range", then move our expected
     * up 16K to get in sync with the sender. DLSW_FST_OUT_OF_WIN_CNT is
     * a magic number that gives us enough confidence to change our 
     * expected value.
     */
    peer->out_of_window_cnt++;
    if (peer->out_of_window_cnt > DLSW_FST_OUT_OF_WIN_CNT) {
        peer->ip_int_seqnumin = (peer->ip_int_seqnumin + 
                                 DLSW_FST_HALF_SEQ_RANGE) % 
                                 DLSW_FST_SEQNUM_MAX;
        peer->out_of_window_cnt = 0;
        return(TRUE);
    }

    return(FALSE);
}


static inline void dlsw_fst_incr_send (peerentry *peer, ushort *ip_id)
{
    *ip_id = peer->ip_int_seqnumout;
    if (peer->ip_int_seqnumout == DLSW_FST_SEQNUM_MAX)
            peer->ip_int_seqnumout = 0;
        else
            peer->ip_int_seqnumout++;
}


static inline ulong dlsw_get_seq_num (peerentry *peer) {
    ulong id;

    id = peer->ip_int_seqnumout;
    peer->ip_int_seqnumout++;
    return(id);
}

static inline fast_cache_t *dlsw_id2fct (ulong session_id)
{
   fast_cache_t *fct = (fast_cache_t *) session_id;

   /* 
    * validmem() checks that the pointer is in a valid memory pool's
    * memory range.
    */
   if (!validmem((void *) session_id))
       return(NULL);

   if (session_id) {
       /*
        * Make sure fct hasn't been freed and returned to a memory pool.
        */
       if (fct->local_id == session_id) {
           GET_TIMESTAMP(fct->last_used);
           return(fct);
       }
   }
   return(NULL);
}

#endif
