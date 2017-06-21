/* $Id: vpn_chain.c,v 1.3.10.3 1996/07/12 17:29:42 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/vpn_chain.c,v $
 *------------------------------------------------------------------
 * vpn_chain.c --  modularity for VPN
 *
 * January 1996, Shoou Yiu
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vpn_chain.c,v $
 * Revision 1.3.10.3  1996/07/12  17:29:42  tkolar
 * CSCdi62195:  L2F tunnel clearing command needed
 * Branch: California_branch
 * Make is possible to shut down an entire tunnel, rather than having
 * to do it MID by MID.
 *
 * Revision 1.3.10.2  1996/05/21  10:08:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.3.10.1  1996/04/27  06:34:28  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.3  1996/04/16  23:58:25  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 1.2.2.2  1996/03/25  05:32:17  tkolar
 * Branch: Dial1_branch
 * Enable Tunnel authentication.
 *
 * Revision 1.2.2.1  1996/02/08  23:20:06  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN integration
 *
 * Revision 1.2  1996/01/31  18:38:41  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:12  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"
#include "../ui/debug.h"
#include "../vpn/vpn_debug.h"
#include "../util/md5.h"
#include "../vpn/l2f.h"


/*
 * Parse chains for VPN commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_vpn.h"
LINK_POINT(vpn_debug_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "exec_show_vpn.h"
LINK_POINT(vpn_show_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE       NONE
#include "cfg_vpn.h"
LINK_POINT(vpn_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE       NONE
#include "exec_clear_vpn.h"
LINK_POINT(vpdn_clear_commands, ALTERNATE);
#undef  ALTERNATE




/*
 * Parse chain registration array for VPN commands
 */
static parser_extension_request vpn_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(vpdn_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(vpn_debug_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(vpn_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(vpn_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * vpn_parser_init
 */
void vpn_parser_init (void)
{
    parser_add_command_list(vpn_chain_init_table, "vpn");
}
