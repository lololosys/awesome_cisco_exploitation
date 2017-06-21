/* $Id: x25.h,v 3.4.10.5 1996/08/07 09:04:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25.h,v $
 *------------------------------------------------------------------
 * x25.h -- X25 Level 3
 *
 * January 1987, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25.h,v $
 * Revision 3.4.10.5  1996/08/07  09:04:46  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.10.4  1996/07/12  23:02:31  swenson
 * CSCdi61933:  pvc-setup can be delayed indefinitely
 * save remaining time on pvc connect timer if interrupted
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/06/27  09:47:59  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.4.10.2  1996/04/29  10:03:23  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/03/18  22:50:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:57:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/13  02:13:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  21:55:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/23  23:21:32  carvalho
 * CSCdi41580:  Calling Address Ext. facility modified while X25 switching
 * Save digit count/authority field for called and calling address
 * extension facilities for re-use when call is forwarded.
 *
 * Revision 3.3  1995/11/17  18:07:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:52:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:53:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/02  22:01:51  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.2  1995/07/20  07:38:15  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.1  1995/06/07 23:21:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __X25_H__
#define __X25_H__

#include "address.h"

/*
 * X25 Return values
 */

#define	X25_ERROR (-1)

#define X25_FORWARD_OK			0
#define X25_FORWARD_ALT			1
#define X25_FORWARD_CMNS_IN_PROGRESS	2
#define X25_FORWARD_ERR			3

/*
 * Reference structures
 */

#define	X25_MAXNVC		8

#define	X25_DCE_RESTART_TRANSMIT	2
#define	X25_CLEAR_TRANSMIT		2
#define	X25_RESET_TRANSMIT		2

#define	X25_MOD8	8
#define	X25_MOD128	128

#define	X25_MODULO_8	1
#define	X25_MODULO_128	2

#define X25_MAX_LCI		0xfff

/*
 * X.25 configuration defaults
 */
#define	X25_DEFAULT_WIN		2
#define	X25_DEFAULT_WOUT	2
#define	X25_DEFAULT_IPS		7
#define	X25_DEFAULT_OPS		7
#define	X25_DEFAULT_MODULO	X25_MOD8
#define	X25_DEFAULT_DTE_T0	180
#define	X25_DEFAULT_DTE_T1	200
#define	X25_DEFAULT_DTE_T2	180
#define	X25_DEFAULT_DTE_T3	180
#define	X25_DEFAULT_DCE_T0	60
#define	X25_DEFAULT_DCE_T1	180
#define	X25_DEFAULT_DCE_T2	60
#define	X25_DEFAULT_DCE_T3	60
#define	X25_DEFAULT_LIC		0
#define	X25_DEFAULT_HIC		0
#define	X25_DEFAULT_LTC		1
#define	X25_DEFAULT_HTC		1024
#define	X25_DEFAULT_LOC		0
#define	X25_DEFAULT_HOC		0
#define	X25_DEFAULT_TH		0
#define	X25_DEFAULT_IDLE	0
#define	X25_DEFAULT_NVC		1
#define	X25_HOLDQ		10

/*
 * CMNS-specific configuration defaults
 */
#define	CMNS_DEFAULT_LIC	0
#define	CMNS_DEFAULT_HIC	0
#define	CMNS_DEFAULT_LTC	1
#define	CMNS_DEFAULT_HTC	X25_MAX_LCI
#define	CMNS_DEFAULT_LOC	0
#define	CMNS_DEFAULT_HOC	0

/*
 * Timers for CMNS (seconds)
 */
#define CMNS_DEFAULT_DXE_T0	60
#define CMNS_DEFAULT_DXE_T1	180
#define CMNS_DEFAULT_DXE_T2	60
#define CMNS_DEFAULT_DXE_T3	60


/*
 * Flag bits for X.25 encapsulation in interface
 */
#define	X25_SUPPR_SRC_ADDR	0x0100	/* suppress calling address */
#define	X25_SUPPR_DST_ADDR	0x0200	/* suppress called address */
#define	X25_SUBST_SOURCE	0x0400	/* use IF source x.121 for calls */
#define	X25_ACCEPTREVERSE	0x0800	/* accept incoming reverse calls */
#define	X25_LINKRESTART		0x1000	/* don't send restart on link reset */
#define	X25_IPPREC		0x2000	/* support IP precedence VCs */
#define	X25_PADMAP		0x4000	/* only accept PAD calls if in map */
#define	X25_COMPRESS		0x8000	/* compress by default */

/*
 *  CUD encapsulation methods
 *
 *  These values define, for a VC, the protocol identification method used
 *  or, for a map, the method to propose on an outgoing Call.
 */
#define X25_CUD_PID_NONE	0	/* VC: doesn't encapsulate datagrams */
#define X25_CUD_PID_DEFAULT	1	/* map: uses interface default method */
#define X25_CUD_PID_CISCO	2	/* Cisco proprietary encapsulation */
#define X25_CUD_PID_IETF	3	/* standard RFC-1356 encapsulation */
#define X25_CUD_PID_SNAP	4	/* RFC-1356 SNAP encapsulation */
#define X25_CUD_PID_MULTI	5	/* RFC-1356 multiprotocol encaps. */

/*
 *  Data encapsulation methods
 *
 *  These values define the protocol identification method used on a VC.
 */
#define X25_DATA_PID_NONE       0       /* no data encapsulation */
#define X25_DATA_PID_MULTI      1       /* RFC-1356 standard encapsulation */
#define X25_DATA_PID_SNAP       2       /* forced RFC-1356 SNAP encapsulation */


#define	LCG(lci) 	((lci >> 8) & 0xf)
#define	LCN(lci) 	(lci & 0xff)
#define	LCI(p)		(((p)->x25_lcg << 8) | (p)->x25_lcn)

#define	X25_FLOW_MASK	0x1f		/* RR, RNR, REJ PTI mask bits	*/
#define	X25_DATA_MASK	1		/* DATA Packet Type Ident. mask	*/
#define X25_MBIT_MASK	0x10		/* extract M bit from x25_pit   */

#define X25_DATA_Q_RX	0x01		/* Q bit set in last data pkt Rx */
#define X25_DATA_M_RX	0x02		/* M bit set in last data pkt Rx */

#define	X25_ADDRESS_LENGTH	20

/*
 * Linked list of those idbs running X.25
 */

struct x25idbtype_ {
    x25idbtype *next;
    hwidbtype *hwidb;
};

/*
 * Blacker Front End context reference definition
 */
STRUCTURE_REF(bfetype);

struct rpoatype_ {
    rpoatype *next;			/* next pointer */
    char *name;				/* name of RPOA */
    uchar nrpoa;			/* number of RPOA entries */
    ushort rpoa[0];			/* entries themselves */
};

