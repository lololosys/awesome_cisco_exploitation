/* $Id: cfg_netbios_name.h,v 3.3.44.1 1996/05/17 11:19:57 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/cfg_netbios_name.h,v $
 *------------------------------------------------------------------
 * Netbios name config commands
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_netbios_name.h,v $
 * Revision 3.3.44.1  1996/05/17  11:19:57  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.32.2  1996/04/26  15:19:29  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.32.1  1996/04/03  14:33:17  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3  1995/12/12  06:09:23  gschleic
 * CSCdi44259:  Netbios name-cache timeout parses incorrectly
 *
 * Revision 3.2  1995/11/17  09:22:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * netbios name-cache <TR-address> <name>
 *			[{ <interface> | ring-group <group> }]
 * no netbios name-cache <TR-address> <name>
 *			[{ <interface> | ring-group <group> }]
 *
 * (*OBJ(paddr,1)) = <TR-address>
 * OBJ(string,1) = <name>
 */
EOLS   (conf_netbios_namecache_addr_eol, netbios_name_cache_command,
	NETBIOS_NAMECACHE_NAME);
NUMBER (conf_netbios_namecache_addr_ringgrp, conf_netbios_namecache_addr_eol,
	 no_alt,
	 OBJ(int,1), 1, SRB_MAX_RING_NUM, "Ring-group number");
KEYWORD (conf_netbios_namecache_addr_ring, conf_netbios_namecache_addr_ringgrp,
	 conf_netbios_namecache_addr_eol,
	 "ring-group", "Accessible via a ring-group", PRIV_CONF);
INTERFACE (conf_netbios_namecache_addr_if, conf_netbios_namecache_addr_eol,
	   conf_netbios_namecache_addr_ring, OBJ(idb,1), IFTYPE_TOKENRING);
NOPREFIX (conf_netbios_namecache_addr_noprefix, conf_netbios_namecache_addr_if,
	  conf_netbios_namecache_addr_eol);
STRING (conf_netbios_namecache_addr_name, conf_netbios_namecache_addr_noprefix,
	no_alt,
	OBJ(string,1), "NetBIOS-name");
ENADDR (conf_netbios_namecache_addr_val, conf_netbios_namecache_addr_name,
	no_alt,
	OBJ(hwaddr,1), "MAC addr of static name-cache entry");
NVGENS (conf_netbios_namecache_addr, conf_netbios_namecache_addr_val,
	netbios_name_cache_command, NETBIOS_NAMECACHE_NAME);

/***************************************************************
 * netbios name-cache recognized-timeout <seconds>
 * no netbios name-cache recognized-timeout [<seconds>]
 *
 * OBJ(int,1) = <seconds>
 */
PARAMS_KEYONLY (conf_netbios_namecache_recog, conf_netbios_namecache_addr,
		"recognized-timeout", OBJ(int,1), 1, -1, 
		netbios_name_cache_command, NETBIOS_NAMECACHE_RECOGTIME,
		"Set the recognized timeout value", "Seconds", PRIV_CONF);

/***************************************************************
 * netbios name-cache query-timeout <seconds>
 * no netbios name-cache query-timeout [<seconds>]
 *
 * OBJ(int,1) = <seconds>
 */
PARAMS_KEYONLY (conf_netbios_namecache_query, conf_netbios_namecache_recog,
		"query-timeout", OBJ(int,1), 1, -1, 
		netbios_name_cache_command, NETBIOS_NAMECACHE_QUERYTIME,
		"Set the query timeout value", "Seconds", PRIV_CONF);

/***************************************************************
 * netbios name-cache broadcast-name-recognized
 * no netbios name-cache broadcast-name-recognized
 *
 */
EOLS (conf_netbios_namecache_broadcast_eol, netbios_name_cache_command,
      NETBIOS_BROADCAST_NAME_RECOGNIZED);
KEYWORD (conf_netbios_namecache_broadcast,
	conf_netbios_namecache_broadcast_eol,
	conf_netbios_namecache_query, "broadcast-name-recognized",
	"Broadcast netbios name_recognized frame", PRIV_CONF);

/***************************************************************
 * netbios name-cache proxy-datagram <seconds>
 * no netbios name-cache proxy-datagram [<seconds>]
 *
 * OBJ(int,1) = <seconds>
 */
PARAMS_KEYONLY (conf_netbios_namecache_proxy, conf_netbios_namecache_broadcast,
		"proxy-datagram", OBJ(int,1), 0, -1, 
		netbios_name_cache_command, NETBIOS_PROXY_DATAGRAM,
		"Enable datagram proxy", "Seconds", PRIV_CONF);

/***************************************************************
 * netbios name-cache name-len <numbers>
 * no netbios name-cache name-len [<numbers>]
 *
 * OBJ(int,1) = <numbers>
 */
PARAMS_KEYONLY (conf_netbios_namecache_name_length, conf_netbios_namecache_proxy,
		"name-len", OBJ(int,1), 8, 16, 
		netbios_name_cache_command, NETBIOS_NAME_LENGTH,
		"Set the length of a netbios name", "Numbers", PRIV_CONF);

/***************************************************************
 * netbios name-cache cache-add-name
 * no netbios name-cache cache-add-name
 */
EOLS (conf_netbios_namecache_cache_add_name_eol, netbios_name_cache_command,
      NETBIOS_NAMECACHE_CACHE_ADD_NAME );
KEYWORD (conf_netbios_namecache_cache_add_name,
	 conf_netbios_namecache_cache_add_name_eol,
	 conf_netbios_namecache_name_length, 
	 "cache-add-name", "Cache NetBIOS names from ADD_NAME_QUERY packets",
	 PRIV_CONF );

/***************************************************************
 * netbios name-cache timeout <minutes>
 * no netbios name-cache timeout [<minutes>]
 *
 * OBJ(int,1) = <minutes>
 */
PARAMS_KEYONLY (conf_netbios_namecache_timeout,
		conf_netbios_namecache_cache_add_name,
		"timeout", OBJ(int,1), 1, 35000, 
		netbios_name_cache_command, NETBIOS_NAMECACHE_TIMEOUT,
		"Set the timeout value", "Minutes", PRIV_CONF);

/***************************************************************/
KEYWORD (config_netbios_namecache, conf_netbios_namecache_timeout, ALTERNATE,
	 "name-cache", "Set name cache parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_netbios_namecache
