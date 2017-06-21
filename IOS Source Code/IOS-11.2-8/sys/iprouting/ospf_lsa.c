/* $Id: ospf_lsa.c,v 3.11.8.25 1996/09/12 21:19:08 myeung Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_lsa.c,v $
 *------------------------------------------------------------------
 * ospf_lsa.c -- Builds Link State Advertisements
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_lsa.c,v $
 * Revision 3.11.8.25  1996/09/12  21:19:08  myeung
 * CSCdi61022:  OSPF fails to delete old subnet mask when mask changed.
 * Branch: California_branch
 * - Fix the case for inter-area route.
 *
 * Revision 3.11.8.24  1996/09/07  11:14:44  myeung
 * CSCdi67884:  Configuration of ospf caused 7K router to crash
 * Branch: California_branch
 *
 * Revision 3.11.8.23  1996/08/28  20:13:49  myeung
 * CSCdi64310:  match internal keyword broken in OSPF redistribution
 * Branch: California_branch
 *
 * Revision 3.11.8.22  1996/08/28  12:53:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.8.21  1996/08/26  22:28:42  myeung
 * CSCdi63803:  2nd type7 lsa is generated and translated when interface
 * is shut
 * Branch: California_branch
 *
 * Revision 3.11.8.20  1996/08/23  21:36:43  myeung
 * CSCdi66427:  LSA refresh is sent across OSPF demand circuit
 * Branch: California_branch
 *
 * Revision 3.11.8.19  1996/08/16  22:10:53  myeung
 * CSCdi62650:  OSPF NSSA does not flush type 5 LSAs
 * Branch: California_branch
 *
 * Revision 3.11.8.18  1996/08/05  08:59:59  myeung
 * CSCdi64894:  ospf nssa type-7 not translated after clear ip ospf red on
 * abr
 * Branch: California_branch
 *
 * Revision 3.11.8.17  1996/07/31  06:10:47  myeung
 * CSCdi63983:  Incorrect forward address set when enabling 2 ospf on
 * forwarding int
 * Branch: California_branch
 *
 * Revision 3.11.8.16  1996/07/19  07:27:06  myeung
 * CSCdi63444:  OSPF router should not be ABR if it dont see neighbor from
 * backbone
 * Branch: California_branch
 *
 * Revision 3.11.8.15  1996/07/09  19:12:52  myeung
 * CSCdi61982:  OSPF network LSA is missing
 * Branch: California_branch
 * - Refine the fix for CSCdi47402
 *
 * Revision 3.11.8.14  1996/07/01  07:44:49  myeung
 * CSCdi56125:  Router links information incorrect for an internal stub
 * area router
 * Branch: California_branch
 * - Schedule SPF when we flush the old DR network LSA
 *
 * Revision 3.11.8.13  1996/06/28  23:19:13  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.8.12  1996/06/25  21:15:57  myeung
 * CSCdi45110:  OSPF does not load balance over External routes
 * Branch: California_branch
 * - Modify OSPF router route data structure and external route calculation
 *
 * Revision 3.11.8.11  1996/05/23  12:06:50  klmj
 * CSCdi45519:  OSPF: Incorrect metric calculation for External Type
 * 1 routes (note:  this also fixes CSCdi44939)
 * Branch: California_branch
 *
 * Revision 3.11.8.10  1996/05/11  00:08:18  myeung
 * CSCdi55640:  sh ip os da asb output incorrect
 * - Set db_rtr_reachable correctly for self-originated type 4 LSA
 * Branch: California_branch
 *
 * Revision 3.11.8.9  1996/05/10  17:40:45  myeung
 * CSCdi48102:  OSPF self-originated MAXAGE external LSA stuck in database
 * Branch: California_branch
 *
 * Revision 3.11.8.8  1996/05/08  04:27:19  myeung
 * CSCdi56577:  Type-7 summary is not removed from database when summary
 * is disabled
 * Branch: California_branch
 *
 * Revision 3.11.8.7  1996/05/04  01:22:02  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.11.8.6  1996/05/01  08:06:53  myeung
 * CSCdi55367:  Summary from NSSA area does not get outside NSSA
 * Branch: California_branch
 * - Set forwarding address for summary type 7 LSA
 *
 * Revision 3.11.8.5  1996/04/24  01:28:56  myeung
 * CSCdi54714:  no-redistribution does not work in NSSA
 * Branch: California_branch
 *
 * Revision 3.11.8.4  1996/04/23  06:17:52  myeung
 * CSCdi54643:  Routers crashed when bad nssa area # is used for default
 * route
 * Branch: California_branch
 *
 * Revision 3.11.8.3  1996/04/23  05:10:07  myeung
 * CSCdi54956:  NSSA summary address with tag is not working
 * Branch: California_branch
 *
 * Revision 3.11.8.2  1996/04/16  19:01:54  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.11.8.1  1996/03/18  20:28:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.3  1996/03/16  06:57:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.2  1996/03/07  09:49:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.2.1  1996/02/20  01:03:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/02/29  10:33:23  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.10  1996/01/25  12:40:03  myeung
 * CSCdi47498:  Clean up OSPF redistribution
 *
 * Revision 3.9  1996/01/22  06:14:31  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/18  20:42:29  myeung
 * CSCdi46393:  SegV Exception in ospf_send_external_lsa
 *
 * Revision 3.7  1996/01/17  20:01:51  myeung
 * CSCdi46409:  AGS+ crashes during OSPF_redist_summary_check
 *
 * Revision 3.6  1996/01/05  10:17:41  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.5  1996/01/03  00:44:19  myeung
 * CSCdi46315:  OSPF-4-NOTREDIST4 message again
 * - Don't set advertisingmask if no external LSA is generated
 *
 * Revision 3.4  1995/12/27  22:41:00  myeung
 * CSCdi46152:  OSPF-4-NOTREDIST4 message for 0.0.0.0
 *
 * Revision 3.3  1995/12/21  05:32:14  myeung
 * CSCdi44588:  OSPF tries to delete MAXAGE LSA when no delete bit is set
 *
 * Revision 3.2  1995/11/17  17:36:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.20  1995/11/08  21:06:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.19  1995/10/14  00:26:49  myeung
 * CSCdi41883:  OSPF LSA fragmentation crashes router
 * - Allow to send update containing huge LSA
 *
 * Revision 2.18  1995/10/13  01:30:39  myeung
 * CSCdi42048:  Boolean in OSPF db structure should be trimmed
 * - Replace db_sum_origin, db_where and other booleans with
 *   bit field variables.
 *
 * Revision 2.17  1995/10/12  11:51:00  myeung
 * CSCdi42047:  Clear up OSPF db flags
 * - Replace bit mask with bit field variable.
 *
 * Revision 2.16  1995/10/08  23:44:32  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 * - Backout the patch
 * - Decide to use bit variable instead of bit mask to replace boolean
 *
 * Revision 2.15  1995/10/08  22:49:44  myeung
 * CSCdi41790:  Boolean db_free in db structure should be replaced by bit
 * field
 *
 * Revision 2.14  1995/09/25  11:49:53  myeung
 * CSCdi40976:  OSPF should send back most recent copy when old lsa is
 * received
 *
 * Revision 2.13  1995/09/19  20:53:53  myeung
 * CSCdi20080:  OSPF crashed when sending LSA in send_ls_update
 *
 * Revision 2.12  1995/09/11  23:50:34  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.11  1995/09/05 22:09:10  myeung
 * CSCdi39807:  OSPF need access-list debugging
 * - Add access-list debugging for SPF calculation and flooding
 * - Add separate debugging for intra-area, inter-area and external SPF
 *
 * Revision 2.10  1995/08/31  04:20:25  myeung
 * CSCdi39623:  OSPF should not generate host route for point-to-point
 * network
 *
 * Revision 2.9  1995/08/30  09:20:03  myeung
 * CSCdi38044:  CPU HOG message
 *
 * Revision 2.8  1995/08/26  00:05:03  myeung
 * CSCdi39228:  OSPF ABR generating illegal Type 4 LSA
 *
 * Revision 2.7  1995/08/11  22:49:08  myeung
 * CSCdi38232:  redistribute connected should not redistribute static
 * route to OSPF
 *
 * Revision 2.6  1995/08/01  15:15:57  myeung
 * CSCdi36150:  OSPF database synchronization problem after route recovers
 * - Always take care of the elasped time when calculating LSA age
 * - The maximum age used for comparsion between LSA should be 3600
 *
 * Revision 2.5  1995/06/28  03:21:33  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.4  1995/06/20 06:55:10  myeung
 * CSCdi34206:  OSPF Memory Leak on border area router
 *
 * Revision 2.3  1995/06/19  01:08:11  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:06:29  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:09:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Routines exported from this mode
 * build_rtr_lsa:	Builds RTR LSA and installs it if necessary
 * build_net_lsa:
 */

#include "master.h"
#include <ciscolib.h>
#include "../os/old_timer_callbacks.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "route_inlines.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../if/network.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */
#include "../os/buffers.h"
#include "../util/tree.h"
#include "ospf_public.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"
#include "route_map.h"
#include "../ip/ip_registry.h"
#include "ospf_inlines.h"

/*
 * External declarations
 */
extern void compute_iso_checksum(uchar *, int, uchar *);
extern void ospf_dec_lsa_table(pdbtype *, areatype *, uchar, ushort);
extern void ospf_inc_lsa_table(pdbtype *, areatype *, uchar, ushort);

/*
 * Forward declarations
 */


static inline boolean
acl_debug_ospf_lsa_gen (ipaddrtype address, ipaddrtype mask)
{
    acl_headertype *acl;

    if (!ospf_debug_lsa_gen)
	return(FALSE);

    if (ospf_lsa_gen_acl) {
	acl = acl_array[ospf_lsa_gen_acl];
	return(route_check(acl, address, mask));
    }
    
    return(TRUE);
}


/*
 * ar_update_lsa
 */
void ar_update_lsa (areatype *area)
{
    idbtype	*idb;
    list_queue 	flood_queue;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "ar_update", area->ar_id);
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_FREEZE)
	return;
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_ACTIVE) {
	mgd_timer_start(&area->ar_update_timer, MINLSINTERVAL * ONESEC);
	return;
    }
    ospf_buginf("\nOSPF: Router/Net Link state Update Timer in area %s",
		area->ar_id_str);
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;
    (void)build_rtr_lsa(area, &flood_queue, TRUE);
    for (idb = area->ar_if_list; idb; idb = idb->ospf_info->next)
	if (idb->ospf_info->if_state == IFS_DR)
	    (void)build_net_lsa(idb, &flood_queue, TRUE);
    flood_area(area, &flood_queue, NULL, NULL);
    mgd_timer_start(&area->ar_update_timer, LS_REFRESH_TIME*ONESEC);
} /* end of ar_update_lsa */


/*
 * ar_delayed_rtr_lsa
 */
void ar_delayed_rtr_lsa (areatype *area)
{
    list_queue flood_queue;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "ar_delay", area->ar_id);
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_FREEZE)
	return;
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_ACTIVE) {
	mgd_timer_start(&area->ar_delay_timer, MINLSINTERVAL * ONESEC);
	return;
    }
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;

    if (area->ar_delay_flag) {
	area->ar_delay_flag = 0;
	(void)build_rtr_lsa(area, &flood_queue, FALSE);
	flood_area(area, &flood_queue, NULL, NULL);
    }
}/* end of ar_delayed_rtr_lsa */


/*
 * if_delayed_net_lsa
 */
void if_delayed_net_lsa (idbtype *idb)
{
    list_queue flood_queue;
    ospf_idb_info *ospf_info = idb->ospf_info;
    areatype *area = ospf_info->area_ptr;

    if (!area)
	return;
    ospf_event(OC_TIMER_EXPIRED, (ulong) "ospf_delay", (ulong) idb);
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_FREEZE)
	return;
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_ACTIVE) {
	mgd_timer_start(&ospf_info->oi_delay_timer, MINLSINTERVAL*ONESEC);
	return;
    }
    flood_queue.q_first = NULL;
    flood_queue.q_last = NULL;

    if ((ospf_info->delay_flag) && (ospf_info->if_state == IFS_DR)) {
	(void)build_net_lsa(idb, &flood_queue, 
			    (ospf_info->delay_flag & DELAY_NET_LSA_UPDATE)
			    ? TRUE : FALSE);
	flood_area(area, &flood_queue, NULL, NULL);
    }
    ospf_info->delay_flag = 0;
}/* ar_delayed_net_lsa */


/*
 * pdb_delayed_ex_lsa
 */
void pdb_delayed_ex_lsa (pdbtype *pdb)
{
    list_queue flood_list;
    boolean delay_flag;
    lsdbtype *db;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;
    int count = 0;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "exlock", 0);
    if (pdb->ospf_dbase_lock == DBASE_FREEZE)
	return;
    if (pdb->ospf_dbase_lock == DBASE_ACTIVE) {
	mgd_timer_start(&ospf_pdb->ospf_exlock_timer, MINLSINTERVAL*ONESEC);
	return;
    }
    INIT_OSPF_SUSPEND(pdb);
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    delay_flag = FALSE;
    for (db = db_get_first_db(pdb->ospf_ex_lsdbase); db;
	 db = db_get_next_db(db)) {
	check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
	if (db->db_lsa_ptr == NULL) 
	    continue;
	if (db->db_flood_dly) {
	    if (HOLD_TIMER_CHECK(db->db_time, MINLSINTERVAL)) {
		delay_flag = TRUE;
		continue;
	    }
	    db->db_flood_dly = FALSE;
	    GET_TIMESTAMP(db->db_time);
	    add_to_flood_list(db, &flood_list);
	}
	if (!count)
	    ex_flood_area(pdb, &flood_list, NULL, NULL);
    }
    if (delay_flag)
	mgd_timer_start(&ospf_pdb->ospf_exlock_timer, MINLSINTERVAL*ONESEC);
    if (ex_flood_area(pdb, &flood_list, NULL, NULL))
	ospf_flood_buginf("\nOSPF: Sending Delayed External Links");
} /* end of pdb_delayed_ex_lsa */

/*
 * pdb_exupdate_sub
 * Refresh external LSA.
 */
static void pdb_exupdate_sub (areatype *area, uchar type)
{
    lsdbtype	*db;
    ex_lsatype	*ex_lsa;
    pdbtype	*pdb;
    ospf_pdb_info *ospf_pdb;
    int		count = 0;
    list_queue	flood_list;
    mgd_timer	*lock_timer;

    pdb = area->ar_pdb_ptr;
    ospf_pdb = pdb->ospf_pdb;
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;

    for (db = db_get_first_db(area->ar_lsdbase[type]); db;
        db = db_get_next_db(db)) {
	check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
	if ((db->db_lsa_ptr == NULL) 
	    || (ADV_AGE(db) >= MAXAGE) || db->db_freeme)
	    continue;
	ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
	if (ex_lsa->la_hdr.lsh_adv_rtr == pdb->ospf_rtr_id) {
	    ex_lsa->la_hdr.lsh_seq = NEXTSEQ(ex_lsa->la_hdr.lsh_seq);
	    /*
	     * There is no change in LSA refresh.
	     */
	    db->db_lsa_changed = FALSE;
	    ospf_dec_lsa_table(pdb, area, type, ex_lsa->la_hdr.lsh_chksum);
	    lsa_checksum(db);
	    ospf_inc_lsa_table(pdb, area, type, ex_lsa->la_hdr.lsh_chksum);
	    if (HOLD_TIMER_CHECK(db->db_time, MINLSINTERVAL)) {
		/* delay the flood */
		db->db_flood_dly = TRUE;
		if (type == LSA_TYPE_ASE)
		    lock_timer = &ospf_pdb->ospf_exlock_timer;
		else 
		    lock_timer = &area->ar_exlock_timer;
		if (!mgd_timer_running(lock_timer))
		    mgd_timer_start(lock_timer, MINLSINTERVAL*ONESEC);
		continue;
	    }
	    db->db_flood_dly = FALSE;
	    GET_TIMESTAMP(db->db_time);
	    /*
	     * Don't delay the flooding of type 7 LSA.
	     * With the upcoming OSPF packet pacing, no explictly
	     * delay is neccessary, so we do not write additional
	     * code to handle type 7 delay flooding here. Given
	     * the common configuration of NSSA area, there should
	     * not be a lot of type 7 external at any ASBR, so
	     * it should be OK not doing delay flooding right now.
	     */
	    if (type == LSA_TYPE_ASE)
		ospf_insert_exdelayq(pdb, db);
	    else
		add_to_flood_list(db, &flood_list);
	    if ((type == LSA_TYPE_7_ASE) && !count)
		flood_area(area, &flood_list, NULL, NULL);
	}
    }
    if (type == LSA_TYPE_7_ASE)
	flood_area(area, &flood_list, NULL, NULL);
}

/*
 * pdb_exupdate:
 * refreshes the external (type 5 and type 7) links generated by this
 * router.
 */
void pdb_exupdate (pdbtype *pdb)
{
    areatype *ar;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_event(OC_GENERIC, (ulong) "pdb_exupdate", 0);
    ospf_flood_buginf("\nOSPF: Updating External Links");
    pdb_exupdate_sub(pdb->ospf_pdb->ospf_dummy_area, LSA_TYPE_ASE);

    for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
	if (ar->ar_nssa) {
	    ospf_flood_buginf("\nOSPF: Updating NSSA External Links "
			      "for area %s", ar->ar_id_str);	    
	    pdb_exupdate_sub(ar, LSA_TYPE_7_ASE);
	}
    }
    mgd_timer_start(&ospf_pdb->ospf_exupdate_timer, LS_REFRESH_TIME*ONESEC);
} /* end of pdb_exupdate */


/*
 * age_net_lsa_flood
 */
void age_net_lsa_flood (idbtype *idb)
{
    list_queue flood_queue;
    lsdbtype	*db;
    areatype	*area;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_GENERIC, (ulong) "age_net_lsa_flood", (ulong) idb);
    area = ospf_info->area_ptr;
    ospf_flood_adj_buginf("\nOSPF: Flush network LSA on %s for area %i",
			  idb->namestring, ROUTER_ID(area));
    flood_queue.q_first = 0;
    flood_queue.q_last = 0;
    db = db_find_lsa(area, idb->ip_address, ROUTER_ID(area), LSA_TYPE_NET);
    if (db == NULL)
	return;
    db->db_lsa_ptr->lsh_seq = NEXTSEQ(db->db_lsa_ptr->lsh_seq);
    db->db_lsa_ptr->lsh_age = 0;
    if (!db->db_freeme) {
	ospf_dec_lsa_table(area->ar_pdb_ptr, area, db->db_lsa_ptr->lsh_type,
			    db->db_lsa_ptr->lsh_chksum);
    }
    /* calculate new checksum */
    compute_iso_checksum((uchar *)db->db_lsa_ptr,
			 db->db_lsa_ptr->lsh_length,
			 (uchar *)&(db->db_lsa_ptr->lsh_chksum));
    /* age it */
    db->db_lsa_ptr->lsh_age = MAXAGE;
    /* free it when flooding is done */
    GET_TIMESTAMP(db->db_time);
    db->db_freeme = TRUE;
    ospf_insert_maxage(area->ar_pdb_ptr, db);
    add_to_flood_list(db, &flood_queue);
    flood_area(area, &flood_queue, NULL, NULL);
    if (db->db_rt_table) {
	ospf_adj_buginf("\nOSPF: Schedule SPF to remove network route");
	spf(area, SPF_SCHD_NET, LSA_ID(db));
    }
} /* end of age_net_lsa_flood */


/*
 * lsdb_enqueue
 */
void lsdb_enqueue (list_queue *queue, lsdb_list *list)
{
    if (queue->q_first == NULL) {
	queue->q_first = list;
	list->list_prev = NULL;
   } else {
	queue->q_last->list_next = list;
	list->list_prev = queue->q_last;
    }
    list->list_next = NULL;
    queue->q_last = list;
    queue->count++;
} /* end of lsdb_enqueue */


/*
 * lsdb_dequeue
 */
