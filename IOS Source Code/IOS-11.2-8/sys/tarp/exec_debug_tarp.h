/* $Id: exec_debug_tarp.h,v 3.3 1996/02/01 23:34:35 widmer Exp $
 * $Source: /release/111/cvs/Xsys/tarp/exec_debug_tarp.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_tarp.h,v $
 * Revision 3.3  1996/02/01  23:34:35  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  18:44:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug tarp events
 *
 */
EOLS	(debug_tarp_events_eol, debug_command, DEBUG_TARP_EVENTS);
KEYWORD (debug_tarp_events, debug_tarp_events_eol, no_alt,
	 "events", "TARP events", PRIV_OPR);

/******************************************************************
 * debug tarp packets
 *
 */
EOLS	(debug_tarp_packets_eol, debug_command, DEBUG_TARP_PACKETS);
KEYWORD (debug_tarp_packets, debug_tarp_packets_eol, debug_tarp_events,
	 "packets", "TARP packet-related information", PRIV_OPR);

/******************************************************************
 * debug tarp
 *
 */
KEYWORD_DEBUG(debug_tarp, debug_tarp_packets, ALTERNATE,
	      OBJ(pdb,1), tarp_debug_arr,
	      "tarp", "TARP information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_tarp
