/* $Id: cfg_int_ip_igmp.h,v 3.6.28.9 1996/08/26 06:48:50 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/cfg_int_ip_igmp.h,v $
 *------------------------------------------------------------------
 * IP multicast related interface commands. Includes IGMP and DVMRP commands.
 *
 * November 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_igmp.h,v $
 * Revision 3.6.28.9  1996/08/26  06:48:50  dino
 * CSCdi67098:  Do not accept DVMRP neighbor probes/reports from
 * non-pruners
 * Branch: California_branch
 *
 * Revision 3.6.28.8  1996/08/12  16:03:12  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.6.28.7  1996/06/26  23:52:11  tylin
 * CSCdi61175:  dvmrp keyword can not be used with route-maps in <ip dvmrp
 * metric>
 * Branch: California_branch
 *
 * Revision 3.6.28.6  1996/05/22  19:19:58  mleelani
 * CSCdi58181:  7000: crash at _fast_check
 * Branch: California_branch
 * Wrong check for slow acl.
 *
 * Revision 3.6.28.5  1996/05/14  00:12:19  mleelani
 * CSCdi56461:  European cisco MBONE needs ip dvmrp metric-offset out
 * command
 * Branch: California_branch
 * Support outgoing DVMRP metric offsets.
 *
 * Revision 3.6.28.4  1996/05/06  16:07:19  dino
 * CSCdi56516:  The sd proecss doesnt process sdr annoucements
 * Branch: California_branch
 *
 * Revision 3.6.28.3  1996/04/24  05:08:25  lwei
 * CSCdi46338:  Incorrect options listed for ip dvmrp metric 2 list ?
 * Branch: California_branch
 *
 * Revision 3.6.28.2  1996/04/20  01:50:54  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.6.28.1  1996/04/03  23:18:57  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.6  1996/01/12  22:55:56  mleelani
 * CSCdi46802:  IGMP needs faster leave response
 * Miscellaneous v2 enhancements.
 *
 * Revision 3.5  1995/12/02  17:34:41  jzwiebel
 * CSCdi43978:  ip dvmrp unicast-routing should no longer be hidden
 *
 * Revision 3.4  1995/11/30  04:41:58  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.3  1995/11/21  23:11:55  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  17:29:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.3  1995/07/01  07:46:27  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.2  1995/06/21  18:16:47  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:05:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] ip sdr listen
 */
EOLS (ci_ip_sdr_eols, sd_commands, SDR_LISTEN_COMMAND);
KEYWORD (ci_ip_sdr_listen, ci_ip_sdr_eols, no_alt,
	 "listen",  "Listen for sdr packets on interface",
	 PRIV_CONF|PRIV_SUBIF);
