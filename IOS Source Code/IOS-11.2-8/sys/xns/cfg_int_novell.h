/* $Id: cfg_int_novell.h,v 3.5.18.10 1996/09/14 00:07:09 sseal Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_int_novell.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ N O V E L L . H
 * 
 * Novell interface subcommands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_novell.h,v $
 * Revision 3.5.18.10  1996/09/14  00:07:09  sseal
 * CSCdi61196:  ipx route-cache options need clean up
 * Branch: California_branch
 *
 * Revision 3.5.18.9  1996/08/16  00:14:05  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.5.18.8  1996/08/12  16:09:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5.18.7  1996/07/26  00:08:27  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.5.18.6  1996/06/27  01:10:00  mschaef
 * CSCdi61334:  Alignment error, building IPX route cache entry
 * Branch: California_branch
 *
 * Revision 3.5.18.5  1996/06/10  19:24:14  kleung
 * CSCdi57965:  Misc. cosmetic command bugs.
 * Branch: California_branch
 *
 * Revision 3.5.18.4  1996/05/30  23:47:47  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.18.3  1996/05/30  00:36:21  widmer
 * CSCdi58426:  Invalid range for ipx gns-response-delay command
 * Branch: California_branch
 * Fix parser ranges to fit variable size
 * Change %d to %u for nv generation of uint's
 *
 * Revision 3.5.18.2  1996/05/03  02:15:49  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.5.18.1  1996/04/02  05:37:57  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.5  1996/02/08  03:08:36  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.4  1996/02/01  23:35:07  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/23  23:33:29  dko
 * CSCdi47086:  Maximum value for ipx delay cmd is set too high for 2 bytes
 * Fixed the type of novell_delay to ushort and reduced Max. tick to 65535.
 *
 * Revision 3.2  1995/11/17  19:21:01  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:38:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/18  22:33:13  john
 * CSCdi34411:  Large ipx output-rip/sap-delays may delay routing from
 *              running
 *
 * Revision 2.1  1995/06/07  23:27:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(interface_novell_extend_here, no_alt);

/******************************************************************************
 * ipx advertise-default-route-only <net#>
 * no ipx advertise-default-route-only <net#>
 *
 * OBJ(int,1) = net # to disable all but default route on
 */
EOLS	(ci_nov_advertise_default_route_only_eol, novellif_command,
	 NOVELL_ADVERTISE_DEFAULT_ROUTE_ONLY);
HEXDIGIT (ci_nov_advertise_default_route_only_num,
	  ci_nov_advertise_default_route_only_eol, no_alt,
	  OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET, "IPX network number");
NVGENS	(ci_nov_advertise_default_route_only_nvgen,
	 ci_nov_advertise_default_route_only_num,
	 novellif_command, NOVELL_ADVERTISE_DEFAULT_ROUTE_ONLY);
KEYWORD	(ci_nov_advertise_default_route_only,
	 ci_nov_advertise_default_route_only_nvgen,
	 interface_novell_extend_here, "advertise-default-route-only",
	 "Only advertise the IPX/RIP default route out onto this network",
	 PRIV_CONF|PRIV_SUBIF);

/****************************************************************
 * ipx throughput <bits-per-second>
 * no ipx throughput <bits-per-second>
 */
