/* $Id: dncnv_chain.c,v 3.5.10.1 1996/05/21 09:44:55 thille Exp $
 * $Source: /release/112/cvs/Xsys/decnet/dncnv_chain.c,v $
 *------------------------------------------------------------------
 * DECnet conversion commands
 *
 * October 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dncnv_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:44:55  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:32:35  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:00:49  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:25:42  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:59:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:44:28  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  20:23:05  hampton
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
#include "../decnet/dnconversion.h"
#include "parser_defs_dec.h"



/*
 * Parse chains for DECnet conversion config commands
 */
#define	ALTERNATE	NONE
#include "cfg_decnet_dncnv.h"
LINK_POINT(dncnv_cfg_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for DECnet conversion
 */
static parser_extension_request dncnv_chain_init_table[] = {
    { PARSE_ADD_CFG_DECNET, &pname(dncnv_cfg_command) },
    { PARSE_LIST_END, NULL }
};


/*
 * dncnv_parser_init - Initialize parse chains for DECnet conversion
 */
void dncnv_parser_init (void)
{
    parser_add_command_list(dncnv_chain_init_table, "decnet conversion");
}
