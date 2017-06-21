/* $Id: at_igrp2_chain.c,v 3.9.4.4 1996/08/12 16:00:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_igrp2_chain.c,v $
 *------------------------------------------------------------------
 * Parse chain file for AppleTalk EIGRP
 *
 * September 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_igrp2_chain.c,v $
 * Revision 3.9.4.4  1996/08/12  16:00:27  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.9.4.3  1996/07/10  22:18:14  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.9.4.2  1996/05/21  09:42:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.4.1  1996/04/02  05:35:43  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.9  1996/03/07  23:30:08  kleung
 * CSCdi45795:  Apple: Filter Statement Incorrectly Placed in Tunnel Config
 *
 * Revision 3.8  1996/02/22  14:32:00  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/01  23:32:52  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.6  1996/02/01  05:59:03  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:26:06  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/18  02:12:04  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.3  1995/12/17  18:24:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:42:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/28  00:25:32  slin
 * Placeholders for AppleTalk Code Modularization
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_atalk.h"
#include "../parser/parser_defs_parser.h"
#include "../iprouting/parser_defs_igrp2.h"
#include "atalk_private.h"
#include "at_igrp2.h"
#include "at_igrp2_debug.h"

/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
/******************************************************************
 * debug apple eigrp-all	- turn on all igrp debugging
 * debug apple eigrp-packet	- noise on IGRP packets being sent
 * debug apple eigrp-external	- IGRP external routing info
 * debug apple eigrp-reply	- IGRP reply events
 * debug apple eigrp-request	- IGRP request events
 * debug apple eigrp-update	- IGRP update events
 * debug apple eigrp-redist	- Redist of RTMP <-> IGRP events
 * debug apple eigrp [<cable-range>] - Set appletalk target address
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 *
 */
