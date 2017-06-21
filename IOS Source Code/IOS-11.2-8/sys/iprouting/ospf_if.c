/* $Id: ospf_if.c,v 3.6.8.14 1996/08/16 22:59:20 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_if.c,v $
 *------------------------------------------------------------------
 * ospf.c -- Supports OSPF interface state machine
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_if.c,v $
 * Revision 3.6.8.14  1996/08/16  22:59:20  rbadri
 * CSCdi66287:  LANE: Clean up IPX/OSPF releated initializaion
 * Branch: California_branch
 * Function to setup broadcast flag for LANE.
 *
 * Revision 3.6.8.13  1996/08/16  22:10:47  myeung
 * CSCdi62650:  OSPF NSSA does not flush type 5 LSAs
 * Branch: California_branch
 *
 * Revision 3.6.8.12  1996/08/07  09:00:55  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.8.11  1996/07/19  07:26:59  myeung
 * CSCdi63444:  OSPF router should not be ABR if it dont see neighbor from
 * backbone
 * Branch: California_branch
 *
 * Revision 3.6.8.10  1996/07/18  08:36:10  myeung
 * CSCdi62058:  DC-feature : loses neighbor with non DC code
 * Branch: California_branch
 * - Add ospf_dc_down_check() to bring P2P DC interface
 *   state to IFS_DOWN when nbr is lost
 *
 * Revision 3.6.8.9  1996/07/09  19:12:45  myeung
 * CSCdi61982:  OSPF network LSA is missing
 * Branch: California_branch
 * - Refine the fix for CSCdi47402
 *
 * Revision 3.6.8.8  1996/06/28  23:19:06  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.8.7  1996/06/28  06:04:19  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.6.8.6  1996/05/24  05:31:11  shj
 * CSCdi58610:  OSPF non-operational on LANE subinterface
 * Branch: California_branch
 * Default to broadcast for ATM subinterfaces with configured LANE client.
 *
 * Revision 3.6.8.5  1996/05/23  12:06:45  klmj
 * CSCdi45519:  OSPF: Incorrect metric calculation for External Type
 * 1 routes (note:  this also fixes CSCdi44939)
 * Branch: California_branch
 *
 * Revision 3.6.8.4  1996/05/23  07:37:52  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.6.8.3  1996/04/16  19:01:50  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.6.8.2  1996/03/29  20:37:30  myeung
 * CSCdi52036:  Uninitialized timer message when unconfigure OSPF
 * Branch: California_branch
 *
 * Revision 3.6.8.1  1996/03/18  20:28:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.8.2  1996/03/16  06:57:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.8.1  1996/03/13  01:39:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6  1996/02/29  10:33:19  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.5  1996/01/08  18:01:55  myeung
 * CSCdi46551:  no ip ospf network broadcast disable OSPF
 *
 * Revision 3.4  1996/01/05  10:17:38  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/18  18:48:15  myeung
 * CSCdi43168:  Clean up OSPF flooding
 * - Consolidate flooding functions
 *
 * Revision 3.2  1995/11/17  17:36:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:06:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/09/26  11:35:46  myeung
 * CSCdi40729:  IP OSPF Hello Interval is not retained after reload
 *
 * Revision 2.7  1995/09/06  22:53:33  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Resolve problem for boot image
 *
 * Revision 2.6  1995/09/05  22:09:06  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.5  1995/08/09  17:29:32  myeung
 * CSCdi35840:  router reload at ospf_interface_up, check_if_abr
 * - Disable OSPF on unnumbered interface too when OSPF is
 *   disabled on the source interface.
 *
 * Revision 2.4  1995/08/07  19:19:44  myeung
 * CSCdi26731:  OSPF is turned off on an interface when secondary address
 * is removed
 *
 * Revision 2.3  1995/07/21  23:56:57  myeung
 * CSCdi35411:  ALIGN-3-SPURIOUS during OSPF configuration or router reload
 *
 * Revision 2.2  1995/06/15  18:45:15  myeung
 * CSCdi35900:  OSPF cost becomes 0 when if is configured with large
 * bandwidth
 * - Force minimum cost to be 1
 *
 * Revision 2.1  1995/06/07  21:09:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/old_timer_callbacks.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */
#include "../if/network.h"
#include "ospf_pak.h"
#include "ospf.h"
#include "ospf_public.h"
#include "../if/atm.h"
#include "../lane/lane.h"

/*
 * Forward declarations
 */

/*
 * Storage
 */

/*
 * ospf_scan_interface
 * Called because of OSPF_IF_JUMPSTART flag when we notice an interface
 * state inconsistency.
 */
void ospf_scan_interface (pdbtype *pdb)
{
    areatype *area;
    idbtype *idb;
    ospf_idb_info *ospf_info;

    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	for (idb = area->ar_if_list; idb ; idb = idb->ospf_info->next) {
	    ospf_info = idb->ospf_info;
	    if (ospf_info->if_type == IF_VIRTUAL_LINK)
		continue;
	    if ((ospf_info->if_state == IFS_DOWN) && 
		(interface_up(idb))) {
		(*(if_state_machine[IFE_UP][ospf_info->if_state])) 
		  (idb, NULL);
	    }
	}
    }
}

/*
 * ospf_save_neighbor_info
 * takes an OSPF idb and saves configured
 * NON-BROADCAST-MULTIACCESS neighbors in nbr_info
 * field. This routine should be called only on NBMA 
 * idbs.
 */
