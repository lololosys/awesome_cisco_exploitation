/* $Id: vines_private.h,v 3.6.12.5 1996/08/07 09:03:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_private.h,v $
 *------------------------------------------------------------------
 * vines_private.h -- definitions for Banyan Vines
 *
 * August 1992, David Hampton
 * from a module dated February 1990, David Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_private.h,v $
 * Revision 3.6.12.5  1996/08/07  09:03:46  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.12.4  1996/07/24  00:56:20  slin
 * CSCdi63865:  Rework obsolete os call in vines
 * Branch: California_branch
 *
 * Revision 3.6.12.3  1996/05/11  16:22:15  thille
 * CSCdi51502:  Missed some duplicate strings in vines_route.h
 * Branch: California_branch
 * Save another 392 bytes.
 *
 * Revision 3.6.12.2  1996/05/11  16:17:38  thille
 * CSCdi51397:  Duplicate strings in vines code
 * Branch: California_branch
 * Clean up some duplicate literals and save 1140 bytes in gs7-j-m.
 *
 * Revision 3.6.12.1  1996/03/18  22:32:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.3  1996/03/16  07:52:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.20.2  1996/03/13  02:04:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.20.1  1996/02/20  21:46:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/14  18:31:01  ehk
 * CSCdi48247:  vines bluemail clients on serverless cannot get time
 * changes to avoid unaligned memory access and compiler padding in
 * VINES packet structures.
 *
 * Revision 3.5  1996/02/08  20:14:00  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4  1995/12/01  21:39:10  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.3  1995/11/17  17:58:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:47:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:45:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/22  00:53:34  slin
 * CSCdi36007:  show apple route detail halts system
 *
 * Revision 2.4  1995/08/17  21:34:54  slin
 * CSCdi38846:  show vines int can halt system
 *
 * Revision 2.3  1995/07/07  20:40:16  hampton
 * Minor vines timer cleanup. [CSCdi36825]
 *
 * Revision 2.2  1995/06/28 09:32:30  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:14:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define NO_INLINES

#ifndef NO_INLINES
#define STATIC static
#define INLINE inline
#else
#define STATIC
#define INLINE
#endif

/*
 * Structure declarations
 */
STRUCTURE_REF(vinespathtype);
STRUCTURE_REF(vinesneighbortype);
STRUCTURE_REF(vinesroutetype);
STRUCTURE_REF(vinesnettype);
STRUCTURE_REF(routingdecision);
STRUCTURE_REF(vrtp_redirecttype);
STRUCTURE_REF(vrtp_type);
STRUCTURE_REF(vsrtp_header);
STRUCTURE_REF(vsrtp_request);
STRUCTURE_REF(vsrtp_update);
STRUCTURE_REF(vinesipctype);
STRUCTURE_REF(vinesipcsocket);
STRUCTURE_REF(vinesnetrpctype);
STRUCTURE_REF(serverentry);

#include "sys_registry.h"
#include "vines.h"
#include "address.h"
#include "../util/tree.h"
#include "mgd_timers.h"

/*
 * Vines Global Variables
 */
extern boolean vinesarp_debug;
extern boolean vinescache_debug;
extern boolean vinesecho_debug;
extern boolean vinespkt_debug;
extern int     vinespkt_debug_list;
extern boolean vines_proxy_debug;
extern boolean vines_reinit_debug;
extern boolean vinesrtp_debug;
extern int     vinesrtp_debug_list;
extern boolean vines_state_debug;
extern boolean vinestable_debug;
extern int     vinestable_debug_list;
extern boolean vinestest_debug;
extern boolean vines_unknown_debug;
extern boolean banyan_bugs_debug;

extern pid_t vines_pid, vines_bg_pid, vines_rtr_pid;

/*
 * Miscellaneous definitions
 */
#define ONETICK (ONESEC / 5)	/* 200 milliseconds */
#define VRTP_LAST 7
#define VRTP55_LAST 5
#define VINES_PORT_MAXKNOWN	0x001A
#define VERBOSE_DEBUG	0x02
#define VINESBUFLEN 80

#define VST_ITEMLEN     32
#define VST_DOMAINLEN   16
#define VST_ORGLEN      16
#define VST_TOTALLEN    (VST_ITEMLEN + VST_DOMAINLEN + VST_ORGLEN)
#define VST_DESCRIPTION 32
#define VST_PASSWORD    16


typedef struct vinesstringtype_ {
    ushort length;
    uchar  data[0];
} vinesstring;

typedef struct vinessmallstringtype_ {
    uchar  length;
    uchar  data[0];
} vinessmallstring;

typedef struct vinesnametype_ {
    struct vinesnametype_ *next;
    ulong  net;
    ushort host;
    char   name[0];
} vinesnametype;


#define RUN_DQUEUE_SELF(queue, ptr) \
    for ((ptr) = (queue).parent; \
	 (((ptr) != &(queue)) && ((ptr)->flink != (ptr))); \
	 (ptr) = (ptr)->flink)
#define RUN_DQUEUE(queue, ptr) \
    for ((ptr) = (queue).flink; \
	 (((ptr) != &(queue)) && ((ptr)->flink != (ptr))); \
	 (ptr) = (ptr)->flink)
#define RUN_DQUEUE_W_PREV(queue, ptr, prev) \
    for ((ptr) = (queue).flink, (prev) = &(queue); \
         (((ptr) != &(queue)) && ((ptr)->flink != (ptr))); \
	 (prev) = (ptr), (ptr) = (ptr)->flink)
#define RUN_DQUEUE_W_NEXT(queue, ptr, next) \
    for ((ptr) = (queue).flink, (next) = (ptr)->flink; \
         (((ptr) != &(queue)) && ((ptr)->flink != (ptr))); \
	 (ptr) = (next), (next) = (next)->flink)
