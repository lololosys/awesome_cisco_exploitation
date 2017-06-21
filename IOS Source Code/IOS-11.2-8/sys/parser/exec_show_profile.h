/* $Id: exec_show_profile.h,v 3.3.20.1 1996/08/12 16:06:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_profile.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ P R O F I L E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_profile.h,v $
 * Revision 3.3.20.1  1996/08/12  16:06:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:26  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:47:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:25:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show profile {detail | terse}
 *
 * OBJ(int,1) = TRUE if "detail" present
 * OBJ(int,2) = TRUE if "terse" present
 */

EOLNS	(show_profile_eol, show_profile_command);
KEYWORD_ID (show_profile_terse, show_profile_eol, show_profile_eol,
	    OBJ(int,2), TRUE, "terse", NULL, PRIV_USER | PRIV_HIDDEN);
KEYWORD_ID (show_profile_detail, show_profile_eol, show_profile_terse,
	    OBJ(int,1), TRUE, "detail", NULL, PRIV_USER | PRIV_HIDDEN);
KEYWORD (show_command_show_profile, show_profile_detail, ALTERNATE,
	 "profile", NULL, PRIV_USER | PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_command_show_profile
