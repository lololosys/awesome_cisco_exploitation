/* $Id: exec_show_ip_alias.h,v 3.2.60.1 1996/08/12 16:02:59 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_ip_alias.h,v $
 *------------------------------------------------------------------
 * IP alias show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_alias.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:59  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:32:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:45  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:24:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ip aliases
 * 
 */
EOLNS	(show_ip_alias_eol, show_ip_alias);
KEYWORD (show_ip_alias_kwd, show_ip_alias_eol, ALTERNATE,
	 "aliases", "IP alias table", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_alias_kwd
