/* $Id: ospf.c,v 3.9.8.15 1996/08/28 12:53:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf.c,v $
 *------------------------------------------------------------------
 * ospf.c -- Open Shortest Path First Interior Gateway Routing Protocol
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf.c,v $
 * Revision 3.9.8.15  1996/08/28  12:53:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.8.14  1996/08/16  22:10:38  myeung
 * CSCdi62650:  OSPF NSSA does not flush type 5 LSAs
 * Branch: California_branch
 *
 * Revision 3.9.8.13  1996/08/03  23:30:28  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.9.8.12  1996/07/19  07:26:51  myeung
 * CSCdi63444:  OSPF router should not be ABR if it dont see neighbor from
 * backbone
 * Branch: California_branch
 *
 * Revision 3.9.8.11  1996/06/28  23:19:02  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.8.10  1996/06/28  06:04:14  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.9.8.9  1996/06/18  01:47:47  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.9.8.8  1996/06/05  07:02:18  myeung
 * CSCdi58092:  ospf crash in get_interval when unconfiguring ospf
 * Branch: California_branch
 *
 * Revision 3.9.8.7  1996/05/23  07:37:45  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.9.8.6  1996/04/30  23:14:11  myeung
 * CSCdi56225:  OSPF type 7 LSA is not handled correctly
 * Branch: California_branch
 * - Fix request list for type 7 LSA
 * - Remove translated type 5 LSA when type 7 LSA is not reachable
 *
 * Revision 3.9.8.5  1996/04/23  06:48:19  myeung
 * CSCdi54958:  NSSA 7000 router crashed when doing no ip routing
 * Branch: California_branch
 *
 * Revision 3.9.8.4  1996/04/22  05:33:01  myeung
 * CSCdi48106:  OSPF fail to install parallel ex rt with forw addr through
 * sum rt
 * Branch: California_branch
 *
 * Revision 3.9.8.3  1996/04/16  19:01:37  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.9.8.2  1996/03/29  20:38:30  myeung
 * CSCdi51283:  ags+ crashes when unconfiguring ospf
 * Branch: California_branch
 *
 * Revision 3.9.8.1  1996/03/18  20:28:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.4  1996/03/16  06:57:20  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.12.3  1996/03/13  01:39:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.12.2  1996/03/07  09:49:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  01:03:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/29  10:33:00  myeung
 * CSCdi47402:  OSPF designated router (re)election causes unnecessary
 * route flap
 * - Keep track of the old DR and put a corresponding entry in router LSA
 * - Delay flushing of the old DR's network LSA
 *
 * Revision 3.8  1996/02/04  06:07:33  myeung
 * CSCdi48132:  Extract additional OSPF specific information out of pdbtype
 *
 * Revision 3.7  1996/02/01  06:05:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/25  12:39:52  myeung
 * CSCdi47498:  Clean up OSPF redistribution
 *
 * Revision 3.5  1996/01/05  10:17:28  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/08  04:55:46  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/11/18  20:34:36  myeung
 * CSCdi25568:  Router crash: at PC
 * 0x16BD88[_ospf_validate_header(0x16bc14)
 *
 * Revision 3.2  1995/11/17  17:35:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/09/25  08:31:06  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.8  1995/09/23  21:40:46  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.7  1995/08/30  09:19:59  myeung
 * CSCdi38044:  CPU HOG message
 *
 * Revision 2.6  1995/08/25  21:57:47  myeung
 * CSCdi33077:  OSPF: crash in db_find_lsa when doing "no router ospf"
 *
 * Revision 2.5  1995/06/28  03:21:20  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.4  1995/06/19 01:08:00  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/18  06:18:40  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/09 13:06:26  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:09:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Routines exported from this mode
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "../os/old_timer_callbacks.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "../os/free.h"
#include "../ui/debug.h"
#include "route.h"
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "route_inlines.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */
#include "../util/tree.h"
#include "ospf_pak.h"
#include "ospf_dbase.h"
#include "ospf.h"
#include "ospf_public.h"
#include "parser_defs_router.h"
#include "../util/md5.h"
#include "ospf_inlines.h"


/*
 * Forward declarations
 */

static void ospf_cleanup(pdbtype *);
static void ospf_router(pdbtype *);
static void ospf_hello_router(pdbtype *);

/*
 * Storage
 */

ushort ospf_running = FALSE;
boolean ospf_name_lookup = FALSE;
chunk_type *ospf_lsdb_list_chunks = NULL;
chunk_type *ospf_nbr_list_chunks = NULL;
chunk_type *ospf_work_chunks = NULL;


/*
 * ospf_build_backup_info
 *
 * Callback from network_update to insert context information in a
 * newly-created backup entry.
 */
static void ospf_build_backup_info (pdbtype *pdb, ndbtype *ndb,
				    backuptype *backup, ushort route_type,
				    boolean replaced)
{
    /*
     * If our route is being replaced, the context is the NDB area id.
     * Otherwise (we're installing a route and a route with a better
     * administrative distance is present) the context is the PDB area
     * id, which we presuably set just before calling network_update.
     * Heinous.
     */
    if (replaced) {
	backup->backup_context_info = ndb->ndb_area_id;
    } else {
	backup->backup_context_info = pdb->ospf_area_id;
    }
}

/*
 * ospf_ndb_init
 *
 * Initialize OSPF-specific NDB fields.
 */
static void ospf_ndb_init (pdbtype *pdb, ndbtype *ndb, boolean new_ndb)
{
    ndb->ndb_area_id = pdb->ospf_area_id;
    ndb->spf_instance = pdb->spf_instance;
    ndb->forw_dist = pdb->ospf_ex_forw_dist;
}

/*
 * pdb_ospf_BLOCK
 * routine to block ospf routing process when no input available or
 * some timer event needs some service.
 */
static boolean pdb_ospf_BLOCK (
    pdbtype *pdb)
{
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    /* Database locks */
    if ((pdb->ospf_dbase_lock == DBASE_ACTIVE)
	|| (pdb->ospf_dbase_lock == DBASE_FREEZE))
	return(TRUE);	

    /* Timer events */
    if (pdb->ospf_flag & (OSPF_SOFT_RESET | OSPF_IF_JUMPSTART
			  | OSPF_IMMEDIATE_SPF | OSPF_SCHD_SPF
			  | OSPF_FLUSH_DO_NOT_AGE | OSPF_CHECK_INDICATION
			  | OSPF_CHECK_ABR)) {
	return(FALSE);
    }

    if (pdb->ospf_flag & OSPF_MAXAGEQ)
	if (!(HOLD_TIMER_CHECK(pdb->ospf_maxage_delete_time, 
			       MAXAGEQ_SCAN_INTERVAL))) 
	    return(FALSE);
	    
    /*
     * Schedule the process if
     * 1) Need to flood some delayed LSAs
     * 2) Ospf Input packets are waiting.
     * 3) Need to add routes to routing table
     * 4) Need to process partial SPF 
     */
    if ((pdb->ospf_exdelayq->qhead) || (pdb->inputq->qhead) || 
	(pdb->add_requests) ||
	(!QUEUEEMPTY(&ospf_pdb->ospf_partial_sum_spfQ)) ||
	(!QUEUEEMPTY(&ospf_pdb->ospf_partial_nssa_spfQ)) ||
	(!QUEUEEMPTY(&ospf_pdb->ospf_partial_ex_spfQ))) {
	return(FALSE);
    }

    /* check if it is necessary to run SPF */
    if ((pdb->run_spf) && (SPF_HOLD_EXPIRED(pdb)))
	return(FALSE);			

    if (TIMER_RUNNING_AND_AWAKE(pdb->ospf_redist_summary_timer))
	return(FALSE);

    if (mgd_timer_expired(&ospf_pdb->ospf_update_timer))
	return(FALSE);

    return(TRUE);
} /* end of pdb_ospf_BLOCK */

/*
 * pdb_ospf_hello_BLOCK
 * routine to block ospf routing process when
 * no Hello input available.
 */
