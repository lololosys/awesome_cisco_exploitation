/* $Id: bgp.h,v 3.3.60.2 1996/04/16 19:01:15 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/bgp.h,v $
 *------------------------------------------------------------------
 * BGP public information
 *
 * 22-January-1989, Kirk Lougheed
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: bgp.h,v $
 * Revision 3.3.60.2  1996/04/16  19:01:15  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/04/05  03:46:27  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.3  1995/11/18  00:58:59  thille
 * CSCdi38736:  BGP snmp trap responses getting lost
 * Fix and modularize the bgp trap support.
 *
 * Revision 3.2  1995/11/17  17:31:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/25  08:30:48  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.3  1995/07/12  17:52:17  rchandra
 * CSCdi36969:  BGP : Add knob to disable client to client reflection
 * - 'bgp client-to-client reflection' command added
 * - when access-list change callback is made, instead of cleaning the
 *   cache immediately, flag the cache for purging by bgp_router.
 *
 * Revision 2.2  1995/06/30  20:25:08  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.1  1995/06/07  21:05:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BGP_H__
#define __BGP_H__

/*
 * Version of BGP protocol supported by this implementation
 * (required by parser only)
 */

#define BGP_VERSION	4
#define BGP_MAX_VERSION 4
#define BGP_MIN_VERSION 2

/*
 * Range checks for BGP weights
 * (required by parser only)
 */
#define	BGP_MIN_WEIGHT	   0
#define	BGP_MAX_WEIGHT	   MAXUSHORT
#define	BGP_SOURCED_WEIGHT 32768
#define	BGP_LEARNED_WEIGHT 0
#define BGP_MAX_COMMONADMIN 256

/*
 * BGP origin codes (required by parser and route maps)
 */

#define ORIGIN_IGP         0
#define ORIGIN_EGP         1
#define ORIGIN_INCOMPLETE  2

/*
 * enum for "show bgp" (required by parser)
 */
typedef enum bgp_show_parser_cons_ {
    BGP_DEFAULT,
    BGP_ADDRESS,
    BGP_LIST,
    BGP_REGEXP,
    BGP_CIDR,
    BGP_ADDRESS_SUBNETS,
    BGP_BOGUS,
    BGP_COMMUNITY,
    BGP_COMMUNITY_LIST,
    BGP_DAMPENED_ROUTES,
    BGP_FSTAT
} bgp_show_parser_cons;

typedef enum bgp_clear_parser_cons_ {
    BGP_CLEAR_NEIGH,
    BGP_CLEAR_NEIGH_AS,
    BGP_CLEAR_FSTAT,
    BGP_CLEAR_FSTAT_ADDR,
    BGP_CLEAR_FSTAT_REGEXP,
    BGP_CLEAR_FSTAT_FLIST,
    BGP_CLEAR_FSTAT_NEIGH,
    BGP_CLEAR_SOFT_ALL,
    BGP_CLEAR_SOFT_PGRP,
    BGP_CLEAR_SOFT_NEIGH,
    BGP_CLEAR_SOFT_AS
} bgp_clear_parser_cons;

#define BGP_SHOW_COMCOUNT 	7

/*
 * defines for "(router) bgp ..." (required by parser)
 */
#define	BGP_DEF_LOCALPREF	0
#define	BGP_FAST_RESET		1
#define	BGP_REDIST_IBGP		2
#define BGP_ROUTER_ID		3
#define BGP_ALWAYS_COMPARE_MED	4
#define	BGP_CONFED_ID		5
#define	BGP_CLUSTER_ID	        6
#define BGP_RMAP_DAMPENING	7
#define BGP_CLIENT_REFLECTION   8
#define BGP_DPA_ENABLED		9
#define BGP_DETERM_MED		10
/*
 * defines for 'show bgp neighbor' commands
 */
typedef enum bgp_show_neighbor_cons_ {
    BGP_NEIGH_ROUTES,
    BGP_NEIGH_DAMPROUTES,
    BGP_NEIGH_FSTAT,
    BGP_NEIGH_RCVDROUTES,
    BGP_NEIGH_ADVROUTES
} bgp_show_neighbor_cons;

/*
 * defines for "bgp route-damp ..." (required by parser)
 */
#define BGP_MAX_HALFLIFE	45
#define BGP_MAX_PENALTY		20000

/*
 * defines for bgp state change traps (used by sr_bgpmib.c)
 */
#define BGP_TRAP_ESTAB		1
#define BGP_TRAP_BACKTRANS	2

/*
 * Prototypes
 */

/* bgp_chain.c */
extern void bgp_parser_init(void);

/* for snmp access */
extern ipaddrtype bgp_rtr_id(void);

/*
Local Variables:
comment-column: 33
End:
*/
#endif __BGP_H__
