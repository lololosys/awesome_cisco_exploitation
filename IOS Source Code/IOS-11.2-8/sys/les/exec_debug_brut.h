/* $Id: exec_debug_brut.h,v 3.2 1995/11/17 17:36:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/exec_debug_brut.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ B R U T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_brut.h,v $
 * Revision 3.2  1995/11/17  17:36:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/19  14:47:25  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:31  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:17:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug hub tx
 *
 */
EOLS	(debug_brut_tx_eol, debug_command, DEBUG_HUB_TX);
KEYWORD (debug_brut_tx, debug_brut_tx_eol, no_alt,
	 "tx", "transmit messages", PRIV_OPR);

/******************************************************************
 * debug hub trace
 *
 */
EOLS	(debug_brut_trc_eol, debug_command, DEBUG_HUB_TRC);
KEYWORD (debug_brut_trc, debug_brut_trc_eol, debug_brut_tx,
	 "trace", "", PRIV_ROOT | PRIV_HIDDEN);

/******************************************************************
 * debug hub rcv
 *
 */
EOLS	(debug_brut_rcv_eol, debug_command, DEBUG_HUB_RCV);
KEYWORD (debug_brut_rcv, debug_brut_rcv_eol, debug_brut_trc,
	 "rcv", "receive messages", PRIV_OPR);

/******************************************************************
 * debug hub
 *
 */
KEYWORD_DEBUG(debug_brut_kwd, debug_brut_rcv, NONE,
	      OBJ(pdb,1), brut_debug_arr,
	      "hub", "DECHub90 Hub management", PRIV_OPR);

ASSERT	(debug_brut, debug_brut_kwd, ALTERNATE,
	 IS_CPU(CPU_IGS_BRUT));

#undef	ALTERNATE
#define	ALTERNATE	debug_brut
