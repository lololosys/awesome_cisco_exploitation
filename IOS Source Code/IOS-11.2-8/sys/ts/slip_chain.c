/* $Id: slip_chain.c,v 3.7.10.5 1996/08/14 06:25:50 irfan Exp $
 * $Source: /release/112/cvs/Xsys/ts/slip_chain.c,v $
 *------------------------------------------------------------------
 * Slip parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: slip_chain.c,v $
 * Revision 3.7.10.5  1996/08/14  06:25:50  irfan
 * CSCdi65960:  slip line command should be removed from IOS
 * Branch: California_branch
 * Remove code for obsolete UI.
 *
 * Revision 3.7.10.4  1996/07/17  23:03:54  irfan
 * CSCdi62510:  PPP modularity break-requires chat
 * Branch: California_branch
 * kill the deprecated "modem chat-script" line subcommand.
 *
 * Revision 3.7.10.3  1996/05/21  10:07:16  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.2  1996/04/27  06:28:31  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.10.3  1996/04/25  23:51:34  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.10.2  1996/04/16  23:47:06  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.10.1  1996/02/06  20:25:55  perryl
 * Branch: Dial1_branch
 *
 * Revision 3.7.10.1  1996/03/28  22:03:23  fox
 * CSCdi50974:  peer default ip address may not be accepted under int.
 * group-async
 * Branch: California_branch
 * Move 'group-async' commands to second parse link point so that it will
 * follow the 'peer default ip address' command.  Thus the 'member n peer
 * default ip address' command can selectively override the async group
 * setting instead of the other way around.
 *
 * Revision 3.7  1996/02/22  14:39:23  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:11:12  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:30:42  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:39:55  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  16:23:55  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  18:56:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:33:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/18  19:39:00  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.2  1995/07/04  01:59:05  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  23:10:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "aaa.h"
#include "../ts/if_slip.h"
#include "../ip/ip_addrpool.h"
#include "../if/async_debug.h"
#include "parser_defs_slip.h"
#include "access.h"
#include "registry.h"
#include "../h/cs_registry.regh"


/*
 * Parse chains for SLIP exec commands
 */
#define	ALTERNATE	NONE
#include "exec_slip.h"
LINK_POINT(slip_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SLIP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_async.h"
LINK_POINT(async_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SLIP show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_slip.h"
#include "exec_show_async.h"
LINK_POINT(slip_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Async Master interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_async_groups.h"
LINK_POINT(async_group_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SLIP interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_async.h"
LINK_POINT(async_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for SLIP interface encapsulation command
 */
LINK_EXIT(cfg_encaps_slip_exit, no_alt);
ENCAPS	(cfg_encaps_slip, cfg_encaps_slip_exit, NONE,
	 OBJ(int,1), ET_SLIP,
	 "slip", "Serial Line IP", (IDB_ASYNC | IDB_DIALER | IDB_VTEMPLATE));
LINK_POINT(cfg_encaps_slip_cmd, cfg_encaps_slip);


/*
 * Parse chain registration array for SLIP
 */
static parser_extension_request slip_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_CMD, &pname(async_interface_commands) },
    /*
     * Add the group-async and member commands into the second set of interface
     * configuration so that it follows the 'peer default ip address' command.
     * Thus the group can be configured one way and individual members the
     * other.			CSCdi50974
     */
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(async_group_interface_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(slip_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(slip_show_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(async_debug_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(cfg_encaps_slip_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encaps_slip_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * slip_parser_init - Initialize SLIP parser support
 */
void slip_parser_init (void)
{
    parser_add_command_list(slip_chain_init_table, "slip");
}
