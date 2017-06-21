/* $Id: exec_debug_atm_dxi.h,v 3.2 1995/11/17 18:01:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/exec_debug_atm_dxi.h,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 *  October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_atm_dxi.h,v $
 * Revision 3.2  1995/11/17  18:01:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:47:18  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug atm dxi packet
 */
EOLS    (debug_atm_dxi_packet_eol, debug_command, DEBUG_ATM_DXI_PACKET);
KEYWORD (debug_atm_dxi_packet, debug_atm_dxi_packet_eol, no_alt,
	 "packet", "ATM DXI packets", PRIV_OPR);

/******************************************************************
 * debug atm dxi events
 */
EOLS	(debug_atm_dxi_events_eol, debug_command, DEBUG_ATM_DXI_EVENTS);
KEYWORD (debug_atm_dxi_events, debug_atm_dxi_events_eol,
	 debug_atm_dxi_packet, "events", "Important atm-dxi events", PRIV_OPR);

/******************************************************************
 * debug atm dxi
 */
KEYWORD_DEBUG(debug_atm_dxi, debug_atm_dxi_events, ALTERNATE,
	      OBJ(pdb,1), atm_dxi_debug_arr,
	      "dxi", "atm-dxi information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_atm_dxi
