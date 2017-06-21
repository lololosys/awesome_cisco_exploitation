/* $Id: cfg_x25_routing.h,v 3.4.28.1 1996/05/21 10:10:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/cfg_x25_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ X 2 5 _ R O U T I N G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_x25_routing.h,v $
 * Revision 3.4.28.1  1996/05/21  10:10:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/01/18  02:16:09  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.3  1995/11/21  08:27:32  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.2  1995/11/17  18:05:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************************************
 * x25 host <hostname> <X.121-addr> [cud <CUD-string>]
 *
 * OBJ(paddr,1) = <X.121-addr>
 * OBJ(string,1)  = <hostname>
 * OBJ(string,2)  = <CUD-string>
 */

EOLS	(x25_host_eol, x25_routing_commands, X25_HOST);

STRING	(x25_host_5, x25_host_eol, no_alt,
	 OBJ(string,2), "Call User Data");
KEYWORD (x25_host_4, x25_host_5, x25_host_eol,
	 "cud", "Specify optional Call User Data", PRIV_CONF);

X121ADDR(x25_host_3, x25_host_4, no_alt,
	 OBJ(paddr,1), "X.121 address of host");
NOPREFIX(x25_host_2, x25_host_3, x25_host_eol);
STRING	(x25_host_1, x25_host_2, no_alt,
	 OBJ(string,1), "X.25 hostname");
NVGENS	(x25_host_0, x25_host_1, x25_routing_commands, X25_HOST);
KEYWORD	(x25_host, x25_host_0, no_alt,
	 "host", "Declare an X.25 hostname", PRIV_CONF);

/***************************************************************************
 *
 * x25 route [#<position>] <x121pattern> [substitute-source <src-pattern>]
 *	[substitute-dest <dest-pattern>] [cud <cud-data>]
 *	{ alias <ifname> | function { chargen | discard | echo | help } |
 *	  interface <interface> |
 *	  ip <ipaddr> [ <ip-alternates> ] [ <xot-active-options> ]
 *	  [ <xot-options> ]
 *
 * <ip-alternates> = [ <ipaddr2> [ ... [ <ipaddr6> ] ] ]
 * <xot-active-options> = [ xot-source <interface> ]
 * <xot-options> = [ xot-keepalive-period <seconds> ]
 *		   [ xot-keepalive-tries <count> ]
 *
 * no x25 route [#<position>] <x121pattern>
 *
 * OBJ(int,1) = positional parameter (-1 if none)
 * OBJ(int,2) = X25_ROUTE_ALIAS, X25_ROUTE_FUNC, 
 *	X25_ROUTE_IF, X25_ROUTE_IP
 * OBJ(int,3) = X25_FUNC_CHARGEN, X25_FUNC_DISCARD,
 *		X25_FUNC_ECHO, X25_FUNC_HELP
 * OBJ(int,4) = TRUE if substitute-source or substitute-dest parsed,
 *		otherwise FALSE
 * OBJ(int,5) = xot-keepalive-period
 * OBJ(int,6) = xot-keepalive-tries
 * OBJ(int,8) = number of ip addresses (1-6) 
 * OBJ(idb,1) = <interface> for common_str_interface or "xot-source"
 * (*OBJ(paddr,1)) = <ipaddr>
 * (*OBJ(paddr,2)) = <ipaddr> alternate
 * (*OBJ(paddr,3)) = <ipaddr> alternate
 * (*OBJ(paddr,4)) = <ipaddr> alternate
 * (*OBJ(paddr,5)) = <ipaddr> alternate
 * (*OBJ(paddr,6)) = <ipaddr> alternate
 * OBJ(string,1) = <x121pattern>
 * OBJ(string,2) = <src-pattern>
 * OBJ(string,3) = <dest-pattern>
 * OBJ(string,4) = <cud-data>
 */

EOLS	(x25_route_eol, x25_routing_commands, X25_ROUTE);

pdecl(x25_route_opts);

/*
 * XOT Active-side-only options
 */