STRUCTURE_REF(packettype);
struct packettype_ {
    uchar x25_q:1;			/* GFI Q bit */
    uchar x25_d:1;			/* GFI D bit */
    uchar x25_sn:2;			/* GFI Sequence number */
    uchar x25_lcg:4;			/* logical channel group */
    uchar x25_lcn:8;			/* logical channel number */
    uchar x25_pti:8;			/* packet type identifier */
};

STRUCTURE_REF(x25paktype);
struct x25paktype_ {
    uchar x25GfiLcg;			/* q, d, sn & lcg fields  */
    uchar x25Lcn;			/* lcn field              */
    uchar x25Pti;			/* pti field              */
};

STRUCTURE_REF(x25clearpaktype);		/* Clear or Reset Indicate */
struct x25clearpaktype_ {
    uchar x25_gfi_lcg;			/* q, d, sn & lcg fields  */
    uchar x25_lcn;			/* lcn field              */
    uchar x25_pti;			/* pti field              */
    uchar x25_cause;			/* Clear/Reset cause	  */
    uchar x25_diag;			/* Clear/Reset diagnostic */
};

struct considbtype_ {
    int   cmns_dce;                     /* interface type DTE/DXE */
    boolean x25_restart;		/* cmns restart */
    boolean x25_upstate;		/* cmns status */
    ulong x25_restartssent;		/* total restarts sent */
    ulong x25_restartsrcvd;		/* total restarts received */
    uchar x25_cause;			/* for restart CMNS */
    uchar x25_diag;			/* also for restart CMNS  */
};

#include "../x25/compress.h"

struct lcitype_ {
    lcitype *lci_next;			/* timer queue pointer */
    queuetype lci_holdQ;		/* output pending queue */
    queuetype lci_reassmQ;		/* reassembly queue */
    queuetype lci_resetQ;		/* RESET proc.; see x25_process_reset */

    uchar lci_state;			/* VC state */
    char lci_ps;			/* VC p(s)--next P(S) to send */
    char lci_pr;			/* VC p(r)--next P(S) expected */
    char lci_ack;			/* VC ack't--last P(R) sent */
    char lci_rpr;			/* VC remote p(r)--last P(R) rec'd */
    uchar lci_busy;			/* VC RNR received */
    uchar lci_allow_int;		/* allow incoming Interrupt? */
    uchar lci_allow_int_cnf;		/* allow incoming Interrupt confirm? */
    uchar lci_tcp_reset_sent;		/* RESET issued on TCP, awaiting CONF */
    uchar lci_confirm_tcp_reset;	/* is TCP RESET handled end-to-end? */
    uchar lci_pvc_con_d2_d3;		/* PVC connected while in D2/D3? */
    uchar lci_pvc_status;		/* status of a PVC */
    ulong lci_pvc_remainder;		/* time remaining for a connect timer */
    tcp_encaps *lci_pvc_swdata;		/* temporary PVC connection handle */
    uchar lci_cud_pid_method;		/* protocol ID method used in Call */
    uchar lci_data_pid_method;		/* ID method for encaps. datagrams Tx */
    char lci_retransmit;		/* control packet retransmit count */
    sys_timestamp lci_timer;		/* VC timer */
    ushort lci_lci;			/* VC LCI */
    ushort lci_data_flags;		/* data packet Q and M bits rec'd */
    ushort lci_tx_first_frag;		/* Tx first datagram fragment flag */
    ushort lci_tx_frag_cnt;		/* Tx datagram fragment count */
    short lci_reassmbytes;		/* bytes in input reassmbly */
    ushort lci_linktype;		/* kind of traffic carried */
    ulong lci_idle;			/* VC timer */
    sys_timestamp lci_starttime;	/* time VC established */
    sys_timestamp lci_lastinput;	/* last time we received input */
    sys_timestamp lci_lastoutput;	/* last time we sent output */

    addrtype lci_src_addr;		/* source (calling) address */
    addrtype lci_dst_addr;		/* destination (called) address */
    addrtype lci_orig_dst_addr;		/* original destination address */

    boolean (*x25_dooutput)(paktype *);	/* output llc2 or lapb */

    /*
     * Facility information
     */
    uchar lci_dbit;			/* D-bit procedure allowed */
    uchar lci_wout;			/* VC outgoing window */
    uchar lci_win;			/* VC incoming window */
    uchar lci_pout;			/* VC outgoing packet size */
    uchar lci_pin;			/* VC incoming packet size */
    uchar lci_force_winsize;		/* force window indication on CALL C */
    uchar lci_force_pksize;		/* force packet size ind. on CALL C */
    uchar lci_prec;			/* VC DDN precedence */
    uchar lci_prec_connected;		/* VC DDN CONNECTED precedence */
    uchar lci_fast;			/* VC fast select */
    uchar lci_confirm_addr;		/* send addresses on CALL C */
    uchar lci_reverse;			/* VC reverse charge requested */
    uchar lci_cug;			/* VC CUG */
    uchar lci_lam;			/* VC Line Address Modification */
    uchar lci_tput_out;			/* VC outgoing throughput */
    uchar lci_tput_in;			/* VC incoming throughput */
    ushort lci_tdelay;			/* VC transit Delay */
    uchar lci_dst_addr_fac_reason;	/* dest. addr. modification reason */
    uchar lci_nuidlen;			/* VC network user id length */
    uchar *lci_nuid;			/* VC network user id */

    /*
     * VC statistics
     */
    ulong lci_bytessent;		/* total bytes sent */
    ulong lci_bytesrcvd;		/* total bytes received */
    ulong lci_packetssent;		/* total packets sent */
    ulong lci_packetsrcvd;		/* total packets received */
    ulong lci_resetssent;		/* total resets sent */
    ulong lci_resetsrcvd;		/* total resets received */
    ulong lci_rnrrcvd;			/* total RNRs received */
    ulong lci_intsrcvd;			/* total INTs received */
    ulong lci_intssent;			/* total INTs sent */
    ulong lci_rejrcvd;			/* total REJs received */

    x25map *lci_map;			/* pointer to map structure (if any) */
    idbtype *lci_idb;			/* ptr to interface data  */
    lcitype *lci_hash;			/* LCI hash list pointer */
    pcbtype *lci_pcb;			/* ptr to PAD data */
    x25routeentrytype *lci_rtp;		/* ptr to route entry if routed */
    int lci_alt_index;			/* alternate route counter */
    int lci_alt_diag;			/* alternate route last diag */

    /*
     * CMNS information
     */
    addrtype lci_src_nsap;		/* Calling Address Extension */
    addrtype lci_dst_nsap;		/* Called Address Extension */
    uchar lci_src_nsap_digits;		/* digit count and authority for
					 * lci_src_nsap */
    uchar lci_dst_nsap_digits;		/* digit count and authority for
					 * lci_dst_nsap */
    uchar *lci_cudptr;			/* ptr to cud for fwd cmns */
    ushort lci_cudlen;			/* cudlen for fwd cmns */

