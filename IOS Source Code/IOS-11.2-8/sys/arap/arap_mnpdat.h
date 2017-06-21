/* $Id: arap_mnpdat.h,v 3.2 1995/11/17 08:38:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_mnpdat.h,v $
 *------------------------------------------------------------------
 * General MNP definitions
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: arap_mnpdat.h,v $
 * Revision 3.2  1995/11/17  08:38:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "arap_fcrbis.h"

/* Data Types */

#ifdef __STDC__
#define SIGN_8 signed char
#define SIGN_16 signed short int
#else
#define SIGN_8 char
#define SIGN_16 short
#endif

#define USIGN_8 unsigned char
#define USIGN_16 unsigned short int

#define SUCCESS 	0	/* success return code from all functions */

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

/*-------------------- Return Codes used by LLVL -------------------------*/

#define FAILURE      (-64)	/* no further information */
#define TIME_OUT     (-65)	/* timeout */
#define NO_PHYSICAL  (-66)	/* physical connection down */
#define LNK_DOWN     (-67)	/* link connection down */
/* see mnp_cb.ld_reason for LD reason code */
#define LR_CODE      (-73)	/* when there is an error in parsing an LR,
				 * an LD is sent back with reason code (1-5)
				 * LR_CODE is added to that reason code and
				 * returned as one of the 5 error codes below
				 * 
				 * -72	received LPDU not LR as expected -71
				 * reserved -70	incompatible or unknown
				 * parameters -69	reserved -68
				 * reserved */

#define BUFF_ERR     (-75)	/* buffer error */

/* ----------------------------------------------------------------------- */

#ifndef NULL
#define NULL		0
#endif

/* Protocol Data Units */

#define LR		1	/* link up request */
#define LD		2	/* link down request */
#define LT		4	/* link transfer (data) */
#define LA		5	/* link ack */
#define LN		6	/* link attention */
#define LNA		7	/* link attn ack */

/* Protocol Data Units Formats (Offsets) */

#define PDU_LI		0	/* header length indicator */
#define PDU_TYPE	1	/* type of pdu */

/***************************************************************************
*									   *
*			Buffer Management Definitions			   *
*									   *
***************************************************************************/


/* each blst (buffer list) structure controls an actual buffer */

struct BLST {
    struct BLST *next_b;	/* pointer to next structure */
    USIGN_16 mark;		/* 0: free | 1: used */
    USIGN_8 *bptr;		/* pointer to the actual buffer */
    USIGN_16 len;		/* lenght of data in buffer */
};

/* each buffer structure controls a linked list of blst structures */

struct BUFFER {
    struct BLST *list;		/* pointer to head node */
    struct BLST *free;		/* pointer to node with free buffer */
    USIGN_16 num;		/* total number of nodes (buffers) */
    USIGN_16 used;		/* number of buffers being used */
    struct BLST *used_lst;	/* pointer to head of list of buffers in use */
    struct BLST *next_lst;
};

#define BIT0_ON 	(0x01)

/***************************************************************************

     				LLVL Definitions

***************************************************************************/

/* Link Disconnect reason code */
#define PROT_ERR		1
#define PROT_LVL_MISMATCH	2
#define BAD_LR_PARMS		3
#define RETRAN_TMR_EXP		4
#define INACT_TMR_EXP		5

struct link_ctl_blk {
    /* variables initialized at one-time initialization */
    USIGN_8 status_1;		/* link status */
    USIGN_8 status_2;		/* link status */
    USIGN_8 status_3;		/* link status */
    USIGN_16 ack_timer;		/* acknowledgement timer value */
    USIGN_16 max_data_sz;	/* negotiated maximum user data size */
    USIGN_16 prot_level;	/* negotiated protocol level */
    USIGN_16 lt_tmr;		/* lt (data) retransmission timer value */
    USIGN_16 srv_class;		/* local service class */
    USIGN_16 window_sz;		/* negotiated window size */
    USIGN_16 window_tmr;	/* flow control window timer value */
    long baud_rate;		/* current link baud rate */
    USIGN_8 ln_rsn;		/* last ln (remote) receive sequence number */
    USIGN_8 ln_ssn;		/* last ln (local) send sequence number */