#define DQUEUE_EMPTY(queue) \
    ((queue).flink == &(queue))
#define DQUEUE_HEAD(queue, ptr) \
    ((ptr) == &(queue))
#define DQUEUE_ENTRY(queue, ptr) \
    (!DQUEUE_HEAD((queue),(ptr)))

enum {VT_ARP,
      VT_CHANGED,
      VT_WAKEUP,
      VT_QUERY,
      VT_SYNC,
      VT_REASSEMBLY,
      VT_RETRY,
      VT_RC,
      VT_SRTP_START,
      VT_PERIODIC,
      VT_IPC_ACK,
      VT_IPC_IDLE,
      VT_IPC_RETRANS,
      VT_SVC_SVC,
      VT_SVC_TREE};

#define VMSG_STATECHANGE	0x8001

/*
 *  MAC LEVEL
 */

/*
 * Vines Echo Packet definition.  Used by Banyan's interface diagnostic
 * programs.
 */
typedef struct vinesmacechotype_ {
    ushort seq;				/* Sequence Number */
    uchar  data[0];
} vinesmacechotype;


/**********************************************************************
 *
 *			   VINES ADDRESSES
 *
 **********************************************************************/
typedef struct simplevinesaddr_ {
    ulong net;
    ushort host;
} simplevinesaddr;

#define VINES_NONCE_LEN 14
typedef uchar vinesmacaddress[6];
typedef uchar vinesnonce[VINES_NONCE_LEN];

typedef struct vinesaddress_ {
    ushort family;
    union {
	
	/*
	 * Vines IP - family 3
	 */
	struct {
	    uchar  nnet[V_NETBYTES];
	    ushort host;
	    ushort port;
	    uchar  fill[6];
	} vip;

	/*
	 * DOD IP - family 9
	 */
	struct {
	    ushort port;
	    ulong  PACKED(ipaddress);
	    uchar  fill[8];
	} tcpip;

	/*
	 * Appletalk - family 14
	 */
	struct {
	    ushort network;
	    uchar  node;
	    uchar  fill1;
	    uchar  socket;
	    uchar  protocol;
	    uchar  fill[8];
	} atalk;
    } PACKED(f);
} vinesaddress;

/*
 * Families
 */
#define VINES_ADDR_NATIVE 3
#define VINES_ADDR_DODIP  9
#define VINES_ADDR_ATALK  14

/*
 * Network Numbers
 */
#define VINES_NET_MFG	 0xffe00000	/* Assigned by Banyan */
#define VINES_NET_SEQ	 0x001fffff	/* Unique within Mfg Id. */
#define VINES_NET_BANYAN 0x1fe00000	/* Mfg Ids reserved for Banyan Servers */
#define VINES_NET_CISCO	 0x30000000	/* Mfg ID assigned to cisco */
#define VINES_NON_BANYAN 0x20000000	/* First non-Banyan address */
#define VINES_LAST_BANYAN 0x1FFFFFFF	/* Last Banyan address */

/*
 * Client Numbers
 */
#define VINES_SERVER	  0x0001
#define VINES_1ST_CLIENT  0x8001
#define VINES_LAST_CLIENT 0xfffe
#define VINES_ALLNETS     0xFFFFFFFF
#define VINES_ALLHOSTS    0xFFFF
#define VINES_UNKNOWNNET  0x00000000
#define VINES_UNKNOWNHOST 0x0000

/**********************************************************************
 *
 *			    NETWORK LEVEL
 *
 **********************************************************************/

/*
 * Vines IP header structure definition is in vines.h
 */

/*
 * Vines IP checksum field
 */
#define VINES_NOCHECKSUM 0xFFFF

/**********************************************************************
 *
 *                     Vines IP protocol types
 *
 **********************************************************************/
#define VINES_IPC	1
#define VINES_SPP	2
#define VINES_ARP	4
#define VINES_RTP	5
#define VINES_ICP	6
#define NVINESIPTYPES	7
/*
 * These next definitions make no sense, as they are never used on top of
 * VINES IP.  This is, however, where Banyan defines them, so they are
 * here for consistency.  These are used by the NSM service.
 */
#define VINES_BA	7	/* Banyan Block Async Protocol      */
#define VINES_HDLC	8	/* HDLC Protocol                    */
#define VINES_X25	9	/* CCITT X.25 level 3		    */
#define VINES_SMDS	21	/* Cisco Defines - SMDS 	    */
#define VINES_FRAME	22	/* Cisco Defined - Frame Relay	    */


/*
 * Vines IP Transport Control Byte
 */
#define VINES_CLASS	  0x70	/* broadcast class or flags */
#define VINES_HOPS	  0x0f	/* hop count                */
#define VINES_TTL0	  0x00
  
#define VINES_B_SERVERS	  0x40	/* class - all networks with servers */
#define VINES_B_LANS	  0x30	/* class - all LANs */
#define VINES_B_GT4800	  0x20	/* class - all networks w/ speed > 4800 */
#define VINES_B_NOCHARG	  0x10	/* class - all networks w/ no charges */
#define VINES_B_ALL	  0x00	/* class - all networks */
  
#define VINES_REDIRECT	  0x40	/* flags - request redirection */
#define VINES_METRIC	  0x20	/* flags - request last hop metric */
#define VINES_ERROR	  0x10	/* flags - request error notification */
#define VINES_NOFLAGS	  0x00

/*
 * Vines IP miscellaneous
 */
#define	VINES_MINSIZE		16
#define	VINES_MAXSIZE	      1500
#define VINES_FORWMAX		25

/**********************************************************************
 *
 *			  "TRANSPORT" LEVEL
 *
 **********************************************************************/

/*
 * Vines InterProcess Communications Protocol (IPC) header structure
 * definition is in vines_ipc.h.
 */


