/* $Id: at_aurp.h,v 3.2.60.3 1996/07/19 22:44:19 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_aurp.h,v $
 *------------------------------------------------------------------
 * Definitions for AppleTalk Update-Based Routing Protocol
 *
 * February 1994, Steven Lin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_aurp.h,v $
 * Revision 3.2.60.3  1996/07/19  22:44:19  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/03/23  01:25:52  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2.60.1  1996/03/18  18:51:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:28:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:33:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:41:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:49:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  06:49:27  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:07:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_AURP_H__
#define __AT_AURP_H__

#include "types.h"
#include "at_fsm.h"

#define AURP_VERSION 1			/* Version of AURP */

#define AURP_SCHED_DELAY 5*ONESEC	/* How often to run port fsms */

#define AURP_MAX_SEQUENCE 65535		/* Max packet sequence number */

#define AURP_MAX_CONNECTION_ID 65535
#define AURP_MIN_CONNECTION_ID  1
typedef ushort connid_t;

#define AURPTUPLEVERSION 0x00		/* Version byte in RI-Rsp packets */
#define OPTIMIZED_TUPLE 0x80
#define OFFSET_MASK     (~(OPTIMIZED_TUPLE << 8))

#define AURP_DEFAULT_TICKLETIME (90*ONESEC) /* How often to tickle peer */
#define AURP_TICKLE_BEFORE_DATA_TIME (2*ONEMIN)
#define AURP_PATHENTRY_INTERVAL (ONEMIN) /* how often to sweep pathentries */
#define AURP_PATHAGER_INTERVAL (ONEMIN)

#define AURP_DEFAULT_UPDATE_RATE (30)	/* In seconds */

/* Send Update Information bit masks */
#define DEFAULT_SUI  0x7800
#define NA_SUI_MASK  0x4000
#define ND_SUI_MASK  0x2000
#define NRC_SUI_MASK 0x2000
#define NDC_SUI_MASK 0x1000
#define ZC_SUI_MASK  0x0800

/* Miscellanceous bit masks */
#define SZI_MASK       0x4000		/* For RI-Rsp and RI-Upd packets */
#define REMAPPING_MASK 0x4000		/* For Open-Rsp packets */
#define HOPREDUCE_MASK 0x2000		/* For Open-Rsp packets */
#define LASTFLAG_MASK  0x8000		/* For RI-Rsp packets */

/*******************************************************************
 * Domain Identifiers
 *******************************************************************/

/* DI Authorities */
typedef enum {
    AURP_NULL_DI = 0,
    AURP_IP_DI   = 1
} di_type;

/* IP Domain Identifier */
#define DI_IP_LENGTH 7			/* length of IP DI, not including */
					/* length byte */
typedef struct ip_di_ {
    uchar         length;
    uchar         authority;
    ushort        reserved;
    ipaddrtype    addr;
} ip_di_t;

/* Null Domain Identifier */
#define DI_NULL_LENGTH 1		/* length of NULL DI, not including */
					/* length byte */
typedef struct null_di_ {
    uchar         length;
    uchar         authority;
} null_di_t;

/* General Domain Identifier */
typedef union {
    ip_di_t       ip;
    null_di_t     null;
    struct {
	uchar     length;
	uchar     authority;
	uchar     id[0];
    } generic;
} di_t;

/*******************************************************************
 * Packet Headers
 *******************************************************************/
    
typedef enum {
    SEQOK,
    SEQDELAYED,
    SEQBADSYNC,
    SEQIGNORE,
} Seq_status;

/* IP-style Domain Packet Header, p. 20 */
#define IP_DOMAINHEADER_SIZE	(DI_IP_LENGTH + 1 + DI_IP_LENGTH + 1 + 6)
typedef struct ip_domainheader_ {
    ip_di_t       dest_di;
    ip_di_t       src_di;
    ushort        version;
    ushort        reserved;
    ushort        pkttype;
} ip_domainheader_t;

typedef enum {				/* AURP Packet Types */
    ATALK_DATA = 2,
    AURP_DATA  = 3
} aurp_pkttype;

typedef enum {				/* AURP Command Codes, p. 59 */
    RI_REQ     = 1,
    RI_RSP     = 2,
    RI_ACK     = 3,
    RI_UPD     = 4,
    RD         = 5,
    ZIP_REQ    = 6,
    ZIP_RSP    = 7,
    OPEN_REQ   = 8,
    OPEN_RSP   = 9,
    TICKLE     = 14,
    TICKLE_ACK = 15
} aurp_cmdtype;

typedef enum {				/* AURP Error Codes, p. 77 */
    AUTHERROR = -7,
    RESOURCEERROR = -6,
    INVALIDVERSION = -5,
    OPTIONERROR = -4,
    LOSTSYNC = -3,
    LOOPFOUND = -2,
    NORMALCLOSE = -1,
} ErrorCode;

