/* $Id: exec_show_reload.h,v 3.1.64.1 1996/08/12 16:08:57 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_show_reload.h,v $
 *------------------------------------------------------------------
 * Parser chain for "show reload" command
 *
 * September 1995, Dave Katz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_reload.h,v $
 * Revision 3.1.64.1  1996/08/12  16:08:57  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1995/11/09  13:41:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/24  00:55:01  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(show_reload_eol, reload_show);
KEYWORD (show_reload, show_reload_eol, ALTERNATE, "reload",
	 "Scheduled reload information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define ALTERNATE	show_reload
