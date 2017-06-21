/* $Id: exec_show_clns.h,v 3.2.60.2 1996/05/21 09:43:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/exec_show_clns.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ C L N S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_clns.h,v $
 * Revision 3.2.60.2  1996/05/21  09:43:54  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.60.1  1996/05/04  20:04:13  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:53:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:47:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:22:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show clns
 */
EOLS	(show_clns_traffic_global, show_clns, SHOW_CLNS_GLOBAL );

/******************************************************************
* show clns filter-set [name]
*
* OBJ(int,1) = TRUE if <name> present
* OBJ(string,1) = name
*/
EOLS    (show_clns_filset_eol, show_clns, SHOW_CLNS_FILTER_SET);
SET     (show_clns_filset_set, show_clns_filset_eol, OBJ(int,1), TRUE);
STRING  (show_clns_filset_name, show_clns_filset_set, show_clns_filset_eol,
	 OBJ(string,1), "A CLNS filter set name");
KEYWORD (show_clns_filter_set, show_clns_filset_name, show_clns_traffic_global,
	 "filter-set", "CLNS filter sets", PRIV_USER);

/******************************************************************
* show clns filter-expr [name] [detail]
*
* OBJ(int,1) = TRUE if <name> present
* OBJ(int,2) = TRUE if <detail> present
* OBJ(string,1) = name
*/
pdecl(show_clns_filexp_detail);
EOLS    (show_clns_filexp_eol, show_clns, SHOW_CLNS_FILTER_EXPR);
KEYWORD_ID(show_clns_filexp_detail2, show_clns_filexp_eol, 
	   show_clns_filexp_eol, OBJ(int,2), TRUE, 
	   "detail", "Detailed filter expression", 
	   PRIV_USER);
SET     (show_clns_filexp_set2, show_clns_filexp_detail2, OBJ(int,1), TRUE);
STRING  (show_clns_filexp_name2, show_clns_filexp_set2, show_clns_filexp_eol,
	 OBJ(string,1), "A CLNS filter expression name");
TEST_MULTIPLE_FUNCS(show_clns_filexp_ambig, show_clns_filexp_name2,
		    NONE, no_alt);
SET     (show_clns_filexp_set1, show_clns_filexp_eol, OBJ(int,1), TRUE);
STRING  (show_clns_filexp_name1, show_clns_filexp_set1, show_clns_filexp_eol,
	 OBJ(string,1), "A CLNS filter expression name");
KEYWORD_ID(show_clns_filexp_detail1, show_clns_filexp_name1, 
	   show_clns_filexp_ambig, OBJ(int,2), TRUE, 
	   "detail", "Detailed filter expression", 
	   PRIV_USER);
KEYWORD (show_clns_filter_expr, show_clns_filexp_detail1, show_clns_filter_set,
	 "filter-expr", "CLNS filter expressions", PRIV_USER);

/******************************************************************
 * show clns traffic
 * 
 */

EOLS	(show_clns_traffic_eol, show_clns, SHOW_CLNS_TRAFFIC);
KEYWORD (show_clns_traffic_kwd, show_clns_traffic_eol, show_clns_filter_expr,
	 "traffic", "CLNS protocol statistics", PRIV_USER);

/******************************************************************
 * show clns route [{ <nsap> | <clns-hostname> }]
 * 
 * (*OBJ(paddr,1)) = nsap
 */

EOLS	(show_clns_route_eol, show_clns, SHOW_CLNS_ROUTE);
NSAPADDR (show_clns_route_nsap, show_clns_route_eol, show_clns_route_eol,
	  OBJ(paddr,1),"CLNS address or host name");
KEYWORD (show_clns_route_kwd, show_clns_route_nsap, show_clns_traffic_kwd,
	 "route", "CLNS prefix routing table", PRIV_USER);

/******************************************************************
 * show clns redirects
 * 
 */

EOLS	(show_clns_redirects_eol, show_clns, SHOW_CLNS_REDIRECTS);
KEYWORD (show_clns_redirects, show_clns_redirects_eol, show_clns_route_kwd,
	 "redirects", "CLNS redirect information", PRIV_USER);

/******************************************************************
 * show clns protocol [<domain>]
 * 
 * OBJ(string,1) = domain
 */

EOLS	(show_clns_protocol_eol, show_clns, SHOW_CLNS_PROTOCOL);
STRING (show_clns_protocol_domain, show_clns_protocol_eol, show_clns_protocol_eol,
        OBJ(string,1),"CLNS domain");
