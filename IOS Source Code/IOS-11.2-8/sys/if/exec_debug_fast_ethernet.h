/* $Id: exec_debug_fast_ethernet.h,v 3.2 1995/11/17 09:27:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/exec_debug_fast_ethernet.h,v $
 *------------------------------------------------------------------
 * parser/exec_debug_fast_ethernet.h: DEBUG command parsing code
 *
 * November 1994, Gary Kramling
 * Adapted from exec_debug_channel.h by William H. Palmer
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_fast_ethernet.h,v $
 * Revision 3.2  1995/11/17  09:27:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:21  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/19  14:48:00  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug fast ethernet events
 */
EOLS	(debug_fast_ethernet_event_eol, debug_command, DEBUG_FAST_ETHERNET);
KEYWORD	(debug_fast_ethernet_event, debug_fast_ethernet_event_eol, no_alt,
	"events", "Fast Ethernet events", PRIV_OPR);

/******************************************************************
 * debug fast ethernet packets
 */
EOLS	(debug_fast_ethernet_packet_eol, 
	 debug_command, DEBUG_FAST_ETHERNET_PKTS);
KEYWORD	(debug_fast_ethernet_packet, debug_fast_ethernet_packet_eol, 
	 debug_fast_ethernet_event, "packets", "Fast Ethernet packets", 
	 PRIV_OPR);

/******************************************************************
 * debug fast ethernet
 *
 */
KEYWORD_DEBUG(debug_fast_ethernet, debug_fast_ethernet_packet, ALTERNATE,
	      OBJ(pdb,1), fast_ethernet_debug_arr,
	      "fastethernet", "Fast Ethernet interface information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_fast_ethernet
