/* $Id: cpp.h,v 3.1.2.2 1996/05/21 16:48:32 wmay Exp $
 * $Source: /release/112/cvs/Xsys/wan/cpp.h,v $
 *------------------------------------------------------------------
 * cpp.h - cpp proprietary protocol definitions
 *
 * September, 1995, Bill May
 *
 * Copyright (c) 1995-96 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpp.h,v $
 * Revision 3.1.2.2  1996/05/21  16:48:32  wmay
 * CSCdi58200:  cpp.h inline does not return value
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/10  01:44:23  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/09  17:24:36  wmay
 * Placeholder for California branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
enum cpp_protos {
    CPP_PROTO_COMP_ORDERED = 1,
    CPP_PROTO_ORDERED = 2,
    CPP_PROTO_HDLC = 3,
    CPP_PROTO_FRAG = 4,
    CPP_PROTO_COMP_FRAG = 5,
};

/*
 * The below is the "highest" protocol that we support.
 */
#define CPP_SUPPORTED_PROTO CPP_PROTO_ORDERED

#define CPP_DEFAULT_KEEPTIME	10

#define CPP_ENCAPBYTES	0
#define CPP_ORDERED_ENCAPBYTES	2

typedef struct remote_isdn_type {
    uchar *number;
    ushort subaddr_len;
    uchar subaddress[12];
} remote_isdn_type;

struct cpp_info_type_ {
    remote_isdn_type ports[2];		/* remote addresses */
    mgd_timer neg_timer;		/* negotiation timer */
    usernametype *remote_user_info;	/* remote user info */
    uchar originating_port;		/* b channel of remote */
    uchar cpp_neg_version;		/* remote cpp version */
    ushort keep_periods_missed;		/* Number of keepalives missed */
    boolean neg_pak_rcvd;		/* True if negotiation packet rcvd */
    enum cpp_protos protocol;		/* Protocol we are running over link */
    uchar remote_addr[IEEEBYTES];	/* remote address */
    ipaddrtype remote_ip_address;	/* remote ip address */
    uchar remote_name[8];		/* name of remote router */
    boolean negotiation_complete;	/* True if negotiation complete */
    boolean seen_keepalive;		/* True if we got a keepalive */
    boolean accept_callback;		/* True if we accept callbacks */
    boolean auth_required;		/* True if authentication required */
    boolean save_arp;
    boolean save_arp_def;
    ulong   save_arp_timeout;
    ushort  negotiation_times;
};

#define CPP_MAX_NEGOTIATION_SENT	6
#define CPP_MAX_KEEP_PERIODS_MISSED	3
#define CPP_NEG_DELAY (ONESEC)

/*
 * This is for each bundle to a specific cpp destination
 */
struct cpp_group_type_ {
    ushort tx_sequence_number;
    ushort rx_next_sequence;
    boolean rx_sequence_valid;
    queuetype rxQ;
    mgd_timer rx_sequence_timer;
};

static inline ushort INC_CPP_SEQ (ushort seq, ushort value)
{
    return((seq + value) & 0x7ff);
}
#define CPP_SEQ_NO_MASK	0x0fff
#define CPP_FLAG_MASK	0xf000
#define CPP_INBAND_SEQ_NO	0xfffe
#define MAX_CPP_RX_PAK	7
/*
 * Offsets in bytes for cpp negotiation packets
 * Offset is from ether->data
 */
#define CPP_NEG_CPROTO	 0
#define CPP_NEG_VERSION 2
#define CPP_NEG_LENGTH  3
#define CPP_NEG_FIELDS  5
#define CPP_NEGBYTES	5

#define CPP_CPROTO 0x0200

/*
 * routines for writing/getting shorts and longs - they
 * use intel formats
 */
static inline ushort GETCPPSHORT (void *ptr)
{
    ushort result;

    result = GETSHORT(ptr);
    return((result >> 8) | ((result << 8) & 0xff00));
}

static inline void PUTCPPSHORT (void *ptr, ushort value)
{
    PUTSHORT(ptr, (value >> 8 & 0xff) | (value << 8));
}

