/* $Id: ospf_parse.c,v 3.11.8.16 1996/08/16 22:59:25 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_parse.c,v $
 *------------------------------------------------------------------
 * ospf_parse.c -- configure OSPF parameters
 *
 * November 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_parse.c,v $
 * Revision 3.11.8.16  1996/08/16  22:59:25  rbadri
 * CSCdi66287:  LANE: Clean up IPX/OSPF releated initializaion
 * Branch: California_branch
 * Function to setup broadcast flag for LANE.
 *
 * Revision 3.11.8.15  1996/08/16  22:11:00  myeung
 * CSCdi62650:  OSPF NSSA does not flush type 5 LSAs
 * Branch: California_branch
 *
 * Revision 3.11.8.14  1996/08/05  09:00:05  myeung
 * CSCdi64894:  ospf nssa type-7 not translated after clear ip ospf red on
 * abr
 * Branch: California_branch
 *
 * Revision 3.11.8.13  1996/07/19  07:27:12  myeung
 * CSCdi63444:  OSPF router should not be ABR if it dont see neighbor from
 * backbone
 * Branch: California_branch
 *
 * Revision 3.11.8.12  1996/07/18  08:36:17  myeung
 * CSCdi62058:  DC-feature : loses neighbor with non DC code
 * Branch: California_branch
 * - Add ospf_dc_down_check() to bring P2P DC interface
 *   state to IFS_DOWN when nbr is lost
 *
 * Revision 3.11.8.11  1996/06/28  23:19:19  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.8.10  1996/06/28  22:15:05  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Remove IDB_OSPF_VL, which is not necessary, to preserve status bit
 *
 * Revision 3.11.8.9  1996/06/28  06:04:21  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.11.8.8  1996/06/18  01:47:57  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.11.8.7  1996/06/17  23:34:12  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.11.8.6  1996/06/05  07:02:21  myeung
 * CSCdi58092:  ospf crash in get_interval when unconfiguring ospf
 * Branch: California_branch
 *
 * Revision 3.11.8.5  1996/05/23  12:06:56  klmj
 * CSCdi45519:  OSPF: Incorrect metric calculation for External Type
 * 1 routes (note:  this also fixes CSCdi44939)
 * Branch: California_branch
 *
 * Revision 3.11.8.4  1996/05/21  09:53:28  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.8.3  1996/04/24  01:29:00  myeung
 * CSCdi54714:  no-redistribution does not work in NSSA
 * Branch: California_branch
 *
 * Revision 3.11.8.2  1996/04/16  19:02:00  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.11.8.1  1996/03/18  20:28:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.4  1996/03/16  06:57:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.3  1996/03/13  01:39:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  09:50:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  01:03:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/02/29  10:33:31  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.10  1996/02/07  16:13:16  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.9  1996/02/04  06:07:38  myeung
 * CSCdi48132:  Extract additional OSPF specific information out of pdbtype
 *
 * Revision 3.8  1996/02/01  06:05:53  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/25  12:40:11  myeung
 * CSCdi47498:  Clean up OSPF redistribution
 *
 * Revision 3.6  1996/01/22  06:14:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/05  10:17:46  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/08  04:55:49  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/11/17  17:36:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:09:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:06:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/26  11:35:50  myeung
 * CSCdi40729:  IP OSPF Hello Interval is not retained after reload
 *
 * Revision 2.6  1995/08/31  00:51:42  myeung
 * CSCdi39540:  Crash in display_ospf_vidb()
 *
 * Revision 2.5  1995/08/07  19:19:47  myeung
 * CSCdi26731:  OSPF is turned off on an interface when secondary address
 * is removed
 *
 * Revision 2.4  1995/06/28  09:27:52  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/23  18:47:58  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/19  01:08:17  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:09:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "../os/old_timer_callbacks.h"
#include "mgd_timers.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "route.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../if/network.h"
#include "../if/if_vidb.h"
#include "logger.h"
#include "../util/tree.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"
#include "ospf_public.h"
#include "parser.h"
#include "parser_defs_ospf.h"
#include "../parser/parser_defs_tcpip.h"
#include "../parser/parser_defs_parser.h"
#include "login.h"

pdbtype *ospf_pdbs_ptr = NULL;
static ulong ospf_vl_count = 0;
static void attach_virtual_interface(pdbtype *, areatype *, parseinfo *);
static void ospf_if_check(pdbtype *, areatype *, idbtype *);
ospf_traffic_t ospf_traffic;


/*
 * l_initqueue
 * Initialize list
 */
static void l_initqueue (l_queue *queue)
{
    queue->count = 0;
    queue->head = NULL;
    queue->tail = &queue->head;
}


/*
 * l_enqueue
 * Insert an element into the end of list.
 */
static l_link *l_enqueue (l_queue *queue, l_link *link) 
{
    if (!queue->head) {
        queue->head = link;
	link->prev = &queue->head;
    } else {
        *queue->tail = link;        
	link->prev = queue->tail;
    }
    link->next = NULL;
    queue->tail = &link->next;
    queue->count++;
    return link;
}


/*
 * l_delete
 * Delete an element from the list
 */
static l_link *l_delete (l_queue *queue, l_link *link) 
{
    *link->prev = link->next;
    if (link->next)
        link->next->prev = link->prev;
    if (queue->tail == &link->next)
        queue->tail = link->prev;
    link->prev = NULL;
    link->next = NULL;
    queue->count--;
    return link;
}

/*
 * clean_router_route
*/
void clean_router_route (pdbtype *pdb) 
{
    rtr_route *route, *tem;
    int i;

    for (i = 0; i < HTBLSIZE; i++) {
	route = (pdb->ospf_routes)[i];
	for ( ; route; route = tem) {
	    tem = route->rt_next;
	    free(route);
	}
	(pdb->ospf_routes)[i] = NULL;
    }
} /* end of clean_router_route */

/*
 * ospf_delete_area
 */

static void ospf_delete_area (pdbtype *pdb, areatype *area)
{
    areatype *temp;
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;

    for (temp = (areatype *)&(pdb->ospf_area_list); temp && temp->ar_next;
	 temp = temp->ar_next) {
	if (temp->ar_next == area) {
	    temp->ar_next = area->ar_next;
	    area->ar_next = NULL;
	    pdb->area_count--;
	    if (area->ar_stub)
		ospf_pdb->ospf_stub_area_count--;
	    else if (area->ar_nssa)
		ospf_pdb->ospf_nssa_area_count--;
	    else
		ospf_pdb->ospf_normal_area_count--;
	    break;
	}
    }
    mgd_timer_stop(&area->ar_master_timer);
    mgd_timer_stop(&area->ar_hello_master_timer);
    if (area->ar_id == BACKBONE_AREA_ID)
	pdb->ospf_backbone = NULL;

    clean_router_route(pdb);
    table_prune_pdb(pdb);
    ospf_force_spf(pdb, NULL);
    pdb->ospf_flag |= OSPF_CHECK_ABR;
} /* end of ospf_delete_area */

/*
 * delete_range
 */

boolean delete_range (
    areatype *area,
    ipaddrtype address,
    ipaddrtype mask)
{
    addr_range *temp;
    addr_range *temp1;

    if (address) {
	temp = (addr_range *)&(area->ar_range_list);
	for (; temp->net_next; temp = temp->net_next) {
	    if ((temp->net_next->net_address == address) 
	                   && (temp->net_next->net_mask == mask)) {
		temp1 = temp->net_next;
		temp->net_next = temp1->net_next;
		free(temp1);
		return(TRUE);
	    }
	}
    }
    return(FALSE);
} /* end of delete_range */

/*
 * ospf_create_area
 */

static areatype *ospf_create_area (
    ulong area_id,
    pdbtype *pdb)
{
    areatype *area, *prev_ar, *cur_ar;
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;
    area = malloc(sizeof(areatype));
    if (area == NULL)
	return(NULL);

    mgd_timer_init_parent(&area->ar_hello_master_timer,
			  &ospf_pdb->ospf_hello_master_timer);
    mgd_timer_init_parent(&area->ar_master_timer,
			  &ospf_pdb->ospf_update_timer);    
    mgd_timer_init_leaf(&area->ar_rebuild_rtr_timer,
			&area->ar_master_timer,
			OSPF_AR_REBUILD_RTR_TIMER, area, FALSE);
    mgd_timer_init_leaf(&area->ar_delay_timer,
			&area->ar_master_timer,
			OSPF_AR_DELAY_TIMER, area, FALSE);
    mgd_timer_init_leaf(&area->ar_update_timer,
			&area->ar_master_timer,
			OSPF_AR_UPDATE_TIMER, area, FALSE);
    mgd_timer_init_leaf(&area->ar_exlock_timer,
			&area->ar_master_timer,
			OSPF_AR_EXLOCK_TIMER, area, FALSE);
    mgd_timer_init_leaf(&area->ar_sumlock_timer,
			&area->ar_master_timer,
			OSPF_AR_SUMLOCK_TIMER, area, FALSE);
    mgd_timer_init_leaf(&area->ar_age_timer,
			&area->ar_master_timer,
			OSPF_AR_AGE_TIMER, area, FALSE);
    area->ar_nssa_flood_list.q_first = NULL;
    area->ar_nssa_flood_list.q_last = NULL;
    area->ar_id = area_id;
    area->ar_pdb_ptr = pdb;
    area->ar_stub = FALSE;
    area->ar_stub_nosum = FALSE;
    area->ar_nssa = FALSE;
    area->ar_options =  (OPTION_E_BIT | pdb->ospf_tos_option);
    area->ar_next = NULL;
    area->ar_def_stub_default_cost = STUB_DEFAULT_COST;
    area->ar_stub_default_cost = STUB_DEFAULT_COST;
    area->ar_message_digest_seq = 0;

    /* Now allocale memory for DATABASE */
    area->ar_lsdbase[0] = RBTreeCreate("OSPF", "OSPF", "Stub LSA", NULL, 
				       &ospf_debug_tree);
    area->ar_lsdbase[1] = RBTreeCreate("OSPF", "OSPF", "Router LSA", NULL, 
				       &ospf_debug_tree);
    area->ar_lsdbase[2] = RBTreeCreate("OSPF", "OSPF", "Network LSA", NULL, 
				       &ospf_debug_tree);
    area->ar_lsdbase[3] = RBTreeCreate("OSPF", "OSPF", "Summary ASBR LSA", NULL,
				       &ospf_debug_tree);
    area->ar_lsdbase[4] = RBTreeCreate("OSPF", "OSPF", "Summary LSA", NULL, 
				       &ospf_debug_tree);
    area->ar_lsdbase[5] = pdb->ospf_ex_lsdbase;
    area->ar_lsdbase[6] = NULL;
    area->ar_lsdbase[7] = RBTreeCreate("OSPF", "OSPF", "Type-7 AS LSA", NULL,
				       &ospf_debug_tree);

    if ((area->ar_lsdbase[0] == NULL) || (area->ar_lsdbase[1] == NULL) ||
	(area->ar_lsdbase[2] == NULL) || (area->ar_lsdbase[3] == NULL) ||
	(area->ar_lsdbase[4] == NULL) || (area->ar_lsdbase[7] == NULL)) { 
	goto area_error;
    }
    mgd_timer_start(&area->ar_age_timer, AGE_TIME*ONESEC);
    /* 
     * Insert the area, sorted by area ID
     */
    prev_ar = NULL;
    for (cur_ar = pdb->ospf_area_list; cur_ar; cur_ar = cur_ar->ar_next) {
	if (area->ar_id > cur_ar->ar_id) {
	    prev_ar = cur_ar;
	    continue;
	}
	break;
    }
    if (prev_ar) {
	area->ar_next = prev_ar->ar_next;
	prev_ar->ar_next = area;
    } else {
	area->ar_next = pdb->ospf_area_list;
	pdb->ospf_area_list = area;
    }
    if (area->ar_id == BACKBONE_AREA_ID)
	pdb->ospf_backbone = area;
    pdb->area_count++;
    ospf_pdb->ospf_normal_area_count++;
    pdb->ospf_flag |= OSPF_CHECK_ABR|OSPF_CHECK_INDICATION;
    return(area);
  area_error :
    RBTreeDestroy(area->ar_lsdbase[0], &ospf_debug_tree);
    RBTreeDestroy(area->ar_lsdbase[1], &ospf_debug_tree);
    RBTreeDestroy(area->ar_lsdbase[2], &ospf_debug_tree);
    RBTreeDestroy(area->ar_lsdbase[3], &ospf_debug_tree);
    RBTreeDestroy(area->ar_lsdbase[4], &ospf_debug_tree);
    RBTreeDestroy(area->ar_lsdbase[7], &ospf_debug_tree);
    free(area);
    return NULL;
} /* end of ospf_create_area */

