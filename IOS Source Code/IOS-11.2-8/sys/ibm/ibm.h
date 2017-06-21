/* $Id: ibm.h,v 3.2.60.2 1996/08/28 12:46:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibm.h,v $
 *------------------------------------------------------------------
 * Definitions for IBM things
 *
 * July 1990, Eric B. Decker
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Byte Stream Stunnel packets (tcp) have a stunhdrtype on the front.  Exact
 * format is as described by the code.  All transports including
 * Direct interface Stunnells require a header.
 *------------------------------------------------------------------
 * $Log: ibm.h,v $
 * Revision 3.2.60.2  1996/08/28  12:46:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/05/17  11:19:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/11  14:37:43  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  14:33:31  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  09:22:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/01  02:32:02  vrawat
 * CSCdi37993:  fix stun problems found at customer site
 *
 * Revision 2.5  1995/07/27  03:11:03  vrawat
 * CSCdi33572:  AS/400 sdlc multidrop enviromnment
 *
 * Revision 2.4  1995/07/06  01:32:14  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.3  1995/07/02 05:15:58  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.2  1995/06/18  23:36:35  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:46:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _IBM_H_
#define _IBM_H_

#include "ibm_types.h"

#define STUN_MAGIC 0x0831

/* Magic and op must ALWAYS be the same order/size as shown below */
typedef struct _stunhdrtype {
    ushort magic;
    ushort  op;
    ushort len;				/* length of this segment/packet */
    uchar  group;
    uchar  addr[1];	/* Actual length may vary based on total addr size */
} stunhdrtype;

#define STUN_MIN_GROUP (1)
#define STUN_MAX_GROUP (255)

#define STUN_OP_PASS	      0x0	/* just pass through, compatibility */
#define STUN_OP_NOP	      0x1	/* do nothing */
#define STUN_OP_IGNORE	      0x2	/* dump data portion */
#define STUN_OP_EMPTY	      0x3	/* header empty, should get header */
#define STUN_OP_VREQ          0x4	/* version req. (must ALWAYS be 4) */
#define STUN_OP_VREP	      0x5	/* version reply (must ALWAYS be 5) */
/* The following set are used for SDLC Local Acknowledgement */
#define STUN_OP_LINKUP_REQ    0x6	/* Local Ack Linkup Request */
#define STUN_OP_LINKUP_RSP    0x7	/* Local Ack Linkup Response */
#define STUN_OP_LINKDOWN_REQ  0x8	/* Local Ack Link Down Request */
#define STUN_OP_LINKDOWN_RSP  0x9	/* Local Ack Link Down Response */
#define STUN_OP_ABORT_REQ     0xA	/* AbortRequest			*/
#define STUN_OP_ABORT_RSP     0xB	/* AbortResponse		*/
#define STUN_OP_RD_REQ        0xC	/* RequestDisconnectRequest	*/
#define STUN_OP_DATA	      0xD	/* SDLC Data			*/
#define STUN_OP_QUENCH_ON     0xE       /* STUN flow control on         */ 
#define STUN_OP_QUENCH_OFF    0xF       /* STUN flow control off        */ 
#define STUN_OP_SIM           0x10      /* SIM                          */
#define STUN_OP_LINK_ESTAB    0x11      /* Link Established             */
#define STUN_OP_REROUTE       0X12      /* reroute I-frame on next link in TG */
 
#define STUN_OP_OPCODE	      0x3F 	/* Opcode portion (highest opcode) */

/* End of SDLC Local Acknowledgement specific types */

/* This is an indicator bit used to show a frame should go down to SDLC */
#define STUN_OP_TO_SDLC	      0x40	/* Pass to SDLC indicator */   

/* These types are "bad" to receive */
#define STUN_OP_ERR	0x80		/* error bit */
#define STUN_OP_BADOP	0x81		/* illegal opcode */
#define STUN_OP_UNKPEER 0x82		/* we don't know who you are */
#define STUN_OP_UNKADDR	0x83		/* we don't recognize this one */

