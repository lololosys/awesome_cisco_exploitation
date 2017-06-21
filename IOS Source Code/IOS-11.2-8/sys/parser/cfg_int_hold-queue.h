/* $Id: cfg_int_hold-queue.h,v 3.2.62.1 1996/08/12 16:05:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_hold-queue.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ H O L D Q U E U E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_hold-queue.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:39  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:43:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:05:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	hold-queue <length> {in | out}
 *	no hold-queue {in | out}
 *
 * OBJ(int,1) = <length>
 * OBJ(int,2) = HOLDQUEUE_IN, HOLDQUEUE_OUT
 */
EOLNS	(ci_holdqueue_endline, holdqueue_command);

KEYWORD_ID (ci_holdqueue_out, ci_holdqueue_endline, no_alt,
	    OBJ(int,2), HOLDQUEUE_OUT,
	 "out", "Output queue", PRIV_CONF);

KEYWORD_ID (ci_holdqueue_in, ci_holdqueue_endline, ci_holdqueue_out,
	    OBJ(int,2), HOLDQUEUE_IN,
	 "in", "Input queue", PRIV_CONF);

TESTVAR	(ci_holdqueue_noprefix, ci_holdqueue_in, no_alt,
	 NONE, NONE, NONE, sense, FALSE);
NUMBER	(ci_holdqueue_num, ci_holdqueue_in, ci_holdqueue_noprefix,
	 OBJ(int,1), 0, 4096, "Queue length");

NVGENNS	(ci_holdqueue_nvgen, ci_holdqueue_num, holdqueue_command);
KEYWORD	(ci_holdqueue, ci_holdqueue_nvgen, ALTERNATE,
	 "hold-queue", "Set hold queue depth", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_holdqueue
