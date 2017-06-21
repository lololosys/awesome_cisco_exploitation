/* $Id: cfg_int_ntp.h,v 3.2 1995/11/17 17:50:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ntp/cfg_int_ntp.h,v $
 *------------------------------------------------------------------
 * NTP interface command parser chains
 *
 * Dave Katz, December 1992
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ntp.h,v $
 * Revision 3.2  1995/11/17  17:50:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:44:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* [no] ntp disable
*
* The "[no] ntp enable" command is hidden but supported for back compatibility.
*/

EOLS    (ntp_enable_eol, ntp_if_config_cmd, NTP_ENABLE_IF_CMD);
KEYWORD (ntp_enable, ntp_enable_eol, no_alt, "enable", 
	 "", PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);
SPLIT   (ntp_enable_nv, ntp_enable, no_alt, nvgen, FALSE);

EOLS    (ntp_disable_eol, ntp_if_config_cmd, NTP_DISABLE_IF_CMD);
KEYWORD (ntp_disable, ntp_disable_eol, ntp_enable_nv, "disable", 
	 "Disable NTP", PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
* ntp broadcast [version #] [key #] [destination ipaddr]
* no ntp broadcast
*
* OBJ(int,1) = version (1-3)
* OBJ(int,2) = TRUE if "key" specified
* OBJ(int,3) = key number
* OBJ(int,4) = TRUE if "destination" specified
* OBJ(paddr,1) = destination address
*/

pdecl   (ntp_bcast_options);
IPADDR_NAME (ntp_bcast_dest_addr, ntp_bcast_options, no_alt, OBJ(paddr,1),
	     "Destination broadcast IP address");
EOLS    (ntp_bcast_eol, ntp_if_config_cmd, NTP_BROAD_IF_CMD);
KEYWORD_ID(ntp_bcast_dest, ntp_bcast_dest_addr, ntp_bcast_eol, OBJ(int,4),
	   TRUE, "destination", "Configure broadcast destination address",
	   PRIV_CONF|PRIV_SUBIF);
NUMBER  (ntp_bcast_key_num, ntp_bcast_options, no_alt, OBJ(int,3), 0, -1,
	 "Broadcast key number");
KEYWORD_ID(ntp_bcast_key, ntp_bcast_key_num, ntp_bcast_dest, OBJ(int,2), TRUE,
	   "key", "Configure broadcast authentication key",
	   PRIV_CONF|PRIV_SUBIF);
NUMBER  (ntp_bcast_version_value, ntp_bcast_options, no_alt, OBJ(int,1), 1,
	 NTP_VERSION, "NTP version number");
KEYWORD (ntp_bcast_options, ntp_bcast_version_value, ntp_bcast_key,
	 "version", "Configure NTP version", PRIV_CONF|PRIV_SUBIF);
SET     (ntp_bcast_version, ntp_bcast_options, OBJ(int,1), NTP_VERSION);
NOPREFIX(ntp_bcast_noprefix, ntp_bcast_version, ntp_bcast_eol);
NVGENS  (ntp_bcast_nvgen, ntp_bcast_noprefix, ntp_if_config_cmd, 
	 NTP_BROAD_IF_CMD);
SPLIT   (ntp_bcast_no, ntp_bcast_nvgen, ntp_bcast_eol, sense, TRUE);

/******************************************************************************
* [no] ntp broadcast client
*/

EOLS    (ntp_broadcastclient_eol, ntp_if_config_cmd, NTP_BROAD_CLIENT_IF_CMD);
KEYWORD (ntp_broadcastclient, ntp_broadcastclient_eol, ntp_bcast_no, "client",
	 "Listen to NTP broadcasts", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ntp_broadcast, ntp_broadcastclient, ntp_disable, "broadcast", 
	 "Configure NTP broadcast service", PRIV_CONF|PRIV_SUBIF);
KEYWORD (ntp_int, ntp_broadcast, NONE, "ntp", "Configure NTP",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
* Only allow NTP on VC subinterfaces
*/

ASSERT (ntp_subif, ntp_int, ALTERNATE, main_if_or_vc_or_vlan(csb->interface));

#undef ALTERNATE
#define ALTERNATE ntp_subif
