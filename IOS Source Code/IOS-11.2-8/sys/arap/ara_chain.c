/* $Id: ara_chain.c,v 3.6.10.1 1996/05/21 09:41:55 thille Exp $
 * $Source: /release/112/cvs/Xsys/arap/ara_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for ARA
 *
 * September 1993, Tim Kolar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ara_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  09:41:55  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:31:53  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  05:58:40  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:25:52  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:23:57  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:37:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  09:34:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  20:04:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "access.h"
#include "ttysrv.h"
#include "ara_debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_arap.h"
#include "../parser/parser_defs_config.h"
#include "aaa.h"

/*
 * Parse chains for EXEC commands
 */
#define ALTERNATE NONE
#include "cfg_arap.h"
LINK_POINT(ara_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for EXEC commands
 */
#define ALTERNATE NONE
#include "exec_arap.h"
LINK_POINT(ara_exec_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_arap.h"
LINK_POINT(ara_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_arap.h"
LINK_POINT(ara_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for LINE commands
 */
#define ALTERNATE NONE
#include "cfg_line_arap.h"
LINK_POINT(ara_line_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registrationa array for ARA
 */
static parser_extension_request ara_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(ara_config_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(ara_exec_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(ara_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(ara_show_commands) },
    { PARSE_ADD_CFG_LINE_CMD, &pname(ara_line_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ara_parser_init - Initialize parse chains for ARA
 *
 * This routine is called at system startup to register the parse chains
 * for ARA.
 */

void ara_parser_init (void)
{
#if FALSE
    parser_add_protoaddr(LINK_MNP4,		PMATCH_ARA PMATCH_DGRAM,
			 "ara",			"AppleTalk ARA",
			 ADDR_ILLEGAL);
#endif
    parser_add_command_list(ara_chain_init_table, "ara");
}

