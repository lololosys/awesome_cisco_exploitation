/* $Id: exec_debug_cls.h,v 3.2.60.1 1996/05/17 10:43:10 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_debug_cls.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ C L S . H
 *
 * April 1994, Steve Berl
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_cls.h,v $
 * Revision 3.2.60.1  1996/05/17  10:43:10  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/11  14:36:19  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/03/28  08:19:47  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.2.4.1  1995/12/17  09:50:18  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.2  1995/11/17  08:56:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:52:23  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/19  14:47:39  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:36  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:18:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug cls vdlc
 *
 */
EOLS	(debug_cls_vdlc_eol, debug_command, DEBUG_CLS_VDLC);
KEYWORD (debug_cls_vdlc, debug_cls_vdlc_eol, no_alt,
	 "vdlc", "VDLC", PRIV_OPR);

/*
 * debug sdlc to cls interface
 *
 */
EOLS  (debug_cls_sdlc_eol, debug_command, DEBUG_CLS_SDLC);
KEYWORD (debug_cls_sdlc, debug_cls_sdlc_eol, debug_cls_vdlc,
         "sdlc", "CLS SDLC", PRIV_OPR);

/*
 * debug cls messages
 *
 */
EOLS	(debug_cls_messages_eol, debug_command, DEBUG_CLS_MESSAGE);
KEYWORD (debug_cls_messages, debug_cls_messages_eol, debug_cls_sdlc,
	 "messages", "CLS messages", PRIV_OPR);

/*
 * debug cls errors
 *
 */
EOLS	(debug_cls_err_eol, debug_command, DEBUG_CLS_ERROR);
KEYWORD (debug_cls_err, debug_cls_err_eol, debug_cls_messages,
	 "errors", "CLS errors", PRIV_OPR);

/*
 * debug cls state
 *
 */
EOLS	(debug_cls_state_eol, debug_command, DEBUG_CLS_STATE);
KEYWORD (debug_cls_state, debug_cls_state_eol, debug_cls_err,
	 "state", "CLS state changes", PRIV_OPR);

/*
 * debug cls_events
 *
 */
KEYWORD_DEBUG(debug_cls, debug_cls_state, ALTERNATE,
	      OBJ(pdb,1), cls_debug_arr,
	      "cls", "CLS Information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_cls
