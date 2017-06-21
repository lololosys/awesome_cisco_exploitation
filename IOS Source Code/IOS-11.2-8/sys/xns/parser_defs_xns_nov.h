/* $Id: parser_defs_xns_nov.h,v 3.9.4.7 1996/08/14 18:09:42 john Exp $
 * $Source: /release/112/cvs/Xsys/xns/parser_defs_xns_nov.h,v $
 *------------------------------------------------------------------
 *	P A R S E R _ D E F S _ X N S _ N O V . H
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_xns_nov.h,v $
 * Revision 3.9.4.7  1996/08/14  18:09:42  john
 * CSCdi64010:  IPX SPX spoofing stops when spoof table gets large -
 *              need to age out
 * Branch: California_branch
 *
 * Revision 3.9.4.6  1996/06/27  01:10:31  mschaef
 * CSCdi61334:  Alignment error, building IPX route cache entry
 * Branch: California_branch
 *
 * Revision 3.9.4.5  1996/06/21  16:50:10  sluong
 * CSCdi57042:  ipx floating static route kicks in after 4 min with EIGRP
 * Branch: California_branch
 *
 * Revision 3.9.4.4  1996/06/12  17:38:03  akr
 * CSCdi60228:  ipx route-cache can grow without limit
 * Branch: California_branch
 *
 * Revision 3.9.4.3  1996/05/30  23:49:42  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.9.4.2  1996/05/03  02:20:42  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.9.4.1  1996/04/02  05:38:37  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.9  1996/03/09  18:13:49  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.8  1996/03/01  07:34:57  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.7  1996/02/14  23:01:50  john
 * CSCdi47314:  IPX network FFFFFFFE ignored when default handling is
 *              disabled. Only treat -2 special if default route enabled.
 *
 * Revision 3.6  1996/01/29  07:01:13  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.5  1995/12/11  04:55:31  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.4  1995/12/07  17:16:21  mschaef
 * CSCdi45172:  Change default interpacket gap for RIP and SAP to 55 mSec
 *
 * Revision 3.3  1995/12/01  06:26:29  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.2  1995/11/17  19:24:41  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:39:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/25  15:00:11  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.5  1995/08/25  05:40:28  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.4  1995/08/25  02:58:45  john
 * CSCdi39233:  Show IPX Server unsorted is sorted
 *
 * Revision 2.3  1995/07/28  23:56:49  john
 * CSCdi37833:  IPX triggered rip/sap delays need a global default
 *
 * Revision 2.2  1995/07/18  22:33:50  john
 * CSCdi34411:  Large ipx output-rip/sap-delays may delay routing from
 *              running
 *
 * Revision 2.1  1995/06/07  23:29:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __PARSER_DEFS_XNS_NOV_H__
#define __PARSER_DEFS_XNS_NOV_H__

/* XNS and Novell defines */

enum {XNS_ADDRESS, XNS_ROUTE, XNS_ROUTING, XNS_FORWARD, 
	  XNS_UBROUTE, XNS_UBEMULATION, XNS_PATHS, XNS_RIP_DELAY, XNS_NTYPES};

enum {XNSIF_ACCESS, XNSIF_ENCTYPE, XNSIF_HELPER, 
	  XNSIF_UNNUMBERED, XNSIF_NETWORK, XNSIF_ROUTEC, 
	  XNSIF_IN_FILTER, XNSIF_OUT_FILTER, 
	  XNSIF_ROUTER_FILTER, XNSIF_UPDATETIME, 
	  XNSIF_OLD3COM, XNSIF_HEARRIP, XNSIF_FLOOD, XNSIF_NTYPES};

enum {XNSIF_NETZERO, XNSIF_ALLNETS, XNSIF_BROADCAST, XNSIF_SPECIFIC};

enum {CLEAR_XNS, CLEAR_XNS_CACHE, CLEAR_XNS_ROUTE};

enum {SHOW_XNS_ACCESS, SHOW_XNS_CACHE, SHOW_XNS_INTERFACE, SHOW_XNS_ROUTE,
	  SHOW_XNS_TRAFFIC};

