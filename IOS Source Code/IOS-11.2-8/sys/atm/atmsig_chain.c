/* $Id: atmsig_chain.c,v 3.6.10.4 1996/07/10 22:20:15 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_chain.c,v $
 *------------------------------------------------------------------
 * atm/atmsig_chain.c: Parser specific routines
 *
 * Novmber 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_chain.c,v $
 * Revision 3.6.10.4  1996/07/10  22:20:15  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/21  09:43:02  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/05/14  06:28:52  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/04/25  23:02:14  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.12.2  1996/04/02  08:33:46  awu
 * Branch: ATM_Cal_branch
 * add rfc1695 support for router products.
 *
 * Revision 3.6.12.1  1996/03/30  03:34:40  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6  1996/02/22  14:32:05  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  05:59:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:26:23  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:24:43  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:46:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  09:35:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  20:11:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../h/interface_private.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_atmsig.h"
#include "../atm/atmsig_debug.h"
#include "../if/atm.h"
#include "atmsig_public.h"


/*
 * Parse chains for ATMSIG clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_atmsig.h"
LINK_POINT(atmSig_clear_vc, ALTERNATE);
#undef  ALTERNATE

KEYWORD (clear_atm_vc, clear_atmvc_if, NONE,
         "atm-vc", "Clear ATM virtual circuits on an interface",
          PRIV_OPR | PRIV_HIDDEN | PRIV_NONVGEN);

#undef ALTERNATE
#define ALTERNATE clear_atm_vc

LINK_POINT(atmSig_clear_vc_old, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for ATMSIG debug commands
 */
#define	ALTERNATE	NONE
#include "../atm/exec_debug_atmsig_router.h"
#include "../atm/exec_debug_atmsig_common.h"
LINK_POINT(atmsig_debug_commands, ALTERNATE);
#undef	ALTERNATE

#ifdef ATMSIG_NOTREQD
/*
 * Parse chains for ATM Signalling show commands
#define ALTERNATE       NONE
#include "exec_show_atmsig.h"
LINK_POINT(atmsig_show_commands, ALTERNATE);
#undef  ALTERNATE
*/
#endif

/*
 * Parse chains for ATM Signalling interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_atmsig.h"
LINK_POINT(atmSig_interface_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for ATM Signalling top-level  commands
 */
#define ALTERNATE NONE
#include "exec_atmsig.h"
LINK_POINT(atmSig_exec_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for ATM Signalling show commands
 */
static parser_extension_request atmsig_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(atmSig_clear_vc_old) },
    { PARSE_ADD_CLEAR_INT_ATM_CMD, &pname(atmSig_clear_vc) },
    { PARSE_ADD_DEBUG_INT_ATM_CMD, &pname(atmsig_debug_commands) },
#ifdef ATMSIG_NOTREQD
    { PARSE_ADD_SHOW_CMD, &pname(atmsig_show_commands) },
#endif
    { PARSE_ADD_CFG_INT_CMD, &pname(atmSig_interface_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(atmSig_exec_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * a t m S i g _ p a r s e r _ i n i t ( )
 *
 * Initialize parse structures for parsing ATM Signalling commands.
 */
void atmSig_parser_init (void)
{

    parser_add_command_list(atmsig_chain_init_table, "ATMSIG");

}
