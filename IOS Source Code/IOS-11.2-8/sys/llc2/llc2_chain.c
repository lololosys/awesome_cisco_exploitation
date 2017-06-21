/* $Id: llc2_chain.c,v 3.6.10.2 1996/07/31 06:48:34 gcox Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llc2_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for LLC2
 *
 * January 5, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: llc2_chain.c,v $
 * Revision 3.6.10.2  1996/07/31  06:48:34  gcox
 * CSCdi64685:  Convert LLC2 to interface_generic.h (except llc2.c)
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/05/21  09:56:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:36:13  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:07:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:03  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:33:15  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:43:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:54:39  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/28  09:29:05  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:41:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_generic.h"
#include "packet.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "llc2.h"
#include "llc2_debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_llc2.h"
#include "../parser/parser_defs_parser.h"


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_llc2.h"
LINK_POINT(llc2_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for SHOW commands
 */
#define ALTERNATE NONE
#include "exec_show_llc2.h"
LINK_POINT(llc2_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_llc2.h"
LINK_POINT(llc2_int_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for LLC2 top-level  commands
 */
#define ALTERNATE NONE
#include "exec_llc2.h"
LINK_POINT(llc2_exec_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for LLC2
 */
static parser_extension_request llc2_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(llc2_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(llc2_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(llc2_int_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(llc2_exec_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * llc2_parser_init - Initialize parse chains for LLC2
 * 
 * This routine is called at system startup to register the parse chains
 * for LLC2.
 */

void llc2_parser_init (subsystype *subsys)
{
    parser_add_protoaddr(LINK_LLC2, PMATCH_LLC2 ,
		"llc2",    "llc2", 0, ADDR_ILLEGAL);
    parser_add_command_list(llc2_chain_init_table, "llc2");
}

/* 
 * LLC2 parser subsystem header
 */
#define LLC2_UI_MAJVERSION 1
#define LLC2_UI_MINVERSION 0
#define LLC2_UI_EDITVERSION 1

SUBSYS_HEADER(llc2_ui, LLC2_UI_MAJVERSION, LLC2_UI_MINVERSION,  LLC2_UI_EDITVERSION,
	      llc2_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "seq: rsrb_ui",
	      NULL);

