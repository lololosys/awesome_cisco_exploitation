/* $Id: access_chain.c,v 3.6.10.1 1996/05/21 09:58:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/access_chain.c,v $
 *------------------------------------------------------------------
 * Access List parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: access_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  09:58:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:36:29  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:29:06  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:34:04  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:44:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:50:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:53:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"


/*
 * Parse chains for Access Lists clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_access-lists.h"
LINK_POINT(access_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Access Lists show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_access-lists.h"
LINK_POINT(access_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Access Lists config commands
 */
#define ALTERNATE       NONE
#include "cfg_access-list.h"
LINK_POINT(access_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Access Lists
 */
static parser_extension_request access_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(access_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(access_clear_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(access_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * access_parser_init - Initialize Access Lists parser support
 */
void access_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_ACCLIST_CMD, "access list",
			  &pname(cfg_accl_extend_here));
    parser_add_command_list(access_chain_init_table, "access");
}
