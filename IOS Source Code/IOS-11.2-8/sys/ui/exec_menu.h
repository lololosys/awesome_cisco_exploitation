/* $Id: exec_menu.h,v 3.1.40.2 1996/08/15 00:11:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_menu.h,v $
 *------------------------------------------------------------------
 * exec_menu.h - the "menu <name>" command
 * 
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_menu.h,v $
 * Revision 3.1.40.2  1996/08/15  00:11:25  widmer
 * CSCdi60637:  http://exec/show/whoami/cr page completes after --more--
 * timeout
 * Branch: California_branch
 *
 * Revision 3.1.40.1  1996/08/12  16:08:50  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  05:14:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:17:44  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:21:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean menu_defined(void);

/******************************************************************
 * menu <name>
 * <name>	= OBJ(string,1)
 */
EOLNS	(exec_menu_eol, menu_command);

/*  name  */
STRING	 (exec_menu_name, exec_menu_eol, no_alt,
	  OBJ(string,1), "Name of menu to run");

/*  menu  */
KEYWORD	(exec_menu_command, exec_menu_name, NONE, 
	 "menu", "Start a menu-based user interface",
	 PRIV_USER|PRIV_INTERACTIVE);

TEST_FUNC(exec_menu, exec_menu_command, NONE, ALTERNATE, menu_defined);

#undef	ALTERNATE
#define	ALTERNATE	exec_menu
