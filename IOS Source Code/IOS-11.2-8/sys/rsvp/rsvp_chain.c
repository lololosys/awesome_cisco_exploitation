/* $Id: rsvp_chain.c,v 1.1.4.3 1996/07/19 06:09:25 asastry Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_chain.c,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_chain.c,v $
 * Revision 1.1.4.3  1996/07/19  06:09:25  asastry
 * CSCdi63472:  Add page breaks to RSVP files
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/21  10:01:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.4.1  1996/04/17  13:57:16  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../ui/common_strings.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "logger.h"
#include "../os/techsupport.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "../ui/debug.h"
#include "../parser/macros.h"
#include "../parser/parser_actions.h"
#include "../parser/parser_defs_parser.h"
#include "../iprouting/route.h"
#include "../ipmulticast/mroute.h"
#include "rsvp_objects.h"
#include "rsvp_database.h"
#include "rsvp_inline.h"
#include "../iprouting/igrp.h"

/*
 * Parse chains for RSVP and general multicast debug commands.
 */
#define ALTERNATE NONE
#include "exec_debug_ip_rsvp.h"
LINK_POINT (rsvp_debug_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for RSVP show commands.
 */
#define ALTERNATE NONE
#include "exec_show_ip_rsvp.h"
LINK_POINT (rsvp_show_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain for RSVP clear commands.
 */
#define ALTERNATE NONE
#include "exec_clear_ip_rsvp.h"
LINK_POINT (rsvp_clear_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for RSVP top-level commands.
 */
#define ALTERNATE NONE
#include "cfg_ip_rsvp.h"
LINK_POINT (rsvp_global_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for RSVP interface commands.
 */
#define ALTERNATE NONE
#include "cfg_int_ip_rsvp.h"
LINK_POINT (rsvp_interface_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for show tech-support rsvp command
 */
LINK_EXIT(show_rsvp_techsupport_exit, no_alt);
KEYWORD_ID(show_rsvp_techsupport, show_rsvp_techsupport_exit,
	   NONE, OBJ(int,2), TECHSUPPORT_IP_RSVP, "rsvp",
	   "IP RSVP related information", PRIV_USER);
LINK_POINT(rsvp_show_techsupport_commands, show_rsvp_techsupport);
/*
 * Parse chain registration array for RSVP.
 */
static parser_extension_request rsvp_chain_init_table[] =
{
    {PARSE_ADD_DEBUG_IP_CMD, &pname(rsvp_debug_commands)},
    {PARSE_ADD_SHOW_IP_CMD, &pname(rsvp_show_commands)},
    {PARSE_ADD_CLEAR_IP_CMD, &pname(rsvp_clear_commands)},
    {PARSE_ADD_CFG_RTG_IP_CMD, &pname(rsvp_global_commands)},
    {PARSE_ADD_CFG_INT_IP_CMD, &pname(rsvp_interface_commands)},
    {PARSE_ADD_SHOW_TECHSUPPORT_CMD, &pname(rsvp_show_techsupport_commands)},
    {PARSE_ADD_SHOW_TECHSUPPORT_EXIT,
	(dynamic_transition *) &pname(show_rsvp_techsupport_exit)},
    {PARSE_LIST_END, NULL}
};

/*
 * rsvp_parser_init - Initialize RSVP parser support.
 */
void
rsvp_parser_init (void)
{
    parser_add_command_list(rsvp_chain_init_table, "rsvp");
}
