/* $Id: exec_show_controllers.h,v 3.1.2.2 1996/08/12 16:08:54 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_show_controllers.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ C O N T R O L L E R S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_controllers.h,v $
 * Revision 3.1.2.2  1996/08/12  16:08:54  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/15  15:00:43  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.1  1996/04/12  19:08:57  widmer
 * Add placeholder files
 *
 * Revision 3.2  1995/11/17  18:47:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:22:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(show_controllers_eol, show_controllers);

LINK_TRANS(show_controllers_extend_here, show_controllers_eol);

KEYWORD (show_controllers, show_controllers_extend_here, ALTERNATE,
	 "controllers", "Interface controller status",
	 PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_controllers
