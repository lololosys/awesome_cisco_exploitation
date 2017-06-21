/* $Id: cfg_addrpool.h,v 3.1 1995/11/09 11:53:53 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/cfg_addrpool.h,v $
 *------------------------------------------------------------------
 * cfg_addrpool.h - IP Address Pooling parse chain components
 *
 * Sept 10, 1995, Craig Fox
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_addrpool.h,v $
 * Revision 3.1  1995/11/09  11:53:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/18  19:35:21  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 ***************************************************************
 * IP subcommand:
 *        ip address-pool < dhcp-proxy-client | local >
 *  [no]  ip address-pool
 ***************************************************************
 */

EOLS	(cr_ip_addr_pool_local_eol, ip_address_pool_command, PEER_ADDR_POOL);

KEYWORD	(cr_ip_addr_pool_local, cr_ip_addr_pool_local_eol, no_alt,
	 "local", "Use Local IP Address pooling",
	 PRIV_CONF);

ASSERT (cr_ip_addr_pool_test_local, cr_ip_addr_pool_local, no_alt,
	(ippools[PEER_ADDR_POOL] &&
	 (ippools[PEER_ADDR_POOL]->pool_type == PEER_ADDR_POOL)));

EOLS	(cr_ip_addr_pool_dhcp_eol, ip_address_pool_command, PEER_ADDR_DHCP);

KEYWORD	(cr_ip_addr_pool_dhcp, cr_ip_addr_pool_dhcp_eol, no_alt,
	 "dhcp-proxy-client", "Use DHCP proxy client feature",
	 PRIV_CONF);

ASSERT (cr_ip_addr_pool_test_dhcp, cr_ip_addr_pool_dhcp,
	cr_ip_addr_pool_test_local,
	(ippools[PEER_ADDR_DHCP] &&
	 (ippools[PEER_ADDR_DHCP]->pool_type == PEER_ADDR_DHCP)));

NVGENS	(cr_ip_addr_pool_nvgens, cr_ip_addr_pool_test_dhcp,
	 ip_address_pool_command, 0);

NOPREFIX(cr_ip_addr_pool_no, cr_ip_addr_pool_nvgens,
	 cr_ip_addr_pool_dhcp_eol);

KEYWORD_MM(cr_ip_addr_pool, cr_ip_addr_pool_no, ALTERNATE,
	 "address-pool", "Specify default IP address pooling mechanism",
	 PRIV_CONF, 8);

#undef	ALTERNATE
#define	ALTERNATE	cr_ip_addr_pool
