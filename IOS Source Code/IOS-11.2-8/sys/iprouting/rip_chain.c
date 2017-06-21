/* $Id: rip_chain.c,v 3.5.10.1 1996/05/21 09:53:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/rip_chain.c,v $
 *------------------------------------------------------------------
 * RIP parse chains
 *
 * March 1995, Paul Traina
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rip_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:53:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:35:23  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:28:49  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:31:05  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:36:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/11  19:38:01  pst
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "access.h"
#include "login.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "iprouting_debug.h"
#include "rip.h"


/*
 * Parse chains for RIP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_rip.h"
LINK_POINT(ip_rip_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for RIP specific interface subcommands
 */
#define ALTERNATE       NONE
#include "cfg_int_ip_rip.h"
LINK_POINT(ip_rip_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for RIP
 */
static parser_extension_request ip_rip_chain_init_table[] = {
    { PARSE_ADD_DEBUG_IP_CMD,   &pname(ip_rip_debug_commands) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(ip_rip_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ip_rip_parser_init - Initialize RIP parser support
 */
void ip_rip_parser_init (void)
{
    parser_add_command_list(ip_rip_chain_init_table, "rip");
}