void lsdb_dequeue (list_queue *queue, lsdb_list *list)
{
    if (queue->q_first == list) {
	queue->q_first = list->list_next;
	if (list->list_next)
	    list->list_next->list_prev = NULL;
	else
	    queue->q_last = NULL;
    } else {
	if (queue->q_last == list) {
	    list->list_prev->list_next = NULL;
	    queue->q_last = list->list_prev;
	} else { /* we are in the middle */
	    list->list_prev->list_next = list->list_next;
	    list->list_next->list_prev = list->list_prev;
	}
    }
    list->list_prev = NULL;
    list->list_next = NULL;    
    queue->count--;
    ospf_lsdb_list_free(list);
} /* end of lsdb_dequeue */


/*
 * add_ack_list:
 */
void add_ack_list (list_queue *ack_queue_ptr, lsdbtype *db, lsatype *lsa)
{
    lsdb_list	*list;
    lsatype	*temp;

    temp = NULL;
    list = ospf_lsdb_list_malloc();
    if (list) {
	list->list_db_ptr = db;
	if (db == NULL) {
	    temp = malloc(sizeof(lsatype));
	    if (temp == NULL) {
		ospf_lsdb_list_free(list);
		return;
	    }
	    bcopy((uchar *)lsa, (uchar *)temp, LINK_STATE_HEADER_SZ);
	} else {
	    db->db_ackcnt++;
	}
	list->list_lsa_ptr = temp;
	
	lsdb_enqueue(ack_queue_ptr, list);
    }
} /* end of add_ack_list */
    

/*
 * ospf_free_ack_list
 *
 * Clean up an ack list.
 */
void ospf_free_ack_list (list_queue *ack_queue)
{
    lsdb_list *list, *next_list;

    list = ack_queue->q_first;

    while (list) {
	next_list = list->list_next;
	list->list_next = NULL;
	if (list->list_db_ptr)
	    list->list_db_ptr->db_ackcnt--;
	else
	    free(list->list_lsa_ptr);
	ospf_lsdb_list_free(list);
	list = next_list;
    }
    ack_queue->q_first = ack_queue->q_last = NULL;
    ack_queue->count = 0;
}

/*
 * rem_db_retrans_list
 */
void rem_db_retrans_list (
    lsdbtype	*db,
    nbrtype	*nbr)
{
    nbr_list  *list;
    nbr_list  *temp;

    SAVE_CALLER();

    if (db == NULL)
	return;
    if (nbr == NULL) {
	/* delete the list */
	for (list = db->db_retrans; list; list = list->list_next){
	    if (list->list_nbr) {
		ospf_retrans_buginf("\ncalling re_nbr to delete nbr %#x %i, "
				    "db %#x %i %d, caller %#x",
				    list->list_nbr, list->list_nbr->nbr_id, db,
				    LSA_ID(db), LSA_TYPE(db), caller());
		rem_nbr_retrans_list(list->list_nbr, db);
	    }
	}
	/* now delete the list */
	for (list = db->db_retrans; list; list = temp) {
	    temp = list->list_next;
	    ospf_nbr_list_free(list);
	}
	db->db_retrans = NULL;
    } else {
	/* delete this neighbor */
	for (list = (nbr_list *)&(db->db_retrans);list && (list->list_next);) {

	    if (list->list_next->list_nbr == nbr) {
		temp = list->list_next;
		list->list_next = temp->list_next;
		ospf_nbr_list_free(temp);
	    } else
		list = list->list_next;
	}
    }
}/* rem_db_rentrans_list */


/*
 * add_to_update_list
 * Add db to list. It allow setting of the list_flood flag.
 * Used by add_to_flood_list() which set list_flood flag to FALSE.
 * It is only explicity used by ospf_rcv_update() in the case when old 
 * lsa is received, which require list_flood flag to be set TRUE.
 */
inline void add_to_update_list (
    lsdbtype *db,
    list_queue *flood_queue,
    boolean flood)
{
    lsdb_list *list;

    if ((list = ospf_lsdb_list_malloc()) == NULL) {
	return;
    }
    list->list_flood = flood;
    list->list_db_ptr = db;
    list->list_next = NULL;
    list->list_prev = NULL;
    /* put lsa on queue to be flooded */
    lsdb_enqueue(flood_queue, list);
} /* end of add_to_update_list */
	      

/*
 * add_to_flood_list
 * Add db to list to be flooded by calling routine 
 * It makes sure the retransmission list is empty.
 */
void add_to_flood_list (
    lsdbtype *db,
    list_queue *flood_queue)
{
    /* if there is a retrans list  free it */
    if (db->db_retrans)
	rem_db_retrans_list(db, NULL);
    add_to_update_list(db, flood_queue, FALSE);
} /* end of add_to_flood_list */


/*
 * check_if_area_address
 * check if the address falls into the network range of an area.
 *
 * compare_area is FALSE if we want to check all areas
 */
static boolean check_if_area_address (areatype *area, ipaddrtype address,
			       boolean compare_area)
{
    pdbtype *pdb;
    ipaddrtype range;
    ipaddrtype mask;
    int i;

    if (!area)
	return(FALSE);

    pdb = area->ar_pdb_ptr;
    if (pdb) {
	for (i = 0; i < pdb->netcount; i++) {
	    if (!(compare_area) || (pdb->network_info[i] == area->ar_id)) {
		range = pdb->networks[i];
		mask = pdb->network_mask[i];
		if ((address & mask) == (range & mask))
		    return(TRUE);
	    }
	}
    }
    return(FALSE);
} /* end of check_if_area_address */

/*
 * build_rtr_lsa
 * Events that call this routine
 * 	1) update time ( we have to flood)
 * 	2) nbr change  ( flood only if there is a change in lsa )
 * 	3) received self orig with earlier seq number ( we have to flood )
 *
 * returns TRUE if there is a need to flood
 * returns FALSE if the LSA has to delayed or there is no need to flood.
 */
boolean build_rtr_lsa (
    areatype *area,		/* build RTR LSA for this area */
    list_queue *flood_queue,	/* insert LSA in this flood list if needed */
    boolean   update_flag)	/* time to update LSA even without changes */
{
    boolean run_spf, old_link_added;
    pdbtype *pdb;
    rtr_lsatype *rtr_lsa;
    link_info *link;
    idbtype *idb;
    ushort length;
    boolean foundlsa;	/* true if lsa was found when adding */
    lsdbtype *db;
    boolean add_to_lsa;
    ulong rtr_id;
    ipaddtype *ptr;
    int i = 0;
    int s = 0;
    ushort total_link_count;
    nbrtype *nbr;
    int nbr_no;
    lsdbtype *dr_net;
    ospf_idb_info *ospf_info;
    ospf_pdb_info *ospf_pdb;

    length = 0;
    pdb = area->ar_pdb_ptr;
    ospf_pdb = pdb->ospf_pdb;
    ospf_event(OC_GENERIC, (ulong) "build_rtr_lsa", area->ar_id);
    ospf_flood_adj_buginf("\nOSPF: Build router LSA for area %s, router ID %i",
			  area->ar_id_str, pdb->ospf_rtr_id);

    if (pdb->ospf_flag & OSPF_ID_CHANGING)
	return(FALSE);

    /* 
     * First check if we are in MinLSInterval window.
     * If we are, set a flag saying that we tried to
     * build the RTR LSA and return.
     */
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_FREEZE)
	return(FALSE);
    if (mgd_timer_running(&area->ar_delay_timer)) {
	ospf_buginf("\nOSPF: Tried to build Router LSA within MinLSInterval");
	if (update_flag)
	    area->ar_delay_flag |= DELAY_RTR_LSA_UPDATE;
	else
	    area->ar_delay_flag |= DELAY_RTR_LSA;
	return(FALSE);
    }
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_ACTIVE) {
	mgd_timer_start(&area->ar_delay_timer, MINLSINTERVAL*ONESEC);
	if (update_flag)
	    area->ar_delay_flag |= DELAY_RTR_LSA_UPDATE;
	else
	    area->ar_delay_flag |= DELAY_RTR_LSA;
	return(FALSE);
    }
    for (idb = area->ar_if_list; idb ; idb = idb->ospf_info->next) {
	ospf_info = idb->ospf_info;
	if (ospf_info->if_state == IFS_POINT_TO_M_POINT) {
	    nbr_no = 0;
	    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
		if (nbr->nbr_state == NBR_FULL)
		    nbr_no++;
	    }
	    /*
	     * Each neighbor takes one link.  An extra link is for
	     * own address at cost zero.
	     */
	    length += ((nbr_no + 1) * sizeof(link_info));
	} else
	    i++;
	    
	for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; 
	     ptr = ptr->next) {
	    if (check_if_area_address(area, ptr->address, TRUE))
		s++;
	}
    }
    length += ((((i * 3) + s) * sizeof(link_info)) + sizeof(rtr_lsatype)); 
 
    if (length > MAXDATAGRAMSIZE) {	
	buginf("\nOSPF: Too many secondary addresses"); 
	return(FALSE);
    }
    /* Allocate enought for all links */
    if ((rtr_lsa = malloc(length)) == NULL) {
	return(FALSE);
    }

    rtr_lsa->la_hdr.lsh_type = LSA_TYPE_RTR;
    rtr_lsa->la_hdr.lsh_id = area->ar_pdb_ptr->ospf_rtr_id;
    rtr_lsa->la_hdr.lsh_adv_rtr = area->ar_pdb_ptr->ospf_rtr_id;
    rtr_lsa->la_hdr.lsh_options = area->ar_options | OPTION_DC_BIT;
    rtr_lsa->la_hdr.lsh_age = 0;

	
    rtr_lsa->la_E_B = 0;
    if (area->ar_pdb_ptr->ospf_area_brtr)
	rtr_lsa->la_E_B = AREA_BORDER_B_BIT;
    if (area->ar_pdb_ptr->ospf_as_brtr)
	rtr_lsa->la_E_B |= AS_BORDER_E_BIT;
    rtr_lsa->la_zero = 0;

    rtr_lsa->la_lnk_cnt = 0;
    length = RTR_LSA_HDR_SIZE;

    link = &rtr_lsa->la_link;

    for (idb = area->ar_if_list; idb ; idb = idb->ospf_info->next) {
        ospf_info = idb->ospf_info;
	if (ospf_info->if_state == IFS_DOWN)
	    continue;
	switch (ospf_info->if_state) {
	  case IFS_POINT_TO_POINT:
 	    if ((nbr = ospf_info->oi_nbr)) {
 		if (nbr->nbr_state == NBR_FULL) {
		    rtr_lsa->la_lnk_cnt++;
		    if (ospf_info->if_type == IF_VIRTUAL_LINK)
			link->lnk_con_type = LINK_IF_VIRTUAL_LINK;
		    else
			link->lnk_con_type = LINK_IF_RTR;
		    length += LINK_INFO_SIZE;
		    link->lnk_id = nbr->nbr_id;
		    if (idb->ip_unnumbered == NULL)
			link->lnk_data = idb->ip_address;
		    else
			link->lnk_data = idb->if_number;
		    link->lnk_tos_count = 0;
		    link->lnk_tos0_metric = ospf_info->if_cost[0];
		    link++;
  		}
  	    }
	    if ((ospf_info->if_type != IF_VIRTUAL_LINK) &&
		(idb->ip_address)) {
		rtr_lsa->la_lnk_cnt++;
		length += LINK_INFO_SIZE;
		link->lnk_con_type = LINK_IF_STUB_NET;
		link->lnk_id = (idb->ip_address & idb->ip_nets_mask);
		link->lnk_tos_count = 0;
		link->lnk_data = idb->ip_nets_mask;
		link->lnk_tos0_metric = ospf_info->if_cost[0];
		link++;
		for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; 
		     ptr = ptr->next){
		    if (check_if_area_address(area, ptr->address, TRUE)) {
			rtr_lsa->la_lnk_cnt++;
			length += LINK_INFO_SIZE;
			link->lnk_con_type = LINK_IF_STUB_NET;
			link->lnk_id = (ptr->address) & (ptr->mask);      
			link->lnk_data = ptr->mask;
			link->lnk_tos_count = 0;
			link->lnk_tos0_metric = ospf_info->if_cost[0];
			link++;
		    }
		}
	    }
	    break;
	  case IFS_LOOPBACK:
	    /* 
	     * declare this as a host route if 
	     * there is interface address 
	     */
	    if (idb->ip_address) {
		rtr_lsa->la_lnk_cnt++;
		length += LINK_INFO_SIZE;
		link->lnk_con_type = LINK_IF_STUB_NET;
		link->lnk_id = idb->ip_address;
		link->lnk_tos_count = 0;
		link->lnk_data = HOST_NET_MASK;
		link->lnk_tos0_metric = ospf_info->if_cost[0];
		link++;
	    }
	    break;

	  case IFS_DR:
	  case IFS_BACKUP:
	  case IFS_OTHER:
	    add_to_lsa = 0;
	    /*
	     * The following code is added to handle a weird condition
	     * for SMDS network. The SMDS network can partition into 
	     * independent network and it causes DR election when the 
	     * partition heal. 
	     * The DR election causes route loss as router and network
	     * LSA generated during the transition do not reflect a
	     * connected topology.
	     * We solve the problem using the following method -
	     * When DR election happen and DR switch
	     * 1. Every router keep its old DR, we don't keep it if this 
	     *    old DR died.
	     * 2. The old DR will not flush its network LSA until 3 min.
	     * 3. When originate its router LSA, the router will add a 
	     *    TRANS NETWORK entry to the old DR if and only if
	     *    - We keep an old DR
	     *    - We have at least 2WAY with the old DR
	     *    - We have the copy of the network LSA generated by the 
	     *      old DR
	     *    - The old DR network LSA have a link pointing to us
	     * 4. After the old DR flush its network LSA, all routers will 
	     *    remove the entry to the old DR and everything back 
	     *    to normal
	     *
	     * See ospf_choose_dr() for related code
	     */
	    old_link_added = FALSE;
	    if (ospf_info->oi_old_dr && 
		(ospf_info->oi_old_dr->nbr_state >= NBR_2WAY)) {
		dr_net = db_find_lsa(area,
				     ospf_info->oi_old_dr->nbr_ipaddr,
				     ospf_info->oi_old_dr->nbr_id,
				     LSA_TYPE_NET);
		if (dr_net && (net_backlink(ROUTER_ID(area), dr_net))) {
		    ospf_flood_adj_buginf("\nOSPF: Include link to "
					  "old DR on %s", idb->namestring);
		    old_link_added = TRUE;
		    rtr_lsa->la_lnk_cnt++;
		    link->lnk_con_type = LINK_IF_TRANS_NET;
		    length += LINK_INFO_SIZE;
		    link->lnk_id = ospf_info->oi_old_dr->nbr_ipaddr;
		    link->lnk_data = idb->ip_address;
		    link->lnk_tos_count = 0;
		    link->lnk_tos0_metric = ospf_info->if_cost[0];
		    link++;
		    for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; 
			 ptr = ptr->next){
			if (check_if_area_address(area, ptr->address, TRUE)) {
			    rtr_lsa->la_lnk_cnt++;
			    length += LINK_INFO_SIZE;
			    link->lnk_con_type = LINK_IF_STUB_NET;
			    link->lnk_id = (ptr->address) & (ptr->mask);      
			    link->lnk_data = ptr->mask;
			    link->lnk_tos_count = 0;
			    link->lnk_tos0_metric = ospf_info->if_cost[0];
			    link++;
			}
		    }
		    /*
		     * Rebuild the router LSA when the old network
		     * LSA is flushed in order to remove this additional
		     * entry. If the timer is already set, don't set it
		     * again to avoid unnecessary delay. 
		     */
		    if (!mgd_timer_running(&area->ar_rebuild_rtr_timer)) {
			mgd_timer_start(&area->ar_rebuild_rtr_timer, 
					REBUILD_RTR_INTERVAL);
		    }
		}
	    }
	    if (ospf_info->oi_old_dr && !old_link_added) {
		/*
		 * If we haven't added link to old DR, we don't
		 * need to remember the old DR anymore.
		 */
		ospf_flood_adj_buginf("\nOSPF: Reset old DR on %s",
				      idb->namestring);
		ospf_info->oi_old_dr = NULL;
	    }
	    if (ospf_info->if_state == IFS_DR) {
		/*
		 * if we are adjacent to atleast one router
		 * add this to lsa.
		 */
		if (ospf_info->nbr_fcnt)
		    add_to_lsa = TRUE;
	    } else {
		/*
		 * check to see if we are adjacent to DR
		 * and add if we are.
		 */
		if ((ospf_info->dr) &&
		    (ospf_info->dr->nbr_state == NBR_FULL))
		    add_to_lsa = TRUE;
	    }
		
	    if (add_to_lsa) {
		rtr_lsa->la_lnk_cnt++;
		link->lnk_con_type = LINK_IF_TRANS_NET;
		length += LINK_INFO_SIZE;
		link->lnk_id = ospf_info->dr->nbr_ipaddr;
		link->lnk_data = idb->ip_address;
		link->lnk_tos_count = 0;
		link->lnk_tos0_metric = ospf_info->if_cost[0];
		link++;
		for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; 
		     ptr = ptr->next){
		    if (check_if_area_address(area, ptr->address, TRUE)) {
			rtr_lsa->la_lnk_cnt++;
			length += LINK_INFO_SIZE;
			link->lnk_con_type = LINK_IF_STUB_NET;
			link->lnk_id = (ptr->address) & (ptr->mask);      
			link->lnk_data = ptr->mask;
			link->lnk_tos_count = 0;
			link->lnk_tos0_metric = ospf_info->if_cost[0];
			link++;
		    }
		}
		break;
	    }
	    /* else fall through */
	  case IFS_WAITING: /* still determining DR */
	    rtr_lsa->la_lnk_cnt++;
	    length += LINK_INFO_SIZE;
	    link->lnk_con_type = LINK_IF_STUB_NET;
	    link->lnk_id = (idb->ip_address) & (idb->ip_nets_mask);      
	    link->lnk_data = idb->ip_nets_mask;
	    link->lnk_tos_count = 0;
	    link->lnk_tos0_metric = ospf_info->if_cost[0];
	    link++;
	    for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; 
		 ptr = ptr->next){
		if (check_if_area_address(area, ptr->address, TRUE)) {
		    rtr_lsa->la_lnk_cnt++;
		    length += LINK_INFO_SIZE;
		    link->lnk_con_type = LINK_IF_STUB_NET;
		    link->lnk_id = (ptr->address) & (ptr->mask);      
		    link->lnk_data = ptr->mask;
		    link->lnk_tos_count = 0;
		    link->lnk_tos0_metric = ospf_info->if_cost[0];
		    link++;
		}
	    }
	    break;
	case IFS_POINT_TO_M_POINT:
	    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
		if (nbr->nbr_state == NBR_FULL) {
		    rtr_lsa->la_lnk_cnt++;
		    if (ospf_info->if_type == IF_VIRTUAL_LINK)
			link->lnk_con_type = LINK_IF_VIRTUAL_LINK;
		    else
			link->lnk_con_type = LINK_IF_RTR;
		    length += LINK_INFO_SIZE;
		    link->lnk_id = nbr->nbr_id;
		    if (idb->ip_unnumbered == NULL)
			link->lnk_data = idb->ip_address;
		    else
			link->lnk_data = idb->if_number;
		    link->lnk_tos_count = 0;
		    link->lnk_tos0_metric = ospf_info->if_cost[0];
		    link++;
 		}
	    }
	    /*
	     * Advertise our own interface address, including the
	     * secondary addresses, at cost 0.
	     */
	    if (idb->ip_address) {
		rtr_lsa->la_lnk_cnt++;
		length += LINK_INFO_SIZE;
		link->lnk_con_type = LINK_IF_STUB_NET;
		link->lnk_id = idb->ip_address;
		link->lnk_data = HOST_NET_MASK;
		link->lnk_tos_count = 0;
		link->lnk_tos0_metric = 0;
		link++;
		for (ptr = (ipaddtype *)idb->ip_addressQ.qhead; ptr; 
		     ptr = ptr->next){
		    if (check_if_area_address(area, ptr->address, TRUE)) {
			rtr_lsa->la_lnk_cnt++;
			length += LINK_INFO_SIZE;
			link->lnk_con_type = LINK_IF_STUB_NET;
			link->lnk_id = ptr->address;
			link->lnk_data = HOST_NET_MASK;
			link->lnk_tos_count = 0;
			link->lnk_tos0_metric = 0;
			link++;
		    }
		}
	    }
	    break;
	} /* end of switch */
    } /* end of for */

    /* add host links here */
    
    rtr_lsa->la_hdr.lsh_length = length;
    rtr_id = area->ar_pdb_ptr->ospf_rtr_id;
    run_spf = FALSE; 
    total_link_count = rtr_lsa->la_lnk_cnt;

    /* if found or not, replace with new one */
    foundlsa = db_add_lsa(&db, 
			  area,
			  rtr_id,
			  rtr_id,
			  (lsatype *)rtr_lsa,
			  LSA_TYPE_RTR);
    if (db == NULL) {
	errmsg(&msgsym(NOMEMORY, OSPF), "link state database");
	return(FALSE);
    }
    if (foundlsa) {
	/* compare to see if it's the same, if not rerun spf */
	if ((rtr_lsa->la_hdr.lsh_length != db->db_lsa_ptr->lsh_length) 
	    || (rtr_lsa->la_lnk_cnt != 
		((rtr_lsatype *)db->db_lsa_ptr)->la_lnk_cnt)
	    || (rtr_lsa->la_E_B != 
		((rtr_lsatype *)db->db_lsa_ptr)->la_E_B)
	    || bcmp(&(rtr_lsa->la_link),
		 &(((rtr_lsatype *)db->db_lsa_ptr)->la_link),
		 (rtr_lsa->la_lnk_cnt * LINK_INFO_SIZE)))
	    run_spf = TRUE;
	if (run_spf || update_flag) {
	    /*
	     * remove old stats
	     */
	    if (!db->db_freeme) {
	        ospf_dec_lsa_table(pdb, area, db->db_lsa_ptr->lsh_type,
				    db->db_lsa_ptr->lsh_chksum);
	    }
	    /* flood only if necessary */
	    rtr_lsa->la_hdr.lsh_seq = NEXTSEQ(db->db_lsa_ptr->lsh_seq);
	    db->db_freeme = FALSE;
	    free(db->db_lsa_ptr);
	    db->db_lsa_ptr = (lsatype *)rtr_lsa;
	    lsa_checksum(db);
	} else
	    /*
	     * No change, keep db->db_lsa_ptr and free rtr_lsa
	     */
	    free(rtr_lsa);
    } else {
	rtr_lsa->la_hdr.lsh_seq = OSPF_FIRSTSEQ;
	lsa_checksum(db);
	run_spf = TRUE;
    }
    if (run_spf) {
	db->db_lsa_changed = TRUE;
    } else {
	if (update_flag)
	    db->db_lsa_changed = FALSE;
    }
    if (run_spf || update_flag) {
        /*
         * add in the new stats
         */
        if (!db->db_freeme) {
            ospf_inc_lsa_table(pdb, area, db->db_lsa_ptr->lsh_type,
                                db->db_lsa_ptr->lsh_chksum);
        }
	/* lock out new instantiation for MinLSInterval if there is
	 * info in the LSA or the router ID is not zero, i.e we were
	 * able to allocate the router ID (there is some operational 
	 * interface left).
	 */
	if ((total_link_count > 0) || (pdb->ospf_rtr_id != 0)) {
	    mgd_timer_start(&area->ar_delay_timer, MINLSINTERVAL*ONESEC);
	    area->ar_delay_flag = 0;
	    ospf_flood_buginf("\nOSPF: add router LSA seq %x to flood"
			      " queue", db->db_lsa_ptr->lsh_seq);
	    add_to_flood_list(db, flood_queue);
	}
	GET_TIMESTAMP(db->db_time);
    }
    if (run_spf) {
	PDB_INC_MIB_TABLE(pdb, ORNLSA_COUNT);
	spf(area, SPF_SCHD_RTR, 0);
    }
    return(run_spf);
} /* end of build_rtr_lsa  */


