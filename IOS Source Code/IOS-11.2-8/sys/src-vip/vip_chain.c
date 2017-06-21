/* $Id: vip_chain.c,v 3.5.10.3 1996/06/03 18:04:51 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_chain.c,v $
 *------------------------------------------------------------------
 * vip_chain.c -- VIP parser support
 *
 * July 1995, Michael Beesley 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_chain.c,v $
 * Revision 3.5.10.3  1996/06/03  18:04:51  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/05/21  10:04:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/03/21  23:52:18  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:36:34  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/10  00:22:26  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/01/16  02:36:15  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.5  1996/02/22  14:38:36  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/02/18  19:42:44  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.3  1995/12/17  18:38:35  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:51:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/21  19:54:10  mbeesley
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "subsys.h"
#include "packet.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"

#include "../pas/if_pas.h"

#include "vip_memd.h"
#include "vip_debug.h"
#include "vip_dma.h"
#include "vip_chain.h"

#define	ALTERNATE	NONE
#include "vip_test_chain.h"
LINK_POINT(test_vip_commands, ALTERNATE);
#undef	ALTERNATE

#if defined(SVIP)
#define ALTERNATE       NONE
#include "../src-rsp/cfg_scheduler_alloc.h"
LINK_POINT(vip_cfg_sched_cmd, ALTERNATE);
#undef  ALTERNATE
#endif

/*
 * VIP Parse chains
 */
static parser_extension_request vip_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(show_vip_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(debug_vip_commands) },
    { PARSE_ADD_TEST_CMD, &pname(test_vip_commands) },
#if defined(SVIP)
    { PARSE_ADD_CFG_SCHED_CMD, &pname(vip_cfg_sched_cmd) },
#endif
    { PARSE_LIST_END, NULL }
};

/*
 * vip_show_statistics:
 * Display running stats for the VIP board
 */
void vip_show_statistics (parseinfo *csb)
{
    vip_netint_throttle_show();
    printf("VIP Cbus error statistics:\n");
    printf("bus_stall=%d, bus_stall_read_events=%d, bus_stall_write_events=%d\n",
	   vsys_vec.bus_stall, vsys_vec.bus_stall_read_err, 
	                   vsys_vec.bus_stall_write_err);
}

/*
 * vip_ui_init :
 * Initialize VIP specific parse chains
 */
static void vip_ui_init (subsystype *subsys)
{
    /*
     * Initialize diagnostic bus parse chains
     */
    vip_init_dbus_parser();

    /*
     * Hook in the VIP parse chains
     */
    parser_add_command_list(vip_chain_init_table,"VIP system commands");
    vip_init_debug();
}

/*
 * vip_ui subsystem header
 */
 
#define VIP_UI_MAJVERSION    1
#define VIP_UI_MINVERSION    0
#define VIP_UI_EDITVERSION   1
 
SUBSYS_HEADER(vip_ui, VIP_UI_MAJVERSION, VIP_UI_MINVERSION,
              VIP_UI_EDITVERSION,
              vip_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

/* end of file */
