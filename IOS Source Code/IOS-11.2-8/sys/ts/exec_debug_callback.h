/* $Id: exec_debug_callback.h,v 3.2 1995/11/17 18:54:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/exec_debug_callback.h,v $
 *------------------------------------------------------------------
 * Parser chain for debug callback
 *
 * April 1995, Shoou Yiu
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_callback.h,v $
 * Revision 3.2  1995/11/17  18:54:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:55  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:47:30  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug callback
 *
 */

EOLS	(debug_callback_eol, debug_command, DEBUG_CALLBACK);
KEYWORD_DEBUG(debug_callback, debug_callback_eol, ALTERNATE,
	      OBJ(pdb,1), callback_debug_arr,
	      "callback", "Callback activity", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_callback
