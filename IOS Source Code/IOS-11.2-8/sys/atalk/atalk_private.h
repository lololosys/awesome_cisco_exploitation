/* $Id: atalk_private.h,v 3.6.12.7 1996/08/28 12:36:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/atalk_private.h,v $
 *------------------------------------------------------------------
 * atalk_private.h -- internal definitions for AppleTalk protocol family
 *
 * September 1990, David Edwards
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atalk_private.h,v $
 * Revision 3.6.12.7  1996/08/28  12:36:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.12.6  1996/07/23  22:52:35  slin
 * CSCdi63836:  Rework obsolete os call in atalk
 * Branch: California_branch
 * - Remove edisms in send_atalkecho
 *
 * Revision 3.6.12.5  1996/07/16  22:56:01  dwong
 * CSCdi62796:  AT-EIGRP takes too long to age out
 * Branch: California_branch
 *
 * Revision 3.6.12.4  1996/05/04  00:34:06  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Atalk protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on IRB/CRB is enabled or not.
 *
 * Revision 3.6.12.3  1996/05/02  08:40:52  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.6.12.2  1996/05/01  14:26:24  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between Appletalk network code and ATCP code.
 *
 * Revision 3.6.12.1  1996/03/23  01:28:06  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6  1996/02/20  09:33:46  dwong
 * CSCdi47658:  appletalk protocol rtmp and eigrp are not valid
 * configuration
 * Disable rtmp and eigrp from the configuration list when dealing
 * with aurp tunnels
 *
 * Revision 3.5  1996/02/15  07:46:02  smackie
 * If it doesn't return a value, it's gotta be void. Fix numerous
 * examples of inlines that slipped through the net. (CSCdi49069)
 *
 * Revision 3.4  1996/02/08  23:08:15  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.3  1995/11/17  08:44:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:58:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:50:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/29  22:32:25  dwong
 * CSCdi40454:  Wants NBP filters to take effect on outbound traffic
 * Make nbp packets (un)interesting packets when nbp filters are used
 * with dialer lists.
 *
 * Revision 2.4  1995/07/13  06:50:21  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.3  1995/06/28 09:21:44  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/14  22:51:09  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Change SMRP socket from 65 to 10.
 *
 * Revision 2.1  1995/06/07  20:10:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATALK_PRIVATE_H__
#define __ATALK_PRIVATE_H__

#include "master.h"
#include "logger.h"			/* Required for next include. */
#include "../atalk/msg_appletalk.c"	/* Not a typo, see logger.h */
#include "atalk.h"
#include "../util/tree.h"
#include "../if/network.h"

STRUCTURE_REF(atalkidbtype);


/*******************************************************************
 * Internal Options
 *******************************************************************/

/* When an ager runs, it will check the table it is aging for consistency */
/* and take corrective measures if problems are detected.  Bug counters */
/* are kept and available via SHOW APPLE PRIVATE. */
#define AGERS_CHECK_CONSISTENCY 1

/* Enable support for Apple's management protocol.  Currently, this feature */
/* is incomplete. */
#undef ATALK_ENABLE_SNMP		/* Disable SNMP for now */

/* Support for serial half-routers.  Currently, this feature is incomplete */
/* and must be enabled manually (not set in non-vol) and is for testing in */
/* the field. */
#define ENABLE_HALF_ROUTER 1

/* Sanity checks are always checked if this flag is enabled; otherwise, they */
/* are only checked when GLOBAL_DEBUG is undefined. */
#define ALWAYS_CHECK_SANITY 1

/* Sanity checks are quiet except via logger.  If this flag is disabled, */
/* buginf() and logging are used.  */
#define QUIET_SANITY_CHECKS 1

treeKey IntervalKey(ushort low,ushort high);


/*******************************************************************
 * Declarations that either are for use in more than one functional 
 * area or don't fit anywhere else.
 *******************************************************************/
#define ATALK_HASHLEN 701		/* AT uses a prime which is a bit */
					/* larger than NETHASHLEN. */
#define ZONELEN 32			/* max 32 chars in a zone name */

/*
 * Internal list structure
 */
typedef struct listItem_ {
    struct listItem_* next;
    boolean deleted;
    void* data;
    ushort busy;
    ulong key;
} listItem;

typedef boolean (*listProc)(listItem*,void*);

#define NEIGHBOR_PACKET_LIMIT 5
#define NEIGHBOR_QUERIES_PER_PACKET 40
#define NEIGHBOR_QUERY_SLEEP_MULTIPLIER 1
#define MAX_ZIP_GWTRIES 10
#define MAX_ZIP_RETRY_MULTIPLIER 10

/*
 * Debugging
 */
typedef struct at_idblist_ {
    idbtype *idb;
    struct at_idblist_ *next;
} at_idblist;

typedef struct atalkdebugtype_ {
    int flag;
    char *text;
    at_idblist list;
    int level;
} atalkdebugtype;

/*
** for at_err().  Usage:
**	#define	DEBUGSWITCH	mydebug
**	...
**	at_err(MODID,msg,args...);
**	...
*/
#define	MODID	__FILE__,__LINE__,DEBUGSWITCH

#undef DEBUG			/* If defined, always report significant */
				/* events etc. */

#ifndef GLOBAL_DEBUG
#undef DEBUG			/* Insure DEBUG flag is off for global */
#endif				/* debugging. */

/*
 * Default IPTalk base UDP port. 768 for pre-CAP 6.0, 200 (nominally)
 * for post CAP 6.0, but the base may be set in /etc/services.
 */
#define ATALK_IPTALK_BASEPORT_DEFAULT	768

typedef enum {
    hiprioProcess,			/* s/b at least ahead of zip,maint */
    aarpProcess,
    zipProcess,
    rtmpProcess,
    igrpProcess,
    nbpProcess,
    inputProcess,
    protocolProcess,
    maintProcess,
    macipProcess,
    aurpProcess,
    aurpProcess2,
    processIDSize			/* Must be last entry */
} processID;

typedef struct {
    mgd_timer timer;
    boolean (*proc)(void);
    uchar resched_int;
} gccontext_t;

extern pid_t atalk_pid;
extern pid_t atalk_rtmp_pid;
extern pid_t atalk_nbp_pid;
extern pid_t atalk_maint_pid;
extern pid_t atalk_zip_pid;

/*
 * To avoid touching interface.h prior to our exodus, we store the FTZ flag
 * as a GZL filter table id.
 */
#define ATFreeTradeZone (-1)

/* NET_xxxx AppleTalk extensions */
#define NET_PROXY (NET_STATIC)

#define ATALK_VALID_IDBS (IDB_SERIAL|IDB_TR|IDB_ETHER|IDB_FDDI|IDB_TUNNEL|IDB_ATM|IDB_LEX|IDB_BVI)
#define ATALK_EXTENDED_IDBS (IDB_SERIAL|IDB_TR|IDB_ETHER|IDB_FDDI|IDB_TUNNEL|IDB_ATM|IDB_LEX|IDB_BVI)
#define ATALK_NONEXTENDED_IDBS (IDB_SERIAL | IDB_ETHER | IDB_ATM | IDB_LEX | IDB_TUNNEL | IDB_BVI)

#define ATALK_PEER_IDBS (IDB_SERIAL|IDB_TUNNEL)

#ifdef ALWAYS_CHECK_SANITY
#define ATASSERT(exp) {if (!(exp)) {atalk_assert(__FILE__,__LINE__);}}
#else
#define ATASSERT(exp)
#endif

/*
 * Flags for the hwidb->atalk_compat_flags field.
 */
#define AT_COMPAT_FDDI		0x00000001
#define AT_PREFDDITALK_ALLOWED	0x00000002
#define AT_FS_DEBUG_FLAG	0x80000000

/*
 * Timing
 */
/* Interval to try to bringup an AT interface */
#define ATALK_RESTART_FREQ (2*ONEMIN)
/* Interval between state transitions */
#define ATALK_STATE_FREQ (2*ONESEC)
/* Interval and count for config queries */
#define ATALK_CONFIG_FREQ (10*ONESEC)
#define ATALK_CONFIG_RETRIES (4)
/* Maximum state tries without a state change */
#define ATALK_STATE_RETRIES (20)



/*******************************************************************
 * AppleTalk Addressing
 *******************************************************************/

#define ATALK_ADDR_MASK 0xffffff
#define ATALK_IFADDR(inum,addr) ((ataddrtype)((addr & ATALK_ADDR_MASK) \
					      |(inum << 24)))
#define ATALK_HOST_MASK 0xff
#define ATALK_NET_MASK 0xffff
#define ATALKNODE(addr) ((ataddrtype)(addr) & ATALK_HOST_MASK)
#define ATALKNET(addr) (((ataddrtype)(addr) >> 8) && ATALK_NET_MASK)

#define ATALK_BROAD_ADDR 0xff		/* LAP broadcast address (all 1's) */
#define ATALK_ANY_ROUTER 0x00		/* LAP any router address (all 0's) */
#define ATALK_ETHER_BADADDR 0xfe	/* LAP bad ATp2 ethernet address */
#define ATALK_UNICAST 0x0000		/* UNIcast destination */
#define ATALK_ALL_MULTICAST 0xffffffff	/* Broadcast */
#define ATALK_SMRP_MULTICAST 0xfffffffe /* SMRP Multicast */
#define MAX_ETHER_APPLE_MC 253
#define MAX_TR_APPLE_MC 19
#define ATALK_MARSHAL_ADDR 254		/* Pseudo-address of marshal */