static boolean pdb_ospf_hello_BLOCK (
    pdbtype *pdb)
{
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    if (pdb->hello_inputq->qhead)
	return(FALSE);			/* input available */

    if (mgd_timer_expired(&ospf_pdb->ospf_hello_master_timer))
	return(FALSE);

    return(TRUE);
} /* end of pdb_ospf_hello_BLOCK */


/*
 * ospf_cleanup_routine
 * wrap routine for
 * ospf_clean
 */
static void ospf_cleanup_routine (
    pdbtype *pdb,
    uchar *null_ptr1,
    uchar *null_ptr2)
{
    ospf_event(OC_TIMER_EXPIRED, (ulong) "cleanup", 0);
    ospf_cleanup(pdb);
} /* end of ospf_cleanup_routine */


/*
 * ospf_rdb_init
 *
 * Initialize OSPF-specific RDB fields
 */
static void ospf_rdb_init (pdbtype *pdb, ndbtype *ndb, rdbtype *rdb)
{
    rdb->rdb_flags |= IP_RDB_OSPF;
}

/*
 * ospf_rdb_cleanup
 *
 * Clean up an OSPF RDB that's about to be freed
 */
static void ospf_rdb_cleanup (pdbtype *pdb, rdbtype *rdb)
{
    rdb->rdb_flags &= ~IP_RDB_OSPF;
    rdb->ospf_rdb_lsdb = NULL;
    rdb->ospf_rdb_area = NULL;
}

/*
 * ospf_distance
 * Return the admin distance according to route type.
 */
int ospf_distance (pdbtype *pdb, ipaddrtype target, ushort route_type, 
		   int flags, int *pdb_peerindex)
{
    if (route_type == RTYPE_INTRA) 
	return (pdb->distance);
    else if (route_type == RTYPE_INTER)
	return (pdb->distance2);
    else
	return (pdb->distance3);
}

/*
 * ospf_pdb_init
 * Initialize an OSPF Protocol Descriptor Block
 */
pdbtype *ospf_pdb_init (
    ushort asystem)
{
    pdbtype *pdb;
    ipaddrtype rtr_id;
    int i, index;
    ospf_pdb_info *ospf_pdb;

    /* Create some nice free lists. */

    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(lsdbtype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(ex_lsatype));

    /* Create the event queue. */

    ospf_init_events();

    /*
     * create the pdb.
     */
    pdb = malloc(sizeof(pdbtype));
    if (pdb == NULL) {
	return(NULL);
    }

    ospf_pdb = malloc(sizeof(ospf_pdb_info));
    if (!ospf_pdb) {
	free(pdb);
	return(NULL);
    }
    pdb->ospf_pdb = ospf_pdb;

    /*
     * Get an unused index and flag for this
     * process.
     */
    if (allocate_pdbindex(pdb) == -1) {
	free(pdb);
	free(ospf_pdb);
	return(NULL);
    }

    /*
     * Allocate memory for input queue
     */
    pdb->inputq = malloc(sizeof(queuetype));
    if (pdb->inputq == NULL)
	goto pdb_error;

    pdb->hello_inputq = malloc(sizeof(queuetype));
    if (pdb->hello_inputq == NULL)
	goto pdb_error;
    
    pdb->ospf_faddrq = malloc(sizeof(queuetype));
    if (pdb->ospf_faddrq == NULL)
	goto pdb_error;

    pdb->mib_table = malloc(4 * OSPF_PDB_MIB_COUNT );

    pdb->name = "ospf";
    pdb->family = PDB_IP;
    pdb->pname = "OSPF Router";
    pdb->hello_pname = "OSPF Hello";
    pdb->process = ospf_router;
    pdb->ospf_scanpid = 0;
    pdb->ospf_scantime = OSPF_SCAN_INTERVAL;
    pdb->ospf_hello_process = ospf_hello_router;
    pdb->cleanup = ospf_cleanup;
    pdb->neighbor = ospf_neighbor;	/* to configure neighbors */
    pdb->area = ospf_area;		/* to configure area */
    pdb->query = ospf_force_spf;
    pdb->redistribute = ospf_redistribute;
    pdb->add_route = ospf_rcv_add_req;
    pdb->build_backupinfo = ospf_build_backup_info;
    pdb->ndb_init = ospf_ndb_init;
    pdb->network_cb = ospf_network_command;
    pdb->malloc_backup = TRUE;
    pdb->asystem = asystem;
    pdb->proctype = PROC_OSPF;
    pdb->distflag = pdb->mask;
    pdb->redistallowed = PROC_REDISTMASK;
    pdb->distance = pdb->def_distance = OSPF_DISTANCE;
    pdb->distance2 = pdb->def_distance2 = OSPF_DISTANCE;
    pdb->distance3 = pdb->def_distance3 = OSPF_DISTANCE;
    pdb->distance_cmp = iprouting_distcmp_generic;
    pdb->distance_cb = ospf_distance;
    pdb->multipath = pdb->def_multipath = DEFMAXROUTES;
    pdb->ospf_next = NULL;
    pdb->atomic_update = TRUE;
    pdb->holddown = (holddowncbtype)NULL;
    pdb->holdokay = FALSE;
    pdb->rdb_init = ospf_rdb_init;
    pdb->rdb_cleanup = ospf_rdb_cleanup;
    pdb->ospf_default_type = METRIC_TYPE2;
    pdb->ospf_auto_cost = TRUE;
    ospf_pdb->ospf_reference_bw = OSPF_DEF_REFERENCE_BW;
    /* we do not support more than one TOS */
    pdb->ospf_tos_option = 0;
    pdb->ospf_dbase_lock = DBASE_INACTIVE;
    pdb->ospf_spf_schd_delay = SPF_SCHD_DELAY;
    pdb->ospf_spf_hold_interval = SPF_HOLD_INTERVAL;
    /* 
     * Initial condition for scheduling spf
     * - run_spf is FALSE
     * - SPF_HOLD_EXPIRED is TRUE
     */
    pdb->run_spf = FALSE;
    pdb->spf_wait_interval = 0;
    GET_TIMESTAMP(pdb->spf_time);
    /*
     * fill the default map metric for each
     * routing process. This will be used during
     * redistribution if configured.
     */
    for (i = 2; i < MAX_IPPROCS; i++) {
	pdb->metric[i].metric.scalar = DEFAULT_MAP_METRIC;
	pdb->metric[i].type = DEFAULT_METRIC_TYPE;
	pdb->metric_def[i].metric.scalar = DEFAULT_MAP_METRIC;
	pdb->metric_def[i].type = DEFAULT_METRIC_TYPE;
    }
    index = pdb_CONNECTED->index;
    pdb->metric[index].metric.scalar = DEFAULT_MAP_METRIC_CONNECTED;
    pdb->metric_def[index].metric.scalar = DEFAULT_MAP_METRIC_CONNECTED;
    pdb->metric[index].type = DEFAULT_METRIC_TYPE;
    pdb->metric_def[index].type = DEFAULT_METRIC_TYPE;

    index = pdb_STATIC->index;
    pdb->metric[index].metric.scalar = DEFAULT_MAP_METRIC_CONNECTED;
    pdb->metric_def[index].metric.scalar = DEFAULT_MAP_METRIC_CONNECTED;
    pdb->metric[index].type = DEFAULT_METRIC_TYPE;
    pdb->metric_def[index].type = DEFAULT_METRIC_TYPE;
    /* 
     * allocate an unused largest interface ip
     * address as OSPF router id.
     */
    rtr_id = allocate_rtr_id(TRUE);
    if (!rtr_id) {
	printf("\nOSPF: Could not allocate router id");
	goto pdb_error;
    }
    pdb->ospf_exdelayq = malloc(sizeof(queuetype));
    if (pdb->ospf_exdelayq == NULL)
	goto pdb_error;

    /*
     * create ospf timers for this process 
     */
    mgd_timer_init_parent(&ospf_pdb->ospf_hello_master_timer, NULL);
    mgd_timer_init_parent(&ospf_pdb->ospf_update_timer, NULL);
    mgd_timer_init_leaf(&ospf_pdb->ospf_sumupdate_timer,
	       		&ospf_pdb->ospf_update_timer,
			OSPF_SUMUPDATE_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&ospf_pdb->ospf_exupdate_timer,
			&ospf_pdb->ospf_update_timer,
			OSPF_EXUPDATE_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&ospf_pdb->ospf_exlock_timer,
			&ospf_pdb->ospf_update_timer,
			OSPF_EXLOCK_TIMER, NULL, FALSE);
    mgd_timer_init_leaf(&ospf_pdb->ospf_faddr_timer,
			&ospf_pdb->ospf_update_timer,
			OSPF_FADDR_TIMER, NULL, FALSE);
    ospf_pdb->ospf_clean_timer = create_timer(ospf_cleanup_routine,
					      pdb, NULL, NULL, "OSPF cleanup",
					      TMT_ONCE);
    if (ospf_pdb->ospf_clean_timer == NULL) {
 	buginf("No memory to create Faddress timer\n");
	goto pdb_error;
    }

    pdb->ospf_ex_lsdbase = RBTreeCreate("OSPF", "OSPF", "External LSA", NULL,
					&ospf_debug_tree);

    if (pdb->ospf_ex_lsdbase == NULL) {
	buginf("\nOSPF: No memory for Link State Database");
	goto pdb_error;
    }

    /*
     * Create a dummy area which is used whenever an area
     * parameter is needed for external LSA. We use
     * this dummy area pointer instead of ospf_area_list,
     * which can be NULL in some corner cases and
     * causes crash.
     */
    ospf_pdb->ospf_dummy_area = malloc(sizeof(areatype));
    if (!ospf_pdb->ospf_dummy_area)
      goto pdb_error;
    ospf_pdb->ospf_dummy_area->ar_pdb_ptr = pdb;
    ospf_pdb->ospf_dummy_area->ar_lsdbase[5] = pdb->ospf_ex_lsdbase;
    sprintf(ospf_pdb->ospf_dummy_area->ar_id_str, "dummy area");

    pdb->ospf_routes = malloc(HTBLSIZE*(sizeof(rtr_route *)));
    if (!pdb->ospf_routes)
	goto pdb_error;

    /*
     * Create chunk memory shared among OSPF processes
     */
    if (!ospf_lsdb_list_chunks) {
	ospf_lsdb_list_chunks = chunk_create(sizeof(lsdb_list),
					     OSPF_LSDB_LIST_CHUNKSIZE,
					     CHUNK_FLAGS_DYNAMIC,
					     NULL, 0, "OSPF lsdb list");
	if (!ospf_lsdb_list_chunks)
	    goto pdb_error;
    }
    if (!ospf_nbr_list_chunks) {
	ospf_nbr_list_chunks = chunk_create(sizeof(nbr_list),
					    OSPF_NBR_LIST_CHUNKSIZE,
					    CHUNK_FLAGS_DYNAMIC,
					    NULL, 0, "OSPF nbr list");
	if (!ospf_nbr_list_chunks)
	    goto pdb_error;
    }
    if (!ospf_work_chunks) {
	ospf_work_chunks = chunk_create(sizeof(ospf_worktype),
					OSPF_WORK_CHUNKSIZE,
					CHUNK_FLAGS_DYNAMIC,
					NULL, 0, "OSPF work entries");
	if (!ospf_work_chunks)
	    goto pdb_error;
    }

    /* 
     * Set up remaining fields
     */
    pdb->ospf_rtr_id = rtr_id;
    pdb->ospf_area_list = NULL;
    pdb->ospf_area_brtr = FALSE;	/* not an area border router */
    pdb->ospf_as_brtr = FALSE;		/* not an as boundary router */
    pdb->ospf_max_headptr = NULL;
    pdb->ospf_max_tailptr = NULL;
    
    /*
     * Insert this pdb in global and ospf pdb queue
     */
    enqueue(&protoQ, pdb);
    if (ospf_pdbs_ptr)
	pdb->ospf_next = ospf_pdbs_ptr;
    ospf_pdbs_ptr = pdb;
    return(pdb);
  pdb_error:
    free_pdbindex(pdb);
    free(pdb->inputq);
    free(pdb->hello_inputq);
    free(pdb->ospf_faddrq);
    free(pdb->mib_table);
    free(pdb->ospf_exdelayq);
    destroy_timer(ospf_pdb->ospf_clean_timer);
    free(ospf_pdb->ospf_dummy_area);
    RBTreeDestroy(pdb->ospf_ex_lsdbase, &ospf_debug_tree);
    free(pdb->ospf_routes);
    free(pdb->ospf_pdb);
    if (!ospf_pdbs_ptr) {
	chunk_destroy(ospf_lsdb_list_chunks);
	chunk_destroy(ospf_nbr_list_chunks);
	chunk_destroy(ospf_work_chunks);
    }
    free(pdb);
    return(NULL);
} /* end of ospf_pdb_init */


