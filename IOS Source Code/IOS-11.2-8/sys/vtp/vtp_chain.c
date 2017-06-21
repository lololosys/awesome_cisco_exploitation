/* $Id: vtp_chain.c,v 1.2.58.6 1996/06/20 19:50:20 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_chain.c,v $
 *------------------------------------------------------------------
 * vtp_chain.c - CLI interface for VTP.
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_chain.c,v $
 * Revision 1.2.58.6  1996/06/20  19:50:20  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.2.58.5  1996/06/18  00:51:00  cyoung
 * CSCdi60069:  router crashes when writing vtp info to tftp file
 * CSCdi59372:  wrong behavior when conflicting vlan info is received
 * CSCdi59933:  RSP crashes for creating-deleting-recreating vtp-domains
 * CSCdi60044:  7000 image crash
 * CSCdi60394:  [sh vtp trunk] crashes 7000
 * CSCdi60528:  rsp can't change vtp database until someone else does
 * CSCdi60541:  Attempt to delete a vlan crashes image
 * CSCdi60632:  cannot set password for vtp management-domain
 * CSCdi60384:  configuration timestamp is not displayed correctly
 * Plus some Cat5k bug fixes rolled in as well as dependant code clean up.
 * Branch: California_branch
 *
 * Revision 1.2.58.4  1996/06/17  23:39:06  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 1.2.58.3  1996/05/21  22:29:24  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.2  1996/05/21  10:08:55  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.2.58.1  1996/05/09  14:48:28  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:30  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.2  1996/04/16  18:58:57  cyoung
 * Using 'exit','end' or '^Z' as an implicit apply breaks when the parser
 * needs to pop out of the sub-command scope to check against the global
 * parse chain.  Make 'apply' an explicit command to avoid this blunder.
 * CSCdi54003:  write to tftp file when it shouldnt, doesnt when it should
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.5  1996/04/03  22:36:56  cyoung
 * New objects for TFTP support, ETR vlan type and trunk table creation
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MIB - TFTP support, ETR type allowed, still need work on Trunk creation
 * dep - use temp buf to determine add/mod mode instead of real database
 * core - add tooBig hook for trap generation
 *
 * Revision 1.2.64.4  1996/03/30  04:14:10  cyoung
 * vtp-domain and vtp-database modes exit cleanly with ^Z or end
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.3  1996/03/25  02:21:47  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.2  1996/03/23  04:55:10  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:44:58  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.4  1996/03/23  02:20:56  cyoung
 * Quick fix for database exit command.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.3  1996/03/22  03:16:02  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 * Revision 1.2.36.2  1996/03/08  18:18:59  cyoung
 * Remove 'vtp network' command.
 * Add tftp server and file name parameters to 'server' keyword
 *  and support.
 * Check managementDomainIndex range in MIB code.
 * Change to client on reception of any error (not just NVRAM error).
 * Set the updater when configured from the CLI.
 * Set the appropriate state of a VLAN in vtp_edit_vlan
 * Remove unneeded function declarations in vtp_dep.h
 * Fix memory corruption in vtp_upload_info.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:23  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.3  1996/01/23  00:47:14  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.2.4.2  1996/01/11  19:52:58  cakyol
 * non sync sync now complete
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.1  1996/01/11  01:47:49  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.5  1996/01/06  00:37:40  dcheng
 * Update the VTP packet format to include ring number and parent VLAN for
 * FDDI/TR VLAN, bridge number and STP type for FDDI/TR net VLAN, and
 * translational bridged VLAN id (0, 1 or 2). Update the database and
 * user interface for these as well.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.4  1996/01/04  00:38:06  dcheng
 * Delete the boolean flag "vtp_enabled" in the idb structure, clean up the
 * related code. Fix some bugs in the CLI for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.3  1996/01/03  01:49:20  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.2  1995/12/23  03:51:26  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  02:00:49  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  01:59:42  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_vtp.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_parser.h"
#include "vtp_externs.h"
#include "vtp_interface.h"

/* This pair for the enumerations */
#define __DECLARE_DEBUG_NUMS__
#include "../ui/debug_macros.h"
#include "vtp_debug_flags.h"

/* This pair for the extern to the debug_item_type array */
#include "../ui/debug_macros.h"
#include "vtp_debug_flags.h"


/*
 * Parse chain for vtp debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_vtp.h"
LINK_POINT(vtp_debug_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain for top-level exec commands
 */
