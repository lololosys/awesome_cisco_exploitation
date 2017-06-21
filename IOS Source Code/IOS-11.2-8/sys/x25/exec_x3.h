/* $Id: exec_x3.h,v 3.3.46.1 1996/08/12 16:09:49 widmer Exp $
 * $Source: /release/112/cvs/Xsys/x25/exec_x3.h,v $
 *------------------------------------------------------------------
 * E X E C _ X 3 . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_x3.h,v $
 * Revision 3.3.46.1  1996/08/12  16:09:49  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/12/15  22:28:27  widmer
 * CSCdi45685:  Need priv flag for interactive commands
 * Add Priv_Interactive to flag interactive commands
 *
 * Revision 3.2  1995/11/17  18:06:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:28:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *
 */

EOLNS	(exec_x3_eol, x3_command);

pdecl(exec_x3_pairs);
EVAL    (exec_x3_eval, exec_x3_pairs,
         (GETOBJ(string,2)[GETOBJ(int,10)++] = GETOBJ(int,4),
          GETOBJ(string,2)[GETOBJ(int,10)++] = GETOBJ(int,5)));
X3PAIR  (exec_x3_pair, exec_x3_eval, NONE,
         OBJ(int,4), OBJ(int,5), "X.3 PAD parameters and values");
TESTVAR (exec_x3_pairs, no_alt, NONE,
         exec_x3_pair, no_alt, exec_x3_eol, OBJ(int,10), PARSEBUF);

X3PAIR  (exec_x3_pair_first, exec_x3_eval, no_alt,
         OBJ(int,4), OBJ(int,5), "X.3 PAD parameters and values");

KEYWORD	(exec_x3_command, exec_x3_pair_first, NONE,
	 "x3", "Set X.3 parameters on PAD",
	 PRIV_USER | PRIV_INTERACTIVE);

TEST_BOOLEAN(exec_x3, exec_x3_command, NONE, ALTERNATE, pad_enable);


#undef	ALTERNATE
#define	ALTERNATE	exec_x3
