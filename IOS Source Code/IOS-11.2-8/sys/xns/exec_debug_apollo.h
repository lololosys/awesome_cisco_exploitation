/* $Id: exec_debug_apollo.h,v 3.2 1995/11/17 19:21:27 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/exec_debug_apollo.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ A P O L L O . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_apollo.h,v $
 * Revision 3.2  1995/11/17  19:21:27  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:47:05  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:27  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:17:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug apollo routing
 *
 */
EOLS	(debug_apollo_routing_eol, debug_command, DEBUG_APOLLORIP);
KEYWORD (debug_apollo_routing, debug_apollo_routing_eol, no_alt,
	 "routing", "Apollo routing transactions", PRIV_OPR);

/******************************************************************
 * debug apollo packet
 *
 */
EOLS	(debug_apollo_packet_eol, debug_command, DEBUG_APOLLO);
KEYWORD (debug_apollo_packet, debug_apollo_packet_eol, debug_apollo_routing,
	 "packet", "Apollo packet traffic", PRIV_OPR);

/******************************************************************
 * debug apollo
 *
 */
KEYWORD_DEBUG(debug_apollo, debug_apollo_packet, ALTERNATE,
	      OBJ(pdb,1), apollo_debug_arr,
	      "apollo", "Apollo information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_apollo
