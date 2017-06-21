/* $Id: ospf.h,v 3.6.8.13 1996/08/16 22:59:16 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf.h,v $
 *------------------------------------------------------------------
 * ospf_flood.c -- Open Shortest Path First Interior Gateway Routing Protocol
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf.h,v $
 * Revision 3.6.8.13  1996/08/16  22:59:16  rbadri
 * CSCdi66287:  LANE: Clean up IPX/OSPF releated initializaion
 * Branch: California_branch
 * Function to setup broadcast flag for LANE.
 *
 * Revision 3.6.8.12  1996/08/16  22:10:43  myeung
 * CSCdi62650:  OSPF NSSA does not flush type 5 LSAs
 * Branch: California_branch
 *
 * Revision 3.6.8.11  1996/08/07  09:00:52  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.8.10  1996/07/19  07:26:55  myeung
 * CSCdi63444:  OSPF router should not be ABR if it dont see neighbor from
 * backbone
 * Branch: California_branch
 *
 * Revision 3.6.8.9  1996/07/18  08:36:04  myeung
 * CSCdi62058:  DC-feature : loses neighbor with non DC code
 * Branch: California_branch
 * - Add ospf_dc_down_check() to bring P2P DC interface
 *   state to IFS_DOWN when nbr is lost
 *
 * Revision 3.6.8.8  1996/07/09  19:12:36  myeung
 * CSCdi61982:  OSPF network LSA is missing
 * Branch: California_branch
 * - Refine the fix for CSCdi47402
 *
 * Revision 3.6.8.7  1996/07/01  07:44:42  myeung
 * CSCdi56125:  Router links information incorrect for an internal stub
 * area router
 * Branch: California_branch
 * - Schedule SPF when we flush the old DR network LSA
 *
 * Revision 3.6.8.6  1996/06/25  21:15:52  myeung
 * CSCdi45110:  OSPF does not load balance over External routes
 * Branch: California_branch
 * - Modify OSPF router route data structure and external route calculation
 *
 * Revision 3.6.8.5  1996/06/18  01:47:52  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.6.8.4  1996/05/23  12:06:38  klmj
 * CSCdi45519:  OSPF: Incorrect metric calculation for External Type
 * 1 routes (note:  this also fixes CSCdi44939)
 * Branch: California_branch
 *
 * Revision 3.6.8.3  1996/05/23  07:37:48  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.6.8.2  1996/04/30  23:14:14  myeung
 * CSCdi56225:  OSPF type 7 LSA is not handled correctly
 * Branch: California_branch
 * - Fix request list for type 7 LSA
 * - Remove translated type 5 LSA when type 7 LSA is not reachable
 *
 * Revision 3.6.8.1  1996/04/16  19:01:38  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/29  10:33:07  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.5  1996/02/04  06:07:35  myeung
 * CSCdi48132:  Extract additional OSPF specific information out of pdbtype
 *
 * Revision 3.4  1996/01/25  12:39:57  myeung
 * CSCdi47498:  Clean up OSPF redistribution
 *
 * Revision 3.3  1995/11/18  18:48:09  myeung
 * CSCdi43168:  Clean up OSPF flooding
 * - Consolidate flooding functions
 *
 * Revision 3.2  1995/11/17  17:35:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/10/14  00:26:32  myeung
 * CSCdi41883:  OSPF LSA fragmentation crashes router
 * - Allow to send update containing huge LSA
 *
 * Revision 2.11  1995/10/02  18:21:15  myeung
 * CSCdi39931:  OSPF should retransmit LSA immediately after receiving ACK
 *
 * Revision 2.10  1995/09/26  11:35:41  myeung
 * CSCdi40729:  IP OSPF Hello Interval is not retained after reload
 *
 * Revision 2.9  1995/09/25  11:49:44  myeung
 * CSCdi40976:  OSPF should send back most recent copy when old lsa is
 * received
 *
 * Revision 2.8  1995/09/23  21:40:48  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.7  1995/09/19  20:53:43  myeung
 * CSCdi20080:  OSPF crashed when sending LSA in send_ls_update
 *
 * Revision 2.6  1995/09/11  23:50:28  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.5  1995/09/05 22:08:57  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.4  1995/08/07  19:19:40  myeung
 * CSCdi26731:  OSPF is turned off on an interface when secondary address
 * is removed
 *
 * Revision 2.3  1995/06/28  03:21:23  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.2  1995/06/20 06:55:07  myeung
 * CSCdi34206:  OSPF Memory Leak on border area router
 *
 * Revision 2.1  1995/06/07  21:09:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * External declarations
 */

/*
 * Forward declarations
 */

/*
 * Storage
 */

#define MAXOSPFPACKETSIZE 1500
#define MAX_OSPF_DATA (MAXOSPFPACKETSIZE - (IPHEADERBYTES(NOPT)+OSPF_HDR_SIZE))
#define OSPF_VERSION 	2
#define OSPF_RUN_TIME   100	/* 100 milliseconds */
#define OSPF_SCAN_RUN_TIME   10	/* 10 milliseconds */
#define OSPF_SCAN_INTERVAL    (ONEMIN)
#define INIT_OSPF_SUSPEND(pdb)	(GET_TIMESTAMP(pdb->ospf_run_time))
#define OSPF_SHOULD_SUSPEND(pdb) \
    (CLOCK_OUTSIDE_INTERVAL((pdb)->ospf_run_time, OSPF_RUN_TIME))
#define LSA_PROCESSCOUNT 20

#define OSPF_AUTH_NONE		0
#define OSPF_AUTH_CT		1
#define OSPF_AUTH_MD		2

#define OSPF_MD5		0

#define MAX_BAD_CKSUM_QUEUE_SIZE	10

typedef struct l_link_ {
   struct l_link_ **prev;
   struct l_link_ *next;
} l_link;

typedef struct l_queue_ {
   l_link *head;
   l_link **tail;
   ulong count;
} l_queue;

typedef struct ospf_bad_cksum_ {
    struct ospf_bad_cksum_ *next;	/* next entry in the queue */
    ushort received_checksum;		/* checksum received in packet */
    ushort computed_checksum;		/* checksum computed */
    ushort length;			/* length of ip_data */
    uchar  ip_data[0];			/* ip data packet starts here */
} ospf_bad_cksum;
    
typedef struct ospf_md_key_ {
  	l_link link;
	uchar key[16];			/* 16 byte Key */
	uchar encrypt_key[37];		/* Encrypted Key */
	uchar id;			/* Key id */
	uchar encrypt_type;		/* Encrypt type */
	uchar algorithm;		/* Algorithm used */
	sys_timestamp start_time;	/* When this key start */
} ospf_md_key;

