/* $Id: exec_clear_isis.h,v 3.1.70.1 1996/08/12 16:00:42 widmer Exp $
 * $Source: /release/112/cvs/Xsys/clns/exec_clear_isis.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ I S I S . H
 *
 * October 1995, Dave Katz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_isis.h,v $
 * Revision 3.1.70.1  1996/08/12  16:00:42  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1995/11/09  11:08:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/21  19:02:56  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear isis [tag] *
 *
 * OBJ(string,1) = tag, or NULL
 */

EOLNS	(clear_isis_star_eol, isis_clear_all);
KEYWORD (clear_isis_star2, clear_isis_star_eol, no_alt, "*",
	 "Clear all ISIS data structures", PRIV_OPR);
STRING	(clear_isis_tag, clear_isis_star2, no_alt, OBJ(string,1),
	 "ISIS process tag");
TEST_MULTIPLE_FUNCS(clear_isis_ambig, clear_isis_tag, NONE, NONE);
KEYWORD (clear_isis_star, clear_isis_star_eol, clear_isis_ambig, "*",
	 "Clear all ISIS data structures", PRIV_OPR);

KEYWORD (clear_isis, clear_isis_star, ALTERNATE, "isis",
	 "Clear IS-IS data structures", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE	clear_isis
