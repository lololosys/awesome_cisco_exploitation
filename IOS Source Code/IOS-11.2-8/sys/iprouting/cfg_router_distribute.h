/* $Id: cfg_router_distribute.h,v 3.3.50.2 1996/08/12 16:03:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_distribute.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ D I S T R I B U T E . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_distribute.h,v $
 * Revision 3.3.50.2  1996/08/12  16:03:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.50.1  1996/05/04  01:21:18  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.3  1995/12/04  06:47:13  tli
 * CSCdi43559:  IP: distribute-list 101 out should work
 *
 * Revision 3.2  1995/11/17  17:32:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] distribute-list [<list #> | <list name>] in [<interface>]
 * [no] distribute-list [<list #> | <list name>] out
 *			[{ <routing-process> | <interface> }]
 *
 * OBJ(int,1) = <list #>
 * OBJ(string,1) = <list name>
 * OBJ(int,2) = IN or OUT
 * OBJ(idb,1) = <interface>
 * OBJ(int,3) = routing process AS number (if needed)
 *
 */

EOLNS	(rtr_distl_eol, distributelist_command);

/* <interface-name><unit> */
INTERFACE (rtr_distl_if, rtr_distl_eol, rtr_distl_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);

/* <routing-process> */
GENERAL_ROUTER(rtr_distl_rtrproc, rtr_distl_eol, rtr_distl_if, router_name,
	       (PROC_BGP | PROC_EGP | PROC_IGRP | PROC_IGRP2 |
		PROC_RIP | PROC_ISIS | PROC_ISOIGRP |
		PROC_OSPF | PROC_STATIC | PROC_CONNECTED |
		RMATCH_IP | RMATCH_OSI | RMATCH_ARGS));

/* in */
KEYWORD_ID (rtr_distl_in, rtr_distl_if, no_alt,
	    OBJ(int,2), IN,
	    "in", "Filter incoming routing updates", PRIV_CONF);

/* out */
KEYWORD_ID (rtr_distl_out, rtr_distl_rtrproc, rtr_distl_in,
	    OBJ(int,2), OUT,
	    "out", "Filter outgoing routing updates", PRIV_CONF);

/* <list>
 * Number or name
 */
GENERAL_STRING (rtr_distl_accname, rtr_distl_out, no_alt,
		OBJ(string,1), "Access-list name", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(rtr_distl_stest, rtr_distl_accname, NONE,
		    no_alt);
NUMBER	(rtr_distl_list, rtr_distl_out, rtr_distl_stest, OBJ(int,1),
	 MINFASTACC, MAXSLOWACC, "IP access list number");
NVGENNS	(rtr_distl_nvgen, rtr_distl_list, distributelist_command);

KEYWORD	(rtr_distribute, rtr_distl_nvgen, NONE,
	 "distribute-list", "Filter networks in routing updates", PRIV_CONF);

ASSERT	(router_distribute, rtr_distribute, ALTERNATE,
	 ((pdbtype *) csb->protocol)->family == PDB_IP ||
	 ((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK);

#undef	ALTERNATE
#define	ALTERNATE	router_distribute
