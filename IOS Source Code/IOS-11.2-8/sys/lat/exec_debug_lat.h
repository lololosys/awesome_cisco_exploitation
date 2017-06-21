/* $Id: exec_debug_lat.h,v 3.2 1995/11/17 17:32:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/exec_debug_lat.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ L A T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_lat.h,v $
 * Revision 3.2  1995/11/17  17:32:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:46  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/19  14:48:16  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:53  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug lat
 *
 */
EOLS	(debug_lat_eol, debug_command, DEBUG_LAT);

/******************************************************************
 * debug lat filtering
 *
 */
EOLS    (debug_lat_filtering_eol, debug_command, DEBUG_LATF);
KEYWORD (debug_lat_filtering, debug_lat_filtering_eol, debug_lat_eol,
         "filtering", "LAT group code filtering actions", PRIV_OPR);


/******************************************************************
 * debug lat packet
 *
 */
EOLS	(debug_lat_packet_eol, debug_command, DEBUG_LATP);
KEYWORD (debug_lat_packet, debug_lat_packet_eol, debug_lat_filtering,
	 "packet", "LAT packet activity", PRIV_OPR);

/******************************************************************
 * debug lat event
 *
 */
EOLS	(debug_lat_event_eol, debug_command, DEBUG_LAT);
KEYWORD (debug_lat_event, debug_lat_event_eol, debug_lat_packet,
	 "event", "LAT events", PRIV_OPR);

/******************************************************************
 * debug lat
 *
 */
KEYWORD_DEBUG(debug_lat, debug_lat_event, ALTERNATE,
	      OBJ(pdb,1), lat_debug_arr,
	      "lat", "LAT Information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_lat
