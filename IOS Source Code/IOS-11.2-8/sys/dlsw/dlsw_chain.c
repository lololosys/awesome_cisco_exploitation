/* $Id: dlsw_chain.c,v 3.7.10.3 1996/07/09 05:49:07 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_chain.c,v $
 *------------------------------------------------------------------
 * DLSw parse chains
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlsw_chain.c,v $
 * Revision 3.7.10.3  1996/07/09  05:49:07  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.7.10.2  1996/05/21  09:45:34  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/05/17  10:44:34  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.2  1996/04/26  15:17:07  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.1  1996/04/26  07:33:36  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7  1996/02/22  14:32:46  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:01:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:27:17  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:26:03  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:02:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:22:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:54:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/04  01:52:51  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/28  09:23:34  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:25:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "subsys.h"
#include "interface_generic.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "access.h"
#include "packet.h"
#include "../parser/parser_defs_parser.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../parser/parser_defs_sse.h"

#include "../ibm/netbios.h"
#include "../srt/rsrb.h"
#include "../ibm/libibm_externs.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../ui/debug.h"
#include "../dlsw/dlsw_debug.h"
#include "../dlsw/dlsw_core_debug.h"

/*
 * Parse chains for DLSw debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_dlsw.h"
LINK_POINT(dlsw_debug_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for DLSw show commands
 */
#define ALTERNATE       NONE
#include "exec_show_dlsw.h"
LINK_POINT(dlsw_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for DLSW config commands
 */
#define ALTERNATE       NONE
#include "cfg_dlsw.h"
LINK_POINT(dlsw_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for DLSW clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_dlsw.h"
LINK_POINT(dlsw_clear_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for DLSw
 */
static parser_extension_request dlsw_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(dlsw_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(dlsw_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(dlsw_clear_commands) },
    { PARSE_ADD_CFG_TOP_AC_CMD, &pname(dlsw_config_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * dlsw_parser_init() 
 */
void dlsw_parser_init (subsystype *subsys)
{
    parser_add_command_list(dlsw_chain_init_table, "dlsw");
    parser_add_protoaddr(LINK_DLSW, PMATCH_DLSW, "dlsw",
                         "Data Link Switching", 0, ADDR_ILLEGAL);
}

/*
 * DLSW subsystem header
 */

/*
 * DLSW parser subsystem header
 */

#define DLSW_UI_MAJVERSION 1
#define DLSW_UI_MINVERSION 0
#define DLSW_UI_EDITVERSION 0

SUBSYS_HEADER(dlsw_ui, DLSW_UI_MAJVERSION, DLSW_UI_MINVERSION,
	      DLSW_UI_EDITVERSION,
	      dlsw_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: cls, srb, iphost", NULL);