#define ATALK_NET_UNKNOWN 0x0000	/* No net known */
#define ATALK_CABLE_ALLNETS 0x0000	/* Broadcast to all nets on cable */
#define ATALK_NET_UNNUMBERED 0xffff	/* Half router */
#define ATALK_STARTUP_START 0xff00	/* Startup cable range */
#define ATALK_STARTUP_END 0xfffe	/* Startup cable range */
#define ATALK_RANGE_UNKNOWN 0xfffe	/* No known cable range */

typedef ipaddrtype ataddrtype;

/*
 * ataddress -- the structure used for "fully qualified addresses"
 *
 * In our implementation of AppleTalk, we have two types of AppleTalk
 * addresses: fully qualified and nominal.
 *
 * A "fully qualified" address is one which specifies the network number,
 * node number and socket number of a service on an AppleTalk internet.
 *
 * A "nominal" address is simply the network number and node number.
 */
typedef struct ataddress_ {
    ushort net;				/* Network number */
    uchar node;				/* Node number */
    uchar spec;				/* idb index/socket/zero */
} ataddress;

typedef union ATAddress_ {
    ataddress f;
    ataddrtype l;
} ATAddress, *ATAddrPtr;


/*******************************************************************
 * AppleTalk idb subblock 
 *******************************************************************/
extern queuetype *atalk_idbQ;

#define FOR_ALL_ATIDBS(idb) \
    for ((idb) = atalk_idbQ->qhead; (idb); (idb) = (idb)->next)

static inline atalkidbtype *
atalk_getidb (struct idbtype_ *idb)
{
    return idb_get_swsb(idb, SWIDB_SB_APPLE);
}

struct atalkidbtype_ {
    struct atalkidbtype_ *
	          next;
    struct atalkidbtype_ *
                  next_deleted;
    idbtype *     phys_idb;	/* Pointer back to physical idb */

    /* TRUE if AppleTalk has been enabled */
    tinybool      atalk_enabled;

    /* TRUE if AppleTalk is operational */
    tinybool      atalk_operational;

    /* TRUE if Discovery mode is set */
    tinybool      atalk_discovery;

    /* TRUE if idb->reset() is done */
    tinybool      atalk_ifinit;

    /* TRUE if Phase 1 style RTMP packets should be used
     * Indicates presence of Phase 1 neighbor
     */
    tinybool      atalk_compatibility;

    /* TRUE if we want to send routing updates */
    tinybool      atalk_sendroutes;

    /* TRUE if we should glean data-link layer addresses from incoming
     * packets
     */
    tinybool      atalk_glean;
    
    /* FALSE if split horizon disabled */
    tinybool      atalk_rtmp_split_horizon;
    
    /* TRUE if we should send only empty RTMP packets */
    tinybool      atalk_rtmp_stub;

    /* TRUE if we are a client interface */
    tinybool      atalk_client_mode;

    /* TRUE if we allow client PPP connections */
    tinybool      atalk_allow_clients;
    
    /* != 0 if we're configured for standby */
    ushort        atalk_rtmp_standby;
    
    /* how many secs ago we heard any RTMP neighbor */
    sys_timestamp atalk_rtmp_sb_lasthrd;
    
    /* Pointer to our path entry */
    struct atalk_path_ *
	          atalk_path;
    
    /* Handler for AARP address probe routine */
    boolean (*atalk_probe)(ataddrtype, atalkidbtype *);

    /* Handler for AARP table lookup */
    struct aarpentry_ *(*atalk_lookup)(ataddrtype, atalkidbtype *, paktype *);

    /* Handler for periodic processing */
    void (*atalk_periodicif)(idbtype *); 
                                         
    /* Timer for interface's state machine  */
    sys_timestamp atalk_timer;
    
    /* Retry counter for interface's state machine
     * -1 disables state machine
     */
    int           atalk_counter;

    /* Timeout period for AARP entries in seconds */
    ulong         atalk_aarp_timeout;
    
    /* Encapsulation to use for AppleTalk */
    int           atalk_enctype;
    
    /* Access list to use for GetZoneList filtering */
    short         atalk_gzlfilter;

    /* Access list to use for access group */
    int           atalk_accesslist;

    /* Access list to use for outgoing distribution lists */
    short         atalk_route_filter;

    /* Access list to use for incoming distribution lists */
    short         atalk_route_filter_in;

    /* Access list to use for ZIP Reply filtering */
    int           atalk_zipreply_filter;

    /* Administrative distance */
    ulong         atalk_distance;

    /* State of interface */
    int           atalk_status;

    /* Start of cable range */
    ushort        atalk_cablestart;

    /* End of cable range, 0 = phase 1 */
    ushort        atalk_cableend;
    
    /* Address of config/verify host */
    ataddrtype    atalk_confaddr; 	

    /* Default GW (last RTMP sender) for this port */
    ataddrtype    atalk_defaultgw;
    
    /* Which routing protocols are in use */
    uchar         atalk_rp;
    
    /* 8-bit network node */
    uchar         atalknode;

    /* 16-bit network number */
    ushort        atalknet;

    /* List of zones for interface */
    struct zipifentry_ *
	          zonelist;

    /* Our node name as ascii */
    char *        atalk_nodename;

    /* Our net name as ascii */
    char *        atalk_netname;

    /* Zone multicast address */
    uchar         atalk_zone_multicast[6];

    /* Count of zones for interface */
    ushort        atalk_zonecount;

    /* encaps to use for Apple AARP */
    int           aarp_enctype;

    /* state of address assignment */
    int           aarp_tentative;

    /* AT domain number, 0 if no domains */
    int           atalk_domain;

    /* domain info */
    struct at_Domain_ *
	          atalk_dminfo;
};

static inline boolean atalkif_net_extended(atalkidbtype* idb)
{
    return ((idb->atalk_cableend != 0)||
	    (idb->atalk_cablestart == ATALK_NET_UNNUMBERED));
}

static inline ushort atalk_netonly(ataddrtype addr) 
	{ return (ushort)(addr >> 8); }
static inline uchar atalk_nodeonly(ataddrtype addr) 
	{ return (uchar)(addr & 0xff); }

/*******************************************************************
 * LAP
 *******************************************************************/
#define ALAPBYTES 1			/* 1 byte addresses with ALAP  */
				
#define lapENQ 0x81			/* enquiry control */
#define lapACK 0x82			/* acknowledgement for lapENQ */
#define lapRTS 0x84			/* Request-to-Send */
#define lapCTS 0x85			/* Clear-to-Send (resp. to lapRTS) */

#define ALAP_DATA_MIN 0x01		/* ALAP data frame types go fm this */
#define ALAP_DATA_MAX 0x7f		/* to this */
#define ALAP_DDP_SHORT 0x01		/* short form DDP packet header */
#define ALAP_DDP_LONG  0x02		/* long form DDP packet header */
#define ALAP_MIN_FRAME_SIZE 3		/* smallest poss LAP frame (hdr */
					/* only) */
#define ALAP_MAX_DATA_SIZE 600		/* <= this many bytes of data in */
					/* ALAP frame */
#define ALAP_MAX_FRAME_SIZE 605		/* largest LAP frame including FCS */

#define LLAP_ENCAPBYTES 3
#define LLAP_PAKTYPE 2
#define LLAP_DSTNODE 0
#define LLAP_SRCNODE 1


/*******************************************************************
 * AARP
 *******************************************************************/

/*
 * AARP 'states' on an interface.
 */
typedef enum {
    AARP_ADDR_UNKNOWN,
    AARP_ADDR_INVALID,
    AARP_ADDR_FAILED,
    AARP_ADDR_TENTATIVE,
    AARP_ADDR_SET
} aarpTypes;


/*******************************************************************
 * Datagram Delivery Protocol (DDP)
 *******************************************************************/

#define DDP_MAX_DATABYTES 586		/* maximum bytes of data in DDP */
					/* packet */

#define DDP_MAX_HOPCOUNT 15		/* hop count cannot exceed this  */
#define DDP_POISON_HOPCOUNT 31		/* hop count for notify neighbor */
#define DDPNETBYTES 2			/* size of long form network address */
#define DDPNODEBYTES 1			/* size of long form node address */
#define DDPCKSUMBYTES 2			/* size of long form checksum */

#define DDPLENGTHMASK 0x03ff
#define DDPLENGTHSHIFT 0
#define DDPHOPMASK 0x3c00
#define DDPHOPSHIFT 10

/* 
 * Short DDP packet.
 */
#define DDPS_OVERHEADBYTES (2+1+1+1)	/* header bytes in short DDP pkt  */

typedef struct ddpshort_ {
  uchar length[2];			/* length field */
  uchar dsock;				/* destination socket number */
  uchar ssock;				/* source socket number */
  uchar ddppt;				/* DDP protocol type */
#if DEAD
  uchar ddp_data[0];			/* variable length data starts here */
#endif
} ddpshort;

/*
 * Long DDP packet.
 */
#define DDP_OVERHEADBYTES (2+2+DDPNETBYTES*2+DDPNODEBYTES*2+1+1+1)