/*
 * ospf_clean_area
 * deallocates all the memory used by
 * an area.
 */
void ospf_clean_area (areatype *area)
{
    lsdbtype *loop_db, *db;
    addr_range *range, *next_range;
    ushort type;
    idbtype *idb;
    idbtype *temp;
    int count = 0;
    pdbtype *pdb = area->ar_pdb_ptr;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_event(OC_CLEAN_AREA, area->ar_id, 0);
    for (idb = area->ar_if_list; idb ; idb = temp) {
	temp = idb->ospf_info->next;
	ospf_clean_if(idb);
    }
    for (range = area->ar_range_list; range; range = next_range) {
	next_range = range->net_next;
	range->net_next = NULL;
	free(range);
    }
    area->ar_range_list = NULL;

    mgd_timer_stop(&area->ar_master_timer);
    mgd_timer_stop(&area->ar_hello_master_timer);

    /*
     * Clean up of partial summary and type 7 SPF scheduled for this area
     */
    ospf_clean_partial_spfQ(area, &ospf_pdb->ospf_partial_sum_spfQ);
    ospf_clean_partial_spfQ(area, &ospf_pdb->ospf_partial_nssa_spfQ);

    /* grab all of the links  */
    for (type = 0; type < LSA_TYPE_ASE; type++) {
        loop_db = db_get_first_db(area->ar_lsdbase[type]);
        while (loop_db) {
	    check_ospf_suspend_limit(area->ar_pdb_ptr, DBASE_FREEZE, &count);
	    db = loop_db;
	    loop_db = db_get_next_db(loop_db);
	    db->db_ackcnt = 0;
	    ospf_delete_maxage(area->ar_pdb_ptr, db);
	    db_free(area->ar_pdb_ptr, db);
	}
        RBTreeDestroy(area->ar_lsdbase[type], &ospf_debug_tree);
	area->ar_lsdbase[type] = NULL;
    } 
    area->ar_dcbitless_lsa_count = 0;
    area->ar_indication_lsa_count = 0;
    area->ar_do_not_age_lsa_count = 0;
} /* end of ospf_clean_area */


/*
 * ospf_cleanup
 * Here to cleanup after an OSPF router has been shutdown.
 * Close areas and release pdb storage.
 */