#define STUN_MAX_QLEN 100		/* Max length of TCP queue */
#define STUN_CONFIG_MAX_QLEN 500        /* Max TCP Qlen configurable */
#define STUN_CONFIG_MIN_QLEN 10         /* Min TCP Qlen configurable */

/*
 * info describing a stunnel peer.
 *
 * Peer States.
 */

typedef enum _stunstatetype {
    STUN_PS_DEAD,			/* temporarily shutdown */
    STUN_PS_CLOSED,			/* no activity */
    STUN_PS_OPENING,			/* waiting for connector */
    STUN_PS_OPENWAIT,			/* waiting for transport open */
    STUN_PS_CONNECTED,			/* fully open */
    STUN_PS_DIRECT			/* direct if other is stun too */
} stunstatetype;

/*
**  States of SLACK_FSM:
*/
typedef enum _slackstatetype
{
    SLACK_DISCONNECTED       = 1,       /* Disconnected          */
    SLACK_AWAIT_LINKUP_RSP   = 2,       /* AwaitLinkupRequest    */
    SLACK_AWAIT_SDLC_OPEN    = 3,       /* AwaitSdlcOpen         */
    SLACK_ACTIVE             = 4,       /* Active                */
    SLACK_AWAIT_LINKDOWN_REQ = 5,       /* AwaitLinkdownRequest  */
    SLACK_AWAIT_LINKDOWN_RSP = 6,       /* AwaitLinkdownResponse */
    SLACK_AWAIT_LINKDOWN_UA  = 7,       /* AwaitLinkdownUa       */
    SLACK_AWAIT_ABEND_DISC   = 8,       /* AwaitDisc, Abend      */
    SLACK_AWAIT_ABORT_RSP    = 9,       /* AwaitAbortResponse    */
    SLACK_AWAIT_NORM_DISC    = 10,      /* AwaitDisc, Normal case */

    SLACK_FSM_MAX = SLACK_AWAIT_NORM_DISC  /* Max legal value of "state" */
} slackstatetype;

typedef struct _slackstatistics
{
    ulong       s_frames_rcvd;          /* we received S-Frames on SDLC */
    ulong       s_frames_rcvd_rr;       /*  .. S-Frames that were RR */
    ulong       s_frames_rcvd_rnr;      /*  .. S-Frames that were RNR */
    ulong       s_frames_sent;          /* we sent S-Frames on SDLC */
    ulong       s_frames_sent_rr;       /*  .. S-Frames that were RR */
    ulong       s_frames_sent_rnr;      /*  .. S-Frames that were RNR */
    ulong       i_frames_rcvd;          /* we received I-Frames on SDLC */
    ulong       i_frames_sent;          /* we sent I-Frames on SDLC */
    int         num_disconnects;        /* times we entered SLACK_DISC.. */
    int         num_connects;           /* times we entered SLACK_CONN.. */
    int         num_slowdowns;          /* times we entered slowdown */
} slackstatistics;

/* Keepalive parameters */
#define STUN_KEEPALIVE_DEFAULT_INTERVAL (30 * ONESEC)
#define STUN_KEEPALIVE_DEFAULT_COUNT    (3)
#define STUN_KEEPALIVE_TX_PERIOD        (30 * ONESEC)
#define STUN_SLACK_FSM_TIMEOUT_DEFAULT  (20)
 
/*
 * STUN_SERIAL_MAXBYTES is the number of bytes we can transport across
 * a Stunnel connection.  This is very dependent on what the
 * physical interface can do.  It is based on what packet size a STUN
 * link uses.  This must include a STUN header.
 */

#define STUN_SERIAL_MAXBYTES	2084

