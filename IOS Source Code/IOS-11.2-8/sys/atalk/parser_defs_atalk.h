/* $Id: parser_defs_atalk.h,v 3.1.40.3 1996/05/02 08:41:30 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/parser_defs_atalk.h,v $
 *------------------------------------------------------------------
 *	P A R S E R _ D E F S _ A T A L K . H
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_atalk.h,v $
 * Revision 3.1.40.3  1996/05/02  08:41:30  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1.40.2  1996/04/02  05:35:47  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.1.40.1  1996/03/23  01:28:42  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.1  1996/02/01  04:57:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.4  1996/01/29  07:00:44  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/24  04:54:58  dwong
 * CSCdi44162:  Appletalk doesnt work between ISDN callers
 *
 * Revision 3.2  1995/11/17  18:49:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:20:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/19  23:55:24  kleung
 * CSCdi38726:  AppleTalk RTMP Stub rtmp_stub feature - only RTMP stubs
 * sent.
 *
 * Revision 2.2  1995/09/12  19:35:43  slin
 * CSCdi39096:  Inability to NBP-register AUFS Server (Unix Host).
 * - Add support for routing between local devices
 *
 * Revision 2.1  1995/06/07  22:28:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__parser_defs_atalk_h__
#define __parser_defs_atalk_h__

enum {
    ATALK_UNDEF, 
    ATALK_ACCESS,
    ATALK_ADDRESS,
    ATALK_CLIENT_MODE,
    ATALK_ALTADDRS,
    ATALK_ARP,
    ATALK_CABLE_RANGE,
    ATALK_CHECKSUM,
    ATALK_DISCOVERY,
    ATALK_ROUTEDIST,
    ATALK_DISTANCE,
    ATALK_ROUTEFILTER,
    ATALK_EIGRP_COMMAND,
    ATALK_EIGRP_LOG,
    ATALK_EIGRP_LOG_SIZE,
    ATALK_EIGRP_LOG_TYPE,
    ATALK_EIGRP_LOG_NBR,
    ATALK_EIGRP_TIMERS,
    ATALK_EVENTS,
    ATALK_FTZ_SECURITY,
    ATALK_ZIPFILTER,
    ATALK_ZIPREPLYFILTER,
    ATALK_GLEAN,
    ATALK_HALFROUTER,
    ATALK_MACIP,
    ATALK_IGN_VERIFY,
    ATALK_IGRP_BW_PCT,
    ATALK_IGRP_TIMERS,
    ATALK_IGRP_SPLIT,
    ATALK_IPTALK,
    ATALK_IPTALK_BASEPORT,
    ATALK_LOOKUPTYPE,
    ATALK_NAMELOOKUP,
    ATALK_PARTIALZONES,
    ATALK_PPPCLIENT,
    ATALK_PREFDDITALK,
    ATALK_PROXYNBP,
    ATALK_REDIST_METRIC,
    ATALK_ROUTEZONES,
    ATALK_ROUTEC,
    ATALK_ROUTE_PROTOCOL,
    ATALK_ROUTING,
    ATALK_RTMP_COMMAND,
    ATALK_RTMP_BW,
    ATALK_RTMP_GC_INTERVAL,
    ATALK_RTMP_JITTER,
    ATALK_RTMP_SPLIT,
    ATALK_RTMP_STANDBY,
    ATALK_RTMP_STUB,
    ATALK_RTMP_TIMERS,
    ATALK_SENDROUTES,
    ATALK_SERVICE,
    ATALK_STRICT,
    ATALK_LOCALROUTE,
    ATALK_TESTTOOL,
    ATALK_TIMERS,
    ATALK_VIRTUAL_NET,
    ATALK_ZONEQUERY,
    ATALK_ZONE,
    ATALK_ARP_PARAM,
    ATALK_ARP_RETRANS,
    ATALK_ARP_INTERVAL,
    ATALK_ARP_TIMEOUT,
    ATALK_ARPTYPE_PROBE,
    ATALK_ARPTYPE_REQUEST,
    ATALK_PORTDISABLE,
    ATALK_STATIC,
    AURP_TICKLE,
    AURP_RP,
    AURP_UPDATE,
    ATALK_DOMAIN,
    ATALK_DOMAIN_REMAP,
    ATALK_DOMAIN_GROUP,
    ATALK_DOMAIN_NAME,
    ATALK_DOMAIN_HOP_REDUCTION,
    ATALK_DOMAIN_HOP_PRESERVE,
    ATALK_MAXPATHS,
};

/*
 * AppleTalk "show" exec commands
 */
