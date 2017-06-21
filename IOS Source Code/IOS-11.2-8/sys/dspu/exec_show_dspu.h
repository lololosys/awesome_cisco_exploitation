/* $Id: exec_show_dspu.h,v 3.2 1995/11/17 09:05:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/exec_show_dspu.h,v $
 *------------------------------------------------------------------
 * Dummy placeholder for Downstream PU (DSPU)
 *
 * April 94, Vivian Tseng
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_dspu.h,v $
 * Revision 3.2  1995/11/17  09:05:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:07  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:57  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:23:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show dspu [[pu|pool] <name>] [all]
 * 
 * OBJ(int,2) = TRUE if "all"; "show all the LU details"
 * OBJ(string,1) = <pu_name>
 */

EOLS	(show_dspu_eol, dspu_show_command, SHOW_DSPU);

KEYWORD_ID  (show_dspu_all, show_dspu_eol, show_dspu_eol,
        OBJ(int, 2), TRUE, "all", "Show DSPU ALL Details Command",PRIV_USER);
STRING	(show_dspu_pool_name, show_dspu_all, show_dspu_eol,
	 OBJ(string, 1), "Show DSPU Pool Name");
STRING	(show_dspu_pu_name, show_dspu_all, show_dspu_eol,
	 OBJ(string, 1), "Show DSPU PU Name");
KEYWORD_ID (show_dspu_pool, show_dspu_pool_name, show_dspu_eol,
	 OBJ(int, 3), TRUE,"pool","Show DSPU POOL Name command",PRIV_USER);
KEYWORD_ID (show_dspu_pu, show_dspu_pu_name, show_dspu_pool,
	 OBJ(int, 1), TRUE,"pu","Show DSPU PU Name command",PRIV_USER);

/******************************************************************/
KEYWORD (show_dspu, show_dspu_pu, ALTERNATE,
	 "dspu", "Display DSPU information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_dspu
