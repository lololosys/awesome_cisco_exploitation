/* $Id: cfg_tarp.h,v 3.2.62.2 1996/05/17 21:03:24 asastry Exp $
 * $Source: /release/112/cvs/Xsys/tarp/cfg_tarp.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_tarp.h,v $
 * Revision 3.2.62.2  1996/05/17  21:03:24  asastry
 * CSCdi57772:  Missed out a line in the log; add it.
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/17  20:55:36  asastry
 * CSCdi57772:  Fix misc. TARP bugs found during interoperability testing
 *
 *               - Don't change NSEL during TARP packet propagation
 *               - Implement loop detection as per amended TARP spec
 *               - Don't propagate TARP packet if TTL is zero (after
 *                 decrement)
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:44:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(conf_tarp_extend_here, no_alt);

/******************************************************************************
 * [no] tarp tid <name>
 *
 */
EOLS	(conf_tarp_tid_eol, tarp_command, TARP_TID);
STRING	(tarp_host_name, conf_tarp_tid_eol, no_alt,
	 OBJ(string,1), "Target ID to give to this host");
NVGENS	(conf_tarp_host_nvgen, tarp_host_name, tarp_command, TARP_TID);
KEYWORD	(conf_tarp_host, conf_tarp_host_nvgen, no_alt,
	 "tid", "Define a target ID (name) for this host", PRIV_CONF);

/******************************************************************************
 * tarp urc <0 | 1>
 * [no] tarp urc [0 | 1]
 *
 * Set update remote cache bit.
 */

PARAMS_KEYONLY (conf_tarp_urc, conf_tarp_host, "urc", OBJ(int,1), 0, 1,
		tarp_command, TARP_URC,
		"Set update remote cache (URC) bit in outgoing packet",
		"URC value", PRIV_CONF);

/******************************************************************************
 * [no] tarp protocol-type <hex digit>
 *
 */
EOLS	(conf_tarp_host_prot_type_eol, tarp_command, TARP_PROTOCOL_TYPE);
HEXDIGIT  (conf_tarp_host_prot_type, conf_tarp_host_prot_type_eol, no_alt,
	   OBJ(int,1), TARP_MIN_PROTOCOL_TYPE, TARP_MAX_PROTOCOL_TYPE,
	   "TARP protocol type");
NVGENS	(conf_tarp_prot_type_nvgen, conf_tarp_host_prot_type, tarp_command,
	 TARP_PROTOCOL_TYPE);
KEYWORD	(conf_tarp_prot_type, conf_tarp_prot_type_nvgen, conf_tarp_urc,
	 "protocol-type",
	 "Specify protocol type to be used in outgoing packets",
	 PRIV_CONF);

/******************************************************************************
 * [no] tarp nselector-type <hex digit>
 *
 */
EOLS	(conf_tarp_host_nsel_type_eol, tarp_command, TARP_NSEL_TYPE);
HEXDIGIT  (conf_tarp_host_nsel_type, conf_tarp_host_nsel_type_eol, no_alt,
	   OBJ(int,1), TARP_MIN_NSEL_TYPE, TARP_MAX_NSEL_TYPE,
	   "TARP N-selector value");
NVGENS	(conf_tarp_nsel_type_nvgen, conf_tarp_host_nsel_type, tarp_command,
	 TARP_NSEL_TYPE);
KEYWORD	(conf_tarp_nsel_type, conf_tarp_nsel_type_nvgen, conf_tarp_prot_type,
	 "nselector-type", "N-selector to be used in TARP PDU's", PRIV_CONF);

/******************************************************************************
 * tarp lifetime <number>
 * no tarp lifetime [<number>]
 *
 */

PARAMS_KEYONLY (conf_tarp_lifetime, conf_tarp_nsel_type, "lifetime",
		OBJ(int,1), TARP_MIN_LIFETIME, TARP_MAX_LIFETIME,
		tarp_command, TARP_LIFETIME,
		"Specify value for packet lifetime",
		"TARP packet lifetime", PRIV_CONF);

/******************************************************************************
 * tarp sequence-number <number>
 * no tarp sequence-number [<number>]
 *
 */

PARAMS_KEYONLY (conf_tarp_sequence, conf_tarp_lifetime, "sequence-number",
		OBJ(int,1), TARP_MIN_SEQUENCE_NUMBER, TARP_MAX_SEQUENCE_NUMBER,
		tarp_command, TARP_SEQUENCE,
		"Specify sequence number for next outgoing packet",
		"TARP sequence number", PRIV_CONF);

