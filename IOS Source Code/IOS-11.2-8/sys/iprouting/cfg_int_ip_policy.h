/* $Id: cfg_int_ip_policy.h,v 3.3.54.1 1996/08/12 16:03:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_int_ip_policy.h,v $
 *------------------------------------------------------------------
 * cfg_int_ip_routing.h -- Interface subcommands for IP policy routing
 *
 * Feb 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_policy.h,v $
 * Revision 3.3.54.1  1996/08/12  16:03:27  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/28  09:17:52  tli
 * CSCdi43739:  Cannot enable IP Policy route-map on subinterfaces.
 * Set the right bits so that the configuration command is valid for
 * subinterfaces.
 *
 * Revision 3.2  1995/11/17  17:31:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:15:57  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  21:05:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	ip policy route-map <map name>
 *
 * OBJ(string, 1) = route map name
 */

EOLNS	(ci_ip_policy_eol, ip_policy_command);

STRING	(ci_ip_policy_route_map_string, ci_ip_policy_eol, no_alt,
	 OBJ(string, 1), "Route map name");

KEYWORD (ci_ip_policy_route_map, ci_ip_policy_route_map_string, no_alt,
	 "route-map", "Policy route map", PRIV_CONF|PRIV_SUBIF);

NVGENNS	(ci_ip_policy_nvgen, ci_ip_policy_route_map, ip_policy_command);

KEYWORD	(ci_ip_policy, ci_ip_policy_nvgen, ALTERNATE,
	 "policy", "Enable policy routing", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_policy
