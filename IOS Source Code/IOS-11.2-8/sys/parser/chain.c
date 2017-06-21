/* $Id: chain.c,v 3.13.4.16 1996/09/05 13:59:43 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/chain.c,v $
 *------------------------------------------------------------------
 * C H A I N . C
 *
 * C file into which the token macros, action routines, and token chains
 * are built.
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chain.c,v $
 * Revision 3.13.4.16  1996/09/05  13:59:43  widmer
 * CSCdi67994:  Remove names from parser link points
 * Branch: California_branch
 *
 * Revision 3.13.4.15  1996/08/12  16:05:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.13.4.14  1996/07/25  21:52:27  widmer
 * CSCdi52618:  Hold-queue interface subcommand not preserved after reload
 * Branch: California_branch
 * nvgen hold-queue command after fair-queue command
 *
 * Revision 3.13.4.13  1996/07/12  02:01:26  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.13.4.12  1996/06/19  23:17:37  smackie
 * The chunk manager displays it's new look for 1996. (CSCdi60779)
 *
 *   o Soothe the raging masses and implement lockable chunks
 *   o Start hiding chunk structures and defines in a private header
 *   o Add a chain file for "show chunk" and remove entry from chain.c
 *   o Tidy up some redundant sections of code
 *
 * Branch: California_branch
 *
 * Revision 3.13.4.11  1996/06/17  23:35:45  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.13.4.10  1996/06/17  08:35:20  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.13.4.9  1996/06/16  21:17:42  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.13.4.8  1996/06/13  19:08:54  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.13.4.7  1996/05/21  09:59:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.4.6  1996/05/17  11:39:21  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.4  1996/05/05  23:41:39  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.3  1996/04/19  02:40:50  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.11.2.2  1996/04/03  21:12:49  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.1  1996/03/17  18:19:09  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.13.4.5  1996/04/24  01:33:26  widmer
 * CSCdi52164:  CSCdi38824 Regressed in 11.0(4) (terminal download)
 * Branch: California_branch
 *
 * Revision 3.13.4.4  1996/04/16  19:11:43  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.13.4.3  1996/04/15  14:59:29  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.13.4.2  1996/03/21  23:22:10  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.13.4.1  1996/03/18  21:33:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.4  1996/03/16  07:22:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.14.3  1996/03/09  05:11:03  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.14.2  1996/02/20  16:44:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/03/09  22:56:44  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.12  1996/02/28  00:40:39  ronnie
 * CSCdi49916:  u no longer an alias for undebug
 *
 * Revision 3.11  1996/02/16  00:48:47  hampton
 * Migrate files out of the parser directory.  [CSCdi49139]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/02/15  18:02:57  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.9  1996/02/13  21:25:29  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.8  1996/02/13  08:15:28  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.7  1996/02/01  06:07:59  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6.14.1  1996/01/24  22:37:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1995/12/17  18:35:15  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/13  02:21:43  thille
 * CSCdi45499:  un is no longer a valid abbreviation for undebug
 * Add default alias "un" for "undebug".
 *
 * Revision 3.4  1995/12/01  15:56:58  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.3  1995/11/27  16:23:51  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  18:45:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:20:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/25  08:36:23  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.6  1995/08/25  15:26:15  widmer
 * CSCdi38824:  terminal download does not play nice with privleges.
 * Enable changing privilege of terminal only commands.
 *
 * Revision 2.5  1995/08/15  23:24:18  tli
 * CSCdi36981:  Need to be able to generate downward compatible
 * configurations
 *
 * Revision 2.4  1995/06/27  00:30:36  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.3  1995/06/15  15:19:17  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.2  1995/06/15  03:42:25  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.1  1995/06/07  22:15:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include <stdlib.h>		/* for atoi */