typedef struct ddplong_ {
  uchar length[2];			/* really as defined above */
  uchar cksum[DDPCKSUMBYTES];		/* DDP checksum */
  uchar dnet[DDPNETBYTES];		/* destination network number */
  uchar snet[DDPNETBYTES];		/* source network number */
  uchar dnode/*[DDPNODEBYTES]*/;	/* destination node number */
  uchar snode/*[DDPNODEBYTES]*/;	/* source node number */
  uchar dsock;				/* destination socket number */
  uchar ssock;				/* source socket number */
  uchar ddppt;				/* DDP protocol type */
#if DEAD
  uchar ddp_data[0];			/* variable length data starts here */
#endif
} ddplong;

/* general purpose DDP definition */
typedef union ddptype_ {
    struct ddpshort_ s;
    struct ddplong_ l;
} ddptype;


/*******************************************************************
 * Echo Protocol (AEP)
 *******************************************************************/

#define DDPECHOBYTES 1			/* size of required data */
#define DDPECHODATABYTES 1+1+4+4	/* size of our echo data */
					/* lap+lf ddp+echo with no data */
#define ATALK_MINECHOSIZE (DDPECHODATABYTES)
#define ATALK_MAXECHOSIZE (DDP_MAX_DATABYTES)

typedef struct ddpecho_ {
  uchar echofunc;		/* DDP_ECHO_{REQUEST,REPLY,TRACE,TREPLY}  */
  ulong PACKED(echoseq);	/* echo seq number */
  uchar data[1];		/* variable length data */
} ddpecho;  

/*
 * data handed around system for outgoing ping/echo packets 
 */
typedef struct atalkechotype_ {
    struct atalkechotype_ *next;
    tt_soc *tty;
    ulong active;
    sys_timestamp timeout;
    ulong seqnum;
    ulong responder;
    ulong deltatime;
    ushort hops;
    uchar how;
} atalkechotype;


/*******************************************************************
 * Routing
 *******************************************************************/

/*
 * AppleTalk routing protocol bits.
 * These are enabled in a bitmask to indicate which routing protocol(s) are
 * enabled/running. Used in the idb->atalk_rp short.
 */
#define	ATALK_RTMP_ENABLED	0x01
#define ATALK_IGRP_ENABLED	0x02
#define	ATALK_AURP_ENABLED	0x04
/* IGRP configured, but not necessarily enabled... */
#define ATALK_IGRP_CONFIGURED	0x08

#define ATALK_REDIST_UNCONFIGURED 0x00
#define ATALK_REDIST_CONFIGURED   0x01
#define ATALK_REDIST_ENABLED      0x02

/* 
 * Routing Table Maintenance Protocol  (RTMP)
 */
#define RTMP_VALIDMULT (2)	/* Correct multipliers for timers */
#define RTMP_INVALIDMULT (3)	/* Make work correctly: apple timers 0 45 0 */

#define RTMP_UPDATETIME 	(10*ONESEC) /* Send-RTMP timer */
#define RTMP_VALIDTIME		(RTMP_VALIDMULT*RTMP_UPDATETIME)
#define RTMP_HOLDTIME		(2*RTMP_VALIDTIME) /* minimum holddown */
#define RTMP_INVALIDTIME	(RTMP_INVALIDMULT*RTMP_VALIDTIME) /* purge */

#define RTMP_P2VERSION 0x82		/* RTMP phase 2 version */
#define RTMP_CMD_NORMAL 1		/* RTMP cmd - Normal updates */
#define RTMP_CMD_SPLIT 2		/* RTMP cmd - Split horizon */
#define RTMP_CMD_ENTIRE 3		/* RTMP cmd - Entire table */

#define ZIP_VERIFY_QCOUNT 2
#define ZIP_VERIFY_QTIME (10*ONESEC)

#define RTMPReq_NetRequest 1		/* RTMP Request Functions */
#define RTMPReq_SplitTable 2
#define RTMPReq_FullTable 3
#define RTMPREQBYTES 1			/* size of an RTMP request */

typedef struct rtmpreq_ {
  uchar cmd;				/* should be RTMPReq_XXX */
} rtmpreq;

typedef struct rtmprsp_ {
  uchar srcnet[DDPNETBYTES];		/* sender's network number */
					/* next two fields form Node ID of */
					/* bridge sending out RTMP packet */
  uchar idlen;				/* ID length */
  uchar id[1];				/* variable length, see idlen */
} rtmprsp;

typedef struct rtmpdata_ {
  uchar srcnet[DDPNETBYTES];		/* sender's network number */
					/* next two fields form Node ID of */
					/* bridge sending out RTMP packet */
  uchar idlen;				/* ID length */
  uchar id[1];				/* variable length, see idlen */
} rtmpdata;

#define RTMPHDRBYTES (DDPNETBYTES + 1 + 1) /* size of RTMP header */


/* any number of these follow RTMPdata packet header */

typedef struct rtmptuple_ {
  uchar net[DDPNETBYTES];		/* network number */
  uchar dist;				/* distance */
} rtmptuple;
#define RTMPTUPLEBYTES (2+1)		/* sizeof doesn't work here */
#define RTMPTUPLEVERSION 0x82
#define RTMPTUPLEDIST 0x0F
#define RTMPTUPLEPOISON 0x1F
#define RTMPTUPLERANGE 0x80

typedef struct rtmpxrsp_ {
    struct rtmprsp_ hdr;
    struct rtmptuple_ netstart;
    /*struct rtmptuple_ netend;  */	/* alignment problems again... */
} rtmpxrsp;

#define RTMPXRSPBYTES (RTMPHDRBYTES + RTMPTUPLEBYTES + RTMPTUPLEBYTES)
#define RTMPRSPBYTES (RTMPHDRBYTES)

#define MAX_ATNET_NAME	64
#define MAX_ATNODE_NAME 64
#define MAX_ATZONE_NAME ((ZONELEN*3)+1)	/* max size if all chars special */

#define AT_IGRP_HOLDDOWN 60             /* allow 60 secs */


/*
 * The following path types are used for both paths and metric structs.
 * This list is in order or our preference for using paths. ie, a connected
 * path will be used before a proxy path, both of those will be used before
 * either IGRP or RTMP; IGRP will be preferred over RTMP.
 */
typedef enum {
    AT_PATH_MIN=0,
    AT_PATH_DUMMY,
    AT_PATH_CONNECTED,
    AT_PATH_PROXY,
    AT_PATH_STATIC,
    AT_PATH_IGRP,
    AT_PATH_RTMP,
    AT_PATH_DRP,
    AT_PATH_AURP,
    AT_PATH_MAX
} at_path_t;

/*
 * The composite metric info for all protocols is represented by
 * the following union.
 */

/* Composite metric for connected paths/routes. */
#define AT_METRIC_CONNECTED	1
#define AT_METRIC_PROXY		2
#define AT_METRIC_STATIC        3
#define AT_METRIC_FLOATING      4

typedef struct atalk_metric_ {
    at_path_t	type;			/* type of composite metric */
    ulong	metric;			/* mapped metric for comparison */
    /*
     * The composite metric for IGRP routes.
     */
    union {
	struct {
	    ushort	hops;		/* proper # of hops */
	    ulong	bandwidth;	/* as defined in the IGRP tables */
	    ulong	delay;		/* but is this ever used? */
	    ulong	reliability;	/* how is this measured? */
	    ulong	mtu;		/* mtu for this channel */
	    ulong	load;		/* channel occupancy */
	    boolean	external;	/* T is this is an external path */
	    ataddrtype	ext_source;	/* source of external path */
	    ushort	ext_hops;	/* external hop count */
	    ushort	ext_proto_id;
	    ushort	ext_flag;
	} igrp;
	/*
	 * The metric for RTMP is simply hops... altho we could add more
	 * if we are really masochistic.
	 * NOTE: this ushort must overlay the IGRP ushort for hopcount.
	 */
	struct {
	    ushort	hops;
	} rtmp;
    } u;
} atalk_metric_t;


/*
 * State (condition) of a path and/or route. Use at_state_invalid for 0
 */
typedef enum {
    at_state_invalid=0,
    at_state_good,			/* good path, in use */
    at_state_suspect,			/* we're shy an update or two */
    at_state_bad,			/* path should now be in holddown */
    at_state_bad1,			/* about to be deleted */
    at_state_poisoned,			/* being held for poison interval */
    at_state_delete			/* poison done, path being removed */
} at_state_t;

/*
 * list of actions which can be taken on a path when we
 * are queuing a path for re-distribution.
 */
typedef enum {
    at_none_path=1,
    at_add_path,			/* path was added */
    at_change_path,			/* path's metric/interface change */
    at_delete_path			/* path was deleted */
} at_path_action_t;

/*
 * path usage ordinal.
 */
typedef enum {
    at_path_use=1,			/* when offered, use this path */
    at_path_unuse,			/* we no longer need this path */
    at_path_reject			/* we're rejecting this path */
} at_path_use_t;

/*
 * States of a routing entry.
 */
typedef enum {
    at_none_route=10,
    at_add_route,			/* path was added */
    at_change_route,			/* path's metric/interface change */
    at_notify0_route,			/* send first notification */
    at_notify1_route,			/* send second notification */
    at_delete_route			/* notifications sent, delete it */
} at_route_action_t;