static void ospf_save_neighbor_info (idbtype *idb)
{
    nbrtype *nbr;
    nbrtype *nbr_info;
    ospf_idb_info *ospf_info = idb->ospf_info;

    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
	if ((nbr->nbr_flags & NBRF_CONFIGURED) == 0)
	    continue;
	if ((nbr_info = malloc(sizeof(nbrtype))) == NULL)
	    return;
	nbr_info->nbr_ipaddr = nbr->nbr_ipaddr;
	nbr_info->nbr_pri = nbr->nbr_pri;
	nbr_info->nbr_poll_interval = nbr->nbr_poll_interval;
	nbr_info->nbr_next = ospf_info->oi_cfg_nbr;
	ospf_info->oi_cfg_nbr = nbr_info;
    }
}

/*
 * ospf_if_disable:
 * stop interface from receiving 
 * ospf packets.
 */
static void ospf_if_disable (
    idbtype *idb)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_info->enabled = FALSE;
    if (ospf_info->if_type != IF_LOOPBACK)
	mgd_timer_stop(&ospf_info->oi_hello_master_timer);
    ospf_info->nbrcnt = 0;
    ospf_info->if_event_count = 0;
    ospf_info->nbr_fcnt = 0;
    ospf_info->dr = NULL;
    ospf_info->bdr = NULL;

    /*
     * remove OSPF multicast group reception from this interface
     */
    ip_multicast_ctl(idb, ALLSPFROUTERS, FALSE);
    ip_multicast_ctl(idb, ALLSPFDROUTERS, FALSE);

} /* end of ospf_if_disable */

/*
 * ospf_clean_if
 */
void ospf_clean_if (idbtype *idb)
{
    areatype *area;
    idbtype *temp;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_CLEAN_IF, (ulong) idb, 0);

    area = ospf_info->area_ptr;
    /*
     * if this interface is a NON-BROADCAST-MULTIACCESS network
     * and if this cleanup is due to router ID change, save the 
     * configured neighbor info.
     */
    if ( (ospf_info->if_type == IF_NONBROADCAST)
	&& (area->ar_pdb_ptr->ospf_flag & OSPF_ID_CHANGING))
	ospf_save_neighbor_info(idb);
    (*(if_state_machine[IFE_DOWN][ospf_info->if_state])) (idb, NULL);

    ospf_if_disable (idb);
    ospf_nbr_delete(idb, NULL, TRUE);
    ospf_delete_one_nbr(idb, ospf_info->oi_myself);
    ospf_info->oi_myself = NULL;
    ospf_info->oi_nbr = NULL;
    ospf_info->area_ptr = NULL;
    if (area->ar_if_list == idb) {
	area->ar_if_list = ospf_info->next;
	area->ar_ifcnt--;
    } else {
	for (temp = area->ar_if_list; temp->ospf_info->next; 
	     temp = temp->ospf_info->next) {
	    if (temp->ospf_info->next == idb) {
		temp->ospf_info->next = ospf_info->next;
		area->ar_ifcnt--;
		break;
 	    }
	}
    }
    if (area->ar_if_list == NULL) {
	mgd_timer_stop(&area->ar_update_timer);
	mgd_timer_stop(&area->ar_hello_master_timer);
    }
    ospf_info->next = NULL;
} /* end of ospf_clean_if */


/*
 * ospf_idb_address_change
 * Handle attach/detach OSPF interface when ip address changed
 * by the ip address command.
 */
void ospf_idb_address_change (idbtype *idb, ipaddrtype new_address,
			      ipaddrtype new_mask, boolean secondary,
			      boolean flag)
{
    areatype *area;
    ulong area_id;
    ipaddrtype address;
    ipaddrtype mask;
    pdbtype *pdb;
    int i;
    ospf_idb_info *ospf_info = idb->ospf_info;
    list_queue src_flood;
    idbtype *ospf_idb, *next_ospf_idb;

    if (flag == FALSE) {
	if (!ospf_info || !(ospf_info->enabled)) {
	    /*
	     * If any pdb use this address as ID, reselect router ID
	     */
	    for (pdb = ospf_pdbs_ptr; pdb; pdb = pdb->ospf_next) {
		if (pdb->ospf_rtr_id == new_address)
		    pdb->ospf_flag |= OSPF_SOFT_RESET;
	    }
	    return;
	}
	/*
	 * detach this interface from the
	 * area if primary address change.
	 */
	area = ospf_info->area_ptr;
	pdb = area->ar_pdb_ptr;
	if (!secondary) {
	    /*
	     * Primary address is removed.
	     *
	     * Remove the interface.
	     */
	    ospf_attach_interface(pdb, area, idb, FALSE);
	    /*
	     * Remove unnumbered interfaces that point to this removed
	     * interface.
	     */
	    for (ospf_idb = area->ar_if_list; ospf_idb;
		 ospf_idb = next_ospf_idb) {
		next_ospf_idb = ospf_idb->ospf_info->next;
		if (ospf_idb->ip_unnumbered == idb)
		    ospf_attach_interface(pdb, area, ospf_idb, FALSE);
	    }
	} else {
	    /*
	     * Secondary address is removed.
	     *
	     * Rebuild the RTR LSA will take care of it.
	     * If there is no change, build_rtr_lsa() will
	     * do nothing.
	     */
	    src_flood.q_first = src_flood.q_last = NULL;
	    build_rtr_lsa(area, &src_flood, FALSE);
	    flood_area(area, &src_flood, NULL, NULL);
	}
	if (pdb->ospf_rtr_id == new_address)
 	    pdb->ospf_flag |= OSPF_SOFT_RESET;
    } else {
	if (ospf_info && ospf_info->enabled) {
	    /*
	     * Add secondary address from the RTR LSA
	     * If there is no change, build_rtr_lsa() will
	     * do nothing.
	     */
	    area = ospf_info->area_ptr;
	    src_flood.q_first = src_flood.q_last = NULL;
	    build_rtr_lsa(area, &src_flood, FALSE);
	    flood_area(area, &src_flood, NULL, NULL);	    
	    return;
	}
	/*
	 * find OSPF pdb and check if it falls into
	 * any of their range.
	 */
	if (idb->ip_address == 0)
	    idb = idb->ip_unnumbered;
	/*
	 * If primary address not set, not ip unnumbered and
	 * secondary address just entered, idb will be NULL.
	 */
	if (!idb)
	    return;
	pdb = ospf_pdbs_ptr;
	for (; pdb ; pdb = pdb->ospf_next) {
	    if ((ip_up_ouraddress(pdb->ospf_rtr_id)) == FALSE)
		pdb->ospf_flag |= OSPF_SOFT_RESET;
	    for (i = 0; i < pdb->netcount; i++) {
		address = pdb->networks[i];
		mask = pdb->network_mask[i];
		area_id = pdb->network_info[i];
		if ((idb->ip_address & mask) == (address & mask)) {
		    ospf_parse_range(pdb, address, mask, area_id, 
                                     OSPF_AREA_ID_FORMAT_IGNORE, TRUE);
		    return;
		}
	    }
	}
    }
} /* end of ospf_idb_address_change */

