/* $Id: novell.h,v 3.11.4.16 1996/08/30 18:25:39 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell.h,v $
 *------------------------------------------------------------------
 * novell.h -- definitions for Novell protocol family
 *
 *     FOR GENERAL DOCUMENTATION OF MANY DATA STRUCTURES, ESPECIALLY
 *     CONCERNING SERVERS, SEE THE BIG COMMENT SECTION AT THE BEGINNING
 *     OF novell.c!
 *
 * June 1989, Bill Westfield
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novell.h,v $
 * Revision 3.11.4.16  1996/08/30  18:25:39  sluong
 * CSCdi65740:  IPX SAP table memory requirements grew by 300% from
 * 10.2(11) to 11.0
 * Branch: California_branch
 *
 * Revision 3.11.4.15  1996/08/14  18:09:34  john
 * CSCdi64010:  IPX SPX spoofing stops when spoof table gets large -
 *              need to age out
 * Branch: California_branch
 *
 * Revision 3.11.4.14  1996/08/14  02:37:21  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.11.4.13  1996/08/08  05:19:09  mschaef
 * CSCdi31506:  NLSP Ager process remains when NLSP is disabled
 * Branch: California_branch
 *
 * Also:
 *
 *   - Convert to use new scheduler primitives
 *   - Remove cruft including unused variables/params
 *
 * Revision 3.11.4.12  1996/08/03  23:44:27  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.11.4.11  1996/08/02  07:22:38  akr
 * CSCdi64780:  access-list 900 broken for plain-english-filters
 * Branch: California_branch
 *
 * Revision 3.11.4.10  1996/07/26  00:08:58  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.11.4.9  1996/07/25  22:28:04  jserpa
 * CSCdi58345:  SAP poison sent counter stays at zero
 * Branch: California_branch
 * Removed displaying unused counters.
 *
 * Revision 3.11.4.8  1996/07/03  22:25:23  irfan
 * CSCdi58638:  ipx compression cipx not accepted under interface
 * group-async1
 * Branch: California_branch
 * IPX interfaces with "ppp-client" type configs don't have
 * primary_nidb but they do have owner_nidb. Use that.
 *
 * Revision 3.11.4.7  1996/06/19  00:22:54  sluong
 * CSCdi42806:  spx-spoofing stops to remote PC, when RPRINTER being used.
 * Branch: California_branch
 *
 * Revision 3.11.4.6  1996/05/30  23:49:25  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.11.4.5  1996/05/03  02:18:13  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.11.4.4  1996/04/21  20:48:15  dkatz
 * CSCdi48351:  ISIS P2P handshake is 2-way
 * Branch: California_branch
 * Add upward-compatible 3-way handshake.  Use same state variables for
 * ISIS and NLSP.
 *
 * Revision 3.11.4.3  1996/04/09  22:33:09  john
 * CSCdi52606:  IPX static routes can cause error reading from NETWORK on
 *              client. Allow configurable metrics.
 * Branch: California_branch
 *
 * Revision 3.11.4.2  1996/04/09  16:46:55  sluong
 * CSCdi39057:  SYS-3-CPUHOG console message on 4000, also on 7000
 * Branch: California_branch
 *
 * Revision 3.11.4.1  1996/04/02  05:38:15  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.11  1996/03/09  18:13:08  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.10  1996/03/07  23:13:09  mschaef
 * CSCdi49527:  %SYS-3-CPUHOG from NLSP Update or IPX EIGRP process
 *
 * Revision 3.9  1996/02/14  23:01:25  john
 * CSCdi47314:  IPX network FFFFFFFE ignored when default handling is
 *              disabled. Only treat -2 special if default route enabled.
 *
 * Revision 3.8  1996/01/24  17:49:46  mschaef
 * CSCdi45840:  ipx parser seems a bit confused
 *
 * Revision 3.7  1996/01/05  18:50:42  john
 * CSCdi43117:  Crash in routine process_handle_watchdog
 *
 * Revision 3.6  1995/12/04  21:15:24  mmcneali
 * CSCdi44882:  Failure to Dynamically Populate ARP cache breaks
 *              IP/ISL Links.
 *
 * Revision 3.5  1995/12/02  08:26:29  mschaef
 * CSCdi37520:  mac-address command not taken upon router reload or
 * initial config
 *
 * Revision 3.4  1995/12/01  06:25:44  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.3  1995/11/20  17:55:17  sluong
 * CSCdi43294:  filtering saps between protocols are not feasible
 *
 * Revision 3.2  1995/11/17  19:22:55  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/11/08  21:39:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.17  1995/10/26  00:53:08  mschaef
 * CSCdi42340:  ipx maximum-path should allow more than 4 if NLSP is not on
 *
 * Revision 2.16  1995/10/13  01:04:37  shj
 * CSCdi41571:  atm interface uses wrong ipx node address
 * Backout CSCdi38984, which caused wrong ipx address on non-LANE ATM
 * interfaces.  Instead, LANE now changes ipx address if ipx is already
 * configured when LANE Client configured.
 *
 * Revision 2.15  1995/10/12  21:10:46  sluong
 * CSCdi41898:  clear ipx route <network> clears config as well as route
 *
 * Revision 2.14  1995/10/04  03:59:43  sluong
 * CSCdi41319:  no ipx routing dont kill connected routes sometimes
 *
 * Revision 2.13  1995/10/04  01:26:23  john
 * CSCdi41272:  SegV exception in IPX fast switch cache update
 *              Part 2: code review suggested better fix.
 *
 * Revision 2.12  1995/09/28  18:01:38  mschaef
 * CSCdi41028:  Memory leak when configuring/Deconfiguring IPX
 *
 * Revision 2.11  1995/09/23  21:43:14  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Generalize NLSP's interface to the ISIS LSP structure so code and
 * structures can be shared with ISIS-IP.
 *
 * Revision 2.10  1995/09/11  23:58:55  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.9  1995/08/25 14:11:47  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.8  1995/07/28 23:56:39  john
 * CSCdi37833:  IPX triggered rip/sap delays need a global default
 *
 * Revision 2.7  1995/07/20  20:22:16  john
 * CSCdi35867:  %ALIGN-3-SPURIOUS: Spurious memory access w/show align
 *              too! Align errors when a SAP queue max is configured.
 *
 * Revision 2.6  1995/07/18  23:00:27  john
 * CSCdi34414:  ipx maximimum-output-processes can be exceeded without
 *              notice. Add a show novell traffic counter.
 *
 * Revision 2.5  1995/07/18  22:33:41  john
 * CSCdi34411:  Large ipx output-rip/sap-delays may delay routing from
 *              running
 *
 * Revision 2.4  1995/07/01  03:38:56  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.3  1995/06/20 21:53:52  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/13  22:15:24  mschaef
 * CSCdi35794:  Problems with IPX default route
 *
 * Revision 2.1  1995/06/07  23:28:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NOVELL_H__
#define __NOVELL_H__

#include "../h/ieee.h"
#include "../xns/parser_defs_xns_nov.h"
#include "xns.h"
#include "../iprouting/route.h"
#include "novell_public.h"

#define NOV_WALK_CONTINUE 0
#define NOV_WALK_ABORT    -1

/*
 * Novell Packet types
 */
#define NOVELL_TYPE_SAPREPLY   0	/* Novell SAP reply */
#define NOVELL_TYPE_NLSP       0	/* Netware Link Services Protocol */
#define	NOVELL_TYPE_RIP	       1	/* Routing information protocol */
#define	NOVELL_TYPE_ECHO       2	/* Echo protocol */
#define NOVELL_TYPE_ECHO_STND  4        /* Echo Protocol Novell Standard */
#define NOVELL_TYPE_PEP        4	/* Pkt Ex Packet (yes, same as SAP) */
#define NOVELL_TYPE_SAPRESP    4	/* Novell SAP unsolicited response */
#define NOVELL_TYPE_NCP       17	/* Novell NCP packets */
#define NOVELL_TYPE_NETBIOS   20        /* Novell NetBIOS */
#define NOVELL_TYPE_SPX        5        /* Novell SPX packet type */
#define NOVELL_TYPE_ANY_PROT 0xFFFF	/* Any protocol type */
#define NOVELL_TYPE_ANY_NET  0xFFFFFFFF	/* Any network */

#define NOVELL_RIP_DEFAULT (60*ONESEC)  /* Default for Novell Rip updates */
#define NOVELL_RIP_INVALIDTIME 3        /* invalidate entries */

#define NOVELL_WATCHDOG_LEN 32          /* Novell Watchdog length */
#define NOVELL_WATCHDOG_QUERY ((uchar) '?') /* Novell Watchdog query char */
#define NOVELL_WATCHDOG_YES ((uchar) 'Y')   /* Novell Watchdog answer "Y" */

#define NOVELL_SPX_KEEPALIVE_LEN 42     /* Length of a SPX keepalive (bytes) */
#define NOVELL_SPX2_KEEPALIVE_LEN 44    /* Length of  SPX2 keepalive (bytes) */
#define NOVELL_SPX_CCID (0x08)          /* SPX2 bit in Connect Control Field */

/*
 * NOVELL Socket numbers
 */
#define NOVELL_SOCK_NCP 0x451		/* Netware Core Protocol */
#define NOVELL_SOCK_SAP 0x452		/* Novell Services advertising */
#define NOVELL_SOCK_RIP 0x453		/* Novell Routing protocol */
#define NOVELL_SOCK_NETBIOS 0x455 	/* Novell NetBIOS socket */
#define NOVELL_SOCK_DIAG 0x456		/* Novell Diagnostic packet */
#define	NOVELL_SOCK_ECHO 2		/* Echo protocol */
#define NOVELL_SOCK_ECHO_STND 0x9086    /* Echo (official) protocol */
#define NOVELL_SOCK_IGRP 0x85BE		/* IPX IGRP socket number */
#define NOVELL_SOCK_NLSP 0x9001 	/* Novell NLSP socket */
#define NOVELL_SOCK_IPXWAN 0x9004 	/* Novell IPXWAN socket */
#define NOVELL_SOCK_SNMP 0x900F		/* SNMP over IPX */
#define NOVELL_SOCK_SNMP_TRAP 0x9010    /* SNMP TRAPS over IPX */
#define NOVELL_SOCK_NLSP 0x9001		/* Netware Link Services Protocol */
#define NOVELL_SOCK_NCS_SRV 0x1D90      /* Netware Connect Server */
#define NOVELL_SOCK_ALL 0		/* All sockets */

/*
 * Selected service type definitions:
 */
#define NOVELL_SAP_FILESERVER 4         /* File server */
#define NOVELL_SAP_NASI       0x24f     /* NW Async Services Interface */

#define	NOVELL_MULTICAST_ETHER	1
#define NOVELL_MULTICAST_TR	0x80
#define NOVELL_FUNCTIONAL 0xC0000080	/* multicast address for Token Ring */

/*
 * Novell minimum MTU
 */
#define NOVELL_MINMTU 576		/* Including IPX header */

/*
 * Novell max paths
 */
#define NOVELL_MAXPATHS 64
#define NOVELL_NLSP_MAXPATHS 4

/*
 * Novell default hash table size:
 */
#define NOV_NETHASHBITS 10     /* 10 bits for a table size of 1024 */
#define NOV_NETHASHLEN  (1 << NOV_NETHASHBITS)
#define NOV_NETHASH(n) \
(((n)^((n) >> NOV_NETHASHBITS)^((n) >> 2*NOV_NETHASHBITS))&(NOV_NETHASHLEN-1))
/*
 * Here is a macro to screen out bogus network numbers. Do this in such
 * a way as to (attempt to) avoid gratuitous constants in the code.
 */
#define NOV_LEGAL_NET(n) \
    (((ulong) (n) >= NOV_MIN_NET) && ((ulong) (n) <= NOV_MAX_NET))

#define NOV_ILLEGAL_NET(n) (!(NOV_LEGAL_NET(n)))

/* ACL Log cache */

#define NOVLOGCACHESIZE 16		/* possible entries */
#define NOVLOGPRINTDELAY (5*ONEMIN)	/* max reproting interval */

typedef struct nov_logcache_entry_ {
        uchar protocol;         /* Protocol type */
    	ulong srcnet;
    	uchar srchost[IEEEBYTES];
        ushort ssock;           /* source socket */
    	ulong dstnet;
    	uchar dsthost[IEEEBYTES];
        ushort dsock;           /* destination socket*/
        ushort list;            /* acl */
        unsigned age;           /* age of this entry (in packets) */
        unsigned count;         /* count of hits */
        sys_timestamp logtime;  /* last time logged to system */
        boolean grant;          /* did we pass this packet? */
} nov_logcache_entry;

/*
 * Novell IPX Packet structure
 */

typedef struct ipxhdrtype_ {
    XNSMACRO				/* IPX header */
} ipxhdrtype;

#define ipxheadstart(ptr) ((ptr)->network_start)
#define ipxcompressionheadstart(ptr) ((ptr)->network_start)
#define ipxdatastart(ptr) ((ptr)->network_start + XNSOVERHEADBYTES)

/*
 * These definitions are to provide for use of better names
 * than boolean TRUE or FALSE for various functions
 */

enum IPX_STATIC_TYPE {
    IPX_STATIC_TYPE_HOST = 0,
    IPX_STATIC_TYPE_INTERFACE = 1
};

enum IPX_FLOATING_STATIC_EXISTS {
    IPX_FLOATING_STATIC_EXISTS_NO = 0,
    IPX_FLOATING_STATIC_EXISTS_YES = 1
};

enum IPX_ROUTE_MATCH_TYPE {
    IPX_ROUTE_MATCH_EXACT = 0,
    IPX_ROUTE_MATCH_FUZZ = 1
};

/*
 * for many functions that take in flags for 'broadcasts vs. unicast'.
 * A 'RIPSAP unicast' indicates send a response as a RIP or SAP packet,
 * even if EIGRP is running on the link.
 */
enum IPX_DESTINATION {
    IPX_DESTINATION_UNICAST = 0,
    IPX_DESTINATION_BROADCAST = 1,
    IPX_DESTINATION_RIPSAP_UNICAST = 2
};

/* for many functions that take in flags for full and partial updates */
enum IPX_FULL {
    IPX_FULL_NOT_NEEDED = 0,
    IPX_FULL_NEEDED = 1
};

