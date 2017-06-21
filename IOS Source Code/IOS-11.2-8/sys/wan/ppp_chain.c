/* $Id: ppp_chain.c,v 3.8.10.3 1996/05/21 10:09:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp_chain.c,v $
 *------------------------------------------------------------------
 * PPP parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ppp_chain.c,v $
 * Revision 3.8.10.3  1996/05/21  10:09:52  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.10.2  1996/05/19  05:49:09  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.8.10.1  1996/04/27  06:37:48  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.10.4  1996/04/17  00:02:09  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.4.10.3  1996/03/09  07:18:51  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 3.4.10.2  1996/02/06  20:30:18  perryl
 * Branch: Dial1_branch
 *
 * Revision 3.4.10.1  1996/01/16  15:45:30  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.8  1996/02/22  14:40:02  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/01  06:12:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/29  07:31:22  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/20  04:44:05  dblair
 * CSCdi45363:  MLP information no longer displayed in show dialer output
 * Move multilink bundle info from show dialer to new show ppp multilink
 * command
 *
 * Revision 3.4  1995/12/17  18:41:24  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  16:24:01  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  18:04:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/20  17:14:24  dblair
 * CSCdi40641:  Bridging over MLP ISDN PRI broken
 *         Bridge code assumed 6 byte header.  Multilink now completes
 *         header when receiving and encapsulated bridge packet
 *
 * Revision 2.3  1995/09/20  22:39:20  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.2  1995/07/02  01:46:14  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:19:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../wan/parser_defs_ppp.h"
#include "../wan/ppp_debug.h"
#include "../h/packet.h"
#include "../wan/ppp.h"
#include "aaa.h"
#include "../if/if_vtemplate.h"


/*
 * Parse chains for PPP debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ppp.h"
LINK_POINT(ppp_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for ppp show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ppp.h"
LINK_POINT(ppp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for PPP interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_ppp.h"
LINK_POINT(ppp_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for PPP interface encapsulation command
 */
LINK_EXIT(cfg_encap_ppp_exit, no_alt);
ENCAPS	(cfg_encap_ppp, cfg_encap_ppp_exit, NONE,
	 OBJ(int,1), ET_PPP,
	 "ppp", "Point-to-Point protocol",
	 (IDB_SYNC | IDB_ASYNC | IDB_ISDN | IDB_DIALER | IDB_VTEMPLATE));
LINK_POINT(cfg_encap_ppp_cmd, cfg_encap_ppp);


/*
 * Parse chain for PPP MLP global commands
 */
#define	ALTERNATE	NONE
#include "cfg_mlp.h"
LINK_POINT(ppp_mlp_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for PPP MLP inteface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_mlp.h"
LINK_POINT(multilink_command, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for PPP commands
 */
static parser_extension_request ppp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(ppp_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(ppp_show_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(ppp_interface_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(cfg_encap_ppp_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_ppp_exit) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(ppp_mlp_commands) } ,
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(multilink_command) },
    { PARSE_LIST_END, NULL }
};


/*
 * ppp_parser_init - Initialize parse structures for parsing PPP commands
 */
void ppp_parser_init (void)
{
    parser_add_command_list(ppp_chain_init_table, "ppp");
}
