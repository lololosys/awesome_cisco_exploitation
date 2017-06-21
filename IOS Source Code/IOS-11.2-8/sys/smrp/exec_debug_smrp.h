/* $Id: exec_debug_smrp.h,v 3.3.28.1 1996/05/21 10:01:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/exec_debug_smrp.h,v $
 *------------------------------------------------------------------
 * Debug commands for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_smrp.h,v $
 * Revision 3.3.28.1  1996/05/21  10:01:56  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/01/18  02:14:54  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:52:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:24:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/01  00:22:35  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.2  1995/06/19  14:49:44  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:44:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
LINK_TRANS(debug_smrp_extend_here, no_alt);

/*****************************************************************
 * debug smrp transaction
 */
EOLS	(debug_smrp_trans_eol, smrp_debug_commands, DEBUG_SMRP_TRANSACTION);
INTERFACE (debug_smrp_trans_int_name, debug_smrp_trans_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_smrp_trans_int, debug_smrp_trans_int_name,
	    debug_smrp_trans_eol, OBJ(int, 1), SMRP_DEBUG_INTERFACE,
	    common_str_interface, "SMRP interface-specific debugging", PRIV_OPR);
KEYWORD (debug_smrp_transaction, debug_smrp_trans_int,
	 debug_smrp_extend_here, "transaction", "SMRP transaction", PRIV_OPR);

/*****************************************************************
 * debug smrp route
 */
EOLS	(debug_smrp_rte_eol, smrp_debug_commands, DEBUG_SMRP_ROUTE);
INTERFACE (debug_smrp_rte_int_name, debug_smrp_rte_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_smrp_rte_int, debug_smrp_rte_int_name,
	    debug_smrp_rte_eol, OBJ(int, 1), SMRP_DEBUG_INTERFACE,
	    common_str_interface, "SMRP interface-specific debugging", PRIV_OPR);
KEYWORD (debug_smrp_route, debug_smrp_rte_int,
	 debug_smrp_transaction, "route", "SMRP routing", PRIV_OPR);

/*****************************************************************
 * debug smrp port
 */
EOLS	(debug_smrp_port_eol, smrp_debug_commands, DEBUG_SMRP_PORT);
INTERFACE (debug_smrp_port_int_name, debug_smrp_port_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_smrp_port_int, debug_smrp_port_int_name,
	    debug_smrp_port_eol, OBJ(int, 1), SMRP_DEBUG_INTERFACE,
	    common_str_interface, "SMRP interface-specific debugging", PRIV_OPR);
KEYWORD (debug_smrp_port, debug_smrp_port_int,
	 debug_smrp_route, "port", "SMRP port", PRIV_OPR);

/*****************************************************************
 * debug smrp neighbor
 */
EOLS	(debug_smrp_nbr_eol, smrp_debug_commands, DEBUG_SMRP_NEIGHBOR);
INTERFACE (debug_smrp_nbr_int_name, debug_smrp_nbr_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_smrp_nbr_int, debug_smrp_nbr_int_name,
	    debug_smrp_nbr_eol, OBJ(int, 1), SMRP_DEBUG_INTERFACE,
	    common_str_interface, "SMRP interface-specific debugging", PRIV_OPR);
KEYWORD (debug_smrp_neighbor, debug_smrp_nbr_int,
	 debug_smrp_port, "neighbor", "SMRP neighbor", PRIV_OPR);

/*****************************************************************
 * debug smrp mcache
 */
EOLS	(debug_smrp_mcache_eol, smrp_debug_commands, DEBUG_SMRP_MCACHE);
INTERFACE (debug_smrp_mcache_int_name, debug_smrp_mcache_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_smrp_mcache_int, debug_smrp_mcache_int_name,
	    debug_smrp_mcache_eol, OBJ(int, 1), SMRP_DEBUG_INTERFACE,
	    common_str_interface, "SMRP interface-specific debugging", PRIV_OPR);
KEYWORD (debug_smrp_mcache, debug_smrp_mcache_int,
	 debug_smrp_neighbor, "mcache", "SMRP fast switching cache", PRIV_OPR);

/*****************************************************************
 * debug smrp group
 */
EOLS	(debug_smrp_grp_eol, smrp_debug_commands, DEBUG_SMRP_GROUP);
INTERFACE (debug_smrp_grp_int_name, debug_smrp_grp_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_smrp_grp_int, debug_smrp_grp_int_name,
	    debug_smrp_grp_eol, OBJ(int, 1), SMRP_DEBUG_INTERFACE,
	    common_str_interface, "SMRP interface-specific debugging", PRIV_OPR);
KEYWORD (debug_smrp_group, debug_smrp_grp_int,
	 debug_smrp_mcache, "group", "SMRP group", PRIV_OPR);

/*****************************************************************
 * debug smrp all 
 */
EOLS	(debug_smrp_all_eol, smrp_debug_commands, DEBUG_SMRP_ALL);
INTERFACE (debug_smrp_all_int_name, debug_smrp_all_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_smrp_all_int, debug_smrp_all_int_name,
	    debug_smrp_all_eol, OBJ(int, 1), SMRP_DEBUG_INTERFACE,
	    common_str_interface, "SMRP interface-specific debugging", PRIV_OPR);
KEYWORD (debug_smrp_all, debug_smrp_all_int,
	 debug_smrp_group, "all", "SMRP all events", PRIV_OPR);

/******************************************************************
 * debug smrp
 *
 */
KEYWORD(debug_smrp, debug_smrp_all, ALTERNATE,
	"smrp", "SMRP information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_smrp
