/* $Id: cfg_line_sessiontimeout.h,v 3.2.62.1 1996/08/06 00:32:01 billw Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_line_sessiontimeout.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ S E S S I O N T I M E O U T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_sessiontimeout.h,v $
 * Revision 3.2.62.1  1996/08/06  00:32:01  billw
 * CSCdi60377:  session-timeout cmd fails for values within 2nd half of
 * interval
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:44:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:20:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/09  22:42:06  widmer
 * CSCdi41336:  session-timeout command has invalid range
 *
 * Revision 2.1  1995/06/07  22:10:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *
 *
 */
EOLS	(line_sessiontimeout_eol, line_command,
	 PARSER_LINE_SESSION_TIMEOUT);
KEYWORD_ID(sessiontimeout_output, line_sessiontimeout_eol,
	   line_sessiontimeout_eol,
	   OBJ(int,2), TRUE,
	   "output", "Include output traffic as well as input traffic",
	   PRIV_CONF);
NUMBER	(sessiontimeout_num, sessiontimeout_output, no_alt,
	 OBJ(int,1), 0, MAXINT/ONEMIN,
	 "Session timeout interval in minutes");
NOPREFIX(sessiontimeout_noprefix, sessiontimeout_num, line_sessiontimeout_eol);
NVGENS	(sessiontimeout_nvgen, sessiontimeout_noprefix,
	 line_command, PARSER_LINE_SESSION_TIMEOUT);
KEYWORD	(line_sessiontimeout, sessiontimeout_nvgen, ALTERNATE,
	 "session-timeout",
	 "Set interval for closing connection when there is no input traffic",
	 PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	line_sessiontimeout
