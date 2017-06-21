/* $Id: netbios_as_chain.c,v 3.6.10.1 1996/05/21 09:48:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_as_chain.c,v $
 *------------------------------------------------------------------
 * netbios_as_chain.c - NBF Access Server parse chains
 *
 * September 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_as_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  09:48:52  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:33:40  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:03:11  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:28:00  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:28:07  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:23:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  00:32:11  syiu
 * Initial placeholder for NBF.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../ibm/parser_defs_netbios.h"
#include "../ui/debug.h"
#include "../ibm/netbios_as_debug.h"
#include "../if/tring.h"


/*
 * Parse chains for NETBIOS debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_netbios_as.h"
LINK_POINT(netbios_as_debug_commands, ALTERNATE);
#undef  ALTERNATE



/*
 * Parse chains for NETBIOS show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_netbios_as.h"
LINK_POINT(netbios_as_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NETBIOS interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_netbios_as.h"
LINK_POINT(netbios_as_interface_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for NetBIOS commands
 */
static parser_extension_request netbios_as_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(netbios_as_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(netbios_as_show_commands) },
    { PARSE_ADD_CFG_INT_NBIOS_CMD, &pname(netbios_as_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * netbios_as_parser_init
 */
void netbios_as_parser_init (void)
{
    parser_add_command_list(netbios_as_chain_init_table, "nbf");
}
