/* $Id: cfg_appletalk_routing.h,v 3.2.60.1 1996/07/19 21:33:47 kleung Exp $
 * $Source: /release/112/cvs/Xsys/atalk/cfg_appletalk_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ A P P L E T A L K _ R O U T I N G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_appletalk_routing.h,v $
 * Revision 3.2.60.1  1996/07/19  21:33:47  kleung
 * CSCdi63179:  appletalk rtmp gc-interval 0 crashes router
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:44:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:50:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/12  19:35:21  slin
 * CSCdi39096:  Inability to NBP-register AUFS Server (Unix Host).
 * - Add support for routing between local devices
 *
 * Revision 2.1  1995/07/02  01:43:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:01:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cfg_atalk_routing_extend_here, no_alt);

/*************************************************************************
 * appletalk lookup-type <serviceType>
 * no appletalk lookup-type <serviceType>
 *
 * OBJ(string,1) = <serviceType>
 */
EOLS (cr_at_lookuptype_eol, appletalk_command, ATALK_LOOKUPTYPE);
TEXT (cr_at_lookuptype_zone, cr_at_lookuptype_eol, no_alt,
      OBJ(string,1), "Appletalk service name");
NVGENS (cr_at_lookuptype_nvgen, cr_at_lookuptype_zone, 
	appletalk_command, ATALK_LOOKUPTYPE);
KEYWORD (cr_at_lookuptype, cr_at_lookuptype_nvgen, cfg_atalk_routing_extend_here,
	"lookup-type", "Enable cache of service names", PRIV_CONF);

/*************************************************************************
 * appletalk proxy-nbp <net-number> <zone-name>
 * no appletalk proxy-nbp <net-number> [<zone-name>]
 *
 * OBJ(int,1) = net-number
 * OBJ(string,1) = zone-name
 */
EOLS (cr_at_proxynbp_eol, appletalk_command, ATALK_PROXYNBP);
TEXT (cr_at_proxynbp_zone, cr_at_proxynbp_eol, no_alt,
      OBJ(string,1), "Zone name");
NOPREFIX (cr_at_noproxynbp, cr_at_proxynbp_zone, cr_at_proxynbp_eol);
NUMBER (cr_at_proxynbp_net, cr_at_noproxynbp, no_alt,
	OBJ(int,1), 1, 65279, "Network number");
NVGENS (cr_at_proxynbpnvgen, cr_at_proxynbp_net,
	appletalk_command, ATALK_PROXYNBP);
KEYWORD	(cr_at_proxynbp, cr_at_proxynbpnvgen, cr_at_lookuptype,
	"proxy-nbp", "Non-extended Appletalk router proxy support", PRIV_CONF);

/*************************************************************************
 * appletalk arp [{ probe | request }] interval <milliseconds>
 * appletalk arp [{ probe | request }] retransmit-count <count>
 *
 * no appletalk arp
 * no appletalk arp [{ probe | request }] interval [<milliseconds>]
 * no appletalk arp [{ probe | request }] retransmit-count [<count>]
 *
 *  If the arp type is not specified, 'probe' is assumed.
 *
 * OBJ(int,1) == ATALK_ARPTYPE_PROBE, ATALK_ARPTYPE_REQUEST
 * OBJ(int,2) == ATALK_ARP_INTERVAL. ATALK_ARP_RETRANS
 * OBJ(int,3) == value of command parameter
 */
EOLS (cr_at_arp_eol, appletalk_command, ATALK_ARP_PARAM);

/* No appletalk arp */
TESTVAR	(cr_at_arp_noprefix, cr_at_arp_eol, no_alt,
	 NONE, NONE, NONE,
	 sense, FALSE);

NUMBER (cr_at_arpretrans_val, cr_at_arp_eol, no_alt,
	OBJ(int,3), 1, -1, "AARP Retransmission count");
