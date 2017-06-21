/* $Id: cfg_int_squelch.h,v 3.2.62.1 1996/08/12 16:04:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/cfg_int_squelch.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S Q U E L C H . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_squelch.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:36:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:07:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * squelch { normal | reduced }
 *
 */
EOLNS	(ci_squelch_eol, lance_squelch_command);

KEYWORD_ID(ci_squelch_reduced, ci_squelch_eol, no_alt,
	   OBJ(int,1), PARSER_SQUELCH_REDUCED,
	   "reduced", "Don't enforce 100 meter limit", PRIV_CONF);
KEYWORD_ID(ci_squelch_normal, ci_squelch_eol, ci_squelch_reduced,
	   OBJ(int,1), PARSER_SQUELCH_NORMAL,
	   "normal", "Limited to 100 meters", PRIV_CONF);

KEYWORD	(ci_squelch, ci_squelch_normal, ALTERNATE,
	 "squelch", "10BaseT 100 meter limit enforcement", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	ci_squelch