static void ospf_cleanup (pdbtype *pdb)
{
    areatype *area;
    areatype *temp;
    pdbtype *tpdb;
    lsdbtype *loop_db, *db;
    rtr_route *route;
    rtr_route *tem;
    paktype *pak;
    ospfbackuptype *backup, *next_entry;
    int count = 0;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_event(OC_CLEANUP, 0, 0);
    if ((pdb->ospf_dbase_lock == DBASE_ACTIVE)
	|| (pdb->ospf_dbase_lock == DBASE_FREEZE)) {
	if (!(active_timer(ospf_pdb->ospf_clean_timer)))
	    set_timer(ospf_pdb->ospf_clean_timer, 5 * ONESEC);
	return;
    }
    INIT_OSPF_SUSPEND(pdb);
    unqueue(&protoQ, pdb);

    /*
     * Kill processes first so that they will not deal
     * with freed structure.
     * Note : Don't suspend before killing processes.
     *        On the other hand, the OSPF processes should
     *        not suspend while it is still holding any queue
     *        element. Otherwise, that element will be lost.
     */
    if (pdb->hello_pid) {
	process_kill(pdb->hello_pid);
	pdb->hello_pid = 0;
	if(pdb->ospf_scanpid)
	    process_kill(pdb->ospf_scanpid);
	pdb->ospf_scanpid = 0;
    }

    /*
     * Indicate that OSPF is cleaning up, free link state database 
     * unconditionally
     */
    pdb->ospf_flag |= OSPF_CLEANING_UP;

    /* clean area structures */
    while ((pak = dequeue(pdb->inputq))) {
	retbuffer(pak);
    }
    free(pdb->inputq);
    pdb->inputq = NULL;
    while ((pak = dequeue(pdb->hello_inputq))) {
	retbuffer(pak);
    }
    free(pdb->hello_inputq);
    pdb->hello_inputq = NULL;
    check_ospf_suspend(pdb, DBASE_FREEZE);
    ospf_flush_type5(pdb);
    /*
     * all db is taken off the maxage queue, flag on pdb is reset
     */
    ospf_destroy_maxage(pdb);

    ospf_clean_partial_spfQ(NULL, &ospf_pdb->ospf_partial_sum_spfQ);
    ospf_clean_partial_spfQ(NULL, &ospf_pdb->ospf_partial_nssa_spfQ);
    ospf_clean_partial_spfQ(NULL, &ospf_pdb->ospf_partial_ex_spfQ);

    for (area = pdb->ospf_area_list; area; area = temp) {
	ospf_flush_area(area);
	ospf_clean_area(area);
	temp = area->ar_next;
	pdb->ospf_area_list = temp;
	if (area->ar_id == BACKBONE_AREA_ID)
	    pdb->ospf_backbone = NULL;
	area->ar_next = NULL;
	free(area);
    }

    pdb->area_count = 0;
    ospf_pdb->ospf_stub_area_count = 0;
    ospf_pdb->ospf_nssa_area_count = 0;
    ospf_pdb->ospf_normal_area_count = 0;

    /*
     * free all add requests from
     * global routing table 
     */
    for (backup = pdb->add_requests; backup;
	 backup = next_entry){
	next_entry = backup->next;
	free(backup);
    }
    mgd_timer_stop(&ospf_pdb->ospf_update_timer);
    mgd_timer_stop(&ospf_pdb->ospf_hello_master_timer);

    ospf_destroy_exdelayq(pdb);
    ospf_destroy_faddrq(pdb);
    free(pdb->ospf_faddrq);
    free(pdb->ospf_exdelayq);
    destroy_timer(ospf_pdb->ospf_clean_timer);
    ospf_pdb->ospf_clean_timer = NULL;


    loop_db = db_get_first_db(pdb->ospf_ex_lsdbase);
    while (loop_db) {
        check_ospf_suspend_limit(pdb, DBASE_FREEZE, &count);
        db = loop_db;
        loop_db = db_get_next_db(loop_db);
        db->db_ackcnt = 0;
	ospf_delete_maxage(pdb, db);
        db_free(pdb, db);
    }
    RBTreeDestroy(pdb->ospf_ex_lsdbase, &ospf_debug_tree);
    pdb->ospf_ex_lsdbase = NULL;
    free(ospf_pdb->ospf_dummy_area);
    ospf_pdb->ospf_dcbitless_lsa_count = 0;
    ospf_pdb->ospf_do_not_age_lsa_count = 0;

    route = (rtr_route *)pdb->ospf_routes;
    for ( ; route; route = tem) {
	tem = route->rt_next;
	free(route);
    }
    if (ospf_pdbs_ptr == pdb)
	ospf_pdbs_ptr = pdb->ospf_next;
    else {
	tpdb = ospf_pdbs_ptr;
	for (;tpdb->ospf_next; tpdb = tpdb->ospf_next) {
	    if (tpdb->ospf_next == pdb) {
		tpdb->ospf_next = pdb->ospf_next;
	        break;
	    }
	}
    }
    if (!ospf_pdbs_ptr) {
	ospf_running = FALSE;
	chunk_destroy(ospf_lsdb_list_chunks);
	chunk_destroy(ospf_nbr_list_chunks);
	chunk_destroy(ospf_work_chunks);
	ospf_lsdb_list_chunks = NULL;
	ospf_nbr_list_chunks = NULL;
	ospf_work_chunks = NULL;
    }
    free_pdbindex(pdb);
    pdb->ospf_next = NULL;
    free(pdb);
} /* end of ospf_clean */


/*
 * ospf_soft_reset
 * Here to cleanup after an OSPF router ID is
 * removed from system.
 */
boolean ospf_soft_reset (pdbtype *pdb)
{
    areatype *area;
    idbtype *temp, *idb;
    paktype *pak;
    int i;
    ipaddrtype address;
    ipaddrtype area_id;
    ipaddrtype mask;
    ospfbackuptype *backup, *next_entry;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ospf_event(OC_GENERIC, (ulong) "ospf_soft_reset", 0);

    /*
     * if virtual links are configured
     * in this router, don't bother changing
     * the router id. 
     */
    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	if (area->ar_vlink)
	    return(FALSE);
    }

    /*
     * Indicate that OSPF router ID is changing, sothat this
     * pdb can save necessary config info to restart this pdb.
     */
    pdb->ospf_flag |= OSPF_ID_CHANGING;

    while ((pak = dequeue(pdb->inputq))) {
	retbuffer(pak);
    }
    ospf_flush_type5(pdb);
    ospf_clean_partial_spfQ(NULL, &ospf_pdb->ospf_partial_sum_spfQ);
    ospf_clean_partial_spfQ(NULL, &ospf_pdb->ospf_partial_nssa_spfQ);
    ospf_clean_partial_spfQ(NULL, &ospf_pdb->ospf_partial_ex_spfQ);

    for (area = pdb->ospf_area_list; area; area = area->ar_next) {
	ospf_flush_area(area);
	for (idb = area->ar_if_list; idb ; idb = temp) {
	    temp = idb->ospf_info->next;
	    ospf_clean_if(idb);
	}
    }

    check_ospf_suspend (pdb, DBASE_FREEZE);
    /*
     * free all add requests from
     * global routing table 
     */
    for (backup = pdb->add_requests; backup;
	 backup = next_entry){
	next_entry = backup->next;
	free(backup);
    }
    pdb->add_requests = NULL;
    ospf_destroy_faddrq(pdb);
    ospf_destroy_exdelayq(pdb);
    clean_router_route(pdb);

    /*
     * We have to set router id to zero first otherwise the
     * uniqueness test in allocate_rtr_id will fail for the
     * calling pdb current router id and force an unnecessary
     * router id change.
     */
    pdb->ospf_rtr_id = 0;
    pdb->ospf_rtr_id = allocate_rtr_id(TRUE);
    if (!pdb->ospf_rtr_id) {
	errmsg(&msgsym(NORTRID, OSPF));
    }
    for (i = 0; i < pdb->netcount; i++) {
	address = pdb->networks[i];
	mask = pdb->network_mask[i];
	area_id = pdb->network_info[i];
	ospf_parse_range(pdb, address, mask, area_id, 
                         OSPF_AREA_ID_FORMAT_IGNORE, TRUE);
    }
    pdb->ospf_flag &= ~OSPF_ID_CHANGING;
    ospf_pdb_check(pdb);
    check_ospf_suspend (pdb, DBASE_ACTIVE);
    return(TRUE);

} /* end of ospf_soft_reset */


/*
 * ospf_validate_header
 *
 * Do basic validation on incoming packets.
 *
 * returns TRUE if header is valid
 *
 * returns pointers to OSPF header and area structure if valid
 */
