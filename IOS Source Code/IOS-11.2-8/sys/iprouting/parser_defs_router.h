/*
 *	P A R S E R _ D E F S _ R O U T E R . H
 *
 * $Id: parser_defs_router.h,v 3.1.40.2 1996/07/30 06:56:06 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/parser_defs_router.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_router.h,v $
 * Revision 3.1.40.2  1996/07/30  06:56:06  rchandra
 * CSCdi64489:  BGP: private-AS number cannot be removed from AS-path
 * Branch: California_branch
 * - add 'neighbor x.x.x.x remove-private-AS' command.
 * - add 'inbound' keyword to 'neighbor x.x.x.x soft-reconfiguration'
 *   command (CSCdi64487)
 *
 * Revision 3.1.40.1  1996/04/16  19:02:10  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:58:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/28  21:14:47  rchandra
 * CSCdi39436:  BGP : Allow conditional unsuppression of suppressed routes
 * - add 'neighbor x.x.x.x unsuppress-map <map>' command
 *
 * Revision 2.3  1995/07/06  23:11:21  rchandra
 * CSCdi36759:  BGP : Add conditional per neighbor default generation
 * - add 'neighbor <x.x.x.x> default-originate route-map <tag>' command.
 *
 * Revision 2.2  1995/06/30  20:24:40  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.1  1995/06/07  22:31:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Router subcommand types */

typedef enum parser_distance_ {
	DISTANCE_BGP,
	DISTANCE_WEIGHT,
	DISTANCE_IGRP2,
} parser_distance;

typedef enum parser_metric_ {
	P_METRIC_WEIGHTS,
	P_METRIC_HOLDDOWN,
	P_METRIC_HOPCOUNT
} parser_metric;

typedef enum parser_timers_ {
	P_TIMERS_BASIC
} parser_timers;

typedef enum parser_neighbor_ {
	NEIGHBOR_STD,
	NEIGHBOR_ANY,
	NEIGHBOR_WEIGHT,
	NEIGHBOR_EGP_3PARTY,
	NEIGHBOR_PERM_CFG,
	NEIGHBOR_BGP_FILAS,
	NEIGHBOR_BGP_FILLIST,
	NEIGHBOR_BGP_DIST,
	NEIGHBOR_BGP_WEIGHT,
	NEIGHBOR_BGP_VERSION,
	NEIGHBOR_BGP_REMAS,
	NEIGHBOR_BGP_UPDSRC,
	NEIGHBOR_BGP_COMMOK,
	NEIGHBOR_BGP_EMHOP,
	NEIGHBOR_BGP_NEIGHLIST,
	NEIGHBOR_BGP_NHSELF,
	NEIGHBOR_BGP_ROUTEMAP,
	NEIGHBOR_BGP_MINADVERT,
	NEIGHBOR_BGP_BACKCOMPAT,
	NEIGHBOR_BGP_PEERGROUP,
	NEIGHBOR_BGP_PEERGP_MEM,
	NEIGHBOR_BGP_PASSWORD,
	NEIGHBOR_BGP_DEFAULT,
	NEIGHBOR_BGP_RRCLIENT,
	NEIGHBOR_BGP_UNSUPPRESS,
	NEIGHBOR_BGP_RECONFIG_INOK,
	NEIGHBOR_BGP_REMOVE_PRIVATE_AS,
	NEIGHBOR_BGP_COND_ADV
} parser_neighbor;

/* Redistribution defines */
#define REDIST_METRIC       0x01
#define REDIST_METRICTYPE   0x02
#define REDIST_TAG          0x04
#define REDIST_SUBNET       0x08
#define REDIST_MATCH	    0x10
#define	REDIST_WEIGHT	    0x40
#define	REDIST_LEVEL	    0x80
#define REDIST_ROUTEMAP     0x100
#define REDIST_DEFAULT      0x00

/*
 * Traffic share algorithm types
 */

#define TRAFFIC_BALANCED	0x0
#define TRAFFIC_MIN		0x1

/*
 * Router command defines
 */
typedef enum parser_default_info_ {
	DEFAULTINF_IN,
	DEFAULTINF_OUT,
	DEFAULTINF_ORIGINATE,
	DEFAULTINF_METRIC,
	DEFAULTINF_METRICTYPE
} parser_default_info;

#define DEFAULT_MAP_METRIC              20
#define DEFAULT_MAP_METRIC_CONNECTED    1
#define DEFAULT_METRIC_TYPE             2

/*
 * defines for "show ip route"
 */
typedef enum parser_show_ipr_ {
	SHOW_IPR_DEFAULT,
	SHOW_IPR_CIDR,
	SHOW_IPR_PREFIX
} parser_show_ipr;
