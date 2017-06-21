/* $Id: rmon_chain.c,v 3.8.10.1 1996/05/21 10:01:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/rmon/rmon_chain.c,v $
 *------------------------------------------------------------------
 * rmon_chain.c:  Remote Monitoring Parse Chains
 *
 * August 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_chain.c,v $
 * Revision 3.8.10.1  1996/05/21  10:01:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8  1996/02/22  14:37:32  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/18  17:57:50  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.6  1996/02/09  07:57:19  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.5  1996/02/01  06:08:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:39  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:36:34  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:39:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_private.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_rmon.h"
#include "rmon_task.h"

/*
 * Parse chains for all rmon show commands
 */
#define ALTERNATE       NONE
#include "exec_show_rmon.h"
LINK_POINT(rmon_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for all rmon global configuration commands
 */
#define ALTERNATE       NONE
#include "cfg_rmon.h"
LINK_POINT(rmon_global_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for all rmon interface configuration commands
 */
#define ALTERNATE       NONE
#include "cfg_int_rmon.h"
LINK_POINT(rmon_int_commands, ALTERNATE);
#undef  ALTERNATE



/*
 * Parse chain registration array for RMON commands
 */
static parser_extension_request rmon_chain_init_table[] = {
    { PARSE_ADD_SHOW_RMON_CMD, &pname(rmon_show_commands) },
    { PARSE_ADD_SHOW_RMON_RET, (dynamic_transition *) &pname(show_rmon_exit) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(rmon_int_commands) },
    { PARSE_ADD_CFG_RMON_CMD, &pname(rmon_global_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * rmon_parser_init - Initialize parse structures for parsing RMON commands
 */
void rmon_parser_init (void)
{
    parser_add_command_list(rmon_chain_init_table, "rmon");
}