/*
 * build_net_lsa
 * build net link state advertisement - this rtr is DR
 */
boolean build_net_lsa (
    idbtype *idb,
    list_queue *flood_queue,
    boolean update_flag)
{
    ushort length;
    pdbtype *pdb;
    net_lsatype *net_lsa;
    nbrtype *nbr;
    boolean foundlsa;
    lsdbtype *db;
    areatype *area;
    ulong    *nbr_id_ptr;
    ulong    rtr_id;
    boolean  run_spf;
    ospf_idb_info *ospf_info = idb->ospf_info;

    ospf_event(OC_GENERIC, (ulong) "build_net_lsa", (ulong) idb);
    area = ospf_info->area_ptr;
    pdb = area->ar_pdb_ptr;
    rtr_id = pdb->ospf_rtr_id;
    ospf_flood_adj_buginf("\nOSPF: Build network LSA for %s, router ID %i",
			  idb->namestring, rtr_id);
    /* 
     * First check if we are in MinLSInterval window.
     * If we are, set a flag saying that we tried to
     * build the NET LSA and return.
     */
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_FREEZE)
	return(FALSE);
    if (mgd_timer_running(&ospf_info->oi_delay_timer)) {
	ospf_flood_buginf("\nOSPF: Net Lsa within MinLSInterval");
	if (update_flag)
	    ospf_info->delay_flag = DELAY_NET_LSA_UPDATE;
	else
	    ospf_info->delay_flag = DELAY_NET_LSA;
	return(FALSE);
    }

    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_ACTIVE) {
	mgd_timer_start(&area->ar_delay_timer, MINLSINTERVAL*ONESEC);
	if (update_flag)
	    ospf_info->delay_flag = DELAY_NET_LSA_UPDATE;
	else
	    ospf_info->delay_flag = DELAY_NET_LSA;
	return(FALSE);
    }

    /* check all nbrs to see if we are adjacent to any */
    if (ospf_info->nbr_fcnt ==  0) {
	ospf_flood_adj_buginf("\nOSPF: No full nbrs to build Net Lsa");
	age_net_lsa_flood (idb);
	return(FALSE);
    }
    /* Allocate enough for all links */
    if ((net_lsa = malloc(sizeof(net_lsatype) +
			  (ospf_info->nbr_fcnt * 4))) == NULL) { 
	return(FALSE);
    }

    length = NET_LSA_HDR_SIZE;
    net_lsa->la_hdr.lsh_type = LSA_TYPE_NET;
    net_lsa->la_hdr.lsh_id = idb->ip_address;
    net_lsa->la_hdr.lsh_adv_rtr = rtr_id;
    net_lsa->la_hdr.lsh_options = area->ar_options | OPTION_DC_BIT;
    net_lsa->la_hdr.lsh_age = 0;

    net_lsa->la_mask = idb->ip_nets_mask;
    
    nbr_id_ptr = &(net_lsa->la_nbr);

    /* First set our own ID */
    *nbr_id_ptr = rtr_id;
    nbr_id_ptr++;
    length += NBR_ID_SZ;
    for (nbr = ospf_info->oi_nbr; nbr; nbr = nbr->nbr_next) {
	if (nbr->nbr_state == NBR_FULL) {
	    *nbr_id_ptr = nbr->nbr_id;
	    length += NBR_ID_SZ;
	    nbr_id_ptr++;
	}
    }
    net_lsa->la_hdr.lsh_length  = length;
    run_spf = FALSE;

    /* if found or not, replace with new one */
    foundlsa = db_add_lsa(&db, 
			  area,
			  idb->ip_address,
			  rtr_id,
			  (lsatype *)net_lsa,
			  LSA_TYPE_NET);
    if (db == NULL) {
	errmsg(&msgsym(NOMEMORY, OSPF), "link state database");
	return(FALSE);
    }
    if (foundlsa) {
	/* compare to see if it's the same, if not rerun spf */
	if ((net_lsa->la_hdr.lsh_options != db->db_lsa_ptr->lsh_options) ||
	    (net_lsa->la_hdr.lsh_length != db->db_lsa_ptr->lsh_length) ||
	    bcmp((uchar*)&(net_lsa->la_mask),
		 (uchar *)&(((net_lsatype *)db->db_lsa_ptr)->la_mask),
		 (length - LINK_STATE_HEADER_SZ))
	    || (ADV_AGE(db) >= MAXAGE))
	    run_spf = TRUE;
	if (run_spf || update_flag) {
            /*
             * remove old stats
             */
            if (!db->db_freeme) {
                ospf_dec_lsa_table(pdb, area, db->db_lsa_ptr->lsh_type,
                                    db->db_lsa_ptr->lsh_chksum);
            }
	    /* flood only if necessary */
	    net_lsa->la_hdr.lsh_seq = NEXTSEQ(db->db_lsa_ptr->lsh_seq);
	    db->db_freeme = FALSE;
	    free(db->db_lsa_ptr);
	    db->db_lsa_ptr = (lsatype *)net_lsa;
	    lsa_checksum(db);
	}
    } else {
	net_lsa->la_hdr.lsh_seq = OSPF_FIRSTSEQ;
	run_spf = TRUE;
	lsa_checksum(db);
    }
    if (run_spf) {
	db->db_lsa_changed = TRUE;
    } else {
	if (update_flag)
	    db->db_lsa_changed = FALSE;
    }
    if (run_spf || update_flag) {
        /*
         * add in the new stats
         */
        if (!db->db_freeme) {
            ospf_inc_lsa_table(pdb, area, db->db_lsa_ptr->lsh_type,
                                db->db_lsa_ptr->lsh_chksum);
        }
	/* lock out new instansiation for MinLSInterval */
	mgd_timer_start(&ospf_info->oi_delay_timer, MINLSINTERVAL*ONESEC);
	ospf_info->delay_flag = 0;
	GET_TIMESTAMP(db->db_time);
	/* put lsa on queue to be flooded */
	add_to_flood_list(db, flood_queue);
    }
    if (db->db_lsa_ptr != (lsatype *)net_lsa)
	free(net_lsa);
    if (run_spf) {
	PDB_INC_MIB_TABLE(pdb, ORNLSA_COUNT);
	spf(area, SPF_SCHD_NET, 0);
    }
    return(run_spf);
} /* end build_net_lsa */


/*
 * build_sum_lsa
 * Build network and ASBR summary link-state advertisements
 * Called by spf - just build the ones that have changed
 * We used to use metric equal SUM_LS_INFINITY to redraw LSA. However,
 * SUM_LS_INFINITY becomes a valid metric for indication LSA, so we
 * need another mechanism to specify whether we want to advertise or
 * redraw indication LSA. The solution is to overload the parameter type.
 * Use OSPF_ADV_INDICATION and OSPF_REMOVE_INDICATION for that purpose.
 */
void build_sum_lsa (
    lsdbtype 	**db_ptr,
    areatype 	*area,
    ipaddrtype 	dest,
    ipaddrtype 	mask,
    ulong	metric,
    uchar	type,
    ushort	sum_origin,		    
    list_queue	*flood_list)
{
    boolean	found;	  /* true if lsa was found when adding */
    lsdbtype	*db;
    pdbtype     *pdb;
    sum_lsatype	*sum_lsa;
    sys_timestamp time;
    uchar 	options;
    boolean 	db_exist;
    boolean	sum_origin_intra, indication, adv_indication;

    /*
     * Origin is either INTRA or INTER
     */
    sum_origin_intra = (sum_origin == RTYPE_INTRA);

    pdb = area->ar_pdb_ptr;

    if (type < LSA_TYPE_SUM_NET) {
	/*
	 * Special type for indication LSA
	 */
	indication = TRUE;
	if (type == OSPF_ADV_INDICATION)
	    adv_indication = TRUE;
	else
	    adv_indication = FALSE;
	type = LSA_TYPE_SUM_ASB;
    } else {
	indication = FALSE;
	adv_indication = FALSE;
    }
    found = FALSE;
    options = area->ar_options; 
    if (!indication)
	options |= OPTION_DC_BIT;
    if (*db_ptr)
	found = TRUE;
    /* if found check if there is any change in parametes */
    if (found) {
	sum_lsa = (sum_lsatype *)(*db_ptr)->db_lsa_ptr;
	if (ADV_AGE(*db_ptr) < MAXAGE) {
	    if (sum_lsa->la_mask == mask) {
		/*
		 * When generating summary from inter-area route, if we 
		 * found a summary LSA generated from intra-area route and
		 * it is not MAXAGE, don't send any other summary LSA, just 
		 * return.
		 */
		if (!sum_origin_intra && (*db_ptr)->db_sum_origin_intra)
		    return;
		if ((sum_lsa->la_tos_metric == metric) &&
		    (sum_lsa->la_hdr.lsh_options == options) &&
		    ((*db_ptr)->db_sum_origin_intra == sum_origin_intra) &&
		    (!indication || adv_indication)) {
		    /* there is no change. Just return */
		    ospf_spf_inter_acl_buginf("\nOSPF: No change for sum from "
					      "%s-area route %i, mask %i, "
					      "type %d, age %d, metric %lu, "
					      "seq %#x to area %s",
					      sum_origin_intra ? 
					      "intra" : "inter",
					      sum_lsa->la_hdr.lsh_id, 
					      sum_lsa->la_mask, type,
					      ADV_AGE(*db_ptr),
					      metric,
					      sum_lsa->la_hdr.lsh_seq, 
					      area->ar_id_str);

		    return;
		}
	    }
	}
	COPY_TIMESTAMP((*db_ptr)->db_time, time);
	/* 
	 * remove the old statistics - we'll reissue the lsa and update them. 
	 */
        if (!((*db_ptr)->db_freeme)) {
              ospf_dec_lsa_table(pdb, area, (*db_ptr)->db_lsa_ptr->lsh_type,
                                  (*db_ptr)->db_lsa_ptr->lsh_chksum);
        }
    } else {
	if ((metric == SUM_LS_INFINITY) && (!indication || !adv_indication))
	    return;
	db_exist = db_add_lsa(db_ptr, area, dest, ROUTER_ID(area), NULL, type);
	if (*db_ptr == NULL) {
	    errmsg(&msgsym(NOMEMORY, OSPF), "Summary database entry");
	    return;
	}
	if (db_exist) {
	    /*
	     * We cannot find the entry before but find it now.
	     * Something go wrong. When we replace the lsa entry,
	     * we end up having memory leak.
	     */
	    errmsg(&msgsym(DBEXIST, OSPF), area->ar_id_str, dest, 
		   mask, ROUTER_ID(area), type);
	}
	if ((sum_lsa = malloc(sizeof(sum_lsatype))) == NULL) {
	    return;
	}
	(*db_ptr)->db_lsa_ptr = (lsatype *)sum_lsa;
    }

    ospf_event(OC_GENERIC, (ulong) "build_sum_lsa", area->ar_id);

    db = *db_ptr;
    sum_lsa->la_mask = mask;
    sum_lsa->la_tos_metric = metric;
    if (indication && found && !db->db_freeme) {
	/*
	 * Decrease the count first
	 */
	area->ar_dcbitless_lsa_count--;
	area->ar_indication_lsa_count--;
    }
    if ((metric == SUM_LS_INFINITY) && (!indication || !adv_indication)) {
	/* has become unreachable */
	sum_lsa->la_hdr.lsh_age = MAXAGE;
	db->db_freeme = TRUE;
    } else {
	sum_lsa->la_hdr.lsh_age = 0;
	db->db_freeme = FALSE;
    }
    ospf_insert_maxage(pdb, db);
    if (found) {
	sum_lsa->la_hdr.lsh_seq = NEXTSEQ(sum_lsa->la_hdr.lsh_seq);
    } else {
	sum_lsa->la_hdr.lsh_seq = OSPF_FIRSTSEQ;
    }
    sum_lsa->la_hdr.lsh_type = type;
    sum_lsa->la_hdr.lsh_options = area->ar_options;
    if (!indication)
	sum_lsa->la_hdr.lsh_options |= OPTION_DC_BIT;
    else {
	if (!db->db_freeme) {
	    /*
	     * We now have DCbitless LSA, flush DoNotAge LSA
	     */
	    area->ar_dcbitless_lsa_count++;
	    area->ar_indication_lsa_count++;
	    pdb->ospf_flag |= OSPF_FLUSH_DO_NOT_AGE;
	}
    }
    sum_lsa->la_hdr.lsh_id = dest;
    sum_lsa->la_hdr.lsh_adv_rtr = ROUTER_ID(area);
    sum_lsa->la_hdr.lsh_chksum = 0;
    sum_lsa->la_hdr.lsh_length = SUM_LSA_HDR_SIZE + 4;
    COPY_TIMESTAMP(db->db_time, time);
    GET_TIMESTAMP(db->db_time);
    db->db_sum_origin_intra = sum_origin_intra;
    lsa_checksum(db);
    /*
     * update the stats with the new info
     */
    if (db->db_freeme == FALSE) {
          ospf_inc_lsa_table(pdb, area, db->db_lsa_ptr->lsh_type,
                              db->db_lsa_ptr->lsh_chksum);
    }
    PDB_INC_MIB_TABLE(pdb, ORNLSA_COUNT);
    ospf_spf_inter_acl_buginf("\nOSPF: Generate sum from %s-area "
			      "route %i, mask %i, type %d, age %d, "
			      "metric %lu, seq %#x to area %s",
			      sum_origin_intra ? "intra" : "inter",
			      sum_lsa->la_hdr.lsh_id, sum_lsa->la_mask, type,
			      ADV_AGE(*db_ptr), metric,
			      sum_lsa->la_hdr.lsh_seq, area->ar_id_str);
    db->db_lsa_changed = TRUE;
    /*
     * Self-originated type 4 LSA is always reachable
     */
    if (type == LSA_TYPE_SUM_ASB)
	db->db_rtr_reachable = TRUE;
    if ((found) && (HOLD_TIMER_CHECK(time, MINLSINTERVAL))) {
	/* delay the flood */
	db->db_flood_dly = TRUE;
	if (!mgd_timer_running(&area->ar_sumlock_timer))
	    mgd_timer_start(&area->ar_sumlock_timer, MINLSINTERVAL*ONESEC);
	return;
    }
    add_to_flood_list(db, flood_list);
} /* end of build_sum_lsa */


/*
 * flush_area_ranges
 * It takes an area and flushes out its ranges
 * from rest of the areas.
 */
void flush_area_ranges (
    pdbtype *pdb,
    areatype *area)
{
    areatype *ar;
    list_queue flood_list;
    lsdbtype *db;
    addr_range *range;
    
    if (pdb->running == FALSE)
	return;
    if (pdb->ospf_area_brtr) {
	for (ar = pdb->ospf_area_list; ar ; ar = ar->ar_next) {
	    if (area == ar || ar->ar_stub_nosum)
		continue;
	    flood_list.q_first = NULL;
	    flood_list.q_last  = NULL;
	    for (range = area->ar_range_list;range ; range = range->net_next) {
		db = ospf_db_find_lsa(ar,
				      range->net_address,
				      range->net_mask,
				      ROUTER_ID(ar),
				      LSA_TYPE_SUM_NET);
		if (db == NULL)
		    continue;
		ospf_send_summary_lsa(&db,
				      ar,
				      LSA_ID(db),
				      range->net_mask,
				      SUM_LS_INFINITY,
				      LSA_TYPE_SUM_NET,
				      RTYPE_INTRA,
				      &flood_list);
	    }
	    flood_area(ar, &flood_list, NULL, NULL);
	}
    }
} /* end of flush_area_ranges */


/*
 * ospf_check_ex_origin
 * If there is another router advertising external LSA with the
 * same information as we do, check if we should withdraw ours.
 */