static boolean ospf_validate_header (paktype *pak, ospftype **ospf_ret,
				     areatype **area_ret)
{
    idbtype *idb_ptr = pak->if_input;
    vlinktype *vlink;
    iphdrtype *ip;
    ospftype *ospf;
    uchar auth[OSPF_AUTH_LEN];
    areatype *area;
    uchar *ospf_trailer, ospf_digest[MD5_LEN];
    ospf_idb_info *ospf_info = idb_ptr->ospf_info;
    ospf_md_key *auth_key;
    MD5_CTX mdcontext;
    ulong seq_num;
    nbrtype *nbr;

    vlink = NULL;
    ip   = (iphdrtype *) ipheadstart(pak);
    ospf = (ospftype *)  ipdatastart(pak);
    *ospf_ret = ospf;
    
    if (!ospf_info->enabled) {
	/*
	 * This test is used to safeguard the race condition in which 
	 * ospf idb information is cleared and we happen to process 
	 * packet for that interface after the cleanup.
	 * Possible cause of the race condition is change of network 
	 * command, ip address and router id. 
	 */
	errmsg(&msgsym(ERRRCV, OSPF), "OSPF not enabled on interface", 
		     ip->srcadr, idb_ptr->namestring);
	return(FALSE);
    }
	
    /* Complain if the length is bogus. */
    
    if (ospf->ospf_length < OSPF_HDR_SIZE || 
	(ospf->ospf_length + (ip->ihl << 2)) > ip->tl) {
	
	errmsg(&msgsym(BADLENGTH, OSPF), ospf->ospf_length,
		     ip->srcadr, ospf->ospf_rtr_id, idb_ptr->namestring);
	return(FALSE);
    }
    
    /*
     * Checksum datagram
     * checksum is done with auth field
     * set to zero.
     * For message digest authentication, checksum is not calculated 
     */
    if (ospf->ospf_autype != OSPF_AUTH_MD) {
        bcopy(ospf->ospf_auth.auth, auth, OSPF_AUTH_LEN);
	memset(ospf->ospf_auth.auth, 0, OSPF_AUTH_LEN);
	if (ipcrc((ushort *)ospf, ospf->ospf_length)) {
	    errmsg(&msgsym(ERRRCV, OSPF), "Bad Checksum", 
		   ip->srcadr, idb_ptr->namestring);
	    ospf_add_bad_checksum_queue(pak);
	    return(FALSE);
	}
    }

    /*
     * Check version number.
     */
    if (ospf->ospf_version != OSPF_VERSION) {
	errmsg(&msgsym(ERRRCV, OSPF), "Bad Version", 
		     ip->srcadr, idb_ptr->namestring);
	return(FALSE);
    }

    /*
     * Validate area number.  If the area ID matches the interface
     * area ID, it must have been received from a neighbor on the
     * same subnet.  If not, it must be the backbone area, in which
     * case we must be an area border router, and it must have been
     * received over a virtual link.
     */
    area = ospf_info->area_ptr;

    if (ospf->ospf_area_id == area->ar_id) { /* Our area */

	/* Make sure that the packet is sent over single hop */

	if (!is_p2p(idb_ptr) &&
	    ((idb_ptr->ip_address & idb_ptr->ip_nets_mask) !=
	     (ip->srcadr & idb_ptr->ip_nets_mask))) {
 	    ospf_adj_buginf("\nOSPF: Rcv pkt from %i, %s, area %i : "
 			    "src not on the same network",
 			    ip->srcadr, idb_ptr->namestring, area->ar_id);
	    return(FALSE);
	}

    } else { /* it must be a virtual link */

	if (ospf->ospf_area_id == 0) {	/* Better be the backbone area */

	    /*
	     * check if this packet is from 
	     * vlink neighbor on this interface
	     */
	    vlink = (vlinktype *)area->ar_vlink;
	    for (; vlink; vlink = vlink->vl_next) {
		if ((ospf->ospf_rtr_id == vlink->vl_nbr_id)) 
		    break;
	    }
	    if (vlink) {

		/* Found the virtual link.  Update the area and interface. 
		 */
		area = vlink->vl_backbone;
		pak->if_input = vlink->vl_idb;
		ospf_info = pak->if_input->ospf_info;
	    } else {			/* No virtual link. */

		errmsg(&msgsym(ERRRCV, OSPF), 
 		       "mismatch area ID, from backbone area "
 		       "must be virtual-link but not found",
 		       ip->srcadr, idb_ptr->namestring);
		return(FALSE);
	    }

	} else {			/* Some other area. */

 	    ospf_adj_buginf("\nOSPF: Rcv pkt from %i, %s, area %i"
 			    "\n      mismatch area %i in the header",
 			    ip->srcadr, idb_ptr->namestring,
 			    area->ar_id, ospf->ospf_area_id);
	    return(FALSE);
	}
    }

    nbr = ospf_find_nbr(idb_ptr, ospf->ospf_rtr_id, ip->srcadr, FALSE);
    /*
     * Check authentication.  If authentication is configured in the
     * area, validate it.
     */
    if (area->ar_autype) {		/* Authentication is on */
	if (ospf->ospf_autype == area->ar_autype) { /* Proper type */
	    switch (ospf->ospf_autype) {
	        case OSPF_AUTH_CT:
		    if (bcmp(auth, ospf_info->auth_key, 
			     OSPF_AUTH_LEN)) {
		        ospf_adj_buginf("\nOSPF: Rcv pkt from %i, %s : "
					"Mismatch Authentication Key - "
					"Clear Text",
					ip->srcadr, idb_ptr->namestring);
			return(FALSE);
		    }
		    break;
		case OSPF_AUTH_MD:
		    auth_key = ospf_get_md_key(ospf_info, 
					       ospf->ospf_auth.auth2.keyid);
		    ospf_trailer = (uchar *) ospf + ospf->ospf_length;
		    bcopy(ospf_trailer, ospf_digest, MD5_LEN);
		    if (auth_key) {
		        /*
			 * Neighbor has a valid key
			 */
		        bcopy(auth_key->key, ospf_trailer, MD5_LEN);
			if (ospf_info->old_md_nbr_count && nbr) {
			    /*
			     * Rollover is in progress and we know this
			     * neighbor, check if this neighbor just
			     * switch to the new key.
			     */
			    if (auth_key == ospf_info->youngest_md_key) {
			        if (nbr->nbr_message_digest_keyid != 
				    auth_key->id) { 
			            ospf_info->old_md_nbr_count--;
				    nbr->nbr_message_digest_keyid = 
				      auth_key->id;
				}
			    }
			}
		    } else if (!ospf->ospf_auth.auth2.keyid && 
			       !ospf_info->youngest_md_key) {
		        /*
			 * The input key id is 0 and we do not have
			 * any key configured, proceed as if we
			 * have a key id 0 with blank key configured.
			 */
		        memset(ospf_trailer, 0, MD5_LEN);
		    } else {
		        ospf_adj_buginf("\nOSPF: Rcv pkt from %i, %s : "
					"Mismatch Authentication Key - "
					"No message digest key %u "
					"on interface",
					ip->srcadr, idb_ptr->namestring,
					ospf->ospf_auth.auth2.keyid);
			return FALSE;
		    }
		    MD5Init(&mdcontext);
		    MD5Update(&mdcontext, (uchar *) ospf, 
			      ospf->ospf_length + MD5_LEN);
		    MD5Final(ospf_trailer, &mdcontext);
		    if (bcmp(ospf_trailer, ospf_digest, MD5_LEN)) {
		        ospf_adj_buginf("\nOSPF: Rcv pkt from %i, %s : "
					"Mismatch Authentication Key - "
					"Message Digest Key %u",
					ip->srcadr, idb_ptr->namestring,
					ospf->ospf_auth.auth2.keyid);
			return(FALSE);
		    }
		    seq_num = 0;
		    if (vlink) {
		        seq_num = vlink->vl_message_digest_seq;
		    } else {			 
			if (nbr && (nbr->nbr_state > NBR_ATTEMPT))
			    seq_num = nbr->nbr_message_digest_seq;
		    }
		    if (ospf->ospf_auth.auth2.seq_num < seq_num) {
		        ospf_adj_buginf("\nOSPF: Rcv pkt form %i, %s : "
					"Invalid Sequence Number - "
					"Message Digest. "
					"Last Rcv %#x Now Rcv %#x",
					ip->srcadr, idb_ptr->namestring,
					seq_num, 
					ospf->ospf_auth.auth2.seq_num);
			return(FALSE);
		    }
		    break;
	    }		    
	} else {			/* Wrong type */
 	    ospf_adj_buginf("\nOSPF: Rcv pkt from %i, %s : "
 			    "Mismatch Authentication type. "
			    "Input packet specified type %d, "
			    "we use type %d",
			    ip->srcadr, idb_ptr->namestring, 
			    ospf->ospf_autype,
			    area->ar_autype);
	    return(FALSE);
	}
    }

    /* Whew.  Made it all the way through. */

    *area_ret = area;
    return(TRUE);
}

