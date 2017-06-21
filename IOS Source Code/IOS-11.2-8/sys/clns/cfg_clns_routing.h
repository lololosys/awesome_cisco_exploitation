/* $Id: cfg_clns_routing.h,v 3.2.60.2 1996/05/04 20:03:44 asastry Exp $
 * $Source: /release/112/cvs/Xsys/clns/cfg_clns_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ C L N S _ R O U T I N G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_clns_routing.h,v $
 * Revision 3.2.60.2  1996/05/04  20:03:44  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:02:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:12:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  13:39:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:51:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:06:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:02:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * clns cache-invalidate-delay [min [max quiet thresh]]
 * no clns cache-invalidate-delay
 *
 * OBJ(int,1) = minimum time in seconds
 * OBJ(int,2) = maximum time in seconds (NZ if any parameters specified)
 * OBJ(int,3) = quiet interval in seconds
 * OBJ(int,4) = quiet threshold
 *
 * "no" turns it off
 * no parameters set defaults
 */
EOLS    (clns_cache_delay_eol, clns_commands, CLNS_CACHE_INVALIDATE);
NUMBER  (clns_cache_delay_thresh, clns_cache_delay_eol, no_alt,
	 OBJ(int,4), 0, -1, "Invalidation request quiet threshold");
NUMBER  (clns_cache_delay_quiet, clns_cache_delay_thresh, no_alt,
	 OBJ(int,3), 1, 600, "Quiet interval in seconds");
NUMBER  (clns_cache_delay_max, clns_cache_delay_quiet, no_alt,
	 OBJ(int,2), 1, 300, "Maximum delay in seconds");
NUMBER  (clns_cache_delay_min, clns_cache_delay_max, clns_cache_delay_eol,
	 OBJ(int,1), 0, 300, "Minimum delay in seconds");
NOPREFIX(clns_cache_delay_no, clns_cache_delay_min, clns_cache_delay_eol);
NVGENS  (clns_cache_delay_nv, clns_cache_delay_no, clns_commands,
	 CLNS_CACHE_INVALIDATE);
KEYWORD (clns_cache_delay, clns_cache_delay_nv, no_alt,
         "cache-invalidate-delay", "Delay CLNS route cache invalidation",
	 PRIV_CONF);

/******************************************************************************
 * clns route <addr> | default <next-hop_addr>
 * clns route <addr> | default <interface>
 * clns route <addr> | default discard
 */
EOLS	(clns_route_eol, clns_commands, CLNS_ROUTE);

SET	(clns_route_name_set, clns_route_eol, OBJ(int,10), CLNS_ROUTE_NAME);
NSAPADDR(clns_route_name, clns_route_name_set, no_alt,
	 OBJ(paddr,2), "NSAP address or host name of next hop router");

TEST_MULTIPLE_FUNCS(clns_route_ambig, clns_route_name, clns_route_eol, NONE);

KEYWORD_ID(clns_route_discard, clns_route_eol, clns_route_ambig,
	   OBJ(int,10), CLNS_ROUTE_DISCARD,
	   "discard", "Discard packets with the NSAP prefix", PRIV_CONF);

SNPAADDR(clns_route_int_snpa, clns_route_eol, no_alt,
	 OBJ(hwaddr,1), "SNPA address");
IFELSE	(clns_route_int_eval, clns_route_eol, clns_route_int_snpa,
	 is_p2p(GETOBJ(idb,1)));
SET	(clns_route_int_set, clns_route_int_eval, OBJ(int,10),
	 CLNS_ROUTE_INTERFACE);
INTERFACE(clns_route_int, clns_route_int_set, clns_route_discard,
	  OBJ(idb,1), IFTYPE_ANYSUB);

NSAPADDR(clns_route_addr, clns_route_int, NONE, OBJ(paddr,1), "NSAP prefix");
TEST_MULTIPLE_FUNCS(clns_route_addr_ambig, clns_route_addr, NONE, no_alt);
EVAL	(clns_route_default_eval, clns_route_int,
	 (GETOBJ(paddr,1)->addr.clns_address[0] = 0,
	  GETOBJ(paddr,1)->type = ADDR_CLNS,
	  GETOBJ(paddr,1)->length = ADDRLEN_CLNS));
KEYWORD	(clns_route_default, clns_route_default_eval, clns_route_addr_ambig,
	 "default", "Default prefix", PRIV_CONF);
NVGENS	(clns_route_nvgen, clns_route_default, clns_commands, CLNS_ROUTE); 
KEYWORD	(clns_route, clns_route_nvgen, clns_cache_delay,
	 "route", "Define a CLNS static route", PRIV_CONF);

/******************************************************************************
 * clns host <name> <OBJ(paddr,1)> [ <OBJ(paddr,2)> <OBJ(paddr,3)> .... <OBJ(paddr,8)> ]
 * no clns host <name>
 *
 */
EOLS	(clns_host_eol, clns_commands, CLNS_HOST);

NSAPADDR(clns_host_addr8, clns_host_eol, clns_host_eol,
	 OBJ(paddr,8), "NSAP address to assign name to");
NSAPADDR(clns_host_addr7, clns_host_addr8, clns_host_eol,
	 OBJ(paddr,7), "NSAP address to assign name to");
NSAPADDR(clns_host_addr6, clns_host_addr7, clns_host_eol,
	 OBJ(paddr,6), "NSAP address to assign name to");
NSAPADDR(clns_host_addr5, clns_host_addr6, clns_host_eol,
	 OBJ(paddr,5), "NSAP address to assign name to");
NSAPADDR(clns_host_addr4, clns_host_addr5, clns_host_eol,
	 OBJ(paddr,4), "NSAP address to assign name to");
NSAPADDR(clns_host_addr3, clns_host_addr4, clns_host_eol,
	 OBJ(paddr,3), "NSAP address to assign name to");
NSAPADDR(clns_host_addr2, clns_host_addr3, clns_host_eol,
	 OBJ(paddr,2), "NSAP address to assign name to");
NSAPADDR(clns_host_addr1, clns_host_addr2, no_alt,
	 OBJ(paddr,1), "NSAP address to assign name to");
NOPREFIX(clns_host_noprefix, clns_host_addr1, clns_host_eol);
STRING	(clns_host_name, clns_host_noprefix, no_alt,
	 OBJ(string,1), "Name to give to a NSAP");
NVGENS	(clns_host_nvgen, clns_host_name, clns_commands, CLNS_HOST);
KEYWORD	(clns_host, clns_host_nvgen, clns_route,
	 "host", "Define a name mapping for an NSAP address", PRIV_CONF);

KEYWORD (cr_clns, clns_host, ALTERNATE,
	 "clns", " ", PRIV_CONF | PRIV_DUPLICATE);

#undef	ALTERNATE
#define	ALTERNATE	cr_clns
