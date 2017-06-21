/* $Id: exec_show_ip_access.h,v 3.2.60.2 1996/08/12 16:02:57 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_ip_access.h,v $
 *------------------------------------------------------------------
 * exec_show_ip_access.h -- Show IP access lists
 *
 * July 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_access.h,v $
 * Revision 3.2.60.2  1996/08/12  16:02:57  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/05/04  01:01:53  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.2  1995/11/17  09:32:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:44  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:24:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ip access-lists
 * 
 * OBJ(int, 1) = access list number (0 == ALL)
 */
EOLNS	(show_ip_access_lists_eol, show_ip_access);

STRING  (show_ip_alist_name, show_ip_access_lists_eol,
	 show_ip_access_lists_eol, OBJ(string, 1), "Access list name");

TEST_MULTIPLE_FUNCS(show_ip_alist_stest, show_ip_alist_name, NONE,
		    no_alt);

NUMBER	(show_ip_alist, show_ip_access_lists_eol, show_ip_alist_stest,
	 OBJ(int, 1), MINFASTACC, MAXSLOWACC,
	 "Access list number");

KEYWORD (show_ip_access, show_ip_alist, ALTERNATE,
	 "access-lists", "List IP access lists", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_access
