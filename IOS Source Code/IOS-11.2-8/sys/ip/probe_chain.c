/* $Id: probe_chain.c,v 3.5.10.1 1996/05/21 09:51:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/probe_chain.c,v $
 *------------------------------------------------------------------
 * probe_chain.c - Parse chains for probe support
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: probe_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:51:37  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:34:41  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:28:37  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:29:52  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:36:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/01  12:55:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  21:00:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../if/arp_debug.h"


/*
 * Parse chains for Probe debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_probe.h"
LINK_POINT(probe_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Probe
 */
static parser_extension_request probe_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(probe_debug_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * probe_parser_init - Initialize Probe parser support
 */
void probe_parser_init (void)
{
    parser_add_command_list(probe_chain_init_table, "probe");
}
