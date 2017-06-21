/* $Id: dlsw_csm.c,v 3.8.6.8 1996/08/22 18:34:35 ravip Exp $
 *------------------------------------------------------------------
 * DLSw Circuit Setup Manager (CSM) main routine.
 *
 * August 1994, Ravi Periasamy 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains functions to handle message from CLSI and DLSW or DLX
 * peers. Also contains the initialization and configuration support
 * for DLSw/DLX.
 *------------------------------------------------------------------
 * $Log: dlsw_csm.c,v $
 * Revision 3.8.6.8  1996/08/22  18:34:35  ravip
 * CSCdi66201:  DLSw load sharing broken
 * Branch: California_branch
 *
 * Revision 3.8.6.7  1996/08/16  06:55:42  ioakley
 * CSCdi58903:  Add dlsw touch timer support for sna/netbios
 *              reachability cache entries.
 * Branch: California_branch
 *
 * Revision 3.8.6.6  1996/08/09  23:42:02  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.8.6.5  1996/08/09  21:55:03  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.8.6.4  1996/08/09  19:47:42  ravip
 * CSCdi52387:  DLSw:CPUHOG process=DLSw Background, PC=6055C4E8
 *              Change CSM timer to use managed timers
 * Branch: California_branch
 *
 * Revision 3.8.6.3  1996/05/31  14:26:16  ravip
 * CSCdi58834:  4500/4000/7000 routers crash on show dlsw reachability
 * Branch: California_branch
 *
 * Revision 3.8.6.2  1996/05/17  10:45:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.4.8  1996/05/15  00:38:43  ravip
 * CSCdi50707:  dlsw the reachability search is linear, should be binary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.7  1996/05/14  23:59:35  ioakley
 * CSCdi57686:  Streamline core_to_csm processing by replacing the
 *              core_to_csm function with inline calls to clsi/ssp/dlx
 *              handler routines from the dlsw_core.c module.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.6  1996/05/02  00:05:16  ravip
 * CSCdi56281:  dlsw sna local switching could cause broadcast storm
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.5  1996/04/26  07:33:53  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.4.4  1996/04/25  19:24:11  ravip
 * CSCdi36046:  cache with wildcard/mask does not work in dlsw
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.3  1996/04/25  16:20:02  ravip
 * CSCdi45773:  dlsw icanreach mac-exclusive command not working
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.4.2  1996/04/03  14:01:06  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.4.1  1996/03/17  17:37:49  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.8.6.1  1996/03/18  19:31:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  06:38:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  08:44:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  00:45:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/03/06  03:49:52  ioakley
 * CSCdi50510: Add clean-up for NR_PEND records in csm_destroy_peer.
 *
 * Revision 3.7  1996/02/08  03:19:19  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.6  1996/02/01  06:01:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/18  22:24:22  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/12  17:30:54  ravip
 * CSCdi46357:  clean up dlsw csm debug messages
 *
 * Revision 3.3  1995/11/17  09:02:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:55:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/20  14:08:01  ravip
 * CSCdi40722: Exclusive reachability option does not filter frames
 *             correctly. Fixed problems with deleting reachability cache.
 *
 * Revision 2.5  1995/08/07  05:31:06  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.4  1995/07/26  19:12:11  hampton
 * Convert DLSW to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37708]
 *
 * Revision 2.3  1995/06/15 15:54:10  ravip
 * CSCdi35890:  dlsw allroute-* command broken
 *              print nv_command instead of the string
 *              check for dlsw_peer_active for local peer instead
 *                    of checking dlsw_running before nv_add
 *
 * Revision 2.2  1995/06/13  17:47:57  ravip
 * CSCdi35261:  DLSw should permit ARB from target
 *              Nerd knob to control ARB and SRB of SNA and Netbios frames
 *              Store test frames if search is pending for the dmac
 *
 * Revision 2.1  1995/06/07  20:26:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../util/fsm.h"
#include "../if/rif_util.h"
#include "../llc2/llc2.h"
#include "../srt/rsrb.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ibm/netbios.h"
#include "../ibm/libibm_externs.h"
#include "../h/mgd_timers.h"


#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_csm_db.h"
#include "../dlsw/dlsw_csm.h"
#include "../dlsw/dlsw_debug.h"
#include "../dlsw/dlsw_dlu.h"


/*
 * Global data
 */
boolean     csm_load_balance = FALSE;

const char def_mac_mask[IEEEBYTES] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

uchar       dlsw_sna_xcast_flag = CSM_DEF_SNA_XCAST_FLAG;
uchar       dlsw_nb_xcast_flag = CSM_DEF_NB_XCAST_FLAG;

/*
 * Global data
 * Various timer values
 */
ulong csm_sna_expl_rt = CSM_DEF_SNA_RETRY_INT;
ulong csm_sna_expl_to = CSM_DEF_SNA_EXPL_TO;
ulong csm_sna_cache_to = CSM_DEF_SNA_CACHE_TO;
ulong csm_sna_cache_verify_to = CSM_DEF_SNA_VERIFY_INT;
ulong csm_nb_expl_to = CSM_DEF_NB_EXPL_TO;
ulong csm_nb_expl_rt = CSM_DEF_NB_RETRY_INT;
ulong csm_nb_cache_to = CSM_DEF_NB_CACHE_TO;
ulong csm_nb_cache_verify_to = CSM_DEF_NB_VERIFY_INT;
ulong csm_negative_cache_to = CSM_DEF_NEG_CACHE_TO;
ulong csm_expl_wait_rt = CSM_DEF_EXPL_WAIT_INT;
ulong csm_expl_delay = CSM_DEF_EXPL_DELAY;
ulong csm_touch_sna_cache_active_to = CSM_DEF_TOUCH_SNA_CACHE_ACTIVE_TO;
ulong csm_touch_sna_cache_reset_to = CSM_DEF_TOUCH_SNA_CACHE_RESET_TO;
ulong csm_touch_nb_cache_active_to = CSM_DEF_TOUCH_NB_CACHE_ACTIVE_TO;
ulong csm_touch_nb_cache_reset_to = CSM_DEF_TOUCH_NB_CACHE_RESET_TO;
ulong csm_pod_retry_to = CSM_DEF_POD_RETRY_TO;
 
static int csm_pod_retry_count = CSM_DEF_POD_RETRY_COUNT;

static int csm_max_mac_cache_size = NO_CSMDB_SIZE_LIMIT;
static int csm_max_nbname_cache_size = NO_CSMDB_SIZE_LIMIT;

/*
 * all the databases used by CSM
 */
db_master_t    *lcache_mac_id = NULL;
db_master_t    *lcache_name_id = NULL;
db_master_t    *rcache_mac_id = NULL;
db_master_t    *rcache_name_id = NULL;
db_master_t    *wcache_mac_id = NULL;
db_master_t    *wcache_name_id = NULL;
db_master_t    *icr_pend_id = NULL;
db_master_t    *resp_pend_id = NULL;
db_master_t    *dlxnr_pend_id = NULL;
db_master_t    *nr_pend_id = NULL;
db_master_t    *nbanq_file_id = NULL;
db_master_t    *startdl_pend_id = NULL;

/*
 * Global data
 * Managed Mater Timers
 */
mgd_timer    csm_cache_del_timer;       /* cache delete timer        */
mgd_timer    csm_cache_expl_timer;      /* cache explorer timer      */
mgd_timer    csm_resp_retry_timer;      /* resp_pend retry timer     */
mgd_timer    csm_resp_del_timer;        /* resp_pend delete timer    */
mgd_timer    csm_icr_del_timer;         /* icr_pend delete timer     */
mgd_timer    csm_icr_respond_timer;     /* icr_pend respond timer    */
mgd_timer    csm_startdl_del_timer;     /* startdl_pend delete timer */
mgd_timer    csm_startdl_retry_timer;   /* startdl_pend retry timer  */
mgd_timer    csm_startdl_respond_timer; /* startdl_pend respond timer*/
mgd_timer    csm_nr_retry_timer;        /* nbnr_pend retry timer     */
mgd_timer    csm_nr_del_timer;          /* nbnr_pend delete timer    */
mgd_timer    csm_dlxnr_del_timer;       /* dlxnr_pend delete timer   */
mgd_timer    csm_anq_del_timer;         /* anq delete timer          */
mgd_timer    csm_anq_retry_timer;       /* anq retry timer           */
 
sys_timestamp    csm_expl_delay_timer;  /* explorer delay timer      */
/*
 * function prototypes
 */
static void purge_cache_entries (void);
static void csm_add_perm_cache_entry (db_master_t *, cache_source_t, char *,
                               reach_sense, object_type_t, config_obj_u);
static void csm_timed_response_proc (void);
static csm_rc_t csm_send_resp(lcache_t *, lportid_t, uchar, uchar, char *,
                      char *, char, char, char *, char *, int);
