
/* $Id: mmc_chain.c,v 3.4.10.1 1996/05/21 10:03:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-ls1010/mmc_chain.c,v $
 *------------------------------------------------------------------
 * Parser Chain routines for mmc driver
 *
 * September 1995, Ram Bhide
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mmc_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  10:03:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:38:12  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1996/02/01  06:09:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/12/17  18:37:48  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/11/09  13:23:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/27  20:56:08  jwilliam
 * inital checkin
 *
 * Revision 2.2  1995/08/03  00:28:55  jjohnson
 * CSCdi38141:  template files contain bogus log information
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../h/interface.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"
#include "../src-4k-ls1010/parser_defs_atmcore_oam.h"
#include "../src-4k-ls1010/mmc_debug_cmd.h"



/*
 * Parse chains for OAM debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_mmc.h"
LINK_POINT(mmc_debug_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for ATM OAM Commands
 */
static parser_extension_request mmc_parse_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(mmc_debug_commands) },
    { PARSE_LIST_END, NULL }
};


void mmc_parser_init(void)
{

    parser_add_command_list(mmc_parse_chain_init_table,"MMC ");

}