typedef enum _stuntypetype {
    STUN_PT_NONE = 0,			/* no type, constrainted to 0 */
    STUN_PT_TCPD,			/* TCP, tcpdriver implementation */
    STUN_PT_IF,				/* Direct interface */
    STUN_PT_FR,				/* Frame Relay Direct Encapsulation */
    STUN_PT_CLS,                        /* CLS driver implementation */ 
    STUN_PT_ANY				/* Any, not defined for reality */
} stuntypetype;

typedef enum _stunconnrtn {		/* return code from vrp->connect */
    STUN_CONN_ABORTED,			/* structures changed, forget it */
    STUN_CONN_FAILED,			/* Couldn't connect */
    STUN_CONN_CONNECTED,
    STUN_CONN_CONNECTING                /* added for cls */ 
} stunconnrtn;

#define STUN_MIN_ADDR_LENGTH (1)
#define STUN_MAX_D_OR_O_ADDR_LENGTH (4)
#define STUN_MAX_ADDR_LENGTH (8)
#define STUN_PRIORITY_COUNT 3

struct stunpeertype_ {
    struct stunpeertype_ *next;
    uchar group;		        /* Stored here as well for future */
    ulong user_def_max_tcp_qlen;        /* only valid for TCP encaps      */
    uchar addr[STUN_MAX_ADDR_LENGTH];
    boolean input_addr_inuse;
    uchar input_addr[STUN_MAX_ADDR_LENGTH];
    boolean all_addr;		        /* Is this the "default" address? */
    stunstatetype state;		/* what state this peer is in */
    boolean slack_flow_control;		/* are we in flow control? */
    slackstatetype slack_state;         /* what slack state this peer is in */
    slackstatistics slack_stats;        /* slack statistics */
    paktype *slack_snrm_pak;            /* The SNRM we had... */
    int retries;			/* used on open */
    sys_timestamp dead_timer;		/* how long before we try again */
    hwidbtype *stun_idb;		/* stun side interface */
    sys_timestamp last_keepalive;       /* last time a keepalive was sent */
    int slack_retry_count;              /* slack retransission counter     */
    boolean slack_ua_rcvd;              /* Linkup response received */

    /*
     * Peer control vectors.
     */
    boolean	  (*sender)(struct stunpeertype_ *, paktype *, hwidbtype *, 
			    boolean, struct stunpeertype_ **, boolean, 
			    boolean);
    void	  (*abort)(struct stunpeertype_ *);
                                       /* (peer), immediate flush */
    void	  (*close)(struct stunpeertype_ *, ulong); 
                                       /* (peer, param) no input, drain out */
    stunconnrtn   (*connect)(struct stunpeertype_ *);	/* (peer, cvrn) */

    /*
     * Other control cells.
     */
    ulong drops;
    ulong pkts_rx;			/* num pkts incoming */
    ulong pkts_tx;			/* num pkts outgoing */
    ulong bytes_rx;			/* num bytes incoming */
    ulong bytes_tx;			/* num bytes outgoing */

    stuntypetype type;

    /*
     * CP DRIVER implementation
     * STUN_PT_TCPD
     */
    int port;				/* tcpdriver, and tcp packet */
    ipaddrtype ipaddr;			/* tcpdriver, and tcp packet */
    tcp_encaps *primary_t;		/* tcpdriver */
    tcp_encaps *secondary_t;
    stunhdrtype inhdr;			/* incoming header */
    boolean local_ack_capable;		/* local ack for this peer? */
    boolean local_ack_required;		/* Require "local-ack" keyword */
    boolean local_ack_active;           /* local ack currently running */
    boolean local_ack_sim_sent;         /* sim sent on sdlc port       */
    boolean local_ack_tg_load;          /* tg's send i-frames after sim/rim */
    boolean priority;			/* Any priority info for this peer? */
    boolean tg_active;			/* TG in use here? */
    struct stunpeertype_ *priority_peers[STUN_PRIORITY_COUNT + 1];
    struct stunpeertype_ *priority_top;
    int peers_open;                     /* number of priority peers open */

