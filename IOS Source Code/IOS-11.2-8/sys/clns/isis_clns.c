/* $Id: isis_clns.c,v 3.10.10.14 1996/08/12 01:06:48 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_clns.c,v $
 *------------------------------------------------------------------
 * CLNS-specific IS-IS processing
 *
 * January 1995, Dave Katz
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module consists of code cleaved off of various other ISIS
 * modules in order to isolate the CLNS-specific components, allowing
 * NLSP to exist without all of CLNS.
 *------------------------------------------------------------------
 * $Log: isis_clns.c,v $
 * Revision 3.10.10.14  1996/08/12  01:06:48  dkatz
 * CSCdi45425:  NLSP may not report RIP/SAP routes
 * Branch: California_branch
 * Don't let the DR timer get in the way of reporting adjacencies (and
 * remove a couple of other checks for it).  Rebuild both non-pseudonode
 * and pseudonode LSPs when an interface changes state.
 *
 * Revision 3.10.10.13  1996/08/11  15:13:11  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.10.10.12  1996/08/08  14:50:45  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.10.10.11  1996/08/07  08:57:20  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.10.10  1996/08/02  02:47:52  hsmit
 * CSCdi64256:  Add knob to ISIS to generate LSPs with longer lifetime
 * Branch: California_branch
 * Implemented lsp-refresh-interval and max-lsp-lifetime for ISIS. Changed
 * the
 * "show clns traffic" command to show the number of changed and refreshed
 * LSPs.
 *
 * Revision 3.10.10.9  1996/07/10  10:08:42  dawnli
 * CSCdi54313:  ISIS: Negation of interface commands are not consistant
 * Branch: California_branch
 *
 * Revision 3.10.10.8  1996/06/18  16:54:30  dkatz
 * CSCdi58953:  cannot change ISIS type from L12 to L1
 * Branch: California_branch
 * Don't do adjacency holddown if no adjacencies are present (so we don't
 * do holddown when we first start up.)
 *
 * Revision 3.10.10.7  1996/06/18  16:50:45  dkatz
 * CSCdi59965:  ISIS retransmits too quickly
 * Branch: California_branch
 * Add a retransmission throttling mechanism.
 *
 * Revision 3.10.10.6  1996/06/18  16:31:27  dkatz
 * CSCdi58953:  cannot change ISIS type from L12 to L1
 * Branch: California_branch
 * Force the protocol to go to sleep long enough for all neighbor
 * adjacencies to time out when the level is changed.  While we're
 * in there, do it for NET changes as well (since these can change
 * adjacency levels).
 *
 * Revision 3.10.10.5  1996/05/26  06:32:13  dkatz
 * CSCdi58433:  ISIS/NLSP can starve input queue
 * Branch: California_branch
 * Do a clear_if_input on incoming packets before requeueing them for the
 * Update process.  Depth-limit the secondary queue to avoid buffer
 * starvation.
 *
 * Revision 3.10.10.4  1996/05/07  17:15:56  dkatz
 * CSCdi54576:  ISIS LSP pacing rate needs to be configurable
 * Branch: California_branch
 * Provide a knob to alter the LSP pacing timer;  the timer is already
 * there.
 *
 * Revision 3.10.10.3  1996/05/06  06:14:53  asastry
 * CSCdi40503:  CLNS routing problems caused by discard routes (DECnet
 * conversion)
 * Branch: California_branch
 *
 * Revision 3.10.10.2  1996/05/03  01:50:44  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.10.10.1  1996/03/28  23:44:17  rchandra
 * CSCdi52899:  ISIS: crash during SPF run
 * Branch: California_branch
 * - check if next_hop structure is null before using it
 *
 * Revision 3.10  1996/02/21  00:19:14  dkatz
 * CSCdi48162:  ISIS doesnt add redundant L2 paths in CLNS routing table
 * Set the SPF instance on all RDBs
 *
 * Revision 3.9  1996/02/07  16:10:32  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/01/29  07:26:50  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.7  1996/01/05  04:55:33  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.6  1995/12/13  08:06:51  tli
 * CSCdi45496:  IS-IS: crash from show_isis_route
 *
 * Revision 3.5  1995/12/11  08:05:32  rchandra
 * CSCdi44622:  CLNS static routes to interface not redistributed in ISIS
 * - allow redistribution of static routes pointing to non-ISIS interface
 *
 * Revision 3.4  1995/12/01  02:48:26  dkatz
 * CSCdi39906:  CPUHOG Process = IS-IS Update
 *
 * Revision 3.3  1995/11/17  08:53:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:09:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  20:52:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/21  19:03:03  dkatz
 * CSCdi42558:  ISIS needs more debugging hooks
 * Provide a means to look at LSP database internal information.
 * Provide a means to flush the LSP database.
 *
 * Revision 2.10  1995/09/23  21:37:37  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.9  1995/08/25  16:02:49  dkatz
 * CSCdi38487:  ISIS LSP MTU needs to be settable
 *
 * Revision 2.8  1995/08/25  15:11:59  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.7  1995/08/25  14:59:46  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.6  1995/08/25  05:40:00  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.5  1995/08/23  18:26:01  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Disentangle SPF triggering and holddown calculations.  Insert a short
 * delay before doing the first SPF run.  Add logging of SPF runs for
 * diagnosability.
 *
 * Revision 2.4  1995/08/23  18:03:08  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.3  1995/06/30  23:39:54  dbath
 * CSCdi35958:  show clns routes command fields not wide enough for slots
 * 10,11...
 * Make show 'clns route', 'clns neigh', 'clns is-neigh',
 * 'clns es-neigh' and 'isis route' all use short names for interfaces
 * to make room for 2 digit slot numbers (should also help for FEIP).
 *
 * Revision 2.2  1995/06/21  08:53:48  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  20:18:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "mgd_timers.h"
#include "subsys.h"
#include "../ip/ip_registry.h"
#include "../if/network.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_adj.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_ipx.h"
#include "../os/filter.h"
#include "../os/filter_externs.h"
#include "../iprouting/route.h"
#include "parser.h"
#include "../clns/parser_defs_isis.h"
#include "isis_inlines.h"
#include "../clns/isis_registry.h"


/*
 * isis_process_timers
 *
 * Process IS-IS timer interval configuration commands.
 */
