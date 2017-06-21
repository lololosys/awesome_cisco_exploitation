/* $Id: cfg_router_distance.h,v 3.2.60.1 1996/05/04 01:21:16 wilber Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_distance.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ D I S T A N C E . H
 *
 * The bgp version must come before the weight version so that both NVGEN
 * and normal processing happen correctly.
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_distance.h,v $
 * Revision 3.2.60.1  1996/05/04  01:21:16  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.2  1995/11/17  17:32:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(router_distance_extend_here, no_alt);

/************************************************************************
 * distance <weight> [ clns | ip | ( <ipsrc-addr> <ip-mask> [<list>] ) ]
 * no distance <weight> [ clns | ip | ( <ipsrc-addr> <ip-mask>
 *			[<list> | <list name>] ) ]
 *
 * OBJ(int,1) = weight
 * OBJ(int,2) = list
 * OBJ(string,1) = list name
 * OBJ(int,3) = "clns" = 0, or "ip" = 1
 * (*OBJ(paddr,1)) = ipsrc-addr
 * (*OBJ(paddr,2)) = ip-mask
 *
 */

EOLS	(rtr_dist_weight_eol, distance_command, DISTANCE_WEIGHT);

/* For BGP and IGRP2 donot accept 'distance <weight>' command */
IFELSE (rtr_dist_eigrp_bgp, no_alt, rtr_dist_weight_eol,
	((pdbtype *) csb->protocol)->proctype & (PROC_IGRP2|PROC_BGP));

/* protocol name (accept only if the protocol family is OSI) */
KEYWORD_ID (rtr_dist_weight_prot_ip, rtr_dist_weight_eol, rtr_dist_weight_eol,
	    OBJ(int,3), 1, "ip","Distance applied for IP derived routes", 
	    PRIV_CONF);
KEYWORD_ID (rtr_dist_weight_prot_clns, rtr_dist_weight_eol,
	    rtr_dist_weight_prot_ip, OBJ(int,3), 0, "clns",
	    "Distance applied for CLNS derived routes", PRIV_CONF);
IFELSE (rtr_dist_check_family, rtr_dist_weight_prot_clns, rtr_dist_eigrp_bgp,
	((pdbtype *) csb->protocol)->family == PDB_OSI);

/* <list> */
GENERAL_STRING (rtr_dist_weight_accname, rtr_dist_weight_eol,
		rtr_dist_weight_eol, 
		OBJ(string,1), "Standard access-list name", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (rtr_dist_weight_stest,
		    rtr_dist_weight_accname, NONE, no_alt);
NUMBER	(rtr_dist_weight_list, rtr_dist_weight_eol, rtr_dist_weight_stest,
	 OBJ(int,2), MINFASTACC, MAXFASTACC, "IP Standard access list number");

/* <ipsrc-addr> <ip-mask> */
IPADDR	(rtr_dist_weight_mask, rtr_dist_weight_list, no_alt,
	 OBJ(paddr,2), "IP address mask");
IPADDR	(rtr_dist_weight_src, rtr_dist_weight_mask, rtr_dist_check_family,
	 OBJ(paddr,1), "IP Source address");

/* <weight> */
NUMBER	(rtr_dist_weight_val, rtr_dist_weight_src, no_alt,
	 OBJ(int,1), MINDISTANCE, MAXDISTANCE, "Administrative distance");

NVGENS	(rtr_dist_weight_nv, rtr_dist_weight_val,
	 distance_command, DISTANCE_WEIGHT);

ASSERT	(rtr_dist_weight, router_distance_extend_here, rtr_dist_weight_nv,
	 ((pdbtype *) csb->protocol)->family == PDB_IP &&
	 (((pdbtype *) csb->protocol)->proctype & 
	  (PROC_IGRP2|PROC_BGP|PROC_OSPF)));
	 

/****************************************************************/
KEYWORD	(router_distance, rtr_dist_weight, ALTERNATE,
	"distance", "Define an administrative distance", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	router_distance