/**********************************************************************
 *
 *			      VINES ARP
 *
 **********************************************************************/

/*
 * Structure and enum declarations for the configuration of 'arp'.  The
 * constants in the enums are required because this value is reported
 * directly via SNMP and must never change.
 */

typedef struct varp_header_ {
    ushort op;			/* top byte always zero */
    ulong  PACKED(net);
    ushort host;
} varp_header;

typedef struct vsarp_header_ {
    uchar  version;		/* always one */
    uchar  op;
    ulong  PACKED(net);
    ushort host;
    ulong  PACKED(sequence);	/* current RTP sequence number */
    ushort metric;		/* metric for the interface (and bridges) */
} vsarp_header;

#define VARP_VER_NSARP 0
#define VARP_VER_SARP  1

#define VARP_SVC_REQ  0
#define VARP_SVC_RESP 1
#define VARP_ASS_REQ  2
#define VARP_ASS_RESP 3
#define VARP_LAST     4

#define VARP_LEARNING_TIME (2 * ONEMIN)
typedef enum varp_states_ {
    VARP_OFF,
    VARP_INITIALIZING,
    VARP_LEARNING,
    VARP_SERVICE,
    VARP_LAST_STATE = VARP_SERVICE
} varp_states;

typedef enum varp_type_ {
    VARP_NEVER   = 0,
    VARP_ALWAYS  = 1,
    VARP_DYNAMIC = 2
} varp_type;

/**********************************************************************
 *
 *			      VINES ICP
 *
 **********************************************************************/

typedef struct vinesicptype_ {
    ushort type;
    ushort info;
    uchar  oldpak[0];
} vinesicptype;

#define VICP_EXCEPTION      0
#define VICP_METRIC	    1
#define VICP_LAST	    2

#define V_ICP_COPIEDBYTES   40

/**********************************************************************
 *
 *			  VINES error codes
 *
 **********************************************************************/

#define VINES_ERR_NO_PROCEDURE	101	/* Procedure does not exist */
#define VINES_ERR_HOST_UNR	155	/* Net/Host Unreachable */
#define VINES_ERR_NO_SOCKET	157	/* No Socket Listener */
#define VINES_ERR_NO_PROTOCOL	160	/* Protocol does not exist */


/**********************************************************************
 *
 *
 *
 **********************************************************************/

typedef struct vinesechotype_ {
    struct vinesechotype_ *next;
    tt_soc *tty;
    ulong  active;
    sys_timestamp timeout;		 /* time to give up on packet */
    sys_timestamp timesent;		 /* when packet was sent */
    ulong  deltatime;			 /* round trip time */
    ushort port;
    uchar  how;
} vinesechotype;

typedef struct vinestracetype_ {
    struct vinestracetype_ *next;	 /* used for queueing */
    tt_soc *tty;			 /* tty used by this instance */
    ulong  active;			 /* flag to say this packet active */
    ushort ttl;				 /* time to live */
    sys_timestamp timeout;		 /* time to give up on packet */
    sys_timestamp timesent;		 /* when packet was sent */
    ulong  deltatime;			 /* round trip time */
    ushort port;			 /* port for this probe */
    uchar  how;				 /* type of termination */
    ulong  net;				 /* who sent reply */
    ushort host;
    ulong  currentnet;			 /* used to parse output */
    ushort currenthost;
  } vinestracetype;

typedef struct vtrace_info_ {
    struct vtrace_info_ *next;		 /* used for queueing */
    tt_soc *tty;			 /* tty used by this instance */
    queuetype new_dataQ;
    queuetype saved_dataQ;
    sys_timestamp timeout;		 /* when to quit */
    ushort port;			 /* port for this probe */
    ushort print;			 /* next reply index to print */
    ushort flags;
} vtrace_info;

#define VTRACE_ABORT		0x01
#define VTRACE_TIMEOUT		0x02
#define VTRACE_INTERRUPTED	0x04

#define VTRACE_MAXWAITTIME	(5 * ONESEC)

/*
 * Vines access lists (normal and extended)
 */

typedef struct vinesacctype_ {
	struct vinesacctype_ *next;    
	boolean grant;
	ulong  srcnet;
	ushort srchost;
	ushort srcsocket;
	ulong  smasknet;
	ushort smaskhost;
	ushort smasksock;
	ulong  dstnet;
	ushort dsthost;
        ushort dstsocket;
	ulong  dmasknet;
	ushort dmaskhost;
	ushort dmasksock;
	ushort protocol;
} vinesacctype;


/*
 * Enum declarations for the configuration of 'propagate' and serverless.  The
 * constants in the enums are required because this value is reported
 * directly via SNMP and must never change.
 */
typedef enum vprop_type_ {
    VPROP_NEVER   = 0,
    VPROP_ALWAYS  = 1,
    VPROP_DYNAMIC = 2
} vprop_type;

typedef enum vsvless_type_ {
    VSVLESS_NEVER        = 0,
    VSVLESS_DYNAMIC      = 1,
    VSVLESS_ALWAYS       = 2,
    VSVLESS_ALWAYS_BCAST = 3
} vsvless_type;

enum VINES_SIGNALS {
    VINES_SIGNAL_IDB_UP = 0x80000000,
    VINES_SIGNAL_IDB_DOWN,
    VINES_SIGNAL_VC_ADDED,
    VINES_SIGNAL_VC_REMOVED,
    VINES_SIGNAL_IDB_DELETION_PENDING
};

/*
 * Vines private IDB structure
 */
struct vinesidbtype_ {
    struct vinesidbtype_ *next;
    struct vinesidbtype_ *next_deleted;
    idbtype *phys_idb;			/* the physical interface descriptor */