#include "boxtypes.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "parser_debug.h"
#include "parser_actions.h"
#include "macros.h"
#include "access.h"
#include "interface_private.h"
#include "name.h"
#include "ttysrv.h"
#include "address.h"
#include "packet.h"
#include "logger.h"
#include "login.h"
#include "file.h"
#include "../os/free.h"
#include "../os/profile.h"
#include "../os/boot.h"
#include "../if/network.h"
#include "../les/isdn_bri.h"
#include "../ui/service.h"
#include "../if/network_debug.h"
#include "../os/os_debug.h"
#include "../pt/translate.h"
#include "../tcp/rcp.h"
#include "../if/atm.h"
#include "../parser/parser_modes.h"
#include "../parser/parser_privilege.h"
#include "../os/parser_defs_buffers.h"
#include "parser_defs_config.h"
#include "parser_defs_exec.h"
#include "parser_defs_misc.h"
#include "parser_defs_parser.h"
#include "parser_defs_service.h"
#include "../les/parser_defs_csu_dsu.h"
#include "../tcp/http.h"
#include "../ip/ip_registry.h"
#include "parser_defs_half_duplex.h"
#include "../les/parser_defs_xx.h"
#include "../if/parser_defs_fast_ethernet.h"
#include "../wan/dialer.h"
#include "../wan/serial.h"
#include "../iprouting/igrp.h"
#include "../os/techsupport.h"
#include "../os/nv.h"

const char parser_help_default_keyword[] = "Set a command to its defaults";
const char parser_help_no_keyword[] = "Negate a command or set its defaults";
const char parser_default_keyword[] = "default";
const char parser_no_keyword[] = "no";

/*  Function for emitting comments about last config and NV update times */
static void write_config_times (parseinfo *csb)
{
    char *string;
    boolean somestring = FALSE;
    if (csb->nvgen && !csb->priv_set) {
	string = last_config_time_string();
	somestring = somestring || (string != NULL);
	nv_write((string != NULL), string);
	string = last_write_mem_time_string();
	somestring = somestring || (string != NULL);
	nv_write((string != NULL), string);
	nv_write(somestring, "!");
    }
}


/* Global parser items */

transition pname(NONE) = {NULL, NULL, (trans_func) NONE_action, NULL};
transition pname(no_alt) = {NULL, NULL, (trans_func) no_alt_action, NULL};
transition pname(link_point) = {NULL, NULL, (trans_func) link_point_action, NULL};
transition pname(link_trans) = {NULL, NULL, (trans_func) NONE_action, NULL};


/*
 *            *** Notice to protocol implementors ***
 *
 * The current scheme has protocol related information showing up in
 * three distinct places in non-volatile memory.  These places are the
 * system globals section, the interface subcommand section, and the
 * dynamic and static routing section.  Please be sure to put new
 * protocols into the same point in the ordering in each section.  The
 * current ordering is:
 *
 *    1)  IP
 *    2)  DECnet
 *    3)  XNS
 *    4)  Novell
 *     )  SMDS
 *    5)  Apollo
 *    6)  Appletalk
 *    7)  CLNS
 *    8)  Vines
 *    9)  Source Route Bridging
 *     )  STUN
 *     )  BSTUN
 *     )  Netbios
 *     )  Lanmgr
 *   10)  X.25 
 *   11)  LAT
 *   12)  NTP
 */

/*********************************************************
 * Clear commands.
 * Linkage to here is from exec_commands/clear.h
 */
LINK_TRANS(exec_clear_extend_here, no_alt);

#undef		ALTERNATE
#define		ALTERNATE	exec_clear_extend_here

#include        "exec_clear_startup-config.h"
#include	"exec_clear_host.h"
#include	"exec_clear_counters.h"
#include        "exec_clear_profile.h"

NOP	(exec_clear_commands, ALTERNATE, NONE);

/*********************************************************
 * Debug commands.
 * Linkage to here is from exec_commands/debug.h
 */
LINK_TRANS(exec_debug_extend_here, no_alt);

#undef		ALTERNATE
#define		ALTERNATE	exec_debug_extend_here

#include	"exec_debug_parser.h"
#include	"exec_debug_packet.h"
#include	"exec_debug_broadcast.h"
#include	"exec_debug_all.h"
#include	"exec_debug_list.h"
#include        "exec_debug_modem.h"
#include        "exec_debug_nvram.h"

NOP	(exec_debug_commands, ALTERNATE, NONE);

/*********************************************************
 * Show commands.
 * Linkage to here is from exec_commands/show.h
 */