    ushort lci_swtype;			/* type of switched connection */
    void *lci_swdata;			/* ptr to some data for switch */
    llctype *lci_llc2;			/* ptr to llc2 Control Block */

    /*
     *  X.25 MIB information
     */
    ulong lci_dte_resets;		/* RESETs w/ 'DTE initiated' cause */
    ulong lci_dce_resets;		/* RESETs w/ other causes */
    ulong lci_t2_count;			/* T12/T22 timeouts */
    uchar lci_outgoing;			/* incoming or outgoing call */

    /* QLLC INFO - TILL WE USE ADDRESS MAPS TO CONNECT THEM */
    qllctype *qllc;

    /*
     *  X.25 PACKET BY PACKET COMPRESSION 
     */
    void *lci_pbp_comp_stac;	/*ptr to stac compression area */
    void *lci_pbp_decomp_stac;	/*ptr to stac decompression area */
    ulong lci_pbp_xmt_in_bytes;	/*total xmt uncompressed bytes*/
    ulong lci_pbp_rcv_out_bytes; /*total rcv compressed bytes*/
    ulong lci_pbp_resets;	/*total count of resets     */
    paktype *lci_pbp_depak;	/*ptr to pre allocated pak for decompression */
    paktype *lci_pbp_ackpak;	/*ptr to ack packet queued for x25_timer()   */
    boolean lci_pbp_enabled;	/*PACKET BY PACKET enabled */
				/*needed to protect against MAX NVC > 1      */ 
    boolean lci_pbp_compress;   /*do compression/decompression */
    boolean lci_pbp_send_id;    /*send PBP CUD ID in call      */
    boolean lci_pbp_local_ack;  /* not end to end ack          */
    boolean lci_thc_applied;    /*THC has been applied to this requeued pak*/
    /*current for delta mod2 comps in x25_pbp_show()*/
    ulong lci_pbp_xmt_in_interv[COMP_STAT_PERIODS_MAX];       /*last xmt in*/
    ulong lci_pbp_bytessent_interv[COMP_STAT_PERIODS_MAX];    /*last xmt out*/
    ulong lci_pbp_rcv_out_interv[COMP_STAT_PERIODS_MAX];      /*last rcv out*/
    ulong lci_pbp_bytesrcvd_interv[COMP_STAT_PERIODS_MAX];    /*last rcv in*/
    /*delayed for delta mod2 comps in x25_pbp_show()*/
    ulong lci_pbp_xmt_in_interv_l[COMP_STAT_PERIODS_MAX];     /*last xmt in*/
    ulong lci_pbp_bytessent_interv_l[COMP_STAT_PERIODS_MAX];  /*last xmt out*/
    ulong lci_pbp_rcv_out_interv_l[COMP_STAT_PERIODS_MAX];    /*last rcv out*/
    ulong lci_pbp_bytesrcvd_interv_l[COMP_STAT_PERIODS_MAX];  /*last rcv in*/
    boolean lci_pbp_rnr;	/*sent an RNR waiting for decompression pak*/
    ulong lci_pbp_need_pak_size; /*decompression pak size required*/
    queuetype lci_pbp_rnr_deferredQ; /*hold queue while RNR is in effect*/
    ushort lci_pbp_next_clk[COMP_STAT_PERIODS_MAX]; /* clk x25_pbp_timer()*/
    ushort lci_pbp_rnr_ticks;	/*max wait counter for rnr before RESET */
    uchar lci_pbp_xmt_sync;	/*sync cntr. initially 0 but never again*/
    uchar lci_pbp_rcv_sync;	/*sync cntr. initially 0 but never again*/
    char lci_pbp_pr;		/*inner node most recent ps received    */ 
    char lci_pbp_ps;		/*inner node send count ps sent         */
    char lci_pbp_ack;		/*inner node last ack pr sent           */
    char lci_pbp_rpr;		/*inner node most recent pr received    */
    char lci_pbp_holdQ_count;   /*far outer nodes lci_holdQ (back pressure)*/
};

/* X.25 api tools */
/* Accessor methods for the lcitype */

/*
 * x25_get_lcn
 *
 * Give the api the LogicalChannel Number
 */

static inline ushort x25_get_lcn(lcitype *lci)
{	return (lci->lci_lci);						}

/*
 * x25_get_dst_address
 *
 * Give the api the Destination (Called) X.121 Address
 */

static inline addrtype *x25_get_dst_address(paktype *pak, lcitype *lci)
{	return (&(lci->lci_dst_addr));					}

/*
 * x25_get_src_address
 *
 * Give the api the Source (Calling) X.121 Address
 */

static inline addrtype *x25_get_src_address(paktype *pak, lcitype *lci)
{	return (&(lci->lci_src_addr));					}

extern boolean x25_called_from_api;

extern addrtype *x25_get_idb_x25_address(idbtype *idb);

#define	X25_LCI_HASH_SIZE	64

extern queuetype x25iQ;			/* input packet queue */
extern queuetype x25aQ;			/* alternate routing */
extern queuetype x25tQ;			/* VC timer queue */
extern queuetype x25LocalAckQ;		/* compression ack queue */

extern lcitype *x25_lci_hash_table[X25_LCI_HASH_SIZE];

extern int x25_vcnum;			/* debug VC number */

#define	X25MINDATA	2		/* min. processable packet size */
#define	X25MAXDATA	4096		/* max. data allowed in data packet */

#define	X25HEADERBYTES	3
#define	X25EHEADERBYTES (X25HEADERBYTES+1)

/*
 * Packet types
 */

/* Call setup and clearing */

#define	X25_CALL_REQUEST		0x0b
#define	X25_CALL_CONNECTED		0x0f
#define	X25_CLEAR			0x13
#define	X25_CLEAR_CONFIRMATION		0x17

/* Data and interrupt */

#define	X25_DATA			0x00
#define	X25_INTERRUPT			0x23
#define	X25_INTERRUPT_CONFIRMATION	0x27

/* Flow control and reset */

#define	X25_RR				0x01
#define	X25_RNR				0x05
#define	X25_DTE_REJ			0x09
#define	X25_RESET			0x1b
#define	X25_RESET_CONFIRMATION		0x1f

/* Restart */

#define	X25_RESTART			0xfb
#define	X25_RESTART_CONFIRMATION	0xff

/* Diagnostic */

#define	X25_DIAGNOSTIC			0xf1

/* Registration */

#define	X25_REGISTRATION		0xf3
#define	X25_REGISTRATION_CONFIRMATION	0xf7

/* cisco Defined */

#define	X25_PVCSETUP			0xf5

#define X25_PVCSETUP_BYTES	12
/*
 *  Remote PVC switching code has existed for some time; prior to the
 *  official support for this feature, a simple implementation was attempted
 *  but didn't work.  The unsupported code may originate PVC setup messages
 *  which contain 2 integers for the remote interface and VC.  The supported
 *  code has a version field that is set to ensure that we can detect this
 *  unsupported message and any old code will not be able to connect using
 *  the new PVC-SETUP message format.
 */
