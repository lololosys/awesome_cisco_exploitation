/* $Id: ncia_chain.c,v 3.1.2.3 1996/06/23 05:53:01 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_chain.c,v $
 *------------------------------------------------------------------
 * ncia_chain.c - NCIA Parse Chains
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_chain.c,v $
 * Revision 3.1.2.3  1996/06/23  05:53:01  schiang
 * CSCdi61120:  split exec_show_ncia.h into exec_show_ncia.h and
 * exec_ncia.h
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/18  19:01:23  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:25  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.2  1996/05/03  22:06:19  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:40  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:14:39  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "interface.h"
#include "subsys.h"
#include "parser.h"
#include "interface_private.h"
#include "../parser/parser_actions.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/macros.h"
#include "access.h"
#include "../if/tring.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../srt/srt_debug.h"
#include "../cls/dlc_public.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc.h"
#include "../cls/ncia.h"


#include "registry.h"
#include "../ui/debug.h"
#include "../cls/ncia_debug.h"
#include "../cls/ncia_debug_flags.h"

#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"



/*
 * Parse chains for NCIA exec commands
 */
#define ALTERNATE       NONE
#include "exec_ncia.h"
LINK_POINT(ncia_exec_commands, ALTERNATE);
#undef  ALTERNATE



/*
 * Parse chains for NCIA debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_ncia.h"
LINK_POINT(ncia_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NCIA show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ncia.h"
LINK_POINT(ncia_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NCIA clear commands
 */
#define ALTERNATE       NONE
#include "exec_clear_ncia.h"
LINK_POINT(ncia_clear_commands, ALTERNATE);
#undef  ALTERNATE



/*
 * Parse chains for NCIA config commands
 */
#define	ALTERNATE	NONE
#include "cfg_ncia.h"
LINK_POINT(ncia_config_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chain registration array for NCIA
 */
static parser_extension_request ncia_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(ncia_exec_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(ncia_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(ncia_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(ncia_clear_commands) },
    { PARSE_ADD_CFG_TOP_AC_CMD, &pname(ncia_config_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ncia_parser_init - Initialize NCIA parser support
 */
static void ncia_parser_init (subsystype *subsys)
{
    parser_add_command_list(ncia_chain_init_table, "ncia");
}

/*
 * NCIA (NCIA_UI) subsystem header
 */

#define NCIA_UI_MAJVERSION 1
#define NCIA_UI_MINVERSION 0
#define NCIA_UI_EDITVERSION  1

SUBSYS_HEADER(ncia_ui, NCIA_UI_MAJVERSION, NCIA_UI_MINVERSION, NCIA_UI_EDITVERSION,
	      ncia_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: dlsw", NULL);
