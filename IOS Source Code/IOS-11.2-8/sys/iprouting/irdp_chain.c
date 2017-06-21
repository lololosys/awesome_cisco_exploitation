/* $Id: irdp_chain.c,v 3.5.10.1 1996/05/21 09:53:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/irdp_chain.c,v $
 *------------------------------------------------------------------
 * IRDP parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: irdp_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:53:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:35:19  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:05:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:31:03  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:35:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:16:30  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  21:08:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_irdp.h"
#include "../iprouting/irdp.h"


/*
 * Parse chains for IRDP show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_irdp.h"
LINK_POINT(irdp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IRDP interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_ip_irdp.h"
LINK_POINT(irdp_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IRDP
 */
static parser_extension_request irdp_chain_init_table[] = {
    { PARSE_ADD_SHOW_IP_CMD, &pname(irdp_show_commands) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(irdp_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * irdp_parser_init - Initialize IRDP parser support
 */
void irdp_parser_init (void)
{
    parser_add_command_list(irdp_chain_init_table, "irdp");
}
