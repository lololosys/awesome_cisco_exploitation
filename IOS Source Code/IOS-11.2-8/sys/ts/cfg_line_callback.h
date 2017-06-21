/* $Id: cfg_line_callback.h,v 3.2 1995/11/17 18:53:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_line_callback.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ C A L L B A C K . H
 *
 * April 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_line_callback.h,v $
 * Revision 3.2  1995/11/17  18:53:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:43  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:02  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:09:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


PARAMS_KEYONLY(cl_line_cback_nodsrwait, NONE, "nodsr-wait",
               OBJ(int,1),
               CALLBACK_DEFAULT_WAIT_NODSR, CALLBACK_MAX_WAIT_NODSR,
               callback_line_command, PARSER_CALLBACK_NODSRWAIT,
               "Extended wait time for DSR to go down",
               "Callback wait for DSR to go down in milliseconds", PRIV_CONF );
PARAMS_KEYONLY(cl_line_cback_forcewait, cl_line_cback_nodsrwait, "forced-wait",
               OBJ(int,1),
               CALLBACK_DEFAULT_FORCEDWAIT_SEC, CALLBACK_MAX_FORCEDWAIT_SEC,
               callback_line_command, PARSER_CALLBACK_FORCEDWAIT,
               "Forced wait before callback script driven to modem",
               "Callback forced wait in seconds", PRIV_CONF );
KEYWORD	(cl_line_cback_kwd, cl_line_cback_forcewait, NONE,
	 "callback", "Callback settings", PRIV_CONF);
TEST_LINE(cl_line_cback, cl_line_cback_kwd, NONE, ALTERNATE, CON_AUX|CON_TTY);

#undef	ALTERNATE
#define	ALTERNATE  cl_line_cback
