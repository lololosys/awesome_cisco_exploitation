/* $Id: cfg_novell.h,v 3.4.4.8 1996/08/16 00:14:08 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_novell.h,v $
 *------------------------------------------------------------------
 * C F G _ N O V E L L . H
 *
 * Novell global configuration commands.
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_novell.h,v $
 * Revision 3.4.4.8  1996/08/16  00:14:08  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.4.4.7  1996/08/14  18:09:07  john
 * CSCdi64010:  IPX SPX spoofing stops when spoof table gets large -
 *              need to age out
 * Branch: California_branch
 *
 * Revision 3.4.4.6  1996/07/26  00:08:29  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.4.4.5  1996/06/20  01:53:35  irfan
 * CSCdi60747:  bad line in config ->ipx route-cache nasi-server enable<-
 * Branch: California_branch
 * fix incorrect alternate transition node.
 *
 * Revision 3.4.4.4  1996/06/12  17:36:39  akr
 * CSCdi60228:  ipx route-cache can grow without limit
 * Branch: California_branch
 *
 * Revision 3.4.4.3  1996/06/10  19:24:19  kleung
 * CSCdi57965:  Misc. cosmetic command bugs.
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/05/30  00:36:23  widmer
 * CSCdi58426:  Invalid range for ipx gns-response-delay command
 * Branch: California_branch
 * Fix parser ranges to fit variable size
 * Change %d to %u for nv generation of uint's
 *
 * Revision 3.4.4.1  1996/05/03  02:16:05  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.4  1996/03/09  18:12:42  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.3  1995/12/01  06:24:55  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.2  1995/11/17  19:21:13  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:38:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/26  00:51:14  mschaef
 * CSCdi42340:  ipx maximum-path should allow more than 4 if NLSP is not on
 *
 * Revision 2.3  1995/07/28  23:56:15  john
 * CSCdi37833:  IPX triggered rip/sap delays need a global default
 *
 * Revision 2.2  1995/07/18  22:33:15  john
 * CSCdi34411:  Large ipx output-rip/sap-delays may delay routing from
 *              running
 *
 * Revision 2.1  1995/06/07  23:27:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(global_novell_extend_here, no_alt);

/**********************************************************************
 * ipx route-cache max-size <number>
 * no ipx route-cache max-size 
 */

EOLS (conf_nov_cache_size_eol,
      novell_command, NOVELL_RCACHE_MAX_SIZE);

NUMBER (conf_nov_cache_get_size, conf_nov_cache_size_eol, no_alt,
	OBJ(int,1), 0, MAXUINT, "Size of the route-cache table (0 = no limit)");

NOPREFIX (conf_nov_cache_size_noprefix,
	  conf_nov_cache_get_size, conf_nov_cache_size_eol);

NVGENS (conf_nov_cache_size_nvgen, conf_nov_cache_size_noprefix,
	novell_command, NOVELL_RCACHE_MAX_SIZE);

KEYWORD (conf_nov_cache_size,
	 conf_nov_cache_size_nvgen, no_alt,
	 "max-size", "Set max size of route-cache table",
	 PRIV_CONF);

/**********************************************************************
 * ipx route-cache update-timeout <minutes> [<rate>]
 * no ipx route-cache update-timeout
 */

EOLS (conf_nov_upd_cache_timeout_eol,
      novell_command, NOVELL_RCACHE_MAX_UPDATE_AGE);

NUMBER (conf_nov_upd_cache_get_rate,
	conf_nov_upd_cache_timeout_eol, conf_nov_upd_cache_timeout_eol, 
	OBJ(int,2), 0, MAXUSHORT, "Max invalidations per minute (0 = no limit)");

SET (conf_nov_upd_cache_set_rate, conf_nov_upd_cache_get_rate,
     OBJ(int,2), NOVRCACHE_DEFAULT_UPDATE_AGE_RATE);

NUMBER (conf_nov_upd_cache_get_timeout, conf_nov_upd_cache_set_rate, no_alt,
	OBJ(int,1), 0, MAXUSHORT, "Max age in minutes (0 = no limit)");

NOPREFIX (conf_nov_upd_cache_timeout_noprefix,
	  conf_nov_upd_cache_get_timeout, conf_nov_upd_cache_timeout_eol);