#define X25_PVCSETUP_VER	0x81

/* internal states of a switched PVC */
#define X25_PVC_DOWN		 0	/* PVC never connected or IF RESTART */
#define X25_PVC_LAST_SHORT_TMR	 7

#define X25_PVC_TCP_SHUT	 8	/* TCP shut (far IF may've RESTARTed) */
#define X25_PVC_TCP_REFUSED	 9	/* TCP connection refused */
#define X25_PVC_TCP_ROUTE_ERR	10	/* TCP routing error */
#define X25_PVC_TCP_TOUT	11	/* TCP timed out connection */
#define X25_PVC_LAST_LONG_TMR	15

#define X25_PVC_TCP_TRYING	16	/* attempting TCP connection */
#define X25_PVC_TCP_CONNECTED	17	/* TCP connection established */
#define X25_PVC_UP		18	/* PVC is connected */
#define X25_PVC_BAD_IF		19	/* remote reported bad interface */
#define X25_PVC_IF_NOT_UP	20	/* remote: IF not up */
#define X25_PVC_IF_NOT_X25	21	/* remote: IF not X.25 encapsulation */
#define X25_PVC_NO_PVC		22	/* remote: no such PVC */
#define X25_PVC_CROSS_CON	23	/* remote: PVC cross-connected */
#define X25_PVC_FLOW_MISMATCH	24	/* remote: flow cont. value mis-match */
#define X25_PVC_BAD_FLOW	25	/* remote: IF can't support flow val. */
#define X25_PVC_UNKNOWN_ERR	26	/* unknown PVC connect protocol error */
#define X25_PVC_CODES		27

extern char *const x25_pvc_err_str[X25_PVC_CODES];	/* error strings */


/*
 *  Standard IDs
 */
#define X25_VERSION_1984	0	/* 1984 CCITT Recommendation X.25 */
#define X25_VERSION_1980_DDN	1	/* DDN operation (based on 1980 X.25 */
#define X25_VERSION_1980_BFE	2	/* BFE operation (based on 1980 X.25 */
#define X25_VERSION_1988	3	/* 1988 CCITT Recommendation X.25 */
#define X25_VERSION_1993	4	/* 1993 ITU-T Recommendation X.25 */
#define X25_VERSION_DEFAULT	X25_VERSION_1988

#define ISDDN(idb)					\
	((idb) &&					\
	 (is_x25(idb)) &&				\
	 ((idb->x25_version == X25_VERSION_1980_DDN) ||	\
	  (idb->x25_version == X25_VERSION_1980_BFE)))
#define ISBFE(idb)					\
	((idb) &&					\
	 (is_x25(idb)) &&				\
	 (idb->x25_version == X25_VERSION_1980_BFE))
#define IS_NON_BFE_DDN(idb)				\
	((idb) &&					\
	 (is_x25(idb)) &&				\
	 (idb->x25_version == X25_VERSION_1980_DDN))

/*
 * Packet sizes
 */

#define	X25_CAUSE_DATA		2		/* cause + diag	fields	*/
#define	X25_SCAUSE_DATA		1		/* short (cause only)	*/
#define	X25_DIAG_DATA		(X25HEADERBYTES + 1)
#define	X25_INT_DATA_MAX_1980	1		/* interrupt data size	*/
#define	X25_INT_DATA_MAX_1984	32		/* ... for 1984 X.25	*/
#define	X25_CALL_DATA		2		/* fac.+addr. overhead	*/
#define	X25_CALL_USER_DATA	16		/* max. call user data	*/
#define	X25_FAST_CALL_USER_DATA	128		/* ... for fast select	*/
#define	X25_FACILITY_DATA_1980	63		/* max. facility field	*/
#define	X25_FACILITY_DATA_1984	109		/* ... for 1984 X.25	*/
#define	X25_PAD_CUD_MAX		12		/* X.29 max. call data	*/

#define X121_MAX_DIGITS		15
						/* length & digits of two */
						/* addresses in semi-octets */
#define X25_MAX_X121_ADDR_BLOCK	(1 + X121_MAX_DIGITS)
						/* maximum CLEAR packet	*/
#define X25_MAX_NON_DATA_PACKET	(X25HEADERBYTES + X25_CAUSE_DATA +	\
				 X25_MAX_X121_ADDR_BLOCK +		\
				 1 + X25_FACILITY_DATA_1984 +		\
				 X25_FAST_CALL_USER_DATA)

/*
 * Clearing causes in clear indication packet
 */

#define X25_CLEAR_DTE_ORIGINATED	0x00
#define	X25_CLEAR_NUMBER_BUSY		0x01
#define	X25_CLEAR_OUT_OF_ORDER		0x09
#define	X25_CLEAR_REMOTE_PROC_ERR	0x11
#define	X25_CLEAR_REVERSE_CHARGE_ERR	0x19
#define	X25_CLEAR_INCOMPAT_DEST		0x21
#define	X25_CLEAR_FAST_SELECT_ERR	0x29
#define	X25_CLEAR_SHIP_ABSENT		0x39
#define	X25_CLEAR_INVALID_FACILITY	0x03
#define	X25_CLEAR_ACCESS_BARRED		0x0B
#define	X25_CLEAR_LOCAL_PROC_ERR	0x13
#define	X25_CLEAR_NETWORK_CONGESTION	0x05
#define	X25_CLEAR_NOT_OBTAINABLE	0x0d
#define	X25_CLEAR_RPOA_OUT_OF_ORDER	0x15
#define	X25_CLEAR_DTE_ORIGINATED_M	0x80

/*
 * Resetting causes in reset indication packet
 */

#define X25_RESET_DTE_ORIGINATED	0x00
#define	X25_RESET_OUT_OF_ORDER		0x01
#define	X25_RESET_REMOTE_PROC_ERR	0x03
#define	X25_RESET_LOCAL_PROC_ERR	0x05
#define X25_RESET_NETWORK_CONGESTION	0x07
#define	X25_RESET_REMOTE_DTE_OPER	0x09
#define	X25_RESET_NETWORK_OPERATIONAL	0x0f
#define	X25_RESET_INCOMPAT_DEST		0x11
#define	X25_RESET_NETWORK_OUT_OF_ORDER	0x1d
#define	X25_RESET_DTE_ORIGINATED_M	0x80

/*
 * MAX BUFFER SIZE FOR NETWORK_USER_ID
 */
#define	X25_MAX_NUID_SIZE	130	/*max NUID internal buffer size*/

/*
 * Restarting causes in restart indication packet
 */

