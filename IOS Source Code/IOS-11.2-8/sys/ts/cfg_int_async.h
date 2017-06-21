/* $Id: cfg_int_async.h,v 3.2 1995/11/17 18:53:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_int_async.h,v $
 *------------------------------------------------------------------
 * cfg_int_async.h - Async interface-specific configuration commands
 *
 * March 18, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_async.h,v $
 * Revision 3.2  1995/11/17  18:53:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/18  19:38:42  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/07/04  01:58:38  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:03:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * async mode interactive
 */
EOLS	(ci_async_mode_interactive_eol, async_config_command, ASYNC_MODE_INT);
KEYWORD	(ci_async_mode_interactive, ci_async_mode_interactive_eol, no_alt,
	 "interactive",
	 "Line may be switched between interactive use and async interface",
	 PRIV_CONF);

/************************************************************
 * async mode dedicated
 */
EOLS	(ci_async_mode_decicated_eol, async_config_command, ASYNC_MODE_DED);
KEYWORD	(ci_async_mode_dedicated, ci_async_mode_decicated_eol,
	 ci_async_mode_interactive,
	 "dedicated", "Line is dedicated as an async interface",
	 PRIV_CONF);

/*
 * [no] async mode
 */

EOLS (ci_async_mode_noeol, async_config_command, ASYNC_MODE_NONE);

NOPREFIX (ci_async_mode_notest,
	  ci_async_mode_dedicated,
	  ci_async_mode_noeol);

KEYWORD (ci_async_mode, ci_async_mode_notest, no_alt,
	 "mode",
	 "Specify line mode (interactive or dedicated interface use)",
	 PRIV_CONF);

/*
 * [no] async dynamic routing
 */

EOLS	(ci_async_dynamic_routing_eol, async_config_command,
	 ASYNC_DYNAMIC_ROUTING);
KEYWORD	(ci_async_dynamic_routing, ci_async_dynamic_routing_eol, no_alt,
	 "routing", "Allow user to run dynamic routing protocols",
	 PRIV_CONF);

/*
 * [no] async dynamic address
 */


EOLS (ci_async_dynamic_address_eol, async_config_command,
	 ASYNC_DYNAMIC_ADDRESS);
KEYWORD (ci_async_dynamic_address, ci_async_dynamic_address_eol,
	ci_async_dynamic_routing,
	"address",
	"Allow user to specify address dynamically",
	PRIV_CONF);

/*
 * [no] async dynamic
 */

KEYWORD (ci_async_dynamic, ci_async_dynamic_address, ci_async_mode,
	"dynamic",
	"Specify parameters which user may change",
	PRIV_CONF);


/*
 * [no] async default routing
 */

EOLS	(ci_async_default_routing_eol, async_config_command,
	 ASYNC_DEFAULT_ROUTING);
KEYWORD	(ci_async_default_routing, ci_async_default_routing_eol, no_alt,
	 "routing", "Enable routing on the line by default",
	 PRIV_CONF);

/*
 * [no] async default ip address
 */

EOLS (ci_async_default_ip_address_eol, peer_config_command, PEER_ADDR_CONFIG);

IPADDR (ci_async_default_ip_address_addr, ci_async_default_ip_address_eol,
	no_alt,
	OBJ(paddr, 1),
	"Default IP address for remote user of async interface");

NVGENS (ci_async_default_ip_address_nvtest,
	ci_async_default_ip_address_addr,
	async_config_command, ASYNC_DEFAULT_IP_ADDRESS);

NOPREFIX(ci_async_default_ip_address_notest,
	 ci_async_default_ip_address_nvtest,
	 ci_async_default_ip_address_eol);
	 

KEYWORD (ci_async_default_ip_address, ci_async_default_ip_address_notest,
	 no_alt,
	 "address",
	 "Specify default IP address",
	 PRIV_CONF);

/*
 * [no] async default ip
 */

KEYWORD (ci_async_default_ip, ci_async_default_ip_address,
	 ci_async_default_routing,
	 "ip",
	 "Specify default IP parameters",
	 PRIV_CONF|PRIV_HIDDEN);


/*
 * [no] async default
 */

KEYWORD (ci_async_default, ci_async_default_ip, ci_async_dynamic,
	 "default",
	 "Specify default parameters",
	 PRIV_CONF);

/*
 * [no] async
 */

KEYWORD	(ci_async, ci_async_default, NONE,
	 "async", "Async interface parameters", PRIV_CONF);

TEST_IDBSTATUS(ci_async_test, ci_async, NONE, ALTERNATE, IDB_ASYNC);

#undef	ALTERNATE
#define	ALTERNATE	ci_async_test
