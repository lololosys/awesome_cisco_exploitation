/* $Id: dlcsw_chain.c,v 3.7.10.3 1996/07/09 05:48:21 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcsw_chain.c,v $
 *------------------------------------------------------------------
 * CLS Protocol Converter Parse Chains
 * 
 * September 1994,  Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlcsw_chain.c,v $
 * Revision 3.7.10.3  1996/07/09  05:48:21  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.7.10.2  1996/05/21  09:44:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/05/17  10:42:46  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.2  1996/04/26  15:16:37  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.1  1996/04/26  07:33:12  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7  1996/02/22  14:32:24  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/01  06:00:36  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:26:58  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:25:24  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  08:55:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:23  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:12:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/13  19:04:03  kchiu
 * CSCdi38691:  FRAS: the frame-relay map commmand must come after the
 * sdlc address
 *
 * Revision 2.3  1995/07/04  01:52:21  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/28  10:18:37  smackie
 * Subsystem header cleanup. Round up the last few stragglers and rope 'em
 * in with the rest of the herd. (CSCdi23568)
 *
 * Revision 2.1  1995/06/07  20:20:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_generic.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_dlcsw.h"
#include "dlcsw_debug.h"
#include "clsi_msg.h"
#include "clsi_queue.h"
#include "ieee.h"
#include "../if/rif.h"
#include "dlu.h"
#include "dlcsw.h"


#define	ALTERNATE	NONE
#include "exec_show_dlcsw.h"
LINK_POINT(dlcsw_show_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "exec_debug_dlcsw.h"
LINK_POINT(dlcsw_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for dlcsw config command
 */
#define	ALTERNATE	NONE
#include "cfg_int_dlcsw.h"
LINK_POINT(dlcsw_config_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for dlcsw
 */
static parser_extension_request dlcsw_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(dlcsw_show_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(dlcsw_debug_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(dlcsw_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * dlcsw_parser_init - Initialize CLS parser support
 */
static void dlcsw_parser_init (subsystype *subsys)
{
    parser_add_command_list(dlcsw_chain_init_table, "fras");
}

/*
 * DLCSW_UI subsystem header
 */

#define DLCSW_UI_MAJVERSION 1
#define DLCSW_UI_MINVERSION 0
#define DLCSW_UI_EDITVERSION  1

SUBSYS_HEADER(dlcsw_ui, DLCSW_UI_MAJVERSION, DLCSW_UI_MINVERSION, DLCSW_UI_EDITVERSION,
	      dlcsw_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "seq: sdlc_ui, srb_ui", "req: dlcsw, srb_ui, sdlc_ui");
