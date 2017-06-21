/* $Id: fddi_chain.c,v 3.5.10.2 1996/05/21 09:49:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/fddi_chain.c,v $
 *------------------------------------------------------------------
 * FDDI parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fddi_chain.c,v $
 * Revision 3.5.10.2  1996/05/21  09:49:48  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/03/18  20:13:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  06:55:19  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/02/20  00:56:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/22  14:34:03  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:03:59  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3.14.1  1996/01/24  22:18:15  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/17  18:28:47  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:27:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:53:28  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:52:13  hampton
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
#include "../parser/parser_defs_parser.h"
#include "../if/parser_defs_fddi.h"
#include "fddi.h"
#include "fddi_private.h"
#include "fddi_debug.h"


/*
 * Parse chains for FDDI exec commands
 */
#define	ALTERNATE	NONE
#include "exec_cmt.h"
LINK_POINT(cmt_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for FDDI debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_fddi.h"
LINK_POINT(fddi_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for FDDI config commands
 */
#define	ALTERNATE	NONE
#include "cfg_smt.h"
LINK_POINT(smt_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for FDDI interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_fddi.h"
LINK_POINT(fddi_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for FDDI show controllers commands
 */
#define	ALTERNATE	NONE
#include "exec_show_controllers_fddi.h"
LINK_POINT(fddi_showcontrollers_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for FDDI
 */
static parser_extension_request fddi_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(cmt_exec_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(fddi_debug_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(smt_config_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(fddi_interface_commands) },
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(fddi_showcontrollers_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * fddi_parser_init - Initialize FDDI parser support
 */
void fddi_parser_init (void)
{
    static fddi_parser_initialized = FALSE;

    if (!fddi_parser_initialized) {
	parser_add_link_point(PARSE_ADD_CFG_INT_FDDI_CMD, "fddi",
			      &pname(cfg_int_fddi_extend_here));
	parser_add_command_list(fddi_chain_init_table, "fddi");
	fddi_parser_initialized = TRUE;
    }
}
