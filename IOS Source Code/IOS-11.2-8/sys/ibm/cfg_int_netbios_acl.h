/* $Id: cfg_int_netbios_acl.h,v 3.3 1996/03/07 06:34:10 kchiu Exp $
 * $Source: /release/111/cvs/Xsys/ibm/cfg_int_netbios_acl.h,v $
 *------------------------------------------------------------------
 * Netbios access list interface commands
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_netbios_acl.h,v $
 * Revision 3.3  1996/03/07  06:34:10  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.2  1995/11/17  09:21:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:06:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * [no] netbios input-access-filter {host|bytes|session-bytes} <name>
 * [no] netbios output-access-filter {host|bytes|session-bytes} <name>
 *
 * OBJ(int,1) = 	NETB_IAFILTER = input-access-filter
 *		NETB_OAFILTER = output-access-filter
 * OBJ(int,2) =	NETBIOS_HOST_FILTER = host
 *		NETBIOS_BYTES_FILTER = bytes
 *		NETBIOS_SESSION_BYTES_FILTER = session-bytes
 * OBJ(string,1) = name
 */

EOLS	(ci_netbios_af_eol, netbios_acl_if_command, NETBIOS_ACCESS_FILTER);

STRING	(ci_netbios_af_name, ci_netbios_af_eol, no_alt,
	 OBJ(string,1), "Name of previously defined access list filter");

NVGENS	(ci_netbios_af_nvgen, ci_netbios_af_name,
	 netbios_acl_if_command, NETBIOS_ACCESS_FILTER);

KEYWORD_ID(ci_netbios_af_sess_bytes, ci_netbios_af_nvgen, no_alt,
           OBJ(int,2), NETBIOS_SESSION_BYTES_FILTER,
           "session-bytes",
           "Filter packets (I-frame) with declared byte offset access list filter",
           PRIV_CONF | PRIV_SUBIF);
KEYWORD_ID(ci_netbios_af_bytes, ci_netbios_af_nvgen, ci_netbios_af_sess_bytes,
	   OBJ(int,2), NETBIOS_BYTES_FILTER,
	   "bytes",
	   "Filter packets with declared byte offset access list filter",
	   PRIV_CONF | PRIV_SUBIF);
KEYWORD_ID(ci_netbios_af_host, ci_netbios_af_nvgen, ci_netbios_af_bytes,
	   OBJ(int,2), NETBIOS_HOST_FILTER,
	   "host", "Filter packets with declared host access list filter",
	   PRIV_CONF | PRIV_SUBIF);

KEYWORD_ID(ci_netbios_oaf, ci_netbios_af_host, no_alt,
	   OBJ(int,1), NETB_OAFILTER,
	   "output-access-filter","Access list filter on outgoing messages",
	   PRIV_CONF | PRIV_SUBIF);
KEYWORD_ID(ci_netbios_iaf, ci_netbios_af_host, ci_netbios_oaf,
	   OBJ(int,1), NETB_IAFILTER,
	   "input-access-filter","Access list filter on incoming messages",
	   PRIV_CONF | PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_netbios_iaf
