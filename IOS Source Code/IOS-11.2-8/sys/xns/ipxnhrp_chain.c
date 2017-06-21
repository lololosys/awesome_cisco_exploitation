/* $Id: ipxnhrp_chain.c,v 3.5.10.2 1996/05/30 23:48:14 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/ipxnhrp_chain.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipxnhrp_chain.c,v $
 * Revision 3.5.10.2  1996/05/30  23:48:14  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.5.10.1  1996/05/21  10:11:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:40:33  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:13:27  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:42:23  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:22:07  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:07  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "xns.h"
#include "novell.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../ui/debug.h"
#include "../wan/nhrp_public.h"
#include "../wan/nhrp_debug.h"
#include "../wan/parser_defs_nhrp.h"
#include "../h/access.h"

/*
 * Parse chains for NHRP show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ipx_nhrp.h"
LINK_POINT(ipx_nhrp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NHRP interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_ipx_nhrp.h"
LINK_POINT(ipx_nhrp_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for NHRP clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_ipx_nhrp.h"
LINK_POINT(ipx_nhrp_clear_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for NHRP
 */
static parser_extension_request ipx_nhrp_chain_init_table[] = {
    { PARSE_ADD_SHOW_IPX_CMD, &pname(ipx_nhrp_show_commands) },
    { PARSE_ADD_CFG_INT_IPX_CMD, &pname(ipx_nhrp_interface_commands) },
    { PARSE_ADD_CLEAR_IPX_CMD, &pname(ipx_nhrp_clear_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * nhrp_ipx_parser_init - Initialize IPXNHRP parser support
 */

void nhrp_ipx_parser_init (void)
{
    parser_add_command_list(ipx_nhrp_chain_init_table, "ipx_nhrp");
}
