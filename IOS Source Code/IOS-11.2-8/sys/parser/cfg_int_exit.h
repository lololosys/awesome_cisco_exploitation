/* $Id: cfg_int_exit.h,v 3.2.62.1 1996/08/12 16:05:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_exit.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ E X I T . H
 * 
 * exit - exit from interface configuration mode
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_exit.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:38  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:43:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:04:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(exit_ci_eol, exit_config_submode);
KEYWORD	(exit_ci_kwd, exit_ci_eol, NONE, "exit", 
	 "Exit from interface configuration mode", PRIV_MIN|PRIV_SUBIF);
TESTVAR	(exit_ci, NONE, exit_ci_kwd, NONE, NONE, ALTERNATE, sense, FALSE);

#undef	ALTERNATE
#define	ALTERNATE	exit_ci
