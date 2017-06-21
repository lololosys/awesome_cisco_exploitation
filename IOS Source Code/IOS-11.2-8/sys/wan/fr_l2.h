/* $Id: fr_l2.h,v 3.1.68.5 1996/08/15 01:28:23 chifei Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_l2.h,v $
 *------------------------------------------------------------------
 * fr_l2.h Frame Relay layer 2 support for SVC per Q.922
 *
 * July 1995, Chifei Cheng
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_l2.h,v $
 * Revision 3.1.68.5  1996/08/15  01:28:23  chifei
 * CSCdi66087:  LAPF T203 default should be 30 seconds
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/07/03  01:57:45  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1.68.3  1996/05/23  14:32:25  chifei
 * CSCdi57332:  FR SVC initialization point needs to be re-examined
 * Branch: California_branch
 * This is layer 2 specific change.
 *
 * Revision 3.1.68.2  1996/04/30  21:39:01  chifei
 * CSCdi56098:  sh int shows svc info when the subsystem doesnt exist
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/25  23:23:23  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.72.1  1996/04/24  04:11:21  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/12/04  19:47:25  shankar
 * FR SVC development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Layer 2 DL-Control States per Q922 Annex B
 *	-- state 1 - 3 are meaningless for implementation
 *	-- ordering here affects lapf_event_table
 */
enum Lapf_state_types {
    TEI_ASSIGNED,		/* equivalent to state 4 */
    AWAITING_ESTABLISHMENT,	/* equivalent to state 5 */
    AWAITING_RELEASE,		/* equivalent to state 6 */
    MULTIPLE_FRAME_ESTABLISHED,	/* equivalent to state 7 */
    TIMER_RECOVERY,		/* equivalent to state 8 */
    LAPF_MAX_STATES		/* keep it last */
};

/*
 * Row indices for lapf_event_table
 * CRYTICAL: LAPF_MAX_EVENTS and the ordering here affects lapf_event_table
 *	Don't add or delete members of Lapf_event_index without the
 *	the corresponding change in fr_l2.c
 */
enum Lapf_event_index {
	LAPF_I_INDEX = 0,
	LAPF_SABME_INDEX,
	LAPF_UA_INDEX,
	LAPF_DISC_INDEX,
	LAPF_DM_INDEX,
	LAPF_FRMR_INDEX,
	LAPF_RR_INDEX,
	LAPF_RNR_INDEX,
	LAPF_REJ_INDEX,
	LAPF_MAX_EVENTS		/* keep it the last */
};

enum Lapf_command_index {
    LAPF_T200_CMD = LAPF_MAX_EVENTS,
    LAPF_N201_CMD,
    LAPF_N200_CMD,
    LAPF_T203_CMD,
    LAPF_K_CMD,
    LAPF_FRMR_CMD,	/* don't send FRMR */
    LAPF_CFG_CMD,	/* keep it at the end of non-hidden commands */
    LAPF_RETRY_CMD,
    LAPF_TEST,		/* test L2 only */
    LAPF_LAPD,		/* talk to LAPD */
    LAPF_CLRCNT,	/* clear counters now and at every link reset */
    LAPF_TEST_CMD
};

/*
 * PH-, MDL- primitives and layer 2 system management events
 */
enum Lapf_sys_events {
    LAPF_HDR_CHK = LAPF_TEST_CMD,
    LAPF_TEARDOWN,
    LAPF_TIMES,
    LAPF_CLR_COUNTERS,
    LAPF_SHOW_IF,	/* info for show interface */
    FR_PH_DEACT,
    FR_PH_ACT,
    FR_MDL_REMOVE,
    FR_MDL_ASSIGN
};

/*
 * IMPORTANT:
 * Values 0xF1 - 0xF8 are preserved for all FR_DL_* primitives
 * which are shared between layer 2 and 3. They don't quite must be such
 * values, but both layer 2 and 3 have their other set-up respectively
 * based on that; so better not to change it. Layer 2's requirement:
 * All constants for lapf_cfgCmd(), i.e. all in Lapf_command_index,
 * Lapf_event_index, and Lapf_sys_events, need to be unique
 * and less than (unsigned) 0xF1 (the first of FR_DL_*); otherwise
 * lapf_cfgCmd() needs to be changed.
 */
