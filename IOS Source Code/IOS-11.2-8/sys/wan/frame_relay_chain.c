/* $Id: frame_relay_chain.c,v 3.11.10.9 1996/08/12 16:09:41 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/frame_relay_chain.c,v $
 *------------------------------------------------------------------
 * Frame Relay parse chains
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: frame_relay_chain.c,v $
 * Revision 3.11.10.9  1996/08/12  16:09:41  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.11.10.8  1996/07/03  01:57:21  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.11.10.7  1996/06/17  23:39:18  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.10.6  1996/05/30  23:46:41  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.11.10.5  1996/05/29  21:07:16  rzagst
 * CSCdi58657:  map-class command broken
 * Branch: California_branch
 *
 * Revision 3.11.10.4  1996/05/21  10:09:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.10.3  1996/05/15  05:55:04  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.11.10.2  1996/04/27  06:36:41  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.11.10.1  1996/04/25  23:24:24  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.10.2  1996/04/17  00:01:05  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.10.1  1996/02/06  20:30:16  perryl
 * Branch: Dial1_branch
 *
 * Revision 3.11  1996/02/22  14:39:56  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.10  1996/02/15  20:59:23  chifei
 * CSCdi48715:  Frame Relay prompts for RFC 1294 should be RFC1490
 *
 * Revision 3.9  1996/02/01  06:12:11  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8  1996/01/29  07:31:16  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.7  1996/01/25  11:23:26  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.6  1995/12/17  18:41:15  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/11/27  16:24:00  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.4  1995/11/21  20:57:09  sdurham
 * CSCdi43966:  frame-relay misspelled in cli commands for snmp trap
 *    controls  migrated both snmp commands to frame-relay.  leave
 *    framerelay as  hidden option
 *
 * Revision 3.3  1995/11/18  00:22:51  jjohnson
 * CSCdi43548:  need mechanism to enable multiple traps for a subsystem
 *
 * Revision 3.2  1995/11/17  18:02:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:46:12  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:18:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../h/interface_private.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "packet.h"
#include "../wan/frame_relay.h"
#include "../wan/fr_lmi.h"
#include "../wan/fr_debug.h"
#include "../wan/fr_class.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_frame.h"
#include "../parser/parser_defs_snmp.h"
#include "../atalk/atalk.h"
#include "../xns/novell.h"
#include "../vines/vines.h"
#include "../h/access.h"
#include "../ip/ip_actions.h"
#include "../ip/ip_macros.h"
#include "registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"

parser_mode *fr_dlci_mode;

LINK_EXIT(after_fr_dlci_exit, no_alt);

NVCOMMENT(after_fr_dlci_comment, after_fr_dlci_exit);
#define	ALTERNATE	after_fr_dlci_comment

#include "cfg_fr_dlci_exit.h"

EOLNS	(after_fr_dlci, fr_dlci_exit_command);
LINK_TRANS(cfg_fr_dlci_extend_here, after_fr_dlci);
 
NO_OR_DEFAULT(config_fr_sub_dlci_no, cfg_fr_dlci_extend_here,
	   PRIV_CONF | PRIV_NONVGEN);

HELP	(top_config_fr_dlci, config_fr_sub_dlci_no,
	 "Frame Relay dlci configuration commands:\n");

#undef ALTERNATE 

/*
 * Parse chains for Frame Relay debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_frame-relay.h"
LINK_POINT(frame_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Frame Relay show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_frame-relay.h"
LINK_POINT(frame_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Frame Relay config commands
 */
#define	ALTERNATE	NONE
#include "cfg_frame-relay.h"
LINK_POINT(frame_config_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Frame Relay interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_frame-relay.h"
LINK_POINT(frame_interface_commands, ALTERNATE);
#undef	ALTERNATE



/*
 * Parse chains for Frame Relay interface commands
 */
#define	ALTERNATE	NONE
#include "../if/cfg_mapclass_fr.h"
LINK_POINT(frame_mapclass_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Frame Relay snmp-server host command
 */
LINK_EXIT(cfg_snmp_host_framerelay_exit, no_alt);
/*
 * migrated to frame-relay in 11.0, but leave framerelay as hidden option
 */
KEYWORD_OR(cfg_snmp_host_framerelay_hidden,
	   cfg_snmp_host_framerelay_exit, NONE,
	   OBJ(int,1), (1<<TRAP_FRAMERELAY),
	   "framerelay", "Allow SNMP framerelay traps", PRIV_HIDDEN);
KEYWORD_OR(cfg_snmp_host_framerelay, cfg_snmp_host_framerelay_exit, cfg_snmp_host_framerelay_hidden,
	   OBJ(int,1), (1<<TRAP_FRAMERELAY),
	   "frame-relay", "Allow SNMP frame-relay traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_framerelay_entry, cfg_snmp_host_framerelay);

static void framerelay_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_FRAMERELAY), " frame-relay");
}