/*
 * Path descriptor block.
 *
 * One of these path descriptor blocks will be created whenever we
 * learn of a new, unique path to a destination network.
 */

#define PDB_MACRO \
    struct atalk_route_   *rdb;	/* point to parent rdb */ \
    atalkidbtype          *idb;	/* the interface by which learned */ \
    struct neighborentry_ *gwptr; /* pointer to neighbor */ \
    ataddrtype             gateway; /* next hop g/w address */ \
    atalk_metric_t         metric; /* composite metric */ \
    at_path_t              type; /* which RP gave us this path */ \
    at_state_t             state; /* state of this path */ \
    dqueue_t               dqLink; /* doubly-linked list struct */

typedef struct atalk_path_ {
    treeLink	link;			/* link of like paths */
    rbTree	*tree;			/* tree which holds this node */

    PDB_MACRO

    ushort	dstrngstr;		/* start of network */ 
    ushort	dstrngend;		/* end of network range */ 
    struct      atalk_path_  *equalpath; /* nxt pdb in list of equal pdb's */ 
    boolean	(*path_action)(struct atalk_path_ *, at_state_t);  
    boolean     (*path_use)(struct atalk_path_ *, at_path_use_t);
    sys_timestamp timer;	        /* expiry timer */ 
    sys_timestamp started;		/* time this path was learned */ 
    sys_timestamp wentbad;		/* time we went into holddown */ 
    ulong	holddown;		/* holddown timer */ 
    boolean	snapshot_no_age; 	/* If TRUE, don't age this path */
    boolean	internal;		/* path to internal network? */ \
    at_path_action_t action;		/* add/change/delete */ 

    ushort	flags;			/* path flags */

#define AT_ROUTE_LEAK_RTMP	0x0001	/* needs to be leaked into RTMP */
#define AT_ROUTE_LEAKED_RTMP	0x0002	/* Has been leaked into RTMP */
#define AT_ROUTE_UNLEAK_RTMP	0x0004  /* Unleak this route? */
#define AT_ROUTE_LEAK_IGRP	0x0008	/* needs to be leaked into IGRP */
#define AT_ROUTE_LEAKED_IGRP	0x0010  /* Has been leaked into IGRP */
#define AT_ROUTE_UNLEAK_IGRP	0x0020  /* Unleak this route? */
#define AT_ROUTE_LEAK_STATIC	0x0040	/* needs to be leaked into STATIC */
#define AT_ROUTE_LEAKED_STATIC	0x0080  /* Has been leaked into STATIC */
#define AT_ROUTE_UNLEAK_STATIC	0x0100  /* Unleak this route? */
#define AT_ROUTE_LEAK_AURP	0x0200  /* needs to be leaked into AURP */
#define AT_ROUTE_LEAKED_AURP	0x0400  /* Has been leaked into AURP */
#define AT_ROUTE_UNLEAK_AURP	0x0800	/* Unleak this route? */
#define AT_ROUTE_LEAK_ANY (AT_ROUTE_LEAK_RTMP | AT_ROUTE_LEAK_IGRP | \
			   AT_ROUTE_LEAK_STATIC | AT_ROUTE_LEAK_AURP)
#define AT_ROUTE_LEAKED_ANY (AT_ROUTE_LEAKED_RTMP | AT_ROUTE_LEAKED_IGRP | \
			     AT_ROUTE_LEAKED_STATIC | AT_ROUTE_LEAKED_AURP)
#define AT_ROUTE_UNLEAK_ANY (AT_ROUTE_UNLEAK_RTMP | AT_ROUTE_UNLEAK_IGRP | \
			     AT_ROUTE_UNLEAK_STATIC | AT_ROUTE_UNLEAK_AURP)

    ulong	uses;
} atalk_pdb_t;

/*
 * Route descriptor block.
 *
 * This is the exclusive property of the at_route.c module.
 */
typedef struct atalk_route_ {
    struct	treeLink_	link;	/* rbtree stuff */
    rbTree	*tree;			/* the tree where this node lives */
    struct atalk_route_ *znext;		/* zip private list */
    struct atalk_route_ *free;		/* freelist link */
    struct listItem_ maintLink;		/* maintenance link item */

    ushort	dstrngstr;		/* dest. net# range start */
    ushort	dstrngend;		/* dest. net# range end, 0==phI */
    int         domain;                 /* AT domain that this route is in */
    ushort      domain_str;             /* domain dest. start if remapped */
    ushort      domain_end;             /* domain dest. end if remapped */ 

    ulong	holddown;		/* != 0 means we're in holddown */

    ushort	flags;			/* configuration flags */
    ulong	uses;			/* number of packets out this route */
    boolean	changed;		/* route has changed since last send */
    at_route_action_t action;		/* add/change/delete */

    sys_timestamp started;		/* time route was installed */
    sys_timestamp wentbad;		/* clock of when this route went bad */
    sys_timestamp timer;	        /* expiry timer */

    /* All that useless baggage of zones... ;-) */
    boolean	zoneupdate;		/* zip being updated if set */
    ushort	zonecnt;		/* count of zones expected, 0==unk*/
    ushort	zoneretries;		/* incr'd after every zone request */
    ataddrtype  zoneaddr;		/* addr of node providing zone info */
    sys_timestamp zonetimer;		/* time for awaiting updates */
    struct	zipnetentry_ *zonelist;	/* zone list for dstnet */
    
    /* The list of paths to the destination net follows */
    ushort	pathcount;		/* # of primary paths currently on list */
    short       rtmp_equalcount;        /* # of secondary equal cost paths given by rtmp*/
    short       eigrp_equalcount;       /* # of secondary equal cost paths given by eigrp*/

    struct      atalk_path_ *next_equalpath; /* next equal cost path to be used */ 
    struct	atalk_path_ *best_path;
    struct {
	PDB_MACRO
    } rpath;			/* route's path */
} atalk_rdb_t; 


/*
 * Neighbor status.
 * Note that to IGRP, the reachability of a neighbor is either "up" or "down",
 * DUAL worries about when a neighbor is "overdue" and simply informs us when
 * the neighbor is no longer considered a feasilble successor. At which point,
 * we should mark the neighbor down (ie, no longer reachable via IGRP)
 * and continue.
 */
typedef enum {			/* Neighbor status values */
    neighborInvalid,		/* Neighbor hasn't been configured yet */
    neighborUp,			/* Neighbor up */
    neighborOverdue,		/* Neighbor keep-alive/update overdue */
    neighborDown		/* Neighbor down */
} neighborStatus;

/*
 * Neighbor structures.
 */
typedef struct neighborentry_ {	/* Adjacent router table entry */
    treeLink link;		/* link data, must be first */
    ushort   reachable_by;	/* routing protocol(s) by which nbr is heard */

    /*
     * Routing-protocol invariant information.
     */
    ataddrtype address;		/* address of neighbor (24-bit net.node) */
    sys_timestamp started;	/* when entry was created (by either proto) */
    atalkidbtype* port;		/* port last heard from */
    sys_timestamp lastheard;	/* when router was last heard (by either) */
    sys_timestamp time_went_down;	/* time the neighbor went down */
    boolean configerror;	/* neighbor is confused */
    ulong badroutes;		/* Total # bad routes which conflict with us */
    ulong newroutes;		/* Total # of new routes */
    boolean oldcisco;		/* neighbor looks like an old cisco */
    boolean obsoleterouter;	/* TRUE if supports only ATp1 */
    neighborStatus state;	/* state of entry (Up, Overdue, Down) */
#if DEAD
    boolean nbpNameUsed;	/* Set when the nbp name is used */
#endif
    ulong flags;		/* Flags (mapping neighbor, etc) */
    boolean snapshot_no_age;    /* Don't age this neighbor */

    /* ZIP information -- routing protocol invariant. */
    ulong zipnetcount;		/* # of net queries we received */
    ulong zipqueries;		/* # of ZIP queries we received */
    ulong zipnotifies;		/* # of ZIP notifies */
    ulong zipreplies;		/* # of ZIP replies we received */
    ulong zipxreplies;		/* # of ZIP extended replies we received */
    ulong zipother;		/* # of ZIP cmds which we don't care about */
    ulong zipunknown;		/* # of ZIP cmds which we don't know */
    ulong zipobsolete;		/* obsolete ZIP commands */
    ulong qpackets;		/* # of query packets sent */
    ulong qreplies;		/* # of query replies sent */
    ulong qxreplies;		/* # of	 query extended replies sent */
    sys_timestamp qtimer;	/* time last queried */
    ulong qcount;		/* # of nets queried */

    sys_timestamp restarted;	/* when neighbor last restarted */
    ulong restarts;		/* count state transitions */

    /* RTMP statistics */
    sys_timestamp rtmp_firstheard; /* When we first heard this nbr via RTMP */
    sys_timestamp rtmp_lastheard; /* Last time we heard an RTMP packet */
    sys_timestamp rtmp_exptimer; /* RTMP reachability expiration timer */
    neighborStatus rtmp_state;	/* state of entry (Up, Overdue, Down) */
    ulong rtpacketsok;		/* # of routing updates accepted */
    ulong rtpacketsbad;		/* # of routing updates discarded */
    ulong rtpacketsaborted;	/* # of routing updates partially invalid */
    ulong rtpacketsignored;     /* # of routing updates ignored by standby */
    ulong rtextendedrts;	/* # of extended routes advertised */
    ulong rtnonextendedrts;	/* # of non-extended routes advertised */
    ulong rtnewrts;		/* # of new routes advertised */
    ulong rtoldrts;		/* # of old routes advertised */
    ulong rtignoredrts;		/* # of uninteresting routes advertised */
    ulong rtbadrts;		/* # of routes which disagree with us */
    ulong rtpoisonedrts;	/* # of routes poisoned */
    sys_timestamp rtmp_restarted; /* When we last regain RTMP reachability */
    ulong rtmp_restarts;	/* # of times we have regained RTMP reach. */
    ulong rtmp_overdues;	/* count of late updates */
   
    /* IGRP statistics */
    ulong igrp_rtr_id;		/* Router ID */
    sys_timestamp igrp_firstheard; /* When we first heard this nbr via IGRP */
    sys_timestamp igrp_lastheard;  /* Last time we heard IGRP neigbor */
    neighborStatus igrp_state;	/* state of entry (Up or Down) */
    ulong igrp_hellos_heard;	/* hello packets heard from this neighbor */
    ulong igrp_hellos_sent;	/* directed hello packets sent to neighbor */
    sys_timestamp igrp_last_hello; /* time last hello heard */
    ulong igrp_queries_heard;	/* queries heard from this neighbor */
    ulong igrp_queries_sent;	/* Queries sent to this neighbor */
    sys_timestamp igrp_last_query; /* time last query was sent */
    ulong igrp_updates_heard;	/* update packets heard from neighbor */
    ulong igrp_updates_sent;	/* update packets heard from neighbor */
    ulong igrp_usndcount;
    ulong igrp_urcvcount;
    sys_timestamp igrp_last_update;
    ulong igrp_replies_heard;	/* # of replies to our queries heard */
    ulong igrp_replies_sent;	/* # of replies to our queries heard */
    sys_timestamp igrp_last_reply; /* time last reply was heard */
    ulong igrp_restarts;	/* # of times we regained IGRP reachability */
    sys_timestamp igrp_restarted; /* when we last regained IGRP reachability */
    ulong igrp_qtimer;		/* Time we last sent IGRP  query */
    ulong igrp_overdues;	/* count of late hellos */
    ulong igrp_qsndcount;	/* # of nets queried */
    ulong igrp_qrcvcount;       /* # of queries received for nets */
    ulong igrp_badrts;		/* # of bad routing metric/updates */
    ulong igrp_ignoredrts;	/* # of ignored routing metric/updates */
    ulong igrp_bogus;		/* # of bogus packets heard for */

    /* STATIC information */
    boolean static_entry;	/* Static neighbor? */
} neighborentry;


