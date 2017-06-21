/* $Id: exec_debug_ip_bgp.h,v 3.3.20.1 1996/04/05 03:46:32 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_debug_ip_bgp.h,v $
 *------------------------------------------------------------------
 * BGP debug commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_bgp.h,v $
 * Revision 3.3.20.1  1996/04/05  03:46:32  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.3  1996/02/01  23:34:04  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:33:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:13  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:18:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug ip bgp
 *
 */
EOLS	(debug_ip_bgp_trans_eol, iprouting_debug_command, DEBUG_BGP);

/******************************************************************
 * debug ip bgp updates
 *
 * OBJ(int,1) = list
 */
EOLS	(debug_ip_bgp_updates_eol, iprouting_debug_command, DEBUG_BGPUPDATE);
NUMBER	(debug_ip_bgp_updates_list, debug_ip_bgp_updates_eol,
	 debug_ip_bgp_updates_eol, OBJ(int,1), 1, MAXSLOWACC, "Access list");
KEYWORD	(debug_ip_bgp_updates, debug_ip_bgp_updates_list,
	 debug_ip_bgp_trans_eol,
	 "updates", "BGP updates", PRIV_OPR);

/******************************************************************
 * debug ip bgp dampening
 *
 * OBJ(int,1) = list
 */
EOLS	(debug_ip_bgp_damp_eol, iprouting_debug_command, DEBUG_BGPDAMP);
NUMBER	(debug_ip_bgp_damp_list, debug_ip_bgp_damp_eol, debug_ip_bgp_damp_eol, 
	 OBJ(int,1), 1, MAXSLOWACC, "Access list");
SET	(debug_ip_bgp_damp_set, debug_ip_bgp_damp_list, OBJ(int,1), 0);
KEYWORD (debug_ip_bgp_damp, debug_ip_bgp_damp_set, debug_ip_bgp_updates,
	 "dampening", "BGP dampening", PRIV_OPR);

/******************************************************************
 * debug ip bgp keepalives
 *
 */
EOLS	(debug_ip_bgp_keep_eol, iprouting_debug_command, DEBUG_BGPKEEP);
KEYWORD (debug_ip_bgp_keep, debug_ip_bgp_keep_eol, debug_ip_bgp_damp,
	 "keepalives", "BGP keepalives", PRIV_OPR);

/******************************************************************
 * debug ip bgp events
 *
 */
EOLS	(debug_ip_bgp_events_eol, iprouting_debug_command, DEBUG_BGPEVENT);
KEYWORD (debug_ip_bgp_events, debug_ip_bgp_events_eol, debug_ip_bgp_keep,
	 "events", "BGP events", PRIV_OPR);

KEYWORD (debug_ip_bgp, debug_ip_bgp_events, ALTERNATE,
	 "bgp", "BGP information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_bgp
