
/* $Id: cfg_cfgtblexit_lane.h,v 3.2.62.1 1996/08/12 16:04:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lane/cfg_cfgtblexit_lane.h,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_cfgtblexit_lane.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:49:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:18:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
** exit from lane lecs config-table command
*/

EOLNS (exit_cfgtbl_eol, lecs_cfgtblexit_function);

KEYWORD	(cfgtbl_exit_kwd, exit_cfgtbl_eol, NONE,
         "exit", "keyword to leave lane config database mode", 
	 PRIV_USER);

TESTVAR	(cfg_exit, cfgtbl_exit_kwd, NONE, NONE, NONE, ALTERNATE,
	 sense, TRUE);

#undef  ALTERNATE
#define ALTERNATE       cfg_exit

