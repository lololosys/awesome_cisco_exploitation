/* $Id: tunnel_chain.c,v 3.5.10.1 1996/05/21 09:51:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/tunnel_chain.c,v $
 *------------------------------------------------------------------
 * Tunnel parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tunnel_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:51:43  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:34:43  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:04:36  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:29:56  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:37:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:55:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  21:01:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ip/tunnel.h"
#include "../ip/tunnel_debug.h"
#include "parser_defs_tunnel.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for Tunnel debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_tunnel.h"
LINK_POINT(tunnel_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Tunnel interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_tunnel.h"
LINK_POINT(tunnel_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Tunnel
 */
static parser_extension_request tunnel_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(tunnel_debug_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(tunnel_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * tunnel_parser_init - Initialize Tunnel parser support
 */
void tunnel_parser_init (void)
{
    parser_add_command_list(tunnel_chain_init_table, "tunnel");
}