NVGENS (conf_nov_upd_cache_timeout_nvgen, conf_nov_upd_cache_timeout_noprefix,
	novell_command, NOVELL_RCACHE_MAX_UPDATE_AGE);

KEYWORD (conf_nov_upd_cache_timeout,
	 conf_nov_upd_cache_timeout_nvgen, conf_nov_cache_size,
	 "update-timeout", "Set max age of cache entries", PRIV_CONF);

/**********************************************************************
 * ipx route-cache inactivity-timeout <minutes> [<rate>]
 * no ipx route-cache inactivity-timeout
 */

EOLS (conf_nov_age_cache_timeout_eol,
      novell_command, NOVELL_RCACHE_MAX_INACTIVE_AGE);

NUMBER (conf_nov_age_cache_get_rate,
	conf_nov_age_cache_timeout_eol, conf_nov_age_cache_timeout_eol,
	OBJ(int,2), 0, MAXUSHORT, "Max invalidations per minute (0 = no limit)");

SET (conf_nov_age_cache_set_rate, conf_nov_age_cache_get_rate,
     OBJ(int,2), NOVRCACHE_DEFAULT_INACTIVE_AGE_RATE);

NUMBER (conf_nov_age_cache_get_timeout, conf_nov_age_cache_set_rate, no_alt,
	OBJ(int,1), 0, MAXUSHORT, "Age in minutes (0 = no limit)");

NOPREFIX (conf_nov_age_cache_timeout_noprefix,
	  conf_nov_age_cache_get_timeout, conf_nov_age_cache_timeout_eol);

NVGENS (conf_nov_age_cache_timeout_nvgen, conf_nov_age_cache_timeout_noprefix,
	novell_command, NOVELL_RCACHE_MAX_INACTIVE_AGE);

KEYWORD (conf_nov_age_cache_timeout,
	 conf_nov_age_cache_timeout_nvgen, conf_nov_upd_cache_timeout,
	 "inactivity-timeout", "Set max age of inactive cache entries",
	 PRIV_CONF);

KEYWORD_MM (conf_nov_age_cache,
	    conf_nov_age_cache_timeout, global_novell_extend_here,
	    "route-cache", "IPX fastswitch cache configuration", PRIV_CONF, 6);

/*****************************************************************
 * [no] ipx potential-pseudonode
 *
 * This is broken out instead of using a PARAMS_KEYONLY to allow the use
 * of the KEYWORD_MM to require 'potential-pseudonode' to be abbreviated
 * to no less than 11 places (potential-p).
 */
EOLS (conf_nov_ppn_eol, novell_command, NOVELL_POTENTIAL_PSEUDONODE);
KEYWORD_MM (conf_nov_ppn, conf_nov_ppn_eol, conf_nov_age_cache,
	    "potential-pseudonode",
	    "Keep backup route and service data as potential pseudonode",
	    PRIV_CONF|PRIV_HIDDEN, 11);

/*****************************************************************
 * [no] ipx per-host-load-share
 */
EOLS	 (conf_nov_ph_eol, novell_command, NOVELL_PER_HOST);
KEYWORD	 (conf_nov_ph, conf_nov_ph_eol, conf_nov_ppn,
	  "per-host-load-share",
	  "Load share per end host (use one path only)", PRIV_CONF);

/*****************************************************************
 * [no] ipx ping-default [ cisco | novell ]
 */
EOLS	 (conf_nov_ping_deflt_eol, novell_command, NOVELL_PING_DEFAULT);

KEYWORD_ID (conf_nov_ping_deflt_novell, conf_nov_ping_deflt_eol,
            conf_nov_ph, OBJ(int,1), IPX_PING_DEFLT_NOVELL,
            "novell", "use Novell Standard echoes for IPX ping",
            PRIV_CONF);

KEYWORD_ID (conf_nov_ping_deflt_cisco, conf_nov_ping_deflt_eol,
            conf_nov_ping_deflt_novell, OBJ(int,1), IPX_PING_DEFLT_CISCO,
            "cisco", "use cisco echoes for IPX ping",
            PRIV_CONF);

NVGENS  (conf_nov_ping_deflt_nvgen, conf_nov_ping_deflt_cisco,
         novell_command, NOVELL_PING_DEFAULT);

