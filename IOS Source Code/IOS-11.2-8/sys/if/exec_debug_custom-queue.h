/* $Id: exec_debug_custom-queue.h,v 3.2.60.1 1996/04/17 13:40:06 fred Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_debug_custom-queue.h,v $
 *------------------------------------------------------------------
 * Debug commands for custom queueing
 *
 * February 1993, Bruce Cole
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_debug_custom-queue.h,v $
 * Revision 3.2.60.1  1996/04/17  13:40:06  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:27:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:20  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:47:42  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	debug custom-queue
 */
EOLS	(debug_custom_eol, debug_command, DEBUG_CUSTOM);
KEYWORD_DEBUG(debug_custom, debug_custom_eol, ALTERNATE,
	      OBJ(pdb,1), network_debug_arr,
	      "custom-queue", "Custom output queueing", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_custom
