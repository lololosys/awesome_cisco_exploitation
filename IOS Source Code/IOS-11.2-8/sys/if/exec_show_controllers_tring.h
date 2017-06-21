/* $Id: exec_show_controllers_tring.h,v 3.2.60.1 1996/08/12 16:02:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_controllers_tring.h,v $
 *------------------------------------------------------------------
 * Token Ring show controllers commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_tring.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:27:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:25  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************
 * show controllers token [<unit number>]
 *
 */
EOLNS	(show_control_token_eol, show_controllers);
UNIT_NUMBER(show_control_token_num, show_control_token_eol,
	    show_control_token_eol,
	    OBJ(idb,1), IFTYPE_TOKENRING);
KEYWORD_ID(show_control_token_kwd, show_control_token_num, NONE,
	   OBJ(int,1), IDB_TR,
	   "tokenring","Show TokenRing controllers", PRIV_USER);
TEST_INT(show_control_token, show_control_token_kwd, NONE, ALTERNATE,
	 nTRs);

#undef	ALTERNATE
#define	ALTERNATE	show_control_token