LINK_TRANS(exec_show_extend_here, no_alt);

#undef		ALTERNATE
#define		ALTERNATE	exec_show_extend_here

#include	"exec_show_sum.h"
#include	"exec_show_stacks.h"
#include        "exec_show_profile.h"
#include	"exec_show_privilege.h"
#include	"exec_show_parser.h"
#include	"../os/exec_show_location.h"
#include	"exec_show_interfaces.h"
#include	"exec_show_hosts.h"
#include	"exec_show_history.h"
#include        "exec_show_extended.h"
#include	"exec_show_coverage.h"
#include	"exec_show_configuration.h"
#include	"exec_show_alias.h"

NOP	(exec_show_commands, ALTERNATE, NONE);

/*********************************************************
 *	Global commands that exist at all levels
 *	These always terminate at 'no_alt' and therefore
 *	must be referenced from the terminal states in
 *	each of the command sets.  
 *
 *	None of the global commands
 *	should have a 'no' prefix format.
 */

LINK_TRANS	(global_extend_here, NONE);
TESTVAR	(globalcmd_start_no, NONE, global_extend_here,
	 NONE, NONE, NONE, sense, FALSE);
TESTVAR	(globalcmd_start, globalcmd_start_no, NONE,
	 NONE, NONE, no_alt, nvgen, FALSE);

/*********************************************************
 *	start of configure line commands
 *
 * The line configuration and terminal characteristic modification
 * commands share several common commands.  They are broken
 * out here into separate sets:
 * 1. Terminal only commands
 * 2. Line only commands
 * 3. Common commands
 * Since the csb->permanent flag is set only when doing
 * line configuration, we can use it to select which commands
 * are visible in each context.
 */

LINK_TRANS	(cfg_line_term_extend_here, globalcmd_start);
#undef		ALTERNATE
#define		ALTERNATE	cfg_line_term_extend_here

/* Line commands available for either configuration
 * or for dynamic user modification.
 */
#include	"cfg_line_help.h"
#include	"cfg_line_editing.h"
#include	"cfg_line_history.h"
#include	"cfg_line_notify.h"
#include	"cfg_line_width.h"
#include	"cfg_line_length.h"
#include        "cfg_line_dns.h"
#include	"cfg_line_specialmask.h"
#include	"cfg_line_execmask.h"
#include	"cfg_line_datamask.h"
#include	"cfg_line_terminaltype.h"
NOP	(cfg_line_common, ALTERNATE, NONE);

/* End of common line commands */

/* Line commands used only for dynamic modification of a
 * user's login line and should never appear in a config
 * file. (csb->permanent == FALSE)
 */
LINK_TRANS	(cfg_term_extend_here, NONE);
NOP	(cfg_line_terminal, cfg_term_extend_here, NONE);

/* End of login line dynamic modifications. */

/* Line commands from here down are configuration-only commands.
 * They are not available to users for customizing their login line.
 * Control transitions from here to cfg_line_term_common.
 */ 
LINK_TRANS	(cfg_line_extend_here, NONE);
#undef		ALTERNATE
#define		ALTERNATE	cfg_line_extend_here
#include	"cfg_line_exit.h"
#include	"cfg_line_privilege.h"
#include	"cfg_line_exec-time.h"
#include	"cfg_line_exec-banner.h"
#include        "cfg_line_motd-banner.h"
#include	"cfg_line_exec.h"
#include	"cfg_line_access.h"
#include	"cfg_line_location.h"
#include	"cfg_line_sessiontimeout.h"
NOP	(cfg_line_only, ALTERNATE, NONE);
/* End of line configuration-only commands */


/* If csb->permanent is TRUE, then we're doing line configuration */
TEST_EXPR(cfg_line_permflag, cfg_line_terminal, cfg_line_only, cfg_line_common,
	  csb->permanent == FALSE);

NO_OR_DEFAULT(configline_no, cfg_line_permflag, PRIV_MIN | PRIV_NONVGEN);
HELP	(top_config_line, configline_no,
	 "Line configuration commands:\n");

/*********************************************************
 *	start of configure interface commands
 */

#undef		ALTERNATE
#define		ALTERNATE	globalcmd_start

