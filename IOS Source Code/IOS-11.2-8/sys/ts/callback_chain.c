/* $Id: callback_chain.c,v 3.6.10.1 1996/05/21 10:07:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/ts/callback_chain.c,v $
 *------------------------------------------------------------------
 * Callback parser chain setup
 *
 * April 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: callback_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:07:03  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:39:12  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:10:56  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:30:33  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:39:44  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:52:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:58:36  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  23:08:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ts/parser_defs_callback.h"
#include "../ts/callback.h"
#include "../ts/callback_debug.h"
#include "access.h"


/*
 * callback line commands
 */
#define ALTERNATE NONE
#include "cfg_line_callback.h"
LINK_POINT(callback_line_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * callback debug commands
 */
#define	ALTERNATE NONE
#include "exec_debug_callback.h"
LINK_POINT(callback_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for callback
 */
static parser_extension_request callback_init_table[] = {
    { PARSE_ADD_LINE_TERM_CMD, &pname(callback_line_commands) } ,
    { PARSE_ADD_DEBUG_CMD, &pname(callback_debug_commands) } ,
    { PARSE_LIST_END, NULL }
};

void callback_parser_init (void)
{
    parser_add_command_list(callback_init_table, "callback");
}
