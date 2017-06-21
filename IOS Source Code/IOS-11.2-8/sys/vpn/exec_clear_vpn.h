/* $Id: exec_clear_vpn.h,v 1.1.2.1 1996/07/12 17:29:37 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/exec_clear_vpn.h,v $
 *------------------------------------------------------------------
 * exec_clear_vpn.h   ---  clear commands for VPN.
 *
 * July 1996, Tim Kolar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_vpn.h,v $
 * Revision 1.1.2.1  1996/07/12  17:29:37  tkolar
 * CSCdi62195:  L2F tunnel clearing command needed
 * Branch: California_branch
 * Make is possible to shut down an entire tunnel, rather than having
 * to do it MID by MID.
 *
 * Revision 1.1  1996/07/12  17:06:49  tkolar
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear vpdn accounting
 * clear vpdn tunnel <nas-name> <gateway-name>
 *
 * OBJ(int,1) = NAS name
 * OBJ(int,2) = Gateway name
 */
EOLS	(clear_vpdn_tunnel_eol, clear_vpdn_command, L2F_CLEAR_TUNNEL);

STRING (clear_vpdn_tunnel_s2, clear_vpdn_tunnel_eol, no_alt,
	 OBJ(string, 2), "Gateway name");

STRING (clear_vpdn_tunnel_s1, clear_vpdn_tunnel_s2, no_alt,
	 OBJ(string, 1), "NAS name");

EOLS	(clear_vpdn_account_eol, clear_vpdn_command, L2F_CLEAR_ACCOUNT);
KEYWORD (clear_vpdn_account, clear_vpdn_account_eol, no_alt,
	 "accounting", "Clear global VPDN accounting numbers", PRIV_HIDDEN);

KEYWORD (clear_vpdn_tunnel, clear_vpdn_tunnel_s1, clear_vpdn_account,
	 "tunnel", "Clear a VPDN tunnel", PRIV_OPR);

KEYWORD (clear_vpdn, clear_vpdn_tunnel, ALTERNATE,
	 "vpdn", "Clear a VPDN entity", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_vpdn
