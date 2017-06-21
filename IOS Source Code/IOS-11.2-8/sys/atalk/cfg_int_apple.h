/* $Id: cfg_int_apple.h,v 3.6.4.4 1996/05/30 23:42:09 hampton Exp $
 * $Source: /release/112/cvs/Xsys/atalk/cfg_int_apple.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ A P P L E . H
 * 
 * Appletalk interface subcommands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_apple.h,v $
 * Revision 3.6.4.4  1996/05/30  23:42:09  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.6.4.3  1996/04/03  02:28:49  slin
 * CSCdi53350:  iptalk is broken
 * Branch: California_branch
 *
 * Revision 3.6.4.2  1996/03/23  01:28:21  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6.4.1  1996/03/18  18:53:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  06:27:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/03/13  01:07:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.1  1996/02/20  13:34:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/03/07  23:30:05  kleung
 * CSCdi45795:  Apple: Filter Statement Incorrectly Placed in Tunnel Config
 *
 * Revision 3.5  1996/02/20  09:33:49  dwong
 * CSCdi47658:  appletalk protocol rtmp and eigrp are not valid
 * configuration
 * Disable rtmp and eigrp from the configuration list when dealing
 * with aurp tunnels
 *
 * Revision 3.4  1995/12/24  04:56:34  dwong
 * CSCdi44162:  Appletalk doesnt work between ISDN callers
 *
 * Revision 3.3  1995/12/07  17:44:57  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  08:44:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:50:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/20  03:10:04  smackie
 * Another postive karma adjustment. Fix non-compiling commit. (CSCdi38726)
 *
 * Revision 2.4  1995/09/19  23:54:58  kleung
 * CSCdi38726:  AppleTalk RTMP Stub rtmp_stub feature - only RTMP stubs
 * sent.
 *
 * Revision 2.3  1995/08/25  21:54:48  kleung
 * CSCdi39338:  AppleTalk tunnel parser is broken
 *
 * Revision 2.2  1995/08/22  06:36:46  tkolar
 * CSCdi38902:  ATCP client mode wont work with ISDN
 * Change method of handing out virtual-net node addresses to accomodate
 * arbitrary interfaces, and prevent non P2P interfaces from being
 * configured for Appletalk client mode.
 *
 * Revision 2.1  1995/07/02  01:43:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:03:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * no appletalk
 */
EOLS	(ci_at_disable_eol, atalkif_command, ATALK_PORTDISABLE);

TESTVAR (ci_at_disable_nvgen, NONE, ci_at_disable_eol, NONE, NONE, NONE,
         nvgen, FALSE);
TESTVAR (ci_at_disable, ci_at_disable_eol, ci_at_disable_nvgen, NONE, NONE,
         NONE, sense, FALSE);

/*******************************************************************
 * <no> appletalk domain-group <number>
 */
#if 0
EOLS    (ci_at_dgroup_eol, atalkif_command, ATALK_DOMAIN_GROUP);

NUMBER  (ci_at_set_dgroup, ci_at_dgroup_eol, no_alt,
         OBJ(int,1), 1, 1000000, "AppleTalk Domain Number");

NVGENS  (ci_at_dgroupnvg, ci_at_set_dgroup, atalkif_command, ATALK_DOMAIN_GROUP);
KEYWORD (ci_at_domain_group, ci_at_dgroupnvg, ci_at_disable,
         "domain-group", "Specify appletalk domain ", PRIV_CONF|PRIV_SUBIF);
#endif
LINK_TRANS(ci_atalk_extend_here, ci_at_disable);

/******************************************************************
 * appletalk pre-fdditalk
 * no appletalk pre-fdditalk
 */
EOLS	(ci_at_prefdditalk_eol, atalkif_command, ATALK_PREFDDITALK);
KEYWORD	(ci_at_prefdditalk, ci_at_prefdditalk_eol, NONE,
	"pre-fdditalk", "Allow recognition of pre-FDDItalk packets",
	 PRIV_CONF|PRIV_SUBIF);
TEST_IDBSTATUS(ci_at_prefdditalk_assert, ci_at_prefdditalk, NONE,
	       ci_atalk_extend_here,IDB_FDDI);

