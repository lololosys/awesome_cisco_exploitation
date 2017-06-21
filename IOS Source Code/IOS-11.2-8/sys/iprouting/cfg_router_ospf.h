/* $Id: cfg_router_ospf.h,v 3.2.60.1 1996/04/16 19:01:25 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_ospf.h,v $
 *------------------------------------------------------------------
 * cfg_router_ospf.h - OSPF router specific command
 *
 * Feburary 1995, Derek M. Yeung
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_ospf.h,v $
 * Revision 3.2.60.1  1996/04/16  19:01:25  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:32:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * ospf auto-cost reference-bandwidth <reference-bandwidth value>
 * no ospf auto-cost
 *
 * OBJ(int,1) = reference-bandwidth value in Mbit/s
 *
 * auto-cost-determinaton is accepted for backward compatibility
 */
EOLS (router_ospf_auto_cost_eol, ospf_command, OSPF_AUTO_COST);
NUMBER (router_ospf_auto_cost_ref_value, router_ospf_auto_cost_eol,
	no_alt, OBJ(int,1), 1, OSPF_MAX_REFERENCE_BW, 
	"The reference bandwidth in terms of Mbits per second");
KEYWORD (router_ospf_auto_cost_ref, router_ospf_auto_cost_ref_value,
	 router_ospf_auto_cost_eol, "reference-bandwidth",
	 "Use reference bandwidth method to assign OSPF cost", PRIV_CONF);
NVGENS (router_ospf_auto_cost_nv, router_ospf_auto_cost_ref,
	ospf_command, OSPF_AUTO_COST);
KEYWORD_MM (router_ospf_auto_cost_det, router_ospf_auto_cost_nv,
	    no_alt, "auto-cost-determination",
	    "Calculate OSPF interface cost according to bandwidth",
	    PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN, 10);
KEYWORD (router_ospf_auto_cost, router_ospf_auto_cost_nv,
	 router_ospf_auto_cost_det, "auto-cost",
	 "Calculate OSPF interface cost according to bandwidth", PRIV_CONF);


/**********************************************************************
 * [no] ospf log-adjacency-changes
 */
EOLS (router_ospf_log_adj_chg_eol, ospf_command, OSPF_LOG_ADJ_CHG);
KEYWORD (router_ospf_log_adj_chg, router_ospf_log_adj_chg_eol,
         router_ospf_auto_cost,
         "log-adjacency-changes", "Log changes in adjacency state", 
         PRIV_CONF);

/**********************************************************************
 * ospf
 */

KEYWORD (router_ospf_kw, router_ospf_log_adj_chg, NONE,
	 "ospf", "OSPF specific commands", PRIV_CONF);

ASSERT	(router_ospf, router_ospf_kw, ALTERNATE,
	 (((pdbtype *) csb->protocol)->family == PDB_IP &&
	  ((pdbtype *) csb->protocol)->proctype == PROC_OSPF));

#undef	ALTERNATE
#define	ALTERNATE	router_ospf