/*
 * ospf_network_command
 * Called via pdb when a 'network' command is added or removed.
 */
void ospf_network_command (pdbtype *pdb, ipaddrtype address, ipaddrtype mask,
			   uchar flags, ulong area_id, int area_id_format, 
			   boolean sense)
{
    ndbtype *ndb;

    ndb = net_lookup(address, FALSE);
    set_ndb_range_flags(ndb, pdb, address, mask, FALSE, sense);
    ospf_parse_range(pdb, address, mask, area_id, area_id_format, sense);
}

/*
 * ospf_remove_network
 *
 * Remove specified network, mask and area_id entry from the pdb.
 * No error code is returned. If the entries are not found, this function
 * is a no op.
 */
static inline void ospf_remove_network (pdbtype *pdb,
					ipaddrtype address,
					ipaddrtype mask,
					ulong area_id)
{
    short i;

    for (i = 0; i < pdb->netcount; i++) {
        if ((pdb->networks[i] == address) &&
            (pdb->network_mask[i] == mask) &&
            (pdb->network_info[i] == area_id)) {
            pdb->netcount--;
            for (; i < pdb->netcount; i++) {
                pdb->networks[i] = pdb->networks[i+1];
                pdb->networkflags[i] = pdb->networkflags[i+1];
                pdb->network_mask[i] = pdb->network_mask[i+1];
                pdb->network_info[i] = pdb->network_info[i+1];
            }
            return;
        }
    }
}       /* end of ospf_remove_network */

/*
 * ospf_reorder_network
 *
 * This function moves all the network/mask/area_id entries in the pdb
 * starting at index up and then inserts the network/mask/area_id at the
 * index entry.
 */
static inline void ospf_reorder_network (pdbtype *pdb,
					 ipaddrtype address,
					 ipaddrtype mask,
					 ulong area_id,
					 short index)
{
    short i;

    for (i = pdb->netcount-1; i > index; i--) {
        pdb->networks[i] = pdb->networks[i-1];
        pdb->network_mask[i] = pdb->network_mask[i-1];
        pdb->network_info[i] = pdb->network_info[i-1];
    }

    pdb->networks[index] = address;
    pdb->network_mask[index] = mask;
    pdb->network_info[index] = area_id;

    return;
}       /* end of ospf_reorder_network */

/*
 * ospf_check_superset_network
 *
 * This function checks if a superset of the specified network/mask has
 * already been configured. If a superset is found, its index in the pdb
 * is returned in index, and TRUE is returned. If no superset is found, then
 * index is undefined, and FALSE is returned.
 *
 * A superset is defined as an entry whose mask is smaller than the specified
 * mask and the network entry equals the specified address & the entry's mask.
 *
 * For example:
 *      171.69.0.0 0.0.255.255 is a superset of 171.69.232.0 0.0.0.255
 *      171.0.0.0 0.255.255.255 is a superset of 171.69.0.0 0.0.255.255
 */
static inline boolean ospf_check_superset_network (pdbtype *pdb,
                                                   ipaddrtype address,
                                                   ipaddrtype mask,
                                                   short *index)
{
    short i;

    for (i = 0; i < pdb->netcount; i++) {
        if ((mask > pdb->network_mask[i]) &&
            (pdb->networks[i] == (address & pdb->network_mask[i]))) {
            *index = i;
            return (TRUE);
        }
    }
    return (FALSE);
}       /* end of ospf_check_superset_network */
      
/*
 * ospf_parse_range
 * Common code for "network" and commands.
 */

void ospf_parse_range (
    pdbtype *pdb,
    ipaddrtype address,
    ipaddrtype mask,
    ulong area_id,
    int area_id_format,
    boolean sense)
{
    areatype 	*area;
    idbtype	*idb;
    idbtype	*temp;
    list_queue src_flood;
    boolean    attach_failed = TRUE;
    boolean    superset_configured = FALSE;
    short      network_index;

    /* lookup the area */
    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	if (area->ar_id == area_id)
	    break;
    }
    if (sense) {
	/* create an area if needed */
 	if (area == NULL) {
	    area = ospf_create_area(area_id, pdb);
	    if (area == NULL) {
		buginf("\nOSPF: No memory for area structure");
		return;
	    }
	}

	/* keep track on the area id display format */
	if (area_id_format == OSPF_AREA_ID_FORMAT_IP)
	    sprintf(area->ar_id_str, "%i", area_id);
	else if (area_id_format == OSPF_AREA_ID_FORMAT_INT)
	    sprintf(area->ar_id_str, "%u", area_id);
	/*
	 * look for all the interfaces in this range
	 * be good to unnumbered interfaces.
	 */
	FOR_ALL_SWIDBS(idb) {
            ospf_idb_info *ospf_info;

            if ((idb->ip_address &&
                 ((idb->ip_address & mask) == (address & mask))) ||
                (idb->ip_unnumbered &&
		 ((idb->ip_unnumbered->ip_address & mask) ==
		  (address & mask)))) {
                if (ospf_attach_interface(pdb, area, idb, TRUE)){
                    attach_failed = FALSE;

                    ospf_info = idb->ospf_info;
                    ospf_info->oi_network = address;
                    ospf_info->oi_mask = mask;
                }
            }
	}

        /*
         * Check if a superset of this network/mask pair has been configured
         */
        superset_configured = ospf_check_superset_network(pdb, address, mask, 
                                                          &network_index);

        if (attach_failed && (area_id_format != OSPF_AREA_ID_FORMAT_IGNORE)) {
            if (superset_configured) {
                /*
                 * In this case, we failed to attach an interface to this
                 * network entry. Since  a superset of this network entry
                 * has already been defined, this entry will never be active.
                 * We remove the network/mask from the pdb which was added
                 * by network_finish, so that it is not NVGEN'd
                 */
                ospf_remove_network(pdb, address, mask, area_id);

                printf("\n%% OSPF: \"network %i %i area %s\" is ignored."
		       " It is a subset of a previous entry.\n",
		        address, ~mask, area->ar_id_str);
		return;
	    }
        } else { /* attach succeeded */
            if (superset_configured) {
                /*
                 * A superset has been configured for this network/mask pair.
                 * We need to rearrange the entries in the pdb, so that the
                 * new entry goes in before the superset. This ensures that
		 * the network entries are NVGEN'd properly.
                 */
                ospf_reorder_network(pdb, address, mask,
				     area_id, network_index);
            }
        }
        
	if (pdb->running == FALSE) {
	    pdb->pid = cfork ((forkproc *)pdb->process, (long)pdb, 0, 
			      pdb->pname, 0);
 	    pdb->hello_pid = cfork ((forkproc *)pdb->ospf_hello_process, 
				    (long)pdb, 0, pdb->hello_pname, 0);
	    pdb->running = TRUE;
	    return;
	}
    } else {
	/* delete the range */
	if (area == NULL)
	    return;
	for (idb = area->ar_if_list; idb ; idb = temp)  {
	    temp = idb->ospf_info->next;
            /*
             * Only detach the interface if the network, mask and area id
             * match exactly.
             */
            if ((idb->ospf_info->oi_network == address) &&
                (idb->ospf_info->oi_mask == mask) &&
                (idb->ospf_info->area_ptr->ar_id == area_id)) {
                ospf_attach_interface(pdb, area, idb, FALSE);
            }
	}
    }
    if (pdb->running) {
	src_flood.q_first = src_flood.q_last = NULL;
	(void)build_rtr_lsa(area, &src_flood, FALSE);
	flood_area(area, &src_flood, NULL, NULL);
	if (pdb->ospf_area_brtr) {
	    for (area = pdb->ospf_area_list;
		 (area && (area->ar_id != BACKBONE_AREA_ID));
		 area = area->ar_next);
	    if (area)
		spf(area, SPF_SCHD_RTR | SPF_SCHD_NET | SPF_SCHD_SNET
		    | SPF_SCHD_ASB , 0);
	}
    }
    return;
} /* end of ospf_parse_range */