/*******************************************************************
 * NBP
 *******************************************************************/

#define NBP_BrRq       1
#define NBP_LkUp       2
#define NBP_LkUpReply  3
#define NBP_FwdReq     4

#define NBP_namelen    32

#define NBP_ZONE_WILD '*'
#define NBP_NAME_WILD '='
#define NBP_PART_WILD '\xc5'

/* NBP Length definitions */
#define NBPNAMELEN 32
#define NBPTYPELEN 32
#define NBPOBJECTLEN (NBPNAMELEN+1+NBPTYPELEN+1+ZONELEN+1)

typedef struct nbpentry_ {
    struct nbpentry_ *next;
    uchar name[NBP_namelen];
    uchar type[NBP_namelen];
    uchar zone[NBP_namelen];
    ushort net;
    uchar node;
    uchar socket;
    uchar enumerator;
} nbpentry;

/*
 * The control field below used to be two bit fields, which were not
 * portable to other processor architectures due to structure alignment 
 * constraints. [If the structure is not aligned on a 32-bit boundary,
 * the fast 32 bit manipulations generated by the compiler fail.  Found 
 * on MIPS & Orion processors.
 */

typedef struct nbptype_ {	/* DDP protocol type = 2 */
  uchar control;		/* 1st nybble = Type (NBP_BrRq, etc.)
				   2nd nybble = Tuple count */
  uchar NBPid;			/* NBP ID number */
/*  uchar data[1]; */	 	/* variable length, ntuples NBP tuples */
} nbptype;

/*
 * The following inlines allow easy access the the nbp control byte without
 * using bit-fields.  See above comment.
 */

static inline uchar nbp_get_type (nbptype *theNBP)
{
    return (theNBP->control & 0xf0) >> 4;
}

static inline void nbp_set_type (nbptype *theNBP, uchar theType)
{
    theNBP->control = (theNBP->control & 0x0f) | (theType << 4);
}

static inline uchar nbp_get_count (nbptype *theNBP)
{
    return theNBP->control & 0x0f;
}

static inline void nbp_set_count (nbptype *theNBP, uchar theSize)
{
    theNBP->control = (theNBP->control & 0xf0) | theSize;
}

#define NBP_OVERHEADBYTES 2

typedef struct nbptuple_ {
				/* next four items form entity address */
  uchar net[DDPNETBYTES];	/* network number */
  uchar node;			/* node ID */
  uchar socket;			/* socket number */
  uchar enumerator;		/* enumerator */
				/* next six items form entity name */
				/* (object,type,zone) */
/*  uchar data[1]; */
} nbptuple;  

#define NBPTUPLESIZE (DDPNETBYTES+1+1+1) /* not sizeof(nbptuple)! */


/*******************************************************************
 * ATP
 *******************************************************************/

#define ATPUSERBYTES 4

#define ATP_STS 0x08
#define ATP_EOM 0x10
#define ATP_XO  0x20
#define ATP_FUNC 0xc0
#define ATP_FUNCSHIFT 6

#define ATP_TREQ	1
#define ATP_TRESP	2
#define ATP_TREL	3

#define ATP_GET_FUNC(CCF)	(((CCF)&ATP_FUNC)>>ATP_FUNCSHIFT)
#define ATP_CLR_FUNC(CCF)	(CCF) &= ~ATP_FUNC
#define ATP_SET_FUNC(CCF,OP)\
	{ATP_CLR_FUNC(CCF); (CCF) |= (ATP_FUNC&((OP)<<ATP_FUNCSHIFT)); }

#define ATP_GET_EOM(CCF)	(((CCF)&ATP_EOM)?1:0)
#define ATP_CLR_EOM(CCF)	(CCF) &= ~ATP_EOM
#define ATP_SET_EOM(CCF)	(CCF) |= ATP_EOM

#define ATP_GET_XO(CCF)		(((CCF)&ATP_XO)?1:0)
#define ATP_CLR_XO(CCF)		(CCF) &= ~ATP_XO
#define ATP_SET_XO(CCF)		(CCF) |= ATP_XO

typedef struct atptype_ {
    uchar ccf;			/* command/control field */
    uchar bitmap;		/* bitmap/sequence no */
    uchar tidhi;		/* transaction identifier, msb */
    uchar tidlo;		/* transaction identifier, lsb */
    uchar userbytes[ATPUSERBYTES]; /* user bytes */
/*  uchar data[1]; */		/* variable length ATP data (0-578 bytes) */
} atptype;

#define ATP_OVERHEADBYTES (1+1+1+1+ATPUSERBYTES)


/*******************************************************************
 * ZIP
 *******************************************************************/

#define ZIPHASHLEN (ATALK_HASHLEN)	/* Our std buckets in zone name hash */
					/* table. */
#define ZIP_QUERYTIME RTMP_UPDATETIME /* ZIP query retransmit timer */
#define ZIP_TAKEDOWN_TIME (10*ONEMIN) /* ZIP takedown interval */
#define GZLReq1 0x40		/* GetZoneList or GetMyZone TReq magic */
#define GZLReq2 0x01		/* GetZoneList or GetMyZone Bitmap magic */
#define GZLRep1 0x90		/* GetZoneList or GetMyZone TRsp magic */
#define GZLRep2 0x00		/* GetZoneList or GetMyZone Sequence magic */
#define GMZ		7	/* GetMyZone opcode */
#define GZL		8	/* GetZoneList opcode */
#define GLZ		9	/* GetLocalZones opcode */

#define ZIP_Query		1	/* ZIP query command */
#define ZIP_Reply		2	/* ZIP reply command */
#define ZIP_Takedown		3	/* OBSOLETE: ZIP takedown command */
#define ZIP_Bringup		4	/* OBSOLETE: ZIP bringup command */
#define ZIP_GetNetInfo		5	/* ZIP GetNetInfo command */
#define ZIP_GetNetInfoReply	6	/* ZIP GetNetInfo command */
#define ZIP_Notify		7	/* ZIP Notify command */
#define ZIP_ExtendedReply	8	/* ZIP Extended reply command */

typedef struct ziptype_ {
    uchar cmd;			/* ZIP command */
    uchar cnt;			/* count of networks */
} ziptype;

typedef struct zipentry_ {
    uchar net[2];		/* network number */
    uchar len;			/* name length */
    uchar name[1];		/* variable length */
} zipentry;

#define ZIPTYPEBYTES 2
#define ZIPENTRYBYTES (2+1+1)

typedef enum NetInfoFlags_ {
    noNetFlags=0,
    zoneInvalid=0x0080,
    useBroadcast=0x0040,
    onlyOneZone=0x0020
    } NetInfoFlags;

#define ZIP_MAX_GETNETINFO DDP_MAX_DATABYTES

