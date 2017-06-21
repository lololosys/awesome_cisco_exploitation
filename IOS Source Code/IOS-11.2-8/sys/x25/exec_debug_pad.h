/* $Id: exec_debug_pad.h,v 3.2 1995/11/17 18:06:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/exec_debug_pad.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ P A D . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_pad.h,v $
 * Revision 3.2  1995/11/17  18:06:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:48:35  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug pad
 *
 */
EOLS	(debug_pad_eol, debug_command, DEBUG_PAD);
KEYWORD_DEBUG(debug_pad, debug_pad_eol, ALTERNATE,
	      OBJ(pdb,1), pad_debug_arr,
	      "pad", "X25 PAD protocol", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_pad