enum IPX_PARTIAL {
    IPX_PARTIAL_NOT_NEEDED = 0,
    IPX_PARTIAL_NEEDED = 1
};

enum IPX_UPD_TYPE {
    IPX_UPD_RIP = 0,
    IPX_UPD_SAP = 1    };

enum IPX_TMR_TYPE {
    IPX_TMR_GAP      = 0,
    IPX_TMR_RIP_UPD  = 1,
    IPX_TMR_SAP_UPD  = 2,
    IPX_TMR_ATLR     = 3,
    IPX_TMR_TBS      = 4  /* Available */
};

/* for many functions that take in flags for advertising */
enum IPX_ADVERTISING {
    IPX_ADVERTISING_NO = 0,
    IPX_ADVERTISING_YES = 1
};

/* for novell_findsapqueue */
enum IPX_SAP_QUEUE_LOOKUP {
    IPX_SAP_QUEUE_LOOKUP_MUST_EXIST = 0,
    IPX_SAP_QUEUE_LOOKUP_MAY_CREATE = 1
};

/* for novell_sap_free */
enum IPX_SAP_FREE {
    IPX_SAP_FREE_IGNORE_BACKUPS = 0,
    IPX_SAP_FREE_LOOKUP_BACKUPS = 1
};

/* for novell_place_in_sorted_sap_list */

enum IPX_SAP_INSERT  {
    IPX_SAP_INSERT_HASH_EXISTS = 0,
    IPX_SAP_INSERT_HASH_NEEDED = 1
};

/* for nov_flood */
enum IPX_FLOOD {
    IPX_FLOOD_NORMAL = 0,
    IPX_FLOOD_TYPE20 = 1
};

/* for novell_netbios_acfcheck */
enum IPX_ACCESS_DIRECTION {
    IPX_ACCESS_DIRECTION_OUTGOING = 0,
    IPX_ACCESS_DIRECTION_INCOMING = 1
};

/* for adding route entries */
enum IPX_ROUTE_ADD {
    IPX_ROUTE_ADD_UNNUMBERED = 0,
    IPX_ROUTE_ADD_NORMAL = 1
};

/*
 * for changing single RDB/SRDBs. DELETED is RDB deleted. ADD is new one added.
 * CHANGE is same source, different info. PUSHED_OUT is special case where an
 * entry is DELETED, but other entries from the SAME PROTOCOL remain in the
 * same RDB/SRDB list as this entry. (I.e., better cost from same protocol,
 * or pruning because maxpaths is shorter.)
 */
enum IPX_ROUTE_EVENT {
    IPX_ROUTE_EVENT_DELETED = 0,
    IPX_ROUTE_EVENT_ADD = 1,
    IPX_ROUTE_EVENT_CHANGE = 2,
    IPX_ROUTE_EVENT_PUSHED_OUT = 3,
};

/* for deleting route entries */
enum IPX_ROUTE_DELETE {
    IPX_ROUTE_DELETE_RETAIN_CONNECTED = 0,
    IPX_ROUTE_DELETE_REMOVE_CONNECTED = 1,
    IPX_ROUTE_DELETE_REMOVE_DYNAMIC = 2
};

/* for freeing route entries */
enum IPX_ROUTE_FREE {
    IPX_ROUTE_FREE_NEVER_IN_TABLE = 0,
    IPX_ROUTE_FREE_WAS_IN_TABLE = 1,
};

/* for round-robin checks */
enum IPX_ROUND_ROBIN {
    IPX_ROUND_ROBIN_NO = 0,
    IPX_ROUND_ROBIN_YES = 1
};

/* Use default route if route lookup fails */
enum IPX_DEFAULT_ROUTE {
    IPX_DEFAULT_ROUTE_NO = 0,
    IPX_DEFAULT_ROUTE_YES = 1
};

/* for IPX Standard or cisco IPX echoes */
enum IPX_ECHO_TYPE {
    IPX_ECHO_TYPE_CISCO = 0,
    IPX_ECHO_TYPE_NOVELL = 1
};

enum IPX_STATIC_DEFINE_TYPE {
    IPX_STATIC_DEFINE_TYPE_NO_NVGEN = 0,
    IPX_STATIC_DEFINE_TYPE_NVGEN = 1
};

enum IPX_RIPSAP_CONFIG {
    IPX_RIPSAP_CONFIG_OFF = 0,
    IPX_RIPSAP_CONFIG_AUTO = 1,
    IPX_RIPSAP_CONFIG_ON = 2
};

enum IPX_IPXWAN_ERROR_CFG {
    IPX_IPXWAN_ERROR_CONFIG_RESET = 0,
    IPX_IPXWAN_ERROR_CONFIG_DOWN = 1,
    IPX_IPXWAN_ERROR_CONFIG_NOOP = 2
};


/*
 * Novell address data structure
 * Used for creating a linked list of Novell addresses
 */

typedef struct novaddtype_ {
	struct novaddtype_ *next;
	ulong novell_net;
	uchar novell_host[IEEEBYTES];
} novaddtype;

#define IPX_NET_BYTES	4		/* Size of IPX network number */
#define IPX_HOST_BYTES	IEEEBYTES	/* Size of IPX host number */
					/* Size of IPX address in bytes */
#define	IPX_ADDR_BYTES	(IPX_NET_BYTES + IPX_HOST_BYTES)	


/*
 *  Client specific structures
 */
struct novell_client_ {
    struct novell_client_ *next;
    idbtype *idb;
};


/*
 * NOVELL RIP packet definitions
 */

typedef struct {
	uchar nnet[XNS_NETBYTES];	/* dest. network */
	ushort metric;			/* routing metric */
	ushort delay;			/* delay info	*/
} novellripinfotype;

typedef struct {
	ushort cmd;			/* kind of packet */
	novellripinfotype nets[1];	/* variable length */
} novellriptype;

#define NOVELL_DEFAULT_AREA_COUNT (6)
#define NOVELL_DEFAULT_MAX_HOPS (15)
#define NOVELL_MAX_HOPS_ALLOWED (255)
#define NOVELL_RIP_ADV_MAX_HOPS (15)
#define NOVELL_RIP_ADV_INFINITY (NOVELL_RIP_ADV_MAX_HOPS + 1)
#define NOVELL_DEFAULT_CONNECTED_TICKS (1)
#define NOVELL_DEFAULT_CONNECTED_HOPS (0)
#define NOVELL_DEFAULT_RIP_HOPS (1)
#define NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED \
	(novell_max_hops == NOVELL_DEFAULT_MAX_HOPS)

#define NOVELL_NETBIOS_FIND_NAME (0x01)
#define NOVELL_NETBIOS_NAME_LEN (16)             /* Field is 16 chars */
#define NOVELL_NETBIOS_USABLE_NAME_LEN (14)      /* Only 14 used if Novell */
#define NOVELL_NETBIOS_NUM_NETS (8)
#define NOVELL_NETBIOS_MACRO \
        ulong PACKED(nov_netbios_nets[NOVELL_NETBIOS_NUM_NETS]); /* Eight hops */ \
        uchar nov_netbios_nametypeflag;		/* Name type flag */ \
	uchar nov_netbios_cmd;			/* NetBIOS command */ \
	uchar nov_netbios_server[NOVELL_NETBIOS_NAME_LEN]; /* Server name */

typedef struct novellnetbiosheader_ {
        NOVELL_NETBIOS_MACRO			/* Novell NetBIOS header */
} novellnetbiosheader;

#define IBM_TICKS(x) ((5499 + ((x) >> 8)) / 5500)

#define NOT_IBM_TICKS(x) ((5500 * (x)) << 8)

#define TICKS_FACTOR 334

/*
 * Macroes to convert between our delay measurements and the IBM clocks
 * that Novell uses. Note that the formula used is not mathmatically
 * precise. Rather, the "factor" is chosen to give a tick count of 1
 * for fddi/tr16/ether/tr4 lan interfaces and a tick count of 6 for
 * a serial T1 interface, assuming that default delay values are assigned.
 * This maps correctly onto what Novell calls for. Recall that the
 * resulting tick count can always be overridden explicitely using the
 * "ipx delay" interface command.
 */

/*
 * Don't call NOVELL_IDB_IPXWAN_TICKS unless you are in the IPXWAN code...
 * call NOVELL_IDB_TICKS instead...
 */
#define NOVELL_IDB_IPXWAN_TICKS(thisnidb) \
  ((thisnidb)->idb->novell_delay ? \
   (thisnidb)->idb->novell_delay : \
   ((((thisnidb)->idb->delay) + (TICKS_FACTOR-1)) \
    /TICKS_FACTOR))

#define NOVELL_IDB_TICKS(thisnidb) (((thisnidb)->nov_ipxwan_delay) ? \
				    (thisnidb)->nov_ipxwan_delay :  \
				    NOVELL_IDB_IPXWAN_TICKS(thisnidb))

/*
 * The NLSP/IPXWAN link 'delay'. Take manual config if present 
 */
#define NOVELL_LINK_DELAY(thisnidb) \
  (((thisnidb)->novell_link_delay) ? \
   (thisnidb)->novell_link_delay :  \
   (thisnidb)->nov_ipxwan_link_delay)

/*
 * The NLSP/IPXWAN link 'throughput'. Take manual config if present 
 */
#define NOVELL_LINK_THROUGHPUT(thisnidb) \
  (((thisnidb)->novell_link_throughput) ? \
   (thisnidb)->novell_link_throughput :  \
   (thisnidb)->nov_ipxwan_link_throughput)
/*
 * If we have an internal ID on this interface use it otherwise
 * use the global Internal ID
 */
#define NOVELL_INTERNAL_ID(thisnidb) \
  (((thisnidb)->nov_ipxwan_internal_net) ? \
   (thisnidb)->nov_ipxwan_internal_net : \
   ipx_internal_net)

#define NOVELL_FASTER_NIDB(nidb1,nidb2) ((NOVELL_IDB_TICKS(nidb1) <         \
				      NOVELL_IDB_TICKS(nidb2)) ||           \
				      ((NOVELL_IDB_TICKS(nidb1) ==          \
				       NOVELL_IDB_TICKS(nidb2)) &&          \
				       ((nidb1)->idb->delay <		    \
					(nidb2)->idb->delay)))
    
#define NOVELL_NIDB_UNNUMBERED(nidb) ((nidb)->novellnet == 0)

#define NOVELL_INFDELAY ((ushort) 0xffff) /* For poisoning; max poss. delay */

#define NOVELL_NO_CHECKSUM (0xffff) /* No checksum in place */


#define NOVELL_SERVER_NAME_LEN 48

/*
 * These macros define how we extract various best route information
 *
 * 'b' is pointer to ripentrytype structures. This macro returns TRUE if 'hop'
 * and 'del' is a better route than 'b', using the algorithm as appropriate.
 * If the routes are of equal cost, the macro returns FALSE.
 *
 */
#define NOVELL_BETTER_ROUTE_INFO(hop, del, b) \
    ((!novell_algorithms) ? ((hop) < (b)->hopcount) : \
     (((del) < (b)->delay) ? TRUE : \
      (((del) > (b)->delay) ? FALSE : \
       ((hop) < (b)->hopcount))))

#define NOVELL_WORSE_ROUTE_INFO(hop, del, b) \
    ((!novell_algorithms) ? ((hop) > (b)->hopcount) : \
     (((del) > (b)->delay) ? TRUE : \
      (((del) < (b)->delay) ? FALSE : \
       ((hop) > (b)->hopcount))))

#define NOVELL_EQUAL_ROUTE_INFO(hop, del, b) \
    ((!novell_algorithms) ? ((b)->hopcount == (hop)) : \
     (((b)->delay != (del)) ? FALSE : \
      ((b)->hopcount == (hop))))

#define NOVELL_BETTER_XROUTE_INFO(hop, del, b) \
    ((!novell_algorithms) ? ((hop) < (b)->ext_hop) : \
     (((del) < (b)->ext_delay) ? TRUE : \
      (((del) > (b)->ext_delay) ? FALSE : \
       ((hop) < (b)->ext_hop))))

#define NOVELL_WORSE_XROUTE_INFO(hop, del, b) \
    ((!novell_algorithms) ? ((hop) > (b)->ext_hop) : \
     (((del) > (b)->ext_delay) ? TRUE : \
      (((del) < (b)->ext_delay) ? FALSE : \
       ((hop) > (b)->ext_hop))))

#define NOVELL_EQUAL_XROUTE_INFO(hop, del, b) \
    ((!novell_algorithms) ? ((b)->ext_hop == (hop)) : \
     (((b)->ext_delay != (del)) ? FALSE : \
      ((b)->ext_hop == (hop))))

/*
 * The novell flashupdate value needs to be WELL under 600 msec for
 * 'Lost Network' handling; set it to such when needed...
 */
#define NOVELL_FLASHUPDATE (250)

/*
 * Delay respondes to lost routers by this many milliseconds
 */
#define NOVELL_ATLR_DELAY (500)

/*
 * Force ATLR updates to occur no more than 10 times a second
 */
#define NOVELL_ATLR_GRANULARITY (100)

/* Comparison constants for nov_sap_distance_compare */
#define NOV_SAP_CMP_GT 0
#define NOV_SAP_CMP_GE 1
#define NOV_SAP_CMP_EQ 2
#define NOV_SAP_CMP_NE 3
#define NOV_SAP_CMP_LE 4
#define NOV_SAP_CMP_LT 5

/*
 * These are STATES that an NIDB can be in. There is a reason for the order;
 * see the macros below
 */
#define NOVELL_NIDB_UP  		1
#define NOVELL_NIDB_DELETE_PENDING 	2
#define NOVELL_NIDB_DOWN_PENDING 	3
#define NOVELL_NIDB_IPXWAN_STARTING 	4
#define NOVELL_NIDB_DELETE_EXECUTING 	5
#define NOVELL_NIDB_DOWN_EXECUTING 	6
#define NOVELL_NIDB_DELETED 		7
#define NOVELL_NIDB_DOWN 		8
#define NOVELL_NIDB_NEW 		9

#define NOVELL_NIDB_ISNEW(s) ((s)->state == NOVELL_NIDB_NEW)

