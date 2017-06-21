/* $Id: span.h,v 3.5.44.6 1996/08/03 23:38:03 fox Exp $
 * $Source: /release/112/cvs/Xsys/srt/span.h,v $
 *------------------------------------------------------------------
 * span.h -- Definitions for Spanning Tree Protocol
 *
 * 7-August-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: span.h,v $
 * Revision 3.5.44.6  1996/08/03  23:38:03  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.5.44.5  1996/07/17  19:32:46  snyder
 * CSCdi63233:  declare some char arrays const to save data
 *              remove a . to let the optimizer save more
 * Branch: California_branch
 *
 * Revision 3.5.44.4  1996/05/17  12:13:51  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.30.3  1996/05/06  00:01:32  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.30.2  1996/04/25  19:31:31  ioakley
 * CSCdi53651:  Add support for hidden message-age-increment option in
 *              "source-bridge spanning X" interface command.  Permits
 *              AST interoperability in mixed vendor bridge environments
 *              where hard-coded MAX_AGE defaults are insufficient for
 *              the network diameter and MAX_AGE is not configurable.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.30.1  1996/04/03  21:59:23  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.44.3  1996/05/09  14:46:44  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.44.2  1996/05/04  01:49:07  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface configuration & bridging
 *   (process-level) support.
 * - BVI smf functions and registry.
 * - BVI settable mtu support & dynamically select mtu for the BVI.
 * - 'bridge irb' & 'bridge <x> route/bridge <protcocol>' support.
 *
 * Constrained Multicast Flooding
 * - Add the CMF parser link points.
 * - Add the CMF data structures to spantype.
 * - Add the creation and deletion of the tbridge monitor.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Manage the CMF data structures when the spanning tree changes.
 *
 * Revision 3.5.44.1.18.1  1996/04/27  07:26:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.44.1.8.1  1996/04/08  02:09:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.44.1  1996/03/18  22:10:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.48.2  1996/03/25  02:21:14  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.48.1  1996/03/22  09:44:00  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.24.2  1996/03/07  02:30:43  cyoung
 * nv_gen supported added for managent-domain keyword.
 * action routines fleshed out.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.24.1  1996/02/27  21:19:36  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.2.2  1996/02/13  04:00:37  cyoung
 * add domain name to bridge group
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.5.2.1  1996/01/31  00:43:06  cyoung
 * Beginning code for autogeneration of bridge groups
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.5  1995/12/28  22:43:31  pitargue
 * CSCdi45879:  atm lane int configured as rfc 1483
 * always check how atm interfaces are configured.  don't assume past
 * states
 * of tbridge configurations.  also add some fields to show span and show
 * bridge
 * group to help debug this in the future.
 *
 * Revision 3.4  1995/12/27  18:59:31  hou
 * CSCdi45767:  Attempting to configure bridge-gr X sse causes
 *              %SYS-3-REGNONE:Registry 8 service 12 doesn't exist
 *              error message
 * - bgroup_sse parser node is removed from bgroup tree and dynamically
 *   added to the 7000 parser chain.
 *
 * Revision 3.3  1995/11/20  18:37:29  rbadri
 * CSCdi38662:  Configuring bridging on multiple ATM sub-interfaces causes
 *              hardware reset
 * Make span_fixup() recognize sub-interfaces, so that for ATM interfaces,
 * we can initiate a config change rather than a hardware reset.
 *
 * Revision 3.2  1995/11/17  18:55:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:30:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/08  16:49:40  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/07/09  21:07:20  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Introduce explicit, bridge-group specific route/bridge indicators
 * for CRB, and the commands to configure them.  Provide some smarts
 * to generate the appropriate 'bridge route' commands when CRB is
 * enabled in the presence of existing bridge groups.
 *
 * Revision 2.2  1995/07/06  00:25:11  wilber
 * CSCdi36382:  Age field is not updated correctly for filtered on-net
 * traffic
 * - Create counters to account for filtered on-net traffic
 * - Show those counters only in verbose option of show bridge output
 * - Get timestamp when creating a bridge table entry
 * - Update the bte age based on rx counter plus filtered rx counter
 *
 * Revision 2.1  1995/06/07  23:01:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SPAN_MAXINDEX                   64      /* Now increased from 10 to
                                                   accommodate Transparently
						   Bridged Virtual LANS */