/*
 * ospf_if_create_info
 * Create the ospf specific idb info and attach it to the idb.
 */
static inline ospf_idb_info *ospf_if_create_info (idbtype *idb) {
    ospf_idb_info *ospf_info;
    if ((ospf_info = malloc(sizeof(ospf_idb_info))) == 	NULL) {
	 return NULL;
    }
    idb->ospf_info = ospf_info;
    l_initqueue(&ospf_info->md_key_q);
    return ospf_info;
}


/*
 * ospf_if_valid_type
 * Check if the network type is valid for this interface
 */
static inline boolean ospf_if_valid_type (idbtype *idb, ushort type) {
    hwidbtype *hwidb = idb->hwptr;

    if (hwidb->status & IDB_VIRTUAL) {
        if ((hwidb->type == IDBTYPE_LB) && 
	    (type != IF_LOOPBACK)) {
	    return FALSE;
	}
	if ((hwidb->type == IDBTYPE_LEX) &&
	    (type != IF_NONBROADCAST) && (type != IF_BROADCAST))
	    return FALSE;
    }
    return TRUE;
}


/*
 * ospf_auth_key_nvgen
 * Perform nvgen for key. Key could be generated in encrypted form
 * depending on encrypt_type.
 */
static void ospf_auth_key_nvgen (uchar *encrypt_type, uchar *key, uchar len, 
			  uchar *encrypt_key)
{
    int i;
    if ((*encrypt_type == ENCRYPT_NONE) && !password_encryption_enable) {
	for (i = 0; i < len; i++) {
	    if (key[i] != '\0')
		nv_add(TRUE, "%c", key[i]);
	}
    } else {
        /* Encryption has been enabled, but the password is
	 * still in plain-text.  We need to encrypt it before
	 * writing it out, if it's non-null
	 */
	         
        if ((*encrypt_type == ENCRYPT_NONE) && encrypt_key[0] && 
	    password_encryption_enable) {
	    uchar temppassword[40];
	    /* we have never written this out encrypted so do so */
	    memset(temppassword, 0, sizeof(temppassword));
	    *encrypt_type = encrypt(encrypt_key, temppassword, 0, 40);
	    strcpy(encrypt_key, temppassword);
	}
	nv_add(encrypt_key[0], 
	       "%d %s", *encrypt_type, encrypt_key);
    }
}


/*
 * ospf_set_auth_key
 * This function is used to figure out both the cleartext key and 
 * the encrypted key according to the encrypt type (could be cleartext
 * too when encrypt type is NONE)
 */
static void ospf_set_auth_key (uchar in_type, uchar *in_key, 
			uchar *encrypt_type, uchar *key,
			uchar len, uchar *encrypt_key)
{
    uchar temppassword[40];
    int i;

    *encrypt_type = in_type;
    strcpy(encrypt_key, in_key);
    if (*encrypt_type == ENCRYPT_NONE)
        encrypt_key[len] = '\0';
    if ((*encrypt_type == ENCRYPT_NONE) && password_encryption_enable) {
        memset(temppassword, 0, 40);
	*encrypt_type = encrypt(encrypt_key, temppassword, 0, 40);
	strcpy(encrypt_key, temppassword);
    } else {
        if (*encrypt_type == ENCRYPT_TEMP) {
	    /* Recover the deencrypted key */
	    decrypt(encrypt_key, temppassword, 40);
	    in_key = temppassword;
	}
    }
    for (i= 0; i < len ; i++)
        key[i] = '\0';
    for (i= 0; (i < len && *in_key != '\0'); i++)
        key[i] = *in_key++;
}


/* 
 * ospf_get_md_key
 * Get the md key given the key id
 */
ospf_md_key *ospf_get_md_key (ospf_idb_info *ospf_info, uchar key_id)
{
    ospf_md_key *md_key;
    md_key = (ospf_md_key *) (ospf_info ? ospf_info->md_key_q.head : NULL);
    while (md_key) {
        if (md_key->id == key_id)
	    break;
	md_key = (ospf_md_key *) md_key->link.next;	    
    }
    return md_key;
}


/*
 * ospf_get_youngest_md_key
 * Get the youngest md key
 */
ospf_md_key *ospf_get_youngest_md_key (ospf_idb_info *ospf_info)
{
    ospf_md_key *md_key, *youngest_md_key;
    md_key = (ospf_md_key *) (ospf_info ? ospf_info->md_key_q.head : NULL);
    youngest_md_key = NULL;
    /*
     * The youngest key is at the end of the queue
     */
    while (md_key) {
	youngest_md_key = md_key;
	md_key = (ospf_md_key *) md_key->link.next;	    
    }
    return youngest_md_key;
}


/*
 * ospf_set_md_key
 * Add or delete a message digest key
 */
static void ospf_set_md_key (ospf_idb_info *ospf_info, uchar key_id, uchar in_type,
		      char *in_key, boolean sense)
{ 
    ospf_md_key *md_key;

    md_key = ospf_get_md_key(ospf_info, key_id);
    if (sense) {
        if (md_key) {
	    printf("\nOSPF: Key %lu already exists", key_id);
	    return;
	}
	md_key = malloc(sizeof(ospf_md_key));
	if (!md_key) {
	    return;
	}
	md_key->id = key_id;
	GET_TIMESTAMP(md_key->start_time);
	ospf_set_auth_key(in_type, in_key, &md_key->encrypt_type, 
			  md_key->key, 16, 
			  md_key->encrypt_key);
	l_enqueue(&ospf_info->md_key_q, &md_key->link);
	/*
	 * Key changed, trigger rollover
	 */
	if (ospf_info->youngest_md_key)
	    ospf_info->old_md_nbr_count = ospf_info->nbrcnt;
	ospf_info->youngest_md_key = md_key;
    } else if (md_key) {
        l_delete(&ospf_info->md_key_q, &md_key->link);
	if (md_key == ospf_info->youngest_md_key) {
	    /*
	     * The youngest key is removed ! The best we can do is
	     * stop the rollover process. 
	     */
	    ospf_info->youngest_md_key = ospf_get_youngest_md_key(ospf_info);
	    ospf_info->old_md_nbr_count = 0; 
	}
	  free(md_key);
    } else
        printf("\nOSPF: Key %lu does not exist", key_id);
}

/*
 * ospf_intfc_command
 */

