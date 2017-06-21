/* $Id: exec_show_hosts.h,v 3.4.20.1 1996/08/12 16:06:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_show_hosts.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ H O S T S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_hosts.h,v $
 * Revision 3.4.20.1  1996/08/12  16:06:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/01/29  07:09:21  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/22  06:57:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:47:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:24:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show hosts [<hostname>]
 * 
 * OBJ(string,1) = <hostname>
 */
EOLNS	(show_hosts_eol, name_show);
EVAL	(show_hosts_noname, show_hosts_eol, *GETOBJ(string,1)='\0');
STRING	(show_hosts_name, show_hosts_eol, show_hosts_noname,
	OBJ(string,1), "Hostname for specific information");
KEYWORD (show_hosts, show_hosts_name, ALTERNATE,
	 "hosts", "IP domain-name, lookup style, nameservers, and host table", 
	 PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_hosts
