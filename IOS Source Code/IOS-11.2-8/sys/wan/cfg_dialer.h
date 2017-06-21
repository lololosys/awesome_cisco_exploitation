/* $Id: cfg_dialer.h,v 3.4.10.4 1996/06/21 23:44:00 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_dialer.h,v $
 *------------------------------------------------------------------
 * C F G _ D I A L E R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_dialer.h,v $
 * Revision 3.4.10.4  1996/06/21  23:44:00  hampton
 * It doesn't help to put back a command if it isn't linked into the parse
 * chains.  Link in the "dialer-list <n> list" command.  [CSCdi60512]
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/06/17  23:58:37  hampton
 * Hidden dialer-list command must be kept around for backward
 * compatibility support.  [CSCdi60512]
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/05/30  23:46:32  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/04/27  06:35:38  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.8.2  1996/04/16  23:59:36  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.3.8.1  1996/02/23  21:31:08  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.4  1996/02/26  06:53:11  dmccowan
 * CSCdi45288:  need appn configuration changes to support multilink ppp
 * Add configuration option for APPN over a dialer interface
 *
 * Revision 3.3  1995/12/22  01:28:36  suresh
 * CSCdi29641:  Dialer dialer-group accepts 0 but does not work
 * Changed the dialer-group range to 1-10; cleaned code so that
 * dialer-list and dialer-group accesses are consistent
 *
 * Revision 3.2  1995/11/17  18:00:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:45  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/21  19:52:35  wilber
 * CSCdi33177:  crash in bridge_accesscheck
 * Bridging extended access list is not supported for DDR - enforce it
 * by parser
 *
 * Revision 2.2  1995/06/21  18:16:26  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:02:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * dialer-list <1-9> list <1-MAXACCESSGROUP>
 */
EOLS	(dialer_list_access_eol, dialerlist_command, DIALLST_ACCESS);
NUMBER	(dialer_list_access_num, dialer_list_access_eol, no_alt,
	 OBJ(int,2), 1, MAXNOVSAPACC, 
	 "Number of access list to add to dialer list");
KEYWORD	(dialer_list_access, dialer_list_access_num, no_alt,
	 "list", "Add access list to dialer list", PRIV_CONF|PRIV_HIDDEN);
 
/***************************************************************
 * dialer-list <0-9> protocol { ... } { permit | deny | list <n> }
 * no dialer-list <0-9> protocol { ... }
 */
EOLS	(dialer_list_protocol_eol, dialerlist_command, DIALLST_PROTOCOL);

KEYWORD_ID(dialer_list_protocol_permit, dialer_list_protocol_eol,
	   no_alt, OBJ(int,3), DIALLST_PERMIT,
	   "permit", "Permit specified protocol", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_deny, dialer_list_protocol_eol,
	   dialer_list_protocol_permit, OBJ(int,3), DIALLST_DENY,
	   "deny", "Deny specified protocol", PRIV_CONF);

EOLS	(dialer_list_no_protocol, dialerlist_command, DIALLST_PROTOCOL);
EQUAL	(dialer_list_no_protocol_test, dialer_list_no_protocol,
	 dialer_list_protocol_deny, sense, FALSE);

NUMBER	(dialer_list_protocol_vineslist_num, dialer_list_protocol_eol, no_alt,
	 OBJ(int,4), VINESMINACC, VINESMAXACC, "Vines access list");
EQUAL	(dialer_list_protocol_vineslist, dialer_list_protocol_vineslist_num,
	 no_alt, OBJ(int,2), LINK_VINES);
ACCESS_LIST(dialer_list_protocol_novlist_num, dialer_list_protocol_eol, no_alt,
	 OBJ(int,4), MINNOVACC, MAXNOVSAPACC,
	 IPX_MIN_STD_ACL, IPX_MAX_SAP_ACL, "Novell access list");
EQUAL	(dialer_list_protocol_novlist, dialer_list_protocol_novlist_num,
	 dialer_list_protocol_vineslist,
	 OBJ(int,2), LINK_NOVELL);
ACCESS_LIST(dialer_list_protocol_applelist_num, dialer_list_protocol_eol, no_alt,
	 OBJ(int,4), MINATALKACC, MAXATALKACC, AT_MIN_ACL, AT_MAX_ACL,
	 "AppleTalk access list");
EQUAL	(dialer_list_protocol_applelist, dialer_list_protocol_applelist_num,
	 dialer_list_protocol_novlist,
	 OBJ(int,2), LINK_APPLETALK);
ACCESS_LIST(dialer_list_protocol_xnslist_num, dialer_list_protocol_eol, no_alt,
	 OBJ(int,4), MINXNSACC, MAXEXNSACC, XNS_MIN_STD_ACL, XNS_MAX_EXT_ACL,
	 "XNS access list");
EQUAL	(dialer_list_protocol_xnslist, dialer_list_protocol_xnslist_num,
	 dialer_list_protocol_applelist,
	 OBJ(int,2), LINK_XNS);
ACCESS_LIST(dialer_list_protocol_bridgelist_num, dialer_list_protocol_eol, no_alt,
	 OBJ(int,4), MINTYPEACC, MAXTYPEACC,
	 TYPECODE_MIN_ACL, TYPECODE_MAX_ACL,
	 "Ethernet-typecode access list");
EQUAL	(dialer_list_protocol_bridgelist, dialer_list_protocol_bridgelist_num,
	 dialer_list_protocol_xnslist,
	 OBJ(int,2), LINK_BRIDGE);
NUMBER	(dialer_list_protocol_iplist_num, dialer_list_protocol_eol, no_alt,
	 OBJ(int, 4), MINFASTACC, MAXSLOWACC, "IP access list");