enum Lapf_dl_primitives {
    FR_DL_EST_REQ = 0xF1,
    FR_DL_EST_CNF,
    FR_DL_EST_IND,
    FR_DL_REL_REQ,
    FR_DL_REL_CNF,
    FR_DL_REL_IND,
    FR_DL_DATA_REQ,
    FR_DL_DATA_IND
};

enum Lapf_userCmd_key {
    LAPF_SHOW,
    LAPF_CFG
};


/*
 * error code for lapf_cause field in lapf_control_block
 *	- errors A-O follow Appendix V/Q.922, others our own implementation
 *	- if Q.922's error code changed, change ours accordingly
 */
enum lapf_err_code {
    LAPF_ERR_SUP_F1 = 'A',	/*'A'  rcv'd unsolicited S frame f=1 */
    LAPF_ERR_DM_F1,		/*'B'  rcv'd unsolicited DM f=1 */
    LAPF_ERR_UA_F1,		/*'C'  rcv'd unsolicited UA f=1 */
    LAPF_ERR_UA_F0,		/*'D'  rcv'd unsolicited UA f=0 */
    LAPF_ERR_DM_F0,		/*'E'  rcv'd unsolicited DM f=0 */
    LAPF_ERR_PEER_SABME,	/*'F'  peer initiate re-establishment */
    LAPF_ERR_REXMT_SABME,	/*'G'  unsuccessful re-xmtting SABME */
    LAPF_ERR_REXMT_DISC,	/*'H'  unsuccessful re-xmtting DISC */
    LAPF_ERR_REXMT_RR,		/*'I'  unsuccessful re-xmtting enquery*/
    LAPF_ERR_NR,		/*'J'  invalid N(R) */
    LAPF_ERR_FRMR,		/*'K'  rcv'ed FRMR in multi-frame state */
    LAPF_ERR_UNDEFINED,		/*'L'  rcv'd non-implementd frame */
    LAPF_ERR_I_FIELD,		/*'M'  info field of I frame too long */
    LAPF_ERR_LENGTH,		/*'N'  rcv'd frame of invalid length */
    LAPF_ERR_REXMT,		/*'O'  up to N200 re-xmt failure */
			/* begin internal error code */
    LAPF_ERR_MEMORY,		/*'P' memory out */
    LAPF_ERR_PEER_DISC,		/*'Q' got DISC in state 7 */
    LAPF_ERR_LMI,		/*'R' LMI detects error and reset link */
    LAPF_ERR_IFDOWN,		/*'S' interface down - admin or hard */
    LAPF_ERR_DISABLED,		/*'T' svc disabled */
    LAPF_ERR_INIT,		/*'U' init state, to be up if no error */
    LAPF_ERR_L3,		/*'V' inconsistency with L3 */
    LAPF_ERR_SET_REXMT,		/*'W' inconsistency in set re-xmt */
    LAPF_ERR_NR_VA,		/*'X' inconsistency in update VA */
    LAPF_ERR_UNACK_Q,		/*'Y' inconsistency in unAck queue */
    LAPF_ERR_I_Q,		/*'Z' inconsistency in I frame queue */
    LAPF_ERR_MAX
};

/*
 * some more internal return code
 */
enum fr_dl_returncode {		/* return code for call from L3 */
    FR_DL_PROCEED,		/* request taken, L2 proceeds */
    FR_DL_CONFIRM,		/* L2 confirms L3's request */
    FR_DL_ERROR			/* errors, request ignored */
};


enum Lapf_timer_type {
    LAPF_T200,
    LAPF_T203,
    LAPF_RETRY
};


/*
 * control headers
 */