typedef struct lsdb_list_  {
    struct lsdb_list_ *list_next;
    struct lsdb_list_ *list_prev;
    lsdbtype *list_db_ptr;
    lsatype *list_lsa_ptr;
    sys_timestamp list_time;		/* Last time this db is sent */ 
    boolean list_flood;
    ulong   list_retrans_count;		/* Number of time we retransmit
					   this LSA */
    short   list_refcount;
} lsdb_list;

struct list_queue_ {
    lsdb_list *q_first;
    lsdb_list *q_last;
    ulong      count;
};

enum {
    OSPF_SUMUPDATE_TIMER,
    OSPF_EXUPDATE_TIMER,
    OSPF_EXLOCK_TIMER,
    OSPF_FADDR_TIMER,
    OSPF_AR_REBUILD_RTR_TIMER,
    OSPF_AR_EXLOCK_TIMER,
    OSPF_AR_SUMLOCK_TIMER,
    OSPF_AR_DELAY_TIMER,
    OSPF_AR_UPDATE_TIMER,
    OSPF_AR_AGE_TIMER,
    OSPF_IF_HELLO_TIMER,
    OSPF_IF_WAIT_TIMER,
    OSPF_IF_DELAY_TIMER,
    OSPF_IF_ACK_TIMER,
    OSPF_IF_FLUSH_NET_TIMER,
    OSPF_NBR_DEAD_TIMER,
    OSPF_NBR_DBD_RETRANS_TIMER,
    OSPF_NBR_DBD_HOLD_TIMER,
    OSPF_NBR_POLL_TIMER,
    OSPF_NBR_LSA_RETRANS_TIMER,
};

struct ospf_idb_info_ {
	ulong enabled;			/* ospf_enabled on this interface */
	idbtype *next;			/* pointer to next ospf idb in queue */
	areatype *area_ptr;		/* pointer to attached area */
	mgd_timer oi_hello_master_timer;/* Interface master timer for hello
					   process */
	mgd_timer oi_hello_timer;	/* hello timer */
	mgd_timer oi_wait_timer;	/* wait timer */
	mgd_timer oi_delay_timer; 	/* flood delay timer */
	mgd_timer oi_ack_timer; 	/* ack timer */
	mgd_timer oi_flush_net_timer;	/* timer for flushing network LSA */
	nbrtype *oi_myself;		/* A nbr data about myself. Used
					   in DR election */
	nbrtype *oi_nbr;		/* list of nbrs on this interface */
	nbrtype *oi_cfg_nbr;		/* list of configured nbrs on 
					   this interface */
	nbrtype *dr;			/* ptr to designated neighbor */
	nbrtype *bdr;			/* ptr to bdr neighbor */
	nbrtype *oi_old_dr;		/* last dr on this idb */
	ulong vnbr_id;			/* transit area for virtual links */
	ulong vnbr_ipaddr;		/* transit area for virtual links */
	areatype *vlink_trans;		/* transit area for virtual links */
	ulong wait_interval;  		/* wait interval before dr election */
	ushort if_event_count;     	/* count of OSPF events on this idb */
	uchar nbrcnt;	 		/* Count of neighbors */
	uchar nbr_fcnt;			/* Count of neighbors in full state */
	list_queue ack_queue;		/* linked list of nbrs waiting acks */ 
	uchar auth_key[8]; 		/* 64 bits of auth */
	uchar encrypt_auth_key[20];	/* encryted auth */
	uchar encrypt_type; 		/* type of encryption used in config */
	uchar if_state;			/* state of inf as seen by OSPF */
	ushort delay_flag;		/* flood delay flag */
	ospf_md_key *youngest_md_key;   /* The youngest message digest key */
	boolean oi_cfg_demand_circuit;	/* TRUE if it is configured as DC */
	boolean oi_demand_circuit;	/* TRUE if we agree to run is as DC */
	l_queue md_key_q;		/* List of message digest key */
	ulong old_md_nbr_count;		/* Num. of neighbor using old key */
	ushort if_cost[8];		/* cost of using interface */
	ushort def_if_cost[8];		/* default cost of using interface */
	ulong hello_interval;		/* hello interval (seconds) */
	ulong def_hello_interval;	/* default hello interval (seconds) */
	ulong oi_poll_interval;		/* poll interval for demand circuit */
	ulong oi_def_poll_interval;	/* default poll interval for DC */
	ulong dead_interval;		/* dead interval (seconds) */
	ulong def_dead_interval;	/* default dead interval (seconds) */
	ulong retr_interval; 		/* retransmitting interval */
	ulong def_retr_interval; 	/* default retransmitting interval */
	ushort if_transdly;		/* delay to transmit a lsa  */
	ushort def_if_transdly;		/* default delay to transmit a lsa  */
	ushort if_type;			/* configured network type */
	ushort def_if_type;		/* default configured network type */
	ushort oi_nbr_ex_load_count;	/* Any neighbor in exchange/load */
	uchar rtr_pri; 			/* priority - if 0 not elig */
	uchar def_rtr_pri; 		/* default priority - if 0 not elig */
	uchar oi_suppress_hello_count;	/* Number of nbr has hello
					   suppressed */
	ipaddrtype oi_network;		/* network the interface is attached */
	ipaddrtype oi_mask;		/* the mask */
};

struct ospf_pdb_info_ {
    mgd_timer ospf_update_timer;
    mgd_timer ospf_hello_master_timer;	/* Master timer for hello process */
    mgd_timer ospf_sumupdate_timer;	/* summary link update timer */
    mgd_timer ospf_exupdate_timer;  	/* external link update timer */
    mgd_timer ospf_exlock_timer;    	/* external link lock timer */
    mgd_timer ospf_faddr_timer;   	/* forward address check timer */
    timer_t *ospf_clean_timer;    	/* clean up timer */
    ushort ospf_normal_area_count;	/* Number of normal area */
    ushort ospf_stub_area_count;	/* Number of stub area */
    ushort ospf_nssa_area_count;	/* Number of NSSA area */
    ushort ospf_set_summary_timer_reason;
    ulong  ospf_do_not_age_lsa_count;	/* Number of external LSA with 
					   DoNotAge set */
    ulong  ospf_dcbitless_lsa_count;	/* Number of external LSA with
					   DC bit clear */
    ulong  ospf_reference_bw;		/* The reference bw for cost
					   calculation, in term of Mbit/s */
    ushort ospf_spf_changes;		/* Summarized the SPF changes
					   information */
    queuetype ospf_partial_sum_spfQ;	/* Partial sum SPF request queue */
    queuetype ospf_partial_nssa_spfQ;	/* Partial nssa SPF request queue */
    queuetype ospf_partial_ex_spfQ;	/* Partial ex SPF request queue */
    areatype *ospf_dummy_area;		/* Dummy area used by type 5 LSA */
    uchar log_adj_changes:1;            /* Log adjacency changes */
};