static void ospf_print_packet (idbtype *idb, ospftype *ospf)
{
    switch (ospf->ospf_autype) {
        case OSPF_AUTH_NONE:
        case OSPF_AUTH_CT:
            buginf("\nOSPF: rcv. v:%d t:%d l:%d rid:%i"
		   "\n      aid:%i chk:%x aut:%d auk:%s from %s",
		   ospf->ospf_version, ospf->ospf_type,
		   ospf->ospf_length, ospf->ospf_rtr_id,
		   ospf->ospf_area_id, ospf->ospf_checksum,
		   ospf->ospf_autype, 
		   ospf->ospf_auth.auth,
		   idb->namestring);
	    break;
	case OSPF_AUTH_MD:
	    buginf("\nOSPF: rcv. v:%d t:%d l:%d rid:%i"
		   "\n      aid:%i chk:%x aut:%d keyid:%u seq:%#x from %s",
		   ospf->ospf_version, ospf->ospf_type,
		   ospf->ospf_length, ospf->ospf_rtr_id,
		   ospf->ospf_area_id, ospf->ospf_checksum,
		   ospf->ospf_autype, 
		   ospf->ospf_auth.auth2.keyid,
		   ospf->ospf_auth.auth2.seq_num,
		   idb->namestring);
	    break;
    }
}

/*
 * ospf_backbone_active
 * Return TRUE if the backbone is active. If check_nbr is TRUE, we
 * require that neighbor must exist.
 */
inline boolean ospf_backbone_active (areatype *backbone, boolean check_nbr) {
    idbtype *idb;
    boolean backbone_active;

    backbone_active = FALSE;
    if (backbone) {
 	/* check if backbone is active */
	for (idb = backbone->ar_if_list; idb; idb = idb->ospf_info->next) {
	    if (ospf_interface_up(idb) && 
		(!check_nbr || (idb->ospf_info->nbrcnt > 0))) {
		backbone_active = TRUE;
		break;
	    }
	}
    } 
    return (backbone_active);
}

/*
 * ospf_check_if_abr
 *
 * checks if this router can become area border router
 * or should stop being area border router.
 */
static void ospf_check_if_abr (pdbtype *pdb)
{
    boolean backbone_active;

    backbone_active = ospf_backbone_active(pdb->ospf_backbone, FALSE);
    if (pdb->ospf_area_brtr) {
	if ((!backbone_active) || (pdb->area_count < MIN_ABR_AREA_COUNT))
	    stop_sum_brtr(pdb);
    } else {
	if ((pdb->area_count >= MIN_ABR_AREA_COUNT) && (backbone_active))
	    start_sum_brtr(pdb);
    }
} /* end of ospf_check_if_abr */

/*
 * ospf_router_process_mgd_timers
 * Process managed timer event for OSPF router process
 */
static void ospf_router_process_mgd_timers (pdbtype *pdb)
{
    areatype *area;
    list_queue flood_queue;
    mgd_timer *expired_timer;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    while ((expired_timer = 
	    mgd_timer_first_expired(&ospf_pdb->ospf_update_timer))) {
	switch (mgd_timer_type(expired_timer)) {
	case OSPF_SUMUPDATE_TIMER:
	    mgd_timer_stop(expired_timer);
	    pdb_sumupdate(pdb);
	    break;
	case OSPF_EXUPDATE_TIMER:
	    mgd_timer_stop(expired_timer);
	    pdb_exupdate(pdb);
	    break;
	case OSPF_EXLOCK_TIMER:
	    mgd_timer_stop(expired_timer);
	    pdb_delayed_ex_lsa(pdb);
	    break;
	case OSPF_FADDR_TIMER:
	    mgd_timer_stop(expired_timer);
	    ospf_ex_faddr(pdb);
	    break;
	case OSPF_AR_REBUILD_RTR_TIMER:
	    area = mgd_timer_context(expired_timer);
	    if (!area)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    flood_queue.q_first = NULL;
	    flood_queue.q_last = NULL;
	    build_rtr_lsa(area, &flood_queue, FALSE);
	    flood_area(area, &flood_queue, NULL, NULL);
	    break;
	case OSPF_AR_EXLOCK_TIMER:
	    mgd_timer_stop(expired_timer);
	    area = mgd_timer_context(expired_timer);
	    if (!area)
		goto timer_error;
	    ar_delayed_ex_lsa(area);
	    break;
	case OSPF_AR_SUMLOCK_TIMER:
	    mgd_timer_stop(expired_timer);
	    area = mgd_timer_context(expired_timer);
	    if (!area)
		goto timer_error;
	    ar_delayed_sum_lsa(area);
	    break;
	case OSPF_AR_DELAY_TIMER:
	    mgd_timer_stop(expired_timer);
	    area = mgd_timer_context(expired_timer);
	    if (!area)
		goto timer_error;
	    ar_delayed_rtr_lsa(area);
	    break;		    
	case OSPF_AR_UPDATE_TIMER:
	    mgd_timer_stop(expired_timer);
	    area = mgd_timer_context(expired_timer);
	    if (!area)
		goto timer_error;
	    ar_update_lsa(area);
	    break;		    
	case OSPF_AR_AGE_TIMER:
	    mgd_timer_stop(expired_timer);
	    area = mgd_timer_context(expired_timer);
	    if (!area)
		goto timer_error;
	    ar_age_dbase(area);
	    break;
	default:
	    buginf("\nOSPF: Unrecognized timer %d", 
		   mgd_timer_type(expired_timer));
	timer_error:
	    mgd_timer_stop(expired_timer);
	}
	check_ospf_suspend(pdb, DBASE_ACTIVE);
    }
}

/*
 * ospf_router
 * Main OSPF routing process.
 */
