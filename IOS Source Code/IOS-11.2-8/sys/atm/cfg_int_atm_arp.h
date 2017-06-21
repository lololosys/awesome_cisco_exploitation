/* $Id: cfg_int_atm_arp.h,v 3.1.4.1 1996/04/25 23:02:34 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/atm/cfg_int_atm_arp.h,v $
 *------------------------------------------------------------------
 * atm/cfg_int_atm_arp.h: atm arp configuration commands parser macros
 *
 * March, 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_atm_arp.h,v $
 * Revision 3.1.4.1  1996/04/25  23:02:34  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  05:53:41  rzagst
 * Place holder for atm modularity files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * ATM arp-server { self [ timeout <time> ] |  nsap <address> }
 *
 * OBJ(int,1) = TRUE if we are defining an ARP server, FALSE if we
 *              are passing the address of the server for our client
 * OBJ(int,2) = timeout time, or ATMARP_SERVERAGE by default
 * OBJ(hwaddr,1) = address of remote ARP server
 */
EOLS	(ci_atm_arpserv_eol, atm_arp_interface_commands, ATM_CMD_ARPSERV);
SET	(ci_atm_arpserv_timeoutdefault, ci_atm_arpserv_eol,
	 OBJ(int,2), ATMARP_SERVERAGE / ONEMIN);
NUMBER	(ci_atm_arpserv_timeouttime, ci_atm_arpserv_eol, no_alt,
	 OBJ(int,2), ATMARP_SERVERAGE / ONEMIN, MAXINT / ONEMIN,
	 "Minutes our ARP Server keeps an entry as valid");
KEYWORD	(ci_atm_arpserv_timeout, ci_atm_arpserv_timeouttime,
	 ci_atm_arpserv_timeoutdefault, "timeout",
	 "Minutes our ARP Server keeps an entry as valid",
	 PRIV_CONF | PRIV_SUBIF);
SET	(ci_atm_arpserv_set, ci_atm_arpserv_timeout, OBJ(int,1), TRUE);
/*
 *  Note that the alternate jump for the ci_atm_arpserv_server node
 *  really ought to be no_alt, but we will accept the old syntax for
 *  backwards compatibility for a couple of releases.
 */
KEYWORD	(ci_atm_arpserv_server, ci_atm_arpserv_set, ci_atm_arpserv_set,
	 "self", "Declare this (sub)interface to be an ARP Server",
	 PRIV_CONF | PRIV_SUBIF);

ATMNSAPADDR (ci_atm_arpserv_nsapaddr, ci_atm_arpserv_eol, no_alt,
	     OBJ(hwaddr,1), "20-octet NSAP address");

KEYWORD	(ci_atm_arpserv_nsap, ci_atm_arpserv_nsapaddr, ci_atm_arpserv_server,
	 "nsap", "Specify NSAP of ARP Server", PRIV_CONF | PRIV_SUBIF);;
NVGENS	(ci_atm_arpserv_nvgen, ci_atm_arpserv_nsap,
	atm_arp_interface_commands, ATM_CMD_ARPSERV);
KEYWORD	(ci_atm_arpserv, ci_atm_arpserv_nvgen, ALTERNATE,
	"arp-server", "Configure IP ARP Server", PRIV_CONF | PRIV_SUBIF);

/******************************************************************
 * ATM classic-ip-extensions { none | bfi }
 *    -- default is no extension
 *
 * OBJ(int,1) = Extension type (RFC1577_EXT_NONE, RFC1577_EXT_BFI)
 */
EOLS	(ci_atm_1577ext_eols, atm_arp_interface_commands, ATM_CMD_1577EXT);
SET	(ci_atm_1577ext_bfi2, ci_atm_1577ext_eols, OBJ(int,1),
	 RFC1577_EXT_BFI);
KEYWORD	(ci_atm_1577ext_bfi, ci_atm_1577ext_bfi2, no_alt,
	 "BFI", "Simple Redundant ARP Servers",	 PRIV_CONF | PRIV_SUBIF);
KEYWORD	(ci_atm_1577ext_none, ci_atm_1577ext_eols, ci_atm_1577ext_bfi,
	 "none", "Standard RFC 1577 behavior (no redundant ARP Servers)",
	 PRIV_CONF | PRIV_SUBIF);
SET	(ci_atm_1577ext_type, ci_atm_1577ext_none, OBJ(int,1),
	 RFC1577_EXT_NONE);
NOPREFIX(ci_atm_1577ext_no, ci_atm_1577ext_type, ci_atm_1577ext_eols);
NVGENS	(ci_atm_1577ext_nv, ci_atm_1577ext_no, atm_arp_interface_commands,
	 ATM_CMD_1577EXT);
KEYWORD_MM (ci_atm_1577ext, ci_atm_1577ext_nv, ci_atm_arpserv,
	 "classic-ip-extensions", "Specify the type of Classic IP extensions",
	 PRIV_CONF | PRIV_SUBIF, 6);


#undef	ALTERNATE
#define	ALTERNATE	ci_atm_1577ext
