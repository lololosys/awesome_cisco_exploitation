/* $Id: exec_debug_isdn.h,v 3.2.62.1 1996/05/21 10:09:28 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_debug_isdn.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ I S D N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_isdn.h,v $
 * Revision 3.2.62.1  1996/05/21  10:09:28  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  18:01:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:48:09  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:47  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug isdn code
 */
EOLS	(debug_isdn_det_eol, isdn_debug_command, DEBUG_ISDN_DETAIL);
KEYWORD	(debug_isdn_det, debug_isdn_det_eol, no_alt,
	 "code", "", PRIV_OPR | PRIV_HIDDEN);

/******************************************************************
 * debug isdn q931 [interface bri x]
 */
EOLS	(debug_isdn_q931_eol, isdn_debug_command, DEBUG_ISDN_Q931);
INTERFACE_KEYWORD(debug_isdn_q931_if, debug_isdn_q931_eol, 
		  debug_isdn_q931_eol,
		  OBJ(idb,1), IFTYPE_BRI | IFTYPE_SERIAL,
		  common_str_interface, "Specify interface for debugging");
KEYWORD	(debug_isdn_q931, debug_isdn_q931_if, debug_isdn_det,
	 "q931", "ISDN Q931 packets", PRIV_OPR);

/******************************************************************
 * debug isdn q921 [interface bri x]
 */
EOLS	(debug_isdn_q921_eol, isdn_debug_command, DEBUG_ISDN_Q921);
INTERFACE_KEYWORD(debug_isdn_q921_if, debug_isdn_q921_eol, 
		  debug_isdn_q921_eol,
		  OBJ(idb,1), IFTYPE_BRI | IFTYPE_SERIAL,
		  common_str_interface, "Specify interface for debugging");
KEYWORD	(debug_isdn_q921, debug_isdn_q921_if, debug_isdn_q931,
	 "q921", "ISDN Q921 packets", PRIV_OPR);

/******************************************************************
 * debug isdn events
 */
EOLS	(debug_isdn_events_eol, isdn_debug_command, DEBUG_ISDN_EVENTS);
KEYWORD	(debug_isdn_events, debug_isdn_events_eol, debug_isdn_q921,
	 "events", "ISDN events", PRIV_OPR);

/******************************************************************
 * debug isdn
 */
KEYWORD_DEBUG(debug_isdn, debug_isdn_events, NONE,
	      OBJ(pdb,1), isdn_debug_arr,
	      "isdn", "ISDN information", PRIV_OPR);

ASSERT (debug_isdn_test, debug_isdn, ALTERNATE,	(nbrinets || nprinets));

#undef	ALTERNATE
#define	ALTERNATE	debug_isdn_test