#define NOVELL_NIDB_ISIPXWAN_STARTING(s) \
  ((s)->state == NOVELL_NIDB_IPXWAN_STARTING)

#define NOVELL_NIDB_DO_NOT_COMPRESS_NOW(s) NOVELL_NIDB_ISIPXWAN_STARTING((s))

#define NOVELL_NIDB_ISUP(s) ((s)->state == NOVELL_NIDB_UP)

#define NOVELL_NIDB_IDB_UP(s) (NOVELL_NIDB_ISUP(s) && interface_up((s)->idb))

#define NOVELL_NIDB_ISDOWN(s) ((s)->state == NOVELL_NIDB_DOWN)

#define NOVELL_NIDB_IS_DELETED(s) ((s)->state == NOVELL_NIDB_DELETED)

#define NOVELL_NIDB_ISDORD(s) \
  (((s)->state == NOVELL_NIDB_DELETED) || \
  (((s)->state == NOVELL_NIDB_DOWN)))

#define NOVELL_NIDB_SHOULD_BRINGDOWN(s) (NOVELL_NIDB_ISDORD(s))

#define NOVELL_NIDB_UPDATES_ALLOWED(s) ((s)->state <= NOVELL_NIDB_DOWN_PENDING)

#define NOVELL_NIDB_DORD_PENDING(s) \
  (((s)->state == NOVELL_NIDB_DELETE_PENDING) || \
  (((s)->state == NOVELL_NIDB_DOWN_PENDING)))

#define NOVELL_NIDB_DORD_EXECUTING(s) \
  (((s)->state == NOVELL_NIDB_DELETE_EXECUTING) || \
  (((s)->state == NOVELL_NIDB_DOWN_EXECUTING)))

#define NOVELL_NIDB_DOWN_MODE(s) \
  (((s)->state == NOVELL_NIDB_DOWN_PENDING) || \
   ((s)->state == NOVELL_NIDB_DOWN_EXECUTING) || \
   ((s)->state == NOVELL_NIDB_DOWN))
  
#define NOVELL_NIDB_DOWN_IN_PROGRESS(s) \
  (((s)->state == NOVELL_NIDB_DOWN_PENDING) || \
   ((s)->state == NOVELL_NIDB_DOWN_EXECUTING))
  
#define NOVELL_NIDB_DELETED_MODE(s) \
    (((s)->state == NOVELL_NIDB_DELETE_PENDING) || \
     ((s)->state == NOVELL_NIDB_DELETE_EXECUTING) || \
     ((s)->state == NOVELL_NIDB_DELETED))

#define NOVELL_NIDB_DELETE_IN_PROGRESS(s) \
    (((s)->state == NOVELL_NIDB_DELETE_PENDING) || \
     ((s)->state == NOVELL_NIDB_DELETE_EXECUTING))

#define NOVELL_NIDB_CONFIGURABLE(s) (!NOVELL_NIDB_DELETED_MODE(s))

#define NOVELL_NIDB_RECEIVES_ALLOWED(s) \
  (((s)->state == NOVELL_NIDB_UP) || \
  (((s)->state == NOVELL_NIDB_IPXWAN_STARTING)))

#define NOVELL_NIDB_SENDS_ALLOWED(s) ((s)->state <= NOVELL_NIDB_DOWN_EXECUTING)

#define NOVELL_NIDB_IPXWAN_START_ALLOWED(s) \
  (((s)->state == NOVELL_NIDB_UP) || \
   ((s)->state == NOVELL_NIDB_NEW))


/*
 * Novell packet type definitions
 */

#define NOVELL_SAP_GENQUERY 		1
#define NOVELL_SAP_GENRESPONSE 		2
#define NOVELL_SAP_NEARQUERY 		3
#define NOVELL_SAP_NEARRESPONSE 	4
#define NOVELL_EVERYONE 		0xFFFF
#define NOVELL_QUERYLENGTH 		34
#define NOVELL_NAMEQUERYLENGTH 		90
#define NOVELL_ECHO_REQUEST 		1
#define NOVELL_ECHO_REPLY 		2
#define NOVELL_TYPE_ECHO_RCV 		0
#define NOVELL_TYPE_ECHO_RPLY 		1
#define NOVELL_SAP_GENNAMEQUERY         0x0C
#define NOVELL_SAP_GENNAMERESPONSE      0x0D
#define NOVELL_SAP_NEARNAMEQUERY        0x0E
#define NOVELL_SAP_NEARNAMERESPONSE     0x0F

/*
 * Define idb->novell_routecache values
 */
#define	NOVELL_RCACHE		1	/* fastswitch enabled */
#define	NOVELL_RCACHE_CBUS	2	/* Autonomous switching enabled */
#define NOVELL_RCACHE_SSE	4 	/* Silicon switching enabled */

/*
 * define nidb->novell_spoof_flag values
 */
#define NOVELL_S_SPOOF          1       /* Spoof SPX keepalives */
#define NOVELL_W_SPOOF          2       /* Spoof Watchdog packets */

typedef struct {
    ushort query_type;
    ushort server_type;
} novell_querytype;

#define NOVELL_SERVER_COMMON_DATA \
    ushort PACKED(server_type);		/* server's type */ \
    char server_name[NOVELL_SERVER_NAME_LEN];	/* server's name */ \
    ulong PACKED(dnet);		/* destination network */ \
    uchar dhost[IEEEBYTES];	/* destination host */    \
    ushort PACKED(dsock);	/* destination socket */  \
    ushort PACKED(nethops);	/* how far away is it */

/* An actual service entry, as advertised via SAP */
typedef struct {
    NOVELL_SERVER_COMMON_DATA
} novell_serveridtype;

/* A linked list of unique server info */
typedef struct nov_server_per_source_list_ {
    /* 'next' MUST be the first element in this structure */
    struct nov_server_per_source_list_ *next; /* Next in list */
    NOVELL_SERVER_COMMON_DATA
} novell_server_per_source_list;

/* A function which is called by novell_accept_sap_data */
typedef void (*novell_accept_sap_data_function)(novell_serveridtype *,
						dual_peertype *, novpdbtype *);

typedef struct {
    ushort response_type;		/* nearest or general */
    novell_serveridtype PACKED(server[1]); /* list of servers (possibly only 1) */
} novell_sappacket;

/* Definitions for Service Query by Name */
#define NOVELL_SERVER_NAME_COMMON_DATA \
    ushort PACKED(server_type);		/* server's type */ \
    char server_name[NOVELL_SERVER_NAME_LEN];	/* server's name */ \
    ulong PACKED(dnet);		/* destination network */ \
    ulong PACKED(dmask);	/* destination mask */
 
typedef struct {
    NOVELL_SERVER_NAME_COMMON_DATA
} novell_servernametype;

typedef struct {
    ushort query_type;		/* nearest or general */
    novell_servernametype PACKED(server[1]); /* list of servers (possibly only 1) */
} novell_querynametype;

/* Hash tables (by name) for SAP common data, and SAP per source data */
#define NOVELL_SAP_HASHLEN 1001
typedef novell_server_per_source_list
    *nov_server_per_source_bynametable[NOVELL_SAP_HASHLEN];

/* Handy definitions for primary nidbs */
#define IPX_PRIMARY_NIDB(i) ((i)->primary_nidb)
#define IPX_NIDB_IS_INTERNAL(n) ((n)->internal_network == TRUE)
#define IPX_SET_PRIMARY_NIDB(i,n) (i)->primary_nidb = (n)
#define IPX_NIDB_IS_PRIMARY(n) ((n) == IPX_PRIMARY_NIDB((n)->idb))
#define IPX_NIDB_IS_SECONDARY(n) (!IPX_NIDB_IS_PRIMARY(n))
#define IPX_CLEAR_PRIMARY_NIDB(i) ((i)->primary_nidb = NULL)
#define IPX_OWNER_NIDB(i)  ((i)->owner_nidb)
#define IPX_CONFIG_NIDB(i) (((i)->primary_nidb)?(i)->primary_nidb:(i)->owner_nidb)

#define IPX_PROCESS_NAME_LEN (16)
#define IPX_PROCESS_NAME_STORAGE (IPX_PROCESS_NAME_LEN + 1)

#define IPX_NCP_TYPE_REQUEST_FROM_WORKSTATION (0x2222)
#define IPX_NCP_TYPE_REPLY_FROM_SERVER        (0x3333)
typedef struct {
    XNSMACRO
	ushort ncp_type;
    uchar  sequence_number;
    uchar  connection_number_low_octet;
    uchar  task_number;
    uchar  connection_number_high_octet;
} ipx_ncp_header;

#define IPX_NCP_OVERHEADBYTES (6)

#define IPX_COMPRESSION_HEADER_STORAGE_SIZE (XNSOVERHEADBYTES + \
 					     IPX_NCP_OVERHEADBYTES)

#define IPX_COMPRESSION_NCP_CONNECTION_NUMBER_FROM_HEADER(h) \
    ((((h)->connection_number_high_octet) << 8) | \
     ((h)->connection_number_low_octet))

/*
 * IPX/NCP compression slot block
 */
#define IPX_COMPRESSION_FLAG_INUSE (0x01)
#define IPX_COMPRESSION_FLAG_CONFIRMED (0x02)
#define IPX_COMPRESSION_FLAG_GIVES_NCP_COMPRESSION (0x04)
#define IPX_COMPRESSION_SLOT_END_OF_CHAIN (0xFFFF)


typedef struct {
    ulong buffer_misses;
    ulong buffer_swaps;
    ushort flags;
    uchar slot;
    uchar id;
    ushort more_recently_used_slot;
    ushort  less_recently_used_slot;
    ipx_ncp_header current_header;
} ipx_ncp_compression_slot;

#define FOR_ALL_NIDBS(x) \
  for ((x) = (nidbtype *) novidbQ.qhead; (x) != NULL; (x) = (x)->next)

#define FOR_ALL_NIDBS_USE_SAVED_NEXT(x, s) \
  for ((x) = (nidbtype *) novidbQ.qhead, \
       (s) = ((x) == (nidbtype *) NULL) ? (nidbtype *) NULL : (x)->next; \
       (x) != NULL; \
       (x) = (s), (s) = ((x) == (nidbtype *) NULL) ? \
       (nidbtype *) NULL : (x)->next)

#define DECREMENT_NOVELLNETS(x) \
    if ((x)->novellnets > 0) (x)->novellnets--



/*
 * This structure defines an NLSP cost descriptor:
 */

enum IPX_NLSP_COST {
    IPX_NLSP_COST_LT = -1,
    IPX_NLSP_COST_EQ = 0,
    IPX_NLSP_COST_GT = 1
    };

#define NLSP_BETTER_COST(a, b)                                            \
    (novnlsp_cost_compare((a), (b))  < IPX_NLSP_COST_EQ)

#define NLSP_SAME_OR_BETTER_COST(a, b)                                    \
    (novnlsp_cost_compare((a), (b)) <= IPX_NLSP_COST_EQ)

#define NLSP_SAME_COST(a, b)                                              \
    (novnlsp_cost_compare((a), (b)) == IPX_NLSP_COST_EQ)

#define NLSP_WORSE_COST(a, b)                                             \
    (novnlsp_cost_compare((a), (b))  > IPX_NLSP_COST_EQ)

#define REG_INVOKE_NLSP_WORSE_COST(a, b)                                  \
    (reg_invoke_ipx_nlsp_cost_compare((a), (b))  > IPX_NLSP_COST_EQ)


/* Defines the significant 'source' bits in the SAP flags word */
#define NOVELL_SAP_SOURCE_FLAGS (NET_RIP | NET_IGRP | NET_STATIC | NET_NLSP | NET_ISIS | NET_CONNECTED)
#define NOVELL_SOURCE_FLAGS NOVELL_SAP_SOURCE_FLAGS
#define NOVELL_SAP_SOURCE(flags) (flags & NOVELL_SAP_SOURCE_FLAGS)
#define NOVELL_ROUTE_SOURCE(flags) NOVELL_SAP_SOURCE(flags)

/*
 * These options flags are defined outside of the normal 'flags' word,
 * because these are protocols, just various 'parameters'
 */
/* Don't NVGEN this static server... */
#define IPX_SERVER_OPTION_STATIC_NO_NVGEN (0x01)

/* Remembers each interface that we learned a service on... */
struct novell_sapentry_rdb_ {
    novell_sapentry_rdb *next;
    nidbtype *nidb;
    sys_timestamp timer;	/* when last update occured */
    boolean snapshot_no_age; 	/* TRUE if no age since snapshot idb */
    ulong snet;
    struct sapentry_ *sap;
    isis_lsp_linkage lsp_linkage; /* LSP linkage information */
    nov_cost_type cost;
    uchar srchost[IEEEBYTES];
};

/*
 * Temporary tracking structure for display routines
 */
typedef struct srdb_list_ {
    struct srdb_list_ *next;
    novell_sapentry_rdb *srdb;
} nov_srdb_list;

/*
 * Differentiate real sap entries from placeholder
 * service entries (inserted by update thread):
 */

enum IPX_SAP_TYPE {
    IPX_SAP_RESERVED = 0,
    IPX_SAP_ACTIVE = 1,
    IPX_SAP_PLACEHOLDER = 2};

typedef struct sapentry_ {
    dqueue_t saps;		/* pointer to next entry block */
    struct sapentry_ *hash_next;/* pointer to next name hash entry block */
    struct sapentry_ *net_hash_next; /* Pointer to next net hash entry */
    struct sapentry_ *next_newest; /* Next in 'newest' chain */
    struct sapentry_ *prev_newest; /* Previous in 'newest' chain */
    novell_sapentry_rdb *paths;
    sys_timestamp sap_timer;	/* Used when aging out... */
    sys_timestamp sap_changed;	/* Changes for 'flash updates' */
    enum IPX_SAP_TYPE type;     /* active ndb or update placeholder */
    ulong flags;		/* protocol flags */
    ulong options;		/* 'Random' options */
    ulong route_delay;		/* 'cached' Delay from routing table */
    ulong route_flags;		/* Either IGRP or RIP */
    struct sapqueue_ *sapq;	/* Sap queue this entry is in */
    backuptype *backup_info;	/* Backup services information */
    ushort route_hops;		/* 'cached' hop count from routing table */
    ushort ext_hop;		/* External route hop count */
    novell_serveridtype server;	/* the information */
    uchar novpdbindex;          /* how we heard it */
    uchar pathcount;		/* Count of paths */
    uchar poisoned:1;		/* unreachable */
    uchar reverse_poisoned:1;	/* advertised via reverse poison */
    uchar nidb_changed:1;	/* NIDB list changed? */
    uchar deleted_static:1;	/* True if a deleted static entry */
} novell_sapentrytype;