#define MAX_SSE_BRIDGE_GROUPS           10       /* The SSE Bridging Trees are
						   hard-coded and in the absence
						   of dynamic memory allocation
						   restrict the number of Bridge
						   Groups it can provide for. */

#define SPAN_MAXPORTS			MAX_INTERFACES

#define SPAN_DEC_MAXAGE		        15	/* seconds (1 - 10000000) */
#define SPAN_DEC_HELLOTIME		 1	/* seconds (1 - 10) */
#define SPAN_DEC_FORWARDDELAY		30	/* seconds (10 - 200) */
#define SPAN_DEC_DEFAULT_PRIORITY0     	 0
#define SPAN_DEC_DEFAULT_PRIORITY1     	128
#define SPAN_DEC_BRIDGE_PRIORITY        128
#define	SPAN_DEC_PATHDIVISOR		100
#define	SPAN_DEC_DEFAULT_PATHCOST	10

#define SPAN_DEFAULT_TCTIME		30
#define SPAN_DEFAULT_TCNTIME		30
#define SPAN_DEFAULT_HOLDTIME		 1	/* one hello/second max */

#define SPAN_PROTO_IEEE		 1	/* IEEE compliant spanning tree */
#define SPAN_PROTO_DEC		 2	/* DEC compliant spanning tree */
/* SRB AST Support */
#define SPAN_PROTO_IBM		 3	/* IBM compliant spanning tree */

#define SPAN_IEEE_MAXAGE		20	/* 6.0 -- 40.0 */
#define SPAN_IEEE_HELLOTIME		2	/* 1.0 -- 10.0 */
#define SPAN_IEEE_FORWARDDELAY		15	/* 4.0 -- 30.0 */
#define SPAN_IEEE_HOLDTIME		1       /* no range allowed */
#define SPAN_IEEE_BRIDGE_PRIORITY      	32768	/* 0 -- 65536 */
#define SPAN_IEEE_PORT_PRIORITY		128	/* 0 -- 255 */
#define	SPAN_IEEE_PATHDIVISOR		10
#define	SPAN_IEEE_DEFAULT_PATHCOST	100

/* SRB AST support */
#define SPAN_IBM_MAXAGE			10	/* 10 seconds */
#define SPAN_IBM_HELLOTIME		2	
#define SPAN_IBM_FORWARDDELAY		4	/* 4 seconds */
#define SPAN_IBM_PATHDIVISOR		39     
#define SPAN_IBM_DEFAULT_PATHCOST	16
#define SPAN_IBM_PORT_PRIORITY          128     /* 0 -- 255 */
#define SPAN_IBM_MESSAGE_AGE_PASSTHRU   0       /* Used with hidden "source- */
                                                /* bridge spanning X" option */
                                                /* for message-age-increment */

#define IDBYTES				8
#define MESSAGE_AGE_INCREMENT		1
#define PRIORITY(ptr) ((ptr[0] << 8) | ptr[1])
#define SPAN_DEFAULT_PRIORITY  \
((SPAN_DEC_DEFAULT_PRIORITY0 << 8) | SPAN_DEC_DEFAULT_PRIORITY1)

#define	CCG_MAXGROUPS			10
#define	CCG_MAXCIRCUITS			10
#define CCG_DEFAULT_PAUSE_INTERVAL	0  /* 0 - 10000 milliseconds */


/*
 * Bridge Table Entry (BTE) data structure.
 * All of it's member names are prefixed with bte_
 */

/*
 * The sole purpose of this struct is to help the C compiler's alignment
 * fetish so we can do two compares (a long and a short) even if our data
 * is not long-word aligned.
 */

typedef struct sls_ {
    ushort one;
    ulong PACKED(two);
    ushort three;
} sls;

#define PERMIT_PORT_SIZE (((SPAN_MAXPORTS-1)/32)+1) /* Number of longwords */


/*
 * Bridge table entry  states; directly map to lec_mac_state_t_ in
 * lane/lec_private.h 
 */
typedef enum bte_state_t_ {
 BTE_READY,
 BTE_LANE_RESOLVED,
 BTE_LANE_FLUSH,
 BTE_LANE_FLOOD,
 BTE_LANE_REVERIFYING,
 BTE_LANE_DELETE
}bte_state_t;