    /*
     * Configuration Information
     */
    uint vines_metric;			/* cost of using this interface */
    boolean metric_is_default;		/* is this the default metric */
    int vines_enctype;			/* encapsulation type */
    int vines_accesslist;		/* Vines Access List */
    sys_timestamp vines_redirect_time;	/* time of next allowed redirect */
    ulong vines_redirect_interval;	/* min interval between redirects */
    boolean vines_split_disabled;	/* disable split horizoning of updates */
    ulong vines_lineup;			/* vines' idea of the link state */
    boolean vines_routecache;		/* fast switching requested */
    boolean vines_fastokay;		/* fast switching req. and supported */
    ulong vines_wakeup_interval;	/* time between routing updates */
    mgd_timer wakeup_timer;		/* time of next routing update */
    ulong vines_update_delta_only;	/* update per wakeup / changes only */
    ulong vines_update_count;		/* number of updates left to send */
    mgd_timer query_timer;		/* time of next query */
    ulong vines_query_count;		/* number of queries left to send */
    int input_router_filter;		/* filter source of rcvd route info */
    int input_network_filter;		/* filter contents of rcvd route info */
    int output_network_filter;		/* filter contents of sent route info */
    boolean deletion_pending;		/* on its way out.... */
    int svc_slot;			/* "slot" number for Banyan Services */
    int svc_line;			/* "line" number for Banyan Services */

    /*
     * ARP processing
     */
    varp_type arp_enabled;		/* configured on/off/dynamic */
    varp_states arp_state;		/* current state */
    mgd_timer arp_timer;

    /*
     * Network state and topology.
     */
    dqueue_t path_list;
    mgd_timer sync_timer;
    ulong v0_servers;
    ulong v0_routers;
    ulong v0_clients;
    ulong v1_servers;
    ulong v1_routers;
    ulong v1_clients;
    vprop_type propagate;		/* propagate lan only broadcasts */
    vsvless_type serverless;		/* do grody hacks for serverless net */

    /*
     * Counters
     *
     * Frames/bytes in/out are on the main idb structure.
     */
    /* failed before testing if vines packet */
    ulong rx_not_enabled;		/* inputs but vines not on this if */
    ulong rx_too_small;			/* inputs smaller than min size */
    ulong rx_too_large;			/* inputs larger than max size */
    ulong rx_bad_size;			/* size mismatch (MAC - Encap != VIP) */

    /* accepted or forwarded */
    ulong rx_local_dest;		/* inputs for this router */
    ulong rx_bcast_in;			/* broadcast received */
    ulong rx_forwarded;			/* inputs forwarded out another interface */
    ulong rx_no_route;			/* inputs dropped because of no route */
    ulong rx_zero_hop_count;		/* inputs not forwarded bec hop count */

    /* internal processing */
    ulong rx_checksum_error;		/* inputs with checksum errors */
    ulong rx_arpv0_type[VARP_LAST];	/* received ARP message of type x */
    ulong rx_arpv1_type[VARP_LAST];	/* received ARP message of type x */
    ulong rx_arpv0_illegal;		/* received ARP w/invalid code */
    ulong rx_arpv1_illegal;		/* received ARP w/invalid code */
    ulong rx_arpv0_discarded;		/* received ARP but discarded */
    ulong rx_arpv1_discarded;		/* received ARP but discarded */
    ulong rx_icp_error;			/* received ICP error message */
    ulong rx_icp_metric;		/* received ICP metric message */
    ulong rx_icp_illegal;		/* received ICP w/invalid code */
    ulong rx_ipc;			/* received IPC message */
    ulong rx_rtpv0_type[VRTP_LAST];	/* received RTP message of type x */
    ulong rx_rtpv1_type[VRTP55_LAST];	/* received RTP message of type x */
    ulong rx_rtpv0_illegal;		/* received RTP w/invalid code */
    ulong rx_rtpv1_illegal;		/* received RTP w/invalid code */
    ulong rx_rtpv0_bcast;		/* received RTP as bcast */
    ulong rx_rtpv1_bcast;		/* received RTP as bcast */
    ulong rx_spp;			/* received SPP message */
    ulong rx_ip_unknown;		/* unknown IP protocol */
    ulong rx_ipc_unknown;		/* unknown IPC port */

    /* broadcast propagation */
    ulong rx_bcast_helpered;		/* inputs helpered to another server */
    ulong rx_bcast_forwarded;		/* broadcast forwarded to other interface(s) */
    ulong rx_bcast_duplicate;		/* duplicate broadcast received */

    /* more specific per-protocol information */
    ulong rx_echo;			/* received IPC layer echo message */
    ulong rx_mac_echo;			/* received MAC layer echo message */
    ulong rx_proxy_reply;

    /* general transmit counts */
    ulong tx_unicast;			/* unicast originated here */
    ulong tx_bcast;			/* broadcast originated here */
    ulong tx_forwarded;			/* forwarded from another interface */
    ulong tx_helpered;			/* inputs helpered to another server */

    /* forwarded frames information */
    ulong tx_failed_encaps;		/* msg to be sent failed encapsulation */
    ulong tx_failed_access;		/* msg to be sent failed access list */
    ulong tx_failed_down;		/* msg to be sent but interface down */

    /* broadcast propagation information */
    ulong tx_bcast_forwarded;		/* bcast forwarded out interface */
    ulong tx_bcast_helpered;		/* bcast helpered toward a server */
    ulong tx_notbcast_toward_source;	/* bcast not trans, leads to source */
    ulong tx_notbcast_noservers;	/* bcast not trans, class servers only */
    ulong tx_notbcast_notlan;		/* bcast not trans, class lan only */
    ulong tx_notbcast_notgt4800;	/* bcast not trans, class >4800 bps only */
    ulong tx_notbcast_ppcharge;		/* bcast not trans, class no per packet charges */

