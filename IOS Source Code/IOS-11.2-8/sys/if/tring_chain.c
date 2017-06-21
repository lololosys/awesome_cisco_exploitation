/* $Id: tring_chain.c,v 3.7.10.1 1996/05/21 09:50:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/tring_chain.c,v $
 *------------------------------------------------------------------
 * Token Ring parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tring_chain.c,v $
 * Revision 3.7.10.1  1996/05/21  09:50:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7  1996/02/22  14:34:13  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:04:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:28:22  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:28:55  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:31:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:53:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:53:31  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:55:31  hampton
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
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../if/tring_debug.h"
#include "../if/parser_defs_rif.h"
#include "../if/parser_defs_tring.h"
#include "../parser/parser_defs_parser.h"
#include "../if/tring.h"

extern int nTRs;


/*
 * Parse chains for Token Ring clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_rif.h"
LINK_POINT(tring_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Token Ring debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_rif.h"
#include "exec_debug_token-ring.h"
LINK_POINT(tring_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Token Ring show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_rif.h"
LINK_POINT(tring_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Token Ring config commands
 */
#define ALTERNATE       NONE
#include "cfg_rif.h"
LINK_POINT(tring_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Token Ring interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_multiring.h"
#include "cfg_int_tr_speed.h"
#include "cfg_int_ethertransit.h"
#include "cfg_int_early.h"
LINK_POINT(tring_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Token Ring show controllers commands
 */
#define	ALTERNATE	NONE
#include "exec_show_controllers_tring.h"
LINK_POINT(tring_showcontrollers_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Token Ring
 */
static parser_extension_request tring_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(tring_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(tring_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(tring_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(tring_config_commands) },    
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(tring_interface_commands) },
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(tring_showcontrollers_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * tring_parser_init - Initialize Token Ring parser support
 */
void tring_parser_init (void)
{
    parser_add_command_list(tring_chain_init_table, "tring");
}
