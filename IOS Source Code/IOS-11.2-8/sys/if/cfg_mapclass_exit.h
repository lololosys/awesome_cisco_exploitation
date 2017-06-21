/* $Id: cfg_mapclass_exit.h,v 3.2.60.1 1996/08/12 16:02:21 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_mapclass_exit.h,v $
 *------------------------------------------------------------------
 * map-class exit command
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_mapclass_exit.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:21  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:26:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * exit - exit from map class configuration command
 * 
 */

EOLNS	(cmc_exit_eol, mapclass_exit_command);


KEYWORD	(cmc_exit_kwd, cmc_exit_eol, NONE,
         "exit-class", "Exit from static map class configuration mode", 
	 PRIV_MIN);
TESTVAR	(cmc_exit, cmc_exit_kwd, NONE, NONE, NONE, ALTERNATE, sense, TRUE);

#undef  ALTERNATE
#define ALTERNATE       cmc_exit
