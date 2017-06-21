/* $Id: rmon_lite_chain.c,v 3.4.12.1 1996/05/21 10:01:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/rmon/rmon_lite_chain.c,v $
 *------------------------------------------------------------------
 * rmon_lite_chain.c:  Remote Monitoring Parse Chains
 *
 * February 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_lite_chain.c,v $
 * Revision 3.4.12.1  1996/05/21  10:01:06  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:37:33  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1996/02/18  17:57:51  jjohnson
 * CSCdi49292:  rmon events and alarms do not survive a reload
 * implement config commands to configure/nvgen events and alarms
 *
 * Revision 3.2  1996/02/09  07:57:20  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.1  1996/02/09  06:12:08  jjohnson
 * Initial placeholder for rmon user interface enhancements
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
#include "parser_defs_rmon.h"
#include "rmon_lite.h"
#include "sr_rmonmibdefs.h"

/*
 * Parse chains for all rmon show commands
 */
#define ALTERNATE       NONE
#include "exec_show_rmon_lite.h"
LINK_POINT(rmon_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for all rmon global configuration commands
 */
LINK_TRANS(cfg_rmon_extend_here, NONE);
#define ALTERNATE       cfg_rmon_extend_here

#include "cfg_rmon_alarm.h"
#include "cfg_rmon_event.h"

KEYWORD (cfg_rmon, ALTERNATE, NONE, "rmon", "Remote Monitoring", PRIV_CONF);
LINK_POINT(rmon_global_commands, cfg_rmon);
#undef  ALTERNATE

/*
 * Parse chain registration array for RMONlite commands
 */
static parser_extension_request rmon_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(rmon_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(rmon_global_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * rmon_lite_parser_init - Initialize parse structures for RMON commands
 */
void
rmon_lite_parser_init (void)
{
    parser_add_command_list(rmon_chain_init_table, "rmon");
    parser_add_link_point(PARSE_ADD_CFG_RMON_CMD, "cfg rmon",
			  &pname(cfg_rmon_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_RMON_CMD, "show rmon",
			  &pname(show_rmon_extend_here));
    parser_add_link_exit(PARSE_ADD_SHOW_RMON_RET, "show rmon exit",
			 &pname(show_rmon_return_here));
}
