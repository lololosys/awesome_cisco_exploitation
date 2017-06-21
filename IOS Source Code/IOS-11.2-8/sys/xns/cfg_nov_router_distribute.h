/* $Id: cfg_nov_router_distribute.h,v 3.3.62.2 1996/08/12 16:09:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_nov_router_distribute.h,v $
 *------------------------------------------------------------------
 * cfg_nov_router_distribute.h
 * 
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_nov_router_distribute.h,v $
 * Revision 3.3.62.2  1996/08/12  16:09:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.62.1  1996/05/30  23:47:52  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/20  17:54:45  sluong
 * CSCdi43294:  filtering saps between protocols are not feasible
 *
 * Revision 3.2  1995/11/17  19:21:07  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:38:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/21  18:18:30  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  23:27:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] distribute-list <list> in [<interface>]
 * [no] distribute-list <list> out [{ <routing-process> | <interface> }]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = IN or OUT
 * OBJ(idb,1) = <interface>
 * OBJ(string,2) = routing process
 * OBJ(int,3) = routing process AS number (if needed)
 *
 */

EOLNS	(nov_rtr_distl_eol, novell_distributelist_command);

/* <interface-name><unit> */
INTERFACE (nov_rtr_distl_if, nov_rtr_distl_eol, nov_rtr_distl_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);

/* <routing-process> */
GENERAL_ROUTER(nov_rtr_distl_rtrproc, nov_rtr_distl_eol, nov_rtr_distl_if, 
	       router_name, (PROC_IGRP2 | PROC_RIP | RMATCH_NOVELL | 
			     RMATCH_ARGS | PROC_NLSP));

/* in */
KEYWORD_ID (nov_rtr_distl_in, nov_rtr_distl_if, no_alt,
	    OBJ(int,2), IN,
	    "in", "Filter incoming routing updates", PRIV_CONF);

/* out */
KEYWORD_ID (nov_rtr_distl_out, nov_rtr_distl_rtrproc, nov_rtr_distl_in,
	    OBJ(int,2), OUT,
	    "out", "Filter outgoing routing updates", PRIV_CONF);

/* <list>
 * Allow the full range of 800 lists and 900 lists to allow the use of network
 * masks in filters (900 lists). upper range MAXENOVACC.
 */
ACCESS_LIST(nov_rtr_distl_list, nov_rtr_distl_out, no_alt,
	 OBJ(int,1), MINNOVACC, MAXENOVACC,
	 IPX_MIN_STD_ACL, IPX_MAX_EXT_ACL, "An IPX access list number");
NVGENNS	(nov_rtr_distl_nvgen, nov_rtr_distl_list, 
	 novell_distributelist_command);

KEYWORD	(nov_rtr_distribute, nov_rtr_distl_nvgen, NONE,
	 "distribute-list", "Filter networks in routing updates", PRIV_CONF);

ASSERT	(nov_rtr_distribute_test, nov_rtr_distribute, ALTERNATE,
	 (((novpdbtype *)csb->protocol)->proctype != NET_NLSP));

/************************************************************************
 * [no] distribute-sap-list <list> in [<interface>]
 * [no] distribute-sap-list <list> out [{ <routing-process> | <interface> }]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = IN or OUT
 * OBJ(idb,1) = <interface>
 * OBJ(string,2) = routing process
 * OBJ(int,3) = routing process AS number (if needed)
 *
 */

EOLNS	(nov_rtr_distl_sap_eol, novell_distributesaplist_command);

/* <interface-name><unit> */
INTERFACE (nov_rtr_distl_sap_if, nov_rtr_distl_sap_eol, nov_rtr_distl_sap_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);

/* <routing-process> */
GENERAL_ROUTER(nov_rtr_distl_sap_rtrproc, nov_rtr_distl_sap_eol, 
	       nov_rtr_distl_sap_if, 
	       router_name, (PROC_IGRP2 | PROC_RIP | RMATCH_NOVELL | 
			     RMATCH_ARGS | PROC_NLSP));

/* in */
KEYWORD_ID (nov_rtr_distl_sap_in, nov_rtr_distl_sap_if, no_alt,
	    OBJ(int,2), IN,
	    "in", "Filter incoming service updates", PRIV_CONF);

/* out */
KEYWORD_ID (nov_rtr_distl_sap_out, nov_rtr_distl_sap_rtrproc, 
	    nov_rtr_distl_sap_in, OBJ(int,2), OUT,
	    "out", "Filter outgoing service updates", PRIV_CONF);

/* <list>
 * Allow the range of 1000 lists.
 */
ACCESS_LIST(nov_rtr_distl_sap_list, nov_rtr_distl_sap_out, no_alt,
	 OBJ(int,1), MINNOVSAPACC, MAXNOVSAPACC,
	 IPX_MIN_SAP_ACL, IPX_MAX_SAP_ACL, "An IPX access list number");
NVGENNS	(nov_rtr_distl_sap_nvgen, nov_rtr_distl_sap_list, 
	 novell_distributesaplist_command);

KEYWORD	(nov_rtr_distribute_sap, nov_rtr_distl_sap_nvgen, 
	 nov_rtr_distribute_test,
	 "distribute-sap-list", "Filter services in SAP updates", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	nov_rtr_distribute_sap