NOPREFIX (cr_at_arpretrans_noprefix, cr_at_arpretrans_val, cr_at_arp_eol);
KEYWORD_ID (cr_at_arpretrans, cr_at_arpretrans_val, cr_at_arp_noprefix,
	OBJ(int,2), ATALK_ARP_RETRANS,
	 "retransmit-count",  "Set number of AARP retransmissions", PRIV_CONF);

NUMBER (cr_at_arpint_val, cr_at_arp_eol, no_alt,
	OBJ(int,3), 33, -1, "Milliseconds");
NOPREFIX (cr_at_arpint_noprefix, cr_at_arpint_val, cr_at_arp_eol);
KEYWORD_ID (cr_at_arpint, cr_at_arpint_val, cr_at_arpretrans,
	OBJ(int,2), ATALK_ARP_INTERVAL,
	"interval", "Set AARP retransmit interval (minimum 33ms)", PRIV_CONF);

/* GETOBJ(int,1) is initialized to ATALK_ARPTYPE_PROBE,
 * so "probe" is selected by default.
 */
KEYWORD_ID (cr_at_request, cr_at_arpint, cr_at_arpint,
	OBJ(int,1), ATALK_ARPTYPE_REQUEST,
	 "request", "AARP request parameters", PRIV_CONF);
KEYWORD_ID (cr_at_probekwd, cr_at_arpint, cr_at_request,
	OBJ(int,1), ATALK_ARPTYPE_PROBE,
	 "probe", "AARP probe parameters (the default)", PRIV_CONF);
SET (cr_at_probe, cr_at_probekwd, OBJ(int,1), ATALK_ARPTYPE_PROBE);

NVGENS (cr_at_arpnvgen, cr_at_probe, appletalk_command,
	 ATALK_ARP_PARAM);
KEYWORD	(cr_at_arp, cr_at_arpnvgen, cr_at_proxynbp,
	 "arp", "Set Appletalk ARP parameters", PRIV_CONF);

/*************************************************************************
 * appletalk zip-query-interval <interval>
 * no appletalk zip-query-interval [<interval>]
 *
 * OBJ(int,1) = interval
 */
EOLS	(cr_at_zonequery_eol, appletalk_command, ATALK_ZONEQUERY);
NUMBER	(cr_at_zonequery_val, cr_at_zonequery_eol, no_alt,
	OBJ(int,1), 1, -1, "Seconds");
NOPREFIX (cr_at_nozonequery, cr_at_zonequery_val, cr_at_zonequery_eol);
NVGENS	(cr_at_zonequery_nvgen, cr_at_nozonequery, 
	appletalk_command, ATALK_ZONEQUERY);
KEYWORD (cr_at_zonequery, cr_at_zonequery_nvgen, cr_at_arp,
	"zip-query-interval", "Interval between ZIP queries", PRIV_CONF);

/*************************************************************************
 * appletalk timers <update-interval> <valid-interval> <invalid-interval>
 * no appletalk timers [<update-interval> <valid-interval> <invalid-interval>]
 *
 * OBJ(int,1) = update-interval
 * OBJ(int,2) = valid-interval
 * OBJ(int,3) = invalid-interval
 */
EOLS	(cr_at_timers_eol, appletalk_command, ATALK_TIMERS);
NUMBER	(cr_at_timers_invalid, cr_at_timers_eol, no_alt,
	OBJ(int,3), 0, -1, "Invalid interval");
NUMBER	(cr_at_timers_valid, cr_at_timers_invalid, no_alt,
	OBJ(int,2), 0, -1, "Valid interval");
NUMBER	(cr_at_timers_update, cr_at_timers_valid, no_alt,
	OBJ(int,1), 0, -1, "Update interval");
NOPREFIX (cr_at_notimers, cr_at_timers_update, cr_at_timers_eol);
NVGENS	(cr_at_timersnvgen, cr_at_notimers, appletalk_command, ATALK_TIMERS);
KEYWORD	(cr_at_timers, cr_at_timersnvgen, cr_at_zonequery,
	"timers", "Alter routing timers", PRIV_CONF);

