/* $Id: cfg_ip_localpool.h,v 3.2.58.1 1996/08/12 16:02:49 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_ip_localpool.h,v $
 *------------------------------------------------------------------
 * IP address pool parse chains
 *
 * April 1995, Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_localpool.h,v $
 * Revision 3.2.58.1  1996/08/12  16:02:49  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/18  19:35:23  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.2  1995/08/29  06:05:03  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.1  1995/08/17  01:43:14  tli
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
 * IP LOCAL-POOL { DEFAULT | <pool-name-string> } <Start-IP> [ <End-IP> ]
 * NO IP LOCAL-POOL { DEFAULT | <pool-name-string> }
 *
 * OBJ(int,1) = TRUE (if DEFAULT) or
 * OBJ(string,1) = <pool-name-string>
 */

EOLNS (cfg_ippool_eol, ip_localpool_command);

IPADDR (cfg_ippool_addr2, cfg_ippool_eol, cfg_ippool_eol,
	OBJ(paddr,2), "Last IP address of range");

IPADDR (cfg_ippool_addr1, cfg_ippool_addr2, no_alt,
	OBJ(paddr,1), "First IP address of range");

NOPREFIX(cfg_no_ippool, cfg_ippool_addr1, cfg_ippool_eol);

STRING	(cfg_ippool_pool, cfg_no_ippool, NONE,
	OBJ(string,1), "Create named local address pool");

TEST_MULTIPLE_FUNCS (cfg_ippool_test, cfg_ippool_pool, NONE, no_alt);

KEYWORD_ID (cfg_ippool_default, cfg_no_ippool, cfg_ippool_test,
	OBJ(int,1), TRUE,
	"default", "Create default local address pool", PRIV_CONF);

KEYWORD_MM (cfg_ippool, cfg_ippool_default, ALTERNATE,
	 "local-pool", "IP Local address pool lists",
	 PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN, 6);

#undef  ALTERNATE
#define ALTERNATE	cfg_ippool
