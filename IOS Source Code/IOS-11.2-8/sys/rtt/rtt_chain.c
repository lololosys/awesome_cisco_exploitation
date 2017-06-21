/* $Id: rtt_chain.c,v 1.1.4.3 1996/06/17 23:36:23 hampton Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_chain.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor UI Debug parse chain.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor Debug parse chain.
 *------------------------------------------------------------------
 * $Log: rtt_chain.c,v $
 * Revision 1.1.4.3  1996/06/17  23:36:23  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/06/07  22:25:57  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:40:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:26  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:21  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:32  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:24  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "rtt_debug.h"
#include "subsys.h"
#include "rtt_util.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "parser_defs_rtt.h"
#include "../os/clock.h"
#include "rtt_cfg.h"
#include "rtt_ipecho_impl.h"


/*
 * Parse chains for RTTMON debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_rttmon.h"
LINK_POINT(rttmon_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for RTTMON show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_rttmon.h"
LINK_POINT(rttmon_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for RTTMON main Configuration
 */
#define ALTERNATE       NONE
#include "cfg_rttmon.h"
LINK_POINT(rttmon_cfg_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chain for RTTMON Entry Configuration
 */
#define ALTERNATE       NONE
#include "cfg_rttmon_entry.h"
LINK_POINT(rttmon_cfg_entry_commands, ALTERNATE);
#undef  ALTERNATE

/* A dynamic link point will be created at the
 *   cfg_rttmon_entry_extend_here transition,
 *   then the above rttmon_cfg_entry_commands
 *   will extend at that point.
 */
LINK_EXIT(after_rttmon_entry_exit, NONE);
#define ALTERNATE       after_rttmon_entry_exit
#include "cfg_rttmon_entry_exit.h"
LINK_TRANS(cfg_rttmon_entry_extend_here, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE cfg_rttmon_entry_extend_here
NO_OR_DEFAULT(cfg_rttmon_entry_no, ALTERNATE, PRIV_MIN | PRIV_NONVGEN); 
HELP(top_cfg_rttmon_entry, cfg_rttmon_entry_no,
     "Response Time Reporter Entry Configuration Commands:\n");



/*
 * Parse chain registration array for RTTMON
 *   PARSE_ADD_DEBUG_CMD -> attach to the main debug command
 *   PARSE_ADD_SHOW_CMD -> attach to the main show command
 *   PARSE_ADD_CFG_RTTMON_CMD -> attach to the top config mode command
 *   PARSE_ADD_CFG_RTTMON_ENTRY_CMD -> attach to the rtr entry
 *                                       config mode command
 */
static parser_extension_request rttmon_chain_init_table[] =
{
    {PARSE_ADD_DEBUG_CMD, &pname(rttmon_debug_commands)},
    {PARSE_ADD_SHOW_CMD, &pname(rttmon_show_commands)},
    {PARSE_ADD_CFG_RTTMON_CMD, &pname(rttmon_cfg_commands)},
    {PARSE_ADD_CFG_RTTMON_ENTRY_CMD, &pname(rttmon_cfg_entry_commands)},
    {PARSE_ADD_GLOBAL_EXIT,
	 (dynamic_transition *) & pname(after_rttmon_entry_exit)},
    {PARSE_LIST_END, NULL}
};



/*
 * rttmon_ui_init - Initialize RTTMON parser support
 */
static void rttmon_ui_init (subsystype * subsys)
{
    register_rtt_mon_impl();
    initBaseRttMonCtrlAdminQ();
    registerRtt();
    initGlobalRttInfo();
    rttmon_init_config_buffers();

    /* register link points with the parser */
    parser_add_link_point(PARSE_ADD_CFG_RTTMON_ENTRY_CMD,
			  "config rtr entry",
			  &pname(cfg_rttmon_entry_extend_here));

    /* link my commands to defined link points */
    parser_add_command_list(rttmon_chain_init_table,
			    D_rttMon_string);

    parser_add_mode(D_rttMon_string,
		    D_rttMon_ConfigSubmode_string,
		    "RTR Entry Configuration",
		    TRUE, TRUE,
		    "configure", NULL, NULL,
		    &pname(top_cfg_rttmon_entry),
		    NULL);

    return;
}
	
/*
 * RTTMON subsystem header
 */
#define RTTMON_MAJVERSION  1
#define RTTMON_MINVERSION  0
#define RTTMON_EDITVERSION 0 
SUBSYS_HEADER(rttmon, RTTMON_MAJVERSION, RTTMON_MINVERSION, RTTMON_EDITVERSION,
	      rttmon_ui_init, SUBSYS_CLASS_MANAGEMENT,
	      "seq: ipdiag",
	      NULL);
