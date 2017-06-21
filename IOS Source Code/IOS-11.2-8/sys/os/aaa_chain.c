/* $Id: aaa_chain.c,v 3.7.10.1 1996/05/21 09:57:59 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/aaa_chain.c,v $
 *------------------------------------------------------------------
 * aaa_chain.c -- Parser chain stuff
 *
 * September 1994, Dave Carrel
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: aaa_chain.c,v $
 * Revision 3.7.10.1  1996/05/21  09:57:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7  1996/02/22  14:36:27  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/16  00:48:42  hampton
 * Migrate files out of the parser directory.  [CSCdi49139]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/02/01  06:07:27  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/17  18:34:03  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:44:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  10:50:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/30  06:42:22  dstine
 * CSCdi36619:  IDB nits
 *         - remove #include of interface_private.h
 *
 * Revision 2.1  1995/06/07  21:53:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "access.h"
#include "ttysrv.h"
#include "aaa.h"
#include "../ui/debug.h"
#include "os_debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_aaa.h"

int count;


/*
 * Parse chains for authentication config commands
 */
#define ALTERNATE NONE
#include "cfg_authentication.h"
LINK_POINT(authen_config_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Debug chains for AAA exec commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_aaa.h"
LINK_POINT(debug_aaa_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registrationa array for AAA
 */
static parser_extension_request aaa_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(authen_config_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(debug_aaa_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * aaa_parser_init - Initialize parse chains for AAA
 *
 * This routine is called at system startup to register the parse chains
 * for AAA.
 */

void aaa_parser_init (void)
{
    parser_add_command_list(aaa_chain_init_table, "aaa");
}
