/* $Id: exec_debug_isis.h,v 3.2 1995/11/17 08:53:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/exec_debug_isis.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ I S I S . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_isis.h,v $
 * Revision 3.2  1995/11/17  08:53:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/25  15:11:58  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.2  1995/08/23  18:02:58  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/07/02  01:47:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:48:11  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:49  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug isis spf-statistics
 *
 */
EOLS	(debug_isis_spf_stats_eol, debug_command, DEBUG_ISISSPFSTATS);
KEYWORD (debug_isis_spf_stats, debug_isis_spf_stats_eol, no_alt,
	    "spf-statistics", "IS-IS SPF Timing and Statistic Data", 
	    PRIV_OPR);

/******************************************************************
 * debug isis update-packets
 *
 */
EOLS	(debug_isis_update_packets_eol, debug_command, DEBUG_ISISUPDATEPKT);
KEYWORD (debug_isis_update_packets, debug_isis_update_packets_eol, 
	 debug_isis_spf_stats, "update-packets", 
	 "IS-IS Update related packets", PRIV_OPR);

/******************************************************************
 * debug isis local-updates
 *
 */
EOLS	(debug_isis_local_updates_eol, debug_command, DEBUG_ISISLOCUPDATE);
KEYWORD (debug_isis_local_updates, debug_isis_local_updates_eol, 
	 debug_isis_update_packets, "local-updates", 
	 "IS-IS local update packets", PRIV_OPR);

/******************************************************************
 * debug isis checksum-errors
 *
 */
EOLS	(debug_isis_cksum_errs_eol, debug_command, DEBUG_ISISCKSUMERR);
KEYWORD (debug_isis_cksum_errs, debug_isis_cksum_errs_eol, 
	 debug_isis_local_updates, "checksum-errors",
	 "IS-IS LSP checksum errors", PRIV_OPR);

/******************************************************************
 * debug isis protocol-errors
 *
 */
EOLS	(debug_isis_proto_errs_eol, debug_command, DEBUG_ISISPROTOERR);
KEYWORD (debug_isis_proto_errs, debug_isis_proto_errs_eol, 
	 debug_isis_cksum_errs, "protocol-errors",
	 "IS-IS LSP protocol errors", PRIV_OPR);

/******************************************************************
 * debug isis spf-events [terse]
 *
 */
EOLS	(debug_isis_spf_events_eol, isis_nlsp_debug_command,
	 DEBUG_ISISSPFEVNT);
KEYWORD_ID(debug_isis_spf_events_terse, debug_isis_spf_events_eol,
	   debug_isis_spf_events_eol, OBJ(int,1), TRUE, "terse",
	   "Don't display leaf routes", PRIV_OPR);
KEYWORD (debug_isis_spf_events, debug_isis_spf_events_terse,
	 debug_isis_proto_errs,
	 "spf-events", "IS-IS Shortest Path First Events", PRIV_OPR);

/******************************************************************
 * debug isis spf-triggers
 *
 */
EOLS	(debug_isis_spf_triggers_eol, debug_command, DEBUG_ISISSPFTRIGGERS);
KEYWORD (debug_isis_spf_triggers, debug_isis_spf_triggers_eol,
	 debug_isis_spf_events,
	 "spf-triggers", "IS-IS SPF triggering events", PRIV_OPR);

/******************************************************************
 * debug isis snp-packets
 *
 */
EOLS	(debug_isis_snp_packets_eol, debug_command, DEBUG_ISISSNPPACKETS);
KEYWORD (debug_isis_snp_packets, debug_isis_snp_packets_eol,
	 debug_isis_spf_triggers,
	 "snp-packets", "IS-IS CSNP/PSNP packets", PRIV_OPR);

/******************************************************************
 * debug isis adj-packets
 *
 */
EOLS	(debug_isis_adj_packets_eol, debug_command, DEBUG_ISISADJPKT);
KEYWORD (debug_isis_adj_packets, debug_isis_adj_packets_eol,
	 debug_isis_snp_packets,
	 "adj-packets", "IS-IS Adjacency related packets", PRIV_OPR);

/******************************************************************
 * debug isis adj-packets
 *
 */
KEYWORD_DEBUG(debug_isis, debug_isis_adj_packets, ALTERNATE,
	      OBJ(pdb,1), clns_debug_arr,
	      "isis", "IS-IS Information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_isis
