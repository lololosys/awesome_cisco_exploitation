/* $Id: cfg_router_neighbor.h,v 3.3.20.1 1996/08/12 16:03:45 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_neighbor.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ N E I G H B O R . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_neighbor.h,v $
 * Revision 3.3.20.1  1996/08/12  16:03:45  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:57  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:32:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	[no] neighbor <address>
 *
 * BGP is the only routing protocol that shouldn't accept this syntax.
 */

EOLS	(rtr_neigh_eol, neighbor_command, NEIGHBOR_STD);
ASSERT	(rtr_neigh_standard, rtr_neigh_eol, no_alt,
	 !(((pdbtype *) csb->protocol)->proctype & PROC_BGP));

/*
 * Protocol specific extensions go here.
 */
LINK_TRANS(rtr_neigh_extend_here, rtr_neigh_standard);

/************************************************************************
 *	[no] neighbor <address> third-party <3rd-party-address>
 *					    [internal | external]
 *
 * EGP ONLY
 *
 * OBJ(int,1)  = TRUE for external; FALSE (default) for internal
 * (*OBJ(paddr,1)) = <address>
 * (*OBJ(paddr,2)) = <3rd-party-address>
 *
 * If the (*OBJ(paddr,1)) is 0, then this is really
 *   neighbor any third-party <address> [internal | external]
 */

EOLS	(rtr_neigh_3rdeol, neighbor_command, NEIGHBOR_EGP_3PARTY);

KEYWORD_ID(rtr_neigh_3rdext, rtr_neigh_3rdeol, rtr_neigh_3rdeol,
	   OBJ(int,1), TRUE,
	   "external", "Advertise in external section of EGP update", PRIV_CONF);
KEYWORD	(rtr_neigh_3rdint, rtr_neigh_3rdeol, rtr_neigh_3rdext,
	 "internal", "Advertise in internal section of EGP update (default)", 
	 PRIV_CONF);
IPADDR	(rtr_neigh_3rdaddr, rtr_neigh_3rdint, no_alt,
	 OBJ(paddr,2), "Third party router IP address");
KEYWORD	(rtr_neigh_3rd, rtr_neigh_3rdaddr, NONE,
	 "third-party", "Specify a third party router", PRIV_CONF);

ASSERT	(rtr_neigh_egp, rtr_neigh_3rd, rtr_neigh_extend_here,
	 ((pdbtype *) csb->protocol)->proctype & PROC_EGP);

/*************************************************************************/

STRING	(rtr_neigh_tag_s, rtr_neigh_extend_here, no_alt,
	 OBJ(string,1), "Neighbor tag");
/*
 * Iff BGP, and if we haven't matched on IP address or "any", check
 * for a symbolic name instead of an address (this is the neighbor tag).
 */
ASSERT	(rtr_neigh_tag, rtr_neigh_tag_s, NONE,
	 GETOBJ(paddr,1)->type == 0);
ASSERT	(rtr_neigh_tag_assert, rtr_neigh_tag, no_alt,
	 ((pdbtype *) csb->protocol)->proctype & PROC_BGP);

IPADDR	(rtr_neigh_net, rtr_neigh_egp, rtr_neigh_tag_assert,
	 OBJ(paddr,1), "Neighbor address");

/*************************************************************************
 * neighbor any [<list>]
 * no neighbor any 
 *
 * EGP ONLY
 *
 * OBJ(int,2) = <list>
 *
 * The command:
 *   neighbor any third-party <address> [internal | external]
 * is detected here and transitions to the third-party command above.
 * (*OBJ(paddr,1)) will be 0 in this case (like the original code).
 *
 */
EOLS	(rtr_neigh_any_eol, neighbor_command, NEIGHBOR_ANY);
NUMBER (rtr_neigh_any_list, rtr_neigh_any_eol, rtr_neigh_eol,
	 OBJ(int,2), MINFASTACC, MAXFASTACC, "IP access list of neighbors");

/* Note: This transition goes to the third-party command above.  We can't
 * transition there for the keyword too, since there is no way to get
 * back here to pick up the <list> and EOL.
 */
KEYWORD (rtr_neigh_any_3rd_kw, rtr_neigh_3rdaddr, NONE,
	 "third-party", "Specify a third party router", PRIV_CONF);
ASSERT	(rtr_neigh_any_3rd, rtr_neigh_any_3rd_kw, rtr_neigh_any_list,
	 ((pdbtype *) csb->protocol)->proctype & PROC_EGP);

KEYWORD (rtr_neigh_any_kw, rtr_neigh_any_3rd, NONE,
	 "any", "EGP 0 neighbor selection", PRIV_CONF);
ASSERT	(rtr_neigh_any, rtr_neigh_any_kw, rtr_neigh_net,
	 (((pdbtype *) csb->protocol)->proctype & PROC_EGP) &&
	 (((pdbtype *) csb->protocol)->asystem == 0));


/*************************************************************************/
/* Beware: because the NVGEN callout happens here, none of the neighbor
 * subcommands can be selectively omitted with the PRIV_NONVGEN flag.
 * Instead, each protocol's neighbor command contains the NVGEN code.
 */
NVGENNS	(rtr_neigh_nvgen, rtr_neigh_any, neighbor_command);

KEYWORD	(rtr_neigh_kw, rtr_neigh_nvgen, NONE,
	 "neighbor", "Specify a neighbor router", PRIV_CONF);
ASSERT	(rtr_neigh, rtr_neigh_kw, ALTERNATE,
	 ((clns_pdbtype *) csb->protocol)->family == PDB_IP);

#undef	ALTERNATE
#define ALTERNATE	rtr_neigh
