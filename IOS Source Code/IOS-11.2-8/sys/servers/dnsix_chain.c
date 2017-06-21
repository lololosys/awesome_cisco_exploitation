/* $Id: dnsix_chain.c,v 3.5.10.1 1996/05/21 10:01:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/dnsix_chain.c,v $
 *------------------------------------------------------------------
 * DMDP parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dnsix_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  10:01:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:37:37  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:08:44  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:36:42  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:50:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:56:45  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:43:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../servers/dmdp.h"
#include "../servers/parser_defs_dnsix.h"
#include "../servers/dnsix_debug.h"


/*
 * Parse chains for DNSIX debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_dnsix.h"
LINK_POINT(dnsix_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for DNSIX show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_dnsix.h"
LINK_POINT(dnsix_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for DNSIX config commands
 */
#define	ALTERNATE	NONE
#include "cfg_dnsix.h"
LINK_POINT(dnsix_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for DNSIX
 */
static parser_extension_request dnsix_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(dnsix_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(dnsix_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(dnsix_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * dnsix_parser_init - Initialize DNSIX parser support
 */
void dnsix_parser_init (void)
{
    parser_add_command_list(dnsix_chain_init_table, "dnsix");
}