void ospf_intfc_command (parseinfo *csb)
{
    idbtype	*idb;
    boolean 	sense;
    int		i;
    uchar	pri;
    ushort	poll_interval;
    ipaddrtype	address;
    list_queue  src_flood;
    ospf_md_key *md_key;
    ospf_idb_info *ospf_info;

    pri = poll_interval = address = 0;
    idb = csb->interface;
    ospf_info = idb->ospf_info;

    if (csb->nvgen) {
        if (!ospf_info)
	    return;
	switch (csb->which) {
	  case OSPF_NETWORK_TYPE:
	    if (ospf_info->if_type != ospf_info->def_if_type) {
	        if (ospf_info->if_type == IF_BROADCAST) {
		    nv_write(TRUE, "%s broadcast", csb->nv_command);
		} else if (ospf_info->if_type == IF_NONBROADCAST) {
		    nv_write(TRUE, "%s non-broadcast", csb->nv_command);
		} else if (ospf_info->if_type == IF_POINT_TO_M_POINT) {
		    nv_write(TRUE, "%s point-to-multipoint", csb->nv_command);
		}
	    }
	    break;

	  case OSPF_RXMT_INTERVAL:
	    if (ospf_info->retr_interval != ospf_info->def_retr_interval)
		nv_write(TRUE, "%s %d", csb->nv_command,
			 ospf_info->retr_interval);
	    break;

	  case OSPF_TRANS_DELAY:
	    if (ospf_info->if_transdly != ospf_info->def_if_transdly)
		nv_write(TRUE, "%s %d", csb->nv_command,
			 ospf_info->if_transdly);
	    break;

	  case OSPF_PRIORITY:
	    if (ospf_info->rtr_pri != ospf_info->def_rtr_pri)
		nv_write(TRUE, "%s %d", csb->nv_command,
			 ospf_info->rtr_pri);
	    break;

	  case OSPF_HELLO_INTERVAL:
	    if (ospf_info->hello_interval != ospf_info->def_hello_interval)
		nv_write(TRUE, "%s %d", csb->nv_command,
			 ospf_info->hello_interval);
	    break;

	  case OSPF_DEAD_INTERVAL:
	    if (ospf_info->dead_interval != ospf_info->def_dead_interval)
		nv_write(TRUE, "%s %d", csb->nv_command,
			 ospf_info->dead_interval);
	    break;

	  case OSPF_AUTH_CT_KEY:
	    if (ospf_info->auth_key[0]) {
	        nv_write(TRUE, "%s ", csb->nv_command); 
		ospf_auth_key_nvgen(&ospf_info->encrypt_type,
				    ospf_info->auth_key, 8,
				    ospf_info->encrypt_auth_key);
		
	    }
	    break;
	  case OSPF_AUTH_MD_KEY:
	    md_key = (ospf_md_key *) ospf_info->md_key_q.head;
	    while (md_key) {
	        nv_write(TRUE, "%s %d md5 ", csb->nv_command, md_key->id);
		ospf_auth_key_nvgen(&md_key->encrypt_type, md_key->key, 16,
				    md_key->encrypt_key);
		md_key = (ospf_md_key *) md_key->link.next;
	    }
	    break;

	  case OSPF_IF_COST:
	    for (i = 0; i < 8; i++)
		if (ospf_info->if_cost[i] != ospf_info->def_if_cost[i])
		    nv_write(TRUE, "%s %d", csb->nv_command,
			     ospf_info->if_cost[i]);
	    break;

	  case OSPF_DEMAND_CIRCUIT:
	    if (ospf_info->oi_cfg_demand_circuit)
		nv_write(TRUE, "%s", csb->nv_command);
	    break;

	  default:
	    printf("NVGEN: Unknown command: %s\n", csb->nv_command);
	    break;
	}
	return;
    }

    sense = csb->sense;
    if (sense && !ospf_info) {
        if ((ospf_info = ospf_if_create_info(idb)) == NULL)
	    return;
	ospf_if_set_default(NULL, idb, ospf_info);
    }
    
    switch (csb->which) {

      case OSPF_NETWORK_TYPE:
	if (sense) {
	    i = GETOBJ(int, 1);
	    if (!ospf_if_valid_type(idb, i)) {
	        printf("\nOSPF: Invalid type for interface");
		return;
	    }
	    if (ospf_info->if_type != i) {
		ospf_idb_change(idb, FALSE);
		ospf_info->if_type = i;
		ospf_idb_change(idb, TRUE);
	    }
	} else {
	    if (ospf_info) {
	        ospf_idb_change(idb, FALSE);
		ospf_info->if_type = ospf_info->def_if_type;
		ospf_idb_change(idb, TRUE);
	    }
	}
        if (ospf_info) {
	    /*
	     * Keep track of the default hello interval
	     */
	    if ((ospf_info->if_type == IF_NONBROADCAST) || 
		(ospf_info->if_type == IF_POINT_TO_M_POINT))
		ospf_if_change_def_hello(ospf_info, HELLO_INTERVAL_NBMA);
	    else
		ospf_if_change_def_hello(ospf_info, HELLO_INTERVAL);
	}
	break;

      case OSPF_RXMT_INTERVAL:
	if (sense) {
	    ospf_info->retr_interval = GETOBJ(int,1);
	} else {
	    if (ospf_info)
	        ospf_info->retr_interval = ospf_info->def_retr_interval;
	}
	break;

      case OSPF_TRANS_DELAY:
	if (sense) {
	    ospf_info->if_transdly = GETOBJ(int,1);
	} else {
	    if (ospf_info)
	        ospf_info->if_transdly = ospf_info->def_if_transdly;
	}
	break;

      case OSPF_PRIORITY:
	pri = ospf_info ? ospf_info->rtr_pri : 0;
	if (sense) {
	    ospf_info->rtr_pri = GETOBJ(int,1);
	} else {
	    if (ospf_info)
 	        ospf_info->rtr_pri = ospf_info->def_rtr_pri;
	}
	if (ospf_info && (pri != ospf_info->rtr_pri) && (ospf_info->enabled)) {
	    if (ospf_info->oi_myself) {
		ospf_info->oi_myself->nbr_pri = ospf_info->rtr_pri;
	    }
            if (ospf_info->if_state > IFS_POINT_TO_POINT) {
		(*if_state_machine[IFE_NBRCHANGE][ospf_info->if_state])
		                                   (idb, NULL);
	    }
	}
	break;

      case OSPF_HELLO_INTERVAL:
	if (sense) {
	    ospf_info->hello_interval = GETOBJ(int,1);
	} else {
	    if (ospf_info)
 	        ospf_info->hello_interval = ospf_info->def_hello_interval;
	}
        if (ospf_info) {
	    ospf_if_change_def_dead(ospf_info, 4 * ospf_info->hello_interval);
	    if (ospf_info->enabled && 
		mgd_timer_running(&ospf_info->oi_hello_timer)) {
	        mgd_timer_start(&ospf_info->oi_hello_timer, 
				ospf_info->hello_interval*ONESEC);
	    }
	    /*
	     * Dead timer could have changed, set the wait timer
	     * accordingly.
	     */
	    ospf_info->wait_interval = ospf_info->dead_interval;
	}
	break;

      case OSPF_DEAD_INTERVAL:
	if (sense) {
	    ospf_info->dead_interval = GETOBJ(int,1);
	} else {
	    if (ospf_info)
	        ospf_info->dead_interval = ospf_info->def_dead_interval;
	}
        if (ospf_info)
	    ospf_info->wait_interval = ospf_info->dead_interval;
	break;

      case OSPF_AUTH_CT_KEY:
        if (sense) {
	    ospf_set_auth_key(GETOBJ(int,1), (uchar *)GETOBJ(string,1), 
			      &ospf_info->encrypt_type, 
			      ospf_info->auth_key, 8, 
			      ospf_info->encrypt_auth_key);
        } else if (ospf_info) {
	    for (i= 0; i < 8 ; i++)
	        ospf_info->auth_key[i] = '\0';
	    ospf_info->encrypt_auth_key[0] = '\0';
	}
	break;

      case OSPF_AUTH_MD_KEY:
	ospf_set_md_key(ospf_info, GETOBJ(int,1), GETOBJ(int,3), 
			(uchar *)GETOBJ(string,1), sense);
	break;

      case OSPF_IF_COST:
	if (sense) {
	    ospf_info->if_cost[0] = GETOBJ(int,1);
	} else {
	    if (ospf_info)
	        ospf_info->if_cost[0] = ospf_info->def_if_cost[0];
	}

	if (ospf_info && (ospf_info->enabled) && (ospf_info->area_ptr)) {
	    src_flood.q_first = src_flood.q_last = NULL;
	    (void)build_rtr_lsa(ospf_info->area_ptr,
				&src_flood, FALSE);
	    flood_area(ospf_info->area_ptr, &src_flood, NULL, NULL);
	}
	break;
      case OSPF_DEMAND_CIRCUIT:
	if (sense != ospf_info->oi_cfg_demand_circuit) {
	    ospf_idb_change(idb, FALSE);
	    ospf_info->oi_cfg_demand_circuit = sense;
	    ospf_info->oi_demand_circuit = sense;
	    ospf_idb_change(idb, TRUE);
	}
	break;
      default:
	printf("\nOSPF: Unknown interface command ");
	break;
    }
} /* end of ospf_intfc_command */

/*
 * ospf_neighbor
 * Process ospf neighbor commands. This routine is very bad and will
 * be rewritteen in the future. In future we will drop the common_str_interface
 * key word and locate the idb using the neighbor address.
 */
void ospf_neighbor (pdbtype *pdb, ipaddrtype naddress,
		    parseinfo *csb, boolean sense)
{
    idbtype 	*idb;
    uchar	pri;
    ulong	poll_interval;
    nbrtype	*nbr;
    int 	count;
    areatype	*area;
    ospf_idb_info *ospf_info;

    if (csb->nvgen) {
	for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	    for (idb = area->ar_if_list; idb; idb = ospf_info->next) {
	        ospf_info = idb->ospf_info;
		if (!ospf_info->enabled)
		    continue;
		if ((ospf_info->if_type != IF_NONBROADCAST) ||
		    (ospf_info->rtr_pri == 0))
		    continue;
		for (nbr = ospf_info->oi_nbr; nbr; nbr= nbr->nbr_next) {
		    if (nbr->nbr_flags == NBRF_CONFIGURED) {
			nv_write(TRUE,"%s %i", csb->nv_command,
				 nbr->nbr_ipaddr);
			nv_add(nbr->nbr_pri, " priority %d", nbr->nbr_pri);
			if (nbr->nbr_def_poll_interval !=
			    nbr->nbr_poll_interval)
			    nv_add(TRUE, " poll-interval %d",
				   nbr->nbr_poll_interval);
		    }
		}
	    }
	}
	return;
    }

    idb = NULL;
    count = 0;

    if (GETOBJ(int,1)) {
	pri = GETOBJ(int,2);
	count++;
    } else {
	pri = 0;
    }
    if (GETOBJ(int,3)) {
	poll_interval = GETOBJ(int,4);
	count++;
    } else {
	poll_interval = POLL_INTERVAL;
    }
    /* end of parsing subcommands */
    if (ip_up_ouraddress(naddress)) {
	printf("\nOSPF: Neighbor address %i is our address", naddress);
	return;
    }
    /* look for matching idb */
    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_address && ((idb->ip_address & idb->ip_nets_mask) ==
				(naddress & idb->ip_nets_mask))) {
	    break;
	}
    }
    if ((!idb)) {
	printf("\nOSPF: Neighbor address does not map to an interface ");
	return;
    }

    ospf_info = idb->ospf_info;
    if (sense && !ospf_info) {
        if ((ospf_info = ospf_if_create_info(idb)) == NULL)
	    return;
	ospf_if_set_default(pdb, idb, ospf_info);
    }

    if (!sense && !ospf_info)
        return;

    if (sense && !(ospf_info->enabled)) {
	if ((nbr = malloc(sizeof(nbrtype))) == NULL)
	    return;
	nbr->nbr_ipaddr = naddress;
	nbr->nbr_pri = pri;
	nbr->nbr_poll_interval = poll_interval;
	nbr->nbr_next = ospf_info->oi_cfg_nbr;
	ospf_info->oi_cfg_nbr = nbr;
	return;
    }

    /*
     * We must only check for ospf_info->if_type after ospf_info->enabled
     * is verified to be TRUE, which means network command has been
     * processed and ospf_info->if_type is valid.
     * (1) It allows user to specify neighbor command before network command.
     * (2) During reload, although neighbor command come after network 
     *     command, ospf_info->if_type may not be set correctly when this
     *     function is executed, because the work of network command is
     *     carried out by a seperate process router_init.
     */
    if (sense && (ospf_info->if_type != IF_NONBROADCAST)) {
	printf("\nOSPF: Neighbor command is allowed only on NBMA networks");
	return;
    }

    if ((nbr = ospf_find_nbr(idb, 0, naddress, sense)) == NULL) {
	printf("\nOSPF: Could not allocate or find the neighbor");
	return;
    }
    if (sense) {
	/* create/ modify neighbor */
	nbr->nbr_pri = pri;
	nbr->nbr_poll_interval = poll_interval;
	nbr->nbr_flags = NBRF_CONFIGURED;
	if ((pdb->running) && (ospf_info->enabled)) {
	    if (interface_up(idb)) {
		(*(nbr_state_machine[NBR_EVENT_START][nbr->nbr_state]))
		    (idb, nbr);
	    } else {
		mgd_timer_start(&nbr->nbr_poll_timer, 
				nbr->nbr_poll_interval*ONESEC);
	    }
	}
    } else {
	(*(nbr_state_machine[NBR_EVENT_KILL_NBR][nbr->nbr_state]))
	    (idb, nbr);
	ospf_nbr_delete(idb, nbr, TRUE);
	ospf_dc_down_check(idb);
    }
} /* end of ospf_neighbor */

/*
 * ospf_nbr_create
 */