static void isis_process_timers (parseinfo *csb)
{

    idbtype    *idb;
    int        level, number;
    boolean    default_l1_hello, default_l2_hello;
    clns_pdbtype *pdb;
    isisidbtype *isisidb;

    idb = csb->interface;
    pdb = find_isis_pdb();

    if (csb->nvgen) {
	isisidb = get_isis_idb(pdb, idb);
	if (!isisidb)
	    return;

	switch (csb->which) {
	case ISIS_HELLO_CMD:
	    default_l1_hello = 
		(isisidb->isis_L1_hello_interval == pdb->def_hello_ivl);
	    default_l2_hello = 
		(isisidb->isis_L2_hello_interval == pdb->def_hello_ivl);
	    nv_write(!default_l1_hello, "%s %d level-1", csb->nv_command, 
		     isisidb->isis_L1_hello_interval/ONESEC);
	    nv_write(!default_l2_hello, "%s %d level-2", csb->nv_command,
		     isisidb->isis_L2_hello_interval/ONESEC);
	    break;

	case ISIS_CSNP_CMD:
	    nv_write((isisidb->csnp_ctl[0].csnp_interval != 
		      isis_default_csnp_timer(idb, PROC_TYPE_ISIS)),
  		     "%s %d level-1", csb->nv_command,
		     isisidb->csnp_ctl[0].csnp_interval/ONESEC);
	    nv_write((isisidb->csnp_ctl[1].csnp_interval !=
		      isis_default_csnp_timer(idb, PROC_TYPE_ISIS)),
  		     "%s %d level-2", csb->nv_command,
		     isisidb->csnp_ctl[1].csnp_interval/ONESEC);
	    break;

	case ISIS_RETRANS_CMD:
	    nv_write((isisidb->isis_retrans_interval != 
		      ISIS_DEF_RETRANS_TIMER),
  		     "%s %d", csb->nv_command,
		     isisidb->isis_retrans_interval/ONESEC);
	    break;

	  case ISIS_MIN_REXMIT_CMD:
	    nv_write(isisidb->isis_min_rexmit_interval, "%s %d",
		     csb->nv_command, isisidb->isis_min_rexmit_interval);
	    break;

	case ISIS_LSPIVL_CMD:
	    nv_write((isisidb->pacing_interval != ISIS_DEF_PACING_INTERVAL),
  		     "%s %d", csb->nv_command, isisidb->pacing_interval);
	    break;

	default:
	    break;
	}
	return;
    }

    isisidb = isis_alloc_isisidb(idb, PROC_TYPE_ISIS, CLNS_PDBINDEX_ISIS0);
    if (!isisidb)
	return;
    number = GETOBJ(int,1) * ONESEC;
    level = GETOBJ(int,2);

    switch (csb->which) {
      case ISIS_HELLO_CMD:
	if (!csb->sense)
	    number = ISIS_DEF_HELLO_IVL;
	if (level == ISIS_CIRCUIT_L1) {
	    isisidb->isis_L1_hello_interval = number;
	    isisidb->isis_L1_dr_hello_interval = 
		isisidb->isis_L1_hello_interval / ISIS_LAN_DR_HELLO_DIVISOR;
	} else {
	    isisidb->isis_L2_hello_interval = number;
	    isisidb->isis_L2_dr_hello_interval = 
		isisidb->isis_L2_hello_interval / ISIS_LAN_DR_HELLO_DIVISOR;
	}
	break;

      case ISIS_CSNP_CMD:
	if (!csb->sense)
	    number = isis_default_csnp_timer(idb, PROC_TYPE_ISIS);
	isisidb->csnp_ctl[level - 1].csnp_interval = number;
	if (isisidb->isis_running)
	    mgd_timer_start(&isisidb->csnp_ctl[level - 1].isis_csnp_timer,
			    number);
	break;

      case ISIS_RETRANS_CMD:
	if (!csb->sense)
	    number = ISIS_DEF_RETRANS_TIMER;
	isisidb->isis_retrans_interval = number;
	break;

      case ISIS_MIN_REXMIT_CMD:
	if (csb->sense)
	    isisidb->isis_min_rexmit_interval = GETOBJ(int,1);
	else
	    isisidb->isis_min_rexmit_interval = 0;
	break;

      case ISIS_LSPIVL_CMD:
	if (csb->sense)
	    isisidb->pacing_interval = GETOBJ(int,1);
	else
	    isisidb->pacing_interval = ISIS_DEF_PACING_INTERVAL;
	break;
    }
}

/*
 * isis_intf_commands
 *
 * Process IS-IS related interface subcommands.
 */
