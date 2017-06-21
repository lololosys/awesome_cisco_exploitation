/* $Id: pt_pad_chain.c,v 3.6.10.1 1996/05/21 10:00:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_pad_chain.c,v $
 *------------------------------------------------------------------
 * PT PAD parse chains
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_pad_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:00:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:37:20  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  06:08:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:32  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:36:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:59:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:56:33  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:35:10  hampton
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
#include "../x25/x25.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_address.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_switch.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_proto.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_bfe.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/x25_debug.h"
#include "../x25/pad.h" /* X25_ENABLE || DDN_ENABLE */
#include "../x25/lapb.h"
#include "../x25/parser_defs_x25.h"
#include "../x25/x25_actions.h"
#include "../pt/parser_defs_pt.h"


/*
 * Parse chains for PAD translate in commands
 */
#define	ALTERNATE	NONE
#include "cfg_translate_x25_in.h"
LINK_POINT(padin_translate_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for PAD translate out commands
 */
#define	ALTERNATE	NONE
#include "cfg_translate_x25_out.h"
LINK_POINT(padout_translate_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for PT PAD
 */
static parser_extension_request pt_pad_chain_init_table[] = {
    { PARSE_ADD_CFG_TRANS_IN_CMD, &pname(padin_translate_commands) },
    { PARSE_ADD_CFG_TRANS_IN_RETURN,
	  (dynamic_transition *) &pname(trans_in_x25_return) },
    { PARSE_ADD_CFG_TRANS_OUT_CMD, &pname(padout_translate_commands) },
    { PARSE_ADD_CFG_TRANS_OUT_RETURN,
	  (dynamic_transition *) &pname(trans_out_x25_return) },
    { PARSE_LIST_END, NULL }
};


/*
 * pt_pad_parser_init - Initialize PT PAD parser support
 */
void pt_pad_parser_init (void)
{
    parser_add_command_list(pt_pad_chain_init_table, "pt_pad");
}
