/* $Id: ospf_dr.c,v 3.3.8.4 1996/07/18 08:36:06 myeung Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_dr.c,v $
 *------------------------------------------------------------------
 * ospf_dr.c -- This module provides functionality to election of Designated 
 *              and Backup Designated routers.
 *
 * December 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_dr.c,v $
 * Revision 3.3.8.4  1996/07/18  08:36:06  myeung
 * CSCdi62058:  DC-feature : loses neighbor with non DC code
 * Branch: California_branch
 * - Add ospf_dc_down_check() to bring P2P DC interface
 *   state to IFS_DOWN when nbr is lost
 *
 * Revision 3.3.8.3  1996/07/09  19:12:40  myeung
 * CSCdi61982:  OSPF network LSA is missing
 * Branch: California_branch
 * - Refine the fix for CSCdi47402
 *
 * Revision 3.3.8.2  1996/07/01  07:44:44  myeung
 * CSCdi56125:  Router links information incorrect for an internal stub
 * area router
 * Branch: California_branch
 * - Schedule SPF when we flush the old DR network LSA
 *
 * Revision 3.3.8.1  1996/05/23  07:37:49  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.3  1996/02/29  10:33:10  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.2  1995/11/17  17:35:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:09:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * routines exported from this module
 *
 *
 */
#include "master.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "ospf_pak.h"
#include "ospf.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */

/*
 * External declarations
 */

/*
 * Forward declarations
 */

/*
 * Storage
 */

/*
 * Macros
 */

/*
 * Functions
 *
 */

/*
 * ospf_max_nbr
 * Pick nbr which is more eligible as DR/BDR.
 */
static inline void ospf_max_nbr (nbrtype **max, nbrtype *nbr)
{
    if (!*max) {
	/*
	 * No max yet, just pick nbr.
	 */
	*max = nbr;
    } else {
	/*
	 * Choose max as the one with higher priority.
	 * Use id as tiebreaker.
	 */
	if ((nbr->nbr_pri > (*max)->nbr_pri) || 
	    ((nbr->nbr_pri == (*max)->nbr_pri) && 
	     (nbr->nbr_id > (*max)->nbr_id))) {
	    *max = nbr;
	}
    }
}

/*
 * ospf_elect_dr_common
 * Common function to elect dr and bdr.
 */ 
void ospf_elect_dr_common (nbrtype *nbr, nbrtype **dr_decl, 
			   nbrtype **dr_hi, boolean elect_bdr)
{
    /*
     * If we cannot see ourself, or are not eligible,
     * skip it.
     */
    if ((nbr->nbr_state < NBR_2WAY) || !nbr->nbr_pri)
	return;
    if (elect_bdr) {
	/*
	 * If we elect BDR, skip those whose declare itself as
	 * DR
	 */
	if (nbr->nbr_dr == nbr->nbr_ipaddr)
	    return;    
	/*
	 * Check if this neighbor declared as BDR 
	 */
	if (nbr->nbr_bdr == nbr->nbr_ipaddr) {
	    ospf_max_nbr(dr_decl, nbr);
	} else {
	    /* 
	     * Keep the highest address nbr for BDR election
	     */
	    ospf_max_nbr(dr_hi, nbr);
	}
    } else {
	/*
	 * Check if this neighbor declared as DR 
	 */
	if (nbr->nbr_dr == nbr->nbr_ipaddr) {
	    ospf_max_nbr(dr_decl, nbr);
	}
    }
}


/*
 * ospf_elect_bdr
 * Chooses backup designated router.
 * Event: nbr change or wait timer
 */
static void ospf_elect_bdr (idbtype *idb)
{
    nbrtype *nbr, *bdr_decl, *bdr_hi, *myself;
    ospf_idb_info *ospf_info = idb->ospf_info;

    nbr = bdr_decl = bdr_hi = NULL;
    myself = ospf_info->oi_myself;
    ospf_elect_dr_common(myself, &bdr_decl, &bdr_hi, TRUE);
    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
	ospf_elect_dr_common(nbr, &bdr_decl, &bdr_hi, TRUE);
    }	

    if (bdr_decl != NULL) {
	ospf_info->bdr = bdr_decl;
    } else { 
	if (bdr_hi != NULL)
	    ospf_info->bdr = bdr_hi;
	else
	    ospf_info->bdr = NULL;
    }
    if (ospf_info->bdr != myself) {
	myself->nbr_bdr = ospf_info->bdr ? ospf_info->bdr->nbr_ipaddr : 0;
    } else {
	myself->nbr_bdr = myself->nbr_ipaddr;
    }

    ospf_event(OC_ELECT_BDR, (ulong) idb,
	       (ulong) (ospf_info->bdr ? ospf_info->bdr->nbr_id : 0));
    ospf_adj_buginf("\nOSPF: Elect BDR %i", ospf_info->bdr ?
		    ospf_info->bdr->nbr_id : 0);
} /* end of ospf_elect_bdr */


/*
 * ospf_elect_dr	
 * Choose dr for an interface
 * Event: nbr change or wait timer
 */
