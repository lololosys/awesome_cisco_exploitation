/* $Id: exec_show_ip_as-path_access-list.h,v 3.3.10.1 1996/08/12 16:04:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_ip_as-path_access-list.h,v $
 *------------------------------------------------------------------
 * exec_show_ip_as-path_access-list.h
 *
 * February-1995, Ravi Chandra
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_as-path_access-list.h,v $
 * Revision 3.3.10.1  1996/08/12  16:04:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/24  02:01:50  pst
 * CSCdi49774:  modularize regexp's for bgp (part 3)
 *
 * Revision 3.2  1995/11/17  17:33:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:17  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:24:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip as-path <access list number>
 * 
 * OBJ(int, 1) = access list number (0 == ALL)
 */
EOLNS	(show_ip_aspath_list_eol, ip_aspath_show);
NUMBER	(show_ip_aspath_list, show_ip_aspath_list_eol, 
	 show_ip_aspath_list_eol, OBJ(int, 1), MINREGEXPACC+1, MAXREGEXPACC,
	 "AS path access list number");

KEYWORD (show_ip_aspath_access, show_ip_aspath_list, ALTERNATE,
	 "as-path-access-list", "List AS path access lists", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_aspath_access

