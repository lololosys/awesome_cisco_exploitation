/* $Id: exec_debug_priority.h,v 3.2 1995/11/17 09:27:15 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/exec_debug_priority.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ P R I O R I T Y . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_priority.h,v $
 * Revision 3.2  1995/11/17  09:27:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:22  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:48:43  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug priority
 *
 */
EOLS	(debug_priority_eol, debug_command, DEBUG_PRIORITY);
KEYWORD_DEBUG(debug_priority, debug_priority_eol, ALTERNATE,
	      OBJ(pdb,1), network_debug_arr,
	      "priority", "Priority output queueing", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_priority
