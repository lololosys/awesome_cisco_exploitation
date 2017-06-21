/* $Id: dlsw_csm_ssp.c,v 3.11.4.11 1996/09/09 17:39:42 fbordona Exp $
 *------------------------------------------------------------------
 * DLSw SSP Message Handler
 *
 * August 1994, Ravi Periasamy 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains routines to handle message from peers talking DLSw
 * standard protocol
 *------------------------------------------------------------------
 * $Log: dlsw_csm_ssp.c,v $
 * Revision 3.11.4.11  1996/09/09  17:39:42  fbordona
 * CSCdi66251:  Filtering at DLSw layer will not work on border router
 * Branch: California_branch
 *
 * Revision 3.11.4.10  1996/08/22  18:35:25  ravip
 * CSCdi66201:  DLSw load sharing broken
 * Branch: California_branch
 *
 * Revision 3.11.4.9  1996/08/20  19:18:12  kpatel
 * CSCdi66529:  Spurious memory access on MIPS platform running Netbios
 *              over DLSw+
 * Branch: California_branch
 *
 * Revision 3.11.4.8  1996/08/16  06:56:09  ioakley
 * CSCdi58903:  Add dlsw touch timer support for sna/netbios
 *              reachability cache entries.
 * Branch: California_branch
 *
 * Revision 3.11.4.7  1996/08/09  23:42:56  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.11.4.6  1996/08/09  21:55:42  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.11.4.5  1996/08/09  19:48:06  ravip
 * CSCdi52387:  DLSw:CPUHOG process=DLSw Background, PC=6055C4E8
 *              Change CSM timer to use managed timers
 * Branch: California_branch
 *
 * Revision 3.11.4.4  1996/08/05  18:39:15  ravip
 * CSCdi62784:  dlsw netbios windows nt cant connect nq sent directed no nr
 * Branch: California_branch
 *
 * Revision 3.11.4.3  1996/05/31  14:26:31  ravip
 * CSCdi58834:  4500/4000/7000 routers crash on show dlsw reachability
 * Branch: California_branch
 *
 * Revision 3.11.4.2  1996/05/17  10:45:28  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.10.2.5  1996/05/15  00:39:05  ravip
 * CSCdi50707:  dlsw the reachability search is linear, should be binary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.4  1996/05/02  00:05:36  ravip
 * CSCdi56281:  dlsw sna local switching could cause broadcast storm
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.3  1996/04/26  07:34:10  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.10.2.2  1996/04/03  14:01:23  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.10.2.1  1996/03/17  17:37:58  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.4.1  1996/03/18  19:31:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  06:38:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  08:44:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  00:45:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/07  19:33:58  ravip
 * CSCdi50382:  netbios dlsw should not send a test_cmd after a nr
 * received.
 *
 * Revision 3.10  1996/02/12  22:13:42  ravip
 * CSCdi48012:  1795 interoperability
 *              - Reverse source and destination mac addresses for
 *                SSP_OP_netbios_nr and _anr frames
 *              - Set remote data link correlator in SSP_OP_netbios_nr
 *                to origin data link correlator values received in
 *                SSP_OP_netbios_nq frame
 *
 * Revision 3.9  1996/02/07  21:45:13  ravip
 * CSCdi46935:  DLSw does not propogate largest frame bits correctly
 *
 * Revision 3.8  1996/02/01  22:25:04  fbordona
 * CSCdi47888:  dlsw fst sets up llc2 circuit.
 *              Add fast peer check in ssp_icr_ex_proc to cover
 *              case where fast circuit started after mac verify.
 *
 * Revision 3.7  1996/02/01  06:01:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/18  22:24:34  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/12  17:32:39  ravip
 * CSCdi46357:  clean up dlsw csm debug messages
 *
 * Revision 3.4  1995/12/19  00:39:44  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.3  1995/11/17  09:03:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:55:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/25  15:34:16  fbordona
 * CSCdi41072:  Netbios sessions do not get established after power off
 *              the client.
 *              CUR_cs verify should send TEST to NULL dsap.
 *              Do not retry SSP_HALT_DL.
 *
 * Revision 2.5  1995/09/25  21:03:22  ravip
 * CSCdi39850:  dlsw sends test to sap04 rather than sap00.
 *              ncp expects sap00
 *              Using using make_forward_rif causes problem with the fep
 *              in some cases
 *
 * Revision 2.4  1995/08/07  05:31:20  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.3  1995/07/18  16:15:52  kpatel
 * CSCdi37237:  DLSw+ ICANREACH frame does not reflect SAP values from
 *              CANUREACH
 *
 * Revision 2.2  1995/06/13  17:49:57  ravip
 * CSCdi35261:  DLSw should permit ARB from target
 *              Nerd knob to control ARB and SRB of SNA and Netbios frames
 *              Store test frames if search is pending for the dmac
 *
 * Revision 2.1  1995/06/07  20:26:46  hampton
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
#include "../if/rif_util.h"
#include "../ui/debug.h"
#include "../util/fsm.h"
#include "../if/tring.h"
#include "../llc2/llc2.h"
#include "../srt/rsrb.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ibm/netbios.h"
#include "../ibm/libibm_externs.h"
#include "../h/mgd_timers.h"


#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_csm_db.h"
#include "../dlsw/dlsw_csm.h"
#include "../dlsw/dlsw_debug.h"



/* prototypes */
static csm_rc_t ssp_cur_ex_proc (peerhandle_t, paktype *);
static csm_rc_t ssp_icr_ex_proc (peerhandle_t, paktype *);
static csm_rc_t ssp_cur_cs_proc (peerhandle_t, paktype *);
static csm_rc_t ssp_nbnq_proc (peerhandle_t, paktype *);
static csm_rc_t ssp_nbnr_proc (peerhandle_t, paktype *);
static csm_rc_t ssp_dataframe_proc (peerhandle_t, paktype *);
csm_rc_t ssp_nonnb_frame_proc (peerhandle_t, paktype *);




/*
 * SSP Message handler
 */
csm_rc_t csm_ssp_handler (peerhandle_t peer_handle, 
		          paktype *pak)
{

    char       origin_mac_addr[IEEEBYTES], object_mac_addr[IEEEBYTES];
    uchar      ssap, dsap;
    long       mtu_size;
    u_portid_type_t     prp_id;
    csm_rc_t            csm_rc;
    ssp_control_frame_t *ssp_hdr;

    netbios_header      *nb_hdr;
    lcache_t            *cache_ptr;
    char                *str;

    if (TIMER_RUNNING(csm_expl_delay_timer)) {
        CSMV_DEBUG("\nCSM: Dropping SSP Message - EXPLORER_DELAY");
        datagram_done(pak);
            return PROC_OK;
    }
    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;
    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
        ieee_copy(ssp_hdr->dlc_id.origin_mac_address, origin_mac_addr);
        ieee_copy(ssp_hdr->dlc_id.target_mac_address, object_mac_addr);
	ssap = ssp_hdr->dlc_id.origin_link_sap;
	dsap = ssp_hdr->dlc_id.target_link_sap;
    } else {
        ieee_copy(ssp_hdr->dlc_id.target_mac_address, origin_mac_addr);
        ieee_copy(ssp_hdr->dlc_id.origin_mac_address, object_mac_addr);
	ssap = ssp_hdr->dlc_id.target_link_sap;
	dsap = ssp_hdr->dlc_id.origin_link_sap;
    }

    /*
     * Add name to cache.
     * Only learn remote max lf on ICR_ex and NR_ex. All other frames
     * search for the best lf and the ssp largest frame bits on these
     * frames are not indicative of the max lf for the remote resource.
     */
    if ((ssp_hdr->info_hdr.msg_type1 == SSP_OP_icanreach) ||
        (ssp_hdr->info_hdr.msg_type1 == SSP_OP_netbios_nr)) {
        if (peer_get_vendor_type(peer_handle) == PEER_NOT_CISCO) {
            mtu_size = ssp_lf_bits_to_bytes(ssp_hdr->largest_frame_bits);
        } else {
            mtu_size = rc_code_to_bytes(ssp_hdr->largest_frame_bits &
                                        RC_LFMASK);
        }
        if (mtu_size <= 0) {
            /* assume maximum */
            CSME_DEBUG("\nCSM: corrupt ssp lf bits %x - mtu %d",
                       ssp_hdr->largest_frame_bits, mtu_size);
            mtu_size = RC_LF17800_VAL;
        }
        mtu_size = min(mtu_size, (peerid_to_mtu(peer_handle)));
        mtu_size = min(mtu_size, BOX_MTU_SIZE);
    } else {
        /*
         * Setting mtu_size to zero in remote reach cache will cause
         * mtu_size to be verified before it is used.
         */
         mtu_size = 0;
    }

    mtu_size = min(mtu_size, BOX_MTU_SIZE);
    prp_id.peer_id = peer_handle;
    /*
     * if peer is a pod having acl defined and answer to this pak would
     * be denied by pod-defaults acl then drop pak here to avoid
     * connecting the pod.
     */
    if (!pod_pass_mac_acl(peer_handle, origin_mac_addr)) {
	CSMV_DEBUG("\nCSM: answer to packet from %e WOULD NOT pass pod acl",
		   origin_mac_addr);
	datagram_done(pak);
	return PROC_OK;
	
    }
    mac_found_update_cache(origin_mac_addr, REMOTE, &prp_id, NULL, mtu_size);

    /* 
     * while we are filtering away frames, lets check for the
     * the local exclusivity filters, if the target address is
     * not a group address. Filter only specific address.
     * Note that this is also applied to Netbios frames with 
     * specific mac address. Apart from this filter, netbios 
     * frames will pass through a similar filtering mechanism 
     * for netbios names.
     */
    if (!(object_mac_addr[0] & TR_GROUP)) {
        if (!(csm_local_mac_excl_pass(object_mac_addr))) {
            CSMV_DEBUG("\nCSM: %e DID NOT pass local mac excl. filter", 
                      object_mac_addr);
            datagram_done(pak);
            return PROC_OK;
        }
    }

    /* 
     * what the heck. let's check for the netbios frame
     * exclusivity filters too!
     * And while we are at it, update netbios name cache
     * if it passes the exclusivity filters.
     */
    if ((dsap == 0xF0) || (ssap == 0xF0)) {
        nb_hdr = (netbios_header *) ((char *) pak->datagramstart +
                                              sizeof(ssp_control_frame_t) +
                                              TRING_ENCAPBYTES +
                                              SRB_MAXRIF +
                                              SAP_HDRBYTES);
        switch (nb_hdr->command) {
          case NETBIOS_NAME_QUERY:
          case NETBIOS_NAME_RECOGNIZED:
          case NETBIOS_STATUS_QUERY:
            /* 
             * if the exclusivity filters are configured, be careful
             * with group names. The filter is applied to all names
             * including group names. This may change later but for
             * now, that is a limitation.
             */

            /* filter on destination name */
            if (!(csm_local_nb_excl_pass(nb_hdr->destname))) {
                NETB_CSMV_DEBUG("\nCSM: %15s DID NOT pass local name excl. filter", 
                          nb_hdr->destname);
                datagram_done(pak);
                return PROC_OK;
            }
            break;
          default:
            /*
             * No filters for the other types of frames
             */
            break;
        }
        /*
         * Add to netbios reachability cache
         */
        switch (nb_hdr->command) {
          case NETBIOS_DATAGRAM:
          case NETBIOS_DATAGRAM_BROADCAST:
          case NETBIOS_NAME_QUERY:
          case NETBIOS_NAME_RECOGNIZED:
            if (nb_hdr->command == NETBIOS_NAME_RECOGNIZED) {
                if (peer_get_vendor_type(peer_handle) == PEER_NOT_CISCO) {
                    mtu_size = ssp_lf_bits_to_bytes(ssp_hdr->largest_frame_bits);
                } else {
                    mtu_size = rc_code_to_bytes(ssp_hdr->largest_frame_bits &
                                                RC_LFMASK);
                }
                if (mtu_size <= 0) {
                      /* assume maximum */
                    NETB_CSM_DEBUG("\nCSM: corrupt ssp lf bits %x - mtu %d",
                                   ssp_hdr->largest_frame_bits, mtu_size);
                    mtu_size = RC_LF17800_VAL;
                }
                mtu_size = min(mtu_size, (peerid_to_mtu(peer_handle)));
            } else {
                mtu_size = 0;
            }
            mtu_size = min(mtu_size, BOX_MTU_SIZE);
            prp_id.peer_id = peer_handle;

	    if (!pod_pass_nb_acl(peer_handle, nb_hdr)) {
		NETB_CSMV_DEBUG("\nCSM: answer to packet from %15s WOULD NOT pass pod acl",
				nb_hdr->srcname);
		datagram_done(pak);
		return PROC_OK;
	    }
            name_found_update_cache(nb_hdr->srcname, REMOTE, 
                                    &prp_id, NULL, mtu_size);
            break;
          default:
            /* 
             * Do not update the cache.
             */
            break;
        }

        /*
         * To prevent re-exploring on a SABME on a stale cache entry
         */
        if (nb_hdr->command == NETBIOS_NAME_RECOGNIZED) {
            get_mac_cache_entries(origin_mac_addr, &cache_ptr,
                                  NULL, NULL, NULL);
            if (cache_ptr != NULL) {
                if (cache_ptr->status == FOUND) {
                    GET_TIMESTAMP(cache_ptr->verify_tstamp);
                }
            }
            get_mac_cache_entries(object_mac_addr, NULL, NULL,
                                  &cache_ptr, NULL);
            if (cache_ptr != NULL) {
                if (cache_ptr->status == FOUND) {
                    GET_TIMESTAMP(cache_ptr->verify_tstamp);
                }
            }
        }
    }
    str = string_getnext();
    switch (ssp_hdr->info_hdr.msg_type1) {
      case SSP_OP_netbios_nq:
      case SSP_OP_netbios_nr:
      case SSP_OP_netbios_anq:
      case SSP_OP_netbios_anr:
        NETB_CSMV_DEBUG("\nCSM: Received SSP %s, csex flags = %x, mac %e, %s",
	     dlsw_sspop2str(ssp_hdr->info_hdr.msg_type1), ssp_hdr->csex_flags,
             origin_mac_addr, peer_handle_to_str(peer_handle, str));
        break;
      default:
        CSMV_DEBUG("\nCSM: Received SSP %s csex flags = %x, mac %e, %s",
	     dlsw_sspop2str(ssp_hdr->info_hdr.msg_type1), ssp_hdr->csex_flags,
             origin_mac_addr, peer_handle_to_str(peer_handle, str));
    }
    switch (ssp_hdr->info_hdr.msg_type1) {
      case SSP_OP_canureach:
        if (ssp_hdr->csex_flags & SSP_FLAGS_EX) {
            csm_rc = ssp_cur_ex_proc(peer_handle, pak);
        }
        else {
            csm_rc = ssp_cur_cs_proc(peer_handle, pak);
        }
        break;
      case SSP_OP_icanreach:
        if (ssp_hdr->csex_flags & SSP_FLAGS_EX) {
            csm_rc = ssp_icr_ex_proc(peer_handle, pak);
        }
        else {
            CSME_DEBUG("\nCSM: SSP_OP_icanreach_cs message, FSM should handle it!");
	    csm_rc = FRAME_REJ;
        }
        break;
      case SSP_OP_netbios_nq:
        csm_rc = ssp_nbnq_proc(peer_handle, pak);
        break;
      case SSP_OP_netbios_nr:
        csm_rc = ssp_nbnr_proc(peer_handle, pak);
        break;
      case SSP_OP_netbios_anq:
      case SSP_OP_netbios_anr:
        csm_rc = ssp_dataframe_proc(peer_handle, pak);
        break;
      case SSP_OP_dataframe:
	if (ssp_hdr->dlc_id.target_link_sap == 0xf0) {
	    csm_rc = ssp_dataframe_proc(peer_handle, pak);
        } else {
	    csm_rc = ssp_nonnb_frame_proc(peer_handle, pak);
        }
	break;
      default:
        CSME_DEBUG("\nDLX message type %c not known to CSM",
                      ssp_hdr->info_hdr.msg_type1);
	csm_rc = FRAME_REJ;
        break;
    }

    return csm_rc;
}




