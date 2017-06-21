/* $Id: tacacs_chain.c,v 3.7.10.2 1996/05/21 10:01:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/tacacs_chain.c,v $
 *------------------------------------------------------------------
 * Tacacs parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tacacs_chain.c,v $
 * Revision 3.7.10.2  1996/05/21  10:01:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/04/19  15:29:33  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.7.20.1  1996/04/04  19:14:44  carrel
 * cvs fixes
 *
 * Revision 2.1.6.6  1996/03/19  08:33:05  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.5  1996/02/28  06:15:06  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.4  1996/02/21  03:59:11  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.3  1995/12/19  04:13:07  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:16:22  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  17:39:34  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.7  1996/02/22  14:37:43  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/16  00:48:50  hampton
 * Migrate files out of the parser directory.  [CSCdi49139]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/02/01  06:08:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/12/17  18:36:46  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/08  00:54:47  irfan
 * CSCdi45224:  tacacs could use a specified source address
 * allow tacacs (TA, XTA, TA+) to use a specific (up) interface's IP
 * address.
 *
 * Revision 3.2  1995/11/17  18:51:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:56:49  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:44:22  hampton
 * Bump version numbers from 1.x to 2.x.
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
#include "aaa.h"
#include "../servers/tacacs_plus.h"
#include "../servers/tacacs.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_tacacs.h"
#include "login.h"
#include "../ui/debug.h"
#include "../os/os_debug.h"

/*
 * Parse chains for Tacacs config commands
 */
#define	ALTERNATE	NONE
#include "cfg_tacacs.h"
LINK_POINT(tacacs_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for top-level IP configuration commands
 */
#define ALTERNATE	NONE
#include "cfg_ip_tacacs.h"
LINK_POINT(tacacs_cfg_ip, ALTERNATE);
#undef ALTERNATE

/*
 * Debug chains for TACACS exec commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_tacacs.h"
LINK_POINT(debug_tacacs_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for Tacacs
 */
static parser_extension_request tacacs_chain_init_table[] = {
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(tacacs_config_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(tacacs_cfg_ip) },
    { PARSE_ADD_DEBUG_CMD, &pname(debug_tacacs_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * tacacs_parser_init - Initialize Tacacs parser support
 */
void tacacs_parser_init (void)
{
    parser_add_command_list(tacacs_chain_init_table, "tacacs");
    parser_add_link_point(PARSE_ADD_CFG_TACACS, "tacacs",
			  &pname(conf_tacacs_extend_here));
}

