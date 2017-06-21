/* $Id: priority_chain.c,v 3.5.10.3 1996/05/21 09:50:08 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/priority_chain.c,v $
 *------------------------------------------------------------------
 * Priority parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: priority_chain.c,v $
 * Revision 3.5.10.3  1996/05/21  09:50:08  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.2  1996/05/17  11:23:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.2  1996/05/05  23:10:06  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.2.1  1996/04/26  07:55:28  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.10.1  1996/04/17  13:40:29  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/22  14:34:08  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1995/12/17  18:28:50  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:30:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:53:29  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:54:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "access.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../if/priority_private.h"
#include "../if/network_debug.h"
#include "../vines/vines.h"
#include "../parser/parser_defs_priority.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for Priority debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_priority.h"
#include "exec_debug_custom-queue.h"
LINK_POINT(priority_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Priority show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_queueing.h"
LINK_POINT(priority_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Priority config commands
 */
#define ALTERNATE       NONE
#include "cfg_priority-list.h"
#include "cfg_queue-list.h"
LINK_POINT(priority_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Priority interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_priority-group.h"
#include "cfg_int_custom-queue.h"
#include "cfg_int_fair-queue.h"
#include "cfg_int_random-detect.h"
LINK_POINT(priority_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Priority
 */
static parser_extension_request priority_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(priority_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(priority_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(priority_config_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(priority_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * priority_parser_init - Initialize Priority parser support
 */
void priority_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_PRIORITY_OPTIONS,
			  "priority list options",
			  &pname(cfg_prilist_extend_here));
    parser_add_command_list(priority_chain_init_table, "priority");
}
