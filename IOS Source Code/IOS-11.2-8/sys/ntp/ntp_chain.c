/* $Id: ntp_chain.c,v 3.4.10.4 1996/08/19 18:54:46 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for NTP
 *
 * January 5, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp_chain.c,v $
 * Revision 3.4.10.4  1996/08/19  18:54:46  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.4.10.3  1996/05/21  09:57:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.2  1996/04/16  19:03:04  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/03/18  21:12:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/16  07:09:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.1  1996/01/24  22:36:06  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/22  14:36:17  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:33:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:50:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:44:42  hampton
 * Bump version numbers from 1.x to 2.x.
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
#include "ntp_debug.h"
#include "ntp_refclock.h"
#include "ntp_registry_private.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_ntp.h"

/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_ntp.h"
LINK_POINT(ntp_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_ntp.h"
LINK_POINT(ntp_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_ntp.h"
LINK_POINT(ntp_int_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for NTP top-level configuation commands
 */
#define ALTERNATE NONE
#include "cfg_ntp.h"
LINK_POINT(ntp_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for NTP exec commands
 */
#define ALTERNATE NONE
#include "exec_ntp.h"
LINK_POINT(ntp_exec_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for NTP
 */
static parser_extension_request ntp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(ntp_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(ntp_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(ntp_int_commands) },
    { PARSE_ADD_CFG_LAST_CMD, &pname(ntp_config_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(ntp_exec_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ntp_chain_init - Initialize parse chains for NTP
 * 
 * This routine is called at system startup to register the parse chains
 * for NTP.
 */

void ntp_chain_init (void)
{
    parser_add_command_list(ntp_chain_init_table, "ntp");
}