#define	X25_RESTART_DTE_ORIGINATED	0x00
#define	X25_RESTART_LOCAL_PROC_ERR	0x01
#define	X25_RESTART_NETWORK_CONGESTION	0x03
#define	X25_RESTART_NETWORK_OPERATIONAL	0x07
#define	X25_RESTART_REG_CAN_CONFIRMED	0x7f
#define	X25_RESTART_DTE_ORIGINATED_M	0x80

/*
 * Registration causes in registration confirmation packet
 */

#define	X25_REGCON_REG_CAN_CONFIRMATION	0x7f
#define	X25_REGCON_INVALID_FACILITY	0x03
#define	X25_REGCON_LOCAL_PROC_ERR	0x13
#define	X25_REGCON_NETWORK_CONGESTION	0x05


/*
 *  Facility and Registration encodings; reference section 7 of 1988 CCITT X.25
 */

/*
 *	reference 1988 CCITT X.25 Table 28/X.25
 */
#define X25_FACILITY_CLASS_MASK		0xc0
#define X25_FACILITY_CLASS_A_CODE	0x00
#define X25_FACILITY_CLASS_B_CODE	0x40
#define X25_FACILITY_CLASS_C_CODE	0x80
#define X25_FACILITY_CLASS_D_CODE	0xc0
#define X25_FACILITY_CLASS_A_LEN	2
#define X25_FACILITY_CLASS_B_LEN	3
#define X25_FACILITY_CLASS_C_LEN	4

#define	X25_FACILITY_EXTENSION		0xff

/*
 *	reference 1988 CCITT X.25 section 7.1
 */
#define	X25_FACILITY_MARKER		0x00
#define	X25_FACILITY_MARKER_LOCAL	0x00
#define	X25_FACILITY_MARKER_REMOTE	0xff
#define	X25_FACILITY_MARKER_CCITT	0x0f

/*
 *	reference 1988 CCITT X.25 Table 29/X.25
 */
#define	X25_FACILITY_FLOW_PACKET	0x42
#define	X25_FACILITY_FLOW_WINDOW	0x43
#define	X25_FACILITY_THROUGHPUT		0x02
#define	X25_FACILITY_CUG_BASIC		0x03
#define	X25_FACILITY_CUG_EXTENDED	0x47
#define	X25_FACILITY_CUG_OUT_BASIC	0x09
#define	X25_FACILITY_CUG_OUT_EXTENDED	0x48
#define	X25_FACILITY_CUG_BILATERAL	0x41
#define	X25_FACILITY_REVERSE_CHARGING	0x01	/* same as fast select! */
#define	X25_FACILITY_FAST_SELECT	0x01	/* same as reverse charge! */
#define	X25_FACILITY_NETWORK_USER_ID	0xc6
#define	X25_FACILITY_CHARGE_REQUEST	0x04
#define	X25_FACILITY_CHARGE_REC_MONEY	0xc5
#define	X25_FACILITY_CHARGE_REC_CNT	0xc2
#define	X25_FACILITY_CHARGE_REC_TIME	0xc1
#define	X25_FACILITY_RPOA_BASIC		0x44
#define	X25_FACILITY_RPOA_EXTENDED	0xc4
#define X25_FACILITY_CALL_DEFLECT_SEL	0xd1
#define	X25_FACILITY_CALL_REDIRECT_NOTE	0xc3
#define	X25_FACILITY_LINE_ADDR_MODIFY	0x08
#define	X25_FACILITY_TDELAY		0x49

/*
 *	reference 1988 CCITT X.25 Table G-1/X.25
 */
#define	X25_FACILITY_CALLING_EXT	0xcb  /* For CMNS NSAP addr */
#define	X25_FACILITY_CALLED_EXT		0xc9  /* For CMNS NSAP addr */
#define X25_FACILITY_QOS_THROUGHPUT	0x0a
#define X25_FACILITY_QOS_TDELAY		0xca
#define X25_FACILITY_QOS_PRIORITY	0xd2
#define X25_FACILITY_QOS_PROTECTION	0xd3
#define X25_FACILITY_EXP_DATA		0x0b  /* Expedited Data negotiation */


/*
 * Facility arguments
 */
#define	X25_FACILITY_REVERSE_CHARGE_ON	0x01	/* 1988 Sec. 7.2.2.6	*/
#define	X25_FACILITY_FAST_SELECT_MASK	0xc0
#define	X25_FACILITY_FAST_SELECT_UNRES	0x80
#define	X25_FACILITY_FAST_SELECT_RES	0xc0
#define	X25_FACILITY_CHARGE_REQ		0x01	/* 1988 Sec. 7.2.2.8.1	*/
#define X25_FACILITY_THROUGHPUT_75	0x03	/* 1988 Table 30/X.25	*/
#define X25_FACILITY_THROUGHPUT_150	0x04
#define X25_FACILITY_THROUGHPUT_300	0x05
#define X25_FACILITY_THROUGHPUT_600	0x06
#define X25_FACILITY_THROUGHPUT_1200	0x07
#define X25_FACILITY_THROUGHPUT_2400	0x08
#define X25_FACILITY_THROUGHPUT_4800	0x09
#define X25_FACILITY_THROUGHPUT_9600	0x0a
#define X25_FACILITY_THROUGHPUT_19200	0x0b
#define X25_FACILITY_THROUGHPUT_48000	0x0c
#define X25_FACILITY_THROUGHPUT_64000	0x0d
#define	X25_FACILITY_REDIRECT_DTE_BUSY	0x01	/* 1988 Table 31/X.25	*/
#define X25_FACILITY_REDIRECT_HUNT_DIST	0x07
#define	X25_FACILITY_REDIRECT_DTE_OUT	0x09
#define	X25_FACILITY_REDIRECT_REDIRECT	0x0f
#define	X25_FACILITY_REDIRECT_DTE_ORIG_MASK	0xc0
#define	X25_FACILITY_CALL_ADDR_DTE_BUSY	0x01	/* 1988 Table 32/X.25	*/
#define	X25_FACILITY_CALL_ADDR_HUNT	0x07
#define	X25_FACILITY_CALL_ADDR_DTE_OUT	0x09
#define	X25_FACILITY_CALL_ADDR_REDIRECT	0x0f
#define	X25_FACILITY_CALL_ADDR_DTE_MASK	0xc0
#define	X25_FACILITY_CALL_ADDR_DTE_ORIG_CODE	0x80
#define	X25_FACILITY_CALL_ADDR_DTE_DEFLECT_CODE	0xc0

/*
 * Registration codes
 * reference 1988 CCITT X.25 Table 33/X.25
 */
#define X25_REGISTRATION_P1_SVCS	0x05
#define X25_REGISTRATION_ANY_TIME	0x45
#define X25_REGISTRATION_AVAILABILITY	0x46
#define X25_REGISTRATION_NON_NEGOTIABLE	0x06
#define X25_REGISTRATION_THROUGHPUT	0x02
#define X25_REGISTRATION_PACKET_SIZES	0x42
#define X25_REGISTRATION_WINDOW_SIZES	0x43
#define X25_REGISTRATION_VC_RANGES	0xc8

