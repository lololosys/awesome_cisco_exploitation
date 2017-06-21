/* $Id: exec_debug_token-ring.h,v 3.2 1995/11/17 09:27:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/exec_debug_token-ring.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ T O K E N . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_token-ring.h,v $
 * Revision 3.2  1995/11/17  09:27:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:23  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/19  14:49:18  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:56:07  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:20:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug token ring
 *
 */
EOLS	(debug_token_ring_eol, debug_command, DEBUG_TOKENRING);
KEYWORD (debug_token_ring, debug_token_ring_eol, no_alt,
	 "ring", "Token ring input and output", PRIV_OPR);

/******************************************************************
 * debug token event
 *
 */
EOLS	(debug_token_event_eol, debug_command, DEBUG_TOKENEVENT);
KEYWORD (debug_token_event, debug_token_event_eol, debug_token_ring,
	 "events", "Significant Token Ring hardware events", PRIV_OPR);

/******************************************************************
 * debug token
 *
 */
KEYWORD_DEBUG(debug_token, debug_token_event, ALTERNATE,
	      OBJ(pdb,1), tring_debug_arr,
	      "token", "Token Ring information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_token
