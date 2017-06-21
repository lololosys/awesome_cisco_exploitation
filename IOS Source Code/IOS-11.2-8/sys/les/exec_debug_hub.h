/* $Id: exec_debug_hub.h,v 3.2 1995/11/17 17:36:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/exec_debug_hub.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ H U B. H
 *
 * September 1994, Viet Nguyen
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Viet Nguyen
 *------------------------------------------------------------------
 * $Log: exec_debug_hub.h,v $
 * Revision 3.2  1995/11/17  17:36:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:52:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:48:05  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug hub
 *
 */
EOLS (debug_hub_eol, debug_command, DEBUG_HUB);
KEYWORD_DEBUG(debug_hub, debug_hub_eol, ALTERNATE,
	      OBJ(pdb,1), hub_debug_arr,
	      "hub", "Hub repeater information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_hub
