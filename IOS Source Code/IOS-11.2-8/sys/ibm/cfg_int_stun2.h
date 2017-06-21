/* $Id: cfg_int_stun2.h,v 3.2.60.1 1996/05/21 09:48:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/cfg_int_stun2.h,v $
 *------------------------------------------------------------------
 * cfg_int_stun2.h -- split the stun interface commands
 *
 * June 30, 1993 - Leo Pereira
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_stun2.h,v $
 * Revision 3.2.60.1  1996/05/21  09:48:46  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  09:21:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:07:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] stun proxy-poll address <address> modulus <modulus>
 *	{ primary | secondary }
 * [no] stun proxy-poll address <address> discovery
 *
 * Which can be re-written as:
 *
 * [no] stun proxy-poll address <address>
 *	{ discovery | modulus <modulus> { primary | secondary } }
 *
 * OBJ(int,1) = type of proxy
 *	discovery = STUNIF_PROXY_DISCOVERY
 *	primary =  STUNIF_PROXY_PRIMARY
 *	secondary = STUNIF_PROXY_SECONDARY
 *
 * OBJ(int,2) = modulus (only for "primary" or "secondary" above)
 * (*OBJ(paddr,1)) = STUN address
 */
EOLS	(ci_stun_pxyeol, stunif_command, STUNIF_PROXY);
/* { primary | secondary } */
KEYWORD_ID(ci_stun_pxysecondary, ci_stun_pxyeol, no_alt,
	   OBJ(int,1), STUNIF_PROXY_SECONDARY,
	   "secondary", "The SDLC device is operating as a secondary node", PRIV_CONF);
KEYWORD_ID(ci_stun_pxyprimary, ci_stun_pxyeol, ci_stun_pxysecondary,
	   OBJ(int,1), STUNIF_PROXY_PRIMARY,
	   "primary", "The SDLC device is operating as a primary node", PRIV_CONF);

/* modulus <modulus> */
KEYWORD_ID(ci_stun_pxymod128, ci_stun_pxyprimary, no_alt,
	   OBJ(int,2), 128,
	   "128", "SDLC host modulus", PRIV_CONF);
KEYWORD_ID(ci_stun_pxymod8, ci_stun_pxyprimary, ci_stun_pxymod128,
	   OBJ(int,2), 8,
	   "8", "SDLC host modulus", PRIV_CONF);
KEYWORD	(ci_stun_pxymodulus, ci_stun_pxymod8, no_alt,
	 "modulus", "Link modulus of the SDLC host", PRIV_CONF);

/* discovery */
KEYWORD_ID(ci_stun_pxydiscovery, ci_stun_pxyeol, ci_stun_pxymodulus,
	   OBJ(int,1), STUNIF_PROXY_DISCOVERY,
	   "discovery", "Negotiate SDLC connections", PRIV_CONF);

NOPREFIX(ci_stun_proxy_noprefix, ci_stun_pxydiscovery, ci_stun_pxyeol );
/* address <address> */
STUNADDR (ci_stun_pxyaddrval, ci_stun_proxy_noprefix, no_alt,
	  OBJ(paddr,1), "STUN address");
KEYWORD	(ci_stun_pxyaddr, ci_stun_pxyaddrval, no_alt,
	 "address", "Set SDLC address", PRIV_CONF);

NVGENS	(ci_stun_proxy_nvgen, ci_stun_pxyaddr, stunif_command, STUNIF_PROXY);
KEYWORD	(ci_stun_proxy, ci_stun_proxy_nvgen, no_alt,
	 "proxy-poll", "Proxy polling of devices", PRIV_CONF);


/************************************************************************
 * [no] stun route { all | address <address> }
 *	{ interface serial <number> [direct] |
 *	  interface serial <number> dlci <number> [<lsap> local-ack [priority <number>]] |
 *	  tcp <ipaddr> [ local-ack | priority | input-address <stunaddr> }
 *
 * OBJ(int,1) = STUN_RTALL if forwarding all addresses
 *         = STUN_RTADDR if forwarding only specific address
 *
 * OBJ(int,2) = STUN_INTF if configuring a STUN serial interface transport
 *         = STUN_INTFDIR if using a direct serial interface
 *         = STUN_TCP if using TCP transport
 *
 * if_name = interface name & number
 *
 * (*OBJ(paddr,1)) = STUN address (if not 'all')
 * (*OBJ(paddr,2)) = IP address (if tcp)
 *
 */

EOLS	(ci_stun_rteol, stunif_command, STUNIF_ROUTE);

/* tcp options */
pdecl( ci_stun_tcpopts );
NUMBER  (ci_stun_tcpopts_qmax_val, ci_stun_tcpopts, no_alt,
         OBJ(int,6), 10, 500,
         "Maximum TCP driver queue length (length is from 10 to 500)");