void isis_intf_commands (parseinfo *csb)
{
    clns_pdbtype     *pdb;
    idbtype          *idb;
    uchar            *pw;
    isisidbtype *isisidb;
    boolean    default_l1_mult, default_l2_mult;
    int number;

    idb = csb->interface;
    if (csb->protocol) {
	pdb = (clns_pdbtype *) csb->protocol;
    } else {
	pdb = find_isis_pdb();
    }

    if (csb->nvgen) {
	isisidb = get_isis_idb(pdb, idb);
	if (!isisidb)
	    return;
	switch (csb->which) {

	  case ISIS_HELLO_MULT_CMD:
	    default_l1_mult = 
		(isisidb->isis_L1_hello_multiplier == ISIS_HT_MULTIPLIER);
	    default_l2_mult = 
		(isisidb->isis_L2_hello_multiplier == ISIS_HT_MULTIPLIER);
	    nv_write(!default_l1_mult, "%s %d level-1", csb->nv_command, 
		     isisidb->isis_L1_hello_multiplier);
	    nv_write(!default_l2_mult, "%s %d level-2", csb->nv_command,
		     isisidb->isis_L2_hello_multiplier);
	    break;

	  case ISIS_METRIC_CMD:
	    nv_write((isisidb->isis_L1_metric != ISIS_DEF_METRIC),
		     "%s %d level-1", csb->nv_command,
		     isisidb->isis_L1_metric);
	    nv_write((isisidb->isis_L2_metric != ISIS_DEF_METRIC),
		     "%s %d level-2", csb->nv_command,
		     isisidb->isis_L2_metric);
	    break;

	  case ISIS_PRIO_CMD:

	    nv_write((isisidb->isis_L1_prio != ISIS_DEF_DR_PRIO),
		     "%s %d level-1", csb->nv_command,
		     isisidb->isis_L1_prio);
	    nv_write((isisidb->isis_L2_prio != ISIS_DEF_DR_PRIO),
		     "%s %d level-2", csb->nv_command,
		     isisidb->isis_L2_prio);
	    break;

	  case ISIS_CIRTYPE_CMD:
	    nv_write((isisidb->isis_circuit_type != ISIS_CIRCUIT_L1L2) &&
		     (isisidb->isis_circuit_type == GETOBJ(int,1)),
		     csb->nv_command);
	    break;

	  case ISIS_PW_CMD:
	    nv_write((boolean) isisidb->isis_L1_pw,
		     "%s %s level-1", csb->nv_command,
		     isisidb->isis_L1_pw);
	    nv_write((boolean) isisidb->isis_L2_pw,
		     "%s %s level-2", csb->nv_command,
		     isisidb->isis_L2_pw);
	    break;

	  case ISIS_HELLO_CMD:
	  case ISIS_CSNP_CMD:
	  case ISIS_RETRANS_CMD:
	  case ISIS_MIN_REXMIT_CMD:
	  case ISIS_LSPIVL_CMD:
	    isis_process_timers(csb);
	    return;

	  case ISIS_ADJFILTER_CMD:
	    nv_write(isisidb->isis_adj_filter, "%s %s %s", csb->nv_command, 
		     filter_name(isisidb->isis_adj_filter), 
		     isisidb->match_all_areas ? "match-all" : "");
	    break;

	  case ISIS_MESHGRP_CMD:
	    if (isisidb->isis_mesh_group) {
		if (isisidb->isis_mesh_group->mesh_group_number) {
		    nv_write(TRUE, "%s %d", csb->nv_command,
			     isisidb->isis_mesh_group->mesh_group_number);
		} else {
		    nv_write(TRUE, "%s blocked", csb->nv_command);
		}
	    }
	    break;

	  default:
	    printf("\nUnknown flag in isis_intf_commands %d", csb->which);
	    break;
	}
	return;

    }

    isisidb = isis_alloc_isisidb(idb, PROC_TYPE_ISIS, CLNS_PDBINDEX_ISIS0);
    if (!isisidb)
	return;
    switch (csb->which) {

      case ISIS_HELLO_MULT_CMD:
	if (csb->sense)
	    number = GETOBJ(int,1);
	else
	    number = ISIS_HT_MULTIPLIER;
	if (GETOBJ(int,2) == ISIS_CIRCUIT_L1) {
	    isisidb->isis_L1_hello_multiplier = number;
	} else {
	    isisidb->isis_L2_hello_multiplier = number;
	}
	break;

	/* 
	 * isis metric <default metric> <delay metric> 
	 *             <error metric> <expense metric> [level-1 | level-2] 
	 */
	case ISIS_METRIC_CMD:
	    if (!csb->sense) {
		if ((GETOBJ(int,5) == ISIS_CIRCUIT_L1) &&
		   (isisidb->isis_L1_metric != ISIS_DEF_METRIC)) {
		    isisidb->isis_L1_metric = ISIS_DEF_METRIC;
		    if (pdb) {
			isis_mark_L1_changed(pdb);
			isis_trigger_spf(pdb, CLNS_LEVEL1,
					 ISIS_SPF_TRIG_NEWMETRIC, NULL, TRUE);
		    }
		}
		if ((GETOBJ(int,5) == ISIS_CIRCUIT_L2) &&
		   (isisidb->isis_L2_metric != ISIS_DEF_METRIC)) {
		    isisidb->isis_L2_metric = ISIS_DEF_METRIC;
		    if (pdb) {
			isis_mark_L2_changed(pdb);
			isis_trigger_spf(pdb, CLNS_LEVEL2,
					 ISIS_SPF_TRIG_NEWMETRIC, NULL, TRUE);
		    }
		}
	    } else {

		/* 
		 * Ignore other metric values and accept level keyword 
		 * anywhere. 
		 */
		if (GETOBJ(int,5) == ISIS_CIRCUIT_L1) {
		    isisidb->isis_L1_metric = GETOBJ(int,1);
		    if (pdb) {
			isis_trigger_spf(pdb, CLNS_LEVEL1,
					 ISIS_SPF_TRIG_NEWMETRIC, NULL, TRUE);
			isis_mark_L1_changed(pdb);
		    }
		}
		if (GETOBJ(int,5) == ISIS_CIRCUIT_L2) {
		    isisidb->isis_L2_metric = GETOBJ(int,1);
		    if (pdb) {
			isis_trigger_spf(pdb, CLNS_LEVEL2,
					 ISIS_SPF_TRIG_NEWMETRIC, NULL, TRUE);
			isis_mark_L2_changed(pdb);
		    }
		}
	    }
	    break;
	
	case ISIS_PRIO_CMD:
	    if (!csb->sense) {
		if (GETOBJ(int,2) == ISIS_CIRCUIT_L1) {
		    isisidb->isis_L1_prio = ISIS_DEF_DR_PRIO;
		    isisidb->isis_L1_cur_prio = ISIS_DEF_DR_PRIO;
		}
		if (GETOBJ(int,2) == ISIS_CIRCUIT_L2) {
		    isisidb->isis_L2_prio = ISIS_DEF_DR_PRIO;
		}
	    } else {
		if (GETOBJ(int,2) == ISIS_CIRCUIT_L1) {
		    isisidb->isis_L1_prio = GETOBJ(int,1);
		    isisidb->isis_L1_cur_prio = GETOBJ(int,1);
		}
		if (GETOBJ(int,2) == ISIS_CIRCUIT_L2) {
		    isisidb->isis_L2_prio = GETOBJ(int,1);
		}
	    }
	    isis_schedule_hello(pdb, isisidb, GETOBJ(int,2));
	    isis_trigger_dr_election(pdb, isisidb, GETOBJ(int,2));
	    break;

	case ISIS_CIRTYPE_CMD:
	    if (!csb->sense) {
		isisidb->isis_circuit_type = ISIS_CIRCUIT_L1L2;
	    } else {
		isisidb->isis_circuit_type = GETOBJ(int,1);
	    }
	    break;
	    
        case ISIS_HELLO_CMD:
        case ISIS_CSNP_CMD:
        case ISIS_RETRANS_CMD:
        case ISIS_MIN_REXMIT_CMD:
        case ISIS_LSPIVL_CMD:
	    isis_process_timers(csb);
	    return;
    
	case ISIS_PW_CMD:
	    if (!csb->sense) {
		if (GETOBJ(int,1) == ISIS_CIRCUIT_L1) {
		    free(isisidb->isis_L1_pw);
		    isisidb->isis_L1_pw = NULL;
		}
		if (GETOBJ(int,1) == ISIS_CIRCUIT_L2) {
		    free(isisidb->isis_L2_pw);
		    isisidb->isis_L2_pw = NULL;
		}
	    } else {
		pw = strdup(GETOBJ(string,1));
		if (!pw) return;
		if (GETOBJ(int,1) == ISIS_CIRCUIT_L1) {
		    if (isisidb->isis_L1_pw) {
			free(isisidb->isis_L1_pw);
		    }
		    isisidb->isis_L1_pw = pw;
		}
		if (GETOBJ(int,1) == ISIS_CIRCUIT_L2) {
		    if (isisidb->isis_L2_pw) {
			free(isisidb->isis_L2_pw);
		    }
		    isisidb->isis_L2_pw = pw;
		}
	    }
	    break;

	case ISIS_ADJFILTER_CMD:
	    if (csb->sense) {
		isisidb->isis_adj_filter = 
		    clns_filter_create(GETOBJ(string,1), 
				       isisidb->isis_adj_filter);
		isisidb->match_all_areas = GETOBJ(int,1);
	    } else {
		clns_filter_destroy(isisidb->isis_adj_filter);
		isisidb->isis_adj_filter = 0;
	    }
	    break;

      case ISIS_MESHGRP_CMD:
	isis_deconfigure_mesh_group(pdb, isisidb);
	if (csb->sense) {
	    if (!isis_configure_mesh_group(pdb, isisidb, GETOBJ(int,1))) {
		printf(nomemory);
	    }
	}
	break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }

}


