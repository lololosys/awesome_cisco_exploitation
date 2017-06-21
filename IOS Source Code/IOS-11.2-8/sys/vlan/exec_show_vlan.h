/*--------------------------------------------------------------------------
 *
 * $Id: exec_show_vlan.h,v 3.3.20.1 1996/08/12 16:09:12 widmer Exp $
 *
 * vlan/exec_show_vlan.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Parser "show command" support for Virtual LANs.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: exec_show_vlan.h,v $
 * Revision 3.3.20.1  1996/08/12  16:09:12  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:10:13  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1996/01/23  08:55:18  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


LINK_TRANS(conf_vlan_extend_here, no_alt);


/**************************************************************************
 *
 * show vlans
 *
 * OBJ(int, 1) = <vLAN ID i.e. Tag>
 *
 **************************************************************************/

EOLNS    (show_vlan_eol, show_vlans);

NUMBER   (show_single_vlan, show_vlan_eol, show_vlan_eol, OBJ(int,1),
          1, MAX_VLAN_ID, "Display configuration for a particular vLAN ID.");

KEYWORD (show_virtual_lans, show_single_vlan, ALTERNATE, "vlans",
	 "Virtual LANs Information", PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_virtual_lans
