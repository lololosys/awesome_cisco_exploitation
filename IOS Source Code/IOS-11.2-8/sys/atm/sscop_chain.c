/* $Id: sscop_chain.c,v 3.6.10.1 1996/05/21 09:43:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop_chain.c,v $
 *------------------------------------------------------------------
 * atm/sscop_chain.c: Parser specific routines
 *
 * Novmber 1993,  Percy P. Khabardar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: sscop_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  09:43:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:32:07  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  05:59:59  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:26:26  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:24:44  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:47:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  09:35:18  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  20:12:56  hampton
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
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_sscop.h"
#include "../atm/sscop_debug.h"
#include "sscop_private.h"


/*
 * Parse chains for SSCOP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_sscop.h"
LINK_POINT(sscop_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for SSCOP show commands
 */
#define ALTERNATE       NONE
#include "exec_show_sscop.h"
LINK_POINT(sscop_show_command, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for SSCOP interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_sscop.h"
LINK_POINT(sscop_interface_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for SSCOP commands.
 */
static parser_extension_request sscop_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(sscop_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(sscop_show_command) },
    { PARSE_ADD_CFG_INT_CMD, &pname(sscop_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * s s c o p _ p a r s e r _ i n i t ( )
 *
 * Initialize parse structures for parsing SSCOP commands.
 */
void sscop_parser_init (void)
{

    parser_add_command_list(sscop_chain_init_table, "SSCOP");

}
