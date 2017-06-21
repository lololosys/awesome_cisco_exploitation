/* $Id: exec_clear_access-lists.h,v 3.2.62.3 1996/09/09 22:17:24 ahh Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_clear_access-lists.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ A C C E S S - L I S T S . H
 *
 * Paul Traina, March 1995
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_access-lists.h,v $
 * Revision 3.2.62.3  1996/09/09  22:17:24  ahh
 * CSCdi68478:  clear access-list counters not working
 * Remove parse-chain ambiguity and allow NULL to be passed into the
 * access_clear registry to clear the counters on all access-lists.
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/08/12  16:05:01  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/05/04  01:34:30  wilber
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
 * Revision 3.2  1995/11/17  18:46:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:15:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * clear access-list counters [list]
 * 
 */
EOLNS	(clear_acl_eol, clear_access);
STRING  (clear_acl_name, clear_acl_eol, clear_acl_eol,
	 OBJ(string,1), "Access list name");
TEST_MULTIPLE_FUNCS(clear_acl_stest, clear_acl_name, NONE, no_alt);
NUMBER	(clear_acl_num, clear_acl_eol, clear_acl_stest,
	 OBJ(int,1), 0, MAXSLOWACC, "Access list number");
KEYWORD (clear_acl_co, clear_acl_num, no_alt,
	 "counters", "Clear access list counters", PRIV_OPR);
KEYWORD (clear_acl, clear_acl_co, ALTERNATE,
	 "access-list", "Clear access list statistical information",
	 PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_acl
