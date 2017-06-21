/* $Id: cfg_xns_routing.h,v 3.2.62.1 1996/05/30 23:48:04 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_xns_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ X N S . H
 *
 * XNS global configuration commands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_xns_routing.h,v $
 * Revision 3.2.62.1  1996/05/30  23:48:04  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:21:20  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:42:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(conf_xns_routing_extend_here, no_alt);

/*********************************************************************
 * xns ub-routing
 * no xns ub-routing
 *
 * This is for backwards compatibility. See xns/xnsinput.c 'case XNS_UBROUTE'.
 */

EOLS	(conf_xns_ubrouting_eol, xns_command, XNS_UBROUTE);
KEYWORD	(conf_xns_ubrouting, conf_xns_ubrouting_eol, conf_xns_routing_extend_here,
	 "ub-routing", "Enable Ungermann-Bass Net/One routing", 
	 PRIV_CONF | PRIV_NONVGEN);

/*********************************************************************
 * xns route <network> <net.host>
 * no xns route <network> [<net.host>]
 *
 */

EOLS	(conf_xns_route_eol, xns_command, XNS_ROUTE);
XNSADDR	(conf_xns_route_host, conf_xns_route_eol, no_alt,
	 OBJ(paddr,1), "Address of forwarding host");
NOPREFIX (conf_xns_noroute, conf_xns_route_host, conf_xns_route_eol);
NUMBER (conf_xns_route_net, conf_xns_noroute, no_alt,
	  OBJ(int,1), 0, -2L, "Destination network");
NVGENS	(conf_xns_route_nvgen, conf_xns_route_net, xns_command, XNS_ROUTE);
KEYWORD	(conf_xns_route, conf_xns_route_nvgen, conf_xns_ubrouting,
	 "route", "Set an XNS static routing table entry", PRIV_CONF);


/**********************************************************************/
KEYWORD	(configure_xns, conf_xns_route, ALTERNATE,
	 "xns", " ", PRIV_CONF | PRIV_DUPLICATE);

#undef  ALTERNATE
#define ALTERNATE configure_xns
