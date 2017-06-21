/* $Id: exec_debug_decnet.h,v 3.2 1995/11/17 08:59:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/exec_debug_decnet.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ D E C N E T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_decnet.h,v $
 * Revision 3.2  1995/11/17  08:59:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:30  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:47:44  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:38  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:18:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug decnet routing
 *
 */
EOLS	(debug_decnet_routing_eol, debug_command, DEBUG_RT_DECNET);
KEYWORD (debug_decnet_routing, debug_decnet_routing_eol, no_alt,
	 "routing", "DECnet routing table transactions", PRIV_OPR);

/******************************************************************
 * debug decnet packets
 *
 */
EOLS	(debug_decnet_packets_eol, debug_command, DEBUG_DECNET_PKT);
KEYWORD (debug_decnet_packets, debug_decnet_packets_eol, debug_decnet_routing,
	 "packets", "DECnet routing updates and HELLO packets", PRIV_OPR);

/******************************************************************
 * debug decnet 
 *
 */
EOLS	(debug_decnet_events_eol, debug_command, DEBUG_DECNET);
KEYWORD (debug_decnet_events, debug_decnet_events_eol, debug_decnet_packets,
	 "events", "DECnet major events", PRIV_OPR);

/******************************************************************
 * debug decnet adjacencies
 *
 */
EOLS	(debug_decnet_adj_eol, debug_command, DEBUG_DECNET_ADJ);
KEYWORD (debug_decnet_adj, debug_decnet_adj_eol, debug_decnet_events,
	 "adjacencies", "DECnet adjacency events", PRIV_OPR);


/******************************************************************
 * debug decnet connects
 *
 */
EOLS	(debug_decnet_connects_eol, debug_command, DEBUG_CONN_DECNET);
KEYWORD (debug_decnet_connects, debug_decnet_connects_eol, debug_decnet_adj,
	 "connects", "DECnet connect access lists", PRIV_OPR);

/******************************************************************
 * debug decnet
 *
 */
KEYWORD_DEBUG(debug_decnet, debug_decnet_connects, ALTERNATE,
	      OBJ(pdb,1), decnet_debug_arr,
	      "decnet", "DECnet information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_decnet
