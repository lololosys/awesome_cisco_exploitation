/* $Id: channel_chain.c,v 3.6.4.2 1996/05/21 09:49:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/channel_chain.c,v $
 *------------------------------------------------------------------
 * Channel parse chain registration routines
 *
 * April 1994, William H. Palmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Dynamically linked parse chains for IBM Channel interface. 
 *------------------------------------------------------------------
 * $Log: channel_chain.c,v $
 * Revision 3.6.4.2  1996/05/21  09:49:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.4.1  1996/05/17  11:22:24  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.2  1996/03/28  11:29:07  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.1  1996/03/17  17:51:13  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6  1996/03/09  22:55:06  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.5  1996/02/22  14:33:58  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1995/12/17  18:28:43  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/01  15:56:17  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  09:26:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/17  15:46:54  ppearce
 * CSCdi40438:  Cannot configure LLC parms for RSRB/LAK on CSNA virtual
 * interface
 *   LLC2 Channel interface commands lost after reload
 *
 * Revision 2.2  1995/07/02  10:53:16  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  20:51:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_channel.h"
#include "channel_debug.h"
#include "../hes/if_cip.h"
#include "../hes/if_cip_vlan.h"
/* =========================== TN3270 =========================== */
#include "../hes/if_cip_tn3270s.h"
/* =========================== TN3270 =========================== */


/*
 * Parse chains for channel debug commands 
 */
#define ALTERNATE	NONE
#include "exec_debug_channel.h"
LINK_POINT(channel_debug_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parse chains for channel interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_channel.h"
LINK_POINT(channel_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for show extended channel commands
 */
#define	ALTERNATE	NONE
#include "exec_show_extended_channel.h"
LINK_POINT(channel_showextended_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for show extended sysmgt commands
 */
#define	ALTERNATE	NONE
#include "exec_show_sysmgt_rpc.h"
LINK_POINT(sysmgt_showextended_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for channel
 */
static parser_extension_request channel_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(channel_debug_commands) },
    { PARSE_ADD_SHOW_EXTENDED_CMD, &pname(channel_showextended_commands) },
    { PARSE_ADD_SHOW_EXTENDED_CMD, &pname(sysmgt_showextended_commands) },
    { PARSE_ADD_CFG_INT_AS2_CMD, &pname(channel_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * channel_parser_init - Initialize channel parser support
 */

void channel_parser_init (void)
{
    static boolean channel_parser_initialized = FALSE;

    if (!channel_parser_initialized) {
	parser_add_command_list(channel_chain_init_table, "channel");
	parser_add_mode("cip-vlan",pVlanPrompt,
			"Virtual Lan configuration mode",
			TRUE, TRUE, common_str_interface,
			vlan_set_vars, vlan_reset_vars,
			&pname(top_config_vlan), 
			NULL);
	parser_add_mode("cip-vadp", pVadapPrompt,
			"Virtual Adapter configuration mode",
			TRUE, TRUE, "cip-vlan",
			vadap_set_vars, vadap_reset_vars,
			&pname(top_config_vadap), NULL);
/* =========================== TN3270 =========================== */
	parser_add_mode(TN3270S_MODENAME_SVR, pTn3270sSvrPrompt,
			"tn3270 server configuration mode",
			TRUE, TRUE, "interface",
			tn3270s_svr_set_vars, tn3270s_svr_reset_vars,
			&pname(top_config_tn3270s), NULL);
	parser_add_mode(TN3270S_MODENAME_PU, pTn3270sPuPrompt,
			"tn3270 server PU configuration mode",
			TRUE, TRUE, TN3270S_MODENAME_SVR,
			tn3270s_pu_set_vars, tn3270s_pu_reset_vars,
			&pname(top_config_tn3270s_pu), NULL);
	parser_add_mode(TN3270S_MODENAME_DLUR, pTn3270sDlurPrompt,
			"tn3270 server DLUR configuration mode",
			TRUE, TRUE, TN3270S_MODENAME_SVR,
			tn3270s_dlur_set_vars, tn3270s_dlur_reset_vars,
			&pname(top_config_tn3270s_dlur), NULL);
	parser_add_mode(TN3270S_MODENAME_DLUR_PU, pTn3270sDlurPuPrompt,
			"tn3270 server DLUR PU configuration mode",
			TRUE, TRUE, TN3270S_MODENAME_DLUR,
			tn3270s_dlur_pu_set_vars, tn3270s_dlur_pu_reset_vars,
			&pname(top_config_tn3270s_dlur_pu), NULL);
	parser_add_mode(TN3270S_MODENAME_DLUR_SAP, pTn3270sDlurSapPrompt,
			"tn3270 server DLUR SAP configuration mode",
			TRUE, TRUE, TN3270S_MODENAME_DLUR,
			tn3270s_dlur_sap_set_vars, tn3270s_dlur_sap_reset_vars,
			&pname(top_config_tn3270s_dlur_sap), NULL);
/* =========================== TN3270 =========================== */
	channel_parser_initialized = TRUE;
    }
}
