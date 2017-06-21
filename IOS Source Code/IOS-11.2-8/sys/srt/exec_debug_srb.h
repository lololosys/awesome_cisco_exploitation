/* $Id: exec_debug_srb.h,v 3.2 1995/11/17 18:53:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/exec_debug_srb.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ S R B . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_srb.h,v $
 * Revision 3.2  1995/11/17  18:53:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:08  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/19  14:49:02  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:56:03  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:20:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug source event
 *
 */
EOLS	(debug_srb_event_eol, srt_debug_command, DEBUG_SRBEVENT);
KEYWORD (debug_srb_event, debug_srb_event_eol, no_alt,
	 "event", "A subset of the source-bridge messages", PRIV_OPR);

/******************************************************************
 * debug source event
 *
 */
EOLS	(debug_srb_error_eol, srt_debug_command, DEBUG_SRBERROR);
KEYWORD (debug_srb_error, debug_srb_error_eol, debug_srb_event,
	 "error", "Source bridging errors", PRIV_OPR);

/******************************************************************
 * debug source bridge
 *
 */
EOLS	(debug_srb_bridge_eol, srt_debug_command, DEBUG_SRB);
KEYWORD (debug_srb_bridge, debug_srb_bridge_eol, debug_srb_error,
	 "bridge", "Source bridging activity", PRIV_OPR);

/******************************************************************
 * debug source
 *
 */
KEYWORD_DEBUG(debug_srb, debug_srb_bridge, ALTERNATE,
	      OBJ(pdb,1), srt_debug_arr,
	      "source", "Source bridging information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_srb
