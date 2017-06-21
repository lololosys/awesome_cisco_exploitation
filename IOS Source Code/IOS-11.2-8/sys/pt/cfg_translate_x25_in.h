/* $Id: cfg_translate_x25_in.h,v 3.2 1995/11/17 18:59:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/cfg_translate_x25_in.h,v $
 *------------------------------------------------------------------
 * X25 translate command
 *
 * December 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_translate_x25_in.h,v $
 * Revision 3.2  1995/11/17  18:59:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/28  22:41:22  billw
 * CSCdi40511:  translate command does not support x25 regular expressions
 *
 * Revision 2.1  1995/07/04  01:56:29  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(trans_in_x25_return, no_alt);

/****************************************************************
 *  x25 and options
 *
 * translate x25 <addr> [ cud <string> | profile <string> |
 *			  pvc <num> | accept-reverse | printer]
 *
 * x25 <addr> = OBJ(string,3)
 * cud <string> = OBJ(string,1)
 * profile <string> = OBJ(string,2)
 * pvc <num> = OBJ(int,2), OBJ(int,3)
 * accept-reverse = OBJ(int,4)
 * printer = OBJ(int,5)
 *
 */
pdecl(trans_in_x25_opts);
KEYWORD_ID(trans_in_x25_printer, trans_in_x25_opts, NONE,
           OBJ(int,5), TRUE,
           "printer", "Enable non-interactive (implies global quiet)",
           PRIV_CONF);
STRING	(trans_in_x25_cud_pattern, trans_in_x25_opts, no_alt,
	 OBJ(string,1), "The Call User Data (CUD)");
KEYWORD	(trans_in_x25_cud, trans_in_x25_cud_pattern, trans_in_x25_printer,
	 "cud", "Specify the Call User Data (CUD)", PRIV_CONF);
KEYWORD_ID(trans_in_x25_reverse, trans_in_x25_opts, trans_in_x25_cud,
	   OBJ(int,4), TRUE,
	   "accept-reverse", "Accept reverse charge on a per-call basis", PRIV_CONF);
NUMBER	(trans_in_x25_pvc_num, trans_in_x25_opts, no_alt,
	 OBJ(int,3), 0, -1, "Number of incoming connection");
KEYWORD_ID(trans_in_x25_pvc, trans_in_x25_pvc_num, trans_in_x25_reverse,
	   OBJ(int,2), TRUE,
	   "pvc", "An incoming connection is actually a PVC", PRIV_CONF);
STRING	(trans_in_x25_profile_name, trans_in_x25_opts, no_alt,
	 OBJ(string,2), "Profile name");
KEYWORD (trans_in_x25_profile, trans_in_x25_profile_name, trans_in_x25_pvc,
	 "profile", "Use a defined X.3 profile", PRIV_CONF);
NOP	(trans_in_x25_opts, trans_in_x25_profile, trans_in_x25_return);

/*************************************************************/
STRING(trans_in_x25_addr, trans_in_x25_opts, no_alt,
	      OBJ(string,3), "X.121 Address pattern");
KEYWORD_ID(trans_in_x25, trans_in_x25_addr, ALTERNATE,
	   OBJ(int,1), TRANS_PROT_X25,
	   "x25", "X.25", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	trans_in_x25
