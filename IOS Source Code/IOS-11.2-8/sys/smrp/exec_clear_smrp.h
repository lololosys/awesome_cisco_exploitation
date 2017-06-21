/* $Id: exec_clear_smrp.h,v 3.3 1996/02/01 23:34:28 widmer Exp $
 * $Source: /release/111/cvs/Xsys/smrp/exec_clear_smrp.h,v $
 *------------------------------------------------------------------
 * Clear commands for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_smrp.h,v $
 * Revision 3.3  1996/02/01  23:34:28  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:52:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:24:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/01  00:22:33  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.1  1995/06/07  22:44:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(clear_smrp_extend_here, no_alt);

/*****************************************************************
 * clear smrp traffic
 */
EOLS    (clear_smrp_traffic_eol, smrp_clear_commands, CLEAR_SMRP_TRAFFIC);

KEYWORD (clear_smrp_traffic, clear_smrp_traffic_eol, clear_smrp_extend_here,
	 "traffic", "SMRP traffic",PRIV_USER);

/*****************************************************************
 * clear smrp route [appletalk <network>]
 */
EOLS    (clear_smrp_route_eol,smrp_clear_commands, CLEAR_SMRP_ROUTE);

SET	(clear_smrp_route_network_set, clear_smrp_route_eol, OBJ(int,3), 1);
NUMBER	(clear_smrp_route_atalk_net, clear_smrp_route_network_set, no_alt,
	 OBJ(int,2), 1, 65279, "Network number");

KEYWORD_ID(clear_smrp_route_atalk, clear_smrp_route_atalk_net,
	   clear_smrp_route_eol,
	   OBJ(int,1), LINK_APPLETALK,
	   "appletalk", "AppleTalk route", PRIV_USER);

KEYWORD (clear_smrp_route, clear_smrp_route_atalk, clear_smrp_traffic,
	 "route", "SMRP routing table",PRIV_USER);

/*****************************************************************
 * clear smrp neighbor [appletalk] [<network_addr>]
 */
EOLS    (clear_smrp_neighbor_eol, smrp_clear_commands, CLEAR_SMRP_NEIGHBOR);

SET	(clear_smrp_nbr_set1, clear_smrp_neighbor_eol, OBJ(int,2), TRUE);
ATADDR	(clear_smrp_nbr_addr, clear_smrp_nbr_set1, clear_smrp_neighbor_eol,
	 OBJ(paddr,1), "AppleTalk address of neighbor");

KEYWORD_ID(clear_smrp_neighbor_atalk, clear_smrp_nbr_addr,
	   clear_smrp_neighbor_eol,
	   OBJ(int,1), LINK_APPLETALK,
	   "appletalk", "AppleTalk neighbors", PRIV_USER);

KEYWORD (clear_smrp_neighbor, clear_smrp_neighbor_atalk, clear_smrp_route,
	 "neighbor", "SMRP neighbor table",PRIV_USER);

/*****************************************************************
 * clear smrp mcache
 */
EOLS    (clear_smrp_mcache_eol,smrp_clear_commands, CLEAR_SMRP_MCACHE);

KEYWORD (clear_smrp_mcache, clear_smrp_mcache_eol, clear_smrp_neighbor,
	 "mcache", "SMRP multicast fast switching cache",PRIV_USER);

/******************************************************************
 * The clear smrp command keyword.
 */
KEYWORD (clear_smrp, clear_smrp_mcache, ALTERNATE,
	 "smrp", "Simple Multicast Routing Protocol (SMRP) statistics",
         PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	clear_smrp