KEYWORD	 (conf_nov_ping_deflt, conf_nov_ping_deflt_nvgen,
	  conf_nov_ph, "ping-default",
	  "Set default to cisco or Novell Standard Pings", PRIV_CONF);

/******************************************************************************
 *	ipx spx-spoof session-clear <minutes>
 *	no ipx spx-spoof session-clear [<minutes>]
 *
 * OBJ(int,1) = number of minutes to clear out idle spx records in
 */

PARAMS_KEYONLY(conf_nov_spx_session_clear, no_alt,
	       "session-clear", OBJ(int,1), 0, -1L, novell_command,
	       NOVELL_SPX_SESSION_CLEAR,
	       "Sets the time in minutes to clear inactive entries",
	       "Session Clear Time", PRIV_CONF);

/******************************************************************************
 *	ipx spx-spoof table-clear <hours>
 *	no ipx spx-spoof table-clear [<hours>]
 *
 * OBJ(int,1) = number of hours to clear spx table in.
 */

PARAMS_KEYONLY(conf_nov_spx_clear, conf_nov_spx_session_clear, "table-clear",
	       OBJ(int,1), 0, -1L, novell_command, NOVELL_SPX_TABLE_CLEAR,
	       "Clear SPX table time in hours",
	       "Clear SPX table time", PRIV_CONF);

KEYWORD	 (conf_nov_spx_spoof, conf_nov_spx_clear,
	  conf_nov_ping_deflt, "spx-spoof",
	  "SPX spoof options", PRIV_CONF);

/*****************************************************************
 * [no] ipx server-split-horizon-on-server-paths
 */
EOLS	 (conf_nov_server_split_eol, novell_command, NOVELL_SERVER_SPLIT);
KEYWORD	 (conf_nov_server_split, conf_nov_server_split_eol,
	  conf_nov_spx_spoof, "server-split-horizon-on-server-paths",
	  "Split horizon SAP on server, not route, paths", PRIV_CONF);

/*****************************************************************
 * [no] ipx kludge
 */
EOLS	 (conf_nov_kludge_eol, novell_command, NOVELL_KLUDGE);
KEYWORD	 (conf_nov_kludge, conf_nov_kludge_eol, conf_nov_server_split,
	  "flooding-unthrottled", "NLSP flooding should be unthrottled",
	  PRIV_CONF);

/*****************************************************************
 * [no] ipx default-route
 */
EOLS	 (conf_nov_rip_def_eol, novell_command, NOVELL_RIP_DEFAULT_ROUTE);
KEYWORD	 (conf_nov_rip_def, conf_nov_rip_def_eol, conf_nov_kludge,
	  "default-route",
	  "Enable default route recognition",
	  PRIV_CONF);

/******************************************************************************
 *	ipx maximum-hops <count>
 *	no ipx maximum-hops [<count>]
 *
 * OBJ(int,1) = maximum number of hops for packet...
 */

PARAMS_KEYONLY(conf_nov_maximum_hops, conf_nov_rip_def, "maximum-hops",
	       OBJ(int,1), 16, 254, novell_command, NOVELL_MAXIMUM_HOPS,
	       "Sets the maximum number of hops",
	       "Max hops", PRIV_CONF);

/******************************************************************************
 *	ipx accounting-transits <count>
 *	no ipx accounting-transits [<count>]
 *
 * OBJ(int,1) = num of transit records to store
 */

PARAMS_KEYONLY(conf_nov_accounting_transits, conf_nov_maximum_hops,
	       "accounting-transits", OBJ(int,1), 0, 32000, novell_command,
	       NOVELL_ACCTTRANSITS,
	       "Sets the maximum number of transit entries",
	       "Max transit entries", PRIV_CONF);

/******************************************************************************
 *	ipx accounting-list <novaddr> <novaddr-mask>
 *	no ipx accounting-list [<novaddr> <novaddr-mask>]
 *
 * OBJ(int,1) =	TRUE if an accounting address and mask given
 *		FALSE for 'no ipx accounting-list'
 *
 * OBJ(paddr,1) = address
 * OBJ(paddr,2) = mask
 *
 */

