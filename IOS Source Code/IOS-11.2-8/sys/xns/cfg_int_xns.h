/* $Id: cfg_int_xns.h,v 3.3.48.2 1996/05/30 23:47:50 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_int_xns.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ X N S. H
 * 
 * XNS interface subcommands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_xns.h,v $
 * Revision 3.3.48.2  1996/05/30  23:47:50  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.48.1  1996/03/18  22:52:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/13  02:14:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.16.1  1996/02/20  21:58:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/07  17:45:45  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  19:21:03  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/02  01:42:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:08:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef NOT_YET
/* XXX "xns unnumbered" is not fully implemented.  From Mark Schaefer 10/15/92:
 * XXX this is not fully implemented and we are
 * XXX not planning to support it in any particular time frame. By the same
 * XXX token, that does not mean that we won't support it in any particular
 * XXX time frame. For now we should put it back the way it was (by commenting
 * XXX out rather than removing the code.)
 *
 * Remember to change the link to this command from the previous
 * command when it is implemented.
 */
/***********************************************************************
 * xns unumbered
 * no xns unnumbered
 *
 *  * Serial interfaces only.
 */
EOLS	(ci_xns_unnumbered_eol, xnsif_command, XNSIF_UNNUMBERED);
KEYWORD	(ci_xns_unnumberedkwd, ci_xns_unnumbered_eol, NONE,
	 "unnumbered", "Mark a serial interface as unnumbered", PRIV_CONF);
ASSERT	(ci_xns_unnumbered, ci_xns_unnumberedkwd, no_alt,
	 is_p2p(csb->interface));
#endif NOT_YET

/**********************************************************************
 * xns flood { broadcast { allnets | net-zero } } | specific allnets }
 * no xns flood { broadcast { allnets  | net-zero} } | specific allnets }
 *
 * OBJ(int,1) = XNSIF_BROADCAST  or XNSIF_SPECIFIC
 * OBJ(int,2) = XNSIF_ALLNETS or XNSIF_NETZERO
 */

EOLS	(ci_xns_floodeol, xnsif_command, XNSIF_FLOOD);

/* net-zero (but only if flooding broadcasts) */
KEYWORD_ID(ci_xns_floodnetzerokwd, ci_xns_floodeol, no_alt,
	   OBJ(int,2), XNSIF_NETZERO,
	   "net-zero", "Flood only to the local net (net-zero)", PRIV_CONF|PRIV_SUBIF);
TESTVAR	(ci_xns_floodnetzero, ci_xns_floodnetzerokwd, no_alt,
	 NONE, NONE, NONE, OBJ(int,1), XNSIF_BROADCAST);

/* allnets */
KEYWORD_ID(ci_xns_floodallnets, ci_xns_floodeol, ci_xns_floodnetzero,
	   OBJ(int,2), XNSIF_ALLNETS,
	   "allnets", "Flood to all networks", PRIV_CONF|PRIV_SUBIF);

/* specific */
KEYWORD_ID(ci_xns_floodspecific, ci_xns_floodallnets, no_alt,
	   OBJ(int,1), XNSIF_SPECIFIC,
	   "specific", "Flood specific packets", PRIV_CONF|PRIV_SUBIF);

/* broadcast */
KEYWORD_ID(ci_xns_floodbcast, ci_xns_floodallnets, ci_xns_floodspecific,
	   OBJ(int,1), XNSIF_BROADCAST,
	   "broadcast", "Flood broadcasts", PRIV_CONF|PRIV_SUBIF);

NVGENS	(ci_xns_flood_nvgen, ci_xns_floodbcast, xnsif_command, XNSIF_FLOOD );
KEYWORD (ci_xns_flood, ci_xns_flood_nvgen, no_alt,
	 "flood", "Flood XNS packets", PRIV_CONF|PRIV_SUBIF);

/***********************************************************************
 * xns access-group <number>
 * no xns access-group [<number>]
 */
