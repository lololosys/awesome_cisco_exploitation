/* $Id: exec_show_vines.h,v 3.2.62.1 1996/05/21 10:08:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/exec_show_vines.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ V I N E S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_vines.h,v $
 * Revision 3.2.62.1  1996/05/21  10:08:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  17:57:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:26:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show vines traffic
 */
EOLS	   (sh_v_tr_eol, show_vines, SHOW_VINES_TRAFFIC);
INTERFACE  (sh_v_tr_getidb, sh_v_tr_eol, sh_v_tr_eol, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD    (show_vines_traffic, sh_v_tr_getidb, no_alt,
            "traffic", "Show protocol statistics", PRIV_USER);

/******************************************************************
 * show vines timers
 */
EOLS	   (sh_v_tm_eol, show_vines, SHOW_VINES_TIMERS);
KEYWORD_ID (sh_v_tm_ver, sh_v_tm_eol, sh_v_tm_eol,
	    OBJ(int,1), TRUE,
            "verbose", "Include times for last update pass", PRIV_USER|PRIV_HIDDEN);
KEYWORD    (show_vines_timers, sh_v_tm_ver, show_vines_traffic,
            "timers", "Show protocol timers", PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show vines services
 * show vines services tree
 * show vines services { nsm | vs | st | ss | fs } [procedures]
 */
EOLS	   (sh_v_s_eol, vsvc_show_services, SHOW_VINES_SERVICES);

KEYWORD_ID (sh_v_s_proc, sh_v_s_eol, sh_v_s_eol,
	    OBJ(int,3), TRUE,
            "procedures", "supported procedures", PRIV_USER|PRIV_HIDDEN);

KEYWORD_ID (sh_v_s_nsm, sh_v_s_proc, sh_v_s_eol,
	    OBJ(int,2), VIPC_PORT_NSM,
            "nsm", "Network and System Management information", PRIV_USER);
KEYWORD_ID (sh_v_s_vs, sh_v_s_proc, sh_v_s_nsm,
	    OBJ(int,2), VIPC_PORT_VS,
            "vs", "Security Service information", PRIV_USER);
KEYWORD_ID (sh_v_s_st, sh_v_s_proc, sh_v_s_vs,
	    OBJ(int,2), VIPC_PORT_ST,
            "st", "Streettalk information", PRIV_USER|PRIV_HIDDEN);
KEYWORD_ID (sh_v_s_ss, sh_v_s_proc, sh_v_s_st,
	    OBJ(int,2), VIPC_PORT_SERVER,
            "ss", "Server Service information", PRIV_USER);
KEYWORD_ID (sh_v_s_fs, sh_v_s_proc, sh_v_s_ss,
	    OBJ(int,2), VIPC_PORT_FILE,
            "fs", "File Service information", PRIV_USER);
SET	   (sh_v_s_data, sh_v_s_fs, OBJ(int,1), SH_V_S_DATA);

KEYWORD_ID (sh_v_s_tree, sh_v_s_eol, sh_v_s_data,
	    OBJ(int,1), SH_V_S_TREE,
            "tree", "server tree information", PRIV_USER|PRIV_HIDDEN);

KEYWORD    (show_vines_services, sh_v_s_tree, show_vines_timers,
            "service", "Show service information", PRIV_USER);

/******************************************************************
 * show vines route
 * show vines route <net>
 * show vines route neighbor <addr>
 * show vines route (by) metric
 *
 * OBJ(int,1) = type of show command
 * OBJ(paddr,1) = optional server number or neighbor address
 */
EOLS	   (sh_v_r_eol, show_vines, SHOW_VINES_ROUTE);
SET        (sh_v_r_all, sh_v_r_eol, OBJ(int,1), DO_ALL);
KEYWORD_ID (sh_v_r_metric, sh_v_r_eol, sh_v_r_all,
	    OBJ(int,1), BY_METRIC,
	    "metric", "Display routes by metric", PRIV_USER);
VINADDR	   (sh_v_r_getneighbor, sh_v_r_eol, no_alt,
	    OBJ(paddr,1), "VINES address");
KEYWORD_ID (sh_v_r_neighbor, sh_v_r_getneighbor, sh_v_r_metric,
	    OBJ(int,1), DO_NEIGHBOR,
	    "neighbor", "Display routes through neighbor", PRIV_USER);
SET        (sh_v_r_addrgiven, sh_v_r_eol, OBJ(int,1), DO_SERVER);
VINESNET   (sh_v_r_addr, sh_v_r_addrgiven, sh_v_r_neighbor,
            OBJ(paddr,1), "VINES route to show");
KEYWORD    (show_vines_route, sh_v_r_addr, show_vines_services,
            "route", "Show routing table entries", PRIV_USER);

/******************************************************************
 * show vines private <neighbor | route>
 */

EOLS	   (s_v_p_r_eol, vroute_ShowStats, SHOW_VINES_ROUTE);
KEYWORD    (s_v_p_route, s_v_p_r_eol, no_alt,
            "route", "Network tree statistics", PRIV_USER);

EOLS	   (s_v_p_n_eol, vneigh_ShowStats, SHOW_VINES_NEIGHBORS);
KEYWORD    (s_v_p_neighbor, s_v_p_n_eol, s_v_p_route,
            "neighbor", "Neighbor tree statistics", PRIV_USER);

KEYWORD    (show_vines_private, s_v_p_neighbor, show_vines_route,
            "private", "Show tree statistics", PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show vines neighbor
 * show vines neighbor <net>
 * show vines neighbor interface <idb>
 *
 * OBJ(int,1)  = type of show command
 * OBJ(paddr1) = optional neighbor address
 * OBJ(idb,1)  = optional interface
 */
EOLS	   (sh_v_n_eol, show_vines, SHOW_VINES_NEIGHBORS);
SET        (sh_v_n_all, sh_v_n_eol, OBJ(int,1), DO_ALL);
VINESNET   (sh_v_n_servernum, sh_v_n_eol, no_alt, OBJ(paddr,1),
	    "Server number");
KEYWORD_ID (sh_v_n_server, sh_v_n_servernum, sh_v_n_all,
	    OBJ(int,1), DO_SERVER,
	    "server", "Display entries by server number", PRIV_USER);
INTERFACE  (sh_v_n_getidb, sh_v_n_eol, no_alt, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD_ID (sh_v_n_idb, sh_v_n_getidb, sh_v_n_server,
	    OBJ(int,1), DO_INTERFACE,
	    common_str_interface, "Display neighbors by interface", PRIV_USER);
SET        (sh_v_n_addrgiven, sh_v_n_eol, OBJ(int,1), DO_ONE);
VINADDR    (sh_v_n_addr, sh_v_n_addrgiven, sh_v_n_idb,
            OBJ(paddr,1), "Display specific neighbor");
KEYWORD    (show_vines_neighbors, sh_v_n_addr, show_vines_private,
            "neighbor", "Show neighbor table entries", PRIV_USER);

/******************************************************************
 * show vines ipc
 *
 */
EOLS	(show_vines_ipc_eol, show_vines, SHOW_VINES_IPC);
KEYWORD (show_vines_ipc, show_vines_ipc_eol, show_vines_neighbors,
            "ipc", "Show IPC protocol information", PRIV_USER);


/******************************************************************
 * show vines interface [<interface>]
 *
 * OBJ(int,1) = TRUE if interface exists
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_vines_interface_eol, show_vines, SHOW_VINES_INTERFACE);
SET     (show_vines_ifname_set, show_vines_interface_eol,
         OBJ(int,1), TRUE);
INTERFACE (show_vines_ifname, show_vines_ifname_set, show_vines_interface_eol,
         OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_vines_if, show_vines_ifname, show_vines_ipc,
            common_str_interface, "Show interface status and configuration", PRIV_USER);


/******************************************************************
 * show vines host [<name>]
 *
 * OBJ(int,1) = TRUE if name exists
 * OBJ(string,1) = <name>
 */
EOLS	(show_vines_host_eol, show_vines, SHOW_VINES_HOST);
SET     (show_vines_hostname_set, show_vines_host_eol, 
         OBJ(int,1), TRUE);
STRING  (show_vines_hostname, show_vines_hostname_set, show_vines_host_eol,
         OBJ(string,1), "Hostname to show");
KEYWORD (show_vines_host, show_vines_hostname, show_vines_if,
            "host", "Show host name/number table", PRIV_USER);

/******************************************************************
 * show vines cache
 */
EOLS	   (sh_v_c_eol, show_vines, SHOW_VINES_CACHE);
SET        (sh_v_c_all, sh_v_c_eol, OBJ(int,1), DO_ALL);
SET        (sh_v_c_addrgiven, sh_v_c_eol, OBJ(int,1), DO_ONE);
VINADDR    (sh_v_c_addr, sh_v_c_addrgiven, sh_v_c_all,
            OBJ(paddr,1), "Display specific entry");
VINESNET   (sh_v_c_servernum, sh_v_c_eol, no_alt, OBJ(paddr,1),
	    "Server number");
KEYWORD_ID (sh_v_c_server, sh_v_c_servernum, sh_v_c_addr,
	    OBJ(int,1), DO_SERVER,
	    "server", "Display entries by server number", PRIV_USER);
VINADDR	   (sh_v_c_getneighbor, sh_v_c_eol, no_alt,
	    OBJ(paddr,1), "VINES address");
KEYWORD_ID (sh_v_c_neighbor, sh_v_c_getneighbor, sh_v_c_server,
	    OBJ(int,1), DO_NEIGHBOR,
	    "neighbor", "Display entries by neighbor", PRIV_USER);
INTERFACE  (sh_v_c_getidb, sh_v_c_eol, no_alt, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD_ID (sh_v_c_idb, sh_v_c_getidb, sh_v_c_neighbor,
	    OBJ(int,1), DO_INTERFACE,
	    common_str_interface, "Display entries by interface", PRIV_USER);
KEYWORD    (show_vines_cache, sh_v_c_idb, show_vines_host,
            "cache", "Show fastswitching cache entries", PRIV_USER);

/******************************************************************
 * show vines both
 *
 * Does both a 'show vines neighbor' and a 'show vines route'
 */
EOLS	(show_vines_both_eol, show_vines,        SHOW_VINES_BOTH);
VINESNET(show_vines_both_addr, show_vines_both_eol, show_vines_both_eol,
         OBJ(paddr,1), "VINES route to show");
KEYWORD (show_vines_both, show_vines_both_addr, show_vines_cache,
            "both", "Show both route and neighbor tables", PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show vines access-list [<list>]
 *
 * OBJ(int,1) = <list>
 *		-1 if no list
 */
EOLS    (show_vines_access_eol, show_vines, SHOW_VINES_ACCESS);
SET	(show_vines_access_nonum, show_vines_access_eol, OBJ(int,1), -1);
NUMBER (show_vines_access_num, show_vines_access_eol, show_vines_access_nonum,
	OBJ(int,1), VINESMINACC, VINESMAXACC, "VINES access list");
KEYWORD (show_vines_access, show_vines_access_num, show_vines_both,
            "access", "Show access lists", PRIV_USER);


/******************************************************************
 * show vines
 */
KEYWORD (show_vines, show_vines_access, ALTERNATE,
	 "vines", "VINES information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_vines
