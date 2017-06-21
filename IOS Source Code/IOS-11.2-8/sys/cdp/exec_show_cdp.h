/* $Id: exec_show_cdp.h,v 3.2.60.2 1996/05/21 09:43:34 thille Exp $ 
 * $Source: /release/112/cvs/Xsys/cdp/exec_show_cdp.h,v $
 *------------------------------------------------------------------
 * Support for CDP show routines
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_cdp.h,v $
 * Revision 3.2.60.2  1996/05/21  09:43:34  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.60.1  1996/04/30  21:29:57  dawnli
 * CSCdi53927:  Show cdp entry should accept wildcards
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:49:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:47:22  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:22:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show cdp 
 */
EOLS	(show_cdp_counters_global, show_cdp, SHOW_CDP_GLOBAL);

/******************************************************************
* show cdp entry <name> [protocol] [version]
*
* OBJ(int,1) = TRUE if <protocol> present
* OBJ(int,2) = TRUE if <version> present
* OBJ(string,1) = name
*/
EOLS    (show_cdp_entry_eol, show_cdp, SHOW_CDP_HOST);
KEYWORD_ID(show_cdp_entry_protocol, show_cdp_entry_eol,
	   show_cdp_entry_eol, OBJ(int,1), TRUE, 
	   "protocol", "Protocol information",
	   PRIV_USER);
KEYWORD_ID(show_cdp_entry_version, show_cdp_entry_protocol,
	   show_cdp_entry_protocol, OBJ(int,2), TRUE, 
	   "version", "Version information",
	   PRIV_USER);
STRING  (show_cdp_entry_name, show_cdp_entry_version, show_cdp_entry_eol,
	 OBJ(string,1), "Name of CDP neighbor entry");
HELP	(show_cdp_entry_all, show_cdp_entry_name,
	 "  *\tall CDP neighbor entries\n");
KEYWORD (show_cdp_entry, show_cdp_entry_all, show_cdp_counters_global,
	 "entry", "Information for specific neighbor entry", PRIV_USER);

/******************************************************************
 * show cdp neighbors [interface] [detail]
 *
 * OBJ(int,1) =  TRUE = "detail" keyword exists
 *		FALSE = no "detail" keyword
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_cdp_neighbors_eol, show_cdp, SHOW_CDP_NEIGHBORS);
KEYWORD_ID (show_cdp_neigh_detail, show_cdp_neighbors_eol,
	    show_cdp_neighbors_eol, OBJ(int,1), TRUE,
	    "detail", "Show detailed information", PRIV_USER);
INTERFACE (show_cdp_neigh_int, show_cdp_neigh_detail, show_cdp_neigh_detail,
	OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_cdp_neighbors, show_cdp_neigh_int, show_cdp_entry,
	 "neighbors", "CDP neighbor entries", PRIV_USER);

/******************************************************************
 * show cdp traffic
 */
EOLS	(show_cdp_traffic_eol, show_cdp, SHOW_CDP_COUNTERS);
KEYWORD (show_cdp_traffic, show_cdp_traffic_eol, show_cdp_neighbors,
	 "traffic", "CDP statistics", PRIV_USER);

/******************************************************************
 * show cdp interface [<interface>]
 * 
 * OBJ(idb,1) = <interface>
 */

EOLS	(show_cdp_interface_eol, show_cdp, SHOW_CDP_INTERFACE);
INTERFACE (show_cdp_get_interface, show_cdp_interface_eol,
	   show_cdp_interface_eol, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_cdp_int, show_cdp_get_interface, show_cdp_traffic,
	 common_str_interface, "CDP interface status and configuration", PRIV_USER);


/******************************************************************
 * show cdp
 */
EOLS	(show_cdp_eol, show_cdp, SHOW_CDP_GLOBAL);
KEYWORD (show_cdp, show_cdp_int, ALTERNATE,
	 "cdp", "CDP information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_cdp