    /* variables initialized/reset at link reset */
    USIGN_8 ltssn_acked;	/* last lt sent seq. # that has been acked */
    /* by remote link */
    USIGN_8 ltrsn_acked;	/* last lt received seq. # that has been */
    /* acked by local link */
    USIGN_8 lt_rsn;		/* last lt (remote) received sequence num */
    USIGN_8 lt_ssn;		/* last lt (local) sent sequence number */
    USIGN_16 ln_ret_cnt;	/* ln retransmission count */
    USIGN_16 lt_ret_cnt;	/* lt retransmission count */

    /* variables initialized at start of data phase */
    USIGN_16 ack_threshold;	/* acknowledgement threshold */
    USIGN_16 lcl_credit;	/* receive (local) credit */
    SIGN_16 rem_credit;		/* send (remote) credit */

    /* other variables */
    USIGN_16 l_disc_code;	/* link layer disconnect code */
    USIGN_16 ln_rtype;		/* ln lpdu type received */
    USIGN_16 ln_stype;		/* ln lpdu type to be sent */
    USIGN_16 lpdu_type;		/* type of last lpdu received */
    USIGN_16 u_disc_code;	/* user disconnect code */
    USIGN_8 lr_parm;		/* lr parameter flag */
};

/*
 * Link Status Byte 1 bit definition (lcb.status_1)
 */
#define LA_RECEIVED	0x01	/* on: la lpdu received */
#define LINK_EST	0x02	/* on: link established */
#define DATA_READY	0x04	/* on: user data available (lt received) */
#define ACK_TIMER	0x08	/* on: timer set */
#define RET_TIMER	0x10	/* on: retran timer set */
#define MODE		0x20	/* on: acceptor; off: initiator */
#define HDUPLEX 	0x40	/* on: half duplex; off: full duplex */
#define ACK		0x80	/* on: ack needed - normal ack sequence */

/*
 * Link Status Byte 2 bit definition (lcb.status_2)
 */
#define FORCE_ACK	0x01	/* on: force ack needed - lt received bad */
#define FORCE_RET	0x02	/* on: lt retran needed - la received bad */
#define ZERO_WNDW	0x04	/* on: zero window in effect */
#define WNDW_TIMER	0x08	/* on: flow control window timer set */

/*
 * Link Status Byte 3 bit definition (lcb.status_3)
 */
#define LN_RECEIVED	0x01	/* on: ln lpdu received */
#define LNA_RECEIVED	0x02	/* on: lna lpdu received */
#define FORCE_LNA	0x04	/* on: need to send an lna lpdu */
#define LN_TIMER	0x10	/* on: ln retran timer set */
#define DUP_IGNORED	0x20	/* on: received an immediate duplicate LT */
/* it was ignored */
#define LN_SENT		0x40	/* on: ln lpdu sent, not acked */

/* timer event flag definition */
#define FRAME_SND	1	/* frame has been sent */
#define FRAME_RCV	2	/* a frame has been received */
#define FRAME_DN	3	/* frame sent, including end flag */

/* offsets of LPDU */
/* sequence number in LT */
#define LT_SEQ		((mnp->lcb.lr_parm & LR_SHORT_MODE) ? 2 : 4)

/* Stream link buffer definitions */
#define RBUF_LEN	2340	/* (260 * 9) was 576 */
#define SBUF_LEN	(1024*8)

/***************************************************************************
*									   *
*			MNP Control Block Definitions			   *
*									   *
***************************************************************************/

