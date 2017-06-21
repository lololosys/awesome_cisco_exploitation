/* $Id: exec_show_cons.h,v 3.4.20.1 1996/08/12 16:09:47 widmer Exp $
 * $Source: /release/112/cvs/Xsys/x25/exec_show_cons.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ C O N S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_cons.h,v $
 * Revision 3.4.20.1  1996/08/12  16:09:47  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/01/29  07:10:20  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/18  02:16:10  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:06:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:22:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show cmns [<interface>]
 *
 * OBJ(idb,1) = <interface>
 * XXX show cmns is only valid for serial interfaces?  Use IFTYPE_HWIDB for
 * now. 
 */

EOLNS	(show_cmns_eol, show_cmns);
INTERFACE (show_cmns_if, show_cmns_eol, show_cmns_eol, OBJ(idb,1),
	   IFTYPE_HWIDB);
KEYWORD (show_cmns_kwd, show_cmns_if, NONE,
	 "cmns", "Connection-Mode networking services (CMNS) information",
	 PRIV_USER|PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_cmns_test, show_cmns_kwd, NONE, ALTERNATE, x25_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_cmns_test
