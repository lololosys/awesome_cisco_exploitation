/* $Id: bstun.h,v 3.5.8.3 1996/09/10 08:48:32 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun.h,v $
 *------------------------------------------------------------------
 * bstun.h - BSTUN management/encapsulation defintions 
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun.h,v $
 * Revision 3.5.8.3  1996/09/10  08:48:32  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.5.8.2  1996/08/28  12:37:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.8.1  1996/05/17  10:40:54  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.32.2  1996/04/11  08:19:43  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.32.1  1996/03/17  17:35:59  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5  1996/02/29  20:34:47  jbalestr
 * CSCdi48520:  BSTUN route command parses DIRECT
 * stops invalid configuration
 *
 * Revision 3.4  1995/12/13  03:22:41  jbalestr
 * CSCdi45429:  bsc lack poller stops transmiting after shutdown/ no shut
 *
 * Revision 3.3  1995/11/21  08:52:24  jbalestr
 * CSCdi42960:  bstun keepalive count
 * Added bstun keepalive interval and count
 *
 * Revision 3.2  1995/11/17  08:48:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/10  07:56:05  sroach
 * CSCdi41879:  Add BSC test command support
 *
 * Revision 2.6  1995/09/03  01:04:09  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.5  1995/08/11  08:21:56  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.4  1995/06/28  09:22:44  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/18  21:27:11  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  06:24:47  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _BSTUN_H_
#define _BSTUN_H_

#define BSTUN_MAGIC 0x0EBD

#define MAXDGRAM_BSTUN    2104	/* bytes, for buffer size checking */

#define BSTUN_MIN_ADDR_LENGTH (1)
#define BSTUN_MAX_D_OR_O_ADDR_LENGTH (4)
#define BSTUN_MAX_ADDR_LENGTH (8)

#define BSTUN_MIN_GROUP (1)
#define BSTUN_MAX_GROUP (255)
#define BSTUN_MAX_ADDRS (256)

#define BSTUN_PRIORITY_COUNT 3

#define BEVENT_HARDWARE  0
#define BEVENT_NETWORK   1
#define BEVENT_TIMEOUT   2

/* Magic and op must ALWAYS be the same order/size as shown below */
typedef struct _bstunhdrtype {
    ushort magic;
    ushort  op;
    ushort len;				/* length of this segment/packet */
    uchar  group;
    uchar  addr[1];	/* Actual length may vary based on total addr size */
} bstunhdrtype;

#define BSTUN_OP_PASS           0x0	/* just pass through, compatibility */
#define BSTUN_OP_NOP	        0x1	/* do nothing */
#define BSTUN_OP_IGNORE	        0x2	/* dump data portion */
#define BSTUN_OP_EMPTY	        0x3	/* header empty, should get header */
#define BSTUN_OP_VREQ           0x4	/* version req. (must ALWAYS be 4) */
#define BSTUN_OP_VREP	        0x5	/* version reply (must ALWAYS be 5) */

/* These types are "bad" to receive */
#define BSTUN_OP_ERR	        0x80	/* error bit */
#define BSTUN_OP_BADOP	        0x81	/* illegal opcode */
#define BSTUN_OP_UNKPEER        0x82	/* we don't know who you are */
#define BSTUN_OP_UNKADDR	0x83	/* we don't recognize this one */

#define BSTUN_MAX_QLEN          100	/* Max length of TCP queue */
#define BSTUN_CONFIG_MIN_QLEN   10      /* Min TCP Qlen configurable */
#define BSTUN_CONFIG_MAX_QLEN   500     /* Max TCP Qlen configurable */

/* keepalive constants */
#define BSTUN_KEEPALIVE_DEFAULT_INTERVAL (30 * ONESEC)
#define BSTUN_KEEPALIVE_DEFAULT_COUNT    (3)
#define BSTUN_KEEPALIVE_TX_PERIOD        (30 * ONESEC)

/*
 * info describing a block serial tunnel peer.
 *
 * Peer States.
 */

typedef enum _bstunstatetype {
    BSTUN_PS_DEAD,			/* temporarily shutdown */
    BSTUN_PS_CLOSED,			/* no activity */
    BSTUN_PS_OPENING,			/* waiting for connector */
    BSTUN_PS_OPENWAIT,			/* waiting for transport open */
    BSTUN_PS_CONNECTED			/* fully open */
} bstunstatetype;

typedef enum {
        BSTUN_ROUTE_ADDR_UNKNOWN,       /* No rte cmd for addr & no rte all. */
        BSTUN_ROUTE_DOWN,               /* Tunnel to remote end down. */
        BSTUN_ROUTE_UP,                 /* Tunnel to remote end up. */
        BSTUN_ROUTE_RESTARTED           /* Tunnel to remote end restarted. */
        } bstunaddrroutestate_e;

typedef struct {
        boolean addr_route_state_change[BSTUN_MAX_ADDRS];
        } bstunaddrroutestatelist_t;

typedef enum _bstunconnrtn {		/* return code from vrp->connect */
    BSTUN_CONN_ABORTED,			/* structures changed, forget it */
    BSTUN_CONN_FAILED,			/* Couldn't connect */
    BSTUN_CONN_CONNECTED
} bstunconnrtn;

typedef enum _bstuntypetype {
    BSTUN_PT_NONE = 0,			/* no type, constrainted to 0 */
    BSTUN_PT_TCPD,			/* TCP, tcpdriver implementation */
    BSTUN_PT_IF,			/* Direct interface */
    BSTUN_PT_TEST,                      /* Test interface */
    BSTUN_PT_ANY			/* Any, not defined for reality */
} bstuntypetype;

