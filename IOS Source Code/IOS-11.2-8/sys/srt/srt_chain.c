/* $Id: srt_chain.c,v 3.6.10.4 1996/06/11 20:28:30 hampton Exp $
 * $Source: /release/112/cvs/Xsys/srt/srt_chain.c,v $
 *------------------------------------------------------------------
 * SRT parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: srt_chain.c,v $
 * Revision 3.6.10.4  1996/06/11  20:28:30  hampton
 * New access-list commands should set the NONVGEN flag. [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/30  23:46:07  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.6.10.2  1996/05/21  10:05:30  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/05/04  01:49:19  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface configuration & bridging
 *   (process-level) support.
 * - BVI smf functions and registry.
 * - BVI settable mtu support & dynamically select mtu for the BVI.
 * - 'bridge irb' & 'bridge <x> route/bridge <protcocol>' support.
 *
 * Constrained Multicast Flooding
 * - Add the CMF parser link points.
 * - Add the CMF data structures to spantype.
 * - Add the creation and deletion of the tbridge monitor.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Manage the CMF data structures when the spanning tree changes.
 *
 * Revision 3.6  1996/02/22  14:38:49  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:10:40  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:30:13  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:38:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:57:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/04  22:47:06  pitargue
 * CSCdi40658:  Forward mac addr. filter failed on dec & ieee bridge
 * (eip-aip-eip)
 * atm needs a map between a mac addr and a vc number.
 *
 * Revision 2.3  1995/07/04  01:58:17  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/28  09:31:47  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:02:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_private.h"
#include "subsys.h"
#include "../ui/debug.h"
#include "parser.h"
#include "packet.h"
#include "access.h"
#include "ieee.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../srt/span.h"
#include "../srt/srt_debug.h"
#include "../tbridge/tbridge_debug.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/atm.h"
#include "../parser/parser_defs_access.h"
#include "parser_defs_bridge.h"
#include "../parser/parser_defs_lat.h"
#include "../parser/parser_defs_parser.h"
#include "../tbridge/tbridge_sde.h"
#include "../parser/parser_defs_priority.h"


/*
 * Parse chains for Bridging clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_bridge.h"
LINK_POINT(srt_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SR and STA debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_span.h"
#include "exec_debug_access.h"
LINK_POINT(srt_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Transparent Bridging debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_tbridge.h"
LINK_POINT(tbridge_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Bridging show commands
 */
#define ALTERNATE       NONE
#include "exec_show_span.h"
#include "exec_show_media.h"
#include "exec_show_bridge.h"
#include "exec_show_access-expr.h"
LINK_POINT(srt_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Bridging routing commands
 */
#define	ALTERNATE	NONE
#include "cfg_bridge.h"
LINK_POINT(srt_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Bridging interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_bridge-group.h"
#include "cfg_int_access-expr.h"
LINK_POINT(srt_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Bridging access list commands
 */
#define	ALTERNATE	NONE
#define	OALTERNATE	NONE
#include "cfg_acclist_ether.h"
#include "cfg_acclist_type.h"
#include "cfg_acclist_xmac.h"
LINK_POINT(osrt_access_list_commands, OALTERNATE);
#undef	OALTERNATE
KEYWORD (almed_access_list, ALTERNATE, no_alt,
	 "access-list", "Media Access lists",
	 PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN);
KEYWORD (media_keyword, almed_access_list, no_alt,
	 "media", "Media Commands", PRIV_CONF|PRIV_HIDDEN);
LINK_POINT(media_commands, media_keyword);
#undef	ALTERNATE


/*
 * Parse chains for Bridging priority queueing
 */
#define	ALTERNATE	NONE
#include "cfg_priority_bridge.h"
LINK_POINT(srt_priority_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for Bridging
 */
static parser_extension_request srt_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(srt_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(srt_debug_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(tbridge_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(srt_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(srt_config_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(srt_interface_commands) },
    { PARSE_ADD_CFG_ACCLIST_CMD, &pname(osrt_access_list_commands) },	 /* Old */
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(media_commands) },	
    { PARSE_ADD_PRIORITY_OPTIONS, &pname(srt_priority_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * srt_parser_init - Initialize Bridging parser support
 */
void srt_parser_init (subsystype *subsys)
{
    parser_add_link_point(PARSE_ADD_CFG_BRIDGE_CMF_CMD,
			  "bridge cgmp",
			  &pname(cfg_bridge_cmf_extend_here));
    parser_add_link_exit(PARSE_ADD_CFG_BRIDGE_CMF_EXIT,
			 "bridge cmf exit",
			 &pname(cfg_bridge_cmf_return_here));

    parser_add_link_point(PARSE_ADD_DEBUG_TBRIDGE_CMF_CMD,
			  "debug tbridge cmf",
			  &pname(debug_tbridge_cmf_extend_here));

    parser_add_link_point(PARSE_ADD_SHOW_BRIDGE_CMF_CMD,
			  "show bridge cmf",
			  &pname(show_bridge_cmf_extend_here));
    parser_add_link_exit(PARSE_ADD_SHOW_BRIDGE_CMF_EXIT,
			 "show bridge cmf exit",
			 &pname(show_bridge_cmf_return_here));

    parser_add_link_point(PARSE_ADD_CLEAR_BRIDGE_CMF_CMD,
			  "clear bridge cmf",
			  &pname(clear_bridge_cmf_extend_here));
    parser_add_link_exit(PARSE_ADD_CLEAR_BRIDGE_CMF_EXIT,
			 "clear bridge cmf exit",
			 &pname(clear_bridge_cmf_return_here));

    parser_add_link_point(PARSE_ADD_CFG_INT_BRIDGE, "interface bridge-group",
			  &pname(bgroup_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_BRIDGE_CMD, "show bridge",
			  &pname(show_bridge_extend_here));
    parser_add_protoaddr(LINK_BRIDGE, PMATCH_BRIDGE PMATCH_DGRAM, "bridge",
			 "Bridging", 0, ADDR_ILLEGAL);
    parser_add_command_list(srt_chain_init_table, "srt");
}

/*
 * Transparent Bridging user interface (TBRIDGE_UI) subsystem header
 */

#define TBRIDGE_UI_MAJVERSION 1
#define TBRIDGE_UI_MINVERSION 0
#define TBRIDGE_UI_EDITVERSION  1

SUBSYS_HEADER(tbridge_ui, TBRIDGE_UI_MAJVERSION, TBRIDGE_UI_MINVERSION,
	      TBRIDGE_UI_EDITVERSION,
	      srt_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: tbridge", NULL);