/*
 * isis_rtr_commands
 *
 * Process IS-IS related router subcommands.
 */
void isis_rtr_commands (parseinfo *csb)
{
    clns_pdbtype     *pdb;
    uchar            *pw;
    isis_addrsumtype *sum;

    if (csb->protocol) {
	pdb = (clns_pdbtype *) csb->protocol;
    } else {
	pdb = find_isis_pdb();
    }

    if (csb->nvgen) {
	switch (csb->which) {
	case ISIS_AREA_PW_CMD:
	    nv_write((boolean) pdb->area_password, "%s %s", csb->nv_command,
		     pdb->area_password);
	    break;

	case ISIS_DOMAIN_PW_CMD:
	    nv_write((boolean) pdb->domain_password, "%s %s", csb->nv_command,
		     pdb->domain_password);
	    break;

	case ISIS_IP_SUM_ADDR_CMD:
	    for (sum = pdb->ip_address_sumq.qhead; sum; sum = sum->next) {
		nv_write(TRUE, "%s %i %i %s", csb->nv_command, sum->sum_address,
			 sum->sum_mask, (sum->sum_level == ISIS_CIRCUIT_L1L2) ?
			 isis_L1L2_str : (sum->sum_level == ISIS_CIRCUIT_L1) ?
			 isis_L1_str : " ");
	    }
	    break;

	case ISIS_PART_AVOID_CMD:
	    nv_write(pdb->partition_avoid_on, "%s", csb->nv_command);
	    break;

	case ISIS_FORCE_HIPPITY_CMD:
	    nv_write(pdb->force_hippity, "%s", csb->nv_command);
	    break;

	case ISIS_IGN_ATT_BIT_CMD:
	    isis_ignore_attached_bit_command(csb, pdb);
	    break;

	case ISIS_NEVER_SET_ATT_CMD:
	    nv_write(pdb->never_set_att_bit, "%s", csb->nv_command);
	    break;

	case ISIS_DSP_RT_DET_CMD:
	    nv_write(pdb->long_route_display, csb->nv_command);
	    break;

 	case ISIS_SPF_IVL_CMD:
 	    isis_spf_ivl_command(csb, pdb);
 	    break;
 
 	case ISIS_LSP_GEN_IVL_CMD:
 	    isis_lsp_gen_command(TRUE, csb->sense, csb->nvgen, csb->nv_command,
				 GETOBJ(int, 1), GETOBJ(int, 2), pdb);
 	    break;

	case ISIS_IGN_LSP_ERR_CMD:
	    nv_write(pdb->ignore_cksum_errs, csb->nv_command);
	    break;

	case ISIS_LOG_ADJ_CHG_CMD:
	    isis_log_adj_changes_command(csb, pdb);
	    break;

	case ISIS_LSP_MTU_CMD:
	    isis_lspmtu_command(TRUE, csb->sense, csb->nvgen, csb->nv_command,
				GETOBJ(int,1), pdb);
	    break;

	case ISIS_UPQ_DEPTH_CMD:
	    isis_upq_depth_command(csb, pdb);
	    break;

	case ISIS_MAX_LSP_GEN_CMD:
	    isis_lsp_refresh_command(TRUE, csb->sense, csb->nvgen,
				     csb->nv_command, GETOBJ(int,1), pdb);
	    break;

	case ISIS_MAXAGE_CMD:
	    isis_lsp_lifetime_command(TRUE, csb->sense, csb->nvgen,
				      csb->nv_command, GETOBJ(int,1), pdb);
	    break;

	default:
	    printf("\nUnknown flag in isis_rtr_commands %d", csb->which);
	    break;
	}
	return;

    }

    switch (csb->which) {
        case ISIS_AREA_PW_CMD:
        case ISIS_DOMAIN_PW_CMD:
	    if (csb->sense) {
		pw = strdup(GETOBJ(string,1));
		if (!pw) return;
		if (csb->which == ISIS_AREA_PW_CMD) {
		    if (pdb->area_password) free(pdb->area_password);
		    pdb->area_password = pw;
		} else {
		    if (pdb->domain_password) free(pdb->domain_password);
		    pdb->domain_password = pw;
		}
	    } else {
		if (csb->which == ISIS_AREA_PW_CMD) {
		    free(pdb->area_password);
		    pdb->area_password = NULL;
		} else {
		    free(pdb->domain_password);
		    pdb->domain_password = NULL;
		}
	    }
	    if (csb->which == ISIS_AREA_PW_CMD) {
		isis_mark_L1_changed(pdb);
	    }
	    if (csb->which == ISIS_DOMAIN_PW_CMD) {
		isis_mark_L2_changed(pdb);
	    }
	    return;
  	    
	case ISIS_IP_SUM_ADDR_CMD:
	    isis_summary_address_command(csb);
	    break;

	case ISIS_PART_AVOID_CMD:
	    pdb->partition_avoid_on = csb->sense;
	    break;

	case ISIS_FORCE_HIPPITY_CMD:
	    if (pdb->force_hippity != csb->sense) {
		pdb->force_hippity = csb->sense;
		isis_mark_L1_changed(pdb);
		isis_mark_L2_changed(pdb);
	    }
	    break;

	case ISIS_IGN_ATT_BIT_CMD:
	    isis_ignore_attached_bit_command(csb, pdb);
	    break;

	case ISIS_NEVER_SET_ATT_CMD:
	    if (pdb->never_set_att_bit != csb->sense) {
		pdb->never_set_att_bit = csb->sense;
		if (isis_is_L2_attached(pdb)) {
		    isis_mark_L1_changed(pdb);
		}
	    }
	    break;

	case ISIS_DSP_RT_DET_CMD:
	    pdb->long_route_display = csb->sense;
	    break;

	case ISIS_SPF_IVL_CMD:
	    isis_spf_ivl_command(csb, pdb);
	    break;

	case ISIS_LSP_GEN_IVL_CMD:
 	    isis_lsp_gen_command(TRUE, csb->sense, csb->nvgen, csb->nv_command,
				 GETOBJ(int, 1), GETOBJ(int, 2), pdb);
	    break;

	case ISIS_IGN_LSP_ERR_CMD:
	    pdb->ignore_cksum_errs = (csb->sense != FALSE);
	    break;

	case ISIS_LOG_ADJ_CHG_CMD:
	    isis_log_adj_changes_command(csb, pdb);
	    break;

	case ISIS_LSP_MTU_CMD:
	    isis_lspmtu_command(TRUE, csb->sense, csb->nvgen, csb->nv_command,
				GETOBJ(int,1), pdb);
	    break;

	case ISIS_UPQ_DEPTH_CMD:
 	    isis_upq_depth_command(csb, pdb);
 	    break;

	case ISIS_MAX_LSP_GEN_CMD:
	    isis_lsp_refresh_command(TRUE, csb->sense, csb->nvgen,
				     csb->nv_command, GETOBJ(int,1), pdb);
	    break;

	case ISIS_MAXAGE_CMD:
	    isis_lsp_lifetime_command(TRUE, csb->sense, csb->nvgen,
				      csb->nv_command, GETOBJ(int,1), pdb);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }

}


