/* $Id: exec_if_console.h,v 3.2.40.1 1996/08/12 16:01:29 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_if_console.h,v $
 *------------------------------------------------------------------
 * E X E C _ I F _ C O N S O L E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_if_console.h,v $
 * Revision 3.2.40.1  1996/08/12  16:01:29  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1996/02/01  23:33:15  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.1  1996/02/01  04:58:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:46:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:21:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * if-console [<slot>] [console|debug]
 */
EOLNS	(exec_if_console_eol, dbus_io_console);
KEYWORD_ID(exec_if_console_debug, exec_if_console_eol, exec_if_console_eol,
	   OBJ(int,3), DBUS_IO_DEBUG,
	   "debug", NULL, PRIV_OPR | PRIV_HIDDEN);
KEYWORD_ID(exec_if_console_console, exec_if_console_eol, exec_if_console_debug,
	   OBJ(int,3), DBUS_IO_CONSOLE,
	   "console", NULL, PRIV_OPR | PRIV_HIDDEN);
SET     (exec_if_console_slot_set, exec_if_console_console, OBJ(int,2), 1);
NUMBER  (exec_if_console_slot, exec_if_console_slot_set, exec_if_console_eol,
	 OBJ(int,1), 0, 15, NULL);
KEYWORD_MM(exec_if_console, exec_if_console_slot, ALTERNATE,
	   "if-console", NULL, PRIV_OPR | PRIV_HIDDEN, 6);

#undef	ALTERNATE
#define	ALTERNATE	exec_if_console