/*
 * ospf_idb_change
 * Convenient function to attach/detach OSPF interface.
 * Called in pair in which the first call detach and the
 * second call attach the interface back. Used in case
 * when OSPF parameter changed.
 * Used when 
 * - encapsulation changed
 * - OSPF network type changed
 * - OSPF demand circuit changed
 */
void ospf_idb_change (
    idbtype *idb,
    boolean flag)
{
    areatype *area;
    ulong area_id;
    ipaddrtype address;
    ipaddrtype mask;
    pdbtype *pdb;
    int i;
    ospf_idb_info *ospf_info = idb->ospf_info;

    if (flag == FALSE) {
	if (!ospf_info || !(ospf_info->enabled))
	    return;
	/*
	 * just detach this interface from the
	 * area.
	 */
	area = ospf_info->area_ptr;
	pdb = area->ar_pdb_ptr;
	ospf_attach_interface(pdb, area, idb, FALSE);
    } else {
	if (ospf_info && ospf_info->enabled)
	    return;
	/*
	 * find OSPF pdb and check if it falls into
	 * any of their range.
	 */
	if (idb->ip_address == 0)
	    idb = idb->ip_unnumbered;
	/*
	 * If primary address not set, not ip unnumbered and
	 * secondary address just entered, idb will be NULL.
	 */
	if (!idb)
	    return;
	pdb = ospf_pdbs_ptr;
	for (; pdb ; pdb = pdb->ospf_next) {
	    for (i = 0; i < pdb->netcount; i++) {
		address = pdb->networks[i];
		mask = pdb->network_mask[i];
		area_id = pdb->network_info[i];
		if ((idb->ip_address & mask) == (address & mask)) {
		    ospf_parse_range(pdb, address, mask, area_id, 
                                     OSPF_AREA_ID_FORMAT_IGNORE, TRUE);
		    return;
		}
	    }
	}
    }
} /* end of ospf_idb_change */

/*
 * ospf_encap_fixup
 * Here when someone changes the interface encapsulation
 * Called by SERVICE_ENCAP_FIXUP
 */

void ospf_encap_fixup (idbtype *swidb, int old_encap)
{
    ospf_idb_info *ospf_info = swidb->ospf_info;

    if (ospf_info && ospf_info->enabled) {
	ospf_idb_change(swidb, FALSE);
	ospf_idb_change(swidb, TRUE);
    }
}

/*
 * ospf_configure_for_broadcast
 * When LANE client is configured on a swidb, make the if_type
 * to be IF_BROADCAST. The boolean is used to set or unset the 
 * if_type.
 */

void ospf_configure_for_broadcast (idbtype *swidb, boolean flag)
{
    ospf_idb_info *ospf_info = swidb->ospf_info;

    if (ospf_info) {
	if (flag) {
	    ospf_info->if_type = IF_BROADCAST;
	} else {
	    /* 
	     * Default to non-broadcast 
	     */
	    ospf_info->if_type = IF_NONBROADCAST;
	}
    }
}

/*
 * ospf_interface_up
 */
boolean ospf_interface_up (idbtype *idb)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    /*
     * Return FALSE if ospf is not on.
     * ospf_info must be non-NULL at this point.
     */
    if ((!ospf_info->enabled)
	|| (ospf_info->if_state <= IFS_DOWN)) 
	return(FALSE);
    return(TRUE);
} /* end of ospf_interface_up */

/*
 * ospf_route_adjust
 */
