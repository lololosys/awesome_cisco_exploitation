/* $Id: exec_debug_sdlc.h,v 3.2.60.1 1996/05/23 06:42:33 pmorton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/exec_debug_sdlc.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ S D L C . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_sdlc.h,v $
 * Revision 3.2.60.1  1996/05/23  06:42:33  pmorton
 * CSCdi58502:  Fix for CSCdi42369 broke FRAD image
 * Branch: California_branch
 * Use 'debug sdlc qllc' instead of 'debug qllc sdlc'
 *
 * Revision 3.2  1995/11/17  09:22:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/19  14:48:51  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:59  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug sdllc
 *
 */
EOLS	(debug_sdllc_eol, sdlc_debug_command, DEBUG_SDLLC);
KEYWORD_DEBUG(debug_sdllc, debug_sdllc_eol, ALTERNATE,
	      OBJ(pdb,1), sdlc_debug_arr,
	      "sdllc", "SDLLC media translation", PRIV_OPR);

/******************************************************************
 * debug sdlc qllc
 */
EOLS	(debug_sdlc_qllc_eol, sdlc_debug_command, DEBUG_SDLC_QLLC);
KEYWORD (debug_sdlc_qllc, debug_sdlc_qllc_eol, no_alt,
	   "qllc", "SDLC QLLC direct conversion", PRIV_OPR);

/******************************************************************
 * debug sdlc packets <maxsize>
 */
EOLS	(debug_sdlc_pak_eol, sdlc_debug_command, DEBUG_SDLC_PACKETS);
NUMBER	(debug_sdlc_pak_num, debug_sdlc_pak_eol, debug_sdlc_pak_eol,
	 OBJ(int,1), 1, 1024, "number of bytes");
KEYWORD_ID(debug_sdlc_pak, debug_sdlc_pak_num, debug_sdlc_qllc,
	   OBJ(int,1), 1024,
	   "packets", "Dump packets", PRIV_OPR);

/******************************************************************
 * debug sdlc local-ack
 */
EOLS	(debug_sdlc_lack_eol, sdlc_debug_command, DEBUG_SDLC_LACK);
NUMBER	(debug_sdlc_lack_num, debug_sdlc_lack_eol, debug_sdlc_lack_eol,
	 OBJ(int,1), 0, 7, "SDLC Local Acknowledgement debug level");
KEYWORD_ID(debug_sdlc_lack, debug_sdlc_lack_num, debug_sdlc_pak,
	   OBJ(int,1), 7,
	   "local-ack", "SDLC Local Acknowledgement events", PRIV_OPR);


/******************************************************************
 * debug sdlc event
 */
EOLS	(debug_sdlc_event_eol, sdlc_debug_command, DEBUG_SDLC);
KEYWORD_ID(debug_sdlc_event, debug_sdlc_event_eol, debug_sdlc_lack,
	   OBJ(int,10), TRUE,
	   "event", "SDLC link protocol", PRIV_OPR);

/******************************************************************
 * debug sdlc
 */
KEYWORD_DEBUG(debug_sdlc, debug_sdlc_event, debug_sdllc,
	      OBJ(pdb,1), sdlc_debug_arr,
	      "sdlc", "SDLC information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_sdlc