KEYWORD (ci_ip_sdr, ci_ip_sdr_listen, ALTERNATE, "sdr",
	    "Session Directory Protocol interface commands",
	    PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip mroute-cache
 *
 */
EOLNS (ci_ip_mcache_eols, ip_mroute_cache_command);
KEYWORD (ci_ip_mcache, ci_ip_mcache_eols, ci_ip_sdr, "mroute-cache",
	 "Enable fast-switching cache for incoming multicast packets",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip dvmrp auto-summary
 */
EOLS (ci_ip_dvmrp_auto_eols, dvmrp_auto_summary_command, 0);
KEYWORD (ci_ip_dvmrp_auto, ci_ip_dvmrp_auto_eols, ALTERNATE,
	 "auto-summary", "Enable DVMRP auto summarization",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip dvmrp summary-address <address> <mask> metric <metric>
 *
 * OBJ(paddr,1) = <address>
 * OBJ(paddr,2) = <mask>
 * OBJ(int,1) = <metric>, default is 1.
 */
EOLS (ci_ip_dvmrp_sa_eols, dvmrp_summary_address_command, 0);
SET (ci_ip_dvmrp_sa_default, ci_ip_dvmrp_sa_eols, OBJ(int,1), 
     DVMRP_DEFAULT_SUMMARY_METRIC);
NUMBER (ci_ip_dvmrp_sa_metric_num, ci_ip_dvmrp_sa_eols, no_alt,
	OBJ(int,1), 1, 32, "Metric value");
KEYWORD (ci_ip_dvmrp_sa_metric, ci_ip_dvmrp_sa_metric_num, 
	 ci_ip_dvmrp_sa_default, "metric", 
	 "Metric advertised with summary address", PRIV_CONF|PRIV_SUBIF);
IPADDR (ci_ip_dvmrp_sa_mask, ci_ip_dvmrp_sa_metric, no_alt, OBJ(paddr,2), 
	"IP netmask");
IPADDR (ci_ip_dvmrp_sa_addr, ci_ip_dvmrp_sa_mask, no_alt, OBJ(paddr,1), 
	"IP address");
NVGENS (ci_ip_dvmrp_sa_nvgen, ci_ip_dvmrp_sa_addr,
	dvmrp_summary_address_command, 0);
KEYWORD (ci_ip_dvmrp_sa, ci_ip_dvmrp_sa_nvgen, ci_ip_dvmrp_auto,
	 "summary-address", "Do DVMRP route summarization",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip dvmrp reject-non-pruners
 */
EOLS (ci_ip_dvmrp_reject_eols, igmp_commands, IGMP_DVMRP_REJECT_NON_PRUNERS);
KEYWORD (ci_ip_dvmrp_reject, ci_ip_dvmrp_reject_eols, ci_ip_dvmrp_sa,
	 "reject-non-pruners", 
	 "Do not peer with non pruning/grafting DVMRP neighbors",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *    ip dvmrp metric-offset <increment>
 * no ip dvmrp metric-offset
 *
 * OBJ(int,1) = <increment>
 * OBJ(int,2) = 0 => incoming metric offset.
 *            = 1 => outgoing metric offset.
 */
EOLS (ci_ip_dvmrp_moffset_eols, igmp_commands, IGMP_DVMRP_MOFFSET);
NUMBER (ci_ip_dvmrp_moffset_num, ci_ip_dvmrp_moffset_eols, no_alt,
	OBJ(int,1), 1, 31, "DVMRP metric offset");
NOPREFIX (ci_ip_dvmrp_moffset_noprefix, ci_ip_dvmrp_moffset_num, 
	  ci_ip_dvmrp_moffset_eols);
KEYWORD_ID(ci_ip_dvmrp_moffset_out, ci_ip_dvmrp_moffset_noprefix,
	   ci_ip_dvmrp_moffset_num, OBJ(int,2), 1,
	   "out", "DVMRP outgoing Reports",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_ip_dvmrp_moffset_in, ci_ip_dvmrp_moffset_noprefix,
	   ci_ip_dvmrp_moffset_out, OBJ(int,2), 0,
	   "in", "DVMRP incoming Reports",
	   PRIV_CONF|PRIV_SUBIF);
NVGENS (ci_ip_dvmrp_moffset_nvgen, ci_ip_dvmrp_moffset_in, 
	igmp_commands, IGMP_DVMRP_MOFFSET);
KEYWORD_MM (ci_ip_dvmrp_moffset, ci_ip_dvmrp_moffset_nvgen, ci_ip_dvmrp_reject,
	    "metric-offset", "DVMRP metric offset for Reports",
	    PRIV_CONF|PRIV_SUBIF, 7);

/************************************************************************
 * ip dvmrp unicast-routing
 *
 */
EOLS (ci_ip_dvmrp_unicast_eols, igmp_commands, IGMP_DVMRP_UNICAST);
KEYWORD (ci_ip_dvmrp_unicast, ci_ip_dvmrp_unicast_eols, ci_ip_dvmrp_moffset,
	 "unicast-routing", "Enable DVMRP unicast routing",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * ip dvmrp default-information originate | only
 * [no] ip dvmrp default-information [originate | only]
 *
 * OBJ(int,1) = DVMRP_SEND_DEFAULT if "originate, DVMRP_DEFAULT_ONLY if "only".
 */
EOLS (ci_ip_dvmrp_default_eols, igmp_commands, IGMP_DVMRP_DEFAULT);
KEYWORD_ID (ci_ip_dvmrp_default_orig, ci_ip_dvmrp_default_eols, no_alt,
	    OBJ(int,1), DVMRP_SEND_DEFAULT,
	    "originate", "DVMRP advertise 0.0.0.0", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_ip_dvmrp_default_only, ci_ip_dvmrp_default_eols,
	    ci_ip_dvmrp_default_orig, OBJ(int,1), DVMRP_DEFAULT_ONLY,
	    "only", "DVMRP advertise 0.0.0.0 only", PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_ip_dvmrp_default_no, ci_ip_dvmrp_default_only,
	  ci_ip_dvmrp_default_eols);
NVGENS (ci_ip_dvmrp_default_nvgen, ci_ip_dvmrp_default_no, igmp_commands,
	IGMP_DVMRP_DEFAULT);
KEYWORD (ci_ip_dvmrp_default, ci_ip_dvmrp_default_nvgen, ci_ip_dvmrp_unicast,
	 "default-information", "DVMRP advertise default route", 
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *      ip dvmrp accept-filter <access-list> [neighbor-list <acl>] [<distance>]
 *   no ip dvmrp accept-filter <access-list>
 *
 * OBJ(int,1) | OBJ(string,1) = <access-list>
 * OBJ(int,2) | OBJ(string,2) = <nbr_acl>
 * OBJ(int,3) = <distance>
 *
 */
EOLS (ci_ip_dvmrp_filter_eols, igmp_commands, IGMP_DVMRP_FILTER);
NUMBER (ci_ip_dvmrp_filter_distance, ci_ip_dvmrp_filter_eols,
	ci_ip_dvmrp_filter_eols, OBJ(int,3), 0, 255, 
	"Administrative distance");
GENERAL_STRING (ci_ip_dvmrp_filter_nbr_nacl, ci_ip_dvmrp_filter_distance,
		no_alt, OBJ(string,2), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (ci_ip_dvmrp_filter_nbr_stest, ci_ip_dvmrp_filter_nbr_nacl,
		     no_alt, NONE);
NUMBER (ci_ip_dvmrp_filter_nbr_acl, ci_ip_dvmrp_filter_distance,
	ci_ip_dvmrp_filter_nbr_stest,
	OBJ(int,2), MINFASTACC, MAXFASTACC, "Access list number");
KEYWORD (ci_ip_dvmrp_filter_nbr, ci_ip_dvmrp_filter_nbr_acl,
	 ci_ip_dvmrp_filter_distance, "neighbor-list", "Neighbors to filter",
	 PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_ip_dvmrp_filter_no, ci_ip_dvmrp_filter_nbr,
	  ci_ip_dvmrp_filter_eols);
GENERAL_STRING (ci_ip_dvmrp_filter_nacl, ci_ip_dvmrp_filter_no, no_alt,
		OBJ(string,1), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (ci_ip_dvmrp_filter_stest, ci_ip_dvmrp_filter_nacl,
		     no_alt, NONE);
NUMBER (ci_ip_dvmrp_filter_acl, ci_ip_dvmrp_filter_nbr,
	ci_ip_dvmrp_filter_stest, OBJ(int,1), MINFASTACC, MAXFASTACC,
	"Access list number");
NVGENS (ci_ip_dvmrp_filter_nvgen, ci_ip_dvmrp_filter_acl, igmp_commands,
	IGMP_DVMRP_FILTER);
KEYWORD (ci_ip_dvmrp_filter, ci_ip_dvmrp_filter_nvgen, ci_ip_dvmrp_default,
	 "accept-filter", "DVMRP incoming Report filters",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip dvmrp metric <metric> [list <access-list>] [route-map <map-name>]
 *			[<protocol> <AS-number> | dvmrp]
 *
 * OBJ(int,1) = <metric>
 * OBJ(int,2) | OBJ(string,2) = <access-list>
 * OBJ(int,3) = TRUE, if "dvmrp", otherwise FALSE
 * OBJ(pdb,1) = Unicast routing protocol and AS/process id.
 * OBJ(string,1) = <map-name>
 */
EOLS (ci_ip_dvmrp_metric_eols, igmp_commands, IGMP_DVMRP_METRIC);
ROUTINGPROTO (ci_ip_dvmrp_metric_pdb, ci_ip_dvmrp_metric_eols,
	      ci_ip_dvmrp_metric_eols);
KEYWORD_ID (ci_ip_dvmrp_metric_dvmrp, ci_ip_dvmrp_metric_eols,
	    ci_ip_dvmrp_metric_pdb, OBJ(int,3), TRUE,
	    "dvmrp", "DVMRP routes", PRIV_CONF);
STRING (ci_ip_dvmrp_metric_routemap_str, ci_ip_dvmrp_metric_pdb, no_alt,
	OBJ(string,1), "Route-map name");
KEYWORD (ci_ip_dvmrp_metric_routemap, ci_ip_dvmrp_metric_routemap_str, 
 	 ci_ip_dvmrp_metric_dvmrp, "route-map", 
 	 "Route-map reference", PRIV_CONF);
GENERAL_STRING (ci_ip_dvmrp_metric_nacl, ci_ip_dvmrp_metric_routemap, no_alt,
		OBJ(string,2), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (ci_ip_dvmrp_metric_stest, ci_ip_dvmrp_metric_nacl,
		     no_alt, NONE);
NUMBER (ci_ip_dvmrp_metric_acl, ci_ip_dvmrp_metric_routemap, 
	ci_ip_dvmrp_metric_stest, OBJ(int,2), MINFASTACC, MAXFASTACC,
	"Access list number");
KEYWORD (ci_ip_dvmrp_metric_list, ci_ip_dvmrp_metric_acl, 
	 ci_ip_dvmrp_metric_routemap, "list", "Access-list",
	 PRIV_CONF|PRIV_SUBIF);
NUMBER (ci_ip_dvmrp_metric_num, ci_ip_dvmrp_metric_list, no_alt,
	OBJ(int,1), 0, 32, "DVMRP Report metric");
NVGENS (ci_ip_dvmrp_metric_nvgen, ci_ip_dvmrp_metric_num, igmp_commands,
	IGMP_DVMRP_METRIC);
KEYWORD (ci_ip_dvmrp_metric, ci_ip_dvmrp_metric_nvgen, ci_ip_dvmrp_filter,
	 "metric", "DVMRP Report metric", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_ip_dvmrp, ci_ip_dvmrp_metric, ci_ip_mcache,
	 "dvmrp", "DVMRP interface commands", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip cgmp [proxy]
 * [no] ip cgmp process-leaves  (hidden command)
 * [no] ip cgmp batch-joins  (hidden command)
 *
 * OBJ(int,1) = TRUE if "proxy" specified
 */
EOLNS (ci_ip_cgmp_eol, cgmp_commands);
KEYWORD_ID (ci_ip_cgmp_proxy, ci_ip_cgmp_eol, ci_ip_cgmp_eol, 
	    OBJ(int,1), TRUE, "proxy", "CGMP proxy for other routers", 
	    PRIV_CONF|PRIV_SUBIF);
NVGENNS (ci_ip_cgmp_nvgen, ci_ip_cgmp_proxy, cgmp_commands);
EOLNS (ci_ip_cgmp_bj_eol, cgmp_batch_joins_command);
KEYWORD (ci_ip_cgmp_bj, ci_ip_cgmp_bj_eol, ci_ip_cgmp_nvgen,
	 "batch-joins", "Send CGMP batch Join messages when IGMP",
	 PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);
EOLNS (ci_ip_cgmp_leave_eol, cgmp_process_leaves_command);
KEYWORD (ci_ip_cgmp_leave, ci_ip_cgmp_leave_eol, ci_ip_cgmp_bj,
	 "process-leaves", 
	 "Send CGMP Leave messages when IGMP Leaves received",
	 PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);

KEYWORD (ci_ip_cgmp, ci_ip_cgmp_leave, ci_ip_dvmrp,
	 "cgmp", "Enable/disable CGMP", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * ip igmp query-timeout <timeout (sec)>
 * no ip igmp query-timeout
 *
 * OBJ(int,1) = <timeout (sec)>
 */
EOLS (ci_ip_igmp_timeout_eol, igmp_commands, IGMP_QUERY_TIMEOUT);
NUMBER (ci_ip_igmp_timeout_num, ci_ip_igmp_timeout_eol, no_alt,
	OBJ(int,1), 60, 300, "timeout value in seconds ");
NOPREFIX (ci_ip_igmp_timeout_no, ci_ip_igmp_timeout_num,
	  ci_ip_igmp_timeout_eol);
NVGENS (ci_ip_igmp_timeout_nvgen, ci_ip_igmp_timeout_no, igmp_commands,
	IGMP_QUERY_TIMEOUT);
KEYWORD (ci_ip_igmp_timeout, ci_ip_igmp_timeout_nvgen, no_alt,
	 "querier-timeout",
	 "IGMP previous querier timeout", PRIV_CONF|PRIV_SUBIF);


/************************************************************************
 * ip igmp query-max-response-time <response time (sec)>
 * no ip igmp query-max-response-time
 *
 * OBJ(int,1) = <response time (sec)>
 */
EOLS (ci_ip_igmp_response_eol, igmp_commands, IGMP_QUERY_RESPONSE);
NUMBER (ci_ip_igmp_response_num, ci_ip_igmp_response_eol, no_alt,
	OBJ(int,1), 1, 25, "query response value in seconds");
NOPREFIX (ci_ip_igmp_response_no, ci_ip_igmp_response_num,
	  ci_ip_igmp_response_eol);
NVGENS (ci_ip_igmp_response_nvgen, ci_ip_igmp_response_no, igmp_commands,
	IGMP_QUERY_RESPONSE);
KEYWORD (ci_ip_igmp_response, ci_ip_igmp_response_nvgen, ci_ip_igmp_timeout,
	 "query-max-response-time",
	 "IGMP max query response value", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * ip igmp version 1 | 2
 * no ip igmp version
 *
 * OBJ(int,1) = 1 or 2
 */
EOLS (ci_ip_igmp_version_eol, igmp_commands, IGMP_VERSION);
NUMBER (ci_ip_igmp_version_num, ci_ip_igmp_version_eol, no_alt,
	OBJ(int,1), 1, 0x2, "version number");
NOPREFIX (ci_ip_igmp_version_no, ci_ip_igmp_version_num,
	  ci_ip_igmp_version_eol);
NVGENS (ci_ip_igmp_version_nvgen, ci_ip_igmp_version_no, igmp_commands,
	IGMP_VERSION);
KEYWORD (ci_ip_igmp_version, ci_ip_igmp_version_nvgen, ci_ip_igmp_response,
	 "version", "IGMP version", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * ip igmp access-group <list>
 * no ip igmp access-group
 *
 * OBJ(int,1) = <list>
 */
EOLS (ci_ip_igmp_access_eol, igmp_commands, IGMP_ACCESS_GROUP);
GENERAL_STRING (ci_ip_igmp_access_nal, ci_ip_igmp_access_eol, no_alt,
		OBJ(string,1), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(ci_ip_igmp_access_stest, ci_ip_igmp_access_nal, no_alt,
		    NONE);
NUMBER (ci_ip_igmp_access_num, ci_ip_igmp_access_eol, ci_ip_igmp_access_stest,
	OBJ(int,1), MINFASTACC, MAXFASTACC, "Access list number");
NOPREFIX (ci_ip_igmp_access_no, ci_ip_igmp_access_num, ci_ip_igmp_access_eol);
NVGENS (ci_ip_igmp_access_nvgen, ci_ip_igmp_access_no, igmp_commands,
	IGMP_ACCESS_GROUP);
KEYWORD (ci_ip_igmp_access, ci_ip_igmp_access_nvgen, ci_ip_igmp_version,
	 "access-group", "IGMP group access group", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * ip igmp query-interval <interval>
 * no ip igmp query-interval
 *
 * OBJ(int,1) = <interval>
 */
EOLS (ci_ip_igmp_query_eol, igmp_commands, IGMP_QUERY_INTVL);
NUMBER (ci_ip_igmp_query_num, ci_ip_igmp_query_eol, no_alt,
	OBJ(int,1), 1, 0xffff, "Query interval in seconds");
NOPREFIX (ci_ip_igmp_query_no, ci_ip_igmp_query_num, ci_ip_igmp_query_eol);
NVGENS (ci_ip_igmp_query_nvgen, ci_ip_igmp_query_no, igmp_commands,
	IGMP_QUERY_INTVL);
KEYWORD (ci_ip_igmp_query, ci_ip_igmp_query_nvgen, ci_ip_igmp_access,
	 "query-interval", "IGMP host query interval", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip igmp static-group <group-address>
 *
 * OBJ(paddr,1) = <group address>
 */
EOLS (ci_ip_igmp_static_eol, igmp_commands, IGMP_STATIC_GROUP);
IPADDR (ci_ip_igmp_static_addr, ci_ip_igmp_static_eol, no_alt,
	OBJ(paddr,1), "IP group address");
NVGENS (ci_ip_igmp_static_nvgen, ci_ip_igmp_static_addr, igmp_commands,
	IGMP_STATIC_GROUP);
KEYWORD (ci_ip_igmp_static, ci_ip_igmp_static_nvgen, ci_ip_igmp_query,
	 "static-group", "IGMP static multicast group", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * [no] ip igmp join-group <group-address>
 *
 * OBJ(paddr,1) = <group address>
 */
EOLS (ci_ip_igmp_join_eol, igmp_commands, IGMP_JOIN_GROUP);
IPADDR (ci_ip_igmp_join_addr, ci_ip_igmp_join_eol, no_alt,
	OBJ(paddr,1), "IP group address");
NVGENS (ci_ip_igmp_join_nvgen, ci_ip_igmp_join_addr, igmp_commands,
	IGMP_JOIN_GROUP);
KEYWORD (ci_ip_igmp_join, ci_ip_igmp_join_nvgen, ci_ip_igmp_static,
	 "join-group", "IGMP join multicast group", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_ip_igmp, ci_ip_igmp_join, ci_ip_cgmp,
	 "igmp", "IGMP interface commands", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_igmp
