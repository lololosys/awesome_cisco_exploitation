/* $Id: cfg_int_mac-address.h,v 3.2.62.1 1996/08/12 16:05:43 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_mac-address.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ M A C _ A D D R E S S . H
 *
 * Source-route bridging support
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_mac-address.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:43  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:43:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:06:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	mac-address <macaddr>
 *
 */

EOLNS	(macaddr_eol, macaddr_command);
ENADDR	(macaddr_addr, macaddr_eol, no_alt, OBJ(hwaddr,1), "MAC address");
NOPREFIX(macaddr_noprefix, macaddr_addr, macaddr_eol);
NVGENNS	(macaddr_nvgen, macaddr_noprefix, macaddr_command);
KEYWORD	(macaddr, macaddr_nvgen, NONE, "mac-address",
	 "Manually set interface MAC address", PRIV_CONF);

TEST_IDBSTATUS(macaddr_test2, NONE, macaddr, NONE, IDB_CHANNEL);
TEST_IDBSTATUS(macaddr_test, macaddr_test2, NONE, ALTERNATE,
	       (IDB_IEEEMASK|IDB_BRIDGE|IDB_BVI));

#undef	ALTERNATE
#define	ALTERNATE	macaddr_test
