/* $Id: exec_clear_vines.h,v 3.2.62.2 1996/08/12 16:09:08 widmer Exp $
 * $Source: /release/112/cvs/Xsys/vines/exec_clear_vines.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ V I N E S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_vines.h,v $
 * Revision 3.2.62.2  1996/08/12  16:09:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/05/21  10:08:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  17:57:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:16:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * clear vines traffic
 *
 */
EOLS	(clear_vines_trafficeol, vines_clear_cmd, CLEAR_VINES_TRAFFIC);
KEYWORD	(clear_vines_traffic, clear_vines_trafficeol, no_alt,
	 "traffic", "Zero all traffic statistics", PRIV_OPR );

/**********************************************************************
 * clear vines server { <address> | * } 
 *
 * OBJ(int,1) = TRUE if an address exists, FALSE if '*'
 * (*OBJ(paddr,1)) = <address>, or zero if all addresses.
 *
 */
EOLS	   (clear_vines_svreol, vines_clear_cmd, CLEAR_VINES_SERVER);
KEYWORD	   (clear_vines_svrall, clear_vines_svreol, no_alt,
	    "*", "Delete all entries", PRIV_OPR );

SET	   (clear_vines_svraddr_set, clear_vines_svreol, OBJ(int,1), TRUE);
VINESNET   (clear_vines_svrnum, clear_vines_svraddr_set, clear_vines_svrall,
	    OBJ(paddr,1), "server to delete from table");
KEYWORD_ID (clear_vines_server, clear_vines_svrnum, clear_vines_traffic,
	    OBJ(int,1), FALSE,
	    "server", "Delete server table entries", PRIV_OPR);

/**********************************************************************
 * clear vines route { <address> | * } 
 *
 * OBJ(int,1) = TRUE if an address exists, FALSE if '*'
 * (*OBJ(paddr,1)) = <address>, or zero if all addresses.
 *
 */
EOLS	   (clear_vines_rteol, vines_clear_cmd, CLEAR_VINES_ROUTE);
KEYWORD	   (clear_vines_rtall, clear_vines_rteol, no_alt,
	    "*", "Delete all entries", PRIV_OPR );

SET	   (clear_vines_rtaddr_set, clear_vines_rteol, OBJ(int,1), TRUE);
VINESNET   (clear_vines_rtnum, clear_vines_rtaddr_set, clear_vines_rtall,
	    OBJ(paddr,1), "Destination route to delete from routing table");
KEYWORD_ID (clear_vines_route, clear_vines_rtnum, clear_vines_server,
	    OBJ(int,1), FALSE,
	    "route", "Delete routing table entries", PRIV_OPR);

/**********************************************************************
 * clear vines neighbor { <address> | * } 
 *
 * OBJ(int,1) = TRUE if an address exists, FALSE if '*'
 * (*OBJ(paddr,1)) = <address>, or zero if all addresses.
 *
 */
EOLS	(clear_vines_neieol, vines_clear_cmd, CLEAR_VINES_NEIGHBOR);
KEYWORD	(clear_vines_neiall, clear_vines_neieol, no_alt,
	 "*", "Delete all entries", PRIV_OPR );

SET	(clear_vines_neiaddr_set, clear_vines_neieol, OBJ(int,1), TRUE);
VINADDR (clear_vines_neiaddr, clear_vines_neiaddr_set, clear_vines_neiall,
	OBJ(paddr,1), "A neighbor address to delete from neighbor table");
KEYWORD_ID (clear_vines_neighbor, clear_vines_neiaddr, clear_vines_route,
	OBJ(int,1), FALSE,
	"neighbor", "Delete neighbor table entries", PRIV_OPR);

/******************************************************************
 * clear vines ipc <connection>
 */
EOLNS	 (cl_v_ipc_eol, vipc_user_clear_conn);
NUMBER	 (cl_v_ipc_num, cl_v_ipc_eol, no_alt,
	  OBJ(int,1), 1, 65534, "Local connection number");
KEYWORD  (clear_vines_ipc, cl_v_ipc_num, clear_vines_neighbor,
	  "ipc", "Delete an IPC connection", PRIV_OPR);

/******************************************************************
 * clear vines cache
 * clear vines cache interface <idb>
 * clear vines cache neighbor <address>
 * clear vines cache server <number>
 * clear vines cache counters
 *
 * OBJ(int,1) = which flavor of the clear command
 * (*OBJ(paddr,1)) = <address>, or zero if all addresses.
 */
EOLS	   (cl_v_cache_eol, vines_clear_cmd, CLEAR_VINES_CACHE);
SET        (cl_v_cache_all, cl_v_cache_eol, OBJ(int,1), DO_ALL);
VINESNET   (cl_v_cache_servernum, cl_v_cache_eol, no_alt, OBJ(paddr,1),
	    "Destination server number");
KEYWORD_ID (cl_v_cache_server, cl_v_cache_servernum, cl_v_cache_all,
	    OBJ(int,1), DO_SERVER,
	    "server", "Delete by server number", PRIV_OPR);
VINADDR    (cl_v_cache_neiaddr, cl_v_cache_eol, no_alt,
	    OBJ(paddr,1), "A neighbor address to delete from neighbor table");
KEYWORD_ID (cl_v_cache_neigh, cl_v_cache_neiaddr, cl_v_cache_server,
	    OBJ(int,1), DO_NEIGHBOR,
	    "neighbor", "Delete by neighbor", PRIV_OPR);
INTERFACE  (cl_v_cache_getidb, cl_v_cache_eol, no_alt,
	    OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD_ID (cl_v_cache_idb, cl_v_cache_getidb, cl_v_cache_neigh,
	    OBJ(int,1), DO_INTERFACE,
	    common_str_interface, "Delete by interface", PRIV_OPR);
EOLS	   (cl_v_cache_cnt_eol, vines_clear_cmd, CLEAR_VINES_CACHE_COUNTERS);
KEYWORD    (cl_v_cache_counters, cl_v_cache_cnt_eol, cl_v_cache_idb,
	    "counters", "Clear FS cache and counters", PRIV_OPR);
KEYWORD    (clear_vines_cache, cl_v_cache_counters, clear_vines_ipc,
	    "cache", "Delete fastswitching cache entries", PRIV_OPR);

/**********************************************************************
 * clear vines ...
 */
KEYWORD (clear_vines, clear_vines_cache, ALTERNATE,
	"vines", "VINES neighbor and routing table entries", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_vines
