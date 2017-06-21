/* $Id: envmon_chain.c,v 3.5.10.1 1996/05/21 09:47:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/envmon_chain.c,v $
 *------------------------------------------------------------------
 * Environmental Monitor parse chains
 *
 * April 1995, Robert Widmer
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: envmon_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:47:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:33:12  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1995/12/17  18:27:19  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/22  23:41:58  sdurham
 * CSCdi35282:  Need command to configure envmon trap enables
 *      add individual trap options in separate parser-mib file
 *      envmon_mib_chain.
 *
 * Revision 3.2  1995/11/17  09:15:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:51:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:39:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "registry.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../hes/envmon_parser.h"

#define ALTERNATE       NONE
#include "exec_show_env.h"
LINK_POINT(show_env_command, ALTERNATE);
#undef  ALTERNATE


static parser_extension_request envmon_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(show_env_command) },
    { PARSE_LIST_END, NULL }
};


void envmon_parser_init (void)
{
    parser_add_command_list(envmon_chain_init_table, "env");
}
