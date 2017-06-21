/* $Id: exec_debug_clns.h,v 3.2 1995/11/17 08:53:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/exec_debug_clns.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ C L N S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_clns.h,v $
 * Revision 3.2  1995/11/17  08:53:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:47:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:47:38  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:34  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:18:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug clns routing
 *
 */
EOLS	(debug_clns_routing_eol, debug_command, DEBUG_CLNS_ROUTING);
KEYWORD (debug_clns_routing, debug_clns_routing_eol, no_alt,
	 "routing", "CLNS routing table activity", PRIV_OPR);

/******************************************************************
 * debug clns filters
 *
 */
EOLS	(debug_clns_filters_eol, debug_command, DEBUG_CLNS_FILTER);
KEYWORD (debug_clns_filters, debug_clns_filters_eol, debug_clns_routing,
	 "filters", "CLNS filters", PRIV_OPR);

/******************************************************************
 * debug clns packets
 *
 */
EOLS	(debug_clns_packets_eol, debug_command, DEBUG_CLNS_PACKET);
KEYWORD (debug_clns_packets, debug_clns_packets_eol, debug_clns_filters,
	 "packets", "CLNS packet switching activity", PRIV_OPR);

/******************************************************************
 * debug clns igrp-packets
 *
 */
EOLS	(debug_clns_igrp_packets_eol, debug_command, DEBUG_CLNS_IGRP);
KEYWORD (debug_clns_igrp_packets, debug_clns_igrp_packets_eol,
	 debug_clns_packets,
	 "igrp-packets", "ISO-IGRP routing activity", PRIV_OPR);

/******************************************************************
 * debug clns events
 *
 */
EOLS	(debug_clns_events_eol, debug_command, DEBUG_CLNS_EVENT);
KEYWORD (debug_clns_events, debug_clns_events_eol, debug_clns_igrp_packets,
	 "events", "CLNS error events", PRIV_OPR);

/******************************************************************
 * debug clns esis packets
 *
 */
EOLS	(debug_clns_esis_packets_eol, debug_command,DEBUG_ESIS_PACKET);
KEYWORD (debug_clns_esis_packets, debug_clns_esis_packets_eol,
	 no_alt,
	 "packets", "ES-IS packet activity", PRIV_OPR);

/******************************************************************
 * debug clns esis events
 *
 */
EOLS	(debug_clns_esis_events_eol, debug_command, DEBUG_ESIS_EVENT);
KEYWORD (debug_clns_esis_events, debug_clns_esis_events_eol,
	 debug_clns_esis_packets,
	 "events", "ES-IS protocol events", PRIV_OPR);

/******************************************************************
 * debug clns esis
 *
 */
KEYWORD (debug_clns_esis, debug_clns_esis_events, debug_clns_events,
	 "esis", "ES-IS protocol events", PRIV_OPR);

/******************************************************************
 * debug clns
 *
 */
KEYWORD_DEBUG(debug_clns, debug_clns_esis, ALTERNATE,
	      OBJ(pdb,1), clns_debug_arr,
	      "clns", "CLNS information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_clns
