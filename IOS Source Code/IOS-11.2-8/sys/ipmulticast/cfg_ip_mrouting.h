/* $Id: cfg_ip_mrouting.h,v 3.2.58.1 1996/08/12 16:03:15 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/cfg_ip_mrouting.h,v $
 *------------------------------------------------------------------
 * cfg_ip_mrouting.h - IP multicast routing configuration commands.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ip_mrouting.h,v $
 * Revision 3.2.58.1  1996/08/12  16:03:15  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:29:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:08:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * [no] ip sdr cache-timeout <minutes>
 */
EOLS (cip_sdr_to_eols, sd_commands, SDR_CACHETIMEOUT_COMMAND);
NUMBER (cip_sdr_to_num, cip_sdr_to_eols, no_alt,
	OBJ(int,1), 1, -1, "Timeout in minutes");
NOPREFIX (cip_sdr_to_no, cip_sdr_to_num, cip_sdr_to_eols);
NVGENS (cip_sdr_to_nvgen, cip_sdr_to_no, sd_commands,
	SDR_CACHETIMEOUT_COMMAND);
KEYWORD (cip_sdr_to, cip_sdr_to_nvgen, no_alt,
	 "cache-timeout", "Timeout period for entries",
	 PRIV_CONF);
KEYWORD (cip_sdr, cip_sdr_to, ALTERNATE,
	 "sdr", "Global IP Multicast SDR Commands", PRIV_CONF);

/*
 * [no] ip multicast cache-headers
 */
EOLS (cip_multicast_cache_eol, ip_multicast_cache_headers_command, 0);
NVGENS (cip_multicast_cache_nvgen, cip_multicast_cache_eol, 
	ip_multicast_cache_headers_command, 0);
KEYWORD (cip_multicast_cache, cip_multicast_cache_nvgen, ALTERNATE,
	 "cache-headers", 
	 "Store IP multicast packet headers in circular buffer", PRIV_HIDDEN);
KEYWORD (cip_multicast, cip_multicast_cache, cip_sdr,
	 "multicast", "Global IP Multicast Commands", PRIV_HIDDEN);

/*
 * [no] ip multicast-routing
 */
EOLNS (cip_multicast_eol, ip_multicast_command);
KEYWORD (cip_mrouting, cip_multicast_eol, cip_multicast,
	 "multicast-routing", "Enable IP multicast forwarding", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_mrouting



