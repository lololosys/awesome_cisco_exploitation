/* $Id: exec_debug_ipx_nlsp.h,v 3.3.18.1 1996/08/12 16:10:03 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_debug_ipx_nlsp.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ I P X _ N L S P . H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ipx_nlsp.h,v $
 * Revision 3.3.18.1  1996/08/12  16:10:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/08  03:08:37  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.2  1995/11/17  19:21:29  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/25  15:12:37  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.3  1995/08/23  18:05:28  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.2  1995/06/21  18:18:33  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  23:27:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ipx nlsp spf-statistics
 *
 */
EOLS	(debug_nlsp_spf_stats_eol, debug_command, DEBUG_NLSPSPFSTATS);
KEYWORD (debug_nlsp_spf_stats, debug_nlsp_spf_stats_eol, no_alt,
	    "spf-statistics", "NLSP SPF Timing and Statistics Data", 
	    PRIV_OPR);

/******************************************************************
 * debug ipx nlsp spf-triggers
 *
 */
EOLS	(debug_nlsp_spf_triggers_eol, debug_command, DEBUG_NLSPSPFTRIGGERS);
KEYWORD (debug_nlsp_spf_triggers, debug_nlsp_spf_triggers_eol, 
	 debug_nlsp_spf_stats, "spf-triggers", 
	 "NLSP SPF triggering events", PRIV_OPR);

/******************************************************************
 * debug ipx nlsp snp-packets
 *
 */
EOLS	(debug_nlsp_snp_packets_eol, debug_command, DEBUG_NLSPSNPPACKETS);
KEYWORD (debug_nlsp_snp_packets, debug_nlsp_snp_packets_eol, 
	 debug_nlsp_spf_triggers, "snp-packets", 
	 "NLSP CSNP/PSNP packets", PRIV_OPR);

/******************************************************************
 * debug ipx nlsp update-packets
 *
 */
EOLS	(debug_nlsp_update_packets_eol, debug_command, DEBUG_NLSPUPDATEPKT);
KEYWORD (debug_nlsp_update_packets, debug_nlsp_update_packets_eol, 
	 debug_nlsp_snp_packets, "update-packets", 
	 "NLSP Update related packets", PRIV_OPR);

/******************************************************************
 * debug ipx nlsp local-updates
 *
 */
EOLS	(debug_nlsp_local_updates_eol, debug_command, DEBUG_NLSPLOCUPDATE);
KEYWORD (debug_nlsp_local_updates, debug_nlsp_local_updates_eol, 
	 debug_nlsp_update_packets, "local-updates", 
	 "NLSP local update packets", PRIV_OPR);

/******************************************************************
 * debug ipx nlsp checksum-errors
 *
 */
EOLS	(debug_nlsp_cksum_errors_eol, debug_command, DEBUG_NLSPCKSUMERR);
KEYWORD (debug_nlsp_cksum_errors, debug_nlsp_cksum_errors_eol, 
	 debug_nlsp_local_updates, "checksum-errors", 
	 "NLSP LSP checksum errors", PRIV_OPR);

/******************************************************************
 * debug ipx nlsp protocol-errors
 *
 */
EOLS	(debug_nlsp_proto_errors_eol, debug_command, DEBUG_NLSPPROTOERR);
KEYWORD (debug_nlsp_proto_errors, debug_nlsp_proto_errors_eol, 
	 debug_nlsp_cksum_errors, "protocol-errors", 
	 "NLSP LSP protocol errors", PRIV_OPR);

/******************************************************************
 * debug ipx nlsp spf-events [terse]
 *
 */
EOLS	(debug_nlsp_spf_events_eol, isis_nlsp_debug_command,
	 DEBUG_NLSPSPFEVNT);
KEYWORD_ID(debug_nlsp_spf_events_terse, debug_nlsp_spf_events_eol,
	   debug_nlsp_spf_events_eol, OBJ(int,1), TRUE, "terse",
	   "Don't display leaf routes", PRIV_OPR);
KEYWORD (debug_nlsp_spf_events, debug_nlsp_spf_events_terse,
	 debug_nlsp_proto_errors,
	 "spf-events", "NLSP Shortest Path First Events", PRIV_OPR);

/******************************************************************
 * debug ipx nlsp adj-packets
 *
 */
EOLS	(debug_nlsp_adj_packets_eol, debug_command, DEBUG_NLSPADJPKT);
KEYWORD (debug_nlsp_adj_packets, debug_nlsp_adj_packets_eol,
	 debug_nlsp_spf_events,
	 "adj-packets", "NLSP Adjacency related packets", PRIV_OPR);

/******************************************************************
 * debug ipx nlsp events
 * debug ipx nlsp activity
 * debug ipx nlsp <ipx-network-number>
 *
 */
EOLS    (debug_novell_nlsp_events_eol, debug_command, DEBUG_NOVELLNLSPEVT);
EOLS    (debug_novell_nlsp_activity_eol, debug_command, DEBUG_NOVELLNLSPACT);
EOLNS    (debug_novell_nlsp_target_eol, novnlsp_debug_command);
HEXDIGIT (debug_novell_nlsp_addr,
	  debug_novell_nlsp_target_eol, debug_nlsp_adj_packets,
	  OBJ(int, 1), NOV_MIN_NET, NOV_RIP_DEFAULT_ROUTE, "IPX network");
KEYWORD (debug_novell_nlsp_activity,
	 debug_novell_nlsp_activity_eol, debug_novell_nlsp_addr,
	 "activity", "IPX NLSP activity", PRIV_OPR);
KEYWORD (debug_novell_nlsp_events,
	 debug_novell_nlsp_events_eol, debug_novell_nlsp_activity,
	 "events", "IPX NLSP events", PRIV_OPR);
KEYWORD (debug_novell_nlsp,
	 debug_novell_nlsp_events, ALTERNATE,
	 "nlsp", "IPX NLSP activity", PRIV_OPR);

#if 0
/******************************************************************
 * debug ipx isis [events]
 * debug ipx isis [<ipx-network-number>]
 *
 */
EOLS    (debug_novell_isis_events_eol, debug_command, DEBUG_NOVELLISISEVT);
EOLS    (debug_novell_isis_eol, debug_command, DEBUG_NOVELLISIS);
EOLNS    (debug_novell_isis_target_eol, novisis_debug_command);
HEXDIGIT (debug_novell_isis_addr,
	  debug_novell_isis_target_eol, debug_novell_isis_eol,
	  OBJ(int, 1), NOV_MIN_NET, NOV_RIP_DEFAULT_ROUTE, "IPX network");
KEYWORD (debug_novell_isis_events,
	 debug_novell_isis_events_eol, debug_novell_isis_addr,
	 "events", NULL /*"IPX ISIS events"*/, PRIV_OPR|PRIV_HIDDEN);
KEYWORD (debug_novell_isis,
	 debug_novell_isis_events, debug_novell_nlsp,
	 "isis", NULL /*"IPX ISIS packets"*/, PRIV_OPR|PRIV_HIDDEN);
#endif

#undef ALTERNATE
#define ALTERNATE debug_novell_nlsp
