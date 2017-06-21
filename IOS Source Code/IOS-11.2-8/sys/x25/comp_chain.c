/* $Id: comp_chain.c,v 3.4.10.1 1996/05/21 10:10:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/comp_chain.c,v $
 *------------------------------------------------------------------
 * COMP_CHAIN.C
 *
 * 10/29/93 Bill Thomas
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: comp_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  10:10:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:40:15  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:41:48  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:05:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:45:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:20:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../x25/compress.h"
#include "../x25/comp_debug.h"
#include "../parser/parser_defs_compress.h"



/*
 * Parse chains for COMPRESS show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_compress.h"
LINK_POINT(comp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for COMPRESS debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_compress.h"
LINK_POINT(comp_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for COMPRESS interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_compress.h"
LINK_POINT(comp_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for COMPRESS 
 */
static parser_extension_request comp_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(comp_show_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(comp_debug_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(comp_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * comp_parser_init - Initialize COMPRESS parser support
 */
void compress_parser_init (void)
{
    parser_add_command_list(comp_chain_init_table, "compress");
}
