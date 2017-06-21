/* $Id: exec_debug_vpn.h,v 1.2.40.2 1996/05/12 19:22:16 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/exec_debug_vpn.h,v $
 *------------------------------------------------------------------
 * exec_debug_vpn.h  --     debugging commands for VPDN
 *
 * January 1996, Shoou Yiu
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_vpn.h,v $
 * Revision 1.2.40.2  1996/05/12  19:22:16  tkolar
 * CSCdi57472:  vpn is an overloaded acronym
 * Branch: California_branch
 * Change "vpn" to "vpdn", and make a few other user interface tweaks
 * while we're at it.
 *
 * Revision 1.2.40.1  1996/04/27  06:34:10  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.2  1996/03/12  04:55:26  tkolar
 * Branch: Dial1_branch
 * Vastly expand L2F comments, add two new debugging variables, and make
 * wraparound sequence numbers fully understood.
 *
 * Revision 1.2.2.1  1996/02/13  23:07:45  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN.
 *
 * Revision 1.2  1996/01/31  18:38:28  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:06  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug vpdn error
 */
EOLS    (debug_vpdn_error_eol, debug_command, DEBUG_VPN_ERROR);
KEYWORD (debug_vpdn_error, debug_vpdn_error_eol, no_alt,
	 "error", "VPDN Protocol errors", PRIV_OPR);
/*
 * debug vpdn packet
 */

EOLS    (debug_vpdn_packet_eol, debug_command, DEBUG_VPN_PACKET);
KEYWORD (debug_vpdn_packet, debug_vpdn_packet_eol, debug_vpdn_error,
	 "packet", "VPDN packet", PRIV_OPR);

/*
 * debug vpdn event
 */

EOLS    (debug_vpdn_event_eol, debug_command, DEBUG_VPN_EVENT);
KEYWORD (debug_vpdn_event, debug_vpdn_event_eol, debug_vpdn_packet,
	 "event", "VPDN event", PRIV_OPR);

/*
 * debug vpdn l2f
 */

EOLS    (debug_vpdn_l2f_packet_eol, debug_command, DEBUG_VPN_L2F_PACKET);
KEYWORD (debug_vpdn_l2f_packet, debug_vpdn_l2f_packet_eol, debug_vpdn_event,
	 "l2f-packets", "L2F protocol packets", PRIV_OPR);

EOLS    (debug_vpdn_l2f_event_eol, debug_command, DEBUG_VPN_L2F_EVENT);
KEYWORD (debug_vpdn_l2f_event, debug_vpdn_l2f_event_eol, debug_vpdn_l2f_packet,
	 "l2f-events", "L2F protocol events", PRIV_OPR);

EOLS    (debug_vpdn_l2f_error_eol, debug_command, DEBUG_VPN_L2F_ERROR);
KEYWORD (debug_vpdn_l2f_error, debug_vpdn_l2f_error_eol, debug_vpdn_l2f_event,
	 "l2f-errors", "L2F protocol errors", PRIV_OPR);


/******************************************************************
 * debug vpdn
 */
KEYWORD_DEBUG(debug_vpdn, debug_vpdn_l2f_error, ALTERNATE,
	      OBJ(pdb,1), vpn_debug_arr,
	      "vpdn", "VPDN information", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	debug_vpdn
