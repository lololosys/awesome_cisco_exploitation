/* $Id: exec_show_smrp.h,v 3.3 1996/02/01 23:34:29 widmer Exp $
 * $Source: /release/111/cvs/Xsys/smrp/exec_show_smrp.h,v $
 *------------------------------------------------------------------
 * Show commands for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_smrp.h,v $
 * Revision 3.3  1996/02/01  23:34:29  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:52:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:45:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(show_smrp_extend_here, no_alt);

/*****************************************************************
 * show smrp traffic [all | group | neighbor | port | route | transaction]
 *
 * OBJ(int,1) = SHOW_SMRP_TRAFFIC (none)
 *              SHOW_SMRP_TRAFFIC_ALL (all)
 *              SHOW_SMRP_TRAFFIC_GROUP (group)
 *              SHOW_SMRP_TRAFFIC_NBR (neighbor)
 *              SHOW_SMRP_TRAFFIC_PORT (port)
 *              SHOW_SMRP_TRAFFIC_ROUTE (route)
 *              SHOW_SMRP_TRAFFIC_TRANS (transaction)
 */
EOLS    (show_smrp_traffic_eol,smrp_show_commands, SHOW_SMRP_TRAFFIC);

KEYWORD_ID (show_smrp_traffic_trans, show_smrp_traffic_eol,
         show_smrp_traffic_eol, OBJ(int,1), SMRP_SHOW_TRAFFIC_TRANS,
	 "transaction", "SMRP transaction table", PRIV_USER);

KEYWORD_ID (show_smrp_traffic_route, show_smrp_traffic_eol,
         show_smrp_traffic_trans, OBJ(int,1), SMRP_SHOW_TRAFFIC_ROUTE,
	 "route", "SMRP route table", PRIV_USER);

KEYWORD_ID (show_smrp_traffic_port, show_smrp_traffic_eol,
         show_smrp_traffic_route, OBJ(int,1), SMRP_SHOW_TRAFFIC_PORT,
	 "port", "SMRP port table", PRIV_USER);

KEYWORD_ID (show_smrp_traffic_neighbor, show_smrp_traffic_eol,
         show_smrp_traffic_port, OBJ(int,1), SMRP_SHOW_TRAFFIC_NBR,
	 "neighbor", "SMRP neighbor table", PRIV_USER);

KEYWORD_ID (show_smrp_traffic_group, show_smrp_traffic_eol,
         show_smrp_traffic_neighbor, OBJ(int,1), SMRP_SHOW_TRAFFIC_GROUP,
	 "group", "SMRP group table", PRIV_USER);

KEYWORD_ID (show_smrp_traffic_all, show_smrp_traffic_eol,
         show_smrp_traffic_group, OBJ(int,1), SMRP_SHOW_TRAFFIC_ALL,
	 "all", "All SMRP tables", PRIV_USER);

KEYWORD (show_smrp_traffic, show_smrp_traffic_all, show_smrp_extend_here,
	 "traffic", "SMRP traffic",PRIV_USER);

/*****************************************************************
 * show smrp route [appletalk [<network>] | <interface>]
 *
 * OBJ(int,1) = network type
 * OBJ(int,2) = network
 * OBJ(int,3) = 0 = neither
 *              1 = network
 *              2 = interface
 * OBJ(idb,1) = <interface>
 *
 */
EOLS    (show_smrp_route_eol,smrp_show_commands, SHOW_SMRP_ROUTE);

SET     (show_smrp_route_int_set, show_smrp_route_eol, OBJ(int,3), 2);
INTERFACE(show_smrp_route_int, show_smrp_route_int_set, show_smrp_route_eol,
	  OBJ(idb,1), IFTYPE_ANYSUB);

SET     (show_smrp_route_network_set, show_smrp_route_eol, OBJ(int,3), 1);
NUMBER  (show_smrp_route_atalk_net, show_smrp_route_network_set,
        show_smrp_route_eol,
        OBJ(int,2), 1, 65279, "Network number");

KEYWORD_ID(show_smrp_route_atalk, show_smrp_route_atalk_net,
	   show_smrp_route_int,
	   OBJ(int,1), LINK_APPLETALK,
	   "appletalk", "AppleTalk route", PRIV_USER);

KEYWORD	(show_smrp_route, show_smrp_route_atalk, show_smrp_traffic,
	 "route", "SMRP routing table",PRIV_USER);

/*****************************************************************
 * show smrp private [all | request | response]
 */
EOLS    (show_smrp_private_eol,smrp_show_commands, SHOW_SMRP_PRIVATE);