/******************************************************************
 * Parse chains for Frame Relay snmp-server enable command
 *
 * [no] snmp-server enable {trap | inform} [snmp | framerelay | ...]
 *      [trap-number | trap-name_string | ...]
 *
 * Currently Frame-relay has no need to specify the trap ID since the
 * current implementation involves only one trap object.
 *
 ******************************************************************/

LINK_EXIT(cfg_snmp_enable_framerelay_exit, no_alt);

/* fix frame-relay here, and make framrelay option hidden for backward
 * compatibility
 */
KEYWORD_OR(conf_snmp_enable_framerelay_hidden, 
	   cfg_snmp_enable_framerelay_exit, NONE,
           OBJ(int,1), (1<<TRAP_FRAMERELAY),
           "framerelay", "Enable SNMP framerelay traps", PRIV_HIDDEN);
KEYWORD_OR(conf_snmp_enable_framerelay,
	   cfg_snmp_enable_framerelay_exit, 
	   conf_snmp_enable_framerelay_hidden, 
           OBJ(int,1), (1<<TRAP_FRAMERELAY),
           "frame-relay", "Enable SNMP frame-relay traps", PRIV_CONF);

LINK_POINT(cfg_snmp_enable_framerelay_entry, conf_snmp_enable_framerelay);


/*
 * Parse chain for frame-relay interface encapsulation command
 */
LINK_EXIT(cfg_encap_fr_exit, no_alt);
KEYWORD_ID(cfg_encap_fr_ietf, cfg_encap_fr_exit, cfg_encap_fr_exit,
	   OBJ(int,1), ET_FR_IETF,
	   "ietf", "Use RFC1490 encapsulation", PRIV_CONF|PRIV_NONVGEN);
ENCAPS	(cfg_encap_fr, cfg_encap_fr_ietf, NONE,
	 OBJ(int,1), ET_FRAME_RELAY,
	 "frame-relay", "Frame Relay networks", 
	 (IDB_SYNC | IDB_ISDN | IDB_DIALER | IDB_VTEMPLATE));
LINK_POINT(cfg_encap_fr_cmd, cfg_encap_fr);


/*
 * Parse chain registration array for Frame Relay commands
 */
static parser_extension_request frame_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(frame_debug_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(frame_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(frame_config_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(frame_interface_commands) },
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_framerelay_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_framerelay_exit) },
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD, &pname(cfg_snmp_enable_framerelay_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_enable_framerelay_exit) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(cfg_encap_fr_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_fr_exit) },
    { PARSE_ADD_CFG_MAPCLASS_CMD, &pname(frame_mapclass_commands) },
    { PARSE_ADD_GLOBAL_EXIT,
	  (dynamic_transition *) &pname(after_fr_dlci_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * frame_relay_parser_init - Initialize parse structures for
 *                           parsing frame relay commands
 */
void frame_relay_parser_init (void)
{
    parser_add_command_list(frame_chain_init_table, "frame-relay");
    parser_add_link_point(PARSE_ADD_FR_DELIST_OPTIONS,
 			  "frame-relay de-list options",
			  &pname(cfg_fr_delist_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_INARP_CMD,
			  "frame-relay inverse ARP options",
			  &pname(cfg_fr_inarp_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_VCQ_CMD,
			  "frame-relay vcq options",
			  &pname(cfg_fr_vcq_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_MAPCLASS_VCQ_CMD,
			  "frame-relay mapclass vcq options",
			  &pname(cmc_fr_vcqmap_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_MAPCLASS_SVC_CMD,
			  "frame-relay mapclass svc options",
			  &pname(cmc_fr_svcmap_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_FRCLASS_CMD,
			  "frame-relay interface-dlci options",
			  &pname(cfg_fr_dlci_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_SVC_CMD,
			  "frame-relay svc options",
			  &pname(ci_fr_svc_extend_here));
    parser_add_link_point(PARSE_ADD_CFG_INT_LAPF_CMD,
			  "frame-relay lapf options",
			  &pname(ci_fr_lapf_extend_here));
    fr_dlci_mode = parser_add_mode("interface-dlci", "config-fr-dlci",
				   "Frame Relay dlci configuration mode",
				   TRUE, TRUE, "interface", NULL, NULL,
				   &pname(top_config_fr_dlci),
				   NULL);

    if (registry_populated(REG_SNMP))
	reg_add_snmp_host_nvgen(framerelay_snmp_host_nvgen,
				"framerelay_snmp_host_nvgen");
    
}
      
