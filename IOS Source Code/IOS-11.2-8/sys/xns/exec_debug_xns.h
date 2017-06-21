/* $Id: exec_debug_xns.h,v 3.2 1995/11/17 19:21:34 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/exec_debug_xns.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ X N S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_xns.h,v $
 * Revision 3.2  1995/11/17  19:21:34  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:49:26  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:56:11  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:20:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug xns routing
 *
 */
EOLS	(debug_xns_routing_eol, debug_command, DEBUG_XNSRIP);
KEYWORD (debug_xns_routing, debug_xns_routing_eol, no_alt,
	 "routing", "XNS routing transactions", PRIV_OPR);

/******************************************************************
 * debug xns packet
 *
 */
EOLS	(debug_xns_packet_eol, debug_command, DEBUG_XNSPKT);
KEYWORD (debug_xns_packet, debug_xns_packet_eol, debug_xns_routing,
	 "packet", "All XNS activity", PRIV_OPR);

/******************************************************************
 * debug xns
 *
 */
KEYWORD_DEBUG(debug_xns, debug_xns_packet, ALTERNATE,
	      OBJ(pdb,1), xns_debug_arr,
	      "xns", "XNS information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_xns