/*
 * Process SSP_OP_canureach messages
 */
static csm_rc_t ssp_cur_ex_proc (peerhandle_t ph, paktype *pak)
{
    int                  ii, rc, reach_index;
    uchar                ws_status;
    ushort               no_in_list;
    e_xmission_type      trans_type;
    lportid_t            port_list[MAX_REACH];
    long                 mtu_size;
    char                 object_mac_addr[IEEEBYTES];
    char                 origin_mac_addr[IEEEBYTES];
    char                 obj_sap;
    char                 ori_sap;
    boolean              add_rpend, discard_packet, ring_list_match;
    csm_rc_t             csm_rc;

    ssp_control_frame_t  *ssp_hdr;
    record_handle        rhandle;
    lcache_t             *cache_ptr, ws_cache;
    resp_pend_t          *resp_pend, ws_rpend;
    u_portid_type_t      o_prp_id;


    discard_packet = FALSE;
    rc = 0;

    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;
    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.target_link_sap;
	ori_sap = ssp_hdr->dlc_id.origin_link_sap;
    } else {   /* cur dirn is always forward. checking jic.... */
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.origin_link_sap;
	ori_sap = ssp_hdr->dlc_id.target_link_sap;
    }

    if (!(ssp_hdr->largest_frame_bits & SSP_LF_IGNORE_BIT)) {
        if (peer_get_vendor_type(ph) == PEER_NOT_CISCO) {
            mtu_size = ssp_lf_bits_to_bytes(ssp_hdr->largest_frame_bits);
        } else {
            mtu_size = rc_code_to_bytes(ssp_hdr->largest_frame_bits &
                                        RC_LFMASK);
        }
        if (mtu_size <= 0) {
            CSME_DEBUG("\nCSM: corrupt ssp lf bits %x - mtu %d",
                       ssp_hdr->largest_frame_bits, mtu_size);
            datagram_done(pak);
            return PROC_OK;
        }
        mtu_size = min(mtu_size, (peerid_to_mtu(ph)));
        mtu_size = min(mtu_size, BOX_MTU_SIZE);
    } else {
        mtu_size = min(BOX_MTU_SIZE, (peerid_to_mtu(ph)));
    }

    /* get lcache info for destination address */
    get_mac_cache_entries(object_mac_addr, &cache_ptr, NULL, NULL, NULL);

    if (cache_ptr == NULL) {
        ws_status = NO_CACHE_INFO;
    } else {
        ws_status = cache_ptr->status;
    }

    add_rpend = FALSE;
    no_in_list = 0;
    trans_type = NO_SEND;
    switch (ws_status) {
      case FOUND:
        ring_list_match = FALSE;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(ph, 
                                  cache_ptr->r_entry[ii].prp_id.port_id)) {
                ring_list_match = TRUE;
                break;
            }
        }
        if (ring_list_match == FALSE) {
            datagram_done(pak);
            return(PROC_OK);
        }

	if (mgd_timer_running(&cache_ptr->del_tstamp)) {
	    if ((mgd_timer_left_sleeping(&cache_ptr->del_tstamp) <=
		 csm_touch_sna_cache_active_to)) {
		 mgd_timer_start(&cache_ptr->del_tstamp,
				 csm_touch_sna_cache_reset_to);
	    }
	}	

        if (CLOCK_OUTSIDE_INTERVAL(cache_ptr->verify_tstamp,
                              csm_sna_cache_verify_to)) {
            /* Time to verify cache entries */
            CSMV_DEBUG("\nCSM: CUR_ex received Time to verify cache entries");
            no_in_list = cache_ptr->no_in_list;
            /*
             * we are in FOUND which means we have a rif. let's try the
             * existing rif first, if not successful we'll retry an explorer
             * on that port (done by background process).
             */
	    for (ii = 0; ii < no_in_list; ii++) {
                csm_lan_put (SPEC_LAN_PORT, 
		             cache_ptr->r_entry[ii].prp_id.port_id, 
	                     LLC_TEST, CMDBIT, P_BIT, 
                             object_mac_addr, origin_mac_addr, 0, ori_sap,
	                     DIRECT_FRAME, cache_ptr->r_entry[ii].rif,
                             cache_ptr->r_entry[ii].mtu_size, NULL, 0, ph);
                port_list[ii] = cache_ptr->r_entry[ii].prp_id.port_id;
	        if (cache_ptr->cache_type == DYNAMIC) {
	            cache_ptr->r_entry[ii].prp_id.port_id = 0;
		    cache_ptr->r_entry[ii].flags = 0;
		    --cache_ptr->no_in_list;
	        } else {
	            cache_ptr->r_entry[ii].flags &= ~AVAILABLE;
	        }
	    }
	    cache_ptr->status = VERIFY;
            GET_TIMESTAMP(cache_ptr->verify_tstamp);
            mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
            add_rpend = TRUE;
	    discard_packet = TRUE;
	} else {
            o_prp_id.peer_id = ph;
            if (ssp_hdr->largest_frame_bits & SSP_LF_IGNORE_BIT) {
                reach_index = get_reach_entry_mac(cache_ptr, 0, 
                                                  &o_prp_id, FALSE);
            } else {
                reach_index = get_reach_entry_mac(cache_ptr, mtu_size, 
                                                  &o_prp_id, FALSE);
            }
            if (reach_index >= 0) {
	        /*
	         * use the same pak. Just flip the direction, fill in the rest
	         * of the stuff, and send the packet to WAN
	         */
                ssp_hdr->info_hdr.msg_type1 = SSP_OP_icanreach;
                ssp_hdr->msg_type2 = SSP_OP_icanreach;
                mtu_size = min(mtu_size,
                               cache_ptr->r_entry[reach_index].mtu_size);
                if (peer_get_vendor_type(ph) == PEER_NOT_CISCO) {
                    ssp_hdr->largest_frame_bits=bytes_to_ssp_lf_bits(mtu_size);
                } else {
                    ssp_hdr->largest_frame_bits = bytes_to_rc_code(mtu_size);
                }
                if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
	            ssp_hdr->frame_direction = SSP_DIRN_BACKWARD;
	            ssp_hdr->target_id.dlc_port_id = DLSW_LPORT_ID_COOKIE;
	            ssp_hdr->target_id.dlc_corr = 0;
                    ssp_hdr->target_id.transport_id = DLSW_XPORT_ID_COOKIE;
                } else {
	            ssp_hdr->frame_direction = SSP_DIRN_FORWARD;
	            ssp_hdr->origin_id.dlc_port_id = DLSW_LPORT_ID_COOKIE;
	            ssp_hdr->origin_id.dlc_corr = 0;
                    ssp_hdr->origin_id.transport_id = DLSW_XPORT_ID_COOKIE;
                }
                csm_peer_put (ph, pak, origin_mac_addr, object_mac_addr, 
		               0, 0, PEER_SSP_TYPE, NULL);
                add_rpend = FALSE;
            } else {
		CSMV_DEBUG("\nCSM: cur_ex ignored - fresh, but no reach");
                discard_packet = TRUE;
                add_rpend = FALSE;
            }
        }
	csm_rc = PROC_OK;
        break;

      case VERIFY:
        add_rpend = TRUE;
	discard_packet = TRUE;
	csm_rc = PROC_OK;
        break;

      case SEARCHING:
        /*
         * If we are in SEARCHING state due to local switching, delete the
         * searching state cache record and treat this CUR_ex as one where
         * no cache information is available for this destination mac address.
         * Else just add the information in the response pending record and
         * discard the CUR_ex packet (earlier behaviour)
         */
        rhandle = seek_record(RESP_PEND, object_mac_addr, (void **) &resp_pend);
        while (rhandle != NULL) {
           if (resp_pend->peer_id != 0) {
               break;
           }
           rhandle = read_next_record(RESP_PEND, rhandle, object_mac_addr,
                                      (void **) &resp_pend);
        }
 
        if (rhandle != NULL) {
            add_rpend = TRUE;
            discard_packet = TRUE;
            csm_rc = PROC_OK;
            break;
        } else {
            delete_csm_cache(LCACHE_MAC, object_mac_addr, NULL, FORCE,
                             DYNAMIC);
            /* Fall through for the case when there is no cache information */
        }

      case NO_CACHE_INFO:
        rc = csm_lan_put (ALL_LAN_PORTS, NULL, LLC_TEST, CMDBIT, P_BIT, 
                          object_mac_addr, origin_mac_addr, 0, ori_sap,
		          dlsw_sna_xcast_flag, NULL, BOX_MTU_SIZE, NULL, 0, ph);
        if (rc > 0) {
            ws_cache.cache_type = DYNAMIC;
            ws_cache.status = SEARCHING;
            ws_cache.reach_direction = LOCAL;
	    ws_cache.no_in_list = 0;
            for (ii = 0; ii < MAX_REACH; ii++) {
                ws_cache.r_entry[ii].prp_id.port_id = 0;
                ws_cache.r_entry[ii].flags = 0;
            }
            memset(&ws_cache.del_tstamp, 0, sizeof(ws_cache.del_tstamp));
            memset(&ws_cache.explore_tstamp, 0, sizeof(ws_cache.explore_tstamp));
            rhandle = add_record(LCACHE_MAC, object_mac_addr, &ws_cache);
            if (rhandle == NULL) {
                CSME_DEBUG("\nCSM: Error adding cache info - ssp_cur_ex_proc");
            } else {
                lcache_t      *tmp_ptr;
                tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
                mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                    &csm_cache_expl_timer,
                                    CSM_LMAC_CACHE, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_cache_del_timer,
                                    CSM_LMAC_CACHE, rhandle, FALSE);
                mgd_timer_stop(&tmp_ptr->explore_tstamp);
                mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_expl_to);
                GET_TIMESTAMP(tmp_ptr->verify_tstamp);

                add_rpend = TRUE;
	        trans_type = ALL_LAN_PORTS;
	        no_in_list = 0;
            }
        }
	discard_packet = TRUE;
	csm_rc = PROC_OK;
        break;

      case UNCONFIRMED:
	/* we may not know the correct rif, so better send explorer */
	no_in_list = 0;
	trans_type = SPEC_LAN_PORT;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            csm_lan_put (SPEC_LAN_PORT, 
		         cache_ptr->r_entry[ii].prp_id.port_id, 
			 LLC_TEST, CMDBIT, P_BIT, 
                         object_mac_addr, origin_mac_addr, 0, ori_sap,
	                 dlsw_sna_xcast_flag, NULL, BOX_MTU_SIZE, NULL, 0, ph);
            port_list[no_in_list] = cache_ptr->r_entry[ii].prp_id.port_id;
	    ++no_in_list;
        }
        cache_ptr->status = VERIFY;
        mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
        add_rpend = TRUE;
	discard_packet = TRUE;
	csm_rc = PROC_OK;
        break;

    case NOT_FOUND:
        discard_packet = TRUE;
	csm_rc = PROC_OK;
        break;

    default:
        CSME_DEBUG("\nCSM: Invalid cache status - %s", csm_stat(ws_status));
	discard_packet = TRUE;
	csm_rc = PROC_OK;
        break;
    }

    if (add_rpend) {
        rhandle = seek_record(RESP_PEND, object_mac_addr, (void **) &resp_pend);
        while (rhandle != NULL) {
            if (resp_pend->peer_id == ph) {
                break;
            }
	    rhandle = read_next_record(RESP_PEND, rhandle, 
				       object_mac_addr, 
				       (void **) &resp_pend);
        }
	if (rhandle == NULL) {
	    ieee_copy(origin_mac_addr, ws_rpend.saddr);
            ws_rpend.ssap = ori_sap;
            ws_rpend.peer_id = ph;
	    ws_rpend.flags = 0;
	    ws_rpend.rem_transport_id = ph;
	    ws_rpend.trans_type = trans_type;
            ws_rpend.localsw_portid = 0;
	    ws_rpend.no_in_list = no_in_list;
            ws_rpend.mtu_size = mtu_size;

            /*
             * Always ignore the mtu in CUR_ex even if other vendors don't
             * set it as they should.
             */
            ws_rpend.mtu_flag = SSP_LF_IGNORE_BIT;

            ws_rpend.eat_test_resp = FALSE;
	    for (ii = 0; ii < no_in_list; ii++) {
		ws_rpend.port_id[ii] = port_list[ii];
            }
	    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
	        ws_rpend.rem_lanport_id = ssp_hdr->origin_id.dlc_port_id;
		ws_rpend.rem_transport_id = ssp_hdr->origin_id.transport_id;
            } else {
	        ws_rpend.rem_lanport_id = ssp_hdr->target_id.dlc_port_id;
		ws_rpend.rem_transport_id = ssp_hdr->target_id.transport_id;
            }
            memset(&ws_rpend.del_tstamp, 0, sizeof(ws_rpend.del_tstamp));
            memset(&ws_rpend.retry_tstamp, 0, sizeof(ws_rpend.retry_tstamp));
            rhandle = add_record(RESP_PEND, object_mac_addr, &ws_rpend);
            if (rhandle == NULL) {
                CSME_DEBUG("\nCSM: Error adding to response pending database - ssp_cur_ex_proc");
            } else {
                resp_pend_t          *tmp_ptr;
                tmp_ptr = (resp_pend_t *) rhandle_to_data(rhandle);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_resp_del_timer,
                                    0, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->retry_tstamp,
                                    &csm_resp_retry_timer,
                                    0, rhandle, FALSE);
                mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_expl_to);
                if (trans_type != NO_SEND) {
                    mgd_timer_start(&tmp_ptr->retry_tstamp, csm_sna_expl_rt);
                } else {
                    mgd_timer_stop(&tmp_ptr->retry_tstamp);
                }
            }
        }
    }

    if (discard_packet) {
	datagram_done(pak);
    }

    return csm_rc;
}





