/* $Id: ipaccount_chain.c,v 3.5.10.1 1996/05/21 09:51:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccount_chain.c,v $
 *------------------------------------------------------------------
 * IP accounting parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipaccount_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:51:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:34:33  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:28:33  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:29:39  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:34:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:55:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  20:58:23  hampton
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
#include "../parser/parser_defs_tcpip.h"


/*
 * Parse chains for IP Accounting clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_ip_account.h"
LINK_POINT(ipaccount_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Accounting show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_account.h"
LINK_POINT(ipaccount_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Accounting config commands
 */
#define	ALTERNATE	NONE
#include "cfg_ip_account.h"
LINK_POINT(ipaccount_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IP Accounting
 */
static parser_extension_request ipaccount_chain_init_table[] = {
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ipaccount_clear_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(ipaccount_show_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(ipaccount_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ipaccount_parser_init - Initialize IP Accounting parser support
 */
void ipaccount_parser_init (void)
{
    parser_add_command_list(ipaccount_chain_init_table, "ipaccount");
}