/*
 * OSPF work description block
 */
struct ospf_worktype_ {
    struct ospf_worktype_ *next;
    areatype *ow_area;
    lsdbtype *ow_db;
    short ow_refcount;
};

/*
 * Flags for summary timer
 */
#define OSPF_REDIST_ROUTE	0x0001
#define OSPF_TRANSLATE_TYPE7	0x0002

/*
 * Convenient macro to check number of non-indication DC bitless LSA
 */
#define NON_INDICATION_LSA_COUNT(area) \
    ((area)->ar_dcbitless_lsa_count - (area)->ar_indication_lsa_count)

/*
 * Knob to generate and redraw indication LSA
 */
#define OSPF_ADV_INDICATION	1
#define OSPF_REMOVE_INDICATION	2

typedef struct listtype_ {
    struct listtype_ *list_next;
    paktype 	*list_pak_ptr;
    ushort	list_length;
    ulong	list_seq;
} listype;

typedef struct faddrtype_ {
    struct faddrtype_ *next;
    ipaddrtype address;
} faddrtype;

typedef struct ls_req_  {
    struct ls_req_ *req_next;
    struct ls_req_ *req_prev;
    ushort	req_age;
    uchar      	req_type;	/* type of lsa being requested */
    ulong	req_ls_id;	
    ulong	req_adv_rtr;	
    long	req_seq;
    ushort 	req_chksum;
    uchar	req_flag;
} ls_req_list;

#define	REQ_INQUEUE	0
#define REQ_SENT	1

struct nbrtype_ {
    nbrtype    *nbr_next;
    idbtype    *nbr_idb;
    ulong 	nbr_id;
    ipaddrtype 	nbr_ipaddr;
    uchar       nbr_i_ms;	 /* more and mast/slve bits */
    uchar	nbr_state;
    uchar       nbr_mode;	   	/* master or slave mode */
    uchar	nbr_pri;		/* 0 means not elig */
    ulong	nbr_dr;
    ulong 	nbr_bdr;
    ulong	nbr_poll_interval;
    ulong	nbr_def_poll_interval;
    mgd_timer	nbr_hello_master_timer;	/* Nbr master timer for hello
					   process */
    mgd_timer	nbr_dead_timer;   	/* Dead timer */
    mgd_timer	nbr_poll_timer;   	/* Poll timer */
    mgd_timer	nbr_dbd_retrs_timer;   	/* DBD retransmission timer */
    mgd_timer	nbr_dbd_hold_timer;   	/* DBD hold timer */
    mgd_timer	nbr_lsa_retrs_timer;   	/* LSA retransmission timer */
    ulong	nbr_seq;
    ulong	nbr_message_digest_seq;	/* seq num for message digest auth. */
    ls_req_list	*nbr_ls_req[8];		/* 0 and 6 is not used */ 
    listype	*nbr_dbd_list;
    list_queue  nbr_retrans_queue;	/* currently unacked ls advs */
    ushort      nbr_retrans_count;      /* Retransmission queue count */
    ushort      nbr_event_count;        /* Count of events on this neighbor */
    boolean	nbr_suppress_hello;	/* True if hello is suppressed */
    boolean	nbr_fast_retrans;	/* TRUE if doing fast retransmission */
    uchar	nbr_options;
    uchar	nbr_flags;
    uchar	nbr_message_digest_keyid;	/* The latest md key id  */
						/* the neighbor is using */
};

 /*  
 * neighbor states 
 */
#define	NBR_DOWN	0
#define	NBR_ATTEMPT	1
#define	NBR_INIT	2
#define	NBR_2WAY	3
#define	NBR_EXSTART	4
#define	NBR_EXCHANGE	5
#define	NBR_LOADING	6
#define	NBR_FULL	7
#define	NBR_SCVIRTUAL	8
#define NBR_STATES	NBR_SCVIRTUAL + 1

/*
 * Events causing neighbor state changes
 */
#define	NBR_EVENT_HELLO_RCV	0
#define	NBR_EVENT_START		1
#define	NBR_EVENT_2WAY		2
#define	NBR_EVENT_ADJ_OK	3
#define	NBR_EVENT_NEGO_DONE	4
#define	NBR_EVENT_EXCH_DONE	5
#define	NBR_EVENT_SEQ_MISMATCH	6
#define	NBR_EVENT_BAD_LS_REQ	7
#define	NBR_EVENT_LOAD_DONE	8
#define	NBR_EVENT_1WAY		9
#define	NBR_EVENT_RST_ADJ	10
#define	NBR_EVENT_KILL_NBR	11
#define	NBR_EVENT_INACT_TIMER	12
#define	NBR_EVENT_LLDOWN	13
#define NBR_EVENTS		NBR_EVENT_LLDOWN + 1

#define START_REDIST	1
/*
 * neighbor mode 
 */
#define	NBR_ADJ_SLAVE		1
#define	NBR_ADJ_MASTER		2
#define	NBR_ADJ_SLAVE_HOLD	4

static inline boolean ospf_no_req (nbrtype *nbr)
{
    return (((nbr->nbr_ls_req[LSA_TYPE_RTR] == NULL) &&
	     (nbr->nbr_ls_req[LSA_TYPE_NET] == NULL) &&
	     (nbr->nbr_ls_req[LSA_TYPE_SUM_NET] == NULL) &&
	     (nbr->nbr_ls_req[LSA_TYPE_SUM_ASB] == NULL) &&
	     (nbr->nbr_ls_req[LSA_TYPE_ASE] == NULL) &&
	     (nbr->nbr_ls_req[LSA_TYPE_7_ASE] == NULL)));

}
#define NO_REQ(N)	(ospf_no_req(N))

static inline boolean ospf_req_to_send (nbrtype *nbr)
{
    return (((nbr->nbr_ls_req[LSA_TYPE_RTR]) &&
	     (nbr->nbr_ls_req[LSA_TYPE_RTR]->req_flag == 0)) ||
	    ((nbr->nbr_ls_req[LSA_TYPE_NET]) &&
	     (nbr->nbr_ls_req[LSA_TYPE_NET]->req_flag == 0)) ||
	    ((nbr->nbr_ls_req[LSA_TYPE_SUM_NET]) &&
	     (nbr->nbr_ls_req[LSA_TYPE_SUM_NET]->req_flag == 0)) ||
	    ((nbr->nbr_ls_req[LSA_TYPE_SUM_ASB]) &&
	     (nbr->nbr_ls_req[LSA_TYPE_SUM_ASB]->req_flag == 0)) ||
	    ((nbr->nbr_ls_req[LSA_TYPE_ASE]) &&
	     (nbr->nbr_ls_req[LSA_TYPE_ASE]->req_flag == 0)) ||
	    ((nbr->nbr_ls_req[LSA_TYPE_7_ASE]) &&
	     (nbr->nbr_ls_req[LSA_TYPE_7_ASE]->req_flag == 0)));
}