/*
 * Process SSP_OP_icanreach messages
 */
static csm_rc_t ssp_icr_ex_proc (peerhandle_t ph, paktype *pak)
{

    int        reach_index;
    boolean    no_test_response;
    boolean    delete_startdl_pend;
    char       object_mac_addr[IEEEBYTES], origin_mac_addr[IEEEBYTES];
    char       obj_sap, ori_sap;
    long       mtu_size;
    uchar      *test_data;
    int        test_data_len;

    csm_rc_t   core_rc;
    ssp_control_frame_t *ssp_hdr;
    u_portid_type_t     prp_id, o_prp_id;
    record_handle       rhandle;
    lcache_t            *cache_ptr, *rem_cache_ptr;
    icr_pend_t          *icr_pend;
    startdl_pend_t      *startdl_pend;
    u_msg_ptr_t         u_msg;
    uchar               *daddr;
    uchar               ssap, dsap;
    TCLSIMsg            *clsi_msg;
    reach_entry_t       reach_entry;

    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;
    if (ssp_hdr->frame_direction == SSP_DIRN_BACKWARD) {
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.target_link_sap;
	ori_sap = ssp_hdr->dlc_id.origin_link_sap;
    } else {
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.origin_link_sap;
	ori_sap = ssp_hdr->dlc_id.target_link_sap;
    }
    if (peer_get_vendor_type(ph) == PEER_NOT_CISCO) {
        mtu_size = ssp_lf_bits_to_bytes(ssp_hdr->largest_frame_bits);
    } else {
        mtu_size = rc_code_to_bytes(ssp_hdr->largest_frame_bits & RC_LFMASK);
    }
    if (mtu_size <= 0) {
        /* assume maximum */
        CSM_DEBUG("\nCSM: corrupt ssp lf bits %x - mtu %d",
                  ssp_hdr->largest_frame_bits, mtu_size);
        mtu_size = RC_LF17800_VAL;
    }
    mtu_size = min(mtu_size, (peerid_to_mtu(ph)));
    mtu_size = min(mtu_size, BOX_MTU_SIZE);

    /*
     * If the station is reachable locally, we do not want to respond
     * to the test frames. So discard icanreach if station is
     * reachable locally.
     */
    no_test_response = FALSE;
    get_mac_cache_entries(object_mac_addr, &cache_ptr, NULL,
                          &rem_cache_ptr, NULL);
    /*
     * If we can find the target mac locally or if it's blocked locally,
     * then do not send test response.
     */
    if ((cache_ptr != NULL) &&
        ((cache_ptr->status == NOT_FOUND) ||
         (cache_ptr->status != SEARCHING))) {
        no_test_response = TRUE;
        CSMV_DEBUG("\nCSM: Station has a local record, state = %s",
                   csm_stat(cache_ptr->status));
    }

    /*
     * If we are waiting for other ICR_ex to arrive, drop frame.
     * If we are not going to send the test response due to other reasons,
     * then fall through.
     *
     * FB - Why would we fall through if waiting.
     */
    if ((no_test_response != TRUE) && (csm_expl_wait_rt)) {
        datagram_done(pak);
        return PROC_OK;
    }

    rhandle = seek_record(ICR_PEND, object_mac_addr, (void **) &icr_pend);
    while (rhandle != NULL) {
      if (no_test_response == FALSE) {
            if (peer_pass_port_filter(ph, icr_pend->port_id)) {
                /*
                 * We always set icr_pend->mtu_flag with SSP_LF_IGNORE_BIT.
                 */
                if ((icr_pend->mtu_size >= mtu_size) ||
                    (icr_pend->mtu_flag & SSP_LF_IGNORE_BIT)) {
	            if (icr_pend->test_data_len) {
		        test_data_len = icr_pend->test_data_len;
		        test_data = icr_pend->test_data;
                    } else {
		        test_data_len = 0;
		        test_data = NULL;
                    }
                    csm_lan_put (SPEC_LAN_PORT, icr_pend->port_id, 
			         (icr_pend->ctrl & ~P_BIT), RSPBIT, 
			         (icr_pend->ctrl & P_BIT), 
                                 icr_pend->saddr, object_mac_addr, 
			         icr_pend->ssap, icr_pend->dsap,
		                 DIRECT_FRAME, icr_pend->rif, 
                                 min(mtu_size, icr_pend->mtu_size),
			         test_data, test_data_len, ph);
                    mgd_timer_stop(&icr_pend->del_tstamp);
                    mgd_timer_stop(&icr_pend->respond_timer);
                    rhandle = delete_record_get_next(ICR_PEND, rhandle);
                    if (!(match_record_key(rhandle, object_mac_addr))) {
                        rhandle = NULL;
                        icr_pend = NULL;
                    } else {
                        icr_pend = rhandle_to_data(rhandle);
                    }
                    continue;
                }
            }
        } else {
            mgd_timer_stop(&icr_pend->del_tstamp);
            mgd_timer_stop(&icr_pend->respond_timer);
            rhandle = delete_record_get_next(ICR_PEND, rhandle);
            if (!(match_record_key(rhandle, object_mac_addr))) {
                rhandle = NULL;
                icr_pend = NULL;
            } else {
                icr_pend = rhandle_to_data(rhandle);
            }
            continue;
        }
        rhandle = read_next_record(ICR_PEND, rhandle, object_mac_addr, 
				   (void **) &icr_pend);
    }

    if ((rem_cache_ptr == NULL) || (rem_cache_ptr->status != FOUND)) {
        CSM_DEBUG("\nCSM: Cache not found or cache status is not FOUND");
	no_test_response = TRUE;
    }

    reach_index = 0;
    rhandle = seek_record(STARTDL_PEND, object_mac_addr, 
					(void **) &startdl_pend);
    while (rhandle) {
        delete_startdl_pend = TRUE;
	if (no_test_response == FALSE) {
            if (startdl_pend->type == CLSI_MSG) {
                o_prp_id.port_id = startdl_pend->prp_id.port_id;

                /*
                 * If the startdl_pend record indicates to ignore it's
                 * mtu size or if the lan type does not support lf 
                 * negotiation, then use the icr_ex mtu for circuit.
                 */
               if (!(startdl_pend->mtu_flag & SSP_LF_IGNORE_BIT) &&
                    (!dlsw_ignore_port_lf(o_prp_id.port_id))) {
                    mtu_size = min(mtu_size, startdl_pend->mtu_size);
                }

                reach_index = get_reach_entry_mac(rem_cache_ptr, mtu_size,
                                                      &o_prp_id, TRUE);
	        if (reach_index >= 0) {
                    if ((startdl_pend->mtu_flag & SSP_LF_IGNORE_BIT) ||
                        (dlsw_ignore_port_lf(o_prp_id.port_id))) {
                        mtu_size = min(mtu_size,
                                rem_cache_ptr->r_entry[reach_index].mtu_size);
                    }

                    if ((startdl_pend->mtu_size <= 
                         rem_cache_ptr->r_entry[reach_index].mtu_size) ||
                        (startdl_pend->mtu_flag & SSP_LF_IGNORE_BIT) ||
                        (dlsw_ignore_port_lf(o_prp_id.port_id))) {
                        if (is_fast_peer(ph)) {
                            clsi_msg = startdl_pend->u_msg_ptr.clsi_msg;
                            daddr = get_daddr_from_clsi(clsi_msg);
                            dsap = get_dsap_from_clsi(clsi_msg);
                            ssap = get_ssap_from_clsi(clsi_msg);
                            if (daddr == NULL) {
                                CSME_DEBUG("\nCSM: Error parsing clsi message - ssp_icr_ex_proc");
                            } else {
                                /* fast cache needs RII set */
                                daddr[IEEEBYTES] |= TR_RII;
                                dlsw_fct_lan2wan_start(clsi_msg, ph,
                                                       daddr, dsap, ssap);
                            }
                            free_clsi_msg(clsi_msg);
                        } else {
                            u_msg.clsi_msg = startdl_pend->u_msg_ptr.clsi_msg;
                            prp_id.port_id = startdl_pend->prp_id.port_id;
                            bcopy(&(rem_cache_ptr->r_entry[reach_index]),
                                  &reach_entry, sizeof(reach_entry_t));
                            reach_entry.mtu_size = mtu_size;
                            CSMV_DEBUG("\nCSM: csm_to_core with CLSI_START_NEWDL");
                            core_rc = csm_to_core(CLSI_START_NEWDL, 
                                                  &prp_id, &u_msg, 
                                                  &reach_entry);
                            if (core_rc == FRAME_REJ) {
                                CSM_DEBUG("\nCSM: Core rejected SSP start circuit");
                                free_clsi_msg(startdl_pend->u_msg_ptr.clsi_msg);
                            } else if (core_rc == POD_PEER_NOT_READY) {
                                /* save this message and retry later */
				bcopy(&reach_entry, 
                                      &startdl_pend->retry_reach_info,
                                      sizeof(reach_entry_t));
                                mgd_timer_start(&startdl_pend->retry_tstamp,
                                                csm_pod_retry_to);
                                delete_startdl_pend = FALSE;
                            }
                        }
                        if (delete_startdl_pend) {
                            mgd_timer_stop(&startdl_pend->del_tstamp);
                            mgd_timer_stop(&startdl_pend->retry_tstamp);
                            mgd_timer_stop(&startdl_pend->respond_timer);
                            rhandle = delete_record_get_next(STARTDL_PEND, 
                                                             rhandle);
                            if (!(match_record_key(rhandle, object_mac_addr))) {
                                rhandle = NULL;
                                startdl_pend = NULL;
                            } else {
                                startdl_pend = rhandle_to_data(rhandle);
                            }
                            continue;
                        }
                    }
                } else {
                    if (reach_index == CSM_NO_RING_MATCH) {
                        CSMV_DEBUG("\nCSM: Ring list did not match");
                    } else {
	                CSME_DEBUG("\nCSM: Reach index error %s", 
                                    csm_error(reach_index));
                    }
                }
            }
        } else {
            if (startdl_pend->type == CLSI_MSG) {
                free_clsi_msg(startdl_pend->u_msg_ptr.clsi_msg);
            } else {
                datagram_done(startdl_pend->u_msg_ptr.pak);
                startdl_pend->u_msg_ptr.pak = NULL;
            }
            mgd_timer_stop(&startdl_pend->del_tstamp);
            mgd_timer_stop(&startdl_pend->retry_tstamp);
            mgd_timer_stop(&startdl_pend->respond_timer);
            rhandle = delete_record_get_next(STARTDL_PEND, rhandle);
            if (!(match_record_key(rhandle, object_mac_addr))) {
                rhandle = NULL;
                startdl_pend = NULL;
            } else {
                startdl_pend = rhandle_to_data(rhandle);
            }
            continue;
        }
	rhandle = read_next_record(STARTDL_PEND, rhandle, object_mac_addr,
				   (void **) &startdl_pend);
    }
    datagram_done(pak);
    return PROC_OK;

}