#define ZIP_NETINFO_HDRSIZE 6

typedef struct zipnetinfo_ {
    uchar cmd;			/* ZIP command (5=Request,6=Reply) */
    uchar flags;		/* ZIP Reply flags (Request=0) */
    uchar netstart[2];		/* ZIP Net range start (Request=0) */
    uchar netend[2];		/* ZIP Net range end (Request=0) */
    uchar zonelen;		/* ZIP Requested zone len/name (Var) */
    uchar zonename[1];
#if 0
    uchar mcastlen;		/* ZIP Reply zone multicast addr (Var) */
    uchar mcastaddr[1];
    uchar defzonelen;		/* ZIP default zone, if req zone bad (Var) */
    uchar defzonename[1];
#endif
} zipnetinfo;

typedef struct ziptabletype_ {
    struct ziptabletype_ *next;	/* pointer to next elt */
    uchar name[ZONELEN];	/* zone name (not null-terminated!) */
    char pname[MAX_ATZONE_NAME]; /* printing version of zone name */
    ushort length;		/* length of name */
    ushort hashval;		/* the raw hash value */
    boolean valid;		/* used by atalk_ZIP_gc */
    ushort ifcount;		/* number of if entries for this zone */
    ushort acccount;		/* number of access entries for this zone */
    struct zipnetentry_* netlist; /* net list for zone */
} ziptabletype;

/*
 * Zip/Network glue records
 */
typedef struct zipnetentry_ {
    boolean deleted;
    struct ziptabletype_* zone;
    struct atalk_route_* net;
    struct zipnetentry_* nextZone;
    struct zipnetentry_* prevZone;
    struct zipnetentry_* nextNet;
    struct zipnetentry_* prevNet;
} zipnetentry;

typedef struct zipifentry_ {
    struct ziptabletype_* zone;
    struct zipifentry_* next;
} zipifentry;

typedef struct zipaccentry_ {
    struct ziptabletype_* zone;
    boolean grant;
    struct zipaccentry_* next;
} zipaccentry;

/*
 * NBP Access Structure
 */
typedef struct nbpaccentry_ {
    struct nbpaccentry_ *next;
    boolean grant;     /* {permit | deny} */
    int nbpentry;      /* nbp entry type */
    int sequence;      /* nbp sequence */
    int object_len;    /* hex filtered upper case object string length */
    char *object;      /* hex filtered upper case object string */
    int  type_len;     /* hex filtered upper case type string length */
    char *type;        /* hex filtered upper case type string */
    int  zone_len;     /* hex filtered upper case zone string length */
    char *zone;        /* hex filtered upper case zone string */
    char *p_object;    /* printable object string */
    char *p_type;      /* printable type string */
    char *p_zone;      /* printable zone string */
} nbpaccentry;


/*******************************************************************
 * Traffic statistics definitions
 *******************************************************************/
enum {
    ATALK_INPUT,
    ATALK_LOCALDEST,
    ATALK_NO_HANDLER,
    ATALK_MARTIAN,
    ATALK_IGNORED,
    ATALK_BCASTIN,
    ATALK_FORWARDED,
    ATALK_BCASTOUT,
    ATALK_CKSUMERR,
    ATALK_HOPCOUNT,
    ATALK_NOACCESS,
    ATALK_NOROUTE,
    ATALK_ENCAPSFAILED,
    ATALK_NOSOURCE,
    ATALK_OUTPUT,
    ATALK_LOOPBACK,
    ATALK_INMULT,
    ATALK_RTMP_IN,
    ATALK_RTMP_OUT,
    ATALK_RTMP_REQUEST,
    ATALK_RTMP_BAD,
    ATALK_RTMP_IGNORED,
    ATALK_IGRP_IN,
    ATALK_IGRP_OUT,
    ATALK_IGRP_OUT_FAILED,
    ATALK_IGRP_HELLO_IN,
    ATALK_IGRP_REQUEST_IN,
    ATALK_IGRP_UPDATE_IN,
    ATALK_IGRP_QUERY_IN,
    ATALK_IGRP_REPLY_IN,
    ATALK_IGRP_HELLO_OUT,
    ATALK_IGRP_REQUEST_OUT,
    ATALK_IGRP_UPDATE_OUT,
    ATALK_IGRP_QUERY_OUT,
    ATALK_IGRP_REPLY_OUT,
    ATALK_IGRP_BAD,
    ATALK_IGRP_IGNORED,
    ATALK_AURP_OPEN_REQ,
    ATALK_AURP_RD,
    ATALK_AURP_RI_OUT,
    ATALK_AURP_RI_IN,
    ATALK_AURP_ZI_OUT,
    ATALK_AURP_ZI_IN,
    ATALK_AURP_GZN_OUT,
    ATALK_AURP_GZN_IN,
    ATALK_AURP_GDZL_OUT,
    ATALK_AURP_GDZL_IN,
    ATALK_AURP_BAD_SEQ,
    ATALK_NBP_IN,
    ATALK_NBP_OUT,
    ATALK_NBP_FAILED,
    ATALK_NBP_LKUP,
    ATALK_NBP_FWD,
    ATALK_NBP_PROXY,
    ATALK_NBP_BAD,
    ATALK_SNMP_IN,
    ATALK_SNMP_OUT,
    ATALK_ATP,
    ATALK_ZIP_IN,
    ATALK_ZIP_OUT,
    ATALK_ZIP_NETINFO,
    ATALK_ECHO,
    ATALK_ECHO_SENT,
    ATALK_ECHO_FAILED,
    ATALK_ECHO_REPLIES,
    ATALK_ECHO_DROPPED,
    ATALK_ECHO_TRACE_REQ,
    ATALK_ECHO_TRACE_REPLY,
    ATALK_ECHO_ILL,
    ATALK_DDP_SHORT,
    ATALK_DDP_LONG,
    ATALK_DDP_CREATED,
    ATALK_NO_BUFFERS,
    ATALK_DDP_WRONGSIZE,
    ATALK_ARP_REQUEST,
    ATALK_ARP_REPLY,
    ATALK_ARP_PROBE,
    ATALK_ARP_UNKNOWN,
    ATALK_ARP_BOGON,
    ATALK_ARP_BADENCS,
    ATALK_ARP_DELAYS,
    ATALK_ARP_DROPS,
    ATALK_ARP_FAILED,
    ATALK_ARP_SENT,
    ATALK_BCAST_NOTMINE,
    ATALK_DADDR_INVALID,
    ATALK_DADDR_IGNORED,
    ATALK_SADDR_INVALID,
    ATALK_USED_DEFGW,
    ATALK_ROUTER_BADVERS,
    ATALK_BAD_ISO2OUI,
    ATALK_UNKNOWN,
    ATALK_FASTFORWARD,
    ATALK_XFASTFORWARD,
    ATALK_NOT_OPERATIONAL,
    ATALK_DEBUGMSGS,
    ATALK_BUGS,
    AT_RESPONDER_IN,
    AT_RESPONDER_BAD_ATP,
    AT_RESPONDER_BAD_OPCODE,
    AT_RESPONDER_CREATE_ERROR,
    AT_RESPONDER_OUT,
    AT_RESPONDER_SEND_FAIL,
    MACIP_DDP_IN,
    MACIP_DDP_IP_OUT,
    MACIP_DDP_IP_FORWARD_FAIL,
    MACIP_DDP_BAD_DDP_TYPE,
    MACIP_DDP_NO_CLIENT_SERVICE,
    MACIP_DDP_CLIENT_CREATE_ERROR,
    MACIP_IP_IN,
    MACIP_IP_DDP_OUT,
    MACIP_IP_DDP_CREATE_ERROR,
    MACIP_IP_DDP_SEND_FAIL,
    MACIP_SERVER_IN,
    MACIP_SERVER_OUT,
    MACIP_SERVER_SEND_FAIL,
    MACIP_SERVER_BAD_ATP,
    MACIP_SERVER_ASSIGN_IN,
    MACIP_SERVER_ASSIGN_OUT,
    MACIP_SERVER_ASSIGN_ERROR,
    MACIP_SERVER_INFO_IN,
    MACIP_SERVER_INFO_OUT,
    MACIP_SERVER_INFO_ERROR,
    MACIP_SERVER_STATIC_IN,
    MACIP_SERVER_STATIC_OUT,
    MACIP_SERVER_STATIC_ERROR,
    MACIP_SERVER_RELEASE_IN,
    MACIP_SERVER_RELEASE_OUT,
    MACIP_SERVER_RELEASE_ERROR,
    MACIP_SERVER_BAD_OPCODE,
    ATALK_DDP_2SHORT_ERR,
    ATALK_DDP_2LONG_ERR,
    ATALK_MAXTRAFFIC		/* always last */
};



/*******************************************************************
 * Typedefs
 *******************************************************************/
typedef void (*voidproc)(void);		/* ptr to function returning void */
typedef boolean (*boolproc)(void);	/* ptr to function returning boolean */
typedef void (*routeProc)(atalk_rdb_t *);
  
typedef enum {
    netInvalid,			/* Generic error */
    netBadForCable,		/* Net is not special and not valid for cable*/
    netCableBroadcast,		/* Net is a cable broadcast */
    netStartupRange,		/* Net is in the startup range */
    netLocalNet,		/* Net is local to cable */
    netAllValid,		/* Test for valid nets (net >= netAllValid) */
    netValidForRouting,		/* Net is valid for routing */
    netValidNonExtended,	/* Net is valid but non-extended */
    netValidExtended		/* Net is valid and extended */
} netTypes;

