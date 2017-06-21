/* $Id: exec_debug_mop.h,v 3.2 1995/11/17 08:59:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/exec_debug_mop.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ M O P . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_mop.h,v $
 * Revision 3.2  1995/11/17  08:59:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:30  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:48:28  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug mop
 *
 */
EOLS	(debug_mop_eol, debug_command, DEBUG_MOP);
KEYWORD_DEBUG(debug_mop, debug_mop_eol, ALTERNATE,
	      OBJ(pdb,1), mop_debug_arr,
	      "mop", "DECnet MOP server events", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_mop