void ospf_route_adjust (idbtype *idb, boolean passive)
{
    pdbtype *pdb;
    ulong state_change = 0;
    ospf_idb_info *ospf_info = idb->ospf_info;

    /*
     * Check if we need to change router ID
     */
    for (pdb = ospf_pdbs_ptr; pdb ; pdb = pdb->ospf_next) {
	if ((ip_up_ouraddress(pdb->ospf_rtr_id)) == FALSE)
	    pdb->ospf_flag |= OSPF_SOFT_RESET;
    }

    /*
     * Return if ospf is not enabled or 
     * no transition occured on this interface.
     */
    if (!ospf_info || !(ospf_info->enabled) || passive)
	return;

    if ((interface_up(idb)) && (ospf_info->if_state == IFS_DOWN)) {
	(*(if_state_machine[IFE_UP][ospf_info->if_state])) (idb, NULL);
	state_change = OSPF_IDB_UP;
    } else if ((!interface_up(idb)) 
	     && (ospf_info->if_state > IFS_DOWN)) {
	(*(if_state_machine[IFE_DOWN][ospf_info->if_state])) (idb, NULL);
	state_change = OSPF_IDB_DOWN;
    }

    if (state_change)
	ospf_event(OC_ROUTE_ADJUST, (ulong) idb, state_change);

} /* end of ospf_route_adjust */


/*
 * if_ack_delayed
 */
void if_ack_delayed (idbtype *idb)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "if_ack_delayed", (ulong) idb);
    if (ospf_info->ack_queue.q_first !=NULL) {
	send_ack_list(&ospf_info->ack_queue, idb, NULL);
    }
} /* end of if_ack_delayed  */


/*
 * if_wait_tm
 * ends the wait time before
 * DR and BDR election.
 */
void if_wait_tm (idbtype *idb)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "if_wait_tm", (ulong) idb);
    (*(if_state_machine[IFE_WAIT_TIMER][ospf_info->if_state]))(idb, NULL);
} /* end of if_wait_tm  */


/*
 * ospf_start_intf:
 * starts ospf interface to receive
 * OSPF multicast packets.
 */
void ospf_start_intf (idbtype *idb)
{
    boolean flag;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_info->enabled = TRUE;

    /*
     * Join the OSPF multicast groups
     */

    ip_multicast_ctl(idb, ALLSPFROUTERS, TRUE);
    ip_multicast_ctl(idb, ALLSPFDROUTERS, TRUE);

    if (ospf_info->if_type == IF_VIRTUAL_LINK) {
	if (ospf_vidb_up(idb, &flag) == FALSE)
	    return;
    }
    /* 
     * generate an INTERFACE UP EVENT on this
     * interface.
     */

    (*(if_state_machine[IFE_UP][ospf_info->if_state]))
	                              (idb, NULL);
} /* end of ospf_start_intf */


/*
 * ospf_if_change_def_cost
 * Change the default metric of an interface.
 */
static inline void ospf_if_change_def_cost (
    ospf_idb_info *ospf_info, 
    ushort cost) 
{
    /*
     * The minimum cost is 1
     */
    if (cost <= 0)
	cost = 1;
    if (ospf_info->if_cost[0] == ospf_info->def_if_cost[0])
        ospf_info->def_if_cost[0] = ospf_info->if_cost[0] = cost;
    else
        ospf_info->def_if_cost[0] = cost;
}


/*
 * ospf_if_change_def_type
 * Change the default if type of an interface.
 */
static inline void ospf_if_change_def_type (
    ospf_idb_info *ospf_info, 
    ushort type) 
{
    if (ospf_info->if_type == ospf_info->def_if_type)
        ospf_info->def_if_type = ospf_info->if_type = type;
    else
        ospf_info->def_if_type = type;
}


/*
 * ospf_if_change_def_hello
 * Change the default hello interval of an interface.
 */
inline void ospf_if_change_def_hello (
    ospf_idb_info *ospf_info, 
    ulong hello) 
{
    if (ospf_info->hello_interval == ospf_info->def_hello_interval)
        ospf_info->def_hello_interval = ospf_info->hello_interval = hello;
    else
        ospf_info->def_hello_interval = hello;
}


/*
 * ospf_if_change_def_retr
 * Change the default retransmission interval of an interface.
 */
static inline void ospf_if_change_def_retr (
    ospf_idb_info *ospf_info, 
    ulong retr) 
{
    if (ospf_info->retr_interval == ospf_info->def_retr_interval)
        ospf_info->def_retr_interval = ospf_info->retr_interval = retr;
    else
        ospf_info->def_retr_interval = retr;
}


/*
 * ospf_if_change_def_transdly
 * Change the default transmission delay of an interface.
 */
static inline void ospf_if_change_def_transdly (
    ospf_idb_info *ospf_info, 
    ushort transdly) 
{
    if (ospf_info->if_transdly == ospf_info->def_if_transdly)
        ospf_info->def_if_transdly = ospf_info->if_transdly = transdly;
    else
        ospf_info->def_if_transdly = transdly;
}


/*
 * ospf_if_change_def_rtr_pri
 * Change the default router priority of an interface.
 */
static inline void ospf_if_change_def_rtr_pri (
    ospf_idb_info *ospf_info, 
    uchar rtr_pri) 
{
    if (ospf_info->rtr_pri == ospf_info->def_rtr_pri)
        ospf_info->def_rtr_pri = ospf_info->rtr_pri = rtr_pri;
    else
        ospf_info->def_rtr_pri = rtr_pri;
}

/*
 * ospf_if_change_def_dead
 * Change the default dead interval of an interface.
 */
inline void ospf_if_change_def_dead (
    ospf_idb_info *ospf_info,
    ulong dead)
{
    if (ospf_info->dead_interval == ospf_info->def_dead_interval)
        ospf_info->def_dead_interval = ospf_info->dead_interval = dead;
    else
        ospf_info->def_dead_interval = dead;
}

