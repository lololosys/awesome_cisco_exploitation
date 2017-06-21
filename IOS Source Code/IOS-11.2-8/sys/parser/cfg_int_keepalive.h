/* $Id: cfg_int_keepalive.h,v 3.2.62.2 1996/08/12 16:05:40 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_keepalive.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ K E E P A L I V E . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_keepalive.h,v $
 * Revision 3.2.62.2  1996/08/12  16:05:40  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/04/29  05:24:26  pst
 * Don't allow keepalive command for tunnel interfaces.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:43:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:06:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] keepalive [seconds]
 *
 * OBJ(int,1) = seconds
 */

EOLNS	(ci_keepalive_eol, keepalive_command);

NUMBER	(ci_keepalive_seconds, ci_keepalive_eol, ci_keepalive_eol,
	 OBJ(int,1), 0, 32767, "Keepalive period (default 10 seconds)");

NVGENNS	(ci_keepalive_nvgen, ci_keepalive_seconds, keepalive_command);

/* Set OBJ(int,1) to 10 for the default period. */
KEYWORD_ID (ci_keepalive, ci_keepalive_nvgen, NONE,
	    OBJ(int,1), 10, "keepalive", "Enable keepalive", PRIV_CONF);

TEST_IDBSTATUS(ci_keepalive_test, NONE, ci_keepalive, ALTERNATE,
	       IDB_TUNNEL | IDB_ATM);

#undef	ALTERNATE
#define	ALTERNATE	ci_keepalive_test
