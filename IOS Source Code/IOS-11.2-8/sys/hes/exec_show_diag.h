/* $Id: exec_show_diag.h,v 3.2.60.3 1996/08/12 16:01:33 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_show_diag.h,v $
 *------------------------------------------------------------------
 * exec_show_diag.h - show diag command
 *
 * January 1993, William H. Palmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_diag.h,v $
 * Revision 3.2.60.3  1996/08/12  16:01:33  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.2  1996/08/01  20:08:30  sjacobso
 * CSCdi42087:  sho diag 31 not working for CI board in 75xx systems
 * Branch: California_branch:  Modified show diag to accept 31.
 *
 * Revision 3.2.60.1  1996/06/05  18:57:09  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:15:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/21  18:17:56  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:23:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show diagbus [ <slot> ]
 * OBJ(int,1) = <slot>, MAXUINT if no slot specified
 */
EOLNS   (show_diagbus_eol, show_diagbus);

SET    (show_diagbus_noslot, show_diagbus_eol, OBJ(int,1), MAXUINT);

NUMBER (show_diagbus_slot, show_diagbus_eol, show_diagbus_noslot,
	OBJ(int,1), 0, 31, "Slot number");

KEYWORD(show_diagbus_kwd, show_diagbus_slot, ALTERNATE,
	"diagbus", "Show diagnostic bus information",
	PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_diagbus_kwd

