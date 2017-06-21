/* $Id: c7100_chain.c,v 3.2.58.9 1996/09/13 16:57:11 dujihara Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_chain.c,v $
 *------------------------------------------------------------------
 * c7100_chain.c - Predator parser support
 *
 * August 1995, Dave Carroll
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_chain.c,v $
 * Revision 3.2.58.9  1996/09/13  16:57:11  dujihara
 * CSCdi67851:  sh tech-support should include show c7200
 * Branch: California_branch
 * Added "show diag" and "show c7200" to techsupport registry.
 *
 * Revision 3.2.58.8  1996/06/17  23:36:58  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2.58.7  1996/06/06  03:08:39  mbeesley
 * CSCdi59592:  Bootstrap should not emit warnings wrt no support for
 * flow/opt switching. Don't print warning is system_loading.
 * Branch: California_branch
 *
 * Revision 3.2.58.6  1996/05/21  10:03:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.58.5  1996/05/07  00:14:34  mbeesley
 * CSCdi55679:  Add IP flow/optimum switching engine
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/05/01  13:28:19  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.2.58.3  1996/04/29  17:59:12  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/04/06  02:05:40  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:47:40  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.8  1996/03/18  23:33:10  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.7  1996/02/16  02:23:59  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.2.2.6  1996/01/23  02:18:11  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.2.2.5  1996/01/16  02:16:43  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.4  1996/01/09  21:27:44  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.2.2.3  1995/12/15  18:42:55  dcarroll
 * Branch: ELC_branch
 * Remove envmon code from boot image. Use common
 * envmon parser code.
 *
 * Revision 3.2.2.2  1995/11/30  02:13:56  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:43:05  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:13  shaker
 * Bump version numbers from 2.x to 3.x.
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
#include "logger.h"
#include "parser.h"
#include "subsys.h"
#include "media_registry.h"
#include "sys_registry.h"

#include "../ip/ip_registry.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../os/gdb.h"
#include "../os/free.h"
#include "../os/techsupport.h"

#include "c7100_pcmap.h"
#include "c7100_fpga.h"
#include "c7100.h"
#include "c7100_macaddr.h"

/*
 * c7100_test_debug_command:
 * Handle "test c7100 debug {off|on}"
 */
enum {
    TEST_C7100_DEBUG_STATE_OFF = 1,
    TEST_C7100_DEBUG_STATE_ON,
};

static void c7100_test_debug_command (parseinfo *csb)
{
    printf("Setting c7200 debug ");
    if (GETOBJ(int,1) == TEST_C7100_DEBUG_STATE_ON) {
	c7100_debug = TRUE;
	printf("on");
    } else {
	c7100_debug = FALSE;
	printf("off");
    }
}

/*
 * show_c7100_command:
 * Handle "show c7100"
 */
static void show_c7100_command (parseinfo *csb)
{
    c7100_netint_throttle_show();
    c7100_show_mpeeprom();
    c7100_show_cpueeprom();
    fault_history_display();
}

/*
 * show_c7100_techsupport:
 * This function is added to the "show tech-support" registry
 */
static void show_c7100_techsupport (parseinfo *csb)
{
  show_techsupport_command(csb, "show diag");
  show_techsupport_command(csb, "show c7200");
}

/*
 * ip_routec_optimum_cmd:
 * Handle "ip route-cache optimum"
 */ 
static boolean ip_routec_optimum_cmd (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = idb->hwptr;

    if (csb->nvgen) {
	/*
	 * Since 'optimum' switching defaults on, only emit
	 * 'no ip route-cache optimum' if the interface can support optimum
	 * switching (and the code to implement it is present), if normal
	 * fast switching is enabled, and if optimum switching is disabled:
	 *
	 * Since this feature is in an alpha state, we will
	 * always emit config.
	 */
	if (reg_invoke_ip_turbo_idb_capable(hwidb) &&
	    (hwidb->ip_routecache & IP_FAST_ROUTECACHE) &&
	    (hwidb->ip_routecache & IP_OPTIMUM_ROUTECACHE)) {
	    nv_write(TRUE, "%s", csb->nv_command);
	}
	return(TRUE);
    }

    /*
     * Only allow 'ip route-cache optimum' to be configured if the
     * interface can support the feature, and only if the code to implement
     * it is present
     */
    if (reg_invoke_ip_turbo_idb_capable(hwidb)) {
	if (csb->set_to_default || csb->sense) {
	    /* Flow and Optimum switching are mutually exclusive */
	    hwidb->ip_routecache |=
		(IP_FAST_ROUTECACHE | IP_OPTIMUM_ROUTECACHE);
	    hwidb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	} else {
	    hwidb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	}
    } else {
	if (!system_loading && !csb->set_to_default && csb->sense) {
	    printf("Optimum switching not supported on %s", idb->namestring);
	} else {
	    hwidb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	}
	return(FALSE);
    }
    return(TRUE);
}