typedef novell_sapentrytype *nov_saproutetable[NOV_NETHASHLEN];

/*
 * A linked list of SAP entries, for a given type. No attempt is made to
 * sort within the SAPS of the same type, but the type list itself is
 * sorted.
 */
#define NOVELL_SAP_SORT_INTERVAL (60*ONESEC)
typedef struct sapqueue_ {
    struct sapqueue_ *next;
    ushort server_type;
    dqueue_t saps;
    boolean sorted;
    ulong sap_count;
    sys_timestamp next_sorted_time;
    novell_sapentrytype *sap_hashtable[NOVELL_SAP_HASHLEN];
} novell_sapqueuetype;

 
#define FOR_ALL_SAP_TYPES(q) \
    for ((q) = (novell_sapqueuetype *)novell_sapQ.qhead; \
	 (q) != NULL; \
	 (q) = (q)->next)

#define FOR_ALL_SAP_TYPES_ALLOW_DELETE(q, n) \
    for ((q) = (novell_sapqueuetype *)novell_sapQ.qhead, \
	 (n) = ((q) ? (novell_sapqueuetype *)(q)->next : NULL); \
	 (q) != NULL; \
	 (q) = (n), (n) = ((q) ? (novell_sapqueuetype *)(q)->next : NULL))

#define FOR_ALL_SAPS_OF_TYPE(e, q) \
    for ((e) = (novell_sapentrytype *)(q)->saps.flink; \
	 (e) != (novell_sapentrytype *)&(q)->saps; \
	 (e) = (novell_sapentrytype *)(e)->saps.flink)

#define FOR_ALL_SAPS_OF_TYPE_ALLOW_DELETE(e, n, q) \
    for ((e) = (novell_sapentrytype *)(q)->saps.flink, \
	 (n) = (novell_sapentrytype *)(e)->saps.flink; \
	 (e) != (novell_sapentrytype *)&(q)->saps; \
	 (e) = (n), (n) = (novell_sapentrytype *)(e)->saps.flink)

#define FOR_ALL_SAPS(e, q) \
    FOR_ALL_SAP_TYPES((q)) \
        FOR_ALL_SAPS_OF_TYPE((e), (q))

/* Used to sort sap entries alphabetically. Note that the ptable structure */
/* is intended to support a generic string sort (see novell.c: dosort())   */

typedef void *ptable[1];                      /* generic array of pointers */
typedef char novell_name_type[NOVELL_SERVER_NAME_LEN];      /* server name */
typedef novell_name_type *novell_namep;       /* pointer to same */

/* Used to point to the second byte of the watchdog packet 
 * IPX Header (30 Bytes), Connection ID (1 Byte), Watchdog (1 byte) 
 */
typedef struct _ipx_watchdog_packet {
    XNSMACRO			/* IPX header */
    uchar session_byte;		/* Session number */
    uchar watchdog_byte;	/* Watchdog query byte */
} ipx_watchdog_packet;

/* Novell's 'magic number' "confidence identifier" for IPX ping packets */
/* ASCII "Ping" */
#define NOVELL_ECHO_SIGNATURE (0x50696E67)
/* Min length for Novell Standard echo. IPX Header, +10, +4 */
#define NOVELL_ECHO_STD_SIZE (40 + sizeof(sys_timestamp))
/* Place where Novell Echo Data Starts */
#define NOVELL_ECHODATASTART 40
/* Novell Standard Echo Version number */
#define NOVELL_ECHO_VERSION 1

/* Used to point at the special information which comes after the
 * IPX Header in an official IPX ping (echo).
 */
typedef struct _ipx_stnd_ping {
  uchar signature[4];           /* Singature = 'Ping' */
  uchar version_byte;           /* Ping version, for now 1 */           
  uchar type_byte;              /* 0 request, 1 response */
  uchar ping_id[2];             /* Unique identifier */
  uchar result_byte;            /* 0 if no data, 1 if data */
  uchar reserved;               /* zero for now (padding) */
  sys_timestamp PACKED(time_sent);
} ipx_stnd_ping;

typedef struct {
  ushort type;                  /* 1 request, 2 response */
  sys_timestamp PACKED(time_sent);
  uchar data[0];
} ipx_cisco_ping;


/*
 * NOVELL statistic elements
 * First, generic type statistics
 */
#define	NOVELL_INPUT		0
#define	NOVELL_BCASTIN		1
#define	NOVELL_FORWARDED	2
#define	NOVELL_BCASTOUT		3
#define	NOVELL_FORMATERROR	4
#define	NOVELL_CKSUMERR		5
#define	NOVELL_HOPCOUNT		6
#define	NOVELL_NOROUTE		7
#define	NOVELL_ENCAPSFAILED	8
#define	NOVELL_OUTPUT		9
#define	NOVELL_INMULT		10

/*
 * Novell specific Traffic elements
 */
#define NOVELL_LOCALDEST	11
#define NOVELL_UNKNOWN		12
#define NOVELL_SAPREQIN		13
#define NOVELL_SAPRESIN		14
#define NOVELL_SAPOUT		15
#define NOVELL_SAPREPLY		16
#define NOVELL_FWDBRD		17
#define NOVELL_ECHOREQOUT	18
#define NOVELL_ECHOREQIN        19
#define NOVELL_ECHOREPOUT	20
#define NOVELL_ECHOREPIN        21
#define NOVELL_SAPFLASH		22
#define NOVELL_FILTER_OUT       23
#define NOVELL_PITCHED  	24
#define NOVELL_THROTTLED	25
#define NOVELL_IGRPIN		26
#define NOVELL_IGRPOUT		27
#define NOVELL_UPDATE_IN	28
#define NOVELL_UPDATE_OUT	29
#define NOVELL_QUERY_IN		30
#define NOVELL_QUERY_OUT	31
#define NOVELL_REPLY_IN		32
#define NOVELL_REPLY_OUT	33
#define NOVELL_IGRP_SAPIN	34
#define NOVELL_IGRP_SAPOUT	35
#define NOVELL_SAPFMTERROR      36
#define NOVELL_RIPFMTERROR      37
#define NOVELL_WATCHDOG_IN      38
#define NOVELL_WATCHDOG_OUT     39
#define NOVELL_RIPREQIN		40
#define NOVELL_RIPRESIN		41
#define NOVELL_RIPOUT		42
#define NOVELL_RIPREPLY		43
#define NOVELL_RIPFLASH		44
#define NOVELL_FILTER_IN        45
#define NOVELL_SAPGNSIN         46
#define NOVELL_SAPGNSREPLY      47
#define NOVELL_RIPREQOUT        48
#define NOVELL_SAPREQOUT        49
#define NOVELL_NETBIOSIN        50
#define NOVELL_UNKNOWN_SOCKET   51
#define NOVELL_UNKNOWN_NOHELP   52
#define NOVELL_UNKNOWN_INFILT   53
#define NOVELL_IN_DISCARDS      54
#define NOVELL_OUT_DISCARDS     55
#define NOVELL_EXCEED_PROC      56
#define NOVELL_SAPNEARNAMEIN    57
#define NOVELL_SAPNEARNAMEOUT   58
#define NOVELL_SAPGENNAMEIN     59
#define NOVELL_SAPGENNAMEOUT    60
#define	NOVELL_MAXTRAFFIC	(NOVELL_SAPGENNAMEOUT + 1)


/*
 * values that nov_forus returns
 */
#define NOVELL_ROUTEIT 0			/* route it */
#define NOVELL_CONSUMEIT 1			/* its for us */
#define NOVELL_PITCHIT 2			/* when we hear ourself */

/*
 * Default GNS response delay, in milliseconds
 */
#define NOVELL_DEFAULT_GNS_RESPONSE_DELAY (0)

/*
 * Maximum Novell packet size. TR/FDDI. Different from XNS.
 */

#define NOVELL_MAXSIZE 4096             /* Max Novell packet size */

/* IPX Accounting */

/*
 * IPX accounting type definitions.
 */

#define IPXACCTGENTRIESPERBLOCK 32
#define IPXACCTGDEFAULTLIMIT    (IPXACCTGENTRIESPERBLOCK * 16)
#define IPXACCTGARRAYSIZE 256	/* hash array size */
#define IPXACCTGDATAARRAYSIZE 2

/*
 * "Accounting" record used to maintain information on all ipx
 * packets. One for each source - destination ipx combination.
 */
typedef struct ipx_acctg_entry_type_ {
    struct ipx_acctg_entry_type_ *next;
    ulong srcnet;
    uchar srchost[IEEEBYTES];
    ulong dstnet;
    uchar dsthost[IEEEBYTES];
    ulong bytes;
    ulong packets;
    ulong *memory;
} ipx_acctg_entry_type;

typedef struct ipx_acctg_data_type_ {
    ulong packets_tossed;
    ulong bytes_tossed;
    sys_timestamp when;
    ipx_acctg_entry_type *store[IPXACCTGARRAYSIZE];
} ipx_acctg_data_type;

/*
 * IPX accounting control record -- controls whether we accumulate
 * accounting information -- and the limits that we will go to i.e.
 * number of entries that will be saved. We also will record the
 * number of packets tossed when this threshold has been exceeded.
 */
typedef struct ipx_acctg_info_type_ {
    ulong count;	
    ulong threshold;	
    ulong generation;
    ipx_acctg_data_type *current;
    ipx_acctg_data_type *backup;
    ipx_acctg_data_type data[IPXACCTGDATAARRAYSIZE]; /* just two (2) */
} ipx_acctg_info_type;

/*
 * The following structure type is used when we request memory.
 */
typedef struct ipx_acctg_mem_type_ {
    struct ipx_acctg_mem_type_ *next;
    ulong fragments;
    ipx_acctg_entry_type entry[IPXACCTGENTRIESPERBLOCK];
} ipx_acctg_mem_type;

/*
 * This structure is used to hold what networks we consider source/sink
 * for IPX accounting data.
 */

typedef struct ipx_acctg_acc_entry_type_ {
    ulong addr_net;
    mac_address_type addr_host;
    ulong mask_net;
    mac_address_type mask_host;
} ipx_acctg_acc_entry_type;

typedef struct ipx_acctg_acc_type_ {
    int count;
    int transits_def;
    int transits;
    ipx_acctg_acc_entry_type list[0];
} ipx_acctg_acc_type;

/***********************************************************************
 * Inlines for comparing two IPX addresses...
 */
static inline boolean ipx_addr_eq (ulong n1, uchar *h1, ulong n2, uchar *h2) {
    if (n1 != n2)
      return(FALSE);
    return(ieee_equal(h1, h2));
}

static inline boolean ipx_addr_gt (ulong n1, uchar *h1, ulong n2, uchar *h2) {
    if (n1 > n2)
      return(TRUE);
    if (n1 < n2)
      return(FALSE);
    return(ieee_compare(h1, h2) > 0);
}    

/***********************************************************************
 *
 *                      IPXWAN specifications
 *
 ***********************************************************************/

/*
 * Novell IPXWAN definitions and defaults
 */

/* Default 'resend' time for Timer Request packets */
#define NOV_IPXWAN_TREQ_INT_DEFAULT (20)

/* Default maximum retry count for Timer Request packets */
#define NOV_IPXWAN_TREQ_MAX_DEFAULT (3)

/* Novell's 'magic number' "confidence identifier" for IPXWAN packets */
#define NOV_IPXWAN_CONFIDENCE_ID (0x5741534d)

/*
 * Novell IPXWAN states
 */
#define NOV_IPXWAN_STATE_DISCONNECT (0)
#define NOV_IPXWAN_STATE_TIMER_REQ_SENT (1)
#define NOV_IPXWAN_STATE_NLSP_THROUGHPUT_SENT (2)
#define NOV_IPXWAN_STATE_SLAVE_WAIT_THROUGHPUT (3)
#define NOV_IPXWAN_STATE_NLSP_DELAY_SENT (4)
#define NOV_IPXWAN_STATE_SLAVE_WAIT_DELAY (5)
#define NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ (6)
#define NOV_IPXWAN_STATE_MASTER_WAIT_ROUTER_INFO_RSP (7) 
#define NOV_IPXWAN_STATE_MASTER_CONNECT (8) 
#define NOV_IPXWAN_STATE_SLAVE_CONNECT (9)

/*
 * Maximum length of a 'state change reason' string
 */
#define NOV_IPXWAN_STATE_REASON_MAX_LEN (59)

/*
 * Timer Request Packet definition and constants
 */

/* Novell IPXWAN Timer Request Packet type and required length */
#define NOV_IPXWAN_TIMER_REQ_PKT_TYPE (0x00)
#define NOV_IPXWAN_TIMER_REQ_PKT_LEN (576)
#define NOV_IPXWAN_IPXWAN_HDR_SIZE (11)
 
/* IPX(XNS) header + IPXWAN header */
#define NOV_IPXWAN_HDRLEN (XNSHEADERBYTES + NOV_IPXWAN_IPXWAN_HDR_SIZE)

#define NOV_IPXWAN_TIMER_REQ_HDRLEN \
   (NOV_IPXWAN_TIMER_REQ_PKT_LEN - NOV_IPXWAN_HDRLEN)

#define NOV_IPXWAN_TIMER_RSP_PKT_TYPE (0x01)
#define NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_TYPE (0x02)
#define NOV_IPXWAN_RIPSAP_INFO_REQ_PKT_LEN (0x0063)
#define NOV_IPXWAN_RIPSAP_INFO_RSP_PKT_TYPE (0x03)

