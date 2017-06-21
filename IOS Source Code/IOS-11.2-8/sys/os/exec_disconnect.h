/* $Id: exec_disconnect.h,v 3.3.46.1 1996/08/12 16:05:03 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_disconnect.h,v $
 *------------------------------------------------------------------
 * E X E C _ D I S C O N N E C T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_disconnect.h,v $
 * Revision 3.3.46.1  1996/08/12  16:05:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/15  22:28:01  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  18:46:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:20:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * disconnect [ <connection-number> | <connection-name> ]
 *
 * OBJ(int,1) = <connection-number>
 * OBJ(string,1) = <connection-name>
 */
EOLNS	(exec_disconn_eol, disconnect_command);

STRING	(exec_disconn_name, exec_disconn_eol, exec_disconn_eol,
	 OBJ(string,1), "The name of an active network connection");

TEST_MULTIPLE_FUNCS(exec_disconn_name_test, exec_disconn_name,
		    exec_disconn_eol, NONE);

NUMBER	(exec_disconn_num, exec_disconn_eol, exec_disconn_name_test,
	 OBJ(int,1), 1, MAX_CONNECTIONS,
	 "The number of an active network connection");

KEYWORD	(exec_disconn, exec_disconn_num, ALTERNATE,
	 "disconnect", "Disconnect an existing network connection",
	 PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_disconn
