/* $Id: cfg_int_ip_clns.h,v 3.2 1995/11/17 08:51:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/cfg_int_ip_clns.h,v $
 *------------------------------------------------------------------
 * IP CLNS interface commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_clns.h,v $
 * Revision 3.2  1995/11/17  08:51:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:06:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:05:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * ip router isis [<tag>]
 *
 * no ip router isis [<tag>]
 */
EOLS	(ci_ip_router_eols, ip_router_command, ISIS_IP_ROUTER_CMD);
STRING	(ci_ip_router_tag, ci_ip_router_eols, ci_ip_router_eols,
	 OBJ(string,1), "Routing process tag");
KEYWORD (ci_ip_router_isis, ci_ip_router_tag, no_alt, "isis",
	 "IS-IS Routing for IP", PRIV_CONF|PRIV_SUBIF);
NVGENS	(ci_ip_router_nvgen, ci_ip_router_isis, ip_router_command, 
	 ISIS_IP_ROUTER_CMD);
KEYWORD (ci_ip_router, ci_ip_router_nvgen, ALTERNATE,
	 "router", 
	 "IP router interface commands", PRIV_CONF|PRIV_SUBIF);


#undef	ALTERNATE
#define	ALTERNATE	ci_ip_router
