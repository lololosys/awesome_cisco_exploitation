/* $Id: cfg_router_aggregate_addr.h,v 3.3.20.1 1996/08/12 16:03:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_aggregate_addr.h,v $
 *------------------------------------------------------------------
 * cfg_router_aggregate_addr.h
 *
 * October 1993, Paul Traina
 *
 * Supports the router subcommand "aggregate-address". Provides BGP aggregation
 * configuration.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_router_aggregate_addr.h,v $
 * Revision 3.3.20.1  1996/08/12  16:03:39  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:54  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:31:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:47  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * aggregate-address <IP address> <IP mask>
 *			[as-set]
 *			[route-map <string>]
 *			[summary-only | suppress-map <name>]
 *
 * aggregate-address <IP address> <IP mask>
 *			[as-set]
 *			[attribute-map <string>]
 *			[advertise-map <string>]
 *			[summary-only | suppress-map <string>]
 *
 * no aggregate-address <IP address> <IP mask>
 *
 * (*OBJ(paddr,1)) = <IP address>
 * (*OBJ(paddr,2)) = <IP mask>
 * OBJ(int,1)      = as-set
 * OBJ(int,2)      = summary-only
 * OBJ(string,1)   = route-map, attribute-map
 * OBJ(string,2)   = suppress-map
 * OBJ(string,3)   = advertise-map
 *
 * OBJ(int,3)      = indicates old style command
 * OBJ(int,4)      = indicates new style command
 */

pdecl(router_agg_opts);

EOLNS (router_agg_addr_eol, bgp_aggregate_command);

STRING(router_agg_addr_supmap_string, router_agg_opts, no_alt, 
	OBJ(string,2), "Route map for suppression");
KEYWORD(router_agg_addr_supmap_kw, router_agg_addr_supmap_string,
	router_agg_addr_eol, "suppress-map",
	"Conditionally filter more specific routes from updates",
	PRIV_CONF);

/* suppress map OR summary only */
KEYWORD_ID(router_agg_addr_sumonly_kw, router_agg_opts,
	   router_agg_addr_supmap_kw,
	   OBJ(int, 2), ADDRSUM_AUTOFLTR,
	   "summary-only", "Filter more specific routes from updates",
	   PRIV_CONF);

/* set integer4 to indicate new style command. */
SET	(router_agg_addr_advmap_set, router_agg_opts,
	 OBJ(int, 4), TRUE);
STRING(router_agg_addr_advmap_string, router_agg_addr_advmap_set, no_alt,
       OBJ(string,3), "Route map to control attribute advertisement");
KEYWORD(router_agg_addr_advmap_kw, router_agg_addr_advmap_string, NONE, 
	"advertise-map", "Set condition to advertise attribute", PRIV_CONF);
TESTVAR	(router_agg_addr_advmap_check, router_agg_addr_advmap_kw,
	 NONE, NONE, NONE, router_agg_addr_sumonly_kw,
	 OBJ(int,3), FALSE);

SET	(router_agg_addr_attrmap_set, router_agg_opts,
	 OBJ(int, 4), TRUE);
STRING(router_agg_addr_attrmap_string, router_agg_addr_attrmap_set, no_alt,
       OBJ(string,1), "Route map for parameter control");
KEYWORD(router_agg_addr_attrmap_kw, router_agg_addr_attrmap_string,
	NONE, "attribute-map",
	"Set attributes of aggregate", PRIV_CONF);
TESTVAR	(router_agg_addr_attrmap_check, router_agg_addr_attrmap_kw,
	 NONE, NONE, NONE, router_agg_addr_advmap_check,
	 OBJ(int,3), FALSE);

/* set integer3 to indicate old style command. */
SET	(router_agg_addr_routemap_set, router_agg_opts,
	 OBJ(int, 3), TRUE);
STRING(router_agg_addr_routemap_string, router_agg_addr_routemap_set, no_alt, 
	OBJ(string,1), "Route map for parameter control");
KEYWORD(router_agg_addr_routemap_kw, router_agg_addr_routemap_string,
	NONE, "route-map",
	"Set parameters of aggregate", PRIV_CONF);
TESTVAR	(router_agg_addr_routemap_check, router_agg_addr_routemap_kw,
	 NONE, NONE, NONE, router_agg_addr_attrmap_check,
	 OBJ(int,4), FALSE);

KEYWORD_ID(router_agg_addr_asset_kw, router_agg_opts,
	   router_agg_addr_routemap_check,
	   OBJ(int,1), ADDRSUM_AS_SET,
	   "as-set", "Generate AS set path information", PRIV_CONF);

NOP	(router_agg_opts, router_agg_addr_asset_kw, NONE);

IPADDR (router_agg_addr_mask, router_agg_opts, no_alt,
	OBJ(paddr,2), "Aggregate mask");
IPADDR (router_agg_addr_ipaddr, router_agg_addr_mask, no_alt,
	OBJ(paddr,1), "Aggregate address");
NVGENNS (router_agg_addr_nvgen, router_agg_addr_ipaddr, 
	bgp_aggregate_command);

KEYWORD (router_agg_addr_kw, router_agg_addr_nvgen, NONE, 
	 "aggregate-address", "Configure BGP aggregate entries",
	 PRIV_CONF);

ASSERT (router_agg_addr, router_agg_addr_kw, ALTERNATE,
	(((pdbtype *) csb->protocol)->family == PDB_IP &&
	 ((pdbtype *) csb->protocol)->proctype == PROC_BGP));

#undef	ALTERNATE
#define	ALTERNATE	router_agg_addr