/* NLSP Throughput and delay Packet types and lengths */
#define NOV_IPXWAN_NLSP_THRU_REQ_PKT_TYPE (0x4)
#define NOV_IPXWAN_NLSP_THRU_REQ_PKT_LEN (576)
#define NOV_IPXWAN_NLSP_THRU_RSP_PKT_TYPE (0x5)
#define NOV_IPXWAN_NLSP_THRU_RSP_PKT_LEN (0x0035)
#define NOV_IPXWAN_NLSP_DELAY_REQ_PKT_TYPE (0x6)
#define NOV_IPXWAN_NLSP_DELAY_RSP_PKT_TYPE (0x7)
#define NOV_IPXWAN_NLSP_DELAY_RSP_PKT_LEN (576)

/* IPXWAN NAK packet type */
#define NOV_IPXWAN_NAK_PKT_TYPE (0xFF)

/* Novell IPXWAN Timer Request Packet options */

/* General 'option' fields */

/* Option accept fields */
#define NOV_IPXWAN_OPTION_ACCEPT_NO (0x00)
#define NOV_IPXWAN_OPTION_ACCEPT_YES (0x01)
#define NOV_IPXWAN_OPTION_ACCEPT_NA (0x03)

/* Option 0: Routing Type */
#define NOV_IPXWAN_OPTION_ROUTING_TYPE (0x00)
#define NOV_IPXWAN_OPTION_ROUTING_TYPE_LENGTH (0x01)

/* Router Type RIP/SAP */
#define NOV_IPXWAN_OPTION_ROUTING_TYPE_RIPSAP (0x00)

/* Router Type NLSP */
#define NOV_IPXWAN_OPTION_ROUTING_TYPE_NLSP (0x01)

/* Router Type Unnumbered RIP SAP */
#define NOV_IPXWAN_OPTION_ROUTING_TYPE_UNN_RIP (0x02)

/* Router Type On Demand, Static */
#define NOV_IPXWAN_OPTION_ROUTING_TYPE_STATIC (0x03)

/* Router Type Workstation */
#define NOV_IPXWAN_OPTION_ROUTING_TYPE_CLIENT (0x04)

/* Option FF: Pad, length variable */
#define NOV_IPXWAN_OPTION_PAD (0xFF)

/* Option 80: Compression type, length variable */
#define NOV_IPXWAN_OPTION_COMPRESSION (0x80)
#define NOV_IPXWAN_OPTION_COMPRESSION_INFO_LENGTH (0x3)

/* Option 1: Define IPX RIP/SAP info exchange */
#define NOV_IPXWAN_OPTION_RIPSAP_INFO (0x01)
#define NOV_IPXWAN_OPTION_RIPSAP_INFO_LENGTH (0x36)

/* Option 2: Define IPX NLSP info exhange */
#define NOV_IPXWAN_OPTION_NLSP_INFO (0x02)
#define NOV_IPXWAN_OPTION_NLSP_INFO_LENGTH (0x08)

/* Option 3: Define IPX NLSP Raw Throughput Data */
#define NOV_IPXWAN_OPTION_NLSP_RAW_INFO (0x03)
#define NOV_IPXWAN_OPTION_NLSP_RAW_INFO_LENGTH (0x08)

/* Option 4: Define IPX Extended Node ID exchange */
#define NOV_IPXWAN_OPTION_EXT_NODE_INFO (0x04)
#define NOV_IPXWAN_OPTION_EXT_NODE_INFO_LENGTH (0x04)

/* Option 5: Define IPX CLIENT MAC address */
#define NOV_IPXWAN_OPTION_CLIENT_INFO (0x05)
#define NOV_IPXWAN_OPTION_CLIENT_INFO_LENGTH (0x06)

#define NOV_IPXWAN_HEADER \
    uchar confidence_identifier[4]; \
    uchar packet_type; \
    uchar node_id[4]; \
    uchar sequence_number; \
    uchar num_options;

/* Novell IPXWAN packet format */
typedef struct nov_ipxwan_header_ {
    NOV_IPXWAN_HEADER
} nov_ipxwan_header;

/* Macro to define an option header */
#define NOV_IPXWAN_OPTION_HEADER \
     uchar option_number;	\
     uchar option_accept;       \
     uchar option_length[2];
#define NOV_IPXWAN_OPTION_HEADER_SIZE (4)

/* Structure to point to any option */
typedef struct nov_ipxwan_option_ {
    NOV_IPXWAN_OPTION_HEADER
} nov_ipxwan_option;

/* Structure to point to a compression option */
#define NOV_IPXWAN_COMPRESSION_INFO \
    NOV_IPXWAN_OPTION_HEADER \
    uchar compression_type; \
    uchar compression_options; \
    uchar compression_slots;

typedef struct nov_ipxwan_option_compression_info_ {
    NOV_IPXWAN_COMPRESSION_INFO
} nov_ipxwan_option_compression_info;

/* Structure to point to a routing type option */
typedef struct nov_ipxwan_option_routing_type_ {
    NOV_IPXWAN_OPTION_HEADER
    uchar routing_type;
} nov_ipxwan_option_routing_type;
#define NOV_IPXWAN_OPTION_ROUTING_SIZE (5)

/* Strucure to point to a pad option */
typedef struct nov_ipxwan_option_pad_ {
    NOV_IPXWAN_OPTION_HEADER
    uchar padding_area[1];
} nov_ipxwan_option_pad;

/* Structure to point to a RIP/SAP info option */
#define NOV_IPXWAN_RIPSAP_INFO \
    NOV_IPXWAN_OPTION_HEADER \
    uchar link_delay[2]; \
    uchar network_number[4]; \
    uchar router_name[NOVELL_SERVER_NAME_LEN];
    
typedef struct nov_ipxwan_option_ripsap_info_ {
    NOV_IPXWAN_RIPSAP_INFO
} nov_ipxwan_option_ripsap_info;

/* Structure to point to a throughput info option */
#define NOV_IPXWAN_THROUGHPUT_INFO \
    NOV_IPXWAN_OPTION_HEADER \
    uchar size[4]; \
    uchar delta[4];

typedef struct nov_ipxwan_option_thruput_info_ {
  NOV_IPXWAN_THROUGHPUT_INFO
} nov_ipxwan_option_thruput_info;

/* Structure to point to a throughput info option */
#define NOV_IPXWAN_EXT_NODE_INFO \
    NOV_IPXWAN_OPTION_HEADER \
    uchar node_id[4];

typedef struct nov_ipxwan_option_ext_node_info_ {
  NOV_IPXWAN_EXT_NODE_INFO
} nov_ipxwan_option_ext_node_info;

/* Structure to point to a nlsp info option */
#define NOV_IPXWAN_NLSP_INFO \
    NOV_IPXWAN_OPTION_HEADER \
    uchar delay[4]; \
    uchar throughput[4];

typedef struct nov_ipxwan_option_nlsp_info_ {
  NOV_IPXWAN_NLSP_INFO
} nov_ipxwan_option_nlsp_info;

/* Structure to point to a client info option */
#define NOV_IPXWAN_CLIENT_INFO \
    NOV_IPXWAN_OPTION_HEADER \
    uchar mac_address[IEEEBYTES];

typedef struct nov_ipxwan_option_client_info_ {
  NOV_IPXWAN_CLIENT_INFO
} nov_ipxwan_option_client_info;

typedef struct ipxwanhdrtype_ {
	XNSMACRO				/* Novell header */
	NOV_IPXWAN_HEADER	 		/* IPXWAN header */
} ipxwanhdrtype;

/* The typical RIPSAPinfo request packet we send */
typedef struct ipxwanripsapinfo_hdrtype_ {
    XNSMACRO			/* Novell header */
    NOV_IPXWAN_HEADER		/* IPXWAN header */
    NOV_IPXWAN_RIPSAP_INFO	/* RIP/SAP info header */
} ipxwanripsapinfo_hdrtype;

/* The throughput info request packet we send */
typedef struct ipxwanthruputinfo_hdrtype_ {
    XNSMACRO			/* Novell header */
    NOV_IPXWAN_HEADER		/* IPXWAN header */
    NOV_IPXWAN_THROUGHPUT_INFO	/* THROUGHPUT info header */
} ipxwanthruputinfo_hdrtype;

#define NOV_IPXWAN_IS_DEAF_STATE(s) \
  ((s) == NOV_IPXWAN_STATE_DISCONNECT)

#define NOV_IPXWAN_IS_CONNECT_STATE(s) \
  (((s) == NOV_IPXWAN_STATE_MASTER_CONNECT) || \
   ((s) == NOV_IPXWAN_STATE_SLAVE_CONNECT))

#define NOV_IPXWAN_IS_RETRY_STATE(s) \
  (((s) == NOV_IPXWAN_STATE_TIMER_REQ_SENT) || \
   ((s) == NOV_IPXWAN_STATE_SLAVE_WAIT_ROUTER_INFO_REQ) || \
   ((s) == NOV_IPXWAN_STATE_MASTER_WAIT_ROUTER_INFO_RSP))

#define NOV_IPXWAN_IS_REASON_TO_RESET_COMPRESSION(s) \
    (((s) == NOV_IPXWAN_STATE_DISCONNECT) || \
     ((s) == NOV_IPXWAN_STATE_TIMER_REQ_SENT))

/*
 * Novell default maximum number of Service Advertisements Per SAP update
 */
#define NOVELL_SAP_HEADERSIZE 2
#define NOVELL_SAP_MAXSAPS 7
#define NOVELL_SAP_MAXSIZE 480
#define NOVELL_SAP_MAXSIZE_TO_SAP_COUNT(x) \
  (((x) - (XNSOVERHEADBYTES +  NOVELL_SAP_HEADERSIZE)) / \
   sizeof(novell_serveridtype))

#define NOVELL_RIP_MAXSIZE 432		/* Default max novell rip size */
#define NOVELL_RIP_MAXRIPS 50
#define NOVELL_RIP_HEADERSIZE 2
#define NOVELL_RIP_MAXSIZE_TO_RIP_COUNT(x) \
  (((x) - (XNSOVERHEADBYTES +  NOVELL_RIP_HEADERSIZE)) / \
   sizeof(novellripinfotype))

/* Defaults for delay and throughput. Means "not set" use IPXWAN negotiated */
/* any non zero value will OVERRIDE IPXWAN derived values */

#define NOVELL_THROUGHPUT_DEFLT 0
#define NOVELL_LINK_DELAY_DEFLT 0

/* Data structures for SPX keepalive spoofing */

# define NOV_SPX_HEADER \
    uchar connectionctl; \
    uchar datastreamtype; \
    ushort sourceconnectid; \
    ushort destconnectid; \
    ushort sequencenumber; \
    ushort acknumber; \
    ushort allocnumber; \

typedef struct nov_spx_header_ {
    XNSMACRO
    NOV_SPX_HEADER
} nov_spx_header;

typedef struct nov_spx_spoof_ {
    struct nov_spx_spoof_ *chain; \
    sys_timestamp timelastchanged; \
    sys_timestamp timelastspoofed; \
    sys_timestamp timeadded; \
    ulong rcvcount; \
    ulong sndcount; \
    ulong destnet; \
    uchar destnode[IEEEBYTES]; \
    ushort destsocket; \
    ushort destconnectid; \
    ulong sourcenet; \
    uchar sourcenode[IEEEBYTES]; \
    ushort sourcesocket; \
    ushort sourceconnectid; \
    uchar transportctl; \
    ushort sequencenumber; \
    ushort acknumber; \
    ushort allocnumber; \
} nov_spx_spoof_record;

#define NOVELL_SPX_IDLE_DEFLT (60)
#define SPOOF_HASH_TABLE_ENTRIES (31)
#define MAX_SPOOF_ENTRIES 10
#define SPX_HANDSHAKE_CONNECT_ID 0xFFFF
#define SPX_END_OF_CONNECTION 0xFE
#define SPX_END_OF_CONNECTION_ACK 0xFF
#define SPX_DEFAULT_IDLE_TIME (60)
#define SPX_DEFAULT_CLEAR_TIME (16)

nov_spx_spoof_record *spoofhashtable[SPOOF_HASH_TABLE_ENTRIES];
nov_spx_spoof_record *spoofheadpointer;
nov_spx_spoof_record *spooftailpointer;

ulong nov_spx_idle_time;                /* when a record resets (minutes) */ 
ulong nov_spx_clear_time;               /* when the table clears (hours)  */
boolean nov_spx_initialize;             /* Does SPX need to be initialized */
sys_timestamp novell_spxagertimer;	/* SPX ager timer */

/*
 * IPX compression constants
 */

/* Minimum and maximum compression slots */
#define IPX_COMPRESSION_MIN_SLOTS (10)
#define IPX_COMPRESSION_MAX_SLOTS (256)

/* The only comporession type we know */
#define IPX_COMPRESSION_TYPE_TELEBIT (1)

/* We negotiate NO options */
#define IPX_COMPRESSION_SUPPORTED_OPTIONS (0)

/*
 * If the 'compressed packet type' is this in the first byte, it means an
 * unprocessed frame, which should not have ANY work done with it!
 */
#define IPX_COMPRESSION_INDICATES_UNPROCESSED (0xff)

/* Various Compression packet types */
#define IPX_COMPRESSION_PACKET_TYPE_COMPRESSED (0)
#define IPX_COMPRESSION_PACKET_TYPE_REGULAR (1)
#define IPX_COMPRESSION_PACKET_TYPE_CONFIRMED_INITIAL (3)
#define IPX_COMPRESSION_PACKET_TYPE_CONFIRM (5)
#define IPX_COMPRESSION_PACKET_TYPE_UNCONFIRMED_INITIAL (7)
#define IPX_COMPRESSION_PACKET_TYPE_REJECT (9)

/* The packet type is contained in the low order four bits of the flags byte */
#define IPX_COMPRESSION_FLAGS_PACKET_TYPE_MASK (0xf)

/*
 * Each type has type specific flags in the high order four bits of the flags
 * byte.
 */
#define IPX_COMPRESSION_FLAGS_TYPE_SPECIFIC_FLAGS_MASK (0xf0)

/* For COMPRESSED packet type */
#define IPX_COMPRESSION_FLAGS_COMPRESSED_SLOT_INCLUDED (0x80)
#define IPX_COMPRESSION_FLAGS_COMPRESSED_CHECKSUM_INCLUDED (0x40)
#define IPX_COMPRESSION_FLAGS_COMPRESSED_LENGTH_INCLUDED (0x20)
#define IPX_COMPRESSION_FLAGS_COMPRESSED_TASK_INCLUDED (0x10)

