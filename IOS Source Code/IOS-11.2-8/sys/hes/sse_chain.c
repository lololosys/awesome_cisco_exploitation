/* $Id: sse_chain.c,v 3.6.10.5 1996/09/14 00:05:17 sseal Exp $
 * $Source: /release/112/cvs/Xsys/hes/sse_chain.c,v $
 *------------------------------------------------------------------
 * sse_chain.c - Registration for SSE commands
 *
 * May 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sse_chain.c,v $
 * Revision 3.6.10.5  1996/09/14  00:05:17  sseal
 * CSCdi61196:  ipx route-cache options need clean up
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/08/28  12:46:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.10.3  1996/08/06  07:12:15  gchristy
 * CSCdi52153:  <ip route-cache sse> on ATM interface lost by nvgen
 * Branch: California_branch
 * - Allow nvgen on first swidb regardless of the state of IP.
 *
 * Revision 3.6.10.2  1996/05/21  09:48:12  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/03/18  19:43:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  06:52:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/13  01:22:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.14.1  1996/02/20  00:53:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/22  14:33:26  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:27:50  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/23  08:47:55  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.3  1995/12/17  18:27:37  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:20:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  10:52:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/15  18:38:11  gchristy
 * CSCdi26854:  SSE access-lists arent enabled with SSE switching
 *  - add a new registry function which causes SSE access lists to be
 *    rebuilt when SSE switching is enabled or disabled on an interface
 *    that has an access-list defined
 *
 * Revision 2.1  1995/06/07  20:45:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../hes/msg_sse.c"		/* Not a typo, see logger.h */
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ui/debug.h"
#include "../util/range.h"
#include "interface_private.h"
#include "../parser/parser_defs_sse.h"
#include "sse_debug.h"
#include "sse.h"
#include "subsys.h"
#include "../ip/ip_registry.h"
#include "../xns/novell.h"
#include "../hes/sse_registry.h"
#include "../hes/sse_private.h"

#include "../if/ether.h"
#include "../h/ieee.h"
#include "../vlan/vlan.h"

/*
 * Parse chains for CLEAR commands
 */

