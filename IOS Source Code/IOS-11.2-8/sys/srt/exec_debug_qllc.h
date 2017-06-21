/* $Id: exec_debug_qllc.h,v 3.2.62.2 1996/05/23 06:42:12 pmorton Exp $
 * $Source: /release/112/cvs/Xsys/srt/exec_debug_qllc.h,v $
 *------------------------------------------------------------------
 * QLLC: debug command syntax
 *
 * April, 1994          Phil Morton
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_qllc.h,v $
 * Revision 3.2.62.2  1996/05/23  06:42:12  pmorton
 * CSCdi58502:  Fix for CSCdi42369 broke FRAD image
 * Branch: California_branch
 * Use 'debug sdlc qllc' instead of 'debug qllc sdlc'
 *
 * Revision 3.2.62.1  1996/05/22  05:00:19  pmorton
 * CSCdi42369:  sdlc-qllc conversion, no auto-reconnect
 * Branch: California_branch
 * 1. When qllc link state changes propagate to SDLC-QLLC conversion (SNX)
 * 2. Add 'debug qllc sdlc' to track this
 * 3. Correct SNX state names
 *
 * Revision 3.2  1995/11/17  18:53:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:05  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:48:46  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug lnx
 *
 */

EOLS	(debug_lnx_eol, qllc_debug_command, DEBUG_LNX);
KEYWORD_DEBUG(debug_lnx_kwd, debug_lnx_eol, ALTERNATE,
	      OBJ(pdb,1), qllc_debug_arr,
	      "lnx", "generic qllc/llc2 conversion activity", PRIV_OPR);

/******************************************************************
 * debug qllc test
 *
 */

EOLS	(debug_qllc_test_eol, qllc_debug_command, DEBUG_QLLC_TEST);
KEYWORD (debug_qllc_test_kwd, debug_qllc_test_eol, no_alt,
	 "test", "qllc test special", PRIV_OPR | PRIV_HIDDEN);

/******************************************************************
 * debug qllc error
 *
 */

EOLS	(debug_qllc_error_eol, qllc_debug_command, DEBUG_QLLC_ERROR);
KEYWORD (debug_qllc_error_kwd, debug_qllc_error_eol, debug_qllc_test_kwd,
	 "error", "qllc errors", PRIV_OPR);

/******************************************************************
 * debug qllc x25
 *
 */
EOLS	(debug_qllc_x25_eol, qllc_debug_command, DEBUG_QLLC_X25);
KEYWORD (debug_qllc_x25_kwd, debug_qllc_x25_eol, debug_qllc_error_kwd,
	 "x25", "qllc x25 events", PRIV_OPR);

/******************************************************************
 * debug qllc timer
 *
 */
EOLS	(debug_qllc_timer_eol, qllc_debug_command, DEBUG_QLLC_TIMER);
KEYWORD (debug_qllc_timer_kwd, debug_qllc_timer_eol, debug_qllc_x25_kwd,
	 "timer", "qllc timers", PRIV_OPR);



/******************************************************************
 * debug qllc packet
 *
 */
EOLS	(debug_qllc_packet_eol, qllc_debug_command, DEBUG_QLLC_PACKET);
KEYWORD (debug_qllc_packet_kwd, debug_qllc_packet_eol, debug_qllc_timer_kwd,
	 "packet", "qllc packets", PRIV_OPR);

/******************************************************************
 * debug qllc event
 *
 */
EOLS	(debug_qllc_event_eol, qllc_debug_command, DEBUG_QLLC_EVENT);
KEYWORD (debug_qllc_event_kwd, debug_qllc_event_eol, debug_qllc_packet_kwd,
	 "event", "qllc events", PRIV_OPR);

/******************************************************************
 * debug qllc state
 *
 */
EOLS	(debug_qllc_state_eol, qllc_debug_command, DEBUG_QLLC_STATE);
KEYWORD (debug_qllc_state_kwd, debug_qllc_state_eol, debug_qllc_event_kwd,
	 "state", "qllc state transitions", PRIV_OPR);

/******************************************************************
 * debug qllc
 *
 */
KEYWORD_DEBUG(debug_qllc, debug_qllc_state_kwd, debug_lnx_kwd, 
	      OBJ(pdb,1), qllc_debug_arr,
	      "qllc", "qllc debug information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_qllc                                           
