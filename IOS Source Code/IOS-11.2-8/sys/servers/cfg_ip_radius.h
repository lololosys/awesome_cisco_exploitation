/* $Id: cfg_ip_radius.h,v 3.1.62.1 1996/07/10 00:50:12 irfan Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_ip_radius.h,v $
 *------------------------------------------------------------------
 * CFG_IP_RADIUS:  Contains parser chains for "ip radius ...."
 *
 * Jan 1996, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_radius.h,v $
 * Revision 3.1.62.1  1996/07/10  00:50:12  irfan
 * CSCdi62197:  ip radius source-interface command lost after reload
 * Branch: California_branch
 * use parser's forward_interface instead of the interface macro.
 *
 * Revision 3.1  1996/01/03  19:57:33  irfan
 * CSCdi46304:  radius packets could use a predicatable source IP address
 * allow radius to use a specific (up) interface's IP address.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * parser chain for ip radius commands
 */

/******************************************************************************
 *	[no] ip radius source-interface <sub-interface>
 *
 * OBJ(idb,1) -- IDB to pull source IP address from
 */

EOLS	(cip_radius_source_eol, radius_cfgcmd, RADIUS_SOURCE_INTERFACE);

FORWARD_INTERFACE (cip_radius_source_name, cip_radius_source_eol, no_alt,
		   OBJ(idb,1), IFTYPE_ANYSUB);

NOPREFIX (cip_radius_source_noprefix, cip_radius_source_name,
	  cip_radius_source_eol);

NVGENS	(cip_radius_source_nvgen, cip_radius_source_noprefix,
	 radius_cfgcmd, RADIUS_SOURCE_INTERFACE);

KEYWORD	(cip_radius_source, cip_radius_source_nvgen, no_alt,
	 "source-interface",
	 "Specify interface for source address in RADIUS packets",
	 PRIV_CONF);

KEYWORD	(cip_radius, cip_radius_source, ALTERNATE,
	 "radius", "RADIUS configuration commands",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_radius