#define LAPF_RR		0x01
#define LAPF_RNR	0x05
#define LAPF_REJ	0x09
#define LAPF_SABME	0x6f
#define LAPF_UA		0x63
#define LAPF_DISC	0x43
#define LAPF_DM		0x0f
#define LAPF_UI		0x03
#define LAPF_FRMR	0x87
#define LAPF_XID	0xaf

#define DLCI_CR_MASK	0x0200
#define LAPF_PF_MASK	0x1
#define LAPF_U_MASK	0xef
#define LAPF_U_PF_SET	0x10
#define LAPF_BIT_ONE	0x01
#define LAPF_BIT_TWO	0x02
#define LAPF_BIT_SET	1
#define LAPF_BIT_CLEAR	0
#define LAPF_U_SIZE	3
#define LAPF_S_SIZE	4

#define LAPF_DLCI0_CMD 0x0001
#define LAPF_DLCI0_RSP 0x0201
#define DUMMY_CALL_REF_LENGTH 1

/*
 * system default values
 */
#define     LAPF_DEF_T200	15	/* max ceti-secs to get an ack */
#define     LAPF_DEF_T203	30	/* max secs w/o activities */
#define     LAPF_DEF_K		7	/* max outstanding I frames */
#define     LAPF_DEF_N201	260	/* max length of I field */
#define     LAPF_DEF_N200	3	/* max retransmissions */
#define     LAPF_DEF_CLRSTATS	0	/* not clear statistics at reset */

#define     LAPF_RETRY_DELAY	600	/* every 10 min retry to come up*/
#define     T203_Q921		10	/* default idle timer by Q.921 */

typedef struct Lapf_o_list {		/* circular double linked list node */
    struct Lapf_o_list *flink;
    paktype *pak;
    struct Lapf_o_list *blink;
} Lapf_o_list_t;

/*
 * control block per interface
 */