static csm_rc_t csm_start_clsi_circuit(lcache_t *, TCLSIMsg *, lportid_t);
static lportid_t get_portid_from_ringid (ushort ring_id);
 
/*
 * Initialize CSM
 */
void dlsw_csm_init ()
{
    /* Initialize all databases */
    if(!(LCACHE_MAC = create_db("DLSw Local MAC Cache", csm_max_mac_cache_size,
                                sizeof(lcache_t), IEEEBYTES, FALSE))) {
        CSME_DEBUG("\nError creating csm database - LCACHE_MAC");
    }
    if(!(RCACHE_MAC = create_db("DLSw Remote MAC Cache",
                                csm_max_mac_cache_size,
                                sizeof(lcache_t), IEEEBYTES, FALSE))) {
        CSME_DEBUG("\nError creating csm database - LCACHE_MAC");
    }
    if(!(LCACHE_NAME = create_db("DLSw NB Local cache",
                                 csm_max_nbname_cache_size,
                                 sizeof(lcache_t),
                                 NETBIOS_NAME_LENGTH_DEFAULT, FALSE))) {
        NETB_CSME_DEBUG("\nError creating csm database - LCACHE_NAME");
    }
    if(!(RCACHE_NAME = create_db("DLSw NB Remote cache",
                                 csm_max_nbname_cache_size,
                                 sizeof(lcache_t),
                                 NETBIOS_NAME_LENGTH_DEFAULT, FALSE))) {
        NETB_CSME_DEBUG("\nError creating csm database - LCACHE_NAME");
    }
    if(!(WCACHE_MAC = create_db("DLSw WC MAC Cache", csm_max_mac_cache_size,
                                sizeof(wc_mac_cache_t),
                                sizeof(mac_cache_key_t), TRUE))) {
        CSME_DEBUG("\nError creating csm database - WCACHE_MAC");
    }
    if(!(WCACHE_NAME = create_db("DLSw WC NB Cache", csm_max_nbname_cache_size,
                                 sizeof(wc_name_cache_t),
                                 NETBIOS_NAME_LENGTH_DEFAULT, TRUE))) {
        NETB_CSME_DEBUG("\nError creating csm database - WCACHE_NAME");
    }
    if(!(RESP_PEND = create_db("DLSw Resp. Pend", NO_CSMDB_SIZE_LIMIT,
                               sizeof(resp_pend_t), IEEEBYTES, TRUE))) {
        CSME_DEBUG("\nError creating csm database - RESP_PEND");
    }
    if(!(ICR_PEND = create_db("DLSw ICR Pend", NO_CSMDB_SIZE_LIMIT,
                              sizeof(icr_pend_t), IEEEBYTES, TRUE))) {
        CSME_DEBUG("\nError creating csm database - ICR_PEND");
    }
    if(!(DLXNR_PEND = create_db("DLSw SSP NR Pend", NO_CSMDB_SIZE_LIMIT,
                                sizeof(dlxnr_pend_t),
                                NETBIOS_MAX_NAME_LEN, TRUE))) {
        NETB_CSME_DEBUG("\nError creating csm database - DLXNR_PEND");
    }
    if(!(NR_PEND = create_db("DLSw NR Pend", NO_CSMDB_SIZE_LIMIT,
                             sizeof(nr_pend_t), NETBIOS_MAX_NAME_LEN, TRUE))) {
        NETB_CSME_DEBUG("\nError creating csm database - NBNR_PEND");
    }
    if(!(NBANQ_FILE = create_db("DLSw ANQ Info", NO_CSMDB_SIZE_LIMIT,
                                sizeof(nbanq_info_t),
                                NETBIOS_MAX_NAME_LEN, TRUE))) {
        NETB_CSME_DEBUG("\nError creating csm database - NBANQ_PEND");
    }
    if(!(STARTDL_PEND = create_db("DLSw Start DL", NO_CSMDB_SIZE_LIMIT,
                                  sizeof(startdl_pend_t), IEEEBYTES, TRUE))) {
        CSME_DEBUG("\nError creating csm database - STARTDL_PEND");
    }

    /* set up all managed timers */
    mgd_timer_init_parent(&csm_cache_del_timer, NULL);
    mgd_timer_init_parent(&csm_cache_expl_timer, NULL);
    mgd_timer_init_parent(&csm_resp_retry_timer, NULL);
    mgd_timer_init_parent(&csm_resp_del_timer, NULL);
    mgd_timer_init_parent(&csm_icr_del_timer, NULL);
    mgd_timer_init_parent(&csm_icr_respond_timer, NULL);
    mgd_timer_init_parent(&csm_startdl_del_timer, NULL);
    mgd_timer_init_parent(&csm_startdl_retry_timer, NULL);
    mgd_timer_init_parent(&csm_startdl_respond_timer, NULL);
    mgd_timer_init_parent(&csm_nr_retry_timer, NULL);
    mgd_timer_init_parent(&csm_nr_del_timer, NULL);
    mgd_timer_init_parent(&csm_dlxnr_del_timer, NULL);
    mgd_timer_init_parent(&csm_anq_del_timer, NULL);
    mgd_timer_init_parent(&csm_anq_retry_timer, NULL);

    /* Initialize some global parameters with default values */

    return;
}



/*
 * peer_to_csm_filter()
 */

boolean peer_to_csm_filter (peer_filter_msg_u *filter_msg)
{
    /* 
     * this routine is not used now. The message from peer does
     * not go through queues, in the current implementation. If
     * that changes, use this routine to filter messages before
     * they are queued.
     */
    return(TRUE);
}




/*
 * purge csm mac/name cache entries
 */
static void purge_cache_entries(void)
{

    lcache_t         *cache_ptr;
    record_handle    rhandle;
    int              cache_type;
    mgd_timer        *expired_timer;
    db_master_t      *db_num;

    while (mgd_timer_expired(&csm_cache_del_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_cache_del_timer);
        rhandle = mgd_timer_context(expired_timer);
        cache_ptr = (lcache_t *) rhandle_to_data(rhandle);
        cache_type = mgd_timer_type(expired_timer);
        if (cache_type == CSM_LMAC_CACHE) {
            db_num = LCACHE_MAC;
        } else if (cache_type == CSM_RMAC_CACHE) {
            db_num = RCACHE_MAC;
        } else if (cache_type == CSM_LNAME_CACHE) {
            db_num = LCACHE_NAME;
        } else if (cache_type == CSM_RNAME_CACHE) {
            db_num = RCACHE_NAME;
        } else {
            CSME_DEBUG("\nCSM: Invalid cache type set in managed timer ...");
            mgd_timer_stop(&cache_ptr->explore_tstamp);
            mgd_timer_stop(&cache_ptr->del_tstamp);
            continue;
        }
	switch (cache_ptr->status) {
	  case FOUND:
          case NOT_FOUND:
          case VERIFY:
	    if (cache_ptr->cache_type == DYNAMIC) {
                CSMV_DEBUG("\nCSM: Deleting Reachability cache");
                mgd_timer_stop(&cache_ptr->explore_tstamp);
                mgd_timer_stop(&cache_ptr->del_tstamp);
	        delete_record_get_next(db_num, rhandle);
            } else {
		CSME_DEBUG("\nCSM: del timer set for STATIC cache!!!!");
                mgd_timer_stop(&cache_ptr->del_tstamp);
            }
	    break;
	  case SEARCHING:
            if (csm_negative_cache_to != 0) {
                CSMV_DEBUG("\nCSM: Changing Reachability cache status to NOT_FOUND");
		cache_ptr->status = NOT_FOUND;
                mgd_timer_start(&cache_ptr->del_tstamp, csm_negative_cache_to);
                mgd_timer_stop(&cache_ptr->explore_tstamp);
            } else {
                CSMV_DEBUG("\nCSM: Deleting Reachability cache");
                mgd_timer_stop(&cache_ptr->explore_tstamp);
                mgd_timer_stop(&cache_ptr->del_tstamp);
                delete_record_get_next(db_num, rhandle);
            }
	    break;
          case UNCONFIRMED:
            CSME_DEBUG("\nCSM: del timer set for STATIC cache!!!!");
            mgd_timer_stop(&cache_ptr->del_tstamp);
            mgd_timer_stop(&cache_ptr->explore_tstamp);
	    break;
          default:
	    CSME_DEBUG("CSM: Invalid cache status - %x\n", cache_ptr->status);
            mgd_timer_stop(&cache_ptr->del_tstamp);
            mgd_timer_stop(&cache_ptr->explore_tstamp);
            break;
        }
    }

    while(mgd_timer_expired(&csm_cache_expl_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_cache_expl_timer);
        rhandle = mgd_timer_context(expired_timer);
        cache_ptr = (lcache_t *) rhandle_to_data(rhandle);
        cache_type = mgd_timer_type(expired_timer);
        if (cache_type == CSM_LMAC_CACHE) {
            db_num = LCACHE_MAC;
        } else if (cache_type == CSM_RMAC_CACHE) {
            db_num = RCACHE_MAC;
        } else if (cache_type == CSM_LNAME_CACHE) {
            db_num = LCACHE_NAME;
        } else if (cache_type == CSM_RNAME_CACHE) {
            db_num = RCACHE_NAME;
        } else {
            CSME_DEBUG("\nCSM: Invalid cache type set in managed timer ...");
            mgd_timer_stop(&cache_ptr->explore_tstamp);
            mgd_timer_stop(&cache_ptr->del_tstamp);
            continue;
        }
	if (cache_ptr->status == VERIFY) {
            mgd_timer_stop(&cache_ptr->explore_tstamp);
            mgd_timer_stop(&cache_ptr->del_tstamp);
            if (cache_ptr->cache_type == DYNAMIC) {
                delete_record_get_next(db_num, rhandle);
            } else {
                cache_ptr->status = UNCONFIRMED;
            }
        } else {
            mgd_timer_stop(&cache_ptr->explore_tstamp);
        }
    }

    return;
}