    /* more specific per-protocol information */
    ulong tx_arpv0_type[VARP_LAST];
    ulong tx_arpv1_type[VARP_LAST];
    ulong tx_icp_error;			/* sent ICP error message */
    ulong tx_icp_metric;		/* sent ICP metric message */
    ulong tx_ipc;			/* sent IPC message */
    ulong tx_rtpv0_type[VRTP_LAST];	/* sent RTP message of type x */
    ulong tx_rtpv1_type[VRTP55_LAST];	/* sent RTP message of type x */
    ulong tx_rtpv0_bcast;		/* sent RTP as bcast */
    ulong tx_rtpv1_bcast;		/* sent RTP as bcast */
    ulong tx_spp;			/* sent SPP message */

    ulong tx_echo;			/* sent IPC layer echo message */
    ulong tx_mac_echo;			/* sent MAC layer echo message */
    ulong tx_proxy;

    /*
     * Saved Information Counters
     *
     * Set by a "clear vines counters" command
     */
    ulong si_rx_not_enabled;		/* inputs but vines not on this if */
    ulong si_rx_too_small;		/* inputs smaller than min size */
    ulong si_rx_too_large;		/* inputs larger than max size */
    ulong si_rx_bad_size;		/* size mismatch (MAC - Encap != VIP) */
    ulong si_rx_local_dest;		/* inputs for this router */
    ulong si_rx_bcast_in;		/* broadcast received */
    ulong si_rx_forwarded;		/* inputs forwarded out another interface */
    ulong si_rx_no_route;		/* inputs dropped because of no route */
    ulong si_rx_zero_hop_count;		/* inputs not forwarded bec hop count */
    ulong si_rx_checksum_error;		/* inputs with checksum errors */
    ulong si_rx_arpv0_type[VARP_LAST];	/* received ARP message of type x */
    ulong si_rx_arpv1_type[VARP_LAST];	/* received ARP message of type x */
    ulong si_rx_arpv0_illegal;		/* received ARP w/invalid code */
    ulong si_rx_arpv1_illegal;		/* received ARP w/invalid code */
    ulong si_rx_arpv0_discarded;	/* received ARP but discarded */
    ulong si_rx_arpv1_discarded;	/* received ARP but discarded */
    ulong si_rx_icp_error;		/* received ICP error message */
    ulong si_rx_icp_metric;		/* received ICP metric message */
    ulong si_rx_icp_illegal;		/* received ICP w/invalid code */
    ulong si_rx_ipc;			/* received IPC message */
    ulong si_rx_rtpv0_type[VRTP_LAST];  /* received RTP message of type x */
    ulong si_rx_rtpv1_type[VRTP55_LAST];/* received RTP message of type x */
    ulong si_rx_rtpv0_illegal;		/* received RTP w/invalid code */
    ulong si_rx_rtpv1_illegal;		/* received RTP w/invalid code */
    ulong si_rx_rtpv0_bcast;		/* received RTP as bcast */
    ulong si_rx_rtpv1_bcast;		/* received RTP as bcast */
    ulong si_rx_spp;			/* received SPP message */
    ulong si_rx_ip_unknown;		/* unknown IP protocol */
    ulong si_rx_ipc_unknown;		/* unknown IPC port */
    ulong si_rx_bcast_helpered;		/* inputs helpered to another server */
    ulong si_rx_bcast_forwarded;	/* broadcast forwarded to other interface(s) */
    ulong si_rx_bcast_duplicate;	/* duplicate broadcast received */
    ulong si_rx_echo;			/* received IPC layer echo message */
    ulong si_rx_mac_echo;		/* received MAC layer echo message */
    ulong si_rx_proxy_reply;

    ulong si_tx_unicast;		/* unicast originated here */
    ulong si_tx_bcast;			/* broadcast originated here */
    ulong si_tx_forwarded;		/* forwarded from another interface */
    ulong si_tx_failed_encaps;		/* msg to be sent failed encapsulation */
    ulong si_tx_failed_access;		/* msg to be sent failed access list */
    ulong si_tx_failed_down;		/* msg to be sent but interface down */
    ulong si_tx_notbcast_toward_source;	/* bcast not trans, leads to source */
    ulong si_tx_notbcast_noservers;	/* bcast not trans, class servers only */
    ulong si_tx_notbcast_notlan;	/* bcast not trans, class lan only */
    ulong si_tx_notbcast_notgt4800;	/* bcast not trans, class >4800 bps only */
    ulong si_tx_notbcast_ppcharge;	/* bcast not trans, class no per packet charges */
    ulong si_tx_bcast_forwarded;	/* bcast forwarded out interface */
    ulong si_tx_bcast_helpered;		/* inputs helpered to another server */
    ulong si_tx_arpv0_type[VARP_LAST];
    ulong si_tx_arpv1_type[VARP_LAST];
    ulong si_tx_icp_error;		/* sent ICP error message */
    ulong si_tx_icp_metric;		/* sent ICP metric message */
    ulong si_tx_ipc;			/* sent IPC message */
    ulong si_tx_rtpv0_type[VRTP_LAST];  /* sent RTP message of type x */
    ulong si_tx_rtpv1_type[VRTP55_LAST];/* sent RTP message of type x */
    ulong si_tx_rtpv0_bcast;		/* sent RTP as bcast */
    ulong si_tx_rtpv1_bcast;		/* sent RTP as bcast */
    ulong si_tx_spp;			/* sent SPP message */
    ulong si_tx_echo;			/* sent IPC layer echo message */
    ulong si_tx_mac_echo;		/* sent MAC layer echo message */
    ulong si_tx_proxy;

    /*
     * Record time taken to send an update
     */
    sys_timestamp update_start_time;
    sys_timestamp update_end_time;
};

#define IDB_VINES_BCAST (IDB_IEEEMASK | IDB_ATM)


/***********************************************************************
 *
 *
 ***********************************************************************/