#include	"cfg_int_exit.h"
#include	"cfg_int_pulse-time.h"
#include	"cfg_int_netbios.h"
#include	"cfg_int_hold-queue.h"

LINK_TRANS	(cfg_int_as2_extend_here, ALTERNATE);
LINK_TRANS	(cfg_int_as_extend_here, cfg_int_as2_extend_here);
LINK_TRANS	(cfg_int_extend_here, cfg_int_as_extend_here);
#undef		ALTERNATE
#define		ALTERNATE	cfg_int_extend_here

#include        "cfg_int_half_duplex.h"
#include        "../if/cfg_int_media.h"
#include        "cfg_int_tx-queue-limit.h"
#include	"cfg_int_shutdown.h"
#include	"cfg_int_keepalive.h"
#include	"cfg_int_loopback.h"
#include	"cfg_int_delay.h"
#include	"cfg_int_carrier-delay.h"
#include	"cfg_int_load-interval.h"
#include	"cfg_int_bandwidth.h"
#include        "../os/cfg_int_logging.h"
LINK_TRANS(cfg_int_ip2_extend_here, ALTERNATE);
#undef	ALTERNATE
#define	ALTERNATE	cfg_int_ip2_extend_here
#include	"cfg_int_encapsulation.h"
LINK_TRANS(cfg_int_ip1_extend_here, ALTERNATE);
#undef	ALTERNATE
#define	ALTERNATE	cfg_int_ip1_extend_here
#include	"cfg_int_backup.h"
#include	"cfg_int_transmit-interface.h"
#include	"cfg_int_mtu.h"
#include	"cfg_int_mac-address.h"
#include	"cfg_int_description.h"
#include	"cfg_int_physical_layer.h"


NO_OR_DEFAULT(configif_no, ALTERNATE, PRIV_MIN | PRIV_SUBIF | PRIV_NONVGEN);
HELP	(top_config_interface, configif_no,
	 "Interface configuration commands:\n");


/*********************************************************
 *	configure commands from here down
 */

#undef		ALTERNATE
#define		ALTERNATE	globalcmd_start

#include	"cfg_exit.h"
#include	"cfg_end.h"
LINK_TRANS(cfg_extend_last_here, ALTERNATE);
#undef	ALTERNATE
#define ALTERNATE	cfg_extend_last_here
#include	"cfg_exception.h"
#include        "cfg_slave_exception.h"

NVCOMMENT(after_line_comment, ALTERNATE);
EVAL	(finish_line_commands, after_line_comment, nv_line_end());
#undef	ALTERNATE
#define ALTERNATE	finish_line_commands
#include	"cfg_line.h"
#include	"cfg_defaultval.h"
#include	"cfg_privilege.h"
#include	"cfg_alias.h"
#include	"cfg_banner.h"

/*
 *  Response Time Reporter
 */
LINK_TRANS      (cfg_rttmon_base_extend_here, ALTERNATE);

/*
 *  Start of static routing commands
 */
LINK_TRANS	(cfg_routing_extend_here, cfg_rttmon_base_extend_here);
LINK_TRANS      (cfg_mapclass_command_here, cfg_routing_extend_here);
LINK_TRANS      (cfg_maplist_command_here, cfg_mapclass_command_here);
LINK_TRANS      (cfg_ip_routing_extend_here, cfg_maplist_command_here);
LINK_TRANS	(cfg_router_command_here, cfg_ip_routing_extend_here);
LINK_TRANS	(cfg_before_router_command_here, cfg_router_command_here);

NVCOMMENT(after_interface_comment, cfg_before_router_command_here);
#undef	ALTERNATE
#define ALTERNATE	after_interface_comment

#include	"cfg_interface.h"

/* The below list is in a protocol specific order.
 * See the introductory comment at the top of this file.
 */