KEYWORD (show_clns_protocol, show_clns_protocol_domain, show_clns_redirects,
	 "protocol", "CLNS routing protocol process information", PRIV_USER);

/******************************************************************
 * show clns neighbors [<interface>] [detail]
 * 
 * OBJ(int,1) =  TRUE = "detail" keyword exists
 *		FALSE = no "detail" keyword
 * OBJ(idb,1) = <interface>
 */

EOLS	(show_clns_neighbors_eol, show_clns, SHOW_CLNS_NEIGHBORS);
KEYWORD_ID (show_clns_neigh_detail, show_clns_neighbors_eol, show_clns_neighbors_eol,
	OBJ(int,1), TRUE,
	"detail", "Show detailed information", PRIV_USER);
INTERFACE (show_clns_neigh_int, show_clns_neigh_detail, show_clns_neigh_detail,
	OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_clns_neighbors, show_clns_neigh_int, show_clns_protocol,
	 "neighbors", "CLNS neighbor adjacencies", PRIV_USER);

/******************************************************************
 * show clns is-neighbors [<interface>] [detail]
 * 
 * OBJ(int,1) =	TRUE = "detail" keyword exists
 *		FALSE = no "detail" keyword
 * OBJ(idb,1) = <interface>
 */

EOLS	(show_clns_is_neighbors_eol, show_clns, SHOW_CLNS_IS_NEIGHBORS);
KEYWORD_ID (show_clns_is_neigh_detail, show_clns_is_neighbors_eol, show_clns_is_neighbors_eol,
	OBJ(int,1), TRUE,
	"detail", "Show detailed information", PRIV_USER);
INTERFACE (show_clns_is_neigh_int, show_clns_is_neigh_detail, show_clns_is_neigh_detail,
	OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_clns_is_neighbors, show_clns_is_neigh_int, show_clns_neighbors,
	 "is-neighbors", "IS neighbor adjacencies", PRIV_USER);

/******************************************************************
 * show clns interface [<interface>]
 * 
 * OBJ(idb,1) = <interface>
 */

EOLS	(show_clns_interface_eol, show_clns, SHOW_CLNS_INTERFACE);
INTERFACE (show_clns_interface_get_interface, show_clns_interface_eol, show_clns_interface_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_clns_interface, show_clns_interface_get_interface,
	    show_clns_is_neighbors,
	 common_str_interface, "CLNS interface status and configuration", PRIV_USER);

/******************************************************************
 * show clns es-neighbors [<interface>] [detail]
 * 
 * OBJ(int,1) =	TRUE = "detail" keyword exists
 *		FALSE = no "detail" keyword
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_clns_es_neighbors_eol, show_clns, SHOW_CLNS_ES_NEIGHBORS);
KEYWORD_ID (show_clns_es_neigh_detail, show_clns_es_neighbors_eol, show_clns_es_neighbors_eol,
	OBJ(int,1), TRUE,
	"detail", "Show detailed information", PRIV_USER);
INTERFACE (show_clns_es_neigh_int, show_clns_es_neigh_detail, show_clns_es_neigh_detail,
	OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_clns_es_neighbors, show_clns_es_neigh_int, show_clns_interface,
	 "es-neighbors", "ES neighbor adjacencies", PRIV_USER);

/******************************************************************
 * show clns cache[invalidations | delay-parameters]
 *
 * OBJ(int,2) = TRUE if "delay-parameters" present
 * OBJ(int,1) = TRUE if "invalidations" present
 */

EOLS	(show_clns_cache_eol, show_clns, SHOW_CLNS_CACHE);
KEYWORD_ID(show_clns_cache_delay, show_clns_cache_eol, show_clns_cache_eol,
	   OBJ(int,2), TRUE, "delay-parameters", "Cache delay parameters",
	   PRIV_USER);
KEYWORD_ID(show_clns_cache_inval, show_clns_cache_eol, show_clns_cache_delay,
	   OBJ(int,1), TRUE, "invalidations", "Invalidation statistics",
	   PRIV_USER);
KEYWORD (show_clns_cache, show_clns_cache_inval, show_clns_es_neighbors,
	 "cache", "CLNS fast-switching route cache", PRIV_USER);

LINK_TRANS(show_clns_extend_here, show_clns_cache);

/******************************************************************
 * show clns 
 * 
 */

EOLS	(show_clns_eol, show_clns, SHOW_CLNS);
KEYWORD (show_clns, show_clns_extend_here, ALTERNATE,
	 "clns", "CLNS network information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_clns
