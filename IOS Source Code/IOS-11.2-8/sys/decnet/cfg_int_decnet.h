/* $Id: cfg_int_decnet.h,v 3.2.60.2 1996/08/12 16:01:04 widmer Exp $
 * $Source: /release/112/cvs/Xsys/decnet/cfg_int_decnet.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ D E C N E T . H
 *
 * DECnet interface subcommands
 *
 * OBJ(int,1) is the ATG network number (default -1 to indicate that
 *	the net number from the idb should be used)
 * OBJ(int,2) is any numeric parameter specified in the command
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_decnet.h,v $
 * Revision 3.2.60.2  1996/08/12  16:01:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/05/30  23:42:32  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:58:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/20  19:03:06  asastry
 * CSCdi41453:  Request for new knob: no decnet split-horizon (for frame
 * relay)
 *
 * Revision 2.1  1995/07/02  01:44:25  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	decnet routing-timer <value>
 *	no decnet routing-timer [<value>]
 */

PARAMS_KEYONLY
	(ci_decnet_rtimer, no_alt, "routing-timer", 
	 OBJ(int,2), 1, 65535,
	 decnetif_command, DC_ROUTE_TIME,
	 "Set interval between transmitted routing messages", "Seconds",
	 PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	decnet router-priority <value>
 *	no decnet router-priority [<value>]
 */

PARAMS_KEYONLY
	(ci_decnet_rprio, ci_decnet_rtimer, "router-priority", 
	 OBJ(int,2), 0, 127,
	 decnetif_command, DC_ROUTER_PRIORITY,
	 "Set priority for determining default router", "Priority value",
	 PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	decnet congestion-threshold
 *	no decnet congestion-threshold
 */

PARAMS_KEYONLY
	(ci_decnet_thresh, ci_decnet_rprio, "congestion-threshold", 
	 OBJ(int,2), 0, 0x7fff,
	 decnetif_command, DC_CONGEST_THRESH, 
	 "Congestion avoidance threshold", "Congestion value", PRIV_CONF);

/**********************************************************************
 *	decnet route-cache			
 *	no decnet route-cache
 */

EOLS	(ci_decnet_rcache_1, decnetif_command, DC_ROUTE_CACHE);
KEYWORD	(ci_decnet_rcache, ci_decnet_rcache_1, ci_decnet_thresh,
	 "route-cache", "Enable fast-switching of DECnet datagrams",
	 PRIV_CONF);

/**********************************************************************
 *	decnet split-horizon
 *	no decnet split-horizon
 */

EOLS	(ci_decnet_split_horizon_1, decnetif_command, DC_SPLIT_HORIZON);
KEYWORD	(ci_decnet_split_horizon, ci_decnet_split_horizon_1, ci_decnet_rcache,
	 "split-horizon", "Enable split horizon on interface",
	 PRIV_CONF);

/**********************************************************************
 *	decnet out-routing-filter <list>
 *	no decnet out-routing-filter [<list>]
 */

PARAMS_ALIST
	(ci_decnet_outrf, ci_decnet_split_horizon, "out-routing-filter", 
	 OBJ(int,2), MINDECACC, MAXDECACC, DN_MIN_ACL, DN_MAX_ACL,
	 decnetif_command, DC_OUT_FILTER,
	 "Set up access control for outgoing routing information",
	 "DECnet access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	decnet in-routing-filter <list>
 *	no decnet in-routing-filter [<list>]
 */

PARAMS_ALIST
	(ci_decnet_inrf, ci_decnet_outrf, "in-routing-filter", 
	 OBJ(int,2), MINDECACC, MAXDECACC, DN_MIN_ACL, DN_MAX_ACL,
	 decnetif_command, DC_IN_FILTER,
	 "Set up access control for incoming routing information",
	 "DECnet access list", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	decnet multicast-map <type> <address>
 *	no decnet multicast-map <type> 
 */

EOLS	(ci_decnet_mumap_eol, decnetif_command, DC_MULTI_MAP);
ENADDR  (ci_decnet_mumap_addr, ci_decnet_mumap_eol, no_alt, 
	 OBJ(hwaddr,1), "MAC address");
NOPREFIX (ci_decnet_mumap_noprefix, ci_decnet_mumap_addr,
	  ci_decnet_mumap_eol);
KEYWORD_ID (ci_decnet_mumap_allendnodes, ci_decnet_mumap_noprefix,
	    no_alt,
	    OBJ(int,2), DNE_ALLENDNODES,
	    "iv-all-endnodes", "All Phase-IV Endnodes", PRIV_CONF);
KEYWORD_ID (ci_decnet_mumap_allprimerouters, ci_decnet_mumap_noprefix,
	    ci_decnet_mumap_allendnodes,
	    OBJ(int,2), DNE_ALLPRIMEROUTERS,
	    "iv-prime-all-routers", "All Phase-IV-Prime Routers", PRIV_CONF);
KEYWORD_ID (ci_decnet_mumap_allrouters, ci_decnet_mumap_noprefix,
	    ci_decnet_mumap_allprimerouters,
	    OBJ(int,2), DNE_ALLROUTERS,
	    "iv-all-routers", "All Phase-IV Routers", PRIV_CONF);
KEYWORD (ci_decnet_mumap, ci_decnet_mumap_allrouters, ci_decnet_inrf,
	 "multicast-map", "DECnet multicast mapping on token ring", PRIV_CONF);

/**********************************************************************
 *	decnet encapsulation {dec|pre-dec}
 *	no decnet encapsulation
 *      OBSOLETE
 */
EOLS	(ci_decnet_encaps_eol, decnetif_command, DC_ENCAPSULATION);
KEYWORD_ID(ci_decnet_encaps_predec, ci_decnet_encaps_eol, no_alt,
	   OBJ(int,2), DNE_CISCO,
	   "pre-dec", "Non-standard encapsulation for pre-9.1 Cisco routers", 
	   PRIV_CONF);
KEYWORD_ID(ci_decnet_encaps_dec, ci_decnet_encaps_eol, ci_decnet_encaps_predec,
	   OBJ(int,2), DNE_DEC,
	   "dec", "Standard encapsulation", PRIV_CONF);
NOPREFIX(ci_decnet_encaps_noprefix, ci_decnet_encaps_dec,
	 ci_decnet_encaps_eol);
KEYWORD	(ci_decnet_encaps, ci_decnet_encaps_noprefix, ci_decnet_mumap,
	 "encapsulation", "DECnet encapsulation on token ring", PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 *	decnet hello-timer <value>		
 *	no decnet hello-timer [<value>]
 */

PARAMS_KEYONLY
	(ci_decnet_htimer, ci_decnet_encaps, "hello-timer", 
	 OBJ(int,2), 1, 8191,
	 decnetif_command, DC_HELLO_TIME,
	 "Set interval between transmitted HELLO messages", "Seconds",
	 PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	decnet cost <value>
 *	no decnet cost [<value>]
 */

PARAMS_KEYONLY
	(ci_decnet_cost, ci_decnet_htimer, "cost", 
	 OBJ(int,2), 1, 63,
	 decnetif_command, DC_COST, 
	 "Set cost value for interface", "Cost value", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 *	decnet access-group <list>
 *	no decnet access-group [<list>]
 */

PARAMS_ALIST
	(ci_decnet_agrp, ci_decnet_cost, "access-group", 
	 OBJ(int,2), MINDECACC, MAXDECACC, DN_MIN_ACL, DN_MAX_ACL,
	 decnetif_command, DC_ACCESS_GROUP,
	 "Set access control for outgoing packets", "DECnet access list",
	 PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 * 
 * Optional ATG network number.  The default is -1 which indicates
 * that we use the net number from the idb.
 */
SET	(ci_decnet_atg_default, ci_decnet_agrp, OBJ(int,1), -1);
NUMBER	(ci_decnet_atg, ci_decnet_agrp, ci_decnet_atg_default,
	 OBJ(int,1), 0, 3, "ATG network number");

/* NVGEN from here to allow the atg number to be checked in nv_decnetif() */
NVGENNS	(ci_decnet_atg_nvgen, ci_decnet_atg, decnetif_command);

KEYWORD	(ci_decnet, ci_decnet_atg_nvgen, NONE, "decnet",
	 "Interface DECnet config commands", PRIV_CONF|PRIV_SUBIF);

ASSERT  (ci_decnet_subif, ci_decnet, ALTERNATE,
	 main_if_or_vc(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	ci_decnet_subif