static void ospf_create_cfg_nbr (pdbtype *pdb, idbtype *idb,
				 ipaddrtype address, ulong poll,
				 uchar pri)
{
    nbrtype *nbr;
    ospf_idb_info *ospf_info = idb->ospf_info;

    if (ospf_info->if_type != IF_NONBROADCAST) {
	printf("\nOSPF: Neighbor command allowed only on NBMA networks");
	return;
    }
    if ((nbr = ospf_find_nbr(idb, 0, address, TRUE)) == NULL) {
	printf("\nOSPF: Could not allocate or find the neighbor");
	return;
    }
    /* create/ modify neighbor */
    nbr->nbr_pri = pri;
    nbr->nbr_poll_interval = poll;
    nbr->nbr_flags = NBRF_CONFIGURED;
    if ((pdb->running) && (ospf_info->enabled)) {
	if (interface_up(idb)) {
	    (*(nbr_state_machine[NBR_EVENT_START][nbr->nbr_state]))
		(idb, nbr);
	} else {
	    mgd_timer_start(&nbr->nbr_poll_timer, 
			    nbr->nbr_poll_interval*ONESEC);
	}
    }
} /* end of ospf_create_cfg_nbr */

/*
 * area_range
 */

static void area_range (
    pdbtype	*pdb,
    areatype	*area,
    ipaddrtype	address,
    ipaddrtype	mask,
    boolean	flag)
{
    addr_range	*range;
    addr_range	*temp;
    areatype	*ar;

    if (flag) {
	/* Don't allow range 0 */
	if (!address) {
	    printf("\nOSPF: Can not add this range as %i represents default",
		   address);
	    return;
	}
	/* CSCdj16943. Check for inconsistent address/mask
	 * combination. Reject the configuration unless we're
	 * booting up. This is to avoid breaking running configurations.
	 */
	if (system_configured && (~mask & address)) {
	    printf("\n%% OSPF: Inconsistent address/mask %i%m for area range",
		   address, mask);
	    return;
	}
	/* check for specified address range in area */
	for (range = area->ar_range_list; range; range = range->net_next) {
	    if ((range->net_address == address) 
		&& (range->net_mask == mask))
		/* yes we found the range */
		return;
	    if (((range->net_address & range->net_mask)
		 == (address & range->net_mask)) &&
		(range->net_mask < mask)) {
		printf("\nNew range: %i%m is superseded by an existing"
		       " range: %i%m", 
		       address, mask,
		       range->net_address, range->net_mask);
	    }
	    if (((range->net_address & mask) == (address & mask)) &&
		(range->net_mask > mask)) {
		printf("\nNew range: %i%m supersedes an existing "
		       "range: %i%m", 
		       address, mask,
		       range->net_address, range->net_mask);
	    }
	}
	/* check if we are adding new range to this area */
	if (range == NULL) {
	    /* 
	     * before creating one check that no other areas have 
	     * this range specified.
	     */
	    for (ar = pdb->ospf_area_list; ar ;ar = ar->ar_next) {
		/* check for specified address range in area */
		for (temp = ar->ar_range_list; temp; temp = temp->net_next) {
		    if ((temp->net_address == address) 
			&& (temp->net_mask == mask)) {
			/* yes we found the range */ 
			printf("\nOSPF: This range in different area %s",
			       ar->ar_id_str);
			return;
		    }
		}
	    }
	    flush_area_ranges(pdb, area);
	    /* yes we can create this range */
	    range = malloc(sizeof(addr_range));
	    if (range == NULL) {
		return;
	    }
	    range->net_address = address;
	    range->net_mask = mask;
	    range->net_cost = SUM_LS_INFINITY;
	    if (area->ar_range_list != NULL)
		range->net_next = area->ar_range_list;
	    area->ar_range_list = range;
	    area->ar_netcnt++;
	    ospf_force_spf(pdb, NULL);
	}
    } else {
	flush_area_ranges(pdb, area);
	if (delete_range(area, address, mask))
	    area->ar_netcnt--;
	/* this might need lsa update
	 * we will worry about it latter.
	 */
	ospf_force_spf(pdb, NULL);
    }
} /* end of area_range */


/*
 * ospf_area
 */
