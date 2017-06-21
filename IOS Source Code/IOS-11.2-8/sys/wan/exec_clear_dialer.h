/* $Id: exec_clear_dialer.h,v 3.3.20.2 1996/08/12 16:09:36 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_clear_dialer.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ D I A L E R . H
 *
 * November 1992, Lionel Bustini
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *
 * New command to clear dialer statistics
 *------------------------------------------------------------------
 * $Log: exec_clear_dialer.h,v $
 * Revision 3.3.20.2  1996/08/12  16:09:36  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/05/21  10:09:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/01/29  07:10:15  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:01:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:35:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/02  01:46:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:15:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear dialer [ interface <interface-name>]
 *
 * OBJ(idb,1) = <interface-name>
 */
EOLNS	(clear_dialer_eol, clear_dialer);
INTERFACE_KEYWORD (clear_dialer_if, clear_dialer_eol, clear_dialer_eol,
	   OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_BRI | IFTYPE_ASYNC,
	common_str_interface, "Clear dialer information on one interface");
KEYWORD (clear_dialer, clear_dialer_if, ALTERNATE,
         "dialer", "Clear dialer statistics ", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_dialer
