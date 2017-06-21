/* $Id: exec_show_access-lists.h,v 3.2.62.2 1996/08/12 16:05:06 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_access-lists.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ A C C E S S - L I S T S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_access-lists.h,v $
 * Revision 3.2.62.2  1996/08/12  16:05:06  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/05/04  01:34:31  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the default function return_one.
 * - Initialize the default function for tbridge_cmf to return_one.
 *
 * Name Access List
 *
 * Revision 3.2  1995/11/17  18:46:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:21:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show access-lists
 * 
 */
EOLNS	(show_access_lists_eol, show_access);
STRING  (show_access_lists_name, show_access_lists_eol,
	 show_access_lists_eol, OBJ(string,1), "ACL name");

TEST_MULTIPLE_FUNCS(show_access_lists_stest, show_access_lists_name, NONE,
		    no_alt);

NUMBER  (show_access_lists_num, show_access_lists_eol,
	 show_access_lists_stest, OBJ(int,1), 1, MAXACCESSGROUP,
	 "ACL number"); 
KEYWORD (show_access_lists, show_access_lists_num, ALTERNATE,
	 "access-lists", "List access lists", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_access_lists