void ospf_area (pdbtype *pdb, parseinfo *csb, boolean sense)
{
    areatype	*area;
    vlinktype *vlink;
    addr_range *range;
    idbtype *vidb;
    list_queue	flood_list;
    int area_id_format;
    ospf_idb_info *ospf_info;
    ospf_md_key *md_key;
    boolean more_vl_line, old_ar_stub, old_ar_nssa, old_ar_nssa_no_redist;
    boolean nssa_no_redist, nssa_def, nssa_nosum;
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;

    if (csb->nvgen) {
	if (!(pdb->proctype & PROC_OSPF))
	    return;

	/* We should be able to have the AREAID action routine walk through all
	 * active areas, pushing parsenodes for each.  Then we can do a more
	 * NV generation of the commands.
	 */

	for (area = pdb->ospf_area_list; area;
	     area = area->ar_next) {
	    if (area->ar_autype) {
		nv_write(TRUE, "%s %s authentication", csb->nv_command,
			 area->ar_id_str);
		if (area->ar_autype == OSPF_AUTH_MD)
		    nv_add(TRUE, " message-digest");
	    }
	    if (area->ar_stub) {
		nv_write(TRUE, "%s %s stub", csb->nv_command, area->ar_id_str);

		if (area->ar_stub_nosum)
		    nv_add(TRUE, " no-summary");

	    }

	    if (area->ar_nssa) {
		nv_write(TRUE, "%s %s nssa", csb->nv_command, area->ar_id_str);
		nv_add(area->ar_nssa_no_redist, " no-redistribution");
		nv_add(area->ar_nssa_default, 
		       " default-information-originate");
		nv_add(area->ar_stub_nosum, " no-summary");
	    }

	    if ((area->ar_stub || area->ar_nssa) && 
		(area->ar_stub_default_cost != 
		 area->ar_def_stub_default_cost)) {
		nv_write(TRUE, "%s %s default-cost %d ", csb->nv_command, 
			 area->ar_id_str, area->ar_stub_default_cost);
	    }

	    for (range = area->ar_range_list;range;range = range->net_next)
		nv_write(TRUE,"%s %s range %i %i",
			 csb->nv_command, area->ar_id_str,range->net_address,
			 range->net_mask);
	    for (vlink = area->ar_vlink; vlink ; vlink = vlink->vl_next) {
		nv_write(TRUE, "%s %s virtual-link %i",
			 csb->nv_command, area->ar_id_str, vlink->vl_nbr_id);
		more_vl_line = FALSE;
		vidb = vlink->vl_idb;
		ospf_info = vidb->ospf_info;
		if (ospf_info->def_retr_interval != ospf_info->retr_interval) {
		    nv_add(TRUE, " retransmit-interval %d", 
			   ospf_info->retr_interval);
		    more_vl_line = TRUE;
		}
		if (ospf_info->def_if_transdly != ospf_info->if_transdly) {
		    nv_add(TRUE, " transmit-delay %d", ospf_info->if_transdly);
		    more_vl_line = TRUE;
		}
		if (ospf_info->def_hello_interval != 
		    ospf_info->hello_interval) {
		    nv_add(TRUE, " hello-interval %d", 
			   ospf_info->hello_interval);
		    more_vl_line = TRUE;
		}
		if (ospf_info->def_dead_interval != ospf_info->dead_interval) {
		    nv_add(TRUE, " dead-interval %d", 
			   ospf_info->dead_interval);
		    more_vl_line = TRUE;
		}

		if (ospf_info->auth_key[0]) {
		    nv_add(TRUE, " authentication-key ");
		    ospf_auth_key_nvgen(&ospf_info->encrypt_type, 
					ospf_info->auth_key, 8,
					ospf_info->encrypt_auth_key);
		    more_vl_line = TRUE;
		}
		md_key = (ospf_md_key *) ospf_info->md_key_q.head;
		while (md_key) {
		    if (more_vl_line) {
		        nv_write(TRUE, "%s %s virtual-link %i",
				 csb->nv_command, area->ar_id_str, 
				 vlink->vl_nbr_id);
		    }
		    nv_add(TRUE, " message-digest-key %d md5 ", md_key->id);
		    ospf_auth_key_nvgen(&md_key->encrypt_type, md_key->key, 16,
					md_key->encrypt_key);
		    md_key = (ospf_md_key *) md_key->link.next;
		    more_vl_line = TRUE;
	       }
	    }
	}
	return;
    }
    /* set area pointer */
    area_id_format = OSPF_AREA_ID_FORMAT_INT;
    if (GETOBJ(paddr,8)->type == ADDR_IP) {	/* XXX  Change when AREAID 
						   macro exists */
        area_id_format = OSPF_AREA_ID_FORMAT_IP;
	GETOBJ(int,1) = (uint)GETOBJ(paddr,8)->ip_addr;
    }

    area = NULL;
    /* get the pointer to area */
    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	if (area->ar_id == GETOBJ(int,1))
	    break;
    }
    if ((area == NULL) && (sense)) {
	area = ospf_create_area(GETOBJ(int,1), pdb);
	if (area == NULL) {
	    printf("\nOSPF: No memory for area structure");
	    return;
	}
	/* only record the format when new area is created */
	if (area_id_format == OSPF_AREA_ID_FORMAT_IP)
	  sprintf(area->ar_id_str, "%i", GETOBJ(int,1));
	else if (area_id_format == OSPF_AREA_ID_FORMAT_INT)
	  sprintf(area->ar_id_str, "%u", GETOBJ(int,1));
    } else {
	if ((area == NULL) && (!sense)) {
	    printf("\nOSPF: Specified area is not configured");
	    return;
	}
    }

    switch (csb->which) {
      case OSPF_AREA_AUTH:
	area->ar_autype = sense ? GETOBJ(int,2) : 0;
	break;

      case OSPF_AREA_STUB:

	if (area->ar_nssa) {
	    printf("\nOSPF: Area is configured as NSSA already");
	    return;
	}
	if (area->ar_id == BACKBONE_AREA_ID) {
	    printf("\nOSPF: Backbone can not be configured as stub area");
	    return;
	}

	/*
	 * Perform this check only when router_init_running is false, i.e
	 * we are not starting up.    At which time, after we parse all
	 * "network ... area ..." command, we will schedule router_init
	 * process to run.  After that we will go on to parse "area ... 
	 * stub" which might happen before router_init starts.   That will 
	 * cause us to complain because other area hasn't been created yet
	 * (by the network command).  Thus, area stub will be dropped 
	 * from the config.
	 */
	if (sense && (!router_init_running)
	    && pdb->ospf_as_brtr && (pdb->area_count == 1)) {
	    printf("\nOSPF: Stub command is invalid when it is ASBR\n");
	    return;
	}

	old_ar_stub = area->ar_stub;
	area->ar_stub = sense;

	/*
	 * Is stub no-summary configured?
	 * "no ... stub no-summary" turns off no-summary but it is still
	 *  a stub area.
	 */
	if (!sense)
	    area->ar_stub_nosum = sense;
	if (GETOBJ(int, 7)) {
	    area->ar_stub_nosum = sense;
	    area->ar_stub = TRUE;
	}
	if (area->ar_stub != old_ar_stub) {
	    /*
	     * Number of stub area change
	     */
	    if (area->ar_stub) {
		ospf_pdb->ospf_stub_area_count++;
		ospf_pdb->ospf_normal_area_count--;
	    } else {
		ospf_pdb->ospf_stub_area_count--;
		ospf_pdb->ospf_normal_area_count++;
	    }
	}

	if (area->ar_stub)
	    area->ar_options &= ~OPTION_E_BIT;
	else
	    area->ar_options |= OPTION_E_BIT;

	if ((pdb->ospf_area_brtr) && (pdb->running)) {
	    /* 
	     * Flush summary ASBR out.
	     * If configure stub summary, flush old summary net LSA also.
	     */
	    flood_list.q_first = NULL;
	    flood_list.q_last  = NULL;
	    flood_list.count   = 0;
	    ospf_flush_sum_from_area(area, &flood_list, area->ar_stub_nosum);
	    flood_area(area, &flood_list, NULL, NULL);

	    /* generate default summary LSA for stub area */
	    if (area->ar_stub)
		stub_default(area, area->ar_stub_default_cost);
	    else
		stub_default(area, SUM_LS_INFINITY);
	}
	/*
	 * Check if we should reoriginate indication LSA into this 
	 * area.
	 */
	if (!area->ar_stub)
	    pdb->ospf_flag |= OSPF_CHECK_INDICATION;
	ospf_reset_adj(area);
	ospf_force_spf(pdb, NULL);
	break;

      case OSPF_AREA_DEFAULTCOST:
	if (sense) {
	    area->ar_stub_default_cost = GETOBJ(int,2);
	} else {
	    area->ar_stub_default_cost = area->ar_def_stub_default_cost;
	}
	if ((area->ar_stub || (area->ar_nssa && area->ar_stub_nosum)) &&
	    pdb->ospf_area_brtr && pdb->running) {
	    stub_default(area, area->ar_stub_default_cost);
	}
	break;

      case OSPF_AREA_NSSA:
	if (area->ar_stub) {
	    printf("\nOSPF: Area is configured as stub area already");
	    return;
	}
	if (area->ar_id == BACKBONE_AREA_ID) {
	    printf("\nOSPF: Backbone can not be configured as NSSA area");
	    return;
	}

	old_ar_nssa = area->ar_nssa;
	old_ar_nssa_no_redist = area->ar_nssa_no_redist;

	nssa_nosum = GETOBJ(int,7);
	nssa_def = GETOBJ(int, 8);
	nssa_no_redist = GETOBJ(int, 9);

	if (sense) {
	    if (nssa_nosum)
		area->ar_stub_nosum = nssa_nosum;
	    if (nssa_no_redist)
		area->ar_nssa_no_redist = nssa_no_redist;
	    if (nssa_def)
		area->ar_nssa_default = nssa_def;
	    area->ar_nssa = sense;
	} else {
	    if (nssa_no_redist || nssa_def || nssa_nosum) {
		if (nssa_nosum)
		    area->ar_stub_nosum = sense;
		if (nssa_no_redist)
		    area->ar_nssa_no_redist = sense;
		if (nssa_def)
		    area->ar_nssa_default = sense;
	    } else {
		area->ar_nssa = sense;
		area->ar_stub_nosum = sense;
		area->ar_nssa_no_redist = sense;
		area->ar_nssa_default = sense;
	    }
	}

	if (area->ar_nssa != old_ar_nssa) {
	    /*
	     * Number of NSSA area change
	     */
	    if (area->ar_nssa) {
		ospf_pdb->ospf_nssa_area_count++;
		ospf_pdb->ospf_normal_area_count--;
		ospf_flush_all_type5_from_stub(area);
		ospf_flush_nssa_type4_from_other(area);
		ospf_flush_all_type4_from_stub(area);
		if (pdb->ospf_as_brtr)
		    ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE);
		area->ar_options &= ~OPTION_E_BIT;
		area->ar_options |= OPTION_N_P_BIT;
	    } else {
		ospf_pdb->ospf_nssa_area_count--;
		ospf_pdb->ospf_normal_area_count++;
		area->ar_options |= OPTION_E_BIT;
		area->ar_options &= ~OPTION_N_P_BIT;
		ospf_flush_all_type7_from_area(area);
	    }
	}

	if ((pdb->ospf_area_brtr) && (pdb->running)) {
	    /* 
	     * Flush summary ASBR out.
	     */
	    flood_list.q_first = NULL;
	    flood_list.q_last  = NULL;
	    flood_list.count   = 0;
	    ospf_flush_sum_from_area(area, &flood_list, area->ar_stub_nosum);
	    flood_area(area, &flood_list, NULL, NULL);
	    if (area->ar_stub_nosum)
		stub_default(area, area->ar_stub_default_cost);
	    else
		stub_default(area, SUM_LS_INFINITY);
	    if (area->ar_nssa_default)
		nssa_default(area, area->ar_stub_default_cost);
	    else
		nssa_default(area, SUM_LS_INFINITY);
	}

	if (pdb->ospf_area_brtr && ospf_pdb->ospf_nssa_area_count && 
	    !pdb->ospf_as_brtr)
	    ospf_redistribute(pdb, NULL, TRUE);
	if (!ospf_pdb->ospf_nssa_area_count && pdb->ospf_as_brtr)
	    ospf_redistribute(pdb, NULL, FALSE);
	/*
	 * Redistribution to NSSA change, rescan.
	 */
	if ((old_ar_nssa_no_redist != area->ar_nssa_no_redist) &&
	    pdb->ospf_as_brtr) {
	    ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE);
	}
	/*
	 * Check if we should reoriginate indication LSA into this 
	 * area.
	 */
	if (!area->ar_nssa)
	    pdb->ospf_flag |= OSPF_CHECK_INDICATION;
	ospf_reset_adj(area);
	ospf_force_spf(pdb, NULL);
	break;

      case OSPF_AREA_RANGE:
	area_range(pdb, area, GETOBJ(paddr,1)->ip_addr,
		   GETOBJ(paddr,2)->ip_addr, sense);
	break;

      case OSPF_AREA_VIRTUAL:
	attach_virtual_interface(pdb, area, csb);
	break;

      case OSPF_AREA_DELETE:
	if (area->ar_if_list) {
	    printf("\nOSPF: Area %s cannot be deleted before "
		   "its network command is removed",
		   area->ar_id_str);
	} else {
	    ospf_flush_area(area);
	    ospf_clean_area(area);
	    ospf_delete_area(pdb, area);
	    free(area);
	    ospf_stub_and_asbr_check(pdb);
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
} /* end of ospf_area */

/*
 * ospf_attach_interface
 */

boolean ospf_attach_interface (
    pdbtype *pdb,
    areatype *area,
    idbtype *idb,
    boolean sense)
{
    list_queue src_flood;
    idbtype *temp;
    nbrtype *nbr;
    nbrtype *temp1;
    ospf_idb_info *ospf_info = idb->ospf_info;

    if (sense) {
        /*
         * check if ip is enabled on this interface with address 
         * or it is detach
         */
        if (!idb->ip_enabled) {
            printf("\nOPPF: IP is disabled on this interface");
            return (FALSE);
        }
        
        /* allow unnumbered interface
         * only on point-to-point interfaces.
         * or allow if it is detach
         */
        if (idb->ip_address == 0) {
            if (!is_p2p(idb)) {
                printf("\nOSPF: No IP address for this interface %s", 
                       idb->namestring);
                return (FALSE);
            }
        }

        if (!ospf_info) {
	    if ((ospf_info = ospf_if_create_info(idb)) == NULL)
	        return (FALSE);
        }
	/*
	 * check if this interface is already 
	 * attached to any other area.
	 */
	if (ospf_info->area_ptr)
	    return (FALSE);
	/* check if this interface already 
	 * attached to this area.
	 */
	for (temp = area->ar_if_list; temp; 
	     temp = temp->ospf_info->next) {
	    if (idb == temp) {
		ospf_if_check(pdb, area, idb);
		return (TRUE);
	    }
	}
	if (!ospf_if_init(pdb, area, idb)) {
	    printf("\nOSPF: Interface init failed");
	    return (FALSE);
	}
	/* attach this interface to the area */
	if (area->ar_if_list != NULL)
	    ospf_info->next = area->ar_if_list;
	else{ 
	    /* 
	     * since this is the first interface to attach to
	     * this area start the refresh timer in this area 
	     * stop this refresh timer when we detach the last 
	     * interface.
	     */
	    if (pdb->running) {
		mgd_timer_start(&area->ar_update_timer,
				LS_REFRESH_TIME*ONESEC);
	    }
	}
	/*
	 * Start hello timer here. It will always run regardless of
	 * the interface state.
	 * Skip sending hello for loopback interface.
	 */
	if (ospf_info->if_type != IF_LOOPBACK) {
	    if (ospf_info->oi_demand_circuit &&
		(ospf_info->if_type == IF_POINT_TO_POINT)) {
		mgd_timer_start(&ospf_info->oi_hello_timer, 
				ospf_info->oi_poll_interval*ONESEC);
	    } else {
		mgd_timer_start(&ospf_info->oi_hello_timer, 
				ospf_info->hello_interval*ONESEC);
	    }
	}
	area->ar_if_list = idb;
	ospf_info->area_ptr = area;
	area->ar_ifcnt++;
	if (pdb->running) {
	    ospf_start_intf(idb);
	    src_flood.q_first = src_flood.q_last = NULL;
	    (void)build_rtr_lsa(area, &src_flood, FALSE);
	    flood_area(area, &src_flood, NULL, NULL);
	}
	if (ospf_info->if_type == IF_NONBROADCAST) {
	    for (nbr = ospf_info->oi_cfg_nbr; nbr; nbr = temp1) {
		ospf_create_cfg_nbr(pdb, idb, nbr->nbr_ipaddr, 
				    nbr->nbr_poll_interval, nbr->nbr_pri);
		temp1 = nbr->nbr_next;
		free(nbr);
	    }
	    ospf_info->oi_cfg_nbr = NULL;
	}
    } else { 
	/* detach this interface from 
	 * this area.
	 */
	if (!ospf_info || !(ospf_info->enabled))
	    return (FALSE);
	ospf_clean_if(idb);
    }
    return (TRUE);
} /* end of ospf_attach_interface */

