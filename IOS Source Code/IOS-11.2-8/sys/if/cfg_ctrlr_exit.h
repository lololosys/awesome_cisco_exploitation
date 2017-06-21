/* $Id: cfg_ctrlr_exit.h,v 3.2.60.1 1996/08/12 16:02:06 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_ctrlr_exit.h,v $
 *------------------------------------------------------------------
 * C F G _ C T R L R _ E X I T . H
 * 
 * exit - exit from controller configuration mode
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ctrlr_exit.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:06  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:25:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:02:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(exit_cc_eol, exit_config_submode);
KEYWORD	(exit_cc_kwd, exit_cc_eol, NONE, "exit", 
	 "Exit from controller configuration mode", PRIV_MIN);
TESTVAR	(exit_cc, NONE, exit_cc_kwd, NONE, NONE, ALTERNATE, sense, FALSE);

#undef	ALTERNATE
#define	ALTERNATE	exit_cc