/*
 * Process SSP_OP_canureach_cs messages
 */
static csm_rc_t ssp_cur_cs_proc (peerhandle_t ph, paktype *pak)
{
    int         ii, rc;
    int         ws_rc;
    int         reach_index;
    ushort      no_in_list;
    char        ws_status;
    char        object_mac_addr[IEEEBYTES], origin_mac_addr[IEEEBYTES];
    char        obj_sap, ori_sap;
    char        rc_code;
    long        mtu_size, peer_mtu_size;
    boolean     skip_flag;
    boolean     add_cache_entry;
    boolean     add_startdl_pend;
    boolean     discard_packet, ring_list_match;

    csm_rc_t            core_rc;
    e_xmission_type     trans_type;
    resp_pend_t         *resp_pend, ws_rpend;
    csm_rc_t            csm_rc;
    ssp_control_frame_t *ssp_hdr;
    record_handle       rhandle;
    lcache_t            *cache_ptr, ws_cache;
    startdl_pend_t      *startdl_pend, ws_startdl_pend;
    reach_entry_t       reach_entry;
    u_msg_ptr_t         u_msg;
    u_portid_type_t     prp_id, o_prp_id;
    lportid_t           port_list[MAX_REACH];
    uchar		*rif_list[MAX_REACH];

    rc = 0;

    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;
    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.target_link_sap;
	ori_sap = ssp_hdr->dlc_id.origin_link_sap;
    } else {
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.origin_link_sap;
	ori_sap = ssp_hdr->dlc_id.target_link_sap;
    }

    if (peer_get_vendor_type(ph) == PEER_NOT_CISCO) {
        mtu_size = ssp_lf_bits_to_bytes(ssp_hdr->largest_frame_bits);
    } else {
        mtu_size = rc_code_to_bytes(ssp_hdr->largest_frame_bits & RC_LFMASK);
    }
    if (mtu_size <= 0) {
        if (ssp_hdr->largest_frame_bits & SSP_LF_IGNORE_BIT) {
            /* assume maximum */
            CSM_DEBUG("\nCSM: corrupt ssp lf bits %x - mtu %d",
                      ssp_hdr->largest_frame_bits, mtu_size);
            mtu_size = RC_LF17800_VAL;
        } else {
            CSME_DEBUG("\nCSM: corrupt mtu size in ssp frame %d", mtu_size);
            datagram_done(pak);
            return PROC_OK;
        }
    }

    peer_mtu_size = min((peerid_to_mtu(ph)), BOX_MTU_SIZE);

    /* if the peer cannot handle the lf size requested.... */
    if (!(ssp_hdr->largest_frame_bits & SSP_LF_IGNORE_BIT)) {
        if (peer_mtu_size < mtu_size) {
            CSME_DEBUG("\nCSM: Peer lf %d less than CUR_cs lf %d",
                       peer_mtu_size, mtu_size);
            datagram_done(pak);
            return PROC_OK;
        }
    } else {
        mtu_size = min(mtu_size, peer_mtu_size);
    }

    /* get lcache info for destination address */
    get_mac_cache_entries(object_mac_addr, &cache_ptr, &rhandle, NULL, NULL);
    if (cache_ptr == NULL) {
        ws_status = NO_CACHE_INFO;
    }
    else {
        ws_status = cache_ptr->status;
    }

    CSMV_DEBUG("\nCSM: CanUReach-CS frame...cache status is %s", 
                csm_stat(ws_status));
    add_startdl_pend = FALSE;
    add_cache_entry = FALSE;
    discard_packet = FALSE;
    trans_type = NO_SEND;
    no_in_list = 0;
    switch (ws_status) {
      case FOUND:
        ring_list_match = FALSE;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(ph, 
                      cache_ptr->r_entry[ii].prp_id.port_id)) {
                ring_list_match = TRUE;
                break;
            }
        }
        if (ring_list_match == FALSE) {
            datagram_done(pak);
            return(PROC_OK);
        }

	if (mgd_timer_running(&cache_ptr->del_tstamp)) {
	    if ((mgd_timer_left_sleeping(&cache_ptr->del_tstamp) <=
		 csm_touch_sna_cache_active_to)) {
		 mgd_timer_start(&cache_ptr->del_tstamp,
				 csm_touch_sna_cache_reset_to);
	    }
	}	

        if (CLOCK_OUTSIDE_INTERVAL(cache_ptr->verify_tstamp,
                              csm_sna_cache_verify_to)) {
            /* Time to verify cache entries */
            CSMV_DEBUG("\nCSM: CUR_cs received Time to verify cache entries");
	    trans_type = SPEC_LAN_PORT;
	    no_in_list = cache_ptr->no_in_list;
            for (ii = 0; ii < no_in_list; ii++) {
	        port_list[ii] = cache_ptr->r_entry[ii].prp_id.port_id;
		if (cache_ptr->cache_type == DYNAMIC) {
		    cache_ptr->r_entry[ii].prp_id.port_id = 0;
		    rif_list[ii] = cache_ptr->r_entry[ii].rif;
		    cache_ptr->r_entry[ii].flags = 0;
		    --cache_ptr->no_in_list;
		} else {
		    cache_ptr->r_entry[ii].flags &= ~AVAILABLE;
		}
	    }
	    add_startdl_pend = TRUE;
	    cache_ptr->status = VERIFY;
            GET_TIMESTAMP(cache_ptr->verify_tstamp);
            mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
	} else {
            o_prp_id.peer_id = ph;
            if ((ssp_hdr->largest_frame_bits & SSP_LF_IGNORE_BIT) ||
                (dlsw_ignore_port_lf(cache_ptr->r_entry[ii].prp_id.port_id))) {
	        reach_index = get_reach_entry_mac(cache_ptr, 0, 
                                                  &o_prp_id, TRUE);
            } else {
	        reach_index = get_reach_entry_mac(cache_ptr, mtu_size, 
                                                  &o_prp_id, TRUE);
            }
	    if (reach_index < 0) {
                /*
                 * we have already checked for ring list match, so this
                 * can only be one of the other errors.
                 */
	        CSME_DEBUG("\nCSM: Reach index error %s", 
                           csm_error(reach_index));
	        discard_packet = TRUE;
            } else {
	        bcopy(&(cache_ptr->r_entry[reach_index]), &reach_entry,
		        sizeof(reach_entry_t));
                reach_entry.mtu_size = min(reach_entry.mtu_size, mtu_size);
                if (reach_entry.rif[0] != 0) {
                    rc_code = bytes_to_rc_code(reach_entry.mtu_size);
                    reach_entry.rif[1] = (reach_entry.rif[1] & ~RC_LFMASK)
                                              | rc_code;
                }
	        u_msg.pak = pak;
	        prp_id.peer_id = ph;
                CSMV_DEBUG("\nCSM: csm_to_core with SSP_START_NEWDL");
                core_rc = csm_to_core(SSP_START_NEWDL, &prp_id, &u_msg,
	                              &reach_entry);
                if (core_rc != PROC_OK) {
                    CSME_DEBUG("\nCSM: Core rejected SSP msg to start circuit");
	            discard_packet = TRUE;
                }
            }
            trans_type = NO_SEND;
	    add_startdl_pend = FALSE;
        }
        csm_rc = PROC_OK;
	break;

      case VERIFY:
      case SEARCHING:
        trans_type = NO_SEND;
	add_startdl_pend = TRUE;
        csm_rc = PROC_OK;
	break;

      case NO_CACHE_INFO:
	trans_type = ALL_LAN_PORTS;
	no_in_list = 0;
	add_cache_entry = TRUE;
	add_startdl_pend = TRUE;
	csm_rc = PROC_OK;
	break;

      case UNCONFIRMED:
	trans_type = SPEC_LAN_PORT;
	no_in_list = 0;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
	    port_list[no_in_list] = cache_ptr->r_entry[ii].prp_id.port_id;
	    rif_list[no_in_list] = NULL;
	    ++no_in_list;
        }
        cache_ptr->status = VERIFY;
	add_startdl_pend = TRUE;
	csm_rc = PROC_OK;
        break;

    case NOT_FOUND:
        discard_packet = TRUE;
        trans_type = NO_SEND;
	csm_rc = PROC_OK;
        break;

    default:
        CSME_DEBUG("\nCSM: Invalid cache status %s", csm_stat(ws_status));
        trans_type = NO_SEND;
        discard_packet = TRUE;
	csm_rc = PROC_OK;
        break;
    }

    if (trans_type != NO_SEND) {
        if (trans_type == ALL_LAN_PORTS) {
            rc = csm_lan_put (ALL_LAN_PORTS, NULL, LLC_TEST, CMDBIT, P_BIT, 
                              object_mac_addr, origin_mac_addr, 0, 
			      ori_sap, dlsw_sna_xcast_flag, NULL, 
			      BOX_MTU_SIZE, NULL, 0, ph);
        } else {
	    for (ii = 0; ii < no_in_list; ii++) {
                rc += csm_lan_put (SPEC_LAN_PORT, port_list[ii], LLC_TEST, 
				  CMDBIT, P_BIT, object_mac_addr,
				  origin_mac_addr, 0, ori_sap,
	                          (rif_list[ii] ? DIRECT_FRAME : dlsw_sna_xcast_flag),
                                  rif_list[ii], BOX_MTU_SIZE, NULL, 0, ph);
            }
        }
	if (rc) {
            rhandle = seek_record(RESP_PEND, object_mac_addr,
			          (void **) &resp_pend);
	    if (rhandle == NULL) {
	        ieee_copy(origin_mac_addr, ws_rpend.saddr);
                ws_rpend.ssap = ori_sap;
                ws_rpend.peer_id = 0;    /* just verification */
	        ws_rpend.flags = 0;
	        ws_rpend.rem_transport_id = ph;
	        ws_rpend.trans_type = trans_type;
                ws_rpend.localsw_portid = 0;
	        ws_rpend.no_in_list = no_in_list;
	        for (ii = 0; ii < no_in_list; ii++) {
		    ws_rpend.port_id[ii] = port_list[ii];
                }
	        ws_rpend.rem_lanport_id = 0;
	        ws_rpend.rem_transport_id = 0;
                ws_rpend.eat_test_resp = FALSE;
                memset(&ws_rpend.del_tstamp, 0, sizeof(ws_rpend.del_tstamp));
                memset(&ws_rpend.retry_tstamp, 0, sizeof(ws_rpend.retry_tstamp));
                rhandle = add_record(RESP_PEND, object_mac_addr, &ws_rpend);
                if (rhandle == NULL) {
                    CSME_DEBUG("\nCSM: Error adding to response pending database - ssp_cur_cs_proc");
                } else {
                    resp_pend_t         *tmp_ptr;
                    tmp_ptr = (resp_pend_t *) rhandle_to_data(rhandle);
                    mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                        &csm_resp_del_timer,
                                        0, rhandle, FALSE);
                    mgd_timer_init_leaf(&tmp_ptr->retry_tstamp,
                                        &csm_resp_retry_timer,
                                        0, rhandle, FALSE);
                    mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_expl_to);
                    mgd_timer_start(&tmp_ptr->retry_tstamp, csm_sna_expl_rt);
                }
            }
	    if (add_cache_entry) {
                ws_cache.cache_type = DYNAMIC;
                ws_cache.status = SEARCHING;
                ws_cache.reach_direction = LOCAL;
	        ws_cache.no_in_list = 0;
                for (ii = 0; ii < MAX_REACH; ii++) {
                    ws_cache.r_entry[ii].prp_id.port_id = 0;
                    ws_cache.r_entry[ii].flags = 0;
                }
                memset(&ws_cache.del_tstamp, 0, sizeof(ws_cache.del_tstamp));
                memset(&ws_cache.explore_tstamp, 0, sizeof(ws_cache.explore_tstamp));
                rhandle = add_record(LCACHE_MAC, object_mac_addr, &ws_cache);
                if (rhandle == NULL) {
                    CSME_DEBUG("\nCSM: Error adding cache info - ssp_cur_cs_proc");
                } else {
                    lcache_t      *tmp_ptr;
                    tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
                    mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                        &csm_cache_expl_timer,
                                        CSM_LMAC_CACHE, rhandle, FALSE);
                    mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                        &csm_cache_del_timer,
                                        CSM_LMAC_CACHE, rhandle, FALSE);
                    mgd_timer_stop(&tmp_ptr->explore_tstamp);
                    mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_expl_to);
                    GET_TIMESTAMP(tmp_ptr->verify_tstamp);
                }
            }
        } else {
            CSM_DEBUG("\nCSM: test frame did not go out.....");
            if (add_startdl_pend) {
	        add_startdl_pend = FALSE;
                discard_packet = TRUE;
            }
        }
    }

    if (add_startdl_pend) {
        skip_flag = FALSE;
        rhandle = seek_record(STARTDL_PEND, 
			      object_mac_addr, (void **) &startdl_pend);
        while ((rhandle != NULL) && (skip_flag == FALSE)) {
            ws_rc = ieee_compare(startdl_pend->saddr, origin_mac_addr);
            if (ws_rc == 0) {
                skip_flag = TRUE;
            } else {
                rhandle = read_next_record(STARTDL_PEND, rhandle, 
					   object_mac_addr, 
					   (void **) &startdl_pend);
            }
        }
        if (skip_flag) {
	    discard_packet = TRUE;
            csm_rc = PROC_OK;
        } else {
            ieee_copy(origin_mac_addr, ws_startdl_pend.saddr);
            ws_startdl_pend.type = SSP_MSG;
            ws_startdl_pend.u_msg_ptr.pak = pak;
            ws_startdl_pend.prp_id.peer_id = ph;
            ws_startdl_pend.mtu_size = mtu_size;
            ws_startdl_pend.mtu_flag = ssp_hdr->largest_frame_bits &
                                       SSP_LF_IGNORE_BIT;
            ws_startdl_pend.retry_count = 0;
            memset(&ws_startdl_pend.del_tstamp, 0,
                                   sizeof(ws_startdl_pend.del_tstamp));
            memset(&ws_startdl_pend.retry_tstamp, 0,
                                   sizeof(ws_startdl_pend.retry_tstamp));
            memset(&ws_startdl_pend.respond_timer, 0,
                                   sizeof(ws_startdl_pend.respond_timer));
            rhandle = add_record(STARTDL_PEND, 
				 object_mac_addr, &ws_startdl_pend);
            if (rhandle == NULL) {
                CSME_DEBUG("\nCSM: Error adding to response pending file - ssp_cur_cs_proc");
	        discard_packet = TRUE;
	        csm_rc = PROC_OK;
            } else {
                startdl_pend_t      *tmp_ptr;
                tmp_ptr = (startdl_pend_t *) rhandle_to_data(rhandle);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_startdl_del_timer,
                                    0, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->retry_tstamp,
                                    &csm_startdl_retry_timer,
                                    0, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->respond_timer,
                                    &csm_startdl_respond_timer,
                                    0, rhandle, FALSE);
                mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_expl_to);
                mgd_timer_stop(&tmp_ptr->retry_tstamp);
                mgd_timer_stop(&tmp_ptr->respond_timer);
	        discard_packet = FALSE;
            }
        }
    }

    if (discard_packet) {
        datagram_done(pak);
    }
    return csm_rc;
}




