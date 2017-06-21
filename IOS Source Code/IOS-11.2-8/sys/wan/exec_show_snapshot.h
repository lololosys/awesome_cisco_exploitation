/* $Id: exec_show_snapshot.h,v 3.6.20.1 1996/08/12 16:09:40 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_show_snapshot.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S N A P S H O T . H
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_snapshot.h,v $
 * Revision 3.6.20.1  1996/08/12  16:09:40  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.6  1996/02/01  23:35:00  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.5  1996/02/01  20:45:17  hampton
 * Commit code review comments.  [CSCdi47717]
 *
 * Revision 3.4  1996/02/01  06:12:00  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:10:18  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:02:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:25:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show snapshot [interface <interface>]
 * 
 * OBJ(idb,1) = <interface>
 * OBJ(int,1) = <private>
 *
 * The 'interface' keyword is not necessary.  The action routine
 * doesn't use it, but we must parse it for compatability.
 */

EOLNS	(show_snapshot_eol, show_snapshot);

INTERFACE (show_snapshot_if, show_snapshot_eol,
	   show_snapshot_eol, OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD_ID_MM(show_snapshot_private, show_snapshot_eol, show_snapshot_if,
	      OBJ(int,1), TRUE,
	      "private", NULL /*"Give private Snapshot information"*/,
	      PRIV_HIDDEN | PRIV_USER, 7);

KEYWORD_MM (show_snapshot, show_snapshot_private, ALTERNATE,
	    SNAPSHOT_KEYWORD, SNAPSHOT_SHOW_HELP, PRIV_USER|PRIV_USER_HIDDEN,
	    SNAPSHOT_UNIQ);

#undef	ALTERNATE
#define	ALTERNATE	show_snapshot
