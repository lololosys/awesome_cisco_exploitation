/* $Id: exec_debug_dlcsw.h,v 3.2 1995/11/17 08:56:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/exec_debug_dlcsw.h,v $
 *------------------------------------------------------------------
 * DLC-SW Debug Exec Command Parser Definition
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_dlcsw.h,v $
 * Revision 3.2  1995/11/17  08:56:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:52:24  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:47:48  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug fras error
 *
 */
EOLS	(debug_dlcsw_error_eol, debug_command, DEBUG_DLCSW_ERR);
KEYWORD (debug_dlcsw_error, debug_dlcsw_error_eol, no_alt,
	 "error", "FRAS Errors", PRIV_OPR);

/******************************************************************
 * debug fras state
 *
 */
EOLS	(debug_dlcsw_state_eol, debug_command, DEBUG_DLCSW_STATE);
KEYWORD (debug_dlcsw_state, debug_dlcsw_state_eol, debug_dlcsw_error,
	 "state", "FRAS FSM States", PRIV_OPR);

/******************************************************************
 * debug fras message
 *
 */
EOLS	(debug_dlcsw_message_eol, debug_command, DEBUG_DLCSW_MESS);
KEYWORD (debug_dlcsw_message, debug_dlcsw_message_eol, debug_dlcsw_state,
	 "message", "FRAS Message", PRIV_OPR);

/******************************************************************
 * debug fras
 *
 */
KEYWORD_DEBUG(debug_dlcsw, debug_dlcsw_message, ALTERNATE,
	      OBJ(pdb,1), dlcsw_debug_arr,
	      "fras", "FRAS Debug ", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_dlcsw