EQUAL	(dialer_list_protocol_iplist, dialer_list_protocol_iplist_num,
	 dialer_list_protocol_bridgelist,
	 OBJ(int,2), LINK_IP);

KEYWORD_ID(dialer_list_protocol_list, dialer_list_protocol_iplist,
	   dialer_list_no_protocol_test, OBJ(int,3), DIALLST_PROLIST,
	   "list", "Add access list to dialer list", PRIV_CONF);

STRING  (dialer_list_protocol_clnslist_name, dialer_list_protocol_eol, no_alt,
         OBJ(string,1), "Name of CLNS filter expression or set");
KEYWORD_ID(dialer_list_protocol_clnslist,
           dialer_list_protocol_clnslist_name,
           dialer_list_no_protocol_test, OBJ(int,3), DIALLST_PROLIST,
           "list", "Add access list to dialer list", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_clns_is, dialer_list_protocol_clnslist,
	   no_alt, OBJ(int,2), LINK_CLNS_ALL_IS,
	   "clns_is", "CLNS Intermediate System", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_clns_es, dialer_list_protocol_clnslist,
	   dialer_list_protocol_clns_is, OBJ(int,2), LINK_CLNS_ALL_ES,
	   "clns_es", "CLNS End System", PRIV_CONF);
TEST_MULTIPLE_FUNCS(dialer_list_protocol_clns_esis,
		    dialer_list_protocol_clns_es,
		    NONE,
		    no_alt);
KEYWORD_ID(dialer_list_protocol_clns, dialer_list_protocol_clnslist,
	   dialer_list_protocol_clns_esis, OBJ(int,2), LINK_CLNS,
	   "clns", "OSI Connectionless Network Service", PRIV_CONF);
ACCESS_LIST(dialer_list_protocol_declist_num, dialer_list_protocol_eol, no_alt,
	 OBJ(int,4), MINDECACC, MAXDECACC, DN_MIN_ACL, DN_MAX_ACL,
	 "DECNET access list");
KEYWORD_ID(dialer_list_protocol_declist, dialer_list_protocol_declist_num,
	   dialer_list_no_protocol_test, OBJ(int,3), DIALLST_PROLIST,
	   "list", "Add access list to dialer list", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_decnet_node, dialer_list_protocol_declist,
	   NONE, OBJ(int,2), LINK_DECNET_NODE,
	   "decnet_node", "DECnet node", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_decnet_router_L2, dialer_list_protocol_declist,
	   dialer_list_protocol_decnet_node,
	   OBJ(int,2), LINK_DECNET_ROUTER_L2,
	   "decnet_router-L2", "DECnet router L2", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_decnet_router_L1, dialer_list_protocol_declist,
	   dialer_list_protocol_decnet_router_L2,
	   OBJ(int,2), LINK_DECNET_ROUTER_L1,
	   "decnet_router-L1", "DECnet router L1", PRIV_CONF);
TEST_MULTIPLE_FUNCS(dialer_list_protocol_decnet_router,
		    dialer_list_protocol_decnet_router_L1,
		    NONE,
		    dialer_list_protocol_clns);
KEYWORD_ID(dialer_list_protocol_decnet, dialer_list_protocol_declist,
	   dialer_list_protocol_decnet_router, OBJ(int,2), LINK_DECNET,
	   "decnet", "DECnet", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_bridge, dialer_list_protocol_list,
	   dialer_list_protocol_decnet, OBJ(int,2), LINK_BRIDGE,
	   "bridge", "Bridging", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_llc2, dialer_list_protocol_list,
	   dialer_list_protocol_bridge, OBJ(int,2), LINK_LLC2,
	   "llc2", "LLC2", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_vines, dialer_list_protocol_list,
	   dialer_list_protocol_llc2, OBJ(int,2), LINK_VINES,
	   "vines", "Banyan Vines", PRIV_CONF);
NOVELL_IPX_KEYWORD_ID(dialer_list_protocol_novell,
		      dialer_list_protocol_list,
		      dialer_list_protocol_vines, OBJ(int,2), LINK_NOVELL,
		      "Novell IPX", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_appletalk, dialer_list_protocol_list,
	   dialer_list_protocol_novell, OBJ(int,2), LINK_APPLETALK,
	   "appletalk", "AppleTalk", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_xns, dialer_list_protocol_list,
	   dialer_list_protocol_appletalk, OBJ(int,2), LINK_XNS,
	   "xns", "XNS", PRIV_CONF);
KEYWORD_ID(dialer_list_protocol_ip, dialer_list_protocol_list,
	   dialer_list_protocol_xns, OBJ(int,2), LINK_IP,
	   "ip", "IP", PRIV_CONF);

KEYWORD	(dialer_list_protocol, dialer_list_protocol_ip, dialer_list_access,
	 "protocol", "Permit or Deny based on protocols", PRIV_CONF);

/***************************************************************
 * no dialer-list <num>
 */
EOLS	(dialer_list_no_dialer, dialerlist_command, NO_DIALLST);
TESTVAR	(dialer_list_no_test, NONE, dialer_list_no_dialer,
	 NONE, NONE, dialer_list_protocol, sense, TRUE);

NUMBER	(dialer_list_num, dialer_list_no_test, no_alt,
	 OBJ(int,1), DIALLST_MINLIST, DIALLST_MAXLIST, "Dialer group number");

NVGENS	(dialer_list_nvgen, dialer_list_num,
	 dialerlist_command, 0);
KEYWORD	(dialer_list, dialer_list_nvgen, ALTERNATE,
	 "dialer-list", "Create a dialer list entry", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	dialer_list
