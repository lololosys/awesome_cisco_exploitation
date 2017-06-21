/* $Id: cfg_ip_local_pool.h,v 3.2.60.1 1996/08/12 16:02:48 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_ip_local_pool.h,v $
 *------------------------------------------------------------------
 * IP address pool parse chains
 *
 * April 1995, Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_local_pool.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:48  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/18  19:35:22  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.2  1995/08/29  06:05:02  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.1  1995/08/17  01:43:13  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.1  1995/06/07  22:08:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*------------------------------------------------------------------
 *
 *
 * IP LOCAL POOL { DEFAULT | <pool-name-string> } <Start-IP> [ <End-IP> ]
 * NO IP LOCAL POOL { DEFAULT | <pool-name-string> }
 *
 * OBJ(int,1) = TRUE (if DEFAULT) or
 * OBJ(string,1) = <pool-name-string>
 */

EOLNS (cfg_ip_local_pool_eol, ip_localpool_command);

IPADDR (cfg_ip_local_pool_addr2, cfg_ip_local_pool_eol, cfg_ip_local_pool_eol,
	OBJ(paddr,2), "Last IP address of range");

IPADDR (cfg_ip_local_pool_addr1, cfg_ip_local_pool_addr2, no_alt,
	OBJ(paddr,1), "First IP address of range");

NOPREFIX(cfg_no_ip_local_pool, cfg_ip_local_pool_addr1, cfg_ip_local_pool_eol);

STRING	(cfg_ip_local_pool_pool, cfg_no_ip_local_pool, NONE,
	OBJ(string,1), "Create named local address pool");

TEST_MULTIPLE_FUNCS (cfg_ip_local_pool_test, cfg_ip_local_pool_pool,
	NONE, no_alt);

KEYWORD_ID (cfg_ip_local_pool_default, cfg_no_ip_local_pool,
	cfg_ip_local_pool_test,
	OBJ(int,1), TRUE,
	"default", "Create default local address pool", PRIV_CONF);

NVGENNS (cfg_ip_local_pool_nv, cfg_ip_local_pool_default,
	ip_localpool_command);

KEYWORD (cfg_ip_local_pool, cfg_ip_local_pool_nv, ALTERNATE,
	 "pool", "IP Local address pool lists",
	 PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	cfg_ip_local_pool