/*
 * isis_add_osi_route
 *
 * Add level-1 or level-2 routes to the OSI forwarding table.
 */
void isis_add_osi_route (clns_pdbtype *pdb,
				uchar *dest,
				isis_lspdbtype *isis_paths,
				isis_lsp_parent *next_hops,
				int metric,
				int level)
{
    isis_ddbtype *ddb;
    isis_rdbtype *rdb;
    clns_ddbtype *clns_ddb;
    clns_rdbtype *clns_rdb;
    clns_adjtype *adj;
    int          hash, i, j;
    boolean      new_entry = FALSE;
    NSAP_T       system_id;
    boolean      same_spf;

    /*
     * ---------- Build Level-1 routes ----------
     */
    if (level == ISIS_CIRCUIT_L1) {
	ddb = isis_get_L1_route(pdb, dest);
	if (ddb) {
	    same_spf = (pdb->current_L1_version == ddb->route_version);

	    /* 
	     * If this metric is greater than routing table metric, don't
	     * add to routing table. This occurs only for leaf nodes.
	     */
	    if (same_spf && metric > ddb->metric) return;

	    /*
	     * If metric is better than routing table metric, replace
	     * all next-hops. Otherwise, add these next-hops to the
	     * current ones in the routing table.
	     */
	    if (!same_spf || metric != ddb->metric) {
		for (i = 0; i < ISIS_MAX_PATHS; i++) {
		    if (ddb->next_hops[i]) free (ddb->next_hops[i]);
		    ddb->next_hops[i] = NULL;
		}
		ddb->route_count = 0;
	    }
	} else {
	    new_entry = TRUE;
	    ddb = malloc_named(sizeof(isis_ddbtype), "ISIS DDB");
	    if (!ddb) return;
	}

	/* 
	 * Copy data into route entry. 
	 */
	station_copy (dest, ddb->destination);
	ddb->metric = metric;
	ddb->route_version = pdb->current_L1_version;

	/* 
	 * Copy next hops from next_hops of last node of PATHS.
	 */
	for (i = 0; next_hops[i].next_hop || !metric; i++) {
	    if (ddb->route_count == ISIS_MAX_PATHS) break;

	    /*
	     * Check if next hop is already in routing table.
	     */
	    for (j = 0; j < ddb->route_count; j++) {
		if (!(rdb = ddb->next_hops[j])) continue;
		if (next_hops[i].next_hop && 
		    (rdb->idb == next_hops[i].next_hop->idb) && 
		    nsap_equ(rdb->system_id, next_hops[i].next_hop->key)) 
		    break;
	    }
	    if (ddb->route_count && j != ddb->route_count)
		continue;
	     
	    /*
	     * Allocate new rdb structure to store with destination route.
	     */
	    rdb = malloc_named(sizeof(isis_rdbtype), "ISIS RDB");
	    if (!rdb) 
		return;
	    ddb->next_hops[ddb->route_count] = rdb;
	    adj = next_hops[i].next_hop;
	    ddb->route_count++;

	    /* 
	     * This is the case where this system is added to the routing 
	     * table. 
	     */
	    if (!adj) break;

	    station_copy (&adj->key[1], &rdb->system_id[1]);
	    rdb->system_id[0] = STATION_LENGTH;
	    rdb->snpa = adj->snpa;
	    rdb->idb = adj->idb;
	}

	/*
	 * Adding nearest level-2 router, if last transit node
	 * on PATHS has attach bit set and this system is the level-2
	 * router, add to routing table.
	 * NB. ISIS-IP and NLSP do their own search for closest L2 router.
	 */
	if (!pdb->isis_best_L2 &&
	    pdb->level == ISIS_CIRCUIT_L1 &&
	    metric &&
	    !pdb->ignore_att_bit_clns &&
	    (isis_paths->data->att & ISIS_LSP_L2_ATTACHED) &&
	    isis_paths->data->is_type == ISIS_CIRCUIT_L1L2 &&
	    station_match(isis_paths->data->lspid.system_id, dest)) {

	    /* Yup, it's the closest attached L1L2 router in the area. */

	    pdb->isis_best_L2 = ddb;
	    spf_ebuginf(pdb, FALSE, "%s is closest L2 IS for CLNS",
			station_string(dest));
	}

	if (new_entry) {

	    /*
	     * Insert in route table.
	     */
	    hash = ISIS_L1_HASH (dest);
	    ddb->next = pdb->route_table[hash];
	    pdb->route_table[hash] = ddb;
	}

	if (isis_nlsp_spf_event_debug[pdb->proc_type]) {
	    buginf("\n%s-SPF: Add %s to L1 route table, metric %d",
		   pdb->errmsg_name, station_string(ddb->destination), 
		   ddb->metric);
	    for (i = 0; i < ddb->route_count; i++) {
		rdb = ddb->next_hops[i];
		if (!rdb->system_id[0])
		    continue;
		buginf ("\n%s-SPF:   Next hop %s (%s)", 
			pdb->errmsg_name, station_string (&rdb->system_id[1]),
			(rdb->idb) ? rdb->idb->namestring : " ");
	    }
	}
	return;
    }

    /*
     * ---------- Build Level-2 routes ----------
     */
    clns_ddb = nsap_lookup(dest, MATCH_EXACT);
    if (clns_ddb) {
	
	/*
	 * If IS-IS is not as preferred as the creator of the
	 * existing route.
	 */
	if (pdb->distance > clns_ddb->distance)
	    return;
	same_spf = FALSE;

	for (i = 0; i < clns_ddb->routecount; i++) {
	    if (!(clns_rdb = clns_ddb->rdb[i]))
		continue;
	    same_spf = (clns_rdb->route_version == pdb->current_L2_version);

	    /*
	     * If metric is greater than routing table metric, do not
	     * add. 
	     */
	    if (same_spf && metric > clns_ddb->metric) return;
	}

	if (!same_spf || metric != clns_ddb->metric) {
	    clns_delete_all_rdbs(clns_ddb);
	}
    }

    /*
     * Handle the situation where area addresses are reachable through
     * this system. 
     */
    clns_rdb = NULL;
    if (!metric) {
	station_copy (pdb->systemId, &system_id[1]);
	system_id[0] = STATION_LENGTH;
	clns_rdb = clns_route_update (dest, system_id, NULL, CLNS_ROUTE_ENTRY,
		 pdb->typeflag, pdb->distance, 0, timer_no_time, metric, NULL);
	if (clns_rdb) {
	    clns_rdb->route_version = pdb->current_L2_version;
	    spf_ebuginf(pdb, FALSE, "Add %n to L2 route table, metric %d",
			dest, metric);
	}
    } else {
	for (i = 0; i < ISIS_MAX_PATHS && next_hops[i].next_hop; i++) {
	    clns_rdb = clns_route_update(dest, next_hops[i].next_hop->key, 
			  next_hops[i].next_hop->idb, CLNS_ROUTE_ENTRY, 
			  pdb->typeflag, pdb->distance, 0, timer_no_time,
			  metric, NULL);
	    if (clns_rdb) {
		pdb->num_L2_routes++;
		clns_rdb->route_version = pdb->current_L2_version;
		spf_ebuginf(pdb, FALSE, "Add %n to L2 route table, metric %d",
			    dest, metric);
	    }
	}
    }
}