/*
 * ospf_if_get_def_type_cost
 *
 * Return the default network type of the interface acccording to idb type.
 * If auto cost is not enabled, calculate the default network cost
 * according to idb type.
 * If auto cost is enabled, calculate the cost based on line speed.
 */ 
static void ospf_if_get_def_type_cost (pdbtype *pdb, idbtype *idb,
				       ushort *type, ushort *cost)
{
    hwidbtype *hwidb = idb->hwptr;
    boolean  config_auto_cost, auto_cost = TRUE;
    ospf_idb_info *ospf_info = idb->ospf_info;
    ulong reference_bw, temp_cost;

    /* 
     * Not a valid operational value - it must be overridden 
     */
    *cost = 0;

    /*
     * We know that ospf_info is non-NULL here
     */

    /*
     * Set up the interface parameters.
     *
     * Note that the is_p2p check catches point-to-point subinterfaces
     * of WAN media.
     */
    if (is_p2p(idb)) {			/* Point-to-point? */
	*type = IF_POINT_TO_POINT;
	*cost = TOS0_IF_COST_SERIAL;
    } else if (is_wan(hwidb) && !is_atm_lane(idb)) {		/* WAN? */
        *type = IF_NONBROADCAST;

	if (is_atm(hwidb)) {
	    *cost = TOS0_IF_COST_ATM;
	} else {
	    *cost = TOS0_IF_COST_X25;
	}

    } else if (hwidb->status & IDB_VIRTUAL) { /* Virtual? */

 	if (hwidb->type == IDBTYPE_LB) { /* Loopback? */

 	    *type = IF_LOOPBACK;
 	    *cost = STUB_HOST_COST;
	    /*
	     * No auto cost for loopback
	     */
	    auto_cost = FALSE;

	} else if (is_lex(hwidb) || is_bvi(hwidb)) {
	    /* 
	     * LAN Extender and BVI interface
	     */
 	    *type = IF_BROADCAST;
	    /* 
	     * Assume Ethernet cost, as we don't know what is behind
	     * the LAN Extender or BVI interface exactly.
	     */
  	    *cost = TOS0_IF_COST_ETHER;
 	} else if (is_ospf_vl(hwidb)) {	
	    /* 
	     * OSPF Virtual link 
	     */
 	    *type = IF_VIRTUAL_LINK;
	    *cost = ospf_info->if_cost[0];
	    /*
	     * No auto cost for virtual link
	     */
	    auto_cost = FALSE;
 	} else {
	    /*
	     * Unrecognized new type of virtual interface
	     */
	    printf("\nOSPF: Unrecogized virtual interface %s. "
		   "Treat it as loopback stub route", idb->namestring);
	    *type = IF_LOOPBACK;
	    *cost = STUB_HOST_COST;
	    auto_cost = FALSE;
	}
    } else {				/* LAN */
 
	*type = IF_BROADCAST;

 	if (hwidb->status & IDB_ETHER) {
 	    *cost = TOS0_IF_COST_ETHER;
 	} else if (hwidb->status & IDB_FDDI) {
	    *cost = TOS0_IF_COST_FDDI;
 	} else if (hwidb->status & IDB_TR) {
 	    if (hwidb->tr_ring_speed == 4)
	      *cost = TOS0_IF_COST_TR;
 	    else
   	      *cost = TOS0_IF_COST_TR16;
  	} else if (hwidb->status & IDB_ATM) {
            *cost = TOS0_IF_COST_ATM;
        }

    }

    /*
     * If auto cost is enabled, calculate cost according to line speed.
     * If pdb is unknown when no OSPF is running over the idb,
     * treat it as if auto cost is enabled.
     */
    if (pdb) {
	reference_bw = pdb->ospf_pdb->ospf_reference_bw;
	config_auto_cost = pdb->ospf_auto_cost;
    } else {
	reference_bw = OSPF_DEF_REFERENCE_BW;
	config_auto_cost = TRUE;
    }
    if (config_auto_cost && auto_cost) {
	if (idb->visible_bandwidth) {
	    temp_cost = reference_bw * OSPF_METRIC_SCALE;
	    temp_cost = temp_cost / idb->visible_bandwidth;
	    if (temp_cost > RTR_LS_MAX_COST) {
		temp_cost = RTR_LS_MAX_COST;
	    } else if (temp_cost < 1) {
		temp_cost = 1;
	    }
	    *cost = temp_cost;
	} else {
	    errmsg(&msgsym(ZERO_BANDWIDTH, OSPF), idb->namestring);
	    *cost = RTR_LS_MAX_COST;
	}
    }
}


/*
 * ospf_if_set_default
 * Fill in the default value of an OSPF idb information.
 * ospf_info is passed in separately so that this function
 * can be used by SNMP to figure out the OSPF default value
 * without really initializing the OSPF field.
 */
