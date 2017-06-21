/* $Id: exec_clear_apple.h,v 3.3.20.1 1996/05/21 09:42:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/exec_clear_apple.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ A P P L E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_apple.h,v $
 * Revision 3.3.20.1  1996/05/21  09:42:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/02/01  23:32:53  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  08:44:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/02  01:43:39  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:17:28  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:15:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS (clear_apple_extend_here, no_alt);

/******************************************************************
 * clear apple traffic
 *
 */
EOLS	(clear_apple_traffic_eol, clear_appletalk, CLEAR_APPLE_TRAFFIC);
KEYWORD (clear_apple_traffic, clear_apple_traffic_eol, clear_apple_extend_here,
	 "traffic", "Reset AppleTalk traffic counters", PRIV_OPR);

/******************************************************************
 * clear apple route-cache
 *
 */
EOLS	(clear_apple_route_cache_eol, clear_appletalk,
	 CLEAR_APPLE_ROUTE_CACHE);
KEYWORD_MM (clear_apple_route_cache, clear_apple_route_cache_eol,
  	    clear_apple_traffic, "route-cache",
	    "Reset AppleTalk fast-switching cache(s)", PRIV_OPR, 6);

/******************************************************************
 * clear apple route <network>
 *
 * OBJ(int,1) = network #
 */
EOLS	(clear_apple_route_eol, clear_appletalk, CLEAR_APPLE_ROUTE);
NUMBER	(clear_apple_route_network, clear_apple_route_eol, no_alt,
	 OBJ(int,1), 1, 65535, "Network number");
KEYWORD (clear_apple_route, clear_apple_route_network, clear_apple_route_cache,
	 "route", "Reset AppleTalk routing table", PRIV_OPR);

/******************************************************************
 * clear apple neighbor <neighbor-address>|"all" [state]
 *
 * OBJ(int,1)	= Address of neighbor
 * OBJ(int,2)	= T if "all" specified
 * OBJ(int,3)	= T if "state" specified
 *
 */
EOLS	(clear_apple_nbr_eol, clear_appletalk, CLEAR_APPLE_NEIGHBOR);
KEYWORD_ID (clear_apple_nbr_state, clear_apple_nbr_eol, clear_apple_nbr_eol,
	    OBJ(int,3), TRUE,
	    "state", "Clear AppleTalk neighbor reachability state", PRIV_OPR);
SET	(clear_apple_nbr_set1, clear_apple_nbr_state, OBJ(int,1), TRUE);
KEYWORD_ID (clear_apple_nbr_all, clear_apple_nbr_state, no_alt,
	    OBJ(int,2), TRUE,
	    "all", "Clear all AppleTalk neighbors in down state", PRIV_OPR);
ATADDR  (clear_apple_nbr_addr, clear_apple_nbr_set1,
	 clear_apple_nbr_all, OBJ(paddr,1), "AppleTalk address of neighbor");
KEYWORD (clear_apple_nbr, clear_apple_nbr_addr,
	 clear_apple_route,
	 "neighbor", "Reset AppleTalk down neighbor information", PRIV_OPR);

/******************************************************************
 * clear apple nbp [<object-address>]
 *
 * OBJ(paddr,1) = Interface of neighbors to get cleared
 *		  if 0, then all entries will be cleared
 * OBJ(int,1)	= T if address was spec'd.
 */
EOLS	(clear_apple_nbp_eol, clear_appletalk, CLEAR_APPLE_NBP);
SET	(clear_apple_nbp_set1, clear_apple_nbp_eol, OBJ(int,1), TRUE);
ATADDR  (clear_apple_nbp_addr, clear_apple_nbp_set1, clear_apple_nbp_eol,
	 OBJ(paddr,1), "AppleTalk address of NBP entry");
KEYWORD (clear_apple_nbp, clear_apple_nbp_addr, clear_apple_nbr,
	 "nbp", "Reset AppleTalk NBP cache information", PRIV_OPR);

/******************************************************************
 * clear apple interface <interface-unit>
 *
 * OBJ(idb,1)   = Interface of neighbors to get cleared
 */
EOLS (clear_apple_interface_eol, clear_appletalk, CLEAR_APPLE_INTERFACE);
INTERFACE (clear_apple_interface_int, clear_apple_interface_eol,
	   no_alt, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (clear_apple_interface, clear_apple_interface_int, clear_apple_nbp,
	 common_str_interface, "Clear & restart AppleTalk interface", PRIV_OPR);


/******************************************************************
 * clear apple arp [<arp-entry-addr>]
 *
 * OBJ(int,1) = FALSE = no arp-entry-addr
 *	        TRUE  = arp-entry-addr set
 * (*OBJ(paddr,1)) = arp-entry-addr
 *
 */
EOLS	(clear_apple_arp_eol, clear_appletalk, CLEAR_APPLE_ARP);
SET	(clear_apple_arp_set1, clear_apple_arp_eol, OBJ(int,1), TRUE);
ATADDR	(clear_apple_arp_addr, clear_apple_arp_set1, clear_apple_arp_eol,
	 OBJ(paddr,1), "AppleTalk address of entry");
KEYWORD (clear_apple_arp, clear_apple_arp_addr, clear_apple_interface,
	 "arp", "Reset AppleTalk information", PRIV_OPR);

/******************************************************************/
KEYWORD (clear_apple, clear_apple_arp, ALTERNATE,
	 "appletalk", "Reset AppleTalk information", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	clear_apple
