/*--------------------------------------------------------------------------
 *
 * $Id: vlan_chain.c,v 3.5.10.1 1996/05/21 10:08:45 thille Exp $
 *
 * vlan/vlan_chain.c
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Functions specific to the protocol independent routing between Inter
 * Switch Link (ISL) and IEEE 802.10 based Virtual LANs.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  10:08:45  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:39:45  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:30:59  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1996/01/23  08:55:27  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "vlan.h"
#include "vlan_debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"



/*
 * Parse chains for CLEAR commands
 */

#define ALTERNATE NONE
#include "exec_clear_vlan.h"
LINK_POINT(vlan_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for VLAN debug commands
 */

#define	ALTERNATE	NONE
#include "exec_debug_vlan.h"
LINK_POINT(vlan_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for top-level exec commands.
 */

#define ALTERNATE NONE
#include "exec_show_vlan.h"
LINK_POINT(vlan_exec_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for vLAN Subsystem.
 */

static parser_extension_request vlan_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(vlan_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(vlan_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(vlan_exec_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * vlan_parser_init - Initialize vLAN Parser support.
 */

void vlan_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_VLAN, "vlan",
			  &pname(conf_vlan_extend_here));

    parser_add_command_list(vlan_chain_init_table, "vlan");
}

