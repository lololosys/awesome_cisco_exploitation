/* $Id: cfg_ip_routing.h,v 3.2.60.1 1996/04/15 21:15:24 bcole Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_ip_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ I P _ R O U T I N G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ip_routing.h,v $
 * Revision 3.2.60.1  1996/04/15  21:15:24  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:32:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/17  01:43:15  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.1  1995/07/01  12:55:38  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:08:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cr_ip_extend_here, no_alt);

/******************************************************************************
 *	ip default-gateway <ip-addr>
 *	no ip default-gateway [<ip-addr>]
 *
 * OBJ(paddr,1) = address
 */
EOLS	(cr_ip_default_gw_eol, ip_command, IP_DEFAULTGATEWAY);
IPADDR	(cr_ip_default_gw_ipaddr, cr_ip_default_gw_eol, no_alt,
	 OBJ(paddr,1), "IP address of default gateway");
NOPREFIX(cr_ip_default_gw_no, cr_ip_default_gw_ipaddr, cr_ip_default_gw_eol);
NVGENS	(cr_ip_default_gw_nvgen, cr_ip_default_gw_no,
	 ip_command, IP_DEFAULTGATEWAY);
KEYWORD	(cr_ip_default_gateway, cr_ip_default_gw_nvgen, cr_ip_extend_here,
	 "default-gateway", "Specify default gateway (if not routing IP)",
	 PRIV_CONF);

/******************************************************************************
 *	[no] ip local ...
 */

LINK_TRANS(cfg_ip_local_extend_here, no_alt);

KEYWORD	(cip_local, cfg_ip_local_extend_here, cr_ip_default_gateway,
	 "local",
	 "Specify local options",
	 PRIV_CONF);

KEYWORD (cr_ip, cip_local, ALTERNATE,
	 "ip", " ", PRIV_CONF | PRIV_DUPLICATE);

#undef	ALTERNATE
#define	ALTERNATE	cr_ip
