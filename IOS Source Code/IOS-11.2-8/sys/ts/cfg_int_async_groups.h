/* $Id: cfg_int_async_groups.h,v 3.3 1996/03/07 22:22:21 widmer Exp $
 * $Source: /release/111/cvs/Xsys/ts/cfg_int_async_groups.h,v $
 *------------------------------------------------------------------
 * Parser chain for interface groups
 *
 * April 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_async_groups.h,v $
 * Revision 3.3  1996/03/07  22:22:21  widmer
 * CSCdi50995:  help broken for dialer map command
 *
 * Revision 3.2  1995/11/17  18:53:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/18  19:38:43  fox
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
 * Revision 2.1  1995/06/07  22:03:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************ 
 *
 * member {number} async default ip address x.x.x.x    (deprecated)
 * member {number} peer default ip address x.x.x.x
 *
 ***********************************************************************/

EOLS (ci_AGC_async_default_ip_address_eol, async_group_config_command,
	GROUP_PEER_DEFAULT_IP_ADDRESS);

IPADDR(ci_AGC_async_default_ip_address_addr, 
			ci_AGC_async_default_ip_address_eol, no_alt,
	OBJ(paddr, 1),
	"Default IP address for remote user of async interface");

NVGENS(ci_AGC_async_default_ip_address_nvtest,
       ci_AGC_async_default_ip_address_addr,
       async_group_config_command, GROUP_PEER_DEFAULT_IP_ADDRESS);

NOPREFIX(ci_AGC_async_default_ip_address_notest,
	 ci_AGC_async_default_ip_address_nvtest,
	 ci_AGC_async_default_ip_address_eol);

KEYWORD(ci_AGC_async_default_ip_address, 
	ci_AGC_async_default_ip_address_notest, no_alt,
	"address", "Specify default IP address",
	PRIV_CONF);

KEYWORD (ci_AGC_async_default_ip, ci_AGC_async_default_ip_address, no_alt,
	"ip", "specify default IP parameters", PRIV_CONF);
KEYWORD (ci_AGC_async_default, ci_AGC_async_default_ip, no_alt,
	"default", "Specify default paramenters", PRIV_CONF);
KEYWORD(ci_AGC_peer, ci_AGC_async_default, no_alt,
	"peer", "Peer interface parameters", PRIV_CONF);
KEYWORD (ci_AGC_async, ci_AGC_async_default, ci_AGC_peer,
	"async", "Async interface parameters", PRIV_CONF|PRIV_HIDDEN);

/************************************************************************ 
 *
 * member {number} description {text}
 *
 ***********************************************************************/

EOLS(ci_AGC_descr_eol, async_group_config_command,
	GROUP_DESCRIPTION);

TEXT    (ci_AGC_descr_text, ci_AGC_descr_eol, no_alt,
	 OBJ(string,1), "A description of this interface");

NOPREFIX (ci_AGC_descr_noprefix, ci_AGC_descr_text, ci_AGC_descr_eol);

NVGENS (ci_AGC_descr_nvgen, ci_AGC_descr_noprefix, 
	async_group_config_command, GROUP_DESCRIPTION);

KEYWORD (ci_AGC_descr, ci_AGC_descr_nvgen, ci_AGC_async,
	"description", "Interface specific description", PRIV_CONF);

/*************************************************************************/

/* above this point are the Async Group Commands  (AGC) */

NUMBER	(ci_async_member_number, ci_AGC_descr, no_alt,
	 OBJ(int,22), 1, 112, "Line number");

NVGENS (ci_async_member_nvgen, ci_async_member_number,
	async_group_config_command, ASYNC_GROUP);

KEYWORD	(ci_async_member, ci_async_member_nvgen, NONE,
	 "member", "Specific member oddities", PRIV_CONF);

/*
 * group-range <start> <end>
 * no group-range
 */
EOLS	(ci_async_range_eol, async_group_config_command, 
					ASYNC_GROUP_RANGE);

NUMBER	(ci_async_range_range2, ci_async_range_eol, no_alt,
	 OBJ(int,2), 1, 112, "End of range");

NUMBER	(ci_async_range_range1, ci_async_range_range2, no_alt,
	 OBJ(int,1), 1, 112, "Start of range");

NVGENS (ci_async_range_nvgen, ci_async_range_range1,
	async_group_config_command, ASYNC_GROUP_RANGE);

NOPREFIX(ci_async_range_noprefix, ci_async_range_nvgen, ci_async_range_eol);

KEYWORD	(ci_async_range, ci_async_range_noprefix, ci_async_member,
	 "group-range", "Range of async interfaces", PRIV_CONF);

ASSERT (ci_async_groups_test, ci_async_range, ALTERNATE, 
	csb->interface->async_group_leader == TRUE);

#undef	ALTERNATE
#define	ALTERNATE	ci_async_groups_test