/*
 * attach_virtual_interface
 */
static void attach_virtual_interface (pdbtype *pdb, areatype *area, parseinfo *csb)
{
    areatype *backbone;
    vlinktype *vlink, *temp_vlink;
    ulong router_id;
    list_queue src_flood;
    idbtype *temp;
    idbtype *vidb;
    int i;
    ospf_idb_info *ospf_info;
    hwidbtype *hwidb;

    vidb = NULL;

    /*
     * First get the router id 
     */
    router_id = (ulong)GETOBJ(paddr,1)->ip_addr;
    backbone = pdb->ospf_backbone;
    if (backbone == NULL) {
 	for (backbone = pdb->ospf_area_list; backbone; 
 	     backbone = backbone->ar_next) {
 	    if (backbone->ar_id == BACKBONE_AREA_ID)
 		break;
 	}
    }
    if (csb->sense) {
	if (backbone == NULL) {
	    if ((backbone = ospf_create_area(BACKBONE_AREA_ID, pdb)) == NULL) {
		printf("\nOSPF: No memory for backbone area structure");
		return;
	    }
	    sprintf(backbone->ar_id_str, "%u", BACKBONE_AREA_ID);
	}
	/*
	 * check if this interface already have the
	 * virtual link.
	 */
	for (vlink = (vlinktype *)area->ar_vlink; vlink ;
	     vlink = vlink->vl_next) {
	    if (vlink->vl_nbr_id == router_id) {
		vidb = vlink->vl_idb;
		ospf_info = vidb->ospf_info;
		goto process_interval;
	    }
	}
	/* create one virtual link */
	vlink = malloc(sizeof(vlinktype));
	if (!vlink)
	    return;
	vidb = vidb_malloc();
	if (!vidb) {
	    free(vlink);
	    return;
	}
	hwidb = vidb->hwptr;
	hwidb->type = IDBTYPE_OSPF_VL;
	hwidb->name = IFNAME_OSPF_VL;
	hwidb->typestring = "OSPF_VL";
	hwidb->unit = ospf_vl_count++;
	hwidb->slotunit = 0;
	hwidb->vc = NO_VC;

	idb_init_names(vidb, TRUE);
        if ((ospf_info = ospf_if_create_info(vidb)) == NULL) {
	    free(vlink);
	    vidb_free(vidb);
	    return;
	}
	vidb->output_if = NULL;
	ospf_info->if_cost[0] = RTR_LS_MAX_COST; 
	ospf_if_set_default(pdb, vidb, ospf_info);
	ospf_info->vnbr_id = router_id;
	ospf_info->vlink_trans = area;
	ospf_info->oi_cfg_demand_circuit = TRUE;
	ospf_info->oi_demand_circuit = TRUE;
	vlink->vl_backbone = backbone;
	vlink->vl_nbr_id = router_id;
	vlink->vl_idb = vidb;
	vlink->vl_next = (vlinktype *)area->ar_vlink;
	area->ar_vlink = vlink;
 	if (!ospf_if_init(pdb, backbone, vidb)) {
	    printf("\nOSPF: Interface init failed");
	    return;
	}
	/* check if this interface already 
	 * attached to this area.
	 */
	for (temp = backbone->ar_if_list; temp; 
	     temp = temp->ospf_info->next) {
	    if (vidb == temp) {
		ospf_if_check(pdb, backbone, vidb);
		goto process_interval;
	    }
	}
	/* attach this interface to the area */
	if (backbone->ar_if_list != NULL)
	    ospf_info->next = backbone->ar_if_list;
	else{ 
	    /* 
 	     * since this is the first interface to attach to
	     * this area start the refresh timer in this area. 
	     * stop this refresh timer when we detach the last 
	     * interface.
	     */
	    if (pdb->running) {
		mgd_timer_start(&backbone->ar_update_timer,
				LS_REFRESH_TIME * ONESEC);
	    }
	}
	/*
	 * start hello timer here.   It will always run regardless of
	 * the interface state.
	 */
	mgd_timer_start(&ospf_info->oi_hello_timer, 
			ospf_info->hello_interval*ONESEC);
	backbone->ar_if_list = vidb;
	ospf_info->area_ptr = backbone;
	backbone->ar_ifcnt++;
	if (pdb->running) {
	    ospf_start_intf(vidb);
	    src_flood.q_first = src_flood.q_last = NULL;
	    (void)build_rtr_lsa(backbone, &src_flood, FALSE);
	    flood_area(backbone, &src_flood, NULL, NULL);
	}
    } else {
	/*
	 * delete the virtual neighbor 
	 */
	backbone = pdb->ospf_backbone;
	if (backbone == NULL)
	    return;
	/*
	 * get the virtual link attached.
	 */
	for (vlink = (vlinktype *)&(area->ar_vlink); vlink && vlink->vl_next ;
	     vlink = vlink->vl_next) {
	    if (vlink->vl_next->vl_nbr_id == router_id) {
		temp_vlink = vlink->vl_next;
		vlink->vl_next = temp_vlink->vl_next;
		vidb = temp_vlink->vl_idb;
		if (GETOBJ(int,2) != 0) {
		    /* Options were specified; Don't delete the link */
		    ospf_info = vidb->ospf_info;
		    goto process_interval;
		}
		ospf_clean_if(vidb);
		vidb_free(vidb);
		free(temp_vlink);
		if (backbone->ar_if_list == NULL) {
		    ospf_clean_area(backbone);
		    ospf_delete_area(pdb, backbone);
		    free(backbone);
		}
		return;
	    }
	}
	return;
    }
  process_interval:
    if (GETOBJ(int,2) & OSPF_VIRT_RETRANS) {	/* retransmit-interval */
	if (csb->sense) {
	    ospf_info->retr_interval = GETOBJ(int,4);
	} else
	    ospf_info->retr_interval = RTR_INTERVAL;
    }
    if (GETOBJ(int,2) & OSPF_VIRT_DEAD) {		/* dead-interval */
	if (csb->sense) {
	    ospf_info->dead_interval = GETOBJ(int,5);
	} else
	    ospf_info->dead_interval = DEAD_INTERVAL;
    }
    if (GETOBJ(int,2) & OSPF_VIRT_HELLO) {		/* hello-interval */
	if (csb->sense) {
	    ospf_info->hello_interval = GETOBJ(int,3);
	} else
	    ospf_info->hello_interval = HELLO_INTERVAL;
	if (ospf_info->enabled && 
	    mgd_timer_running(&ospf_info->oi_hello_timer)) {
	    mgd_timer_start(&ospf_info->oi_hello_timer,
			    ospf_info->hello_interval * ONESEC);
	}
    }
    if (GETOBJ(int,2) & OSPF_VIRT_TRANS) {		/* transmit-delay */
	if (csb->sense) {
	    ospf_info->if_transdly = GETOBJ(int,6);
	} else
	    ospf_info->if_transdly = TRANSDELAY;
    }
    if (GETOBJ(int,2) & OSPF_VIRT_AUTH_CT) {	    /* authentication-key */
	/* Clear the old key and if there
	 * is a new one, copy it in.
	 */
 	if (csb->sense) {
	    ospf_set_auth_key(GETOBJ(int,8), (uchar *)GETOBJ(string,1),
			      &ospf_info->encrypt_type,
			      ospf_info->auth_key, 8,
			      ospf_info->encrypt_auth_key);
	} else {
	    for (i= 0; i < 8 ; i++)
		ospf_info->auth_key[i] = '\0';
	    ospf_info->encrypt_auth_key[0] = '\0';
	}
   }
   if (GETOBJ(int,2) & OSPF_VIRT_AUTH_MD) {
       ospf_set_md_key(ospf_info, GETOBJ(int,9), GETOBJ(int,8), 
		       (uchar *)GETOBJ(string,1), csb->sense);
   }
} /* end of attach_virtual_interface */

/*
 * ospf_nv_add_area_id
 * Write the area_id in IP or Integer format during NVGEN
 */
char *ospf_area_id_str (pdbtype *pdb, ulong area_id)
{
    areatype *area;
    for (area = pdb->ospf_area_list; area; area = area->ar_next)
      if (area->ar_id == area_id)
	  return (area->ar_id_str);
    return NULL;
}

/*
 * ospf_if_check
 */

static void ospf_if_check (pdbtype *pdb, areatype *area, idbtype *idb)
{
    nbrtype *nbr, *temp1;
    ospf_idb_info *ospf_info = idb->ospf_info;

    /*
     * check if this interface is attached 
     * to any other area.
     */
    if (area != ospf_info->area_ptr) {
	printf("\nOSPF: Interface %s is attached to more than one area",
	       idb->namestring);
	ospf_info->enabled = FALSE;
	return;
    }
    if (ospf_info->enabled == FALSE) {
	ospf_start_intf(idb);
    }

    if (ospf_info->if_type == IF_NONBROADCAST) {
	for (nbr = ospf_info->oi_cfg_nbr; nbr; nbr = temp1) {
	    ospf_create_cfg_nbr(pdb, idb, nbr->nbr_ipaddr, 
				nbr->nbr_poll_interval, nbr->nbr_pri);
	    temp1 = nbr->nbr_next;
	    free(nbr);
	}
	ospf_info->oi_cfg_nbr = NULL;
    }
    /*
     * start the neighbors in case of NONBRADCAST
     * networks.
     */
    if (ospf_info->if_type == IF_NONBROADCAST) {
	for (nbr = ospf_info->oi_nbr ; nbr; nbr = nbr->nbr_next) {
	    if (nbr->nbr_state != NBR_DOWN)
		continue;
	    if (interface_up(idb)) {
		(*(nbr_state_machine[NBR_EVENT_START][nbr->nbr_state]))
		    (idb, nbr);
	    } else {
		mgd_timer_start(&nbr->nbr_poll_timer, 
				nbr->nbr_poll_interval*ONESEC);
	    }
	}
    }
} /* end of ospf_if_check */

