/* $Id: ipfast_vip_chain.c,v 3.6.10.2 1996/05/21 09:51:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_vip_chain.c,v $
 *------------------------------------------------------------------
 * IP fast parse chains for VIP
 *
 * August 1995, Don Lobete
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipfast_vip_chain.c,v $
 * Revision 3.6.10.2  1996/05/21  09:51:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/04/15  21:16:25  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/22  14:34:39  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:04:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:28:36  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:29:48  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:35:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:06  dlobete
 * Placeholders for VIP_branch development.
 *
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
#include "../parser/parser_defs_tcpip.h"
#include "parser_defs_arp.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_priority.h"
#include "../wan/parser_defs_frame.h"
#include "../parser/parser_defs_snmp.h"
#include "ip_actions.h"
#include "ip_macros.h"



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
 * Parse chains for IP show commands (DFS FIXME)
 */
#define ALTERNATE       NONE
#include "exec_show_ip.h"
#include "exec_show_arp.h"
LINK_POINT(ip_show_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain registration array for IP Fast
 */
static parser_extension_request ipfast_chain_init_table[] = {
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ipfast_clear_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(ipfast_show_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Parse chain registration array for IP (DFS FIXME)
 */
static parser_extension_request ip_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(ip_show_commands) },
    { PARSE_LIST_END, NULL }
};



/*
 * ipfast_parser_init - Initialize IP Fast parser support
 */
void ipfast_parser_init (void)
{
    /* DFS FIXME - hack in here for now. */
    parser_add_link_point(PARSE_ADD_SHOW_IP_CMD, "show ip",
                          &pname(show_ip_extend_here));

    /* DFS FIXME - hack in here for now. */
    parser_add_command_list(ip_chain_init_table, "ip");

    parser_add_command_list(ipfast_chain_init_table, "ipfast");
}

/* DFS FIXME - some stubs */
void show_iptraffic (parseinfo *csb)
{
}

void show_ipsockets (parseinfo *csb)
{
}

void show_paths (parseinfo *csb)
{
}

void ip_show_interface (parseinfo *csb)
{
}

void ip_arp_show_command (parseinfo *csb)
{
}


