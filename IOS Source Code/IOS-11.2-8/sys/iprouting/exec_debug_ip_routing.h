/* $Id: exec_debug_ip_routing.h,v 3.3.20.1 1996/06/27 04:40:57 mikel Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_debug_ip_routing.h,v $
 *------------------------------------------------------------------
 * IP routing debug commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_routing.h,v $
 * Revision 3.3.20.1  1996/06/27  04:40:57  mikel
 * CSCdi37749:  <debug ip eigrp> help wrong
 * Branch: California_branch
 * clean up ip-eigrp AS debugging
 *
 * Revision 3.3  1996/02/01  23:34:07  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:33:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/01  12:16:15  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:18:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ip routing
 *
 */
EOLS	(debug_ip_routing_eol, iprouting_debug_command, DEBUG_ROUTING);
NUMBER	(debug_ip_routing_list, debug_ip_routing_eol,
	 debug_ip_routing_eol, OBJ(int, 1), 1, MAXSLOWACC, "Access list");
KEYWORD (debug_ip_routing, debug_ip_routing_list, ALTERNATE,
	 "routing", "Routing table events", PRIV_OPR);

/******************************************************************
 * debug ip igrp events [<ip-address>]
 *
 * OBJ(paddr,1) = ip-address of neighbor
 * OBJ(int,1) = count of addresses present
 */
EOLS	(debug_ip_igrp_events_eol, iprouting_debug_command, DEBUG_IGRP_EVENTS);
SET	(debug_ip_igrp_events_one, debug_ip_igrp_events_eol,
	 OBJ(int,1), 1);
IPADDR	(debug_ip_igrp_events_addr, debug_ip_igrp_events_one,
	 debug_ip_igrp_events_eol,
	 OBJ(paddr,1), "IP address of neighbor");
KEYWORD (debug_ip_igrp_events, debug_ip_igrp_events_addr, no_alt,
	 "events", "IGRP protocol events", PRIV_OPR);

/******************************************************************
 * debug ip igrp transactions [<ip-address> [<ip-address>]]
 *
 * OBJ(paddr,1) = ip-address of neighbor
 * OBJ(paddr,2) = ip-address of target network
 * OBJ(int,1) = count of addresses present
 */
EOLS	(debug_ip_igrp_eol, iprouting_debug_command, DEBUG_IGRP);
SET	(debug_ip_igrp_two, debug_ip_igrp_eol, OBJ(int,1), 2);
IPADDR	(debug_ip_igrp_target, debug_ip_igrp_two, debug_ip_igrp_eol,
	 OBJ(paddr,2), "Target route");
SET	(debug_ip_igrp_one, debug_ip_igrp_target, OBJ(int,1), 1);
IPADDR	(debug_ip_igrp_addr, debug_ip_igrp_one, debug_ip_igrp_eol,
	 OBJ(paddr,1), "IP address of neighbor");
KEYWORD	(debug_ip_igrp_trans, debug_ip_igrp_addr, debug_ip_igrp_events,
	 "transactions", "IGRP protocol transactions", PRIV_OPR);

/******************************************************************
 * debug ip igrp
 *
 */
KEYWORD (debug_ip_igrp, debug_ip_igrp_trans, debug_ip_routing,
	 "igrp", "IGRP information", PRIV_OPR);

/******************************************************************
 * debug ip cache
 *
 */
EOLS	(debug_ip_cache_eol, iprouting_debug_command, DEBUG_IPCACHE);
NUMBER	(debug_ip_cache_list, debug_ip_cache_eol,
	 debug_ip_cache_eol, OBJ(int, 1), 1, MAXFASTACC, "Access list");
KEYWORD (debug_ip_cache, debug_ip_cache_list, debug_ip_igrp,
	 "cache", "IP cache operations", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_cache
