/* $Id: loghist_chain.c,v 3.1.48.2 1996/08/28 22:54:37 ramk Exp $
 * $Source: /release/112/cvs/Xsys/os/loghist_chain.c,v $
 *------------------------------------------------------------------
 * loghist_chain.c
 *
 * September 1995, Anke Dosedal
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: loghist_chain.c,v $
 * Revision 3.1.48.2  1996/08/28  22:54:37  ramk
 * CSCdi66055:  Config command: no snmp enable traps syslog fails
 * Branch: California_branch
 *
 * Revision 3.1.48.1  1996/05/21  06:36:37  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/01/19  03:24:21  anke
 * Placeholder for Syslog MIB
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "syslogmib_history.h"
#include "parser_defs_logger.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_loghist.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_snmp.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"



/*
 * Parse chains for syslog history show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_syslog_history.h"
LINK_POINT(loghist_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Dynamically linked parse chains for configuring syslog history commands
 */
#define ALTERNATE       NONE
#include "cfg_syslog_history.h"
LINK_POINT(syslog_history_commands, ALTERNATE);
#undef  ALTERNATE


/* 
 * Parse chain registration array for syslog history commands
 */
static parser_extension_request syslog_hist_chain_init_table[] = {
    { PARSE_ADD_SHOW_LOG_CMD, &pname(loghist_show_commands) },
    { PARSE_ADD_CFG_LOG_CMD, &pname(syslog_history_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * syslog_parser_init - Initialize syslog history parser support
 */
void 
syslog_parser_init (void)
{
    parser_add_command_list(syslog_hist_chain_init_table, "logging history");
}

/*
 * snmp-server host command for writing to NVRAM
 */

static void 
syslog_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_SYSLOG), " syslog");
}

/*
 * enable traps for snmp-server enable traps command
 */

static void syslog_trap_cfg_set (boolean enable, uint subsys, uint traps)
{
    if ((subsys & (1 << TRAP_SYSLOG)))
        syslog_notification_enabled = enable;
}

/*
 * snmp-server enable trap command for writing to NVRAM
 */

static void 
syslog_trap_cfg_nvwrt (parseinfo *csb)
{
    nv_write(syslog_notification_enabled, "%s traps syslog", csb->nv_command);
}

/*
 * parser chain addition for snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_syslog_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_syslog,
           cfg_snmp_host_syslog_exit, NONE,
           OBJ(int,1), (1<<TRAP_SYSLOG),
           "syslog", "Allow SNMP syslog traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_syslog_entry, cfg_snmp_host_syslog);

/*
 * parser chain addition for snmp-server enable trap command
 */

LINK_EXIT(cfg_snmp_enable_syslog_exit, no_alt);
KEYWORD_OR(conf_snmp_enable_syslog, 
           cfg_snmp_enable_syslog_exit, NONE,
           OBJ(int,1), (1<<TRAP_SYSLOG),
           "syslog", "Enable SNMP syslog traps", PRIV_CONF);
LINK_POINT(cfg_snmp_enable_syslog_entry, conf_snmp_enable_syslog);

/*
 * Parse chain registration array for syslog history
 */
static parser_extension_request syslog_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_syslog_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
          (dynamic_transition *) &pname(cfg_snmp_host_syslog_exit) },
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD,
          &pname(cfg_snmp_enable_syslog_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
        (dynamic_transition *) &pname(cfg_snmp_enable_syslog_exit) },
    { PARSE_LIST_END, NULL }
};

/*
 * syslogmib_parser_init - Initialize syslog mib parser support
 */
void 
syslogmib_parser_init (void)
{
    reg_add_snmp_host_nvgen(syslog_snmp_host_nvgen, "syslog_snmp_host_nvgen");
    reg_add_Trap_cfg_set(syslog_trap_cfg_set, "syslog_trap_cfg_set");
    reg_add_Trap_cfg_nvwrt(syslog_trap_cfg_nvwrt, "syslog_trap_cfg_nvwrt");
    parser_add_command_list(syslog_chain_init_table, "syslog");
}
      