static boolean ospf_check_ex_origin (pdbtype *pdb, lsdbtype *db, 
				     lsdbtype *our_db,
				     ulong our_mask, ulong our_faddr, 
				     ulong our_metric, list_queue *list)
{
    ex_lsatype *ex_lsa;
    lsatype *our_lsa;
    rtr_route *route;
    ospf_rtr_path *path;
    ndbtype *ndb;

    ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
    if ((our_faddr == 0) || (ex_lsa->la_metric.as_forw_addr == 0))
       return(TRUE);

    if ((ex_lsa->la_mask == our_mask) &&
	(ex_lsa->la_metric.as_forw_addr == our_faddr) &&
	(ex_lsa->la_metric.as_e_tos_metric ==  our_metric) &&
	(route = lookup_rtr_route(pdb, NULL, ex_lsa->la_hdr.lsh_adv_rtr,
				  NULL,0,0, &path))) {
 	if (ex_lsa->la_hdr.lsh_adv_rtr < pdb->ospf_rtr_id) {
	    /* we win */
	    return(TRUE);
	} else {
	    /* we loose  we have to flush */
	    if (our_db) {
		our_lsa = our_db->db_lsa_ptr;
                if (!our_db->db_freeme) {
                    ospf_dec_lsa_table(pdb, pdb->ospf_area_list,
                                        our_lsa->lsh_type,
                                        our_lsa->lsh_chksum);
                }
		our_lsa->lsh_age = MAXAGE;
		our_db->db_freeme = TRUE;
		if (our_db->db_retrans != NULL) 
		    rem_db_retrans_list(our_db, NULL);
		our_lsa->lsh_seq = NEXTSEQ(our_lsa->lsh_seq);
		/* add new chksum */
		GET_TIMESTAMP(our_db->db_time);
		lsa_checksum(our_db);
		ospf_insert_maxage(pdb, our_db);
		add_to_flood_list(our_db, list);
		ospf_spf_ext_buginf("\nOSPF: Flush %i age %d  adv %i\n",
				    our_lsa->lsh_id, our_lsa->lsh_age,
				    pdb->ospf_rtr_id);

		/*
		 * reset ndb->advertisingmask since we no longer advertise
		 * this ndb
		 */
		ndb = ip_rtlookup(LSA_ID(our_db) & our_mask, our_mask);
		if (ndb)
		    ndb->advertisingmask &= ~pdb->mask;
	    }
	    db->db_won_redist = TRUE;
	    if (path)
		path->orp_flag |= REDIST_LOST;
	    return(FALSE);
	}
    }
    return(TRUE);
} /* end of ospf_check_ex_origin */


/*
 * ospf_get_type7_forward_addr
 * Figure out a forwarding address for type 7 LSA
 */
static ulong ospf_get_type7_forward_addr (pdbtype *pdb, areatype *area,
					  idbtype *hint_idb, idbtype **idb_ptr)
{
    /*
     * Return an OSPF interface address for type-7 LSA
     */
    if (hint_idb && ospf_interface_up(hint_idb)) {
	*idb_ptr = hint_idb;
	return (hint_idb->ip_address);
    }
    /*
     * Try to get an interface from the area in which we do the
     * redistribution first. If fail, pick any OSPF interface
     */
    for (hint_idb = area->ar_if_list; hint_idb; 
	 hint_idb = hint_idb->ospf_info->next) {
	if (ospf_interface_up(hint_idb)) {
	    *idb_ptr = hint_idb;
	    if (hint_idb->ip_unnumbered)
		return (hint_idb->ip_unnumbered->ip_address);
	    else
	        return (hint_idb->ip_address);
	}
    }
    return(0);
}

/*
 * ospf_get_forward_addr
 */
static ulong ospf_get_forward_addr (pdbtype *pdb, areatype *area, ndbtype *sdb,
				    idbtype **idb_ptr, uchar type)
{
    rdbtype	*rdb;
    idbtype	*idb, *temp_idb;
    int		i, idb_pdb_index;
    ospf_idb_info *ospf_info;

    temp_idb = NULL;
    *idb_ptr = NULL;
    if (sdb == NULL) {
	return(0);
    }
    for (i = 0; i < sdb->routecount; i++) {
      	rdb = sdb->rdb[i];
	/*
	 * If next hop address is not on one of our interfaces, never set
	 * forwarding address.  Route which requires non-connected
	 * route to go to its next hop will fail.
	 * This is the right thing because if we would use the immediate 
	 * next hop after recursive lookup, that might change without us 
	 * knowing.  For example, if we have a static route to X with next 
	 * hop pointing to Y, and another static route to network of Y with
	 * next hop to Z.   Later, the third route is added to Y with next 
	 * hop to ZZ and the second one is deleted.   The immediate next
	 * hop has changed but there is no change to route X and we never 
	 * get notified.
	 */
	idb = samecable(rdb->gateway);

	if (idb == NULL)
	   continue;
	   
	/*
	 * use this next hop as forwarding address only when
	 *    ospf is enabled on this interface
	 *    ospf is not passive on this interface
	 *    interface is not a p2p or p2mp
	 *    same ospf process is running on this interface
	 *    and it falls into OSPF network range.  (Need this check
	 *      because OSPF might be running on the primary address only,
	 *      but the gateway is on the secondary address of the same
	 *      interface)
	 */
	ospf_info = idb->ospf_info;
	if (ospf_info && ospf_info->enabled &&
	    ((idb->ip_passive_router & pdb->mask) == 0) && 
	    (ospf_info->if_type != IF_POINT_TO_POINT) &&
	    (ospf_info->if_type != IF_POINT_TO_M_POINT)) {
	    idb_pdb_index = ospf_info->area_ptr->ar_pdb_ptr->index;
	    if ((idb_pdb_index == pdb->index) &&
		(check_if_area_address(pdb->ospf_area_list, 
				       rdb->gateway, FALSE))) {
	       *idb_ptr = idb;
	       return(rdb->gateway);
	    }
	}
	if (ospf_info)
	    temp_idb = idb;
    }
    if (type == LSA_TYPE_7_ASE)
	return (ospf_get_type7_forward_addr(pdb, area, temp_idb, idb_ptr));
    return(0);
} /* end of ospf_get_forward_addr */
	    

/*
 * start_redist_net
 * insert info about this
 * network for redistribution
 * latter.
 */

static void start_redist_net (
    pdbtype *pdb,
    ipaddrtype address,
    ipaddrtype mask)
{
    ospfbackuptype *backup;

    if ((backup = malloc(sizeof(ospfbackuptype))) == NULL)
	return;
    backup->netnumber = address;
    backup->context_info = mask;
    backup->route_type = 0;
    if (pdb->add_requests)
	backup->next = pdb->add_requests;
    pdb->add_requests = backup;

} /* end of start_redist_net */

/*
 * redistribute_check
 * It return FALSE if the route should be ignored; TRUE if the route
 * is eligible for redistribution.
 */
boolean redistribute_check (
    pdbtype *pdb,   /* OSPF pdb */
    ndbtype *sdb,
    pdbtype *dist,  /* redistributed protocol */
    int	    *event)
{
    boolean pass_filter;

    if (!dist ||
	((pdb->distflag & dist->mask) == 0) ||
	(sdb->attrflags & NET_NOREDIST) ||
	((sdb->attrflags & NET_SUMMARY) && 
	 (INDEX2MASK(sdb->pdbindex) & pdb->mask)) ||
	(pdb->index == sdb->pdbindex) ||
	((NDB2PROCTYPE(sdb) == PROC_CONNECTED) 
	 && !ip_redist_flag(sdb, pdb)) ||
 	((NDB2PROCTYPE(sdb) != PROC_CONNECTED)
	 && (sdb->pdbindex != dist->index)) ||
	((sdb->attrflags & NET_SUBNET)
	 && (pdb->metric[dist->index].subnets == FALSE)))
	return(FALSE);

    /*
     * Check route type if redistributed from another OSPF process
     */
    if (dist->proctype == PROC_OSPF) {
	if (!ospf_check_match_routetype(pdb, dist, sdb))
	    return (FALSE);
    }

    /*
     * pass dist->index to pdb->outfilter, instead of sdb->pdbindex
     * because in case of connected route with RIP (or IGRP) turned
     * on, sdb->pdbindex will list CONNECTED pdb which is zero and 
     * that cause pdb_outfilter to NOT check anything.   And dist
     * is the redistributed protocol so it should be checked.
     */
    pass_filter = pdb_outfilter(pdb, NULL, dist, sdb->netnumber);
    pass_filter = pass_filter && pdb_routemap(pdb, dist, sdb);
    if (!pass_filter || !sdb->routecount)
	*event = ROUTE_DOWN;
	    
    return(TRUE);
} /* end of redistribute_check */

/* 
 * build_ex_lsa:
 */
void build_ex_lsa (
    lsdbtype 	**db_ptr,
    pdbtype 	*pdb,
    areatype	*area,
    ipaddrtype 	dest,
    ipaddrtype 	mask,
    ulong	metric,
    ipaddrtype	forw_addr,
    ulong	tag,
    list_queue	*flood_list,
    ushort	metric_type,
    boolean	delay_flag,
    boolean	replace_flag,
    uchar	type)
{
    boolean	found;	  /* true if lsa was found when adding */
    lsdbtype	*db;
    ex_lsatype	*ex_lsa;
    sys_timestamp time;
    ulong       temp;
    mgd_timer	*lock_timer;

    TIMER_STOP(time);
    if (metric_type == METRIC_TYPE2)
	temp = (metric | ASE_TYPE2_METRIC_BIT);
    else
	temp  = metric;
	
    if (*db_ptr)
	found = TRUE;
    else
	found = FALSE;

    /* if found check if there is any change in parametes */
    if (found) {
	ex_lsa = (ex_lsatype *)(*db_ptr)->db_lsa_ptr;
	if ((ex_lsa->la_mask == mask) 
	    && (ex_lsa->la_metric.as_forw_addr == forw_addr)
	    && (ex_lsa->la_metric.as_ex_tag == tag)
	    && (ex_lsa->la_metric.as_e_tos_metric == temp)
	    && (ADV_AGE(*db_ptr) < MAXAGE))
	    /* there is no change. Just return */
	    return;
	COPY_TIMESTAMP((*db_ptr)->db_time, time);
	if ((*db_ptr)->db_freeme == FALSE) {
            /*
             * pull out the old checksum stats - we'll reissue the lsa
             */
	    ospf_dec_lsa_table(pdb, area, type, ex_lsa->la_hdr.lsh_chksum);
        }
    } else {
	/*
	 * When a LSA is re-issued, because another LSA takes over its old
	 * LS ID, with host bit set in lsid, generate it without checking 
	 * metric infinity.
	 */
	if ((replace_flag == FALSE) && (metric == SUM_LS_INFINITY))
	    return;
	db_add_lsa(db_ptr, area, dest, pdb->ospf_rtr_id, NULL, type);

	if (*db_ptr == NULL) {
	    errmsg(&msgsym(NOMEMORY, OSPF), "External Database entry");
	    return;
	}
	if ((ex_lsa = malloc(sizeof(ex_lsatype))) == NULL) {
	    return;
	}
	(*db_ptr)->db_lsa_ptr = (lsatype *)ex_lsa;
	/*
	 * Newly created LSA must have age and db_time set correctly here
	 * because it can be checked if metric is infinity and both are used.
	 */
	ex_lsa->la_hdr.lsh_age = 0;
	GET_TIMESTAMP((*db_ptr)->db_time);
    }
    ospf_event(OC_GENERIC, (ulong) "build_ex_lsa", dest);

    db = *db_ptr;
    ex_lsa->la_mask = mask;
    if (metric == SUM_LS_INFINITY) {
	/*
	 * set age to MAXAGE and freeme to TRUE always
	 */
	ex_lsa->la_hdr.lsh_age = MAXAGE;
	db->db_freeme = TRUE;
 	metric_type = EXLSA_TO_METRIC_TYPE(ex_lsa);
    } else {
	ex_lsa->la_hdr.lsh_age = 0;
	db->db_freeme = FALSE;
    }
    ospf_insert_maxage(pdb, db);
    ex_lsa->la_metric.as_forw_addr = forw_addr;
    ex_lsa->la_metric.as_ex_tag = tag;
    if (metric_type == METRIC_TYPE2)
	ex_lsa->la_metric.as_e_tos_metric = (metric | ASE_TYPE2_METRIC_BIT);
    else
	ex_lsa->la_metric.as_e_tos_metric = metric;
    if (found) {
	ex_lsa->la_hdr.lsh_seq = NEXTSEQ(ex_lsa->la_hdr.lsh_seq);
    } else {
	ex_lsa->la_hdr.lsh_seq = OSPF_FIRSTSEQ;
    }
    ex_lsa->la_hdr.lsh_type = type;
    ex_lsa->la_hdr.lsh_options = pdb->ospf_tos_option | OPTION_DC_BIT;
    if ((type == LSA_TYPE_7_ASE) && !pdb->ospf_pdb->ospf_normal_area_count) {
	/*
	 * For type-7 LSA, we set the P-bit if we are not
	 * connected to any normal area.
	 */
	ex_lsa->la_hdr.lsh_options |= OPTION_N_P_BIT;
    }
    ex_lsa->la_hdr.lsh_id = dest;
    ex_lsa->la_hdr.lsh_adv_rtr = pdb->ospf_rtr_id;
    ex_lsa->la_hdr.lsh_chksum = 0;
    ex_lsa->la_hdr.lsh_length = ASE_LSA_HDR_SIZE + AS_METRIC_SZ;
    GET_TIMESTAMP(db->db_time);
    lsa_checksum(db);
    PDB_INC_MIB_TABLE(pdb, ORNLSA_COUNT);
    if (!db->db_freeme) {
	ospf_inc_lsa_table(pdb, area, type, db->db_lsa_ptr->lsh_chksum);
    }
    /*
     * If the status of default change, schedule a partial SPF
     */
    if (dest == 0) {
	ospf_spf_ext_acl_pass = acl_ospf_spf_ext_debug(dest);	
	ospf_schedule_partial_spf(area, db);
	ospf_spf_ext_acl_pass = FALSE;
    }

    db->db_lsa_changed = TRUE;
    if (acl_debug_ospf_lsa_gen(dest, mask)) {
	buginf("\nOSPF: Generate external LSA %i, mask %i, "
	       "type %d, age %d, metric %lu, seq %#x%s%s",
	       dest, mask, type, ADV_AGE(*db_ptr), metric,
	       ex_lsa->la_hdr.lsh_seq,
	       (type == LSA_TYPE_7_ASE) ? " to area " : "",
	       (type == LSA_TYPE_7_ASE) ? area->ar_id_str : "");
    }
    if ((found && (HOLD_TIMER_CHECK(time, MINLSINTERVAL))) 
	|| (flood_list == NULL)) {
	/* delay the flood */
	db->db_flood_dly = TRUE;
	if (type == LSA_TYPE_ASE)
	    lock_timer = &pdb->ospf_pdb->ospf_exlock_timer;
	else
	    lock_timer = &area->ar_exlock_timer;
	if (!mgd_timer_running(lock_timer))
	    mgd_timer_start(lock_timer, MINLSINTERVAL*ONESEC);
	return;
    }
    if (delay_flag && (type == LSA_TYPE_ASE)) {
 	db->db_flood_dly = TRUE;
 	ospf_insert_exdelayq(pdb, db);
    } else
 	add_to_flood_list(db, flood_list);
    return;
} /* end of build_ex_lsa */

/* 
 * ospf_send_external_lsa_sub:
 * A wrapper routine for build_ex_lsa() to figure out what LS ID we
 * should use in external LSA.   This is to support redistribution of
 * supernet route into OSPF.   Details of the rationale and algorithm is
 * in Appendix F of OSPF RFC.
 */
void ospf_send_external_lsa_sub (
    lsdbtype 	**db_ptr,
    pdbtype 	*pdb,
    areatype	*area,
    ipaddrtype 	dest,
    ipaddrtype 	mask,
    ulong	metric,
    ipaddrtype	forw_addr,
    ulong	tag,
    list_queue	*flood_list,
    ushort	metric_type,
    boolean	delay_flag,
    uchar       type)
{
    lsdbtype 	*rdb, *rdb_super;
    ipaddrtype	dest_with_host_bit;
    ipaddrtype	old_mask;
    ulong	old_metric;
    ipaddrtype	old_forw_addr;
    ulong	old_tag;
    ushort	old_metric_type;
    ex_lsatype	*ex_lsa;
    boolean 	normal_found;

    normal_found = FALSE;

    /* First, look for the LSA with normal LSA ID */
    rdb = db_find_lsa(area, dest, pdb->ospf_rtr_id, type);
    /*
     * If found a db, check if the mask is the same.
     * Should be the case for all normal case (no suppernetting or subnet zero)
     * If we find it but the mask doesn't match, that means we have 
     * advertised other LSA with the same LS ID already.   Look further
     * for LS ID with mask bit.
     */
    if (rdb) {
	normal_found = TRUE;
	ex_lsa = (ex_lsatype *)(rdb->db_lsa_ptr);
	if (ex_lsa->la_mask == mask) {
	    build_ex_lsa(&rdb, pdb, area, dest, mask, metric, forw_addr, tag,
			 flood_list, metric_type, delay_flag, FALSE, type);
	    *db_ptr = rdb;
	    return;
	} 
    } 

    /* 
     * Try looking up LSA with host bit.   Even though we might not find
     * the LSA with normal LS ID, it might have been purged out after we 
     * have generated the LSA with the funky mask already.   So we have 
     * to look for that LSA before generating a LSA with normal LS ID.
     * This means we will stuck with the funky mask until we restart 
     * redistribution process. (no redist ... , then redist again).
     */
    dest_with_host_bit = (dest | ~mask);
    rdb_super = db_find_lsa(area, dest_with_host_bit, pdb->ospf_rtr_id, type);
    if (rdb_super) {
	ex_lsa = (ex_lsatype *)(rdb_super->db_lsa_ptr);
	if (ex_lsa->la_mask == mask) {
	    build_ex_lsa(&rdb_super, pdb, area, dest_with_host_bit, mask,
			 metric, forw_addr, tag, flood_list, 
			 metric_type, delay_flag, FALSE, type);
	    *db_ptr = rdb_super;
	} else {
	    /*
	     * This is the case when we have supernet LSA with host bit
	     * bitwise-or in, which makes the LSA ID looks like normal
	     * LSA ID for normal major network.   For example, 
	     * 193.1.128.0/255.255.254.0 got LS ID. 193.1.129.255.
	     * Then when we want to generate LSA for 193.1.129.0/24
	     * For the reason above, we look for funky mask LSA and
	     * find one but the mask is not the same.   This is ok
	     * if we haven't found the normal one.   It is error otherwise.
	     */
	    if (normal_found == FALSE) {
		build_ex_lsa(&rdb, pdb, area, dest, mask, metric,
			     forw_addr, tag, flood_list, metric_type, 
			     delay_flag, FALSE, type);
		*db_ptr = rdb;
	    } else {
		errmsg(&msgsym(CONFLICTING_LSAID, OSPF),
		       ex_lsa->la_hdr.lsh_id, ex_lsa->la_mask, 
		       dest, mask);
	    }
	}
    } else {
	if (normal_found) {
	    /*
	     * There is no LSA with the host bit set yet but there is
	     * one with normal LSA ID,  we will send one of these with
	     * host bit set.   The one with shorter mask will use
	     * normal LSA ID, while the other one will use LSA ID with
	     * inverse mask bits bitwise-or in.
	     */
	    ex_lsa = (ex_lsatype *)(rdb->db_lsa_ptr);
	    if (mask > ex_lsa->la_mask) { 
		/* 
		 * Old mask is shorter, it keeps the LSA ID.
		 * Generate the new one with LSA ID with host bit set
		 */
		build_ex_lsa(&rdb_super, pdb, area, dest_with_host_bit, mask,
			     metric, forw_addr, tag, flood_list, 
			     metric_type, delay_flag, FALSE, type);
		*db_ptr = rdb_super;
	    } else {
		/* 
		 * New mask is shorter, steal the existing db by
		 * using rdb but give it a new mask.
		 * Then re-generates the old one with new LSA ID with
		 * host bit, with the old mask.
		 */
		old_mask = ex_lsa->la_mask;
		old_forw_addr = ex_lsa->la_metric.as_forw_addr;
		old_tag = ex_lsa->la_metric.as_ex_tag;
		old_metric = ASE_METRIC(ex_lsa->la_metric.as_e_tos_metric);
		old_metric_type = EXLSA_TO_METRIC_TYPE(ex_lsa);

		build_ex_lsa(&rdb, pdb, area, dest, mask, metric,
			     forw_addr, tag, flood_list, metric_type, 
			     delay_flag, FALSE, type);

		build_ex_lsa(&rdb_super, pdb, area, (dest | ~old_mask), 
			     old_mask, old_metric, old_forw_addr, old_tag, 
			     flood_list, old_metric_type, delay_flag, 
			     TRUE, type);

		/* 
		 * copy the value from the old one to the re-issued one
		 * rdb_super can be NULL when memory is low.
		 */
		if (rdb_super) {
		    rdb_super->db_pdbindex = rdb->db_pdbindex;
		    rdb_super->db_pdbdist = rdb->db_pdbdist;
		}
		*db_ptr = rdb;
	    }
	} else {
	    /* there is none yet, just generate one */
	    build_ex_lsa(&rdb, pdb, area, dest, mask, metric, forw_addr, tag,
			 flood_list, metric_type, delay_flag, FALSE, type);
	    *db_ptr = rdb;
	}
    }
} /* end of ospf_send_external_lsa_sub */