void ospf_if_set_default (pdbtype *pdb, idbtype *idb, 
			  ospf_idb_info *ospf_info) 
{
    ushort type, cost = 0;  /* an invalid operational cost value */
    ospf_info->next = NULL;

    /*
     * No OSPF by default
     */
    ospf_info->enabled = FALSE;

    ospf_if_get_def_type_cost(pdb, idb, &type, &cost);

    ospf_if_change_def_type(ospf_info, type);
    ospf_if_change_def_cost(ospf_info, cost);

    ospf_info->if_state = IFS_DOWN;
    if ((ospf_info->if_type == IF_NONBROADCAST) || 
	(ospf_info->if_type == IF_POINT_TO_M_POINT))
	ospf_if_change_def_hello(ospf_info, HELLO_INTERVAL_NBMA);
    else
	ospf_if_change_def_hello(ospf_info, HELLO_INTERVAL);

    ospf_if_change_def_rtr_pri(ospf_info, DEFAULT_PRIORITY);
    ospf_if_change_def_dead(ospf_info, 4 * ospf_info->hello_interval);
    ospf_if_change_def_retr(ospf_info, RTR_INTERVAL);
    ospf_if_change_def_transdly(ospf_info, TRANSDELAY);
    ospf_info->oi_def_poll_interval = ospf_info->oi_poll_interval = 
	POLL_INTERVAL;
    ospf_info->wait_interval = ospf_info->dead_interval;
}
 

/*
 * ospf_if_init:
 * initializes ospf specific fields in
 * idb.
 */
boolean ospf_if_init (pdbtype *pdb, areatype *area, idbtype *idb)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    /*
     * ospf_info is assumed to be non-null here
     */
    ospf_if_set_default(pdb, idb, ospf_info);

    ospf_info->delay_flag = 0;
    ospf_info->nbrcnt = 0;
    ospf_info->nbr_fcnt = 0;
    ospf_info->dr = NULL;
    ospf_info->bdr = NULL;
    ospf_info->oi_nbr = NULL;
    mgd_timer_init_parent(&ospf_info->oi_hello_master_timer,
			  &area->ar_hello_master_timer);
    mgd_timer_init_leaf(&ospf_info->oi_hello_timer,
			&ospf_info->oi_hello_master_timer,
			OSPF_IF_HELLO_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&ospf_info->oi_ack_timer,
			&ospf_info->oi_hello_master_timer,
			OSPF_IF_ACK_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&ospf_info->oi_wait_timer,
			&ospf_info->oi_hello_master_timer,
			OSPF_IF_WAIT_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&ospf_info->oi_delay_timer,
			&ospf_info->oi_hello_master_timer,
			OSPF_IF_DELAY_TIMER, idb, FALSE);
    mgd_timer_init_leaf(&ospf_info->oi_flush_net_timer,
			&ospf_info->oi_hello_master_timer,
			OSPF_IF_FLUSH_NET_TIMER, idb, FALSE);
    /*
     * Create myself entry
     * This entry is used in DR election. Moreover, its state
     * indicate if i_up() has been done over the interface.
     * The nbr state will be NBR_2WAY if i_up() is performed; 
     * NBR_DOWN otherwise.
     */
    ospf_info->oi_myself = ospf_create_one_nbr(idb, pdb->ospf_rtr_id, 
					       idb->ip_address);
    if (!ospf_info->oi_myself)
	return(FALSE);
    ospf_info->oi_myself->nbr_pri = ospf_info->rtr_pri;

    return(TRUE);
} /* end of ospf_if_init */


/*
 * ospf_if_recalculate_cost
 * A convenient function to change the default cost without paying
 * attention to the auto cost configuration stuff. 
 */
inline void ospf_if_recalculate_cost (pdbtype *pdb, idbtype *idb)
{
    ushort     type, cost;

    ospf_if_get_def_type_cost(pdb, idb, &type, &cost);
    ospf_if_change_def_cost(idb->ospf_info, cost);
}


/*
 *
 * ospf_bandwidth_changed
 * Notification that the bandwidth of an interface has changed.
 * Set ospf interface cost according to bandwidth;
 */
void ospf_bandwidth_changed (idbtype *idb) {
    ospf_idb_info *ospf_info = idb->ospf_info;
    areatype *area;
    list_queue flood_queue;

    if (!ospf_info)
        return;
    area = ospf_info->area_ptr;
    if (area && ospf_info->enabled) {
	if (area->ar_pdb_ptr->ospf_auto_cost) {
	    ospf_if_recalculate_cost(area->ar_pdb_ptr, idb);
	    flood_queue.q_first = flood_queue.q_last = NULL;
	    build_rtr_lsa(area, &flood_queue, FALSE);
	    flood_area(area, &flood_queue, NULL, NULL);
	}
    } else {
	ospf_if_recalculate_cost(NULL, idb);
    }
}


/*
 * i_error
 * Invalid interface
 * state transition
 */
static void i_error (
    idbtype 	*idb,
    nbrtype 	*nbr)
{
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_info->if_event_count++;
    ospf_event(OC_I_ERROR, (ulong) idb, ospf_info->if_state);
    ospf_adj_buginf("\nOSPF: i_error : Bad interface state change");
} /* end of i_error */


/*
 * i_up
 */
