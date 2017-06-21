/* $Id: ipfast_chain.c,v 3.5.10.2 1996/05/21 09:51:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_chain.c,v $
 *------------------------------------------------------------------
 * IP fast parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipfast_chain.c,v $
 * Revision 3.5.10.2  1996/05/21  09:51:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/04/15  21:16:24  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/22  14:34:37  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:28:35  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:29:42  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:34:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:55:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  20:58:59  hampton
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
#include "../parser/parser_defs_parser.h"
#include "../iprouting/parser_defs_iprouting.h"


/*
 * Parse chains for IP Fast clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_ip_fast.h"
LINK_POINT(ipfast_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Fast show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_fast.h"
LINK_POINT(ipfast_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IP Fast config commands
 */
#define ALTERNATE       NONE
#include "../iprouting/cfg_ip_cache.h"
LINK_POINT(ipfast_ip_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for IP Fast
 */
static parser_extension_request ipfast_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(ipfast_ip_commands) },
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ipfast_clear_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(ipfast_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ipfast_parser_init - Initialize IP Fast parser support
 */
void ipfast_parser_init (void)
{
    parser_add_command_list(ipfast_chain_init_table, "ipfast");
}