KEYWORD (ci_stun_tcpopts_qmax, ci_stun_tcpopts_qmax_val, no_alt,
         "tcp-queue-max", "Set the max depth of the TCP driver queue", PRIV_CONF);
STUNADDR(ci_stun_tcpopts_inaddr_addr, ci_stun_tcpopts, no_alt,
	 OBJ(paddr,3), "STUN address" );
KEYWORD_ID(ci_stun_tcpopts_inaddr, ci_stun_tcpopts_inaddr_addr, 
           ci_stun_tcpopts_qmax,
	   OBJ(int,5), TRUE,
	   "input-address", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(ci_stun_tcpopts_priority, ci_stun_tcpopts, ci_stun_tcpopts_inaddr,
	   OBJ(int,4), TRUE,
	   "priority", "Enable priority queueing for SDLC frames", PRIV_CONF );
KEYWORD_ID(ci_stun_tcpopts_lack, ci_stun_tcpopts, ci_stun_tcpopts_priority,
	   OBJ(int,3), TRUE,
	   "local-ack", "SDLC session is locally terminated", PRIV_CONF );
NOP	(ci_stun_tcpopts, ci_stun_tcpopts_lack, ci_stun_rteol );

/* tcp <ipaddr> */
IPADDR	(ci_stun_rttcpaddr, ci_stun_tcpopts, no_alt,
	 OBJ(paddr,2), "Remote STUN peer's IP address");
KEYWORD_ID(ci_stun_rttcp, ci_stun_rttcpaddr, no_alt,
	   OBJ(int,2), STUN_TCP,
	   "tcp", "Use TCP transport to propagate frames", PRIV_CONF);

/* interface serial <number>
 * (The existing (9.0) parser allows "interface serial0"
 * because it uses the 'get_interface()' function.)
 */
/* Frame Relay dlci */
NUMBER  (ci_stun_cls_pri_num, ci_stun_rteol, no_alt,
         OBJ(int,9), 1, 10, "Priority number ");
KEYWORD_ID(ci_stun_cls_priority, ci_stun_cls_pri_num, ci_stun_rteol,
           OBJ(int,8), TRUE, "priority", "Enable priority ", PRIV_CONF );
KEYWORD_ID(ci_stun_rtlocack, ci_stun_cls_priority, no_alt,
           OBJ(int,7), TRUE, "local-ack", "SDLC session is locally terminated", PRIV_CONF);
HEXDIGIT  (ci_stun_rtlsap, ci_stun_rtlocack, ci_stun_rteol,
           OBJ(int,6), 0, 0xFF, "Local SAP number");
NUMBER(ci_stun_rtdlci, ci_stun_rtlsap, no_alt,
	   OBJ(int, 5), 16, 1007, "Frame-Relay DLCI number");
KEYWORD_ID(ci_stun_rtfr, ci_stun_rtdlci, ci_stun_rteol,
	   OBJ(int,4), TRUE,
	   "dlci", "Interface is a direct Frame Relay link", PRIV_CONF);
/* [direct] */
KEYWORD_ID(ci_stun_rtifdirect, ci_stun_rteol, ci_stun_rtfr,
	   OBJ(int,3), TRUE,
	   "direct", "Interface is a direct STUN link", PRIV_CONF);
INTERFACE (ci_stun_rtifname, ci_stun_rtifdirect, no_alt, OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_SUBIF);
KEYWORD_ID(ci_stun_rtif, ci_stun_rtifname, ci_stun_rttcp,
	   OBJ(int,2), STUN_INTF,
	   common_str_interface, "Use the Serial Transport to propagate frames", PRIV_CONF);

/* address <address> */
STUNADDR (ci_stun_rtaddrval, ci_stun_rtif, no_alt,
	  OBJ(paddr,1), "STUN address");
KEYWORD_ID(ci_stun_rtaddr, ci_stun_rtaddrval, no_alt,
	   OBJ(int,1), STUN_RTADDR,
	   "address", "Set STUN address", PRIV_CONF);

/* all */
KEYWORD_ID(ci_stun_rtall, ci_stun_rtif, ci_stun_rtaddr,
	   OBJ(int,1), STUN_RTALL,
	   "all", "Forward all frames", PRIV_CONF);

NVGENS	(ci_stun_route_nvgen, ci_stun_rtall, stunif_command, STUNIF_ROUTE);
KEYWORD	(ci_stun_route, ci_stun_route_nvgen, ci_stun_proxy,
	 "route", "Set forwarding of STUN frames", PRIV_CONF);

KEYWORD	(ci_stun1, ci_stun_route, NONE, "stun",
	 "STUN interface subcommands", PRIV_CONF|PRIV_DUPLICATE);

TEST_IDBSTATUS(ci_stun1_test, ci_stun1, NONE, ALTERNATE, IDB_SERIAL);

#undef  ALTERNATE
#define ALTERNATE ci_stun1_test
