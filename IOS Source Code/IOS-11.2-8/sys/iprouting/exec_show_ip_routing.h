/* $Id: exec_show_ip_routing.h,v 3.3.54.2 1996/08/12 16:04:11 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_ip_routing.h,v $
 *------------------------------------------------------------------
 * IP routing show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_routing.h,v $
 * Revision 3.3.54.2  1996/08/12  16:04:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.54.1  1996/04/02  05:36:56  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.3  1995/11/28  03:08:51  widmer
 * CSCdi44514:  ipc commands break ambiguity detection
 *
 * Revision 3.2  1995/11/17  17:33:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:06:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/03  05:46:14  dino
 * CSCdi38154:  Provide show ip route <dns-name> support.
 *
 * Revision 2.1  1995/07/01  12:16:20  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.2  1995/06/24  00:34:42  bcole
 * CSCdi36387:  NHRP: Purge packet support
 *
 * Add purge message support.
 * Repair refresh of cache entries.
 *
 * Revision 2.1  1995/06/07  22:24:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ip route [{ <network> [<mask>] [longer-prefixes] | supernets-only | summary }]
 * 
 * (*OBJ(paddr,1)) = <network>
 * (*OBJ(paddr,2)) = <mask>
 */
EOLS	(show_ip_route_eol, display_routes, SHOW_IPR_DEFAULT);
EOLS	(show_ip_route_prefix_eol, display_routes, SHOW_IPR_PREFIX);
KEYWORD (show_ip_route_prefix, show_ip_route_prefix_eol, show_ip_route_eol,
	 "longer-prefixes",
	 "Show route matching the specified Network/Mask pair only", PRIV_USER);
IPADDR	(show_ip_route_mask, show_ip_route_prefix, show_ip_route_prefix,
	 OBJ(paddr,2), "Network mask");
IPADDR_NAME (show_ip_route_network, show_ip_route_mask, NONE,
	     OBJ(paddr,1), "Network to display information about or hostname");
TEST_MULTIPLE_FUNCS(show_ip_route_ambig, show_ip_route_network,
		    NONE, show_ip_route_eol);

EOLS	(show_ip_route_cidr_eol, display_routes, SHOW_IPR_CIDR);
KEYWORD (show_ip_route_cidr, show_ip_route_cidr_eol, show_ip_route_ambig,
	 "supernets-only", "Show supernet entries only", PRIV_USER);

EOLNS	(show_ip_route_summary_eol, display_route_summary);
KEYWORD (show_ip_route_summary, show_ip_route_summary_eol, show_ip_route_cidr,
	 "summary", "Summary of all routes", PRIV_USER);

ROUTINGPROTO(show_ip_route_proto, show_ip_route_eol, show_ip_route_summary);
KEYWORD (show_ip_route, show_ip_route_proto, ALTERNATE,
	 "route", "IP routing table", PRIV_USER);

/******************************************************************
 * show ip protocols [<summary>]
 * 
 */
EOLNS	(show_ip_protocols_eol, show_ipprotocol);
EOLNS	(show_ip_protocols_summary_eol, show_ipproto_summary);
KEYWORD	(show_ip_protocols_summary, show_ip_protocols_summary_eol,
	 show_ip_protocols_eol, "summary", "Short form answer", PRIV_USER);
KEYWORD (show_ip_protocols, show_ip_protocols_summary, show_ip_route,
	 "protocols", "IP routing protocol process parameters and statistics",
	 PRIV_USER);

/******************************************************************
 * show ip masks <address>
 */
EOLNS	(show_ip_masks_eol, show_ipmasks);
IPADDR	(show_ip_masks_addr, show_ip_masks_eol, show_ip_masks_eol,
	 OBJ(paddr,1), "Network number");
KEYWORD	(show_ip_masks_kwd, show_ip_masks_addr, show_ip_protocols,
	 "masks", "Masks associated with a network", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_masks_kwd