/******************************************************************************
 * tarp map <name> <NSAP>
 * no tarp map <name>
 */
EOLS	(conf_tarp_map_eol, tarp_command, TARP_MAP);

NSAPADDR(tarp_host_addr1, conf_tarp_map_eol, no_alt,
	 OBJ(paddr,1), "CLNS NET corresponding to this TID");
STRING	(tarp_map_name, tarp_host_addr1, no_alt,
	 OBJ(string,1), "Target ID for this static cache entry");
NVGENS	(conf_tarp_map_nvgen, tarp_map_name, tarp_command, TARP_MAP);
KEYWORD	(conf_tarp_map, conf_tarp_map_nvgen, conf_tarp_sequence,
	 "map", "Add a static map entry in TID<->NET cache", PRIV_CONF);

/******************************************************************************
 * [no] tarp blacklist-adjacency <NSAP>
 */
EOLS	(conf_disallow_nsap_eol, tarp_command, TARP_BLACKLIST_CMD);

NSAPADDR(tarp_disallow_nsap, conf_disallow_nsap_eol, no_alt, OBJ(paddr,1),
	 "NSAP of adjacency that should be blacklisted");
NVGENS	(conf_tarp_disallow_nvgen, tarp_disallow_nsap, tarp_command,
	 TARP_BLACKLIST_CMD);
KEYWORD	(conf_tarp_disallow_adj, conf_tarp_disallow_nvgen, conf_tarp_map,
	 "blacklist-adjacency",
	 "Disallow TARP PDU propagation based on NSAP", PRIV_CONF);

/******************************************************************************
 * [no] tarp route-static <NSAP>
 */
EOLS	(conf_static_nsap_eol, tarp_command, TARP_STATIC_ADJ_CMD);

NSAPADDR(tarp_static_nsap, conf_static_nsap_eol, no_alt, OBJ(paddr,1),
	 "NSAP of static TARP adjacency");
NVGENS	(conf_tarp_static_nvgen, tarp_static_nsap, tarp_command,
	 TARP_STATIC_ADJ_CMD);
KEYWORD	(conf_tarp_static_adj, conf_tarp_static_nvgen,
	 conf_tarp_disallow_adj, "route-static",
	 "Configure a static TARP adjacency", PRIV_CONF);

/******************************************************************************
 * [no] tarp global-propagate 
 *
 * We have 'global' as part of the command, in order to distinguish the
 * interface-specific flavor of this command (which is 'tarp propagate').
 */

EOLS	(conf_tarp_propagate_eol, tarp_command, TARP_PROPAGATE_CMD);
NVGENS	(conf_tarp_propagate_nvgen, conf_tarp_propagate_eol, tarp_command,
	 TARP_PROPAGATE_CMD);
KEYWORD	(conf_tarp_propagate, conf_tarp_propagate_nvgen,
	 conf_tarp_static_adj, "global-propagate",
	 "Enable capability to propagate TARP PDUs (globally)", PRIV_CONF);

/******************************************************************************
 * [no] tarp allow-caching
 *
 */

EOLS	(conf_tarp_disable_cache_eol, tarp_command, TARP_DISABLE_CACHE_CMD);
NVGENS	(conf_tarp_disable_cache_nvgen, conf_tarp_disable_cache_eol,
	 tarp_command, TARP_DISABLE_CACHE_CMD);
KEYWORD	(conf_tarp_disable_cache, conf_tarp_disable_cache_nvgen,
	 conf_tarp_propagate, "allow-caching",
	 "Enable TID<->NET cache (globally)", PRIV_CONF);

/******************************************************************************
 * [no] tarp originate
 *
 */

EOLS	(conf_tarp_originate_eol, tarp_command, TARP_ORIGINATE_CMD);
NVGENS	(conf_tarp_originate_nvgen, conf_tarp_originate_eol, tarp_command,
	 TARP_ORIGINATE_CMD);
KEYWORD	(conf_tarp_originate, conf_tarp_originate_nvgen,
	 conf_tarp_disable_cache, "originate",
	 "Enable capability to originate TARP PDUs", PRIV_CONF);

/******************************************************************************
 * [no] tarp post-t2-response-timer <interval>
 *
 */
PARAMS_KEYONLY( conf_tarp_post_t2_resp_timer, conf_tarp_originate,
	       "post-t2-response-timer", OBJ(int,1),
	        TARP_MIN_POST_TYPE2_TIMEOUT/ONESEC,
	        TARP_MAX_POST_TYPE2_TIMEOUT/ONESEC,
		tarp_command, TARP_POST_TYPE2_TIMER,
	        "Specify timeout for a second T2 timer (sec)",
	        "Timeout value for a second T2 timer (sec)",
	        PRIV_CONF);