/*
 * isis_append_one_prefix_ad
 *
 */
static void isis_append_one_prefix_ad (isis_info_type *item, uchar *key)
{
    uchar *one_neighbor;
    int   size;
    
    size = key[0] + 1;
    one_neighbor = ((uchar *)item) + item->length + ISIS_INFO_TYPE_OVERHEAD;
    
    /* 
     * Prefix length is stored in semi-octets.
     */
    nsap_copy(*(NSAP_T *)key, *(NSAP_T *)one_neighbor);
    *one_neighbor <<= 1;
    
    item->length += size;
}


/*
 * isis_add_one_prefix_ad
 */
void isis_add_one_prefix_ad (isis_info_type *item, uchar *key, 
			     ulong metric)
{
    isis_prefix_nbr_ad *ad;
    
    ad = (isis_prefix_nbr_ad *)&item->value;
    ad->metric.default_metric = metric;
    ad->metric.delay_metric = ISIS_METRIC_UNSUPPORTED;
    ad->metric.expense_metric = ISIS_METRIC_UNSUPPORTED;
    ad->metric.error_metric = ISIS_METRIC_UNSUPPORTED;
    
    item->code = ISIS_PREFIX_NEIGHBORS;
    item->length = sizeof (isis_metric_type);
    isis_append_one_prefix_ad(item, key);
}

/*
 * isis_add_prefix_neighbors
 *
 * Add all prefix neighbors to the LSP.
 */
void isis_add_prefix_neighbors (isis_lspcontext *context)
{
    clns_pdbtype   *pdb;
    isis_info_type *item;
    clns_adjtype  *neighbor;
    int            bucket;
    clns_ddbtype  *ddb;
    route_maptype *map;
    uchar         metric;
    
    pdb = context->pdb;
    
    /* Loop over all adjacencies.  Install the useful ones. */
    
    neighbor = NULL;
    while ((neighbor = next_neighbor(pdb->adjacency_db, NULL,
				     CLNS_PREFIX_ADJ_TYPE, 
				     neighbor, &bucket)) != NULL) {
	isisidbtype *isisidb;

	if (neighbor->state != CLNS_ADJ_UP_STATE)
	    continue;
	if (!(pdb->redistallowed & neighbor->protocol_source))
	    continue;
	ddb = nsap_lookup(neighbor->key, MATCH_EXACT);
	if (!ddb)
	    continue;
	/*
	 * Check route-map.
	 */
	if (!clns_pdb_routemap(pdb, ddb))
	    continue;

	/*
	 * Set the metric of the DECnet discard routes to 1 since DECNIS
	 * boxes do not appear to like a metric of zero. 
	 */
	metric = (neighbor->protocol_source & DECNET_DISCARD) ? 1 : 0;
	if (neighbor->idb) {
	    isisidb = get_isis_idb(pdb, neighbor->idb);
	    if (isisidb)
		metric = isisidb->isis_L2_metric;
	}

	/*
	 * Get configured metric from route-map, if any.
	 */
	map = pdb->route_map;
	if (map) {
	    if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
		!map->set_list.metric.igrp_metric) {
		metric = map->set_list.metric.value.scalar;
	    }
	}
	
	/* Create the item. */

	item = isis_new_item(context, ISIS_PREFIX_NEIGHBORS,
			     sizeof(isis_metric_type) + neighbor->key[0] + 1, 
			     NULL, NULL, TRUE);
	if (!item)
	    return;

	/* Insert prefix in LSP. */

	isis_add_one_prefix_ad(item, (char *)neighbor->key, metric);
	ADVANCE_ITEM(item, TRUE);	/* Length guaranteed nonzero */
	isis_item_done(context, item);
    }
}    

