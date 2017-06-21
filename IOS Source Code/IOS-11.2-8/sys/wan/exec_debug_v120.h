/* $Id: exec_debug_v120.h,v 3.2 1995/11/17 18:01:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/exec_debug_v120.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ V 1 2 0 . H
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_v120.h,v $
 * Revision 3.2  1995/11/17  18:01:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:35:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  23:30:22  wmay
 * Placeholder for CS_Arkansas_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * debug v120 event
 */

EOLS    (debug_v120_event_eol, debug_command, DEBUG_V120_EVENT);
KEYWORD (debug_v120_event, debug_v120_event_eol, no_alt,
	 "event", "Protocol events", PRIV_OPR);

/*
 * debug v120 packet
 */

EOLS    (debug_v120_packet_eol, debug_command, DEBUG_V120_PACKET);
KEYWORD (debug_v120_packet, debug_v120_packet_eol, debug_v120_event,
	 "packet", "Low-level V120 packet dump", PRIV_OPR);

/******************************************************************
 * debug v120
 */

KEYWORD_DEBUG(debug_v120, debug_v120_packet, ALTERNATE,
	      OBJ(pdb,1), v120_debug_arr,
	      "v120", "V120 information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_v120
