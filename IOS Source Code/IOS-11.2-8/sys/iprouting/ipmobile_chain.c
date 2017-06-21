/* $Id: ipmobile_chain.c,v 3.5.10.1 1996/05/21 09:53:20 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipmobile_chain.c,v $
 *------------------------------------------------------------------
 * ipmobile_chain.c -- Parser commands for IP mobility
 *
 * Nov 1993 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipmobile_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:53:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:35:12  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:05:36  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:30:59  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:34:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:16:27  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  21:07:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "interface_private.h"
#include "access.h"
#include "login.h"
#include "../ui/debug.h"
#include "iprouting_debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"

#include "parser_defs_ipmobile.h"



/*
 * Parse chains for IP mobility interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_ip_mobile.h"
LINK_POINT(ip_mobile_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP mobility host database commands
 */

#define	ALTERNATE	NONE
#include "cfg_ip_mobile-host.h"
LINK_POINT(ip_mobile_host_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for IP mobility debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_mobile.h"
LINK_POINT(ip_mobile_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IP mobility
 */
static parser_extension_request ip_mobile_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(ip_mobile_interface_commands) },
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(ip_mobile_host_commands) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(ip_mobile_debug_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ip_mobile_parser_init - Initialize BGP parser support
 */
void ip_mobile_parser_init (void)
{
    parser_add_command_list(ip_mobile_chain_init_table, "ip mobile");
}