/******************************************************************************
 * [no] tarp arp-request-timer <interval>
 *
 */
PARAMS_KEYONLY( conf_tarp_arp_req_timer, conf_tarp_post_t2_resp_timer,
	       "arp-request-timer", OBJ(int,1),
		TARP_MIN_ARP_RQST_TIMEOUT/ONESEC,
	        TARP_MAX_ARP_RQST_TIMEOUT/ONESEC,
		tarp_command, TARP_ARP_TIMER,
	        "Specify timeout for response to ARP request (sec)",
	        "Timeout value for response to ARP request (sec)",
	        PRIV_CONF);

/******************************************************************************
 * [no] tarp ldb-zero-sequence-timer <interval>
 *
 */
PARAMS_KEYONLY( conf_tarp_ldb_zero_sequence_timer, conf_tarp_arp_req_timer,
	       "ldb-zero-sequence-timer",
		OBJ(int,1), TARP_MIN_LDB_ZERO_SEQ_TIMER/ONESEC,
	        TARP_MAX_LDB_ZERO_SEQ_TIMER/ONESEC,
		tarp_command, TARP_LDB_ZERO_SEQ_TIMER,
	      "Specify how long incoming zero-seq # packets are ignored (sec)",
	      "Timeout value for zero sequence timer (sec)",
	        PRIV_CONF);

/******************************************************************************
 * [no] tarp t2-resp-timer <interval>
 *
 */
PARAMS_KEYONLY( conf_tarp_t2_resp_timer, conf_tarp_ldb_zero_sequence_timer,
	       "t2-response-timer",
		OBJ(int,1), TARP_MIN_TYPE2_TIMEOUT/ONESEC,
	        TARP_MAX_TYPE2_TIMEOUT/ONESEC,
		tarp_command, TARP_T2_RESP_TIMER,
	        "Specify timeout for response to type 2 TARP PDU (sec)",
	        "Timeout value for response to type 2 TARP PDU (sec)",
	        PRIV_CONF);

/******************************************************************************
 * [no] tarp t1-resp-timer <interval>
 *
 */
PARAMS_KEYONLY( conf_tarp_t1_resp_timer, conf_tarp_t2_resp_timer,
	       "t1-response-timer",
		OBJ(int,1), TARP_MIN_TYPE1_TIMEOUT/ONESEC,
	        TARP_MAX_TYPE1_TIMEOUT/ONESEC,
		tarp_command, TARP_T1_RESP_TIMER,
	        "Specify timeout for response to type 1 TARP PDU (sec)",
	        "Timeout value for response to type 1 TARP PDU (sec)",
	        PRIV_CONF);

/******************************************************************************
 * [no] tarp cache-timer <interval>
 *
 */
PARAMS_KEYONLY( conf_tarp_tid_cache_timer, conf_tarp_t1_resp_timer,
	        "cache-timer",
		OBJ(int,1), TARP_MIN_TID_CACHE_TIMER/ONESEC,
	        TARP_MAX_TID_CACHE_TIMER/ONESEC,
		tarp_command, TARP_TID_TIMER,
	        "Specify timeout for TID<->NET cache entry (sec)",
	        "Timeout value for TID cache entry (sec)", PRIV_CONF);

/******************************************************************************
 * [no] tarp ldb-timer <interval>
 *
 */
PARAMS_KEYONLY( conf_tarp_ldb_timer, conf_tarp_tid_cache_timer, "ldb-timer",
		OBJ(int,1), TARP_MIN_LDB_FLUSH_TIMER/ONESEC,
	        TARP_MAX_LDB_FLUSH_TIMER/ONESEC,
		tarp_command, TARP_LDB_TIMER,
	        "Specify timeout for loop database entry (sec)",
	        "Timeout value for loop database entry (sec)", PRIV_CONF);

/******************************************************************************
 * [no] tarp run
 *
 */
EOLS	(conf_tarp_running_eol, tarp_command, TARP_RUNNING);
NVGENS	(conf_tarp_nvgen, conf_tarp_running_eol, tarp_command, TARP_RUNNING);
KEYWORD	(conf_tarp_running, conf_tarp_nvgen, conf_tarp_ldb_timer,
	 "run", "Start TARP process", PRIV_CONF);


KEYWORD	(configure_tarp, conf_tarp_running, ALTERNATE, "tarp",
	 "Global TARP configuration subcommands", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_tarp

