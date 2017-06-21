/* $Id: exec_test_dhcp.h,v 3.1.40.1 1996/08/12 16:03:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_test_dhcp.h,v $
 *-------------------------------------------------------------------
 * exec_test_dhcp.h - DHCP proxy client   Test & Diagnostics parse
 *                                        chain
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_test_dhcp.h,v $
 * Revision 3.1.40.1  1996/08/12  16:03:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:58:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:47:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:27:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test dhcp  { allocate | release | renew } [ip-address]
 */

EOLNS (priv_test_dhcp_eol, test_dhcp_command);

IPADDR_NAME (priv_test_dhcp_ipaddr, priv_test_dhcp_eol,
	     priv_test_dhcp_eol,
	     OBJ(paddr,1),
	     NULL /*"IP address or IP name to allocate/release/renew"*/);

KEYWORD_ID (priv_test_dhcp_renew, priv_test_dhcp_ipaddr,
	    no_alt,
	    OBJ(int,1), 3,
	    "renew", NULL /*"Renew an IP address lease"*/, PRIV_OPR);

KEYWORD_ID (priv_test_dhcp_release, priv_test_dhcp_ipaddr,
	    priv_test_dhcp_renew,
	    OBJ(int,1), 2,
	    "release", NULL /*"Release an IP address"*/, PRIV_OPR);

KEYWORD_ID (priv_test_dhcp_allocate, priv_test_dhcp_ipaddr,
	    priv_test_dhcp_release,
	    OBJ(int,1), 1,
	    "allocate", NULL /*"Allocate an IP address"*/, PRIV_OPR);

KEYWORD	(priv_test_dhcp, priv_test_dhcp_allocate, ALTERNATE,
	 "dhcp", NULL /*"DHCP diagnostic code"*/, PRIV_ROOT|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_dhcp