#define ALTERNATE NONE
#include "exec_clear_sse.h"
LINK_POINT(sse_clear_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
#include "exec_debug_sse.h"
LINK_POINT(sse_debug_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for SHOW CLNS commands
 */
#define ALTERNATE NONE
#include "exec_show_clns_sse.h"
LINK_POINT(sse_show_clns_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for top-level exec commands
 */
#define ALTERNATE NONE
#include "exec_show_sse.h"
LINK_POINT(sse_exec_show_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE NONE
#include "exec_test_sse.h"
LINK_POINT(sse_exec_test_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Top level SSE commands
 */

#define ALTERNATE NONE
#include "cfg_sse.h"
LINK_POINT(sse_commands, ALTERNATE);
#undef ALTERNATE



LINK_EXIT (ci_ip2_routecache_sse_exit, no_alt);
KEYWORD_ID(ci_ip2_routecache_sse, ci_ip2_routecache_sse_exit, NONE,
	   OBJ(int,1), IP_SSE_ROUTECACHE,
	   "sse", "Enable SSE fast-switching cache", PRIV_CONF|PRIV_SUBIF);
LINK_POINT(ci_ip2_routecache_sse_enter, ci_ip2_routecache_sse);

/*
 * For IPX fastswitching 
 */

LINK_EXIT (ci_ipx_routecache_sse_exit, no_alt);
KEYWORD_ID(ci_ipx_routecache_sse, ci_ipx_routecache_sse_exit, NONE,
	    OBJ(int,1), NOVELL_RCACHE_SSE,
	    "sse", "Enable sse fast-switching cache", PRIV_CONF);
LINK_POINT(ci_ipx_routecache_sse_enter, ci_ipx_routecache_sse);


/*
 * Parse chain registration array for SSE commands
 */

static parser_extension_request sse_chain_init_table[] = {
    { PARSE_ADD_CLEAR_CMD, &pname(sse_clear_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(sse_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(sse_debug_commands) }, 
    { PARSE_ADD_SHOW_CMD, &pname(sse_exec_show_commands) },
    { PARSE_ADD_TEST_CMD, &pname(sse_exec_test_commands) },
    { PARSE_ADD_SHOW_CLNS_CMD, &pname(sse_show_clns_commands) },
    { PARSE_ADD_CFG_INT_IPROUTEC_CMD, &pname(ci_ip2_routecache_sse_enter) },
    { PARSE_ADD_CFG_INT_IPXROUTEC_CMD, &pname(ci_ipx_routecache_sse_enter) },
    { PARSE_ADD_CFG_INT_IPROUTEC_EXIT,
	  (dynamic_transition *) &pname(ci_ip2_routecache_sse_exit) },
    { PARSE_ADD_CFG_INT_IPXROUTEC_EXIT,
	  (dynamic_transition *) &pname(ci_ipx_routecache_sse_exit) },
    { PARSE_LIST_END, NULL }
};


static boolean ip_routec_sse_cmd (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb = idb->hwptr;

    if (csb->nvgen) {

	if ((is_first_swidb(idb) &&
	     (hwidb->ip_routecache & IP_SSE_ROUTECACHE))
	    || IS_SSE_SWITCHED_VLAN(idb)) 
	    nv_write(TRUE, "%s", csb->nv_command);

	return(TRUE);
    }

    if (is_subinterface(idb) == TRUE) {

        /*
	 * Currently we only support SSE switching of ISL
	 * vLAN encapsulated IP datagrams on subinterfaces.
	 */

	if (IS_ISL_VLAN_SUBINTERFACE(idb) == FALSE) {

	    if (csb->sense)
		printf("\n\nvLAN: The configuration of SSE IP switching on a"
		       " subinterface is currently\n      only supported for"
		       " ISL vLAN encapsulated datagrams on Fast Ethernet.\n");
            return(FALSE);
	}

	/*
	 * O.K. Flag this vLAN SSE Switched.
	 */

	reg_invoke_set_vlan_sse_switched(csb->interface, csb->sense);
	return(TRUE);
    }

    if (reg_invoke_sse_idb_capable(hwidb, TRUE)) {

	if (csb->sense) {
	    hwidb->ip_routecache |=
		(IP_FAST_ROUTECACHE | IP_SSE_ROUTECACHE);
	} else {
	    hwidb->ip_routecache &= ~IP_SSE_ROUTECACHE;
	}
	reg_invoke_sse_access_check(idb);
    } else {
	if (csb->sense) {
	    printf("SSE switching not supported on %s", idb->namestring);
	} else {
	    hwidb->ip_routecache &= ~IP_SSE_ROUTECACHE;
	}
	return(FALSE);
    }
    return(TRUE);
}

/*
 * sse_chain_init - Initialize parse chains for SSE commands
 * 
 * This routine is called at system startup to register the parse chains
 * for SSE commands.
 */

static void sse_parser_init (subsystype *subsys)
{
    if (sse_manager_running) {
	parser_add_link_point(PARSE_ADD_TEST_SSE_CMD, "test sse",
			      &pname(test_sse_extend_here));
	parser_add_command_list(sse_chain_init_table, "sse");
	reg_add_ip_route_cache_cmd(IP_SSE_ROUTECACHE, ip_routec_sse_cmd,
				   "ip_routec_sse_cmd");
    }
}

/*
 * sse parser subsystem header
 */
#define	SSE_UI_MAJVERSION	1
#define	SSE_UI_MINVERSION	0
#define	SSE_UI_EDITVERSION	1

SUBSYS_HEADER(sse_ui, SSE_UI_MAJVERSION,
	      SSE_UI_MINVERSION, SSE_UI_EDITVERSION,
	      sse_parser_init, SUBSYS_CLASS_MANAGEMENT,
	      NULL,
	      "req: sse");
