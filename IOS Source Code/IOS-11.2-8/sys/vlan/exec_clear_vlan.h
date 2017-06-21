/*-------------------------------------------------------------------------
 *
 * $Id: exec_clear_vlan.h,v 3.3.20.1 1996/08/12 16:09:11 widmer Exp $
 *
 * vlan/exec_clear_vlan.h
 *
 * May 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Virtual LANs related statistics.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: exec_clear_vlan.h,v $
 * Revision 3.3.20.1  1996/08/12  16:09:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:10:12  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1996/01/23  08:55:15  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


/******************************************************************
 *
 * clear vlan counters
 *
 * OBJ(int, 1) = <vLAN ID i.e. Tag>
 *
 ******************************************************************/

EOLNS	   (vlan_clear_eol, vlan_clear_statistics);

KEYWORD    (vlan_clear_statistics_kwd, vlan_clear_eol, no_alt,
	    "statistics", "Clear vLAN statistics", PRIV_OPR);

NUMBER     (particular_vlan, vlan_clear_eol, vlan_clear_statistics_kwd,
	    OBJ(int, 1), 1, MAX_VLAN_ID,
	    "Clear vLAN statistics for a particular vLAN ID.");

KEYWORD	   (vlan_clear_kwd, particular_vlan, ALTERNATE,
	    "vlan","Clear vLAN statistical information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	vlan_clear_kwd
