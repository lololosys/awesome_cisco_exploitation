/* $Id: exec_debug_lanmgr.h,v 3.2 1995/11/17 09:22:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/exec_debug_lanmgr.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ L A N M G R . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_lanmgr.h,v $
 * Revision 3.2  1995/11/17  09:22:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/19  14:48:13  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:51  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug lnm mac
 *
 */
EOLS	(debug_lanmgr_mac_eol, debug_command, DEBUG_LM_MAC);
KEYWORD (debug_lanmgr_mac, debug_lanmgr_mac_eol, no_alt,
	 "mac", "Lan Network Manager MAC frames", PRIV_OPR);

/******************************************************************
 * debug lnm llc
 *
 */
EOLS	(debug_lanmgr_llc_eol, debug_command, DEBUG_LM_LLC);
KEYWORD (debug_lanmgr_llc, debug_lanmgr_llc_eol, debug_lanmgr_mac,
	 "llc", "Lan Network Manager LLC frames", PRIV_OPR);

/******************************************************************
 * debug lnm event
 *
 */
EOLS	(debug_lanmgr_event_eol, debug_command, DEBUG_LM_E);
KEYWORD (debug_lanmgr_event, debug_lanmgr_event_eol, debug_lanmgr_llc,
	 "event", "Lan Network Manager Events", PRIV_OPR);

/******************************************************************
 * debug lnm
 *
 */
KEYWORD_DEBUG(debug_lanmgr, debug_lanmgr_event, ALTERNATE,
	      OBJ(pdb,1), ibmnm_debug_arr,
	      "lnm", "Lan Network Manager information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_lanmgr
