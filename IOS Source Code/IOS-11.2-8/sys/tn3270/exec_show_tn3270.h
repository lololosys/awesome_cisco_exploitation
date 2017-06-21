/* $Id: exec_show_tn3270.h,v 3.3.20.2 1996/08/12 16:08:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/exec_show_tn3270.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ T N 3 2 7 0 . H
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_tn3270.h,v $
 * Revision 3.3.20.2  1996/08/12  16:08:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/07/12  16:32:48  widmer
 * CSCdi62720:  Disable show tn3270 ascii-hexval from http server
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/01  23:34:40  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:50:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:26:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show tn3270 character-map [{ all | <ebcdic character (in hex)> }]
 * show tn3270 ascii-keyval <press key>
 *
 * OBJ(int,1) =	EBCDIC character to show mapping for
 * OBJ(int,2) =	TRUE = Keyword 'all' was parsed
 *	      = FALSE = No keyword 'all' was seen
 * OBJ(int,3) = TN3270_SHOW_CHARMAP  = show ebcdic mapping
 *            = TN3270_SHOW_ASCIIVAL = show ascii keyval
 */

EOLNS	(show_tn3270_eol, tn3270_show_command);

OPT_HEXDIGIT (show_charmap_ebc, show_tn3270_eol, no_alt,
	      OBJ(int, 1), MIN_EBCDIC, MAX_EBCDIC,
	      "hexadecimal value of EBCDIC character");

KEYWORD_ID_MM(show_charmap_all, show_tn3270_eol, show_charmap_ebc,
	      OBJ(int,2), TRUE,
	      "all", "All non-standard character mappings", PRIV_USER, 2);

KEYWORD_ID (show_ascii_keyval, show_tn3270_eol, no_alt,
	    OBJ(int, 3), TN3270_SHOW_ASCIIVAL,
	    "ascii-hexval", "Hexadecimal value of ASCII key",
	    PRIV_USER|PRIV_INTERACTIVE);
KEYWORD_ID (show_charmap, show_charmap_all, show_ascii_keyval, OBJ(int, 3),
	    TN3270_SHOW_CHARMAP, "character-map",
	    "EBCDIC to ASCII mappings", PRIV_USER);
KEYWORD (exec_show_tn3270, show_charmap, ALTERNATE,
	 "tn3270", "TN3270 settings", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	exec_show_tn3270