/************************************************************************
 * [no] appletalk rtmp <sub-command>
 *
 * OBJ(int,1) = AppleTalk RTMP sub-command
 *
 * 	"[no] appletalk rtmp jitter {<percentage (0-100)>}
 *		OBJ(int,1) = ATALK_RTMP_JITTER
 *		OBJ(int,2) = <percentage>
 *
 * 	"[no] appletalk rtmp bandwidth {0 - 0xffffffff}
 *		OBJ(int,1) = ATALK_RTMP_BW
 *		OBJ(int,2) = <percentage>
 *
 *	"[no] appletalk rtmp timers <update-int> <valid-int> <invalid-int>
 *		OBJ(int,1) = ATALK_RTMP_TIMERS
 *		OBJ(int,2) = update-interval
 *		OBJ(int,3) = valid-interval
 *		OBJ(int,4) = invalid-interval
 *
 *	"[no] appletalk rtmp gc-interval <gc-interval>"
 *		OBJ(int,1) = ATALK_RTMP_GC_INTERVAL
 *		OBJ(int,2) = gc-interval
 *
 * In the future, all RTMP-specific configuration commands will be moved
 * under this hierarchy.
 */
EOLS	(cr_at_rtmp_eol, appletalk_command, ATALK_RTMP_COMMAND);

NUMBER  (cr_at_rtmp_gc_interval_number, cr_at_rtmp_eol, no_alt,
	 OBJ(int,2), 1, 15, "RTMP garbage collection period, in minutes");
NOPREFIX (cr_at_rtmp_nogc_interval, cr_at_rtmp_gc_interval_number,
	  cr_at_rtmp_eol);
NVGENS	(cr_at_rtmp_gc_interval_nvgen, cr_at_rtmp_nogc_interval,
	 appletalk_command, ATALK_RTMP_COMMAND);
KEYWORD_ID (cr_at_rtmp_gc_interval, cr_at_rtmp_gc_interval_nvgen,
	    no_alt,
	    OBJ(int,1), ATALK_RTMP_GC_INTERVAL,
	    "gc-interval", "Interval (in minutes) between RTMP GC runs",
	    PRIV_CONF|PRIV_HIDDEN);

NUMBER	(cr_at_rtmp_timers_invalid, cr_at_rtmp_eol, no_alt,
	OBJ(int,4), 0, -1, "Invalid interval");
NUMBER	(cr_at_rtmp_timers_valid, cr_at_rtmp_timers_invalid, no_alt,
	OBJ(int,3), 0, -1, "Valid interval");
NUMBER	(cr_at_rtmp_timers_update, cr_at_rtmp_timers_valid, no_alt,
	OBJ(int,2), 0, -1, "Update interval");
NOPREFIX (cr_at_rtmp_notimers, cr_at_rtmp_timers_update, cr_at_rtmp_eol);
NVGENS	(cr_at_rtmp_timers_nvgen, cr_at_rtmp_notimers,
	 appletalk_command, ATALK_RTMP_COMMAND);
KEYWORD_ID (cr_at_rtmp_timers, cr_at_rtmp_timers_nvgen, cr_at_rtmp_gc_interval,
	    OBJ(int,1), ATALK_RTMP_TIMERS,
	    "timers", "Alter RTMP routing timers", PRIV_CONF|PRIV_HIDDEN);

NUMBER  (cr_at_rtmp_bw_number, cr_at_rtmp_eol, no_alt,
	 OBJ(int,2), 0, 0xffffffff, "RTMP bandwidth (0 to 4 billion)");
NOPREFIX (cr_at_rtmp_nobw, cr_at_rtmp_bw_number, cr_at_rtmp_eol);
NVGENS	(cr_at_rtmp_bw_nvgen, cr_at_rtmp_nobw,
	 appletalk_command, ATALK_RTMP_COMMAND);
KEYWORD_ID (cr_at_rtmp_bw, cr_at_rtmp_bw_nvgen, cr_at_rtmp_timers,
	    OBJ(int,1), ATALK_RTMP_BW,
	    "bandwidth", "Set RTMP composite metric bandwidth factor",
	    PRIV_CONF|PRIV_HIDDEN);

