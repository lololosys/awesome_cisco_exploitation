/* $Id: ntp_refclock_chain.c,v 3.1.2.1 1996/08/19 18:55:14 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_refclock_chain.c,v $
 *------------------------------------------------------------------
 * Parser chains for NTP reference clock support
 *
 * July 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntp_refclock_chain.c,v $
 * Revision 3.1.2.1  1996/08/19  18:55:14  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1  1996/07/28  02:34:09  dkatz
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define CISCO

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "../if/network.h"
#include "access.h"
#include "../ui/debug.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "ntp_debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_ntp.h"


/*
 * Parse chains for line commands
 */
#define ALTERNATE NONE
#include "cfg_line_ntp.h"
LINK_POINT(ntp_line_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for NTP reference clock support
 */
static parser_extension_request ntp_refclock_chain_init_table[] = {
    { PARSE_ADD_CFG_LINE_CMD, &pname(ntp_line_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ntp_refclock_chain_init - Initialize parse chains for NTP refclocks
 * 
 * This routine is called at system startup to register the parse chains
 * for NTP reference clocks.
 */

void ntp_refclock_chain_init (void)
{
    parser_add_command_list(ntp_refclock_chain_init_table, "ntp");
}