/*
 * Process non netbios datagram frame from peer.
 */
csm_rc_t ssp_nonnb_frame_proc (peerhandle_t ph, paktype *pak)
{

    int                 rc, ii, no_in_list;
    int                 data_len;
    uchar               object_mac_addr[IEEEBYTES];
    uchar               origin_mac_addr[IEEEBYTES];
    uchar               obj_sap;
    uchar               ori_sap;
    uchar               *rif, *dgm_data;
    long                mtu_size;

    e_xmission_type     trans_type;
    lportid_t           port_list[MAX_REACH];
    lcache_t            *cache_ptr;
    ssp_control_frame_t *ssp_hdr;
    tring_hdr           *tr_hdr;
    sap_hdr             *llc_hdr;

    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;
    tr_hdr = (tring_hdr *) ((char *) ssp_hdr + sizeof(ssp_control_frame_t));
    rif = NULL;
    llc_hdr = (sap_hdr *) ((char *) tr_hdr + TRING_ENCAPBYTES + SRB_MAXRIF);
    dgm_data = (char *) llc_hdr + SAP_HDRBYTES;
    data_len = ssp_hdr->info_hdr.message_len - ssp_hdr->dlc_header_len;

    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.target_link_sap;
	ori_sap = ssp_hdr->dlc_id.origin_link_sap;
    } else {
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.origin_link_sap;
	ori_sap = ssp_hdr->dlc_id.target_link_sap;
    }

    if (peer_get_vendor_type(ph) == PEER_NOT_CISCO) {
        mtu_size = ssp_lf_bits_to_bytes(ssp_hdr->largest_frame_bits);
    } else {
        mtu_size = rc_code_to_bytes(ssp_hdr->largest_frame_bits & RC_LFMASK);
    }
    if (mtu_size <= 0) {
        /* assume maximum */
        CSM_DEBUG("\nCSM: corrupt ssp lf bits %x - mtu %d",
                  ssp_hdr->largest_frame_bits, mtu_size);
        mtu_size = RC_LF17800_VAL;
    }
    mtu_size = min(mtu_size, peerid_to_mtu(ph));
    mtu_size = min(mtu_size, BOX_MTU_SIZE);

    if (object_mac_addr[0] & TR_GROUP) {
        cache_ptr = NULL;
    } else {
        get_mac_cache_entries(object_mac_addr, &cache_ptr, NULL, NULL, NULL);
    }

    trans_type = NO_SEND;
    no_in_list = 0;

    if (cache_ptr == NULL) {
        trans_type = ALL_LAN_PORTS;
    } else {
        switch (cache_ptr->status) {
          case NOT_FOUND:
            trans_type = NO_SEND;
            break;
          case FOUND:
          case VERIFY:
            trans_type = NO_SEND;
            for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                if (cache_ptr->r_entry[ii].flags & AVAILABLE) {
                    trans_type = SPEC_LAN_PORT;
                    no_in_list = 1;
                    port_list[0] = cache_ptr->r_entry[ii].prp_id.port_id;
                    if (cache_ptr->r_entry[ii].rif[0] != 0) {
                        rif = cache_ptr->r_entry[ii].rif;
                    } else {
                        rif = NULL;
                    }
                    break;
                }
            }
            if ((trans_type == NO_SEND) && 
                       (cache_ptr->cache_type != USER_CONF)) {
                trans_type = ALL_LAN_PORTS;
            }
            break;
          case UNCONFIRMED:
            no_in_list = 0;
            for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                if (peer_pass_port_filter(ph, 
                          cache_ptr->r_entry[ii].prp_id.port_id)) {
                    port_list[no_in_list] = 
                          cache_ptr->r_entry[ii].prp_id.port_id;
                    ++no_in_list;
                }
            }
            if (no_in_list) {
                trans_type = SPEC_LAN_PORT;
		rif = NULL;
            } else {
                trans_type = NO_SEND;
            }
            break;
          case SEARCHING:
            trans_type = ALL_LAN_PORTS;
            break;
          default:
            SNA_CSME_DEBUG("\nCSM: Cache status error %s ... xcasting frame", 
                    csm_stat(cache_ptr->status));
            trans_type = ALL_LAN_PORTS;
            break;
        }
    }
    
    if (trans_type == ALL_LAN_PORTS) {
        rc = csm_lan_put (ALL_LAN_PORTS, NULL, UI, 0, 0, object_mac_addr, 
                          origin_mac_addr, obj_sap, ori_sap, 
		          dlsw_nb_xcast_flag, NULL, mtu_size, 
                          dgm_data, data_len, ph);
    } else if (trans_type == SPEC_LAN_PORT) {
	for (ii = 0; ii < no_in_list; ii++) {
            rc += csm_lan_put (SPEC_LAN_PORT, port_list[ii], UI, 0, 0,
                               object_mac_addr, origin_mac_addr,
                               obj_sap, ori_sap,
                               (rif ? DIRECT_FRAME : dlsw_nb_xcast_flag),
			       rif, mtu_size, dgm_data, data_len, ph);
        }
    }

    datagram_done(pak);
    return PROC_OK;
}