#define REQ_TO_SEND(N)	(ospf_req_to_send(N))

/*
 * destination decoding codes
 */
#define ALL_UP_NBRS          1
#define ALL_ELIG_NBRS        2
#define ALL_EXCH_NBRS        3
#define ALL_VIRTUAL_NBRS     4
#define DR_BDR_NBRS          5

/*
 * nbr flags
 */
#define NBRF_CONFIGURED (1<<0)
#define NBRF_NEW	(1<<1)

/*
 * net associated with this area( address 
 * range associated ) 
 */
typedef struct addr_range_ {
    struct addr_range_ *net_next;
    ipaddrtype  net_address;
    ipaddrtype  net_mask;
    ulong	net_cost;
} addr_range;

	 
struct areatype_ {
    areatype   *ar_next;	/* pointer to next area */
    ulong	ar_id;		/* Area id ( 0 is reserved for backbone ) */
    char	ar_id_str[16];	/* Area id as ip or integer format */
    pdbtype     *ar_pdb_ptr;    /* Pointer to pdb this area is attached */
    boolean     ar_stub;	/* TRUE if this is stub area */
    boolean     ar_stub_nosum; 	/* TRUE if no summary for stub area */
    boolean	ar_nssa;	/* TRUE if this is nssa area */
    boolean	ar_nssa_no_redist; /* TRUE if not allow redistribution */
    boolean	ar_nssa_default;/* TRUE if originate type 7 default for ABR */
    boolean	ar_nssa_translate_type7; /* TRUE if this router should
					    translate type 7 LSA */
    list_queue  ar_nssa_flood_list; /* Flood list for NSSA */
    addr_range  *ar_range_list;  /* list of component networks */
    idbtype     *ar_if_list;	 /* list of interfaces attached to this area */
    boolean	ar_schd_spf;	 /* Flag to indicate spf is scheduled for
				    this area.  Has effect on intra-area
				    route calculation only */
    lsdbtype 	*ar_spf_ptr;	 /* spf tree; head is this rtr */
    rbTree	*ar_lsdbase[8];	 /* O is for stub nets 
				    6 is not used */
    lsdbtype	*ar_clist;  	 /* candidate list for spf procedure */
    mgd_timer	ar_hello_master_timer;	/* Area master timer for hello
					   process */
    mgd_timer	ar_master_timer;	/* Area master timer for router
					   process */
    mgd_timer	ar_rebuild_rtr_timer;	/* Rebuild rtr LSA timer */
    mgd_timer	ar_update_timer;	/* LSA update timer for this area */
    mgd_timer	ar_delay_timer;		/* LSA delay timer  */
    mgd_timer	ar_age_timer;		/* LSA aging timer */
    mgd_timer	ar_sumlock_timer;	/* Delay flood summary LSA timer */
    mgd_timer	ar_exlock_timer;	/* Delay flood type 7 LSA timer */
    lsdbtype	*ar_sum_delete_list;/* summary routes delete list */
    lsdbtype	*ar_sum_insert_list;/* summary routes insert list */
    lsdbtype	*ar_insert_list;    /* Intra routes insert list */
    lsdbtype	*ar_delete_list;    /* Intra routes delete list */
    lsdbtype	*ar_ex_insert_list; /* Type 7 external route insert list */
    lsdbtype	*ar_ex_delete_list; /* Type 7 external route delete list */
    struct vlink_ *ar_vlink;	    /* Virtual links in this area */
    ulong	ar_ospfSpfRuns;	/* no. of time that intra-area SPF has run */
    ulong	ar_message_digest_seq;	/* Sequence number use in */
    					/* message digest authentication */
    ulong	ar_dcbitless_lsa_count;	/* Number of LSA without DC bit 
					   set */
    ulong	ar_do_not_age_lsa_count;/* Number of DoNotAge LSA */
    ulong	ar_indication_lsa_count;/* Number of indication LSA */
    ushort	ar_def_stub_default_cost;/* cost for default route */
    ushort	ar_stub_default_cost;/* cost for default route */
    ushort	ar_netcnt;		/* no. of net ranges for this area */
    ushort	ar_ifcnt;   		/* no. of interface with this area */
    ushort	ar_spf_changes;		/* SPF events scheduled */
    ushort	ar_spf_instance;	/* SPF instance */
    ushort	ar_autype;		/* area authentication type */
    ushort	ar_delay_flag;		/* indicates which LSA is delayed */
    ushort	ar_sumdelay_flag;	/* summary LSA delay flag */
    ushort	ar_bdr_count;		/* No. of ABR in the area */
    ushort	ar_asr_count;		/* No. of ASBR in the area */
    ushort	ar_lsa_count;		/* No. of LSA in this area */
    ulong	ar_lsa_chksumsum;	/* LSA sum of checksum */
    uchar	ar_options;		/* TOS service and STUB area */
};

/* flags for delay field */

#define	DELAY_RTR_LSA	        (1<<0)
#define	DELAY_NET_LSA	        (1<<1)
#define	DELAY_RTR_LSA_UPDATE    (1<<2)
#define	DELAY_NET_LSA_UPDATE	(1<<3)

#define	DELAY_INTRA_NET		1
#define	DELAY_INTER_NET		2
#define	DELAY_INTRA_ASB         3
 
extern pdbtype *ospf_pdbs_ptr;

typedef struct vlink_ {
    struct vlink_ *vl_next;
    areatype *vl_backbone;
    idbtype *vl_idb;
    ulong vl_nbr_id;
    ulong vl_message_digest_seq;	/* seq num for message digest auth. */
} vlinktype;

typedef struct ospf_rtr_path_ {
    struct ospf_rtr_path_ *orp_next;
    areatype   *orp_area;
    idbtype    *orp_idb;
    lsdbtype   *orp_db;
    ipaddrtype	orp_forw_addr;
    ipaddrtype  orp_infosource;
    ulong	orp_cost;
    ushort	orp_spf_inst;
    ushort 	orp_pdb_spf_inst;
    uchar	orp_dest_type;
    uchar	orp_route_type;
    uchar	orp_flag;
} ospf_rtr_path;

typedef struct rtr_route_ {
    struct rtr_route_  *rt_next;
    ipaddrtype		rt_dest;
    ulong		rt_min_cost;
    ospf_rtr_path      *rt_path_list;
} rtr_route;

