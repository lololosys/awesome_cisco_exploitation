/* $Id: exec_debug_snapshot.h,v 3.4 1996/02/01 20:45:14 hampton Exp $
 * $Source: /release/111/cvs/Xsys/wan/exec_debug_snapshot.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ S N A P S H O T . H
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_snapshot.h,v $
 * Revision 3.4  1996/02/01  20:45:14  hampton
 * Commit code review comments.  [CSCdi47717]
 *
 * Revision 3.3  1996/02/01  06:11:58  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  18:01:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:48:58  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug snapshot
 *
 */
EOLS	(debug_snapshot_eol, debug_command, DEBUG_SNAPSHOT);
SET	(debug_snapshot_set, debug_snapshot_eol,
	 OBJ(pdb,1), snapshot_debug_arr);
KEYWORD_MM (debug_snapshot, debug_snapshot_set, ALTERNATE,
	    SNAPSHOT_KEYWORD, SNAPSHOT_DEBUG_HELP, PRIV_OPR,
	    SNAPSHOT_UNIQ);

#undef	ALTERNATE
#define	ALTERNATE	debug_snapshot
