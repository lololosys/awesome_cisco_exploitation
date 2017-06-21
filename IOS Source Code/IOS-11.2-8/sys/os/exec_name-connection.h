/* $Id: exec_name-connection.h,v 3.3.46.1 1996/08/12 16:05:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_name-connection.h,v $
 *------------------------------------------------------------------
 * E X E C _ N A M E _ C O N N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_name-connection.h,v $
 * Revision 3.3.46.1  1996/08/12  16:05:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/15  22:28:02  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  18:46:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:21:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * name-connection
 */
EOLNS	(exec_nameconn_eol, name_command);
KEYWORD	(exec_nameconn, exec_nameconn_eol, ALTERNATE,
	 "name-connection", "Name an existing network connection",
	 PRIV_USER | PRIV_INTERACTIVE);

#undef	ALTERNATE
#define	ALTERNATE	exec_nameconn