/*
 * isis_add_static_nbrs
 * Add static neighbors to ISIS adjacency database. The neigbors are obtained 
 * from the clns adjacency database. Consider only static ES and IS 
 * adjacencies.
 * This is routine is used when ISIS is enabled on an interface.
 */
void isis_add_static_nbrs(clns_pdbtype *pdb, idbtype *idb)
{
    int bucket;
    clns_adjtype *adj;

    adj = NULL;
    while ((adj = next_neighbor(pdb->adjacency_db, idb, 0, adj, &bucket))) {
	if ((adj->protocol_source & ISO_STATIC) && 
	    ((adj->adjacencyType == CLNS_ES_ADJ_TYPE) ||
	     (adj->adjacencyType == CLNS_IS_ADJ_TYPE))) {
	    clns_adjust_static_routes (adj, NULL, TRUE, TRUE);
	}
    }
}


/*
 * isis_get_L1_route
 *
 */
isis_ddbtype *isis_get_L1_route (clns_pdbtype *pdb, uchar *dest)
{

    isis_ddbtype *ddb;

    for (ddb = pdb->route_table[ISIS_L1_HASH(dest)]; ddb; ddb = ddb->next) {
	if (station_match(ddb->destination, dest)) break;
    }
    return (ddb);
}
 

/*
 * isis_route
 *
 * Perform route lookup in level-1 routing table. This function will
 * retrieve the next hop information associated with a given system id.
 *
 * The destination control block is retrieved from the level-1 database.
 * Each next hop for the ddb is examined to see if it is still valid.
 * If there is more than one next hop per ddb, it is assumed they are
 * all equal cost paths and load splitting will be performed.
 *
 * Once a viable next hop system id is determined, the associated 
 * adjacency is retrieved from the adjacency table. The adjacency entry 
 * has the relavent next hop SNPA and interface (idb).
 *
 * Returns:
 * 	NULL - Route not found.
 *      > 0  - Address of next hop adjacency (route found).
 *      = -1 - Destination is us via level-1 routing.
 */
clns_adjtype *isis_route (clns_pdbtype *pdb, NSAP_T destination)
{

    isis_ddbtype   *ddb;

    /*
     * Lookup destination in level-1 routing table.
     */
    ddb = isis_get_L1_route (pdb, get_station (destination));

    /*
     * Did not find route.
     */
    if (!ddb || pdb->current_L1_version != ddb->route_version) {
	return (NULL);
    }

    /*
     * Any route in level-1 table with route_count 0 means this
     * system. Destination is us via level-1 routing.
     */
    if (!ddb->metric) return ((clns_adjtype *) -1);

    /*
     * Found destination - build route cache and get adjacency.
     */
    return (isis_make_route_cache (destination, ddb));

}

/*
 * isis_make_route_cache
 *
 * Build routing cache from IS-IS next hops entries.
 */
clns_adjtype *isis_make_route_cache (NSAP_T destination, isis_ddbtype *ddb)
{

    isis_rdbtype   *rdb;
    clns_adjtype   *adj, *save_adj;
    clns_cachetype *cache;
    int            i;
    
    cache = NULL;
    save_adj = NULL;

    for (i = 0; i < ddb->route_count; i++) {
	rdb = ddb->next_hops[i];
	if (!rdb) {
	    continue;
	}
	adj = find_adjacency(clns_adjacency_db, rdb->system_id, FALSE,
			     rdb->idb, 0);
	if (adj) {
	    if (!save_adj)
		save_adj = adj;
	    if (adj->idb->clns_enabled) {
		if (!i) {
		    cache = clns_make_route_cache(destination, adj);
		} else {
		    clns_make_cache_adj(cache, adj);
		}
	    }
	}
    }
    return (save_adj);
}

/*
 * isis_show_route
 *
 * Process the "show isis route" command.
 *
 */
static const char route_hdr[] = {"\nIS-IS Level-1 Routing Table - Version %d"};
static const char route_hdr2[] = {"\nSystem Id       Next-Hop        SNPA            Interface   Metric  State"};

void show_isis_route (parseinfo *csb)
{

    clns_pdbtype *pdb;
    isis_ddbtype *ddb, *nddb;
    isis_rdbtype *rdb;
    int          i, j;
    boolean      banner;

    banner = FALSE;
    pdb = find_isis_pdb();
    if (!pdb) return;
    automore_enable(NULL);
    for (i = 0; i < CLNS_NETHASHLEN; i++) {
	for (ddb = pdb->route_table[i]; ddb;) {
	    mem_lock(ddb);
	    for (j = 0; j < ddb->route_count; j++) {
		if (!(rdb = ddb->next_hops[j])){
		    continue;
		}
		mem_lock(rdb);
		if (!banner) {
		    printf(route_hdr, pdb->current_L1_version);
		    printf(route_hdr2);
		    banner = TRUE;
		}
		printf("\n%16s%16s%16s%12s%8d%6s%s", 
		       (!j) ? clns_stationid_lookup(pdb, ddb->destination) :
		       " ", 
		       clns_stationid_lookup(pdb, &rdb->system_id[1]), 
		       (rdb->idb) ? snpa_string(&rdb->snpa) : "--",
		       (rdb->idb) ? rdb->idb->short_namestring : "--", ddb->metric,
		       (pdb->current_L1_version == ddb->route_version) ?
		       "Up" : "Down", 
		       (!j && ddb == pdb->isis_best_L2) ? "L2-IS" : " ");
		free(rdb);		/* unmem_lock */
	    }
	    nddb = ddb->next;
	    free(ddb);
	    ddb = nddb;
	}
    }
    automore_disable();
}

/*
 * isis_clear_all
 *
 * Restart ISIS by wiping out the LSP database and all adjacencies.
 *
 * OBJ(string,1) = process tag, or NULL
 */
void isis_clear_all (parseinfo *csb)
{
    clns_pdbtype *pdb;
    char *name;

    name = GETOBJ(string,1);
    if (strlen(name) == 0)
	name = NULL;
    pdb = clns_find_router_name(name);
    if (pdb) {
	isis_restart(pdb);
    } else {
	printf("\n%%Can't find ISIS process");
    }
}

/*
 * isis_age_L1_routing_table
 *
 * Age the L1 routing table.
 */