static void i_up (
    idbtype *idb,
    nbrtype *null_ptr)
{
    nbrtype *nbr;
    ulong nbr_rtr_id;
    list_queue flood_queue;
    areatype *area;
    pdbtype  *pdb;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_I_UP, (ulong) idb, ospf_info->if_state);

    /* return if interface is down */
    if (!(interface_up(idb))) {
        return;
    }
    area = ospf_info->area_ptr;
    pdb = area->ar_pdb_ptr;
    ospf_adj_buginf("\nOSPF: Interface %s going Up", idb->namestring);
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;
    nbr_rtr_id = ospf_info->area_ptr->ar_pdb_ptr->ospf_rtr_id;
    /*
     * Set the state of myself entry as an indication that
     * UP event is performed.
     */
    ospf_info->oi_myself->nbr_state = NBR_2WAY;
    switch(ospf_info->if_type) {
      case IF_BROADCAST:
	ospf_info->if_state = IFS_WAITING;
	mgd_timer_start(&ospf_info->oi_wait_timer, 
			ospf_info->wait_interval*ONESEC);
	ospf_send_hello(ospf_info->area_ptr, idb, NULL);
	break;
      case IF_POINT_TO_POINT:
      case IF_POINT_TO_M_POINT:
      case IF_VIRTUAL_LINK:
	if (ospf_info->oi_demand_circuit && 
	    (ospf_info->if_type == IF_POINT_TO_POINT)) {
	    /*
	     * P2P demand circuit is up if and only if there is nbr with
	     * state 1_way or higher
	     */
	    ospf_info->if_state = IFS_DOWN;
	} else {
	    ospf_info->if_state = (ospf_info->if_type == IF_POINT_TO_M_POINT) ?
		IFS_POINT_TO_M_POINT : IFS_POINT_TO_POINT;
	}
	ospf_send_hello(ospf_info->area_ptr, idb, NULL);
	break;
      case IF_NONBROADCAST:
	/*
	 * neighbor structures allocated during init time 
	 * with poll timer set.
	 */
	for (nbr = ospf_info->oi_nbr; nbr ; nbr = nbr->nbr_next)
		(*(nbr_state_machine[NBR_EVENT_START][nbr->nbr_state]))
		                                    (idb, nbr);
	if (ospf_info->rtr_pri) {
	    ospf_info->if_state = IFS_WAITING;
	    mgd_timer_start(&ospf_info->oi_wait_timer, 
			    ospf_info->wait_interval*ONESEC);
	} else 
	    ospf_info->if_state = IFS_OTHER;
	break;
      case IF_LOOPBACK:
	ospf_info->if_state = IFS_LOOPBACK;
	break;
    }
    ospf_info->if_event_count++;
    build_rtr_lsa(ospf_info->area_ptr, &flood_queue, FALSE);
    flood_area(area, &flood_queue, NULL, NULL);
    area->ar_pdb_ptr->ospf_flag |= OSPF_CHECK_ABR;
    ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE);
} /* end of i_up */


/* 
 * i_down
 * Interface has gone down or interface is 
 * being detached.
 */
static void i_down (
    idbtype *idb,
    nbrtype *nbr)
{
    list_queue flood_queue;
    areatype *area;
    pdbtype  *pdb;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_I_DOWN, (ulong) idb, ospf_info->if_state);
    area = ospf_info->area_ptr;
    pdb = area->ar_pdb_ptr;
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;
    ospf_adj_buginf("\nOSPF: Interface %s going Down",idb->namestring);
    /* reset intf variables */
    mgd_timer_stop(&ospf_info->oi_wait_timer);
    /* 
     * Kill all neighbours 
     * Perform n_down() on the myself entry also to bring its state
     * to NBR_DOWN.
     */
    (*(nbr_state_machine[NBR_EVENT_KILL_NBR][ospf_info->oi_myself->nbr_state]))
	(idb, ospf_info->oi_myself);
    for (nbr = ospf_info->oi_nbr; nbr != NULL; nbr = nbr->nbr_next) {
	(*(nbr_state_machine[NBR_EVENT_KILL_NBR][nbr->nbr_state]))(idb, nbr);
    }
    if (ospf_info->if_type != IF_NONBROADCAST) {
	/* delete all neighbours on this interface */
	ospf_nbr_delete(idb, NULL, TRUE);
    } else {
	ospf_nbr_delete(idb, NULL, FALSE);
    }
    ospf_info->oi_old_dr = NULL;
    ospf_info->dr = NULL;
    ospf_info->bdr = NULL;
    ospf_info->nbrcnt = 0;
    ospf_info->nbr_fcnt = 0;
    ospf_info->oi_suppress_hello_count = 0;
    ospf_info->if_state = IFS_DOWN;
    ospf_info->oi_demand_circuit = ospf_info->oi_cfg_demand_circuit;
    ospf_info->if_event_count++;
    ospf_free_ack_list(&ospf_info->ack_queue);
    build_rtr_lsa(ospf_info->area_ptr, &flood_queue, FALSE);
    flood_area(area, &flood_queue, NULL, NULL);
    area->ar_pdb_ptr->ospf_flag |= OSPF_CHECK_ABR;
    ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE);
} /* end of i_down */


/* 
 * i_loop
 * Interface has gone into 
 * loop back state.
 */
static void i_loop (
    idbtype *idb,
    nbrtype *nbr)
{
    list_queue flood_queue;
    areatype *area;
    ospf_idb_info *ospf_info = idb->ospf_info;
    
    ospf_info->if_event_count++;
    ospf_event(OC_I_LOOP, (ulong) idb, ospf_info->if_state);
    ospf_adj_buginf("\nOSPF: Interface %s going to Loop state",
  		    idb->namestring);
    ospf_info->if_state = IFS_LOOPBACK;
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;
    area = ospf_info->area_ptr;
    build_rtr_lsa(area, &flood_queue, FALSE);
    flood_area(area, &flood_queue, NULL, NULL);
} /* end of loop */


/* 
 * i_unloop
 * Interface has gone into 
 * unloop back state.
 */
