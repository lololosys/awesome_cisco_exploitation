/* $Id: flh_chain.c,v 3.4.10.2 1996/05/21 09:56:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/flh_chain.c,v $
 *------------------------------------------------------------------
 * flh_chain.c -- parser commands for flh.
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flh_chain.c,v $
 * Revision 3.4.10.2  1996/05/21  09:56:03  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.1  1996/03/18  20:40:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/16  07:11:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.1  1996/01/24  22:22:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/22  14:35:50  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:32:42  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:37:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:52:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:31:39  hampton
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
#include "../parser/parser_defs_flash.h"

/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_flh.h"
LINK_POINT(flh_show_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for COPY commands
 */
#define ALTERNATE NONE
#include "exec_copy_flhlog.h"
LINK_POINT(flh_copy_commands, ALTERNATE);
#undef ALTERNATE


static parser_extension_request flh_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(flh_show_commands) },
    { PARSE_ADD_COPY_CMD, &pname(flh_copy_commands) },
    { PARSE_LIST_END, NULL }
};

void flh_parser_init (void)
{
    parser_add_command_list(flh_chain_init_table, "flh");
}