static void ospf_router (register pdbtype *pdb)
{
    paktype *pak;
    iphdrtype *ip;
    ospftype *ospf;
    areatype *area;
    ospf_pdb_info *ospf_pdb;

    edisms((blockproc *)systeminitBLOCK,(ulong)pdb->inputq);
    ospf_running = TRUE;
    ospf_pdb_check(pdb);
    pdb->hello_pid = cfork((forkproc *)pdb->ospf_hello_process, (long)pdb,
			   0,"OSPF Hello", 0);
    ospf_pdb = pdb->ospf_pdb;
    while (TRUE) {
	/* Block until OSPF input */
      start_again:
	edisms((blockproc *)pdb_ospf_BLOCK, (ulong)pdb);
 	INIT_OSPF_SUSPEND(pdb);
 	if (pdb->ospf_flag & OSPF_SOFT_RESET) {
 	    pdb->ospf_flag &= ~OSPF_SOFT_RESET;
 	    if (ospf_soft_reset(pdb))
 		goto start_again;
 	}
  	if (pdb->ospf_flag & OSPF_IF_JUMPSTART) {
	    pdb->ospf_flag &= ~OSPF_IF_JUMPSTART;
  	    ospf_scan_interface(pdb);
  	}
 	if (pdb->ospf_flag & OSPF_IMMEDIATE_SPF) {
 	    pdb->ospf_flag &= ~OSPF_IMMEDIATE_SPF;
 	    ospf_set_all_for_immediate_spf(pdb);
 	}
 	if (pdb->ospf_flag & OSPF_SCHD_SPF) {
 	    pdb->ospf_flag &= ~OSPF_SCHD_SPF;
	    ospf_set_spf_timer(pdb);
	}
	if (pdb->ospf_flag & OSPF_CHECK_ABR) {
	    pdb->ospf_flag &= ~OSPF_CHECK_ABR;
	    /*
	     * Flushing sum could potentially take long,
	     * let's suspend first.
	     */
	    check_ospf_suspend(pdb, DBASE_ACTIVE);
	    ospf_check_if_abr(pdb);
	}
	if (pdb->ospf_flag & OSPF_FLUSH_DO_NOT_AGE) {
	    pdb->ospf_flag &= ~OSPF_FLUSH_DO_NOT_AGE;
	    /*
	     * We would have checked indication when we flush DoNotAge LSA.
	     */
	    pdb->ospf_flag &= ~OSPF_CHECK_INDICATION;
	    ospf_flush_all_do_not_age_lsa(pdb);
	}
	if (pdb->ospf_flag & OSPF_CHECK_INDICATION) {
	    pdb->ospf_flag &= ~OSPF_CHECK_INDICATION;
	    ospf_check_indication_lsa(pdb);
	}
	check_ospf_suspend(pdb, DBASE_ACTIVE);

	while ((pak = dequeue(pdb->inputq))) {
	    ip = (iphdrtype *)ipheadstart(pak);
	    ospf = (ospftype *)ipdatastart(pak);
	    area = pak->if_input->ospf_info->area_ptr;
	    ospf_traffic.inputs++;

	    /* switch the packet based on type */
	    switch (ospf->ospf_type) {
	      case OSPF_DB_DES:
		ospf_traffic.input_dbdes++;
		ospf_rcv_dbd(pak, ospf, area);
		break;
	      case OSPF_LS_REQ:
		ospf_traffic.input_req++;
		ospf_rcv_req(pak, ospf, area);
		break;
	      case OSPF_LS_UPD:
		ospf_traffic.input_lsupd++;
		ospf_rcv_update(pak, ospf, area);
		break; 
	      case OSPF_LS_ACK:
		ospf_traffic.input_ack++;
		ospf_rcv_ack(pak, ospf, area);
		break; 
	      default:
		errmsg(&msgsym(ERRRCV, OSPF), "Invalid Type",
			    ip->srcadr, pak->if_input->namestring);
		break;
	    }
	    /*
	     * Dispose of processed datagram.
	     */
	    datagram_done(pak);
	    check_ospf_suspend(pdb, DBASE_ACTIVE);
	}
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	/*
	 * Process partial SPF
	 */
	if (!QUEUEEMPTY(&ospf_pdb->ospf_partial_sum_spfQ) ||
	    !QUEUEEMPTY(&ospf_pdb->ospf_partial_nssa_spfQ) ||
	    !QUEUEEMPTY(&ospf_pdb->ospf_partial_ex_spfQ)) {
	    ospf_service_partial_spf(pdb);
	}

	check_ospf_suspend(pdb, DBASE_ACTIVE);
	if ((pdb->run_spf) && (SPF_HOLD_EXPIRED(pdb))) {
	    run_spf(pdb);
	    check_ospf_suspend(pdb, DBASE_ACTIVE);
	}
	ospf_service_exdelayq(pdb);
	process_add_req(pdb);

	check_ospf_suspend(pdb, DBASE_ACTIVE);
	if (pdb->ospf_flag & OSPF_MAXAGEQ) {
	    /* 
	     * Check timer here again in case the timer is just set
	     * because of the newly arrived maxage LSA (in this thread).  
	     * We want to wait so spf re-calculation has a chance to run
	     * before we delete the maxage LSA.
	     */
	    if (!(HOLD_TIMER_CHECK(pdb->ospf_maxage_delete_time, 
				   MAXAGEQ_SCAN_INTERVAL))) {
		pdb->ospf_flag &= ~OSPF_MAXAGEQ;
		ospf_service_maxage(pdb);
	    }
	}
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	if (TIMER_RUNNING_AND_AWAKE(pdb->ospf_redist_summary_timer)) {
	    TIMER_STOP(pdb->ospf_redist_summary_timer);
	    ospf_service_redist_summary(pdb);
	    GET_TIMESTAMP(pdb->ospf_redist_summary_last_time);
	}
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	if (mgd_timer_expired(&ospf_pdb->ospf_update_timer))
	    ospf_router_process_mgd_timers(pdb);
    }
} /* end of ospf_router */

/*
 * ospf_hello_process_mgd_timers
 * Process managed timer event for OSPF hello process
 */
static void ospf_hello_process_mgd_timers (pdbtype *pdb)
{
    idbtype *idb;
    nbrtype *nbr;
    mgd_timer *expired_timer;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    while ((expired_timer = 
	    mgd_timer_first_expired(&ospf_pdb->ospf_hello_master_timer))) {
	switch (mgd_timer_type(expired_timer)) {
	case OSPF_IF_HELLO_TIMER:
	    idb = mgd_timer_context(expired_timer);
	    if (!idb || !idb->ospf_info)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    ospf_send_hello(idb->ospf_info->area_ptr, idb, NULL);
	    mgd_timer_start(expired_timer,
			    idb->ospf_info->hello_interval * ONESEC);
	    break;
	case OSPF_IF_ACK_TIMER:
	    idb = mgd_timer_context(expired_timer);
	    if (!idb || !idb->ospf_info)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    if_ack_delayed(idb);
	    break;
	case OSPF_IF_WAIT_TIMER:
	    idb = mgd_timer_context(expired_timer);
	    if (!idb || !idb->ospf_info)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    if_wait_tm(idb);
	    break;
	case OSPF_IF_DELAY_TIMER:
	    idb = mgd_timer_context(expired_timer);
	    if (!idb || !idb->ospf_info)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    if_delayed_net_lsa(idb);
	    break;	    
	case OSPF_IF_FLUSH_NET_TIMER:
	    idb = mgd_timer_context(expired_timer);
	    if (!idb || !idb->ospf_info)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    age_net_lsa_flood(idb);
	    break;
	case OSPF_NBR_DEAD_TIMER:
	    nbr = mgd_timer_context(expired_timer);
	    if (!nbr)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    ospf_nbr_dead(nbr, FALSE);
	    break;
	case OSPF_NBR_DBD_RETRANS_TIMER:
	    nbr = mgd_timer_context(expired_timer);
	    if (!nbr)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    ospf_nbr_retrans_dbd(nbr);	    
	    break;
	case OSPF_NBR_DBD_HOLD_TIMER:
	    nbr = mgd_timer_context(expired_timer);
	    if (!nbr)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    ospf_nbr_hold_dbd(nbr);	    
	    break;
	case OSPF_NBR_POLL_TIMER:
	    nbr = mgd_timer_context(expired_timer);
	    if (!nbr)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    ospf_nbr_poll(nbr);	    
	    break;
	case OSPF_NBR_LSA_RETRANS_TIMER:
	    nbr = mgd_timer_context(expired_timer);
	    if (!nbr)
		goto timer_error;
	    mgd_timer_stop(expired_timer);
	    ospf_nbr_retrans_lsa(nbr);	    
	    break;
	default:
	    buginf("\nOSPF: Unrecognized timer %d", 
		   mgd_timer_type(expired_timer));
	timer_error:
	    mgd_timer_stop(expired_timer);
	}
	check_ospf_suspend(pdb, DBASE_ACTIVE);
    }
}

/*
 * ospf_hello_router
 * OSPF process receiving
 * hello.
 */
