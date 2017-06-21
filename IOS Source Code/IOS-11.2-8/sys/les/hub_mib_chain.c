/* $Id: hub_mib_chain.c,v 3.4.10.1 1996/05/21 09:56:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/hub_mib_chain.c,v $
 *------------------------------------------------------------------
 * Hub parse chains for SNMP MIB commands
 *
 * December 1995, Sandra Durham
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hub_mib_chain.c,v $
 * Revision 3.4.10.1  1996/05/21  09:56:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1996/02/22  14:35:53  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1996/02/01  06:06:40  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/12/17  18:32:43  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/12/16  00:14:16  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
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
#include "../les/parser_defs_hub.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"
#include "../les/sr_rptrmib.h"


/*
 * parse chain to add snmp trap illegal-address to hub commands
 */

#define	ALTERNATE NONE
#include "cfg_hub_snmp-trap.h"
LINK_POINT(hub_mib_trap_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Hub MIB  snmp-server host command
 */

LINK_EXIT(cfg_snmp_host_hub_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_hub, cfg_snmp_host_hub_exit, NONE,
           OBJ(int,1), (1<<TRAP_REPEATER),
           "repeater", "Allow hub mib related traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_hub_entry, cfg_snmp_host_hub);




/******************************************************************
 * Parse chains for HUB  MIBs  snmp-server enable command
 *
 * [no] snmp-server enable {trap | inform} [snmp | repeater | ...]
 *      [trap-number | trap-name_string | ...]
 *
 * for hub :
 *       [health | reset]
 *
 ******************************************************************/

LINK_EXIT(cfg_snmp_enable_hub_exit, no_alt);

pdecl(cfg_snmp_enable_hub_opts);

KEYWORD_ORTEST(cfg_snmp_enable_hub_reset, cfg_snmp_enable_hub_opts,
               NONE,
               OBJ(int,2), (1<<TRAP_REPEATER_RESET),
               "reset", "Enable SNMP IETF HUB MIB reset traps", PRIV_CONF);
KEYWORD_ORTEST(cfg_snmp_enable_hub_health, cfg_snmp_enable_hub_opts,
               cfg_snmp_enable_hub_reset,
               OBJ(int,2), (1<<TRAP_REPEATER_HEALTH),
               "health", "Enable SNMP IETF HUB MIB health traps", PRIV_CONF);
NOP     (cfg_snmp_enable_hub_opts, cfg_snmp_enable_hub_health,
         cfg_snmp_enable_hub_exit);

KEYWORD_OR(cfg_snmp_enable_hub, cfg_snmp_enable_hub_opts,NONE,
           OBJ(int,1), (1<<TRAP_REPEATER),
           "repeater", "Enable SNMP hub mib traps", PRIV_CONF);
LINK_POINT(cfg_snmp_enable_hub_entry, cfg_snmp_enable_hub);

/******************************************************************
 * Parse chains for HUB  MIBs  snmp-server trap-frequency command
 *
 * [no] snmp-server trap-frequency {decay } [repeater | ...]
 *      [trap-number | trap-name_string | ...]
 *
 * for hub :
 *       [illegal-address]
 *
 * WARNING!!! This command is ONLY for the trap decay feature for
 * the MAC Security illegal address trap! This  should not 
 * be duplicated ANYWHERE FOR ANY REASON without prior approval
 * from the mib police...in triplicate!  This has only be added here
 * as a hidden command to get the associated object saved over a reboot!
 *
 ******************************************************************/

LINK_EXIT(cfg_snmp_trap_freq_hub_exit, no_alt);

pdecl(cfg_snmp_trap_freq_hub_opts);

KEYWORD_ORTEST(cfg_snmp_trap_freq_hub_ill_addr, cfg_snmp_trap_freq_hub_opts,
               NONE,
               OBJ(int,2), (1<<    TRAP_REPEATER_ILLEGAL_ADDRESS),
               "illegal-address", "set illegal-address trap frequency", PRIV_CONF | PRIV_HIDDEN);
NOP     (cfg_snmp_trap_freq_hub_opts, cfg_snmp_trap_freq_hub_ill_addr,
         cfg_snmp_trap_freq_hub_exit);

KEYWORD_OR(cfg_snmp_trap_freq_hub, cfg_snmp_trap_freq_hub_opts,NONE,
           OBJ(int,1), (1<<TRAP_REPEATER),
           "repeater", "set SNMP hub mib traps frequency",PRIV_CONF|PRIV_HIDDEN);
LINK_POINT(cfg_snmp_trap_freq_hub_entry, cfg_snmp_trap_freq_hub);

static parser_extension_request hub_mib_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_hub_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
          (dynamic_transition *) &pname(cfg_snmp_host_hub_exit) },
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD,
	  &pname(cfg_snmp_enable_hub_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_enable_hub_exit) },
    { PARSE_ADD_CFG_SNMP_TRAP_FREQ_CMD,
	  &pname(cfg_snmp_trap_freq_hub_entry) },
    { PARSE_ADD_CFG_SNMP_TRAP_FREQ_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_trap_freq_hub_exit) },
    { PARSE_ADD_CFG_HUB_PORT_CMD, &pname(hub_mib_trap_commands) },
    { PARSE_LIST_END, NULL }
};

static void hub_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_REPEATER), " repeater");
}

void hub_mib_parser_init (void)
{
    parser_add_command_list(hub_mib_chain_init_table, "hubmib");
    reg_add_snmp_host_nvgen(hub_snmp_host_nvgen,
			    "hub_snmp_host_nvgen");
}
      
