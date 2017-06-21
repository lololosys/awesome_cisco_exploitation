/* $Id: cfg_line_modem.h,v 3.5.18.1 1996/08/12 16:04:58 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_line_modem.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ M O D E M . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_modem.h,v $
 * Revision 3.5.18.1  1996/08/12  16:04:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5  1996/02/07  19:09:16  tkolar
 * CSCdi48318:  modem InOut is not displayed in config file
 * Undo CSCdi45325.
 *
 * Revision 3.4  1996/02/01  23:34:10  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/11/20  10:21:18  billw
 * CSCdi42491:  Transition modem ri-is-cd to modem dialin
 *
 * Revision 3.2  1995/11/17  18:45:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:18:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/02  10:50:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************/
LINK_TRANS(line_modem_extend_here, no_alt);

/***************************************************************
 *  [no] modem callin
 *  [no] modem callout
 *  [no] modem cts-required
 *  [no] modem DTR-Active
 *  [no] modem RI-is-CD
 *  [no] modem InOut
 *  [no] modem printer
 *  [no] modem host
 *
 */

EOLS	(line_modem_eol, modem_command, PARSER_LINE_MODEM);
KEYWORD_ID(modem_host, line_modem_eol, line_modem_extend_here,
	   OBJ(int,9), MODEMTYPE_HOST,
	   "Host", "Devices that expect an incoming modem call",
	   PRIV_CONF);
KEYWORD_ID(modem_printer, line_modem_eol, modem_host,
	   OBJ(int,9), MODEMTYPE_PRINTER,
	   "Printer", "Devices that require DSR/CD active",
	   PRIV_CONF);
KEYWORD_ID(modem_inout, line_modem_eol, modem_printer,
	   OBJ(int,10), MODEMINOUT,
	   "InOut", "Configure line for incoming AND outgoing use of modem",
	   PRIV_CONF);
KEYWORD_ID(modem_ri_is_cd, line_modem_eol, modem_inout,
	   OBJ(int,10), MODEMRI,
	   "ri-is-cd",
	   "Configure line for a modern dial-in modem",
	   PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN);
KEYWORD_ID(modem_dialin, line_modem_eol, modem_ri_is_cd,
	   OBJ(int,10), MODEMRI,
	   "Dialin",
	   "Configure line for a modern dial-in modem",
	   PRIV_CONF);
KEYWORD_ID(modem_dtr_active, line_modem_eol, modem_dialin,
	   OBJ(int,10), MODEMDTR,
	   "DTR-active",
	   "Leave DTR low unless line has an "
	   "active incoming connection or EXEC",
	   PRIV_CONF);
KEYWORD_ID(modem_cts_required, line_modem_eol, modem_dtr_active,
	   OBJ(int,10), MODEMCTS,
	   "cts-required", "Require CTS as modem signal (obsolete)",
	   PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID(modem_callout, line_modem_eol, modem_cts_required,
	   OBJ(int,10), MODEMOUT,
	   "callout", "Simulate MODEM using CTS as modem signal (obsolete)",
	   PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID(modem_callin, line_modem_eol, modem_callout,
	   OBJ(int,10), MODEMIN,
	   "callin", "Configure line for an obsolete style dial-in modem",
	   PRIV_CONF|PRIV_HIDDEN);
SET(modem_types, modem_callin, OBJ(int,9), 0);

/***************************************************************
 * [no] modem answer-timeout <number>
 *
 */
EOLNS	(modem_answer_timeout_eol, modem_answer_timeout);
NUMBER	(modem_answer_timeout_time, modem_answer_timeout_eol, no_alt,
	 OBJ(int,2), 0, MAXUINT, "Answer timeout in seconds");
HELP	(modem_answer_timeout_nohelp, modem_answer_timeout_eol,
	 "Remove modem control from line");
TESTVAR	(modem_answer_timeout_noprefix, modem_answer_timeout_nohelp,
	 modem_answer_timeout_time, NONE, NONE, NONE, sense, FALSE);
NVGENNS	(modem_answer_timeout_nvgen, modem_answer_timeout_noprefix,
	 modem_answer_timeout);
KEYWORD_ID(modem_answer_timeout, modem_answer_timeout_nvgen, modem_types,
	   OBJ(int,10), PARSER_MODEM_ANSWER,
	   "answer-timeout",
	   "Set interval between the time the server raises DTR"
	   " in response to RING and the modem responds to CTS",
	   PRIV_CONF);

KEYWORD	(line_modem, modem_answer_timeout, ALTERNATE,
	 "modem", "Configure the Modem Control Lines", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	line_modem
