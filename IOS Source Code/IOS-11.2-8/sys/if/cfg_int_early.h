/* $Id: cfg_int_early.h,v 3.2.60.1 1996/08/12 16:02:09 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_early.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ E A RL  Y . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_early.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:09  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:26:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:04:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] early-token-release
 *
 */
EOLNS	(ci_early_eol, tr_etr_command);
KEYWORD	(ci_early, ci_early_eol, NONE,
	 "early-token-release", "Enable early token release", PRIV_CONF);

TEST_IDBSTATUS(ci_early_test, ci_early, NONE, ALTERNATE, IDB_TR);

#undef	ALTERNATE
#define	ALTERNATE	ci_early_test
