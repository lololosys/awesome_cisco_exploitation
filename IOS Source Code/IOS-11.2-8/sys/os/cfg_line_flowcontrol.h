/* $Id: cfg_line_flowcontrol.h,v 3.2.62.1 1996/03/27 10:44:20 irfan Exp $
 * $Source: /release/111/cvs/Xsys/os/cfg_line_flowcontrol.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ F L O W C O N T R O L . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_flowcontrol.h,v $
 * Revision 3.2.62.1  1996/03/27  10:44:20  irfan
 * CSCdi33144:  telnet should be configurable to refuse some options
 * Branch: California_branch
 * Add a bit in tty->capabilities to allow telnet/rlogin to silently
 * ignore remote host's flow control change requests.
 *
 * Revision 3.2  1995/11/17  18:45:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:09:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * flowcontrol { software | hardware | none } [ in | out ]
 *
 */
EOLS	(flowcontrol_eol, flowcontrol_command, PARSER_LINE_FLOW);

KEYWORD_ID(flowcontrol_lock, flowcontrol_eol, flowcontrol_eol,
	   OBJ(int,3), TRUE,
	   "lock", "Ignore network host requests to change flow control",
	   PRIV_USER);
SET     (flowcontrol_lock_set, flowcontrol_lock,
	 OBJ(int,3), FALSE);
TESTVAR (flowcontrol_lock_test, flowcontrol_eol, flowcontrol_lock_set,
	 NONE, NONE, NONE,
	 OBJ(int,1), FLOWCONTROL_HARDWARE);
SET   (flowcontrol_both, flowcontrol_lock_test,
       OBJ(int,2), FLOWCONTROL_BOTH);
KEYWORD_ID(flowcontrol_out, flowcontrol_lock_test, flowcontrol_both,
	   OBJ(int,2), FLOWCONTROL_OUT,
	   "out", "Send flow control information to the attached device",
	   PRIV_USER);

KEYWORD_ID(flowcontrol_in, flowcontrol_lock_test, flowcontrol_out,
	   OBJ(int,2), FLOWCONTROL_IN,
	   "in", "Listen to flow control from the attached device",
	   PRIV_USER);


NVGENS	(flowcontrol_nvgens, flowcontrol_in,
	 flowcontrol_command, PARSER_LINE_FLOW);

SET	(flowcontrol_noprefix_set, flowcontrol_both,
	 OBJ(int,1), FLOWCONTROL_NONE);
TESTVAR (flowcontrol_noprefix, NONE, flowcontrol_noprefix_set,
	 NONE, NONE, no_alt, sense, TRUE);

KEYWORD_ID(flowcontrol_none, flowcontrol_nvgens, flowcontrol_noprefix,
	   OBJ(int,1), FLOWCONTROL_NONE,
	   "NONE", "Set no flow control", PRIV_USER);
KEYWORD_ID(flowcontrol_hardware_kwd, flowcontrol_nvgens, NONE,
	   OBJ(int,1), FLOWCONTROL_HARDWARE,
	   "hardware", "Set hardware flow control", PRIV_USER);
TEST_LINE(flowcontrol_hardware, flowcontrol_hardware_kwd, NONE,
	  flowcontrol_none, CON_CONSOLE|CON_AUX|CON_TTY);
KEYWORD_ID(flowcontrol_software, flowcontrol_nvgens, flowcontrol_hardware,
	   OBJ(int,1), FLOWCONTROL_SOFTWARE,
	   "software", "Set software flow control", PRIV_USER);


KEYWORD	(line_flowcontrol, flowcontrol_software, ALTERNATE,
	 "flowcontrol", "Set the flow control", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	line_flowcontrol
