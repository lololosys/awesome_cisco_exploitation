/* $Id: exec_debug_ip_egp.h,v 3.3 1996/02/01 23:34:05 widmer Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/exec_debug_ip_egp.h,v $
 *------------------------------------------------------------------
 * Egp debugging commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_egp.h,v $
 * Revision 3.3  1996/02/01  23:34:05  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:33:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/19  10:50:30  tli
 * CSCdi36065:  Rationalize EGP parser file placement
 * Move parser support to iprouting subdirectory.
 *
 * Revision 2.1  1995/06/07  22:18:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug ip egp events [<ip-address>]
 *
 * OBJ(paddr,1) = ip-address
 * OBJ(int,1) = TRUE if ip-address is set, else FALSE
 */
EOLS	(debug_ip_egp_events_eol, iprouting_debug_command, DEBUG_EGPEVENT);

SET	(debug_ip_egp_events_true, debug_ip_egp_events_eol,
	 OBJ(int,1), TRUE);
IPADDR	(debug_ip_egp_events_addr, debug_ip_egp_events_true,
	 debug_ip_egp_events_eol,
	 OBJ(paddr,1), "IP address");

KEYWORD (debug_ip_egp_events, debug_ip_egp_events_addr, no_alt,
	 "events", "Major EGP transactions", PRIV_OPR);

/******************************************************************
 * debug ip egp transactions [<ip-address>]
 *
 * OBJ(paddr,1) = ip-address
 * OBJ(int,1) = TRUE if ip-address is set, else FALSE
 */
EOLS	(debug_ip_egp_eol, iprouting_debug_command, DEBUG_EGP);

SET	(debug_ip_egp_true, debug_ip_egp_eol, OBJ(int,1), TRUE);
IPADDR	(debug_ip_egp_ipaddr, debug_ip_egp_true, debug_ip_egp_eol,
	 OBJ(paddr,1), "IP address");

KEYWORD (debug_ip_egp_trans, debug_ip_egp_ipaddr, debug_ip_egp_events,
	 "transactions", "EGP transactions", PRIV_OPR);

/******************************************************************
 * debug ip egp
 *
 */
KEYWORD (debug_ip_egp, debug_ip_egp_trans, ALTERNATE,
	 "egp", "EGP information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_egp
