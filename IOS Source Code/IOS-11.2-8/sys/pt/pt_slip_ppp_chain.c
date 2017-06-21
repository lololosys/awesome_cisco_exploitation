/* $Id: pt_slip_ppp_chain.c,v 3.7.10.1 1996/05/21 10:00:51 thille Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_slip_ppp_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for PT SLIP or PPP
 *
 * May 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pt_slip_ppp_chain.c,v $
 * Revision 3.7.10.1  1996/05/21  10:00:51  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7  1996/02/22  14:37:22  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/22  05:20:28  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.5  1996/02/01  06:08:23  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:29:34  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:36:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:00:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:56:34  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:35:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "aaa.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_pt.h"
#include "../parser/parser_defs_parser.h"
#include "../wan/parser_defs_ppp.h"
#include "pt_slip_ppp.h"

/*
 * Parse chains for SLIP translate out commands
 */
#define       ALTERNATE       NONE
#include "cfg_translate_slip_ppp_out.h"
LINK_POINT(slip_ppp_out_translate_commands, ALTERNATE);
#undef        ALTERNATE

/*
 * Parse chains for PT SLIP-PPP config commands
 */
#define ALTERNATE       NONE
#include "cfg_pt_slip_ppp.h"
LINK_POINT(pt_slip_ppp_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for SLIP or PPP
 */
static parser_extension_request pt_slip_ppp_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(pt_slip_ppp_config_commands) },
    { PARSE_ADD_CFG_TRANS_OUT_CMD, &pname(slip_ppp_out_translate_commands) },
    { PARSE_ADD_CFG_TRANS_OUT_RETURN, 
	  (dynamic_transition *) &pname(trans_out_slip_ppp_return) },
    { PARSE_LIST_END, NULL }
};

void pt_slip_ppp_parser_init (void)
{
    parser_add_command_list(pt_slip_ppp_chain_init_table, "pt_async");
}

