/* $Id: exec_debug_ip_rip.h,v 3.2 1995/11/17 17:33:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/exec_debug_ip_rip.h,v $
 *------------------------------------------------------------------
 * IP RIP debug commands
 *
 * Feburary 1995, Paul Traina
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_rip.h,v $
 * Revision 3.2  1995/11/17  17:33:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/11  19:37:59  pst
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ip rip
 * debug ip rip events
 */
EOLS	(debug_ip_rip_eol, iprouting_debug_command, DEBUG_RIP);
EOLS	(debug_ip_rip_events_eol, iprouting_debug_command, DEBUG_RIP_EVENTS);
KEYWORD (debug_ip_rip_events, debug_ip_rip_events_eol, debug_ip_rip_eol,
	 "events", "RIP protocol events", PRIV_OPR);
KEYWORD (debug_ip_rip, debug_ip_rip_events, ALTERNATE,
	 "rip", "RIP protocol transactions", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_rip