/*
 * ospf_send_external_lsa
 * Function to build both type 5 and type 7 LSA
 */
static void ospf_send_external_lsa (lsdbtype **db_ptr, pdbtype *pdb,
			     ipaddrtype dest, ipaddrtype mask,
			     ulong metric, ipaddrtype forw_addr,
			     ulong tag, list_queue *flood_list,
			     ushort metric_type, boolean delay_flag)
{
    areatype *ar;
    ulong temp_metric;
    idbtype *dummy_idb;
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;

    if (ospf_pdb->ospf_normal_area_count) { 
	ospf_send_external_lsa_sub(db_ptr, pdb, pdb->ospf_area_list, 
				   dest, mask, metric,
				   forw_addr, tag, flood_list,
				   metric_type, delay_flag,
				   LSA_TYPE_ASE);
    }
    if (ospf_pdb->ospf_nssa_area_count) {
	for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
	    if (ar->ar_nssa) {
		if (!forw_addr) {
		    /*
		     * Type 7 summary should have forward address set
		     * too.
		     */
		    forw_addr = ospf_get_type7_forward_addr(pdb, ar, NULL,
							    &dummy_idb);
		}
		temp_metric = ar->ar_nssa_no_redist ? 
		    SUM_LS_INFINITY : metric; 
		ospf_send_external_lsa_sub(db_ptr, pdb, ar, dest, mask, 
					   temp_metric, forw_addr, tag, 
					   &ar->ar_nssa_flood_list,
					   metric_type, delay_flag,
					   LSA_TYPE_7_ASE);
	    }
	}
    }
}    

/*
 * build_ex_route_sub
 */
static void build_ex_route_sub (
    pdbtype 	*pdb,
    areatype	*area,
    ndbtype 	*ndb,
    list_queue	*flood_list,
    ushort	event,
    pdbtype	*s_pdb,
    boolean 	delay_flag,
    uchar       type)
{
    ulong	metric, temp_metric;
    lsdbtype	*db;
    lsdbtype	*other_db;
    ipaddrtype	forw_addr;
    idbtype	*idb;
    ushort	metric_type;
    ulong	tag;

    if (s_pdb == NULL)
	s_pdb = INDEX2PDB(ndb->pdbindex);
    metric_type = METRIC_TYPE2;
    metric = DEFAULT_MAP_METRIC;
    if (pdb->metric[s_pdb->index].type == 1)
	metric_type = METRIC_TYPE1;
    forw_addr = ospf_get_forward_addr(pdb, area, ndb, &idb, type);
    
    metric = ospf_get_external_metric(pdb, s_pdb, ndb, event);
    /*
     * Set the tag.
     */
#ifdef NOTDEF
This has been disabled until the IETF can figure out what to do.
    if (s_pdb->proctype & PROC_BGP)
	tag = (ndb->tag & TAG_ASPART) | TAG_BGP;
    else if (s_pdb->proctype & PROC_EGP)
	tag = (ndb->tag & TAG_ASPART) | TAG_EGP;
    else if (s_pdb->proctype & (PROC_CONNECTED|PROC_STATIC))
	tag = pdb->asystem;
    else if (s_pdb->proctype & (PROC_IGRP|PROC_OSPF))
	tag = s_pdb->asystem | TAG_INCOMPLETE;
    else
	tag = pdb->asystem | TAG_INCOMPLETE;
    if (pdb->metric[ndb->pdbindex].tag !=
	pdb->metric_def[ndb->pdbindex].tag)
	tag = pdb->metric[ndb->pdbindex].tag | TAG_INCOMPLETE;
#else
    /*
     * Always inherit the tag from the route by default
     */
    tag = ndb->tag;
    if (pdb->metric[s_pdb->index].tag_set)
	tag = pdb->metric[s_pdb->index].tag;
#endif

    /*
     * If there is route-map for this ndb, get set values.
     */

    if (metric != SUM_LS_INFINITY) {
    	if (pdb->route_map)
	    ospf_get_route_map_set_value(pdb->route_map, &metric, 
					 &metric_type, &tag);
    }

    db = ospf_db_find_lsa(area, ndb->netnumber, ndb->netsmask, 
			  pdb->ospf_rtr_id, type);
    other_db = find_ex_db(area, ndb->netnumber, pdb->ospf_rtr_id,
			  type);
    if (!other_db) {
	/* 
	 * LSA might have host bit set, try look up for that.
	 */
	other_db = find_ex_db(area, ndb->netnumber | ~ndb->netsmask, 
			      pdb->ospf_rtr_id, type);
    }

    /*
     * if we already have a LSA generated because of other protocol with
     * lower distance and it is not MAXAGE, don't generate a new one.
     */
    if ((db)
	&& (db->db_pdbdist < s_pdb->distance)
	&& (ADV_AGE(db) < MAXAGE)) {
	return;
    }
    temp_metric = ((metric_type == METRIC_TYPE2) ? 
	(metric | ASE_TYPE2_METRIC_BIT) : metric);

    if (other_db) {
	if (!ospf_check_ex_origin(pdb, other_db, db, ndb->netsmask, 
				  forw_addr, temp_metric, flood_list)) {
	    errmsg(&msgsym(NOTREDIST3, OSPF), ndb->netnumber, ndb->netsmask,
		   ADV_RTR(other_db));
	    return;
	}
    }
    ospf_send_external_lsa_sub(&db, pdb, area, ndb->netnumber, ndb->netsmask,
			       metric, forw_addr, tag, flood_list, metric_type,
			       delay_flag, type);
    if (db) {
	db->db_pdbindex = s_pdb->index;
	db->db_pdbdist = s_pdb->distance;
    }
    if (!db || (event == ROUTE_DOWN))
	ndb->advertisingmask &= ~pdb->mask;
    else
	ndb->advertisingmask |= pdb->mask;
} /* end of build_ex_route_sub */


/*
 * build_ex_route
 */
static void build_ex_route (pdbtype *pdb, ndbtype *ndb, list_queue *ex_flood_list,
		     ushort event, pdbtype *s_pdb, boolean delay_flag)
{
    areatype *ar;
    ushort temp_event;
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;

    if (ospf_pdb->ospf_normal_area_count || (event == ROUTE_DOWN)) {
	build_ex_route_sub(pdb, ospf_pdb->ospf_dummy_area, ndb, ex_flood_list, 
			   event, s_pdb, delay_flag, LSA_TYPE_ASE);
    }
    if (ospf_pdb->ospf_nssa_area_count || (event == ROUTE_DOWN)) {
	for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
	    if (ar->ar_nssa) {
		temp_event = ar->ar_nssa_no_redist ? ROUTE_DOWN : event;
		build_ex_route_sub(pdb, ar, ndb, &ar->ar_nssa_flood_list, 
				   temp_event, s_pdb, FALSE, LSA_TYPE_7_ASE);
	    }
	}
    }
}    

/*
 * build_ex_default_route
 */
static void build_ex_default_route_sub (
    pdbtype 	*pdb,
    areatype    *area,
    ndbtype 	*ndb,
    list_queue	*flood_list,
    ushort	event,
    boolean 	delay_flag,
    uchar	type)
{
    ulong	metric;
    lsdbtype	*db;
    ipaddrtype	forw_addr;
    idbtype	*idb;
    ushort      metric_type;
    ulong       tag;

    idb = NULL;

    if ((ndb == NULL) && (event == ROUTE_UP) 
 	&&(!(pdb->default_originate && pdb->default_always)) && 
	!pdb->default_routemap)
 	return;

    if ((ndb) && (ndb->pdbindex == pdb->index)) {
	if (pdb->default_always)
	    ndb = NULL;
	else {
	    if (pdb->default_originate == FALSE)
		event = ROUTE_DOWN;
	    else
		return;
	}
    }

    forw_addr = ospf_get_forward_addr(pdb, area, ndb, &idb, type);
    if (pdb->ospf_default_metric) {
	metric = pdb->ospf_default_metric;
    } else { 
	if ((idb) && (idb->ospf_info->enabled))
	    metric = idb->ospf_info->if_cost[0];
	else
	    metric = 1;
    }
       
    /*
     * If the default_always is set, try to
     * generate default always.
     */
    if ((event == ROUTE_DOWN) && pdb->default_always) {
	event = ROUTE_UP;
	forw_addr = 0;
    }

    /*
     * Check route-map to determine if default should be advertised.
     */
    if (pdb_routemap_for_default(pdb)) {
	if (pdb->route_map) {
	    event = ROUTE_UP;
	}
    } else {
	event = ROUTE_DOWN;
    }

    metric_type = (pdb->ospf_default_type == 1) ? METRIC_TYPE1 : METRIC_TYPE2;
    tag = pdb->asystem;

    /*
     * If route-map for this ndb, get set values.
     */
    if (metric != SUM_LS_INFINITY) {
	if (pdb->route_map)
	    ospf_get_route_map_set_value(pdb->route_map, &metric, 
					 &metric_type, &tag);
    }

    db = db_find_lsa(area, 0, pdb->ospf_rtr_id, type);
    build_ex_lsa(&db, pdb, area, 0, 0,
		 ((event == ROUTE_DOWN) ? SUM_LS_INFINITY: metric),
		 forw_addr, tag, flood_list, metric_type, delay_flag,
		 FALSE, type);

} /* end of build_ex_default_route_sub */

static void build_ex_default_route (pdbtype *pdb, ndbtype *ndb, 
			     list_queue *ex_flood_list,
			     ushort event, boolean delay_flag)
{
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;

    if (ospf_pdb->ospf_normal_area_count || (event == ROUTE_DOWN)) 
	build_ex_default_route_sub(pdb, ospf_pdb->ospf_dummy_area, ndb, 
				   ex_flood_list, event, delay_flag,
				   LSA_TYPE_ASE);
}    

/*
 * ex_route_callback
 */
static void ex_route_callback (
    pdbtype	*pdb,
    ndbtype	*ndb,
    int		event,
    int         flag)
{
    list_queue	flood_list;
    pdbtype *src_pdb;

    ospf_event(OC_GENERIC, (ulong) "ex_route_callback", (ulong) ndb);
    if (!pdb->running)
	return;
    if (pdb->ospf_area_list == NULL) {
	return;
    }
    if (pdb->ospf_dbase_lock == DBASE_FREEZE) {
	errmsg(&msgsym(NOTREDIST1, OSPF), ndb->netnumber, ndb->netsmask,
	       "Database Lock");
	return;
    }
    if (ndb && ndb->attrflags & NET_SUBNETTED)
	return;
    if (ndb)
 	ndb->advertisingmask &= ~pdb->mask;
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    if (flag == DEFAULT_ROUTE) {
	if (pdb->default_originate) {
	    if ((ndb == NULL) && (pdb->default_always == FALSE) && 
		!pdb->default_routemap)
		event = ROUTE_DOWN;
	} else
	    event = ROUTE_DOWN;
	build_ex_default_route(pdb, ndb, &flood_list, event, TRUE);
    } else {
	/*
	 * Never redistribute 0.0.0.0, which should be generated
	 * by default-information originate command.
	 */
	if (!ndb || !ndb->netnumber)
	    return;

	src_pdb = ip_get_src_pdb(pdb, ndb, TRUE);

	/*
	 * If the event is UP and we can't find any source protocol, punt.
	 * When the event is DOWN, it can be the case when the protocol 
	 * that OSPF is redistributing disappeared, e.g. user does 
	 * "no router ...".   We have to go on to MAXAGE the LSA we had
	 * originated.
	 */
	if (!src_pdb && (event == ROUTE_UP)) {
	    if (!(ip_redist_flag(ndb, pdb))) {
		/*
		 * Do the above check because we may get callback when 
		 * ndb->pdb_overmask is set (OSPF is running on this 
		 * network), but we are not redistributing connected 
		 * route causing src_pdb to be NULL.   That case is
		 * correct and we don't wanna spit any message.
		 */
		errmsg(&msgsym(NOSRCPDB, OSPF), ndb->netnumber, 
		       ndb->netsmask);
	    }
	    return;
	}
	
	/*
         * if the source of this route is same as this pdb, just return.
	 */
	if (src_pdb == pdb)
	    return;

	if (redistribute_check(pdb, ndb, src_pdb, &event)) {
	    /*
	     * The ndb is eligible for redistribution.
	     * Test with summary to see if we should redistribute this
	     * ndb individually.
	     */
	    if (!ospf_redist_summary_check(pdb, ndb, src_pdb, event, TRUE)) {
		/*
		 * Summary route should be generated.
		 */
		event = ROUTE_DOWN;
	    }
	} else {
	    /*
	     * The ndb is not eligible for redistribution.
	     */
	    event = ROUTE_DOWN;
	}
	build_ex_route(pdb, ndb, &flood_list, event, src_pdb, TRUE);
    }
    ex_flood_area(pdb, &flood_list, NULL, NULL);
} /* end of ex_route_callback */

/* 
 * build_inter_route:
 * db is an inter-area route belonging to the backbone area,
 * but not originated by this router
 * add to every area except backbone area 
 */
static void build_inter_route (
    lsdbtype 	*db,
    areatype 	*src_area,
    areatype	*dst_area,
    list_queue 	*flood_list)
{
    sum_lsatype	*sum_lsa;
    pdbtype 	*pdb;
    boolean	found;
    lsdbtype	*inter_db;
    ndbtype	*ndb;
    boolean 	flush_lsa = FALSE;
    ulong       new_mask;
    ulong	new_metric;
    rtr_route   *routendb;
    ipaddrtype  dest_id;
    ushort	lsa_type;
    ipaddrtype	adv_rtr;
    ospf_rtr_path *path;

    lsa_type = db->db_lsa_ptr->lsh_type;
    
    pdb = src_area->ar_pdb_ptr;
    if ((src_area == dst_area) || (dst_area->ar_stub_nosum) ||
	((dst_area->ar_stub || dst_area->ar_nssa) && 
	 (lsa_type == LSA_TYPE_SUM_ASB))) {
	return;
    }
    ndb = NULL;
    routendb = NULL;
    inter_db = NULL;
    found = FALSE;
    sum_lsa = (sum_lsatype *)db->db_lsa_ptr;
    dest_id = sum_lsa->la_hdr.lsh_id;
    /*
     * If db_mask is non-zero, use it so we have the right mask
     * when it is called from delete_route_old_mask().
     * Otherwise, db_mask is not initialized and we need to grab the one
     * in the LSA.
     */
    new_mask = db->db_mask ? db->db_mask : sum_lsa->la_mask;
    adv_rtr = 0;

    /*
     * We will not generate a LSA unless we have an inter-area route
     * to the destination advertised by the db
     */

    if (lsa_type == LSA_TYPE_SUM_NET) {
	ndb = ospf_lookup_net_route(src_area, dest_id & new_mask, new_mask, 
				    RTYPE_INTER, TRUE);
	if (ndb == NULL) 
	    flush_lsa = TRUE;
	else {
	    if (ndb->routecount)
		adv_rtr = ndb->rdb[0]->infosource;
	    else
		return;
	}
    }

    if (lsa_type == LSA_TYPE_SUM_ASB) {
 	routendb = lookup_rtr_route(dst_area->ar_pdb_ptr, src_area, dest_id,
				    NULL, DTYPE_ASBR, RTYPE_INTER, &path);
	if (routendb == NULL)
	    flush_lsa = TRUE;
	else
	    adv_rtr = path->orp_infosource;    
    }

    if (!flush_lsa) {
	if ((lookup_rtr_route(src_area->ar_pdb_ptr, src_area, adv_rtr, 
			      NULL, DTYPE_ABR, RTYPE_INTRA, NULL)) == NULL)
	    /* can not reach the area border router */
	    flush_lsa = TRUE;
    }
    
    if (flush_lsa || !db->db_abr_flag)
	new_metric = SUM_LS_INFINITY;
    else {
	if (lsa_type == LSA_TYPE_SUM_NET)
	    new_metric = (ndb ? ndb->metric : SUM_LS_INFINITY);
	else 
	    new_metric = (path ? path->orp_cost : SUM_LS_INFINITY);
    }

    if (lsa_type == LSA_TYPE_SUM_ASB) {
	inter_db = db_find_lsa(dst_area, dest_id, ROUTER_ID(dst_area), 
			       lsa_type);
	build_sum_lsa(&inter_db, dst_area, dest_id, 0, new_metric,
		      lsa_type, RTYPE_INTER, flood_list);		      
    } else {
	inter_db = NULL;
	ospf_send_summary_lsa(&inter_db, dst_area, dest_id & new_mask,
			      new_mask, new_metric, lsa_type, RTYPE_INTER, 
			      flood_list);
    }	
}


/*
 * ospf_vidb_up
 */
boolean ospf_vidb_up (
    idbtype *idb,
    boolean *flag)
{
    ulong router_id;
    lsdbtype *db;
    areatype *area;
    rtr_lsatype *rtr_lsa;
    link_info *link;
    idbtype *interface;
    ulong old_dist;
    ulong old_addr;
    ulong distance;
    ulong metric;
    ulong length;
    int i;
    ulong nbr_addr;
    ndbtype *ndb;
    idbtype *last_if;
    ospf_idb_info *ospf_info = idb->ospf_info, *last_if_ospf_info;

    ospf_event(OC_GENERIC, (ulong) "ospf_vidb_up", (ulong) idb);
    router_id = ospf_info->vnbr_id;
    area = ospf_info->vlink_trans;
    db = db_find_lsa(area, router_id, router_id, LSA_TYPE_RTR);
    if (db == NULL)
	return(FALSE);
    rtr_lsa = (rtr_lsatype *)db->db_lsa_ptr;
    if (rtr_lsa == NULL)
	return(FALSE);
    distance = RTR_LS_MAX_COST;
    interface = NULL;
    nbr_addr = 0;
    link = &rtr_lsa->la_link;
    for (i = rtr_lsa->la_lnk_cnt;i; i--) {
	 if ((link->lnk_con_type == LINK_IF_STUB_NET)
	     || (link->lnk_con_type == LINK_IF_VIRTUAL_LINK)) {
	     length = link->lnk_tos_count * LINK_METRIC_SIZE;
	     link++;
	     link = (link_info *)((ulong *)link + length);
	     continue;
	 }
	 if ((ndb = ospf_lookup_net_route(area, link->lnk_data, 0, 0, TRUE)) 
	     == NULL) {
	     length = link->lnk_tos_count * LINK_METRIC_SIZE;
	     link++;
	     link = (link_info *)((ulong *)link + length);
	     continue;
	 }
	 last_if = ndb->lastinterface;
	 last_if_ospf_info = last_if ? last_if->ospf_info : NULL;
	 if ((last_if != NULL) && last_if_ospf_info &&
	     (last_if_ospf_info->enabled == FALSE)) {
	     length = link->lnk_tos_count * LINK_METRIC_SIZE;
	     link++;
	     link = (link_info *)((ulong *)link + length);
	     continue;
	 }
	 if (ndb->attrflags & NET_ATTACHED) {
	     metric = last_if_ospf_info->if_cost[0];
	 } else	
	     metric = ndb->metric;

	 if (distance >= metric) {
	     interface = last_if;
	     distance = metric;
	     nbr_addr = link->lnk_data;
	 }
 	 length = link->lnk_tos_count * LINK_METRIC_SIZE;
	 link++;
	 link = (link_info *)((ulong *)link + length);
     }

    old_dist = 	ospf_info->if_cost[0];
    old_addr = ospf_info->vnbr_ipaddr;

    if ( interface == NULL) {
	/* no route to this router */
	ospf_info->if_cost[0] = ospf_info->def_if_cost[0] = RTR_LS_MAX_COST;
	ospf_info->vnbr_ipaddr = 0;
	idb->output_if = NULL;
	if (old_dist != RTR_LS_MAX_COST)
	    *flag = TRUE;
	else
	    *flag = FALSE;
	idb->hwptr->state = IDBS_DOWN;
	return(FALSE);
    } else {
	ospf_info->if_cost[0] = ospf_info->def_if_cost[0] = distance;
	ospf_info->vnbr_ipaddr = nbr_addr;
	idb->output_if = interface;
	if (old_dist != distance )
	    *flag = TRUE;
	else
	    *flag = FALSE;
	idb->hwptr->state = IDBS_UP;
    }
    return(TRUE);
}/* end of ospf_vidb_up */
  

