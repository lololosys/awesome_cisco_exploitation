/* $Id: exec_show_stun.h,v 3.4.20.1 1996/08/12 16:02:01 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ibm/exec_show_stun.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S T U N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_stun.h,v $
 * Revision 3.4.20.1  1996/08/12  16:02:01  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/02/01  23:33:25  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/29  07:08:47  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:22:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:26:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show stun
 *
 */
EOLNS	(show_stun_eol, show_stun);
SET	(show_stun_protocol_true, show_stun_eol, OBJ(int,1), TRUE);
STRING	(show_stun_protocol, show_stun_protocol_true, show_stun_eol,
	 OBJ(string,1), "STUN protocol name");
KEYWORD (show_stun_kwd, show_stun_protocol, NONE,
	 "stun", "STUN status and configuration", PRIV_USER|PRIV_USER_HIDDEN);

TEST_BOOLEAN(show_stun_test, show_stun_kwd, NONE, ALTERNATE,
	     bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	show_stun_test
