/* $Id: exec_show_controllers_e1.h,v 3.3.46.1 1996/08/12 16:02:32 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_controllers_e1.h,v $
 *------------------------------------------------------------------
 * Show controller e1 command
 *
 * 20-June-1994, Bill May (from exec_show_controllers_t1.h)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_e1.h,v $
 * Revision 3.3.46.1  1996/08/12  16:02:32  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/07  17:45:02  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  09:27:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:23  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/******************************************************************
 * show controllers e1
 *	OBJ(int,1) -- specific slot, or -1 if show all
 *	OBJ(int,2) -- specific applique, iff OBJ(int,1) != -1
 */
EOLNS	(show_control_e1_eol, show_e1_controller);

GENERAL_NUMBER	(show_control_e1_appl, show_control_e1_eol, no_alt,
		 OBJ(int,2), 0, DEFAULT_DSX1_CTRLR_INTERFACES - 1,
		 "Applique number",
		 (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |
		  NUMBER_DEC | NUMBER_WS_OK | NUMBER_NO_WS_OK |
		  NUMBER_HELP_CHECK));

KEYWORD_NOWS	(show_control_e1_slash, show_control_e1_appl, no_alt, "/",
		 "'/' followed by applique number", PRIV_USER);

GENERAL_NUMBER	(show_control_e1_slot_platform_spec, show_control_e1_slash,
		 show_control_e1_eol,
		 OBJ(int,1), 0, DEFAULT_DSX1_CTRLR_SLOTS - 1,
		 "Specific controller to show",
		 (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |
		  NUMBER_DEC | NUMBER_NO_WS_OK |
		  NUMBER_HELP_CHECK));

GENERAL_NUMBER	(show_control_e1_unit_platform_spec, show_control_e1_eol,
		 show_control_e1_eol,
		 OBJ(int,1), 0, DEFAULT_DSX1_CTRLR_UNITS,
		 "Specific controller to show",
		 (NUMBER_HEX | HEX_ZEROX_OK | NUMBER_OCT | OCT_ZERO_OK |
		  NUMBER_DEC | NUMBER_NO_WS_OK |
		  NUMBER_HELP_CHECK));

TEST_INT(show_control_e1_platform_check, show_control_e1_unit_platform_spec,
	 show_control_e1_slot_platform_spec, NONE, ctrlr_units);
	
KEYWORD_ID(show_control_e1_kwd, show_control_e1_platform_check, ALTERNATE,
	   OBJ(int,1), MAXUINT,
	   "e1", "E1 controller internal state", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_control_e1_kwd