EOLS	(conf_nov_acctlist_eol, novell_command, NOVELL_ACCTLIST);
SET	(conf_nov_acctlist_set, conf_nov_acctlist_eol, OBJ(int,1), TRUE);
NOVADDR	(conf_nov_acctlist_get_mask, conf_nov_acctlist_set, no_alt,
	 OBJ(paddr,2), "IPX network.host mask");

/* Check for an IPX address.
 * If there was no IPX address, then check for a 'no' prefix and look for EOL.
 */
TESTVAR	(conf_nov_acctlist_noprefix, conf_nov_acctlist_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);
NOVADDR	(conf_nov_acctlist_get_addr, conf_nov_acctlist_get_mask,
	 conf_nov_acctlist_noprefix, OBJ(paddr,1), "IPX network.host of host");
NVGENS	(conf_nov_acctlist_nvgen, conf_nov_acctlist_get_addr,
	 novell_command, NOVELL_ACCTLIST);
KEYWORD	(conf_nov_acctlist, conf_nov_acctlist_nvgen,
	 conf_nov_accounting_transits, "accounting-list",
	 "Select nets for which IPX accounting information is kept", PRIV_CONF);

/******************************************************************************
 *	ipx accounting-threshold <threshold>
 *	no ipx accounting-threshold [<threshold>]
 *
 * OBJ(int,1) = max accounting entries
 */

PARAMS_KEYONLY (conf_nov_accounting_threshold, conf_nov_acctlist,
		"accounting-threshold", OBJ(int,1), 0, -1, novell_command,
		NOVELL_ACCTTHRESHOLD,
		"Sets the maximum number of accounting entries",
		"Max acct entries", PRIV_CONF);

/*****************************************************************
 * [no] ipx broadcast-fastswitching
 *
 */
EOLS	 (conf_nov_fs_bcast_eol, novell_command,
  	  NOVELL_FASTSWITCH_BCAST);
KEYWORD	 (conf_nov_fs_bcast, conf_nov_fs_bcast_eol,
  	  conf_nov_accounting_threshold, "broadcast-fastswitching",
  	  "Fastswitch directed broadcast packets",
  	  PRIV_CONF);

/*****************************************************************
 * [no] ipx type-20-helpered
 *
 */
EOLS	 (conf_nov_type20_helpered_eol, novell_command,
	  NOVELL_TYPE20_HELPERED);
KEYWORD	 (conf_nov_type20_helpered, conf_nov_type20_helpered_eol,
	  conf_nov_fs_bcast, "type-20-helpered",
	  "Forward Type-20 using helper lists, ignore trace",
	  PRIV_CONF);

/*****************************************************************
 * [no] ipx type-20-output-checks
 */
EOLS	 (conf_nov_type20_out_eol, novell_command, NOVELL_TYPE20_OUT);
KEYWORD	 (conf_nov_type20_out, conf_nov_type20_out_eol,
	  conf_nov_type20_helpered, "type-20-output-checks", 
	  "Do additional output checks on type 20 propagation packets",
	  PRIV_CONF);

/*****************************************************************
 * [no] ipx type20-input-checks
 */
EOLS	 (conf_nov_type20_in_eol, novell_command, NOVELL_TYPE20_IN);
KEYWORD	 (conf_nov_type20_in, conf_nov_type20_in_eol, conf_nov_type20_out,
	  "type-20-input-checks", 
	  "Do additional input checks on type 20 propagation packets",
	  PRIV_CONF);

/*****************************************************************
 * [no] ipx netbios-socket-input-checks
 */
EOLS	 (conf_nov_netbios_in_eol, novell_command, NOVELL_NETBIOS_IN);
KEYWORD	 (conf_nov_netbios_in, conf_nov_netbios_in_eol, conf_nov_type20_in,
	  "netbios-socket-input-checks", 
	  "Limit input of non-type 20 netbios bc packets",
	  PRIV_CONF);

/*****************************************************************
 * [no] ipx sap-uses-routing-info
 *
 * This is broken out instead of using a PARAMS_KEYONLY to allow the use
 * of the KEYWORD_MM to require 'sapq-uses-routing-info' to be abbreviated
 * to no less than five places (sap-u).
 */
