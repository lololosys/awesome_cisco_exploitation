/* $Id: exec_clear_controller.h,v 3.2.60.1 1996/08/12 16:02:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_clear_controller.h,v $
 *------------------------------------------------------------------
 * Clear controller command
 *
 * December 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_controller.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:27:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:18  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:15:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(clear_controller_eol, clear_controller_command);
CONTROLLER_COMMAND(clear_controller_controller, clear_controller_eol, no_alt);
KEYWORD	(clear_controller, clear_controller_controller, ALTERNATE,
	 "controller", "Clear controller", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	clear_controller