PARAMS_ALIST  (ci_xns_accessgrp, ci_xns_flood,
	       "access-group",
	       OBJ(int,1), MINXNSACC, MAXEXNSACC, 
	       XNS_MIN_STD_ACL, XNS_MAX_EXT_ACL,
	       xnsif_command, XNSIF_ACCESS,
	       "Apply an access list to output packets",
	       "A valid XNS access list number", PRIV_CONF|PRIV_SUBIF);


/**********************************************************************
 * xns hear-rip [<list>]
 * no xns hear-rip [<list>]
 *
 * OBJ(int,1) = IP access list number
 */
EOLS	(ci_xns_hearrip_eol, xnsif_command, XNSIF_HEARRIP);
NUMBER (ci_xns_hearrip_val, ci_xns_hearrip_eol, ci_xns_hearrip_eol,
	 OBJ(int,1), MINFASTACC, MAXFASTACC, "IP access list number");
NOPREFIX (ci_xns_hearrip_noprefix, ci_xns_hearrip_val, ci_xns_hearrip_eol);
NVGENS	(ci_xns_hearripnvgen, ci_xns_hearrip_noprefix, xnsif_command, 
	 XNSIF_HEARRIP);
KEYWORD	(ci_xns_hearrip, ci_xns_hearripnvgen, ci_xns_accessgrp,
	 "hear-rip", "Listen to RIP updates", PRIV_CONF|PRIV_SUBIF);


/***********************************************************************
 * xns old-3com
 * no xns old-3com
 */
EOLS	(ci_xns_old3com_eol, xnsif_command, XNSIF_OLD3COM);
KEYWORD	(ci_xns_old3comkwd, ci_xns_old3com_eol, NONE,
	 "old-3com", "XNS on Token Ring", PRIV_CONF|PRIV_SUBIF);
TEST_IDBSTATUS(ci_xns_old3com, ci_xns_old3comkwd, NONE, ci_xns_hearrip, IDB_TR);


/***********************************************************************
 * xns update-time <seconds>
 * no xns update-time [<seconds>]
 */
PARAMS_KEYONLY (ci_xns_updtimer, ci_xns_old3com, 
	     "update-time", OBJ(int,1), 10, -1L, 
	     xnsif_command, XNSIF_UPDATETIME,
	     "Set XNS routing update timer",
	     "Routing update timer", PRIV_CONF|PRIV_SUBIF);

/***********************************************************************
 * xns router-filter <number>
 * no xns router-filter [<number>]
 */

EOLS (ci_xns_rtfilter_eol, xnsif_command, XNSIF_ROUTER_FILTER);

ACCESS_LIST(ci_xns_rtfilter_number, ci_xns_rtfilter_eol, no_alt, OBJ(int,1),
	MINXNSACC, MAXEXNSACC, XNS_MIN_STD_ACL, XNS_MAX_EXT_ACL,
        "A valid XNS access list number");

NOPREFIX (ci_xns_rtfilter_noprefix, ci_xns_rtfilter_number,
	  ci_xns_rtfilter_eol);

NVGENS (ci_xns_rtfilter_nvgen, ci_xns_rtfilter_noprefix, xnsif_command,
	XNSIF_ROUTER_FILTER);

KEYWORD_MM (ci_xns_rtfilter, ci_xns_rtfilter_nvgen, ci_xns_updtimer,
	    "router-filter", "Filter sources of routing updates",
	    PRIV_CONF|PRIV_SUBIF, 6);

/***********************************************************************
 * xns output-network-filter <number>
 * no xns output-network-filter [<number>]
 */
PARAMS_ALIST   (ci_xns_outnetfilter, ci_xns_rtfilter, 
		"output-network-filter", OBJ(int,1), MINXNSACC, MAXEXNSACC,
		XNS_MIN_STD_ACL, XNS_MAX_EXT_ACL,
		xnsif_command, XNSIF_OUT_FILTER,
		"Filter outgoing routing updates", 
		"A valid XNS access list number", PRIV_CONF|PRIV_SUBIF);


/***********************************************************************
 * xns input-network-filter <number>
 * no xns input-network-filter [<number>]
 */
