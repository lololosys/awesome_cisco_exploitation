/* $Id: lapb_chain.c,v 3.6.10.1 1996/05/21 10:10:35 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/lapb_chain.c,v $
 *------------------------------------------------------------------
 * LAPB parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lapb_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:10:35  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:40:20  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:12:55  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/17  18:41:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  16:24:09  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  18:06:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:45:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:21:00  hampton
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
#include "../x25/lapb.h"
#include "../x25/lapb_debug.h"
#include "parser_defs_lapb.h"
#include "parser_defs_x25.h"


/*
 * Parse chains for LAPB debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_lapb.h"
LINK_POINT(lapb_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for LAPB interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_lapb.h"
LINK_POINT(lapb_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for LAPB interface encapsulation command
 */
#define	ALTERNATE	NONE
#include "cfg_int_encaps_lapb.h"
LINK_POINT(lapb_encaps_cmd, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for LAPB
 */
static parser_extension_request lapb_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(lapb_debug_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(lapb_interface_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(lapb_encaps_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_lapb_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * lapb_parser_init - Initialize LAPB parser support
 */
void lapb_parser_init (void)
{
    parser_add_command_list(lapb_chain_init_table, "lapb");
}
