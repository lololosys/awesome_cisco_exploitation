/* $Id: exec_show_location.h,v 3.1.2.2 1996/08/12 16:05:11 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_location.h,v $
 *------------------------------------------------------------------
 * Parse chain for "show location"
 *
 * December 1995, Dave Katz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_location.h,v $
 * Revision 3.1.2.2  1996/08/12  16:05:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/16  19:09:55  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  09:39:12  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show location
 */
EOLNS	(show_location_eol, show_location);
KEYWORD (show_location, show_location_eol, ALTERNATE,
	 "location", "Display the system location", PRIV_USER);

#undef	ALTERNATE
#define ALTERNATE show_location