/*
 * Timer function to do house keeping
 */
void csm_house_keeping ()
{

    int              ii;
    resp_pend_t      *resp_pend;
    icr_pend_t       *icr_pend;
    startdl_pend_t   *startdl_pend;
    nbanq_info_t     *nbanq_ptr;
    dlxnr_pend_t     *dlxnr_ptr;
    nr_pend_t        *nrpend_ptr;
    record_handle    rhandle;

    mgd_timer        *expired_timer;

    u_msg_ptr_t         u_msg;
    reach_entry_t       reach_entry;
    u_portid_type_t     prp_id;
    csm_rc_t            core_rc;
 
    csm_timed_response_proc();
    purge_cache_entries();

    while(mgd_timer_expired(&csm_resp_del_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_resp_del_timer);
        rhandle = mgd_timer_context(expired_timer);
        resp_pend = (resp_pend_t *) rhandle_to_data(rhandle);
        mgd_timer_stop(&resp_pend->del_tstamp);
        mgd_timer_stop(&resp_pend->retry_tstamp);
        delete_record_get_next(RESP_PEND, rhandle);
    }
 
    while (mgd_timer_expired(&csm_resp_retry_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_resp_retry_timer);
        rhandle = mgd_timer_context(expired_timer);
        resp_pend = (resp_pend_t *) rhandle_to_data(rhandle);
	switch (resp_pend->trans_type) {
          case ALL_LAN_PORTS:
	    /* send test frame to all LAN ports */
	    csm_lan_put(ALL_LAN_PORTS, 0, LLC_TEST, CMDBIT, 0, 
			RECORD_KEY(rhandle), resp_pend->saddr, 
                        0, DLSW_SAP_COOKIE, 
			dlsw_sna_xcast_flag, NULL, BOX_MTU_SIZE, 
			NULL, 0, resp_pend->peer_id);
            break;
          case SPEC_LAN_PORT:
            for (ii = 0; ii < resp_pend->no_in_list; ii++) {
                csm_lan_put(SPEC_LAN_PORT, resp_pend->port_id[ii], 
                            LLC_TEST, CMDBIT, 0, RECORD_KEY(rhandle),
                            resp_pend->saddr, 0, DLSW_SAP_COOKIE, 
                            dlsw_sna_xcast_flag, NULL, BOX_MTU_SIZE, 
                            NULL, 0, resp_pend->peer_id);
            }
            break;
          case SPEC_RING_GROUP:
            /* just send it to all ports */
            /* CL - RP */
            csm_lan_put(ALL_LAN_PORTS, 0, LLC_TEST, CMDBIT, 0, 
			RECORD_KEY(rhandle), resp_pend->saddr, 
                        0, DLSW_SAP_COOKIE, 
			dlsw_sna_xcast_flag, NULL, BOX_MTU_SIZE, 
			NULL, 0, resp_pend->peer_id);
            break;
          case CSM_LOCAL_SWITCH:
            csm_lan_put(CSM_LOCAL_SWITCH, resp_pend->localsw_portid,
                        LLC_TEST, CMDBIT, 0, RECORD_KEY(rhandle),
                        resp_pend->saddr, 0, DLSW_SAP_COOKIE,
                        dlsw_sna_xcast_flag, NULL, BOX_MTU_SIZE,
                        NULL, 0, resp_pend->peer_id);
            break;
	  case NO_SEND:
            /* timer should not have been set */
            CSME_DEBUG("\nCSM: Timer - trastype setting mismatch");
            break;
	  case ALL_WAN_PEERS:
	  case SPEC_WAN_PEER:
	  case SPEC_PEER_GROUP:
	  default:
            /* invalid setting in this case */
            CSM_DEBUG("\nCSM: Invalid trans type! Send test to all ports");
	    csm_lan_put(ALL_LAN_PORTS, 0, LLC_TEST, CMDBIT, 0, 
			RECORD_KEY(rhandle), resp_pend->saddr, 
                        0, DLSW_SAP_COOKIE, 
                        dlsw_sna_xcast_flag, NULL, BOX_MTU_SIZE, 
			NULL, 0, resp_pend->peer_id);
            break;
        }  
        if (resp_pend->trans_type != NO_SEND) {
	    mgd_timer_start(&resp_pend->retry_tstamp, csm_sna_expl_rt);
        } else {
	    mgd_timer_stop(&resp_pend->retry_tstamp);
        }
    }

    while(mgd_timer_expired(&csm_icr_del_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_icr_del_timer);
        rhandle = mgd_timer_context(expired_timer);
        icr_pend = (icr_pend_t *) rhandle_to_data(rhandle);
        mgd_timer_stop(&icr_pend->del_tstamp);
        mgd_timer_stop(&icr_pend->respond_timer);
        delete_record_get_next(ICR_PEND, rhandle);
    }

    while(mgd_timer_expired(&csm_startdl_retry_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_startdl_retry_timer);
        rhandle = mgd_timer_context(expired_timer);
        startdl_pend = (startdl_pend_t *) rhandle_to_data(rhandle);
        if (startdl_pend->type != CLSI_MSG) {
            mgd_timer_stop(&startdl_pend->retry_tstamp);
            continue;
        }
        u_msg.clsi_msg = startdl_pend->u_msg_ptr.clsi_msg;
        prp_id.port_id = startdl_pend->prp_id.port_id;
        bcopy(&startdl_pend->retry_reach_info,
              &reach_entry, sizeof(reach_entry_t));
        /*
         * reach_entry.mtu_size was correctly set when the start_dl
         * retry timer was started. No need to re-calculate it here.
         */
        core_rc = csm_to_core(CLSI_START_NEWDL, &prp_id, &u_msg, &reach_entry);
        if ((core_rc == POD_PEER_NOT_READY)
                    && (startdl_pend->retry_count < csm_pod_retry_count)) {
            mgd_timer_start(&startdl_pend->retry_tstamp, csm_pod_retry_to);
            ++startdl_pend->retry_count;
            continue;
        }
 
        if ((core_rc == POD_PEER_INVALID) || 
            (core_rc == FRAME_REJ) ||
            (core_rc == POD_PEER_NOT_READY)) {
            free_clsi_msg(startdl_pend->u_msg_ptr.clsi_msg);
        }
        mgd_timer_stop(&startdl_pend->del_tstamp);
        mgd_timer_stop(&startdl_pend->retry_tstamp);
        mgd_timer_stop(&startdl_pend->respond_timer);
        delete_record_get_next(STARTDL_PEND, rhandle);
    }

    while(mgd_timer_expired(&csm_startdl_del_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_startdl_del_timer);
        rhandle = mgd_timer_context(expired_timer);
        startdl_pend = (startdl_pend_t *) rhandle_to_data(rhandle);
        mgd_timer_stop(&startdl_pend->del_tstamp);
        mgd_timer_stop(&startdl_pend->retry_tstamp);
        mgd_timer_stop(&startdl_pend->respond_timer);
        if (startdl_pend->type == CLSI_MSG) {
            free_clsi_msg(startdl_pend->u_msg_ptr.clsi_msg);
        } else {
            datagram_done(startdl_pend->u_msg_ptr.pak);
        }
        delete_record_get_next(STARTDL_PEND, rhandle);
    }
 
    while(mgd_timer_expired(&csm_anq_del_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_anq_del_timer);
        rhandle = mgd_timer_context(expired_timer);
        nbanq_ptr = (nbanq_info_t *) rhandle_to_data(rhandle);
	NETB_CSMV_DEBUG("\nCSM: Deleting netbios add name query record....");
        mgd_timer_stop(&nbanq_ptr->del_tstamp);
        mgd_timer_stop(&nbanq_ptr->retry_tstamp);
        delete_record_get_next(NBANQ_FILE, rhandle);
    }

    /* clean up netbios add name query db */
    while (mgd_timer_expired(&csm_anq_retry_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_anq_retry_timer);
        rhandle = mgd_timer_context(expired_timer);
        nbanq_ptr = (nbanq_info_t *) rhandle_to_data(rhandle);
        if ((nbanq_ptr->direction == REMOTE) &&
            (nbanq_ptr->retry_info.trans_type == ALL_LAN_PORTS)) {
            csm_lan_put (ALL_LAN_PORTS, 0, UI, 0, 0,
                         nbanq_ptr->retry_info.daddr,
                         nbanq_ptr->retry_info.saddr,
                         nbanq_ptr->retry_info.dsap,
                         nbanq_ptr->retry_info.ssap,
                         dlsw_nb_xcast_flag, NULL, BOX_MTU_SIZE,
                         nbanq_ptr->retry_info.ui_data,
                         nbanq_ptr->retry_info.ui_data_len,
                         nbanq_ptr->prp_id.peer_id);
            mgd_timer_start(&nbanq_ptr->retry_tstamp, csm_nb_expl_rt);
        } else {
            /*
             * CSM only retries ANQ's on the lan when reach direction
             * is REMOTE (received SSP_anq). We always retry on all lan
             * ports since dest mac is a functional address.
           */
          NETB_CSME_DEBUG("\nCSM: Add Name Query retry timer stopped");
          mgd_timer_stop(&nbanq_ptr->retry_tstamp);
        }
    }

    while(mgd_timer_expired(&csm_dlxnr_del_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_dlxnr_del_timer);
        rhandle = mgd_timer_context(expired_timer);
        dlxnr_ptr = (dlxnr_pend_t *) rhandle_to_data(rhandle);
	NETB_CSMV_DEBUG("\nCSM: Deleting DLX NR pending record....");
        mgd_timer_stop(&dlxnr_ptr->del_tstamp);
        delete_record_get_next(DLXNR_PEND, rhandle);
    }

    while(mgd_timer_expired(&csm_nr_del_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_nr_del_timer);
        rhandle = mgd_timer_context(expired_timer);
        nrpend_ptr = (nr_pend_t *) rhandle_to_data(rhandle);
	NETB_CSMV_DEBUG("\nCSM: Deleting DLX NR pending record....");
        mgd_timer_stop(&nrpend_ptr->retry_tstamp);
        mgd_timer_stop(&nrpend_ptr->del_tstamp);
        delete_record_get_next(NR_PEND, rhandle);
    }

    while (mgd_timer_expired(&csm_nr_retry_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_nr_retry_timer);
        rhandle = mgd_timer_context(expired_timer);
        nrpend_ptr = (nr_pend_t *) rhandle_to_data(rhandle);

        /* resend the name query frame */
	switch (nrpend_ptr->retry_info.trans_type) {
          case ALL_LAN_PORTS:
          case SPEC_RING_GROUP:    /* CL - RP */
            csm_lan_put (ALL_LAN_PORTS, 0, UI, 0, 0,
                         nrpend_ptr->retry_info.daddr, 
                         nrpend_ptr->retry_info.saddr,
                         nrpend_ptr->retry_info.dsap,
                         nrpend_ptr->retry_info.ssap,
                         dlsw_nb_xcast_flag,
                         NULL, nrpend_ptr->mtu_size, 
                         nrpend_ptr->retry_info.ui_data,
                         nrpend_ptr->retry_info.ui_data_len,
                         nrpend_ptr->peer_id);
            break;
          case SPEC_LAN_PORT:
            for (ii = 0; ii < nrpend_ptr->retry_info.no_in_list; ii++) {
                csm_lan_put (SPEC_LAN_PORT, 
                             nrpend_ptr->retry_info.prp_id[ii].port_id, 
                             UI, 0, 0,
                             nrpend_ptr->retry_info.daddr, 
                             nrpend_ptr->retry_info.saddr,
                             nrpend_ptr->retry_info.dsap,
                             nrpend_ptr->retry_info.ssap,
                             dlsw_nb_xcast_flag,
                             NULL, nrpend_ptr->mtu_size, 
                             nrpend_ptr->retry_info.ui_data,
                             nrpend_ptr->retry_info.ui_data_len,
                             nrpend_ptr->peer_id);
            }
            break;
	  case NO_SEND:
	    /* timer should not have been set */
	    NETB_CSME_DEBUG("\nCSM: Name Recognized pending timer should not have been set");
	    mgd_timer_stop(&nrpend_ptr->retry_tstamp);
	    break;
	  case ALL_WAN_PEERS:
	  case SPEC_WAN_PEER:
	  case SPEC_PEER_GROUP:
          default:
	    /* invalid setting in this case */
	    NETB_CSME_DEBUG("\nCSM: Received invalid transmit type - sending Name Query to all ports");
            csm_lan_put (ALL_LAN_PORTS, 0, UI, 0, 0,
                         nrpend_ptr->retry_info.daddr, 
                         nrpend_ptr->retry_info.saddr,
                         nrpend_ptr->retry_info.dsap,
                         nrpend_ptr->retry_info.ssap,
                         dlsw_nb_xcast_flag,
                         NULL, nrpend_ptr->mtu_size, 
                         nrpend_ptr->retry_info.ui_data,
                         nrpend_ptr->retry_info.ui_data_len,
                         nrpend_ptr->peer_id);
            break;
        }  
        if (nrpend_ptr->retry_info.trans_type != NO_SEND) {
            mgd_timer_start(&nrpend_ptr->retry_tstamp, csm_nb_expl_rt);
        } else {
            mgd_timer_stop(&nrpend_ptr->retry_tstamp);
        }
    }

   if (TIMER_RUNNING_AND_AWAKE(csm_expl_delay_timer))
       TIMER_STOP(csm_expl_delay_timer);

   return;
}


