/* $Id: rtt_snmp_chain.c,v 1.1.4.2 1996/06/07 22:26:26 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_snmp_chain.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor snmp parse chain.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor snmp parse chain.
 *------------------------------------------------------------------
 * $Log: rtt_snmp_chain.c,v $
 * Revision 1.1.4.2  1996/06/07  22:26:26  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:44  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:56  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:50  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:36  lmetzger
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
#include "../parser/parser_defs_snmp.h"


/*
 *  Parse Chain for "snmp-server host"
 */
LINK_EXIT(cfg_snmp_host_rtt_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_rtt,
	   cfg_snmp_host_rtt_exit, NONE,
	   OBJ(int, 1), (1 << TRAP_RTT),
	   D_rttMon_string,
	   "Allow SNMP Response Time Reporter traps",
	   PRIV_CONF);

LINK_POINT(cfg_snmp_host_rtt_entry, cfg_snmp_host_rtt);


/*
 *  Parse Chain for "snmp-server enable"
 */
LINK_EXIT(cfg_snmp_enable_rtt_exit, no_alt);
KEYWORD_OR(cfg_snmp_enable_rtt,
	   cfg_snmp_enable_rtt_exit, NONE,
	   OBJ(int, 1), (1 << TRAP_RTT),
	   D_rttMon_string,
	   "Enable SNMP Response Time Reporter traps",
	   PRIV_CONF);

LINK_POINT(cfg_snmp_enable_rtt_entry, cfg_snmp_enable_rtt);



/*
 * Parse chain registration array for RTTMON
 */
static parser_extension_request rttmon_snmp_chain_init_table[] =
{
    {PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_rtt_entry)},
    {PARSE_ADD_CFG_SNMP_HOST_EXIT,
     (dynamic_transition *) & pname(cfg_snmp_host_rtt_exit)},
    {PARSE_ADD_CFG_SNMP_ENABLE_CMD,
     &pname(cfg_snmp_enable_rtt_entry)},
    {PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
     (dynamic_transition *) & pname(cfg_snmp_enable_rtt_exit)},
    {PARSE_LIST_END, NULL}
};



/*
 * rttmon_snmp_chain_init - Initialize RTTMON snmp parser support
 */
void rttmon_snmp_chain_init (void)
{
    parser_add_command_list(rttmon_snmp_chain_init_table, D_rttMon_string);
}