/*
 * structure to keep track of information
 * about routes that need to be added to the
 * routing table.
 */
struct ospfbackuptype_ {
    struct ospfbackuptype_ *next;  /* ptr to next backup info */
    ipaddrtype netnumber;      /* netnumber requested */
    ipaddrtype netmask;
    ulong context_info;	       /* possible area of the route */
    ushort route_type;         /* type of route */
};

#define	DEFAULT_MAP_METRIC		20
#define	DEFAULT_MAP_METRIC_CONNECTED	1

/* destnation types */
#define	DTYPE_NET	(1<<0)
#define	DTYPE_ABR	(1<<1)
#define	DTYPE_ASBR	(1<<2)

/* ospf_flag */
#define REDIST_LOST		0x0001
#define OSPF_SOFT_RESET		0x0002
#define OSPF_MAXAGEQ		0x0020
#define OSPF_ID_CHANGING	0x0040
#define OSPF_CLEANING_UP	0x0080
#define OSPF_IF_JUMPSTART	0x0100
#define OSPF_SCHD_SPF		0x0200
#define OSPF_IMMEDIATE_SPF	0x0400
#define OSPF_FLUSH_DO_NOT_AGE	0x0800
#define OSPF_CHECK_INDICATION	0x1000
#define OSPF_CHECK_ABR		0x2000

/* costants used */
#define MAXAGE		(60 * 60)		/* 1 hour  */
#define MAXDIFFAGE	(15 * 60)      		/* 15 minutes */
#define BACKBONE_AREA_ID	0
#define TOS0_IF_COST_ASYNC	10000
#define TOS0_IF_COST_X25	5208
#define TOS0_IF_COST_SERIAL	1562
#define TOS0_IF_COST_ETHER	10
#define TOS0_IF_COST_TR		25
#define TOS0_IF_COST_TR16	6
#define TOS0_IF_COST_FDDI	1
#define TOS0_IF_COST_ATM	1		/* equal to FDDI because it
						   cannot get any lower */
#define STUB_DEFAULT_COST	1
#define STUB_HOST_COST		0

#define HELLO_INTERVAL		10		/* 10 seconds */
#define HELLO_INTERVAL_NBMA	30		/* 30 seconds */
#define DEFAULT_PRIORITY	1

#define DEAD_INTERVAL		(4 * HELLO_INTERVAL)
#define DEAD_INTERVAL_NBMA	(4 * HELLO_INTERVAL_NBMA)
#define WAIT_INTERVAL		(4 * HELLO_INTERVAL)
#define POLL_INTERVAL		(2 * HELLO_INTERVAL_NBMA)
#define RTR_INTERVAL		5
#define TRANSDELAY		1
#define MINLSINTERVAL		5
#define FLUSH_NET_INTERVAL	(3 * ONEMIN)
#define REBUILD_RTR_INTERVAL	(FLUSH_NET_INTERVAL + 5 * ONESEC)
#define MAXAGEQ_SCAN_INTERVAL	10
#define LS_REFRESH_TIME		(30 * 60)	/* 30 minutes */
#define AGE_TIME 		(20 * 60)	/* 20 minutes */
#define ACK_PACKET_LIMIT_NUM	6		/* 6 ACK packets at a time */
#define DELAYED_ACK_INTERVAL	200		/* 200ms */
#define SPF_HOLD_TIME		(8 * 1000)	/* 8 seconds */
#define MAX_EX_LSA_PAK		40		/* 40 LSA/pkt */
#define MAX_EX_PAK_RATE		6	 	/* 6 pkts at a time */
#define MIN_ABR_AREA_COUNT      2               /* Min area count for abr */
#define SPF_SCHD_DELAY          5               /* SPF delay after schd */
#define SPF_HOLD_INTERVAL       10              /* Hold time between two
						   SPF */
#define FADDR_INTERVAL          30              /* forward address lookup */
						/* timer interval */
#define SCANNING_HOLD_TIME	10		/* Redistribution summary
						   scanning hold time */
#define OSPF_MAX_QDEPTH		200             /* Maximum number of
						   input protocol packet */

/*
 * OSPF chunk size
 */
#define OSPF_LSDB_LIST_CHUNKSIZE	50
#define OSPF_NBR_LIST_CHUNKSIZE		50
#define OSPF_WORK_CHUNKSIZE		50

/*
 * returns FALSE if the timer expired,
 * returns TRUE if the timer running.
 */
#define HOLD_TIMER_CHECK(time, interval)      (CLOCK_IN_INTERVAL(time, \
 							((interval) * ONESEC)))
/*
 * returns TRUE, if the timer expired,
 * returns FALSE if the timer running.
*/
 
#define SPF_HOLD_EXPIRED(pdb)           (CLOCK_OUTSIDE_INTERVAL(pdb->spf_time,\
 					   pdb->spf_wait_interval * ONESEC))

#define OSPF_DATA_PTR(pak_ptr) (ipheadstart(pak_ptr) +    \
				IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE);

#define VALID_IPADDRESS(address) (get_majornet(address))

#define ospf_buginf		if (ospf_debug_events) buginf
#define ospf_adj_buginf		if (ospf_debug_events || ospf_debug_adj) buginf
#define ospf_flood_buginf	if (ospf_debug_flood) buginf
#define ospf_flood_acl_buginf	if (ospf_flood_acl_pass) buginf
#define ospf_flood_adj_buginf	if (ospf_debug_flood || ospf_debug_adj) buginf
#define ospf_spec_buginf	if (ospf_debug_spf_monitor) buginf
#define ospf_adj_spec_buginf	if (ospf_debug_spf_monitor || \
				    ospf_debug_adj) buginf
#define ospf_lsa_gen_buginf	if (ospf_debug_lsa_gen) buginf
#define ospf_packet_buginf	if (ospf_debug_packet) buginf
#define ospf_retrans_buginf	if (ospf_debug_retrans) buginf
#define ospf_spf_intra_buginf	if (ospf_debug_spf_intra) buginf
#define ospf_spf_inter_buginf	if (ospf_debug_spf_inter) buginf
#define ospf_spf_ext_buginf	if (ospf_debug_spf_ext) buginf
#define ospf_spf_buginf		if (ospf_debug_spf_intra || \
				    ospf_debug_spf_inter || \
				    ospf_debug_spf_ext) buginf
#define ospf_spf_intra_acl_buginf	if (ospf_spf_intra_acl_pass) buginf
#define ospf_spf_inter_acl_buginf	if (ospf_spf_inter_acl_pass) buginf
#define ospf_spf_ext_acl_buginf		if (ospf_spf_ext_acl_pass) buginf
#define ospf_spf_acl_buginf	if (ospf_spf_intra_acl_pass || \
				    ospf_spf_inter_acl_pass || \
				    ospf_spf_ext_acl_pass) buginf

