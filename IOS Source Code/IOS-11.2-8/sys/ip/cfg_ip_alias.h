/* $Id: cfg_ip_alias.h,v 3.2 1995/11/17 09:32:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/cfg_ip_alias.h,v $
 *------------------------------------------------------------------
 * IP alias config commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_ip_alias.h,v $
 * Revision 3.2  1995/11/17  09:32:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:08:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************
 * ip alias <ipaddr> <port>
 * no ip alias <ipaddr> [<port>]
 *
 * (*OBJ(paddr,1)) = <ipaddr>
 * OBJ(int,1) = <port>
 */
EOLS	(cip_alias_eol, ip_alias_command, IP_ALIAS);
NUMBER	(cip_alias_port, cip_alias_eol, no_alt,
	 OBJ(int,1), 0, 65535, "IP port number");
NOPREFIX (cip_alias_noprefix, cip_alias_port, cip_alias_eol);
IPADDR	(cip_alias_addr, cip_alias_noprefix, no_alt,
	 OBJ(paddr,1), "IP address to alias to a port");
NVGENS	(cip_alias_nvgen, cip_alias_addr, ip_alias_command, IP_ALIAS);
KEYWORD	(cip_alias, cip_alias_nvgen, ALTERNATE,
	 "alias", "Alias an IP address to a TCP port", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_alias
