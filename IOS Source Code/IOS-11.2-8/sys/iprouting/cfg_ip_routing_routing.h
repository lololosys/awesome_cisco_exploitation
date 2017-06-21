/* $Id: cfg_ip_routing_routing.h,v 3.3.46.3 1996/08/12 16:03:33 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_ip_routing_routing.h,v $
 *------------------------------------------------------------------
 * IP routing routing commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_ip_routing_routing.h,v $
 * Revision 3.3.46.3  1996/08/12  16:03:33  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.46.2  1996/06/17  23:33:31  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.3.46.1  1996/04/15  21:17:41  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1995/12/10  00:42:36  pst
 * CSCdi38750:  Allow static routes with both next hop and interface
 *              also add "permanent" keyword for non-flapping static routes
 *
 * Revision 3.2  1995/11/17  17:31:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/23  18:33:19  pst
 * CSCdi42608:  allow IP cache ager to be shut down, make the
 *              "ip cache-ager" and "ip cache-invalidate-delay"
 *              commands internal only.
 *
 * Revision 2.3  1995/07/13  21:39:05  ahh
 * CSCdi36962:  UDP parsing of tacacs-ds port name broken
 * Restore "tacacs-ds" as a keyword, but hide it and map it to TACACS_PORT
 * for backwards compatibility.
 *
 * Revision 2.2  1995/07/11  21:52:08  raj
 * CSCdi36510:  parser doesnt accept ip forward-protocol udp xdmcp
 * (Actually the parser didn't accept *any* symbolic names in the position
 * of the UDP port number; only numbers.  It now uses the UDP_PORT macro
 * instead.
 *
 * Revision 2.1  1995/07/01  12:15:42  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:08:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************
 *    ip route <network> [<mask>] { <address> | <interface> } [<distance>]
 *    no ip route <network> [<mask>] [{ <address> | <interface> }] [<distance>]
 *
 * OBJ(idb,1) = <interface>
 * (*OBJ(paddr,1)) = <network>
 * (*OBJ(paddr,2)) = [<mask>] if it exists, else <address>
 * (*OBJ(paddr,3)) = <address> if <mask> was given
 * OBJ(int,1) = <distance>
 * OBJ(int,2) = TRUE = <distance> was parsed
 *		FALSE = no <distance>
 * OBJ(int,3) = <tag>
 * OBJ(int,4) = permanent
 *
 * If <mask>'s high octet is 255 (CLASS_A_MASK), then it isn't valid 
 * as <address> and we should expect <address> next.
 */
pdecl	(cr_ip_route_loop);
EOLS	(cr_ip_route_eol, ip_route_command, IP_ROUTE);

KEYWORD_ORTEST(cr_ip_route_perm, cr_ip_route_loop, cr_ip_route_eol,
	       OBJ(int,4), TRUE, "permanent", "permanent route", PRIV_CONF);

NUMBER 	(cr_ip_route_tag, cr_ip_route_loop, NONE, OBJ(int,3),
	 1, MAXULONG, "Tag value");
KEYWORD	(cr_ip_route_tag_kw, cr_ip_route_tag, NONE,
	 "tag", "Set tag for this route", PRIV_USER);
EQUAL	(cr_ip_route_tag_assert, cr_ip_route_tag_kw, cr_ip_route_perm,
 	 OBJ(int,3), 0);

/* <distance> */
SET	(cr_ip_route_distance_set, cr_ip_route_loop, OBJ(int,2), TRUE);
NUMBER	(cr_ip_route_distance, cr_ip_route_distance_set, NONE,
	 OBJ(int,1), CONNECTED_DISTANCE+1, UNKNOWN_DISTANCE,
	 "Distance metric for this route");
NEQ	(cr_ip_route_dist_assert, cr_ip_route_distance, cr_ip_route_tag_assert,
	 OBJ(int,2), TRUE);
NOP	(cr_ip_route_loop, cr_ip_route_dist_assert, NONE);

/*
 * We had better have specified either a next hop or an interface if we're
 * adding a route right now.
 */
IFELSE	(cr_ip_route_test, cr_ip_route_loop, no_alt,
	 (ISOBJ(idb,1) || (GETOBJ(paddr,3)->type == ADDR_IP) || !csb->sense));

/* <nh-address> */
IPADDR	(cr_ip_route_nh_address, cr_ip_route_test, cr_ip_route_test,
	 OBJ(paddr,3), "Forwarding router's address");

/* <interface> */
INTERFACE (cr_ip_route_interface, cr_ip_route_nh_address,
	   cr_ip_route_nh_address, OBJ(idb,1), IFTYPE_ANYSUB);

/* <mask> */
IPADDR	(cr_ip_route_get_mask, cr_ip_route_interface, no_alt,
	 OBJ(paddr,2), "Destination prefix mask");

/* <prefix> */
IPADDR	(cr_ip_route_get_prefix, cr_ip_route_get_mask, no_alt,
	 OBJ(paddr,1), "Destination prefix");

NVGENS	(cr_ip_route_nvgen, cr_ip_route_get_prefix,
	 ip_route_command, IP_ROUTE);
KEYWORD	(cr_ip_route_kwd, cr_ip_route_nvgen, NONE,
	 "route", "Establish static routes", PRIV_CONF);
TEST_BOOLEAN(cr_ip_route, cr_ip_route_kwd, ALTERNATE, NONE,
	     router_enable);

