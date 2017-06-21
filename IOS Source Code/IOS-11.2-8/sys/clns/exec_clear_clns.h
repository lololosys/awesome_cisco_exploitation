/* $Id: exec_clear_clns.h,v 3.2 1995/11/17 08:53:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/exec_clear_clns.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ C L N S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_clns.h,v $
 * Revision 3.2  1995/11/17  08:53:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:47:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:15:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear clns route
 *
 */
EOLS	(clear_clns_route_eol, clear_clns_command, CLEAR_CLNS_ROUTE);

KEYWORD (clear_clns_route, clear_clns_route_eol, no_alt,
	 "route", "CLNS dynamic routing information", PRIV_OPR);

/******************************************************************
 * clear clns [ es-neighbor | is-neighbor | neighbor ]
 *
 * The cisco comments say:
 *      clear clns [is-|es-]neighbors [<net> | <tag>]
 * but the code doesn't handle the <net> | <tag>.
 * We implement the code's versoin.
 */
EOLS	(clear_clns_neigh_eol, clear_clns_command, CLEAR_CLNS_NEIGHBORS);
KEYWORD (clear_clns_neigh, clear_clns_neigh_eol, clear_clns_route,
	"neighbors", "All CLNS neighbor routes", PRIV_OPR);

EOLS	(clear_clns_isneigh_eol, clear_clns_command, CLEAR_CLNS_IS_NEIGHBORS);
KEYWORD (clear_clns_isneigh, clear_clns_isneigh_eol, clear_clns_neigh,
	"is-neighbors", "CLNS intermediate-system neighbor routes", PRIV_OPR);

EOLS	(clear_clns_esneigh_eol, clear_clns_command, CLEAR_CLNS_ES_NEIGHBORS);
KEYWORD (clear_clns_esneigh, clear_clns_esneigh_eol, clear_clns_isneigh,
	"es-neighbors", "CLNS end-system neighbor routes", PRIV_OPR);

/******************************************************************
 * clear clns cache
 *
 */
EOLS	(clear_clns_cache_eol, clear_clns_command, CLEAR_CLNS_CACHE);
KEYWORD (clear_clns_cache, clear_clns_cache_eol, clear_clns_esneigh,
	 "cache", "CLNS routing cache", PRIV_OPR);

/******************************************************************/
KEYWORD (clear_clns, clear_clns_cache, ALTERNATE,
	"clns", "CLNS", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_clns
