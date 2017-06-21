/* $Id: exec_show_vpn.h,v 1.2.40.2 1996/08/12 16:09:13 widmer Exp $
 * $Source: /release/112/cvs/Xsys/vpn/exec_show_vpn.h,v $
 *------------------------------------------------------------------
 * exec_show_vpn.h   ---   show commands for VPDN
 *
 * January 1996, Tim Kolar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_vpn.h,v $
 * Revision 1.2.40.2  1996/08/12  16:09:13  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 1.2.40.1  1996/05/12  19:22:17  tkolar
 * CSCdi57472:  vpn is an overloaded acronym
 * Branch: California_branch
 * Change "vpn" to "vpdn", and make a few other user interface tweaks
 * while we're at it.
 *
 * Revision 1.2  1996/01/31  18:38:30  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:07  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show vpdn
 */
EOLNS (show_vpdn_eol, show_l2f_command);

STRING (show_vpdn_gw, show_vpdn_eol, no_alt,
	 OBJ(string, 2), "Name of Gateway end of tunnel");

STRING (show_vpdn_nas, show_vpdn_gw, show_vpdn_eol,
	 OBJ(string, 1), "Name of NAS end of tunnel");

KEYWORD (show_vpdn, show_vpdn_nas, NONE,
            "vpdn", "VPDN information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_vpdn