/*
 * Process netbios datagram frame from peer.
 */
static csm_rc_t ssp_dataframe_proc (peerhandle_t ph, paktype *pak)
{

    int                 rc, ii, no_in_list;
    int                 data_len;
    uchar               object_mac_addr[IEEEBYTES];
    uchar               origin_mac_addr[IEEEBYTES];
    uchar               obj_sap;
    uchar               ori_sap;
    uchar               *rif;
    boolean             add_nbanq_db;

    netbios_header      *nb_hdr;
    e_xmission_type     trans_type;
    lportid_t           port_list[MAX_REACH];
    lcache_t            *cache_ptr;
    nbanq_info_t        *nbanq_ptr, ws_nbanq;
    record_handle       rhandle;
    ssp_control_frame_t *ssp_hdr;
    tring_hdr           *tr_hdr;
    sap_hdr             *llc_hdr;

    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;
    tr_hdr = (tring_hdr *) ((char *) ssp_hdr + sizeof(ssp_control_frame_t));
    llc_hdr = (sap_hdr *) ((char *) tr_hdr + TRING_ENCAPBYTES + SRB_MAXRIF);
    nb_hdr = (netbios_header *) ((char *) llc_hdr + SAP_HDRBYTES);
    data_len = ssp_hdr->info_hdr.message_len - ssp_hdr->dlc_header_len;

    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.target_link_sap;
	ori_sap = ssp_hdr->dlc_id.origin_link_sap;
    } else {
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.origin_link_sap;
	ori_sap = ssp_hdr->dlc_id.target_link_sap;
    }

    switch (nb_hdr->command) {
      case NETBIOS_STATUS_QUERY:
      case NETBIOS_DATAGRAM:
        get_name_cache_entries(nb_hdr->destname, &cache_ptr, NULL, NULL, NULL);
        break;
      case NETBIOS_STATUS_RESPONSE:
      case NETBIOS_NAME_IN_CONFLICT:
      case NETBIOS_ADD_NAME_RESPONSE:
        if (object_mac_addr[0] & TR_GROUP) {
            cache_ptr = NULL;
        } else {
            get_mac_cache_entries(object_mac_addr, &cache_ptr, 
                                  NULL, NULL, NULL);
        }
        break;
      default:
        /* 
         * Frames to other types of netbios frame are by protocol
         * broadcast. The filter is not applied to these frame types.
         */
        cache_ptr = NULL;
        break;
    }

    rif = NULL;
    trans_type = NO_SEND;
    add_nbanq_db = FALSE;
    no_in_list = 0;

    switch (nb_hdr->command) {
      case NETBIOS_ADD_NAME_QUERY:
        rhandle = seek_record(NBANQ_FILE, nb_hdr->destname, 
                                          (void **) &nbanq_ptr);
        if ((nbanq_ptr != NULL) && (nbanq_ptr->direction != REMOTE)) {
            /*
             * this is either a potential loop or a conflict in the name
             * let us just be a passive spectator
             */
            rhandle = read_next_record(NBANQ_FILE, rhandle, nb_hdr->destname,
				       (void **) &nbanq_ptr);
        }
        if ((nbanq_ptr != NULL) && (nbanq_ptr->direction == REMOTE)) {
            trans_type = NO_SEND;
            add_nbanq_db = FALSE;
        } else {
            trans_type = ALL_LAN_PORTS;
            add_nbanq_db = TRUE;
        }
        break;
      case NETBIOS_ADD_GROUP_QUERY:
      case NETBIOS_DATAGRAM_BROADCAST:
      case NETBIOS_TERMINATE_TRACE_REM:
      case NETBIOS_TERMINATE_TRACE_REM_LOC:
        trans_type = ALL_LAN_PORTS;
        break;
      default:
        if (cache_ptr == NULL) {
            trans_type = ALL_LAN_PORTS;
        } else {
            switch (cache_ptr->status) {
              case NOT_FOUND:
                trans_type = NO_SEND;
                break;
              case FOUND:
              case VERIFY:
                trans_type = NO_SEND;
                no_in_list = 0;
                for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                    if (cache_ptr->r_entry[ii].flags & AVAILABLE) {
                          no_in_list = 1;
                          trans_type = SPEC_LAN_PORT;
                          port_list[0] = cache_ptr->r_entry[ii].prp_id.port_id;
                          if (cache_ptr->r_entry[ii].rif[0] != 0) {
                              rif = cache_ptr->r_entry[ii].rif;
                          } else {
                              rif = NULL;
                          }
                          break;
                    }
                }
                if ((trans_type == NO_SEND) && 
                            (cache_ptr->cache_type != USER_CONF)) {
                    trans_type = ALL_LAN_PORTS;
                }
                break;
              case UNCONFIRMED:
                no_in_list = 0;
                for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                    if (peer_pass_port_filter(ph, 
                              cache_ptr->r_entry[ii].prp_id.port_id)) {
                        port_list[no_in_list]=cache_ptr->r_entry[ii].prp_id.port_id;
                        ++no_in_list;
                    }
                }
                if (no_in_list) {
                    trans_type = SPEC_LAN_PORT;
                } else {
                    trans_type = NO_SEND;
                }
                break;
              case SEARCHING:
                trans_type = ALL_LAN_PORTS;
                break;
              default:
                NETB_CSM_DEBUG("\nCSM: Cache status error %s ... xcasting frame", 
                               csm_stat(cache_ptr->status));
                trans_type = ALL_LAN_PORTS;
                break;
            }
        }
        break;
    }

    rc = 0;
    if (trans_type == ALL_LAN_PORTS) {
        rc = csm_lan_put (ALL_LAN_PORTS, NULL, UI, 0, 0, object_mac_addr, 
                          origin_mac_addr, obj_sap, ori_sap, 
		          dlsw_nb_xcast_flag, NULL, BOX_MTU_SIZE, 
                          (char *) nb_hdr, data_len, ph);
    } else if (trans_type == SPEC_LAN_PORT) {
	for (ii = 0; ii < no_in_list; ii++) {
            rc += csm_lan_put (SPEC_LAN_PORT, port_list[ii], UI, 0, 0,
                               object_mac_addr, origin_mac_addr,
			       obj_sap, ori_sap,
                               (rif ? DIRECT_FRAME : dlsw_nb_xcast_flag),
			       rif, BOX_MTU_SIZE, (char *) nb_hdr,
                               data_len, ph);
        }
    }
    if ((add_nbanq_db) && (rc)) {
        ws_nbanq.direction = REMOTE;
        ws_nbanq.prp_id.peer_id = ph;
        ws_nbanq.retry_info.trans_type = trans_type;
        ws_nbanq.retry_info.no_in_list = no_in_list;
        for (ii = 0; ii < no_in_list; ii++) {
            ws_nbanq.retry_info.prp_id[ii].port_id = port_list[ii];
        }
        ieee_copy(object_mac_addr, ws_nbanq.retry_info.daddr);
        ieee_copy(origin_mac_addr, ws_nbanq.retry_info.saddr);
        ws_nbanq.retry_info.dsap = obj_sap;
        ws_nbanq.retry_info.ssap = ori_sap;
        ws_nbanq.retry_info.xcast_type = RC_BROADBIT | RC_SPANBIT;
        ws_nbanq.retry_info.ui_data_len = data_len;
        bcopy((char *) nb_hdr, ws_nbanq.retry_info.ui_data, data_len);
        memset(&ws_nbanq.del_tstamp, 0, sizeof(ws_nbanq.del_tstamp));
        memset(&ws_nbanq.retry_tstamp, 0, sizeof(ws_nbanq.retry_tstamp));
        rhandle = add_record(NBANQ_FILE, nb_hdr->destname, (void *) &ws_nbanq);
        if (rhandle == NULL) {
            NETB_CSME_DEBUG("\nCSM: Error adding to nbanq database - ssp_dataframe_proc");
        } else {
            nbanq_info_t        *tmp_ptr;
            tmp_ptr = (nbanq_info_t *) rhandle_to_data(rhandle);
            mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                &csm_anq_del_timer,
                                0, rhandle, FALSE);
            mgd_timer_init_leaf(&tmp_ptr->retry_tstamp,
                                &csm_anq_retry_timer,
                                0, rhandle, FALSE);
            mgd_timer_start(&tmp_ptr->del_tstamp, csm_nb_expl_to);
            mgd_timer_start(&tmp_ptr->retry_tstamp, csm_nb_expl_rt);
        }
    }
    datagram_done(pak);
    return PROC_OK;
}



/*
 * FIND_NAMES and CALL_NAMES: ----
 * Please read the comments in dlsw_csm_lan.c
 */


/*
 * Process SSP_OP_netbios_nq messages
 */