static void csm_timed_response_proc (void) 
{
    mgd_timer        *expired_timer;
    record_handle    rhandle;
    icr_pend_t       *icr_pend;
    startdl_pend_t   *startdl_pend;
    lcache_t         *cache_ptr;
    csm_rc_t         rc;

    while(mgd_timer_expired(&csm_icr_respond_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_icr_respond_timer);
        rhandle = mgd_timer_context(expired_timer);
        icr_pend = (icr_pend_t *) rhandle_to_data(rhandle);
        get_mac_cache_entries(RECORD_KEY(rhandle), NULL, NULL, 
                              &cache_ptr, NULL);
        if ((cache_ptr) && (cache_ptr->status == FOUND)) {
            csm_send_resp(cache_ptr, icr_pend->port_id, 
                          (icr_pend->ctrl & ~P_BIT), 
                          (icr_pend->ctrl & P_BIT),
                          icr_pend->saddr, RECORD_KEY(rhandle),
                          icr_pend->ssap, icr_pend->dsap, icr_pend->rif, 
                          icr_pend->test_data, icr_pend->test_data_len);
            mgd_timer_stop(&icr_pend->del_tstamp);
            mgd_timer_stop(&icr_pend->respond_timer);
	    rhandle = delete_record_get_next(ICR_PEND, rhandle);
        } else {
            mgd_timer_start(&icr_pend->respond_timer, csm_expl_wait_rt);
        }
    }

    while(mgd_timer_expired(&csm_startdl_respond_timer)) {
        expired_timer = mgd_timer_first_expired(&csm_startdl_respond_timer);
        rhandle = mgd_timer_context(expired_timer);
        startdl_pend = (startdl_pend_t *) rhandle_to_data(rhandle);
        if (startdl_pend->type != CLSI_MSG) {
            mgd_timer_stop(&startdl_pend->respond_timer);
            continue;
        }
        get_mac_cache_entries(RECORD_KEY(rhandle), NULL, NULL, 
                              &cache_ptr, NULL);
        if ((cache_ptr) && (cache_ptr->status == FOUND)) {
            rc = csm_start_clsi_circuit(cache_ptr, 
                                        startdl_pend->u_msg_ptr.clsi_msg,
                                        startdl_pend->prp_id.port_id);
            if (rc == POD_PEER_NOT_READY) {
                mgd_timer_start(&startdl_pend->respond_timer, csm_expl_wait_rt);
                continue;
            }
            if (rc != PROC_OK) {
	        free_clsi_msg(startdl_pend->u_msg_ptr.clsi_msg);
            }
            mgd_timer_stop(&startdl_pend->del_tstamp);
            mgd_timer_stop(&startdl_pend->retry_tstamp);
            mgd_timer_stop(&startdl_pend->respond_timer);
	    delete_record_get_next(STARTDL_PEND, rhandle);
        } else {
            mgd_timer_start(&startdl_pend->respond_timer, csm_expl_wait_rt);
        }
    }

    return;

}



/*
 * nbnr_recd_update_info(char direction, 
 *                       char *name, 
 *                       u_portid_type_t *prp_id,
 *                       char *rif)
 *      the direction is either LOCAL/REMOTE
 *      src_name is the source name
 *      dst_name is the destination name
 *      prp_id contains the peer_id or the port_id depending on the direction
 *      rif set to not null if rif is available, else set to NULL
 *
 */