EOLS (conf_nov_sap_uses_routing_eol, novell_command, NOVELL_SAP_USES_ROUTING);
KEYWORD_MM (conf_nov_sap_uses_routing, conf_nov_sap_uses_routing_eol,
	    conf_nov_netbios_in, "sap-uses-routing-info",
  	    "Use routing information in ordering the SAP table",
	    PRIV_CONF|PRIV_HIDDEN, 5);


/*****************************************************************
 * [no] ipx internal-net <number>
 */
EOLS (conf_nov_internal_net_eol, novell_command, NOVELL_INTERNAL_NET);
HEXDIGIT (conf_nov_internal_net_num, conf_nov_internal_net_eol, no_alt, 
	  OBJ(int,1), NOV_MIN_NET, NOV_MAX_NET-1, "Internal network number");
NVGENS (conf_nov_internal_net_nvgens, conf_nov_internal_net_num,
	novell_command, NOVELL_INTERNAL_NET); 
KEYWORD (conf_nov_internal_net, conf_nov_internal_net_nvgens,
	 conf_nov_sap_uses_routing, "internal-network",
	 "Specify internal IPX network for router", PRIV_CONF);

/*****************************************************************
 * [no] ipx algorithms
 */
EOLS (conf_nov_algorithms_eol, novell_command, NOVELL_ALGORITHMS);
KEYWORD (conf_nov_algorithms, conf_nov_algorithms_eol,
	 conf_nov_internal_net, "algorithms",
	 "Use IPX routing algorithms", PRIV_CONF|PRIV_HIDDEN);


/**********************************************************************
 * ipx backup-server-query-interval <seconds>
 * no ipx backup-server-query-interval [<seconds>]
 */
PARAMS_KEYONLY	(conf_nov_backup_server_int, conf_nov_algorithms,
	 "backup-server-query-interval", OBJ(int,1), 2, 2000,
	 novell_command, NOVELL_BACKUP_SERVER_INTERVAL,
	 "Set minimum interval between successive backup server table queries",
	 "Minimum query interval, in seconds", PRIV_CONF);


/*****************************************************************
 * [no] ipx gns-round-robin
 */
EOLS	 (conf_nov_nsqrr_eol, novell_command, NOVELL_NSQRR);
KEYWORD	 (conf_nov_nsqrr, conf_nov_nsqrr_eol, conf_nov_backup_server_int,
	  "gns-round-robin", 
	  "Round-robin responses to get nearest server", PRIV_CONF);

/**********************************************************************
 * ipx gns-response-delay <number>
 * no ipx gns-reponse-delay [<number>]
 */
PARAMS_KEYONLY	(conf_nov_gns_rd, conf_nov_nsqrr,
	 "gns-response-delay", OBJ(int,1), 0, -1,
	 novell_command, NOVELL_GNS_RESPONSE_DELAY,
	 "Set msec delay in replying to a GNS Request",
	 "Delay in milliseconds", PRIV_CONF);

/**********************************************************************
 * ipx default-triggered-sap-delay <number>
 * [no] ipx default-triggered-sap-delay [<number>]
 */
PARAMS_KEYONLY (conf_nov_def_trig_sap_dly, conf_nov_gns_rd,
		"default-triggered-sap-delay", OBJ(int,1), 0, -1, 
		novell_command, NOVELL_DEFAULT_TRIG_SAP_DELAY,
		"Interpacket delay for triggered SAP updates",
		"Delay in milliseconds", PRIV_CONF);

/**********************************************************************
 * ipx default-triggered-rip-delay <number>
 * [no] ipx default-triggered-rip-delay [<number>]
 */
PARAMS_KEYONLY (conf_nov_def_trig_rip_dly, conf_nov_def_trig_sap_dly,
		"default-triggered-rip-delay", OBJ(int,1), 0, -1, 
		novell_command, NOVELL_DEFAULT_TRIG_RIP_DELAY,
		"Interpacket delay for triggered RIP updates",
		"Delay in milliseconds", PRIV_CONF);

/**********************************************************************
 * ipx default-output-sap-delay <number>
 * [no] ipx default-output-sap-delay [<number>]
 */
