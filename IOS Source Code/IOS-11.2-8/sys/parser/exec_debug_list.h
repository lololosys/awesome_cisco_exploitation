/* $Id: exec_debug_list.h,v 3.3.28.1 1996/08/12 16:06:08 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_debug_list.h,v $
 *------------------------------------------------------------------
 * debug list command
 *
 * October 1994, Jacob Teplitsky
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_list.h,v $
 * Revision 3.3.28.1  1996/08/12  16:06:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/18  02:14:17  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:46:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:48:19  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug list
 *
 */
EOLNS	(debug_list_eol, debug_list_command);
NUMBER  (debug_list, debug_list_eol, debug_list_eol,
	 OBJ(int,1), 0,  MAXACCESSGROUP,
         "access list");
INTERFACE(debug_list_idb, debug_list, debug_list,
 	  OBJ(idb,1), IFTYPE_HWIDB     );
KEYWORD (debug_list_kwd, debug_list_idb, ALTERNATE,
 	 "list", "Set interface or/and access list for the next debug command",
 	 PRIV_OPR);
 
#undef	ALTERNATE
#define	ALTERNATE	debug_list_kwd