static csm_rc_t ssp_nbnq_proc (peerhandle_t ph, paktype *pak)
{

    int                 rc, ii, no_in_list, ws_no_in_list;
    int                 data_len;
    int                 reach_index;
    long                mtu_size, ws_mtu_size, peer_mtu_size;
    uchar               ws_status;
    uchar               object_mac_addr[IEEEBYTES];
    uchar               origin_mac_addr[IEEEBYTES];
    uchar               obj_sap, ori_sap;
    boolean             add_nrpend, add_cache, ring_list_match;

    netbios_header      *nb_hdr;
    e_xmission_type     trans_type;
    lportid_t           port_list[MAX_REACH];
    lcache_t            *cache_ptr, ws_cache;
    nr_pend_t           *nrpend_ptr, ws_nrpend;
    record_handle       rhandle;
    ssp_control_frame_t *ssp_hdr;
    tring_hdr           *tr_hdr;
    sap_hdr             *llc_hdr;
    u_portid_type_t o_prp_id;
    char                nq_call_flag;
    char                *rif_list[MAX_REACH];

    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;
    tr_hdr = (tring_hdr *) ((char *) ssp_hdr + sizeof(ssp_control_frame_t));
    llc_hdr = (sap_hdr *) ((char *) tr_hdr + TRING_ENCAPBYTES + SRB_MAXRIF);
    nb_hdr = (netbios_header *) ((char *) llc_hdr + SAP_HDRBYTES);
    if (nb_hdr->data2[1]) {
        nq_call_flag = NQ_CALL_NAME;
    } else {
        nq_call_flag = NQ_FIND_NAME;
    }
    data_len = ssp_hdr->info_hdr.message_len - ssp_hdr->dlc_header_len;

    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.target_link_sap;
	ori_sap = ssp_hdr->dlc_id.origin_link_sap;
    } else {
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.origin_link_sap;
	ori_sap = ssp_hdr->dlc_id.target_link_sap;
    }
    if (peer_get_vendor_type(ph) == PEER_NOT_CISCO) {
        mtu_size = ssp_lf_bits_to_bytes(ssp_hdr->largest_frame_bits);
    } else {
        mtu_size = rc_code_to_bytes(ssp_hdr->largest_frame_bits & RC_LFMASK);
    }
    if (mtu_size <= 0) {
        /* assume maximum */
        NETB_CSM_DEBUG("\nCSM: corrupt ssp lf bits %x - mtu %d",
                       ssp_hdr->largest_frame_bits, mtu_size);
        mtu_size = RC_LF17800_VAL;
    }
    peer_mtu_size = min((peerid_to_mtu(ph)), BOX_MTU_SIZE);
    ws_mtu_size = min(mtu_size, peer_mtu_size);

    /* if the peer cannot handle the lf size requested.... */
    if (!(ssp_hdr->largest_frame_bits & SSP_LF_IGNORE_BIT)) {
        if (peer_mtu_size < mtu_size) {
            NETB_CSM_DEBUG("\nCSM: Cannot handle size req. drop NB_NQ");
            datagram_done(pak);
            return PROC_OK;
        }
    } 

    get_name_cache_entries(nb_hdr->destname, &cache_ptr, NULL, NULL, NULL);
    if (cache_ptr == NULL) {
        ws_status = NO_CACHE_INFO;
    } else {
        ws_status = cache_ptr->status;
    }

    rhandle = seek_record(NR_PEND, nb_hdr->destname, (void **) &nrpend_ptr);
    while (rhandle != NULL) {
        rc = bcmp(nb_hdr->srcname, nrpend_ptr->sname, NETBIOS_MAX_NAME_LEN);
        if ((rc == 0) && (nrpend_ptr->nq_call_flag == nq_call_flag)) {
            /* match found */
            datagram_done(pak);
            return PROC_OK;
        }
        rhandle = read_next_record(NR_PEND, rhandle, nb_hdr->destname,
				       (void **) &nrpend_ptr);
    }

    trans_type = NO_SEND;
    add_nrpend = FALSE;
    add_cache = FALSE;
    no_in_list = 0;

    NETB_CSMV_DEBUG("\nCSM: Received SSP NQ message ... cache status %s", 
                     csm_stat(ws_status));
    switch (ws_status) {
      case NOT_FOUND:
        trans_type = NO_SEND;
        add_nrpend = FALSE;
        break;
      case VERIFY:
        trans_type = NO_SEND;
        add_nrpend = TRUE;
        break;
      case FOUND:
        ring_list_match = FALSE;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(ph, 
                      cache_ptr->r_entry[ii].prp_id.port_id)) {
                ring_list_match = TRUE;
                break;
            }
        }
        if (ring_list_match == FALSE) {
            datagram_done(pak);
            return(PROC_OK);
        }

	if (mgd_timer_running(&cache_ptr->del_tstamp)) {
	    if ((mgd_timer_left_sleeping(&cache_ptr->del_tstamp) <=
		 csm_touch_nb_cache_active_to)) {
		 mgd_timer_start(&cache_ptr->del_tstamp,
				 csm_touch_nb_cache_reset_to);
	    }
	}	

        if (CLOCK_OUTSIDE_INTERVAL(cache_ptr->verify_tstamp,
                              csm_nb_cache_verify_to)) {
            /*
             * since it is netbios send verify to only the ports that
             * pass the ring list filter
             * because of the limitaion in the way the cache is organized,
             * all the ports in the list will not be verified correctly
             * This in-efficient verification process
             * will be fixed when the cache is re-orged.
             */
            NETB_CSMV_DEBUG("\nCSM: Time to verify netbios cache entries");
            no_in_list = 0;
            ws_no_in_list = cache_ptr->no_in_list;
            for (ii = 0; ii < ws_no_in_list; ii++) {
                port_list[no_in_list]=cache_ptr->r_entry[ii].prp_id.port_id;
		rif_list[no_in_list]=cache_ptr->r_entry[ii].rif;
                ++no_in_list;
	        if (cache_ptr->cache_type == DYNAMIC) {
	            cache_ptr->r_entry[ii].prp_id.port_id = 0;
		    cache_ptr->r_entry[ii].flags = 0;
		    --cache_ptr->no_in_list;
	        } else {
	            cache_ptr->r_entry[ii].flags &= ~AVAILABLE;
	        }
            }
            /*
             * since we have already checked for ring list filter,
             * it is assumed that there is atleast one reach entry that
             * needs to be verified
             */
            trans_type = SPEC_LAN_PORT;
            cache_ptr->status = VERIFY;
            GET_TIMESTAMP(cache_ptr->verify_tstamp);
            mgd_timer_start(&cache_ptr->explore_tstamp, csm_nb_expl_to);
            add_nrpend = TRUE;
        } else {
            o_prp_id.peer_id = ph;
            if (ssp_hdr->largest_frame_bits & SSP_LF_IGNORE_BIT) {
	        reach_index = get_reach_entry_mac(cache_ptr, 0, 
                                                  &o_prp_id, FALSE);
            } else {
	        reach_index = get_reach_entry_mac(cache_ptr, ws_mtu_size, 
                                                  &o_prp_id, FALSE);
            }
	    if (reach_index < 0) {
                /* already checked for ring list match */
	        NETB_CSM_DEBUG("\nCSM: Cache found. But no port info entry (%s)",
                     csm_error(reach_index));
                trans_type = NO_SEND;
                add_nrpend = FALSE;
            } else {
                trans_type = SPEC_LAN_PORT;
                no_in_list = 1;
                rif_list[0] = cache_ptr->r_entry[reach_index].rif;
                port_list[0] = cache_ptr->r_entry[reach_index].prp_id.port_id;
                add_nrpend = TRUE;
            }
        }
        break;
      case UNCONFIRMED:
        no_in_list = 0;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(ph, 
                      cache_ptr->r_entry[ii].prp_id.port_id)) {
                port_list[no_in_list]=cache_ptr->r_entry[ii].prp_id.port_id;
		rif_list[no_in_list] = NULL;
                ++no_in_list;
            }
        }
        if (no_in_list) {
            trans_type = SPEC_LAN_PORT;
            add_nrpend = TRUE;
        } else {
            trans_type = NO_SEND;
            add_nrpend = FALSE;
        }
        break;
      case SEARCHING:
        trans_type = ALL_LAN_PORTS;    /* CL - RP */
        add_nrpend = TRUE;
        break;
      case NO_CACHE_INFO:
        trans_type = ALL_LAN_PORTS;    /* CL - RP */
        add_nrpend = TRUE;
        add_cache = TRUE;
        break;
      default:
        NETB_CSME_DEBUG("\nCSM: Cache status error %s ... xcasting frame", 
                        csm_stat(ws_status));
        trans_type = ALL_LAN_PORTS;
        add_nrpend = TRUE;
        break;
    }

    rc = 0;
    if (trans_type == ALL_LAN_PORTS) {
        rc = csm_lan_put (ALL_LAN_PORTS, NULL, UI, 0, 0, object_mac_addr, 
                          origin_mac_addr, obj_sap, ori_sap, 
		          dlsw_nb_xcast_flag, NULL, ws_mtu_size, 
                          (char *) nb_hdr, data_len, ph);
        if (rc == 0) {
            add_nrpend = FALSE;
            add_cache = FALSE;
        }
    } else if (trans_type == SPEC_LAN_PORT) {
	for (ii = 0; ii < no_in_list; ii++) {
            rc += csm_lan_put (SPEC_LAN_PORT, port_list[ii], UI, 0, 0,
                               object_mac_addr, origin_mac_addr,
                               obj_sap, ori_sap,
			       rif_list[ii] ? DIRECT_FRAME : dlsw_nb_xcast_flag,
                               rif_list[ii] , ws_mtu_size, (char *) nb_hdr,
                               data_len, ph);
        }
        if (rc == 0) {
            add_nrpend = FALSE;
            add_cache = FALSE;
        }
    }

    if (add_cache) {
        ws_cache.cache_type = DYNAMIC;
        ws_cache.status = SEARCHING;
        ws_cache.reach_direction = LOCAL;
	ws_cache.no_in_list = 0;
        for (ii = 0; ii < MAX_REACH; ii++) {
            ws_cache.r_entry[ii].prp_id.port_id = 0;
            ws_cache.r_entry[ii].flags = 0;
        }
        memset(&ws_cache.del_tstamp, 0, sizeof(ws_cache.del_tstamp));
        memset(&ws_cache.explore_tstamp, 0, sizeof(ws_cache.explore_tstamp));
        rhandle = add_record(LCACHE_NAME, nb_hdr->destname, &ws_cache);
        if (rhandle == NULL) {
            NETB_CSME_DEBUG("\nCSM: Error adding cache info - ssp_nbnq_proc");
        } else {
            lcache_t      *tmp_ptr;
            tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
            mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                &csm_cache_expl_timer,
                                CSM_LNAME_CACHE, rhandle, FALSE);
            mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                &csm_cache_del_timer,
                                CSM_LNAME_CACHE, rhandle, FALSE);
            mgd_timer_stop(&tmp_ptr->explore_tstamp);
            mgd_timer_start(&tmp_ptr->del_tstamp, csm_nb_expl_to);
            GET_TIMESTAMP(tmp_ptr->verify_tstamp);
        }
    }
    if (add_nrpend) {
        bcopy(nb_hdr->srcname, ws_nrpend.sname, NETBIOS_MAX_NAME_LEN);
        ws_nrpend.peer_id = ph;
        ws_nrpend.csex_flags = ssp_hdr->csex_flags;
        ws_nrpend.retry_info.trans_type = trans_type;
        ws_nrpend.retry_info.no_in_list = no_in_list;
        ws_nrpend.mtu_size = ws_mtu_size;
        ws_nrpend.mtu_flag = ssp_hdr->largest_frame_bits & SSP_LF_IGNORE_BIT;
        for (ii = 0; ii < no_in_list; ii++) {
            ws_nrpend.retry_info.prp_id[ii].port_id = port_list[ii];
        }
	ws_nrpend.retry_info.xcast_type = dlsw_nb_xcast_flag;
        ieee_copy(object_mac_addr, ws_nrpend.retry_info.daddr);
        ieee_copy(origin_mac_addr, ws_nrpend.retry_info.saddr);
        ws_nrpend.retry_info.dsap = obj_sap;
        ws_nrpend.retry_info.ssap = ori_sap;
        ws_nrpend.retry_info.ui_data_len = data_len;
        ws_nrpend.nq_call_flag = nq_call_flag;
        if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
            ws_nrpend.remote_dlc_port_id = ssp_hdr->origin_id.dlc_port_id;
            ws_nrpend.remote_transport_id = ssp_hdr->origin_id.transport_id;
        } else {
            ws_nrpend.remote_dlc_port_id = ssp_hdr->target_id.dlc_port_id;
            ws_nrpend.remote_transport_id = ssp_hdr->target_id.transport_id;
        }
        bcopy((char *) nb_hdr, ws_nrpend.retry_info.ui_data, data_len);
        memset(&ws_nrpend.del_tstamp, 0, sizeof(ws_nrpend.del_tstamp));
        memset(&ws_nrpend.retry_tstamp, 0, sizeof(ws_nrpend.retry_tstamp));
        rhandle = add_record(NR_PEND, nb_hdr->destname, (void *) &ws_nrpend);
        if (rhandle == NULL) {
            NETB_CSME_DEBUG("\nCSM: Error adding to nrpend database - ssp_nbnq_proc");
        } else {
            nr_pend_t      *tmp_ptr;
            tmp_ptr = (nr_pend_t *) rhandle_to_data(rhandle);
            mgd_timer_init_leaf(&tmp_ptr->retry_tstamp,
                                &csm_nr_retry_timer,
                                0, rhandle, FALSE);
            mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                &csm_nr_del_timer,
                                0, rhandle, FALSE);
            mgd_timer_start(&tmp_ptr->del_tstamp, csm_nb_expl_to);
            if (trans_type != NO_SEND) {
                mgd_timer_start(&tmp_ptr->retry_tstamp, csm_nb_expl_rt);
            } else {
                mgd_timer_stop(&tmp_ptr->retry_tstamp);
            }
        }
    }

    datagram_done(pak);
    return PROC_OK;

}