PARAMS_KEYONLY (conf_nov_def_sap_dly, conf_nov_def_trig_rip_dly,
		"default-output-sap-delay", OBJ(int,1), 0, -1, 
		novell_command, NOVELL_DEFAULT_SAP_DELAY,
		"Interpacket delay for SAP updates",
		"Delay in milliseconds", PRIV_CONF);

/**********************************************************************
 * ipx default-output-rip-delay <number>
 * [no] ipx default-output-rip-delay [<number>]
 */
PARAMS_KEYONLY (conf_nov_def_rip_dly, conf_nov_def_sap_dly,
		"default-output-rip-delay", OBJ(int,1), 0, -1, 
		novell_command, NOVELL_DEFAULT_RIP_DELAY,
		"Interpacket delay for RIP updates",
		"Delay in milliseconds", PRIV_CONF);

/**********************************************************************
 * ipx throttle-update-proc <number>
 * [no] ipx throttle-update-proc [<number>]
 */
PARAMS_KEYONLY (conf_nov_proc_th, conf_nov_def_rip_dly,
		"throttle-update-proc", OBJ(int,1), 0, -1, 
		novell_command, NOVELL_THROTTLE_UPDATE_PROC,
		"Throttle update process creation",
		"Delay in milliseconds", PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * ipx throttle-update-sched <number>
 * [no] ipx throttle-update-sched [<number>]
 */
PARAMS_KEYONLY (conf_nov_sched_th, conf_nov_proc_th,
		"throttle-update-sched", OBJ(int,1), 0, -1, 
		novell_command, NOVELL_THROTTLE_UPDATE_SCHED,
		"Throttle update scheduling",
		"Delay in milliseconds", PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * ipx sap-queue-maximum <number>
 * no ipx sap-queue-maximum [<number>]
 *
 * This is broken out instead of using a PARAMS_KEYONLY to allow the use
 * of the KEYWORD_MM to require 'sap-queue-maximum' to be abbreviated to
 * no less than five places (sap-u).
 */

EOLS (conf_nov_sqm_eol, novell_command, NOVELL_SAPQUEUEMAX);

NUMBER (conf_nov_sqm_number, conf_nov_sqm_eol, no_alt, OBJ(int,1), 0, -1,
	"Queue depth");

NOPREFIX (conf_nov_sqm_noprefix, conf_nov_sqm_number, conf_nov_sqm_eol);

NVGENS (conf_nov_sqm_nvgen, conf_nov_sqm_noprefix, novell_command,
	NOVELL_SAPQUEUEMAX); 

KEYWORD_MM (conf_nov_sqm, conf_nov_sqm_nvgen, conf_nov_sched_th,
	    "sap-queue-maximum", "Set maximum SAP processing queue depth",
	    PRIV_CONF, 5);

/**********************************************************************
 * 
 * ipx maximum-paths <paths>
 * no ipx maximum-paths [<paths>]
 */
PARAMS_KEYONLY	(conf_novell_maxpath, conf_nov_sqm,
	 "maximum-paths", OBJ(int,1), 1, NOVELL_MAXPATHS,
	 novell_command, NOVELL_PATHS,
	 "Forward IPX packets over multiple paths", 
	 "Number of paths", PRIV_CONF);

/**********************************************************************
 * ipx routing [<host>]
 * no ipx routing [<host>]
 */

EOLS	(conf_novell_routing_eol, novell_command, NOVELL_ADDRESS);
SET	(conf_novell_routing_gotaddr, conf_novell_routing_eol, OBJ(int,1), TRUE);
NOVHOST	(conf_novell_routing_addr, conf_novell_routing_gotaddr,
	 conf_novell_routing_eol, OBJ(paddr,1), "IPX address of this router");
NOPREFIX (conf_novell_norouting, conf_novell_routing_addr, conf_novell_routing_eol);
NVGENS	(conf_novell_routing_nvgen, conf_novell_norouting,
	 novell_command, NOVELL_ADDRESS);
KEYWORD	(conf_novell_routing, conf_novell_routing_nvgen, conf_novell_maxpath,
	 "routing", "Enable IPX routing", PRIV_CONF);
NOVELL_IPX_KEYWORD(configure_novell, conf_novell_routing, ALTERNATE,
		   "Novell/IPX global configuration commands", PRIV_CONF);


#undef  ALTERNATE
#define ALTERNATE configure_novell
