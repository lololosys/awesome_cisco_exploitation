/* $Id: cfg_apple.h,v 3.3.26.3 1996/07/19 21:33:46 kleung Exp $
 * $Source: /release/112/cvs/Xsys/atalk/cfg_apple.h,v $
 *------------------------------------------------------------------
 * C F G _ A P P L E . H
 *
 * Appletalk global configuration commands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_apple.h,v $
 * Revision 3.3.26.3  1996/07/19  21:33:46  kleung
 * CSCdi63179:  appletalk rtmp gc-interval 0 crashes router
 * Branch: California_branch
 *
 * Revision 3.3.26.2  1996/05/21  09:42:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.26.1  1996/05/02  08:41:19  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.3  1996/01/18  02:12:05  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  08:44:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/02  01:43:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:01:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cfg_atalk_extend_here, no_alt);

/******************************************************************
 * apple virtual-net <net-number> <zone-name>
 * no apple virtual-net <net-number> [<zone-name>]
 *
 * OBJ(int,1) = net-number
 * OBJ(string,1) = zone-name
 */
EOLS	(conf_at_virtnet_eol, appletalk_command, ATALK_VIRTUAL_NET);
TEXT(conf_at_virtnet_zone, conf_at_virtnet_eol, no_alt,
     OBJ(string,1), "Zone name");
NOPREFIX(conf_at_virtnet_nonet, conf_at_virtnet_zone, conf_at_virtnet_eol);
NUMBER(conf_at_virtnet_net, conf_at_virtnet_nonet, no_alt,
       OBJ(int,1), 1, 65279, "Network number");
NVGENS(conf_at_virtnet_nvgen, conf_at_virtnet_net,
	appletalk_command, ATALK_VIRTUAL_NET);
KEYWORD (conf_at_virtnet, conf_at_virtnet_nvgen, cfg_atalk_extend_here,
	"virtual-net", "Internal Appletalk Network", PRIV_CONF);

/******************************************************************
 * appletalk route-redistribution
 * no appletalk route-redistribution
 *
 */
EOLS	(conf_at_routedist_eol, appletalk_command, ATALK_ROUTEDIST);

KEYWORD	(conf_at_routedist, conf_at_routedist_eol, conf_at_virtnet,
	"route-redistribution", "Leak networks between protocols",
	 PRIV_CONF);

/******************************************************************
 * appletalk rtmp-bandwidth <bandwidth>
 * no appletalk rtmp-bandwidth
 * 
 * Sets an RTMP bandwidth factor for all routes being dist'ed into EIGRP
 * from RTMP. This is now a depricated command, see the "rtmp" command
 * chain.
 */
EOLS (conf_at_rtmp_bw_eol, appletalk_command, ATALK_RTMP_BW);
NUMBER (conf_at_rtmp_bw_arg, conf_at_rtmp_bw_eol, no_alt, OBJ(int,1),
	0,0xffffffff, "RTMP bandwidth (0 to 4 billion)");
NOPREFIX (conf_at_nortmp_bw, conf_at_rtmp_bw_arg, conf_at_rtmp_bw_eol);
NVGENS (conf_at_rtmp_bw_nvgen, conf_at_nortmp_bw, appletalk_command,
	ATALK_RTMP_BW);
KEYWORD_MM (conf_at_rtmp_bw, conf_at_rtmp_bw_nvgen, conf_at_routedist,
	    "rtmp-bandwidth", "Set RTMP bandwidth factor", 
	    PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN, 5);

/*************************************************************************
 * appletalk event-logging [<level>]
 * no appletalk event-logging [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS (conf_at_events_eol, appletalk_command, ATALK_EVENTS);
INTERFACE(conf_at_events_int_name, conf_at_events_eol,
	  no_alt, OBJ(idb, 1), IFTYPE_HWIDB     );
KEYWORD_ID (conf_at_events_int, conf_at_events_int_name, conf_at_events_eol,
	    OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_CONF);
NVGENS (conf_at_events_nvgen, conf_at_events_int,
	appletalk_command, ATALK_EVENTS);
KEYWORD (conf_at_events, conf_at_events_nvgen, conf_at_rtmp_bw,
	 "event-logging", "Set appletalk event logging level", PRIV_CONF);

/************************************************************************
 * appletalk maximum-paths [<paths>]
 * [no] appletalk maximum-paths [<paths>]
 *
 * OBJ(int,1) = <paths>
 */
EOLS    (conf_at_maxpaths_eol, appletalk_command, ATALK_MAXPATHS);
NUMBER  (conf_at_maxpaths_number, conf_at_maxpaths_eol, no_alt,
	 OBJ(int,1), 1, 16, "Number of parallel routing paths");
NVGENS  (conf_at_maxpaths_nvgen, conf_at_maxpaths_number, appletalk_command,
	 ATALK_MAXPATHS);
KEYWORD (conf_at_maxpaths, conf_at_maxpaths_nvgen, conf_at_events,
	 "maximum-paths", "Set number of parallel routing paths", PRIV_CONF);

/*************************************************************************
 * appletalk routing [eigrp router-address]
 * no appletalk routing [eigrp]
 *
 * The keyword "eigrp" is optional. If it does not appear on the
 * command line, then the AT/EIGRP process was not started. The EIGRP
 * parse sub-tree is in another file.
 *
 * NOTE: We never generate 'appletalk service' during NV generation.
 * OBJ(int,1) = 0 RTMP only,  1 RTMP & EIGRP
 * OBJ(int,2) = router ID for use with EIGRP.
 */
EOLS (conf_at_routing_eol, appletalk_command, ATALK_ROUTING);

LINK_TRANS(cfg_apple_routing_extend_here, conf_at_routing_eol);

#if 0
KEYWORD (conf_at_routing_rtmp, conf_at_routing_eol,
	 cfg_atalk_routing_extend_here,
	 "rtmp", "Enable AppleTalk RTMP routing", PRIV_CONF);
#endif
KEYWORD	(conf_at_routing, cfg_apple_routing_extend_here, conf_at_maxpaths,
	"routing", "Enable Appletalk routing", PRIV_CONF);

/*************************************************************************/
KEYWORD	(configure_appletalk, conf_at_routing, ALTERNATE,
	 "appletalk", "Appletalk global configuration commands", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_appletalk
