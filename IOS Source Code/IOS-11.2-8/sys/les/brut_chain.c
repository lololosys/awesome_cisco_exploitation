/* $Id: brut_chain.c,v 3.4.10.2 1996/05/21 09:56:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/brut_chain.c,v $
 *------------------------------------------------------------------
 * BRUT parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: brut_chain.c,v $
 * Revision 3.4.10.2  1996/05/21  09:56:01  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.1  1996/03/18  20:40:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:11:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/09  05:07:11  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.14.1  1996/01/24  22:22:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/22  14:35:46  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:32:40  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:36:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:52:27  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:30:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "brut_debug.h"


/*
 * Parse chains for BRUT debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_brut.h"
LINK_POINT(brut_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for BRUT
 */
static parser_extension_request brut_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(brut_debug_commands) },
    {PARSE_LIST_END, NULL }
};


/*
 * brut_parser_init - Initialize BRUT parser support
 */
void brut_parser_init (void)
{
    parser_add_command_list(brut_chain_init_table, "brut");
}
