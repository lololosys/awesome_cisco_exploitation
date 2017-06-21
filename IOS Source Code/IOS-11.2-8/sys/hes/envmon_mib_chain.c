/* $Id: envmon_mib_chain.c,v 3.3.10.1 1996/05/21 09:47:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/envmon_mib_chain.c,v $
 *------------------------------------------------------------------
 * Environmental Monitor parse chains for SNMP mib support
 *
 * November, 1995 Sandra Durham
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: envmon_mib_chain.c,v $
 * Revision 3.3.10.1  1996/05/21  09:47:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1996/02/22  14:33:13  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.2  1995/12/17  18:27:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/11/22  23:43:30  sdurham
 * CSCdi35282:  Need command to configure envmon trap enables
 *        ooph! forgot to cvs add this file!
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "registry.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_snmp.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"
#include "../hes/sr_envmib.h"

/*
 * Parse chains for Environmental Monitor snmp-server host command
 */

LINK_EXIT(cfg_snmp_host_envmon_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_envmon, cfg_snmp_host_envmon_exit, NONE,
           OBJ(int,1), (1<<TRAP_ENVMON),
           "envmon", "Allow environmental monitor traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_envmon_entry, cfg_snmp_host_envmon);




/******************************************************************
 * Parse chains for ENVIRONMENTAL MONITOR MIB snmp-server enable command
 *
 * [no] snmp-server enable {trap | inform} [snmp | envmon | ...]
 *      [trap-number | trap-name_string | ...]
 *
 * for envmon :
 *       [shutdown] [voltage] [temperature] [fan] [supply]
 *
 ******************************************************************/

LINK_EXIT(cfg_snmp_enable_envmon_exit, no_alt);

pdecl(cfg_snmp_enable_envmon_opts);

KEYWORD_ORTEST(cfg_snmp_enable_envmon_volt, cfg_snmp_enable_envmon_opts,
               NONE,
               OBJ(int,2), (1<<TRAP_ENVMON_VOLT),
               "voltage", "Enable SNMP environmental monitor voltage traps", PRIV_CONF);
KEYWORD_ORTEST(cfg_snmp_enable_envmon_shutdown, cfg_snmp_enable_envmon_opts,
               cfg_snmp_enable_envmon_volt,
               OBJ(int,2), (1<<TRAP_ENVMON_SHUTDOWN),
               "shutdown", "Enable SNMP environmental monitor shutdown traps", PRIV_CONF);

KEYWORD_ORTEST(cfg_snmp_enable_envmon_temp, cfg_snmp_enable_envmon_opts,
               cfg_snmp_enable_envmon_shutdown,
               OBJ(int,2), (1<<TRAP_ENVMON_TEMP),
               "temperature", "Enable SNMP environmental monitor temperature traps", PRIV_CONF);
KEYWORD_ORTEST(cfg_snmp_enable_envmon_fan, cfg_snmp_enable_envmon_opts,
               cfg_snmp_enable_envmon_temp,
               OBJ(int,2), (1<<TRAP_ENVMON_FAN),
               "fan", "Enable SNMP envmon fan traps", PRIV_CONF);
KEYWORD_ORTEST(cfg_snmp_enable_envmon_supply, cfg_snmp_enable_envmon_opts,
               cfg_snmp_enable_envmon_fan,
               OBJ(int,2), (1<<TRAP_ENVMON_SUPPLY),
               "supply", "Enable SNMP environmental monitor supply traps", PRIV_CONF);
NOP     (cfg_snmp_enable_envmon_opts, cfg_snmp_enable_envmon_supply,
         cfg_snmp_enable_envmon_exit);

KEYWORD_OR(cfg_snmp_enable_envmon, cfg_snmp_enable_envmon_opts,NONE,
           OBJ(int,1), (1<<TRAP_ENVMON),
           "envmon", "Enable SNMP environmental monitor traps", PRIV_CONF);
LINK_POINT(cfg_snmp_enable_envmon_entry, cfg_snmp_enable_envmon);

static parser_extension_request envmon_mib_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_envmon_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
          (dynamic_transition *) &pname(cfg_snmp_host_envmon_exit) },
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD,
	  &pname(cfg_snmp_enable_envmon_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_enable_envmon_exit) },
    { PARSE_LIST_END, NULL }
};

static void envmon_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_ENVMON), " envmon");
}

void envmon_mib_parser_init (void)
{
    parser_add_command_list(envmon_mib_chain_init_table, "envmib");
    reg_add_snmp_host_nvgen(envmon_snmp_host_nvgen,
			    "envmon_snmp_host_nvgen");
}
      