LINK_TRANS(cfg_hub_command_extend_here, ALTERNATE);
LINK_TRANS(cfg_after_srb_extend_here, cfg_hub_command_extend_here);
LINK_TRANS(cfg_ac_extend_here, cfg_after_srb_extend_here);
LINK_TRANS(cfg_controller_command_extend_here, cfg_ac_extend_here);
LINK_TRANS(cfg_extend_here, cfg_controller_command_extend_here);
#undef	ALTERNATE
#define ALTERNATE	cfg_extend_here
#include	"cfg_netbios.h"
#include	"cfg_boot.h"
#include	"cfg_hostname.h"
#include	"cfg_profile.h"
#include	"cfg_coverage.h"
#include	"cfg_service.h"
#include	"../ui/cfg_downward.h"
#include	"cfg_version.h"

EVAL	(config_times, ALTERNATE, write_config_times(csb));
NVCOMMENT(first_comment, config_times);
NO_OR_DEFAULT(config_no, first_comment, PRIV_MIN | PRIV_NONVGEN);
HELP	(top_configure, config_no, "Configure commands:\n");


/*********************************************************
 * Exec Commands
 *
 * If we're enabled, then include the priv_commands, otherwise, go
 * to the global command set.
 *
 */

LINK_TRANS(exec_extend_here, globalcmd_start);

#undef	ALTERNATE
#define ALTERNATE	exec_extend_here

#include	"exec_undebug.h"
#include	"exec_test.h"
#include	"exec_terminal.h"
#include	"exec_show.h"
#include        "exec_profile.h"
#include	"exec_lock.h" /* TROUTER_ENABLE */
#include	"exec_exit.h"
#include	"exec_debug.h"
#include	"exec_clear.h"

HELP	(top, ALTERNATE, "Exec commands:\n");


/*
 * NULL interface commands
 */
EOLNS    (null_exit_eol, exit_config_submode);
KEYWORD (null_exit, null_exit_eol, NONE, "exit",
         "Exit from interface configuration mode", PRIV_CONF|PRIV_SUBIF);
TESTVAR (null_exit_test, NONE, null_exit, NONE,
	 NONE, globalcmd_start, sense, FALSE);

LINK_TRANS(null_int_extend_here, null_exit_test);

NO_OR_DEFAULT(null_no, null_int_extend_here,
	   PRIV_MIN | PRIV_SUBIF | PRIV_NONVGEN);
HELP	(top_null_interface, null_no,
	 "Interface configuration commands:\n");


/*
 * string_1_mode commands
 */
static void string_1_command (parseinfo *csb)
{
    strcpy(csb->parse_line_pointer, GETOBJ(string,1));
    csb->exit_all = TRUE;
}

EOLNS	(string_1_eol, string_1_command);
STRING	(string_1_top, string_1_eol, string_1_eol,
	 OBJ(string,1), NULL);


#ifdef	PARSER_DEBUG_LINKS
#define	LINK(which,str,where,link)\
    {which,str,((dynamic_transition *)&pname(where)),link},
#else	/* PARSER_DEBUG_LINKS */
#define	LINK(which,str,where,link)\
    {which,((dynamic_transition *)&pname(where)),link},
#endif	/* PARSER_DEBUG_LINKS */