extern boolean vines_short_access_check(ulong, ushort, int); /*vines_access.c*/

static inline boolean do_vines_pkt_debug (ulong net, ushort host)
{
    return(vinespkt_debug &&
	   vines_short_access_check(net, host, vinespkt_debug_list));
}

static inline boolean do_vines_rtp_debug (ulong net, ushort host)
{
    return(vinesrtp_debug &&
	   vines_short_access_check(net, host, vinesrtp_debug_list));
}

static inline boolean do_vines_rtp_v_debug (ulong net, ushort host)
{
    return((vinesrtp_debug & VERBOSE_DEBUG) &&
	   vines_short_access_check(net, host, vinesrtp_debug_list));
}

static inline boolean do_vines_table_debug (ulong net, ushort host)
{
    return(vinestable_debug &&
	   vines_short_access_check(net, host, vinestable_debug_list));
}

#ifdef __INTERFACE_H__

#define FOR_ALL_VIDBS(idb) \
    for ((idb) = vines_idbQ->qhead; (idb); (idb) = (idb)->next)

static inline vinesidbtype *
vines_getidb (struct idbtype_ *idb)
{
    return idb_get_swsb(idb, SWIDB_SB_VINES);
}

static inline boolean vines_is_lan (vinesidbtype *idb)
{
    return(idb_is_ieeelike(idb->phys_idb));
}

static inline boolean vines_is_wan (vinesidbtype *idb)
{
    return(!vines_is_lan(idb));
}

static inline boolean vines_valid_net (ulong net)
{
    return((net != VINES_UNKNOWNNET) && (net != VINES_ALLNETS));
}

static inline boolean vines_valid_host (ushort host)
{
    return((host != VINES_UNKNOWNHOST) && (host != VINES_ALLHOSTS));
}

static inline boolean vines_is_this_addr (ulong net, ushort host)
{
    return((net == vines_network) && (host == vines_host));
}

static inline boolean vines_is_bcast_addr (ulong net, ushort host)
{
    return((net == VINES_ALLNETS) || (host == VINES_ALLHOSTS));
}

static inline boolean server_is_banyan (ulong network)
{
    return((network & VINES_NET_MFG) <= VINES_NET_BANYAN);
}

static inline paktype *vip_getbuffer (int length)
{
    paktype *pak;

    pak = getbuffer(sizeof(vinesiptype) + length);
    if (pak) {
	pak->transport_start = pak->network_start + sizeof(vinesiptype);
	pak->datagramsize = sizeof(vinesiptype) + length;
    }
    return(pak);
}

/*
 *
 */
static inline boolean vines_no_updates (vinesidbtype *idb)
{
    return(idb && (idb->vines_update_delta_only) &&
	   idb->phys_idb && !(idb_is_ieeelike(idb->phys_idb)));
}

static inline void vines_flush_queue (queuetype *queue)
{
    paktype *pak;

    while ((pak = dequeue(queue)) != NULL)
	datagram_done(pak);
}

static inline ulong vines_frames_input (hwidbtype *hwidb)
{
    return(hwidb->counters.rx_frames[ACCT_PROTO_VINES] +
	   hwidb->counters.inputs_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.inputs_auto[ACCT_PROTO_VINES]);
}

static inline ulong vines_frames_output (hwidbtype *hwidb)
{
    return(hwidb->counters.tx_frames[ACCT_PROTO_VINES] +
	   hwidb->counters.outputs_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.outputs_auto[ACCT_PROTO_VINES]);
}

static inline ulong vines_frames_deltain (hwidbtype *hwidb)
{
    idb_si_counters_t *sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);

    return(hwidb->counters.rx_frames[ACCT_PROTO_VINES] +
	   hwidb->counters.inputs_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.inputs_auto[ACCT_PROTO_VINES] -
	   sic->rx_frames[ACCT_PROTO_VINES]);
}

static inline ulong vines_frames_deltaout (hwidbtype *hwidb)
{
    idb_si_counters_t *sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);

    return(hwidb->counters.tx_frames[ACCT_PROTO_VINES] +
	   hwidb->counters.outputs_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.outputs_auto[ACCT_PROTO_VINES] -
	   sic->tx_frames[ACCT_PROTO_VINES]);
}

static inline ulong vines_frames_fastin (hwidbtype *hwidb)
{
    return(hwidb->counters.inputs_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.inputs_auto[ACCT_PROTO_VINES]);
}

static inline ulong vines_frames_fastout (hwidbtype *hwidb)
{
    return(hwidb->counters.outputs_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.outputs_auto[ACCT_PROTO_VINES]);
}

static inline ulong vines_bytes_input (hwidbtype *hwidb)
{
    return(hwidb->counters.rx_bytes[ACCT_PROTO_VINES] +
	   hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.rx_cumbytes_auto[ACCT_PROTO_VINES]);
}

static inline ulong vines_bytes_output (hwidbtype *hwidb)
{
    return(hwidb->counters.tx_bytes[ACCT_PROTO_VINES] +
	   hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.tx_cumbytes_auto[ACCT_PROTO_VINES]);
}

static inline ulong vines_bytes_deltain (hwidbtype *hwidb)
{
    idb_si_counters_t *sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);

    return(hwidb->counters.rx_bytes[ACCT_PROTO_VINES] +
	   hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.rx_cumbytes_auto[ACCT_PROTO_VINES] -
	   sic->rx_bytes[ACCT_PROTO_VINES]);
}

static inline ulong vines_bytes_deltaout (hwidbtype *hwidb)
{
    idb_si_counters_t *sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);

    return(hwidb->counters.tx_bytes[ACCT_PROTO_VINES] +
	   hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_VINES] +
	   hwidb->counters.tx_cumbytes_auto[ACCT_PROTO_VINES] -
	   sic->tx_bytes[ACCT_PROTO_VINES]);
}

#endif

