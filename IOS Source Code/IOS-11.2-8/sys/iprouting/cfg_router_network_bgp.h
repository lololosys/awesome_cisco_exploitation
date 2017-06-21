/* $Id: cfg_router_network_bgp.h,v 3.2.12.1 1996/08/12 16:03:47 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_network_bgp.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ N E T W O R K _ B G P . H
 *
 * Month Year, Your Name
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_network_bgp.h,v $
 * Revision 3.2.12.1  1996/08/12  16:03:47  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1996/02/13  03:01:58  asastry
 * CSCdi48790:  ISIS does not allow multiple NETs. Make sure that PDB's
 * family is PDB_IP before generating the BGP 'network' command (the
 * 'network' command causes the ISIS 'net' command to become ambiguous).
 *
 * Revision 3.1  1995/11/09  12:05:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  10:48:43  tli
 * CSCdi40960:  ISP jumbo patch 2
 * Missed one file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * [no] network <address> [mask] [[weight xxxx] | [route-map <string>]] 
 * 		                 [backdoor]
 * OBJ(int,1) = weight modifer (0 if none, 0 = default)
 * OBJ(int,2) = TRUE if backdoor
 * OBJ(string,1) = route-map name
 *
 */

EOLNS	(bgp_network_eol, bgp_network_command);
KEYWORD_ID (bgp_network_backdoor, bgp_network_eol, bgp_network_eol,
	    OBJ(int,2), TRUE,
	    "backdoor", "Specify a BGP backdoor route", PRIV_CONF);

/* [weight] [route-map <string>] */

NUMBER (bgp_network_weightval, bgp_network_backdoor, no_alt,
	 OBJ(int,1), BGP_MIN_WEIGHT, BGP_MAX_WEIGHT, "absolute weight");
STRING	(bgp_network_routemap_s, bgp_network_backdoor, no_alt,
	 OBJ(string, 1), "Name of the route map");
KEYWORD	(bgp_network_routemap, bgp_network_routemap_s, bgp_network_backdoor,
	 "route-map", "Route-map to modify the attributes", PRIV_CONF);
KEYWORD (bgp_network_weight, bgp_network_weightval, bgp_network_routemap,
	 "weight", "Set BGP weight for network", PRIV_CONF);

/* [mask <mask>] */
IPADDR	(bgp_network_mask_addr, bgp_network_weight, no_alt,
	 OBJ(paddr,2), "Network mask");

KEYWORD (bgp_network_mask, bgp_network_mask_addr, bgp_network_weight,
	 "mask", "Network mask", PRIV_CONF);

IPADDR	(bgp_network_net, bgp_network_mask, no_alt,
	 OBJ(paddr,1), "Network number");

NVGENNS	(bgp_network_nvgen, bgp_network_net, bgp_network_command);
KEYWORD	(bgp_network_kwd, bgp_network_nvgen, NONE,
	 "network", "Specify a network to announce via BGP", PRIV_CONF);

ASSERT	(bgp_network, bgp_network_kwd, ALTERNATE,
	 (((pdbtype *) csb->protocol)->family == PDB_IP) &&
	  ((pdbtype *) csb->protocol)->proctype & PROC_BGP);

#undef	ALTERNATE
#define ALTERNATE	bgp_network