/*
 *	reference 1988 CCITT X.25 Table 34/X.25
 */
#define X25_REGISTRATION_P1_MOD_128	0x01
#define X25_REGISTRATION_P1_DTE_REJ	0x02
#define X25_REGISTRATION_P1_MODIFY_D	0x04

/*
 *	reference 1988 CCITT X.25 Table 35/X.25
 */
#define X25_REGISTRATION_ANY_BAR_INCOM	0x0100
#define X25_REGISTRATION_ANY_BAR_OUTGO	0x0200
#define X25_REGISTRATION_ANY_FAST_SEL	0x0400
#define X25_REGISTRATION_ANY_REV_CHARGE	0x0800
#define X25_REGISTRATION_ANY_FLOW_NEG	0x1000
#define X25_REGISTRATION_ANY_THRU_NEG	0x2000
#define X25_REGISTRATION_ANY_CHARGING	0x4000

/*
 *	reference 1988 CCITT X.25 Table 36/X.25
 */
#define X25_REGISTRATION_AVAIL_DEF_THRU		0x0001
#define X25_REGISTRATION_AVAIL_DEF_WINDOW	0x0002
#define X25_REGISTRATION_AVAIL_DEF_PACKET	0x0004
#define X25_REGISTRATION_AVAIL_VC_RANGES	0x0008
#define X25_REGISTRATION_AVAIL_RPOA		0x0010
#define X25_REGISTRATION_AVAIL_MOD_128		0x0100
#define X25_REGISTRATION_AVAIL_DTE_REJ		0x0200
#define X25_REGISTRATION_AVAIL_MODIFY_D		0x0400
#define X25_REGISTRATION_AVAIL_MODIFY_CALLED	0x0800
#define X25_REGISTRATION_AVAIL_CHARGE_INT	0x1000
#define X25_REGISTRATION_AVAIL_CHARGE_SVC	0x2000
#define X25_REGISTRATION_AVAIL_REVERSE_ACCEPT	0x4000
#define X25_REGISTRATION_AVAIL_REVERSE_CHARGE	0x8000

/*
 *	reference 1988 CCITT X.25 Section 7.3.2.4
 */
#define X25_REGISTRATION_NON_NEG_LOCAL_CHARGING	0x01


/*
 * Diagnostic codes
 */
#define	X25_DIAG_NO_INFO		0
#define	X25_DIAG_INVALID_PS		1
#define	X25_DIAG_INVALID_PR		2

#define	X25_DIAG_INVALID_PTI		16
#define	X25_DIAG_INVALID_PTI_R1		17
#define	X25_DIAG_INVALID_PTI_R2		18
#define	X25_DIAG_INVALID_PTI_R3		19
#define	X25_DIAG_INVALID_PTI_P1		20
#define	X25_DIAG_INVALID_PTI_P2		21
#define	X25_DIAG_INVALID_PTI_P3		22
#define	X25_DIAG_INVALID_PTI_P4		23
#define	X25_DIAG_INVALID_PTI_P5		24
#define	X25_DIAG_INVALID_PTI_P6		25
#define	X25_DIAG_INVALID_PTI_P7		26
#define	X25_DIAG_INVALID_PTI_D1		27
#define	X25_DIAG_INVALID_PTI_D2		28
#define	X25_DIAG_INVALID_PTI_D3		29

#define	X25_DIAG_PACKET_NOT_ALLOWED	32
#define	X25_DIAG_UNIDENT_PACKET		33
#define	X25_DIAG_CALL_ON_1WAY_LC	34
#define	X25_DIAG_INVALID_PTI_PVC	35
#define	X25_DIAG_PACKET_UNASS_LC	36
#define	X25_DIAG_REJ_NOT_SUBSCRIBED	37
#define	X25_DIAG_PACKET_TOO_SHORT	38
#define	X25_DIAG_PACKET_TOO_LONG	39
#define	X25_DIAG_INVALID_GFI		40
#define	X25_DIAG_ILLEGAL_LCI		41
#define	X25_DIAG_PACK_INCOMPAT_FACILITY	42
#define	X25_DIAG_UNAUTH_INTR_CONF	43
#define	X25_DIAG_UNAUTH_INTR		44
#define	X25_DIAG_UNAUTH_REJ		45

#define	X25_DIAG_TIMER_EXPIRED		48
#define	X25_DIAG_TE_INCOMING_CALL	49
#define	X25_DIAG_TE_CLEAR_INDICATION	50
#define	X25_DIAG_TE_RESET_INDICATION	51
#define	X25_DIAG_TE_RESTART_INDICATION	52

#define	X25_DIAG_CALL_ERROR		64
#define	X25_DIAG_CODE_ILLEGAL		65
#define	X25_DIAG_FACILITY_PARAM_ILLEGAL	66
#define	X25_DIAG_INVALID_CALLED_ADDR	67
#define	X25_DIAG_INVALID_CALLING_ADDR	68
#define	X25_DIAG_INVALID_LENGTH		69
#define	X25_DIAG_INCOMING_CALL_BARRED	70
#define	X25_DIAG_LC_UNAVAIL		71
#define	X25_DIAG_CALL_COLLISION		72
#define	X25_DIAG_DUP_FACILITY		73
#define	X25_DIAG_NONZERO_ADDR_LEN	74
#define	X25_DIAG_NONZERO_FACILITY_LEN	75
#define	X25_DIAG_FACILITY_EXPECTED	76
#define	X25_DIAG_INVALID_DTE_FACILITY	77

#define	X25_DIAG_MISCELLANEOUS		80
#define	X25_DIAG_ILLEGAL_DTE_CAUSE_CODE	81
#define	X25_DIAG_NOT_ALIGNED_OCTET	82
#define	X25_DIAG_INCONSISTENT_Q_BIT	83
#define	X25_DIAG_INVALID_NUID_FACILITY	84	

#define	X25_DIAG_INTERNATIONAL_PROBLEM	112
#define	X25_DIAG_REMOTE_NETWORK		113
#define	X25_DIAG_REMOTE_PROTOCOL	114
#define	X25_DIAG_LINK_OUT_OF_ORDER	115
#define	X25_DIAG_LINK_BUSY		116
#define	X25_DIAG_TRANSIT_NET_FACILITY	117
#define	X25_DIAG_REMOTE_NET_FACILITY	118
#define	X25_DIAG_ROUTING_ERROR		119
#define	X25_DIAG_TEMP_ROUTING_ERROR	120
#define	X25_DIAG_UNKNOWN_CALLED_DNIC	121
#define	X25_DIAG_MAINTENANCE_ACTION	122

/*
 * X25 States
 */

/* Transfer of restart packets */

#define X25_R1 0			/* Packet level ready */
#define	X25_R2 1			/* DTE restart request */
#define	X25_R3 2			/* DCE restart indication */

