/* $Id: local_lnm_chain.c,v 3.5.10.2 1996/05/21 09:56:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/local_lnm_chain.c,v $
 *------------------------------------------------------------------
 * Local LNM  parse chains
 *
 * September 1994, Johnny Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: local_lnm_chain.c,v $
 * Revision 3.5.10.2  1996/05/21  09:56:18  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/05/17  11:26:42  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.1  1996/04/26  07:56:21  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5  1996/02/22  14:36:04  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/29  07:28:57  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:32:51  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:41:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:54:17  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  21:38:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "interface_generic.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"


/*
 * Parse chains for LOCAL_LNM enable command
 */
#define	ALTERNATE	NONE
#include "cfg_int_local_lnm.h"
LINK_POINT(local_lnm_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array LOCAL_LNM
 */
static parser_extension_request local_lnm_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(local_lnm_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * local_lnm_parser_init - Initialize Access Lists parser support
 */
void local_lnm_parser_init (void)
{
    parser_add_command_list(local_lnm_chain_init_table, "local_lnm");
}
