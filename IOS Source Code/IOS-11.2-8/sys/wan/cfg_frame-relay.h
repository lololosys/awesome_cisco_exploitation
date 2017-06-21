/* $Id: cfg_frame-relay.h,v 3.3.28.2 1996/05/30 23:46:35 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_frame-relay.h,v $
 *------------------------------------------------------------------
 * C F G _ F R A M E - R E L A Y . H
 *
 * Global configuration commands for frame relay.
 *
 * Summer 92, Sanjeev Newarikar
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_frame-relay.h,v $
 * Revision 3.3.28.2  1996/05/30  23:46:35  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/05/21  10:09:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/01/18  02:15:52  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  18:00:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:46  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/23  19:02:39  widmer
 * CSCdi36361:  frame-relay de-list protocol command not modular
 * Change to use PROTONAME
 *
 * Revision 2.1  1995/06/07  22:03:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*******************************************************************
 * frame-relay switching
 * [no] frame-relay switching
 */
EOLS (cfg_fr_switching_eol, frame_relay_global_command, FRAME_RELAY_SWITCHING);
/*
NOPREFIX (cfg_fr_switching_noprefix, cfg_fr_switching_eol, 
	  no_alt);
NVGENS (cfg_fr_switching_nvgen, cfg_fr_switching_noprefix,
	frame_relay_global_command, FRAME_RELAY_SWITCHING);
KEYWORD (cfg_fr_switching, cfg_fr_switching_nvgen, NONE,
	 "switching", "enable frame relay pvc switching", PRIV_CONF);
*/
KEYWORD (cfg_fr_switching, cfg_fr_switching_eol, no_alt,
	 "switching", "enable frame relay pvc switching", PRIV_CONF);

EOLS	(fr_pr_eol, frame_relay_global_command, FR_LISTNUM);

/**********************************************************************
 *
 *
 * [<args>]
 * Valid arguments are:
 *
 *	gt <byte-count>
 *	lt <byte-count>
 *	list <access-list>
 *	list <ethernet-typecode-list>
 *
 * OBJ(int,5) = FR_PR_GT, FR_PR_LT, FR_PRIOR_LIST
 * OBJ(int,6) = <byte-count>, <access-list>, <ethernet-typecode-list>
 *
 * First we check if we're parsing the 'de-list <list> protocol' 
 * command.  Then we check for BRIDGE, XNS, VINES, NOVELL, or APPLETALK
 * protocol for the 'list' argument.  The 'gt' and 'lt' arguments are valid
 * for all other protocols.  Also check all other extensions.
 *
 * All other commands transition to fr_pr_eol.
 */

/* list <access-list>
 * list <ethernet-typecode-list>
 * These lists are only valid for 'bridge,' 'xns,' 'vines,' 'novell,'
 * and 'appletalk' protocols.
 */
NUMBER	(fr_pr_vineslistno, fr_pr_eol, no_alt,
	 OBJ(int, 6), VINESMINACC, VINESMAXACC, "Vines access list");
EQUAL	(fr_pr_vineslistcheck, fr_pr_vineslistno, no_alt,
	 OBJ(int, 3), LINK_VINES);
ACCESS_LIST(fr_pr_novelllistno, fr_pr_eol, no_alt,
	 OBJ(int, 6), MINNOVACC, MAXNOVSAPACC,
	 IPX_MIN_STD_ACL, IPX_MAX_SAP_ACL, "Novell access list");
EQUAL	(fr_pr_novelllistcheck, fr_pr_novelllistno, fr_pr_vineslistcheck,
	 OBJ(int, 3), LINK_NOVELL);
ACCESS_LIST(fr_pr_xnslistno, fr_pr_eol, no_alt,
	 OBJ(int, 6), MINXNSACC, MAXEXNSACC,
	 XNS_MIN_STD_ACL, XNS_MAX_EXT_ACL, "XNS access list");
EQUAL	(fr_pr_xnslistcheck, fr_pr_xnslistno, fr_pr_novelllistcheck,
	 OBJ(int, 3), LINK_XNS);
ACCESS_LIST(fr_pr_applelistno, fr_pr_eol, no_alt,
	 OBJ(int, 6), MINATALKACC, MAXATALKACC,
	 AT_MIN_ACL, AT_MAX_ACL, "AppleTalk access list");
EQUAL	(fr_pr_applelistcheck, fr_pr_applelistno, fr_pr_xnslistcheck,
	 OBJ(int, 3), LINK_APPLETALK);
ACCESS_LIST(fr_pr_bridgelistno, fr_pr_eol, no_alt,
	 OBJ(int,6), MINTYPEACC, MAXTYPEACC, TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	 "Ethernet-typecode access list");