/* Call setup phase */

#define	X25_P1 3			/* Ready */
#define	X25_P2 4			/* DTE waiting */
#define	X25_P3 5			/* DCE waiting */
#define	X25_P4 6			/* Data transfer */
#define	X25_P5 7			/* Call collision */

/* Call clearing phase */

#define	X25_P6 8			/* DTE clear request */
#define	X25_P7 9			/* DCE clear indication */

/* Transfer of reset packets */

#define X25_D1 10			/* Flow control ready */
#define	X25_D2 11			/* DTE reset request */
#define X25_D3 12			/* DCE reset indication */

#define	X25_NSTATES	13

#define	X25_X1	13			/* Connection held down */

#define CMNS_FWD_MAXNVC	2

/*
 * LLC2 and X25 PLP (CMNS)
 */
#define LLC2HEADERBYTES 4
#define ISIEEEMSK(idb)  ( idb && (idb->status & IDB_IEEEMASK ))
#define ISCMNS_IEEE(idb) ( ISIEEEMSK(idb) && idb->cmns_intfc_enabled)

/*
 * Facility parsing roles
 */
#define X25_ROLE_CALLING_DTE	0
#define X25_ROLE_CALLED_DTE	1
#define X25_ROLE_		2	/* no role */

/* 
 * return values from x25_reassemble
 */
typedef enum {
    X25_PACKET,		/* *pakptr has a real packet			*/
    X25_QUEUED,		/* input packet was put on the reassembly queue */
    X25_NOPACKET	/* reassembly failed, reassem queue is purged.	*/
} x25_reassm_e;

/*
 * X25 event codes for notifying an API about significant events
 */

typedef enum {
    X25_BOGUS_EVENT = 0,
    X25_INCALL_EVENT,			/* incoming call 	*/
    X25_ACCEPT_EVENT,			/* call accept		*/
    X25_CLEAR_IND_EVENT,		/* clear indication	*/
    X25_CLEAR_CONF_EVENT,		/* clear confirmation	*/
    X25_RESET_IND_EVENT,		/* reset indication	*/
    X25_RESET_CONF_EVENT,		/* reset confirmation	*/
    X25_RR_EVENT,			/* packet level RR	*/
    X25_RNR_EVENT,			/* Packet level RNR	*/
    X25_UNQUENCH_EVENT,			/* Packet level RR, or holdq not full */
    X25_QUENCH_EVENT,			/* Packet level RNR, or hold q full */
    X25_DELETE_EVENT,			/* lci is being torn down - let api know */
    X25_NUM_EVENTS			/* count the event codes */
} x25_event_t;

/*
 * low water mark : so that we can maximize line utilization 
*/
#define X25_LCI_LOW_WATER	1	/* assuming x25 is much slower than
					 * the lan
					 */

/* 
 * Macros 
 */

#define CCITT_marker(lci)	(lci->lci_dst_nsap.length ||	\
				 lci->lci_src_nsap.length)

#define IS_X25_CLEAR_DTE_ORIGINATED(c)					\
				(*c == X25_CLEAR_DTE_ORIGINATED ||	\
				 (*c & X25_CLEAR_DTE_ORIGINATED_M))

#define IS_X25_RESTART_DTE_ORIGINATED(c)				\
				(*c == X25_RESTART_DTE_ORIGINATED ||	\
				 (*c & X25_RESTART_DTE_ORIGINATED_M))

#define IS_LCI_PVC(idb, lci)	((lci) <				\
				 (idb->x25_lic ? idb->x25_lic :		\
				  (idb->x25_ltc ? idb->x25_ltc :	\
				   (idb->x25_loc ? idb->x25_loc : 0x1000))))

/*
 *  The DDN encapsulations are based on the 1980 CCITT Recommendation
 *  where the standard encapsulations are operate according the 1984 CCITT
 *  Recommendation.  DDN does have some quirks apart from the differences
 *  between CCITT Recommendations, so the macro IS_1980 is used where the
 *  CCITT version requires different behavior and macro ISDDN is used where
 *  the DDN X.25 specification differs from 1980 CCITT operation.
 */
#define IS_1980(idb)	((idb->x25_version == X25_VERSION_1980_DDN) ||	\
			 (idb->x25_version == X25_VERSION_1980_BFE))
#define IS_1984(idb)	(idb->x25_version == X25_VERSION_1984)
#define IS_1988(idb)	(idb->x25_version == X25_VERSION_1988)

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern boolean x25_routing;
extern boolean x25_tcp_use_if_defs;
extern boolean x25_xot_confirm_svc_reset;
extern void (*x25_state_table[X25_NSTATES])(hwidbtype *hwidb,
					    paktype *pak, lcitype *lci);

/*
 * regexp.c
 * regsub.c
 *
 * These are bogus prototypes to hide the internal workings of the
 * regular expression code from the rest of the system.
 */
extern regexp *regcomp(char *pattern);
extern int regexec(regexp *exp, char *ck_str);
extern int regsub(regexp *exp, char *rewrite_pattern, char *result, int max);

/*
 * x25.c
 */
extern void x25_init(subsystype *subsys);
extern boolean ISDTE(hwidbtype *hwidb, considbtype *considb);
extern boolean ISDCE(hwidbtype *hwidb, considbtype *considb);
extern paktype *getx25(hwidbtype *hwidb, lcitype *lci, int bytes, int extended);
extern char *x25_prstate(hwidbtype *hwidb, lcitype *lci);
extern void x25_prpack(hwidbtype *hwidb, paktype *pak, lcitype *lci, char io);
extern void x25_restart(hwidbtype *hwidb, uchar ack);
extern lcitype *x25_call_setup(idbtype *idb, x25map *map,
			       addrtype *proto_addr, uchar *subaddr,
			       ushort lci_linktype,
			       uchar prec, uchar *cud, uint cud_len,
			       boolean reverse);
extern void x25_call_remove(lcitype *lci, uchar cause, uchar diag);
extern void x25_call_reset(lcitype *lci, boolean kill);
extern void x25_deencapsulate(hwidbtype *hwidb);
extern boolean x25_extract_lci(paktype *pak, long address, addrtype *proto_addr,
			       lcitype **lci_ptr, boolean *broadcast);
extern boolean x25_vencapsulate(paktype *pak, long address);
extern boolean x25_encaps(paktype *pak, long address, x25map *map);
extern void x25_send_quench(paktype *pak);
extern boolean x25_output(hwidbtype *hwidb, paktype *pak, enum HEADTAIL which);
extern boolean x25_lapboutput(paktype *pak);
extern void x25_senddiag(hwidbtype *hwidb, paktype *pak,
			 uchar err, lcitype *lci);
extern void x25_sendint(lcitype *lic, paktype *pak, uchar *data, uint len);
extern boolean x25_badgfi(uchar qbit, uchar dbit, uchar sn,
			  uchar pti, uchar modulo);