NUMBER  (cr_at_rtmp_jitter_number, cr_at_rtmp_eol, no_alt,
	 OBJ(int,2), 0, 100, "RTMP update timer jitter, in percent (0-100)");
NOPREFIX (cr_at_rtmp_nojitter, cr_at_rtmp_jitter_number, cr_at_rtmp_eol);
NVGENS	(cr_at_rtmp_jitter_nvgen, cr_at_rtmp_nojitter,
	 appletalk_command, ATALK_RTMP_COMMAND);
KEYWORD_ID (cr_at_rtmp_jitter, cr_at_rtmp_jitter_nvgen,
	    cr_at_rtmp_bw,
	    OBJ(int,1), ATALK_RTMP_JITTER,
	 "jitter", "Percentage of RTMP update interval to vary", PRIV_CONF);

KEYWORD (cr_at_rtmp, cr_at_rtmp_jitter, cr_at_timers,
	 "rtmp", "AppleTalk rtmp subcommands", PRIV_CONF);

/*************************************************************************
 * appletalk name-lookup-interval <seconds>
 * no appletalk name-lookup-interval [<seconds>]
 *
 * OBJ(int,1) = seconds
 */
PARAMS_KEYONLY	(cr_at_namelookup, cr_at_rtmp,
	"name-lookup-interval", OBJ(int,1), 0, -1,
	appletalk_command, ATALK_NAMELOOKUP,
	"Set the service polling interval for lookup-type command", 
	"Seconds", PRIV_CONF);

/*************************************************************************
 * appletalk require-route-zones
 * no appletalk require-route-zones
 */
EOLS	(cr_at_routezones_eol, appletalk_command, ATALK_ROUTEZONES);
KEYWORD	(cr_at_routezones, cr_at_routezones_eol, cr_at_namelookup,
	"require-route-zones",
	 "Require network/zone associations before advertising routes",
	 PRIV_CONF);

/*************************************************************************
 * appletalk strict-rtmp-checking
 * no appletalk strict-rtmp-checking
 */
EOLS	(cr_at_strict_eol, appletalk_command, ATALK_STRICT);
KEYWORD	(cr_at_strict, cr_at_strict_eol, cr_at_routezones,
	"strict-rtmp-checking", "Enforce maximum checking of routing packets",
	 PRIV_CONF);

/*************************************************************************
 * appletalk alternate-addressing
 * no appletalk alternate-addressing
 */

EOLS	(cr_at_altaddrs_eol, appletalk_command, ATALK_ALTADDRS);
KEYWORD	(cr_at_altaddrs, cr_at_altaddrs_eol, cr_at_strict,
	"alternate-addressing", "Use alternate addressing", PRIV_CONF);

/*************************************************************************
 * appletalk permit-partial-zones
 * no appletalk permit-partial-zones
 */
EOLS	(cr_at_partialzone_eol, appletalk_command, ATALK_PARTIALZONES);
KEYWORD	(cr_at_partialzone, cr_at_partialzone_eol, cr_at_altaddrs,
	"permit-partial-zones", "Allow zone routing if part of the zone is visible",
	 PRIV_CONF);

/*************************************************************************
 * appletalk local-routing
 *
 */
EOLS    (cr_at_localroute_eol, appletalk_command, ATALK_LOCALROUTE);
KEYWORD (cr_at_localroute, cr_at_localroute_eol, cr_at_partialzone,
	 "local-routing", "Allow routing between two local devices on an "
	 "interface", PRIV_CONF);

/*************************************************************************
 * appletalk testtool-compatible
 * no appletalk testtool-compatible
 */
EOLS	(cr_at_testtool_eol, appletalk_command, ATALK_TESTTOOL);
KEYWORD	(cr_at_testtool, cr_at_testtool_eol, cr_at_localroute,
	"testtool-compatible", "", PRIV_CONF | PRIV_HIDDEN);

/*************************************************************************
 * appletalk ignore-verify-errors
 * no appletalk ignore-verify-errors
 */