/**********************************************************************
 *
 *		   Generic Timer Queue Manipulation
 *
 **********************************************************************/

static inline char *vtimer_pretty_value (sys_timestamp timer, char *zero,
					 char *temp)
{
    if (!TIMER_RUNNING(timer))
	return(zero);
    sprint_ticks_in_dhms(temp, TIME_LEFT_SLEEPING(timer));
    return(temp);
}

static inline char *vtimer_pretty_print (mgd_timer *timer, char *zero, char *temp)
{
    if (!mgd_timer_running(timer))
	return(zero);
    sprint_ticks_in_dhms(temp, mgd_timer_left_sleeping(timer));
    return(temp);
}

/*
 * vip_send_pak
 *
 * Fill in all local fields in a VINES packet and send it.
 */
extern vinesidbtype *vip_send_pak_common(paktype *, ulong, ushort, ulong,
					 ushort, int, int, int);

static inline vinesidbtype *vip_send_pak (
    paktype *pak,
    ulong dnet,
    ushort dhost,
    int protocol,
    int flags,
    int ttl)
{
    return(vip_send_pak_common(pak, dnet, dhost, vines_network,
			       vines_host, protocol, flags, ttl));
}


/***********************************************************************
 *
 *
 ***********************************************************************/

/*
 * Vines Queues
 */
extern watched_queue *vinesQ;		/* VINES input queue */
extern watched_queue *vinesbgQ;		/* VINES input queue */
extern watched_queue *vinesrtpQ;	/* VINES RIP socket queue */
extern queuetype vinesechoQ;		/* VINES protocol level echo queue */
extern queuetype vines_nameQ;		/* VINES names */
extern queuetype vines_proxyQ;

/*
 * Vines Global Variables
 */
extern boolean vines_onlyone;
extern boolean vines_decimal_addresses;
extern char *const vines_ip_types[];
extern char *const vicp_types[];
extern ulong vinescache_debug_count;
extern mgd_timer vines_timer, vines_idb_timer, vines_changed, vines_periodic_timer;
extern mgd_timer vsrtp_retry_timer, vsrtp_reassembly_timer, vsrtp_sync_timer;
extern mgd_timer vrtp_rc_timer;
extern mgd_timer varp_timer, vipc_timer, vines_svc_timer, vines_bkgnd_timer;
extern ulong vines_slowest_update;
extern ushort vines_client;		/* next Vines host number to assign */
extern vinesacctype *vines_access_lists[];
extern vinesidbtype *vinesnullidb;
extern boolean vsrtp_enabled;
extern boolean waste_bandwidth_because_banyan_whines;
extern char vines_hostname[];

extern rbTree *vines_neighbor_tree;     /* moved from vines_neighbor.c *bs* */
extern uint vines_paths, vines_neighbor_version;     /* ditto *bs* */

extern rbTree *vines_network_tree;      /* moved from vines_routes.c *bs* */
extern uint vines_routes, vines_net_version;         /* ditto *bs* */


/*
 * Vines shared strings.  Some are used in inlines, other are used by
 * multiple routines.  Goal is to avoid duplicated strings in the
 * compiled image.
 */

/*
 * vines_neighbor.c
 */
extern const char vines_cannot_find_neighbor_msg[];
extern const char vines_neighbor_malloc_str[];

/*
 * vines_netrpc.c
 */
extern const char vines_VINES_str[];
extern const char vines_Vines_str[];
extern const char vines_invalid_string_len_msg[];
extern const char vines_cannot_alloc_netrpc_packet_msg[];

/*
 * vines_route.c
 */
extern const char vsrtp_network_suppression_msg[];
extern const char vines_cannot_find_network_msg[];
extern const char vines_vines_network_str[];

/*
 * vines_rtp.c
 */
extern const char vsrtp_no_packet_msg[];
extern const char vrtp_mac_rif_msg[];
extern const char vines_network_metric_msg[];
extern const char vines_network_metric_seq_flags_msg[];
extern const char vines_seconds_msg[];

/*
 * vines_services.c
 */
extern const char vsvc_str[];


/*
 * Vines Function Declarations
 *	 vines/vines_access.c
 *	 vines/vines_arp.c
 *	 vines/vines_echo.c
 *	 vines/vines_input.c
 *	 vines/vines_neighbor.c
 *	 vines/vines_route.c
 *	 vines/vines_support.c
 *	 vines/vines_test.c
 */

/* vinesasm.S */
extern ushort vines_crc1(vinesiptype *);
extern void vines_qcsum(uchar *, int /*ushort*/, int);

/* vines_access.c */
extern void vines_access_command(parseinfo *);
extern boolean vines_access_check(paktype *, vinesacctype *);
extern char *vines_access_str(char *, int, vinesacctype *);
extern void vines_access_display(int);

/* vines_arp.c */
extern char *varp_state_name(varp_states state);
extern void varp_timer_expired(mgd_timer *);
extern void varp_idb_init(vinesidbtype *);
extern void varp_init(void);
extern void vines_arp(paktype *);
extern uchar *varp_decode_packet(uchar *, int *, void (*fn)(const char *fmt, ...));
extern void varp_display_interface(vinesidbtype *);
extern void varp_new_state(vinesidbtype *, varp_states);
extern void varp_notify(vinesidbtype *);
extern void varp_display_timer(void);

/* vines_echo.c */
extern void vines_mac_echo(paktype *);
extern void vines_ping(char *);
extern boolean vines_ping_parse(char *, int, int *, char **, boolean *);
extern boolean vines_echo_send(vinesipcsocket **, vinesechotype *, int);
extern void vines_echo_bounce(vinesipcsocket *, uchar *, int);
extern void vines_ipc_echo_sink(vinesipcsocket *, uchar *, int);
extern void vines_icp_echo_sink(paktype *);
extern void vines_ciscotrace_showprobe(boolean, boolean, vinestracetype *, ushort);
extern void vines_ciscotrace_sendprobe(vinesipcsocket *, vinestracetype *);
extern void vtrace_rcv_data(ushort, void *);
extern void vines_init_echo(void);
extern void register_snmp_vines_echo(void);