void nbnr_recd_update_info (char direction, 
                            char *src_name, 
                            char *dst_name,
                            u_portid_type_t *prp_id,
                            char *rif)
{

    long                ws_mtu_size;
    int                 rc;
    record_handle       rhandle;
    nr_pend_t           *nrpend_ptr;
    dlxnr_pend_t        *dlxnr_ptr;


    if (direction == LOCAL) {
        if (rif) {
            ws_mtu_size = rc_code_to_bytes(rif[1] & RC_LFMASK);
            ws_mtu_size = min(ws_mtu_size, portid_to_mtu(prp_id->port_id));
        } else {
            ws_mtu_size = portid_to_mtu(prp_id->port_id);
        }
    } else {
        ws_mtu_size = peerid_to_mtu(prp_id->peer_id);
    }
    ws_mtu_size = min(ws_mtu_size, BOX_MTU_SIZE);
    if (ws_mtu_size == 0) {
        ws_mtu_size = RC_LF516_VAL;
    }
    name_found_update_cache(src_name, direction, prp_id, rif, ws_mtu_size);

    if (direction == LOCAL) {
        rhandle = seek_record(NR_PEND, src_name, (void **) &nrpend_ptr);
        while (rhandle != NULL) {
            rc = bcmp(dst_name, nrpend_ptr->sname, NETBIOS_MAX_NAME_LEN);
            if (rc == 0) {
                mgd_timer_stop(&nrpend_ptr->del_tstamp);
                mgd_timer_stop(&nrpend_ptr->retry_tstamp);
                delete_record_get_next(NR_PEND, rhandle);
                break;
            }
            rhandle = read_next_record(NR_PEND, rhandle, src_name,
                                       (void **) &nrpend_ptr);
        }
    } else {     /* REMOTE */
        rhandle = seek_record(DLXNR_PEND, src_name, (void **) &dlxnr_ptr);
        while (rhandle != NULL) {
            rc = bcmp(dst_name, dlxnr_ptr->sname, NETBIOS_MAX_NAME_LEN);
            if (rc == 0) {
                mgd_timer_stop(&dlxnr_ptr->del_tstamp);
                delete_record_get_next(DLXNR_PEND, rhandle);
                break;
            }
            rhandle = read_next_record(DLXNR_PEND, rhandle, src_name,
                                       (void **) &dlxnr_ptr);
        }

    }

    return;
}


#define GETRING(x) (((x)&RD_RNMASK)>>RD_RNSHIFT)

static ushort rif2ringid (ushort *ptr)
{
    srbroute_t	*rif;
    ulong	riflen;

    rif = (srbroute_t *)ptr;
    riflen = (rif->rc_blth)&RC_LTHMASK;
    riflen = (riflen - 2) / 2; /* convert into rif->rd index */
    /*
     * since we are DLSw, rd[0] or rd[riflen-1] is the ring-group,
     * so rd[1] or rd[riflen-2] is the tokenring.
     */
    if ((rif->rc_dlf) & RC_D) 
        return(GETRING(rif->rd[riflen-2]));
    else 
        return(GETRING(rif->rd[1]));
}

#undef	GETRING

/*
 * csm_update_mac_entry()
 *
 * Add a mac address based CSM cache entry.
 */
void csm_update_mac_entry (cache_action_t action, cache_source_t source,
                               char *macaddr, char *macmask, reach_sense sense,
                               object_type_t obj_type, config_obj_u obj)
{

    char             cache_type_flag;
    u_portid_type_t  prp_id;
    ulong	     ring_id;


    if (action == SET) {
        if (obj_type == PEER_ID) {
            csm_add_perm_cache_entry(RCACHE_MAC, source, macaddr, 
                                     sense, obj_type, obj);
        } else {
            csm_add_perm_cache_entry(LCACHE_MAC, source, macaddr, 
                                     sense, obj_type, obj);
        }
    } else {    /* action == CLEAR */
        if (source == USER_CONFIGURED) {
            cache_type_flag = USER_CONF;
        } else if (source == CAP_XCHANGE) {
            cache_type_flag = CAP_EXCHANGE;
        } else {
            /*
             * in this routing we handle only calls from the config
             * routine and the capex routine...
             */
            cache_type_flag = 0;
        }
        if (obj_type == PEER_ID) {
            prp_id.peer_id = obj.peer_id;
            delete_csm_cache(RCACHE_MAC, macaddr, &prp_id, 
                             FORCE, cache_type_flag);
	} else if (obj_type == RIF_STRING) {
            ring_id = rif2ringid(obj.rif);
	    prp_id.port_id = get_portid_from_ringid(ring_id);
	    if (prp_id.port_id == NULL) {
                CSME_DEBUG("\nCSM: Error getting portid from rif string - perm");
                cache_type_flag |= SAP_PEND;
                prp_id.ring_id = ring_id;
	    }
            delete_csm_cache(LCACHE_MAC, macaddr, &prp_id, 
                             FORCE, cache_type_flag);
        } else {
	    ring_id = obj.ring_id;
            prp_id.port_id = get_portid_from_ringid(ring_id);
            if (prp_id.port_id == NULL) {
                CSME_DEBUG("\nCSM: Error getting portid from ring number - mac");
                cache_type_flag |= SAP_PEND;
                prp_id.ring_id = ring_id;
            }
	    cache_type_flag |= USER_CONF_RIF;
            delete_csm_cache(LCACHE_MAC, macaddr, &prp_id, 
                             FORCE, cache_type_flag);
        }
    }

    return;

}





/*
 * csm_update_nbname_entry()
 *
 * Add a NetBIOS Name based CSM cache entry.
 */

void csm_update_nbname_entry (cache_action_t action, cache_source_t source,
                           char *nbname, reach_sense sense,
                           object_type_t obj_type, config_obj_u obj)
{

    char             cache_type_flag;
    u_portid_type_t  prp_id;
    ulong	     ring_id;

    if (action == SET) {
        if (obj_type == PEER_ID) {
            csm_add_perm_cache_entry(RCACHE_NAME, source, nbname, 
                                     sense, obj_type, obj);
        } else {
            csm_add_perm_cache_entry(LCACHE_NAME, source, nbname, 
                                     sense, obj_type, obj);
        } 
    } else {    /* action == CLEAR */
        if (source == USER_CONFIGURED) {
            cache_type_flag = USER_CONF;
        } else if (source == CAP_XCHANGE) {
            cache_type_flag = CAP_EXCHANGE;
        } else {
            /*
             * in this routing we handle only calls from the config
             * routine and the capex routine...
             */
            cache_type_flag = 0;
        }
        if (obj_type == PEER_ID) {
            prp_id.peer_id = obj.peer_id;
            delete_csm_cache(RCACHE_NAME, nbname, &prp_id, 
                             FORCE, cache_type_flag);
	} else if (obj_type == RIF_STRING) {
	    cache_type_flag |= USER_CONF_RIF;
	    ring_id = rif2ringid(obj.rif);
	    prp_id.port_id = get_portid_from_ringid(ring_id);
	    if (prp_id.port_id == NULL) {
                CSME_DEBUG("\nCSM: Error getting portid from rif string - perm");
		cache_type_flag |= SAP_PEND;
		prp_id.ring_id = ring_id;
	    }
            delete_csm_cache(LCACHE_NAME, nbname, &prp_id, 
                             FORCE, cache_type_flag);
        } else {
	    ring_id = obj.ring_id;
            prp_id.port_id = get_portid_from_ringid(ring_id);
            if (prp_id.port_id == NULL) {
                NETB_CSME_DEBUG("\nCSM: Error getting portid from ring number - nbname");
                cache_type_flag |= SAP_PEND;
                prp_id.ring_id = ring_id;
            }
            delete_csm_cache(LCACHE_NAME, nbname, &prp_id, 
                             FORCE, cache_type_flag);
        }
    }

    return;


}

/*
 * csm_add_perm_cache_entry()
 *
 * Add a mac address based CSM cache entry.
 */