/*
 * ospf_area_check
 */

static void ospf_area_check (
    pdbtype *pdb,
    areatype *area)
{
    idbtype *idb;
    list_queue src_flood;

    for (idb = area->ar_if_list; idb ; idb = idb->ospf_info->next)
	ospf_if_check(pdb, area, idb);

    if (!mgd_timer_running(&area->ar_update_timer) && area->ar_ifcnt) {
	mgd_timer_start(&area->ar_update_timer, LS_REFRESH_TIME*ONESEC);
	src_flood.q_first = src_flood.q_last = NULL;
	(void)build_rtr_lsa(area, &src_flood, FALSE);
	flood_area(area, &src_flood, NULL, NULL);
    }
} /* end of ospf_area_check */

/*
 * ospf_pdb_check
 */

void ospf_pdb_check (pdbtype *pdb)
{
    areatype *area;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    for (area = pdb->ospf_area_list; area; area = area->ar_next)
	ospf_area_check(pdb, area);

    if ((pdb->ospf_area_brtr) && 
	(!mgd_timer_running(&ospf_pdb->ospf_sumupdate_timer))) {
	mgd_timer_start(&ospf_pdb->ospf_sumupdate_timer, 
			LS_REFRESH_TIME*ONESEC);
	start_sum_brtr(pdb);
    }    
    if (pdb->ospf_as_brtr) {
	mgd_timer_start(&ospf_pdb->ospf_exupdate_timer, 
			LS_REFRESH_TIME*ONESEC);
	ospf_perform_asbr_duty(pdb);
    }
} /* end of ospf_pdb_check */


void ospf_routing_commands (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	  case IP_OSPFNAMELOOKUP:
	    nv_write(ospf_name_lookup, "%s", csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case IP_OSPFNAMELOOKUP:
	ospf_name_lookup = csb->sense;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 *
 */
static void ospf_init (subsystype *subsys)
{
    /*
     * clear OSPF traffic statistics
     */
    memset(&ospf_traffic, 0, sizeof(ospf_traffic));

    /*
     * Parser chain initialization
     */
    ospf_parser_init();

    /*
     * Register some functions
     */
    ip_set_proto_outcounter(OSPF_PROT, &ospf_traffic.outputs);
    reg_add_encap_fixup(ospf_encap_fixup, "ospf_encap_fixup");
    reg_add_ip_enqueue(OSPF_PROT, ospf_enqueue, "ospf_enqueue");
    reg_add_ip_show_traffic(ospf_show_traffic, "ospf_show_traffic");
    reg_add_ip_address_change(ospf_idb_address_change,
			      "ospf_idb_address_change");
    reg_add_ip_interface_route_adjust(ospf_route_adjust, "ospf_route_adjust");
    reg_add_bandwidth_changed(ospf_bandwidth_changed, 
			      "ospf_bandwidth_changed");
    reg_add_iprouting_pdb_init(PROC_OSPF, ospf_pdb_init, "ospf_pdb_init");
    reg_add_iprouting_external_flush(PROC_OSPF, ospf_flush_type5, 
				     "ospf_flush_type5");
    reg_add_iprouting_area_id(PROC_OSPF, ospf_area_id_str, "ospf_area_id_str");
    reg_add_ospf_perform_asbr_duty(ospf_perform_asbr_duty,
				   "ospf_perform_asbr_duty");
    reg_add_ospf_configure_for_broadcast(ospf_configure_for_broadcast,
					 "ospf_configure_for_broadcast");
} /* end of ospf_init */

/*
 * ospf_timers_command
 * Set ospf SPF delay and hold time interval
 * Called after parsing router subcommand "timers ospf"
 */
void ospf_timers_command (parseinfo *csb)
{
    pdbtype *pdb = csb->protocol;

    if (!router_enable)
	return;

    if (!(pdb->proctype & PROC_OSPF))
	return;

    if (csb->nvgen) {
	nv_write(((pdb->ospf_spf_hold_interval != SPF_HOLD_INTERVAL) ||
		  (pdb->ospf_spf_schd_delay != SPF_SCHD_DELAY)),
		 "%s %d %d", csb->nv_command,
		 pdb->ospf_spf_schd_delay, pdb->ospf_spf_hold_interval);
	return;
    }

    if (csb->sense == FALSE) {
	pdb->ospf_spf_schd_delay = SPF_SCHD_DELAY;
	pdb->ospf_spf_hold_interval = SPF_HOLD_INTERVAL;
    } else {
	pdb->ospf_spf_schd_delay = GETOBJ(int,1);
	pdb->ospf_spf_hold_interval = GETOBJ(int,2);
    }

}

static void ospf_if_recalculate_all_cost (pdbtype *pdb)
{
    areatype  *area;
    idbtype   *idb;
    list_queue flood_queue;

    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	for (idb = area->ar_if_list; idb ; idb = idb->ospf_info->next)
	    ospf_if_recalculate_cost(pdb, idb);
	flood_queue.q_first = flood_queue.q_last = NULL;
	build_rtr_lsa(area, &flood_queue, FALSE);
	flood_area(area, &flood_queue, NULL, NULL);
    }
}

/*
 * ospf_auto_cost_cmd
 *
 */
void ospf_auto_cost_cmd(parseinfo *csb, pdbtype *pdb)
{
    boolean   auto_cost;
    ulong     reference_bw;
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;

    if (csb->nvgen) {
	if (pdb->ospf_auto_cost) {
	    if (ospf_pdb->ospf_reference_bw != OSPF_DEF_REFERENCE_BW) {
		nv_write(TRUE, "%s reference-bandwidth %d", 
			 csb->nv_command,
			 ospf_pdb->ospf_reference_bw);
	    }
	} else {
	    nv_write(TRUE, "no %s", csb->nv_command);
	}
	return;
    }

    auto_cost = pdb->ospf_auto_cost;
    reference_bw = ospf_pdb->ospf_reference_bw;
    pdb->ospf_auto_cost = csb->sense;
    ospf_pdb->ospf_reference_bw = GETOBJ(int,1);
    if ((!pdb->ospf_auto_cost && ospf_pdb->ospf_reference_bw) || 
	csb->set_to_default) {
	/*
	 * Reference bandwidth is specified with the no command,
	 * or the default keyword is used. In this case, 
	 * keep auto cost on but set the reference bandwidth
	 * back to default.
	 */
	pdb->ospf_auto_cost = TRUE;
	ospf_pdb->ospf_reference_bw = OSPF_DEF_REFERENCE_BW;
    }
    if (!ospf_pdb->ospf_reference_bw) {
	/*
	 * Reference bandwidth could be zero if either
	 * no ospf auto-cost or ospf_auto_cost command
	 * is entered. In this case, set the reference
	 * bandwidth to default.
	 */
	ospf_pdb->ospf_reference_bw = OSPF_DEF_REFERENCE_BW;
    }
    if (reference_bw != ospf_pdb->ospf_reference_bw) {
	if (pdb->ospf_auto_cost && system_configured) {
	    /*
	     * Auto cost is enabled and the reference bandwidth
	     * changed, warn the user to keep reference bandwidth
	     * consistent with other routers.
	     * Skip the message at boot time.
	     */
	    printf("\n%% OSPF: Reference bandwidth is changed. "
		   "\n        Please ensure reference bandwidth is "
		   "consistent across all routers.");
	}
	ospf_if_recalculate_all_cost(pdb);
    } else if (auto_cost != pdb->ospf_auto_cost) {
	ospf_if_recalculate_all_cost(pdb);
    }
}

/*
 * ospf_log_adj_chg_cmd
 *
 * set the "log adjacency changes" flag in the pdb
 */
void ospf_log_adj_chg_cmd(parseinfo *csb, pdbtype *pdb)
{
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    if(csb->nvgen) {
	nv_write(ospf_pdb->log_adj_changes, csb->nv_command);
	return;
    }
    ospf_pdb->log_adj_changes = csb->sense;
}

/*
 * ospf_command
 * Parse router ospf subcommands
 */
void ospf_command (parseinfo *csb)
{
    pdbtype  *pdb = (pdbtype *) csb->protocol;

    switch (csb->which) {
      case OSPF_AUTO_COST:
	ospf_auto_cost_cmd(csb, pdb);
	break;

      case OSPF_LOG_ADJ_CHG:
	ospf_log_adj_chg_cmd(csb, pdb);
	break;

      default:
	printf("\nOSPF: Unknown router command");
	break;
    }
}


/*
 * ospf_distance_command
 * Parse a "distance ospf" command.
 */
void ospf_distance_command (parseinfo *csb)
{
    pdbtype *pdb = csb->protocol;
    ulong distance1, distance2, distance3;
    
    if (!router_enable)
	return;

    if (!(pdb->proctype & PROC_OSPF))
	return;

    if (csb->nvgen) {
	/*
	 * NVGEN OSPF distance command iff not all 3 distances 
	 * are the same, which also implies that at least
	 * one distance is different from the default.
	 */
	if ((pdb->distance  != pdb->distance2) ||
	    (pdb->distance2 != pdb->distance3)) {
	    nv_write(TRUE, "%s ", csb->nv_command);
	    if (pdb->distance != pdb->def_distance)
		nv_add(TRUE, "intra-area %d ", pdb->distance);
	    if (pdb->distance2 != pdb->def_distance2)
		nv_add(TRUE, "inter-area %d ", pdb->distance2);
	    if (pdb->distance3 != pdb->def_distance3)
		nv_add(TRUE, "external %d", pdb->distance3);
	}
	return;
    }

    distance1 = GETOBJ(int,1);
    distance2 = GETOBJ(int,2);
    distance3 = GETOBJ(int,3);

    if (csb->sense) {
	if (distance1)
	    pdb->distance = distance1;
	if (distance2)
	    pdb->distance2 = distance2;
	if (distance3)
	    pdb->distance3 = distance3;
    } else {
	pdb->distance = pdb->def_distance;
	pdb->distance2 = pdb->def_distance2;
	pdb->distance3 = pdb->def_distance3;
    }
}

/*
 * OSPF subsystem header
 */

#define OSPF_MAJVERSION 1
#define OSPF_MINVERSION 0
#define OSPF_EDITVERSION  1

SUBSYS_HEADER(ospf, OSPF_MAJVERSION, OSPF_MINVERSION,
	      OSPF_EDITVERSION, ospf_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iprouting", "req: iprouting");