PARAMS_KEYONLY (ci_ipx_throughput, ci_nov_advertise_default_route_only,
               "throughput", OBJ(int,1), 0, MAXUINT,
               novellif_command, NOVELL_THROUGHPUT,
               "Set IPX link throughput in bit per second",
               "Throughput", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx link-delay <number>
 * no ipx link-delay <number>
 */
PARAMS_KEYONLY (ci_ipx_linkdelay, ci_ipx_throughput,
                "link-delay", OBJ(int,1), 0, MAXUINT,
                novellif_command, NOVELL_LINKDELAY,
                "Set an IPX link delay on the interface, in microseconds",
                "A delay value, in microseconds", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx update-sap-after-rip
 * no ipx update-sap-after-rip
 */
EOLS	(ci_nov_saprip_eol, novellif_command, NOVELL_SAP_AFTER_RIP);
KEYWORD	(ci_nov_saprip, ci_nov_saprip_eol, ci_ipx_linkdelay,
	 "update-sap-after-rip",
	 "Send periodic SAP update after completion of periodic RIP update",
	 PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

/************************************************************************
 *	[no] ipx accounting
 */
EOLNS	(ci_ipx_acct_eol, ipx_accounting_command);
KEYWORD	(ci_ipx_acct, ci_ipx_acct_eol, ci_nov_saprip,
	 "accounting", "Enable IPX accounting on this interface",
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx spx-idle-time <number>
 * no ipx spx-idle-time <number>
 */
PARAMS_KEYONLY (ci_ipx_spxidletime, ci_ipx_acct,
                "spx-idle-time", OBJ(int,1), 0, MAXUINT,
                novellif_command, NOVELL_SPXIDLETIME,
                "Set an SPX idle time on the interface, in seconds",
                "An idle time value, in seconds", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx spx-spoofing
 * no ipx spx-spoofing
 */
EOLS	(ci_ipx_spxspoof_eol, novellif_command, NOVELL_SPXSPOOF);
KEYWORD	(ci_ipx_spxspoof, ci_ipx_spxspoof_eol, ci_ipx_spxidletime,
	 "spx-spoof",
	 "Spoof SPX keepalives packets",
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx watchdog-spoofing
 * no ipx watchdog-spoofing
 */
EOLS	(ci_ipx_watchdogspoof_eol, novellif_command, NOVELL_WATCHDOGSPOOF);
KEYWORD	(ci_ipx_watchdogspoof, ci_ipx_watchdogspoof_eol, ci_ipx_spxspoof,
	 "watchdog-spoof",
	 "Answer Server Watchdog packets for Client machines",
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx pad-process-switched-packets
 * no ipx pad-process-switched-packets
 */
EOLS	(ci_ipx_pad_eol, novellif_command, NOVELL_EVENIZE);
KEYWORD	(ci_ipx_pad, ci_ipx_pad_eol, ci_ipx_watchdogspoof,
	 "pad-process-switched-packets",
	 "Pad odd-length packets on output (process-switched only)",
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx type-20-propagation
 * no ipx type-20-propagation
 */
EOLS	(ci_ipx_type20_eol, novellif_command, NOVELL_TYPE20);
KEYWORD	(ci_ipx_type20, ci_ipx_type20_eol, ci_ipx_pad,
	 "type-20-propagation", 
	 "Forward IPX type 20 propagation packets", PRIV_CONF|PRIV_SUBIF);

/****************************************************************
 * ipx update-time <seconds>
 * no ipx update-time [<seconds>]
 */
PARAMS_KEYONLY (ci_nov_updtimer, ci_ipx_type20,
	       "update-time", OBJ(int,1), 10, -1L,
	       novellif_command, NOVELL_UPDATETIME,
	       "Set IPX routing update timer",
	       "Routing update timer", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx sap-interval <minutes>
 * no ipx sap-interval [<minutes>]
 *
 */
PARAMS_KEYONLY (ci_nov_sapinterval, ci_nov_updtimer,
	 "sap-interval", OBJ(int,1), 0, MAXINT,
	 novellif_command, NOVELL_SAPINT,
	 "Set SAP update period",
	 "Minutes (0=update only at boot & shutdown)", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx router-sap-filter <number>
 * no ipx router-sap-filter [<number>]
 */
EOLS    (ci_nov_rtsapfilter_eol, novellif_command, NOVELL_SAP_ROUTERFILTER);
ACCESS_LIST(ci_nov_rtsapfilter_num, ci_nov_rtsapfilter_eol, no_alt,
	 OBJ(int,1), MINNOVSAPACC, MAXNOVSAPACC,
	 IPX_MIN_SAP_ACL, IPX_MAX_SAP_ACL, "An valid IPX SAP access list number");
NOPREFIX(ci_nov_rtsapfilter_no, ci_nov_rtsapfilter_num, 
	 ci_nov_rtsapfilter_eol);
NVGENS  (ci_nov_rtsapfilter_nv, ci_nov_rtsapfilter_no, novellif_command,
	 NOVELL_SAP_ROUTERFILTER);
KEYWORD_MM (ci_nov_rtsapfilter, ci_nov_rtsapfilter_nv, ci_nov_sapinterval,
	    "router-sap-filter", 
	    "Select source router and service type of SAP updates",
	    PRIV_CONF|PRIV_SUBIF, 8);	/* Disambiguate "router" */

/*****************************************************************
 * ipx router-filter <number>
 * no ipx router-filter [<number>]
 */
EOLS    (ci_nov_rtfilter_eol, novellif_command, NOVELL_ROUTER_FILTER);
ACCESS_LIST(ci_nov_rtfilter_num, ci_nov_rtfilter_eol, no_alt,
	 OBJ(int,1), MINNOVACC, MAXNOVACC,
	 IPX_MIN_STD_ACL, IPX_MAX_STD_ACL, "A valid IPX access list number");
NOPREFIX(ci_nov_rtfilter_no, ci_nov_rtfilter_num, ci_nov_rtfilter_eol);
NVGENS  (ci_nov_rtfilter_nv, ci_nov_rtfilter_no, novellif_command,
	 NOVELL_ROUTER_FILTER);
KEYWORD_MM (ci_nov_rtfilter, ci_nov_rtfilter_nv, ci_nov_rtsapfilter,
	    "router-filter", "Filter sources of routing updates",
	    PRIV_CONF|PRIV_SUBIF, 8); /* Disambiguate "router" */

/*****************************************************************
 * ipx route-cache [cbus] [sse]
 * no ipx route-cache [cbus] [sse]
 * OBJ(int,1) = 0 by default
 *		NOVELL_RCACHE_CBUS if 'cbus' was specified
 *		NOVELL_RCACHE_SSE if 'sse' was specified
 */
EOLS	(ci_nov_rcache_eol, novellif_command, NOVELL_ROUTEC);
LINK_TRANS(ci_nov_rcache_return_here, ci_nov_rcache_eol);
LINK_TRANS(ci_nov_rcache_extend_here, ci_nov_rcache_eol); 
KEYWORD	(ci_nov_rcache, ci_nov_rcache_extend_here, ci_nov_rtfilter,
	 "route-cache", "Enable fast switching", PRIV_CONF);

/*****************************************************************
 * ipx gns-response-delay <number>
 * no ipx gns_response-delay [<number>]
 */
PARAMS_KEYONLY	(ci_nov_gns_delay, ci_nov_rcache,
	"gns-response-delay", OBJ(int,1), 0, MAXUINT,
	 novellif_command, NOVELL_INT_GNS_RESPONSE_DELAY,
	 "Delay in answering GNS on this interface",
	 "Interval, in ms, 100 = 0.1 sec", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx gns-reply-disable
 * no ipx gns-reply-disable
 */
EOLS	(ci_nov_gns_reply_disable_eol, novellif_command,
	 NOVELL_GNS_REPLY_DISABLE);
KEYWORD	(ci_nov_gns_reply_disable, ci_nov_gns_reply_disable_eol,
	 ci_nov_gns_delay, "gns-reply-disable",
	 "Disable Get Nearest Server replies on this interface",
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx sap-multiplier <number>
 * no ipx sap-multiplier [<number>]
 */
PARAMS_KEYONLY	(ci_nov_sap_multiplier, ci_nov_gns_reply_disable,
	"sap-multiplier", OBJ(int,1), 1, 1000,
	 novellif_command, NOVELL_SAP_MULTIPLIER,
	 "Multiple of SAP update interval for aging of SAP routes",
	 "Interval, in minutes", PRIV_CONF|PRIV_SUBIF);


/*****************************************************************
 * ipx sap-max-packetsize <number>
 * no ipx sap-max-packetsize [<number>]
 */
PARAMS_KEYONLY	(ci_nov_sap_max_packetsize, ci_nov_sap_multiplier,
	"sap-max-packetsize", OBJ(int,1), NOVELL_SAP_MAXSIZE, 4096,
	 novellif_command, NOVELL_SAP_MAX_PACKETSIZE,
	 "Maximum size of SAP packets being sent on interface",
	 "Size of packets", PRIV_CONF|PRIV_SUBIF);


/*****************************************************************
 * ipx rip-max-packetsize <number>
 * no ipx rip-max-packetsize [<number>]
 */
PARAMS_KEYONLY	(ci_nov_rip_max_packetsize, ci_nov_sap_max_packetsize,
	"rip-max-packetsize", OBJ(int,1), NOVELL_RIP_MAXSIZE, 4096,
	 novellif_command, NOVELL_RIP_MAX_PACKETSIZE,
	 "Maximum size of RIP packets being sent on interface",
	 "Size of packets", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx rip-multiplier <number>
 * no ipx rip-multiplier [<number>]
 */
PARAMS_KEYONLY	(ci_nov_rip_multiplier, ci_nov_rip_max_packetsize,
	"rip-multiplier", OBJ(int,1), 1, 1000,
	 novellif_command, NOVELL_RIP_MULTIPLIER,
	 "Multiple of RIP update interval for aging of RIP routes",
	 "Interval, in minutes", PRIV_CONF|PRIV_SUBIF);


/*****************************************************************
 * ipx output-gns-filter <number>
 * no ipx output-gns-filter [<number>]
 */
PARAMS_ALIST(ci_nov_outnsqfilter, ci_nov_rip_multiplier,
	"output-gns-filter", OBJ(int,1), MINNOVSAPACC, MAXNOVSAPACC,
	 IPX_MIN_SAP_ACL, IPX_MAX_SAP_ACL, 
	 novellif_command, NOVELL_NSQ_OUTFILTER,
	 "Filter services reported in response to Get Nearest Server",
	 "A valid IPX SAP access list number", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx output-sap-filter <number>
 * no ipx output-sap-filter [<number>]
 */
PARAMS_ALIST  (ci_nov_outsapfilter, ci_nov_outnsqfilter,
	       "output-sap-filter", OBJ(int,1), MINNOVSAPACC, MAXNOVSAPACC,
	       IPX_MIN_SAP_ACL, IPX_MAX_SAP_ACL,
	       novellif_command, NOVELL_SAP_OUTFILTER,
	       "Filter services reported via SAP",
	       "A valid IPX SAP access list number", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx output-sap-delay <delay>
 * no ipx output-sap-delay [<delay>]
 */
PARAMS_KEYONLY(ci_nov_outsapdelay, ci_nov_outsapfilter,
	       "output-sap-delay", OBJ(int,1), 0, MAXUINT,
	       novellif_command, NOVELL_SAP_DELAY,
	       "Interpacket delay for SAP updates",
	       "Delay in milliseconds",
	       PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx output-rip-delay <delay>
 * no ipx output-rip-delay [<delay>]
 */
PARAMS_KEYONLY(ci_nov_outripdelay, ci_nov_outsapdelay,
	       "output-rip-delay", OBJ(int,1), 0, MAXUINT,
	       novellif_command, NOVELL_RIP_DELAY,
	       "Interpacket delay for RIP updates",
	       "Delay in milliseconds",
	       PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx triggered-rip-delay <delay>
 * no ipx triggered-rip-delay [<delay>]
 */
PARAMS_KEYONLY(ci_nov_trigripdelay, ci_nov_outripdelay,
	       "triggered-rip-delay", OBJ(int,1), 0, MAXUINT,
	       novellif_command, NOVELL_TRIG_RIP_DELAY,
	       "Interpacket delay for triggered RIP updates (override output-rip-delay for triggered updates only)",
	       "Delay in milliseconds",
	       PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx triggered-sap-delay <delay>
 * no ipx triggered-sap-delay [<delay>]
 */
PARAMS_KEYONLY(ci_nov_trigsapdelay, ci_nov_trigripdelay,
	       "triggered-sap-delay", OBJ(int,1), 0, -1L,
	       novellif_command, NOVELL_TRIG_SAP_DELAY,
	       "Interpacket delay for triggered SAP updates (override output-rip-delay for triggered updates only)",
	       "Delay in milliseconds",
	       PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx output-network-filter <number>
 * no ipx output-network-filter [<number>]
 */
PARAMS_ALIST  (ci_nov_outnetfilter, ci_nov_trigsapdelay,
	       "output-network-filter", OBJ(int,1), MINNOVACC, MAXENOVACC,
	       IPX_MIN_STD_ACL, IPX_MAX_EXT_ACL,
	       novellif_command, NOVELL_OUT_FILTER,
	       "Filter outgoing routing updates",
	       "A valid IPX access list number",
	       PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx down <net>
 * OBJ(int,1) = net
 */
EOLS	(ci_nov_down_eol, novellif_command, NOVELL_DOWN);
HEXDIGIT (ci_nov_down_num, ci_nov_down_eol, no_alt,
	  OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET, "IPX network number");
NVGENS	(ci_nov_down_nvgen, ci_nov_down_num,
	 novellif_command, NOVELL_DOWN);
KEYWORD	(ci_nov_down, ci_nov_down_nvgen, ci_nov_outnetfilter,
	 "down", "Bring an IPX network administratively down",
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx network <net> [encapsulation {novell-ether|arpa|snap|sap}]
 *			[secondary]
 * no ipx network [<net>]
 *
 * OBJ(int,1) = net - 0x0 if unnumbered, 0xffffffff if unspecified.
 * OBJ(int,2) = encapsulation  (= ET_NULL if no encapsulation specified )
 * OBJ(int,3) = secondary flag (= TRUE if "secondary" keyword present)
 */
EOLS	(ci_nov_netw_eol, novellif_command, NOVELL_NETWORK);

KEYWORD_ID(ci_nov_netw_sec, ci_nov_netw_eol, ci_nov_netw_eol,
	   OBJ(int,3), TRUE,
	   "secondary", "Make this network a secondary network",
	   PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_nov_netw_tr, ci_nov_netw_sec, no_alt,
	   OBJ(int,2), ET_SAP,
	   "novell-tr", "IEEE 802.2 on Token Ring",
	   PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);
KEYWORD_ID(ci_nov_netw_iso1, ci_nov_netw_sec, ci_nov_netw_tr,
	   OBJ(int,2), ET_SAP,
	   "iso1", "", PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);
KEYWORD_ID(ci_nov_netw_sap, ci_nov_netw_sec, ci_nov_netw_iso1,
	   OBJ(int,2), ET_SAP,
	   "sap", "IEEE 802.2 on Ethernet, FDDI, Token Ring",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_netw_snap, ci_nov_netw_sec, ci_nov_netw_sap,
	   OBJ(int,2), ET_SNAP,
	   "snap", "IEEE 802.2 SNAP on Ethernet, Token Ring, and FDDI",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_netw_hdlc, ci_nov_netw_sec, ci_nov_netw_snap,
	   OBJ(int,2), ET_HDLC,
	   "hdlc", "HDLC on serial links", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_netw_arpa, ci_nov_netw_sec, ci_nov_netw_hdlc,
	   OBJ(int,2), ET_ARPA,
	   "arpa", "IPX Ethernet_II", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_netw_fddi, ci_nov_netw_sec, ci_nov_netw_arpa,
	   OBJ(int,2), ET_NOVELL_FDDI,
	   "novell-fddi", "IPX FDDI RAW", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_netw_ether, ci_nov_netw_sec, ci_nov_netw_fddi,
	   OBJ(int,2), ET_NOVELL_ETHER,
	   "novell-ether", "IPX Ethernet_802.3", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_nov_netw_enc, ci_nov_netw_ether, ci_nov_netw_eol,
	   OBJ(int,2), ET_NOVELL_ETHER,
	   "encapsulation", "IPX encapsulation", PRIV_CONF|PRIV_SUBIF);
NOPREFIX(ci_nov_netw_noprefix, ci_nov_netw_enc, ci_nov_netw_eol);
HEXDIGIT (ci_nov_netw_num_off, ci_nov_netw_noprefix, ci_nov_netw_eol,
	 OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET, "IPX network number");
HEXDIGIT (ci_nov_netw_num_on, ci_nov_netw_noprefix, ci_nov_netw_eol,
	 OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET-1,
	  "IPX network number (default route enabled)");
IFELSE(ci_nov_netw_assert, ci_nov_netw_num_on, ci_nov_netw_num_off,
       novell_default_route);
KEYWORD_ID (ci_nov_netw_numorun, ci_nov_netw_noprefix, ci_nov_netw_assert,
	    OBJ(int,1), 0,
	    "unnumbered", NULL /*"unnumbered interface"*/,
	    PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

SET     (ci_nov_netw_nosec, ci_nov_netw_numorun,   OBJ(int,3), FALSE);
SET     (ci_nov_netw_noenc, ci_nov_netw_nosec, OBJ(int,2), ET_NULL);
SET     (ci_nov_netw_nonet, ci_nov_netw_noenc, OBJ(int,1), 0xffffffff);

NVGENS	(ci_nov_netw_nvgen, ci_nov_netw_nonet,
	 novellif_command, NOVELL_NETWORK);
KEYWORD	(ci_nov_netw, ci_nov_netw_nvgen, ci_nov_down,
	 "network", "Assign an IPX network & enable IPX routing",
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx input-sap-filter <number>
 * no ipx input-sap-filter [<number>]
 */
PARAMS_ALIST  (ci_nov_insapfilter, ci_nov_netw,
	       "input-sap-filter", OBJ(int,1), MINNOVSAPACC, MAXNOVSAPACC,
	       IPX_MIN_SAP_ACL, IPX_MAX_SAP_ACL,
	       novellif_command, NOVELL_SAP_INFILTER,
	       "Filter services learned from the Service Advertising Protocol",
	       "A valid IPX SAP access list number", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx input-network-filter <number>
 * no ipx input-network-filter [<number>]
 */
PARAMS_ALIST  (ci_nov_innetfilter, ci_nov_insapfilter,
	       "input-network-filter", OBJ(int,1), MINNOVACC, MAXENOVACC,
	       IPX_MIN_STD_ACL, IPX_MAX_EXT_ACL,
	       novellif_command, NOVELL_IN_FILTER,
	       "Filter incoming routing updates",
	       "A valid IPX access list number", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx helper-list <number>
 * no ipx helper-list [<number>]
 */
PARAMS_ALIST  (ci_nov_helperlist, ci_nov_innetfilter,
	       "helper-list", OBJ(int,1), MINNOVACC, MAXENOVACC,
	       IPX_MIN_STD_ACL, IPX_MAX_EXT_ACL,
	       novellif_command, NOVELL_HLPLIST,
	       "Filter helpered IPX packets on input",
	       "A valid IPX access list number", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx helper-address <net.host>
 * no ipx helper-address [<net.host>]
 */
EOLS	(ci_nov_helperaddr_eol, novellif_command, NOVELL_HLPADDR);
TESTVAR (ci_nov_nohelpaddr, ci_nov_helperaddr_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);
NOVADDR	(ci_nov_helperaddr_addr, ci_nov_helperaddr_eol, ci_nov_nohelpaddr,
	 OBJ(paddr,1),"N.HHHH.HHHH.HHHH");
NVGENS	(ci_nov_helperaddr_nvgen, ci_nov_helperaddr_addr, novellif_command,
	 NOVELL_HLPADDR);
KEYWORD	(ci_nov_helperaddr, ci_nov_helperaddr_nvgen, ci_nov_helperlist,
	 "helper-address", "Forward broadcasts to a specific address",
	 PRIV_CONF|PRIV_SUBIF);


/****************************************************************
 * ipx encapsulation { novell-ether | arpa }
 * no ipx encapsulation [{ novell-ether | arpa }]
 *
 * OBJ(int,1) = ET_NOVELL_ETHER or ET_ARPA
 */

EOLS	(ci_nov_encap_eol, novellif_command, NOVELL_ENCTYPE);
KEYWORD_ID(ci_nov_encap_tr, ci_nov_encap_eol, no_alt,
	   OBJ(int,1), ET_SAP,
	   "novell-tr", "IEEE 802.2 on Token Ring",
	   PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);
KEYWORD_ID(ci_nov_encap_iso1, ci_nov_encap_eol, ci_nov_encap_tr,
	   OBJ(int,1), ET_SAP,
	   "iso1", "", PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);
KEYWORD_ID(ci_nov_encap_sap, ci_nov_encap_eol, ci_nov_encap_iso1,
	   OBJ(int,1), ET_SAP,
	   "sap", "IEEE 802.2 on Ethernet, Token Ring, and FDDI",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_encap_snap, ci_nov_encap_eol, ci_nov_encap_sap,
	   OBJ(int,1), ET_SNAP,
	   "snap", "IEEE 802.2 SNAP on Ethernet, Token Ring, and FDDI",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_encap_arpa, ci_nov_encap_eol, ci_nov_encap_snap,
	   OBJ(int,1), ET_ARPA,
	   "arpa", "IPX Ethernet_II", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_encap_hdlc, ci_nov_encap_eol, ci_nov_encap_arpa,
	   OBJ(int,1), ET_HDLC,
	   "hdlc", "HDLC on serial links", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_nov_encap_ether, ci_nov_encap_eol, ci_nov_encap_hdlc,
	   OBJ(int,1), ET_NOVELL_ETHER,
	   "novell-ether", "IPX Ethernet_802.3", PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_nov_encap_noprefix, ci_nov_encap_ether, ci_nov_encap_eol);
KEYWORD	(ci_nov_encap, ci_nov_encap_noprefix, ci_nov_helperaddr,
	 "encapsulation", "IPX encapsulation", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx delay <number>
 * no ipx delay <number>
 */
PARAMS_KEYONLY (ci_nov_delay, ci_nov_encap,
		"delay", OBJ(int,1), 0, 65535,
		novellif_command, NOVELL_DELAY,
		"Set an IPX delay on the interface, in 'ticks'",
		"A delay value, in 'ticks'", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * ipx ppp-client 
 * no ipx ppp-client
 */
EOLS (ci_nov_client_eol, novellif_command, NOVELL_PPPCLIENT);
FORWARD_INTERFACE  (ci_nov_client_interface, ci_nov_client_eol, no_alt,
	    OBJ(idb,1), IFTYPE_LOOPBACK);

NVGENS(ci_nov_client_nvgen, ci_nov_client_interface,
	novellif_command, NOVELL_PPPCLIENT);

KEYWORD (ci_nov_client, ci_nov_client_nvgen, no_alt,
	 "ppp-client", "Configure interface for PPP client mode", 
		PRIV_CONF|PRIV_SUBIF);

ASSERT (ci_nov_client_assert, ci_nov_client, ci_nov_delay,
	is_p2p(csb->interface));

/*****************************************************************
 * ipx access-group <number> [in | out]
 * no ipx access-group [in | out]
 */

EOLS    (ci_nov_agrp_out_eol, novellif_command, NOVELL_ACCESS);
KEYWORD (ci_nov_agrp_out, ci_nov_agrp_out_eol, ci_nov_agrp_out_eol,
	 "out", "outbound packets", PRIV_CONF|PRIV_SUBIF);

EOLS    (ci_nov_agrp_in_eol, novellif_command, NOVELL_INACCESS);
KEYWORD (ci_nov_agrp_in, ci_nov_agrp_in_eol, ci_nov_agrp_out,
	 "in", "inbound packets", PRIV_CONF|PRIV_SUBIF);

TESTVAR	(ci_nov_agrp_noprefix, ci_nov_agrp_in, no_alt,
	 NONE, NONE, NONE, sense, FALSE);

ACCESS_LIST(ci_nov_agrp_grp, ci_nov_agrp_in, ci_nov_agrp_noprefix,
	 OBJ(int,1), MINNOVACC, MAXENOVACC,
	 IPX_MIN_STD_ACL, IPX_MAX_EXT_ACL,
	 "A valid IPX access list number");
NVGENS  (ci_nov_agrp_nvgen, ci_nov_agrp_grp, novellif_command, NOVELL_ACCESS);
KEYWORD (ci_nov_agrp, ci_nov_agrp_nvgen, ci_nov_client_assert,
	 "access-group", "Apply an access list to inbound or outbound packets",
	 PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * Must match at least 3 chars to distinguish 'no' from 'novell'.
 */
NOVELL_IPX_KEYWORD (ci_novell, ci_nov_agrp, NONE,
		    "Novell/IPX interface subcommands", PRIV_CONF|PRIV_SUBIF);

/*****************************************************************
 * Must be a main interface or an IEEE (E/TR/FDDI) subinterface or a
 * VC subinterface
 */

ASSERT  (ci_nov_subif, ci_novell, ALTERNATE,
	 main_if_or_iftype(csb->interface, IDB_IEEEMASK) ||
	 main_if_or_vc(csb->interface));

#undef  ALTERNATE
#define ALTERNATE ci_nov_subif
