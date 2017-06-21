/* $Id: cfg_int_netbios_name.h,v 3.2 1995/11/17 09:21:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/cfg_int_netbios_name.h,v $
 *------------------------------------------------------------------
 * Netbios name interface commands
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_netbios_name.h,v $
 * Revision 3.2  1995/11/17  09:21:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:06:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * [no] netbios enable-name-cache
 *
 */
EOLS	(ci_netbios_namecache_eol, netbios_name_if_command,
	 NETBIOS_ENABLE_NAMECACHE);
KEYWORD (ci_netbios_namecache, ci_netbios_namecache_eol, ALTERNATE,
	 "enable-name-cache", "Enable NETBIOS name cache on this interface",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_netbios_namecache
