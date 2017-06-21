/* $Id: exec_show_env.h,v 3.2.60.1 1996/08/12 16:01:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/exec_show_env.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ E N V . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_env.h,v $
 * Revision 3.2.60.1  1996/08/12  16:01:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:15:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:23:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show env [all | last | table]
 * 
 * OBJ(int,1) =	code indicating which keyword of "all","last","table" 
 *		or none was supplied
 */

EOLNS	(show_env_eol, show_env);
KEYWORD_ID(show_env_table, show_env_eol, show_env_eol,
	   OBJ(int,1), SHOW_ENV_TABLE,
	   "table", "Temperature and voltage ranges", PRIV_USER );
KEYWORD_ID(show_env_last, show_env_eol, show_env_table,
	   OBJ(int,1), SHOW_ENV_LAST,
	   "last", "Last environmental monitor parameters", PRIV_USER );
KEYWORD_ID(show_env_all, show_env_eol, show_env_last,
	   OBJ(int,1), SHOW_ENV_ALL,
	   "all", "All environmental monitor parameters", PRIV_USER );
KEYWORD_ID(show_env_kwd, show_env_all, ALTERNATE,
	   OBJ(int,1), SHOW_ENV_DEFAULT,
	   "environment", "Environmental monitor statistics",
	   PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_env_kwd