enum {
    SHOW_APPLE_AURP_TOPOLOGY,
    SHOW_APPLE_AURP_EVENTS,
    SHOW_APPLE_ACCESS,
    SHOW_APPLE_ADJACENT,
    SHOW_APPLE_ARP,
    SHOW_APPLE_CACHE,
    SHOW_APPLE_GLOBALS,
    SHOW_APPLE_INTERFACE,
    SHOW_APPLE_EIGRP_EVENT,
    SHOW_APPLE_EIGRP_SIAEVENT,
    SHOW_APPLE_EIGRP_NBR,
    SHOW_APPLE_EIGRP_TOP,
    SHOW_APPLE_EIGRP_TRAFFIC,
    SHOW_APPLE_MACIP_CLIENTS,
    SHOW_APPLE_MACIP_SERVERS,
    SHOW_APPLE_MACIP_TRAFFIC,
    SHOW_APPLE_MAP,
    SHOW_APPLE_NAMES,
    SHOW_APPLE_NBP,
    SHOW_APPLE_NEIGHBORS,
    SHOW_APPLE_PRIVATE,
    SHOW_APPLE_REMAP,
    SHOW_APPLE_DOMAIN,
    SHOW_APPLE_ROUTE,
    SHOW_APPLE_SOCKETS,
    SHOW_APPLE_STATIC,
    SHOW_APPLE_TRAFFIC,
    SHOW_APPLE_ZONES,
};

/*
 * AppleTalk "show" exec command sub-parses.
 */
#define	ATALK_SHOW_PVT_NULL		0
#define ATALK_SHOW_PVT_DEFAULT		(ATALK_SHOW_PVT_NULL+1)
#define ATALK_SHOW_PVT_GLOBALS		(ATALK_SHOW_PVT_DEFAULT+1)
#define ATALK_SHOW_PVT_INTERFACES	(ATALK_SHOW_PVT_GLOBALS+1)
#define ATALK_SHOW_PVT_ROUTES		(ATALK_SHOW_PVT_INTERFACES+1)
#define ATALK_SHOW_PVT_AARP		(ATALK_SHOW_PVT_ROUTES+1)
#define ATALK_SHOW_PVT_IGRP		(ATALK_SHOW_PVT_AARP+1)
#define ATALK_SHOW_PVT_CACHE		(ATALK_SHOW_PVT_IGRP+1)
#define ATALK_SHOW_PVT_REMAP		(ATALK_SHOW_PVT_CACHE+1)
#define ATALK_SHOW_PVT_RTMP		(ATALK_SHOW_PVT_REMAP+1)
#define ATALK_SHOW_PVT_ALL		(ATALK_SHOW_PVT_RTMP+1)

#define MACIP_DISABLE            0
#define MACIP_STATIC             (MACIP_DISABLE+1)
#define MACIP_DYNAMIC            (MACIP_STATIC+1)
#define MACIP_SERVER             (MACIP_DYNAMIC+1)

enum {
    ATALK_IGRP_ROUTING
};

/*
 * Routing protocol types.
 */
#define	ATALK_RP_UNDEF		0
#define	ATALK_RP_RTMP		(ATALK_RP_UNDEF+1)
#define ATALK_RP_IGRP		(ATALK_RP_RTMP+1)
#define ATALK_RP_ISIS		(ATALK_RP_IGRP+1)
#define ATALK_RP_AURP		(ATALK_RP_ISIS+1)

/*
 * AppleTalk "map" command
 */
#define AT_MAP_INVALID			0
#define AT_MAP_SMDS			(AT_MAP_INVALID+1)
#define AT_MAP_FR			(AT_MAP_SMDS+1)
#define AT_MAP_X25			(AT_MAP_FR+1)
#define AT_MAP_LAPB			(AT_MAP_X25+1)

/*
 * AppleTalk Access Parse keywords
 *
 * The keywords are used in at_access.c and in the parse chain for the
 * access-list command.  They are created as defines so that
 * only one instantiation of them will exist, hopefully preventing
 * typographical errors between multiple uses.
 */
#define ATACC_ANYZONES 1
#define ATACCKWD_ADDITIONAL_ZONES   "additional-zones"

#define ATACC_CABLE 2
#define ATACCKWD_CABLE_RANGE        "cable-range"

#define ATACC_INCLUDES 3
#define ATACCKWD_INCLUDES           "includes"

#define ATACC_NETWORK 4
#define ATACCKWD_NETWORK            "network"

#define ATACC_OTHERS 5
#define ATACCKWD_OTHER_ACCESS       "other-access"

#define ATACC_WITHIN 6
#define ATACCKWD_WITHIN             "within"

#define ATACC_ZONE 7
#define ATACCKWD_ZONE               "zone"

#define ATACC_DELETE 8
#define ATACCKWD_NO_APPLETALK       ""

/*
 * NBP Filter related constants
 *
 */

#define ATACCKWD_NBP                "nbp"

#define ATACC_NBP_OBJ 9
#define ATACCKWD_NBP_OBJ            "object"

#define ATACC_NBP_TYPE 10
#define ATACCKWD_NBP_TYPE           "type"

#define ATACC_NBP_ZONE 11
#define ATACCKWD_NBP_ZONE           "zone"

#define ATACC_NBP_OTHERS 12
#define ATACCKWD_NBP_OTHERS         "other-nbps"

#endif


/*
 * AppleTalk "clear" exec command sub-parses.
 */
enum {
    CLEAR_APPLE_NEIGHBOR,
    CLEAR_APPLE_TRAFFIC,
    CLEAR_APPLE_ROUTE,
    CLEAR_APPLE_ROUTE_CACHE,
    CLEAR_APPLE_INTERFACE,
    CLEAR_APPLE_ARP,
    CLEAR_APPLE_NBP,
};