extern boolean ospf_spf_intra_acl_pass;
extern boolean ospf_spf_inter_acl_pass;
extern boolean ospf_spf_ext_acl_pass;
extern boolean ospf_flood_acl_pass;

extern acl_ospf_spf_intra_debug (ulong);
extern acl_ospf_spf_inter_debug (ulong);
extern acl_ospf_spf_ext_debug (ulong);

/*
 * Global OSPF Mib definitions
 */

#define EXLSA_COUNT      0
#define EXLSA_CHKSUMSUM  1
#define ORNLSA_COUNT     2
#define RCNLSA_COUNT     3

#define OSPF_PDB_MIB_COUNT 4

#define MIB_TABLE(pdb) (pdb->mib_table)

#define PDB_INC_MIB_TABLE(pdb, type)     if (MIB_TABLE(pdb)) \
                                               (MIB_TABLE(pdb))[type]++
#define PDB_DEC_MIB_TABLE(pdb, type)     if (MIB_TABLE(pdb)) \
                                               (MIB_TABLE(pdb))[type]--
#define PDB_ADD_MIB_TABLE(pdb, type, value)    if (MIB_TABLE(pdb)) \
                                               (MIB_TABLE(pdb))[type] += value
#define PDB_SUB_MIB_TABLE(pdb, type, value)    if (MIB_TABLE(pdb)) \
                                               (MIB_TABLE(pdb))[type] -= value

/* OSPF SPF statistic information */

#define SPF_TIME_MAX 10
#define SPF_PROC_TIME_MAX 250
#define ADD_EX_TIME		1
#define PROCESS_ASE_TIME 	2
#define PROCESS_SUM_TIME	3
 
typedef struct _spf_run_time {
     ulong mtime[7];  /* seven steps in run_spf() */
     ushort change;
     sys_timestamp timestamp; 
} spfruntimetype;

typedef struct _process_time {
    ulong ptime[5];
} processtimetype;

typedef struct _write_time {
    processtimetype proc_time[SPF_PROC_TIME_MAX];
    ushort first;
    ushort last;
} spfstattimetype;


/*
 * OSPF Global Variables
 */
extern boolean ospf_name_lookup;

/*
 * OSPF event handling
 */

typedef enum {
    OC_GENERIC = 1,
    OC_TIMER_EXPIRED,
    OC_SUSPEND,
    OC_UNSUSPEND,
    OC_BAD_PKT_RCVD,
    OC_CLEAN_AREA,
    OC_CLEANUP,
    OC_DB_ADD,
    OC_DB_FREE,
    OC_ELECT_BDR,
    OC_ELECT_DR,
    OC_CLEAN_IF,
    OC_ROUTE_ADJUST,
    OC_SPF,
    OC_I_ERROR,
    OC_I_UP,
    OC_I_DOWN,
    OC_I_LOOP,
    OC_I_UNLOOP,
    OC_I_WAITTM,
    OC_I_BACKUP,
    OC_I_NBRCH,
    OC_N_ERROR,
    OC_N_START,
    OC_N_2WAY,
    OC_N_ADJOK,
    OC_N_NEGDONE,
    OC_N_EXDONE,
    OC_N_BADREQ,
    OC_N_BADSEQ,
    OC_N_LOADDONE,
    OC_N_1WAY,
    OC_N_RSTAD,
    OC_N_DOWN,
    OC_SERVICE_MAXAGE,
    OC_INSERT_MAXAGE,
    OC_DEL_1_RT_LST,
    OC_DEL_RT_LST,
    OC_DEL_RT_LST_INFO,
    OC_INS_RT_LST,
    OC_INS_RT_LST_INFO,
    OC_DEL_RT_LST_2,
    OC_MASK_CHANGED,
    OC_REDIST_SUM_CHECK,
    OC_SERVICE_REDIST_SUM
} ospf_event_code;

typedef struct ospf_event_type_ {
    ospf_event_code code;		/* Event code */
    ulong data1;			/* Interesting data bit 1 */
    ulong data2;			/* Interesting data bit 2 */
    sys_timestamp timestamp;		/* Current timestamp */
} ospf_event_type;

/* 
 * for ospf_route_adjust(), idb changes state
 */
#define OSPF_IDB_UP 1
#define OSPF_IDB_DOWN 2

/* 
 * Type of attached network 
 */
#define	IF_BROADCAST		1
#define	IF_NONBROADCAST		2
#define	IF_POINT_TO_POINT  	3
#define	IF_POINT_TO_M_POINT 	4
#define	IF_VIRTUAL_LINK    	5
#define	IF_LOOPBACK        	6

/*
 * State of interface as seen by OSPF
 */
#define	IFS_DOWN		0
#define	IFS_LOOPBACK		1
#define	IFS_WAITING        	2
#define	IFS_POINT_TO_M_POINT 	3
#define	IFS_POINT_TO_POINT 	4
#define	IFS_DR             	5
#define	IFS_BACKUP         	6
#define	IFS_OTHER          	7
#define IF_STATES          	8

/* 
 * events for interface state
 * machine.
 */
#define	IFE_UP             0
#define	IFE_WAIT_TIMER     1
#define	IFE_BACKUP_SEEN    2
#define	IFE_NBRCHANGE      3
#define	IFE_LOOP_IND       4
#define	IFE_UNLOOP_IND     5
#define	IFE_DOWN           6
#define IF_EVENTS          7

extern void (*const nbr_state_machine[NBR_EVENTS][NBR_STATES])(idbtype *, nbrtype *);
extern void (*const if_state_machine[IF_EVENTS][IF_STATES])(idbtype *, nbrtype *);

/*
 * default reference bandwidth
 */
#define OSPF_DEF_REFERENCE_BW	100		/* Default reference 100M */
#define OSPF_MAX_REFERENCE_BW	4294967		/* Maximum reference 4000G */
#define OSPF_METRIC_SCALE	1000L		/* Mbit/s to kbit/s scale */
/*
 * OSPF traffic statistics
 */
typedef struct ospf_traffic_t_ {
    ulong inputs;
    ulong input_hello;
    ulong input_dbdes;
    ulong input_req;
    ulong input_lsupd;
    ulong input_ack;
    ulong outputs;
    ulong checksumerr;
} ospf_traffic_t;
extern ospf_traffic_t ospf_traffic;

typedef boolean (*ospf_flush_function)(pdbtype *, areatype *, lsdbtype *);

/*
 * OSPF Function Declarations
 */