chain_link chain_gang[] = {
    LINK(PARSE_ADD_EXEC_CMD, "exec", exec_extend_here,
	  PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_SHOW_CMD, "show", exec_show_extend_here,
      PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CLEAR_CMD, "clear", exec_clear_extend_here,
      PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_DEBUG_CMD, "debug", exec_debug_extend_here,
      PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_TERM_CMD, "terminal", cfg_term_extend_here,
      PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_TEST_CMD, "test", exec_test_extend_here,
      PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_TOP_CMD, "configuration", cfg_extend_here,
      PARSER_ENTRY_LINK_POINT)
    /*
     *  PARSE_ADD_CFG_TOP_AC_CMD
     *  This link point is for adding commands to the top of the config file
     *  after the controller commands have appeared.  Because the controller
     *  commands create interface names, any command which references an
     *  interface name must appear no higher on the parse tree than this.
     */
    LINK(PARSE_ADD_CFG_TOP_AC_CMD, "configuration (after controllers)",
	  cfg_ac_extend_here, PARSER_ENTRY_LINK_POINT)
    /*
     *  PARSE_ADD_CFG_AFTER_SRB_CMD
     *  This link point is for adding commands to the top of the config file
     *  after the source-bridge commands have appeared.  
     */
    LINK(PARSE_ADD_CFG_AFTER_SRB_CMD, "configuration (after source-bridge)",
	  cfg_after_srb_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_HUB_CMD, "hub configuration command",
	  cfg_hub_command_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_INT_CMD, "interface configuration",
	  cfg_int_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_INT_AS_CMD, "interface configuration(2)",
	  cfg_int_as_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_INT_AS2_CMD, "interface configuration(after INT_AS_CMD)",
	  cfg_int_as2_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_LINE_CMD, "line configuration",
	  cfg_line_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_ROUTING_CMD, "routing configuration",
	  cfg_routing_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_LINE_TERM_CMD, "line_term",
	  cfg_line_term_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_ROUTER_COMMAND, "router",
	  cfg_router_command_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_GLOBAL_CMD, "global", global_extend_here,
      PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_INTERFACE_IP1_CMD, "ip interface",
	  cfg_int_ip1_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_INTERFACE_IP2_CMD, "ip2 interface",
	  cfg_int_ip2_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_ROUTING_IP_CMD, "routing ip command",
	  cfg_ip_routing_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_LAST_CMD, "end of top level configuration commands",
	  cfg_extend_last_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_NBIOS_CMD, "netbios config",
	  config_netbios_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_INT_NBIOS_CMD, "netbios interface",
	  ci_netbios_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_NULLINT_CMD, "null interface",
	  null_int_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_BEFORE_RTR_CMD, "config before router",
          cfg_before_router_command_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_MAPLIST, "map-list command",
          cfg_maplist_command_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_MAPCLASS, "map-class command",
          cfg_mapclass_command_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_CTRLLR_CMD, "controller command",
	  cfg_controller_command_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_SERVICE_CMD, "service command",
	  cfg_service_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_RTTMON_CMD, "response time reporter commands",
          cfg_rttmon_base_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_SHOW_EXTENDED_CMD, "show extended",
	  show_extended_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_BOOT_SYS_CMD, "boot sys",
		conf_boot_sys_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_BOOT_CONFIG_CMD, "boot config",
		conf_boot_config_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_GLOBAL_EXIT, "global exit",
	  globalcmd_start, PARSER_EXIT_LINK_POINT)
    LINK(PARSE_ADD_CFG_EXCEP_PROTO_CMD, "exception proto extend",
	  exception_proto_extend, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_CFG_EXCEP_PROTO_EXIT, "exception proto exit",
	  exception_proto_exit, PARSER_EXIT_LINK_POINT)
    LINK(PARSE_ADD_INT_ENCAPS_CMD, "encaps",
	  config_encap_extend_here, PARSER_ENTRY_LINK_POINT)
    LINK(PARSE_ADD_INT_ENCAPS_EXIT, "encaps exit",
	  config_encap_return, PARSER_EXIT_LINK_POINT)
    LINK(PARSE_ADD_DEBUG_MODEM_CMD, "debug modem",
          debug_modem_extend_here, PARSER_ENTRY_LINK_POINT)
    { PARSE_LIST_END,
#ifdef	PARSER_DEBUG_LINKS
	  NULL,
#endif	/* PARSER_DEBUG_LINKS */
	  NULL, 0}
};


/*
 * chain.c common mode information
 */

/*
 * Local variables
 */
parser_mode *exec_mode;
parser_mode *config_mode;
parser_mode *interface_mode;
parser_mode *router_mode;
parser_mode *line_mode;
parser_mode *subinterface_mode;
parser_mode *nullinterface_mode;
parser_mode *string_1_mode;

static boolean exec_http_init (parseinfo *csb)
{
    char *old_index = csb->http_state->uri_index;
    char *status = reg_invoke_http_get_uri_token(csb->http_state);

    if (status) {
	if (strcasecmp(status, "disabled") == 0) {
	    csb->priv = PRIV_USER;
	    SET_PRIV(stdio->privilege, PRIV_USER);
	} else if (strcasecmp(status, "enabled") != 0) {
	    /* stick token back */
	    csb->http_state->uri_index = old_index;
	}
    }
    return(TRUE);
}

static void *interface_save_var (parseinfo *csb)
{
    idbtype *idb = csb->interface;

    csb->interface = NULL;
    return(idb);
}

