/* $Id: if_les_bri_chain.c,v 3.5.10.1 1996/05/21 09:56:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_bri_chain.c,v $
 *------------------------------------------------------------------
 * BRI parse chains
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_bri_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:56:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:35:59  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1995/12/17  18:32:48  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  17:40:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:52:42  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:35:51  hampton
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
#include "interface_private.h"
#include "logger.h"
#include "../parser/parser_defs_parser.h"
#include "../ui/debug.h"
#include "../if/bri_debug.h"

/*
 * Temporary hack until all the BRI drivers are combined
 */
#ifdef SAPPHIRE
#include "../les/if_les_bri_c1000.h"
#else
#include "../les/if_les_bri.h"
#endif


#define	ALTERNATE	NONE
#include "exec_show_controllers_bri.h"
LINK_POINT(bri_showcontroller_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for bri debug commands. The "debug bri" is applicable
 * only for low end and midrange platforms. ie why this is here.
 */
#define ALTERNATE       NONE
#include "exec_debug_bri.h"
LINK_POINT(bri_debug_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for BRI
 */
static parser_extension_request bri_chain_init_table[] = {
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(bri_showcontroller_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(bri_debug_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * Initialize BRI parser support
 */
void bri_parser_init (void)
{
    static boolean bri_parser_initialized = FALSE;

    if (!bri_parser_initialized) {
	parser_add_command_list(bri_chain_init_table, "bri");
	bri_debug_init();
    }
    
    bri_parser_initialized = TRUE;
}

