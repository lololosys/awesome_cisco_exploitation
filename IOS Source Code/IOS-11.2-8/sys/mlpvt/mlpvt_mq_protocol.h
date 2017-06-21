/* $Id: mlpvt_mq_protocol.h,v 1.1.42.1 1996/04/27 06:34:34 syiu Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt_mq_protocol.h,v $
 *------------------------------------------------------------------
 * mlpvt_mq_proto.h -- MLPVT mastership query protocol
 * 
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt_mq_protocol.h,v $
 * Revision 1.1.42.1  1996/04/27  06:34:34  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.5  1996/03/02  02:57:24  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.4  1996/03/01  21:43:14  abaindur
 * Branch: Dial1_branch
 *  o fix the query simulator; a casualty of the integration with MLP/VPN
 *  o enhance query state m/c to handle case where a peer goes down just
 *  when we're sending it a query msg. The TCP write blocks and a peer loss
 *  event blows the peer state from beneath.
 *
 * Revision 1.1.2.3  1996/02/07  20:23:38  syiu
 * Branch: Dial1_branch
 * Add MLP Stackability calls support
 *
 * Revision 1.1.2.2  1996/02/05  16:56:50  abaindur
 * Branch: Dial1_branch
 *  Plug a memory leak. Fix a corner case in the query state m/c.
 *  Remove unneeded simulation code.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:43  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:44  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MLPVT_MQ_PROTOCOL_H__
#define __MLPVT_MQ_PROTOCOL_H__

#define MLPVT_YOU_ARE_MASTER		1

/* Mastership query states */
enum mq_states {
    MQ_STATE_UNKNOWN = 0,
    MQ_STATE_QUERY_TO_PEERS,
    MQ_STATE_QUERY_DONE,
    MQ_STATE_QUERY_ABORTED,
    MQ_STATE_QUERY_FROM_PEERS
};

/* Peer bidding states */
enum pb_states {
    PB_STATE_CREATED = 0,
    PB_STATE_BID_OPENED_TO_PEER,
    PB_STATE_BID_OPENED_FROM_PEER,
    PB_STATE_BID_SENT,
    PB_STATE_BID_RCVD,
    PB_STATE_BID_XCHGD_PASSIVE,
    PB_STATE_BID_XCHGD_ACTIVE,
    PB_STATE_BID_CLOSED,
    PB_STATE_DELETED
};

/* Peer bidding substates */
enum pb_substates {
    PB_SUBSTATE_NONE = 0,
    PB_SUBSTATE_WE_WON,
    PB_SUBSTATE_PEER_WON
};

/* Messages */

#define MLPVT_P2P_UNKNOWN		0
#define MLPVT_P2P_MQO			0x10
#define MLPVT_P2P_MB			0x11
#define MLPVT_P2P_BA			0x12
#define MLPVT_P2P_MQC			0x20
#define MLPVT_P2P_MQA			0x21
#define MLPVT_P2P_MRR			0x80

#define MLPVT_MQO_TIMER			(2*ONESEC)
#define MLPVT_MB_TIMER			(2*ONESEC)
#define MLPVT_MQO_RETRIES		5
#define MLPVT_MB_RETRIES		5
#define MLPVT_MQ_PROCESS_WAKEUP		200

#define MLPVT_MQ_MSG_MINSIZE		20
#define MLPVT_MQ_MSG_MAXSIZE		128

/* Message fields */
#define MLPVT_MQ_MSG_TYPE		1
#define MLPVT_MQ_PEER_ID		2
#define MLPVT_MQ_MEMBER_ID		MLPVT_MQ_PEER_ID
#define MLPVT_MQ_BUNDLE			3
#define MLPVT_MQ_OUR_REFERENCE		4
#define MLPVT_MQ_PEER_REFERENCE		5

#define MLPVT_MQ_ECHO_OF_BID_SEEN	10
#define MLPVT_MQ_NEW_BID		11
#define MLPVT_MQ_MASTER_ID		12

/* Mastership query outcome -- who is the master ?? */
enum mq_result {
    MLPVT_NO_MASTER = 0,
    MLPVT_WE_ARE_MASTER,
    MLPVT_PEER_IS_MASTER
};

#define MLPVT_MAX_HANDLES		30 /* Max handles for same query */
#define MLPVT_REF_MASK			0x7FFF

typedef struct PB_state {
    struct PB_state *next;		/* Next peer */
    struct PB_state *prev;		/* Previous peer */
    struct MQ_state *parent;		/* Parent */
    short	state;			/* Peer query state */
    short	sub_state;		/* Bidding sub state */
    ulong	peer_id;		/* Unique peer id */
    BIDTYPE	peer_bid;		/* Peer's bid value */
    ushort	ref;			/* Peer's query reference number */
    short	retries;		/* Retry count */
    sys_timestamp bid_timer;		/* Timeout for bids */
    ipaddrtype	addr;			/* Peer's address */
} PB_state_t;

typedef struct MQ_state {
    struct MQ_state *next;		/* Next query */
    char 	*bundle;		/* Bundle name */
    ulong	our_id;			/* Our unique id */
    ushort	ref;			/* Query reference number */
    short	state;			/* Query state */
    struct PB_state  *PB_states;	/* List of peer states */
    void	*handles[MLPVT_MAX_HANDLES]; /* Handles for a bundle query */
    int		handle_count;		/* Count of handles */
    BIDTYPE	our_bid;		/* Our bid value for user */
    int		master;			/* Whether master found */
    ulong	master_id;		/* Master's id (peer id) */
    BIDTYPE	master_bid;		/* Master's bid */
    ipaddrtype	master_addr;		/* Master's address */
    sys_timestamp start;		/* Query resolution start */
    long	elapsed;		/* Query resolution time */
} MQ_state_t;

typedef struct {
    ushort	type;			/* Type of field */
    ushort	len;			/* Total length of field */
    char	data[0];		/* Start of data for field */
} msgfield_t;

typedef struct {
    int msg_type;			/* Type of msg - MQO, MB, BA, .. */
    ulong id;				/* Sender's ID */
    ulong grp_id;			/* MLPVT group ID */
    ushort ref;				/* Query reference */
    BIDTYPE bid_seen;			/* Bid seen */
    BIDTYPE new_bid;			/* New bid */
    ulong master_id;			/* ID of master */
} MQmsg_t;

#ifdef MQ_SIMULATOR
typedef struct vpntype_t {
    int status;
    char *bundle;
    ipaddrtype addr;
} vpntype_test;
#endif

#endif /* __MLPVT_MQ_PROTOCOL_H__ */
