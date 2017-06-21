/* $Id: srb_chain.c,v 3.6.10.3 1996/07/09 06:06:11 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_chain.c,v $
 *------------------------------------------------------------------
 * SRB parse chaincs
 *
 * October 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: srb_chain.c,v $
 * Revision 3.6.10.3  1996/07/09  06:06:11  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.6.10.2  1996/05/21  10:05:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/05/17  12:14:27  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.2  1996/04/26  15:23:47  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.1  1996/04/26  07:59:00  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6  1996/02/22  14:38:47  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:10:39  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:30:11  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:38:52  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:57:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:58:16  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/28  09:31:45  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:02:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "interface_generic.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "access.h"
#include "../if/tring.h"
#include "../srt/rsrb.h"
#include "packet.h"
#include "../srt/span.h"
#include "../ui/debug.h"
#include "../srt/srt_debug.h"
#include "../ibm/parser_defs_sdlc.h"
#include "../srt/parser_defs_srb.h"
#include "../parser/parser_defs_sse.h"


/*
 * Parse chains for SRB clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_srb.h"
LINK_POINT(srb_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SRB debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_srb.h"
LINK_POINT(srb_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SRB show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_srb.h"
LINK_POINT(srb_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SRB config commands
 */
#define	ALTERNATE	NONE
#include "cfg_srb.h"
LINK_POINT(srb_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_srb.h"
LINK_POINT(srb_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for SRB
 */
static parser_extension_request srb_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(srb_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(srb_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(srb_show_commands) },
    { PARSE_ADD_CFG_TOP_AC_CMD, &pname(srb_config_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(srb_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * srb_parser_init - Initialize SRB parser support
 */
void srb_parser_init (subsystype *subsys)
{
    parser_add_protoaddr(LINK_RSRB, PMATCH_RSRB, "rsrb",
			 "Remote Source-Route Bridging", 0, ADDR_ILLEGAL);
    parser_add_link_point(PARSE_ADD_SHOW_SRB_CMD, "show source-bridge",
			  &pname(show_source_bridge_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_TOP_SRB_CMD, "source-bridge conf cmd",
			  &pname(csrb_extend_here));
    parser_add_command_list(srb_chain_init_table, "srb");
}


/*
 * Source Route Bridging user interface (SRB_UI) subsystem header
 */

#define SRB_UI_MAJVERSION 1
#define SRB_UI_MINVERSION 0
#define SRB_UI_EDITVERSION  1

SUBSYS_HEADER(srb_ui, SRB_UI_MAJVERSION, SRB_UI_MINVERSION, SRB_UI_EDITVERSION,
	      srb_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: srb", NULL);



