/* $Id: nhrp_debug.c,v 3.4.10.2 1996/07/25 09:57:22 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/nhrp_debug.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_debug.c,v $
 * Revision 3.4.10.2  1996/07/25  09:57:22  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/05/21  10:09:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:40:00  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:41:16  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:03:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:05  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "nhrp_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "nhrp_debug_flags.h"

/*
 * Parse chains for NHRP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_nhrp.h"
LINK_POINT(nhrp_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for NHRP commands
 */
static parser_extension_request nhrp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(nhrp_debug_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * nhrp_debug_all
 * is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void nhrp_debug_all (boolean flag)
{
    generic_debug_all(nhrp_debug_arr, flag);
}

/*
 * nhrp_debug_show
 * is called to display the values of all the debugging variables.
 */

static void nhrp_debug_show (void)
{
    debug_show_flags(&(nhrp_debug_arr[0]), "NHRP");
}

/*
 * nhrp_debug_init
 * is called at subsystem startup. It registers
 * the routines to maintain and display the subsystem's debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when the subsystem is started.
 */

static void nhrp_debug_init (void)
{
    /*
     * Register for "debug all" and "show debug" events.
     */
    reg_add_debug_all(nhrp_debug_all, "nhrp_debug_all");
    reg_add_debug_show(nhrp_debug_show, "nhrp_debug_show");
    /*
     * Make sure the debug flags are right at startup. If "debug all"
     * is in effect when the subsystem is initialized, we want to turn on
     * all the subsystem debugging right now.
     */
    nhrp_debug_all(debug_all_p());
}

/*
 * nhrp_parser_init - Initialize parse structures for NHRP protocol independent
 * commands.
 */

void nhrp_parser_init (void)
{
    parser_add_command_list(nhrp_chain_init_table, "nhrp");
    nhrp_debug_init();
}
