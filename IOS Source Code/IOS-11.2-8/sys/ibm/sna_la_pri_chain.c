/* $Id: sna_la_pri_chain.c,v 3.4.10.2 1996/05/21 09:48:55 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sna_la_pri_chain.c,v $
 *------------------------------------------------------------------
 * SNA parser chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sna_la_pri_chain.c,v $
 * Revision 3.4.10.2  1996/05/21  09:48:55  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.1  1996/05/17  11:21:29  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.1  1996/04/26  07:54:36  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4  1996/02/22  14:33:46  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:28:09  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:24:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  10:54:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:48:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_generic.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../if/tring.h"
#include "../ibm/sna_la_pri.h"


/*
 * Parse chains for Local address priority list commands
 */
#define	ALTERNATE	NONE
#include "cfg_locaddr.h"
LINK_POINT(locaddr_pri_list_command, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SNA interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_locaddr.h"
LINK_POINT(sna_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for SNA
 */
static parser_extension_request sna_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(locaddr_pri_list_command) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(sna_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * sna_parser_init - Initialize SNA parser support
 */
void sna_parser_init (subsystype *subsys)
{
    parser_add_command_list(sna_chain_init_table, "sna");

    /* 
     * init sna lu loc addr priority struct. no need to
     * make this a separate subsystem.  in fact, we could 
     * change the code so these aren't pre-allocated.  not
     * going to bother yet because there are only 11 allocated.
     */
    snalapri_init();		
}


/*
 * SNA PRI UI (SNA_UI) subsystem header
 */

#define SNA_UI_MAJVERSION 1
#define SNA_UI_MINVERSION 0
#define SNA_UI_EDITVERSION  1

SUBSYS_HEADER(sna_ui, SNA_UI_MAJVERSION, SNA_UI_MINVERSION, SNA_UI_EDITVERSION,
	      sna_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "NULL",
	      "NULL");
