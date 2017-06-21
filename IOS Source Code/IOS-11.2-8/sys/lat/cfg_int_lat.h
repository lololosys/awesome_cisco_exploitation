/* $Id: cfg_int_lat.h,v 3.2 1995/11/17 17:32:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cfg_int_lat.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ L A T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_lat.h,v $
 * Revision 3.2  1995/11/17  17:32:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:44  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:06:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * lat host <service> <node> <enaddr>
 * no lat host <service> <node>
 */
EOLS	(lat_host_eol, lat_int_command, LAT_HOST );
ENADDR	(lat_host_enaddr, lat_host_eol, no_alt,
	 OBJ(hwaddr,1), "MAC address of the remote node" );
NOPREFIX(lat_host_noprefix, lat_host_enaddr, lat_host_eol );
STRING	(lat_host_service, lat_host_noprefix, no_alt,
	 OBJ(string,2), "Remote node providing this service" );
STRING	(lat_host_node, lat_host_service, no_alt,
	 OBJ(string,1), "Service name" );
NVGENS	(lat_host_nvgen, lat_host_node, lat_int_command, LAT_HOST );
KEYWORD	(lat_host, lat_host_nvgen, no_alt,
	 "host", "Statically define LAT services", PRIV_CONF );

/***************************************************************
 * [no] lat enabled
 */
EOLS	(lat_enabled_eol, lat_int_command, LAT_ENABLED );
KEYWORD (lat_enabled, lat_enabled_eol, lat_host,
	 "enabled", "Enable LAT protocol translation", PRIV_CONF );

KEYWORD	(lat_int, lat_enabled, ALTERNATE,
	 "lat", "LAT commands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	lat_int