EOLS	(debug_apple_eigrp_external_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_EXTERNAL);
INTERFACE (debug_apple_eigrp_external_int_name, debug_apple_eigrp_external_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID(debug_apple_eigrp_external_int,
	   debug_apple_eigrp_external_int_name,
	   debug_apple_eigrp_external_eol,
	   OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	   common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_eigrp_external, debug_apple_eigrp_external_int,
	 no_alt, "eigrp-external", "AT/EIGRP external functions", PRIV_OPR);

EOLS	(debug_apple_eigrp_redist_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_REDIST);
INTERFACE (debug_apple_eigrp_redist_int_name, debug_apple_eigrp_redist_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_eigrp_redist_int, debug_apple_eigrp_redist_int_name,
	    debug_apple_eigrp_redist_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_eigrp_redist, debug_apple_eigrp_redist_int,
	 debug_apple_eigrp_external,
	 "eigrp-redistribution", "AT/EIGRP route redistribution", PRIV_OPR);

EOLS	(debug_apple_eigrp_request_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_REQUEST);
INTERFACE (debug_apple_eigrp_request_int_name, debug_apple_eigrp_request_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_eigrp_request_int, debug_apple_eigrp_request_int_name,
	    debug_apple_eigrp_request_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_eigrp_request, debug_apple_eigrp_request_int,
	 debug_apple_eigrp_redist,
	 "eigrp-request", "AT/EIGRP external functions", PRIV_OPR);

EOLS	(debug_apple_eigrp_hello_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_HELLO);
INTERFACE (debug_apple_eigrp_hello_int_name, debug_apple_eigrp_hello_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_eigrp_hello_int, debug_apple_eigrp_hello_int_name,
	    debug_apple_eigrp_hello_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_eigrp_hello, debug_apple_eigrp_hello_int,
	 debug_apple_eigrp_request,
	 "eigrp-hello", "AT/EIGRP hello functions", PRIV_OPR);

EOLS	(debug_apple_eigrp_query_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_QUERY);
INTERFACE (debug_apple_eigrp_query_int_name, debug_apple_eigrp_query_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_eigrp_query_int, debug_apple_eigrp_query_int_name,
	    debug_apple_eigrp_query_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_eigrp_query, debug_apple_eigrp_query_int,
	 debug_apple_eigrp_hello,
	 "eigrp-query", "AT/EIGRP query functions", PRIV_OPR);

EOLS	(debug_apple_eigrp_update_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_UPDATE);
INTERFACE (debug_apple_eigrp_update_int_name, debug_apple_eigrp_update_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_eigrp_update_int, debug_apple_eigrp_update_int_name,
	    debug_apple_eigrp_update_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_eigrp_update, debug_apple_eigrp_update_int,
	 debug_apple_eigrp_query,
	 "eigrp-update", "AT/EIGRP update functions", PRIV_OPR);

EOLS	(debug_apple_eigrp_packet_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_PACKET);
INTERFACE (debug_apple_eigrp_packet_int_name, debug_apple_eigrp_packet_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_eigrp_packet_int, debug_apple_eigrp_packet_int_name,
	    debug_apple_eigrp_packet_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_eigrp_packet, debug_apple_eigrp_packet_int,
	 debug_apple_eigrp_update,
	 "eigrp-packet", "AT/EIGRP packet debugging", PRIV_OPR);

EOLS	(debug_apple_eigrp_all_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_ALL);
INTERFACE (debug_apple_eigrp_all_int_name, debug_apple_eigrp_all_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_eigrp_all_int, debug_apple_eigrp_all_int_name,
	    debug_apple_eigrp_all_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_eigrp_all, debug_apple_eigrp_all_eol,
	 debug_apple_eigrp_packet,
	 "eigrp-all", "All AT/EIGRP functions", PRIV_OPR);

EOLS    (debug_apple_eigrp_target_eol, atigrp2_debug_command,
	 DEBUG_ATALKIGRP_TARGET);
CRANGE  (debug_apple_eigrp_dest, debug_apple_eigrp_target_eol,
	 debug_apple_eigrp_target_eol, OBJ(int,1), OBJ(int,2));
KEYWORD (debug_apple_eigrp, debug_apple_eigrp_dest, debug_apple_eigrp_all,
	 "eigrp-target", "Appletalk/EIGRP for targeting address", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE	debug_apple_eigrp
LINK_POINT(atigrp2_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for AT/EIGRP interface commands
 */
#define ALTERNATE NONE
/******************************************************************
 * appletalk eigrp-bandwidth-percent
 * no appletalk eigrp-s-bandwidth-percent
 */
EOLS	(ci_at_igrp_bw_pct_eol, atigrp2_if_command, ATALK_IGRP_BW_PCT);
NUMBER  (ci_at_igrp_bw_pct_num, ci_at_igrp_bw_pct_eol, no_alt, OBJ(int,1),
	 1, 999999, "Percentage of link bandwidth that EIGRP may use");
NOPREFIX(ci_at_igrp_bw_pct_no, ci_at_igrp_bw_pct_num, ci_at_igrp_bw_pct_eol);
NVGENS  (ci_at_igrp_bw_pct_nv, ci_at_igrp_bw_pct_no, atigrp2_if_command,
	 ATALK_IGRP_BW_PCT);
KEYWORD	(ci_at_igrp_bw_pct, ci_at_igrp_bw_pct_nv, ALTERNATE,
	"eigrp-bandwidth-percent", "Set EIGRP bandwidth limit",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk eigrp-splithorizon
 * no appletalk eigrp-splithorizon
 */
EOLS	(ci_at_igrpsplit_eol, atigrp2_if_command, ATALK_IGRP_SPLIT);
KEYWORD	(ci_at_igrpsplit, ci_at_igrpsplit_eol, ci_at_igrp_bw_pct,
	"eigrp-splithorizon",
	 "Enable Split Horizon processing generating AT/EIGRP updates",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * appletalk eigrp-timers <hello-interval> <holdtime>
 * no appletalk eigrp-timers
 *
 */
EOLS	(ci_at_igrp_timers_eol, atigrp2_if_command, ATALK_IGRP_TIMERS);
NUMBER  (ci_at_igrp_timers_hold, ci_at_igrp_timers_eol, no_alt,
	 OBJ(int,2),
	 1,0xffff, "Number of seconds that DUAL holds AT/EIGRP routes");
NUMBER  (ci_at_igrp_timers_hello, ci_at_igrp_timers_hold, no_alt,
	 OBJ(int,1),
	 1,0xffff, "Number of seconds between AT/EIGRP hello packets");
NOPREFIX (ci_at_igrp_timers_no, ci_at_igrp_timers_hello,ci_at_igrp_timers_eol);
NVGENS  (ci_at_igrp_timers_nvgen, ci_at_igrp_timers_no,
	 atigrp2_if_command, ATALK_IGRP_TIMERS);
KEYWORD (ci_at_igrp_timers, ci_at_igrp_timers_nvgen, ci_at_igrpsplit,
	 "eigrp-timers", "AT/EIGRP hello and holdtime timers",
	 PRIV_CONF|PRIV_SUBIF);

ASSERT  (ci_at_igrp_subif, ci_at_igrp_timers, ALTERNATE,
	 main_if_or_vc(csb->interface));


#undef ALTERNATE
#define ALTERNATE	ci_at_igrp_subif
LINK_POINT(atigrp2_interface_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for AT/EIGRP top-level configuration commands
 */
#define ALTERNATE NONE
/************************************************************************
 * [no] appletalk eigrp <sub-command>
 *
 * OBJ(int,1) = AppleTalk EIGRP sub-command
 *
 * 	[no] appletalk eigrp active-time {<max-active-time> | disable}
 *		OBJ(int,2) = <max-active-time>
 *      [no] appletalk eigrp event-log-size <n>
 *      [no] appletalk eigrp log-event-type [dual] [transport] [xmit]
 *	[no] appletalk eigrp log-neighbor-changes
 *
 */
EOLS	(conf_at_eigrp_eol, atigrp2_config_command, ATALK_EIGRP_COMMAND);

pdecl(conf_at_eigrp_log_options);
KEYWORD_ID (conf_at_eigrp_log_transport, conf_at_eigrp_log_options,
 	    conf_at_eigrp_eol, OBJ(int,4), TRUE, "transport",
 	    NULL /*"Log transport events"*/, PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (conf_at_eigrp_log_xmit, conf_at_eigrp_log_options,
 	    conf_at_eigrp_log_transport, OBJ(int,3), TRUE, "xmit",
 	    NULL /*"Log transmission events"*/, PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (conf_at_eigrp_log_dual, conf_at_eigrp_log_options,
 	    conf_at_eigrp_log_xmit, OBJ(int,2), TRUE, "dual",
 	    NULL /*"Log DUAL events"*/, PRIV_CONF|PRIV_HIDDEN);
NOP	(conf_at_eigrp_log_options, conf_at_eigrp_log_dual, NONE);
NOPREFIX(conf_at_eigrp_log_no, conf_at_eigrp_log_options,
	 conf_at_eigrp_eol);
NVGENS	(conf_at_eigrp_log_nv, conf_at_eigrp_log_no, atigrp2_config_command,
	 ATALK_EIGRP_COMMAND);
KEYWORD_ID(conf_at_eigrp_logtype, conf_at_eigrp_log_nv, no_alt,
	   OBJ(int,1), ATALK_EIGRP_LOG_TYPE, "log-event-type",
	   NULL /*"Set event types logged"*/, PRIV_CONF|PRIV_HIDDEN);

NUMBER  (conf_at_eigrp_evlogsize_size, conf_at_eigrp_eol, no_alt,
	 OBJ(int,2), 0, MAXUINT, NULL /*"Event log size"*/);
NOPREFIX(conf_at_eigrp_evlogsize_no, conf_at_eigrp_evlogsize_size,
 	 conf_at_eigrp_eol);
NVGENS  (conf_at_eigrp_evlogsize_nv, conf_at_eigrp_evlogsize_no,
 	 atigrp2_config_command, ATALK_EIGRP_COMMAND);
KEYWORD_ID (conf_at_eigrp_evlogsize, conf_at_eigrp_evlogsize_nv,
	    conf_at_eigrp_logtype, OBJ(int,1), ATALK_EIGRP_LOG_SIZE,
	    "event-log-size", NULL /*"Set AT-EIGRP event log size"*/,
	    PRIV_CONF|PRIV_HIDDEN);
 
KEYWORD_ID (conf_at_eigrp_log, conf_at_eigrp_eol, conf_at_eigrp_evlogsize,
	    OBJ(int,1), ATALK_EIGRP_LOG,
	    "event-logging", NULL /*"Set AT/EIGRP event logging level"*/,
	    PRIV_CONF|PRIV_HIDDEN);

KEYWORD_ID (conf_at_eigrp_lognbr, conf_at_eigrp_eol, conf_at_eigrp_log,
	    OBJ(int,1), ATALK_EIGRP_LOG_NBR,
	    "log-neighbor-changes", "Enable/Disable AT/EIGRP neighbor logging",
	    PRIV_CONF);

NUMBER  (conf_at_eigrp_timers_time, conf_at_eigrp_eol, no_alt,
	 OBJ(int,2), 1, MAXUINT, "EIGRP active-state time limit in minutes");
KEYWORD (conf_at_eigrp_timers_disable, conf_at_eigrp_eol,
	 conf_at_eigrp_timers_time,
	 "disabled", "disable EIGRP time limit for active state", PRIV_CONF);
NOPREFIX (conf_at_eigrp_timers_noprefix, conf_at_eigrp_timers_disable,
	  conf_at_eigrp_eol);
NVGENS  (conf_at_eigrp_timers_nvgen, conf_at_eigrp_timers_noprefix,
	 atigrp2_config_command, ATALK_EIGRP_COMMAND);
KEYWORD_ID (conf_at_eigrp_timers, conf_at_eigrp_timers_nvgen,
	    conf_at_eigrp_lognbr, OBJ(int,1), ATALK_EIGRP_TIMERS,
	    "active-time", "EIGRP time limit for active state", PRIV_CONF);

KEYWORD (conf_at_eigrp, conf_at_eigrp_timers, ALTERNATE,
	 "eigrp", "AppleTalk EIGRP subcommands", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE	conf_at_eigrp
LINK_POINT(atigrp2_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain for AT/EIGRP "appletalk routing" command
 */
#define ALTERNATE NONE
EOLS (conf_at_routing_eigrp_eol, atigrp2_routing_command, ATALK_IGRP_ROUTING);

NUMBER (conf_at_routing_eigrp_id, conf_at_routing_eigrp_eol, no_alt,
	OBJ(int,2), 1, MAXINT, "AT/EIGRP Router ID");
NOPREFIX (conf_at_routing_eigrp_no, conf_at_routing_eigrp_id,
	  conf_at_routing_eigrp_eol);
NVGENS (conf_at_routing_eigrp_nvgen, conf_at_routing_eigrp_no,
	atigrp2_routing_command, ATALK_IGRP_ROUTING);
KEYWORD	(conf_at_routing_eigrp, conf_at_routing_eigrp_nvgen, ALTERNATE,
	"eigrp", "Enable AT/EIGRP routing", PRIV_CONF);


#undef ALTERNATE
#define ALTERNATE conf_at_routing_eigrp
LINK_POINT(atigrp2_routing_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain for AT/EIGRP "appletalk routing" command
 */
#define ALTERNATE NONE
/*
 * (appletalk protocol) "eigrp"
 *
 * This is a stub which is linked into the "appletalk protocol" command.
 */
EOLS (ci_at_rp_igrp_eol, atigrp2_protocol_command, ATALK_ROUTE_PROTOCOL);
KEYWORD_ID (ci_at_rp_igrp, ci_at_rp_igrp_eol, no_alt,
	    OBJ(int,1), ATALK_RP_IGRP,
	    "eigrp", "Enable AT/EIGRP processing on this interface",
	    PRIV_CONF|PRIV_SUBIF);
ASSERT(ci_aurp_tunnel_assert_igrp, ci_at_rp_igrp, no_alt,
       !atalkif_ForAURP(csb->interface));

#undef ALTERNATE
#define ALTERNATE	ci_aurp_tunnel_assert_igrp
LINK_POINT(atigrp2_routing_protocol, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain for SHOW commands.
 */
#define ALTERNATE NONE
/******************************************************************
 * show appletalk eigrp topology [<network>
 *                                | all-links | active | zero-successors]
 *
 * OBJ(int,1) network number specified.
 * OBJ(int,2) network number
 * OBJ(int,3) show type (all, active, zeros)
 */
EOLS (show_apple_igrp2_top_eols, atigrp2_show_cmd, SHOW_APPLE_EIGRP_TOP);
SET     (show_apple_igrp2_top_eol_set, show_apple_igrp2_top_eols,
	 OBJ(int,3), IGRP2_TOP_FS);
KEYWORD_ID(show_apple_igrp2_top_all, show_apple_igrp2_top_eols,
	   show_apple_igrp2_top_eol_set,
	   OBJ(int,3), IGRP2_TOP_ALL,
	   "all-links", "Show all links in topology table", PRIV_USER);
KEYWORD_ID(show_apple_igrp2_top_zero, show_apple_igrp2_top_eols,
	   show_apple_igrp2_top_all,
	   OBJ(int,3), IGRP2_TOP_ZERO,
	   "zero-successors", "Show only zero successor entries", PRIV_USER);
KEYWORD_ID(show_apple_igrp2_top_pending, show_apple_igrp2_top_eols,
	   show_apple_igrp2_top_zero,
	   OBJ(int,3), IGRP2_TOP_PENDING,
	   "pending", "Show only entries pending transmission", PRIV_USER);
KEYWORD_ID(show_apple_igrp2_top_summary, show_apple_igrp2_top_eols,
	   show_apple_igrp2_top_pending,
	   OBJ(int,3), IGRP2_TOP_SUMMARY,
	   "summary", "Show a summary of the topology table", PRIV_USER);
KEYWORD_ID(show_apple_igrp2_top_active, show_apple_igrp2_top_eols,
	   show_apple_igrp2_top_summary,
	   OBJ(int,3), IGRP2_TOP_ACTIVE,
	   "active", "Show only active entries", PRIV_USER);
SET	(show_apple_igrp2_top_set, show_apple_igrp2_top_eol_set,
	 OBJ(int,1), 1);
NUMBER	(show_apple_igrp2_top_net, show_apple_igrp2_top_set,
	 show_apple_igrp2_top_active,
	 OBJ(int,2), 1, 65279, "Network number");

KEYWORD (show_apple_igrp2_top, show_apple_igrp2_top_net, no_alt,
	 "topology", "AppleTalk/EIGRP Topology Table", PRIV_USER);

/******************************************************************
 * show appletalk eigrp neighbors|interfaces [detail] [<interface><unit>] 
 *
 * OBJ(idb,1) = <interface><unit>
 * OBJ(int,2) = TRUE == detail, FALSE == nominal
 * OBJ(int,3) = TRUE == neighbors, FALSE == interface
 */
EOLS (show_apple_igrp2_nbrs_eols, atigrp2_show_cmd, SHOW_APPLE_EIGRP_NBR);
INTERFACE (show_apple_igrp2_nbrs_int, show_apple_igrp2_nbrs_eols, 
  	   show_apple_igrp2_nbrs_eols, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD_ID (show_apple_igrp2_nbrs_detail, show_apple_igrp2_nbrs_int,
	    show_apple_igrp2_nbrs_int, OBJ(int,2), TRUE,
	    "detail", "Show detailed peer information", PRIV_USER);
KEYWORD	(show_apple_igrp2_int, show_apple_igrp2_nbrs_detail,
	 show_apple_igrp2_top,
	 "interfaces", "AppleTalk/EIGRP Interfaces", PRIV_USER);
KEYWORD_ID (show_apple_igrp2_nbrs, show_apple_igrp2_nbrs_detail,
 	    show_apple_igrp2_int, OBJ(int,3), TRUE,
 	    "neighbors", "AppleTalk/EIGRP Neighbors", PRIV_USER);
  
/******************************************************************
 * show appletalk eigrp traffic
 *
 */
EOLS (show_apple_igrp2_traffic_eols, atigrp2_show_cmd,
      SHOW_APPLE_EIGRP_TRAFFIC);
KEYWORD (show_apple_igrp2_traffic, show_apple_igrp2_traffic_eols,
	 show_apple_igrp2_nbrs,
         "traffic", "AppleTalk/EIGRP Traffic Statistics", PRIV_USER);

/******************************************************************
 * show appletalk eigrp event
 *
 */
EOLS (show_apple_igrp2_event_eols, atigrp2_show_cmd, SHOW_APPLE_EIGRP_EVENT);
KEYWORD (show_apple_igrp2_event, show_apple_igrp2_event_eols,
	 show_apple_igrp2_traffic,
         "events", NULL /*"AppleTalk/EIGRP Events"*/, PRIV_USER | PRIV_HIDDEN);

/******************************************************************
 * show appletalk eigrp sia-event [<asystem>]
 *
 * OBJ(int,1) = <asystem>
 */
EOLS (show_apple_igrp2_siaevent_eols, atigrp2_show_cmd,
      SHOW_APPLE_EIGRP_SIAEVENT);
KEYWORD (show_apple_igrp2_siaevent, show_apple_igrp2_siaevent_eols,
         show_apple_igrp2_event,
         "sia-event", NULL /*"AppleTalk/EIGRP SIA event"*/,
	 PRIV_USER | PRIV_HIDDEN);

/******************************************************************
 *
 * show appletalk eigrp
 *
 * This is the top of the "show apple eigrp" command chain.
 */
KEYWORD (show_apple_igrp2, show_apple_igrp2_siaevent, ALTERNATE,
	 "eigrp", "AppleTalk/EIGRP show commands", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE	show_apple_igrp2
LINK_POINT(atigrp2_show_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain for CLEAR commands.
 */
#define ALTERNATE NONE
/******************************************************************
 * clear apple eigrp events
 */
EOLNS	(clear_apple_igrp2_log_eol, atigrp2_clear_log);
KEYWORD (clear_apple_igrp2_log, clear_apple_igrp2_log_eol, no_alt,
 	 "events", NULL /*"Clear AT-EIGRP event logs"*/, PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * clear apple eigrp logging 
 */
EOLNS	(clear_apple_igrp2_evlog_eol, atigrp2_clear_event_logging);
KEYWORD (clear_apple_igrp2_evlog, clear_apple_igrp2_evlog_eol,
	 clear_apple_igrp2_log,
	 "logging", NULL /*"Stop AT-EIGRP event logging"*/,
	 PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * clear apple eigrp neighbors [<appletalk-address> | <interface-unit>]
 *
 * OBJ(paddr,1) = Neighbor appletalk address
 * OBJ(idb,1)   = Interface of neighbors to get cleared
 */
EOLNS	(clear_apple_igrp2_nbr_eol, atigrp2_clear_neighbors);
INTERFACE (clear_apple_igrp2_nbr_int, clear_apple_igrp2_nbr_eol,
           clear_apple_igrp2_nbr_eol, OBJ(idb,1), IFTYPE_ANYSUB);
ATADDR  (clear_apple_igrp2_nbr_addr, clear_apple_igrp2_nbr_eol,
         clear_apple_igrp2_nbr_int,
	 OBJ(paddr,1), "APPLE-EIGRP neighbor address");
KEYWORD (clear_apple_igrp2_nbr, clear_apple_igrp2_nbr_addr,
	 clear_apple_igrp2_evlog,
         "neighbors", "Clear AppleTalk/EIGRP neighbors", PRIV_OPR);

KEYWORD (clear_apple_igrp2, clear_apple_igrp2_nbr, ALTERNATE,
         "eigrp", "Clear AppleTalk/EIGRP", PRIV_OPR);
#undef ALTERNATE
#define ALTERNATE     clear_apple_igrp2
LINK_POINT(atigrp2_clear_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for AT/EIGRP
 */
static parser_extension_request atigrp2_chain_init_table[] = {
    { PARSE_ADD_CLEAR_ATALK_CMD, &pname(atigrp2_clear_commands) },
    { PARSE_ADD_SHOW_ATALK_CMD, &pname(atigrp2_show_commands) },
    { PARSE_ADD_DEBUG_ATALK_CMD, &pname(atigrp2_debug_commands) },
    { PARSE_ADD_CFG_TOP_ATALK_ROUTING_CMD,
	  &pname(atigrp2_routing_config_commands) },
    { PARSE_ADD_CFG_INT_ATALK_CMD, &pname(atigrp2_interface_commands) },
    { PARSE_ADD_ATALK_PROTOCOL_CMD, &pname(atigrp2_routing_protocol) },
    { PARSE_ADD_CFG_ATALK_ROUTING_CMD, &pname(atigrp2_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * atigrp2_parser_init - Initialize AT/EIGRP parser support
 */
void
atigrp2_parser_init (void)
{
    parser_add_command_list(atigrp2_chain_init_table, "atigrp2");
}
