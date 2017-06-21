/* $Id: exec_debug_span.h,v 3.2 1995/11/17 18:53:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/exec_debug_span.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ S P A N . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_span.h,v $
 * Revision 3.2  1995/11/17  18:53:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:05  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:49:01  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug span
 *
 */
EOLS	(debug_spanevents_eol, srt_debug_command, DEBUG_SPANEVENTS);
KEYWORD (debug_spanevents, debug_spanevents_eol, no_alt,
	 "events", "Spanning-tree topology events", PRIV_OPR);
EOLS	(debug_spantree_eol, debug_command, DEBUG_SPAN);
KEYWORD (debug_spantree, debug_spantree_eol, debug_spanevents,
	 "tree", "Spanning-tree protocol data units", PRIV_OPR);

KEYWORD_DEBUG(debug_span, debug_spantree, ALTERNATE,
	      OBJ(pdb,1), srt_debug_arr,
	      "spanning", "Spanning-tree information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_span