#define IPX_COMPRESSION_FLAGS_UNDERSTOOD \
(IPX_COMPRESSION_FLAGS_COMPRESSED_SLOT_INCLUDED | \
 IPX_COMPRESSION_FLAGS_COMPRESSED_CHECKSUM_INCLUDED | \
 IPX_COMPRESSION_FLAGS_COMPRESSED_LENGTH_INCLUDED | \
 IPX_COMPRESSION_FLAGS_COMPRESSED_TASK_INCLUDED)

/* Offsets and size in this header are dependent on the flags above... */
#define IPX_COMPRESSION_COMPRESSED_SLOT_NUMBER_OFFSET (0)
#define IPX_COMPRESSION_ONE_BYTE_LENGTH_MAX (127)
#define IPX_COMPRESSION_TWO_BYTE_LENGTH_MAX (16383)
#define IPX_COMPRESSION_THREE_BYTE_LENGTH_MAX (65534)

/* For REGULAR packet type */
#define IPX_COMPRESSION_FLAGS_REGULAR_MUST_BE_THIS (0x00)
#define IPX_COMPRESSION_REGULAR_SIZE (1)

/* For CONFIRMED INITIAL packet type */
#define IPX_COMPRESSION_FLAGS_CONFIRMED_INITIAL_MUST_BE_THIS (0x00)
/* Offsets in header */
#define IPX_COMPRESSION_CONFIRMED_INITIAL_SLOT_NUMBER_OFFSET (0)
#define IPX_COMPRESSION_CONFIRMED_INITIAL_ID_OFFSET (1)
/* Fixed size */
#define IPX_COMPRESSION_CONFIRMED_INITIAL_SIZE (3)


/* For CONFIRM packet type */
#define IPX_COMPRESSION_FLAGS_CONFIRM_MUST_BE_THIS (0x00)
/* Offsets in header */
#define IPX_COMPRESSION_CONFIRM_SLOT_NUMBER_OFFSET (0)
#define IPX_COMPRESSION_CONFIRM_ID_OFFSET (1)
#define IPX_COMPRESSION_CONFIRM_SIZE (3)

/* For UNCONFIRMED INITIAL packet type */
#define IPX_COMPRESSION_FLAGS_UNCONFIRMED_INITIAL_MUST_BE_THIS (0x00)
/* Offsets in header */
#define IPX_COMPRESSION_UNCONFIRMED_INITIAL_SLOT_NUMBER_OFFSET (0)
#define IPX_COMPRESSION_UNCONFIRMED_INITIAL_SIZE (2)


/* For REJECT packet type */
#define IPX_COMPRESSION_FLAGS_REJECT_MUST_BE_THIS (0x00)
/* Offsets in header */
#define IPX_COMPRESSION_REJECT_SLOT_NUMBER_OFFSET (0)
#define IPX_COMPRESSION_REJECT_REJECTED_FLAGS_OFFSET (1)
#define IPX_COMPRESSION_REJECT_SIZE (3)

/* A compressed header consists of one flags byte plus up to 7 bytes data */
#define IPX_COMPRESSION_MAX_HEADER_DATA (7)
typedef struct {
    uchar flags;
    uchar data[IPX_COMPRESSION_MAX_HEADER_DATA];
} ipx_compressed_header;

/*
 * Minimum time, in seconds, between successive queries to the SAP backup
 * table. This implements some period of quiet in frequent update/change
 * storms
 */
#define NOVELL_DEFAULT_BACKUP_SERVER_INTERVAL (60)

/*
 * Initial holddown time (mSec) for periodic and other non-flash updates:
 */

#define NOV_UPDATE_HOLDDOWN (5)


#define NOV_INDEX2PDB(index) (nov_proctable[index])

/*
 * Novell access lists (new numbering)
 */
#define IPX_MIN_STD_ACL   1
#define IPX_MAX_STD_ACL 100
#define IPX_MIN_EXT_ACL 101
#define IPX_MAX_EXT_ACL 200
#define IPX_MIN_SAP_ACL 201
#define IPX_MAX_SAP_ACL 300
#define IPX_MIN_SUM_ACL 301
#define IPX_MAX_SUM_ACL 400


#include "novroute.h"


/*
 * This structure defines the current state of an NIDB having output done.
 * It allows the novell_sendupdate and novell_sendripupdate code to be
 * reentrant.
 */

/*
 * Update state variable:
 */
typedef struct nov_usv_ {
  /*
   * Request descriptor:
   */
  nidbtype *nidb;
  uchar const *host;
  uchar host_addr[IEEEBYTES];
  ushort sock;
  enum IPX_FULL full;
  enum IPX_DESTINATION dest;
  ushort servertype;
  /*
   * Parameters specific to SAP name query request:
   */
  ushort query_type; /* Response or update */
  ulong dnet;        /* Server network to match, possibly with mask */
  ulong dmask;       /* Modifier for server network */
  uchar server_name[NOVELL_SERVER_NAME_LEN]; /* Name, maybe with wildcards */
  
  /*
   * Request state:
   */
  ulong state_start;	 	     /* Value of 'NIDB state' when started */
  boolean rip_request;               /* Update is RIP */
  boolean sap_request;               /* Update is SAP */
  sys_timestamp lastupdate;	     /* Tmp last update time */
  sys_timestamp last_update_time;    /* Time the last update was sent */
  /* Route selector function */
  boolean (*adv_rip)(novndbtype *entry,
		     struct nov_usv_ *usv,
		     nidbtype *nidb,
		     sys_timestamp lastupdate,
		     enum IPX_FULL fullupdate,
		     enum IPX_DESTINATION destination,
		     boolean reverse);
  /* Service selector function */
  boolean (*adv_sap)(novell_sapentrytype *entry,
		     struct nov_usv_ *usv,
		     nidbtype *nidb,
		     sys_timestamp lastupdate,
		     enum IPX_FULL fullupdate,
		     enum IPX_DESTINATION destination,
		     boolean reverse, boolean linkdown,
		     boolean *eigrp_must_poison);
  novndbtype *ndb_placeholder;	     /* Current update placeholder */
  novell_sapentrytype *sap_placeholder; /* Current update placeholder */
  sys_timestamp rip_start_timestamp; /* Time the update began */
  sys_timestamp sap_start_timestamp; /* Time the update began */
  paktype *update_pak;		     /* Update packet being built.. */
  novell_sappacket *sap;	     /* Current SAP pointer */
  novellriptype *rip;		     /* Current RIP pointer */
  int rip_update_count;		     /* Current update count into RIP pkt */
  int sap_update_count;		     /* Current update count into SAP pkt */
  int rip_update_total;		     /* Total update count (packets) */
  int sap_update_total;		     /* Total update count (packets) */
  int rip_bytes;		     /* Bytes left in building RIP frame */
  novellripinfotype *rip_ptr;	     /* ptr to next entry in current RIPpkt */
  boolean error_occured;	     /* Error encountered during update */
  boolean sent_an_update;	     /* TRUE if any updates sent */
  boolean placeheld;                 /* Placeholder in "newest" list */
  boolean  reverse_poison;           /* Request is reverse poison */
  boolean linkdown_poison;           /* Request is linkdown poison */
  boolean active;                    /* Update active */
  boolean pending;                   /* New update request made while active */
  boolean sap_after_rip;             /* Send SAP update only following RIP */
  int gap;                           /* Interpacket gap for this request */
  mgd_timer timer;                   /* Timer for pausing between packets */
} nov_usv;

/*
 * Novell virtual network descriptor. There may be several of these
 * per physical interface.
 */
struct nidbtype_ {
  struct nidbtype_ *next;
  ulong    novellnet;                /* Network number */
  int      enctype;                  /* Encapsulation */
  boolean  explicit_encap;           /* Encapsulation is set explicitly */
  idbtype  *idb;                     /* Physical IDB */
  char     *pname;                   /* nlsp router tag name */
  boolean  internal_network;	     /* Internal network */
  boolean  up_pending;	    	     /* Come up after down */
  boolean  node_pending;	     /* Node address change pending */
  int      static_routes;            /* Static routes via this net */
  sys_timestamp rip_lastupdate;      /* Time of last rip update */
  sys_timestamp sap_lastupdate;      /* Time of last SAP update */
  sys_timestamp rip_revp_lastupdate; /* Time of last reverse poison */
  sys_timestamp sap_revp_lastupdate; /* Time of last reverse poison */
  sys_timestamp lastupdate;	     /* Temp. used by SAP and RIP senders */

/* State variables (usv) for various predefined/preallocated updates */
  nov_usv *rip_full;                 /* full */
  nov_usv *rip_part;                 /* partial */
  nov_usv *rip_down;                 /* linkdown */
  nov_usv *rip_revp;                 /* reverse poison */

  nov_usv *sap_full;                 /* full */
  nov_usv *sap_part;                 /* partial */
  nov_usv *sap_down;                 /* linkdown */
  nov_usv *sap_revp;                 /* reverse poison */

  mgd_timer rip_update_timer;        /* Periodic RIP update timer */
  mgd_timer sap_update_timer;        /* Periodic SAP update timer */

  ulong    trig_rip_delay;           /* Delay between packets non-updates */
  ulong    trig_sap_delay;           /* Delay between packets non-updates */
  boolean  old_style;                /* Remember nets cfg'd w/o encap */
  boolean  adv_default_route_only;   /* Advertise only the default route */
  boolean novell_sap_after_rip;      /* Send periodic sap updates after rip */
  ulong	   state;		     /* State of this NIDB */
  boolean doing_linkdown_poisons;
  boolean doing_reverse_poisons;     /* Doing reverse poisons out this NIDB */
  boolean nov_sent_linkdown_poisons; /* Sent poisons for flapping linkdown? */
  boolean igrp_sap_updates;	     /* Send saps via igrp reliable transport */
  boolean igrp_sap_only;	     /* Use IGRP for SAP updates *only* */
  boolean novell_interface_up;	     /* Itf at last route_adjust() callback */
  boolean administratively_shutdown; /* User issued 'down' command */
  ulong   state_version;	     /* Tells bkground procs this changed */
  int     iprotofltr[MAX_NOVPROCS];  /* list for filtering input */
  int     oprotofltr[MAX_NOVPROCS];  /* list for filtering output */
  int     sap_iprotofltr[MAX_NOVPROCS];  /* list for sap filtering input */
  int     sap_oprotofltr[MAX_NOVPROCS];  /* list for sap filtering output */

  /* IPX/NCP compression information */
  boolean compression_enabled;	     /* IPX/NCP compression enabled? */
  ulong   compression_uncompressed_bytes_out;
                                   /* Total bytes out interface if uncomp. */
  ulong   compression_sent_cipx_regular;    /* CIPX sent regular */
  ulong   compression_sent_cipx_compressed; /* CIPX sent compressed */
  ulong   compression_sent_cipx_cinitial;   /* CIPX sent confirmed Initial */
  ulong   compression_sent_cipx_uinitial;   /* CIPX sent unconfirmed Intial */
  ulong   compression_sent_cipx_confirm;    /* CIPX sent unconfirmed Intial */
  ulong   compression_sent_cipx_reject;     /* CIPX sent rejects */

  ulong   compression_rx_cipx_regular;    /* CIPX rcvd regular */
  ulong   compression_rx_cipx_compressed; /* CIPX rcvd compressed */
  ulong   compression_rx_cipx_cinitial;   /* CIPX rcvd confirmed Initial */
  ulong   compression_rx_cipx_uinitial;   /* CIPX rcvd unconfirmed Intial */
  ulong   compression_rx_cipx_confirm;    /* CIPX rcvd unconfirmed Intial */
  ulong   compression_rx_cipx_reject;     /* CIPX rcvd rejects */

  ulong   compression_bytes_saved;   /* Total bytes saved by compression */
  boolean compression_negotiated_enabled; /* Negotiated compression values */
  uchar   compression_slots;
  uchar   compression_negotiated_type;
  uchar   compression_negotiated_options;
  uchar   compression_negotiated_slots;
  uchar   compression_most_recently_used_slot;   /* Most recently used slot */
  uchar   compression_least_recently_used_slot;  /* Least recently used */
  uchar   alignment0[2];
  ipx_ncp_compression_slot *inbound_compression_slots; /* Array of slots */
  ipx_ncp_compression_slot *outbound_compression_slots;/* Array of slots */
   
  /* client information */
  queuetype clientQ;	/* clients attached to this interface */

  ulong novell_spoof_flag;           /* Answer Watchdog/SPX-keep packets? */

  char    ipxwan_link_reset_process_name[IPX_PROCESS_NAME_STORAGE];
  char    alignment1[1];

  /*
   * IPXWAN specific information
   */
  boolean nov_ipxwan_enabled;           /* Is IPXWAN enabled on this link? */
  boolean nov_ipxwan_need_ripsap; 	/* Does IPXWAN *need* RIP/SAP? */
  ulong nov_ipxwan_internal_net; 	/* Internal net number for this link */
  ulong nov_ipxwan_external_net; 	/* External net number for this link */
  ulong nov_ipxwan_treq_count;	/* Times we tried sending timer req. */
  ulong nov_ipxwan_treq_expect;	/* Value that we expect */
  ulong nov_ipxwan_treq_int; 	/* How often to send a timer req */
  ulong nov_ipxwan_treq_max; 	/* Maximum number of times to try */
  ulong nov_ipxwan_state;		/* Current IPXWAN state of this link */
  sys_timestamp nov_ipxwan_pak_sent_time; /* Time packet last sent (msec) */
  sys_timestamp nov_ipxwan_treq_next; 	/* When next to send an update */
  sys_timestamp nov_ipxwan_link_delay_timer; /* delay of current echo */
  sys_timestamp nov_ipxwan_link_throughput_timer;
  uchar nov_ipxwan_routername[49]; /* Novell Router Name for this link */
  uchar nov_ipxwan_remote_routername[49]; /* Name of remote side */
  char alignment2[2];
  ulong nov_ipxwan_remote_internal_net; /* Internal net of remote */
  ulong nov_ipxwan_state_reason[60]; /* Reason state changed */
  ulong nov_ipxwan_delay;	/* IPXWAN computed delay */
  ulong nov_ipxwan_link_delay;  /* negotiated NLSP link delay */
  ulong nov_ipxwan_link_throughput;  /* negotiated NLSP link throughput */
  ulong nov_ipxwan_routing_type; /* The routing type IPXWAN negotiated */
  ulong novell_spxidletime;     /* SPX idle timer, time before we spoof */
  boolean nov_ipxwan_client;     /* client mode enabled */
  boolean nov_ipxwan_static;     /* static mode enabled */
  enum IPX_IPXWAN_ERROR_CFG nov_ipxwan_error; /* what to do on error */
  uchar nov_ipxwan_clnt_address[IEEEBYTES]; /* If client it's address */
  char alignment3[2];
  /*
   * Manual settings for link delay and throughput (for NLSP)
   * these over-ride the above IPXWAN link thoughput and delay if not 0.
   * see NOVELL_LINK_DELAY and NOVELL_LINK_THROUGHPUT above for usage.
   */
  ulong novell_link_delay;              /* Link Delay in microseconds */
  ulong novell_link_throughput;         /* Link Throughput in bits per sec */

