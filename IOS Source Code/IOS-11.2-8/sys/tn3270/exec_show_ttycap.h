/* $Id: exec_show_ttycap.h,v 3.3.20.1 1996/08/12 16:08:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/exec_show_ttycap.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ T T Y C A P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_ttycap.h,v $
 * Revision 3.3.20.1  1996/08/12  16:08:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:57  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:51:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:26:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ttycap [{ all | <ttycap-name> }]
 * 
 * OBJ(int,1) =	TRUE = Keyword 'all' was parsed
 *		FALSE = No keyword 'all' was seen
 * OBJ(int,2) =	TRUE = A ttycap-name was given
 * 		FALSE = No ttycap-name was given
 * OBJ(string,1) = <ttycap-name>
 */
EOLNS	(show_ttycap_eol, show_ttycap);
SET	(show_ttycap_set, show_ttycap_eol, OBJ(int,2), TRUE);
STRING	(show_ttycap_name, show_ttycap_set, NONE,
	OBJ(string,1), "TTYcap entry to display");
TEST_MULTIPLE_FUNCS(show_ttycap_test, show_ttycap_name, NONE, show_ttycap_eol);
KEYWORD_ID (show_ttycap_all, show_ttycap_eol, show_ttycap_test,
	OBJ(int,1), TRUE,
	"all", "All ttycaps", PRIV_USER);
KEYWORD (show_ttycap, show_ttycap_all, ALTERNATE,
	 "ttycap", "Terminal capability tables", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_ttycap