EQUAL	(fr_pr_bridgelistcheck, fr_pr_bridgelistno, fr_pr_applelistcheck,
	 OBJ(int,3), LINK_BRIDGE);

KEYWORD_ID (fr_pr_listkwd, fr_pr_bridgelistcheck, NONE,
	    OBJ(int,5), FR_PR_LIST,
	    "list", "To specify an access list", PRIV_CONF);
ASSERT	(fr_pr_list, fr_pr_listkwd, fr_pr_eol, 
	 GETOBJ(int,3) == LINK_BRIDGE 
	 || GETOBJ(int, 3) == LINK_APPLETALK
	 || GETOBJ(int, 3) == LINK_XNS
	 || GETOBJ(int, 3) == LINK_VINES
	 || GETOBJ(int, 3) == LINK_NOVELL);

LINK_TRANS(cfg_fr_delist_extend_here, fr_pr_list);

/* gt <size>
 * lt <size>
 */
NUMBER	(fr_pr_size, fr_pr_eol, no_alt,
	 OBJ(int,6), 0, 0xffff,
	 "Packet size (include MAC encapsulation bytes)");
KEYWORD_ID (fr_pr_lt, fr_pr_size, cfg_fr_delist_extend_here,
	    OBJ(int,5), FR_PR_LT,
	    "lt", "Prioritize packets less than a specified size", PRIV_CONF);

KEYWORD_ID (fr_pr_gt, fr_pr_size, fr_pr_lt,
	    OBJ(int,5), FR_PR_GT,
	    "gt", "Prioritize packets greater than a specified size",
	    PRIV_CONF);

/* [args] (see above) are only valid for the 'de-list protocol' command,
 */
TESTVAR	(fr_pr_args_test, fr_pr_gt, fr_pr_eol,
	 NONE, NONE, NONE, OBJ(int,2), FR_PRICMD_PROTOCOL);

/************************************************************************
 * frame-relay [dlci\de]-list <list> protocol <protocol-name> [args]
 * no frame-relay [dlci\de]-list <list> protocol <protocol-name> [args]
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = FR_PRICMD_PROTOCOL
 * OBJ(int,3) = <protocol-name>
 *
 * Get the protocol name.
 */

TESTVAR	(fr_pr_proto_test, fr_pr_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);

PROTONAME(fr_pr_protoname, fr_pr_args_test, fr_pr_proto_test,
	  OBJ(int,3), PMATCH_ARP PMATCH_APOLLO PMATCH_APPLETALK PMATCH_BRIDGE
	  PMATCH_CLNS PMATCH_CLNS_ALL_ES PMATCH_CLNS_ALL_IS
	  PMATCH_COMPRESSED_TCP PMATCH_DECNET PMATCH_DECNET_NODE
	  PMATCH_DECNET_ROUTER_L1 PMATCH_DECNET_ROUTER_L2 PMATCH_IP
	  PMATCH_NOVELL PMATCH_VINES PMATCH_XNS);

NVGENS	(fr_pr_nvgen, fr_pr_protoname, frame_relay_global_command, FR_LISTNUM);

KEYWORD_ID (fr_pr_protocol, fr_pr_nvgen, no_alt,
	    OBJ(int,2), FR_PRICMD_PROTOCOL,
	    "protocol", "Classify packets by protocol", PRIV_CONF);

/************************************************************************
 * frame-relay [de\dlci]-list <list> interface <interface-name>
 * no frame-relay de-list <list> interface
 * OBJ(int,1) = <list>
 * OBJ(int,2) = FR_PRICMD_INTERFACE
 * if_name = <interface-name>
 *
 */

INTERFACE (fr_pr_if_name, fr_pr_eol, no_alt,
	   OBJ(idb,1), IFTYPE_HWIDB     );

NVGENS	(fr_pr_if_nvgen, fr_pr_if_name, 
	 frame_relay_global_command, FR_LISTNUM);

KEYWORD_ID (fr_pr_interface, fr_pr_if_nvgen, fr_pr_protocol,
	    OBJ(int,2), FR_PRICMD_INTERFACE,
	    common_str_interface, "Classify packets from a named interface",
	    PRIV_CONF);

/*************************************************************************/
/*
 * List number
 */
NUMBER_NV(fr_pr, fr_pr_interface, no_alt,
	  OBJ(int,1), 1, 10, "Priority list numbers");
	  

KEYWORD (cfg_fr_pr, fr_pr, cfg_fr_switching,
	 "de-list", "Build a DE bit classification list", PRIV_CONF);

KEYWORD (cfg_fr, cfg_fr_pr, ALTERNATE,
	 "frame-relay", "global frame relay configuration commands", 
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_fr
