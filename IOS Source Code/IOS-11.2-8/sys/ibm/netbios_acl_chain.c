/* $Id: netbios_acl_chain.c,v 3.5.10.1 1996/05/21 09:48:51 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/netbios_acl_chain.c,v $
 *------------------------------------------------------------------
 * Netbios access list parse chains
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: netbios_acl_chain.c,v $
 * Revision 3.5.10.1  1996/05/21  09:48:51  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/02/22  14:33:38  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/01  06:03:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/17  18:28:06  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:23:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  10:53:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/28  09:25:39  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:47:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ibm/parser_defs_netbios.h"




/*
 * Parse chains for NETBIOS config commands
 */
#define	ALTERNATE	NONE
#include "cfg_netbios_acl.h"
LINK_POINT(netbios_acl_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NETBIOS interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_netbios_acl.h"
LINK_POINT(netbios_acl_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NETBIOS novell interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_novell_netbios.h"
LINK_POINT(netbios_nov_int_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for NETBIOS access lists
 */
static parser_extension_request netbios_acl_chain_init_table[] = {
    { PARSE_ADD_CFG_NBIOS_CMD, &pname(netbios_acl_config_commands) },
    { PARSE_ADD_CFG_INT_NBIOS_CMD, &pname(netbios_acl_interface_commands) },
    { PARSE_ADD_CFG_INT_IPX_CMD, &pname(netbios_nov_int_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * netbios_acl_parser_init - Initialize NETBIOS access list parser support
 */
void netbios_acl_parser_init (subsystype *subsys)
{
    parser_add_command_list(netbios_acl_chain_init_table, "netbios acl");
}

/*
 * NETBIOS ACL UI (NETBIOS_ACL_UI) subsystem header
 */

#define NETBIOS_ACL_UI_MAJVERSION 1
#define NETBIOS_ACL_UI_MINVERSION 0
#define NETBIOS_ACL_UI_EDITVERSION  1

SUBSYS_HEADER(netbios_acl_ui, NETBIOS_ACL_UI_MAJVERSION, NETBIOS_ACL_UI_MINVERSION, NETBIOS_ACL_UI_EDITVERSION,
	      netbios_acl_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: netbios_acl", NULL);
