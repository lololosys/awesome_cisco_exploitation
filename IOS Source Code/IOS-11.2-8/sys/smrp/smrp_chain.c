/* $Id: smrp_chain.c,v 3.5.10.1 1996/05/21 10:01:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_chain.c,v $
 *------------------------------------------------------------------
 * Parser chain for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  10:01:57  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:37:51  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:29:44  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:36:59  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:53:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:56:57  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/14  22:54:54  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "smrp_private.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_smrp.h"
#include "../parser/parser_defs_parser.h"

/*
 * Parse chains for CLEAR commands
 */
#define ALTERNATE NONE
#include "exec_clear_smrp.h"
LINK_POINT(smrp_clear_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for DEBUG commands
 */

#define ALTERNATE NONE
#include "exec_debug_smrp.h"
LINK_POINT(smrp_debug_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for SHOW commands
 */

#define ALTERNATE NONE
#include "exec_show_smrp.h"
LINK_POINT(smrp_show_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_smrp.h"
LINK_POINT(smrp_int_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for SMRP top-level configuation commands
 */
#define ALTERNATE NONE
#include "cfg_smrp.h"
LINK_POINT(smrp_config_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for SMRP access list commands
 */
#if NOT_USED
#define	ALTERNATE	NONE
#include "cfg_acclist_smrp.h"
LINK_POINT(smrp_acclist_commands, ALTERNATE);
#undef	ALTERNATE
#endif

/*
 * Parse chain registration array for SMRP
 */
static parser_extension_request smrp_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(smrp_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(smrp_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(smrp_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(smrp_int_commands) },
#if NOT_USED
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(smrp_routing_commands) },
#endif
    { PARSE_ADD_CFG_TOP_CMD, &pname(smrp_config_commands) },
#if NOT_USED
    { PARSE_ADD_CFG_ACCLIST_CMD, &pname(smrp_acclist_commands) },
    { PARSE_ADD_PING_CMD, &pname(smrp_ping_commands) },
#endif
    { PARSE_LIST_END, NULL }
};

/*
 * smrp_parser_init - Initialize parse chains for SMRP
 * 
 * This routine is called at system startup to register the parse chains
 * for SMRP.
 */

void smrp_parser_init (void)
{
#if NOT_USED
    parser_add_link_point(PARSE_ADD_CLEAR_SMRP_CMD, "clear smrp",
			  (dynamic_transition *)
			  &pname(clear_smrp_extend_here));
    parser_add_link_point(PARSE_ADD_DEBUG_SMRP_CMD, "debug smrp",
			  (dynamic_transition *)
			  &pname(debug_smrp_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_TOP_SMRP_CMD, "config smrp",
			  (dynamic_transition *)
			  &pname(cfg_smrp_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_SMRP_CMD, "show smrp",
			  (dynamic_transition *)
			  &pname(show_smrp_extend_here));
#endif
    parser_add_command_list(smrp_chain_init_table, "smrp");
}

