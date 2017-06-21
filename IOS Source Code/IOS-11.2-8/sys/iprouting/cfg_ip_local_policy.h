/* $Id: cfg_ip_local_policy.h,v 3.1.62.1 1996/08/12 16:03:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_ip_local_policy.h,v $
 *------------------------------------------------------------------
 * cfg_ip_local_policy.h -- Configure local policy
 *
 * August 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_local_policy.h,v $
 * Revision 3.1.62.1  1996/08/12  16:03:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1995/11/09  12:04:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/19  22:22:01  tli
 * CSCdi36983:  Need policy routing for locally generated packets
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	ip local policy route-map <map name>
 *
 * OBJ(string, 1) = route map name
 */

EOLNS	(cfg_ip_local_policy_eol, ip_local_policy_command);

STRING	(cfg_ip_local_policy_route_map_string, cfg_ip_local_policy_eol, no_alt,
	 OBJ(string, 1), "Route map name");

KEYWORD (cfg_ip_local_policy_route_map,
	 cfg_ip_local_policy_route_map_string, no_alt, 
	 "route-map", "Policy route map", PRIV_CONF);

NVGENNS	(cfg_ip_local_policy_nvgen, cfg_ip_local_policy_route_map,
	 ip_local_policy_command);

KEYWORD	(cfg_ip_local_policy, cfg_ip_local_policy_nvgen, ALTERNATE,
	 "policy", "Enable policy routing", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_ip_local_policy
