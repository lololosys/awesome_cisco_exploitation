/* $Id: cfg_dhcp.h,v 3.1.40.1 1996/09/05 23:06:11 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_dhcp.h,v $
 *------------------------------------------------------------------
 * cfg_dhcp.h - DHCP proxy client   parse chain components
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_dhcp.h,v $
 * Revision 3.1.40.1  1996/09/05  23:06:11  snyder
 * CSCdi68126:  declare some stuff in ip const
 *              228 out of data, only 4 from image
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:58:27  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:42:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/18  19:37:13  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.2  1995/06/21  18:16:24  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:02:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 ***************************************************************
 * IP subcommand:
 *        ip dhcp-server  <OBJ(paddr,1)>
 *  [no]  ip dhcp-server  <OBJ(paddr,1)>
 ***************************************************************
 */

static const char PARSE_ip_dhcpsrv_addr_help[] = "DHCP  server IP address";

EOLS	(cr_ip_dhcpsrv_eol, dhcpsrv_command, IP_DHCPSERVER);

IPADDR_NAME (cr_ip_dhcpsrv_addr, cr_ip_dhcpsrv_eol,
	     no_alt,
	     OBJ(paddr,1), "IP address or Name of DHCP server");

NVGENS	(cr_ip_dhcpsrv_nvgen, cr_ip_dhcpsrv_addr,
	 dhcpsrv_command, IP_DHCPSERVER);

KEYWORD	(cr_ip_dhcpsrv_server, cr_ip_dhcpsrv_nvgen, ALTERNATE,
	 "dhcp-server", "Specify address of DHCP server to use",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cr_ip_dhcpsrv_server