static void ospf_elect_dr (idbtype *idb)
{
    nbrtype *nbr, *dr_decl, *myself;
    ospf_idb_info *ospf_info = idb->ospf_info;
    
    dr_decl = nbr = NULL;
    myself = ospf_info->oi_myself;
    /* choose dr */
    ospf_elect_dr_common(myself, &dr_decl, NULL, FALSE);
    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
	ospf_elect_dr_common(nbr, &dr_decl, NULL, FALSE);
    }
    if (dr_decl != NULL) {
	ospf_info->dr = dr_decl;
    } else {
	/* promote backup */
	ospf_info->dr = ospf_info->bdr;
    }
    if (ospf_info->dr != myself) {
	myself->nbr_dr = ospf_info->dr ? ospf_info->dr->nbr_ipaddr : 0;
    } else {
	myself->nbr_dr = myself->nbr_ipaddr;
    }

    ospf_event(OC_ELECT_DR, (ulong) idb,
	       (ulong) (ospf_info->dr ? ospf_info->dr->nbr_id : 0));
    ospf_adj_buginf("\nOSPF: Elect DR %i", ospf_info->dr ?
		    ospf_info->dr->nbr_id : 0);
} /* end of ospf_elect_dr */


/*
 * ospf_choose_dr	
 * Choose dr and bdr for intf
 * Event: nbr change or wait timer
 * Boolean immediately is TRUE if the interface is going down, so we
 * will flood the our network LSA immediately.
 */
boolean ospf_choose_dr (idbtype *idb, list_queue *flood_q, boolean immediately)
{
    nbrtype	*old_dr, *old_bdr;
    nbrtype	*nbr, *myself;
    nbrtype	*new_dr, *new_bdr;
    ospf_idb_info *ospf_info = idb->ospf_info;

    myself = ospf_info->oi_myself;
    if (!myself) {
	errmsg(&msgsym(NOSELF, OSPF), idb->namestring, ospf_info->if_state);
	return(FALSE);
    }
    old_dr = ospf_info->dr;
    old_bdr = ospf_info->bdr;
    new_bdr = new_dr = NULL;
    ospf_adj_buginf("\nOSPF: DR/BDR election on %s ", idb->namestring);
    ospf_info->bdr = NULL;
    ospf_info->dr = NULL;
    ospf_elect_bdr(idb);
    ospf_elect_dr(idb); 

    new_dr = ospf_info->dr;
    new_bdr = ospf_info->bdr;

    if (((old_dr == myself) && (new_dr != myself)) ||
	((old_bdr == myself) && (new_bdr != myself))  ||
	((old_dr != myself) && (new_dr == myself)) ||
	((old_bdr != myself) && (new_bdr == myself))) {
	ospf_elect_bdr(idb);
	ospf_elect_dr(idb);
	new_dr = ospf_info->dr;
	new_bdr = ospf_info->bdr;
    }

    /* set the state of the interface */
    if (new_dr == myself) {
	ospf_info->if_state = IFS_DR;
    } else if (new_bdr == myself) {
	ospf_info->if_state = IFS_BACKUP;
    } else {
	ospf_info->if_state = IFS_OTHER;
    }
    /* debug help */
    if (new_dr) {
	ospf_adj_buginf("\n       DR: %i (Id)", new_dr->nbr_id);
    } else
	ospf_adj_buginf("\n       DR: none ");
    if (new_bdr) {
	ospf_adj_buginf("   BDR: %i (Id)", new_bdr->nbr_id);
    } else 
	ospf_adj_buginf("   BDR: none ");

    if (ospf_info->if_type == IF_NONBROADCAST) {
	if ((myself != old_dr) && (myself != old_bdr) &&
	    ((myself == new_dr) || (myself == new_bdr))) {
	    /*
	     * Just become DR/BDR
	     */
	    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
		if (!nbr->nbr_pri)
		    (*(nbr_state_machine[NBR_EVENT_START][nbr->nbr_state]))
			(idb, nbr);
	    }
	}
    }
    if ((new_dr != old_dr) || (new_bdr != old_bdr)) {
	ospf_info->if_event_count++;
	for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
	    if (nbr->nbr_state >= NBR_2WAY)
		(*(nbr_state_machine[NBR_EVENT_ADJ_OK][nbr->nbr_state]))
		    (idb, nbr);
	}
    }
    /*
     * The following is Cisco specific.
     * We try keeping the old DR and avoid route flapping.
     */
    if ((new_dr == myself) && (old_dr != myself)) {
	if (mgd_timer_running(&ospf_info->oi_flush_net_timer)) {
	    ospf_adj_buginf("\nOSPF: Reset %s flush timer", idb->namestring);
	    mgd_timer_stop(&ospf_info->oi_flush_net_timer);
	}
	build_net_lsa(idb, flood_q, TRUE);
    }
    if ((new_dr != myself) && (old_dr == myself)) {
	/*
	 * We are no longer DR, set timer to remove our DR later
	 */
	if (immediately) {
	    ospf_adj_buginf("\nOSPF: Flush network LSA immediately");
	    age_net_lsa_flood(idb);
	} else {
	    ospf_adj_buginf("\nOSPF: Set %s flush timer", idb->namestring); 
	    mgd_timer_start(&ospf_info->oi_flush_net_timer,
			    FLUSH_NET_INTERVAL);
	}
    }
    /*
     * Remember the old DR
     */
    if (new_dr != old_dr)
	ospf_info->oi_old_dr = old_dr;
    return(FALSE);

}/* end of ospf_choose_dr */


/*
 * ospf_reset_adj
 * resets adjacencies on all interface in 
 * an area.
 */
void ospf_reset_adj (areatype *area)
{
    idbtype	*idb;
    nbrtype	*nbr;
    ospf_idb_info *ospf_info;

    for (idb = area->ar_if_list; idb; idb = idb->ospf_info->next) {
        ospf_info = idb->ospf_info;
	if (ospf_info->if_state <= IFS_WAITING)
	    continue;
	for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
	    (*(nbr_state_machine[NBR_EVENT_KILL_NBR][nbr->nbr_state]))
		(idb, nbr);	
	}
	ospf_dc_down_check(idb);
    }

} /* end of ospf_reset_adj */

/* end of ospf_dr.c */