/* 
 * build_inter_route_all:
 */
void build_inter_route_all (
    lsdbtype 	*db,
    areatype 	*area)
{
    areatype    *ar;
    list_queue  list;
    pdbtype     *pdb;

    pdb = area->ar_pdb_ptr;
    list.q_first = NULL;
    list.q_last  = NULL;
    for (ar = (areatype * )pdb->ospf_area_list; ar; ar = ar->ar_next) {
	build_inter_route(db, area, ar, &list);
	flood_area(ar, &list, NULL, NULL);
    }
}/* end of build_inter_route_all */



/*
 * sum_update:
 */
static void sum_update(areatype *area, list_queue* flood_list)
{
    ushort	type;
    lsdbtype 	*db;
    lsatype	*lsa;
    int		count = 0;

    ospf_event(OC_GENERIC, (ulong) "sum_update", area->ar_id);

    for (type = LSA_TYPE_SUM_NET; type < LSA_TYPE_ASE; type++) {
        for (db = db_get_first_db(area->ar_lsdbase[type]); db;
	     db = db_get_next_db(db)) {
	    check_ospf_suspend_limit(area->ar_pdb_ptr, DBASE_ACTIVE, &count);
	    if ((db->db_lsa_ptr == NULL) 
		|| (ADV_AGE(db) >= MAXAGE)
		|| (db->db_freeme))
		continue;
	    lsa = db->db_lsa_ptr;
	    if (lsa->lsh_adv_rtr == ROUTER_ID(area)) {
		lsa->lsh_seq = NEXTSEQ(lsa->lsh_seq);
		/*
		 * There is no change in LSA refresh.
		 */
		db->db_lsa_changed = FALSE;
		area->ar_lsa_chksumsum -= lsa->lsh_chksum;
		lsa_checksum(db);
		area->ar_lsa_chksumsum += lsa->lsh_chksum;
		GET_TIMESTAMP(db->db_time);
		db->db_flood_dly = FALSE;
		add_to_flood_list(db, flood_list);
	    }
	}
    }
} /* sum_update */


/*
 * ar_delayed_sum_lsa
 */
void ar_delayed_sum_lsa (areatype *area)
{
    list_queue 	flood_list;
    ushort	delay_flag;
    lsdbtype	*db;
    pdbtype	*pdb;
    ushort	type;
    int		count = 0;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "ar_sumlock", area->ar_id);
    pdb = area->ar_pdb_ptr;
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    delay_flag = 0;

    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_FREEZE)
	return;
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_ACTIVE) {
	mgd_timer_start(&area->ar_sumlock_timer, MINLSINTERVAL * ONESEC);
	return;
    }
    INIT_OSPF_SUSPEND(pdb);
    if (area->ar_pdb_ptr->ospf_area_brtr) {
	for (type = LSA_TYPE_SUM_NET; type < LSA_TYPE_ASE; type++) {
	    for (db = db_get_first_db(area->ar_lsdbase[type]); db;
		 db = db_get_next_db(db)) {
		check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
		if (db->db_lsa_ptr == NULL) 
		    continue;

		if (db && db->db_flood_dly) {
		    if (HOLD_TIMER_CHECK(db->db_time, MINLSINTERVAL)) {
			delay_flag |= DELAY_INTRA_NET;
			continue;
		    }
		    db->db_flood_dly = FALSE;
		    GET_TIMESTAMP(db->db_time);
		    add_to_flood_list(db, &flood_list);
		}
		if (!count)
		    flood_area(area, &flood_list, NULL, NULL);
	    }
	    flood_area(area, &flood_list, NULL, NULL);	
	}
	if (delay_flag)
	    mgd_timer_start(&area->ar_sumlock_timer, MINLSINTERVAL*ONESEC);
    }	
}/* ar_delayed_sum_lsa */


/*
 * ar_delayed_ex_lsa
 */
void ar_delayed_ex_lsa (areatype *area)
{
    list_queue 	flood_list;
    boolean	delay_flag;
    lsdbtype	*db;
    pdbtype	*pdb;
    int		count = 0;

    ospf_event(OC_TIMER_EXPIRED, (ulong) "ar_exlock", area->ar_id);
    pdb = area->ar_pdb_ptr;
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    delay_flag = 0;

    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_FREEZE)
	return;
    if (area->ar_pdb_ptr->ospf_dbase_lock == DBASE_ACTIVE) {
	mgd_timer_start(&area->ar_exlock_timer, MINLSINTERVAL * ONESEC);
	return;
    }
    INIT_OSPF_SUSPEND(pdb);
    if (area->ar_nssa) {
	for (db = db_get_first_db(area->ar_lsdbase[LSA_TYPE_7_ASE]); db;
	     db = db_get_next_db(db)) {
	    check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
	    if (db->db_lsa_ptr == NULL) 
		continue;

	    if (db && db->db_flood_dly) {
		if (HOLD_TIMER_CHECK(db->db_time, MINLSINTERVAL)) {
		    delay_flag = TRUE;
		    continue;
		}
		db->db_flood_dly = FALSE;
		GET_TIMESTAMP(db->db_time);
		add_to_flood_list(db, &flood_list);
	    }
	    if (!count)
		flood_area(area, &flood_list, NULL, NULL);
	}
	flood_area(area, &flood_list, NULL, NULL);	
    }
    if (delay_flag)
	mgd_timer_start(&area->ar_exlock_timer, MINLSINTERVAL*ONESEC);
}/* ar_delayed_ex_lsa */


/*
 * pdb_sumupdate
 */
void pdb_sumupdate (pdbtype *pdb)
{
    list_queue	flood_list;
    areatype	*ar;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_event(OC_GENERIC, (ulong) "pdb_sumupdate", 0);
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    if (pdb->ospf_area_brtr) {
	for (ar = pdb->ospf_area_list; ar ; ar= ar->ar_next) {
	    check_ospf_suspend(pdb, DBASE_ACTIVE);
	    sum_update(ar, &flood_list);
	    flood_area(ar, &flood_list, NULL, NULL);
	}
    }
    mgd_timer_start(&ospf_pdb->ospf_sumupdate_timer, LS_REFRESH_TIME*ONESEC);
} /* end of pdb_sumupdate */


/*
 * stub_default
 */
void stub_default (areatype *area, ulong cost)
{
    lsdbtype *db;
    list_queue flood_list;

    ospf_event(OC_GENERIC, (ulong) "stub_default", area->ar_id);
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    db = NULL;
    db = db_find_lsa(area,
		     0,
		     ROUTER_ID(area),
		     LSA_TYPE_SUM_NET);
    build_sum_lsa(&db,
		  area,
		  0,
		  0,
		  cost,
		  LSA_TYPE_SUM_NET,
		  RTYPE_INTRA,
		  &flood_list);
    flood_area(area, &flood_list, NULL, NULL);

}/* end of stub_default */

/* 
 * nssa_default
 * Generate NSSA default
 */
void nssa_default (areatype *area, ulong cost)
{
    lsdbtype *db;
    list_queue flood_list;

    ospf_event(OC_GENERIC, (ulong) "nssa_default", area->ar_id);
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    db = NULL;
    db = db_find_lsa(area, 0, ROUTER_ID(area), LSA_TYPE_7_ASE);
    build_ex_lsa(&db, area->ar_pdb_ptr, area, 0, 0, cost, 0, 0, &flood_list,
		 METRIC_TYPE2, FALSE, FALSE, LSA_TYPE_7_ASE);
    flood_area(area, &flood_list, NULL, NULL);

}/* end of nssa_default */

/*
 * start_sum_brtr
 */
void start_sum_brtr (pdbtype *pdb)
{
    areatype 	*area;
    ipaddrtype	mask;
    list_queue	flood_list;
    ospf_pdb_info *ospf_pdb;

    ospf_event(OC_GENERIC, (ulong) "start_sum_brtr", 0);
    ospf_pdb = pdb->ospf_pdb;
    mask = 0;

    if (pdb->running == FALSE)
	return;
    flood_list.q_first = NULL;  
    flood_list.q_last  = NULL;
    if (pdb->ospf_area_brtr == FALSE) {
	pdb->ospf_area_brtr = TRUE;
	ospf_force_spf(pdb, NULL);
	mgd_timer_start(&ospf_pdb->ospf_sumupdate_timer, 
			LS_REFRESH_TIME*ONESEC);
	for (area = pdb->ospf_area_list; area;area=area->ar_next) {
	    build_rtr_lsa( area, &flood_list, FALSE);
	    flood_area(area, &flood_list, NULL, NULL);
	    if (area->ar_stub || (area->ar_nssa && area->ar_stub_nosum))
		stub_default(area, area->ar_stub_default_cost);
	    if (area->ar_nssa && area->ar_nssa_default)
		nssa_default(area, area->ar_stub_default_cost);
	}
	/*
	 * Every NSSA ABR is also ASBR
	 */
	if (ospf_pdb->ospf_nssa_area_count && !pdb->ospf_as_brtr)
	    ospf_redistribute(pdb, NULL, TRUE);
    }
} /* end of start_sum_brtr */
	

/*
 * stop_sum_brtr
 *
 */
void stop_sum_brtr (pdbtype *pdb)
{
    areatype 	*area;
    list_queue	flood_list;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_event(OC_GENERIC, (ulong) "stop_sum_brtr", 0);
    flood_list.q_first = NULL;
    flood_list.q_last  = NULL;
    if (pdb->ospf_area_brtr) {
	/* clean up all the mess
	 * in each area.
	 */
	pdb->ospf_area_brtr = FALSE;
	mgd_timer_stop(&ospf_pdb->ospf_sumupdate_timer);
	for (area = pdb->ospf_area_list; area;area=area->ar_next) {
	    ospf_flush_sum_from_area(area, &flood_list, TRUE);
	    build_rtr_lsa( area, &flood_list, FALSE);
	    flood_area(area, &flood_list, NULL, NULL);
	}
	if (pdb->ospf_as_brtr)
	    ospf_redistribute(pdb, NULL, FALSE);
    }
} /* end of stop_sum_brtr */


/*
 * ospf_check_indication_lsa
 * Advertise or remove indication LSA
 */
void ospf_check_indication_lsa (pdbtype *pdb)
{
    areatype *area, *target;
    boolean indication, db_exist;
    sum_lsatype *lsa;
    lsdbtype *db;
    list_queue flood_list;

    flood_list.q_first = NULL;
    flood_list.q_last = NULL;

    ospf_spf_inter_acl_pass = acl_ospf_spf_inter_debug(pdb->ospf_rtr_id);
    for (target = pdb->ospf_area_list; target; target = target->ar_next) {
	indication = TRUE;
	/*
	 * Don't generate indication LSA if there is non-indication
	 * DCbitless LSA in the target area.
	 */
	if (target->ar_nssa || target->ar_stub || 
	    (NON_INDICATION_LSA_COUNT(target) > 0)) {
	    indication = FALSE;
	}
	if (indication) {
	    db = db_get_first_db(target->ar_lsdbase[LSA_TYPE_SUM_ASB]);
	    for (; db; db = db_get_next_db(db)) {
		lsa = (sum_lsatype *)db->db_lsa_ptr;
		if (!lsa || db->db_freeme || 
		    GET_DC_BIT(lsa->la_hdr.lsh_options) ||
		    (lsa->la_tos_metric != SUM_LS_INFINITY)) {
		    continue;
		}
		if (LSA_ID(db) != ADV_RTR(db))
		    continue;
		if (pdb->ospf_rtr_id < ADV_RTR(db)) {
		    /*
		     * Other router with higher router id has generated the
		     * indication already
		     */
		    indication = FALSE;
		    break;
		}
	    }
	}
	if (indication) {
	    /*
	     * We could probably generate indication into target area.
	     */
	    indication = FALSE;
	    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
		if ((area == target) || area->ar_nssa || area->ar_stub)
		    continue;
		if (NON_INDICATION_LSA_COUNT(area) > 0)
		    indication = TRUE;
		if (!indication && (area == pdb->ospf_backbone)) {
		    if (area->ar_indication_lsa_count) {
			/*
			 * Check if we have generated indication into backbone
			 */
			db = db_find_lsa(area, pdb->ospf_rtr_id, 
					 pdb->ospf_rtr_id, 
					 LSA_TYPE_SUM_ASB);
			if (!db || db->db_freeme)
			    indication = TRUE;
		    }
		}
		/*
		 * If one area tell us to generate indication, no need to 
		 * look at other areas.
		 */
		if (indication)
		    break;
	    }
	}
	db = db_find_lsa(target, pdb->ospf_rtr_id, 
			 pdb->ospf_rtr_id, LSA_TYPE_SUM_ASB);
	db_exist = (db && !db->db_freeme);
	if ((indication && !db_exist) || (!indication && db_exist)) {
	    build_sum_lsa(&db, target, pdb->ospf_rtr_id, 0, 
			  SUM_LS_INFINITY, 
			  indication ? OSPF_ADV_INDICATION : 
			  OSPF_REMOVE_INDICATION,
			  RTYPE_INTRA, &flood_list);
	    flood_area(target, &flood_list, NULL, NULL);
	}
    }
    ospf_spf_inter_acl_pass = FALSE;
}

/*
 * ospf_perform_asbr_duty
 */
void ospf_perform_asbr_duty (pdbtype *pdb)
{
    list_queue	flood_list;
    areatype	*area;
    
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;

    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
        (void)build_rtr_lsa(area, &flood_list, TRUE);
        flood_area(area, &flood_list, NULL, NULL);
    }

    ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE);
}

/*
 * ospf_redistribute
 */
void ospf_redistribute (
    pdbtype *pdb,
    pdbtype *dist,
    boolean flag)
{
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    /* check pdb->distflag & dist->mask in case when user only
     * turns off some option in redist command, e.g. "no redist 
     * rip metric-type 1".   redistribute_command() calls us
     * with flag == FALSE but we don't want to actually stop
     * redist routes from that protocol.
     */
    if (flag || (dist && (pdb->distflag & dist->mask)) ) {
	pdb->redist_callback = ex_route_callback;
	pdb->summary_address_cb = ospf_summary_address;
	pdb->ospf_as_brtr = TRUE;
 	if (pdb->ospf_scanpid == 0)
	    pdb->ospf_scanpid = cfork((forkproc *)ospf_scanner, (long)pdb, 0, 
				      "OSPF Scanner", 0L);
	if (pdb->running) {
	    if (!mgd_timer_running(&ospf_pdb->ospf_exupdate_timer)) {
		mgd_timer_start(&ospf_pdb->ospf_exupdate_timer, 
				LS_REFRESH_TIME*ONESEC);
	    }
	    ospf_perform_asbr_duty(pdb);
	}
    } else {
	if ( ((pdb->distflag & ~pdb->mask)== 0) 
	    && (pdb->default_originate == FALSE)) {
	    mgd_timer_stop(&ospf_pdb->ospf_exupdate_timer);
	    pdb->redist_callback = NULL;
	    pdb->summary_address_cb = NULL;
	    pdb->ospf_as_brtr = FALSE;
	    if (pdb->ospf_scanpid) {
		process_kill(pdb->ospf_scanpid);
		pdb->ospf_scanpid = 0;
	    }
	}
	ospf_perform_asbr_duty(pdb);
    }

    ospf_stub_and_asbr_check(pdb);
}


/*
 * ospf_spf_check_ex_origin
 * Wrapper routine for external SPF calculation to check
 * if we should with withdraw our LSA.
 */
void ospf_spf_check_ex_origin (areatype *area, lsdbtype *db)
{
    pdbtype *pdb;
    lsdbtype *our_db;
    list_queue flood_list;
    ex_lsatype *ex_lsa, *our_ex_lsa;

    pdb = area->ar_pdb_ptr;
    ex_lsa = (ex_lsatype *)db->db_lsa_ptr;
    our_db = ospf_db_find_lsa(area, ex_lsa->la_hdr.lsh_id, 
			      ex_lsa->la_mask, pdb->ospf_rtr_id, LSA_TYPE(db));
    /*
     * If the other router withdraw its LSA, let us advertise ours.
     */
    if (db->db_won_redist && (ADV_AGE(db) >= MAXAGE)) {
	db->db_won_redist = FALSE;
	start_redist_net(pdb, ex_lsa->la_hdr.lsh_id & ex_lsa->la_mask, 
			 ex_lsa->la_mask);
	return;
    }

    /*
     * if we generate default, don't compare.  We will not flush 
     * default LSA! 
     */
    if ((pdb->default_originate) && (LSA_ID(db) == 0L))
	return;

    if (our_db) {
	if ((ADV_AGE(our_db) >= MAXAGE) || (our_db->db_freeme)) {
	    return;
	}
	our_ex_lsa = (ex_lsatype *)our_db->db_lsa_ptr;
	flood_list.q_first = flood_list.q_last = NULL;
	ospf_check_ex_origin(pdb, db, our_db, our_ex_lsa->la_mask,
			     our_ex_lsa->la_metric.as_forw_addr,
			     our_ex_lsa->la_metric.as_e_tos_metric,
			     &flood_list);
	ex_flood_area(pdb, &flood_list, NULL, NULL);
    }  
} /* ospf_spf_check_ex_origin */


/*
 * ospf_spf_check_default
 * Checks if we should install the defaults from others.
 */
boolean ospf_spf_check_default (lsdbtype *db, areatype *area)
{
    pdbtype	*pdb;
    lsdbtype	*our_db;

    pdb = area->ar_pdb_ptr;
    /*
     * if we generate default, don't install default from others
     */
    if ((pdb->default_originate) && (LSA_ID(db) == 0L)) {
	/*
	 * Check if our default LSA exists
	 */
	our_db = db_find_lsa(area, 0, pdb->ospf_rtr_id, LSA_TYPE(db));
	if (our_db && !our_db->db_freeme)
	    return(FALSE);
    }
    return(TRUE);
}/* ospf_spf_check_default */



/*
 * Common routine used to flush LSA.
 * It take a boolean function and use it to determine
 * whether the LSA should be flushed.
 * The boolean function take the pdb, area and db as parameters.
 * If the boolean function is NULL, it mean all LSAs should be
 * flushed.
 * If caller_flood_list is NULL, this function will flood the LSAs.
 * Otherwise, LSA is added to global_flood_list. The caller will
 * flood the LSA later.
 */