/******************************************************************
 * appletalk client-mode
 * no appletalk client-mode
 */
EOLS	(ci_at_clientmode_eol, atalkif_command, ATALK_CLIENT_MODE);
KEYWORD	(ci_at_clientmode, ci_at_clientmode_eol, NONE,
	"client-mode", "Allow PPP client connections.", PRIV_CONF);
ASSERT  (ci_at_clientmode_assert, ci_at_clientmode, ci_at_prefdditalk_assert,
	is_p2p_hw(csb->interface->hwptr));


/******************************************************************
 * appletalk arp-timeout [<time in minutes>]
 * no appletalk arp-timeout [<time in minutes>]
 */

PARAMS_KEYONLY (ci_at_aarp_to, ci_at_clientmode_assert,
		"arp-timeout", OBJ(int,1), 1, 86400,
		atalkif_command, ATALK_ARP_TIMEOUT,
		"arp-timeout",
		"Set timeout interval (in minutes) for AARP entries",
		PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk send-rtmps
 * no appletalk send-rtmps
 */
EOLS	(ci_at_sendrtmps_eol, atalkif_command, ATALK_SENDROUTES);
KEYWORD	(ci_at_sendrtmps, ci_at_sendrtmps_eol, ci_at_aarp_to,
	"send-rtmps", "Send Appletalk routing updates", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk rtmp-splithorizon
 * no appletalk rtmp-splithorizon
 */
EOLS	(ci_at_rtmpsplit_eol, atalkif_command, ATALK_RTMP_SPLIT);
KEYWORD	(ci_at_rtmpsplit, ci_at_rtmpsplit_eol, ci_at_sendrtmps,
	"rtmp-splithorizon",
	 "Enable Split Horizon processing generating AT/RTMP updates",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk rtmp-stub
 * no appletalk rtmp-stub
 */
EOLS  (ci_at_rtmp_stub_eol, atalkif_command, ATALK_RTMP_STUB);
KEYWORD       (ci_at_rtmp_stub, ci_at_rtmp_stub_eol, ci_at_rtmpsplit,
      "rtmp-stub", "Send only RTMP stubs, no routes in updates",
       PRIV_CONF|PRIV_SUBIF);


/******************************************************************
 * appletalk rtmp-standby <seconds>
 * no appletalkl rtmp-standby
 * OBJ(int,1) = <standy interval>
 */
PARAMS_KEYONLY (ci_at_rtmp_standby, ci_at_rtmp_stub,
		"rtmp-standby", OBJ(int,1), 0, (RTMP_INVALIDTIME / ONESEC),
		atalkif_command, ATALK_RTMP_STANDBY,
		"RTMP Standby routing", "Standby interval in seconds",
		PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

/******************************************************************
 * appletalk distance {hops|metric} <distance>
 * no appletalk distance
 * 
 * Sets an administrative distance for all routes coming
 * in this interface.
 */
EOLS (ci_at_distance_eol, atalkif_command, ATALK_DISTANCE);
NUMBER (ci_at_distance_arg, ci_at_distance_eol, no_alt, OBJ(int,1),
	0,0xffffffff, "metric (0 to 4 billion)");
NOPREFIX (ci_at_nodistance, ci_at_distance_arg, ci_at_distance_eol);
NVGENS (ci_at_distance_nvgen, ci_at_nodistance, atalkif_command,
	ATALK_DISTANCE);
KEYWORD	(ci_at_distance, ci_at_distance_nvgen, ci_at_rtmp_standby,
	"distance", "Set AppleTalk administrative distance", 
	 PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

/******************************************************************
 * appletalk zip-reply-filter <list>
 * no appletalk zip-reply-filter
 * OBJ(int,1) = <list>
 */
PARAMS_ALIST (ci_at_zipreplyfilter, ci_at_distance,
		"zip-reply-filter", OBJ(int,1), MINATALKACC, MAXATALKACC,
	        AT_MIN_ACL, AT_MAX_ACL,
		atalkif_command, ATALK_ZIPREPLYFILTER,
		"Filter ZIP replies", "A pre-defined Appletalk access list",
		PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk route-cache
 * no appletalk route-cache
 */
EOLS	(ci_at_rcache_eol, atalkif_command, ATALK_ROUTEC);
KEYWORD	(ci_at_rcache, ci_at_rcache_eol, ci_at_zipreplyfilter,
	"route-cache", "Enable appletalk route cache", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk half-router
 * no appletalk half-router
 */
EOLS	(ci_at_hrouter_eol, atalkif_command, ATALK_HALFROUTER);
KEYWORD	(ci_at_hrouter, ci_at_hrouter_eol, ci_at_rcache,
	"half-router", "Configure as a half-router",
	 PRIV_CONF|PRIV_HIDDEN|PRIV_SUBIF);

/******************************************************************
 * appletalk glean-packets
 * no appletalk glean-packets
 */
EOLS	(ci_at_glean_eol, atalkif_command, ATALK_GLEAN);
KEYWORD	(ci_at_glean, ci_at_glean_eol, ci_at_hrouter,
	"glean-packets",
	 "Glean AARP information from packets",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk getzonelist-filter <list>
 * no appletalk getzonelist-filter <list>
 *
 * OBJ(int,1) = <list>
 */
PARAMS_KEYONLY (ci_at_zonefilter, ci_at_glean,
	"getzonelist-filter", OBJ(int,1), 0, -1,
	atalkif_command, ATALK_ZIPFILTER,
	"Filter zone-list replies", "A pre-defined Appletalk access list", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk distribute-list <list> { in | out }
 * no appletalk distribute-list <list> { in | out }
 *
 * OBJ(int,1) = <list>
 * OBJ(int,2) = IN, OUT
 */
EOLS	(ci_at_distlist_eol, atalkif_command, ATALK_ROUTEFILTER);

KEYWORD_ID (ci_at_distlist_out, ci_at_distlist_eol, no_alt,
	    OBJ(int,2), OUT,
	"out", "Filter networks from outgoing updates", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID (ci_at_distlist_in, ci_at_distlist_eol, ci_at_distlist_out,
	    OBJ(int,2), IN,
	"in", "Filter networks from incoming updates", PRIV_CONF|PRIV_SUBIF);

ACCESS_LIST(ci_at_distlist_list, ci_at_distlist_in, no_alt,
	OBJ(int,1), MINATALKACC, MAXATALKACC, 
	AT_MIN_ACL, AT_MAX_ACL,
	"A pre-defined Appletalk access list");

NVGENS	(ci_at_distlist_nvgen, ci_at_distlist_list, 
	atalkif_command, ATALK_ROUTEFILTER);
KEYWORD	(ci_at_distlist, ci_at_distlist_nvgen, ci_at_zonefilter,
	"distribute-list", "Filter networks from routing updates", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk free-trade-zone
 * no appletalk free-trade-zone
 */
EOLS	(ci_at_ftz_eol, atalkif_command, ATALK_FTZ_SECURITY);
KEYWORD	(ci_at_ftz, ci_at_ftz_eol, ci_at_distlist,
	"free-trade-zone", "Enhanced security for one-way shared networks",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk discovery
 * no appletalk discovery
 */
EOLS	(ci_at_discovery_eol, atalkif_command, ATALK_DISCOVERY);
KEYWORD	(ci_at_discovery, ci_at_discovery_eol, ci_at_ftz,
	"discovery", "Reset discovery mode for new cable range discovery", 
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk access-group <list>
 * no appletalk access-group [<list>]
 */

PARAMS_ALIST (ci_at_accessgrp, ci_at_discovery,
	 "access-group", OBJ(int,1), MINATALKACC, MAXATALKACC,
	 AT_MIN_ACL, AT_MAX_ACL,
	 atalkif_command, ATALK_ACCESS,
	 "Appletalk access group", "Appletalk access list", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk protocol {rtmp|eigrp}
 * no appletalk protocol {rtmp|eigrp}
 *
 * The default for this command will be "protocol rtmp", so
 * that existing configurations aren't broken.
 */

EOLS	(ci_at_rp_eol, atalkif_command, ATALK_ROUTE_PROTOCOL);
KEYWORD_ID (ci_at_rp_proto_rtmp, ci_at_rp_eol, no_alt,
	    OBJ(int,1), ATALK_RP_RTMP,
	    "rtmp", "Enable RTMP processing on this interface", 
	    PRIV_CONF|PRIV_SUBIF);
ASSERT(ci_aurp_tunnel_assert, ci_at_rp_proto_rtmp, no_alt,
       !atalkif_ForAURP(csb->interface));
LINK_TRANS(ci_at_rp_extend, ci_aurp_tunnel_assert);
KEYWORD	(ci_at_rp, ci_at_rp_extend, ci_at_accessgrp,
	"protocol", "Select AppleTalk routing protocol", 
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk zone <zonename>
 * no appletalk zone [<zonename>]
 *
 * OBJ(string,1) = <zonename>
 *
 * If no zone name is specified, then all zones are cleared.  In this
 * case, OBJ(string,1) will be an empty string (i.e. OBJ(string,1)[0] == 0).
 */
EOLS	(ci_at_zone_eol, atalkif_command, ATALK_ZONE);

TEXT	(ci_at_zone_nameno, ci_at_zone_eol, ci_at_zone_eol, OBJ(string, 1),
	 "An AppleTalk zone name");
TEXT 	(ci_at_zone_nameyes, ci_at_zone_eol, no_alt, OBJ(string, 1),
	 "An AppleTalk zone name");
TESTVAR (ci_at_zonetest, ci_at_zone_nameyes, ci_at_zone_nameno, NONE, NONE,
	 NONE, sense, TRUE);
NVGENS	(ci_at_zonenvgen, ci_at_zonetest, atalkif_command, ATALK_ZONE);
KEYWORD	(ci_at_zone, ci_at_zonenvgen, ci_at_rp,
	 "zone", "Assign an appletalk zone name", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk cable-range <start>-<end> [<net.node>]
 * no appletalk cable-range [<start>-<end> [<net.node>]]
 *
 * OBJ(int,1) = start cable number
 * OBJ(int,2) = end cable number
 * (*OBJ(paddr,1)) = optional appletalk address
 */

EOLS	(ci_at_cr_eol, atalkif_command, ATALK_CABLE_RANGE);

ATADDR	(ci_at_cr_addr, ci_at_cr_eol, ci_at_cr_eol, OBJ(paddr,1),
	 "Suggested net.node address");

CRANGE	(ci_at_cr_start, ci_at_cr_addr, no_alt, OBJ(int,1), OBJ(int,2));

NOPREFIX (ci_at_nocr, ci_at_cr_start, ci_at_cr_eol);
NVGENS	(ci_at_cr_nvgen, ci_at_nocr, atalkif_command, 
	 ATALK_CABLE_RANGE);
KEYWORD	(ci_at_cablerng, ci_at_cr_nvgen, ci_at_zone,
	 "cable-range", "Set appletalk Phase 2 address", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk address <net.node>
 * no appletalk address [<net.node>]
 */

EOLS	(ci_at_addreol, atalkif_command, ATALK_ADDRESS);
ATADDR	(ci_at_addr, ci_at_addreol, no_alt, 
	 OBJ(paddr,1), "Appletalk Phase 1 address");

/* No address needed if there was a 'no' prefix */
NOPREFIX	(ci_at_noaddr, ci_at_addr, ci_at_addreol);
NVGENS	(ci_at_addrnvgen, ci_at_noaddr, atalkif_command, ATALK_ADDRESS);
KEYWORD	(ci_at_address, ci_at_addrnvgen, ci_at_cablerng,
	 "address", "Set appletalk Phase 1 address", PRIV_CONF|PRIV_SUBIF);


/******************************************************************/
KEYWORD	(ci_appletalk, ci_at_address, NONE,
	"appletalk", "Appletalk interface subcommands", PRIV_CONF|PRIV_SUBIF);

ASSERT  (ci_appletalk_subif, ci_appletalk, ALTERNATE, 
	 main_if_or_vc(csb->interface));

#undef  ALTERNATE
#define ALTERNATE ci_appletalk_subif
