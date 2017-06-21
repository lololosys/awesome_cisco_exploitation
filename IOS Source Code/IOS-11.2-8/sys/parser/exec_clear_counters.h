/* $Id: exec_clear_counters.h,v 3.4.20.1 1996/08/12 16:06:01 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_clear_counters.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ C O U N T E R S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_counters.h,v $
 * Revision 3.4.20.1  1996/08/12  16:06:01  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/01/29  07:09:15  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/18  02:14:16  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:45:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:15:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear counters [<interface-name>]
 *
 * OBJ(int,1) = TRUE if <interface-name> was given
 * csb->interface = idb of specified interface
 */
EOLNS  (clear_ctrs_eol, clear_counters);
SET   (clear_ctrs_set, clear_ctrs_eol, OBJ(int,1), TRUE);
INTERFACE (clear_ctrs_name, clear_ctrs_set, clear_ctrs_eol, OBJ(idb,1),
           (IFTYPE_HWIDB & ~IFTYPE_LEX));
KEYWORD (clear_counters_kwd, clear_ctrs_name, ALTERNATE,
	 "counters", "Clear counters on one or all interfaces",
	 PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	clear_counters_kwd
