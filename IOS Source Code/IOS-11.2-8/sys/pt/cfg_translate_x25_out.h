/* $Id: cfg_translate_x25_out.h,v 3.2 1995/11/17 18:59:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/cfg_translate_x25_out.h,v $
 *------------------------------------------------------------------
 * X25 translate out command
 *
 * December 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_translate_x25_out.h,v $
 * Revision 3.2  1995/11/17  18:59:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:29  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(trans_out_x25_return, no_alt);

/****************************************************************
 *  x25 and options
 *
 * translate x25 <addr> [ cud <string> | profile <string> |
 *			  pvc <num> | reverse ]
 *
 * x25 <addr> = OBJ(paddr,2)
 * cud <string> = OBJ(string,4)
 * profile <string> = OBJ(string,5)
 * pvc <num> = OBJ(int,7), OBJ(int,8)
 * reverse = OBJ(int,9)
 * no-reverse = OBJ(int,10)
 *
 */
pdecl(trans_out_x25_opts);
KEYWORD_ID(trans_out_x25_noreverse, trans_out_x25_opts, NONE,
	   OBJ(int,10), TRUE,
	   "no-reverse", "Do not request reverse charge facility", PRIV_CONF);
KEYWORD_ID(trans_out_x25_reverse, trans_out_x25_opts, trans_out_x25_noreverse,
	   OBJ(int,9), TRUE,
	   "reverse", "Request reverse charge on a per call basis", PRIV_CONF);
NUMBER	(trans_out_x25_pvc_num, trans_out_x25_opts, no_alt,
	 OBJ(int,8), 0, -1, "Number of incoming connection");
KEYWORD_ID(trans_out_x25_pvc, trans_out_x25_pvc_num, trans_out_x25_reverse,
	   OBJ(int,7), TRUE,
	   "pvc",
	   "An incoming connection is actually a PVC",
	   PRIV_CONF);
STRING	(trans_out_x25_profile_name, trans_out_x25_opts, no_alt,
	 OBJ(string,5), "Profile name");
KEYWORD (trans_out_x25_profile, trans_out_x25_profile_name, trans_out_x25_pvc,
	 "profile", "Use a defined X.3 profile", PRIV_CONF);
STRING	(trans_out_x25_cud_pattern, trans_out_x25_opts, no_alt,
	 OBJ(string,4), "The Call User Data (CUD)");
KEYWORD	(trans_out_x25_cud, trans_out_x25_cud_pattern, trans_out_x25_profile,
	 "cud", "Specify the Call User Data (CUD)", PRIV_CONF);
NOP	(trans_out_x25_opts, trans_out_x25_cud, trans_out_x25_return);

/*************************************************************/
X121ADDR_NAME(trans_out_x25_addr, trans_out_x25_opts, no_alt,
	      OBJ(paddr,2), "X.121 Address");
KEYWORD_ID(trans_out_x25, trans_out_x25_addr, ALTERNATE,
	   OBJ(int,6), TRANS_PROT_X25,
	   "x25", "X.25", PRIV_CONF);

SET	(trans_out_x25addr_set, trans_out_x25_opts,
	 OBJ(int,6), TRANS_PROT_OLD);
X121ADDR_NAME(trans_out_x25addr, trans_out_x25addr_set, NONE,
	      OBJ(paddr,2), "X.121 Address");
TESTVAR	(trans_outgoing_x25_test, trans_out_x25addr, NONE,
	 NONE, NONE, trans_out_x25, OBJ(int,1), TRANS_PROT_OLD);

#undef	ALTERNATE
#define	ALTERNATE	trans_outgoing_x25_test