static void csm_add_perm_cache_entry (db_master_t *db_num, cache_source_t source,
                               char *key, reach_sense sense,
                               object_type_t obj_type, config_obj_u obj)
{
    record_handle    rhandle, loc_rhandle, rem_rhandle;
    lcache_t         *loc_cache_ptr, *rem_cache_ptr, *cache_ptr, ws_cache;
    char             direction;
    srbroute_t       *rif;
    lportid_t        port_id;
    peerhandle_t     peer_id;
    boolean          match_found;
    int              ii, ws_ii;
    boolean	     sap_pend;
    ulong	     ring_id;

    port_id = NULL;
    peer_id = 0;
    sap_pend = FALSE;
    ring_id = 0;
    rif = NULL;
    if (obj_type == PEER_ID) {
        direction = REMOTE;
        peer_id = obj.peer_id;
        if (peer_id == (peerhandle_t) 0) {
            CSME_DEBUG("\nCSM: Invalid peer id received");
            return;
        }
    } else if (obj_type == RIF_STRING) {
	direction = LOCAL;
	rif = (srbroute_t *)obj.rif;
	ring_id = rif2ringid(obj.rif);
        port_id = get_portid_from_ringid(ring_id);
	if (port_id == NULL) {
	    sap_pend = TRUE;
            CSME_DEBUG("\nCSM: Error getting portid from rif string - perm");
	}
    } else if (obj_type == RING_ID) {     
        direction = LOCAL;
        port_id = get_portid_from_ringid(obj.ring_id);
        if (port_id == NULL) {
	    ring_id = obj.ring_id;
	    sap_pend = TRUE;
            CSME_DEBUG("\nCSM: Error getting portid from ring number - perm");
        }
    } else {
	CSME_DEBUG("\nCSM: adding invalid type %x/%x", db_num, obj_type);
	return;
    }
    if ((db_num == LCACHE_MAC) || (db_num == RCACHE_MAC)) {
        get_mac_cache_entries(key, &loc_cache_ptr, &loc_rhandle, 
                                   &rem_cache_ptr, &rem_rhandle);
    } else {     /* LCACHE_NAME or RCACHE_NAME */
        get_name_cache_entries(key, &loc_cache_ptr, &loc_rhandle, 
                                    &rem_cache_ptr, &rem_rhandle);
    } 
    if (((db_num == LCACHE_MAC) || (db_num == LCACHE_NAME)) && 
        (loc_rhandle != NULL)) {
        if (((source == USER_CONFIGURED) && 
                       (!(loc_cache_ptr->cache_type & USER_CONF))) ||
                    ((source == CAP_XCHANGE) && 
                       (loc_cache_ptr->cache_type == DYNAMIC))) {
            /* delete information */
            mgd_timer_stop(&loc_cache_ptr->explore_tstamp);
            mgd_timer_stop(&loc_cache_ptr->del_tstamp);
	    delete_record_get_next(db_num, loc_rhandle);
	    loc_rhandle = NULL;
	    loc_cache_ptr = NULL;
        } else if (loc_cache_ptr->cache_type & USER_CONF) {
            if (source == CAP_XCHANGE) {
                CSMV_DEBUG("\nCSM: User configured same resource.... loc_rhandle");
	        return;
            } else if ((source == USER_CONFIGURED) && (obj_type == PEER_ID)) {
                CSMV_DEBUG("\nCSM: User configured both local and remote - loc_rhandle");
	        return;
            }
        } 
    } else if (((db_num == RCACHE_MAC) || (db_num == RCACHE_NAME)) && 
               (rem_rhandle != NULL)) {
        if (((source == USER_CONFIGURED) && 
                       (!(rem_cache_ptr->cache_type & USER_CONF))) ||
                    ((source == CAP_XCHANGE) && 
                       (rem_cache_ptr->cache_type == DYNAMIC))) {
            /* delete information */
            mgd_timer_stop(&rem_cache_ptr->explore_tstamp);
            mgd_timer_stop(&rem_cache_ptr->del_tstamp);
	    delete_record_get_next(db_num, rem_rhandle);
	    rem_rhandle = NULL;
	    rem_cache_ptr = NULL;
        } else if (rem_cache_ptr->cache_type & USER_CONF) {
            if (source == CAP_XCHANGE) {
                CSMV_DEBUG("\nCSM: User configured same resource.... rem_rhandle");
	        return;
            } else if ((source == USER_CONFIGURED) && (obj_type != PEER_ID)) {
                CSM_DEBUG("\nCSM: User configured both local and remote - rem_rhandle");
	        return;
            }
        }
    }

    if (obj_type == PEER_ID) {
        if (rem_rhandle == NULL) {
	    /* new entry */
	    cache_ptr = NULL;
        } else {
	    /* add to existing entry */
	    cache_ptr = rem_cache_ptr;
        }
    } else {     /* PORT_ID */
        if (loc_rhandle == NULL) {
	    /* new entry */
	    cache_ptr = NULL;
        } else {
            /* add to existing entry */
	    cache_ptr = rem_cache_ptr;
        }
    }
    if (cache_ptr == NULL) {
        if (source == USER_CONFIGURED) {
            ws_cache.cache_type = USER_CONF;
	    if (obj_type == RIF_STRING)
		ws_cache.cache_type |= USER_CONF_RIF;
        } else {     /* CAP_XCHANGE */
            ws_cache.cache_type = CAP_EXCHANGE;
        }
        if (sense == REACHABLE) {
            ws_cache.status = UNCONFIRMED;
        } else {    /* NOT_REACHABLE */
            ws_cache.status = NOT_REACHABLE;
        }
        ws_cache.reach_direction = direction;
        ws_cache.no_in_list = 1;
	if (rif)
	    memcpy(ws_cache.r_entry[0].rif, rif, (rif->rc_blth&RC_LTHMASK));
	else
            memset(ws_cache.r_entry[0].rif, 0, SRB_MAXRIF);
        ws_cache.r_entry[0].mtu_size = 0;
        if (direction == LOCAL) {
	    if (sap_pend) {
		ws_cache.cache_type |= SAP_PEND;
		ws_cache.r_entry[0].prp_id.ring_id = ring_id;
	    } else
        	ws_cache.r_entry[0].prp_id.port_id = port_id;
        } else {
            ws_cache.r_entry[0].prp_id.peer_id = peer_id;
        }
        ws_cache.r_entry[0].flags = INDELIBLE;
        for (ii = 1; ii < MAX_REACH; ii++) {
            ws_cache.r_entry[ii].prp_id.peer_id = 0;
            ws_cache.r_entry[ii].flags = 0;
            ws_cache.r_entry[ii].mtu_size = 0;
            memset(ws_cache.r_entry[ii].rif, 0, SRB_MAXRIF);
        }
        memset(&ws_cache.del_tstamp, 0, sizeof(ws_cache.del_tstamp));
        memset(&ws_cache.explore_tstamp, 0, sizeof(ws_cache.explore_tstamp));
        rhandle = add_record(db_num, key, &ws_cache);
        if (rhandle == NULL) {
            CSME_DEBUG("\nCSM: Error adding cache info - csm_add_perm_cache_entry");
        } else {
            lcache_t     *tmp_ptr;
            tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
            if (db_num == LCACHE_MAC) {
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_LMAC_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_LMAC_CACHE, rhandle, FALSE);
            } else if (db_num == RCACHE_MAC) {
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_RMAC_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_RMAC_CACHE, rhandle, FALSE);
            } else if (db_num == LCACHE_NAME) {
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_LNAME_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_LNAME_CACHE, rhandle, FALSE);
            } else {
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_RNAME_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_RNAME_CACHE, rhandle, FALSE);
            }
            mgd_timer_stop(&tmp_ptr->del_tstamp);
            mgd_timer_stop(&tmp_ptr->explore_tstamp);
            TIMER_STOP(tmp_ptr->verify_tstamp);
        }
    } else {
        /* find a blank entry in the reach_entry table */
        ws_ii = -1;
        match_found = FALSE;
        for (ii = 0; ii < MAX_REACH; ii++) {
            if ((cache_ptr->r_entry[ii].flags & ~USE_BIT) == 0) {
                if (ws_ii == -1) {
		    ws_ii = ii;
                }
            } else {
	        if (direction == LOCAL) {
		    if (cache_ptr->r_entry[ii].prp_id.port_id == port_id) {
		        match_found = TRUE;
			ws_ii = ii;
			break;
                    }
                } else {
		    if (cache_ptr->r_entry[ii].prp_id.peer_id == peer_id) {
		        match_found = TRUE;
		        ws_ii = ii;
		        break;
                    }
                }
            }   
        }   
        if (match_found) {
	    CSMV_DEBUG("\nCSM: Resource already configured");
        } else if (ws_ii == -1) {
	    CSME_DEBUG("\nCSM: Reach entry table full");
        } else {
            cache_ptr->no_in_list++;
	    cache_ptr->r_entry[ws_ii].flags = INDELIBLE;
	    if (direction == REMOTE) {
	        cache_ptr->r_entry[ws_ii].prp_id.peer_id = peer_id;
            } else {
	        cache_ptr->r_entry[ws_ii].prp_id.port_id = port_id;
            }
        }
    }

    return;
}





/*
 * csm_update_sap_entry()
 *
 * Add a SAP based CSM cache entry.
 * CSM only perform LAN and WAN input SAP filtering for the local box.
 * The Peer Manager will perform SAP filtering output based on a remote 
 * peer's SAP support learned via capabilities exchange.
 */

void csm_update_sap_entry (cache_action_t action, cache_source_t source, 
                           uchar sap, reach_sense sense, peerhandle_t peer_id)
{

    return;
}


/*
 * csm_clear_sap_entries()
 */

void csm_clear_sap_entries (cache_source_t source)
{

    return;
}


/*
 * csm_clear_cap_entries()
 */

