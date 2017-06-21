/* $Id: lex_chain.c,v 3.6.10.3 1996/05/30 23:44:47 hampton Exp $
 * $Source: /release/112/cvs/Xsys/lex/lex_chain.c,v $
 *------------------------------------------------------------------
 * lex_chain.c -- LEX parse chains
 *
 * Feb. 1994, David Coli
 *
 * Copyright (c) 1994-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex_chain.c,v $
 * Revision 3.6.10.3  1996/05/30  23:44:47  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.6.10.2  1996/05/21  09:56:44  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/05/17  11:26:51  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.1  1996/04/26  07:56:28  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6  1996/02/22  14:36:10  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:07:03  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:01  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:33:07  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:42:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:54:33  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  21:40:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "access.h"
#include "ieee.h"
#include "interface_private.h"
#include "../if/priority_private.h" /* needed for PRIORITY_MAXLIST definition */
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_lex.h"
#include "../parser/parser_defs_access.h"
#include "lex_debug.h"
#include "lex_rcmd.h"
#include "lex.h"  /* needed for num_lex_interfaces definition */

/*
 * Parse chains for LEX debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_lex.h"
LINK_POINT(lex_debug_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for all lex interface configuration commands
 */
#define ALTERNATE       NONE
#include "cfg_int_lex.h"
LINK_POINT(lex_int_lex_commands, ALTERNATE);
#undef  ALTERNATE

/* 
 * Parse chains for LEX show controller command
 */
#define ALTERNATE       NONE
#include "exec_show_controllers_lex.h"
LINK_POINT(lex_show_controller_command, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "exec_copy_lex.h"
LINK_POINT(lex_copy_command, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "exec_copy_flash_lex.h"
LINK_POINT(lex_copy_flash_command, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "exec_clear_counters_lex.h"
LINK_POINT(lex_clear_ctrs_command, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "exec_clear_controller_lex.h"
LINK_POINT(lex_clear_controller_command, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "exec_show_interfaces_lex.h"
LINK_POINT(lex_show_interface_command, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for LEX commands
 */
static parser_extension_request lex_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(lex_debug_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(lex_int_lex_commands) },
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(lex_show_controller_command) },
    { PARSE_ADD_COPY_CMD, &pname(lex_copy_command) },
    { PARSE_ADD_COPY_CMD, &pname(lex_copy_flash_command) },
    { PARSE_ADD_CLEAR_CMD, &pname(lex_clear_ctrs_command) },
    { PARSE_ADD_CLEAR_CMD, &pname(lex_clear_controller_command) },
    { PARSE_ADD_SHOW_CMD, &pname(lex_show_interface_command) },
    { PARSE_LIST_END, NULL }
};

/*
 * lex_parser_init - Initialize parse structures for parsing LEX commands
 */
void lex_parser_init (void)
{
    parser_add_command_list(lex_chain_init_table, "lex");
}