void isis_age_L1_routing_table (clns_pdbtype *pdb)
{
    isis_ddbtype *ddb, *next, *prev;
    int i, j;
				
    if (pdb->L1_routes_changed) {
	for (i = 0; i < CLNS_NETHASHLEN; i++) {
	    prev = NULL;
	    for (ddb = pdb->route_table[i]; ddb; prev = ddb, ddb = next) {
		next = ddb->next;
		if (pdb->current_L1_version == ddb->route_version) {
		    if (!ddb->route_count) continue;
		    
		    /*
		     * Check equal-cost case. This is the case where there
		     * were more than one next hop to a destination and one
		     * of them became unreachable. The route count reflects
		     * the correct number of reachable next hops.
		     */	
		    for (j = ddb->route_count; j < ISIS_MAX_PATHS; j++) {
			if (!ddb->next_hops[j]) continue;
			free(ddb->next_hops[j]);
			ddb->next_hops[j] = NULL;
		    }
		    continue;
		}

		/*
		 * Delete route table entry - first delete next hop memory than
		 * routing table entry.
		 */
		for (j = 0; j < ddb->route_count; j++) {
		    if (!ddb->next_hops[j]) continue;
		    free(ddb->next_hops[j]);
		}

		ddb->next = NULL;
		free(ddb);

		/*
		 * Delink from route table.
		 */
		if (!prev) 
		    pdb->route_table[i] = next;
		else 
		    prev->next = next;
		ddb = prev;
	    }
	}
	pdb->L1_routes_changed = FALSE;
    }
}

/*
 * isis_do_partition_avoidance
 *
 * Figure out whether or not partition avoidance is active.
 */
void isis_do_partition_avoidance (clns_pdbtype *pdb)
{
    /*
     * If partition avoidance is enabled, look for L1 and ES
     * adjacencies.  If none are present, set the pdb flag for
     * ISIS to notice.
     */
    if (pdb->partition_avoid_on) {
	int bucket;
	clns_adjtype *adj;
	pdb->partition_avoid_ok = TRUE;
	adj = NULL;
	while ((adj = next_neighbor(pdb->adjacency_db, NULL, 0, adj,
				    &bucket))) {
	    if (adj->adjacencyType == CLNS_ES_ADJ_TYPE ||
		adj->adjacencyType == CLNS_IS_ADJ_TYPE ||
		adj->adjacencyType == CLNS_L1_IS_ADJ_TYPE || 
		(adj->adjacencyType == CLNS_L2_IS_ADJ_TYPE &&
		 !adj->L2only)) {
		pdb->partition_avoid_ok = FALSE;
		break;
	    }
	}
    } else {
	pdb->partition_avoid_ok = FALSE;
    }
}

/*
 * isis_track_interface_state
 *
 * Track interface state in the ISISIDB for this interface.
 */
void isis_track_interface_state (clns_pdbtype *pdb, idbtype *idb)
{
    isisidbtype *isisidb;
    boolean new_state;

    isisidb = get_isis_idb(pdb, idb);
    new_state = interface_up(idb);

    if (!isisidb)
	return;

    /*
     * Note the new state.  Manipulate the SRM mask.  Rebuild the
     * LSPs.
     */
    if (isisidb->isis_interface_up != new_state) {
	isisidb->isis_interface_up = new_state;
	isis_manipulate_srm_mask(pdb, isisidb);
	isis_mark_L1_changed(pdb);
	isis_mark_L1_dr_changed(pdb, isisidb);
    }
}

/*
 * isis_update_interface_state
 *
 * Update the interface state.
 *
 * Done when the interface is first enabled for ISIS.  This is effectively
 * identical to a route_adjust.
 */
void isis_update_interface_state (idbtype *idb)
{
    clns_pdbtype *pdb;

    pdb = find_isis_pdb();
    if (pdb) {
	isis_track_interface_state(pdb, idb);
    }
}

/*
 * isis_adjacency_holddown
 *
 * Start a holddown on adjacency establishment.  We need to ensure that
 * all neighbor adjacencies time out before we start squawking hellos
 * again.
 *
 * If there are no adjacencies on any interface, we won't bother with the
 * holddown.  This keeps us from going into holddown when we boot up the
 * first time.
 */
void isis_adjacency_holddown (clns_pdbtype *pdb)
{
    ulong maxhold;
    idbqueuetype *idbqueue;
    idbtype *idb;
    isisidbtype *isisidb;
    ulong adjacency_count;
    ulong level_ix;

    maxhold = 0;
    adjacency_count = 0;

    /* Walk each interface to find the max holding time. */

    for (idbqueue = pdb->idbqueue; idbqueue; idbqueue = idbqueue->next) {
	idb = idbqueue->idb;
	isisidb = get_isis_idb(pdb, idb);
	if (isisidb && isisidb->isis_running) {
	    maxhold = max(maxhold, isisidb->isis_L1_hello_interval *
			  isisidb->isis_L1_hello_multiplier);
	    maxhold = max(maxhold, isisidb->isis_L2_hello_interval *
			  isisidb->isis_L2_hello_multiplier);
	    for (level_ix = 0; level_ix < ISIS_NUM_LEVELS; level_ix++) {
		adjacency_count += isisidb->isis_adj_count[level_ix];
	    }
	}
    }

    /*
     * Now hold down for the max time, plus 20 seconds (to take care of
     * the sloppy timer handling in the older implementation) if there
     * were any adjacencies.
     */
    if (adjacency_count) {
	TIMER_START(pdb->adj_holddown_timer, maxhold + (20 * ONESEC));
    }
}

/*
 * isisclns_init
 *
 * Called at subsystem link time
 */
static void isisclns_init (subsystype *subsys)
{
    reg_add_isis_add_osi_route(isis_add_osi_route, "isis_add_osi_route");
    reg_add_isis_add_prefix_neighbors(isis_add_prefix_neighbors,
				      "isis_add_prefix_neighbors");
    reg_add_isis_age_L1_routing_table(isis_age_L1_routing_table,
				      "isis_age_L1_routing_table");
    reg_add_isis_do_partition_avoidance(isis_do_partition_avoidance,
					"isis_do_partition_avoidance");
    reg_add_ip_isis_command_parse(isis_rtr_commands, "isis_rtr_commands");
    reg_add_isis_update_interface_state(isis_update_interface_state,
					"isis_update_interface_state");
}

/*
 * ISIS-CLNS subsystem header
 */
#define ISISCLNS_MAJVERSION 1
#define ISISCLNS_MINVERSION 0
#define ISISCLNS_EDITVERSION  1

SUBSYS_HEADER(isisclns, ISISCLNS_MAJVERSION, ISISCLNS_MINVERSION,
	      ISISCLNS_EDITVERSION, isisclns_init, SUBSYS_CLASS_PROTOCOL,
	      "req: isis, clns",
	      "seq: isis");
