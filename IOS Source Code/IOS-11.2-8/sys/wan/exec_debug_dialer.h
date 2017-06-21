/* $Id: exec_debug_dialer.h,v 3.4.20.3 1996/07/19 19:50:37 wmay Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_debug_dialer.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ D I A L E R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_dialer.h,v $
 * Revision 3.4.20.3  1996/07/19  19:50:37  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/07/16  17:58:59  wmay
 * CSCdi63102:  debug dialer<cr> doesnt work anymore
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/04/27  06:36:23  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.16.1  1996/02/23  21:31:27  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.3  1995/11/29  01:59:04  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.2  1995/11/17  18:01:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:47:47  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug dialer events
 *
 */
EOLS	(debug_dialer_eol, debug_command, DEBUG_DIALER);
KEYWORD	(debug_dialer_events, debug_dialer_eol, debug_dialer_eol,
	 "events", "Dial on demand events", PRIV_OPR);

/******************************************************************
 * debug dialer holdq
 */
EOLS	(debug_dialer_holdq_eol, debug_command, DEBUG_DIALER_HOLDQ);
KEYWORD (debug_dialer_holdq, debug_dialer_holdq_eol, debug_dialer_events,
	    "holdq", "Dial on Demand holdq messages", 
	    PRIV_OPR | PRIV_HIDDEN);

/******************************************************************
 * debug dialer detailed
 *
 */
EOLS	(debug_dialer_detailed_eol, debug_command, DEBUG_DIALER_DETAILED);
KEYWORD_MM (debug_dialer_detailed, debug_dialer_detailed_eol,
	    debug_dialer_holdq,
	    "detailed", "Dial on Demand detailed messages", 
	    PRIV_OPR | PRIV_HIDDEN, 8);

/******************************************************************
 * debug dialer packets
 *
 */
EOLS	(debug_dialer_pkt_eol, debug_command, DEBUG_DIALER_PACKET);
KEYWORD	(debug_dialer_pkt, debug_dialer_pkt_eol, debug_dialer_detailed,
	 "packets", "Dial on demand traffic", PRIV_OPR);

/******************************************************************
 * debug dialer
 *
 */
KEYWORD_DEBUG(debug_dialer, debug_dialer_pkt, ALTERNATE,
	      OBJ(pdb,1), dialer_debug_arr,
	      "dialer", "Dial on Demand", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_dialer