struct btetype_ {
    /* interface on which the MAC address was learned as an SA */
    idbtype *bte_interface;

    /* on expiry, causes the bte to be checked for received packet activity */
    sys_timestamp bte_timer;

    /* aggregate received packet count when bte_timer last expired */
    ulong bte_prev_rxcount;

    ulong bte_rxcount;		/* aggregate received packet count */
    ulong bte_auto_rxcount;	/* autonomous received packet count */
    ulong bte_txcount;		/* aggregate forwarded packet count */
    ulong bte_auto_txcount;	/* autonomous forwarded packet count */

    /* aggregate count for filtered on-net packets */
    ulong bte_filtered_rxcount;
    ulong bte_filtered_txcount;

    /*
     * Note: Don't place bte_next at the beginning of btetype struct.
     * In the middle of printing loop, if aging process comes along
     * to clear the btes which should be aged out, zero out the
     * bte content and return them to betQ, we may suddenly print
     * the bteQ(showing 0000.0000.0000 entries) since the first four
     * byte of queuetype is used as the pointer pointing to the
     * next element in queue, which by accident is bte->next.
     * Without elegant way of inter-process communication, move
     * bte_next out of the area in which potential non-zero value could
     * be placed when being enqueued.
     */
    struct btetype_ *bte_next;

    /* bit maps indexed by port_id encoding MAC address access lists */
    ulong bte_permit_in[PERMIT_PORT_SIZE];
    ulong bte_permit_out[PERMIT_PORT_SIZE];

    /* 48-bit IEEE MAC address of the source of this bte */
    union {
	ushort sword[4];
	ulong lword[2];
	sls straddle;
    } bte_mac_addr;

    /* encapsulating link layer address for transit bridged packets */
    union {
	ushort fr_dlci;		/* Frame Relay transit bridging */
	x25map *x25map_ptr;	/* X.25 transit bridging */
	uchar proxy_mac[8];	/* FDDI and SMDS transit bridging */
	ushort vc_num;		/* ATM transit bridging */
    } bte_proxy_addr;

    short bte_action;		/* what to do with received packets */
    short bte_attributes;	/* learned or static */
    short bte_bce_addr;		/* address of autonomous bridge cache entry */
    bte_state_t bte_state;      /* State of BTE entry */
};

/*
 * Bridge table entry actions
 */
#define BRIDGE_FORWARD	0
#define BRIDGE_DISCARD	1
#define BRIDGE_RECEIVE	2

/*
 * Bridge table entry attributes
 */
#define BTE_STATIC      0x01
#define BTE_IFSPECIFIED 0x02
#define BTE_LAT         0x08	/* LAT multicast block */

#define BTE_ALIQUOT	300
#define BTE_MINIMUM	25
#define BTE_GC		(15*ONEMIN)
#define BTE_MAXAGE	(5*ONEMIN)


/*
 * Circuit Group data structure
 */
typedef struct ccgtype_ {
    boolean srcmac_based;		/* src-mac based or (src, dst) based */
    long ccg_pause_interval;		/* Pause interval to suspend tx */
    sys_timestamp ccg_pause_timer;	/* Pause interval timer */
    uint ccg_idb_count;			/* active idb count */
    uint ccg_avail_idb_count; 		/* available idb count */
    uint ccg_unavail_idb_count;		/* unavailable idb count */
    idbtype *idb_array[CCG_MAXCIRCUITS];
} ccgtype;

/*
 * Spanning tree data structure
 */
struct spantype_ {
    
    /*
     * cisco implementation variables
     */
    int spanindex;			/* spanning tree index */
    int port_count;			/* count of assigned ports */
    int tr_port_count;			/* Count of TR interfaces */
    int fddi_port_count;		/* Count of FDDI interfaces */
    idbtype *port[SPAN_MAXPORTS];	/* pointer to port data structures */
    queuetype floodQ;			/* queue of descriptors (tbifd's) */
					/* of flooding ports */
    int auto_flood_count;		/* count of SP floodable ports */
    boolean running;			/* TRUE if we're initialized */
    btetype *bridge_tbl[NETHASHLEN];	/* hash table of forwarding entries */
    queuetype mcast_router_ports;
    queuetype *mcast_groups[NETHASHLEN];
    queuetype shifaQ;                   /* queue for shadowing SP IFA list */
    ushort priority;			/* our bridge priority */