/* vines_input.c */
extern void vines_rxtypes_update(idbtype *);
extern void vines_traffic_checkpoint(void);
extern void vines_start(void);
extern process vines_input(void);
extern boolean vines_forus(paktype *);
extern boolean vines_forward(paktype *);
extern vinesidbtype *vines_send1(paktype *, routingdecision *);
extern vinesidbtype *vines_send(paktype *);
extern void vines_flood_bcast(paktype *, boolean, boolean, vinesidbtype *);
extern process vines_background(void);
extern boolean vines_forward_nearest(paktype *);
extern void vicp_send_metric(paktype *, int);
extern void vines_crc(paktype *);
extern boolean vines_validcrc(paktype *);
extern void vip_send_broadcast(paktype *, vinesidbtype *, vinesidbtype *, int, int, int);
extern vinesidbtype *vip_send_specific(paktype *,vinesneighbortype *,vinespathtype *, int, int);
extern vinesidbtype *vip_rtp_send_specific(paktype *,vinespathtype *, int);

/* vines_media.c */
extern int vines_compute_metric(paktype *);
extern boolean vines_interface_up(vinesidbtype *);
extern void vines_media_registry(void);
extern int vines_compute_default_metric(idbtype *);
extern char *vines_dlc(char *, hwaddrtype *);
extern boolean vines_is_my_saddr(paktype *pak);
extern ulong vines_pick_default_net(void);
extern boolean vines_validate_net(ulong net);

/* vines_neighbor.c */
#include "vines_srtp_fsm.h"
#include "vines_neighbor.h"

/* vines_route.c */
#include "vines_route.h"

/* vines_rtp.c */
#include "../ui/common_strings.h"	/* needed for vines_rtp.h */
#include "vines_rtp.h"

/* vines_srtp.c */
#include "vines_srtp.h"

/* vines_support.c */
extern void vines_routing(boolean);
extern void vines_parser_init(void);
extern void vines_command(parseinfo *);
extern void vinesif_command(parseinfo *);
extern void vines_interface_display(vinesidbtype *);
extern void vines_traffic_display(vinesidbtype *);
extern void show_vines(parseinfo *);
extern void vines_clear_cmd(parseinfo *);
extern void vines_name_add(char *, ulong, ushort);
extern void vines_name_delete(char *);
extern void vines_name_display(char *);
extern void vinesbug(paktype *, char *);
extern void vinesbuggw(paktype *, vinesneighbortype *, char *);
extern void print_vinespak(paktype *);
extern void buginf_vinespak(paktype *);
extern void vines_set_wakeup_time(vinesidbtype *target);
extern int vines_printf_bigz(const char **, char **, tt_soc *, va_list *, boolean,
			     int, int, int, int);
extern int vines_printf_littlez(const char **, char **, tt_soc *, va_list *, boolean,
				int, int, int, int);
extern char *vpath_id(vinespathtype *);
extern void vtimer_dump_path_queue(mgd_timer *, char *, char *);
extern void vines_bandwidth_changed(idbtype  *);
extern ulong vines_extract_serverid(vinesaddress *address);
extern char *vines_printf_address(char *string, vinesaddress *address);
extern void vines_swif_erase(idbtype *);

/* vines_test.c */
extern void vinest_cksum(void);
extern boolean vinest_send_arp(parseinfo *, paktype *);
extern boolean vinest_send_echo(parseinfo *, paktype *);
extern boolean vinest_send_icp(parseinfo *, paktype *);
extern boolean vinest_send_loop(parseinfo *, paktype *);
extern void vinest_build_ipc(paktype *);
extern boolean vinest_send_ipc_spp(parseinfo *, paktype *, int);
extern boolean vinest_send_ipc(parseinfo *, paktype *);
extern boolean vinest_send_spp(parseinfo *, paktype *);
extern boolean vinest_buildpak(paktype *);
extern boolean vinest_send_custom(parseinfo *, paktype *);
extern void vinest_send(parseinfo *);
extern void vines_test_command(parseinfo *);
extern void vines_test(parseinfo *);
extern void vines_test_init(subsystype *);

/* vines_icp.c */
extern void vicp_send_error(paktype *, int, boolean);
extern void vines_icp_receive(paktype *, vinesidbtype *);

/* vines_ipc.c */
extern void vines_init_ipc(void);
extern void vipc_set_metric(ulong, ushort, ushort);
extern void vipc_display(void);
extern void vines_ipc_receive(paktype *pak, vinesidbtype *idb);
extern void vines_process_connections(void);
extern uchar *display_vinespak_ipc(uchar *, int *, void (*fn)(const char *fmt, ...));
extern uchar *display_vinespak_spp(uchar *, int *, void (*fn)(const char *fmt, ...));
extern vinesaddress *vines_create_port(int, ushort);

/* vines_netrpc.c */
extern void vines_init_netrpc(void);
extern ushort vines_netrpc_extract_procedure(paktype *);

/* vines_services.c */
extern void vines_init_services(void);
extern void vsvc_process_service(mgd_timer *);
extern void vsvc_process_tree(void);
extern void vsvc_start_services(void);
extern void vsvc_stop_services(void);
extern void vsvc_new_neighbor(ulong, vinesidbtype *);
extern void vss_command(parseinfo *csb);
extern void vst_command(parseinfo *csb);

/*
 * vines_setup.c
 */
extern void vines_setup_global(setup_struct *);
extern void vines_setup_int(setup_struct *);
extern void vines_setup_end(setup_struct *);
