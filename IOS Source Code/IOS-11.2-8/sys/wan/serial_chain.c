/* $Id: serial_chain.c,v 3.7.10.2 1996/05/21 10:09:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/serial_chain.c,v $
 *------------------------------------------------------------------
 * serial_chain.c - Parse chains for generic serial support
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: serial_chain.c,v $
 * Revision 3.7.10.2  1996/05/21  10:09:53  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.1  1996/03/22  00:15:21  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.2.5  1996/03/18  23:57:48  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.4  1996/02/08  10:01:52  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.3  1996/01/26  02:51:37  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.3.2.2  1996/01/16  04:52:48  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  19:52:09  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.7  1996/02/22  14:40:04  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/01/29  07:31:24  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1995/12/17  18:41:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/27  16:24:03  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.3  1995/11/17  18:04:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:50:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  01:46:15  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  23:19:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "serial_debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "serial.h"
#include "serial_parser.h"
#include "media_registry.h"


/*
 * Parse chains for serial debug commands
 */
#define	ALTERNATE	NONE
#include "exec_debug_serial.h"
LINK_POINT(serial_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for serial interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_dte.h"
#include "cfg_int_invtxc.h"
#include "cfg_int_dceterm.h"
#include "cfg_int_clockrate.h"
#include "cfg_int_nrzi.h"
#include "cfg_int_serial.h"
#include "cfg_int_rxsync.h"
#include "cfg_int_ts16.h"
#include "cfg_int_crc4.h"
#include "cfg_int_clocksource.h"
#include "cfg_int_timeslot.h"
#include "cfg_int_idle-character.h"
#include "cfg_int_downloop.h"
#include "cfg_int_ignore_dcd.h"
#include "cfg_int_ignore_hw.h"
#include "../hes/cfg_int_crc.h"
LINK_POINT(serial_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for HDLC interface encapsulation commands
 */
LINK_EXIT(cfg_encap_hdlc_exit, no_alt);
ENCAPS	(cfg_encap_hdlc, cfg_encap_hdlc_exit, NONE,
	 OBJ(int,1), ET_HDLC,
	 "hdlc", "Serial HDLC synchronous",
	 (IDB_SYNC | IDB_ISDN | IDB_DIALER));
LINK_POINT(cfg_encap_hdlc_cmd, cfg_encap_hdlc);


/*
 * Parse chain registration array for serial commands
 */
static parser_extension_request serial_chain_init_table[] = {
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(serial_interface_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(serial_debug_commands) },
    { PARSE_ADD_INT_ENCAPS_CMD, &pname(cfg_encap_hdlc_cmd) },
    { PARSE_ADD_INT_ENCAPS_EXIT,
	  (dynamic_transition *) &pname(cfg_encap_hdlc_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * serial_parser_init - Initialize serial parser support
 */
void serial_parser_init (void)
{
    parser_add_command_list(serial_chain_init_table, "serial");
}
