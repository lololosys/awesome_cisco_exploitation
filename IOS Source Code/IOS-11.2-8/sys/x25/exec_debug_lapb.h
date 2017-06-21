/* $Id: exec_debug_lapb.h,v 3.2 1995/11/17 18:06:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/exec_debug_lapb.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ L A P B . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_lapb.h,v $
 * Revision 3.2  1995/11/17  18:06:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:48:14  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug lapb
 *
 */
EOLS	(debug_lapb_eol, debug_command, DEBUG_LAPB);
KEYWORD_DEBUG(debug_lapb, debug_lapb_eol, ALTERNATE,
	      OBJ(pdb,1), lapb_debug_arr,
	      "lapb", "LAPB protocol transactions", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_lapb