/* ospf.c exported functions */
extern void ospf_clean_area(areatype *);
extern boolean ospf_soft_reset(pdbtype *pdb);
extern void ospf_enqueue(paktype *);
extern forktype ospf_scanner(pdbtype *pdb);
extern void check_ospf_database(pdbtype *pdb, ndbtype *ndb);
extern pdbtype *ospf_pdb_init(ushort);
extern boolean ospf_backbone_active(areatype *area, boolean check_nbr);

/* ospf_nbr.c exported functions */
extern nbrtype *ospf_find_nbr(idbtype *, ulong, ipaddrtype, boolean);
extern void free_dbd_sum(nbrtype *);
extern void free_ls_req_list(idbtype *, nbrtype *, ls_req_list *, uchar);
extern boolean add_db_to_nbr_retrans(nbrtype *, lsdbtype *, idbtype *);
extern void ospf_nbr_delete(idbtype *, nbrtype *, boolean);
extern nbrtype *ospf_create_one_nbr(idbtype *, ulong, ipaddrtype);
extern void ospf_delete_one_nbr(idbtype *, nbrtype *);
extern void rem_nbr_retrans_list(nbrtype *, lsdbtype *);
extern boolean lsa_nbr_retrans(nbrtype *, lsdbtype *);
extern boolean is_db_in_nbr_retrans(nbrtype *, lsdbtype *);
extern void ospf_nbr_dead(nbrtype *, boolean);
extern void ospf_nbr_retrans_dbd(nbrtype *);
extern void ospf_nbr_hold_dbd(nbrtype *);
extern void ospf_nbr_poll(nbrtype *);
extern void ospf_nbr_retrans_lsa(nbrtype *);

/* ospf_if.c exported functions */
extern void ospf_clean_if(idbtype *);
extern void ospf_if_change_def_hello(ospf_idb_info *, ulong hello);
extern void ospf_if_change_def_dead(ospf_idb_info *, ulong dead);
extern void ospf_if_set_default(pdbtype *, idbtype *, ospf_idb_info *);
extern boolean ospf_if_init(pdbtype *, areatype *, idbtype *);
extern void ospf_start_intf(idbtype *);
extern boolean ospf_interface_up(idbtype *);
extern void ospf_encap_fixup(idbtype *, int);
extern void ospf_idb_address_change(idbtype *,ipaddrtype, ipaddrtype,
				    boolean, boolean);
extern void ospf_idb_change(idbtype *, boolean);
extern void ospf_scan_interface (pdbtype *);
extern void ospf_bandwidth_changed(idbtype *);
extern void ospf_if_recalculate_cost(pdbtype *, idbtype *);
extern void ospf_dc_down_check(idbtype *);
extern void ospf_configure_for_broadcast(idbtype *, boolean);
extern void if_ack_delayed(idbtype *);
extern void if_wait_tm(idbtype *);

/* ospf_lsa.c exported functions */
extern void ar_delayed_rtr_lsa(areatype *);
extern void if_delayed_net_lsa(idbtype *);
extern void ar_delayed_sum_lsa(areatype *);
extern void ar_delayed_ex_lsa(areatype *);
extern void pdb_delayed_ex_lsa(pdbtype *);
extern void ar_update_lsa(areatype *);
extern void pdb_sumupdate(pdbtype *);
extern void pdb_exupdate(pdbtype *);
extern boolean build_rtr_lsa(areatype *, list_queue *, boolean);
extern boolean build_net_lsa(idbtype *, list_queue *, boolean);
extern void build_ex_lsa(lsdbtype **, pdbtype *, areatype *,
			 ipaddrtype, ipaddrtype,
			 ulong, ipaddrtype, ulong, list_queue *, ushort, 
			 boolean, boolean, uchar);
extern void ospf_translate_one_type7 (pdbtype *, ndbtype *, ulong, ulong, 
				      list_queue *, boolean);
extern void add_to_flood_list(lsdbtype *, list_queue *);
extern void add_to_update_list(lsdbtype *, list_queue *, boolean);
extern void rem_db_retrans_list(lsdbtype *, nbrtype *);
extern void add_ack_list(list_queue *, lsdbtype *, lsatype *);
extern void ospf_free_ack_list(list_queue *);
extern void lsdb_enqueue(list_queue *, lsdb_list *);
extern void lsdb_dequeue(list_queue *, lsdb_list *);
extern void ospf_flush_type5(pdbtype *);
extern void ospf_flush_all_type5_from_stub(areatype *);
extern void ospf_flush_nssa_type4_from_other(areatype *);
extern void ospf_flush_area(areatype *);
extern void ospf_flush_all_do_not_age_lsa(pdbtype *);
extern void ospf_flush_sum_from_area(areatype *, list_queue *, boolean);
extern void ospf_flush_all_type7_from_area(areatype *);
extern void ospf_flush_all_type4_from_stub(areatype *);
extern void ospf_check_indication_lsa(pdbtype *);
extern void age_net_lsa_flood(idbtype *);
extern void stub_default(areatype *, ulong);
extern void nssa_default(areatype *, ulong);
extern void start_sum_brtr(pdbtype *);
extern void stop_sum_brtr(pdbtype *);
extern void build_sum_lsa(lsdbtype **,areatype *, ipaddrtype,ipaddrtype,ulong,\
			  uchar, ushort, list_queue *);
extern void  ospf_spf_check_ex_origin(areatype *area, lsdbtype *db); 
extern boolean ospf_spf_check_default(lsdbtype *,areatype *);
extern boolean ospf_vidb_up(idbtype *, boolean *);
extern void flush_area_ranges(pdbtype *, areatype *);
extern void ospf_redistribute(pdbtype *, pdbtype *, boolean);
extern void ospf_network_command(pdbtype *, ipaddrtype, ipaddrtype, uchar,
				 ulong, int, boolean);
extern void ospf_send_external_lsa_sub(lsdbtype **, pdbtype *, areatype *,
				       ipaddrtype, ipaddrtype, ulong, 
				       ipaddrtype, ulong, list_queue *, 
				       ushort, boolean, uchar);
extern void ospf_send_summary_lsa(lsdbtype **, areatype *, ipaddrtype,
				  ipaddrtype, ulong, uchar, ushort, 
				  list_queue *);

extern void ospf_service_redist_summary (pdbtype *pdb);
extern ulong ospf_get_external_metric (pdbtype *pdb, pdbtype *s_pdb, 
				       ndbtype *ndb, ushort event);
extern void ospf_get_route_map_set_value (route_maptype *map,
					  ulong   *metric,
					  ushort  *metric_type,
					  ulong   *tag);
extern boolean redistribute_check (pdbtype *pdb, ndbtype *sdb,
				   pdbtype *dist, int *event);
extern boolean ospf_redist_summary_check (pdbtype *, ndbtype *, pdbtype *, 
					  ushort, boolean);