void csm_clear_cap_entries (peerhandle_t peer_id)
{

    u_portid_type_t  prp_id;

    prp_id.peer_id = peer_id;
    delete_csm_cache(RCACHE_MAC, NULL, &prp_id, FORCE, CAP_EXCHANGE);
    delete_csm_cache(RCACHE_NAME, NULL, &prp_id, FORCE, CAP_EXCHANGE);

    return;

}


/*
 * csm_destroy_peer()
 */
void csm_destroy_peer(peerhandle_t peer_handle)
{

    u_portid_type_t  prp_id;
    record_handle    rhandle;
    resp_pend_t      *resp_pend;
    nr_pend_t        *nr_pend;
    startdl_pend_t   *startdl_pend;
    nbanq_info_t     *nbanq_file;

    prp_id.peer_id = peer_handle;
    delete_csm_cache(RCACHE_MAC, NULL, &prp_id,
                     FORCE, (CAP_EXCHANGE | DYNAMIC));
    delete_csm_cache(RCACHE_MAC, NULL, &prp_id,
                     NO_FORCE, USER_CONF);
    delete_csm_cache(RCACHE_NAME, NULL, &prp_id,
                     FORCE, (CAP_EXCHANGE | DYNAMIC));
    delete_csm_cache(RCACHE_NAME, NULL, &prp_id,
                     NO_FORCE, USER_CONF);

    rhandle =  read_next_record(RESP_PEND, NULL, NULL, (void **) &resp_pend);
    while (rhandle != NULL) {
        if (resp_pend->peer_id == peer_handle) {
            CSMV_DEBUG("\nCSM: Peer down: Deleting resp pend record....");
            mgd_timer_stop(&resp_pend->del_tstamp);
            mgd_timer_stop(&resp_pend->retry_tstamp);
	    rhandle = delete_record_get_next(RESP_PEND, rhandle);
            resp_pend = rhandle_to_data(rhandle);
        } else {
	    rhandle = read_next_record(RESP_PEND, rhandle, NULL, 
				   (void **) &resp_pend);
        }
    }
    rhandle =  read_next_record(NR_PEND, NULL, NULL, (void **) &nr_pend);
    while (rhandle != NULL) {
        if (nr_pend->peer_id == peer_handle) {
            CSMV_DEBUG("\nCSM: Peer down: Deleting nr pend record....");
            mgd_timer_stop(&nr_pend->del_tstamp);
            mgd_timer_stop(&nr_pend->retry_tstamp);
	    rhandle = delete_record_get_next(NR_PEND, rhandle);
            nr_pend = rhandle_to_data(rhandle);
        } else {
	    rhandle = read_next_record(NR_PEND, rhandle, NULL, 
				   (void **) &nr_pend);
        }
    }
    rhandle =  read_next_record(STARTDL_PEND, NULL, NULL, 
				(void **) &startdl_pend);
    while (rhandle != NULL) {
        if (startdl_pend->retry_reach_info.prp_id.peer_id == peer_handle) {
            CSMV_DEBUG("\nCSM: Peer down: Deleting startdl pend record...");
            mgd_timer_stop(&startdl_pend->del_tstamp);
            mgd_timer_stop(&startdl_pend->retry_tstamp);
            mgd_timer_stop(&startdl_pend->respond_timer);
            rhandle = delete_record_get_next(STARTDL_PEND, rhandle);
            startdl_pend = rhandle_to_data(rhandle);
        } else if (startdl_pend->prp_id.peer_id == peer_handle) {
            CSMV_DEBUG("\nCSM: Peer down: Deleting startdl pend record...");
            mgd_timer_stop(&startdl_pend->del_tstamp);
            mgd_timer_stop(&startdl_pend->retry_tstamp);
            mgd_timer_stop(&startdl_pend->respond_timer);
            rhandle = delete_record_get_next(STARTDL_PEND, rhandle);
            startdl_pend = rhandle_to_data(rhandle);
        } else {
            rhandle = read_next_record(STARTDL_PEND, rhandle, NULL,
                                       (void **) &startdl_pend);
        }
    }
  
    rhandle =  read_next_record(NBANQ_FILE, NULL, NULL, (void **) &nbanq_file);
    while (rhandle != NULL) {
        if (nbanq_file->prp_id.peer_id == peer_handle) {
            CSMV_DEBUG("\nCSM: Peer down: Deleting nbanq file record...");
            mgd_timer_stop(&nbanq_file->del_tstamp);
            mgd_timer_stop(&nbanq_file->retry_tstamp);
            rhandle = delete_record_get_next(NBANQ_FILE, rhandle);
            nbanq_file = rhandle_to_data(rhandle);
        } else {
            rhandle = read_next_record(NBANQ_FILE, rhandle, NULL,
                                       (void **) &nbanq_file);
        }
    }
    return;

}

/*
 * csm_delete_cache()
 */
void csm_delete_cache(peerhandle_t peer_handle)
{

    u_portid_type_t  prp_id;

    prp_id.peer_id = peer_handle;
    delete_csm_cache(RCACHE_MAC, NULL, &prp_id, NO_FORCE, 0);
    delete_csm_cache(RCACHE_NAME, NULL, &prp_id, NO_FORCE, 0);
}


/*
 * csm_deactivate_ring()
 */
#ifdef NOBODY_CALLS_ME
void csm_deactivate_ring (lportid_t port_id)
{

    u_portid_type_t  prp_id;
    record_handle    rhandle;
    icr_pend_t       *icr_pend;

    prp_id.port_id = port_id;
    /*
     * local cache through a ring is either DYNAMIC or USER_CONF
     * So we dont have to handle CAP_EXCHANGE in this case
     */
    delete_csm_cache(LCACHE_MAC, NULL, &prp_id, NO_FORCE, 0);
    delete_csm_cache(LCACHE_NAME, NULL, &prp_id, NO_FORCE, 0);

    rhandle = read_next_record(ICR_PEND, NULL, NULL, (void **) &icr_pend);
    while (rhandle != NULL) {
        if (icr_pend->port_id == port_id) {
            CSMV_DEBUG("\nCSM: Port deactivated...Deleting icr pend record...");
            mgd_timer_stop(&icr_pend->del_tstamp);
            mgd_timer_stop(&icr_pend->respond_timer);
            rhandle = delete_record_get_next(ICR_PEND, rhandle);
            icr_pend = rhandle_to_data(rhandle);
        } else {
            rhandle = read_next_record(ICR_PEND, rhandle, NULL, 
                                   (void **) &icr_pend);
        }
    }
    return;

}
#endif

ushort portid2ringid (lportid_t portid)
{
    dlsw_ring_t *dgra_tmp;
 
    dgra_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
    while(dgra_tmp) {
        if (dgra_tmp == (dlsw_ring_t *)portid) {
            return (dgra_tmp->ring_number);
        }
        dgra_tmp = dgra_tmp->next;
    }
    return 0;
}

static lportid_t get_portid_from_ringid (ushort ring_id)
{
    dlsw_ring_t *dgra_tmp;

    dgra_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
    while(dgra_tmp) {
        if (dgra_tmp->ring_number == ring_id) {
            return ((lportid_t) dgra_tmp);
        }
        dgra_tmp = dgra_tmp->next;
    }

    return NULL;
}




void clear_csm_cache (parseinfo *csb)
{

    if (csb->sense) {
        CSM_DEBUG("\nCSM: Clearing CSM cache");
        delete_csm_cache(LCACHE_MAC, NULL, NULL, NO_FORCE, 0);
        delete_csm_cache(RCACHE_MAC, NULL, NULL, NO_FORCE, 0);
        delete_csm_cache(LCACHE_NAME, NULL, NULL, NO_FORCE, 0);
        delete_csm_cache(RCACHE_NAME, NULL, NULL, NO_FORCE, 0);
    } else {
        CSME_DEBUG("\nCSM: clear cache called with no sense");
    }

}



/*
 * csm_local_mac_excl_pass()
 * Return : TRUE if the frame passes the filter check
 *          FALSE if the frame fails the filter
 */
boolean csm_local_mac_excl_pass (char *mac_addr)
{

    macentry_t *macentry;
    int        rc = 0;

    if (mac_addr == NULL) {
        return TRUE;
    }

    if (dlsw_is_local_mac_excl()) {
        macentry = get_local_mac_reach_qhead();
        for (; macentry; macentry = macentry->next) {
            rc = masked_mac_cmp(mac_addr, macentry->macaddr, macentry->macmask);
            if (rc == 0) {
                return TRUE;
            }
        }
        CSMV_DEBUG("\nCSM: mac address NOT found in LOCAL list");
        return FALSE;
    }
    return TRUE;

}




/*
 * csm_local_nb_excl_pass()
 * Return : TRUE if the frame passes the filter check
 *          FALSE if the frame fails the filter
 */
