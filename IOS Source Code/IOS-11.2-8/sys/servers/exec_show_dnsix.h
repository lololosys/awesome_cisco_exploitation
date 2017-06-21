/* $Id: exec_show_dnsix.h,v 3.2.62.1 1996/08/12 16:07:09 widmer Exp $
 * $Source: /release/112/cvs/Xsys/servers/exec_show_dnsix.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ D N S I X . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_dnsix.h,v $
 * Revision 3.2.62.1  1996/08/12  16:07:09  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:51:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:47  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:23:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show dnsix
 * 
 */

EOLNS	(show_dnsix_eol, show_dnsix);
KEYWORD (show_dnsix_kwd, show_dnsix_eol, ALTERNATE,
	 "dnsix", "Shows Dnsix/DMDP information", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_dnsix_kwd
