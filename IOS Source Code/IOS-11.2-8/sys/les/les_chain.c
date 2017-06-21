/* $Id: les_chain.c,v 3.4.10.2 1996/07/09 21:07:24 etrehus Exp $
 * $Source: /release/112/cvs/Xsys/les/les_chain.c,v $
 *------------------------------------------------------------------
 * LES parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: les_chain.c,v $
 * Revision 3.4.10.2  1996/07/09  21:07:24  etrehus
 * CSCdi45163:  transmitter-delay help message is confusing
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/05/21  09:56:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:36:02  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:32:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:41:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/03  06:33:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Automation missed on a dual-homed file.
 *
 * Revision 2.2  1995/07/02  10:52:44  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:37:50  hampton
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
#include "logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../les/if_les.h"
#include "registry.h"
#include  "../h/media_registry.regh"


/*
 * Parse chains for LES show controllers command
 */
#define	ALTERNATE	NONE
#include "exec_show_controllers_serial.h"
#include "exec_show_controllers_pcbus.h"
#include "exec_show_controllers_les.h"
LINK_POINT(les_showcontrol_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for LES interface commands
 */
#define	ALTERNATE	NONE
#include "../if/cfg_int_transmitter-delay.h"
LINK_POINT(les_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for LES test commands
 */
#define	ALTERNATE	NONE
#include "exec_test_leds.h"
LINK_POINT(les_test_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for LES
 */
static parser_extension_request les_chain_init_table[] = {
    { PARSE_ADD_SHOW_CONTROLLER_CMD, &pname(les_showcontrol_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(les_interface_commands) },
    { PARSE_ADD_TEST_CMD, &pname(les_test_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * les_ui_init - Initialize LES parser support
 */
void les_ui_init (subsystype *subsys)
{
    parser_add_command_list(les_chain_init_table, "les");
}


/*
 * les_ui subsystem header
 */
 
#define LES_UI_MAJVERSION    1
#define LES_UI_MINVERSION    0
#define LES_UI_EDITVERSION   1
 
SUBSYS_HEADER(les_ui, LES_UI_MAJVERSION, LES_UI_MINVERSION,
              LES_UI_EDITVERSION,
              les_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
