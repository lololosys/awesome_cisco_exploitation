/* $Id: cfg_int_peer.h,v 3.2.6.4 1996/06/17 23:31:51 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_int_peer.h,v $
 *------------------------------------------------------------------
 * cfg_int_peer.h - DHCP proxy client   Interface "peer"  parse chain
 *
 * Mar 10, 1995, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_int_peer.h,v $
 * Revision 3.2.6.4  1996/06/17  23:31:51  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2.6.3  1996/04/27  05:51:09  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.2  1996/04/26  01:20:27  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2.6.2  1996/04/04  16:24:16  fox
 * CSCdi53596:  group-async interface can have a specific peer address
 * Branch: California_branch
 * Disallow 'peer default ip address a.b.c.d' for a group-async interface
 *
 * Revision 3.2.6.1  1996/03/18  20:15:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.12.2  1996/03/16  06:57:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.12.1  1996/03/13  01:37:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2  1996/03/05  07:07:30  fox
 * CSCdi50489:  unnecessary neighbor route installed, backup line never
 * drops
 * Add '[no] peer neighbor-route' command
 *
 * Revision 3.1  1996/02/01  04:58:28  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/30  16:03:19  fox
 * CSCdi47721:  Peer IP address supported on non-PPP/SLIP interfaces
 * Check for enctype of PPP or SLIP instead of for point-to-point link
 *
 * Revision 3.2  1995/11/17  18:43:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/18  19:37:15  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.2  1995/08/29  06:05:42  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.1  1995/06/07  22:06:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *   peer default ip address A.B.C.D
 */

EOLS (ci_peer_default_ip_addr_eol, peer_config_command, PEER_ADDR_CONFIG);

IPADDR (ci_peer_default_ip_addr_addr, ci_peer_default_ip_addr_eol, no_alt,
	OBJ(paddr, 1),
	"Default IP address for remote end of this interface");

ASSERT (ci_peer_default_ip_addr_test, ci_peer_default_ip_addr_addr, no_alt,
	!huntgroup_member(csb->interface->hwptr) &&
	!csb->interface->async_group_leader);

/*
 *  peer default IP address pool [POOLNAME
 *
 *  OBJ(string,1) = POOLNAME  (if poolname entered)
 */

EOLS (ci_peer_default_ip_pool_eol, peer_config_command, PEER_ADDR_DEFAULT);

EOLS (ci_peer_default_ip_pool_name_eol, peer_config_command, PEER_ADDR_POOL);

STRING (ci_peer_default_ip_pool_name, ci_peer_default_ip_pool_name_eol,
	ci_peer_default_ip_pool_eol, OBJ(string,1),
	"Name of IP address local-pool");

KEYWORD (ci_peer_default_ip_pool, ci_peer_default_ip_pool_name, no_alt,
	 "pool", "Use IP pool mechanism to allocate a peer IP address",
	 PRIV_CONF);

ASSERT (ci_peer_default_ip_pool_test, ci_peer_default_ip_pool,
	ci_peer_default_ip_addr_test,
	(ippools[PEER_ADDR_POOL] &&
	 (ippools[PEER_ADDR_POOL]->pool_type == PEER_ADDR_POOL)));

/*
 *   peer default ip address dhcp
 */

EOLS (ci_peer_default_ip_dhcp_eol, peer_config_command, PEER_ADDR_DHCP);

KEYWORD (ci_peer_default_ip_dhcp, ci_peer_default_ip_dhcp_eol, no_alt,
	 "dhcp",
	 "Use DHCP proxy client mechanism to allocate a peer IP address",
	 PRIV_CONF);

ASSERT (ci_peer_default_ip_dhcp_test, ci_peer_default_ip_dhcp,
	ci_peer_default_ip_pool_test,
	(ippools[PEER_ADDR_DHCP] &&
	 (ippools[PEER_ADDR_DHCP]->pool_type == PEER_ADDR_DHCP)));

/*
 * no peer default ip address
 */

EOLS (ci_no_peer_default_ip_addr_eol, peer_config_command, PEER_ADDR_NONE);

NOPREFIX(ci_peer_default_ip_addr_notest,
	 ci_peer_default_ip_dhcp_test,
	 ci_no_peer_default_ip_addr_eol);
	 
NVGENS (ci_peer_default_ip_addr_nvtest, ci_peer_default_ip_addr_notest,
	peer_config_command, PEER_ADDR_NONE);

KEYWORD (ci_peer_default_ip_address, ci_peer_default_ip_addr_nvtest,
	 no_alt,
	 "address",
	 "Specify default IP address",
	 PRIV_CONF);

/*
 * [no] peer default ip
 */

KEYWORD (ci_peer_default_ip, ci_peer_default_ip_address,
	 no_alt,
	 "ip",
	 "Specify default IP parameters",
	 PRIV_CONF);

/*
 * [no] peer default
 */

KEYWORD (ci_peer_default, ci_peer_default_ip, no_alt,
	 "default",
	 "Specify default parameters",
	 PRIV_CONF);

/*
 * [no] peer neighbor-route
 */

EOLS (ci_peer_neighborroute_eol, peer_config_command, PEER_NEIGHBOR_ROUTE);

KEYWORD (ci_peer_neighborroute, ci_peer_neighborroute_eol, ci_peer_default,
	 "neighbor-route", "Create neighbor route to peer if needed",
	 PRIV_CONF);

/*
 * [no] peer
 */

KEYWORD	(ci_peer_key, ci_peer_neighborroute, NONE,
	 "peer", "Peer parameters for point to point interfaces", PRIV_CONF);

ASSERT (ci_peer, ci_peer_key, ALTERNATE,
	is_ppp_or_slip(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE	ci_peer