static inline void ospf_flush_lsa_inline (areatype *area, uchar type, 
					  ospf_flush_function flush_it,
					  list_queue *caller_flood_list,
					  int dbase_lock)
{
    pdbtype 	*pdb;
    ospf_pdb_info *ospf_pdb;
    list_queue  flood_list;
    lsdbtype 	*db;
    lsatype	*lsa;
    ex_lsatype	*ex_lsa;
    boolean	dc_bit, do_not_age_bit;
    boolean	old_non_indication_lsa_count, old_indication_lsa_count;
    int		count = 0;

    pdb = area->ar_pdb_ptr;
    ospf_pdb = pdb->ospf_pdb;

    flood_list.q_first = NULL;
    flood_list.q_last = NULL;

    /*
     * We should check the number of indication lsa.
     */
    old_non_indication_lsa_count = NON_INDICATION_LSA_COUNT(area);
    old_indication_lsa_count = area->ar_indication_lsa_count;

    for (db = db_get_first_db(area->ar_lsdbase[type]); db;
	 db = db_get_next_db(db)) {
	check_ospf_suspend_limit(pdb, dbase_lock, &count);
	if ((db->db_lsa_ptr == NULL) || (ADV_AGE(db) >= MAXAGE) ||
	    (db->db_freeme))
	    continue;
	if (!flush_it || (*flush_it)(pdb, area, db)) {
	    lsa = db->db_lsa_ptr;
	    lsa->lsh_seq = NEXTSEQ(lsa->lsh_seq);
	    do_not_age_bit = db->db_do_not_age;
	    /*
	     * Either flushing normal or DoNotAge LSA, the
	     * DoNotAge bit must be reset.
	     */
	    db->db_do_not_age = FALSE;
	    lsa->lsh_age = MAXAGE;
	    db->db_lsa_changed = TRUE;
	    dc_bit = GET_DC_BIT(lsa->lsh_options);
	    if ((type >= LSA_TYPE_SUM_NET) && (type <= LSA_TYPE_7_ASE)) {
		/*
		 * Use external LSA to access type 3 to type 7 LSA metric
		 * field. The offset is the same.
		 * Maxage LSA should have infinity metric;
		 */
		ex_lsa = (ex_lsatype *)lsa;
		ex_lsa->la_metric.as_e_tos_metric |= SUM_LS_INFINITY;
	    }
	    /*
	     * Note that self-originated LSA will always has
	     * DC bit set (except indication LSA) and DoNotAge bit
	     * reset.
	     */
	    ospf_dec_lsa_table(pdb, area, type, lsa->lsh_chksum);
	    if (type == LSA_TYPE_ASE) {
		if (do_not_age_bit)
		    ospf_pdb->ospf_do_not_age_lsa_count--;
		if (!dc_bit)
		    ospf_pdb->ospf_dcbitless_lsa_count--;
	    } else {
		if (do_not_age_bit)
		    area->ar_do_not_age_lsa_count--;
		if (!dc_bit)
		    area->ar_dcbitless_lsa_count--;
	    }
	    /*
	     * Are we flushing an indication LSA ?
	     */
	    if ((type == LSA_TYPE_SUM_ASB) && (LSA_ID(db) == ADV_RTR(db))) {
		area->ar_indication_lsa_count--;
	    }
	    db->db_freeme = TRUE;
	    lsa_checksum(db);
	    GET_TIMESTAMP(db->db_time);
	    db->db_flood_dly = FALSE;
	    ospf_insert_maxage(pdb, db);
	    if (caller_flood_list)
		add_to_flood_list(db, caller_flood_list);
	    else
		add_to_flood_list(db, &flood_list);
	}
	if (!caller_flood_list && !count) {
	    if (type == LSA_TYPE_ASE)
		ex_flood_area(pdb, &flood_list, NULL, NULL);
	    else
		flood_area(area, &flood_list, NULL, NULL);
	}
    }
    if (!caller_flood_list) {
	if (type == LSA_TYPE_ASE)
	    ex_flood_area(pdb, &flood_list, NULL, NULL);
	else
	    flood_area(area, &flood_list, NULL, NULL);
    }

    if ((old_non_indication_lsa_count > 0) &&
	(NON_INDICATION_LSA_COUNT(area) == 0)) {
	/*
	 * Non-indication DCbitless LSA become zero,
	 * withdraw indication LSA.
	 */
	area->ar_pdb_ptr->ospf_flag |= OSPF_CHECK_INDICATION;
    } else if (old_indication_lsa_count != area->ar_indication_lsa_count) {
	area->ar_pdb_ptr->ospf_flag |= OSPF_CHECK_INDICATION;
    }
} /* ospf_flush_lsa_inline */


/*
 * This function return TRUE if we are the advertising router for
 * the LSA.
 */
static boolean is_self_originated_lsa (pdbtype *pdb, areatype *area, 
				       lsdbtype *db)
{
    return (db->db_lsa_ptr->lsh_adv_rtr == pdb->ospf_rtr_id);
}


/*
 * This function return TRUE if it is a do not age LSA
 */
static boolean is_do_not_age_lsa (pdbtype *pdb, areatype *area, lsdbtype *db)
{
    return (db->db_do_not_age);
}

/*
 * This function return TRUE if we are not ABR or the type 5 LSA is
 * originated within this area. It return FALSE otherwise.
 */
static boolean is_type5_in_nssa (pdbtype *pdb, areatype *area, lsdbtype *db)
{
    lsdbtype *asbr_db;
    /*
     * If not connected to regular area, flush all external LSA.
     * Otherwise, flush only those LSA originated within this NSSA area
     * If that NSSA ASBR is also connected to regular area and we flush its
     * type 5 LSA, it will be no problem as the NSSA ASBR will 
     * reoriginate the type 5 LSA again.
     */
    if (pdb->ospf_pdb->ospf_normal_area_count) {
	asbr_db = db_find_lsa(area, ADV_RTR(db), ADV_RTR(db), LSA_TYPE_RTR);
	if (!asbr_db)
	    return FALSE;
    }
    return TRUE;
} /* end of is_type5_in_nssa */


/*
 * ospf_flush_type5
 * Flush all external LSAs originated by this router
 * from database.
 */
void ospf_flush_type5 (pdbtype *pdb)
{
    if ((pdb->ospf_dbase_lock == DBASE_ACTIVE) ||
	(pdb->ospf_dbase_lock == DBASE_FREEZE)) {
	return;
    }
    ospf_buginf("\nOSPF: Flushing External Links");
    ospf_flush_lsa_inline(pdb->ospf_pdb->ospf_dummy_area, LSA_TYPE_ASE, 
			  is_self_originated_lsa, NULL, DBASE_FREEZE);
} /* end of ospf_flush_type5 */


/*
 * Flush type 5 external LSA from stub/NSSA area.
 * If it is an ABR, only flush those LSA originated within this area
 */
void ospf_flush_all_type5_from_stub (areatype *area)
{
    if (!area->ar_nssa && !area->ar_stub)
	return;
    ospf_flush_lsa_inline(area, LSA_TYPE_ASE, is_type5_in_nssa, NULL, 
			  DBASE_ACTIVE);
} /* ospf_flush_all_type5_from_stub */

/*
 * ospf_flush_area:
 * Flush self-originated type 1-4 and 7 link state.  (External is flushed by
 * ospf_flush_type5())
 */
void ospf_flush_area (areatype *area)
{
    pdbtype	*pdb;
    ushort	type;

    pdb = area->ar_pdb_ptr;
    if ((pdb->ospf_dbase_lock == DBASE_ACTIVE) ||
	(pdb->ospf_dbase_lock == DBASE_FREEZE)) {
	return;
    }
    ospf_buginf("\nOSPF: Flushing Link states in area %s", area->ar_id_str);
    /* grab all of the links */
    for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++) {
	if ((type == LSA_TYPE_IGNORE) || (type == LSA_TYPE_ASE))
	    continue;
	ospf_flush_lsa_inline(area, type, is_self_originated_lsa, NULL, 
			      DBASE_FREEZE);
    }
} /* end of ospf_flush_area */


/*
 * ospf_flush_all_do_not_age_lsa
 * Flush all DoNotAge LSA, generate indication LSA if neccessary.
 */
void ospf_flush_all_do_not_age_lsa (pdbtype *pdb)
{
    areatype *area;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;
    boolean check_external = FALSE;
    int type;

    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	if (area->ar_dcbitless_lsa_count > 0) {
	    check_external = TRUE;
	    if (area->ar_do_not_age_lsa_count > 0) {
		for (type = LSA_TYPE_RTR; type <= LSA_TYPE_7_ASE; type++) {
		    if ((type == LSA_TYPE_IGNORE) || (type == LSA_TYPE_ASE))
			continue;
		    ospf_flush_lsa_inline(area, type, is_do_not_age_lsa, 
					  NULL, DBASE_ACTIVE);
		}
	    }
	}
    }
    if (check_external && ospf_pdb->ospf_do_not_age_lsa_count) {
	ospf_flush_lsa_inline(ospf_pdb->ospf_dummy_area, LSA_TYPE_ASE, 
			      is_do_not_age_lsa, NULL, DBASE_ACTIVE);
    }
    /*
     * Generate indication LSA
     */
    if (pdb->ospf_pdb->ospf_normal_area_count)
	ospf_check_indication_lsa(pdb);
}


/* 
 * ospf_flush_all_type7_from_area 
 * flush all NSSA type 7 LSA for an area.
 */
void ospf_flush_all_type7_from_area (areatype *area)
{
    ospf_flush_lsa_inline(area, LSA_TYPE_7_ASE, NULL, NULL, DBASE_ACTIVE);
} /* end of ospf_flush_all_type7_from_area */

/* 
 * ospf_flush_sum_from_area 
 * flush summary LSA for an area.  Depending on the include_sum_net boolean,
 * we will flush summary network LSA (type 3) only when "area stub no-summary"
 * is configured.
 */
void ospf_flush_sum_from_area (areatype *area, list_queue *flood_list,
			       boolean include_sum_net)
{
    ushort	type;

    /* 
     * start with summary network if include_sum_net is set, otherwise
     * start with summary ASBR (type 4)
     */
    type = (include_sum_net ? LSA_TYPE_SUM_NET : LSA_TYPE_SUM_ASB);
    for (; type < LSA_TYPE_ASE; type++) {
	ospf_flush_lsa_inline(area, type, is_self_originated_lsa, 
			      flood_list, DBASE_ACTIVE);
    }
} /* end of ospf_flush_sum_from_area */


/*
 * ospf_flush_all_type4_from stub/NSSA area
 * Flush all type 4 LSA from an area
 */
void ospf_flush_all_type4_from_stub (areatype *area)
{
    ospf_flush_lsa_inline(area, LSA_TYPE_SUM_ASB, NULL, NULL, DBASE_ACTIVE);
} /* end of ospf_flush_all_type4_from_stub */


/*
 * Flush type 4 LSA for ASBR within this NSSA area if we are
 * ABR.
 */
void ospf_flush_nssa_type4_from_other (areatype *area)
{
    pdbtype	*pdb;
    lsdbtype	*db, *other_asbr_db;
    rtr_lsatype *rtr_lsa;
    list_queue	flood_list;
    areatype	*ar;
    int		count = 0;

    pdb = area->ar_pdb_ptr;
    if ((pdb->ospf_dbase_lock == DBASE_ACTIVE)
	|| (pdb->ospf_dbase_lock == DBASE_FREEZE))
	return;
    if (!pdb->ospf_area_brtr)
	return;
    INIT_OSPF_SUSPEND(pdb);
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    for (db = db_get_first_db(area->ar_lsdbase[LSA_TYPE_RTR]); db;
	 db = db_get_next_db(db)) {
	check_ospf_suspend_limit(pdb, DBASE_ACTIVE, &count);
	if ((db->db_lsa_ptr == NULL) 
	    || (ADV_AGE(db) >= MAXAGE) || db->db_freeme)
	    continue;
	rtr_lsa = (rtr_lsatype *)db->db_lsa_ptr;
	if (!(rtr_lsa->la_E_B & AS_BORDER_E_BIT))
	    continue;
	/*
	 * If it is a summary for ASBR in this area, which is becoming
	 * NSSA, we flush it.
	 */
	other_asbr_db = NULL;
	for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
	    if ((ar == area) || ar->ar_stub || ar->ar_nssa)
		continue;
	    other_asbr_db = db_find_lsa(ar, LSA_ID(db), LSA_ID(db), 
					LSA_TYPE_RTR);
	    if (other_asbr_db)
		break;
	}
	/*
	 * If it is also an ASBR in another regular area, keep the
	 * summary.
	 */
	if (other_asbr_db)
	    continue;
	generate_sum(area, db, SUM_LS_INFINITY);
    }
} /* end of ospf_flush_nssa_type4_from_other */


/* 
 * ospf_send_summary_lsa:
 * A wrapper routine for build_sum_lsa() to figure out what LS ID we
 * should use in summary LSA.   This is to support route summarization
 * in supernet style between areas in OSPF domain.
 */
void ospf_send_summary_lsa (
    lsdbtype 	**db_ptr,
    areatype 	*area,
    ipaddrtype 	dest,
    ipaddrtype 	mask,
    ulong	metric,
    uchar	type,
    ushort	sum_origin,			    
    list_queue	*flood_list)
{
    pdbtype	*pdb;
    lsdbtype 	*rdb, *rdb_super;
    ipaddrtype	dest_with_host_bit;
    ipaddrtype	old_mask;
    ulong	old_metric;
    ushort	old_sum_origin;
    sum_lsatype	*sum_lsa;
    boolean 	normal_found;

    pdb = area->ar_pdb_ptr;

    normal_found = FALSE;

    if (type == LSA_TYPE_SUM_ASB) {
	build_sum_lsa(db_ptr,
		      area,
		      dest,
		      mask,
		      metric,
		      LSA_TYPE_SUM_ASB,
		      sum_origin,
		      flood_list);
	return;
    }

    /* First, look for the LSA with normal LSA ID */
    rdb = db_find_lsa(area,
		      dest,
		      pdb->ospf_rtr_id,
		      type);
    /*
     * If found a db, check if the mask is the same.
     * Should be the case for all normal case (no suppernetting or subnet zero)
     * If we find it but the mask doesn't match, that means we have 
     * advertised other LSA with the same LS ID already.   Look further
     * for LS ID with mask bit.
     */
    if (rdb) {
	normal_found = TRUE;
	sum_lsa = (sum_lsatype *)(rdb->db_lsa_ptr);
	if (sum_lsa->la_mask == mask) {
	    build_sum_lsa(&rdb, area, dest, mask, metric, type,
			  sum_origin, flood_list);
	    *db_ptr = rdb;
	    return;
	} 
    } 

    /* 
     * Try looking up LSA with host bit.   Even though we might not find
     * the LSA with normal LS ID, it might have been purged out after we 
     * have generated the LSA with the funky mask already.   So we have 
     * to look for that LSA before generating a LSA with normal LS ID.
     * This means we will stuck with the funky mask until we restart 
     * redistribution process. (no redist ... , then redist again).
     */
    dest_with_host_bit = (dest | ~mask);
    rdb_super = db_find_lsa(area,
			    dest_with_host_bit,
			    pdb->ospf_rtr_id,
			    type);
    if (rdb_super) {
	sum_lsa = (sum_lsatype *)(rdb_super->db_lsa_ptr);
	if (sum_lsa->la_mask == mask) {
	    build_sum_lsa(&rdb_super, area, dest_with_host_bit, mask, 
			  metric, type, sum_origin, flood_list);
	    *db_ptr = rdb_super;
	} else {
	    /*
	     * This is the case when we have supernet LSA with host bit
	     * bitwise-or in, which makes the LSA ID looks like normal
	     * LSA ID for normal major network.   For example, 
	     * 193.1.128.0/255.255.254.0 got LS ID. 193.1.129.255.
	     * Then when we want to generate LSA for 193.1.129.0/24
	     * For the reason above, we look for funky mask LSA and
	     * find one but the mask is not the same.   This is ok
	     * if we haven't found the normal one.   It is error otherwise.
	     */
	    if (normal_found == FALSE) {
		build_sum_lsa(&rdb, area, dest, mask, metric, type, 
			      sum_origin, flood_list);
		*db_ptr = rdb;
	    } else {
		errmsg(&msgsym(CONFLICTING_LSAID, OSPF),
		       sum_lsa->la_hdr.lsh_id, sum_lsa->la_mask, 
		       dest, mask);
	    }
	}
    } else {
	if (normal_found) {
	    /*
	     * There is no LSA with the host bit set yet but there is
	     * one with normal LSA ID,  we will send one of these with
	     * host bit set.   The one with shorter mask will use
	     * normal LSA ID, while the other one will use LSA ID with
	     * inverse mask bits bitwise-or in.
	     */
	    sum_lsa = (sum_lsatype *)(rdb->db_lsa_ptr);
	    if (mask > sum_lsa->la_mask) { 
		/* 
		 * Old mask is shorter, it keeps the LSA ID.
		 * Generate the new one with LSA ID with host bit set
		 */
		build_sum_lsa(&rdb_super, area, dest_with_host_bit, mask, 
			      metric, type, sum_origin, flood_list);
		*db_ptr = rdb_super;
	    } else {
		/* 
		 * New mask is shorter, steal the existing db by
		 * using rdb but give it a new mask.
		 * Then re-generates the old one with new LSA ID with
		 * host bit, with the old mask.
		 */
		old_mask = sum_lsa->la_mask;
		old_metric = sum_lsa->la_tos_metric;
		if (rdb->db_sum_origin_intra) 
		    old_sum_origin = RTYPE_INTRA;
		else 
		    old_sum_origin = RTYPE_INTER;
		build_sum_lsa(&rdb, area, dest, mask, metric, type, 
			      sum_origin, flood_list);

		build_sum_lsa(&rdb_super, area, (dest | ~old_mask),
			      old_mask, old_metric, type, old_sum_origin,
			      flood_list);

		*db_ptr = rdb;
	    }
	} else {
	    /* there is none yet, just generate one */
	    build_sum_lsa(&rdb, area, dest, mask, metric, type,
			  sum_origin, flood_list);
	    *db_ptr = rdb;
	}
    }
} /* end of ospf_send_summary_lsa */

/*
 * ospf_get_external_metric:
 *
 * Figure out the metric to be advertised in an external LSA for ndb.
 */
ulong ospf_get_external_metric (pdbtype *pdb, pdbtype *s_pdb, ndbtype *ndb,
				ushort event)
{
    ulong metric;

    if (event == ROUTE_DOWN)
       return(SUM_LS_INFINITY);

    metric = DEFAULT_MAP_METRIC;

    if (ip_def_metric_configured(pdb, s_pdb->index)) {
	metric = ip_get_def_metric_scalar(pdb, s_pdb->index);
    } else {
	if (s_pdb->proctype & PROC_BGP)
	    metric = 1;
	/*
	 * Preserve OSPF metric when redistributes between 2 OSPF processes
	 * if metric is not specified.   If the sdb is a connected route,
	 * use OSPF interface cost.
	 */
	if (s_pdb->proctype & PROC_OSPF) {
	    if (NDB2PROCTYPE(ndb) != PROC_CONNECTED) {
		metric = ndb->metric ? ndb->metric : 1;
	    } else {
		if (ndb->rdb[0]) {
                    if (ndb->rdb[0]->idb->ospf_info)
                        metric =
                            ndb->rdb[0]->idb->ospf_info->if_cost[0];
                    else {
                        /*
                         * This code is needed because we have seen, only
                         * at iniatilization time, cases where we try to
                         * redistribute between ospf processes before the
                         * ospf_info is allocated.  Here's the sequence of
                         * events: 
                         *
                         * network_finish
                         *  ospf_network_command
                         *   set_ndb_range_flags
                         *    set_ndb_flags_subr
                         *      network_redist
                         *       ex_route_callback
                         *        build_ex_route
                         *         ospf_get_external_metric (no ospf_info)
                         * ospf_parse_range attach_interface
                         *                           (creates ospf_info)
                         *
                         * In this case, we start the summary timer, 
                         * and return the default metric.
                         */
                         ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE);
                    }
		} else {
		    /*
		     * The interface has gone down. The caller try
		     * to figure out the metric if the interface
		     * is up.
		     * Make the metric equal 1 (the smallest valid metric) 
		     * so it will trigger a rescan
		     */
		    metric = 1;
		}
	    }
	}
    }

    return(metric);
}

/*
 * ospf_get_route_map_set_value
 *
 * Get values set in route-map.   Caller should call pdb_routemap()
 * to find the map before calling this routine.
 */
void ospf_get_route_map_set_value (route_maptype *map,
				   ulong   *metric,
				   ushort  *metric_type,
				   ulong   *tag)
{
    if (map) {
	if (map->set_list.set_flags & ROUTE_MAP_METRIC_SET &&
	    !map->set_list.metric.igrp_metric) {
	    *metric = map->set_list.metric.value.scalar;
	}
	if (map->set_list.set_flags & ROUTE_MAP_METRICTYPE_SET) {
	    if (map->set_list.metric_type == ROUTE_MAP_SET_TYPE2) {
		*metric_type = METRIC_TYPE2;
	    }
	    if (map->set_list.metric_type == ROUTE_MAP_SET_TYPE1) {
		*metric_type = METRIC_TYPE1;
	    }
	}
	if (map->set_list.set_flags & ROUTE_MAP_TAG_SET) {
	    *tag = map->set_list.tag;
	}
    }
}

/*
 * ospf_redist_summary_check:
 * Check if sdb should be summarized or not.   If yes, compare metric and
 * reset it if it is less than what is in the summary-address structure.
 *
 * Return boolean telling whether the external LSA for sdb should be 
 * advertised (TRUE) or withdrawn (in case sdb falls into a summary-address
 * range).
 *
 * If set_timer, we set the summary timer if neccessary.
 */
