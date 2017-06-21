/* $Id: snmp_chain.c,v 3.7.10.1 1996/05/21 10:02:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/snmp_chain.c,v $
 *------------------------------------------------------------------
 * SNMP parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snmp_chain.c,v $
 * Revision 3.7.10.1  1996/05/21  10:02:22  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7  1996/02/22  14:37:54  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/01/29  07:29:49  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/17  19:46:12  jjohnson
 * CSCdi47027:  Need mechanism for debugging snmp timers
 *
 * Revision 3.4  1995/12/17  18:37:13  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/16  00:14:35  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
 *
 * Revision 3.2  1995/11/17  18:58:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/04  01:57:09  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/07/03  22:26:24  jjohnson
 * CSCdi35401:  Parser help for snmp-server packetsize is incorrect
 * Differentiate between the minimum, maximum, and default packetsize
 *
 * Revision 2.1  1995/06/07  22:48:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sr_snmpd.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "packet.h"
#include "../ui/debug.h"
#include "snmp_debug.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_snmp.h"
#include "../os/buffers.h"
#include "login.h"


/*
 * Parse chains for SNMP debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_snmp.h"
LINK_POINT(snmp_debug_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for SNMP show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_snmp.h"
LINK_POINT(snmp_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SNMP config commands
 */
#define ALTERNATE       NONE
#include "cfg_snmp.h"
LINK_POINT(snmp_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for SNMP interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_snmp.h"
LINK_POINT(snmp_interface_commands, ALTERNATE);
#undef	ALTERNATE




/*
 * Parse chain registration array for SNMP commands
 */
static parser_extension_request snmp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(snmp_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(snmp_show_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(snmp_config_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(snmp_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * snmp_parser_init - Initialize parse structures for parsing SNMP commands
 */
void snmp_parser_init (void)
{
    static boolean snmp_parser_initialized = FALSE;

    if (!snmp_parser_initialized) {
	parser_add_command_list(snmp_chain_init_table, "snmp");
	parser_add_link_point(PARSE_ADD_CFG_SNMP_CMD, "config snmp",
			      &pname(conf_snmp_extend_here));
	parser_add_link_point(PARSE_ADD_CFG_SNMP_HOST_CMD, "config snmp host",
			      &pname(conf_snmp_host_extend_here));
	parser_add_link_exit(PARSE_ADD_CFG_SNMP_HOST_EXIT, "config snmp host exit",
			     &pname(conf_snmp_host_return_here));
	parser_add_link_point(PARSE_ADD_CFG_SNMP_ENABLE_CMD,
			      "config snmp trap/inform",
			      &pname(conf_snmp_enable_extend_here));
	parser_add_link_exit(PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
			     "config snmp trap/inform exit",
			     &pname(conf_snmp_enable_return_here));
	parser_add_link_point(PARSE_ADD_CFG_SNMP_TRAP_FREQ_CMD,
			      "config snmp trap frequency",
			      &pname(conf_snmp_trap_freq_extend_here));
	parser_add_link_exit(PARSE_ADD_CFG_SNMP_TRAP_FREQ_EXIT,
			     "config snmp trap frequency exit",
			     &pname(conf_snmp_trap_freq_return_here));
	snmp_parser_initialized = TRUE;
    }
}