#define ALTERNATE	NONE
#include "exec_show_vtp.h"
LINK_POINT(vtp_show_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE       NONE
#include "exec_vtp_load.h"
LINK_POINT(vtp_exec_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain for VTP interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_vtp.h"
LINK_POINT(vtp_interface_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for vtp-domain command
 */
#define	ALTERNATE	NONE
#include "cfg_vtp_domain.h"
LINK_POINT(vtp_cfg_domain_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for vtp-domain subcommands
 */
#define	ALTERNATE	NONE
#include "cfg_vtp_dom_subcommands.h"
LINK_POINT(vtp_cfg_dom_subcommands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for vtp-database command
 */
#define	ALTERNATE	NONE
#include "cfg_vtp_database.h"
LINK_POINT(vtp_cfg_database_command, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for vtp-database sub-commands
 */
#define	ALTERNATE	NONE
#include "cfg_vtp_db_subcommands.h"
LINK_POINT(vtp_cfg_db_subcommands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain for vtp-database mode
 */
LINK_EXIT(cfg_vtp_exit, no_alt);

NVCOMMENT(cfg_vtp_database_comment, cfg_vtp_exit);
#define ALTERNATE	cfg_vtp_database_comment
#include "cfg_vtp_database_exit.h"
LINK_TRANS (cfg_vtp_database_extend_here, ALTERNATE);

NO_OR_DEFAULT(config_vtp_database_no, cfg_vtp_database_extend_here,
           PRIV_MIN | PRIV_NONVGEN);

HELP    (top_config_vtp_database, config_vtp_database_no,
         "VTP database editing buffer manipulation commands:\n");

#undef ALTERNATE


/*
 * Parse chains for VTP domain command.
 */
NVCOMMENT(cfg_vtp_domain_comment, cfg_vtp_exit);
#define ALTERNATE	cfg_vtp_domain_comment
#include "cfg_vtp_domain_exit.h"
LINK_TRANS (cfg_vtp_domain_extend_here, ALTERNATE);

NO_OR_DEFAULT(config_vtp_domain_no, cfg_vtp_domain_extend_here,
           PRIV_MIN | PRIV_NONVGEN);

HELP    (top_config_vtp_domain, config_vtp_domain_no,
         "VTP domain commands:\n");



/*
 * Parse chain registration array for VTP
 */
static parser_extension_request vtp_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(vtp_debug_commands) }, 
    { PARSE_ADD_SHOW_CMD, &pname(vtp_show_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(vtp_exec_commands) }, 
    { PARSE_ADD_CFG_INT_CMD, &pname (vtp_interface_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname (vtp_cfg_domain_command) },
    { PARSE_ADD_CFG_VTP_DOMAIN_CMD, &pname (vtp_cfg_dom_subcommands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname (vtp_cfg_database_command) },
    { PARSE_ADD_CFG_VTP_DATABASE_CMD, &pname (vtp_cfg_db_subcommands) },
    { PARSE_ADD_GLOBAL_EXIT, 
	  (dynamic_transition *)&pname (cfg_vtp_exit) },
    { PARSE_LIST_END, NULL }
};

void vtp_cli_database_enter (parseinfo *csb, void *var)
{
    csb->vtp_domain = var;
    return;
}

void* vtp_cli_database_exit (parseinfo *csb) {
    return csb->vtp_domain;
}

void vtp_database_exit (parseinfo *csb){
    if (csb->vtp_domain) {
	free(csb->vtp_domain);
	csb->vtp_domain = NULL;
    }
    exit_config_submode(csb);
}

void vtp_cli_domain_enter (parseinfo *csb, void *var)
{
    csb->vtp_domain = var;
    return;
}

void* vtp_cli_domain_exit (parseinfo* csb) {
    return csb->vtp_domain;
}

void vtp_domain_exit (parseinfo *csb)
{
    if (csb->vtp_domain) {
	free(csb->vtp_domain);
	csb->vtp_domain = NULL;
    }
    exit_config_submode(csb);
}



/*
 * vtp_parser_init - Initialize VTP parser support
 */
void vtp_parser_init (void)
{
    parser_add_link_point(PARSE_ADD_CFG_VTP_DOMAIN_CMD,
			  "vtp-domain",
			  &pname(cfg_vtp_domain_extend_here));

    parser_add_link_point(PARSE_ADD_CFG_VTP_DATABASE_CMD,
			  "vtp-database",
			  &pname(cfg_vtp_database_extend_here));

    parser_add_protoaddr(LINK_VTP, PMATCH_VTP,
 			 "vtp",	"Cisco VLAN Transport Protocol",
 			 2, ADDR_ILLEGAL);

    parser_add_command_list(vtp_chain_init_table, "vtp");


    parser_add_mode("vtp-domain", "config-vtp-domain",
                    "VTP domain definition",
                    TRUE, TRUE,
                    "configure",
                    vtp_cli_domain_exit, vtp_cli_domain_enter,
                    &pname(top_config_vtp_domain),
		    NULL);

    parser_add_mode("vtp-database", "config-vtp-database",
                    "VTP database editing buffer",
                    TRUE, TRUE,
                    "configure",
                    vtp_cli_database_exit, vtp_cli_database_enter,
                    &pname(top_config_vtp_database),
		    NULL);
}






