/* $Id: cfg_line_logsync.h,v 3.2.62.1 1996/08/12 16:04:57 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_line_logsync.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ L O G S Y N C . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_logsync.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:57  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:45:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * [no] logging synchronous [level x|all] [limit y]
 *
 * OBJ(int,1) = TRUE if level x
 * OBJ(int,2) = x   (-1 for "level all")
 * OBJ(int,3) = TRUE if limit y
 * OBJ(int,4) = y
 */
pdecl(cl_logsync_level);

EOLNS	  (cl_logsync_eol, log_sync_command);
DECIMAL   (cl_logsync_qsize, cl_logsync_level, no_alt,
	   OBJ(int,4), 0, 2147483647, "Number of buffers");
KEYWORD_ID(cl_logsync_limit, cl_logsync_qsize, cl_logsync_eol,
	   OBJ(int,3), TRUE, "limit", "Messages queue size", PRIV_CONF);
KEYWORD_ID(cl_logsync_levelall, cl_logsync_level, no_alt,
	   OBJ(int,2), -1, "all", 
	   "Turn off all severity level for asynchronous output", PRIV_CONF);
DECIMAL   (cl_logsync_lnum, cl_logsync_level, cl_logsync_levelall,
	   OBJ(int,2), LOGSYNC_SEVER_START, LOGSYNC_SEVER_END,
	   "Severity level; lower number for higher severity");
KEYWORD_ID(cl_logsync_level, cl_logsync_lnum, cl_logsync_limit,
	   OBJ(int,1), TRUE, "level", 
	   "Severity level to output asynchronously", PRIV_CONF);
NOPREFIX  (cl_logsync_noprefix, cl_logsync_level, cl_logsync_eol);
NVGENNS	  (cl_logsync_nvgen, cl_logsync_noprefix, log_sync_command);
KEYWORD   (cl_logsync_sync, cl_logsync_nvgen, no_alt,
	   "synchronous", "Synchronized message output", PRIV_CONF);
KEYWORD	  (cl_logsync, cl_logsync_sync, ALTERNATE,
	   "logging", "Modify message logging facilities", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	cl_logsync