/* AURP packet header */
#define AURP_HEADER_SIZE	(8)
typedef struct aurp_ {
    ushort        connid;	/* Connection ID */
    ushort        sequence;	/* Sequence number */
    ushort        command;	/* Command */
    ushort        flags;	/* Flags */
} aurp_t;

/* Open-Req header, p. 61 */
#define OPEN_REQ_SIZE 	(3)
typedef struct Open_Req_ {
    ushort        version;
    uchar         option_count;
} Open_Req_t;

/* Open-Rsp header, p. 62 */
#define OPEN_RSP_SIZE	(3)
typedef struct Open_Rsp_ {
    short         upd_rate;
    uchar         option_count;
} Open_Rsp_t;

/* RD header, p. 68 */
#define RD_SIZE 	(2)
typedef struct RD_ {
    short         error;
} RD_t;

/* ZIP Rsp subcodes */
enum {
    ZIRSP = 1,
    XZIRSP = 2,
    GZNRSP = 3,
    GDZLRSP = 4,
};

/* ZIP Req subcodes */
enum {
    ZIREQ = 1,
    GZNREQ = 3,
    GDZLREQ = 4,
};

/* ZI-Req header, p. 69 */
#define ZI_REQ_SIZE	(2)
#define GDZL_REQ_SIZE   (4)
typedef struct {
    ushort        subcode;
    ushort        index;
} ZI_Req_t;

/* ZI-Rsp header, p. 70 */
#define ZI_RSP_SIZE	(4)
typedef struct {
    ushort        subcode;
    ushort        tuple_count;
} ZI_Rsp_t;

typedef struct {
    ushort        net;
    union {
	struct {
	    uchar length;
	    uchar name[1];
	} long_tuple;
	ushort    offset;
    } zone_info;
} aurp_zipentry;

/* GDZL-Rsp header, p. 73 */
#define GDZL_RSP_SIZE	(4)
typedef struct {
    ushort        subcode;
    ushort        index;
} GDZL_Rsp_t;

/* GZN-Req header, p. 74 */
#define GZN_REQ_SIZE	(2)
typedef struct {
    ushort        subcode;
} GZN_Req_t;

/* GZN-Rsp header, p. 75 */
#define GZN_RSP_SIZE	(2)
typedef struct {
    ushort        subcode;
} GZN_Rsp_t;

#define EVENT_TUPLE_SIZE	(4)
typedef struct {
    uchar         event_code;
    uchar         net[2];
    uchar         dist;
} Event_tuple_t;



/*******************************************************************
 * Main AURP data structure.
 *
 * Each tunnel interface maintains an at_aurp_info data structure
 * which contains all operational characteristics for the connection.
 *******************************************************************/
typedef struct at_aurp_info_ {
    boolean       enabled;
    atalkidbtype *idb;
    di_type	  ditype;
    di_t          local_di;
    di_t          remote_di;

    /* As data receiver */
    connid_t      r_connid;	/* Receiver's Connection ID */
    ushort        r_sui;	/* Receiver's SUI flags */
    ushort        r_sequence;	/* Receiver's Sequence Number */
    ushort        r_retry_count; /* Packet retransmission counter */
    sys_timestamp r_retry_timer;
    ushort        r_update_rate; /* Peer data sender's nominal rate */
    boolean       r_remapping;	/* Is peer remapping? */
    boolean       r_hopreduce;	/* Is peer reducing hop counts? */
    boolean       r_informed;	/* Have we gotten peer's table? */
    sys_timestamp r_lastheard;		/* Time we last heard from peer. */
    ErrorCode     r_error;	/* Error code to send in RD */
    ulong         r_tickletime;
    neighborentry *
                  neighbor;
    at_fsm        receiver_fsm;
    
    /* As data sender */
    connid_t      s_connid;	/* Sender's Connection ID */
    ushort        s_sui;	/* Sender's SUI flags */
    ushort        s_sequence;	/* Sender's Sequence Number */
    boolean       s_informed;	/* Have we informed our peer? */
    ErrorCode     s_error;	/* Error code to send in RD */
    ushort        s_retry_count;
    sys_timestamp s_retry_timer;
    at_fsm        sender_fsm;
    ushort        s_update_rate;
    paktype *     pktqueue;
    ulong         s_last_sent_update;
} at_aurp_info;

/*******************************************************************
 * AURP "ops" structure
 *
 * When we receive an AURP packet, we decode it into one of these
 * structures to reduce the amount of logic we have to carry around
 * with us to break out the various variable-length fields.
 *******************************************************************/
typedef struct _AURPOps {
    paktype    *  pak;		/* pointer to pak */
    at_aurp_info *aurp;		/* pointer to aurp info */
    di_t	  src_di;	/* source domain identifier */
    di_t	  dest_di;	/* destination domain identifier */
    uchar    	  version;	/* AURP version */
    uchar      	  paktype;
} aurpOps_t;