    int protocol;			/* whether IEEE, DEC or IBM compatible */
    ulong route_linktypes;            	/* C/IRB routing indicators */
    ulong bridge_linktypes;           	/* IRB bridging indicators */
    idbtype *irb_bg_vidb;         	/* IRB Bridge-group virtual Interface */
    boolean acquire;			/* TRUE if acquiring new addresses */
    boolean mcast_source;		/* TRUE if forwarding mcast source */
    boolean lat_svc_filter;		/* TRUE when filtering LAT services */
    uchar domain;		        /* IEEE spanning "domain" number */
    queuetype spanQ;			/* queue for received HELLOs. */
    int path_divisor;			/* bandwith divisor for IEEE vs DEC */
    boolean timer_running;
    sys_timestamp timer;
    uint cbus_bce_count;
    boolean sse_update_needed;		/* TRUE if the SSE should hurry */
    uint sse_interfaces;		/* num SSE interfaces in group */
    ccgtype cct_groups[CCG_MAXGROUPS];	/* Circuit group data structure */
    boolean cbus_copy_capable;		/* TRUE if cbus copy assistance
					   for flooding is enabled */
    unsigned long vlan_id;              /* Used to explicitly bind a
					   bridge group to a VLAN */
#define VTP_DOMAIN_NAME_MAX_LENGTH 33
    unsigned char domain_name[VTP_DOMAIN_NAME_MAX_LENGTH];      
    /* Which VLAN management domain the bridge group is in */

    unsigned char dname_length;         /* Length of the above
					   management domain */
					   
    /*              
     * MIB implementation variables
     */
    uint topology_change_count;
    uint learned_entry_discards;
    ulong topology_change_start_time;   /* Time point of last spanning tree change */
    /*
     * IEEE 802.1 protocol variables
     */
    uchar designated_root[IDBYTES];
    int root_path_cost;
    short root_port_id;
    idbtype *root_port;
    short max_age;
    short hello_time;
    short forward_delay;
    uchar bridge_id[IDBYTES];
    int bridge_max_age;
    int bridge_hello_time;
    int bridge_forward_delay;
    int bridge_aging_time;	/* time to expire unused forwarding entries */
    boolean topology_change_detected;
    boolean topology_change;
    boolean shortflag;
    int hold_time;
    int topology_change_time;
    int topology_change_notification_time;
    int hello_timer;
    int topology_change_notification_timer;
    int topology_change_timer;
    int shorttimer;		/* used by DEC variant only */
};

/*
 * Possible values for port_state field
 * We assume that LEARNING and FORWARDING are greater than the other states.
 */
#define PORT_DISABLED	0
#define PORT_BLOCKING	1
#define PORT_LISTENING	2
#define PORT_LEARNING	3
#define PORT_FORWARDING	4
#define PORT_BROKEN     5       /* defined for the MIB only */

/*
 * Bridge Protocol Data Unit packet format - IEEE 802.1 format.
 */

/*
 * WARNING:  The bit fields in the DEC and IEEE BPDU structures have been
 * made volatile to get around a MIPS gcc compiler bug that generates
 * an lhu instead of a lbu.
 */


#define BPDU_PROTOCOL    	0x0000
/*
 * In cisco's implementation, the high four bits of the version
 * indicate the "domain" of the spanning tree. The low order four
 * bits remain as the IEEE version. Note that this allows a domain
 * of "zero" to be compatible with other vendor IEEE bridges.
 */
#define BPDU_VERSION	 	0x00
#define BPDU_TYPE_CONFIG 	0x00
#define BPDU_TYPE_TCN    	0x80
#define BPDU_TCN_SIZE		 4
#define BPDU_CONFIG_BYTES 	35
#define BPDU_TCN_BYTES		 4