/*
 * ip_routec_flow_cmd:
 * Handle "ip route-cache flow"
 */
static boolean ip_routec_flow_cmd (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = idb->hwptr;

    if (csb->nvgen) {
	if (hwidb->ip_routecache & IP_FLOW_ROUTECACHE) {
	    nv_write(TRUE, "%s", csb->nv_command);
	}
	return(TRUE);
    }

    if (reg_invoke_ip_turbo_idb_capable(hwidb)) {
	if (csb->sense) {
	    /* Flow and Optimum switching are mutually exclusive */
	    hwidb->ip_routecache |=
		(IP_FAST_ROUTECACHE | IP_FLOW_ROUTECACHE);
	    hwidb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	} else {
	    hwidb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	    hwidb->ip_routecache |= IP_OPTIMUM_ROUTECACHE;
	}
    } else {
	if (!system_loading && csb->sense) {
	    printf("Flow switching not supported on %s", idb->namestring);
	} else {
	    hwidb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	}
	return(FALSE);
    }
    return(TRUE);
}

/*
 * Parser chains
 */

#define ALTERNATE       NONE
#include "exec_test_c7100.h"
LINK_POINT(c7100_test_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "exec_show_c7100.h"
#include "../pas/exec_pas_show_diag.h"
LINK_POINT(c7100_show_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "../src-rsp/cfg_scheduler_alloc.h"
LINK_POINT(c7100_cfg_sched_cmd, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "../if/cfg_int_transmitter-delay.h"
LINK_POINT(c7100_ci_transmit_delay_cmd, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "cfg_oir_c7100.h"
LINK_POINT(c7100_cfg_oir_cmd, ALTERNATE);
#undef  ALTERNATE

LINK_EXIT(ci_ip2_routecache_optimum_exit, no_alt);
KEYWORD_ID(ci_ip2_routecache_flow, ci_ip2_routecache_optimum_exit,
           NONE,
           OBJ(int,1), IP_FLOW_ROUTECACHE,
           "flow", "Enable Flow fast-switching cache", PRIV_CONF);
KEYWORD_ID(ci_ip2_routecache_optimum, ci_ip2_routecache_optimum_exit,
           ci_ip2_routecache_flow,
           OBJ(int,1), IP_OPTIMUM_ROUTECACHE,
           "optimum", "Enable Optimum fast-switching cache", PRIV_CONF);
LINK_POINT(ci_ip2_routecache_optimum_enter, ci_ip2_routecache_optimum);

static parser_extension_request c7100_chain_init_table[] = {
    { PARSE_ADD_TEST_CMD, &pname(c7100_test_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(c7100_show_commands) },
    { PARSE_ADD_CFG_SCHED_CMD, &pname(c7100_cfg_sched_cmd) },
    { PARSE_ADD_CFG_INT_CMD, &pname(c7100_ci_transmit_delay_cmd) },
    { PARSE_ADD_CFG_INT_IPROUTEC_CMD, 
	  &pname(ci_ip2_routecache_optimum_enter) },
    { PARSE_ADD_CFG_INT_IPROUTEC_EXIT,
	  (dynamic_transition *) &pname(ci_ip2_routecache_optimum_exit) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(c7100_cfg_oir_cmd) },
    { PARSE_LIST_END, NULL }
};

/*
 * c7100_parser_init:
 * Install Predators parse chains
 */
static void c7100_parser_init (subsystype *subsys)
{
    parser_add_command_list(c7100_chain_init_table, "c7100");
    reg_add_ip_route_cache_cmd(IP_FLOW_ROUTECACHE, ip_routec_flow_cmd,
			       "ip_routec_flow_cmd");
    reg_add_ip_route_cache_cmd(IP_OPTIMUM_ROUTECACHE, ip_routec_optimum_cmd,
			       "ip_routec_optimum_cmd");
    reg_add_show_techsupport(show_c7100_techsupport,
                             "show_c7100_techsupport");
}

#define	C7100_UI_MAJVERSION	1
#define	C7100_UI_MINVERSION	0
#define	C7100_UI_EDITVERSION	1

SUBSYS_HEADER(c7100_ui, C7100_UI_MAJVERSION,
	      C7100_UI_MINVERSION, C7100_UI_EDITVERSION,
	      c7100_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      NULL, NULL);

/* end of file */