typedef struct bstunpeertype_ {
    struct bstunpeertype_ *next;
    uchar group;		        /* Stored here as well for future */
    ulong user_def_max_tcp_qlen;        /* only valid for TCP encaps      */
    uchar addr[BSTUN_MAX_ADDR_LENGTH];
    boolean input_addr_inuse;
    uchar input_addr[BSTUN_MAX_ADDR_LENGTH];
    boolean all_addr;		        /* Is this the "default" address? */
    bstunstatetype state;		/* what state this peer is in */
    boolean addr_route_state_change;
    bstunaddrroutestatelist_t* arsl_p;  /* Address Route State List pointer. */
    int retries;			/* used on open */
    hwidbtype *bstun_idb;		/* bstun side interface */
    sys_timestamp last_keepalive;       /* last time a keepalive was sent */

    /*
     * Peer control vectors.
     */
    boolean	  (*sender)(struct bstunpeertype_ *, paktype *, hwidbtype *, 
			    boolean, struct bstunpeertype_ **, boolean, 
			    boolean);
    void	  (*abort)(struct bstunpeertype_ *);
                                       /* (peer), immediate flush */
    void	  (*close)(struct bstunpeertype_ *, ulong); 
                                       /* (peer, param) no input, drain out */
    bstunconnrtn   (*connect)(struct bstunpeertype_ *);	/* (peer, cvrn) */

    /*
     * Other control cells.
     */
    ulong drops;
    ulong pkts_rx;			/* num pkts incoming */
    ulong pkts_tx;			/* num pkts outgoing */
    ulong bytes_rx;                     /* bytes incoming */
    ulong bytes_tx;                     /* bytes outgoing */

    /*---- TCP transmission stuff. */
    bstuntypetype type;
    int port;				/* tcpdriver, and tcp packet */
    ipaddrtype ipaddr;			/* tcpdriver, and tcp packet */
    tcp_encaps *primary_t;		/* tcpdriver */
    tcp_encaps *secondary_t;
    bstunhdrtype inhdr;			/* incoming header */
    boolean priority;                   /* Any priority info for this peer? */
    struct bstunpeertype_ *priority_peers[BSTUN_PRIORITY_COUNT + 1];
    struct bstunpeertype_ *priority_top;

    /*---- Direct interface transimission stuff. */
    hwidbtype *outidb;			/* direct interface idb */
    uchar macaddr[6];			/* if a mac address is needed. */
} bstunpeertype;


typedef struct _bstun_protocol_info {
    /*--Beginning of what user can create--*/
    char name[NAMEDTHINGNAMELEN + 1];
    ulong addrlength;		/* Length, in bytes, of address field */
    ulong addroffset;		/* Offset, in bytes, of start of addr field  */
    char addrfmt;	        /* 'd' = decimal, 'o' = octal, 'x' = hex */
    boolean onlyall;		/* Only allow "all" keyword */
    boolean neverall;		/* NEVER allow "all" keyword */
    /*---end of what user can create--*/
    boolean local_ack_capable;  /* Local ack capable. */
    boolean permanent;	        /* User deletable? */
    long groupUnroutableTx;     /* Number of unroutable frames received
                                   by this group from the remote partner*/
    long groupUnroutableRx;     /* Number of frames received from the serial
                                   interface with an unsupported poll address*/
    boolean (*validate_address)(hwidbtype* idb, uchar* address, ushort length);
    ushort (*hardware_forward_hook)(hwidbtype *, paktype *); 
                                /* Code to call before fwd from serial */
    boolean (*network_forward_hook)(hwidbtype *, paktype *); 
                                /* Code to call before fwd from net */
    void (*shut_interface)(hwidbtype*);      /* Shut an interface */
    void (*start_interface)(hwidbtype*);     /* Start an interface. */
    void (*network_receive_hook)(paktype*, hwidbtype*);
    void (*hardware_receive_hook)(paktype*, hwidbtype*);
    void (*timer_receive_hook)(paktype*, hwidbtype*);
    void (*print_frame)(paktype *, int, char *, hwidbtype *);	
                                /* Print out a frame */
    boolean (*line_state)(hwidbtype *idb);
} bstun_protocol_info;

extern bstun_protocol_info *bstun_pinfo[BSTUN_MAX_GROUP + 1];


typedef struct _bstun_protocol_list {
    struct _bstun_protocol_list *next;
    NAMEDTHINGMACRO
    bstun_protocol_info *q;
} bstun_protocol_list;


/*
 * bstun_chain.c
 */
extern void bstun_parser_init(subsystype *);

#define BSTUN_MAX_OPEN_TRIES	3
#define BSTUN_MAX_PEER_NAME	37

/*
 * debug variable flags 
 * shared with BSC code
 */

#define B_DEBUG_EVENT         0x01
#define B_DEBUG_PACKET        0x02
#define B_DEBUG_GROUP_SIZE      32

typedef struct _debug_group {
    uint    count;
    uint    flags;
    char    group[B_DEBUG_GROUP_SIZE];
} debug_group;

extern boolean is_debugging_group(const debug_group *debug, uint group);
extern void debug_group_clear(debug_group *debug);
extern boolean debug_set_flag(debug_group *debug, uint flag_on, 
                              uint bitmask, uint group);
extern void print_debug_message(const char* msg, boolean newline, uint flag_on,
                                uint fn_str, uint group);
extern uint debug_display_size(const char*msg, uint sense, uint new_size);
extern void debug_output_frame_data(const uchar* frame,
                                    uint datasize,uint display_size);
#endif