typedef struct lapf_control_block 
{
    struct lapf_control_block *lapfcb_link;	/* required for scheduler */
    struct lapf_control_block *flink;	/* for I re-queue process */
    uchar lapf_state;		/* protocol state */
    uchar lapf_vs;		/* send state variable */
    uchar lapf_va;		/* acknowledge state variable */
    uchar lapf_vr;		/* receive state variable */
    uchar lapf_ns;		/* last received N(S) */
    uchar lapf_nr;		/* last received N(R) */
    uchar lapf_k;		/* k window size */
    uchar lapf_n200;		/* max retransmission count */
    uchar lapf_n200_cnt;	/* retransmission count */
    uchar lapf_t200_delay;	/* T200 re-xmt timer value */
    uchar lapf_t203_delay;	/* T203 idle timer value */
    uchar lapf_cause;		/* cause code for data link down */
    uint lapf_retry_delay;	/* auto retry to come up */
    uint lapf_n201;		/* max Info field size */
    uint lapf_k_cnt;		/* # frames in I queue */
    mgd_timer lapf_timers;	/* master timer */
    mgd_timer lapf_t200;	/* T200 re-xmt timer */
    mgd_timer lapf_t203;	/* T203 idle timer */
    mgd_timer lapf_retry_timer;	/* layer 2 auto retry to come up */
    queuetype lapf_iQ;		/* I frame queue */
    Lapf_o_list_t lapf_unAckQ;	/* xmted packets waiting to be acked */
    struct Lapf_stats {
	ulong lapf_Iframe_xmt_cnt;	/* total I frames sent */
	ulong lapf_Iframe_rcv_cnt;	/* total I frames received */
	ulong lapf_rexmt_cnt;		/* total retransmit frames */
	ulong lapf_rr_xmt_cnt;		/* total RRs sent */
	ulong lapf_rr_rcv_cnt;		/* total RRs received */
	ulong lapf_rnr_xmt_cnt;		/* total RNRs sent */
	ulong lapf_rnr_rcv_cnt;		/* total RNRs received */
	ulong lapf_rej_xmt_cnt;		/* total REJs sent */
	ulong lapf_rej_rcv_cnt;		/* total REJs received */
	ulong lapf_sabme_xmt_cnt;	/* total SABMEs sent */
	ulong lapf_sabme_rcv_cnt;	/* total SABMEs received */
	ulong lapf_ua_xmt_cnt;		/* total UAs sent */
	ulong lapf_ua_rcv_cnt;		/* total UAs received */
	ulong lapf_dm_xmt_cnt;		/* total DMs sent */
	ulong lapf_dm_rcv_cnt;		/* total DMs received */
	ulong lapf_frmr_xmt_cnt;	/* total FRMRs sent */
	ulong lapf_frmr_rcv_cnt;	/* total FRMRs received */
	ulong lapf_disc_xmt_cnt;	/* total DICSs sent */
	ulong lapf_disc_rcv_cnt;	/* total DISCs received */
	ulong lapf_ui_rcv_cnt;		/* total non-LMI UIs received */
	ulong lapf_xid_rcv_cnt;		/* total XIDs received */
	ulong lapf_tx_defer_cnt;	/* total Tx deferrals due to RNR */
	ulong lapf_i_xmt_drop_cnt;	/* total dropped outgoing I */
	ulong lapf_i_rcv_drop_cnt;	/* total dropped outgoing I */
	ulong lapf_rcv_drop_cnt;	/* total dropped incoming packet */
	ulong lapf_ignore_cnt;		/* total events ignored */
	ulong lapf_reset_cnt;		/* total times of lapf link reset */
    } lapf_stats;

    paktype *lapf_requeue_pak;	/* packet driver was unable to send */

    union lapf_flags_ {			/* flags used for state machine */
	uchar allflags;
	struct {
	    unsigned rcvRsp	: 1;	/* received external L2 Response */
	    unsigned rcvRel	: 1;	/* DL-Release-Req outstanding */
	    unsigned ackPending	: 1;	/* had polled my peer */
	    unsigned rcvPF	: 1;	/* received frame has P/F bit set */
					/* must keep this bit here that maps
					 * to U frame header byte */
	    unsigned peerRNR	: 1;	/* peer receive busy */
	    unsigned rejExcpt	: 1;	/* reject exception */
	    unsigned vs_wrap	: 1;	/* set if v(s) just wrapped */
	    unsigned rcvEst	: 1;	/* DL-Establish-Req outstanding */
	} flag;
    } lapf_flags;			/* flags for lapf operations */

    struct {				/* flags for internal functions */
	unsigned svc_enabled	: 1;	/* remember the flg before test mode */
	unsigned flg_changed	: 1;	/* had changed enabled flag */
	unsigned i_requeued	: 1;	/* CB is re-queued for I frame output */
	unsigned i_queued	: 1;	/* CB is queued for I frame output */
	unsigned no_frmr	: 1;	/* clear if xmt FRMR at frame reject */
	unsigned clrStats	: 1;	/* clear statistics when L2 link down */
	unsigned simulate	: 1;	/* simulating LAPD for testing */
	unsigned testL2		: 1;	/* set to test L2 only */
    } lapf_flags2;

    uchar rcvFrame;			/* mark which frame just received */
    uchar echoI_cnt;			/* for test only */
    hwidbtype *hwidb;			/* that this block is associated with */
} Lapfcb_t;

#define LAPF_FLAGS_RESET 0x3		/* mask to reset lapf_flags */

/* flags for rcvFrame */
#define LAPF_RCV_I	0x80
#define LAPF_RCV_SABME	0x40
#define LAPF_RCV_UA	0x20
#define LAPF_RCV_DSDM	0x10
#define LAPF_RCV_FRMR	0x08
#define LAPF_RCV_RR	0x04
#define LAPF_RCV_RNR	0x02
#define LAPF_RCV_REJ	0x01


typedef struct lapf_hdr_ {	/* header for I or S frame */
    ushort dlci;		/* 2 byte DLCI */
    ushort hdr;
} Lapf_hdr_t;

typedef union lapf_id_ {	/* 3rd and 4th bytes */
    ushort id;
    struct {
	uchar id1;
	uchar id2;
    } oct;
} Lapf_id_t;


extern char lapf_l3_to_l2 (uchar, fr_idb_struct_type *, paktype *);
