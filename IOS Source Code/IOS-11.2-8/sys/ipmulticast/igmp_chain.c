/* $Id: igmp_chain.c,v 3.5.10.2 1996/05/22 19:20:02 mleelani Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/igmp_chain.c,v $
 *------------------------------------------------------------------
 * igmp_chain.c - IGMP parse chains.
 *
 * November 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: igmp_chain.c,v $
 * Revision 3.5.10.2  1996/05/22  19:20:02  mleelani
 * CSCdi58181:  7000: crash at _fast_check
 * Branch: California_branch
 * Wrong check for slow acl.
 *
 * Revision 3.5.10.1  1996/05/21  09:52:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:34:54  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:04:47  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:30:17  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:29:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:56:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  21:03:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../parser/macros.h"
#include "../parser/parser_actions.h"
#include "../parser/parser_defs_parser.h"
#include "../h/access.h"
#include "parser_defs_igmp.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../util/radix.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/dvmrp.h"
#include "../iprouting/route.h"

/*
 * Parse chains for IGMP debug commands.
 */
#define	ALTERNATE	NONE
#include "exec_debug_ip_igmp.h"
LINK_POINT(igmp_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IGMP show commands.
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_igmp.h"
LINK_POINT(igmp_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for IGMP interface commands.
 */
#define ALTERNATE       NONE
#include "cfg_int_ip_igmp.h"
LINK_POINT(igmp_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for IGMP.
 */
static parser_extension_request igmp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_IP_CMD, &pname(igmp_debug_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(igmp_show_commands) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(igmp_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * igmp_parser_init - Initialize IGMP parser support.
 */
void igmp_parser_init (void)
{
    parser_add_command_list(igmp_chain_init_table, "igmp");
}



