/* $Id: exec_show_xremote.h,v 3.3.20.1 1996/08/12 16:10:10 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xremote/exec_show_xremote.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ X R E M O T E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_xremote.h,v $
 * Revision 3.3.20.1  1996/08/12  16:10:10  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:35:17  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  19:26:31  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:43  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:26:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show xremote [line <line>]
 * 
 * OBJ(int,1) =	TRUE if "line"
 *		FALSE if no "line"
 * OBJ(int,2) = <line>
 */

EOLNS	(show_xremote_eol, show_xremote);

NUMBER (show_xremote_line_num, show_xremote_eol, no_alt,
	OBJ(int,2), 0, 256, "Line number");
KEYWORD_ID(show_xremote_line, show_xremote_line_num, show_xremote_eol,
	   OBJ(int,1), TRUE,
	   "line", "Statistics for a specific line", PRIV_USER);
KEYWORD (show_xremote, show_xremote_line, NONE,
	 "xremote", "XRemote statistics", PRIV_USER|PRIV_USER_HIDDEN);


TEST_BOOLEAN(show_xremote_test, show_xremote, NONE, ALTERNATE, xremote_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_xremote_test
