/* $Id: cfg_int_novell_netbios.h,v 3.2 1995/11/17 09:21:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/cfg_int_novell_netbios.h,v $
 *------------------------------------------------------------------
 * Netbios Novell interface commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_novell_netbios.h,v $
 * Revision 3.2  1995/11/17  09:21:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:06:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] novell netbios input-access-filter {host|bytes} <name>
 * [no] novell netbios output-access-filter {host|bytes} <name>
 *
 * NOTE: This ties into the netbios code. NETB_NOV_IAFILTER/NETB_NOV_OAFILTER
 *       are defined in those sections in parser_defs_netbios.h.
 *
 * OBJ(int,1) = 	NETB_NOV_IAFILTER = input-access-filter
 *		NETB_NOV_OAFILTER = output-access-filter
 * OBJ(int,2) =	FALSE = host
 *		TRUE = bytes
 * OBJ(string,1) = name
 */
EOLS	(ci_nov_netbios_af_eol,
	 netbios_acl_if_command, NETBIOS_ACCESS_FILTER);

STRING	(ci_nov_netbios_af_name, ci_nov_netbios_af_eol, no_alt,
	OBJ(string,1), "Name of previously defined access list filter");

NVGENS	(ci_nov_netbios_af_nvgen, ci_nov_netbios_af_name,
	 netbios_acl_if_command, NETBIOS_ACCESS_FILTER);

KEYWORD_ID(ci_nov_netbios_af_bytes, ci_nov_netbios_af_nvgen, no_alt,
	   OBJ(int,2), TRUE,
	   "bytes",
	   "Filter packets with declared byte offset access list filter",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_netbios_af_host, ci_nov_netbios_af_nvgen,
	   ci_nov_netbios_af_bytes, OBJ(int,2), FALSE,
	   "host", "Filter packets with declared host access list filter",
	   PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_nov_netbios_oaf, ci_nov_netbios_af_host, no_alt,
	   OBJ(int,1), NETB_NOV_OAFILTER,
	   "output-access-filter","Access list filter on outgoing messages",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_netbios_iaf, ci_nov_netbios_af_host, ci_nov_netbios_oaf,
	   OBJ(int,1), NETB_NOV_IAFILTER,
	   "input-access-filter","Access list filter on incoming messages",
	   PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_nov_netbios, ci_nov_netbios_iaf, ALTERNATE,
	 "netbios","Setup IPX NetBIOS filters and caching on this interface",
	 PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_nov_netbios
