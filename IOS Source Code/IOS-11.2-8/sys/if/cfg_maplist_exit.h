/* $Id: cfg_maplist_exit.h,v 3.2.60.1 1996/08/12 16:02:22 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_maplist_exit.h,v $
 *------------------------------------------------------------------
 * map-list exit command
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_maplist_exit.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:22  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:26:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * exit - exit from map list configuration command
 * 
 */

EOLNS	(cml_exit_eol, maplist_exit_command);


KEYWORD	(cml_exit_kwd, cml_exit_eol, NONE,
         "exit-class", "Exit from static map class configuration mode", 
	 PRIV_MIN);
TESTVAR	(cml_exit, cml_exit_kwd, NONE, NONE, NONE, ALTERNATE, sense, TRUE);

#undef  ALTERNATE
#define ALTERNATE       cml_exit