/* xot-source <interface> */
INTERFACE (x25_route_src_name, x25_route_opts, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD	(x25_route_src, x25_route_src_name, no_alt,
	 "xot-source", "Specify source interface for XOT connections",
	 PRIV_CONF);

EQUAL	(x25_route_opts_active, x25_route_src, no_alt,
	 OBJ(int,2), X25_ROUTE_IPADDR);

/*
 * XOT Active- and Passive-side options
 */

/* xot-keepalive-tries <number> */
NUMBER	(x25_route_tcp_kt_num, x25_route_opts, no_alt,
	 OBJ(int,6), 3, ~0, "Keepalive tries");
KEYWORD	(x25_route_tcp_kt, x25_route_tcp_kt_num, x25_route_opts_active,
	 "xot-keepalive-tries", "Specify TCP keepalive tries",
	 PRIV_CONF);

/* xot-keepalive-period <number> */
NUMBER	(x25_route_tcp_kp_num, x25_route_opts, no_alt,
	 OBJ(int,5), 1, ~0, "Keepalive period in seconds");
KEYWORD	(x25_route_tcp_kp, x25_route_tcp_kp_num, x25_route_tcp_kt,
	 "xot-keepalive-period", "Specify TCP keepalive period",
	 PRIV_CONF);

NOP	(x25_route_opts, x25_route_tcp_kp, x25_route_eol);

/* ip <ipaddr> */
SET	(x25_route_ip_addr_7, x25_route_opts, OBJ(int,8), 6);

IPADDR	(x25_route_ip_addr_6a, x25_route_ip_addr_7, x25_route_opts,
	 OBJ(paddr,6), "Alternate Address of remote Cisco");
SET	(x25_route_ip_addr_6, x25_route_ip_addr_6a, OBJ(int,8), 5);

IPADDR	(x25_route_ip_addr_5a, x25_route_ip_addr_6, x25_route_opts,
	 OBJ(paddr,5), "Alternate Address of remote Cisco");
SET	(x25_route_ip_addr_5, x25_route_ip_addr_5a, OBJ(int,8), 4);

IPADDR	(x25_route_ip_addr_4a, x25_route_ip_addr_5, x25_route_opts,
	 OBJ(paddr,4), "Alternate Address of remote Cisco");
SET	(x25_route_ip_addr_4, x25_route_ip_addr_4a, OBJ(int,8), 3);

IPADDR	(x25_route_ip_addr_3a, x25_route_ip_addr_4, x25_route_opts,
	 OBJ(paddr,3), "Alternate Address of remote Cisco");
SET	(x25_route_ip_addr_3, x25_route_ip_addr_3a, OBJ(int,8), 2);

IPADDR	(x25_route_ip_addr_2a, x25_route_ip_addr_3, x25_route_opts,
	 OBJ(paddr,2), "Alternate Address of remote Cisco");
SET	(x25_route_ip_addr_2, x25_route_ip_addr_2a, OBJ(int,8), 1);

IPADDR	(x25_route_ip_addr, x25_route_ip_addr_2, no_alt,
	 OBJ(paddr,1), "Address of remote Cisco");
KEYWORD_ID(x25_route_ip, x25_route_ip_addr, NONE,
	   OBJ(int,2), X25_ROUTE_IPADDR,
	   "ip", "Route to a remote Cisco router", PRIV_CONF);

/* function { chargen | discard | echo | help } */
KEYWORD_ID(x25_route_func_help, x25_route_eol, no_alt,
	   OBJ(int,3), X25_FUNC_HELP,
	   "help", "Help function", PRIV_CONF);
KEYWORD_ID(x25_route_func_echo, x25_route_eol, x25_route_func_help,
	   OBJ(int,3), X25_FUNC_ECHO,
	   "echo", "Echo service", PRIV_CONF);
KEYWORD_ID(x25_route_func_disc, x25_route_eol, x25_route_func_echo,
	   OBJ(int,3), X25_FUNC_DISCARD,
	   "discard", "Discard service", PRIV_CONF);
KEYWORD_ID(x25_route_func_char, x25_route_eol, x25_route_func_disc,
	   OBJ(int,3), X25_FUNC_CHARGEN,
	   "chargen", "Character generator function", PRIV_CONF);
KEYWORD_ID(x25_route_func, x25_route_func_char, x25_route_ip,
	   OBJ(int,2), X25_ROUTE_FUNC,
	   "function", "Set X25 service functions", PRIV_CONF | PRIV_HIDDEN);

/* alias <ifname> */
INTERFACE(x25_route_alias_int, x25_route_eol, no_alt, OBJ(idb,1),
	  IFTYPE_HWIDB);
KEYWORD_ID(x25_route_alias, x25_route_alias_int, x25_route_func,
	   OBJ(int,2), X25_ROUTE_ALIAS,
	   "alias", "Treat the X.121 address as local", PRIV_CONF);
NEQ	(x25_route_neq, x25_route_alias, no_alt, OBJ(int,4), TRUE);

/* interface <interface> */
INTERFACE(x25_route_int_interface, x25_route_opts,
	  no_alt, OBJ(idb,1), IFTYPE_HWIDB    );
KEYWORD_ID(x25_route_int, x25_route_int_interface, x25_route_neq,
	   OBJ(int,2), X25_ROUTE_IF,
	   common_str_interface, "Route to a local interface", PRIV_CONF);

/* cud <pattern> */
STRING	(x25_route_cud_str, x25_route_int, no_alt,
	 OBJ(string,4), "CUD pattern to match");
KEYWORD	(x25_route_cud, x25_route_cud_str, x25_route_int,
	 "cud", "Called User Data pattern to match", PRIV_CONF);

/* substitute-dest <rewrite-pattern> */
STRING	(x25_route_subdst_str, x25_route_cud, no_alt,
	 OBJ(string,3), "Destination address rewrite pattern");
KEYWORD_ID(x25_route_subdst, x25_route_subdst_str, x25_route_cud,
	   OBJ(int,4), TRUE,
           "substitute-dest", "Specify destination rewrite pattern",
           PRIV_CONF);

/* substitute-source <rewrite-pattern> */
STRING	(x25_route_subsrc_str, x25_route_subdst, no_alt,
	 OBJ(string,2), "Source address rewrite pattern");
KEYWORD_ID(x25_route_subsrc, x25_route_subsrc_str, x25_route_subdst,
	   OBJ(int,4), TRUE,
           "substitute-source", "Specify source rewrite pattern",
	   PRIV_CONF);

/* <x121pattern> - Do a string check only if '#<number' didn't match.
 * This is so that during help search, the parser doesn't match 
 * both '#' and STRING, resulting an an ambiguous command.
 */
STRING	(x25_route_address, x25_route_subsrc, no_alt,
	 OBJ(string,1), "X.121 address pattern to match");
TESTVAR	(x25_route_test, no_alt, x25_route_address,
	 NONE, NONE, NONE, OBJ(int,10), X25_ROUTE_POSITION);

/* [<# position>] */
NUMBER	(x25_route_position_4, x25_route_address, no_alt,
	 OBJ(int,11), 0, 4096, "Insert after this numbered entry");
SET	(x25_route_position_3, x25_route_position_4,
	 OBJ(int,10), X25_ROUTE_POSITION);
CHARACTER(x25_route_position_2, x25_route_position_3, x25_route_test, '#');
SET	(x25_route_position_1, x25_route_position_2, OBJ(int,11), -1);
HELP	(x25_route_position, x25_route_position_1,
	 "  #<number>\tOptional positional parameter\n");

NVGENS	(x25_route_nvgen, x25_route_position,
	 x25_routing_commands, X25_ROUTE);
KEYWORD	(x25_route, x25_route_nvgen, x25_host,
	 "route", "Add an entry to the X.25 routing table", PRIV_CONF);

KEYWORD (cr_x25, x25_route, ALTERNATE,
	 "x25", "", PRIV_CONF | PRIV_DUPLICATE);

#undef	ALTERNATE
#define	ALTERNATE	cr_x25