typedef struct MNP_CB {
#ifdef FCRMNP
    SIGN_16(*l_connect) ();	/* establish a link connection */
    SIGN_16(*l_disconnect) ();	/* disconnect a link connection */
    SIGN_16(*l_link) ();	/* maintain link */
    SIGN_16(*lt_send) ();	/* send an lt lpdu */
    SIGN_16(*lt_receive) ();	/* receive an lt lpdu */
    SIGN_16(*l_update) ();	/* change link variables */
    SIGN_16(*ret_rcvbuf) ();	/* return a receive buffer */
#endif	/* FCRMNP */

    struct BUFFER lkb;		/* addr of lnk xmit buffers */
    struct BUFFER rlkb;		/* addr of lnk rec buffers */
    struct BUFFER ftb;		/* addr of transmit buffers */

    struct BLST lkblst[2];
    struct BLST rlkblst[2];
    struct BLST iatblst[8];

    SIGN_8 lkbuff[MAX_LPDU_SZ * 2];
    USIGN_8 rlkbuff[MAX_LPDU_SZ * 2];
    USIGN_8 iatbuff[(BLK_DATA_SZ + 5) * 8];

    SIGN_16 ld_reason;		/* reason code in the received LD */
    USIGN_16 ld_source;		/* disc source: 0=layer, 1=user */
    USIGN_16 parity;		/* stream link parity type */

    int fd;			/* i/o file descriptor */
    int fd_flags;
#define	FD_HWFLOW	0x0001
#define	FD_USE_DTR	0x0002
#define	FD_IGNORE_CD	0x0004
#define	FD_SOFT_CD	0x0010
    int old_fd_flags;

    USIGN_8 linestat;		/* line status var */
    int garbage_chars;		/* interframe garbage chars */

    long baudrate;		/* line speed */
    USIGN_8 chan_no;		/* hardware channel no. */

    boolean modem_out_busy;	/* RS-232 hardware status flag 0=transmitter
				 * not active 1=transmitter active */

    boolean sf_lt;		/* 'sending LT' flag */
    boolean sf_busy;		/* 'send frame' flag */
    boolean frame_snt;		/* 'info field sent' flag */
    boolean frame_rcvd;		/* 'frame received' flag */
    boolean frame_dne;		/* 'frame sent' flag */

    USIGN_16 rdle_flg;		/* 'previous char DLE' flag */
    USIGN_8 *sf_ptr;
    USIGN_16 sf_state;		/* send framer state var */
    USIGN_16 rf_state;		/* receive framer state var */
    USIGN_16 sf_len;

    struct link_ctl_blk lcb;	/* link control block */

    USIGN_8 rbuf[RBUF_LEN];	/* receive ring buffer */
    USIGN_16 rb_cnt;		/* receive count */
    USIGN_8 *rb_iptr;		/* rcv buf insert pointer */
    USIGN_8 *rb_rptr;		/* rcv remove ptrs */
    SIGN_16 tbcnt;

    USIGN_8 sbuf[SBUF_LEN];	/* send ring buffer */
    USIGN_16 sb_cnt;		/* snd count */
    USIGN_8 *sb_iptr;		/* snd insert ptr */
    USIGN_8 *sb_rptr;		/* snd remove ptr */

    struct BLST *iatb_struct;	/* transmit buffers */

    /* timers */
    USIGN_16 ln_tm;		/* LN retransmission timer */
    USIGN_16 lt_tm;		/* LT retransmission timer */
    USIGN_16 g_tm;
    USIGN_16 ack_tm;
    USIGN_16 fcw_tm;		/* window timer */
    USIGN_16 lr_tm;

    /* framing pointers */
    USIGN_8 *rf_hptr;
    USIGN_8 *rf_dptr;
    USIGN_16 rf_hdrlen;		/* receive header length  */
    USIGN_16 hdrcnt;
    USIGN_16 rf_datlen;
    USIGN_16 rf_datlen_max;

    long frames_xmit;		/* frames sent */
    long frames_rexmit;		/* frames retransmitted */
    long frames_recv;		/* frames received */
    long frames_recv_bad;	/* frames received w/errors */

    struct BLST *dat_struct;	/* currently active inbound pkt's data */
    struct BLST *hdr_struct;	/* currently active inbound pkt's hdr */
    USIGN_8 *rf_tiptr;		/* temp insert ptr */

    struct {
	USIGN_8 low;
	USIGN_8 hi;
    } snd_fcs,			/* send fcs */
     rcv_fcs,			/* calculated receive fcs */
     rfcs;			/* received fcs */

    /* Link status block */
    struct link_stat_blk {
	USIGN_16 p_status;
	USIGN_16 l_status;
	USIGN_16 s_count;
	USIGN_16 r_count;
	USIGN_16 all_sent;
    } stat;

    boolean debug_flow;
    boolean debug_timers;
    boolean debug_framing;
    boolean debug_mnp;
    boolean debug_v42;

    boolean neg_v42bis;		/* negotiate v.42bis? */

    int v42_dump;
    v42bis_connection_t v42bis;
    u_char v42_ebuf[1024];	/* v.42bis buffer */
#ifdef V42_DUMP_ENABLED
    u_int16 v42_size;
    u_int16 v42_type;
#endif
    u_char v42_dbuf[1024];	/* v.42bis buffer */

} mnp_t;

#define FLOW(s)		if (mnp->debug_flow) logf s
#define T_DEBUG(s)	if (mnp->debug_timers) logf s
#define F_DEBUG(s)	if (mnp->debug_framing) logf s
#define M_DEBUG(s)	if (mnp->debug_mnp) logf s
#define M_DEBUG_ON	(mnp->debug_mnp)