typedef enum {
    srcIsForeign,		/* Src: Address is not associated with this */
				/*      port */
    srcIsCableSpecific,		/* Src: Address is valid for this cable only */
    srcIsCableMarshal,		/* Src: Address is marshal for this cable */
    srcOnThisCable,		/* Src: Address is valid for cable */

    dstNotForCable,		/* Dst: Address valid but is not on this */
				/*      cable. */
    dstCableBroadcast,		/* Dst: Address is broadcast for this cable. */
    dstForCableMarshal,		/* Dst: Address is for our marshal. */
    dstIsMyAddress,		/* Dst: Address is for me. */
    dstForCableRouter,		/* Dst: Address is for any router on cable. */
    dstForThisCable,		/* Dst: Address is on this cable. */
    
    addrErrorDetected,		/* Both: All codes greater than this are */
				/*       errors */
    addrIndeterminate,		/* Both: Generic error and/or protocol */
				/*       violation */
    addrIsInvalid,		/* Both: Address is not valid for AppleTalk. */
    cableUnavailable,		/* Both: AppleTalk not enabled or line down. */
    srcBadForCable		/* Src: Address is not valid for this cable */
} addrTypes;

typedef enum {
    DDP_TYPE_Invalid=0,
    DDP_TYPE_RTMPReq=5,	/* DDP protocol type for RTMP Request */
    DDP_TYPE_RTMPRsp=1,	/* DDP protocol type for RTMP Response/data */
    DDP_TYPE_NBP=2,	/* DDP protocol type for NBP packet */
    DDP_TYPE_ATP=3,	/* DDP protocol type for ATP packet */
    DDP_TYPE_ZIP=6,	/* DDP protocol type for ZIP (except */
			/* GetZoneList, GetMyZone and GZL,GMZ reply */
			/* which are DDP_TYPE_ATP */
    DDP_TYPE_ECHO=4,	/* DDP protocol type for Echo */
    DDP_TYPE_SNMP=8,	/* DDP protocol type for SNMP */
    DDP_TYPE_ADSP=7,	/* DDP protocol type for ADSP */
    DDP_TYPE_SMRP=10,   /* DDP protocol type for SMRP */
    DDP_TYPE_MACIP=22,	/* DDP protocol type for MacIP encapsulation */
    DDP_TYPE_IGRP2=88   /* DDP Protocol type for IGRP2 packets */
} ddpTypes;

typedef enum {
    DDP_ECHO_REQUEST=1,		/* AEP request */
    DDP_ECHO_REPLY=2,		/* AEP reply */
    DDP_ECHO_TRACE=64,		/* AEP traceroute request (private function)*/
    DDP_ECHO_TREPLY=65		/* AEP traceroute reply (private function)*/
} echoFunctions;

typedef enum {
    opUnknown,
    opNoAddress,		/* Unable to obtain a node address */
    opProbing,			/* Attempting to obtain a node address */
    opCableMismatch,		/* Verification failed, mismatch nv/cable */
    opConfigBad,		/* Configuration info incomplete */
    opLineProtoErr,		/* Line protocol not compatible w/ AT config */
    opAcquire,			/* Acquiring cable information, awaiting an */
				/* 	RTMP */
    opAcquirePeer,		/* Acquiring a peer */
    opAcquireZones,		/* Acquiring zone information */
    opVerify,			/* Verifying cable information */
    opVerifyZones,		/* Cable info OK - Check zones */
    opValidate,			/* Validate final port configuration */
    opNetConflict,		/* port parameters conflict */
    opNetDown,			/* line protocol down */
    opResExhausted,		/* no buffers avail to config port */
    opHRSupportErr,		/* Half-router's configured port is down */
    opRemotePeerErr,		/* Remote peer incorrectly configed */
    opInternalError,		/* Recoverable internal bug */
    opInternalBug,		/* Unrecoverable internal bug */
    opCompatBad,		/* Port violates compatibility mode */
    opRestartPort,		/* request to restart port, always valid */
    opValid
} opStatus;

/* 
 * Appletalk socket numbers
 * most Appletalk sockets are looked up via NBP, not static
 */

typedef enum {
    ATALK_INVALID_SOCKET=0,
    ATALK_MIN_SOCKET=1,			/* as per "Inside AppleTalk" */
    ATALK_SOCK_RTMP=1,			/* Routing Table Maintenance socket */
    ATALK_SOCK_NBP=2,			/* Names Information socket for NBP */
    ATALK_SOCK_ECHO=4,			/* Echo protocol socket */
    ATALK_SOCK_ZIP=6,			/* Zone Information Services */
    ATALK_SOCK_SNMP=8,			/* SNMP services */
    ATALK_SOCK_SMRP=10,                 /* SMRP services */
    ATALK_SOCK_MACIP=72,		/* MacIP routing socket */
    ATALK_SOCK_IGRP2=88,		/* AppleTalk IGRP2 socket */
    ATALK_SOCK_DYN_LOWER=128,		/* lower of dynamic range available */
    ATALK_SOCK_PING=253,		/* atalk_echo socket */
    ATALK_SOCK_OURNAME=254,		/* atalk_Responder socket*/
    ATALK_SOCK_DYN_UPPER=254,		/* upper of dynamic range available */
    ATALK_MAX_SOCKET=254		/* as per "Inside Appletalk" */
} sockTypes;

typedef boolean (*sockfunc_t)(paktype *);

typedef enum {				/* ForUs return codes */
    ATALK_PITCHIT=1,			/* drop packet on floot */
    ATALK_ROUTEIT,			/* route to next hop */
    ATALK_CONSUMEIT,			/* process in this router */
    ATALK_COPYAND_ROUTEIT,		/* make a copy for us and forward */
    ATALK_MARSHALIT			/* toss the packet to curr. marshal */
} forusTypes;


/*******************************************************************
 * at_access.c
 *******************************************************************/
void 		show_atalkaccess(int list);
void 		show_atalkaccess_shell(int list);
boolean 	atalk_accesscheck(ushort net,int list,boolean isbroadcast);
boolean 	atalk_routecheck(ushort low,ushort high,int list);
boolean 	atalk_zonecheck(ziptabletype* zone,int list);
boolean 	atalk_rtmpzonecheck(ziptabletype* zone,int list);
boolean 	atalk_gzlzonecheck(ziptabletype *zone, int list);
boolean 	atalk_sendcheck(ushort net, int list);
boolean 	atalk_priorityaccesscheck(paktype *pak, int list);
boolean		atalk_ftzPortCheck(atalkidbtype* idb);
paktype*      atalk_nbpcheck(paktype *, int prioritylist);

/*******************************************************************
 * at_arp.c
 *******************************************************************/
process         aarp_background(void);
void 		aarp_show_table(ushort, uchar);
void 		aarp_flush_cache(idbtype*);
boolean 	aarp_resolve(ataddrtype, atalkidbtype*, int, boolean);
void 		aarp_fix(void);
boolean 	aarp_probe(ataddrtype, atalkidbtype*);
void 		aarp_init(void);
void 		atalk_ifarp(atalkidbtype*, ulong);
boolean 	aarp_pick_addr(atalkidbtype*);
void 		flush_aarp(atalkidbtype*);
void 		nv_atalk_arp(void);
void 		aarp_StatusReport(void);
void 		atalk_show_pvt_aarp(ataddrtype);
struct aarpentry_ *
                aarp_lookup(ataddrtype, atalkidbtype *, paktype *);
void    	atfast_SetupIF(atalkidbtype *);
void            aarp_remove(ipaddrtype, atalkidbtype *);
void            flush_all_aarp(ushort net);
void            aarp_ager(void);
void 		aarp_enqueue(paktype*);
struct aarpentry_ *
                aarp_FindEntry(ataddrtype, atalkidbtype *);
struct aarpentry_ *	
                aarp_GetNextEntry(struct aarpentry_ *);

/*******************************************************************
 * at_aurp.c
 *******************************************************************/

/*******************************************************************
 * at_chain.c
 *******************************************************************/
extern void atalk_parser_init(void);

/*******************************************************************
 * at_debug.c
 *******************************************************************/
#include "../atalk/at_debug.h"

/*******************************************************************
 * at_echo.c
 *******************************************************************/
boolean 	atalk_get_sweep(ulong*,ulong*,ulong*);
void 		atalkblast(ushort,uchar*);
void 		atalk_echo(paktype*);
void 		atalk_echo_rcvd(paktype*);
void 		atalk_PingHandler(paktype*);
void 		atalk_EchoHandler(paktype*);
void 		atalk_Responder(paktype*);
boolean         atalk_ping_parse(char *, int, int *, char **, boolean *);
void            atalk_ping(char *);
void 		register_snmp_atalk_echo(void);

/*******************************************************************
 * at_fast.c
 *******************************************************************/
void 		atalk_UpdateCache(paktype* pak);
void 		atalk_InvalidateCache(char* rn);
void 		atalk_ReinitIF(atalkidbtype*);
void            atalk_fastsetup(hwidbtype *hwidb);
void            atalk_fscache_age(void);
void            atalk_fscache_flush(void);
void		atfast_show_pvt_cache(ataddrtype);