boolean csm_local_nb_excl_pass (char *nb_name)
{

    int       rc;
    nbentry_t *nbentry;

    if (nb_name == NULL) {
        return TRUE;
    }

    if (dlsw_is_local_nb_excl()) {
        nbentry = get_local_name_reach_qhead();
        for (; nbentry; nbentry = nbentry->next) {
            rc = wc_strn_cmp(nb_name, nbentry->nbname,
                             NETBIOS_NAME_LENGTH_DEFAULT);
            if (rc == 0) {
                return TRUE;
            }
        }
        NETB_CSMV_DEBUG("\nCSM: netbios name NOT found in LOCAL list");
        return FALSE;
    }

    return TRUE;

}




/*
 * csm_peer_mac_excl_pass()
 * Return : TRUE if the frame passes the filter check
 *          FALSE if the frame fails the filter
 */
boolean csm_peer_mac_excl_pass (peerhandle_t ph, char *mac_addr)
{

    macentry_t *macentry;
    int        rc = 0;

    if ((mac_addr == NULL) || (ph == 0)) {
        return TRUE;
    }

    if (dlsw_is_peer_mac_excl(ph)) {
        macentry = peer_get_mac_reach_qhead(ph);
        for (; macentry; macentry = macentry->next) {
            rc = masked_mac_cmp(mac_addr, macentry->macaddr, macentry->macmask);
            if (rc == 0) {
                return TRUE;
            }
        }
        SNA_CSMV_DEBUG("\nCSM: mac address NOT found in PEER reachability list");
        return FALSE;
    }
    return TRUE;
}




/*
 * csm_peer_nb_excl_pass()
 * Return : TRUE if the frame passes the filter check
 *          FALSE if the frame fails the filter
 */
boolean csm_peer_nb_excl_pass (peerhandle_t ph, char *nb_name)
{
    int       rc;
    nbentry_t *nbentry;

    if ((nb_name == NULL) || (ph == 0)) {
        return TRUE;
    }

    if (dlsw_is_peer_nb_excl(ph)) {
        nbentry = peer_get_name_reach_qhead(ph);
        for (; nbentry; nbentry = nbentry->next) {
            rc = wc_strn_cmp(nb_name, nbentry->nbname,
                             NETBIOS_NAME_LENGTH_DEFAULT);
            if (rc == 0) {
                return TRUE;
            }
        }
        NETB_CSMV_DEBUG("\nCSM: netbios name NOT found in PEER reachability list");
        return FALSE;
    }

    return TRUE;
}



static csm_rc_t csm_send_resp(lcache_t *cache_ptr, lportid_t port_id, 
                   uchar msg_type, uchar poll_final,
                   char *daddr, char *saddr, char dsap, char ssap,
                   char *rif, char *test_data, int test_data_len)
{
    int              idx;
    u_portid_type_t  prp_id;
    int              rc;

    prp_id.port_id = port_id;
    idx = get_reach_entry_mac(cache_ptr, 0, &prp_id, FALSE);
    if (idx < 0) {
        if (idx == CSM_NO_RING_MATCH) {
            CSMV_DEBUG("\nCSM: Ring list did not match");
        } else {
            CSME_DEBUG("\nCSM: Reach index error %s", csm_error(idx));
        }
        return FALSE;
    }
    rc = csm_lan_put (SPEC_LAN_PORT, port_id, msg_type, RSPBIT, poll_final,
                 daddr, saddr, dsap, ssap, DIRECT_FRAME, rif,
                 cache_ptr->r_entry[idx].mtu_size,
                 test_data, test_data_len,
                 cache_ptr->r_entry[idx].prp_id.peer_id);
    if (rc) {
        return PROC_OK;
    } else {
        return FRAME_REJ;
    }

}



/*
 * the cache is REMOTE and we are starting a new local circuit
 */
static csm_rc_t csm_start_clsi_circuit(lcache_t *cache_ptr, 
                              TCLSIMsg *clsi_msg,
                              lportid_t port_id)
{

    int              idx, rif_rc;
    long             mtu_size;
    u_portid_type_t  prp_id;
    uchar            rif[SRB_MAXRIF];
    uchar            *saddr, *daddr, ssap, dsap;
    csm_rc_t         core_rc;
    u_msg_ptr_t      u_msg;
    reach_entry_t    reach_entry;



    saddr = get_saddr_from_clsi(clsi_msg);
    daddr = get_daddr_from_clsi(clsi_msg);
    ssap = get_ssap_from_clsi(clsi_msg);
    dsap = get_dsap_from_clsi(clsi_msg);
    rif_rc = get_rif_from_clsi(clsi_msg, rif);
    if (rif_rc < 0) {
        return FRAME_REJ;
    }
    ssap &= ~RSPBIT;
    saddr[0] &= ~TR_RII;
    if (rif[0] != 0) {
        mtu_size = rc_code_to_bytes((rif[1] & RC_LFMASK));
        if (rif[0] & RC_RTMASK) {
            /* Adjust the mtu_size only if it is a broadcast */
            mtu_size = min(mtu_size, (portid_to_mtu(port_id)));
            mtu_size = min(mtu_size, BOX_MTU_SIZE);
        }
    } else {
        mtu_size = portid_to_mtu(port_id);
    }
    prp_id.port_id = port_id;
    idx = get_reach_entry_mac(cache_ptr, mtu_size, &prp_id, TRUE);
    if (idx < 0) {
        if (idx == CSM_NO_RING_MATCH) {
            CSMV_DEBUG("\nCSM: Ring list did not match");
        } else {
            CSME_DEBUG("\nCSM: Reach index error %s", csm_error(idx));
        }
        return NO_REACH_ENTRY;
    }

    u_msg.clsi_msg = clsi_msg;
    prp_id.port_id = port_id;
    bcopy(&(cache_ptr->r_entry[idx]), &reach_entry, sizeof(reach_entry_t));
    reach_entry.mtu_size = mtu_size;
    core_rc = csm_to_core(CLSI_START_NEWDL, &prp_id, &u_msg, &reach_entry);
    return(core_rc);

}

void csm_start_explr_delay_timer(void)
{
    if (csm_expl_delay)
        TIMER_START (csm_expl_delay_timer, csm_expl_delay);
}

void delete_xidfailure_mac_entry (uchar *mac_addr, u_portid_type_t *prp_id)
{
    lcache_t       *lcache_ptr;
    lcache_t       *rcache_ptr;

    CSMV_DEBUG("\nCSM: Clear reachability for mac address %e due to circuit failure",
               mac_addr);

    get_mac_cache_entries(mac_addr, &lcache_ptr, NULL, &rcache_ptr, NULL);

    if (lcache_ptr) {
        delete_csm_cache(LCACHE_MAC, mac_addr, prp_id, NO_FORCE, 0);
    }

    if (rcache_ptr) {
        if (rcache_ptr->no_in_list <= 1) {
            delete_csm_cache(RCACHE_MAC, mac_addr, NULL, NO_FORCE, 0);
        } else {
            if (!csm_load_balance) {
                delete_csm_cache(RCACHE_MAC, mac_addr, prp_id, NO_FORCE, 0);
            }
        }
    }
}

void csm_activate_static (dlsw_ring_t *ring)
{
    lcache_t        *cache_ptr;
    record_handle   rhandle;
    ushort	    ring_id;
 
    CSMV_DEBUG("\nCSM: Activating static reachability for ring %d",
                ring->ring_number);

    rhandle = read_next_record(LCACHE_MAC, NULL, NULL, (void **) &cache_ptr);
    while (rhandle) {
	if (cache_ptr->cache_type & SAP_PEND) {
	    ring_id = cache_ptr->r_entry[0].prp_id.ring_id;
	    if (ring_id == ring->ring_number) {
		cache_ptr->r_entry[0].prp_id.port_id =
		    get_portid_from_ringid(ring_id);
		if (cache_ptr->r_entry[0].prp_id.port_id) {
		    cache_ptr->cache_type &= ~SAP_PEND;
		} else {
		    CSMV_DEBUG("\nCSM: error activating static entry");
		}
	    }
	}
	rhandle = read_next_record(LCACHE_MAC, rhandle, NULL,
				   (void **) &cache_ptr);
    }
    rhandle = read_next_record(LCACHE_NAME, NULL, NULL, (void **) &cache_ptr);
    while (rhandle) {
	if (cache_ptr->cache_type & SAP_PEND) {
	    ring_id = cache_ptr->r_entry[0].prp_id.ring_id;
	    if (ring_id == ring->ring_number) {
		cache_ptr->r_entry[0].prp_id.port_id =
		    get_portid_from_ringid(ring_id);
		if (cache_ptr->r_entry[0].prp_id.port_id) {
		    cache_ptr->cache_type &= ~SAP_PEND;
		} else {
		    NETB_CSMV_DEBUG("\nCSM: error activating static entry");
		}
	    }
	}
	rhandle = read_next_record(LCACHE_MAC, rhandle, NULL,
				   (void **) &cache_ptr);
    }
}