/* Global Novell commands */
enum {NOVELL_ADDRESS, NOVELL_ROUTER_RIP, NOVELL_ROUTER_IGRP, 
	  NOVELL_ROUTE, NOVELL_PATHS, NOVELL_SAP, NOVELL_NSQRR, 
	  NOVELL_ALGORITHMS, NOVELL_INTERNAL_NET, 
	  NOVELL_SAPQUEUEMAX, NOVELL_SAP_USES_ROUTING, 
	  NOVELL_GNS_RESPONSE_DELAY, NOVELL_BACKUP_SERVER_INTERVAL, 
	  NOVELL_ACCTTRANSITS, 
	  NOVELL_ACCTLIST, NOVELL_ACCTTHRESHOLD, NOVELL_TYPE20_IN, 
	  NOVELL_TYPE20_OUT, NOVELL_NETBIOS_IN, NOVELL_MAXIMUM_HOPS,
	  NOVELL_RIP_DEFAULT_TYPE, NOVELL_RIP_DEFAULT_ROUTE, NOVELL_KLUDGE,
	  NOVELL_SERVER_SPLIT, NOVELL_TYPE20_HELPERED, NOVELL_PING_DEFAULT,
	  NOVELL_POTENTIAL_PSEUDONODE, NOVELL_EIGRP_SAP_SPLIT_HORIZON,
	  NOVELL_DEFAULT_SAP_DELAY, NOVELL_DEFAULT_RIP_DELAY,
	  NOVELL_DEFAULT_TRIG_SAP_DELAY, NOVELL_DEFAULT_TRIG_RIP_DELAY,
	  NOVELL_THROTTLE_UPDATE_PROC, NOVELL_THROTTLE_UPDATE_SCHED,
 	  NOVELL_FASTSWITCH_BCAST,
	  NOVELL_PER_HOST,
 	  NOVELL_RCACHE_MAX_INACTIVE_AGE,
 	  NOVELL_RCACHE_MAX_UPDATE_AGE,
	  NOVELL_RCACHE_MAX_SIZE,
  	  NOVELL_SPX_SESSION_CLEAR, NOVELL_SPX_TABLE_CLEAR, NOVELL_NTYPES
	  };

/* Interface specific Novell commands */
enum {NOVELL_ACCESS, NOVELL_UNNUMBERED, NOVELL_NETWORK, 
	  NOVELL_ENCTYPE, NOVELL_SAPINT, NOVELL_HLPADDR, NOVELL_HLPLIST,
	  NOVELL_INACCESS, NOVELL_ROUTEC, NOVELL_RIP_MULTIPLIER, 
	  NOVELL_SAP_MULTIPLIER, NOVELL_IN_FILTER, NOVELL_OUT_FILTER, 
	  NOVELL_ROUTER_FILTER, NOVELL_SAP_INFILTER, 
	  NOVELL_SAP_OUTFILTER, NOVELL_SAP_ROUTERFILTER, 
	  NOVELL_UPDATETIME, NOVELL_SAP_DELAY, 
	  NOVELL_RIP_DELAY, NOVELL_NSQ_OUTFILTER, NOVELL_DELAY, 
	  NOVELL_IPXWAN_ENABLE, NOVELL_RIP_MAX_PACKETSIZE, 
	  NOVELL_SAP_MAX_PACKETSIZE, NOVELL_DOWN, NOVELL_TYPE20, 
	  NOVELL_EVENIZE, NOVELL_WATCHDOGSPOOF, 
	  NOVELL_GNS_REPLY_DISABLE, NOVELL_INT_GNS_RESPONSE_DELAY, 
	  NOVELL_LINKDELAY, NOVELL_THROUGHPUT,
	  NOVELL_ADVERTISE_DEFAULT_ROUTE_ONLY, NOVELL_SPXSPOOF,
	  NOVELL_SPXIDLETIME, NOVELL_PPPCLIENT,
	  NOVELL_TRIG_RIP_DELAY, NOVELL_TRIG_SAP_DELAY,
	  NOVELL_SAP_AFTER_RIP,
	  NOVELLIF_NTYPES
	  };

/* IPX IGRP2 specific commands */

enum {IPXIGRP_HELLO, IPXIGRP_HOLDTIME, IPXIGRP_SAPUPDATE, IPXIGRP_SPLIT,
	  IPXIGRP_SUMMARY, IPXIGRP_BW_PCT, IPXIGRP_HOLDDOWN};
enum {IPXIGRP_DEB_FSM, IPXIGRP_DEB_NEI};

/* IPX ISIS specific commands */

enum {IPX_ISIS_RIP_MODE, IPX_ISIS_SAP_MODE};

/* IPX NLSP specific interface commands */

enum {IPX_NLSP_RIP_MODE, IPX_NLSP_SAP_MODE, IPX_NLSP_ENABLE, IPX_NLSP_METRIC,
	  IPX_NLSP_PRIO, IPX_NLSP_HELLO_IVL, IPX_NLSP_CSNP_IVL, 
	  IPX_NLSP_RETRANS_IVL, IPX_NLSP_HELLO_MULT, IPX_NLSP_LSPIVL_CMD};

/* IPX NLSP specific global commands */
enum {IPX_NLSP_EIGRP_PREFERRED};

/* Defines for interface commands */

#define IPX_NLSP_PRIO_DEFAULT			44

