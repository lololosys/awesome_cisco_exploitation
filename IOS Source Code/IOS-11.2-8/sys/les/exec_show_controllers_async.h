/* $Id: exec_show_controllers_async.h,v 3.1.6.2 1996/08/12 16:04:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_controllers_async.h,v $
 *------------------------------------------------------------------
 * Parse chain for "show controllers async" for low-end systems.
 * The cd2430 has its own way of implementing "show controllers async"
 * and its parse chain is in ts/exec_show_controllers_async.h.
 *
 * March, 1996, David Coli
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_async.h,v $
 * Revision 3.1.6.2  1996/08/12  16:04:31  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.6.1  1996/06/12  19:56:08  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.1  1996/03/20  01:28:59  jng
 * Initial Async Catwoman placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show controllers async [<unit>]
 *
 */
EOLNS	(show_control_async_eol, show_controllers);
UNIT_NUMBER(show_control_async_unit, show_control_async_eol,
	    show_control_async_eol,
	    OBJ(idb,1),  IFTYPE_ASYNC);
KEYWORD_ID(show_control_async_kwd, show_control_async_unit, NONE,
	   OBJ(int,1), IDB_ASYNC,
	   "async", "Show async controllers", PRIV_USER);
TEST_INT(show_controllers_async, show_control_async_kwd, NONE, ALTERNATE,
	 nttylines);

#undef	ALTERNATE
#define	ALTERNATE	show_controllers_async
