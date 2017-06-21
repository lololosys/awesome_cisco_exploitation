/* $Id: exec_clear_cdp.h,v 3.3.20.1 1996/08/12 16:00:36 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cdp/exec_clear_cdp.h,v $
 *------------------------------------------------------------------
 * Support for 'Clear CDP' command
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_cdp.h,v $
 * Revision 3.3.20.1  1996/08/12  16:00:36  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:21  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:49:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:47:21  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:15:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear cdp table
 *
 */
EOLNS	(cdp_clear_table_eol, cdp_clear_table);
KEYWORD	(cdp_clear_table_kwd, cdp_clear_table_eol, no_alt,
	 "table", "Clear cdp table", PRIV_OPR );

/******************************************************************
 * clear cdp counters
 *
 */
EOLNS	(cdp_clear_eol, cdp_clear_counters);
KEYWORD (cdp_clear_counter_kwd, cdp_clear_eol, cdp_clear_table_kwd, 
            "counters", "Clear CDP counters", PRIV_OPR );
KEYWORD	(cdp_clear_kwd, cdp_clear_counter_kwd, ALTERNATE,
	 "cdp", "Reset cdp information", PRIV_OPR );

#undef	ALTERNATE
#define	ALTERNATE	cdp_clear_kwd
