/* $Id: exec_show_privilege.h,v 3.2.62.2 1996/08/12 16:06:23 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_privilege.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ P R I V I L E G E . H
 *
 * Show privilege commands
 *
 * November 1994, Robert Widmer
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_privilege.h,v $
 * Revision 3.2.62.2  1996/08/12  16:06:23  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/06/05  18:57:24  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:47:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:25:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(show_priv_eol, show_priv_command);
KEYWORD	(show_priv, show_priv_eol, ALTERNATE,
	 "privilege", "Show current privilege level",
	 PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_priv
