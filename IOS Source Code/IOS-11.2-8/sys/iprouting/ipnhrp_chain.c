/* $Id: ipnhrp_chain.c,v 3.5.10.1 1996/05/21 09:53:20 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipnhrp_chain.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnhrp_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:53:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:35:14  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:05:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:30:59  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:34:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:54:53  bcole
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
#include "exec_show_ip_nhrp.h"
LINK_POINT(ip_nhrp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NHRP interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_ip_nhrp.h"
LINK_POINT(ip_nhrp_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for NHRP debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_ip_nhrp.h"
LINK_POINT(ip_nhrp_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for NHRP clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_ip_nhrp.h"
LINK_POINT(ip_nhrp_clear_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for NHRP
 */
static parser_extension_request ip_nhrp_chain_init_table[] = {
    { PARSE_ADD_SHOW_IP_CMD, &pname(ip_nhrp_show_commands) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(ip_nhrp_interface_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(ip_nhrp_debug_commands) },
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ip_nhrp_clear_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * nhrp_ip_parser_init - Initialize IPNHRP parser support
 */

void nhrp_ip_parser_init (void)
{
    parser_add_command_list(ip_nhrp_chain_init_table, "ip_nhrp");
}
