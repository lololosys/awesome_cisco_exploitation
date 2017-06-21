/* $Id: cfg_router_neighbor_ospf.h,v 3.3.26.1 1996/05/21 09:53:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_neighbor_ospf.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ N E I G H B O R _ O S P F . H
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_neighbor_ospf.h,v $
 * Revision 3.3.26.1  1996/05/21  09:53:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/01/18  02:13:24  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  17:32:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * [no] neighbor <address> [{ priority <num> | poll-interval <seconds> }]
 *
 * interface parameter parsed and ignored for 9.1 compatibility.
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(int,1) = TRUE if priority <num>
 * OBJ(int,2) = <num>
 * OBJ(int,3) = TRUE if poll-interval <seconds>
 * OBJ(int,4) = <seconds>
 */
pdecl(rtr_neigh_ospf);

EOLS (rtr_neigh_ospf_eol, neighbor_command, NEIGHBOR_STD);
ASSERT (rtr_neigh_ospf_eol_check, rtr_neigh_ospf_eol, NONE,
	(GETOBJ(int,1) | GETOBJ(int,3)));
/* the interface specification is obsolete */
INTERFACE (rtr_neigh_if_name, rtr_neigh_ospf, no_alt, 
	   OBJ(idb,1), IFTYPE_HWIDB     );
KEYWORD (rtr_neigh_interface, rtr_neigh_if_name, rtr_neigh_ospf_eol_check,
	 common_str_interface, "Interface to OSPF non-broadcast neighbor", 
	 PRIV_HIDDEN | PRIV_CONF);

/* { priority <num> | poll-interval <seconds> } */
NUMBER	(rtr_neigh_if_poll_num, rtr_neigh_ospf, no_alt,
	 OBJ(int,4), 0, -1, "Seconds");
KEYWORD_ID (rtr_neigh_if_poll, rtr_neigh_if_poll_num, rtr_neigh_interface,
	    OBJ(int,3), TRUE,
	    "poll-interval", "OSPF dead-router polling interval", PRIV_CONF);

NUMBER	(rtr_neigh_if_pri_num, rtr_neigh_ospf, no_alt,
	 OBJ(int,2), 0, 255, "Priority");
KEYWORD_ID (rtr_neigh_ospfpri, rtr_neigh_if_pri_num, rtr_neigh_if_poll,
	    OBJ(int,1), TRUE,
	    "priority", "OSPF priority of non-broadcast neighbor", PRIV_CONF);

ASSERT	(rtr_neigh_ospf, rtr_neigh_ospfpri, ALTERNATE,
	 ((pdbtype *) csb->protocol)->proctype & PROC_OSPF);

#undef	ALTERNATE
#define ALTERNATE	rtr_neigh_ospf
