/* $Id: exec_show_controllers_bri.h,v 3.2.60.1 1996/08/12 16:04:32 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_controllers_bri.h,v $
 *------------------------------------------------------------------
 * BRI show controllers commands
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_bri.h,v $
 * Revision 3.2.60.1  1996/08/12  16:04:32  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:37:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:22:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show controllers bri [<unit number>]
 *
 */
EOLNS	(show_control_bri_eol, show_controllers);
UNIT_NUMBER(show_control_bri_unit, show_control_bri_eol, show_control_bri_eol,
	    OBJ(idb,1),  IFTYPE_BRI);
KEYWORD_ID(show_control_bri_kwd, show_control_bri_unit, NONE,
	   OBJ(int,1), IDB_ISDN,
	   "bri", "Show BRI controllers", PRIV_USER);
TEST_INT(show_control_bri, show_control_bri_kwd, NONE, ALTERNATE,
	 nbrinets);

#undef	ALTERNATE
#define	ALTERNATE show_control_bri
