/* $Id: ipaccess_chain.c,v 3.5.10.2 1996/05/21 09:51:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess_chain.c,v $
 *------------------------------------------------------------------
 * IP access lists parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipaccess_chain.c,v $
 * Revision 3.5.10.2  1996/05/21  09:51:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/04/19  15:41:23  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.5.18.1  1996/04/17  23:01:41  carrel
 * Branch: IosSec_branch
 * Final commit from IP code review comments
 *
 * Revision 3.5  1996/02/22  14:34:31  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:04:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:29:36  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:34:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:55:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  20:58:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "subsys.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "access.h"
#include "../parser/parser_defs_access.h"
#include "../ipmulticast/parser_defs_igmp.h"
#include "parser_defs_ip.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess.h"
#include "ip_macros.h"
#include "ip_actions.h"
#include "../ipmulticast/igmp.h"


/*
 * Parse chains for show IP access-list commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_access.h"
LINK_POINT(ip_show_access_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP Access lists config commands
 */
#define	ALTERNATE	NONE
#include "cfg_acclist_ip.h"
LINK_POINT(ip_acclist_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IP Access lists
 */
static parser_extension_request acclist_chain_init_table[] = {
    { PARSE_ADD_CFG_ACCLIST_CMD, &pname(ip_acclist_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(ip_show_access_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ipacclist_parser_init - Initialize IP Access lists parser support
 */
void ipacclist_parser_init (void)
{
    parser_add_command_list(acclist_chain_init_table, "ip access lists");
}