extern boolean x25_data(paktype *pak, int *pr, int *mbit, int *ps, int *len);
extern boolean x25_ack(hwidbtype *hwidb, lcitype *lci, int pr);
extern void x25_sendcause(hwidbtype *hwidb, lcitype *lci, uchar type,
			  uchar reason, uchar diag, paktype *opak);
extern void sendpacket(hwidbtype *hwidb, lcitype *lci, uchar type,
		       uchar dbit, paktype *opak);
extern void x25_sendflow(hwidbtype *hwidb, lcitype *lci, uchar type);
extern boolean x25_sendcall(lcitype *lci, uchar *cudptr, uint cudlen);
extern void x25_input(hwidbtype *hwidb, paktype *pak);
extern void x25_process_other(hwidbtype *hwidb, lcitype *lci, paktype *pak,
			      int pr, int mbit, int ps, int len);
extern void x25_process_data (hwidbtype * idb, lcitype * lci, paktype * pak,
			int pr, int mbit, int ps, int len);
extern boolean x25BLOCK(void);
extern forktype x25_timer(void);
extern void x25_addtimer(lcitype *lci);
extern long x25_getlink(paktype *pak);
extern int x25_default_n1(hwidbtype *hwidb);
extern int x25_payload(hwidbtype *hwidb);
extern boolean x25_fix_flow_fac(lcitype *lci);
extern boolean x25_valid_config(hwidbtype *hwidb, boolean set);
extern boolean x25_setup(parseinfo *csb, hwidbtype *hwidb);
extern void x25_cstate(hwidbtype *hwidb);
extern void x25_interface(hwidbtype *hwidb);
extern void x25_clearcounters(hwidbtype *hwidb);
extern void DteProtocolViolation(lcitype *lci, hwidbtype *hwidb, uchar err);
extern boolean x25_set_pvc_con_time(lcitype *lci);

/*
 * x25_facil.c
 */
extern boolean x25_parsefacility(hwidbtype *, lcitype *, uchar *, uchar, uchar *, uchar *, int);
extern paktype *x25_insert_flow_facil(paktype *, uchar, uchar, uchar, uchar);
extern int x25_facil_speed(hwidbtype *, uint);
extern int x25_facil_thru(uint);
extern void x25_format_nsap(char *, char *, int);
extern uchar *x25_printfacility(uchar *);

/*
 * x25_parse.c
 */
extern char x25_parse_nuid(lcitype *);
extern rpoatype *x25_rpoa_lookup(char *);
extern lcitype *x25_pvc_setup(idbtype *, int);
extern boolean x25_clns_parse_addr_map(parseinfo *, hwaddrtype *, addrtype *);

/*
 * x25_parser.c
 */
extern void copy_x25_fields(hwidbtype *, hwidbtype *);

/*
 * x25_service.c
 */
extern boolean x25serviceBLOCK(void);
extern boolean x25_service_accept(lcitype *);
extern forktype x25_service(void);
extern void x25_service_init(void);
extern void x25_service_input(lcitype *, paktype *);
extern void x25_service_remove(lcitype *);
extern void x25_service_start(void);

/*
 * x25_state.c
 */
extern void x25_d1(hwidbtype *, paktype *, lcitype *);
extern void x25_d2_d3(hwidbtype *, paktype *, lcitype *);
extern void x25_p1(hwidbtype *, paktype *, lcitype *);
extern void x25_p2_p3(hwidbtype *, paktype *, lcitype *);
extern void x25_p4(hwidbtype *, paktype *, lcitype *);
extern void x25_p5(hwidbtype *, paktype *, lcitype *);
extern void x25_p6_p7(hwidbtype *, paktype *, lcitype *);
extern void x25_process_call(lcitype *lci, paktype *pak,
			     uchar *cud, int cud_len);
extern void x25_process_call_collision(hwidbtype *, lcitype *, paktype *);
extern void x25_process_call_connected(hwidbtype *, lcitype *, paktype *);
extern void x25_process_pad_accept_call(lcitype *);
extern void x25_process_clear(hwidbtype *, lcitype *, paktype *, uchar, uchar , boolean);
extern void x25_process_interrupt(lcitype *, paktype *, uchar *, uint);
extern void x25_reenter_d1(lcitype *);
extern void x25_process_reset(lcitype *, paktype *, uchar, uchar);
extern void x25_r1(hwidbtype *, paktype *, lcitype *);
extern void x25_r2_r3(hwidbtype *, paktype *, lcitype *);

/*
 * x25_hash.c
 */
#define X25_NO_VC_ASSIGNED      0       /* for x25_lci_enter() */
extern void x25_lci_hash_init(void);
extern void x25_lci_hash_restart(hwidbtype *idb, llctype *llc2);
extern lcitype *x25_lci_llc2_lookup(hwidbtype *idb, llctype *llc2,
				    boolean nconnected);
extern lcitype *x25_lci_lookup(hwidbtype *idb, int vc, llctype *llc2);
extern lcitype *x25_lci_lookup_next(hwidbtype *idb, int vc, llctype *llc2);
extern lcitype *x25_lci_lookup_by_x121(hwidbtype *hwidb, uchar *addr, int addr_len);
extern boolean x25_lci_get_info(lcitype *, int *, hwaddrtype *, ulong *);
extern boolean x25_lci_get_stats(lcitype *, ulong *, ulong *, ulong *, ulong *, ulong *, ulong *);
extern void x25_lci_holddown(lcitype *lci);
extern uint x25_free_lci(hwidbtype *idb, llctype *llc2);
extern lcitype *x25_lci_enter(idbtype *idb, int vc, llctype *llc2);
extern void x25_lci_delete(lcitype *lci);
extern lcitype *lci_hashpick(lcitype *hp, int index);
extern void show_x25status(int vc);
extern void x25_pvc_nvgen(parseinfo *csb);
extern void x25_clear_vc(parseinfo *csb);

/*
 * x25_cons.c
 */
extern void cons_enqueue(paktype *);
extern void cons_restart(hwidbtype *, lcitype *, paktype *, int);
extern considbtype *get_lci_considb(lcitype *);
extern boolean set_lci_considb(hwidbtype *, lcitype *);
extern void cons_cstate(hwidbtype *, llctype *, boolean);
extern lcitype *cons_restart_lci(hwidbtype *, paktype *);
extern boolean x25_llc2output(paktype *pak);
extern void x25_llc2_clsproc(llctype *, int);
extern void x25_cons_route_failed(lcitype *, int);

/* 
 * cons_parse.c
 */
extern void cons_interface(idbtype *);
extern void show_llc2_cons(llctype *);

/*
 * x25_chain.c
 */
extern void x25_parser_init(void);

/*
 * cons_chain.c
 */
extern void cons_parser_init(void);

#endif __X25_H__