static void interface_reset_var (parseinfo *csb, void *var)
{
    csb->interface = var;
}

static boolean interface_http_init (parseinfo *csb)
{
    idbtype *idb;
    char *status = reg_invoke_http_get_uri_token(csb->http_state);

    if (status) {
	FOR_ALL_SWIDBS(idb) {
	    if (parser_http_debug) {
		buginf("\nComparing interface '%s' to '%s'",
		       idb->namestring, status);
	    }
	    if (reg_invoke_http_strcasecmp(idb->namestring, status) == 0) {
		csb->interface = idb;
		return(TRUE);
	    }
	}

	if (strcasecmp(status, nullidb->namestring) == 0) {
	    csb->interface = nullidb;
	    csb->mode = nullinterface_mode;
	    return(TRUE);
	}
    }

    reg_invoke_http_send_not_found(csb->http_state,
				   "\nInvalid interface");
    return(FALSE);
}

static boolean null_interface_http_init (parseinfo *csb)
{
    char *status = reg_invoke_http_get_uri_token(csb->http_state);

    if (!status) {
	return(FALSE);
    }

    if (strcasecmp(status, nullidb->namestring) == 0) {
	csb->interface = nullidb;
	return(TRUE);
    }

    return(FALSE);
}

static void *line_save_var (parseinfo *csb)
{
    int line = csb->line1;

    csb->line1 = 0;
    return((void *)line);
}

static void line_reset_var (parseinfo *csb, void *var)
{
    csb->line1 = (int)var;
}

static boolean line_http_init (parseinfo *csb)
{
    char *status = reg_invoke_http_get_uri_token(csb->http_state);

    if (!status) {
	return(FALSE);
    }

    csb->line1 = atoi(status);
    csb->line2 = csb->line1 + 1;
    csb->permanent = TRUE;
    return(TRUE);
}

static void show_techsupport_chain (parseinfo *csb)
{
    show_techsupport_command(csb, "show stacks");
    show_techsupport_command(csb, "show interfaces");
}

void parser_add_main_modes (void)
{
    exec_mode = parser_add_mode("exec", NULL, "Exec mode",
				TRUE, TRUE, NULL, NULL, NULL,
				&pname(top),
				exec_http_init);
    add_default_alias(exec_mode, "h", "help");
    add_default_alias(exec_mode, "lo", "logout");
    add_default_alias(exec_mode, "p", "ping");
    add_default_alias(exec_mode, "r", "resume");
    add_default_alias(exec_mode, "s", "show");
    add_default_alias(exec_mode, "u", "undebug");
    add_default_alias(exec_mode, "un", "undebug");
    add_default_alias(exec_mode, "w", "where");

    config_mode = parser_add_mode("configure", "config",
				  "Global configuration mode",
				  TRUE, TRUE, NULL, NULL, NULL,
				  &pname(top_configure),
				  NULL);
    interface_mode = parser_add_mode(common_str_interface, "config-if",
				     "Interface configuration mode",
				     TRUE, TRUE, "configure",
				     interface_save_var, interface_reset_var,
				     &pname(top_config_interface),
				     interface_http_init);
    subinterface_mode = parser_add_mode("subinterface", "config-subif",
					"Subinterface configuration mode",
					TRUE, FALSE, "configure",
					interface_save_var, interface_reset_var,
					&pname(top_config_interface),
					interface_http_init);
    nullinterface_mode = parser_add_mode("null-interface", "config-if",
					 "Null interface configuration mode",
					 TRUE, TRUE, "configure",
					 interface_save_var, interface_reset_var,
					 &pname(top_null_interface),
					 null_interface_http_init);
    line_mode = parser_add_mode("line", "config-line",
				"Line configuration mode",
				TRUE, TRUE, "configure",
				line_save_var, line_reset_var,
				&pname(top_config_line),
				line_http_init);

    string_1_mode = parser_add_mode("string_1", NULL, NULL,
				    FALSE, FALSE, NULL, NULL, NULL,
				    &pname(string_1_top), NULL);

    reg_add_show_techsupport(show_techsupport_chain,
			     "show_techsupport_chain");
}
