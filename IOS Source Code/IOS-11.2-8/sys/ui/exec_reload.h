/* $Id: exec_reload.h,v 3.2.62.1 1996/08/12 16:08:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_reload.h,v $
 *------------------------------------------------------------------
 * E X E C _ R E L O A D . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_reload.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:48:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/24  00:55:01  dkatz
 * CSCdi40898:  Synchronized reload needed
 *
 * Revision 2.1  1995/07/04  01:59:29  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:21:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/parser_defs_reload.h"

/******************************************************************
 * reload {cancel | [{in [hh:]mm | at hh:mm [{Mon dd | dd Mon}] }] [text]}
 *
 * OBJ(string,1) = reload reason
 * OBJ(int,1) = RELOAD_TYPE_IMMEDIATE if no special parameters
 *		RELOAD_TYPE_IN if "in"
 *		RELOAD_TYPE_AT if "at"
 *		RELOAD_TYPE_CANCEL if "cancel"
 * OBJ(int,2) = hours (at);  hours or minutes (in) depending on (int,3)
 * OBJ(int,3) = minutes (at); minutes, or MAXUINT (in) if no hours specified
 * OBJ(int,4) = month (at) (or 0 if date not specified)
 * OBJ(int,5) = day (at)
 */
EOLNS	(exec_reload_eol, reload_command);
TEXT	(exec_reload_reason_text, exec_reload_eol, exec_reload_eol,
	 OBJ(string,1), "Reason for reload");
TEST_MULTIPLE_FUNCS (exec_reload_reason, exec_reload_reason_text, NONE,
		     no_alt);
WHITESPACE (exec_reload_in_ws, exec_reload_reason, no_alt);
SET	(exec_reload_no_colon, exec_reload_in_ws, OBJ(int,3), MAXUINT);
IDECIMAL (exec_reload_in_minutes, exec_reload_in_ws, no_alt,
	  OBJ(int,3), 0, 59);
CHARACTER (exec_reload_in_colon, exec_reload_in_minutes, exec_reload_no_colon,
	   ':');
IDECIMAL (exec_reload_in_hours, exec_reload_in_colon, no_alt,
	  OBJ(int,2), 0, MAXUINT);
HELP    (exec_reload_in_help, exec_reload_in_hours,
	 "Delay before reload (mmm or hhh:mm)");
KEYWORD_ID (exec_reload_in, exec_reload_in_help, exec_reload_reason,
	    OBJ(int,1), RELOAD_TYPE_IN,
	    "in", "Reload after a time interval", PRIV_ROOT);
NUMBER  (exec_reload_at_day_2, exec_reload_reason, no_alt,
	 OBJ(int,5), 1, 31, "Day of the month");
SPLIT   (exec_reload_at_test, exec_reload_at_day_2, exec_reload_reason,
	 OBJ(int,5), 0);
SPLIT	(exec_reload_nodate, exec_reload_reason, no_alt, OBJ(int,5), 0);
MONTH   (exec_reload_at_month, exec_reload_at_test, exec_reload_nodate,
	 OBJ(int,4), "Month of the year");
NUMBER  (exec_reload_at_day, exec_reload_at_month, exec_reload_at_month,
	 OBJ(int,5), 1, 31, "Day of the month");
TIME    (exec_reload_at_time, exec_reload_at_day, no_alt, OBJ(int,2),
	 OBJ(int,3), "Time to reload (hh:mm)");
KEYWORD_ID (exec_reload_at, exec_reload_at_time, exec_reload_in,
	    OBJ(int,1), RELOAD_TYPE_AT,
	    "at", "Reload at a specific time/date", PRIV_ROOT);
KEYWORD_ID (exec_reload_cancel, exec_reload_eol, exec_reload_at,
	    OBJ(int,1), RELOAD_TYPE_CANCEL,
	    "cancel", "Cancel pending reload", PRIV_ROOT);
KEYWORD	(exec_reload, exec_reload_cancel, ALTERNATE, 
	 "reload", "Halt and perform a cold restart", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_reload