#define IPX_NLSP_HELLO_IVL_DEFAULT 		20
#define IPX_NLSP_HELLO_IVL_MAX			100

#define IPX_NLSP_LSPIVL_MIN		NLSP_MINIMUM_PACING_INTERVAL
#define IPX_NLSP_LSPIVL_MAX		5000

#define IPX_NLSP_CSNP_IVL_DEFAULT		30
#define IPX_NLSP_CSNP_IVL_MAX			600

#define IPX_NLSP_RETRANS_IVL_MAX		60

/* IPX IPXWAN command Modes */

enum {IPX_IPXWAN_DEFAULT, IPX_IPXWAN_CLIENT, IPX_IPXWAN_STATIC, 
	IPX_IPXWAN_ERROR};

/* IPX NLSP/ISIS RIP/SAP modes */

enum {IPX_RIPSAP_ON, IPX_RIPSAP_OFF, IPX_RIPSAP_AUTO};

/* show novell server sort indicators */

enum {IPXSAP_SORT_BY_NONE, IPXSAP_SORT_BY_NAME, IPXSAP_SORT_BY_NET,
	IPXSAP_SORT_BY_TYPE};

/* NLSP global commands */

enum {NOV_NLSP_AREA_ADDR, NOV_NLSP_LSP_GEN, NOV_NLSP_LSP_MTU, 
	  NOV_NLSP_MAX_LSP_GEN, NOV_NLSP_MAXAGE, NOV_NLSP_SPF_IVL,
	  NOV_NLSP_PRC_IVL, NOV_NLSP_SYSID_DISPLAY, NOV_NLSP_LOG_ADJ_CHG, 
	  NOV_NLSP_ENABLE_SUMMARY, NOV_NLSP_ENABLE_NP_LSP_SUMMARY,
	  NOV_NLSP_LSP_HASH_ENABLE};

/* Ping default command modes */

enum {IPX_PING_DEFLT_CISCO, IPX_PING_DEFLT_NOVELL};

/* Defines for global commands */

#define NOV_NLSP_LSP_MTU_MIN			512
#define NOV_NLSP_LSP_MTU_DEFAULT		512
#define NOV_NLSP_LSP_MTU_MAX			4096

#define NOV_NLSP_MAX_LSP_GEN_DEFAULT		7200
#define NOV_NLSP_MAX_LSP_GEN_MAX		50000

#define NOV_NLSP_MAXAGE_DEFAULT			7500
#define NOV_NLSP_MAXAGE_MAX			50000

#define NOV_OUTPUT_DELAY_DEFAULT		55      /* Novell conformant */

/* Parser definitions for IPX network numbers: */

#define NOV_RIP_DEFAULT_ROUTE 0xfffffffeL
#define NOV_MIN_NET 1
/* Changed becaused novell default route handling might be disabled
 * Need to check at run time if default route is enabled then max net
 * drops to 0xFFFFFFFD.
 */
#define NOV_MAX_NET (ulong) (NOV_RIP_DEFAULT_ROUTE)
#define NOV_MIN_TICKS 1
#define NOV_MAX_TICKS 65535
#define NOV_MIN_AREA_COUNT 1
#define NOV_MAX_AREA_COUNT 255

/* Novell clear commands */
enum {CLEAR_NOVELL, CLEAR_NOVELL_ACCOUNTING, CLEAR_NOVELL_CACHE,
      CLEAR_NOVELL_ROUTE, CLEAR_NOVELL_SPX_SPOOF};

/* Novell show commands */
enum {SHOW_NOVELL_ACCESS,
      SHOW_NOVELL_ACCOUNTING, SHOW_NOVELL_BACKUP, SHOW_NOVELL_CACHE,
      SHOW_NOVELL_INTERFACE, SHOW_NOVELL_NEARTEST, SHOW_NOVELL_QUERYTEST,
      SHOW_NOVELL_RIPTEST, SHOW_NOVELL_ROUTE, SHOW_NOVELL_SAPTEST,
      SHOW_NOVELL_SERVERS, SHOW_NOVELL_TRAFFIC, SHOW_NOVELL_SPX_SPOOF,
      SHOW_NOVELL_URD};

/* Novell Test command defines */
enum {
 TEST_NOVELL,
 TEST_NOVELL_GNS,
 TEST_NOVELL_RIP,
 TEST_NOVELL_NETBIOS_FINDNAME,
 TEST_NOVELL_ECHO,
 TEST_NOVELL_DEBUG,
 TEST_NOVELL_WATCHDOG,
 TEST_NOVELL_CAPACITY,
 TEST_NOVELL_RIPREQ,
 TEST_NOVELL_NAME
};

#endif __PARSER_DEFS_XNS_NOV_H__
