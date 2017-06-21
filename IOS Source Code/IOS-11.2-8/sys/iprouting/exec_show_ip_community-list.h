/* $Id: exec_show_ip_community-list.h,v 3.2.60.1 1996/08/12 16:04:06 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_ip_community-list.h,v $
 *------------------------------------------------------------------
 * exec_show_ip_community-list.h
 *
 * February-1995, Ravi Chandra
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ip_community-list.h,v $
 * Revision 3.2.60.1  1996/08/12  16:04:06  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:33:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:18  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:24:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip community-list <community-list number>
 * 
 * OBJ(int, 1) = community-list number (0 == ALL)
 */
EOLNS	(show_ip_clist_eol, ip_community_show_list);

NUMBER	(show_ip_clist_num, show_ip_clist_eol, show_ip_clist_eol, 
	 OBJ(int, 1), 1, MAX_EXT_COMLIST, "Community-list number");

KEYWORD (show_ip_clist, show_ip_clist_num, ALTERNATE,
	 "community-list", "List community-list", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_clist