  /*
   * IS-IS/NLSP specific information
   */
  boolean nov_lsp_enabled;		/* NLSP or IS-IS enabled on this i/f */
  boolean nov_lsp_isis;			/* TRUE if IS-IS is running */
  boolean nov_mcast_capable;		/* All systems can rcv multicast */
  ushort nov_nlsp_if_state;		/* Interface state for NLSP WAN */
  ushort nov_nlsp_media;		/* Media type for NLSP */
  /*
   * RIP/SAP compatibility mode switches:
   * _cfg indicates what the user configured. _on indicates if the
   * compatibility is ON right now, be it 'auto' or configured. _timestamp
   * is used by 'auto' mode to turn off later.
   */
  enum IPX_RIPSAP_CONFIG lsp_rip_cfg;
  boolean lsp_rip_on;
  sys_timestamp lsp_rip_timestamp;

  enum IPX_RIPSAP_CONFIG lsp_sap_cfg;
  boolean lsp_sap_on;
  sys_timestamp lsp_sap_timestamp;
};
/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern pid_t novell_ipxwanbg_pid;		/* IPXWAN Background */
extern pid_t novell_ipxwan_pid;       		/* IPXWAN */
extern novpdbtype *nov_proctable[MAX_NOVPROCS];
extern struct radix_node_head *nov_rnhead;
extern nidbtype *nullnidb;
extern boolean ipx_eigrp_sap_split_horizon;  /* EIGRP SAP has split horizon */

/*
 * Novell Queues
 */
extern watched_queue *novQ;		/* Novell input queue */
extern queuetype novechoQ;
extern queuetype novbgQ;		/* Novell services queue */
extern queuetype novell_gnsQ;		/* Queue of Novell GNS Requests */
extern queuetype novell_ipxwanQ; 	/* packet queue for IPXWAN */
extern watched_queue *novell_snmpQ; 	/* packet queue for SNMP */
extern queuetype novell_genericQ;       /* echo reply, and helpered packets */
extern queuetype novell_sapQ;		/* Queue of Novell SAPs (table) */
extern queuetype novpdbQ;
extern queuetype nov_router_initQ;
extern queuetype nov_freed_ndbQ;	/* Freed NDB queue for later proc. */
extern queuetype nov_free_nidbQ;	/* Free nidb's available for recycle */


/* Novell helpernet queue */
/***********************************
extern queuetype nov_helpernetQ;
************************************/


/*
 * Novell Global Variables
 */
extern nidbtype *ipx_internal_network_nidb;
extern boolean novell_backup_servers_needed; 	/* We need some SAP info */
extern boolean ipx_eigrp_sap_split_horizon; /* IPX EIGRP SAP Split horizon? */
extern boolean ipx_server_split_on_server_paths;
extern boolean ipx_kludge_enabled_flag;		/* Test specific kludges */
extern boolean novell_nlsp_enabled;	      /* A quick check. Is it there? */
extern boolean novigrp_enabled;	              /* A quick check. Is it there? */
extern sys_timestamp novell_backup_servers_timer; /* Timer is useful */
extern ulong   novell_max_hops;			/* Configurable maximum hops */
extern ulong   novell_infinity;			/* Max Hops + 1  */
extern pid_t   novell_inp_pid;       		/* Novell Input PID */
extern pid_t   novell_snmp_pid;			/* SNMP PID for IPX */
extern ulong    ipx_checkpointvalue; 		/* IPX checkpointvalue */
extern ulong novell_route_count;
extern ulong novell_server_count;
extern nidbtype   *novell_next_output_nidb;
extern novpdbtype novconnected_pdb;
extern novpdbtype novstatic_pdb;
extern novpdbtype novfstatic_pdb;
extern novpdbtype novrip_pdb;
extern boolean novelligrp_debug, novelligrp_event_debug;
extern boolean novell_debug, novellrip_debug, novellsap_debug;
extern boolean novellrip_event_debug, novellsap_event_debug;
extern boolean novellipxwan_debug;
extern boolean novellisis_debug, novellisis_event_debug;
extern boolean novellnlsp_debug, novellnlsp_event_debug;
extern boolean novellcompression_debug, novellcompression_event_debug;
extern boolean novellredist_debug;
extern boolean novell_verbose_debug;
extern boolean novell_private_debug;
extern boolean novell_ripupd_debug;
extern boolean novell_sapupd_debug;
extern boolean novell_spoof_debug;
extern boolean novell_default_route;
extern novndbtype *novell_default_ndb;
extern boolean novell_running;
extern boolean novell_shutting_down;
extern boolean novell_algorithms;
extern boolean novell_sap_uses_routing;
extern boolean novell_potential_pseudonode;
extern boolean novell_started;
extern boolean nov_gns_roundrobin;
extern boolean nov_load_share_per_host;
extern boolean novell_type20_in_checks;
extern boolean novell_type20_out_checks;
extern boolean novell_netbios_in_checks;
extern boolean novell_type20_helpered;
extern boolean novell_fastswitch_bcast;
extern boolean novell_global_acl_checks;
extern long novcache_version;
extern ulong novell_freed_ndbq_wakeup;
extern nov_routetable *novell_table;
extern nov_statictable *static_table;
extern nov_backup_routetable *novell_table2;
extern nov_backup_saptable *novell_table3;
extern nov_urd_table_type *nov_urd_table; /* Unreachable sap routes */
extern ulong novell_urd_insert_count;
extern ulong novell_urd_callback_count;
extern ulong novell_urd_scrub_count;
extern novroutetype *nov_pessimal_route_list;
extern nov_svc_type *nov_pessimal_sap_list;
extern queuetype novidbQ;
extern nov_saproutetable *novell_sap_route_hashtable;
extern ulong novell_stat[];
extern ulong nov_last_bad_sap_net;
extern uchar nov_last_bad_sap_host[IEEEBYTES];
extern ulong nov_last_bad_rip_net;
extern uchar nov_last_bad_rip_host[IEEEBYTES];
extern ushort novell_maxpaths;
extern ushort novell_cfg_maxpaths;
extern ulong novell_sapqueuemax;
extern ulong novell_gns_response_delay;
extern enum IPX_ECHO_TYPE novell_ping_deflt;
extern boolean novell_rip_running;
extern boolean novell_igrp_running;
extern int novell_nlsp_running;
extern ulong novell_backup_server_interval;
extern ulong novell_update_proc_throttle;
extern ulong novell_update_sched_throttle;
extern ulong novell_def_output_rip_dly;
extern ulong novell_def_output_sap_dly;
extern ulong novell_def_trig_rip_dly;
extern ulong novell_def_trig_sap_dly;
extern boolean novell_auto_sw_supported;
extern ulong novell_routes;
extern novndbtype *ipx_pak_ndb_for_cache;
/*
 * New RIP/SAP wakeup timers and booleans:
 */
extern mgd_timer novell_rip_delay_timer;
extern mgd_timer novell_sap_delay_timer;
extern mgd_timer novell_super_root_timer;
extern mgd_timer novell_atlr_timer;

extern watched_boolean *novell_rip_part_wb;
extern watched_boolean *novell_sap_part_wb;
extern watched_boolean *novell_link_down_wb;
extern watched_boolean *novell_sent_down_wb;

extern ulong novell_debug1, novell_debug2, novell_debug3, novell_debug4;
extern ulong novell_debug5, novell_debug6, novell_debug7, novell_debug8;

extern ulong novell_debug_net1, novell_debug_net2;
extern uchar novell_debug_host1[IEEEBYTES];
extern uchar novell_debug_host2[IEEEBYTES];
extern ulong ipx_internal_net;                  /* System global IPX number */
extern uchar ipx_internal_hostid[IEEEBYTES];    /* 0000.0000.0001 */
extern uchar ipx_internal_name[NOVELL_SERVER_NAME_LEN];  /* System Name */
extern uchar novell_allhosts[];
extern ipx_acctg_info_type ipx_acctg_info;	/* accounting data structure */
extern ipx_acctg_acc_type *ipx_acctg_acc;	/* access information for networks */
extern queuetype ipxacctQ;			/* local queue of memory */
extern queuetype ipxmemQ;			/* REAL memory obtained */
extern boolean ipx_acctg_on;                   /* whether we do accounting */
extern novell_sapentrytype *novell_newest_servers;
extern novndbtype *novell_newest_routes;
extern novndbtype novell_atlr_head;
extern int nov_static_route_count;
extern int nov_static_server_count;
extern uchar nov_defaddr[IEEEBYTES];
extern boolean ipxwan_enabled;

/*
 * Novell Function Declarations
 *        xns/novecho.c
 *        xns/novell.c
 *        xns/novinput.c
 *        xns/novell_chain.c
 *	  xns/novriproute.c
 *	  xns/novipxwan.c
 *	  xns/ipxaccount.c
 */

extern void novell_set_nidb_rip_mode(nidbtype *, enum IPX_RIPSAP_CONFIG);
extern void novell_set_nidb_sap_mode(nidbtype *, enum IPX_RIPSAP_CONFIG);
extern void novell_start_nidb_rip_auto(nidbtype *);
extern void novell_start_nidb_sap_auto(nidbtype *);
extern void novell_stop_nidb_rip_auto(nidbtype *);
extern void novell_stop_nidb_sap_auto(nidbtype *);
extern void nov_hwaddr_notify(idbtype *, uchar *, ushort);
extern void nov_mac_address_change(hwidbtype *);
extern void novell_sap_poison(novell_sapentrytype *);
extern void nov_update_global_acl_flags(void);
extern void ipx_acct_init(void);
extern void ipx_accumulate_acctg(ipxhdrtype *ipx);
extern void ipx_acctlist(parseinfo *);
extern void show_ipxaccounting(parseinfo *);
extern void clear_ipx_accounting(parseinfo *);
extern void clear_ipx_accounting_func(boolean);
extern void ipx_acct_fixup(boolean);
extern boolean novell_auto_sw_active(void);
extern boolean novell_sse_sw_active(void);
extern void novrdb_free(novrdbtype *);
extern void novsrdb_free(novell_sapentry_rdb *);
extern void novell_delete_network(nidbtype *);
extern void novell_nidb_bringup(nidbtype *);
extern void novell_nidb_bringdown(nidbtype *);
extern void novell_nidb_newstate(nidbtype *, ulong);
extern void novell_mark_changed(novndbtype *);
extern void novell_mark_newest(novndbtype *);
extern void novell_mark_rdb_changed(novndbtype *, novrdbtype *, ulong,
				    enum IPX_ROUTE_EVENT);
extern void nov_network_finish(novpdbtype *, int, boolean);
extern boolean allocate_novpdbindex(novpdbtype *);

boolean novell_client_add(nidbtype *, idbtype *);
boolean novell_client_delete(nidbtype *, idbtype *);
void novell_pppclient_configure(idbtype *, idbtype *, boolean);
idbtype *novell_get_owner_idb(idbtype *);

extern void novell_mark_advertise_to_lost(novndbtype *);
extern boolean novrip_init_backup_tables(void);
extern void novrip_age_backup_routes(void);
extern void novrip_delete_all_backup_data(ulong, ulong);
extern boolean novrip_inject_one_backup_route(ulong, ulong);
extern void novrip_inject_all_backup_routes(boolean, novpdbtype *);
extern void novrip_inject_all_backup_services(boolean, novpdbtype *);
extern void novrip_process_backup_route(novpdbtype *, nidbtype *,
					ulong, ulong, ushort, ulong,
					uchar *, ushort, ushort, novpdbtype *);
extern void novrip_process_backup_service(novpdbtype *, nidbtype *,
					  novell_serveridtype *, ulong,
					  uchar *, ulong);
extern void novell_init_atlr_list(void);
extern void novell_remove_from_atlr_list(novndbtype *);
extern void novell_insert_into_atlr_list_tail(novndbtype *);
extern novndbtype *novell_get_atlr_list_head(void);
extern int novell_count_atlr_list_length(void);
extern void novell_mark_rip_changed(novndbtype *);
extern void novell_update_sap_route_info(novndbtype *);
extern boolean novell_sap_split_horizon(novell_sapentrytype *, nidbtype *,
					enum IPX_ADVERTISING);
extern boolean nov_accesscheck(paktype *, int);
extern boolean nov_saproutercheck(ulong, uchar *, ushort, char *, int);
extern boolean nov_allnets_test(paktype *);
extern void nov_flood(paktype *, novndbtype *, nidbtype *, enum IPX_FLOOD);
extern boolean nov_forme(paktype *);
extern boolean nov_multicast_addr(hwidbtype *, uchar);
extern boolean nov_sapcheck(novell_serveridtype *, ushort, int);
extern boolean nov_send(paktype *);
extern boolean nov_send1(paktype *, uchar const *, uint);
extern boolean nov_watchdogspoof(paktype *);
extern boolean novellBLOCK(queuetype *);
extern boolean novell_forward(paktype *);
extern boolean novell_routeBLOCK(queuetype *);
extern boolean nov_type20_output_loop(paktype *pak, ulong dnet);
extern boolean parse_novelladdress(char *, ulong *, uchar *);
extern boolean send_novecho(long, uchar *, xnsechotype *, int,
			    enum IPX_ECHO_TYPE);
