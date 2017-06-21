/* $Id: tbridge_cmf_chain.c,v 3.1.8.2 1996/05/21 10:05:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_cmf_chain.c,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_cmf_chain.c
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Parse chains for Constrained Multicasting Flooding (CMF)
 *------------------------------------------------------------------
 * $Log: tbridge_cmf_chain.c,v $
 * Revision 3.1.8.2  1996/05/21  10:05:49  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.8.1  1996/05/04  05:08:52  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - New file(tbridge_bvi.c) for Bridge-group Virtual Interface
 * functions.
 * - Obsolete tbridge_crb_inline.h with tbridge_irb_inline.h.
 * - Bridge/route decision(tbridge_irb_inline.h).
 * - Learn routable packets(DLLI_IRBPKT) in bridge table.
 * - Move locate_bte_inline() and tbridge_get_arpa_type_inline() to
 *   tbridge_common_inline.h.
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the tbridge monitor.
 * - Add the tbridge registry.
 * - Add the entire CMF subsystem.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Add the call outs to CMF from the LES, RSP, and PAS forwarding
 *   functions.
 *
 * Revision 3.1  1996/04/21  23:06:47  speakman
 * Placeholders for the CMF subsystem and eventual partition of the
 * bridging registries.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"

#include "../tbridge/parser_defs_tbridge_cmf.h"
#include "../tbridge/tbridge_debug.h"

/*
 * Parse chains for CONFIG commands
 */

#define ALTERNATE NONE
#include "cfg_bridge_cmf.h"
LINK_POINT(cfg_bridge_cmf_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for DEBUG commands
 */

#define ALTERNATE NONE
#include "exec_debug_cmf.h"
LINK_POINT(debug_tbridge_cmf_command, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for SHOW commands
 */

#define ALTERNATE NONE
#include "exec_show_bridge_cmf.h"
LINK_POINT(show_bridge_cmf_command, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for CLEAR commands
 */

#define ALTERNATE NONE
#include "exec_clear_bridge_cmf.h"
LINK_POINT(clear_bridge_cmf_command, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for CMF
 */
static parser_extension_request tbridge_cmf_chain_init_table[] = {
    { PARSE_ADD_CFG_BRIDGE_CMF_CMD, &pname(cfg_bridge_cmf_command) },
    { PARSE_ADD_CFG_BRIDGE_CMF_EXIT, (dynamic_transition *)
	  &pname(cfg_bridge_cmf_command_exit) },
    { PARSE_ADD_DEBUG_TBRIDGE_CMF_CMD, &pname(debug_tbridge_cmf_command) },
    { PARSE_ADD_SHOW_BRIDGE_CMF_CMD, &pname(show_bridge_cmf_command) },
    { PARSE_ADD_SHOW_BRIDGE_CMF_EXIT, (dynamic_transition *)
	  &pname(show_bridge_cmf_command_exit) },
    { PARSE_ADD_CLEAR_BRIDGE_CMF_CMD, &pname(clear_bridge_cmf_command) },
    { PARSE_ADD_CLEAR_BRIDGE_CMF_EXIT, (dynamic_transition *)
	  &pname(clear_bridge_cmf_command_exit) },
    { PARSE_LIST_END, NULL }
};

void tbridge_cmf_parser_init (void)
{
    parser_add_command_list(tbridge_cmf_chain_init_table, "tbridge_cmf");
}