static void i_unloop (
    idbtype *idb,
    nbrtype *nbr)
{
    list_queue flood_queue;
    areatype *area;
    ospf_idb_info *ospf_info = idb->ospf_info;
 
    ospf_event(OC_I_UNLOOP, (ulong) idb, ospf_info->if_state);
    ospf_adj_buginf("\nOSPF: Interface %s going to Unloop state",
		    idb->namestring);
    ospf_info->if_event_count++;
    ospf_info->if_state = IFS_DOWN;
    
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;
    area = ospf_info->area_ptr;
    build_rtr_lsa(area, &flood_queue, FALSE);
    flood_area(area, &flood_queue, NULL, NULL);
} /* end of unloop */


/*
 * i_waittm:
 * Wait timer has gone off from state WAIT.
 * time to elect DR and BDR
 */
static void i_waittm (
    idbtype	*idb,
    nbrtype	*nbr)
{
    list_queue flood_queue;
    areatype *area;
    ospf_idb_info *ospf_info = idb->ospf_info;
    
    ospf_event(OC_I_WAITTM, (ulong) idb, ospf_info->if_state);
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;
    area = ospf_info->area_ptr;
    ospf_adj_buginf("\nOSPF: end of Wait on interface %s",
		                     idb->namestring);
    /* if we are elig and there are neighbors */
    /* reform adjacencies */
    ospf_choose_dr(idb, &flood_queue, FALSE);
    build_rtr_lsa(area, &flood_queue, FALSE);
    flood_area(area, &flood_queue, NULL, NULL);
} /* end of i_waittn */


/*
 * i_backup
 * Backup has been seen while in
 * Waitstate and time to elect BDR and DR
 */
static void i_backup (
    idbtype	*idb,
    nbrtype	*nbr)
{
    list_queue flood_queue;
    areatype *area;
    ospf_idb_info *ospf_info = idb->ospf_info;
 
    ospf_event(OC_I_BACKUP, (ulong) idb, ospf_info->if_state);
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;

    area = ospf_info->area_ptr;
    ospf_adj_buginf("\nOSPF: Backup seen Event before WAIT timer on %s",
		               idb->namestring);
    mgd_timer_stop(&ospf_info->oi_wait_timer);
    /* if we are elig and there are neighbors */
    ospf_choose_dr(idb, &flood_queue, FALSE);
    (void)build_rtr_lsa( area, &flood_queue, FALSE);
    flood_area(area, &flood_queue, NULL, NULL);
} /* end of i_backup */


/*
 * i_nbrch
 */
static void i_nbrch (
    idbtype *idb,
    nbrtype *nbr)
{
    list_queue flood_queue;
    areatype *area;
    boolean immediately;
    ospf_idb_info *ospf_info = idb->ospf_info;
    
    ospf_event(OC_I_NBRCH, (ulong) idb, ospf_info->if_state);
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;

    area = ospf_info->area_ptr;
    ospf_adj_buginf("\nOSPF: Neighbor change Event on interface %s",
		    idb->namestring);
    /*
     * Is interface going down ?
     */
    immediately = (nbr && (ospf_info->oi_myself == nbr));
    (void)ospf_choose_dr(idb, &flood_queue, immediately);
    (void)build_rtr_lsa( area, &flood_queue, FALSE);
    flood_area(area, &flood_queue, NULL, NULL);
} /* end of i_nbrch */


/*
 * ospf_dc_down_check
 * This function is called after N_DOWN event (except within I_DOWN)
 * to bring down P2P DC interface.
 */
void ospf_dc_down_check (idbtype *idb)
{
    list_queue flood_queue;
    ospf_idb_info *ospf_info = idb->ospf_info;
    
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;

    /*
     * P2P demand circuit interface is up if and only if the neighbor
     * is in 1 way or higher.
     * We don't call i_down() for several reason -
     * 1. It is not an event
     * 2. i_down() means the interface is not functional and that
     *	  cause us not receiving hello.
     */
    if ((ospf_info->if_type == IF_POINT_TO_POINT) &&
	(ospf_info->nbrcnt == 0) &&
	(ospf_info->if_state > IFS_DOWN) &&
	(ospf_info->oi_demand_circuit)) {
	ospf_adj_buginf("\nOSPF: Nbr down force DC interface %s down",
			idb->namestring);
	ospf_info->if_state = IFS_DOWN;
	ospf_info->oi_demand_circuit = ospf_info->oi_cfg_demand_circuit;
	build_rtr_lsa(ospf_info->area_ptr, &flood_queue, FALSE);
	flood_area(ospf_info->area_ptr, &flood_queue, NULL, NULL);
    }
}


void	(*const if_state_machine[IF_EVENTS][IF_STATES])(idbtype *, nbrtype *) = {
 
/*
 State:DOWN    LOOPBACK WAITING  P2MP     P2P      DR       BACKUP   OTHER
 Event */
/* UP*/
     { i_up,   i_error, i_error, i_error, i_error, i_error, i_error, i_error},
/*WAIT_TM*/
     { i_error,i_error, i_waittm,i_error, i_error, i_error, i_error, i_error},
/*BACKUP*/
     { i_error,i_error, i_backup,i_error, i_error, i_error, i_error, i_error},
/*NBR_CH*/
     { i_error,i_error, i_error ,i_error, i_error, i_nbrch, i_nbrch, i_nbrch}, 
/*LOOP*/
     { i_loop, i_error, i_error, i_error, i_error, i_error, i_error, i_error},
/*UNLOOP*/
     { i_error,i_unloop,i_error, i_error, i_error, i_error, i_error, i_error}, 
/*DOWN*/
     { i_error,i_down,  i_down,  i_down,  i_down,  i_down,  i_down,  i_down },
};

