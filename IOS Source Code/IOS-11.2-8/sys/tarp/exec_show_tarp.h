/* $Id: exec_show_tarp.h,v 3.3.20.1 1996/05/21 10:05:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/tarp/exec_show_tarp.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_tarp.h,v $
 * Revision 3.3.20.1  1996/05/21  10:05:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/02/01  23:34:36  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:44:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show tarp
 */
EOLS	(show_tarp_counters_global, show_tarp, SHOW_TARP_GLOBAL);

/******************************************************************
* show tarp host <name>
*
* OBJ(string,1) = name
*/
EOLS    (show_tarp_entry_eol, show_tarp, SHOW_TARP_HOST);
STRING  (show_tarp_entry_name, show_tarp_entry_eol, no_alt,
	 OBJ(string,1), "Name of TARP neighbor entry");
KEYWORD (show_tarp_entry, show_tarp_entry_name, show_tarp_counters_global,
	 "host", "Information about a specific TARP host", PRIV_USER);

/******************************************************************
 * show tarp ldb
 *
 */
EOLS	(show_tarp_ldb_cache_eol, show_tarp, SHOW_TARP_LDB_CACHE);
KEYWORD (show_tarp_ldb_cache, show_tarp_ldb_cache_eol, show_tarp_entry,
	 "ldb", "TARP LDB (Loop Detection Buffer)", PRIV_USER);

/******************************************************************
 * show tarp tid-cache [detail]
 *
 * OBJ(int,1) =  TRUE = "detail" keyword exists.
 *               FALSE = no "detail" keyword.
 */
EOLS	(show_tarp_tid_cache_eol, show_tarp, SHOW_TARP_TID_CACHE);
KEYWORD_ID (show_tarp_tid_cache_detail, show_tarp_tid_cache_eol,
	    show_tarp_tid_cache_eol, OBJ(int,1), TRUE,
	    "detail", "Show detailed information", PRIV_USER);
KEYWORD (show_tarp_tid_cache, show_tarp_tid_cache_detail, show_tarp_ldb_cache,
	 "tid-cache", "TARP TID<->NET cache", PRIV_USER);

/******************************************************************
 * show tarp blacklisted-adjacencies
 */
EOLS	(show_tarp_blacklist_adj_eol, show_tarp, SHOW_TARP_BLACKLIST_ADJ);
KEYWORD (show_tarp_blacklist_adj, show_tarp_blacklist_adj_eol,
	 show_tarp_tid_cache, "blacklisted-adjacencies",
	 "Display all adjacencies we won't propagate PDUs to",
	 PRIV_USER);

/******************************************************************
 * show tarp static-adjacencies
 */
EOLS	(show_tarp_static_adj_eol, show_tarp, SHOW_TARP_STATIC_ADJ);
KEYWORD (show_tarp_static_adj, show_tarp_static_adj_eol,
	 show_tarp_blacklist_adj, "static-adjacencies",
	 "Display all static TARP adjacencies", PRIV_USER);

/******************************************************************
 * show tarp map
 */
EOLS	(show_tarp_map_eol, show_tarp, SHOW_TARP_STATIC_MAP);
KEYWORD (show_tarp_map, show_tarp_map_eol,
	 show_tarp_static_adj, "map",
	 "Display all static TARP (TID<->NET) maps", PRIV_USER);

/******************************************************************
 * show tarp traffic
 */
EOLS	(show_tarp_traffic_eol, show_tarp, SHOW_TARP_COUNTERS);
KEYWORD (show_tarp_traffic, show_tarp_traffic_eol, show_tarp_map,
	 "traffic", "TARP statistics", PRIV_USER);

/******************************************************************
 * show tarp interface [<interface>]
 *
 * OBJ(idb,1) = <interface>
 */

EOLS	(show_tarp_interface_eol, show_tarp, SHOW_TARP_INTERFACE);
INTERFACE (show_tarp_get_interface, show_tarp_interface_eol,
	   show_tarp_interface_eol, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_tarp_int, show_tarp_get_interface, show_tarp_traffic,
	 common_str_interface, "TARP interface status and configuration", PRIV_USER);


/******************************************************************
 * show tarp
 */
EOLS	(show_tarp_eol, show_tarp, SHOW_TARP_GLOBAL);
KEYWORD (show_tarp, show_tarp_int, ALTERNATE,
	 "tarp", "TARP information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_tarp
