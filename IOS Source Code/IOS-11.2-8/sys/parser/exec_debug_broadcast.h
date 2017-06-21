/* $Id: exec_debug_broadcast.h,v 3.2 1995/11/17 18:46:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/exec_debug_broadcast.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ B C A S T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_broadcast.h,v $
 * Revision 3.2  1995/11/17  18:46:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  14:47:24  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug bcast
 *
 */
EOLS	(debug_bcast_eol, debug_command, DEBUG_BROADCAST);
KEYWORD_DEBUG(debug_bcast, debug_bcast_eol, ALTERNATE,
	      OBJ(pdb,1), network_debug_arr,
	      "broadcast", "MAC broadcast packets", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_bcast