    /*
     * Frame Relay Direct Interface implementation
     * STUN_PT_FR
     */
    idbtype *outswidb;		
    ushort dlci;			

    /* Frame Relay Local and Remote SAPs for Local-ack
     */
    byte lsap; 
    byte rsap; 
    byte cls_priority;
    void*    thisCep;    /* for cls driver */ 
    void*    thisDlu;    /* for cls driver */ 
    void*    thisPort;   /* for cls driver */ 

    /* SNRM and XID quick response */ 
    paktype *stored_xid_response;
    boolean slack_xid_rsp_received;
    boolean now_xid_passthru; 

    /*
     * Direct Interface implementation
     * STUN_PT_IF
     */
    hwidbtype *outidb;			/* direct interface idb */
    uchar macaddr[6];			/* if a mac address is needed. */
};

typedef struct _stun_protocol_info {
    /*--Beginning of what user can create--*/
    char name[NAMEDTHINGNAMELEN + 1];
    ulong addrlength;		/* Length, in bytes, of address field */
    ulong addroffset;		/* Offset, in bytes, of start of addr field  */
    char addrfmt;	        /* 'd' = decimal, 'o' = octal, 'x' = hex */
    char prototype;		/* SDLC_PTYPE, GENERIC_PTYPE */
    boolean onlyall;		/* Only allow "all" keyword */
    boolean neverall;		/* NEVER allow "all" keyword */
    boolean local_ack_capable;	/* Allow "local-ack" (and priority) keywords */
    boolean local_ack_required;	/* Require "local-ack" keyword */
    /*---end of what user can create--*/
    boolean permanent;	        /* User deletable? */
    boolean uses_tg;		/* Uses TG support? */
    int (*serial_forward_hook)(hwidbtype *, paktype *, stunpeertype *); 
                                /* Code to call before fwd from serial */
    boolean (*network_forward_hook)(hwidbtype *, paktype *, stunpeertype *, 
			    ushort); /* Code to call before fwd from net */
    void (*shut_interface)(hwidbtype *);     /* Shut an interface */
    void (*long_periodic_hook)(hwidbtype *); /* Code to cal once a minute */
    void (*show_hook)(void);    /* Show hook */
    void (*little_show_hook)(boolean, hwidbtype *, stunpeertype *);
                                /* Little show hook */
    boolean (*global_parse_hook)(parseinfo *); /* Global parse hook */
    boolean (*interface_parse_hook)(parseinfo *); /* Interface parse hook */
    void (*global_nv_hook)(struct _stun_protocol_info *, parseinfo *); 
                                /* Global non-vol support */
    void (*interface_nv_hook_1)(struct _stun_protocol_info *, hwidbtype *); 
                                /* Interface non-vol support, < routes */
    void (*interface_nv_hook_2)(struct _stun_protocol_info *, hwidbtype *); 
                                /* Interface non-vol support, > routes */
    void (*print_frame)(paktype *, int, char *, hwidbtype *);	
                                /* Print out a frame */
    boolean (*peer_change_hook)(hwidbtype *, stunpeertype *, boolean); 
                                /* Change status/existence of peer */
} stun_protocol_info;

/*
 * Stun transmission group resequencing information.
 */
typedef struct _stun_tg_info {
    struct _stun_tg_info *next;
    ulong        group;              /* stun protocol group number */
    boolean      tg_wait_for_zero;   /* expected PIU num for cos-reseq */
    stunpeertype *tg_stp;            /* used to ensure order after seq wrap */
    queuetype    tg_reseqQ;          /* queue used for piu resequencing */
} stun_tg_info;

extern stun_protocol_info *stun_pinfo[STUN_MAX_GROUP + 1];

#define STUN_MAX_OPEN_TRIES	3
#define STUN_REMOPEN_DEFTMO	(10 * ONESEC)
#define MAX_PEER_NAME		37

#define SDLC_PTYPE    1
#define GENERIC_PTYPE 2 

#endif















