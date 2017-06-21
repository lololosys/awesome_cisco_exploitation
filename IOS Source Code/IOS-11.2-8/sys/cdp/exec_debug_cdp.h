/* $Id: exec_debug_cdp.h,v 3.2.58.1 1996/04/15 21:13:15 bcole Exp $
 * $Source: /release/112/cvs/Xsys/cdp/exec_debug_cdp.h,v $
 *------------------------------------------------------------------
 * Parser support for CDP debug commands 
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_cdp.h,v $
 * Revision 3.2.58.1  1996/04/15  21:13:15  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:49:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:47:21  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:47:33  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug cdp ip
 *
 */
EOLS	(debug_cdp_ip_eol, debug_command, DEBUG_CDP_IP);
KEYWORD (debug_cdp_ip, debug_cdp_ip_eol, no_alt,
	 "ip", "CDP ip info", PRIV_OPR);

/******************************************************************
 * debug cdp adjacency
 *
 */
EOLS	(debug_cdp_adj_eol, debug_command, DEBUG_CDP_ADJ);
KEYWORD (debug_cdp_adj, debug_cdp_adj_eol, debug_cdp_ip,
	 "adjacency", "CDP neighbor info", PRIV_OPR);

/******************************************************************
 * debug cdp events
 *
 */
EOLS	(debug_cdp_events_eol, debug_command, DEBUG_CDP_EVENTS);
KEYWORD (debug_cdp_events, debug_cdp_events_eol, debug_cdp_adj,
	 "events", "CDP events", PRIV_OPR);

/******************************************************************
 * debug cdp packets
 *
 */
EOLS	(debug_cdp_packets_eol, debug_command, DEBUG_CDP_PACKETS);
KEYWORD (debug_cdp_packets, debug_cdp_packets_eol, debug_cdp_events,
	 "packets", "CDP packet-related information", PRIV_OPR);

/******************************************************************
 * debug cdp
 *
 */
KEYWORD_DEBUG(debug_cdp, debug_cdp_packets, ALTERNATE,
	      OBJ(pdb,1), cdp_debug_arr,
	      "cdp", "CDP information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_cdp