boolean ospf_redist_summary_check (pdbtype *pdb, 
				   ndbtype *sdb, 
				   pdbtype *dist, 
				   ushort event,
				   boolean set_timer)
{
    addrsumtype	*summary;
    ushort 	metric_type;
    ulong  	new_metric;
    ulong	tag;

    ospf_event(OC_REDIST_SUM_CHECK, (ulong) sdb->netnumber, 
	       ((ulong) dist->proctype) | (event << 24));

    summary = ip_get_summary(pdb, sdb->netnumber);

    /*
     * Don't fall into any range, advertise the sdb itself.
     */
    if (!summary)
	return(TRUE);

    /*
     * Suppress this summary range
     */
    if (summary->flags & ADDRSUM_NOT_ADV)
	return FALSE;

    /*
     * Clear the old information for translating type 7
     */
    if (summary->flags & ADDRSUM_OSPF_TYPE7) {
	summary->metric = SUM_LS_INFINITY;
	summary->metric_type = 0;
	summary->adv_tag = 0;
	summary->flags &= ~ADDRSUM_OSPF_TYPE7;
    }

    summary->flags |= ADDRSUM_OSPF_REDIST;
    /* 
     * Look up for metric that we would advertise if the sdb is up.
     */
    new_metric = ospf_get_external_metric(pdb, dist, sdb, ROUTE_UP);
    metric_type = pdb->metric[dist->index].type;
    tag = pdb->metric[dist->index].tag;

    if (new_metric != SUM_LS_INFINITY) {
	if (pdb->route_map)
	    ospf_get_route_map_set_value(pdb->route_map, &new_metric, 
					 &metric_type, &tag);
    }

    switch (event) {
    case ROUTE_UP:
    case ROUTE_MODIF:
	/*
	 * If the new cost is better, set the metric for summary.
	 * If this ndb is redistributed as an OSPF route and the
	 *   metric chages (better or worse), we have to change 
	 *   the metric of the summary.
	 * If the new cost is worse or equal, do nothing.
	 */
	if (new_metric < summary->metric) {
	    if (acl_debug_ospf_lsa_gen(sdb->netnumber, sdb->netsmask))
		buginf("\nOSPF: net %i up, new metric decreases:"
		       " old %d, new %d", sdb->netnumber,
		       summary->metric, new_metric);

	} else if (dist->proctype & PROC_OSPF) {
	    if (new_metric == summary->metric)
		return(FALSE);
	    if (acl_debug_ospf_lsa_gen(sdb->netnumber, sdb->netsmask))
		buginf("\nOSPF: net %i is OSPF's, metric changes:"
		       " old %d, new %d", sdb->netnumber,
		       summary->metric, new_metric);

	} else {
	    if (acl_debug_ospf_lsa_gen(sdb->netnumber, sdb->netsmask))
		buginf("\nOSPF: net %i up, old cost %d, "
		       "new cost %d",
		       sdb->netnumber,	summary->metric, new_metric);
	    return(FALSE);
	}

	summary->metric = new_metric;
	summary->metric_type = metric_type;
	break;

    case ROUTE_DOWN:
	/*
	 * If the redistributed metric of this ndb is more than
	 *   what the summary is having, punt.
	 * If it is equal, send out a debug message.
	 * Otherwise, we should have a rescan.
	 */
	if (new_metric > summary->metric) {
	    return(FALSE);

	} else if (new_metric == summary->metric) {
	    if (acl_debug_ospf_lsa_gen(sdb->netnumber, sdb->netsmask))
		buginf("\nOSPF: net %i down, metric %d equal to "
		       "summary metric", sdb->netnumber, summary->metric);
	}
	break;
    }

    /*
     * Check for both redistribution and translation.
     * If all redistributed routes go away, we would still have to
     * generated the summary because of translation.
     */
    if (set_timer)
	ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE | OSPF_TRANSLATE_TYPE7);
    return(FALSE);
} /* end of ospf_redist_summary_check */

/*
 * ospf_redistribute_one_route
 * Redistribute one route
 */
static void ospf_redistribute_one_route (pdbtype *pdb, ndbtype *ndb, 
				  list_queue *flood_list) 
{
    int event;
    pdbtype *src_pdb;

    /*
     * Skip default route OR route in holddown OR NET_NOREDIST route
     */
    if ((!ndb->netnumber) ||
	(ndb->metric == METRIC_INACCESSIBLE) ||
	(ndb->attrflags & NET_NOREDIST))
	return;

    /*
     * Skip summary OSPF route.
     */
    if ((ndb->attrflags & NET_SUMMARY) && 
	(INDEX2MASK(ndb->pdbindex) & pdb->mask))
	return;

    /*
     * Get source pdb for route. It is INDEX2PDB(ndb->pdbindex) unless 
     * it is attached, then it is the best distance pdb.
     */
    src_pdb = ip_get_src_pdb(pdb, ndb, TRUE);

    /*
     * if the source of this route is same as this pdb, just return.
     */
    if (src_pdb == pdb)
	return;

    event = ROUTE_UP;
    if (redistribute_check(pdb, ndb, src_pdb, &event)) {
	/*
	 * The ndb is eligible for redistribution.
	 * Only test with summary when ROUTE_UP as we are 
	 * recalculating summary metric.
	 */
	if ((event == ROUTE_UP) &&
	    !ospf_redist_summary_check(pdb, ndb, src_pdb, event, FALSE)) {
	    /*
	     * Summary route should be generated.
	     */
	    event = ROUTE_DOWN;
	}
    } else {
	/*
	 * The ndb is not eligible for redistribution.
	 */
	event = ROUTE_DOWN;
    }
    build_ex_route(pdb, ndb, flood_list, event, src_pdb, FALSE);
}

/*
 * ospf_translate_type7_sum_check
 * Check if the LSA fall into summary range
 */
static boolean ospf_translate_type7_sum_check (pdbtype *pdb, ulong lsid, ulong mask,
					ulong metric, ushort metric_type, 
					ulong forw_addr, ulong tag,
					ushort event, boolean set_timer)
{
    addrsumtype *sum;
    boolean exact_match;
    ospf_pdb_info *ospf_pdb;

    ospf_pdb = pdb->ospf_pdb;
    sum = ip_get_summary(pdb, lsid & mask);
    if (!sum)
	return TRUE;

    /*
     * If there is type 5 LSA fall in this range, don't bother translating
     * type 7
     */
    if (sum->flags & ADDRSUM_OSPF_REDIST)
	return FALSE;
    /*
     * Suppress this summary range
     */
    if (sum->flags & ADDRSUM_NOT_ADV)
	return FALSE;

    if ((sum->network == lsid) && (sum->mask == mask)) {
	exact_match = TRUE;
	sum->flags |= ADDRSUM_EXACT_MATCH;
    } else {
	exact_match = FALSE;
	sum->flags &= ~ADDRSUM_EXACT_MATCH;
    }

    sum->flags |= ADDRSUM_OSPF_TYPE7;

    switch (event) {
    case ROUTE_UP:
    case ROUTE_MODIF:
	if (exact_match) {
	    if (!sum->metric_type || (sum->metric == metric) || 
		(sum->forw_addr != forw_addr)) {
		/*
		 * No summary generated yet or
		 * different metric or different forwarding address.
		 */
		sum->metric_type = metric_type;
		sum->metric = metric;
		sum->forw_addr = forw_addr;
		sum->adv_tag = tag;
	    }
	} else {
	    /*
	     * More specific route
	     */
	    if ((metric_type >= sum->metric_type)) {
		if ((sum->metric == SUM_LS_INFINITY) || (metric > sum->metric)) {
		    sum->metric = metric;
		    sum->metric_type = metric_type;
		}
	    } else {
		return FALSE;
	    }
	}
	break;
    case ROUTE_DOWN:
	/*
	 * Alway set timer
	 */
	break;
    }
    if (set_timer)
	ospf_set_summary_timer(pdb, OSPF_TRANSLATE_TYPE7);
    return FALSE;
}

/*
 * ospf_translate_one_type7
 * Translate one type 7 LSA into type 7 LSA
 */
void ospf_translate_one_type7 (pdbtype *pdb, ndbtype *ndb, ulong lsid,
			       ulong mask, list_queue *flood_list, 
			       boolean set_timer)
{
    ushort event;
    ulong metric, tag, forw_addr;
    ushort metric_type;
    lsdbtype *lsdb;
    rdbtype *rdb;
    areatype *area;
    ex_lsatype *ex_lsa;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    event = ROUTE_UP;
    metric = SUM_LS_INFINITY;
    metric_type = 0;
    tag = 0;
    forw_addr = 0;

    if (!ndb) {
	event = ROUTE_DOWN;
    } else if ((pdb->index != ndb->pdbindex) ||
	       ((ndb->route_type != RTYPE_NSSA1 ) && 
		(ndb->route_type != RTYPE_NSSA2))) {
	return;
    } else {
	rdb = ndb->rdb[0];
	area = rdb->ospf_rdb_area;
	if (!area->ar_nssa_translate_type7) {
	    event = ROUTE_DOWN;
	} else {
	    lsid = LSA_ID(rdb->ospf_rdb_lsdb);
	    ex_lsa = (ex_lsatype *) rdb->ospf_rdb_lsdb->db_lsa_ptr;
	    mask = ex_lsa->la_mask;
	    metric = ASE_METRIC(ex_lsa->la_metric.as_e_tos_metric);
	    metric_type = EXLSA_TO_METRIC_TYPE(ex_lsa);
	    forw_addr = ex_lsa->la_metric.as_forw_addr;
	    tag = ex_lsa->la_metric.as_ex_tag;
	    if (!(ex_lsa->la_hdr.lsh_options & OPTION_N_P_BIT) ||
		!forw_addr) {
		metric = SUM_LS_INFINITY;
		event = ROUTE_DOWN;
	    }
	}
    }

    if (!ospf_translate_type7_sum_check(pdb, lsid & mask, mask, metric,
					metric_type, forw_addr, tag, event,
					set_timer)) {
	metric = SUM_LS_INFINITY;
    }

    lsdb = NULL;
    if (metric == SUM_LS_INFINITY) {
	lsdb = ospf_db_find_lsa(ospf_pdb->ospf_dummy_area, lsid & mask, 
				mask, pdb->ospf_rtr_id, LSA_TYPE_ASE);
	if (!lsdb)
	    return;
	ex_lsa = (ex_lsatype *) lsdb->db_lsa_ptr;
	lsid = LSA_ID(lsdb);
	mask = ex_lsa->la_mask;
	metric_type = EXLSA_TO_METRIC_TYPE(ex_lsa);
	forw_addr = ex_lsa->la_metric.as_forw_addr;
	tag = ex_lsa->la_metric.as_ex_tag;
    }
    ospf_send_external_lsa_sub(&lsdb, pdb, ospf_pdb->ospf_dummy_area,
			       lsid & mask, mask, metric, forw_addr,
			       tag, flood_list, metric_type,
			       FALSE, LSA_TYPE_ASE);
}

/*
 * ospf_service_redist_summary
 * Called because of ospf_redist_summary_timer, to service route 
 * redistribution.   We scan the routing table so the right thing will
 * happen for summary-address.
 */
void ospf_service_redist_summary (pdbtype *pdb)
{
    ndbtype	*ndb, *sdb, *db, *tmp_ndb;
    int		ndb_bucket, sdb_bucket;
    addrsumtype	*sum, *save_sum;
    list_queue	flood_list;
    lsdbtype 	*lsdb;
    pdbtype	*src_pdb;
    ulong       start_time, flags, tag;
    rdbtype	*rdb;
    ospf_pdb_info *ospf_pdb;
    ushort	reason;
    boolean	ospf_debug;

    ospf_event(OC_SERVICE_REDIST_SUM, 0, 0);
    if (ospf_debug_lsa_gen || ospf_debug_spf_monitor)
	buginf("\nOSPF: Start redist-scanning");
    ospf_pdb = pdb->ospf_pdb;
    reason = ospf_pdb->ospf_set_summary_timer_reason;

    ospf_debug = (ospf_debug_lsa_gen || ospf_debug_spf_monitor);

    flags = 0;
    if (reason == (OSPF_REDIST_ROUTE | OSPF_TRANSLATE_TYPE7)) {
	if (ospf_debug)
	    buginf("\nOSPF: Scan for both redistribution and translation");
	flags = ADDRSUM_OSPF_REDIST | ADDRSUM_OSPF_TYPE7;
    } else if (reason & OSPF_REDIST_ROUTE) {
	if (ospf_debug)
	    buginf("\nOSPF: Scan for redistribution");
	flags = ADDRSUM_OSPF_REDIST;
    } else if (reason & OSPF_TRANSLATE_TYPE7) {
	if (ospf_debug)
	    buginf("\nOSPF: Scan for translation");
	flags = ADDRSUM_OSPF_TYPE7;
    }
    start_time = runtime();

    ndb_bucket = sdb_bucket = 0;
    ndb = sdb = NULL;
    src_pdb = NULL;
    flood_list.q_first = NULL;
    flood_list.q_last = NULL;
    flood_list.count  = 0;

    /*
     * Set summaries initially as unreachable.
     * If we are doing translation, only clear summaries created by 
     * translation.
     * Similarily, if we are doing redistribution, only clear summaries 
     * created by redistribution.
     * Clear all summaries if we are doing both.
     */
    for (sum = (addrsumtype *) pdb->ip_address_sumq.qhead; sum;
	 sum = sum->next) {
	if (sum->flags & flags) {
	    sum->metric = SUM_LS_INFINITY;
	    sum->metric_type = 0;
	    sum->adv_tag = 0;
	    sum->flags &= ~(ADDRSUM_OSPF_REDIST | ADDRSUM_OSPF_TYPE7);
	}
    }
    flags = 0;

    while ((db = geteachroute(&ndb, &ndb_bucket, &sdb, &sdb_bucket, TRUE))) {
	pdb->ospf_dbase_lock = DBASE_INACTIVE;

	if (pdb == INDEX2PDB(db->pdbindex)) {
	    if (reason & OSPF_TRANSLATE_TYPE7)
		ospf_translate_one_type7(pdb, db, db->netnumber, db->netsmask,
					 &flood_list, FALSE);
	} else if (reason & OSPF_REDIST_ROUTE) {
	    ospf_redistribute_one_route(pdb, db, &flood_list);
	}
	pdb->ospf_dbase_lock = DBASE_ACTIVE;
    }

    pdb->ospf_dbase_lock = DBASE_INACTIVE;
    /*
     * Now all summary-address is updated, generate a external LSA for
     * summary-address if it is a good range.  Also, create a summary
     * route to NULL0.   If the range has INFINITY metric, attempt to
     * delete the summary route and max-age the LSA because we might
     * have generate it in the past.
     */

    for (sum = (addrsumtype *) pdb->ip_address_sumq.qhead; sum;) {
	if (sum->flags & ADDRSUM_EXACT_MATCH)
	    tag = sum->adv_tag;
	else
	    tag = sum->tag;
	if (sum->metric != SUM_LS_INFINITY) {
	    /*
	     * If there is no real route exactly match the summary network 
	     * number, install a summary route.
	     */
	    tmp_ndb = ip_rtlookup(sum->network, sum->mask);
	    if ((tmp_ndb == NULL) || (tmp_ndb->attrflags & NET_SUMMARY)) {
		flags = NET_SUMMARY;
		rdb = network_update(pdb, sum->network, sum->mask,
				     IPADDR_ZERO, nullidb, 0, flags,
				     0, 0, NULL, RTYPE_INTRA);
	    }
	    if (sum->flags & ADDRSUM_OSPF_REDIST) {
		/*
		 * Generate both type 5 and type 7
		 */
		ospf_send_external_lsa(&lsdb, pdb, sum->network, sum->mask,
				       sum->metric, 0, tag, &flood_list, 
				       sum->metric_type, FALSE);
	    } else {
		/*
		 * Generate type 5 only
		 */
		ospf_send_external_lsa_sub(&lsdb, pdb, ospf_pdb->ospf_dummy_area,
					   sum->network, sum->mask,
					   sum->metric, 0, tag, &flood_list,
					   sum->metric_type, FALSE, 
					   LSA_TYPE_ASE);
	    }
	    if (acl_debug_ospf_lsa_gen(sum->network, sum->mask))
		buginf("\nOSPF: generate external LSA for "
		       "summary %i %i, metric %d",
		       sum->network, sum->mask, sum->metric);
	    sum = sum->next;

	} else {
	    ip_route_delete(pdb, sum->network, sum->mask,
			    IPADDR_ZERO, IPADDR_ZERO, NULL,
			    RTYPE_INTRA);
	    ospf_send_external_lsa(&lsdb, pdb, sum->network, 
				   sum->mask, SUM_LS_INFINITY, 0, 
				   sum->tag, &flood_list, 
				   sum->metric_type, FALSE);
	    if (acl_debug_ospf_lsa_gen(sum->network, sum->mask))
		buginf("\nOSPF: max-aged external LSA for summary %i %i",
		       sum->network, sum->mask);
	    /*
	     * When summary-address is removed, it is marked with 
	     * ADDRSUM_INVALID so the summary external LSA can be
	     * flushed before summary-address itself is removed.
	     * Calls start_redist_net to check in case there is a
	     * real route that matches the summary-address exactly
	     * so it will be redistributed.
	     */
	    save_sum = sum->next;
	    if (!(sum->flags & ADDRSUM_VALID)) {
		start_redist_net(pdb, sum->network, sum->mask);
		unqueue(&pdb->ip_address_sumq, sum);
		free(sum);
	    }
	    sum = save_sum;
	}
    }

    if (pdb->default_originate)
	if ((default_ndb && !ip_own_ndb(default_ndb, pdb))
	    || pdb->default_always || pdb->default_routemap)
	    build_ex_default_route(pdb, default_ndb, &flood_list, ROUTE_UP,
				   FALSE);

    check_ospf_suspend(pdb, DBASE_ACTIVE);

    ex_flood_area(pdb, &flood_list, NULL, NULL);

    check_ospf_suspend(pdb, DBASE_ACTIVE);

    ospf_pdb->ospf_set_summary_timer_reason = 0;
    if (ospf_debug)
	buginf("\nOSPF: End scanning, Elapsed time %ums",
	       runtime() - start_time);

}


/*
 * ospf_clear_redistribute
 * Reset ospf redistribution by flush all the external LSAs we have
 * originated.  Then set redistribute timer to re-evaluate the
 * routing table.
 */
void ospf_clear_redistribute (parseinfo *csb)
{
    pdbtype *pdb;
    areatype *area;

    for (pdb = ospf_pdbs_ptr; pdb ; pdb = pdb->ospf_next) {
	ospf_flush_type5(pdb);
	for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	    ospf_flush_lsa_inline(area, LSA_TYPE_7_ASE, is_self_originated_lsa,
				  NULL, DBASE_FREEZE);
	}
	ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE|OSPF_TRANSLATE_TYPE7);
    }
}


/*
 * ospf_summary_address
 * Function to service summary-address configuration for OSPF.
 * Set the timer to rescan the routing table and go.
 */

void ospf_summary_address (pdbtype *pdb, boolean sense)
{
    ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE|OSPF_TRANSLATE_TYPE7);
}

/*
 * ospf_set_summary_timer
 *
 * Set ospf_redist_summary_timer to start the routing table scanning for
 * redistribution.  Rate-limited to once every 10 secs.   
 * ospf_redist_summary_last_time is set to the current summary time in 
 * case this routine gets called when the scanning is being suspended, 
 * we won't schedule it to one second.   last_time will be reset again 
 * at the end of the scan.
 */
void ospf_set_summary_timer (pdbtype *pdb, ushort flags)
{
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    if (!TIMER_RUNNING(pdb->ospf_redist_summary_timer)) {
	if (CLOCK_IN_INTERVAL(pdb->ospf_redist_summary_last_time, 
			      (SCANNING_HOLD_TIME * ONESEC)))
	    pdb->ospf_redist_summary_timer =
		TIMER_ADD_DELTA(pdb->ospf_redist_summary_last_time,
				SCANNING_HOLD_TIME * ONESEC);
	else
	    TIMER_START(pdb->ospf_redist_summary_timer, ONESEC);

	COPY_TIMESTAMP(pdb->ospf_redist_summary_timer,
		       pdb->ospf_redist_summary_last_time);
    }
    ospf_pdb->ospf_set_summary_timer_reason |= flags;
}
/* end of ospf_lsa.c */