extern uchar novecho_byte(uchar, ulong *);
extern ushort novecho_rand(ulong *);
extern boolean novecho_test(long, uchar *, xnsechotype *, int,
			    ushort, uchar, ulong *, ulong);
extern boolean novecho_testreply(paktype *, xnsechotype *);
extern process nov_input(void);
extern forktype novell_fork(void);
extern int novell_count_routes(void);
extern forktype novell_forwarder(void);
extern forktype novell_router(novpdbtype *);
extern boolean nov_set_nidb_idb(paktype *);
extern int nov_forus(paktype *);
extern void display_novell(idbtype *);
extern void nov_log_stats(int, paktype *);
extern void nov_format_dispose(paktype *);
extern void nov_dispose(int, paktype *);
extern void nov_echo(paktype *);
extern void nov_error(paktype *, int, int, int);
extern void nov_forward(paktype *);
extern void nov_init_idb(idbtype *, boolean);
extern void nov_init(subsystype *);
extern void nov_netbios_buginf(paktype *, char *);
extern void novell_addserver(novpdbtype *, novell_serveridtype *,
			     nidbtype *, uchar *, ulong, ulong,
			     ushort, nov_cost_type *,
			     isis_lsp_linkage **, ushort, ushort, boolean);
extern void novell_rxtypes_update(idbtype *);
extern void novell_enqueue(paktype *);
extern void novell_printpak(paktype *, char *);
extern void novell_printnamepak(paktype *, char *);
extern void novell_printrippak(paktype *);
extern void novell_prune_nidb_sapq(nidbtype *);
extern void novell_rcv_query(paktype *, novell_querytype *);
extern void novell_rcv_name_query(paktype *, novell_querynametype *);
extern boolean nov_good_query_packet(paktype *, novell_querytype *);
extern forktype novell_rcv_nearquery(void);
extern void novell_rcv_response(paktype *, novell_sappacket *);
extern void novell_rcvriprequest(paktype *, novellriptype *);
extern void novell_riprcvupd(paktype *, novellriptype *, boolean);
extern void novell_set_timer(novrdbtype *, novell_sapentry_rdb *);
extern void novell_posit_backup_servers_needed(void);
extern boolean novell_posit_sap_query_response(nidbtype *, ulong,
					       uchar const *, ushort,
					       enum IPX_FULL,
					       enum IPX_DESTINATION,
					       ushort, ushort,
					       uchar *, ulong, ulong);
extern boolean novell_posit_sap_update(nidbtype *,
				       ulong,
				       uchar const *,
				       ushort,
				       enum IPX_FULL,
				       enum IPX_DESTINATION,
				       ushort,
				       boolean,
				       boolean);
extern boolean novell_posit_rip_update(nidbtype *,
				       ulong,
				       uchar const *,
				       ushort,
				       enum IPX_FULL,
				       enum IPX_DESTINATION,
				       boolean,
				       boolean);
extern void novell_send_gen_rip_query(nidbtype *);
extern void novell_send_gen_sap_query(nidbtype *, ushort);
extern void novell_start(void);
extern boolean novell_ping_parse(char *, int, int *, char **, boolean *);
extern void nov_ping(char *);
extern void test_novell_echo(parseinfo *csb);
extern void novecho_test_driver(long, uchar *, ulong, ulong, ulong, ulong, ushort, uchar, ulong, ulong);
extern void nv_novaccess(parseinfo *);
extern void show_novaccess(int);
extern void nov_quicksort (ptable *, int, int, int,
			   boolean (*)(void *, void *));
extern void show_novell_servers(int, boolean, uchar *);
extern void show_novellroute(ulong, boolean);
extern void show_novelltraffic(void);
extern void show_novell_spx_spoof(void);
extern void novell_clear_spx_spoof(void);
extern void novell_returnspoofrecord(nov_spx_spoof_record  *);
extern void novell_send_response (paktype *, paktype *, boolean, int);
extern boolean novell_netbios_acfcheck(paktype *,
				       enum IPX_ACCESS_DIRECTION, nidbtype *);
extern boolean novrip_interface_up(novrdbtype *);
extern boolean nov_same_idb(idbtype *, novrdbtype *);
extern boolean nov_same_nidb(nidbtype *, novrdbtype *);
extern boolean nov_srdb_same_idb(idbtype *, novell_sapentry_rdb *);
extern novndbtype *novrip_find_rt(ulong, ulong, enum IPX_ROUTE_MATCH_TYPE);
extern void novrip_network_command(novpdbtype *, ulong, boolean);
extern uchar *novrip_first_hop(ulong, uchar *, nidbtype **,
			       ulong *, novndbtype *,
			       enum IPX_ROUND_ROBIN,
			       enum IPX_DEFAULT_ROUTE);
extern void novrip_add_rt(novpdbtype *, nidbtype *, ulong, ulong, ulong, 
			  uchar *, int, int, ulong, enum IPX_ROUTE_ADD,
			  enum IPX_STATIC_DEFINE_TYPE, enum IPX_STATIC_TYPE,
			  enum IPX_FLOATING_STATIC_EXISTS);
extern void nov_add_floating(novpdbtype *, ulong, ulong, ulong, ulong);
extern void novrip_shutdown(void);
extern void novrip_ager(void);
extern void novrip_del_rt(ulong, ulong, enum IPX_ROUTE_DELETE);
extern void nov_del_floating(ulong, ulong);
extern void novrip_prune_nidb(nidbtype *);
extern void novrip_route_adjust(idbtype *);
extern void novrip_static_routes(nidbtype *, boolean);
extern boolean novrip_exist_associated_statics(nidbtype *);
extern void novrip_delete_associated_statics(nidbtype *);
extern void novrip_update(nidbtype *, ulong, ulong, ushort, ulong, uchar *,
			  ushort, ushort, backuptype *);
extern void show_novriproutes(ulong, boolean);
extern void novrip_display_backup_routes(ulong, ulong);
extern void novrip_purgetable(void);
extern void clear_novell_command(parseinfo *);
extern void novell_paths_command(boolean, boolean, int);
extern void novell_rip_multiplier_ifcommand (boolean, idbtype *, int);
extern void novell_sap_multiplier_ifcommand (boolean, idbtype *, int);
extern void novell_gns_reply_disable_ifcommand (boolean, idbtype *);
extern void novell_updatetime_ifcommand (boolean, idbtype *, int);
extern void novell_rip_max_packetsize_ifcommand (boolean, idbtype *, int);
extern void novell_sap_max_packetsize_ifcommand (boolean, idbtype *, int);
extern void novell_sapint_ifcommand (boolean, boolean, idbtype *, int);
extern void novell_address_command (boolean, boolean, ulong, uchar *);
extern void novell_internal_net_command (boolean, boolean, ulong);
extern void nov_nlsp_router_command (boolean,  boolean, parseinfo *,
				     novpdbtype *, char *);
extern void nov_route_command(boolean, boolean, int, int, uchar *,
			      int, int, int, idbtype *, boolean, ulong, int,
			      enum IPX_STATIC_DEFINE_TYPE);
extern void nov_sap_command(boolean, boolean, uchar *, int, int, uchar *, int,
			    int, enum IPX_STATIC_DEFINE_TYPE);
extern void novell_ipxwan_enable_ifcommand(boolean, boolean, idbtype *, int,
					   int, char *, int, int, int);
extern void novmisc_pdbinit(void);
extern void novell_common_restart(void);
extern void novell_common_cleanup(void);
extern void novrip_pdbinit(void);
extern void novell_parser_init(void);
extern void novell_start_aging(idbtype *, paktype *);
extern forktype novell_ipxwan(void); 
extern forktype novell_ipxwan_bg(void); 
extern void nov_ndb_free_newest(novndbtype *);
extern void nov_ndb_free(novndbtype *, enum IPX_ROUTE_FREE, boolean);
extern novndbtype *nov_ndb_malloc(void);
extern novndbtype *nov_ndb_malloc_placeholder(void);
extern void nov_insert_ndb_placeholder(novndbtype *, novndbtype *);
extern novndbtype *nov_delete_ndb_placeholder(novndbtype *);
extern novndbtype *nov_get_first_ndb(void);
extern novndbtype *nov_get_next_ndb(novndbtype *);
extern void nov_get_backup_servers(void);
extern void novell_accept_sap_data(novpdbtype *, paktype *,
				   novell_sappacket *,
				   ulong, char *,
				   novell_accept_sap_data_function,
				   dual_peertype *, novpdbtype *);
extern int nov_sap_hash (novell_serveridtype *);
extern void nov_sap_command(boolean, boolean, uchar *, int, int, uchar *,
			    int, int, enum IPX_STATIC_DEFINE_TYPE);
extern void novell_media_registry(void);
extern void nov_access_delete(int);
extern boolean nov_filtercheck(ulong, int);
extern boolean novrip_out_filter(novndbtype *ndb, ulong net, idbtype *idb);
extern void novell_nidb_set_network_number(nidbtype *, ulong);
extern boolean nov_nonreliable_ndb_on_free_queue(ulong, ulong);
extern void register_snmp_ipx_echo(void);
extern void novell_swif_erase(idbtype *);
extern novndbtype *nov_get_each_route(novndbtype *, int *, boolean);
extern novndbtype *nov_get_each_route_per_index(novndbtype *, int *, boolean);
extern novell_sapentrytype *nov_get_each_service(novell_sapentrytype *, int *,
						 boolean);
extern novell_sapentrytype *nov_get_each_service_per_index
                                (novell_sapentrytype *, int *, boolean);
extern void novell_mark_srdb_changed(novell_sapentrytype *,
				     novell_sapentry_rdb *, ulong,
				     enum IPX_ROUTE_EVENT);
extern void nov_display_urd_table(ulong); /* URD table */
extern void nov_clean_urd_table(novpdbtype *pdb);
extern void nov_insert_urd(nov_urd_type *urd);
extern void nov_update_urd_list(novpdbtype *pdb, ulong dnet,
				backuptype *binfo, boolean);
extern void nov_accesslog_cache_print(void);

/*
 * novell_setup.c
 */
extern void novell_setup_global(setup_struct *);
extern void novell_setup_global2(setup_struct *);
extern void novell_setup_int(setup_struct *);
extern void novell_setup_end(setup_struct *);
/*
 * novell_test.c
 */
extern void novell_test_init(void);
/*
 * ipx_snmp.c
 */
extern void ipx_snmp_init(void);

extern uchar *novdbg_proctype2string (ulong proctype, boolean sap_flag);
extern uchar *novdbg_event2string (int event);
extern void novell_set_max_hops(ulong value);
extern void ipx_insert_backup_info(backuptype **, backuptype *,
				   novpdbtype *, boolean);
extern void ipx_ndb_self_backup(novndbtype *ndb);
extern void ipx_sap_self_backup(novell_sapentrytype *sap, boolean debug);
extern void ipx_delete_backup_list(backuptype *backup_list);
extern void novell_sap_check_backup_queue (novell_sapentrytype *entry);
extern void novell_return_static_counts(int *, int *);
extern int nv_write_patricia(rntype *, va_list);
extern int display_patricia(rntype *, va_list);
extern int reset_patricia(rntype *, va_list);
extern boolean ipx_validate_contiguous_mask(ulong);
extern int patricia_static_routes(rntype *, va_list);
extern int patricia_paths_command(rntype *, va_list);
extern int patricia_start_aging (rntype *, va_list);
extern int patricia_novrip_ager(rntype *, va_list);
extern int patricia_shutdown(rntype *, va_list);
extern int patricia_delete_associated_statics(rntype *, va_list);
extern boolean ipx_distribute_sap_list_check(novell_sapentrytype *, 
					     novpdbtype *, nidbtype *);
extern void ipx_configure_for_lane(idbtype *);
extern nidbtype *novell_nidb_new(idbtype *idb, ulong net, int enctype,
				 boolean primary, boolean old_style,
				 boolean internal_net, boolean finish,
				 boolean);
extern void nov_network_redist(novpdbtype *, novndbtype *, int);
extern void nov_router_erase(novpdbtype *);
extern void nov_service_redist(novpdbtype *, novell_sapentrytype *, int);
extern void nov_set_ndb_flags(novndbtype *, novpdbtype *, boolean);
extern forktype nov_router_init(void);
extern boolean novigrp_net_in_pdb (novpdbtype *, ulong);
extern void ipx_set_encaps(idbtype *, int);
/*
 * nov_ipxwan_on_nidb
 *
 * Return TRUE if IPXWAN is enabled for the given NIDB.
 */
static inline boolean nov_ipxwan_on_nidb (nidbtype *nidb)
{
    if (nidb == NULL)
        return(FALSE);
    return(nidb->nov_ipxwan_enabled);
}

/*
 * nov_ipxwan_up_nidb
 *
 * Return TRUE if IPXWAN is fully RUNNING on the given NIDB.
 */
static inline boolean nov_ipxwan_up_nidb (nidbtype *nidb)
{
    if (nidb == NULL)
        return(FALSE);
    return(NOV_IPXWAN_IS_CONNECT_STATE(nidb->nov_ipxwan_state));
}

extern int novell_get_default_encapsulation(idbtype *idb);


/*
 * IPX Access list support.
 */
/*
 * The structure definitions and typedefs should eventually migrate here.
 * When they do, the following definitions can use the typedef instead of
 * the explicit structure name.
 */

static inline struct xnsnacctype_ *ipx_get_std_acl (int number)
{
    return((struct xnsnacctype_ *)accessarray[number]);
}

static inline void ipx_set_std_acl (int number, struct xnsnacctype_ *acl)
{
    accessarray[number] = (accessptr *)acl;
}

static inline struct xnseacctype_ *ipx_get_ext_acl (int number)
{
    return((struct xnseacctype_ *)accessarray[number]);
}

static inline void ipx_set_ext_acl (int number, struct xnseacctype_ *acl)
{
    accessarray[number] = (accessptr *)acl;
}

static inline struct nov_sumlisttype_ *ipx_get_sum_acl (int number)
{
    return((struct nov_sumlisttype_ *)accessarray[number]);
}

static inline void ipx_set_sum_acl (int number, struct nov_sumlisttype_ *acl)
{
    accessarray[number] = (accessptr *)acl;
}
extern ulong ipx_nexthop_network (paktype *);

#endif __NOVELL_H__