KEYWORD_ID(show_smrp_private_resp, show_smrp_private_eol,
	   show_smrp_private_eol,
	   OBJ(int,1), SMRP_SHOW_PVT_RESP,
	   "response", "Pending responses", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID(show_smrp_private_req, show_smrp_private_eol,
	   show_smrp_private_resp,
	   OBJ(int,1), SMRP_SHOW_PVT_REQ,
	   "request", "Pending requests", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID(show_smrp_private_all, show_smrp_private_eol,
	   show_smrp_private_req,
	   OBJ(int,1), SMRP_SHOW_PVT_ALL,
	   "all", "All private SMRP info", PRIV_USER|PRIV_HIDDEN);

KEYWORD (show_smrp_private, show_smrp_private_all, show_smrp_route,
	 "private", "SMRP private info",PRIV_USER|PRIV_HIDDEN);

/*****************************************************************
 * show smrp port [appletalk [<interface>]]
 *
 * OBJ(int,1) = network type
 * OBJ(int,2) = FALSE (no interface)
 *              TRUE (interface set)
 * OBJ(idb,1) = interface
 */
EOLS    (show_smrp_port_eol,smrp_show_commands, SHOW_SMRP_PORT);

SET (show_smrp_port_set1, show_smrp_port_eol, OBJ(int,2), TRUE);
INTERFACE(show_smrp_port_interface, show_smrp_port_set1,
	  show_smrp_port_eol,
	  OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD_ID(show_smrp_port_atalk, show_smrp_port_interface,
	   show_smrp_port_eol,
	   OBJ(int,1), LINK_APPLETALK,
	   "appletalk", "AppleTalk port", PRIV_USER);

KEYWORD	(show_smrp_port, show_smrp_port_atalk, show_smrp_private,
	 "port", "SMRP port table",PRIV_USER);

/*****************************************************************
 * show smrp neighbor [appletalk [<network_addr>]]
 *
 * OBJ(int,1) = network type
 * OBJ(int,2) = FALSE (no addr)
 *              TRUE (addr set)
 * OBJ(paddr,1) = address
 */
EOLS    (show_smrp_neighbor_eol,smrp_show_commands, SHOW_SMRP_NBR);

SET	(show_smrp_nbr_set1, show_smrp_neighbor_eol, OBJ(int,2), TRUE);
ATADDR	(show_smrp_nbr_addr, show_smrp_nbr_set1, show_smrp_neighbor_eol,
	 OBJ(paddr,1), "AppleTalk address of neighbor");

KEYWORD_ID(show_smrp_neighbor_atalk, show_smrp_nbr_addr,
	   show_smrp_neighbor_eol,
	   OBJ(int,1), LINK_APPLETALK,
	   "appletalk", "AppleTalk neighbors", PRIV_USER);

KEYWORD (show_smrp_neighbor, show_smrp_neighbor_atalk, show_smrp_port,
	 "neighbor", "SMRP neighbor table",PRIV_USER);

/*****************************************************************
 * show smrp mcache [appletalk [<group_addr>]]
 *
 * OBJ(int,1) = network type
 * OBJ(int,2) = start range
 * OBJ(int,3) = end range
 * OBJ(paddr,1) = address
 */
EOLS    (show_smrp_mcache_eol,smrp_show_commands, SHOW_SMRP_MCACHE);

SET (show_smrp_mcache_addr_set1, show_smrp_mcache_eol, OBJ(int,4), TRUE);
SMRPGROUPADDR(show_smrp_mcache_addr, show_smrp_mcache_addr_set1,
	      show_smrp_mcache_eol,
	      OBJ(int,2), OBJ(int,3));

KEYWORD_ID(show_smrp_mcache_atalk, show_smrp_mcache_addr,
	   show_smrp_mcache_eol,
	   OBJ(int,1), LINK_APPLETALK,
	   "appletalk", "AppleTalk group", PRIV_USER);

KEYWORD (show_smrp_mcache, show_smrp_mcache_atalk, show_smrp_neighbor,
	 "mcache", "SMRP multicast fast switching cache",PRIV_USER);

/*****************************************************************
 * show smrp group [appletalk [<group_addr>]]
 *
 * OBJ(int,1) = network type
 * OBJ(int,2) = start range
 * OBJ(int,3) = end range
 * OBJ(int,4) = FALSE (no addr)
 *              TRUE (addr set)
 * OBJ(paddr,1) = address
 */
EOLS    (show_smrp_group_eol,smrp_show_commands, SHOW_SMRP_GROUP);

SET (show_smrp_group_addr_set1, show_smrp_group_eol, OBJ(int,4), TRUE);
SMRPGROUPADDR(show_smrp_group_addr, show_smrp_group_addr_set1,
	      show_smrp_group_eol,
	      OBJ(int,2), OBJ(int,3));

KEYWORD_ID(show_smrp_group_atalk, show_smrp_group_addr,
	   show_smrp_group_eol,
	   OBJ(int,1), LINK_APPLETALK,
	   "appletalk", "AppleTalk group", PRIV_USER);

KEYWORD (show_smrp_group, show_smrp_group_atalk, show_smrp_mcache,
	 "group", "SMRP group table",PRIV_USER);

/*****************************************************************
 * show smrp globals
 */
EOLS    (show_smrp_globals_eol,smrp_show_commands, SHOW_SMRP_GLOBALS);

KEYWORD (show_smrp_globals, show_smrp_globals_eol, show_smrp_group,
	 "globals", "SMRP global parameters",PRIV_USER);

/*****************************************************************
 * show smrp forward [appletalk [<group_addr>]]
 *
 * OBJ(int,1) = network type
 * OBJ(int,2) = start range
 * OBJ(int,3) = end range
 * OBJ(paddr,1) = address
 */
EOLS	(show_smrp_forward_eol, smrp_show_commands, SHOW_SMRP_FWD);

SET (show_smrp_fwd_addr_set1, show_smrp_forward_eol, OBJ(int,4), TRUE);
SMRPGROUPADDR(show_smrp_forward_addr, show_smrp_fwd_addr_set1,
	      show_smrp_forward_eol,
	      OBJ(int,2), OBJ(int,3));

KEYWORD_ID(show_smrp_forward_atalk, show_smrp_forward_addr,
	   show_smrp_forward_eol,
	   OBJ(int,1), LINK_APPLETALK,
	   "appletalk", "AppleTalk group", PRIV_USER);

KEYWORD (show_smrp_forward, show_smrp_forward_atalk, show_smrp_globals,
	 "forward", "SMRP forwarding table",PRIV_USER);

/******************************************************************
 * The show smrp command keyword.
 */
KEYWORD (show_smrp, show_smrp_forward, ALTERNATE,
	 "smrp", "Simple Multicast Routing Protocol (SMRP) information",
         PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_smrp