/*
 * Process SSP_OP_netbios_nr messages
 */
static csm_rc_t ssp_nbnr_proc (peerhandle_t ph, paktype *pak)
{

    int                 rc, data_len;
    long                mtu_size, ws_mtu_size;
    uchar               object_mac_addr[IEEEBYTES];
    uchar               origin_mac_addr[IEEEBYTES];
    uchar               obj_sap, ori_sap;
    uchar               *rif;

    netbios_header      *nb_hdr;
    dlxnr_pend_t        *dlxnr_ptr, *best_dlxnr_ptr = NULL;
    record_handle       rhandle, best_rhandle = NULL;
    ssp_control_frame_t *ssp_hdr;
    tring_hdr           *tr_hdr;
    sap_hdr             *llc_hdr;
    char                nq_call_flag;

    ssp_hdr = (ssp_control_frame_t *) pak->datagramstart;
    tr_hdr = (tring_hdr *) ((char *) ssp_hdr + sizeof(ssp_control_frame_t));
    rif = (char *) tr_hdr + TRING_ENCAPBYTES;
    llc_hdr = (sap_hdr *) ((char *) rif + SRB_MAXRIF);
    nb_hdr = (netbios_header *) ((char *) llc_hdr + SAP_HDRBYTES);
    if (nb_hdr->data2[1]) {
        nq_call_flag = NQ_CALL_NAME;
    } else {
        nq_call_flag = NQ_FIND_NAME;
    }
    data_len = ssp_hdr->info_hdr.message_len - ssp_hdr->dlc_header_len;

    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.target_link_sap;
	ori_sap = ssp_hdr->dlc_id.origin_link_sap;
    } else {
	ieee_copy(ssp_hdr->dlc_id.origin_mac_address, object_mac_addr);
	ieee_copy(ssp_hdr->dlc_id.target_mac_address, origin_mac_addr);
	obj_sap = ssp_hdr->dlc_id.origin_link_sap;
	ori_sap = ssp_hdr->dlc_id.target_link_sap;
    }

    if (peer_get_vendor_type(ph) == PEER_NOT_CISCO) {
        mtu_size = ssp_lf_bits_to_bytes(ssp_hdr->largest_frame_bits);
    } else {
        mtu_size = rc_code_to_bytes(ssp_hdr->largest_frame_bits & RC_LFMASK);
    }
    if (mtu_size <= 0) {
        /* assume maximum */
        NETB_CSM_DEBUG("\nCSM: corrupt ssp lf bits %x - mtu %d",
                       ssp_hdr->largest_frame_bits, mtu_size);
        mtu_size = RC_LF17800_VAL;
    }
    mtu_size = min(mtu_size, (peerid_to_mtu(ph)));
    mtu_size = min(mtu_size, BOX_MTU_SIZE);

    rhandle = seek_record(DLXNR_PEND, nb_hdr->srcname, (void **) &dlxnr_ptr);
    while (rhandle != NULL) {
        if (peer_pass_port_filter(ph, dlxnr_ptr->port_id)) {
            rc = bcmp(nb_hdr->destname, dlxnr_ptr->sname, NETBIOS_MAX_NAME_LEN);
            if (rc == 0) {
                /*
                 * NetBIOS name matched. Now attempt to match the nq_call_flag.
                 * If we can match it with a dlxnr record, pass the NR to
                 * the port and delete the nrpend record. If the nq_call_flag
                 * doesn't match, pass the NR to the port but do NOT delete the
                 * dlxnr record. The dlxnr record is used to drop duplicate
                 * find/call name queries.
                 */
                best_rhandle = rhandle;
                best_dlxnr_ptr = dlxnr_ptr;
                if (dlxnr_ptr->nq_call_flag == nq_call_flag) {
                    break;
                }
            }
        }
        rhandle = read_next_record(DLXNR_PEND, rhandle, nb_hdr->srcname,
                                     (void **) &dlxnr_ptr);
    }

    if (best_dlxnr_ptr) {
        ws_mtu_size = min(mtu_size, best_dlxnr_ptr->mtu_size);
        csm_lan_put(SPEC_LAN_PORT, best_dlxnr_ptr->port_id, UI, 0, 0,
                    object_mac_addr, origin_mac_addr, obj_sap, ori_sap,
                    RC_BROADBIT | RC_SPANBIT, NULL, ws_mtu_size,
                    (char *) nb_hdr, data_len, ph);

        /*
         * If the nq_call_flag matches, delete the record. Otherwise, keep
         * it around so it can be used to drop duplicate name queries.
         */
        if (best_dlxnr_ptr->nq_call_flag == nq_call_flag) {
            mgd_timer_stop(&best_dlxnr_ptr->del_tstamp);
            delete_record_get_next(DLXNR_PEND, best_rhandle);
        }
    }

    datagram_done(pak);
    return PROC_OK;
}




/*
 * Send message to peer transport manager
 */
int csm_peer_put (peerhandle_t peer_id, paktype *pak, 
		   char *daddr, char *saddr, char dsap, char ssap,
		   uchar flags, lportid_t origin_lan_port) 
{

    int                  rc;
    peer_msg_u           peer_msg;
    core2peer_ret_t      peerput_rc;
    char                 *str;


    rc = 0;

    str = string_getnext();
    if (TIMER_RUNNING(csm_expl_delay_timer)) {
        CSMV_DEBUG("\nCSM: Write to peer %s not ok - EXPLORER_DELAY",
                   peer_id?peer_handle_to_str(peer_id, str):"all peers");
        return rc;
    }
    if (peer_id != 0) {
        peer_msg.p_put.type = PEER_PUT;
        peer_msg.p_put.dest_u.peer_handle = peer_id;
        peer_msg.p_put.pak_cat = PEER_REACH_ADMIN;
    } else {
	peer_msg.p_put.type = PEER_PUT_BCAST;
	peer_msg.p_put.dest_u.peer_handle = 0;
        peer_msg.p_put.pak_cat = PEER_BCAST;
    }
    peer_msg.p_put.pak = pak;
    peer_msg.p_put.flags = flags;
    peer_msg.p_put.lan_port = origin_lan_port;

    peer_msg.p_put.dmac = daddr;
    peer_msg.p_put.smac = saddr;
    peer_msg.p_put.dsap = dsap;
    peer_msg.p_put.ssap = ssap;

    if (flags & PEER_NETBIOS_UI) {
        pak->info_start = (char *) (pak->datagramstart +
                                    sizeof(ssp_control_frame_t) +
                                    TRING_ENCAPBYTES + SRB_MAXRIF +
                                    SAP_HDRBYTES);
    }

    peerput_rc = core_to_peer(&peer_msg);
    switch (peerput_rc) {
      case PEER_SUCCESS:
      case PEER_SUCCESS_AND_BUSY:
        /* core_to_peer releases the packet */
	rc = 1;
        CSMV_DEBUG("\nCSM: Write to %s ok.", 
                   peer_id?peer_handle_to_str(peer_id, str):"all peers");
        break;
      case PEER_FILTERED:
        /* core_to_peer releases the packet */
	rc = 0;
        CSME_DEBUG("\nCSM: Write to %s not ok - PEER_FILTERED", 
	           peer_id?peer_handle_to_str(peer_id, str):"all peers");
        break;
      case PEER_UNREACHABLE:
        /* core_to_peer releases the packet */
	rc = 0;
        CSME_DEBUG("\nCSM: Write to %s not ok - PEER_UNREACHABLE", 
		   peer_id?peer_handle_to_str(peer_id, str):"all peers");
        break;
      case PEER_BUSY:
        /* core_to_peer releases the packet */
	rc = 0;
        CSME_DEBUG("\nCSM: Write to %s not ok - PEER_BUSY", 
		   peer_id?peer_handle_to_str(peer_id, str):"all peers");
        break;
      case PEER_NO_CONNECTIONS:
        /* core_to_peer releases the packet */
	rc = 0;
        CSME_DEBUG("\nCSM: Write to %s not ok - PEER_NO_CONNECTIONS", 
		  peer_id?peer_handle_to_str(peer_id, str):"all peers");
        break;
      case PEER_INVALID_HANDLE:
        /* core_to_peer releases the packet */
	rc = 0;
        CSME_DEBUG("\nCSM: Write to %s not ok - PEER_INVALID_HANDLE", 
                   peer_id?peer_handle_to_str(peer_id, str):"all peers");
        break;

      case PEER_PARAMETER_ERROR:
      default:
        /* core_to_peer DOES NOT release the packet */
	rc = 0;
        datagram_done(pak);
        CSME_DEBUG("\nCSM: Write to %s not ok - PEER_PARAMETER_ERROR", 
                   peer_id?peer_handle_to_str(peer_id, str):"all peers");
        break;
    }

    return rc;

}




/*
 * peerid_to_mtu (peerhandle_t ph)
 * given peerhandle, return mtu size in bytes
 */
long peerid_to_mtu (peerhandle_t ph)
{

    uchar     rc_code;

    rc_code = peer_get_largest_frame(ph);
    if ((rc_code & RC_LFMASK) == RC_LFINITIAL) {
        rc_code = RC_LF17800;
    }
    return(rc_code_to_bytes(rc_code));

}
