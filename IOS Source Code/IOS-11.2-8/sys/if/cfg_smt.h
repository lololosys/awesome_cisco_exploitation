/* $Id: cfg_smt.h,v 3.2.60.1 1996/08/12 16:02:24 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_smt.h,v $
 *------------------------------------------------------------------
 * C F G _ S M T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_smt.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:26:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:14  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:13:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] smt-queue-threshold [<num>]
 */
EOLNS	(config_smt_eol, smt_threshold_command);
SET	(config_smt_number_set, config_smt_eol, OBJ(int,2), TRUE);
NUMBER	(config_smt_number, config_smt_number_set, config_smt_eol,
	 OBJ(int,1), 0, MAXUINT, "Max number of unprocessed SMT frames");
NOPREFIX(config_smt_noprefix, config_smt_number, config_smt_eol);
NVGENNS	(config_smt_nvgen, config_smt_noprefix, smt_threshold_command);
KEYWORD	(config_smt, config_smt_nvgen, NONE,
	 "smt-queue-threshold",
	 "Set the max number of unprocessed SMT frames",
	 PRIV_CONF);

TEST_INT(config_smt_test, config_smt, NONE, ALTERNATE, nfddinets);

#undef	ALTERNATE
#define	ALTERNATE	config_smt_test
