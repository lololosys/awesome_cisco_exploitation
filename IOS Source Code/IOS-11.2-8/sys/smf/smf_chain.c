/* $Id: smf_chain.c,v 3.5.10.1 1996/05/21 10:01:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/smf/smf_chain.c,v $
 *------------------------------------------------------------------
 * Software MAC Addres Filter (SMF) parse chains.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The SMF is intended for use when an interface is listening
 * promiscuously to match and dispose of received packets based
 * upon their destination MAC addresses in an efficient way.
 * In particular, it provides a facility that may be used on a
 * per-interface basis in support of Concurrent Routing and
 * Bridging (CRB) and eventually Integrated Routing and Bridging (IRB).
 *
 *------------------------------------------------------------------
 * $Log: smf_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  10:01:49  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:37:48  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:29:41  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:36:55  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:52:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:56:55  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/16  23:39:29  widmer
 * CSCdi35982:  show ? displays some wrong info
 *
 * Revision 2.1  1995/06/07  22:44:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "packet.h"
#include "interface_private.h"

#include "config.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
/*
  #include "../parser/parser_defs_smf.h"
 */
#include "../parser/parser_defs_parser.h"

#include "../ui/debug.h"
#include "smf_debug.h"
#include "smf_public.h"


/*
 * Parse chains for SMF debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_smf.h"
LINK_POINT(smf_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for top-level exec commands
 */
#define ALTERNATE NONE
#include "exec_show_smf.h"
LINK_POINT(smf_exec_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for SMF
 */
static parser_extension_request smf_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(smf_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(smf_exec_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * smf_parser_init - Initialize SMF parser support
 */
void smf_parser_init (void)
{
    parser_add_command_list(smf_chain_init_table, "smf");
}