typedef struct bpduhdrtype_ {
    ushort protocol;            /* protocol ID */
    uchar version;		/* version identifier */
    uchar type;			/* BPDU type */
    volatile uchar tc_acknowledgement: 1; /* topology change acknowledgement */
    volatile uchar notusedflags: 6;
    volatile uchar tc: 1;	/* topology change flag */
    uchar root_id[IDBYTES];
    uchar root_path_cost[4];
    uchar bridge_id[IDBYTES];
    uchar port_id[2];
    uchar message_age[2];
    uchar max_age[2];
    uchar hello_time[2];
    uchar forward_delay[2];
} bpduhdrtype;

#define bpduheadstart(ptr) ((ptr)->network_start)

/*
 * Bridge Protocol Data Unit packet format - DEC format.
 */

#define SPAN_DEC_SHORTTIME  50

#define DEC_CODE       0xE1
#define DEC_VERSION    0x1

#define DEC_BPDU_LENGTH     27   /* sizeof(structure_type) not
                                    advisable on 4500 platform */
#define DEC_TYPE_HELLO 0x19
#define DEC_TYPE_TCN   0x02

typedef struct dechdrtype_ {
    uchar code;				/* E1 */
    uchar type;				/* 19, or 02 */
    uchar version;			/* 01 */
    volatile uchar tc:1;		/* topology change notification */
    volatile uchar tcnack:1;		/* TCN acknowledged */
    volatile uchar unused:5;		/* not used */
    volatile uchar shorttime:1;		/* use short timers */
    uchar root_id[IDBYTES];
    ushort root_path_cost;
    uchar bridge_id[IDBYTES];
    uchar port_id;
    uchar message_age;			/* was zero ??? */
    uchar hello_time;			/* range 1-10, default 1     01 */
    uchar max_age;			/* range 1-100, default 15   0F */
    uchar forward_delay;		/* range 10-200, default 30  1E */
} dechdrtype;

#define decheadstart(ptr) ((ptr)->network_start)

/*
 * Structure for shadowing IFA list in the SP.  These will reside on shifaQ
 * in the same order as the corresponding IFAs are linked in the SP.  The
 * sole purpose of this queue is to record that order since the list in
 * the SP cannot be read back.  The pointer to the corresponding IDB gives
 * us everything we want: the address of the IFA, the encapsulation type,
 * and the interface speed.
 */
typedef struct shifaQtype_ {
    struct shifaQtype_ *next;
    hwidbtype *hwidb;
} shifaQtype;

/*
 * LAT compression constants
 */

#define TYPE_LAT		0x6004
#define LAT_ZEROSIZE    	(ETHER_ARPA_ENCAPBYTES + 8)
#define LAT_ONEOFFSET  		6
#define LAT_ENTRYOVERHEAD	4

/*
 * LAT multicast filtering definitions
 */

#define LAT_SERVICE		0x28

/*
 * External Declarations
 */

extern spantype *spanarray[SPAN_MAXINDEX];
extern watched_queue *bridgeQ;
extern queuetype bteQ;
extern boolean hybridge_started;
extern int bte_count;
extern int bte_total;
extern const char badspan[];
extern const char nobridging[];
extern const char notsupp[]; 

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * bridge.c
 */
extern process hybridge_input(void);
extern void bridge_address_check(btetype *);
extern void tbridge_ager(void);
extern boolean bridge_enq(paktype *, boolean);
extern void bridge_routefailed(paktype *, boolean, boolean);
extern void bridge_entry_global_remove(uchar const *);
extern btetype *tbridge_bte_entry_locate(spantype *, uchar const *,
				         btetype **);
extern btetype *bridge_entry_locate(spantype *, uchar const *,
				    btetype **);
extern void tbridge_bte_entry_remove(spantype *, btetype *, btetype *);
extern void bridge_entry_remove(spantype *, btetype *, btetype *);
extern idbtype *dld_get_output_idb(spantype *, short, uchar *, uchar *);
extern void ccg_pause_timer_expiry(spantype*, int);
extern idbtype *ccg_get_next_idb (idbtype *);
extern char *tbridge_encap_string(idbtype *);
extern void bridge_address_check (btetype *mac);
extern void tbridge_clear_cache(int index);

/*
 * span1.c
 */