/******************************************************************************
 *	ip forward-protocol  { sdns | nd | turbo-flood | udp [<port>] |
 *			       spanning-tree [any-local-broadcast] }
 *	no ip forward-protocol  [{ sdns | nd | turbo-flood | udp [<port>] |
 *			       spanning-tree [any-local-broadcast] }]
 *
 * OBJ(int,1) = PROTOCOL_ND, PROTOCOL_SDNS, PROTOCOL_SPANNING, PROTOCOL_UDP
 * In the PROTOCOL_UDP case:
 * OBJ(int,2) = TRUE if a UDP port number was given
 * OBJ(int,3) = UDP port number
 *
 * The UDP port number is optional for the 'no' version of the command.
 * OBJ(int,2) == FALSE in this case.
 *
 * In the PROTOCOL_SPANNING case:
 * OBJ(int,2) = TRUE if any broadcast permitted.
 */

EOLNS	(cip_forward_eol, ip_forwardproto_command);

/* udp [<port>] */
SET	(cip_forward_port_set, cip_forward_eol, OBJ(int,2), TRUE);
/* Note "tacacs-ds" -> 49 is wrong, but needed for backwards compatibility */
KEYWORD_ID_MM (cip_forward_port_tacacs, cip_forward_port_set, cip_forward_eol,
	       OBJ(int, 3), TACACS_PORT,
	       "tacacs-ds", NULL, PRIV_CONF|PRIV_NOHELP, 7);
UDP_PORT (cip_forward_port, cip_forward_port_set, cip_forward_port_tacacs,
	  OBJ(int,3));
EQUAL   (cip_forward_udp_default, cip_forward_eol, cip_forward_port,
	 set_to_default, TRUE);
NVGENNS	(cip_forward_udp_nvgen, cip_forward_udp_default,
	 ip_forwardproto_command);
KEYWORD_ID (cip_forward_udp, cip_forward_udp_nvgen, no_alt,
	  OBJ(int,1), PROTOCOL_UDP,
	 "udp", "Packets to a specific UDP port", PRIV_CONF);
/*
 * NVGEN for "nd" and "spanning-tree" and "turbo-flood" and
 * "any-local-broadcast"
 */
NVGENNS	(cip_forward_nvgen, cip_forward_eol, ip_forwardproto_command);

/* turbo-flood */
KEYWORD_ID(cip_forward_flood, cip_forward_nvgen, cip_forward_udp,
	   OBJ(int,1), PROTOCOL_FLOOD,
	   "turbo-flood", "Fast flooding of UDP broadcasts", PRIV_CONF);

/* any-broadcast */
KEYWORD_ID(cip_forward_any, cip_forward_eol, cip_forward_eol, OBJ(int,2), TRUE,
	   "any-local-broadcast",
	   "Accept any local broadcast when flooding", PRIV_CONF);

NVGENNS    (cip_forward_stree_nvgen, cip_forward_any, ip_forwardproto_command);

/* spanning-tree */
KEYWORD_ID(cip_forward_stree, cip_forward_stree_nvgen, cip_forward_flood,
	   OBJ(int,1), PROTOCOL_SPANNING,
	   "spanning-tree",
	   "Use transparent bridging to flood UDP broadcasts",
	   PRIV_CONF);

/* nd */
KEYWORD_ID(cip_forward_nd, cip_forward_nvgen, cip_forward_stree,
	   OBJ(int,1), PROTOCOL_ND,
	   "nd", "Sun's Network Disk protocol", PRIV_CONF);

/* sdns */
KEYWORD_ID (cip_forward_sdns, cip_forward_nvgen, cip_forward_nd,
	 OBJ(int,1), PROTOCOL_SDNS,
	 "sdns", "Network Security Protocol", PRIV_CONF);

KEYWORD	(cip_forward_protocol, cip_forward_sdns, cr_ip_route,
	 "forward-protocol",
	 "Controls forwarding of physical and directed IP broadcasts",
	 PRIV_CONF);

/******************************************************************************
 *	ip default-network <network_address>
 *	no ip default-network <network_address>
 *
 * (*OBJ(paddr,1)) = address
 */
EOLS	(cr_ip_default_net_eol, ip_defaultnetwork_command, IP_DEFAULTNETWORK);
IPADDR	(cr_ip_default_network_get_network, cr_ip_default_net_eol, no_alt, 
	 OBJ(paddr,1), "IP address of default network");
NVGENS	(cr_ip_default_net_nvgen, cr_ip_default_network_get_network, 
	 ip_defaultnetwork_command, IP_DEFAULTNETWORK);
KEYWORD	(cr_ip_default_network, cr_ip_default_net_nvgen, cip_forward_protocol,
	 "default-network",
	 "Flags networks as candidates for default routes", PRIV_CONF);

/******************************************************************************
 *	ip slow-converge
 *	no ip slow-converge
 */

EOLS	(cr_ip_slow_converge_eol, iprouting_command, IPR_SLOW_CONVERGE);
KEYWORD	(cr_ip_slow_converge, cr_ip_slow_converge_eol, cr_ip_default_network,
	 "slow-converge", "Perform slower convergence on link failures",
	 PRIV_CONF|PRIV_HIDDEN); 

/******************************************************************************
 *	ip classless
 *	no ip classless
 */

EOLS	(cr_ip_classless_eol, iprouting_command, IPR_CLASSLESS);
KEYWORD	(cr_ip_classless, cr_ip_classless_eol, cr_ip_slow_converge,
	 "classless", "Follow classless routing forwarding rules", PRIV_CONF); 

#undef	ALTERNATE
#define	ALTERNATE	cr_ip_classless