PARAMS_ALIST (ci_xns_innetfilter, ci_xns_outnetfilter, 
		"input-network-filter",
		OBJ(int,1), MINXNSACC, MAXEXNSACC,
		XNS_MIN_STD_ACL, XNS_MAX_EXT_ACL,
		xnsif_command, XNSIF_IN_FILTER,
		"Filter incoming routing updates",
		"A valid XNS access list number", PRIV_CONF|PRIV_SUBIF);

/**********************************************************************
 * [no] xns encapsulation [{ arpa | snap | ub | 3com }]
 *
 * OBJ(int,1) = encapsulation type
 *
 * The 'no' version sets encapsulation to 'arpa' for ethernet interfaces
 * and 'snap' for other 802.X interfaces.
 */

EOLS	(ci_xns_encap_eol, xnsif_command, XNSIF_ENCTYPE);
KEYWORD_ID(ci_xns_encap_3com, ci_xns_encap_eol, no_alt,
	   OBJ(int,1), ET_3COM_TR,
	   "3com-tr", "3-Com encapsulation (Token Ring only)", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_xns_encap_ub, ci_xns_encap_eol, ci_xns_encap_3com,
	   OBJ(int,1), ET_UB_TR,
	   "ub-tr", "Ungermann-Bass encapsulation (Token Ring only)", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_xns_encap_snap, ci_xns_encap_eol, ci_xns_encap_ub,
	   OBJ(int,1), ET_SNAP,
	   "snap", "SNAP encapsulation (Token Ring only)", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_xns_encap_arpa, ci_xns_encap_eol, ci_xns_encap_snap,
	    OBJ(int,1), ET_ARPA,
	    "arpa", "ARPA encapsulation (Ethernet only)", PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_xns_encap_noprefix, ci_xns_encap_arpa, ci_xns_encap_eol);
NVGENS	(ci_xns_nvgen, ci_xns_encap_noprefix, xnsif_command, XNSIF_ENCTYPE);
KEYWORD	(ci_xns_encap, ci_xns_nvgen, ci_xns_innetfilter,
	 "encapsulation", "Set encapsulation method for XNS packets", PRIV_CONF|PRIV_SUBIF);

/***********************************************************************
 * xns route-cache
 * no xns route-cache
 */
EOLS	(ci_xns_rcache_eol, xnsif_command, XNSIF_ROUTEC);
KEYWORD_MM (ci_xns_rcache, ci_xns_rcache_eol, ci_xns_encap,
	    "route-cache", "Enable fast switching", PRIV_CONF|PRIV_SUBIF, 6);

/**********************************************************************
 * xns helper-address <net.host>
 * no xns helper-address [<net.host>]
 */
EOLS	(ci_xns_helperaddr_eol, xnsif_command, XNSIF_HELPER);
XNSADDR	(ci_xns_helperaddr_addr, ci_xns_helperaddr_eol, no_alt, 
	 OBJ(paddr,1), "Host address to receive helper forwarded packets");
NOPREFIX (ci_xns_nohelperaddr, ci_xns_helperaddr_addr, ci_xns_helperaddr_eol);
NVGENS	(ci_xns_helperaddr_nvgen, ci_xns_nohelperaddr, xnsif_command,
	 XNSIF_HELPER);
KEYWORD	(ci_xns_helperaddr, ci_xns_helperaddr_nvgen, ci_xns_rcache,
	 "helper-address", "Forward broadcasts to a specific address", PRIV_CONF|PRIV_SUBIF);

/***********************************************************************
 * xns network <net>
 * no xns network [<net>]
 */
PARAMS_KEYONLY(ci_xns_network, ci_xns_helperaddr,
	       "network", OBJ(int,1), 1, -2L, 
	       xnsif_command, XNSIF_NETWORK,
	       "Assign an XNS network & enable XNS routing",
	       "Network number", PRIV_CONF|PRIV_SUBIF);

/***********************************************************************/
KEYWORD	(ci_xns, ci_xns_network, ALTERNATE,
	 "xns", "XNS interface subcommands", PRIV_CONF|PRIV_SUBIF);

#undef  ALTERNATE
#define ALTERNATE ci_xns
