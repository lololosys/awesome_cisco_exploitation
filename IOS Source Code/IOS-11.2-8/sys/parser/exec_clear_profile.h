/* $Id: exec_clear_profile.h,v 3.3.20.1 1996/08/12 16:06:03 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_clear_profile.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ P R O F I L E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_profile.h,v $
 * Revision 3.3.20.1  1996/08/12  16:06:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:09:17  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:45:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:16:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * clear profile
 *
 */
EOLNS	(clear_profile_eol, clear_profile_command);
KEYWORD (clear_profile, clear_profile_eol, ALTERNATE,
	"profile", NULL, PRIV_OPR | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	clear_profile