extern void ospf_perform_asbr_duty(pdbtype *pdb);
extern void ospf_summary_address (pdbtype *pdb, boolean sense);
extern void ospf_set_summary_timer (pdbtype *pdb, ushort);

/* ospf_flood.c exported functions */
extern void send_ack_list(list_queue *, idbtype *, nbrtype *);
extern inline boolean flood_area(areatype *, list_queue *, idbtype *, 
				 nbrtype *);
extern boolean ex_flood_area(pdbtype *, list_queue *, idbtype *, nbrtype *);
extern void ospf_rcv_update(paktype *, ospftype *, areatype *);
extern void ospf_rcv_ack(paktype *, ospftype *, areatype *);
extern void send_ls_update(lsdb_list *, idbtype *, nbrtype *, boolean);
extern void free_db_list(lsdb_list *);

/* ospf_hello.c exported functions */
extern void ospf_send_hello(areatype *, idbtype *, nbrtype *);
extern void ospf_rcv_hello(paktype *, ospftype *, areatype *);
extern void ospf_send_packet(paktype *,idbtype *, ushort, ushort,\
			     ipaddrtype, boolean);
extern void build_inter_route_all(lsdbtype *, areatype *);

/* ospf_support.c exported functions */
extern void lsa_checksum(lsdbtype *);
extern addr_range *find_area_range(areatype *, ipaddrtype);
extern areatype * get_area_ptr(pdbtype *, ulong);
extern void add_to_array(spfruntimetype);
extern void write_circular(ushort , ulong *, ushort);
extern void ospf_show_traffic(void);
extern void ospf_event(ospf_event_code, ulong, ulong);
extern void ospf_stub_and_asbr_check(pdbtype *);
extern void ospf_init_events(void);
extern void change2lsatype(ushort change, char *type);
extern void ospf_add_bad_checksum_queue(paktype *pak);
extern char* ospf_nbr_state_string(int state);

/* ospf_dbase.c exported functions */
extern boolean db_add_lsa(lsdbtype **, areatype *, ulong, ulong, lsatype *,
		                 uchar);
extern lsdbtype *db_find_lsa(areatype *, ulong, ulong, uchar);
extern lsdbtype *db_get_first_db(rbTree *);
extern lsdbtype *db_get_next_db(lsdbtype *db);
extern inline boolean db_valid_db(lsdbtype *db);
extern boolean db_free(pdbtype *, lsdbtype *);
extern void db_install(lsdbtype **, lsatype *, boolean, areatype *, idbtype *,
   nbrtype *, list_queue *, list_queue *, list_queue *, list_queue *);
extern void ar_age_dbase(areatype *);
extern void area_service(pdbtype *);
extern boolean db_lookup_lsa(areatype *, lsatype *);
extern lsdbtype *find_ex_db(areatype *,ulong, ulong, uchar);
extern lsdbtype *find_other_ex_db(areatype *, ulong, ulong, ulong *, int,
				  uchar);
extern lsdbtype *find_other_sum_db(areatype *, ulong, ulong, ulong *, int, 
				   uchar);
extern lsdbtype *ospf_db_find_lsa(areatype *, ulong, ulong, ulong, uchar);

/* ospf_spf.c exported functions */
extern void generate_sum(areatype *, lsdbtype *, ulong);
extern void process_sum(lsdbtype *, areatype *, boolean);
extern void process_ase(lsdbtype *, pdbtype *, areatype *, boolean);
extern void ospf_clean_partial_spfQ (areatype *area, queuetype *partial_spfQ);
extern void ospf_schedule_partial_spf(areatype *, lsdbtype *);
extern void ospf_service_partial_spf(pdbtype *);
extern rtr_route *lookup_rtr_route (pdbtype *, areatype *, 
				    ipaddrtype, lsdbtype *, ushort, 
				    ushort, ospf_rtr_path **);
extern void run_spf(pdbtype *);
extern void spf(areatype *, ushort, ulong);
extern void ospf_force_spf(pdbtype *, idbtype *);
extern void free_plist(lsdbtype *);
extern inline void check_ospf_suspend(pdbtype *, int);
extern inline void check_ospf_suspend_limit(pdbtype *, int, int *);
extern void ospf_insert_exdelayq(pdbtype *, lsdbtype * );
extern void ospf_service_exdelayq(pdbtype *);
extern void ospf_destroy_exdelayq(pdbtype *);
extern void ospf_rcv_add_req (pdbtype *, ipaddrtype, ipaddrtype, backuptype *);
extern void ospf_destroy_maxage(pdbtype *);
extern void ospf_service_maxage(pdbtype *);
extern void ospf_insert_maxage(pdbtype *, lsdbtype *);
extern void ospf_delete_maxage(pdbtype *, lsdbtype *);
extern ndbtype *ospf_lookup_net_route(areatype *, ipaddrtype, ipaddrtype, 
				      ushort, boolean);
extern void ospf_destroy_faddrq(pdbtype *);
extern void process_add_req(pdbtype *);
extern void ospf_ex_faddr(pdbtype *);
extern void delete_route_old_mask(pdbtype *, areatype *, lsdbtype *);
extern void ospf_set_all_for_immediate_spf(pdbtype *);
extern void ospf_set_spf_timer(pdbtype *);
extern boolean net_backlink(ulong, lsdbtype *);

/* ospf_parse.c exported functions */
extern void ospf_pdb_check(pdbtype *);
extern boolean ospf_attach_interface(pdbtype *, areatype *, 
				     idbtype *, boolean);
extern void ospf_parse_range(pdbtype *, ipaddrtype, ipaddrtype, ulong, int,
			     boolean);
extern void ospf_area (pdbtype *pdb, parseinfo *csb, boolean sense);
extern void ospf_neighbor(pdbtype *, ipaddrtype, parseinfo *, boolean);
extern void clean_router_route(pdbtype *); 
extern ospf_md_key *ospf_get_md_key(ospf_idb_info *, uchar key_id);
extern ospf_md_key *ospf_get_youngest_md_key(ospf_idb_info *);

/* ospf_adj.c exported functions */
extern void ospf_rcv_dbd(paktype *, ospftype *, areatype *);
extern void ospf_rcv_req(paktype *, ospftype *, areatype *);
extern void ospf_send_req(idbtype *, nbrtype *);
extern void ospf_send_dbd(idbtype *, nbrtype *);
extern void build_dbd_sum(idbtype *, nbrtype *);

/* ospf_dr.c exported functions */
extern void ospf_reset_adj(areatype *);
extern boolean ospf_choose_dr(idbtype *, list_queue *, boolean);

/* ospf_chain.c exported functions */
extern void ospf_parser_init(void);
