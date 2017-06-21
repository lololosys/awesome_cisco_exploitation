/* $Id: exec_show_netbios.h,v 3.3.20.1 1996/08/12 16:01:59 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ibm/exec_show_netbios.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ N E T B I O S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_netbios.h,v $
 * Revision 3.3.20.1  1996/08/12  16:01:59  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:45  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:22:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:25:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show netbios-cache
 * 
 */

EOLNS	(show_netbios_cache_eol, display_netbios_cache);
KEYWORD (show_netbios, show_netbios_cache_eol, NONE,
	 "netbios-cache", "NetBIOS name cache contents",
	 PRIV_USER|PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_netbios_test, show_netbios, NONE, ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_netbios_test
