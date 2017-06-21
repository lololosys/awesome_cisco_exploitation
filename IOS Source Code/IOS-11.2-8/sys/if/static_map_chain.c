/* $Id: static_map_chain.c,v 3.7.4.4 1996/06/17 23:31:44 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/static_map_chain.c,v $
 *------------------------------------------------------------------
 * static_map_chain.c -- Support for static maps configuration commands.
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: static_map_chain.c,v $
 * Revision 3.7.4.4  1996/06/17  23:31:44  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.7.4.3  1996/05/21  09:50:10  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.4.2  1996/04/25  23:13:47  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.4.1.2.1  1996/04/24  04:06:58  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.7.4.1  1996/03/18  20:14:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.3  1996/03/16  06:56:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.14.2  1996/03/07  09:44:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.14.1  1996/02/20  14:24:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/09  22:55:29  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.6  1996/02/22  14:34:11  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1995/12/17  18:28:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/01  15:56:21  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.3  1995/11/17  09:31:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/02  10:53:30  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/20  17:14:54  widmer
 * CSCdi36160:  map-class mode broken
 *
 * Revision 2.2  1995/06/15  15:18:31  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  20:55:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "static_map.h"
#include "../wan/parser_defs_frame.h"
#include "../tcp/http.h"
#include "../ip/ip_registry.h"
#include "../if/network.h"

parser_mode *map_class_mode;
parser_mode *map_list_mode;

/*
 * Parse chains for map-class command
 */
#define ALTERNATE       NONE
#include "cfg_map-class.h"
LINK_POINT(static_map_class_command, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain for map-list commands
 */
#define ALTERNATE       NONE
#include "cfg_static-map.h"
LINK_POINT(static_map_list_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain for map-list command
 */
#define ALTERNATE       NONE
#include "cfg_map-list.h"
LINK_POINT(static_map_list_command, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain for map-group command
 */
#define ALTERNATE       NONE
#include "cfg_int_map-group.h"
LINK_POINT(static_map_interface_command, ALTERNATE);
#undef  ALTERNATE

/*********************************************************
 *      start of configure map-class commands
 */

LINK_EXIT(after_mapclass_exit, no_alt);

NVCOMMENT(after_mapclass_comment, after_mapclass_exit);
#define	ALTERNATE	after_mapclass_comment
#include "cfg_mapclass_exit.h"
LINK_TRANS(cfg_mapclass_extend_here, ALTERNATE);

NO_OR_DEFAULT(configmapclass_no, cfg_mapclass_extend_here,
	   PRIV_MIN | PRIV_NONVGEN);
HELP	(top_config_mapclass, configmapclass_no,
	 "Static maps class configuration commands:\n");

/*********************************************************
 *      start of configure map-list commands
 */


NVCOMMENT(after_maplist_comment, after_mapclass_exit);
#undef	ALTERNATE
#define	ALTERNATE	after_maplist_comment
#include "cfg_maplist_exit.h"
LINK_TRANS      (cfg_maplist_extend_here, ALTERNATE);

NO_OR_DEFAULT(configmaplist_no, cfg_maplist_extend_here,
	   PRIV_MIN | PRIV_NONVGEN);

HELP	(top_config_maplist, configmaplist_no,
	 "Static maps list configuration commands:\n");


/*
 * Parse chain registration array for static map commands
 */
static parser_extension_request static_map_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_CMD, &pname(static_map_interface_command) },
    { PARSE_ADD_CFG_MAPLIST, &pname(static_map_list_command) },
    { PARSE_ADD_CFG_MAPLIST_CMD, &pname(static_map_list_commands) },
    { PARSE_ADD_CFG_MAPCLASS, &pname(static_map_class_command) },
    { PARSE_ADD_GLOBAL_EXIT,
	  (dynamic_transition *) &pname(after_mapclass_exit) },
    { PARSE_LIST_END, NULL }
};

static void *mapclass_save_var (parseinfo *csb)
{
    mapclass_type *mapclass = csb->mapclass;

    csb->mapclass = NULL;
    return(mapclass);
}

static void mapclass_reset_var (parseinfo *csb, void *var)
{
    csb->mapclass = var;
}

static boolean map_class_http_init (parseinfo *csb)
{
    char *encap = reg_invoke_http_get_uri_token(csb->http_state);
    char *name = reg_invoke_http_get_uri_token(csb->http_state);
    uint type;
    mapclass_type *class_ptr;

    type = encaptype(encap);
    class_ptr = mapclass_lookup(name, type);
    if (class_ptr) {
	csb->mapclass = class_ptr;
	csb->mode = map_class_mode;
	return(TRUE);
    }
    reg_invoke_http_send_not_found(csb->http_state, "\nInvalid map class.");
    return(FALSE);
}

static void *maplist_save_var (parseinfo *csb)
{
    maplist_type *maplist = csb->maplist;

    csb->maplist = NULL;
    return(maplist);
}

static void maplist_reset_var (parseinfo *csb, void *var)
{
    csb->maplist = var;
}

static boolean map_list_http_init (parseinfo *csb)
{
    char *name = reg_invoke_http_get_uri_token(csb->http_state);
    maplist_type *list_ptr;

    list_ptr = maplist_lookup(name);
    if (list_ptr) {
	csb->maplist = list_ptr;
	csb->mode = map_list_mode;
	return(TRUE);
    }
    reg_invoke_http_send_not_found(csb->http_state, "\nInvalid map list.");
    return(FALSE);
}


/* static_map_parser_init
 * Initialize static map parser support
 */
void static_map_parser_init (void)
{
    parser_add_command_list(static_map_chain_init_table, "static-map");

    parser_add_link_point(PARSE_ADD_CFG_STATIC_MAP,
			  "static maps configuration",
			  &pname(cfg_static_map_here));
    parser_add_link_point(PARSE_ADD_CFG_MAPLIST_CMD,
			  "static map list configuration",
			  &pname(cfg_maplist_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_MAPCLASS_CMD,
			  "static map class configuration",
			  &pname(cfg_mapclass_extend_here));
    map_class_mode = parser_add_mode("map-class", "config-map-class",
				     "Map class configuration mode",
				     TRUE, TRUE, "configure",
				     mapclass_save_var, mapclass_reset_var,
				     &pname(top_config_mapclass),
				     map_class_http_init);
    map_list_mode = parser_add_mode("map-list", "config-map-list",
				    "Map list configuration mode",
				    TRUE, TRUE, "configure",
				    maplist_save_var, maplist_reset_var,
				    &pname(top_config_maplist),
				    map_list_http_init);
}
