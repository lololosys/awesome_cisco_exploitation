/* $Id: gdp_chain.c,v 3.5.10.1 1996/05/21 09:53:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/gdp_chain.c,v $
 *------------------------------------------------------------------
 * GDP parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: gdp_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:53:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:35:05  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:05:19  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:30:55  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:33:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:16:24  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  21:06:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_gdp.h"

#include "../ip/ip.h"


/*
 * Parse chains for GDP commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_ip_gdp.h"
LINK_POINT(gdp_ip_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for GDP
 */
static parser_extension_request gdp_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(gdp_ip_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * gdp_parser_init - Initialize GDP parser support
 */
void gdp_parser_init (void)
{
    parser_add_command_list(gdp_chain_init_table, "gdp");
}
