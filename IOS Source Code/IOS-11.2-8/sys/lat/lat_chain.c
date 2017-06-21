/* $Id: lat_chain.c,v 3.4.10.1 1996/05/21 09:55:10 thille Exp $
 * $Source: /release/112/cvs/Xsys/lat/lat_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for LAT
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lat_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  09:55:10  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:35:40  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:32:01  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:33:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/04  01:53:49  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  21:24:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_lat.h"
#include "../parser/parser_defs_exec.h"
#include "../lat/lat_debug.h"
#include "access.h"

/*
 * LAT debug commands
 */
#define	ALTERNATE NONE
#include "exec_debug_lat.h"
LINK_POINT(lat_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * LAT show commands
 */
#define ALTERNATE NONE
#include "exec_show_lat.h"
LINK_POINT(lat_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * LAT line commands
 */
#define ALTERNATE NONE
#include "cfg_line_lat.h"
LINK_POINT(lat_line_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * LAT config interface commands
 */
#define ALTERNATE NONE
#include "cfg_int_lat.h"
LINK_POINT(lat_config_int_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * LAT config routing commands
 */
#define ALTERNATE NONE
#include "cfg_lat_routing.h"
LINK_POINT(lat_config_routing_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * LAT config commands
 */
#define ALTERNATE NONE
#include "cfg_lat.h"
LINK_POINT(lat_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * LAT exec commands
 */
#define ALTERNATE NONE
#include "exec_lat.h"
LINK_POINT(lat_exec_commands, ALTERNATE);
#undef  ALTERNATE

LINK_EXIT(exec_connect_lat_return, no_alt);
STRING	(exec_connect_lat_port_name, exec_connect_lat_return, no_alt,
	 OBJ(string,3), "Destination LAT port name");
KEYWORD_ORTEST(exec_connect_lat_port, exec_connect_lat_port_name, NONE,
	       OBJ(int,2), LAT_PORT_SET,
	       "port", "Destination LAT port name", PRIV_USER);
STRING	(exec_connect_lat_node_name, exec_connect_lat_return, no_alt,
	 OBJ(string,2), "Name of a specific LAT node");
KEYWORD_ORTEST(exec_connect_lat_node, exec_connect_lat_node_name,
	       exec_connect_lat_port,
	       OBJ(int,2), LAT_NODE_SET,
	       "node", "Connect to a specific LAT node", PRIV_USER);
TESTVAR	(exec_connect_lat_test, exec_connect_lat_node, NONE,
	 NONE, NONE, NONE, OBJ(int,9), CMD_CONNECT_DEFAULT);
LINK_POINT(exec_connect_lat_enter, exec_connect_lat_test);

/*
 * Parse chain registration array for LAT
 */
static parser_extension_request latparse_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(lat_debug_commands) } ,
    { PARSE_ADD_SHOW_CMD, &pname(lat_show_commands) } ,
    { PARSE_ADD_LINE_TERM_CMD, &pname(lat_line_commands) } ,
    { PARSE_ADD_CFG_INT_CMD, &pname(lat_config_int_commands) } ,
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(lat_config_routing_commands) } ,
    { PARSE_ADD_CFG_TOP_CMD, &pname(lat_config_commands) } ,
    { PARSE_ADD_EXEC_CMD, &pname(lat_exec_commands) } ,
    { PARSE_ADD_EXEC_CONNECT_CMD, &pname(exec_connect_lat_enter) },
    { PARSE_ADD_EXEC_CONNECT_EXIT,
	  (dynamic_transition *) &pname(exec_connect_lat_return) },
    { PARSE_LIST_END, NULL }
};

void lat_parser_init (void)
{
    parser_add_command_list(latparse_init_table, "lat");
}