static inline ulong GETCPPLONG (void *ptr)
{
    uchar *p;
    ulong result;
    p = ptr;
    result = GETCPPSHORT(p + sizeof(ushort));
    result <<= 16;
    result |= GETCPPSHORT(p);
    return(result);
}

static inline void PUTCPPLONG (void *ptr, ulong value)
{
    uchar *p;
    p = ptr;
    PUTCPPSHORT(p, value & 0xffff);
    PUTCPPSHORT(p + sizeof(ushort), value >> 16);
}
/*
 * Negotiation field definitions
 */
#define CPP_NEG_PORT_1_NUMBER	1
#define CPP_NEG_PORT_2_NUMBER	2
#define CPP_NEG_ORIG_PORT	3
#define CPP_NEG_PROTOCOL	4
#define CPP_NEG_PROTO_LIST	5
#define CPP_NEG_PASSWORD	6
#define CPP_NEG_ETHERNET_ID	7
#define CPP_NEG_DISCONNECT	8
#define CPP_NEG_COMPLETE	9
#define CPP_NEG_LINE_INTEGRITY	10
#define CPP_NEG_RESET_COMP	11
#define CPP_NEG_IP_ADDRESS	12
#define CPP_NEG_PORT_1_SUBADDR	13
#define CPP_NEG_PORT_2_SUBADDR	14
#define CPP_NEG_REMOTE_NAME	15

/*
 * Bit masks for fields to send during negotiation
 */
#define CPP_SEND_PORT_1_NUMBER	0x001
#define CPP_SEND_PORT_2_NUMBER	0x002
#define CPP_SEND_ORIG_PORT	0x004
#define CPP_SEND_PROTOCOL	0x008
#define CPP_SEND_PROTO_LIST	0x010
#define CPP_SEND_PASSWORD	0x020
#define CPP_SEND_ETHERNET_ID	0x040
#define CPP_SEND_DISCONNECT	0x080
#define CPP_SEND_COMPLETE	0x100
#define CPP_SEND_LINE_INTEGRITY	0x200
#define CPP_SEND_RESET_COMP	0x400
#define CPP_SEND_IP_ADDRESS	0x800
#define CPP_SEND_PORT_1_SUBADDR	0x1000
#define CPP_SEND_PORT_2_SUBADDR	0x2000
#define CPP_SEND_REMOTE_NAME	0x4000

#define CPP_SEND_PORTS	(CPP_SEND_PORT_1_NUMBER | CPP_SEND_PORT_2_NUMBER | \
			 CPP_SEND_PORT_1_SUBADDR | CPP_SEND_PORT_2_SUBADDR)

#define CPP_SEND_ON_UP  (CPP_SEND_PROTOCOL | CPP_SEND_PROTO_LIST | \
			 CPP_SEND_ETHERNET_ID | CPP_SEND_IP_ADDRESS | \
			 CPP_SEND_ORIG_PORT | CPP_SEND_REMOTE_NAME | \
			 CPP_SEND_LINE_INTEGRITY | CPP_SEND_PORT_1_NUMBER | \
			 CPP_SEND_PORT_2_NUMBER | CPP_SEND_PORT_1_SUBADDR | \
			 CPP_SEND_PORT_2_SUBADDR)

/*
 * CPP negotation errors
 */
#define CPP_ERR_NO_ERR		0
#define CPP_ERR_PASSWORD_BAD	1
#define CPP_ERR_NO_CALLBACK_NO	2
#define CPP_ERR_BAD_ETHER	3
#define CPP_ERR_CALL_YOU_BACK	4
#define CPP_ERR_BAD_PACKET	5
#define CPP_ERR_NO_PROTOS	6
#define CPP_ERR_TIMEOUT		7
#define CPP_ERR_LIV		8
#define CPP_ERR_UNSPECIFIED	100

/*
 * cpp managed timers
 */
#define CPP_GROUP_RX_SEQUENCE	0
#define CPP_INTERFACE_NEG_TIMER 1

/*
 * parser definitions
 */
enum cpp_command_type {
    CPP_AUTH,
    CPP_CALLBACK,
};

/*
 * external routines from cpp_chain.c
 */
extern void cpp_parser_init(void);
extern void cpp_command(parseinfo *csb);
