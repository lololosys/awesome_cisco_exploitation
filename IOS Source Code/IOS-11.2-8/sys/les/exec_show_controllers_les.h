/* $Id: exec_show_controllers_les.h,v 3.2.60.1 1996/08/12 16:04:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_controllers_les.h,v $
 *------------------------------------------------------------------
 * LES show controllers commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_les.h,v $
 * Revision 3.2.60.1  1996/08/12  16:04:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:37:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show controllers { cache | all }
 *
 * OBJ(int,1) = SHOW_CONTROLLERS_CACHE, SHOW_CONTROLLERS_ALL
 * OBJ(idb,1) = <interface>
 */
EOLNS	(show_control_les_eol, show_les_controllers);
KEYWORD_ID(show_control_cache, show_control_les_eol, ALTERNATE,
	   OBJ(int,1), SHOW_CONTROLLERS_CACHE,
	   "cache", "Controller cache", PRIV_USER);
KEYWORD_ID(show_control_all, show_control_les_eol, show_control_cache,
	   OBJ(int,1), SHOW_CONTROLLERS_ALL,
	   "all", "All interface controllers", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_control_all
