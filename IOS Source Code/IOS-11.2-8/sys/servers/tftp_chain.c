/* $Id: tftp_chain.c,v 3.4.10.1 1996/05/21 10:01:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/tftp_chain.c,v $
 *------------------------------------------------------------------
 * TFTP Server parser chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: tftp_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  10:01:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:37:45  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:36:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:52:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:56:50  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:44:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_misc.h"
#include "access.h"
#include "../ip/tftp_debug.h"


/*
 * Parse chains for TFTP Server exec commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_tftp.h"
LINK_POINT(tftp_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for TFTP Server config commands
 */
#define	ALTERNATE	NONE
#include "cfg_tftp.h"
LINK_POINT(tftp_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for Tftp
 */
static parser_extension_request tftp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(tftp_debug_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(tftp_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * tftp_parser_init - Initialize TFTP Server parser support
 */
void tftp_parser_init (void)
{
    parser_add_command_list(tftp_chain_init_table, "tftp");
}