EOLS	(cr_at_ignverify_eol, appletalk_command, ATALK_IGN_VERIFY);
KEYWORD	(cr_at_ignverify, cr_at_ignverify_eol, cr_at_testtool,
	"ignore-verify-errors",
	 "Allow router to start routing in misconfigured networks", PRIV_CONF);

/*************************************************************************
 * appletalk checksum
 * no appletalk checksum
 */
EOLS	(cr_at_cksum_eol, appletalk_command, ATALK_CHECKSUM);
KEYWORD	(cr_at_checksum, cr_at_cksum_eol, cr_at_ignverify,
	 "checksum", "Enable Appletalk checksumming", PRIV_CONF);


/**********************************************************************
 * appletalk static {network <net> | cable <cable>} to <addr> [floating] [hop <hopcnt>] zone <zone>
 * no appletalk static {network <net> | cable <cable>} to <addr> [floating] [hop <hopcnt>] zone <zone>
 *
 * OBJ(int,1) = Address if Phase I, Cablerange-Low if Phase II
 * OBJ(int,2) = 0 if Phase I, Cablerange-High, if Phase II
 * OBJ(paddr,1) = next hop address
 * OBJ(int, 3) = TRUE = hop count set
 * OBJ(int, 4) = hop count
 * OBJ(int, 5) = floating static
 * OBJ(string,1) = zone name
 */

EOLS	(cr_at_static_eol, appletalk_command, ATALK_STATIC);
 
TEXT	(cr_at_static_zone_nameno, cr_at_static_eol, no_alt, OBJ(string, 1),
	 "An AppleTalk zone name");

KEYWORD (cr_at_static_zone, cr_at_static_zone_nameno, no_alt,
	  "zone", "Configure static zone to associate with route", PRIV_CONF);

NUMBER  (cr_at_static_hop_num, cr_at_static_zone, cr_at_static_zone, OBJ(int,4), 0, 16, "hop count");

KEYWORD_ID (cr_at_static_hop, cr_at_static_hop_num, cr_at_static_zone,
         OBJ(int,3), TRUE, "hop", "specify hop count", PRIV_CONF|PRIV_HIDDEN);

KEYWORD_ID (cr_at_static_floating, cr_at_static_hop, cr_at_static_hop,
	 OBJ(int,5), TRUE, "floating", "floating static", PRIV_CONF);

NOPREFIX (cr_at_static_noprefix, cr_at_static_floating, cr_at_static_eol);

ATADDR	(cr_at_static_nextaddr, cr_at_static_noprefix, no_alt, 
	 OBJ(paddr,1), "Appletalk net.node address");
 
KEYWORD (cr_at_static_to, cr_at_static_nextaddr, no_alt,
	 "to", "Set static next hop router", PRIV_CONF);


SET     (cr_at_static_address_set, cr_at_static_to, OBJ(int,2), 0);

NUMBER	(cr_at_static_address, cr_at_static_address_set, no_alt,
	 OBJ(int,1), 1, 65279, "Appletalk network number");

KEYWORD (cr_at_static_network, cr_at_static_address, no_alt, 
	 "network", "Configure a static Phase 1 route", PRIV_CONF);

CRANGE  (cr_at_static_crange, cr_at_static_to, no_alt, OBJ(int,1), OBJ(int,2));

KEYWORD (cr_at_static_cable, cr_at_static_crange, cr_at_static_network, 
	  "cable-range", "Configure a static Phase 2 route", PRIV_CONF);

NVGENS  (cr_at_static_nvgen, cr_at_static_cable, appletalk_command,
	  ATALK_STATIC);

KEYWORD (cr_at_static, cr_at_static_nvgen, cr_at_checksum,
	  "static", "Configure a static Appletalk route", PRIV_CONF);



/*************************************************************************/
KEYWORD	(cr_appletalk, cr_at_static, ALTERNATE,
	 "appletalk", "Appletalk global configuration commands",
	 PRIV_CONF | PRIV_DUPLICATE);


#undef	ALTERNATE
#define	ALTERNATE	cr_appletalk
