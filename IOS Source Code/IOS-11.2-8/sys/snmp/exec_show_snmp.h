/* $Id: exec_show_snmp.h,v 3.3 1996/02/02 01:29:07 widmer Exp $
 * $Source: /release/111/cvs/Xsys/snmp/exec_show_snmp.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ S N M P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_snmp.h,v $
 * Revision 3.3  1996/02/02  01:29:07  widmer
 * CSCdi48041:  show snmp help is missing
 *
 * Revision 3.2  1995/11/17  18:58:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:08  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:25:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 *  show snmp
 * 
 */
EOLS	(show_snmp_eol, show_snmp, SNMP_SHOW_GLOBALS);

/******************************************************************
 *  show snmp view
 * 
 */
EOLS	(show_snmp_view_eol, show_snmp, SNMP_SHOW_VIEW);
KEYWORD (show_snmp_view, show_snmp_view_eol, show_snmp_eol,
	 "view", NULL, PRIV_ROOT|PRIV_HIDDEN);

/******************************************************************
 *  show snmp party
 * 
 */
EOLS	(show_snmp_party_eol, show_snmp, SNMP_SHOW_PARTY);
KEYWORD (show_snmp_party, show_snmp_party_eol, show_snmp_view,
	 "party", NULL, PRIV_ROOT|PRIV_HIDDEN);

/******************************************************************
 *  show snmp context
 * 
 */
EOLS	(show_snmp_context_eol, show_snmp, SNMP_SHOW_CONTEXT);
KEYWORD (show_snmp_context, show_snmp_context_eol, show_snmp_party,
	 "context", NULL, PRIV_ROOT|PRIV_HIDDEN);

/******************************************************************
 *  show snmp acl
 * 
 */
EOLS	(show_snmp_acl_eol, show_snmp, SNMP_SHOW_ACL);
KEYWORD (show_snmp_acl, show_snmp_acl_eol, show_snmp_context,
	 "access-policy", NULL, PRIV_ROOT|PRIV_HIDDEN);

/******************************************************************
 *  show snmp mib
 * 
 */
EOLS	(show_snmp_mib_eol, show_snmp, SNMP_SHOW_MIB);
KEYWORD (show_snmp_mib, show_snmp_mib_eol, show_snmp_acl,
	 "mib", NULL, PRIV_ROOT|PRIV_HIDDEN);

KEYWORD (show_snmp, show_snmp_mib, ALTERNATE,
	 "snmp", "snmp statistics", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_snmp
