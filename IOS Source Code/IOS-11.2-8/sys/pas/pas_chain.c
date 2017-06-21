/* $Id: pas_chain.c,v 3.6.10.1 1996/05/21 10:00:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/pas_chain.c,v $
 *------------------------------------------------------------------
 * pas_chain.c - Platform independent PA parse chains
 *
 * January 1995, Michael Beesley 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: pas_chain.c,v $
 * Revision 3.6.10.1  1996/05/21  10:00:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6  1996/02/22  14:37:10  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:29:20  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:36:03  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/09  21:57:13  mbeesley
 * CSCdi45202:  VIP/PCI parse chains need slight reorganization
 *
 * Revision 3.2  1995/11/17  18:55:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "interface.h"
#include "config.h"
#include "parser.h"
#include "logger.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"

#include "if_pas.h"
#include "pas_show_chain.h"

static parser_extension_request pas_chain_init_table[] = {
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(pas_show_controller_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(pas_show_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(pci_show_commands) },
    { PARSE_ADD_SHOW_PCI_CMD, &pname(pci_show_hw_command) },
    { PARSE_LIST_END, NULL }
};

void show_pas_null (parseinfo *csb)
{
}

/*
 * pas_parser_init :
 * Initialize the platform independent parser chains for port adaptors
 */
void pas_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_SHOW_PAS_CMD, "show pas",
                          &pname(show_pas_extend_here));
    parser_add_link_point(PARSE_ADD_SHOW_PCI_CMD, "show pci",
                          &pname(show_pci_extend_here));
    parser_add_command_list(pas_chain_init_table, "pas");
}

/* end of file */
