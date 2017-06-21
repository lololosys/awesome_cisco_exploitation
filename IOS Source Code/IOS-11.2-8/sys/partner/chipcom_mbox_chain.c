/* $Id: chipcom_mbox_chain.c,v 3.3.10.2 1996/05/21 09:59:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/partner/chipcom_mbox_chain.c,v $
 *------------------------------------------------------------------
 * Chipcom 2500  parse chains
 *
 * July 1995, Greg Stovall
 *
 * This file is just a rename of les/if_c3000_chain.c.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: chipcom_mbox_chain.c,v $
 * Revision 3.3.10.2  1996/05/21  09:59:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.10.1  1996/03/18  21:34:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.14.3  1996/03/16  07:23:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.14.2  1996/03/09  05:11:12  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.14.1  1996/02/20  16:49:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/22  14:37:06  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.2  1995/12/17  18:35:45  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/11/09  12:57:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/12  22:27:29  gstovall
 * CSCdi36994:  Partner code needs to be pulled out of core subsystem
 * Move the parse chain init from the c3000 specific file to a partner
 * specific file.  Make "debug mailbox" work on the 4500 in the process.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../les/mailbox_debug.h"


/*
 * Parse chains for PAN debugging commands
 */
#define ALTERNATE	NONE
#include "../les/if_les.h"
#include "../les/exec_debug_mailbox.h"
LINK_POINT(chipcom_mbox_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for Chipcom
 */
static parser_extension_request chipcom_mbox_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(chipcom_mbox_debug_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * chipcom_mbox_parser_init - Initialize product specific parser support
 */
void chipcom_mbox_parser_init (void)
{
    parser_add_command_list(chipcom_mbox_chain_init_table,
			    "chipcom_mbox_debug");
}