static void ospf_hello_router (register pdbtype *pdb)
{
    paktype *pak;
    iphdrtype *ip;
    ospftype *ospf;
    idbtype *idb_ptr;
    areatype *area;
    nbrtype *nbr;
    ospf_idb_info *ospf_info;
    int pak_count;
    ospf_pdb_info *ospf_pdb = pdb->ospf_pdb;

    ip = (iphdrtype *)ipheadstart(pak);

    while (TRUE) {
	/* Block until OSPF input */
	edisms((blockproc *)pdb_ospf_hello_BLOCK, (ulong)pdb);
	pak_count = PAK_SWITCHCOUNT;
	while (TRUE) {
            /*
             * Process only a limited number of packets per pass
             */
            if (pak_count-- == 0) {
                break;
            }
	    
	    pak = dequeue(pdb->hello_inputq);
	    if (!pak)
	        break;

	    ip = (iphdrtype *)ipheadstart(pak);
	    ospf_traffic.inputs++;

	    /*
	     * First credit the input interface for the packet,
	     * but since we do so we have to enforce the maximum 
	     * queue depth on the input queue, or we could run the 
	     * box out of buffers. (See below)
	     */
	    idb_ptr = pak->if_input;
	    clear_if_input(pak);
	    pak->if_input = idb_ptr;

 	    /* Validate the header. */

 	    if (!ospf_validate_header(pak, &ospf, &area)) {
 		ospf_event(OC_BAD_PKT_RCVD, (ulong) ip->srcadr, 
 			   ospf->ospf_type);
		datagram_done(pak);
		continue;
	    }

	    if (ospf_debug_packet)
	        ospf_print_packet(idb_ptr, ospf);
			       
	    /*
	     * pak->if_input could have been changed in case of 
	     * virtual link.
	     */
	    idb_ptr = pak->if_input;

	    /* switch the packet based on type */
	    switch (ospf->ospf_type) {
	      case OSPF_HELLO:
		ospf_traffic.input_hello++;
		ospf_rcv_hello(pak, ospf, area);
		break;
	      case OSPF_DB_DES:
	      case OSPF_LS_REQ:
	      case OSPF_LS_UPD:
	      case OSPF_LS_ACK:
		/*
		 * Whenever we get a packet (not only hello) from an
		 * adjacency, update its dead timer.
		 * By using this approach, we will keep adjacencies alive,
		 * even if we are dropping packets.
		 */
		ospf_info = pak->if_input->ospf_info;
		if (ip_up_ouraddress(ip->srcadr)) {
		    errmsg(&msgsym(RCVSELF, OSPF), 
			   pak->if_input->namestring, ip->srcadr,
			   ip->dstadr, ospf->ospf_rtr_id, ospf->ospf_type,
			   ospf_info->if_state);
		    break;
		}
		nbr = ospf_find_nbr(idb_ptr, ospf->ospf_rtr_id, ip->srcadr,
				    FALSE);
		if (nbr) {
		    mgd_timer_start(&nbr->nbr_dead_timer, 
				    ospf_info->dead_interval*ONESEC);
		}
		/* 
		 * Enforce the maximum queue depth on the input queue.
		 */
		if (pdb->inputq->count < OSPF_MAX_QDEPTH) {
		    enqueue(pdb->inputq, pak);
		    continue;
		} else {
		    ospf_packet_buginf("\nOSPF: Drop packet");
		}
		break;
	      default:
		errmsg(&msgsym(ERRRCV, OSPF), "Invalid Type", 
		       ip->srcadr, idb_ptr->namestring);
		break;
	    }
	    /*
	     * Dispose of processed datagram.
	     */
	    datagram_done(pak);
	}
	check_ospf_suspend(pdb, DBASE_ACTIVE);
	if (mgd_timer_expired(&ospf_pdb->ospf_hello_master_timer))
	    ospf_hello_process_mgd_timers(pdb);
    }
} /* end of ospf_hello_router */
 

/*
 * check_ospf_database:
 * check if the route in IP routing
 * table is in sync with OSPF database.
 */
void check_ospf_database (
    pdbtype *pdb,
    ndbtype *ndb)
{
    lsdbtype *db;
    ospf_pdb_info *ospf_pdb;
    areatype *ar;
    boolean found;

    found = TRUE;
    ospf_pdb = pdb->ospf_pdb;
    
    if ((pdb->redist_callback)
	&& (pdb->index != ndb->pdbindex)
	&& (ip_redist_flag(ndb, pdb) & ~pdb->mask)
	&& (ndb->advertisingmask & pdb->mask)) {
	/*
	 * Check if type 5 external is generated.
	 */
	if (ospf_pdb->ospf_normal_area_count) {
	    db = ospf_db_find_lsa(ospf_pdb->ospf_dummy_area, ndb->netnumber,
				  ndb->netsmask, pdb->ospf_rtr_id,
				  LSA_TYPE_ASE);
	    if (!db)
		found = FALSE;
	}
	/*
	 * Check if type 7 external is generated
	 */
	if (found && ospf_pdb->ospf_nssa_area_count) {
	    for (ar = pdb->ospf_area_list; ar; ar = ar->ar_next) {
		if (ar->ar_nssa && !ar->ar_nssa_no_redist) {
		    db = ospf_db_find_lsa(ar, ndb->netnumber,
					  ndb->netsmask, pdb->ospf_rtr_id,
					  LSA_TYPE_7_ASE);
		    if (!db) {
			found = FALSE;
			break;
		    }
		}
	    }
	}
    }
    if (!found) {
	errmsg(&msgsym(NOTREDIST4, OSPF), ndb->netnumber, ndb->netsmask);
	ospf_set_summary_timer(pdb, OSPF_REDIST_ROUTE);
	return;
    }
}


/*
 * ospf_nettable_scan:
 * Scan the IP routing table
 * and check if any route outoff
 * sync with OSPF database.
 */
 
static void ospf_nettable_scan (
    pdbtype *pdb)
{
    ndbtype 	*ndb, *sdb;
    int 	prev, i;
    sys_timestamp time;

    GET_TIMESTAMP(time);
    for (i = 0; i < NETHASHLEN; i++) {
	if (CLOCK_OUTSIDE_INTERVAL(time, OSPF_SCAN_RUN_TIME)) {
 	    ospf_event(OC_SUSPEND, current_pc(), 0);
  	    process_suspend();
	    ospf_event(OC_UNSUSPEND, 0, 0);
 	    GET_TIMESTAMP(time);
 	}
 	for (ndb = nettable[i]; ndb; ndb = ndb->next) {
 	    if (ndb->attrflags & NET_SUBNETTED) {
		prev = 0;
 		while (prev < NETHASHLEN) {
 		    sdb = ndb->subnettable[prev];
 		    prev++;
 		    for ( ; sdb ; sdb = sdb->next)
 			check_ospf_database(pdb, sdb);
 		}
 	    } else
 		check_ospf_database(pdb, ndb);
 	}
 	for (ndb = supernettable[i]; ndb; ndb = ndb->next) {
	    check_ospf_database(pdb, ndb);
	}
    }
    if (pdb->default_originate)
	if ((default_ndb && !ip_own_ndb(default_ndb, pdb))
	    || pdb->default_always || pdb->default_routemap)
	    (*pdb->redist_callback)(pdb, default_ndb, ROUTE_UP,
				    DEFAULT_ROUTE);
}
 

/*
 * ospf_scanner
 * Scan the OSPF database external database and main routing tables to
 * ensure consistency. This has its own process since it takes a *VERY* 
 * long time to scan. We don't want to starve out other processes
 * and actions for runtime, especially things like KEEPALIVE handling.
 */
 
forktype ospf_scanner (
    pdbtype *pdb)
{
    ulong run_time;
    process_set_priority(PRIO_LOW);
    while (TRUE) {
 	process_sleep_for(pdb->ospf_scantime);
 	ospf_spec_buginf("\nOSPF: Syncing Routing table with OSPF Database");
 	run_time = runtime();
 	ospf_nettable_scan(pdb);
 	ospf_spec_buginf("\nOSPF: Completed Syncing and runtime is %d msec",
 			 (runtime() - run_time));
    }
}


/*
 * ospf_enqueue
 * enqueues this packet
 * for ospf to process.
 */
void ospf_enqueue (paktype *pak)
{
    idbtype *idb;
    pdbtype *pdb;
    iphdrtype *ip;
    ospftype *ospf;
    ospf_idb_info *ospf_info;

    ip = (iphdrtype *)ipheadstart(pak);

    idb = pak->if_input;
    ospf_info = idb->ospf_info;
    if (ospf_info && (ospf_info->enabled) && (ospf_running) && 
	!idb->static_routing) {
	pdb = ospf_info->area_ptr->ar_pdb_ptr;
	if ((pdb->inputq && pdb->running) 
	    && (!(idb->ip_passive_router & pdb->mask))) {
	    if ((ospf_info->if_state == IFS_OTHER)
		&& (ip->dstadr == ALLSPFDROUTERS)) {
		retbuffer(pak);
		return;
	    }
	    ospf = (ospftype *)(ipdatastart(pak));
	    enqueue(pdb->hello_inputq, pak);
	    return;
	}
    } else if (ospf_debug)
	buginf("\nOSPF: Input packet while OSPF disabled on %s",
	       pak->if_input->namestring);

    retbuffer(pak);

} /* end of ospf_enqueue */

/* end of ospf.c */