extern boolean dec_supersedes_port_info(spantype *, idbtype *, paktype *);
extern boolean designated_for_some_port(spantype *);
extern boolean designated_port(spantype *, idbtype *);
extern boolean ieee_supersedes_port_info(spantype *, idbtype *, paktype *);
extern boolean root_bridge(spantype *);
extern forktype span_process(spantype *);
extern void acknowledge_topology_change(spantype *, idbtype *);
extern void become_designated_port(spantype *, idbtype *);
extern void bpdu_enqueue(spantype *, idbtype *, paktype *);
extern void config_bpdu_generation(spantype *);
extern void configuration_update(spantype *);
extern void dec_enqueue(spantype *, idbtype *, paktype *);
extern void dec_out(idbtype *, paktype *);
extern void dec_received_config_bpdu(spantype *, idbtype *, paktype *);
extern void dec_record_config_information(idbtype *, paktype *);
extern void dec_record_config_timeout_values(spantype *, paktype *);
extern void dec_transmit_config(spantype *, idbtype *, paktype *);
extern void dec_transmit_tcn(idbtype *, paktype *);
extern void designated_port_selection(spantype *);
extern void forward_delay_timer_expiry(spantype *, idbtype *);
extern void hello_timer_expiry(spantype *, idbtype *);
extern void hold_timer_expiry(spantype *, idbtype *);
extern void ieee_out(idbtype *, paktype *, int);
extern void ieee_received_config_bpdu(spantype *, idbtype *, paktype *);
extern void ieee_record_config_information(idbtype *, paktype *);
extern void ieee_record_config_timeout_values(spantype *, paktype *);
extern void ieee_transmit_config(spantype *, idbtype *, paktype *);
extern void ieee_transmit_tcn(idbtype *, paktype *, uchar);
extern boolean make_blocking(spantype *, idbtype *);
extern boolean make_forwarding(spantype *, idbtype *);
extern void message_age_timer_expiry(spantype *, idbtype *);
extern void port_state_selection(spantype *);
extern void received_tcn_bpdu(spantype *, idbtype *);
extern void root_selection(spantype *);
extern void span_bridge_priority(spantype *, int);
extern void span_enqueue(paktype *);
extern void span_initialize(spantype *);
extern void span_path_cost(spantype *);
extern void span_port_disable(spantype *, idbtype *);
extern void span_port_enable(spantype *, idbtype *);
extern void span_port_priority(spantype *, idbtype *, ushort);
extern void ibm_span_port_priority(spantype *, idbtype *, ushort);
extern boolean ibm_port_priority_check(spantype *, ushort);
extern void tcn_timer_expiry(spantype *);
extern void topology_change_acknowledged(spantype *);
extern void topology_change_detection(spantype *);
extern void topology_change_timer_expiry(spantype *);
extern void transmit_config(spantype *, idbtype *);
extern void transmit_tcn(spantype *);
extern void ast_peer_transmit_config(spantype *, idbtype *, void *);
extern boolean ast_received_disjoint_peers(spantype *, idbtype *, paktype *);

/*
 * span2.c
 */
extern char *baction2string(int);
extern char *bstate2string(int);
extern spantype *span_lookup(int, boolean, int);
extern void gc_stations(spantype *, boolean);
extern void grow_stations(int);
extern void set_station_bits(int);
extern void show_macaddraccess(int);
extern void show_typeaccess(int);
extern void span_circfix(idbtype *, int /*ushort*/);
extern void span_circstate(idbtype *);
extern void span_clearif(spantype *, idbtype *);
extern void span_delete(int);
extern void span_delete_port(idbtype *);
extern void span_bandwidth_change(idbtype *idb);
extern void span_fixup(int, hwidbtype *, idbtype *);
extern void span_fastsetup(hwidbtype *);
extern void span_floodtable(spantype *);
extern void tbridge_network_fixup(idbtype *, int);
extern void tbridge_adjust(idbtype *);
extern void bridge_init(subsystype *);
extern void bridge_init_idb(idbtype *, boolean);
extern void bridge_hwidb_init(hwidbtype *);
extern void span_bridge_id_address_select(spantype *);
/* SRB AST support */
extern char *stype2string(spantype *);
extern void tbridge_check_cbus_copy_capable(spantype *);

/*
 * srt_media.c
 */
extern void srt_media_registry(void);

/*
 * srt_chain.c
 */
extern void srt_parser_init(subsystype *subsys);

/*
 * tbridge_bvi.c
 */
extern void tbridge_bvi_mtu_select(hwidbtype *);