/*******************************************************************
 * at_globals.c
 *******************************************************************/
extern void 	atalk_init(subsystype *);
#include "../atalk/at_globals.h"

/*******************************************************************
 * at_input.c
 *******************************************************************/
ushort 		atalk_compute_crc(paktype *);
void 		atalk_DDPcksum(paktype *);
boolean		atalk_check(paktype *pak, ddpTypes myddp, int errstat, 
			    boolean ignore_if);
void 		atalk_dispose(int,paktype *);
void 		atalk_enqueue(paktype* pak);
idbtype * 	atalk_findidb(ushort);
void 		atalk_forward(paktype *);
void 		atalk_input(void);
boolean 	atalk_send(paktype *);
boolean 	atalk_send1(paktype *, long);
boolean 	atalk_valid_DDPcksum(paktype *);
long 		llap_encapsulate(paktype* pak,int len,ulong address);
long 		llap_vencapsulate(paktype* pak,ulong address);
void 		etalk_enqueue(paktype*);

/*******************************************************************
 * at_iptalk.c
 *******************************************************************/
void iptalk_init(void);
idbtype *iptalk_createidb(idbtype *);

/*******************************************************************
 * at_maint.c
 *******************************************************************/
gccontext_t    *atmaint_AddGCContext(boolean (*)(void), int);
void		atmaint_ResetRequestInfo(atalk_rdb_t *);
atalk_rdb_t *	atmaint_GetFirstRequest(void);
atalk_rdb_t *	atmaint_GetNextRequest(atalk_rdb_t *);
atalk_rdb_t *   atmaint_NextNeighborZipQuery(neighborentry *, atalk_rdb_t *);
void 		atmaint_Initialize(void);
void 		atalkif_CableInfoReport(atalkidbtype*,ushort,ushort,
					ataddrtype);
void		atmaint_CancelZoneRequest(atalk_rdb_t *);
void 		atalkif_ConfigError(atalkidbtype*,char*);
void		atalkif_DisablePort(atalkidbtype *);
neighborentry * atmaint_CreateNeighbor(ataddrtype, atalkidbtype*);
neighborentry * atmaint_FindNeighbor(ataddrtype, atalkidbtype*);
neighborentry * atmaint_GetNeighbor(ataddrtype, atalkidbtype*,boolean);
neighborentry * atmaint_GetNextNeighbor(neighborentry*);
void 		atmaint_MarkNeighborObsolete(neighborentry*);    
void 		atmaint_NeighborConfigBad(neighborentry*);       
void 		atmaint_ReleaseNeighbor(neighborentry*);  
void            atmaint_ScheduleGCRun(gccontext_t *);
void 		atmaint_RestartNeighbor(neighborentry *, sys_timestamp); 
void 		atalkif_RestartPort(atalkidbtype* idb);
void 		atmaint_ScheduleZoneRequest(atalk_rdb_t*);
void		atmaint_ScheduleZoneRequestShell(atalk_rdb_t *, listItem **);
void 		atmaint_ProtectNeighbor(neighborentry*);  
void 		atmaint_UnprotectNeighbor(neighborentry*);
void 		atalkif_RouteZonesReady(atalkidbtype*);
void 		atalkif_RouterProbe(atalkidbtype*,ataddrtype);
void 		atalkif_PrimaryConfirmed(atalkidbtype*);
void 		atalkif_NetConfirmed(atalkidbtype*);
void 		atalk_maint(void);
char * 		atmaint_GetNodeName(ataddrtype);
neighborentry * atmaint_FindMatchingNeighbor(ataddrtype, atalkidbtype*);
char * 		atmaint_NeighborState(neighborStatus);
boolean         atmaint_NeighborIsReachable(neighborentry *);
ATAddress 	atmaint_FirstRegisteredKey(neighborentry* n,ulong *idx);
ATAddress 	atmaint_NextRegisteredKey(ATAddress key,ulong *idx);
char * 		atmaint_GetRegisteredName(ATAddress key,ulong idx);
void 		atmaint_ShowLookupList(void);
void 		atmaint_StatusReport(void);               
void 		atmaint_ShowLookupCache(void);
void 		atmaint_WriteNV(char *);                  
void 		atmaint_ScheduleIFWakeup(atalkidbtype* idb);
void 		atmaint_ClearNameCache(void);
void 		atmaint_AddLookupType(int, uchar*);
void 		atmaint_DeleteLookupType(int, uchar*);
boolean		atmaint_ZonesMatch(atalkidbtype *, atalk_rdb_t *);
void 		atmaint_DeleteNeighbor(neighborentry *);
void		atmaint_ScheduleGCNow(void);
void		atmaint_FreeAllNeighbors(void);
void            atmaint_start_namelookup_timer(void);
void            atmaint_stop_namelookup_timer(void);
void            atmaint_update_nbprequest_timer(void);

/*******************************************************************
 * at_media.c
 *******************************************************************/
void            atalk_media_registry(void);

/*******************************************************************
 * at_nbp.c
 *******************************************************************/
uchar * 	atnbp_print_tuple(nbptuple*);
void 		atnbp_print_pkt(nbptype*);
void 		atalk_nbp(void);
boolean 	atnbp_namematch(uchar* s1,uchar* s2,int len1,int len2,uchar);
void		nbp_test(void);

/*******************************************************************
 * at_parse.c
 *******************************************************************/
void  	        atalkif_clear(idbtype*);
char * 		atalkif_ErrForExtended(atalkidbtype*);
char * 		atalkif_ErrForNonExtended(atalkidbtype*);
char * 		atalkif_ErrForPeers(atalkidbtype*);
boolean 	atalk_parse_address(char*,ushort*,uchar*,uchar*);
char * 		atalkif_ErrForDiscovery(atalkidbtype *);
boolean 	atalkif_InternetCompatible(atalkidbtype *);
boolean		atalkif_InterfaceCompatible(atalkidbtype *);
void 		atalkif_shutdown(atalkidbtype *);
boolean 	atalkif_setup(atalkidbtype*,ushort,ushort,ushort,uchar,
			      boolean);
int 		encode_zonename(char*,uchar*);
void		atalk_clear_neighbor(ataddrtype, boolean, boolean);
void            atalk_network_fixup(idbtype*, int);

/*******************************************************************
 * at_rtmp.c
 *******************************************************************/
#include "../atalk/at_rtmp.h"

/*******************************************************************
 * at_route.c
 *******************************************************************/
void 		atalk_route_adjust(idbtype *);
#include "../atalk/at_route.h"

/*******************************************************************
 * at_show.c
 *******************************************************************/
#include "../atalk/at_show.h"

/*******************************************************************
 * at_snmp.c
 *******************************************************************/
void		atalk_snmp_register(void);
void		atsnmp_Initialize(void);

/*******************************************************************
 * at_trace.c
 *******************************************************************/
void 		atalk_trace(char*);

/*******************************************************************
 * at_util.c
 *******************************************************************/
zipifentry* 	atalkif_firstZone(atalkidbtype*);
zipifentry* 	atalkif_nextZone(zipifentry*);
char * 		atalkif_PrimaryZoneName(atalkidbtype*);
ziptabletype * 	atalk_PrimaryZone(atalk_rdb_t*);
boolean		atalk_idb_hwaddr(idbtype *, uchar *, ushort *);
void    	atalkif_SetAarpTimeout(atalkidbtype *, ulong);
ulong		atalkif_ArpTimeout(atalkidbtype *);
boolean         atalkif_IPTalkTunnel(atalkidbtype *);
boolean 	atalkif_CaymanTunnel(atalkidbtype *);
boolean         atalkif_AURPTunnel(atalkidbtype *);
boolean		atalkif_ForAURP(idbtype *);
ataddrtype 	atalk_pak_srcaddr(paktype*);
ataddrtype 	atalk_pak_dstaddr(paktype*);
boolean 	atalk_on_idb(idbtype *idb);
uchar * 	atalk_broadcast(atalkidbtype*);
uchar * 	atalk_zone_multicast(atalkidbtype*,uint);
void		atalk_FilteredZoneListLengthAndCount(atalk_rdb_t *, int *,
						     int *, int);
#include "../atalk/at_util.h"

addrTypes	atalkif_DstAddrType(atalkidbtype*,ataddrtype);
addrTypes	atalkif_SrcAddrType(atalkidbtype*,ataddrtype);
addrTypes	atalk_PrintAddrType(idbtype*,ataddrtype,addrTypes);

/*******************************************************************
 * at_zip.c
 *******************************************************************/
#include "../atalk/at_zip.h"

/*******************************************************************
 * at_setup.c
 *******************************************************************/
extern void atalk_setup_global(setup_struct *);
extern void atalk_setup_int(setup_struct *);

static inline boolean atalkif_InterfaceUp(atalkidbtype* idb)
{
    return (idb && interface_up(idb->phys_idb) &&
	    idb->aarp_tentative == AARP_ADDR_SET &&
	    idb->atalk_enabled && atalk_running);
}

/*******************************************************************
 * at_test.c
 *******************************************************************/
extern	void	atalk_test_command(parseinfo *);
extern	void	atalk_test(parseinfo *);
extern	void	atalk_test_init(subsystype *);

#endif __ATALK_PRIV_H__