/*******************************************************************
 * Routing-Updates data structures
 *******************************************************************/
#define GC_EVENTS_INTERVAL (60*ONESEC)	/* How often to GC events database */
#define EXTENDED_TUPLE_MASK 0x80	/* Bit-mask marking extended tuple */
#define EXTENDED_EVENT_TUPLE_SIZE 6	/* Length of extended tuple in bytes */
#define NONEXTENDED_EVENT_TUPLE_SIZE 4	/* Length of normal tuple in bytes */
	
/* Types of update events */
typedef enum {
    NULL_EVENT,
    NA_EVENT,
    ND_EVENT,
    NRC_EVENT,
    NDC_EVENT,
    ZC_EVENT,
} EventCode;

/* Same tree database, sorted by order of entry into database */
typedef struct EventQueue_ {
    dqueue_t dqueue;
    struct EventEntry_ *
                  event;
    ulong	  count;
} EventQueue;

/* A node in the tree database of pending update events */
typedef struct EventEntry_ {
    struct treeLink_ 
                  link;		/* must be first */
    EventQueue    eqlink;
    ushort        dstrngstr;
    ushort        dstrngend;
    uchar         dist;
    EventCode     eventtype;
    atalkidbtype *     idb;
} EventEntry;

/*******************************************************************
 * AURP private path database data structures
 *
 * Each route can potentially have multiple AURP paths.  We must keep
 * track of all of them because our peers will not reinform us of them
 * if their status does not change.  For each route, the database
 * creates an aurp_pathentry node.  All AURP paths for that route are
 * kept as aurp_pdbs which are hung off the pathentry node.
 *******************************************************************/
typedef struct {
    dqueue_t      dqueue;	/* Must be first. */
    struct aurp_pathentry_ *
                  pathentry;	/* Pointer back to pathentry */
    neighborentry *
                  n;		/* Neighbor we learned path from */
    atalkidbtype *     idb;
    atalk_metric_t 
                  metric;	/* Metric of this path */
} aurp_pdb_t;

typedef struct aurp_pathentry_ {
    treeLink	   link;
    ushort	   dstrngstr;
    ushort	   dstrngend;
    atalk_metric_t best_metric;		/* Best metric of all pdbs */
    aurp_pdb_t     pathlist;		/* sentinel */
} aurp_pathentry;

/*******************************************************************
 * 
 *******************************************************************/

/* Useful routines for accessing start of AURP fields */
extern inline uchar *
aurpheadstart(paktype *pak)
{
    return (pak->network_start);
}

extern inline aurp_t *
aurphdrtype_start(paktype *pak)
{
    return ((aurp_t *)aurpheadstart(pak));
}

extern inline uchar *
aurpdatastart(paktype *pak)
{
    return (aurpheadstart(pak) + AURP_HEADER_SIZE);
}

extern inline uchar *
aurpdatastart_aurph(aurp_t *aurph)
{
    return ((uchar *) aurph + AURP_HEADER_SIZE);
}

/*******************************************************************
 * 
 *******************************************************************/

#define FOR_ALL_AURP_TUNNELS(tinfo, ainfo) \
    FOR_ALL_TUNNELS(tinfo) \
        if (((ainfo) = (tinfo)->aurp) != NULL)

/*******************************************************************
 * Function declarations
 *******************************************************************/
ushort 	aurp_next_connid(void);
void 	aurp_Send_Open_Req(at_aurp_info *);
void 	aurp_Send_RI_Req(at_aurp_info *);
void 	aurp_Send_Tickle(at_aurp_info *);
void 	aurp_Send_RD(at_aurp_info *, ErrorCode, boolean);
void 	aurp_Disconnect(at_fsm *);
void 	aurp_Reset(at_fsm *);
void 	aurp_parser_init(void);
void 	aurp_Send_RI_Rsp(at_aurp_info *);
void 	aurp_FreeResources(at_aurp_info *);
void 	aurp_Build_RIRsps(at_aurp_info *);
boolean aurp_Build_RIUpd(at_aurp_info *);
void 	aurp_Send_nullRI(at_aurp_info *);
void 	aurp_Send_RI_Upd(at_aurp_info *);
void 	aurp_ClearPathsFor(atalkidbtype *);
void 	aurp_fsm(at_fsm *, atfsmEvent);
void    aurp_init(void);
boolean	aurp_config_tunnel(atalkidbtype *, boolean, di_type);
void    aurp_UpdateNeighborEntry(at_aurp_info *, neighborStatus);

aurp_pathentry *aurp_FindPathentry (ushort rngstart, ushort rngend);

extern const ulong RIRspIntervals[];
extern const ulong RIUpdIntervals[];
extern mgd_timer aurp_update_eventsTimer;

#endif
