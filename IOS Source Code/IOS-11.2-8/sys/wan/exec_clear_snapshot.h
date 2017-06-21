/* $Id: exec_clear_snapshot.h,v 3.2.62.1 1996/08/12 16:09:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_clear_snapshot.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ S N A P S H O T . H
 *
 * August, 1994 Joel P. Bion
 *
 * Clear commands for Snapshot support.
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_snapshot.h,v $
 * Revision 3.2.62.1  1996/08/12  16:09:38  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:01:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:16:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear snapshot commands
 *
 */

/******************************************************************
 * clear snapshot quiet-period <interface>
 *
 * OBJ(idb,1)   = Interface to bring out of quiet time.
 */
EOLNS (clear_snapshot_quiet_period_eol, snapshot_clear_quiet_period);

INTERFACE (clear_snapshot_quiet_period_int, clear_snapshot_quiet_period_eol,
	   no_alt, OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD (clear_snapshot_quiet_period, clear_snapshot_quiet_period_int,
	 no_alt, SNAPSHOT_KEYWORD_QUIET_TIME,
	 "Clear quiet time, enter active time if client side", PRIV_OPR);

KEYWORD_MM (clear_snapshot_kwd, clear_snapshot_quiet_period,
			ALTERNATE, SNAPSHOT_KEYWORD, "Clear Snapshot timers",
			PRIV_OPR, SNAPSHOT_UNIQ);

#undef	ALTERNATE
#define	ALTERNATE	clear_snapshot_kwd
