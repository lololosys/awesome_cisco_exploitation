/* $Id: cfg_line_exec-time.h,v 3.2.62.1 1996/06/12 01:02:57 billw Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_line_exec-time.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ E X E C - T I M E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_exec-time.h,v $
 * Revision 3.2.62.1  1996/06/12  01:02:57  billw
 * CSCdi59173:  exec-timeout cannot be >= 35792
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:44:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:20:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/09  22:42:00  widmer
 * CSCdi41336:  session-timeout command has invalid range
 *
 * Revision 2.1  1995/06/07  22:09:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * exec-timeout <minutes> [<seconds>]
 * no exec-timeout [<minutes> [<seconds>]]
 *
 * OBJ(int,1) = <minutes>
 * OBJ(int,2) = <seconds>
 */
EOLS	(cl_exectime_eol, line_command, LINE_EXECTIMEOUT);

NUMBER	(cl_exectime_sec, cl_exectime_eol, cl_exectime_eol,
	 OBJ(int,2), 0, MAXINT/ONESEC, "Timeout in seconds");

NUMBER	(cl_exectime_min, cl_exectime_sec, no_alt,
	 OBJ(int,1), 0, MAXINT/ONEMIN, "Timeout in minutes");

NOPREFIX (cl_exectime_noprefix, cl_exectime_min, cl_exectime_eol);
NVGENS	(cl_exectime_nvgen, cl_exectime_noprefix, line_command, LINE_EXECTIMEOUT);

KEYWORD	(cl_exectime_kwd, cl_exectime_nvgen, NONE,
	 "exec-timeout", "Set the EXEC timeout", PRIV_CONF);
TEST_MULTIPLE_FUNCS(cl_exectime, cl_exectime_kwd, NONE, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	cl_exectime
