/* $Id: novell_chain.c,v 3.9.4.7 1996/09/14 00:07:17 sseal Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for NOVELL
 *
 * January 5, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: novell_chain.c,v $
 * Revision 3.9.4.7  1996/09/14  00:07:17  sseal
 * CSCdi61196:  ipx route-cache options need clean up
 * Branch: California_branch
 *
 * Revision 3.9.4.6  1996/07/26  00:09:01  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.9.4.5  1996/06/17  23:40:22  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.4.4  1996/05/30  23:49:35  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.9.4.3  1996/05/21  10:11:39  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.4.2  1996/05/03  02:18:22  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.9.4.1  1996/04/02  05:38:18  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.9  1996/03/09  22:58:33  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.8  1996/03/09  18:13:12  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.7  1996/02/22  14:40:36  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:13:28  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:31:59  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:42:38  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:57:22  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  19:23:02  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/07  18:00:27  sluong
 * CSCdi38216:  distribute-list command not configurable for ipx router rip
 *
 * Revision 2.3  1995/08/04  03:08:28  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.2  1995/06/15  15:19:51  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  23:28:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "../iprouting/route.h"
#include "../xns/novell.h"
#include "../xns/novroute.h"
#include "../ui/debug.h"
#include "xns.h"
#include "xnsfast.h"
#include "novell.h"
#include "novell_debug.h"

#include "../parser/parser_actions.h"
#include "../xns/novell_actions.h"
#include "../parser/macros.h"
#include "parser_defs_xns_nov.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_sse.h"
#include "../iprouting/parser_defs_igrp2.h"
#include "../parser/parser_defs_priority.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/parser_defs_isis.h"

/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_novell.h"
LINK_POINT(novell_clear_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_novell.h"
LINK_POINT(novell_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_novell.h"
LINK_POINT(novell_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for TEST commands
 */
#define ALTERNATE NONE
#include "exec_test_novell.h"
LINK_POINT(novell_test_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_novell.h"
LINK_POINT(novell_int_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for NOVELL router commands
 */
#define ALTERNATE NONE
#include "cfg_novell_routing.h"
LINK_POINT(novell_routing_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for NOVELL top-level configuation commands
 */
#define ALTERNATE NONE
#include "cfg_novell.h"
LINK_POINT(novell_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for NOVELL access list commands
 */
#define	ALTERNATE	NONE
#define	OALTERNATE	NONE
#include "cfg_acclist_novell.h"
LINK_POINT(onovell_acclist_commands, OALTERNATE);
LINK_POINT(novell_acclist_commands, ALTERNATE);
#undef	OALTERNATE
#undef	ALTERNATE


/*
 * Parse chains for NOVELL ROUTER router commands
 */
LINK_TRANS(router_novell_extend_here, no_alt);
#define	ALTERNATE	router_novell_extend_here
#include	"cfg_nov_router_network.h" 
#include	"cfg_nov_router_distribute.h"
#include	"cfg_nov_router_redist.h"
LINK_POINT(novell_router_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NOVELL priority queueing options
 */
#define	ALTERNATE	NONE
#include "cfg_priority_novell.h"
LINK_POINT(novell_priority_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NOVELL ping command
 */
LINK_EXIT(exec_ping_novell_exit, no_alt);
NOVELL_IPX_KEYWORD_ID(exec_ping_novell, exec_ping_novell_exit, NONE,
		      OBJ(int,1), LINK_NOVELL, "Novell/IPX echo", PRIV_USER);
LINK_POINT(novell_ping_commands, exec_ping_novell);



/*********************************************************
 *	start of configure IPX/Novell router commands
 */
LINK_TRANS(after_novrouter_exit, no_alt);
NVCOMMENT(after_novrouter_comment, after_novrouter_exit);
#undef ALTERNATE
#define ALTERNATE after_novrouter_comment
#include "cfg_nov_router_exit.h"
LINK_TRANS(cfg_novell_router_extend_here, ALTERNATE);

NO_OR_DEFAULT(confignovrouter_no, cfg_novell_router_extend_here,
	   PRIV_MIN | PRIV_NONVGEN);
HELP	(top_config_nov_router, confignovrouter_no,
	 "IPX router configuration commands:\n");

/*
 * Parse chain registration array for NOVELL
 */
static parser_extension_request novell_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(novell_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(novell_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(novell_show_commands) },
    { PARSE_ADD_TEST_CMD, &pname(novell_test_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(novell_int_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(novell_routing_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(novell_config_commands) },
    { PARSE_ADD_CFG_NOVELL_ROUTING_CMD, &pname(novell_acclist_commands) },
    { PARSE_ADD_CFG_ACCLIST_CMD, &pname(onovell_acclist_commands) }, /* Old */
    { PARSE_ADD_CFG_NOVELL_ROUTER_CMD, &pname(novell_router_commands) },
    { PARSE_ADD_PING_CMD, &pname(novell_ping_commands) },
    { PARSE_ADD_PRIORITY_OPTIONS, &pname(novell_priority_commands) },
    { PARSE_ADD_PING_EXIT, 
	  (dynamic_transition *) &pname(exec_ping_novell_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * novell_parser_init - Initialize parse chains for NOVELL
 * 
 * This routine is called at system startup to register the parse chains
 * for NOVELL.
 */

void novell_parser_init (void)
{
    parser_add_mode("ipx-router", "config-ipx-router",
		    "IPX router configuration mode",
		    TRUE, TRUE, "configure", NULL, NULL,
		    &pname(top_config_nov_router),
		    NULL);
    parser_add_protoaddr(LINK_NOVELL,		PMATCH_NOVELL PMATCH_DGRAM,
			 "ipx",			"Novell IPX",
			 3, ADDR_NOVELL);
    parser_add_protoaddr(LINK_NOVELL,		PMATCH_NOVELL PMATCH_DGRAM,
			 "novell",		NULL,
			 0, ADDR_NOVELL);
    parser_add_address(ADDR_NOVELL, novell_addr_short_help, match_nov_addr);
    parser_add_command_list(novell_chain_init_table, "ipx");
    parser_add_router(RMATCH_NOVELL, parser_novell_router);
    parser_add_link_point(PARSE_ADD_CLEAR_IPX_CMD, "clear ipx",
			  &pname(clear_novell_extend_here)); 
    parser_add_link_point(PARSE_ADD_SHOW_IPX_CMD, "show ipx",
			  &pname(show_novell_extend_here));
    parser_add_link_point(PARSE_ADD_DEBUG_IPX_CMD, "debug ipx",
			  &pname(debug_novell_extend_here)); 
    parser_add_link_point(PARSE_ADD_CFG_INT_IPX_CMD, "interface ipx",
			  &pname(interface_novell_extend_here)); 
    parser_add_link_point(PARSE_ADD_CFG_TOP_IPX_CMD, "global ipx",
			  &pname(global_novell_extend_here)); 
    parser_add_link_point(PARSE_ADD_ROUTER_IPX_CMD, "router ipx",
			  &pname(router_novell_extend_here)); 
    parser_add_link_point(PARSE_ADD_CFG_NOVELL_ROUTER_CMD, "ipx router",
			  &pname(cfg_novell_router_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_NOVELL_ROUTING_CMD, "ipx routing",
			  &pname(cfg_novell_routing_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_IPXROUTEC_CMD,
			  "int ipx route-cache",
			  &pname(ci_nov_rcache_extend_here));
    parser_add_link_exit(PARSE_ADD_CFG_INT_IPXROUTEC_EXIT,
			 "int ipx route-cache return",
			 &pname(ci_nov_rcache_return_here));

}


