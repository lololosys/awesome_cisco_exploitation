/* $Id: dlsw_csm_lan.c,v 3.15.4.12 1996/08/22 18:35:07 ravip Exp $
 *------------------------------------------------------------------
 * DLSw LAN Message Handler
 *
 * August 1994, Ravi Periasamy 
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Contains routines to handle message from LAN protocols
 *------------------------------------------------------------------
 * $Log: dlsw_csm_lan.c,v $
 * Revision 3.15.4.12  1996/08/22  18:35:07  ravip
 * CSCdi66201:  DLSw load sharing broken
 * Branch: California_branch
 *
 * Revision 3.15.4.11  1996/08/19  14:02:44  rbatz
 * CSCdi62416:  DLSw / QLLC: Inconsistent LF field in the RIF.
 * Branch: California_branch
 *
 * Revision 3.15.4.10  1996/08/16  06:56:00  ioakley
 * CSCdi58903:  Add dlsw touch timer support for sna/netbios
 *              reachability cache entries.
 * Branch: California_branch
 *
 * Revision 3.15.4.9  1996/08/09  23:42:40  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.15.4.8  1996/08/09  21:55:32  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.15.4.7  1996/08/09  19:47:59  ravip
 * CSCdi52387:  DLSw:CPUHOG process=DLSw Background, PC=6055C4E8
 *              Change CSM timer to use managed timers
 * Branch: California_branch
 *
 * Revision 3.15.4.6  1996/08/05  18:39:07  ravip
 * CSCdi62784:  dlsw netbios windows nt cant connect nq sent directed no nr
 * Branch: California_branch
 *
 * Revision 3.15.4.5  1996/07/23  13:21:34  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.15.4.4  1996/06/02  05:09:35  schiang
 * CSCdi57445:  DLSw+ local switch doesnt block traffic from RSRB to
 * non-NCIA IFs
 * Branch: California_branch
 *
 * Revision 3.15.4.3  1996/05/31  14:26:26  ravip
 * CSCdi58834:  4500/4000/7000 routers crash on show dlsw reachability
 * Branch: California_branch
 *
 * Revision 3.15.4.2  1996/05/17  10:45:22  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.14.2.10  1996/05/15  00:39:00  ravip
 * CSCdi50707:  dlsw the reachability search is linear, should be binary
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.9  1996/05/14  03:20:16  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.8  1996/05/12  15:46:34  vkamat
 * CSCdi57297:  Invalid_peer traceback in DLSW local-switch configuration
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.7  1996/05/10  15:00:52  ioakley
 * CSCdi57325:  Remove dlsw reliance on port_number/slot_number
 *              and identify all dlsw ports by associated swidb.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.6  1996/05/02  00:05:30  ravip
 * CSCdi56281:  dlsw sna local switching could cause broadcast storm
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.5  1996/04/26  07:34:05  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.14.2.4  1996/04/25  04:27:57  ioakley
 * CSCdi55673:  Reduce number of calls on DLSw LAN side and remove
 *              functions that are no longer useful or necessary.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.3  1996/04/11  14:37:10  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.14.2.2  1996/04/03  14:01:17  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.14.2.1  1996/03/17  17:37:53  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.15.4.1  1996/03/18  19:31:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.2.3  1996/03/16  06:38:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.10.2.2  1996/03/07  08:44:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.2.1  1996/02/20  00:45:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/03/07  19:33:53  ravip
 * CSCdi50382:  netbios dlsw should not send a test_cmd after a nr
 * received.
 *
 * Revision 3.14  1996/02/12  22:12:52  ravip
 * CSCdi48012:  1795 interoperability
 *              - Reverse source and destination mac addresses for
 *                SSP_OP_netbios_nr and _anr frames
 *              - Set remote data link correlator in SSP_OP_netbios_nr
 *                to origin data link correlator values received in
 *                SSP_OP_netbios_nq frame
 *
 * Revision 3.13  1996/02/07  21:44:20  ravip
 * CSCdi46935:  DLSw does not propogate largest frame bits correctly
 *
 * Revision 3.12  1996/02/06  00:51:32  ioakley
 * CSCdi48149: Remove redundant instruction in send_ui_frame_to_peers.
 *
 * Revision 3.11  1996/02/01  06:01:16  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/01/18  22:24:29  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1996/01/12  17:31:57  ravip
 * CSCdi46357:  clean up dlsw csm debug messages
 *
 * Revision 3.8  1995/12/19  00:39:40  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.7  1995/12/15  17:18:18  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.6  1995/12/12  17:36:55  ravip
 * CSCdi44808:  crash in csm_clsi_handler
 *
 * Revision 3.5  1995/11/29  22:06:21  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.4  1995/11/21  20:56:58  kpatel
 * CSCdi43226:  Unknown CLS messages with DLSw+/TCP when frame-relay
 *              also configured
 *
 * Revision 3.3  1995/11/17  09:02:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:55:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/24  16:27:04  fbordona
 * CSCdi42216:  %DLSWP-3-PNOMEM: DLSw: No memory to send to group members
 *                Add check for getbuffer failure.
 *                Turn off RII in SSP dataframes.
 *
 * Revision 2.8  1995/10/20  14:08:49  ravip
 * CSCdi40722: Exclusive reachability option does not filter frames
 *             correctly. Fixed problems with deleting reachability cache.
 *
 * Revision 2.7  1995/09/25  21:02:42  ravip
 * CSCdi39850:  dlsw sends test to sap04 rather than sap00.
 *              ncp expects sap00
 *              Using using make_forward_rif causes problem with the fep
 *              in some cases
 *
 * Revision 2.6  1995/08/07  05:31:14  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.5  1995/07/19  21:00:49  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add registry service for callback to get QLLC's Activate Ring CLSI
 * message.
 * Move code around for default Activate ring, so that it doesn't fall in
 * the qllc path.
 *
 * Revision 2.4  1995/06/16  20:38:38  ravip
 * CSCdi35675:  DLSW session cannot start. XID exchange failure
 *
 * Revision 2.3  1995/06/14  18:10:14  ravip
 * CSCdi34577:  dlsw session not established SAPs changed
 *              Handle XIDs to NULL SAP.
 *
 * Revision 2.2  1995/06/13  17:49:13  ravip
 * CSCdi35261:  DLSw should permit ARB from target
 *              Nerd knob to control ARB and SRB of SNA and Netbios frames
 *              Store test frames if search is pending for the dmac
 *
 * Revision 2.1  1995/06/07  20:26:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../if/tring.h"
#include "../ui/debug.h"
#include "../cls/clsi_msg.h"
#include "../llc2/llc2.h"
#include "../ibm/netbios.h"
#include "../util/fsm.h"
#include "../if/rif_util.h"
#include "../ibm/libibm_externs.h"
#include "../h/mgd_timers.h"

 
#include "../dlsw/parser_defs_dlsw.h"
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
 * Function Prototypes
 */
static csm_rc_t test_frame_proc (lportid_t, TCLSIMsg *, csm_info_t *);
static csm_rc_t resp_frame_proc (lportid_t, TCLSIMsg *, csm_info_t *);
static csm_rc_t nb_frame_proc (lportid_t, TCLSIMsg *, csm_info_t *);
static csm_rc_t nonnb_uiframe_proc (lportid_t, TCLSIMsg *, csm_info_t *);
static csm_rc_t new_connection (lportid_t, TCLSIMsg *, csm_info_t *);

static void nb_data_frame_proc (lportid_t, TCLSIMsg *, csm_info_t *);
static void nb_nq_frame_proc (lportid_t, TCLSIMsg *, csm_info_t *);
static void nb_nr_frame_proc (lportid_t, TCLSIMsg *, csm_info_t *);
static void make_new_rif (lportid_t, uchar, uchar, char *);
static boolean local_test_frame_proc (dlsw_ring_t *, TCLSIMsg *, csm_info_t *,
                                      boolean, boolean);


/*
 * argh!  please document 
 */
csm_rc_t csm_clsi_handler(lportid_t lan_port_id, TCLSIMsg *clsi_msg)
{
    csm_info_t      csm;
    ushort          msg_type;
    uchar           *ws_rif;
    uchar           ws_rtmask, ws_dirmask, ws_lfmask, str_saddr0;
    boolean         freeup_clsi_msg;
    u_portid_type_t prp_id;
    csm_rc_t        csm_rc;
    int             rif_rc;
    char           *str;
    lcache_t       *cache_ptr;

    freeup_clsi_msg = FALSE;
    csm_rc = PROC_OK;
    ws_lfmask = ws_rtmask = ws_dirmask = 0;
    csm.mtu_bytes = 0;

    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    csm.saddr = get_saddr_from_clsi(clsi_msg);
    csm.daddr = get_daddr_from_clsi(clsi_msg);
    csm.ssap = get_ssap_from_clsi(clsi_msg);
    csm.dsap = get_dsap_from_clsi(clsi_msg);
    csm.pf_flag = get_pfflag_from_clsi(clsi_msg);
    csm.ctrl = get_ctrl_from_clsi(clsi_msg);
    if ((csm.saddr == NULL) || (csm.daddr == NULL)) {
        CSME_DEBUG("\nCSM: Error parsing clsi message - csm_clsi_handler()");
        free_clsi_msg(clsi_msg);
        return PROC_OK;
    }
    str_saddr0 = csm.saddr[0];
    csm.saddr[0] &= ~TR_RII;

    /*
     * Fix rif to match the mtu sizes configured
     * And store the mtu size in the rif received
     */
    rif_rc = get_rif_from_clsi(clsi_msg, csm.rif);
    if (rif_rc < 0) {
        free_clsi_msg(clsi_msg);
        return PROC_OK;
    }

    if (dlsw_ignore_port_lf(lan_port_id)) {
        csm.mtu_bytes = BOX_MTU_SIZE;
        ws_rif = NULL;
    } else if (csm.rif[0] != 0) {
        ws_rtmask = csm.rif[0] & RC_RTMASK;
        ws_dirmask = csm.rif[1] & RC_D;
        ws_lfmask = csm.rif[1] & RC_LFMASK;
        csm.rif[0] &= ~RC_RTMASK;
        csm.rif[1] ^= RC_D;
        csm.mtu_bytes = rc_code_to_bytes((csm.rif[1] & RC_LFMASK));
        if (csm.mtu_bytes <= 0) {
            /* assume maximum */
            CSM_DEBUG("\nCSM: mtu size in frame received %d", csm.mtu_bytes);
            CSM_DEBUG("\nCSM: rc code in rif is %x", (csm.rif[1] & RC_LFMASK));
            csm.mtu_bytes = RC_LF17800_VAL;
        }
        csm.mtu_bytes = min(csm.mtu_bytes, (portid_to_mtu(lan_port_id)));
        csm.mtu_bytes = min(csm.mtu_bytes, BOX_MTU_SIZE);
        csm.mtu_code = bytes_to_rc_code(csm.mtu_bytes);
        if (csm.mtu_code == 0xff)
            csm.mtu_code = RC_LF17800;
        csm.rif[1] = (csm.rif[1] & ~RC_LFMASK) | csm.mtu_code;
        ws_rif = csm.rif;
    } else {
        csm.mtu_bytes = min(BOX_MTU_SIZE, portid_to_mtu(lan_port_id));
        ws_rif = NULL;
    }

    /*
     * check if it passes local exclusivity filters
     * If it passes, add name to cache.
     */
    if (csm_local_mac_excl_pass(csm.saddr)) {
        prp_id.port_id = lan_port_id;
        if (dlsw_can_switch(lan_port_id->swidb, lan_port_id->swidb,
                           REMOTE_SWITCH))
            mac_found_update_cache(csm.saddr, LOCAL, &prp_id,
                                   ws_rif, csm.mtu_bytes);
    } else {
        CSM_DEBUG("\nCSM: %e DID NOT pass local mac excl. filter", csm.saddr);
        /*
         * drop the frame, if the frame type is Test 
         * since the ICR will get filtered anyway.
         */
        if (msg_type == (CLS_TEST_STN | CLS_INDICATION)) {
            SNA_CSM_DEBUG("\nCSM: And it is a test frame - drop frame");
            free_clsi_msg(clsi_msg);
            return PROC_OK;
        }
    }

    /*
     * while we are at it, lets check for netbios filters.
     */
    if ((csm.ssap == 0xF0) || (csm.dsap == 0xF0)) {
        csm.nb_hdr = (netbios_header *) get_usrdata_from_clsi(clsi_msg);
        switch (csm.nb_hdr->command) {
          case NETBIOS_NAME_QUERY:
          case NETBIOS_NAME_RECOGNIZED:
          case NETBIOS_DATAGRAM:
          case NETBIOS_STATUS_QUERY:
            if (!(csm_local_nb_excl_pass(csm.nb_hdr->srcname))) {
                NETB_CSM_DEBUG("\nCSM: %15s DID NOT pass local name excl. filter",
                           csm.nb_hdr->srcname);
                free_clsi_msg(clsi_msg);
                return PROC_OK;
            }
            break;
          default:
            /* Other nb frames are not filtered here */
            break;
        }
        /*
         * Add to netbios reachability cache
         */
        switch (csm.nb_hdr->command) {
          case NETBIOS_DATAGRAM:
          case NETBIOS_DATAGRAM_BROADCAST:
          case NETBIOS_NAME_QUERY:
          case NETBIOS_NAME_RECOGNIZED:
            prp_id.port_id = lan_port_id;
            name_found_update_cache(csm.nb_hdr->srcname, LOCAL, &prp_id,
                                    csm.rif, csm.mtu_bytes);
            break;
          default:
            /* Do no update cache for other frame types */
            break;
        }
        /*
         * To prevent re-exploring on a SABME on a stale cache entry
         */
        if (csm.nb_hdr->command == NETBIOS_NAME_RECOGNIZED) {
            get_mac_cache_entries(csm.saddr, &cache_ptr, NULL, NULL, NULL);
            if (cache_ptr != NULL) {
                if (cache_ptr->status == FOUND) {
                    GET_TIMESTAMP(cache_ptr->verify_tstamp);
                }
            }
            get_mac_cache_entries(csm.daddr, NULL, NULL, &cache_ptr, NULL);
            if (cache_ptr != NULL) {
                if (cache_ptr->status == FOUND) {
                    GET_TIMESTAMP(cache_ptr->verify_tstamp);
                }
            }
        }
    }
 

    /*
     * reset the saddr and the rtbits and direction bit in the rif
     * before passing this frame to the other processes. they may
     * rely on these information to process the frame. 
     * restore the mtu size only if the frame received is a non
     * broadcast. If this is larger than the sizes that we can
     * handle, then it is a problem. Let the frame process routines
     * handle this.
     */ 
    csm.saddr[0] = str_saddr0;
    if (csm.rif[0] != 0) {
        csm.rif[0] = (csm.rif[0] & ~RC_RTMASK) | ws_rtmask;
        csm.rif[1] = (csm.rif[1] & ~RC_D) | ws_dirmask;
        if (!(csm.rif[0] & RC_BROADBIT)) {
            csm.rif[1] = (csm.rif[1] & ~RC_LFMASK) | ws_lfmask;
        }
    }
    str = string_getnext();
    if ((csm.dsap == 0xF0) || (csm.ssap == 0xF0)) {
        NETB_CSM_DEBUG("\nCSM: Received %s from %s",
             CLSIMsgSprintfShort(clsi_msg, str), 
             lan_port_id->swidb->hwptr->hw_namestring);
        NETB_CSM_DEBUG("\nCSM:   smac %e, dmac %e, ssap %2x, dsap %2x",
             csm.saddr, csm.daddr, csm.ssap, csm.dsap);
    } else {
        CSM_DEBUG("\nCSM: Received %s from %s",
             CLSIMsgSprintfShort(clsi_msg, str), 
             lan_port_id->swidb->hwptr->hw_namestring);
        CSM_DEBUG("\nCSM:   smac %e, dmac %e, ssap %2x, dsap %2x",
             csm.saddr, csm.daddr, csm.ssap, csm.dsap);
    }
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	if (((TestStnInd_t *)clsi_msg)->fTESTCmdRsp == CLS_TEST_CMD) {
            csm_rc = test_frame_proc(lan_port_id, clsi_msg, &csm);
        } else {
            csm_rc = resp_frame_proc(lan_port_id, clsi_msg, &csm);
        }
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
        /*
         * for the first pass ID_STN_RESP is not handled. DLSw does
         * not write ID_STN_REQ out. So we do not expect ID_STN_RSPs
         * Treat XID as a test frame if it's dsap is NULL or if it's
         * a broadcast.
         */
        if ((csm.dsap == 0) || (csm.rif[0] & RC_BROADBIT)) {
            csm_rc = test_frame_proc(lan_port_id, clsi_msg, &csm);
        } else {
            csm_rc = new_connection(lan_port_id, clsi_msg, &csm);
        }
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
        if (csm.dsap == 0xf0) {
            csm_rc = nb_frame_proc(lan_port_id, clsi_msg, &csm);
        } else {
            csm_rc = nonnb_uiframe_proc(lan_port_id, clsi_msg, &csm);
        }
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
        csm_rc = new_connection(lan_port_id, clsi_msg, &csm);
	break;

      case (CLS_TEST_STN | CLS_CONFIRM):     /* Test Frame        */
      case (CLS_ID_STN | CLS_CONFIRM):       /* Contextless XID   */
      case (CLS_UDATA_STN | CLS_CONFIRM):    /* UIDATA            */
      case (CLS_CONNECT_STN | CLS_CONFIRM):  /* Contextless SABME */
	CSM_DEBUG("\nCSM: Received CLS_CONFIRM....Dropping message");
	csm_rc = PROC_OK;
	freeup_clsi_msg = TRUE;
	break;

      default:
	csm_rc = PROC_OK;
	freeup_clsi_msg = TRUE;
	CSM_DEBUG("\nCSM: Invalid primitive type %x...Dropping message", msg_type);
	break;

    }

    if (freeup_clsi_msg) {
	free_clsi_msg (clsi_msg);
    }

    return csm_rc;
}


/*
 * handle test frames from lan
 */
static csm_rc_t test_frame_proc (lportid_t lan_port_id, TCLSIMsg *clsi_msg,
                          csm_info_t *csm)
{
    int            rc_ssp, no_in_list, ii, usr_data_len, reach_rc;
    uchar          *usr_data, cmd_rsp, ws_status;
    long           ws_mtu_size;
    boolean        add_icr_pend, add_cache_entry, no_mtu_size;
    boolean        ring_list_match;

    record_handle  rhandle;
    lcache_t       *cache_ptr, *local_cache_ptr, ws_cache;
    icr_pend_t     *icr_pend, ws_icr_pend;
    paktype        *ssp_pak, *tmp_pak;
    ssp_control_frame_t *ssp_hdr;
    e_xmission_type  trans_type;
    peerhandle_t     peer_list[MAX_REACH];
    u_portid_type_t o_prp_id;

    cmd_rsp = get_cmdrsp_from_clsi(clsi_msg);
    usr_data_len = get_usrdata_len_from_clsi(clsi_msg);
    if (usr_data_len) {
        usr_data = get_usrdata_from_clsi(clsi_msg);
    } else {
        usr_data = NULL;
    }

    /* fix the rif and saddr*/
    csm->ssap &= ~RSPBIT;
    csm->saddr[0] &= ~TR_RII;
    if (csm->rif[0] != 0) {
        csm->rif[0] &= ~RC_RTMASK;
        csm->rif[1] ^= RC_D;
    }

    get_mac_cache_entries(csm->daddr, &local_cache_ptr, NULL, &cache_ptr, NULL);
    if (local_cache_ptr != NULL) {
        if ((local_cache_ptr->status != NOT_FOUND) &&
                              (local_cache_ptr->status != SEARCHING)) {
            /*
             * Reachable locally. If entry is fresh and source or
             * destination port is a "local_switch" interface, then
             * respond locally.
             *
             * First find destination port.
             */
            o_prp_id.port_id = lan_port_id;
            reach_rc = get_reach_entry_mac(local_cache_ptr, 0, NULL, FALSE);
            if ((reach_rc >= 0) &&
                dlsw_can_switch(lan_port_id->swidb,
                local_cache_ptr->r_entry[reach_rc].prp_id.port_id->swidb,
                LOCAL_SWITCH)) {
                    ws_mtu_size = min(csm->mtu_bytes,
                                  local_cache_ptr->r_entry[reach_rc].mtu_size);
                    csm_lan_put(SPEC_LAN_PORT, lan_port_id, (csm->ctrl & ~P_BIT),
                                RSPBIT, csm->pf_flag, csm->saddr, csm->daddr,
                                csm->ssap, csm->dsap, 0,
                                csm->rif, ws_mtu_size, usr_data,
                                usr_data_len, (peerhandle_t) 0);
            }
            free_clsi_msg(clsi_msg);
            return PROC_OK;
        }
    } else {
        /*
         * Explore locally.
         */
        local_test_frame_proc(lan_port_id, clsi_msg, csm, TRUE, FALSE);
    }

    if (cache_ptr == NULL) {
        ws_status = NO_CACHE_INFO;
    } else {
        ws_status = cache_ptr->status;
    }

    CSMV_DEBUG("\nCSM: test_frame_proc: ws_status = %s", csm_stat(ws_status));

    add_icr_pend = FALSE;
    add_cache_entry = FALSE;
    trans_type = NO_SEND;
    no_in_list = 0;
    switch (ws_status) {
      case FOUND:
        no_mtu_size = TRUE;
        ring_list_match = FALSE;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (cache_ptr->r_entry[ii].mtu_size != 0) {
                no_mtu_size = FALSE;
            }
            if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id,
                                      lan_port_id)) {
                ring_list_match = TRUE;
            } 
            if ((ring_list_match == TRUE) && (no_mtu_size == FALSE)) {
                break;
            }
        }

        if (ring_list_match == FALSE) {
            /*
             * Allow port list to do it's job and drop packet.
             * When current cache times out, we'll explore to all peers.
             */
            free_clsi_msg(clsi_msg);
            return PROC_OK;
        }

	if (mgd_timer_running(&cache_ptr->del_tstamp)) {
	    if ((mgd_timer_left_sleeping(&cache_ptr->del_tstamp) <=
		 csm_touch_sna_cache_active_to)) {
		 mgd_timer_start(&cache_ptr->del_tstamp,
				 csm_touch_sna_cache_reset_to);
	    }
	}	

        if ((CLOCK_OUTSIDE_INTERVAL(cache_ptr->verify_tstamp, 
                 csm_sna_cache_verify_to)) || (no_mtu_size == TRUE)) {
            /* Time to verify cache entries */
            CSMV_DEBUG("\nCSM: Test Frame recd. Time to verify cache entries");
	    trans_type = SPEC_WAN_PEER;
	    no_in_list = cache_ptr->no_in_list;
	    for (ii = 0; ii < no_in_list; ii++) {
	        peer_list[ii] = cache_ptr->r_entry[ii].prp_id.peer_id;
		if (cache_ptr->cache_type == DYNAMIC) {
		    cache_ptr->r_entry[ii].prp_id.peer_id = 0;
		    cache_ptr->r_entry[ii].flags = 0;
		    --cache_ptr->no_in_list;
		} else {
		    cache_ptr->r_entry[ii].flags &= ~AVAILABLE;
		}
            }
	    cache_ptr->status = VERIFY;
            GET_TIMESTAMP(cache_ptr->verify_tstamp);
            mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
	    add_icr_pend = TRUE;
        } else if ((csm_expl_wait_rt) && 
                   (CLOCK_IN_INTERVAL(cache_ptr->verify_tstamp,
                                           csm_expl_wait_rt))) {
            trans_type = NO_SEND;
            add_icr_pend = TRUE;
        } else {
            trans_type = NO_SEND;
            add_icr_pend = FALSE;
            o_prp_id.port_id = lan_port_id;
            reach_rc = get_reach_entry_mac(cache_ptr, 0, &o_prp_id, FALSE);
	    if (reach_rc >= 0) {
                /* already checked for ring list filtering */
                ws_mtu_size = min(csm->mtu_bytes,
                                  cache_ptr->r_entry[reach_rc].mtu_size);
                csm_lan_put(SPEC_LAN_PORT, lan_port_id, (csm->ctrl & ~P_BIT),
                            RSPBIT, csm->pf_flag, csm->saddr, csm->daddr, 
                            csm->ssap, csm->dsap, 0, csm->rif, ws_mtu_size,
                            usr_data, usr_data_len, (peerhandle_t) 0);
            } else if (reach_rc == CSM_NO_MTU_SIZE) {
		/* no mtu size available, verify cache entry to get mtu */
                CSM_DEBUG("\nCSM: send cur_ex to get the correct mtu size");
                trans_type = SPEC_WAN_PEER;
		no_in_list = cache_ptr->no_in_list;
		for (ii = 0; ii < no_in_list; ii++) {
		    peer_list[ii] = cache_ptr->r_entry[ii].prp_id.peer_id;
		    if (cache_ptr->cache_type == DYNAMIC) {
			cache_ptr->r_entry[ii].prp_id.peer_id = 0;
			cache_ptr->r_entry[ii].flags = 0;
			--cache_ptr->no_in_list;
		    } else {
			cache_ptr->r_entry[ii].flags &= ~AVAILABLE;
		    }
		}
		cache_ptr->status = VERIFY;
		GET_TIMESTAMP(cache_ptr->verify_tstamp);
		mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
                add_icr_pend = TRUE;
            } else {
                CSME_DEBUG("\nCSM: Invalid reach index %s", csm_error(reach_rc));
            }
        }
        break;
      case VERIFY:
      case SEARCHING:
        /* just drop the frame, let the end station retry it! */
        trans_type = NO_SEND;
	add_icr_pend = TRUE;
        break;
      case UNCONFIRMED:
        no_in_list = 0;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id,
                          lan_port_id)) {
                peer_list[no_in_list] = cache_ptr->r_entry[ii].prp_id.peer_id;
                ++no_in_list;
            }
        }
        if (no_in_list) {
            trans_type = SPEC_WAN_PEER;
            add_icr_pend = TRUE;
            cache_ptr->status = VERIFY;
            mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
        } else {
            trans_type = NO_SEND;
            add_icr_pend = FALSE;
        }
        break;
      case NO_CACHE_INFO:
        trans_type = ALL_WAN_PEERS;
        add_icr_pend = TRUE;
        add_cache_entry = TRUE;
        break;
      case NOT_FOUND:
        trans_type = NO_SEND;
        add_icr_pend = FALSE;
        break;
      default:
        CSME_DEBUG("\nCSM: Invalid cache status %s", csm_stat(ws_status));
        trans_type = NO_SEND;
        add_icr_pend = FALSE;
        break;
    }

    rc_ssp = 0;
    if (trans_type != NO_SEND) {
        ssp_pak = get_ssp_packet(SSP_OP_canureach, 0, 0, 0, 0);
        if (ssp_pak == NULL) {
            CSME_DEBUG("\nCSM: Error getting pak - ssp test_frame_proc()");
        } else {
            ssp_hdr = (ssp_control_frame_t *) ssp_pak->datagramstart;
            mk_ssp_cntrl_header_from_clsipkt(ssp_hdr, clsi_msg, SSP_DIRN_FORWARD);
            ssp_hdr->csex_flags = SSP_FLAGS_EX;
            ssp_hdr->origin_id.dlc_port_id = (uint)lan_port_id->swidb;
            ssp_hdr->origin_id.transport_id = DLSW_XPORT_ID_COOKIE;

            /*
             * In CUR_ex always try to get the best LF possible!
             * Cisco differs in it's use of ssp lf bits from 1795.
             * The peer code will convert cisco ssp lf bits to be
             * 1795 comliant when frame is sent to all peer (ie. the
             * peer handle is zero.
             */
            ssp_hdr->largest_frame_bits = SSP_LF_IGNORE_BIT | RC_LF17800;
        }

        if (trans_type == ALL_WAN_PEERS) {
            if (ssp_pak) {
                rc_ssp = csm_peer_put((peerhandle_t) 0, ssp_pak,
                                      csm->daddr, csm->saddr, csm->dsap, csm->ssap,
                                      (PEER_SSP_TYPE | PEER_SNA_BCAST),
                                      lan_port_id);
                if (!rc_ssp) {
                    CSMV_DEBUG("\nCSM: csm_peer_put returned rc_ssp not OK");
                }
            }
        } else {
            for (ii = 0; ii < no_in_list; ii++) {
                if (ssp_pak) {
                    tmp_pak = pak_duplicate(ssp_pak);
                    if (tmp_pak == NULL) {
                        CSME_DEBUG("\nCSM: Error duplicating pak - ssp test_frame_proc()");
                        break;
                    }
                    rc_ssp += csm_peer_put(peer_list[ii], tmp_pak, csm->daddr,
                                           csm->saddr, csm->dsap, csm->ssap,
                                           (PEER_SSP_TYPE | PEER_SNA_BCAST),
                                           lan_port_id);
                }
                CSMV_DEBUG("\nCSM: csm_peer_put returned rc_ssp %d", rc_ssp);
            }
            if (ssp_pak != NULL) {
                datagram_done(ssp_pak);
            }
        }
        if (rc_ssp == 0) {
            add_cache_entry = FALSE;
            add_icr_pend = FALSE;
        }
    }

    if (add_cache_entry) {
        ws_cache.cache_type = DYNAMIC;
        ws_cache.status = SEARCHING;
	ws_cache.reach_direction = REMOTE;
	ws_cache.no_in_list = 0;
        for (ii = 0; ii < MAX_REACH; ii++) {
            ws_cache.r_entry[ii].prp_id.peer_id = 0;
            ws_cache.r_entry[ii].flags = 0;
            ws_cache.r_entry[ii].mtu_size = 0;
        }
        memset(&ws_cache.del_tstamp, 0, sizeof(ws_cache.del_tstamp));
        memset(&ws_cache.explore_tstamp, 0, sizeof(ws_cache.explore_tstamp));
        rhandle = add_record(RCACHE_MAC, csm->daddr, (void *) &ws_cache);
        if (rhandle == NULL) {
            CSME_DEBUG("\nCSM: test_frame_proc - RCACHE_MAC db add error");
        } else {
            lcache_t     *tmp_ptr;
            tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
            mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                &csm_cache_expl_timer,
                                CSM_RMAC_CACHE, rhandle, FALSE);
            mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                &csm_cache_del_timer,
                                CSM_RMAC_CACHE, rhandle, FALSE);
            mgd_timer_stop(&tmp_ptr->explore_tstamp);
            mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_expl_to);
            GET_TIMESTAMP(tmp_ptr->verify_tstamp);
        }
    }

    if (add_icr_pend) {
        rhandle = seek_record(ICR_PEND, csm->daddr, (void **) &icr_pend);
        while (rhandle != NULL) {
            if ((ieee_compare(csm->saddr, icr_pend->saddr)) 
                       || (icr_pend->port_id != lan_port_id)) {
                rhandle = read_next_record(ICR_PEND, rhandle, csm->daddr,
                                           (void **) &icr_pend);
            } else
                break;
        }
        if (rhandle == NULL) {
            bcopy(csm->saddr, ws_icr_pend.saddr, IEEEBYTES);
            if (csm->rif) {
                bcopy(csm->rif, ws_icr_pend.rif, SRB_MAXRIF);
            } else {
                bzero(ws_icr_pend.rif, SRB_MAXRIF);
            }
            ws_icr_pend.dsap = csm->dsap;
            ws_icr_pend.ssap = csm->ssap;
            ws_icr_pend.port_id = lan_port_id;
            ws_icr_pend.ctrl = csm->ctrl;
            ws_icr_pend.cmd_rsp = cmd_rsp;
            ws_icr_pend.mtu_size = csm->mtu_bytes;
            ws_icr_pend.mtu_flag = SSP_LF_IGNORE_BIT;
            if ( (usr_data_len > 0) && (usr_data_len < MAX_TESTDATA_SIZE)) {
                ws_icr_pend.test_data_len = usr_data_len;
                bcopy(usr_data, (uchar *) &(ws_icr_pend.test_data[0]),
                      usr_data_len);
            } else {
                ws_icr_pend.test_data_len = 0;
            }
            memset(&ws_icr_pend.del_tstamp, 0, sizeof(ws_icr_pend.del_tstamp));
            memset(&ws_icr_pend.respond_timer, 0, sizeof(ws_icr_pend.respond_timer));
            CSMV_DEBUG("\nCSM: adding new icr pend record - test_frame_proc");
            rhandle = add_record(ICR_PEND, csm->daddr, (void *) &ws_icr_pend);
            if (rhandle == NULL) {
                CSME_DEBUG("\nCSM: test_frame_proc - ICR_PEND db add error");
            } else {
                icr_pend_t     *tmp_ptr;
                tmp_ptr = (icr_pend_t *) rhandle_to_data(rhandle);
                mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                    &csm_icr_del_timer,
                                    0, rhandle, FALSE);
                mgd_timer_init_leaf(&tmp_ptr->respond_timer,
                                    &csm_icr_respond_timer,
                                    0, rhandle, FALSE);
                mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_expl_to);
                if (csm_expl_wait_rt) {
                    mgd_timer_start(&tmp_ptr->respond_timer, csm_expl_wait_rt);
                } else {
                    mgd_timer_stop(&tmp_ptr->respond_timer);
                }
            }
        }
    }

    free_clsi_msg(clsi_msg);
    return PROC_OK;
}


/*
 * local_test_frame_proc()
 *
 * Explore local ports for "daddr" parameter.
 *
 * Input parameters:
 *
 *  inport        : input port search was initiated from
 *  clsi_msg      : input clsi_msg that is causing search
 *  daddr         : destination mac address of search target
 *  send_test_data: TRUE if "clsi_msg" is a Test_Stn and the caller
 *                  wants any test data sent when local exploring.
 *                  "clsi_msg" could be another clsi primitive
 *                  and CSM does not have any reachability info
 *                  for daddr, so it needs to generate a test frame.
 *  eat_test_resp : TRUE if caller does not want the test response 
 *                  forwarded back to "inport" if/when it arrives.
 *                  Just need to learn reachability for the "daddr" and 
 *                  start the circuit. 
 *
 * Returns : TRUE if any local test frames sent, FALSE otherwise.
 */
static boolean local_test_frame_proc (dlsw_ring_t *inport, TCLSIMsg *clsi_msg,
                                      csm_info_t *csm, boolean send_test_data,
                                      boolean eat_test_resp)
{
    lcache_t *local_cache_ptr;
    dlsw_ring_t *outport;
    int usr_data_len, rc, ii;
    uchar *usr_data;
    record_handle rhandle;
    resp_pend_t *resp_pend, ws_rpend;
    lcache_t ws_cache;
    ushort msg_type;

    /*
     * First check to see if we are already searching locally.
     * If we find a local entry, then we either known about this
     * resource or are searching for it. Either way, nothing more to do.
     */
    get_mac_cache_entries(csm->daddr, &local_cache_ptr, NULL, NULL, NULL);
    if (local_cache_ptr != NULL)
        return(FALSE);

    /*
     * Do we need to do any local exploring. If this is
     * a "local_switch" interface, or we have some local switch
     * interfaces in the box, then we need to do local exploring.
     */
    rc = 0;
    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);

    CSMV_DEBUG("\nDLSW+: %s I d=%e-%x s=%e-%x %s",
              inport->swidb->hwptr->hw_namestring,
	      csm->daddr, csm->dsap, csm->saddr, csm->ssap, 
 	      (dlsw_can_switch(inport->swidb, inport->swidb, LOCAL_SWITCH) ? 
              "local-switched":""));

    if (send_test_data) {
        usr_data_len = get_usrdata_len_from_clsi(clsi_msg);
        if (usr_data_len)
            usr_data = get_usrdata_from_clsi(clsi_msg);
        else
            usr_data = NULL;
    } else {
        usr_data_len = 0;
        usr_data = NULL;
    }

    /* fix the rif and saddr*/
    csm->ssap &= ~RSPBIT;
    csm->saddr[0] &= ~TR_RII;

    outport = (dlsw_ring_t *) dlsw_ringQ.qhead;
    for (; outport; outport = outport->next)  {
        if (outport->enabled == TRUE) {
            /*
             * Send to all lan ports except the input port.
             * If input port is not a "local_switch" port, then
             * just send to "local_switch" ports (eg. TR to SDLC).
             */
            if (outport == inport)
                continue;

            if (!dlsw_can_switch(inport->swidb, outport->swidb, LOCAL_SWITCH))
                continue;

            if (!dlsw_can_switch(outport->swidb, outport->swidb, 
                                 NETBIOS_SUPPORT)) {
                if ((msg_type == UI) ||
                    (csm->dsap == SAP_NETBIOS) || (csm->dsap == SAP_IBMNM)) {
                    continue;
                }
            }

            CSMV_DEBUG("\nCSM: output dlsw interface %s",
                      outport->swidb->hwptr->hw_namestring);
            /* send TEST frames to dsap 0x00 */
            rc += csm_send_to_disp(outport, LLC_TEST, 0,
                                   P_BIT, csm->daddr, csm->saddr, 0, csm->ssap,
                                   (RC_BROADBIT | RC_SPANBIT), NULL,
                                   csm->mtu_code, usr_data, usr_data_len);
        }
    }

    /*
     * If we explored locally then add LCACHE_MAC and RESP_PEND records.
     */
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
        rhandle = add_record(LCACHE_MAC, csm->daddr, &ws_cache);
        if (rhandle == NULL) {
            CSME_DEBUG("\nCSM: local_test_frame_proc - LCACHE_MAC db add error");
            return(FALSE);
        } else {
            lcache_t     *tmp_ptr;
            tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
            /* initialise timers */
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

        rhandle = seek_record(RESP_PEND, csm->daddr, (void **) &resp_pend);
        if (rhandle == NULL) {
            ieee_copy(csm->saddr, ws_rpend.saddr);
            ws_rpend.ssap = csm->ssap;
            ws_rpend.peer_id = 0;
            ws_rpend.flags = 0;
            ws_rpend.rem_transport_id = 0;
            ws_rpend.trans_type = CSM_LOCAL_SWITCH;
            ws_rpend.localsw_portid = inport;
            ws_rpend.no_in_list = 0;
            ws_rpend.mtu_size = 0;
            ws_rpend.mtu_flag = 0;
            ws_rpend.rem_lanport_id = 0;
            ws_rpend.rem_transport_id = 0;
            bcopy(csm->rif, ws_rpend.rif, rif_length(csm->rif));
            ws_rpend.eat_test_resp = eat_test_resp;

            memset(&ws_rpend.del_tstamp, 0, sizeof(ws_rpend.del_tstamp));
            memset(&ws_rpend.retry_tstamp, 0, sizeof(ws_rpend.retry_tstamp));

            rhandle = add_record(RESP_PEND, csm->daddr, &ws_rpend);
            if (rhandle == NULL) {
                CSME_DEBUG("\nCSM: local_test_frame_proc - RESP_PEND db add error");
                return(FALSE);
            } else {
                resp_pend_t *tmp_ptr;
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
        return(TRUE);
    }
    return(FALSE);
}


/*
 * Process response frame from LAN.
 */
static csm_rc_t resp_frame_proc (lportid_t lan_port_id, TCLSIMsg *clsi_msg,
                          csm_info_t *csm)
{
    int             rc, reach_rc, usr_data_len;
    long            ws_mtu_size, path_mtu;
    uchar           *usr_data;
    boolean         keep_record;
    char            rc_code;

    csm_rc_t        core_rc;
    resp_pend_t     *rpend_ptr;
    startdl_pend_t  *startdl_ptr;
    record_handle   rhandle;
    paktype         *ssp_pak;
    lcache_t        *cache_ptr;
    reach_entry_t   reach_entry;
    ssp_control_frame_t *ssp_hdr;
    u_msg_ptr_t      u_msg;
    u_portid_type_t o_prp_id;
    lportid_t       target_port_id;

    /* fix the saddr*/
    csm->ssap &= ~RSPBIT;
    csm->saddr[0] &= ~TR_RII;

    /* 
     * Read the response pending file and startdl pending files
     * If no match discard frame. If there is a match, in
     * response pending file, send ICRs out. If there is a 
     * match in startdl pending file, send message to FSM to start
     * a new link station.
     */

    ssp_pak = NULL;
    keep_record = FALSE;
    rhandle = seek_record(RESP_PEND, csm->saddr, (void **) &rpend_ptr);
    while (rhandle != NULL) {
        if (rpend_ptr->peer_id != 0) {
            /* NULL peer id is just a verify explorer, do not send icr */
            if ((dlsw_ignore_port_lf(lan_port_id) ||
                 (csm->mtu_bytes <= rpend_ptr->mtu_size) ||
                 (rpend_ptr->mtu_flag & SSP_LF_IGNORE_BIT)) &&
                (peer_pass_port_filter(rpend_ptr->peer_id, lan_port_id))) {
                ssp_pak = get_ssp_packet(SSP_OP_icanreach, 0, 0,
                                         rpend_ptr->rem_lanport_id, 0);
                if (ssp_pak == NULL) {
                    CSME_DEBUG("\nCSM: Error getting pak - ssp resp_frame_proc");
                } else {
                    ssp_hdr = (ssp_control_frame_t *) ssp_pak->datagramstart;
                    mk_ssp_cntrl_header_from_clsipkt(ssp_hdr, clsi_msg,
                                                     SSP_DIRN_BACKWARD);
                    ssp_hdr->csex_flags = SSP_FLAGS_EX;
                    ssp_hdr->target_id.dlc_port_id = (uint)lan_port_id->swidb;
                    ssp_hdr->target_id.transport_id = DLSW_XPORT_ID_COOKIE;
                    ssp_hdr->origin_id.dlc_port_id = rpend_ptr->rem_lanport_id;
                    ssp_hdr->origin_id.transport_id =
                                                 rpend_ptr->rem_transport_id;
                    ws_mtu_size = min(csm->mtu_bytes, rpend_ptr->mtu_size);
                    if (peer_get_vendor_type(rpend_ptr->peer_id) == 
                                                        PEER_NOT_CISCO) {
                        ssp_hdr->largest_frame_bits =
                                 bytes_to_ssp_lf_bits(ws_mtu_size);
                    } else {
                        ssp_hdr->largest_frame_bits =
                                 bytes_to_rc_code(ws_mtu_size);
                    }
                    rc = csm_peer_put(rpend_ptr->peer_id, ssp_pak,
                                      csm->daddr, csm->saddr, csm->dsap,
                                      csm->ssap, PEER_SSP_TYPE, NULL);
                    if (!rc) {
                        CSMV_DEBUG("\nCSM: returned from csm_peer_put rc not OK");
                    }
                    keep_record = FALSE;
                }
            } else {
                keep_record = TRUE;
            }
        } else {
            /*
             * Since peer handle in resp pending record is NULL, let's
             * check to see if we are local switching. Send a TEST response
             * on the origin lan if necessary. The 'eat_test_resp' flag will
             * be FALSE for SDLC (QLLC?) interfaces.
             */
            if (rpend_ptr->eat_test_resp == FALSE) {
                get_mac_cache_entries(csm->daddr, &cache_ptr, NULL, NULL, NULL);
                if (cache_ptr) {
                    /*
                     * We can reach the target resource locally. Now check
                     * if one of these ports is "local bridging". First
                     * get the port of the target resource.
                     */
                    reach_rc = get_reach_entry_mac(cache_ptr, 0, NULL, FALSE);
                    if (reach_rc < 0) {
                        CSME_DEBUG("\nCSM: local resp_frame_proc() reach_rc %s"
                                 ,csm_error(reach_rc));
                        rhandle = read_next_record(RESP_PEND, rhandle, csm->saddr,
                                                   (void **) &rpend_ptr);
                        continue;
                    }

                    target_port_id =
                              cache_ptr->r_entry[reach_rc].prp_id.port_id;

                    if (!dlsw_can_switch(lan_port_id->swidb,
                                        target_port_id->swidb, LOCAL_SWITCH)) {
                        rhandle = read_next_record(RESP_PEND, rhandle, csm->saddr,
                                                   (void **) &rpend_ptr);
                        continue;
                    }

                    /*
                     * Ok to send this test response locally.
                     */
                    usr_data_len = get_usrdata_len_from_clsi(clsi_msg);
                    if (usr_data_len)
                        usr_data = get_usrdata_from_clsi(clsi_msg);
                    else
                        usr_data = NULL;

                    path_mtu = min(csm->mtu_bytes,
                                   cache_ptr->r_entry[reach_rc].mtu_size);
                    csm_lan_put(SPEC_LAN_PORT, target_port_id,
                                LLC_TEST, RSPBIT, P_BIT, csm->daddr, csm->saddr,
                                csm->dsap, csm->ssap, DIRECT_FRAME,
                                cache_ptr->r_entry[reach_rc].rif, path_mtu,
                                usr_data, usr_data_len, (peerhandle_t) 0);
                } else {
                    CSM_DEBUG("\nCSM: resp_pend, no peer, no local for %e",
                              csm->daddr);
                }
            }
        }

        if (keep_record == FALSE) {
            mgd_timer_stop(&rpend_ptr->del_tstamp);
            mgd_timer_stop(&rpend_ptr->retry_tstamp);
            rhandle = delete_record_get_next(RESP_PEND, rhandle);
            if (!(match_record_key(rhandle, csm->saddr))) {
                rhandle = NULL;
                rpend_ptr = NULL;
            } else {
                rpend_ptr = rhandle_to_data(rhandle);
            }
        } else {
            rhandle = read_next_record(RESP_PEND, rhandle, csm->saddr,
                                              (void **) &rpend_ptr);
        }
    }

    rhandle = seek_record(STARTDL_PEND, csm->saddr, (void **) &startdl_ptr);
    if (rhandle != NULL) {
        get_mac_cache_entries(csm->saddr, &cache_ptr, NULL, NULL, NULL);
    }
    while (rhandle != NULL) {
        if ((startdl_ptr->type == DLX_MSG) || (startdl_ptr->type == SSP_MSG)) {
            o_prp_id.peer_id = startdl_ptr->prp_id.peer_id;
            if (startdl_ptr->mtu_flag & SSP_LF_IGNORE_BIT) {
                reach_rc = get_reach_entry_mac(cache_ptr, 0, &o_prp_id, TRUE);
            } else {
                reach_rc = get_reach_entry_mac(cache_ptr, 
                                               startdl_ptr->mtu_size, 
                                               &o_prp_id, TRUE);
            }
            if (reach_rc < 0) {
	        CSM_DEBUG("\nCSM: No matching entry rc %s", 
                          csm_error(reach_rc));
                rhandle = read_next_record(STARTDL_PEND, rhandle, csm->saddr,
                                               (void **) &startdl_ptr);
                continue;
            } else {
	        bcopy(&(cache_ptr->r_entry[reach_rc]), &reach_entry,
		        sizeof(reach_entry_t));
                reach_entry.mtu_size = min(reach_entry.mtu_size,
                                           startdl_ptr->mtu_size);
                if (reach_entry.rif[0] != 0) {
                    ws_mtu_size = rc_code_to_bytes((reach_entry.rif[1] &
                                                    RC_LFMASK));
                    if (ws_mtu_size > reach_entry.mtu_size) {
                        rc_code = bytes_to_rc_code(reach_entry.mtu_size);
                        reach_entry.rif[1] = (reach_entry.rif[1] & ~RC_LFMASK)
                                              | rc_code;
                    }
                }
            }
            
            u_msg.pak = startdl_ptr->u_msg_ptr.pak;
	    core_rc = csm_to_core(SSP_START_NEWDL, &(startdl_ptr->prp_id), 
	            &u_msg, &reach_entry);
            if (core_rc != PROC_OK) {
                CSM_DEBUG("\nCSM: core rejected frame - ssp resp_frame_proc");
	        datagram_done(startdl_ptr->u_msg_ptr.pak);
	        startdl_ptr->u_msg_ptr.pak = NULL;
            }
        } else if ((lan_port_id == startdl_ptr->prp_id.port_id) ||
           !dlsw_can_switch(lan_port_id->swidb, 
                            startdl_ptr->prp_id.port_id->swidb, LOCAL_SWITCH)) {
            /*
             * CLSI type! Reachable locally. Can't local switch.
             *
             * The above logic goes like this...
             * If source and destination lan ports are the same OR both
             * ports are NOT "local_switch" enabled, can't local switch.
             */
            free_clsi_msg(startdl_ptr->u_msg_ptr.clsi_msg);
        } else {
            /*
             * CLSI type! Reachable locally. Attempt to local switch.
             * Find the local cache entry for the src of the response.
             */
            get_mac_cache_entries(csm->saddr, &cache_ptr, NULL, NULL, NULL);
            if ((cache_ptr) &&
                (cache_ptr->status != NOT_FOUND) &&
                (cache_ptr->status != SEARCHING)) {
                /*
                 * We can reach the target resource locally. Now check
                 * if one of these ports is "local bridging". First
                 * get the port of the target resource.
                 */
                o_prp_id.port_id = startdl_ptr->prp_id.port_id;
                reach_rc = get_reach_entry_mac(cache_ptr, 0, &o_prp_id, TRUE);
                if (reach_rc < 0) {
                    CSME_DEBUG("\nCSM: local new_connection() reach_rc %s",
                              csm_error(reach_rc));
                } else {
                    csm_to_local(startdl_ptr->prp_id.port_id,
                                 &(cache_ptr->r_entry[reach_rc]),
                                 startdl_ptr->u_msg_ptr.clsi_msg);
                    startdl_ptr->u_msg_ptr.clsi_msg = NULL;
                }
            }
        }

        mgd_timer_stop(&startdl_ptr->del_tstamp);
        mgd_timer_stop(&startdl_ptr->retry_tstamp);
        mgd_timer_stop(&startdl_ptr->respond_timer);
        rhandle = delete_record_get_next(STARTDL_PEND, rhandle);
        if (!(match_record_key(rhandle, csm->saddr))) {
            rhandle = NULL;
            startdl_ptr = NULL;
        } else {
            startdl_ptr = rhandle_to_data(rhandle);
        }
    }

    if (clsi_msg)
        free_clsi_msg(clsi_msg);

    return PROC_OK;

}




/*
 * Handle sabme commands or xid to non null saps.
 */
static csm_rc_t new_connection (lportid_t lan_port_id, TCLSIMsg *clsi_msg,
                         csm_info_t *csm)
{

    char             ws_status;
    int              reach_rc, ii, no_in_list, rc_ssp, bcmp_rc;
    boolean          rc_local, pod_retry;
    boolean          add_startdl_pend, add_cache_entry, ring_list_match;

    csm_rc_t         core_rc;
    e_xmission_type  trans_type;
    record_handle    rhandle;
    ssp_control_frame_t *ssp_hdr;
    csm_rc_t         csm_rc;
    lcache_t         *cache_ptr, *local_cache_ptr, ws_cache;
    startdl_pend_t   *startdl_ptr, ws_startdl;
    u_portid_type_t  prp_id;
    u_msg_ptr_t      u_msg;
    paktype          *ssp_pak, *tmp_pak;
    peerhandle_t     peer_list[MAX_REACH];
    u_portid_type_t o_prp_id;
    reach_entry_t   reach_entry;
    reach_entry_t    *dst_re;

    csm_rc = PROC_OK;
    rc_ssp = 0;
    rc_local = FALSE;
    rhandle = NULL;

    /* fix saddr and change rif direction */
    csm->ssap &= ~RSPBIT;
    csm->saddr[0] &= ~TR_RII;

    if ((!dlsw_ignore_port_lf(lan_port_id)) &&
        ((csm->mtu_bytes > BOX_MTU_SIZE) ||
         (csm->mtu_bytes > portid_to_mtu(lan_port_id)))) {
        /* Cannot handle the mtu_size requested. Drop frame. */
        CSME_DEBUG("\nCSM: Can't handle mtu_size %d on %s, drop frame",
                  csm->mtu_bytes, lan_port_id->swidb->hwptr->hw_namestring);
        free_clsi_msg(clsi_msg);
        return PROC_OK;
    }

    get_mac_cache_entries(csm->daddr, &local_cache_ptr, NULL, &cache_ptr, NULL);
    if (local_cache_ptr != NULL) {
	if ((local_cache_ptr->status != NOT_FOUND) &&
                              (local_cache_ptr->status != SEARCHING)) {
            /*
             * We can reach the target resource locally. Now check
             * if one of these ports is "local bridging". First
             * get the port of the target resource.
             */
            o_prp_id.port_id = lan_port_id;
            reach_rc = get_reach_entry_mac(local_cache_ptr, 0, NULL, TRUE);
            if (reach_rc < 0) {
                CSME_DEBUG("\nCSM: local new_connection() reach_rc %s",
                          csm_error(reach_rc));
                free_clsi_msg(clsi_msg);
                return PROC_OK;
            }

            /*
             * If origin and target port are different and at least one of 
             * them is local switch capable, then start a local circuit.
             */
            dst_re = &local_cache_ptr->r_entry[reach_rc];
            if ((lan_port_id != dst_re->prp_id.port_id) &&
                dlsw_can_switch(lan_port_id->swidb,
                                dst_re->prp_id.port_id->swidb, LOCAL_SWITCH)) {
                /* csm_to_local() eats the clsi msg */
                csm_to_local(lan_port_id, dst_re, clsi_msg);

                /*
                 * Clean up any STARTDL_PEND records since we would not want 
                 * a test response to try to re-start the same circuit.
                 */
                rhandle = seek_record(STARTDL_PEND, csm->daddr,
                                      (void **) &startdl_ptr);
                while (rhandle) {
                    if (startdl_ptr->type == CLSI_MSG) {
                        bcmp_rc = ieee_compare(csm->saddr, startdl_ptr->saddr);
                        if (bcmp_rc == 0) {
                            if (startdl_ptr->u_msg_ptr.clsi_msg) {
                                free_clsi_msg(startdl_ptr->u_msg_ptr.clsi_msg);
                                startdl_ptr->u_msg_ptr.clsi_msg = NULL;
                            } 
    
                            mgd_timer_stop(&startdl_ptr->del_tstamp);
                            mgd_timer_stop(&startdl_ptr->retry_tstamp);
                            mgd_timer_stop(&startdl_ptr->respond_timer);
                            rhandle = delete_record_get_next(STARTDL_PEND, rhandle);
                            startdl_ptr = rhandle_to_data(rhandle);
                            continue;
                        }
                    }
                    rhandle = read_next_record(STARTDL_PEND,rhandle,csm->daddr,
                                               (void **) &startdl_ptr);
                }
                return PROC_OK;
            }
        }
    }

    if (cache_ptr == NULL) {
        ws_status = NO_CACHE_INFO;
    } else {
        ws_status = cache_ptr->status;
    }

    CSMV_DEBUG("\nCSM: new_connection: ws_status = %s", csm_stat(ws_status));
    trans_type = NO_SEND;
    add_startdl_pend = FALSE;
    add_cache_entry = FALSE;
    pod_retry = FALSE;
    no_in_list = 0;
    switch (ws_status) {
      case FOUND:
        ring_list_match = FALSE;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id,
                                      lan_port_id)) {
                ring_list_match = TRUE;
            } 
            if (ring_list_match) {
                break;
            }
        }

        if (ring_list_match == FALSE) {
            /*
             * Allow port list to do it's job and drop packet.
             * When current cache times out, we'll explore to all peers.
             */
            free_clsi_msg(clsi_msg);
            return PROC_OK;
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
            CSMV_DEBUG("\nCSM: New connection Time to verify cache entries");
            no_in_list = cache_ptr->no_in_list;
            for (ii = 0; ii < no_in_list; ii++) {
	        peer_list[ii]=cache_ptr->r_entry[ii].prp_id.peer_id;
		if (cache_ptr->cache_type == DYNAMIC) {
		    cache_ptr->r_entry[ii].prp_id.peer_id = 0;
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
            trans_type = SPEC_WAN_PEER;
	    cache_ptr->status = VERIFY;
            add_startdl_pend = TRUE;
            GET_TIMESTAMP(cache_ptr->verify_tstamp);
            mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
        } else if ((csm_expl_wait_rt) && 
                   (CLOCK_IN_INTERVAL(cache_ptr->verify_tstamp,
                                           csm_expl_wait_rt))) {
            trans_type = NO_SEND;
            add_startdl_pend = TRUE;
        } else {
            o_prp_id.port_id = lan_port_id;
            if (dlsw_ignore_port_lf(lan_port_id)) {
                reach_rc = get_reach_entry_mac(cache_ptr, 0,
                                               &o_prp_id, TRUE);
            } else {
                reach_rc = get_reach_entry_mac(cache_ptr, csm->mtu_bytes,
                                               &o_prp_id, TRUE);
            }

            if (reach_rc >= 0) {
                /* ring list filter already checked */
                if (is_fast_peer(cache_ptr->r_entry[reach_rc].prp_id.peer_id)) {
                    csm->saddr[0] |= TR_RII; /* fast cache needs RII set */
                    dlsw_fct_lan2wan_start(clsi_msg,
                             cache_ptr->r_entry[reach_rc].prp_id.peer_id,
                             csm->daddr, csm->dsap, csm->ssap);
                    free_clsi_msg(clsi_msg);
                    return PROC_OK;
                }

	        prp_id.port_id = lan_port_id;
	        u_msg.clsi_msg = clsi_msg;
                bcopy(&(cache_ptr->r_entry[reach_rc]), &reach_entry,
                        sizeof(reach_entry_t));

                /*
                 * mtu_size was calculated above to be the min of the rif lf
                 * (if present) and the box lf. For port types where we should
                 * ignore the lan lf, start circuit with an mtu that is the
                 * smallest of the mtu_size, peer's max lf, and remote reach
                 * entry's max lf.
                 */
                if (dlsw_ignore_port_lf(lan_port_id)) {
                    reach_entry.mtu_size = min(csm->mtu_bytes,
                                    peerid_to_mtu(reach_entry.prp_id.peer_id));
                    reach_entry.mtu_size = min(reach_entry.mtu_size,
                                    cache_ptr->r_entry[reach_rc].mtu_size);
                } else {
                    /* set the mtu size in reach to the one requested */
                    reach_entry.mtu_size = csm->mtu_bytes;
                }

                CSMV_DEBUG("\nCSM: Calling csm_to_core with CLSI_START_NEWDL");
	        core_rc = csm_to_core(CLSI_START_NEWDL, &prp_id, &u_msg, 
	                              &reach_entry);
                if (core_rc == FRAME_REJ) {
                    CSME_DEBUG("\nCSM: core rejected frame - new_connection");
	            free_clsi_msg(clsi_msg);
                } else if (core_rc == POD_PEER_NOT_READY) {
                    /* add to startdl pend file */
                    add_startdl_pend = TRUE;
                    trans_type = NO_SEND;
                    pod_retry = TRUE;
                }
                /*
                 * check for existing STARTDL_PEND record 
                 * if we have one and and the pod is still not ready,
                 * keep the existing one, else purge the existing record
                 */
                rhandle = seek_record(STARTDL_PEND, csm->daddr,
                                      (void **) &startdl_ptr);
                while (rhandle) {
                    if (startdl_ptr->type == CLSI_MSG) {
                        bcmp_rc = ieee_compare(csm->saddr, startdl_ptr->saddr);
                        if (bcmp_rc == 0) {
                            if (add_startdl_pend) {
                                add_startdl_pend = FALSE;
                            } else {
                                CSMV_DEBUG("\nCSM: purging startdl_pend record");
                                if (startdl_ptr->u_msg_ptr.clsi_msg) {
                                    free_clsi_msg(startdl_ptr->u_msg_ptr.clsi_msg);
                                    startdl_ptr->u_msg_ptr.clsi_msg = NULL;
                                }
                                mgd_timer_stop(&startdl_ptr->del_tstamp);
                                mgd_timer_stop(&startdl_ptr->retry_tstamp);
                                mgd_timer_stop(&startdl_ptr->respond_timer);
                                delete_record_get_next(STARTDL_PEND, rhandle);
                                break;
                            }
                        }
                    }
                    rhandle = read_next_record(STARTDL_PEND, rhandle, csm->daddr,
                                               (void **) &startdl_ptr);
                }

            } else if (reach_rc == CSM_NO_MTU_SIZE) {
                CSM_DEBUG("\nCSM: send cur_ex to get the correct mtu size");
                no_in_list = 0;
                for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
		    if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id, lan_port_id)) {
                        peer_list[no_in_list] = cache_ptr->r_entry[ii].prp_id.peer_id;
                        ++no_in_list;
		    }
                }
		if (no_in_list) {
                    trans_type = SPEC_WAN_PEER;
                    add_startdl_pend = TRUE;
		} else {
		    CSME_DEBUG("\nCSM: no mtu size and cur_ex didn't pass port filter");
	            free_clsi_msg(clsi_msg);
                    return PROC_OK;
		}
            } else {
	        CSME_DEBUG("\nCSM: reach entry %s - mtu size %d",
                    csm_error(reach_rc), csm->mtu_bytes);
	        free_clsi_msg(clsi_msg);
                return PROC_OK;
	    }
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
	trans_type = ALL_WAN_PEERS;
	no_in_list = 0;
        add_startdl_pend = TRUE;
        add_cache_entry = TRUE;
	csm_rc = PROC_OK;
        break;

      case UNCONFIRMED:
        no_in_list = 0;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id,
                                      lan_port_id)) {
	        peer_list[no_in_list] = cache_ptr->r_entry[ii].prp_id.peer_id;
                ++no_in_list;
            }
        }
        if (no_in_list) {
            trans_type = SPEC_WAN_PEER;
	    cache_ptr->status = VERIFY;
            mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
            add_startdl_pend = TRUE;
	    csm_rc = PROC_OK;
        } else {
            trans_type = NO_SEND;
            free_clsi_msg(clsi_msg);
            csm_rc = PROC_OK;
        }
	break;

      case NOT_FOUND:
	CSM_DEBUG("\nCSM: new connection....cache status is NOTFOUND");
        free_clsi_msg(clsi_msg);
        csm_rc = PROC_OK;
        break;

      default:
	CSM_DEBUG("\nCSM: Invalid Cache status - %s....Dropping frame", 
                  csm_stat(ws_status));
	free_clsi_msg(clsi_msg);
	csm_rc = PROC_OK;
	break;

    }

    /*
     * If we are searching everywhere, explore on "local_switch" interfaces.
     * local_test_frame_proc() returns TRUE if it sent any explorers.
     */
    if (trans_type == ALL_WAN_PEERS) {
        rc_local = local_test_frame_proc(lan_port_id, clsi_msg,
                                         csm, FALSE, TRUE);
        if (rc_local)
            add_startdl_pend = TRUE;
    }

    if (trans_type != NO_SEND) {
        ssp_pak = get_ssp_packet(SSP_OP_canureach, 0, 0, 0, 0);
        if (ssp_pak != NULL) {
            ssp_hdr = (ssp_control_frame_t *)ssp_pak->datagramstart;
            mk_ssp_cntrl_header_from_clsipkt(ssp_hdr, clsi_msg,
                                             SSP_DIRN_FORWARD);
            ssp_hdr->csex_flags = SSP_FLAGS_EX;
            ssp_hdr->origin_id.dlc_port_id = (uint)lan_port_id->swidb;
            ssp_hdr->origin_id.transport_id = DLSW_XPORT_ID_COOKIE;

            /*
             * In CUR_ex always try to get the best LF possible!
             * Cisco differs in it's use of ssp lf bits from 1795.
             * The peer code will convert cisco ssp lf bits to be
             * 1795 comliant when frame is sent to all peer (ie. the
             * peer handle is zero.
             */
            ssp_hdr->largest_frame_bits = SSP_LF_IGNORE_BIT |
                                          bytes_to_rc_code(BOX_MTU_SIZE);
        } else {
            CSME_DEBUG("\nCSM: Error getting pak - ssp new_connection");
        }

        if (trans_type == ALL_WAN_PEERS) {
            if (ssp_pak) {
                rc_ssp = csm_peer_put((peerhandle_t) 0, ssp_pak,
                                      csm->daddr, csm->saddr, csm->dsap, csm->ssap,
                                      (PEER_SSP_TYPE | PEER_SNA_BCAST),
                                      lan_port_id);
            }
        } else {
            for (ii = 0; ii < no_in_list; ii++) {
                if (ssp_pak) {
                    /* send packet to peer manager */
                    tmp_pak = pak_duplicate(ssp_pak);
                    if (tmp_pak == NULL) {
                        CSME_DEBUG("\nCSM: Error duplicating pak - new_connection");
                        break;
                    }
                    rc_ssp = csm_peer_put(peer_list[ii], tmp_pak,
                                          csm->daddr, csm->saddr, csm->dsap,
                                          csm->ssap,
                                          (PEER_SSP_TYPE | PEER_SNA_BCAST),
                                          lan_port_id);
                }
            }
            if (ssp_pak) {
                datagram_done(ssp_pak);
            }
        }

        if (rc_ssp) {
	    if (add_cache_entry) {
                ws_cache.cache_type = DYNAMIC;
                ws_cache.status = SEARCHING;
	        ws_cache.reach_direction = REMOTE;
		ws_cache.no_in_list = 0;
                for (ii = 0; ii < MAX_REACH; ii++) {
                    ws_cache.r_entry[ii].prp_id.peer_id = 0;
                    ws_cache.r_entry[ii].flags = 0;
                }
                memset(&ws_cache.del_tstamp, 0, sizeof(ws_cache.del_tstamp));
                memset(&ws_cache.explore_tstamp, 0, sizeof(ws_cache.explore_tstamp));
                rhandle = add_record(RCACHE_MAC, csm->daddr, (void *) &ws_cache);
                if (rhandle == NULL) {
                    CSME_DEBUG("\nCSM: new_connection - RCACHE_MAC db add error");
                } else {
                    lcache_t     *tmp_ptr;
                    tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
                    /* initialise timers */
                    mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                        &csm_cache_expl_timer,
                                        CSM_RMAC_CACHE, rhandle, FALSE);
                    mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                        &csm_cache_del_timer,
                                        CSM_RMAC_CACHE, rhandle, FALSE);
                    mgd_timer_stop(&tmp_ptr->explore_tstamp);
                    mgd_timer_start(&tmp_ptr->del_tstamp, csm_sna_expl_to);
                    GET_TIMESTAMP(tmp_ptr->verify_tstamp);
                }
            }
        } else if (rc_local == FALSE) {
            if (add_startdl_pend) {
                add_startdl_pend = FALSE;
                free_clsi_msg(clsi_msg);
            }
        }
    }

    if (add_startdl_pend) {
	/* add to startdl pend file */
        rhandle = seek_record(STARTDL_PEND, csm->daddr, (void **) &startdl_ptr);
        if (rhandle != NULL) {
            if (startdl_ptr->type == CLSI_MSG) {
                bcmp_rc = ieee_compare(csm->saddr, startdl_ptr->saddr);
                if (bcmp_rc == 0) {
                    free_clsi_msg(clsi_msg);
                    return csm_rc;
                }
            }
        }
	ieee_copy(csm->saddr, ws_startdl.saddr);
	ws_startdl.type = CLSI_MSG;
	ws_startdl.u_msg_ptr.clsi_msg = clsi_msg;
	ws_startdl.prp_id.port_id = lan_port_id;
        ws_startdl.mtu_size = csm->mtu_bytes;
        ws_startdl.mtu_flag = 0;
        ws_startdl.retry_count = 0;
        if (pod_retry) {
            bcopy(&reach_entry, &ws_startdl.retry_reach_info,
                  sizeof(reach_entry_t));
        } else {
            memset(&ws_startdl.retry_reach_info, 0, sizeof(reach_entry_t));
        }
        memset(&ws_startdl.del_tstamp, 0, sizeof(ws_startdl.del_tstamp));
        memset(&ws_startdl.retry_tstamp, 0, sizeof(ws_startdl.retry_tstamp));
        memset(&ws_startdl.respond_timer, 0, sizeof(ws_startdl.respond_timer));
        rhandle = add_record(STARTDL_PEND, csm->daddr, (void *) &ws_startdl);
        if (rhandle == NULL) {
            CSME_DEBUG("\nCSM: new_connection - STARTDL_PEND db add error");
	    free_clsi_msg(clsi_msg);
        } else {
            startdl_pend_t   *tmp_ptr;
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
            if (csm_expl_wait_rt) {
                mgd_timer_start(&tmp_ptr->respond_timer, csm_expl_wait_rt);
            } else {
                mgd_timer_stop(&tmp_ptr->respond_timer);
            }
            if (pod_retry) {
                mgd_timer_start(&tmp_ptr->retry_tstamp, csm_pod_retry_to);
            } else {
                mgd_timer_stop(&tmp_ptr->retry_tstamp);
            }

        }
    }

    return csm_rc;
}





/*
 * Handle Contextless non-Netbios UI Frames
 */
static csm_rc_t nonnb_uiframe_proc (lportid_t lan_port_id, TCLSIMsg *clsi_msg,
                             csm_info_t *csm)
{
    int                 ii, no_in_list;
    int                 data_len;
    char                *usr_data;
    e_xmission_type     trans_type;

    lcache_t            *cache_ptr, *local_cache_ptr;
    peerhandle_t        peer_list[MAX_REACH];

    cache_ptr = NULL;
    local_cache_ptr = NULL;
    trans_type = NO_SEND;
    no_in_list = 0;

    usr_data = get_usrdata_from_clsi(clsi_msg);
    data_len = get_usrdata_len_from_clsi(clsi_msg);
    if ((usr_data == NULL) || (data_len <= 0)) {
        SNA_CSM_DEBUG("\nCSM: Error parsing clsi message - nonnb_uiframe_proc 2");
        free_clsi_msg(clsi_msg);
        return PROC_OK;
    }

    /* fix the rif and saddr*/
    csm->ssap &= ~RSPBIT;
    csm->saddr[0] &= ~TR_RII;

    if (csm->daddr[2] & TR_GROUP) {
        cache_ptr = NULL;
    } else {
        get_mac_cache_entries(csm->daddr, NULL, NULL, &cache_ptr, NULL);
    }
    if (cache_ptr == NULL) {
        SNA_CSMV_DEBUG("\nCSM: No cache info, xcasting UI");
        trans_type = ALL_WAN_PEERS;
	no_in_list = 0;
    } else {
        SNA_CSM_DEBUG("\nCSM: UI Cache entry status = %s", 
                      csm_stat(cache_ptr->status));
        switch (cache_ptr->status) {
          case NOT_FOUND:
            trans_type = NO_SEND;
            break;
          case FOUND:
          case VERIFY:
            trans_type = NO_SEND;
            for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id,
                                          lan_port_id)) {
		    trans_type = SPEC_WAN_PEER;
		    no_in_list = 1;
                    peer_list[0] = cache_ptr->r_entry[ii].prp_id.peer_id;
                    break;
                } 
            } 
            if (trans_type == NO_SEND) {
                free_clsi_msg(clsi_msg);
                return PROC_OK;
            }
            break;
          case UNCONFIRMED:
            no_in_list = 0;
            for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id,
                                          lan_port_id)) {
	            peer_list[no_in_list]=cache_ptr->r_entry[ii].prp_id.peer_id;
                    ++no_in_list;
                }
            } 
            if (no_in_list) {
                trans_type = SPEC_WAN_PEER;
            } else {
                trans_type = NO_SEND;
            }
            break;
          case SEARCHING:
            trans_type = ALL_WAN_PEERS;
            break;
          default:
            SNA_CSM_DEBUG("\nCSM: Cache status error %s ... xcasting frame", 
                           csm_stat(cache_ptr->status));
            trans_type = ALL_WAN_PEERS;
            break;
        } 
    }

    if (trans_type != NO_SEND) {
        send_ui_frame_to_peers(peer_list, no_in_list, usr_data, data_len, 
                                SSP_OP_dataframe, lan_port_id, 0,
                                csm->mtu_bytes, SSP_LF_IGNORE_BIT,
                                0, 0, csm);
    }

    free_clsi_msg(clsi_msg);
    return PROC_OK;
}



/*
 * Handle Contextless Netbios Frames
 */
static csm_rc_t nb_frame_proc (lportid_t lan_port_id, TCLSIMsg *clsi_msg,
                        csm_info_t *csm)
{
    if (!csm->nb_hdr) {
        free_clsi_msg(clsi_msg);
        return(PROC_OK);
    }

    /* fix rif, saddr and sap */
    csm->saddr[0] &= ~TR_RII;

    NETB_CSM_DEBUG("\nCSM: Received frame type NETBIOS %s from %e, %s", 
        csm_nbcom(csm->nb_hdr->command), csm->saddr,
        lan_port_id->swidb->hwptr->hw_short_namestring);

    switch (csm->nb_hdr->command) {
      case NETBIOS_ADD_GROUP_QUERY:
      case NETBIOS_DATAGRAM_BROADCAST:
      case NETBIOS_TERMINATE_TRACE_REM:
      case NETBIOS_TERMINATE_TRACE_REM_LOC:
      case NETBIOS_STATUS_QUERY:
      case NETBIOS_DATAGRAM:
      case NETBIOS_STATUS_RESPONSE:
      case NETBIOS_NAME_IN_CONFLICT:
      case NETBIOS_ADD_NAME_QUERY:
      case NETBIOS_ADD_NAME_RESPONSE:
        nb_data_frame_proc(lan_port_id, clsi_msg, csm);
        break;
      case NETBIOS_NAME_QUERY:
        nb_nq_frame_proc(lan_port_id, clsi_msg, csm);
        break;
      case NETBIOS_NAME_RECOGNIZED:
        nb_nr_frame_proc(lan_port_id, clsi_msg, csm);
        break;
      default:
	CSM_DEBUG("\nCSM: Invalid netbios command, 0x%x", 
                  csm->nb_hdr->command);
	free_clsi_msg(clsi_msg);
        break;
    }

    return PROC_OK;

}



static void nb_data_frame_proc (lportid_t lan_port_id, TCLSIMsg *clsi_msg,
                         csm_info_t *csm)
{

    int                 rc, ii, no_in_list;
    int                 data_len;
    uchar               ssp_frame_type;
    boolean             add_nbanq_db;

    record_handle  rhandle;
    e_xmission_type     trans_type;
    peerhandle_t        peer_list[MAX_REACH];
    lcache_t            *cache_ptr, *local_cache_ptr;
    nbanq_info_t        *nbanq_ptr, ws_nbanq;

    add_nbanq_db = FALSE;
    no_in_list = 0;
    trans_type = NO_SEND;

    data_len = get_usrdata_len_from_clsi(clsi_msg);

    switch (csm->nb_hdr->command) {
      case NETBIOS_STATUS_QUERY:
      case NETBIOS_DATAGRAM:
        get_name_cache_entries(csm->nb_hdr->destname, &local_cache_ptr, NULL, 
			       &cache_ptr, NULL);
        break;
      case NETBIOS_STATUS_RESPONSE:
      case NETBIOS_NAME_IN_CONFLICT:
      case NETBIOS_ADD_NAME_RESPONSE:
        get_mac_cache_entries(csm->daddr, &local_cache_ptr, NULL,
                              &cache_ptr, NULL);
        break;
      default:
        cache_ptr = NULL;
        local_cache_ptr = NULL;
        break;
    }

    ssp_frame_type = SSP_OP_dataframe;
    switch (csm->nb_hdr->command) {
      case NETBIOS_ADD_NAME_QUERY:
        rhandle = seek_record(NBANQ_FILE, csm->nb_hdr->destname, 
                                          (void **) &nbanq_ptr);
        if ((nbanq_ptr != NULL) && (nbanq_ptr->direction != LOCAL)) {
            rhandle = read_next_record(NBANQ_FILE, rhandle, csm->nb_hdr->destname,
                                       (void **) &nbanq_ptr);
        }
        if ((nbanq_ptr != NULL) && (nbanq_ptr->direction == LOCAL)) {
	    trans_type = NO_SEND;
	    add_nbanq_db = FALSE;
        } else {
            trans_type = ALL_WAN_PEERS;
	    ssp_frame_type = SSP_OP_netbios_anq;
	    add_nbanq_db = TRUE;
        }
	break;
      case NETBIOS_ADD_GROUP_QUERY:
      case NETBIOS_DATAGRAM_BROADCAST:
      case NETBIOS_TERMINATE_TRACE_REM:
      case NETBIOS_TERMINATE_TRACE_REM_LOC:
	ssp_frame_type = SSP_OP_dataframe;
	trans_type = ALL_WAN_PEERS;
        break;
      default:
	if (csm->nb_hdr->command == NETBIOS_ADD_NAME_RESPONSE) {
	    ssp_frame_type = SSP_OP_netbios_anr;
        } else {
	    ssp_frame_type = SSP_OP_dataframe;
        }
        if (local_cache_ptr != NULL) {
	    if ((local_cache_ptr->status != NOT_FOUND) &&
                              (local_cache_ptr->status != SEARCHING)) {
                /* reachable locally */
	        free_clsi_msg(clsi_msg);
	        return;
            }
        }
	if (cache_ptr == NULL) {
	    trans_type = ALL_WAN_PEERS;
        } else {
            switch (cache_ptr->status) {
              case NOT_FOUND:
                trans_type = NO_SEND;
                break;
              case SEARCHING:
                trans_type = ALL_WAN_PEERS;
                break;
              case FOUND:
              case VERIFY:
                trans_type = NO_SEND;
                no_in_list = 0;
                for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                    if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id, lan_port_id)) {
		        trans_type = SPEC_WAN_PEER;
		        no_in_list = 1;
                        peer_list[0] = cache_ptr->r_entry[ii].prp_id.peer_id;
                        break;
                    } 
                } 
                if (trans_type == NO_SEND) {
                    free_clsi_msg(clsi_msg);
                    return;
                }
                break;
              case UNCONFIRMED:
                no_in_list = 0;
                for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
                    if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id, lan_port_id)) {
	                peer_list[no_in_list] = cache_ptr->r_entry[ii].prp_id.peer_id;
                        ++no_in_list;
                    }
                }
                if (no_in_list) {
                    trans_type = SPEC_WAN_PEER;
                } else {
                    trans_type = NO_SEND;
                }
                break;
              default:
                NETB_CSME_DEBUG("\nCSM: Cache status error %s... xcasting frame - nb_data_frame_proc", 
                                csm_stat(cache_ptr->status));
                trans_type = ALL_WAN_PEERS;
                break;
            } 
        }
        break;
    }

    rc = 0;
    if (trans_type != NO_SEND) {
        rc = send_ui_frame_to_peers(peer_list, no_in_list, (char *) csm->nb_hdr,
                                    data_len, ssp_frame_type, lan_port_id, 0,
                                    RC_LF_MAX, SSP_LF_IGNORE_BIT, 0, 0, csm);
    } 
    if ((add_nbanq_db) && (rc)) {
        ws_nbanq.direction = LOCAL;
        ws_nbanq.prp_id.port_id = lan_port_id;
        memset(&ws_nbanq.del_tstamp, 0, sizeof(ws_nbanq.del_tstamp));
        memset(&ws_nbanq.retry_tstamp, 0, sizeof(ws_nbanq.retry_tstamp));
        rhandle = add_record(NBANQ_FILE, csm->nb_hdr->destname, (void *) &ws_nbanq);
	if (rhandle == NULL) {
	    NETB_CSME_DEBUG("\nCSM: Error adding to nbanq database");
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
            mgd_timer_stop(&tmp_ptr->retry_tstamp);
        }
    }
    free_clsi_msg(clsi_msg);
    return;

}



/*
 * CALL NAMES AND FIND NAMES --------------
 *
 * Currently we are assuming that there will only be one outstanding
 * name query. This needs to be handled better. This will be done in 
 * three parts.
 *
 * PART I:
 *    With the new enhancement towards handling netbios traffic better, 
 *    I am adding code that permits one find name and one call name
 *    type in name query. 
 *
 * FUTURE ENHANCEMENTS (PART II and PART III):
 * But even with this enhancement, DLSw does not look at the xmit and rsp 
 * correlators to sync the command and response sequences properly. This
 * should be done in the next phase. Due to the long time lag in some slower
 * speed WAN networks the delay introuduced may cause the end station to
 * continuously retry frames. This could be avoided with the third phase
 * that shall respond locally to name query find names....
 */
 

static void nb_nq_frame_proc (lportid_t lan_port_id, TCLSIMsg *clsi_msg,
                       csm_info_t *csm)
{
    int            ii, no_in_list, rc;
    int            reach_rc;
    int            data_len;
    uchar          ws_status;
    boolean        add_dlxnr_pend, add_cache, ring_list_match;

    e_xmission_type     trans_type;
    peerhandle_t        peer_list[MAX_REACH];
    lcache_t            *cache_ptr, *local_cache_ptr, ws_cache;
    dlxnr_pend_t        *dlxnr_ptr, tmp_dlxnr;
    record_handle       rhandle;
    u_portid_type_t     o_prp_id;
    char                nq_call_flag;

    add_dlxnr_pend = FALSE;
    add_cache = FALSE;
    no_in_list = 0;
    ws_status = 0;

    if (csm->nb_hdr->data2[1] != 0) {
        nq_call_flag = NQ_CALL_NAME;
    } else {
        nq_call_flag = NQ_FIND_NAME;
    }
    data_len = get_usrdata_len_from_clsi(clsi_msg);

    get_name_cache_entries(csm->nb_hdr->destname, &local_cache_ptr, NULL, 
					     &cache_ptr, NULL);
    if (local_cache_ptr != NULL) {
	if ((local_cache_ptr->status != NOT_FOUND) &&
                              (local_cache_ptr->status != SEARCHING)) {
            /* reachable locally */
	    free_clsi_msg(clsi_msg);
	    return;
        }
    }
    if (cache_ptr == NULL) {
        ws_status = NO_CACHE_INFO;
    } else {
        ws_status = cache_ptr->status;
    }
    NETB_CSMV_DEBUG("\nCSM: Netbios Name Query: ws_status = %s", 
                    csm_stat(ws_status));
    rhandle = seek_record(DLXNR_PEND, csm->nb_hdr->destname, 
                          (void **) &dlxnr_ptr);
    while (rhandle != NULL) {
        rc = bcmp(csm->nb_hdr->srcname, dlxnr_ptr->sname, NETBIOS_MAX_NAME_LEN);
        if ((rc == 0) && (dlxnr_ptr->nq_call_flag == nq_call_flag)) {
            /* match found */
            NETB_CSMV_DEBUG("\nCSM: Name Recognized pending - drop name query");
            free_clsi_msg(clsi_msg);
            return;
        }
        rhandle = read_next_record(DLXNR_PEND, rhandle, csm->nb_hdr->destname,
                                       (void **) &dlxnr_ptr);
    }

    switch (ws_status) {
      case NO_CACHE_INFO:
        trans_type = ALL_WAN_PEERS;
        add_dlxnr_pend = TRUE;
        add_cache = TRUE;
        break;
      case NOT_FOUND:
        trans_type = NO_SEND;
        add_dlxnr_pend = FALSE;    /* just drop the frame */
        break;
      case SEARCHING:
      case VERIFY:
        trans_type = NO_SEND;
        add_dlxnr_pend = TRUE;
        break;
      case FOUND:
        ring_list_match = FALSE;
        for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id,
                                      lan_port_id)) {
                ring_list_match = TRUE;
                break;
            }
        }

        if (ring_list_match == FALSE) {
            /*
             * Allow port list to do it's job and drop packet.
             * When current cache times out, we'll explore to all peers.
             */
            free_clsi_msg(clsi_msg);
            return;
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
             * since it is netbios send verify to only the peers that
             * pass the ring list filter
             * because of the limitaion in the way the cache is organized,
             * all the peers in the list will not be verified correctly
             * This in-efficient verification process
             * will be fixed when the cache is re-orged.
             */
            NETB_CSMV_DEBUG("\nCSM: Time to verify netbios cache entries");
            no_in_list = cache_ptr->no_in_list;
            for (ii = 0; ii < no_in_list; ii++) {
                peer_list[ii]=cache_ptr->r_entry[ii].prp_id.peer_id;
		if (cache_ptr->cache_type == DYNAMIC) {
		    cache_ptr->r_entry[ii].prp_id.peer_id = 0;
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
            trans_type = SPEC_WAN_PEER;
	    cache_ptr->status = VERIFY;
            add_dlxnr_pend = TRUE;
            GET_TIMESTAMP(cache_ptr->verify_tstamp);
            mgd_timer_start(&cache_ptr->explore_tstamp, csm_sna_expl_to);
        } else {
            o_prp_id.port_id = lan_port_id;
            reach_rc = get_reach_entry_mac(cache_ptr, 0, &o_prp_id, FALSE);

            if (reach_rc >= 0) {
	        trans_type = SPEC_WAN_PEER;
	        no_in_list = 1;
                peer_list[0] = cache_ptr->r_entry[reach_rc].prp_id.peer_id;
                add_dlxnr_pend = TRUE;
            } else if (reach_rc == CSM_NO_MTU_SIZE) {
                no_in_list = 0;
                for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
		    if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id, lan_port_id)) {
                        peer_list[no_in_list] = cache_ptr->r_entry[ii].prp_id.peer_id;
                        ++no_in_list;
		    }
                }
		if (no_in_list)
		    trans_type = SPEC_WAN_PEER;
		else
                    trans_type = ALL_WAN_PEERS;
		add_dlxnr_pend = TRUE;
	    } else {
                /* ring list match already checked */
	        NETB_CSME_DEBUG("\nCSM: Error in reach entry ... reach_rc %s",
                                csm_error(reach_rc));
                add_dlxnr_pend = FALSE;
                trans_type = NO_SEND;
            }
        }
        break;
      case UNCONFIRMED:
        no_in_list = 0;
	for (ii = 0; ii < cache_ptr->no_in_list; ii++) {
            if (peer_pass_port_filter(cache_ptr->r_entry[ii].prp_id.peer_id, 
                                      lan_port_id)) {
                peer_list[no_in_list] = cache_ptr->r_entry[ii].prp_id.peer_id;
                ++no_in_list;
            }
        }
        if (no_in_list) {
	    trans_type = SPEC_WAN_PEER;
            add_dlxnr_pend = TRUE;
        } else {
            trans_type = NO_SEND;
            add_dlxnr_pend = FALSE;
        }
        break;
      default:
        NETB_CSME_DEBUG("\nCSM: Cache status error %s ... xcasting frame", 
                         csm_stat(ws_status));
        trans_type = ALL_WAN_PEERS;
        add_dlxnr_pend = TRUE;
        break;
    } 

    if (trans_type != NO_SEND) {
        rc = send_ui_frame_to_peers(peer_list, no_in_list, (char *) csm->nb_hdr,
                                     data_len, SSP_OP_netbios_nq, lan_port_id,
                                     SSP_FLAGS_EX, csm->mtu_bytes,
                                     SSP_LF_IGNORE_BIT, 0, 0, csm);
    } else {
        rc = 1;    /* to help with add_dlxnr_pend.... */
    }

    if ((add_cache) && (rc)) {
        ws_cache.cache_type = DYNAMIC;
        ws_cache.status = SEARCHING;
	ws_cache.reach_direction = REMOTE;
	ws_cache.no_in_list = 0;
        for (ii = 0; ii < MAX_REACH; ii++) {
            ws_cache.r_entry[ii].prp_id.peer_id = 0;
            ws_cache.r_entry[ii].flags = 0;
            ws_cache.r_entry[ii].mtu_size = 0;
        }
        memset(&ws_cache.del_tstamp, 0, sizeof(ws_cache.del_tstamp));
        memset(&ws_cache.explore_tstamp, 0, sizeof(ws_cache.explore_tstamp));
        rhandle = add_record(RCACHE_NAME, csm->nb_hdr->destname,
                            (void *) &ws_cache);
        if (rhandle == NULL) {
            NETB_CSME_DEBUG("\nCSM: nb_nq_frame_proc - RCACHE_NAME db add error");
        } else {
            lcache_t     *tmp_ptr;
            tmp_ptr = (lcache_t *) rhandle_to_data(rhandle);
            /* initialise timers */
            mgd_timer_init_leaf(&tmp_ptr->explore_tstamp,
                                &csm_cache_expl_timer,
                                CSM_RNAME_CACHE, rhandle, FALSE);
            mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                &csm_cache_del_timer,
                                CSM_RNAME_CACHE, rhandle, FALSE);
            mgd_timer_stop(&tmp_ptr->explore_tstamp);
            mgd_timer_start(&tmp_ptr->del_tstamp, csm_nb_expl_to);
            GET_TIMESTAMP(tmp_ptr->verify_tstamp);
        }
    }
    if ((add_dlxnr_pend) && (rc)) {
        bcopy(csm->nb_hdr->srcname, tmp_dlxnr.sname, NETBIOS_MAX_NAME_LEN);
        tmp_dlxnr.port_id = lan_port_id;
        bcopy(csm->rif, tmp_dlxnr.input_rif, SRB_MAXRIF);
        tmp_dlxnr.retry_info.trans_type = trans_type;
        tmp_dlxnr.retry_info.no_in_list = no_in_list;
        for (ii = 0; ii < no_in_list; ii++) {
            tmp_dlxnr.retry_info.prp_id[ii].peer_id = peer_list[ii];
        }
        ieee_copy(csm->daddr, tmp_dlxnr.retry_info.daddr);
        ieee_copy(csm->saddr, tmp_dlxnr.retry_info.saddr);
        tmp_dlxnr.retry_info.dsap = csm->dsap;
        tmp_dlxnr.retry_info.ssap = csm->ssap;
        tmp_dlxnr.retry_info.ui_data_len = data_len;
        tmp_dlxnr.mtu_size = csm->mtu_bytes;
        tmp_dlxnr.mtu_flag = SSP_LF_IGNORE_BIT;
        tmp_dlxnr.nq_call_flag = nq_call_flag;
        bcopy(csm->nb_hdr, tmp_dlxnr.retry_info.ui_data, data_len);
        memset(&tmp_dlxnr.del_tstamp, 0, sizeof(tmp_dlxnr.del_tstamp));
        rhandle = add_record(DLXNR_PEND, csm->nb_hdr->destname, (void *) &tmp_dlxnr);
        if (rhandle == NULL) {
            NETB_CSME_DEBUG("\nCSM: nb_nq_frame_proc - DLXNR_PEND db add error");
        } else {
            dlxnr_pend_t        *tmp_ptr;
            tmp_ptr = (dlxnr_pend_t *) rhandle_to_data(rhandle);
            mgd_timer_init_leaf(&tmp_ptr->del_tstamp,
                                &csm_dlxnr_del_timer,
                                0, rhandle, FALSE);
            mgd_timer_start(&tmp_ptr->del_tstamp, csm_nb_expl_to);
        }
    }

    free_clsi_msg(clsi_msg);
    return;
}




static void nb_nr_frame_proc (lportid_t lan_port_id, TCLSIMsg *clsi_msg,
                       csm_info_t *csm)
{

    int      rc, no_in_list, data_len;
    uchar    csex_flags;
    long     ws_mtu_size;
    char     nq_call_flag;

    peerhandle_t        peer_list[MAX_REACH];
    record_handle       rhandle, best_rhandle = NULL;
    nr_pend_t           *nrpend_ptr, *best_nrpend_ptr = NULL;

    if (csm->nb_hdr->data2[1] != 0) {
        nq_call_flag = NQ_CALL_NAME;
    } else {
        nq_call_flag = NQ_FIND_NAME;
    }
    data_len = get_usrdata_len_from_clsi(clsi_msg);

    rhandle = seek_record(NR_PEND, csm->nb_hdr->srcname, (void **) &nrpend_ptr);
    while (rhandle != NULL) {
        if (peer_pass_port_filter(nrpend_ptr->peer_id, lan_port_id)) {
            rc = bcmp(csm->nb_hdr->destname, nrpend_ptr->sname, 
                      NETBIOS_MAX_NAME_LEN);
            if (rc == 0) {
                /*
                 * NetBIOS name matched. Now attempt to match the nq_call_flag.
                 * If we can match it with a nrpend record, pass the NR to
                 * the peer and delete the nrpend record. If the nq_call_flag
                 * doesn't match, pass the NR to the peer but do NOT delete the
                 * nrpend record. The nrpend record is used to drop duplicate
                 * find/call name queries.
                 */
                best_rhandle = rhandle;
                best_nrpend_ptr = nrpend_ptr;
                if (nrpend_ptr->nq_call_flag == nq_call_flag) {
                    break;
                }
            }
        }
        rhandle = read_next_record(NR_PEND, rhandle, csm->nb_hdr->srcname,
                                       (void **) &nrpend_ptr);
    }

    if (best_nrpend_ptr) {
        no_in_list = 1;
        peer_list[0] = best_nrpend_ptr->peer_id;
        csex_flags = best_nrpend_ptr->csex_flags;
        if ((best_nrpend_ptr->mtu_size <= csm->mtu_bytes) ||
            (best_nrpend_ptr->mtu_flag & SSP_LF_IGNORE_BIT)) {
            ws_mtu_size = min(csm->mtu_bytes, best_nrpend_ptr->mtu_size);
            send_ui_frame_to_peers(peer_list, no_in_list, (char *) csm->nb_hdr,
                                   data_len, SSP_OP_netbios_nr, lan_port_id,
                                   csex_flags, ws_mtu_size,
                                   best_nrpend_ptr->mtu_flag,
                                   best_nrpend_ptr->remote_dlc_port_id,
                                   best_nrpend_ptr->remote_transport_id, csm);

            /*
             * If the nq_call_flag matches, delete the record. Otherwise, keep
             * it around so it can be used to drop duplicate name queries.
             */
            if (best_nrpend_ptr->nq_call_flag == nq_call_flag) {
                mgd_timer_stop(&best_nrpend_ptr->retry_tstamp);
                mgd_timer_stop(&best_nrpend_ptr->del_tstamp);
                delete_record_get_next(NR_PEND, best_rhandle);
            }
        }
    }

    free_clsi_msg(clsi_msg);
    return;
}


/*
 * Function call to send ui data messages to peers
 */
int send_ui_frame_to_peers (peerhandle_t *peer_list, int no_in_list,
                            char *usr_data, int data_len,
                            uchar ssp_frame_type, lportid_t lan_port_id,
                            char csex_flags, long mtu_size, char mtu_flag,
                            ulong remote_dlc_port_id,
                            ulong remote_transport_id, csm_info_t *csm)
{
    int            rc, ii;
    int            ws_ssp_data_len, ws_ssp_dlc_hdr_len;
    long           ws_mtu_size;
    char           *dlx_usr_data, *ssp_rif;
    char           rc_code;
    paktype        *ssp_pak, *tmp_pak;

    ssp_control_frame_t *ssp_hdr;
    tring_hdr           *tr_hdr;
    sap_hdr             *llc_hdr;

    rc = 0;
    ws_ssp_dlc_hdr_len = TRING_ENCAPBYTES + SAP_HDRBYTES + SRB_MAXRIF;
    ws_ssp_data_len = data_len + ws_ssp_dlc_hdr_len;
    ssp_pak = get_ssp_packet(ssp_frame_type, ws_ssp_data_len, 0, 0, 0);
    if (ssp_pak == NULL) {
        CSME_DEBUG("\nCSM: Error getting pak - ssp send_ui_frame_to_peers");
    } else {
        ssp_hdr = (ssp_control_frame_t *) ssp_pak->datagramstart;
        if ((ssp_frame_type == SSP_OP_netbios_nr) ||
            (ssp_frame_type == SSP_OP_netbios_anr)) {
            ieee_copy(csm->saddr, ssp_hdr->dlc_id.target_mac_address);
            ieee_copy(csm->daddr, ssp_hdr->dlc_id.origin_mac_address);
            ssp_hdr->dlc_id.target_link_sap = csm->ssap;
            ssp_hdr->dlc_id.origin_link_sap = csm->dsap;
            ssp_hdr->frame_direction = SSP_DIRN_BACKWARD;
            ssp_hdr->origin_id.dlc_port_id = remote_dlc_port_id;
            ssp_hdr->origin_id.transport_id = remote_transport_id;
            ssp_hdr->target_id.dlc_port_id = (uint)lan_port_id->swidb;
            ssp_hdr->target_id.transport_id = DLSW_XPORT_ID_COOKIE;
        } else {
            ieee_copy(csm->daddr, ssp_hdr->dlc_id.target_mac_address);
            ieee_copy(csm->saddr, ssp_hdr->dlc_id.origin_mac_address);
            ssp_hdr->dlc_id.target_link_sap = csm->dsap;
            ssp_hdr->dlc_id.origin_link_sap = csm->ssap;
            ssp_hdr->frame_direction = SSP_DIRN_FORWARD;
            ssp_hdr->origin_id.dlc_port_id = (uint)lan_port_id->swidb;
            ssp_hdr->origin_id.transport_id = DLSW_XPORT_ID_COOKIE;
            ssp_hdr->target_id.dlc_port_id = remote_dlc_port_id;
            ssp_hdr->target_id.transport_id = remote_transport_id;
        }
        ssp_hdr->dlc_header_len = ws_ssp_dlc_hdr_len;
        ssp_hdr->csex_flags = csex_flags;
        /* DLSw standard expects tr and sap hdr here */
        tr_hdr = (tring_hdr *) ((char *) ssp_pak->datagramstart +
                                         sizeof(ssp_control_frame_t));
        ssp_rif = (char *) tr_hdr + TRING_ENCAPBYTES;
        llc_hdr = (sap_hdr *) ((char *) ssp_rif + SRB_MAXRIF);
        dlx_usr_data = (char *) llc_hdr + SAP_HDRBYTES;
        tr_hdr->ac = DLSW_AC_COOKIE;
        tr_hdr->fc = DLSW_FC_COOKIE;
        ieee_copy(csm->daddr, tr_hdr->daddr);
        ieee_copy(csm->saddr, tr_hdr->saddr);
         if (csm->rif) {
            /* whatever the rif size, leave 18bytes for the rif */
            bcopy(csm->rif, ssp_rif, SRB_MAXRIF);
            /* for interoperability with other DLSw vendors set RII */
            tr_hdr->saddr[0] |= TR_RII;
        } else {
            /*
             * eventhough there is no rif and the RII bit is not set,
             * leave space (the whole 18bytes) per protocol
             */
            bzero(ssp_rif, SRB_MAXRIF);
        }
        llc_hdr->dsap = csm->dsap;
        llc_hdr->ssap = csm->ssap;
        llc_hdr->control = csm->ctrl;
        bcopy(usr_data, dlx_usr_data, data_len);
    }

    if (no_in_list == 0) {
        if (ssp_pak) {
            ssp_hdr = (ssp_control_frame_t *) ssp_pak->datagramstart;
            ws_mtu_size = min(mtu_size, BOX_MTU_SIZE);
            /*
             * In NetBIOS datagram broadcasts always try to find the best 
             * LF possible! Cisco differs in it's use of ssp lf bits from 
             * 1795. The peer code will convert cisco ssp lf bits to be
             * 1795 comliant when frame is sent to all peer (ie. the
             * peer handle is zero).
             */
            rc_code = bytes_to_rc_code(ws_mtu_size);
            ssp_hdr->largest_frame_bits = rc_code | mtu_flag;
            rc = csm_peer_put((peerhandle_t) 0, ssp_pak, csm->daddr, csm->saddr,
                              csm->dsap, csm->ssap,
                              (PEER_SSP_TYPE | PEER_NETBIOS_UI),
                              lan_port_id);
        }
    } else {
        for (ii = 0; ii < no_in_list; ii++) {
            if (ssp_pak) {
                ssp_hdr = (ssp_control_frame_t *) ssp_pak->datagramstart;
                ws_mtu_size = min(mtu_size, (peerid_to_mtu(peer_list[ii])));
                ws_mtu_size = min(ws_mtu_size, BOX_MTU_SIZE);
                if (peer_get_vendor_type(peer_list[ii]) == PEER_NOT_CISCO) {
                    ssp_hdr->largest_frame_bits =
                             bytes_to_ssp_lf_bits(ws_mtu_size) | mtu_flag;
                } else {
                    ssp_hdr->largest_frame_bits = 
                             bytes_to_rc_code(ws_mtu_size) | mtu_flag;
                }
                /* send packet to peer manager */
                tmp_pak = pak_duplicate(ssp_pak);
                if (tmp_pak == NULL) {
                    break;
                }
                rc += csm_peer_put(peer_list[ii], tmp_pak,
                                   csm->daddr, csm->saddr, csm->dsap, csm->ssap,
                                   (PEER_SSP_TYPE | PEER_NETBIOS_UI),
                                   lan_port_id);
            }
        }
        if (ssp_pak) {
            datagram_done(ssp_pak);
        }
    }

    return rc;
}



/*
 * Function to put messages on the LAN
 *
 * Input:
 *    lancast_type- ALL_LAN_PORTS -- send to all ports
 *                  SPEC_LAN_PORT -- send to lan port shown in port_id
 *                  CSM_LOCAL_SWITCH -- send to local switch ports
 *                                      other than the one in portid
 *    port_id     - The portid to send the frame to 
 *                  0 to send to all lanports
 *    msg_type    - The message type as in llc2/llc2.h
 *    rsp_bit     - Command or Response 
 *                  (command = 0 (CMDBIT) , response = 1 (RSPBIT))
 *    poll_final  - Poll or Final 
 *                  (0 to not set it, 0x10 (P_BIT) to set it)
 *    dst_addr    - Pointer to the destination mac address
 *    src_addr    - Pointer to the source mac address
 *    dst_sap     - Destination Sap
 *    src_sap     - Source Sap
 *    xcast_type  - Broadcast Type
 *                  0 - Non Broadcast (rif is included)
 *                  RC_BROADBIT - All Routes Broadcast
 *                  RC_BROADBIT | RC_SPANBIT - Single Routes Broadcast
 *    rif         - Pointer to the rif (if any)
 *                  Set to NULL if broadcast bit is set
 *                  Required if non broadcast is set
 *                  The length and direction bits should be set 
 *                  correctly in the control portion of the rif
 *    mtu_size    - Mtu Size to be set in the frames in bytes
 *    u_data      - Pointer to the user data
 *    u_data_len  - length of the user data
 *    input_ph    - The peer handle of the peer this frame was received from
 *
 * Return Codes:
 *    zero - Unsuccesssful
 *    non-zero - The number of ports the frame was written on
 *    
 */
int csm_lan_put (int lancast_type, lportid_t port_id, uchar msg_type, 
		 uchar rsp_bit, uchar poll_final, char *dst_addr, 
		 char *src_addr, uchar dst_sap, uchar src_sap, uchar xcast_type,
		 char *rif, long mtu_size, char *u_data, int u_data_len,
                 peerhandle_t input_ph)
{
    int         rc;
    long        ws_mtu_size;
    uchar       rc_code;
    dlsw_ring_t *dlsw_ring_tmp;

    rc = 0;
    if (mtu_size == 0) {
        mtu_size = BOX_MTU_SIZE;
    } else {
        mtu_size = min(mtu_size, BOX_MTU_SIZE);
    }
    ws_mtu_size = mtu_size;
    if (lancast_type == CSM_LOCAL_SWITCH) {
        /*
         * if the input port was not local switch enabled
         * send only to ports with local switch enabled
         * else send to all ports (other than the one that
         * originated the frame
         */
        for (dlsw_ring_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
                    dlsw_ring_tmp;
                    dlsw_ring_tmp = dlsw_ring_tmp->next)  {
            if ((!dlsw_ring_tmp->enabled) || (dlsw_ring_tmp == port_id)) {
                continue;
            }
            if (dlsw_ring_tmp->port_type == CLS_FRAME_RELAY) {
                continue;
            }
            if (!dlsw_can_switch(port_id->swidb,
                                 dlsw_ring_tmp->swidb, LOCAL_SWITCH)) {
                continue;
            }
            if (!dlsw_can_switch(dlsw_ring_tmp->swidb, 
                                 dlsw_ring_tmp->swidb, NETBIOS_SUPPORT)) {
                if ((msg_type == UI) || (dst_sap == SAP_NETBIOS) ||
                        (dst_sap == SAP_IBMNM)) {
                    continue;
                }
            }
            ws_mtu_size = min(mtu_size, (portid_to_mtu(dlsw_ring_tmp)));
            rc_code = (uchar) bytes_to_rc_code(ws_mtu_size);
            if (rc_code == 0xff) {
                rc_code = RC_LF17800;
            }
            rc += csm_send_to_disp(dlsw_ring_tmp, msg_type, rsp_bit,
                                    poll_final, dst_addr, src_addr,
                                    dst_sap, src_sap, xcast_type,
                                    rif, rc_code, u_data, u_data_len);
        }
    } else if (lancast_type != ALL_LAN_PORTS) {
        /*
         * If input_ph is not NULL, check for ring list match.
         * input_ph would be NULL if we are sending a TEST R
         * to a received TEST C due to a fresh remote cache entry.
         */
        if (input_ph) {
            if (!(peer_pass_port_filter(input_ph, port_id)))
                return CSM_NO_RING_MATCH;
        }
        ws_mtu_size = min(mtu_size, (portid_to_mtu(port_id)));
        rc_code = (uchar) bytes_to_rc_code(ws_mtu_size);
        if (rc_code == 0xff) {
            rc_code = RC_LF17800;
        }
      if (port_id->enabled == TRUE) {
        if (!dlsw_can_switch(port_id->swidb, port_id->swidb, 
                             NETBIOS_SUPPORT)) {
	  if ( (msg_type == UI) || (dst_sap == SAP_NETBIOS) || 
	      (dst_sap == SAP_IBMNM))
	    /*
	     * jic..should have been filtered by the source driver.
	     * If a brian dead non-cisco router sends us these frames..
	     */
	    return rc;
	}
        rc = csm_send_to_disp (port_id, msg_type, rsp_bit, poll_final, 
		               dst_addr, src_addr, dst_sap, src_sap, 
		               xcast_type, rif, rc_code, u_data, u_data_len);
      }
    } else {
      for (dlsw_ring_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
	   dlsw_ring_tmp;
	   dlsw_ring_tmp = dlsw_ring_tmp->next)  {
        /* check for ring list match */
        if (input_ph) {
            if (!(peer_pass_port_filter(input_ph, dlsw_ring_tmp)))
                continue;
        }
        if (!dlsw_can_switch(dlsw_ring_tmp->swidb, dlsw_ring_tmp->swidb, 
                             REMOTE_SWITCH)) {
            continue;
        }
	if (dlsw_ring_tmp->enabled == TRUE) {
          if (dlsw_ring_tmp->port_type == CLS_FRAME_RELAY)
              continue;
          if (!dlsw_can_switch(dlsw_ring_tmp->swidb, dlsw_ring_tmp->swidb, 
                               NETBIOS_SUPPORT)) {
	    if ( (msg_type == UI) || (dst_sap == SAP_NETBIOS) ||
		 (dst_sap == SAP_IBMNM))
	      continue;
	  }
          ws_mtu_size = min(mtu_size, (portid_to_mtu(dlsw_ring_tmp)));
          rc_code = (uchar) bytes_to_rc_code(ws_mtu_size);
          if (rc_code == 0xff) {
              rc_code = RC_LF17800;
          }
	  rc += csm_send_to_disp(dlsw_ring_tmp, msg_type, rsp_bit,
				 poll_final, dst_addr, src_addr, 
				 dst_sap, src_sap, xcast_type, 
				 rif, rc_code, u_data, u_data_len);
	}
      }
    }
    return rc;
  }


/*
 * Actual routine that sends the message to disp to write to LAN
 */
int csm_send_to_disp (lportid_t port_id, uchar msg_type, uchar rsp_bit,
		      uchar poll_final, char *dst_addr, char *src_addr,
		      uchar dst_sap, uchar src_sap, uchar xcast_type,
		      char *rif, uchar rc_code, char *u_data, int u_data_len)

{
    union D_primitives    dp;
    char                  new_rif[SRB_MAXRIF];

    switch (msg_type) {
      case LLC_TEST:
        if (rsp_bit)
            dp.type = D_TESTSTN_RSP;
        else
            dp.type = D_TESTSTN_REQ;
        break;
      case XID:
        if (rsp_bit)
            dp.type = D_IDSTN_RSP;
        else
            dp.type = D_IDSTN_REQ;
        break;
      case UI:
        dp.type = D_UDATASTN_REQ;
        break;
      default:
        CSME_DEBUG("\nCSM: Invalid primitive type %s csm_send_to_disp", 
                   csm_prim(msg_type));
        return(0);
    }

    dp.disp_generic_stn_msg.p_sap_id  = port_id->p_sap_id;
    dp.disp_generic_stn_msg.frame_Data = u_data;
    dp.disp_generic_stn_msg.frame_Length = u_data_len;
    ieee_copy(dst_addr, dp.disp_generic_stn_msg.remoteAddr);
    dp.disp_generic_stn_msg.rSap = dst_sap;
    ieee_copy(src_addr, dp.disp_generic_stn_msg.localAddr);
    dp.disp_generic_stn_msg.lSap = src_sap;
    dp.disp_generic_stn_msg.options = 0;
    if ((rif != NULL) && (rif[0] != 0)) {
        if ((rif[1] & RC_LFMASK) != rc_code) {
            rif[1] = ((rif[1] & ~RC_LFMASK) | rc_code);
        }
        dp.disp_generic_stn_msg.rifLength = RIF_LENGTH(rif[0]);
        dp.disp_generic_stn_msg.rif = rif;
    } else {
        make_new_rif(port_id, xcast_type, rc_code, new_rif);
        dp.disp_generic_stn_msg.rifLength = RIF_LENGTH(new_rif[0]);
        dp.disp_generic_stn_msg.rif = new_rif;
    }

    if ((dst_sap == 0xF0) || (src_sap == 0xF0)) {
        NETB_CSMV_DEBUG("\nCSM: sending %s to %s", csm_prim(msg_type),
              port_id->swidb->hwptr->hw_namestring);
    } else {
        CSMV_DEBUG("\nCSM: sending %s to %s", csm_prim(msg_type),
              port_id->swidb->hwptr->hw_namestring);
    }
    fsm_to_disp(&dp);
    return(1);
}


/*
 * making rif out of the xcast type, rc_code size etc.
 */
static void make_new_rif(lportid_t port_id, uchar xcast_type, 
			    uchar rc_code, char *rif)
{
    ushort       ring_group;
    ushort       ring_number;
    ushort       bridge_number;
    long         port_mtu_size;

    srbroute_t    *bridged_rif;
    srbrd_t 	*rd;

    ring_group = port_id->ring_group;
    bridge_number = port_id->bridge_number;
    ring_number = port_id->ring_number;
    port_mtu_size = portid_to_mtu(port_id);

    bridged_rif = (srbroute_t *) rif;
    bridged_rif->rc_blth = xcast_type | 6;

    if (((rc_code & RC_LFMASK) == RC_LFINITIAL) ||
             ((rc_code_to_bytes(rc_code)) > port_mtu_size)) {
        rc_code = bytes_to_rc_code(port_mtu_size);
    }
    bridged_rif->rc_dlf = rc_code & RC_LFMASK;

    rd = (srbrd_t *) (&(bridged_rif->rd[0]));
    PUTSHORT(rd, (((ring_group << 4) & 0xfff0) | (bridge_number & 0x000f)));
    bridge_number = 0; /* clear the last bridge number */
    PUTSHORT(++rd, (((ring_number << 4) & 0xfff0)| (bridge_number & 0x000f)));
    return;
}


/*
 * Get the pointer to the destination address from clsi message
 * Change this later to use the access functions provided by clsi
 */
uchar *get_daddr_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    uchar        *daddr;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	daddr = (uchar *) clsi_msg + 
				((TestStnInd_t *)clsi_msg)->fLocalMacOffset;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	daddr = (uchar *) clsi_msg + 
				((IdStnInd_t *)clsi_msg)->fLocalMacOffset;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	daddr = (uchar *) clsi_msg + 
				((UDataStnInd_t *)clsi_msg)->fLocalMacOffset;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	daddr = (uchar *) clsi_msg + 
				((ConnectStnInd_t *)clsi_msg)->fLocalMacOffset;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_daddr_from_clsi", 
                  csm_prim(msg_type));
	daddr = NULL;
	break;
    }

    return(daddr);

}



/*
 * Get the pointer to the source address from clsi message
 * Change this later to use the access functions provided by clsi
 */
uchar *get_saddr_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    uchar        *saddr;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	saddr = (uchar *) clsi_msg + 
			      ((TestStnInd_t *)clsi_msg)->fRemoteMacOffset;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	saddr = (uchar *) clsi_msg + 
			      ((IdStnInd_t *)clsi_msg)->fRemoteMacOffset;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	saddr = (uchar *) clsi_msg + 
			      ((UDataStnInd_t *)clsi_msg)->fRemoteMacOffset;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	saddr = (uchar *) clsi_msg + 
	       	              ((ConnectStnInd_t *)clsi_msg)->fRemoteMacOffset;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_saddr_from_clsi", 
                   csm_prim(msg_type));
	saddr = NULL;
	break;
    }

    return(saddr);

}





/*
 * Get the pointer to the rif from clsi message
 * Change this later to use the access functions provided by clsi
 */
int get_rif_from_clsi (TCLSIMsg *clsi_msg, uchar *new_rif)
{

    ushort       msg_type;
    ushort       ring_group;
    ushort       ring_number;
    ushort       bridge_number;
    srbroute_t   *bridged_rif;
    uchar	 *rif;
    srbrd_t 	*rd;
    uchar	ii;
    uchar       rif_len;
    dlsw_ring_t *dlsw_ring_tmp;
    long        port_mtu_size;
    uchar       rc_code;

    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	rif_len = ((TestStnInd_t *)clsi_msg)->fRoutingLength;
	if (rif_len != 0 ) 
	    rif = (uchar *) clsi_msg + 
		((TestStnInd_t *)clsi_msg)->fRoutingOffset;
	else
	    rif = NULL;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	rif_len = ((IdStnInd_t *)clsi_msg)->fRoutingLength;
	if (rif_len != 0 ) 
	    rif = (uchar *) clsi_msg + 
		((IdStnInd_t *)clsi_msg)->fRoutingOffset;
	else
	    rif = NULL;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	rif_len = ((UDataStnInd_t *)clsi_msg)->fRoutingLength;
	if (rif_len != 0 ) 
	    rif = (uchar *) clsi_msg + 
		((UDataStnInd_t *)clsi_msg)->fRoutingOffset;
	else
	    rif = NULL;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	rif_len = ((ConnectStnInd_t *)clsi_msg)->fRoutingLength;
	if (rif_len != 0 ) 
	    rif = (uchar *) clsi_msg + 
		((ConnectStnInd_t *)clsi_msg)->fRoutingOffset;
	else
	    rif = NULL;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_rif_from_clsi", 
                   csm_prim(msg_type));
	rif_len = 0;
	rif = NULL;
	break;
    }

    /*
     * The last hop bridge number does not get initialized to 0,
     * when make_forward rif is called.
     * So set the new rif to zeros, here.
     */
    memset(new_rif, 0, SRB_MAXRIF);
    if ( (rif_len == 0 ) || (rif == NULL)) {
	return 0;
    }
    else
	bcopy((uchar *)rif, (uchar *)new_rif, SRB_MAXRIF);

    bridged_rif = (srbroute_t *)new_rif;

    if (RC_BROADBIT & rif[0]) {
        /* If only it is broad cast */
        dlsw_ring_tmp =    usapid2dgra(clsi_msg->fIdentifier);

        ring_group = dlsw_ring_tmp->ring_group;
        bridge_number = dlsw_ring_tmp->bridge_number;
        ring_number = dlsw_ring_tmp->ring_number;
        port_mtu_size = portid_to_mtu(dlsw_ring_tmp);

        ii = ((RIF_LENGTH(rif[0])-2)/2);

        if (RIF_LENGTH(rif[0]) == 2 ) {
	    /* virgin rif - add ring-bridge-ring-bridge(0) 4 bytes */
            rd = (srbrd_t *) (&(bridged_rif->rd[ii]));
            PUTSHORT(rd, (((ring_number << 4) & 0xfff0) | (bridge_number & 0x000f)));
            PUTSHORT(++rd, ((ring_group << 4) & 0xfff0));
            bridged_rif->rc_blth = ((bridged_rif->rc_blth & RC_RTMASK) | 6);
        } else if (RIF_LENGTH(rif[0]) > 16) {
            /* we need atleast 2 bytes to add the vring hop ... */
            CSM_DEBUG("CSM: Rif received is already %d bytes long",
                       RIF_LENGTH(rif[0]));
            return -1;
        } else {
	    /* rif already exists - add bridge-ring 2 bytes */
            ii = RIF_LENGTH(rif[0]) - 2;
            rd = (srbrd_t *) &new_rif[ii];
            PUTSHORT(rd, ((GETSHORT(rd) & 0xfff0) | (bridge_number & 0x000f)));
            PUTSHORT(++rd, ((ring_group << 4) & 0xfff0));
            bridged_rif->rc_blth =  ((bridged_rif->rc_blth & RC_RTMASK) | 
  				 ((RIF_LENGTH(rif[0]) + 2) & RC_LTHMASK) );
        }
        /* now, fix the lf size */
        if (((new_rif[1] & RC_LFMASK) == RC_LFINITIAL) || 
               ((rc_code_to_bytes(new_rif[1] & RC_LFMASK)) > port_mtu_size)) { 
            rc_code = bytes_to_rc_code(port_mtu_size);
            new_rif[1] = (new_rif[1] & ~RC_LFMASK) | rc_code;
        }
    }

    return 0;
}




/*
 * Get the destination sap from clsi message
 * Change this later to use the access functions provided by clsi
 */
uchar get_dsap_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    uchar        dsap;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	dsap = ((TestStnInd_t *)clsi_msg)->fLSap;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	dsap = ((IdStnInd_t *)clsi_msg)->fLSap;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	dsap = ((UDataStnInd_t *)clsi_msg)->fLSap;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	dsap = ((ConnectStnInd_t *)clsi_msg)->fLSap;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_dsap_from_clsi", 
                  csm_prim(msg_type));
	dsap = 0;
	break;
    }

    return(dsap);

}



/*
 * Get the source sap from clsi message
 * Change this later to use the access functions provided by clsi
 */
uchar get_ssap_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    uchar        ssap;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	ssap = ((TestStnInd_t *)clsi_msg)->fRSap;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	ssap = ((IdStnInd_t *)clsi_msg)->fRSap;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	ssap = ((UDataStnInd_t *)clsi_msg)->fRSap;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	ssap = ((ConnectStnInd_t *)clsi_msg)->fRSap;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_ssap_from_clsi", 
                   csm_prim(msg_type));
	ssap = 0;
	break;
    }

    return(ssap);

}




/*
 * Get the poll final flag from clsi message
 * return 0 if the Poll is off, P_BIT if the poll is on
 * Change this later to the access functions provided by clsi
 */
uchar get_pfflag_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    uchar        pf;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	pf = ((TestStnInd_t *)clsi_msg)->fTESTPollFinal;
	pf = (pf == CLS_TEST_POLL_ON) ? P_BIT : 0;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	pf = ((IdStnInd_t *)clsi_msg)->fXIDPollFinal;
	pf = (pf == CLS_XID_POLL_ON) ? P_BIT : 0;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	pf = 0;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	pf = ((ConnectStnInd_t *)clsi_msg)->fCONNECTPollFinal;
	pf = (pf == CLS_CONNECT_POLL_ON) ? P_BIT : 0;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_pfflag_from_clsi", 
                    csm_prim(msg_type));
	pf = 0;
	break;
    }

    return(pf);

}





/*
 * Get the the control field from clsi message
 * Change this later to the access functions provided by clsi
 */
uchar get_ctrl_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    uchar        ctrl;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	ctrl = LLC_TEST;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	ctrl = XID;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	ctrl = UI;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	ctrl = SABME;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_ctrl_from_clsi", 
                   csm_prim(msg_type));
	ctrl = 0;
	break;
    }

    return(ctrl);

}




/*
 * Get the the command response field from clsi message
 * Change this later to the access functions provided by clsi
 */
uchar get_cmdrsp_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    uchar        cmdrsp;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	cmdrsp = ((TestStnInd_t *)clsi_msg)->fTESTCmdRsp;
	cmdrsp = (cmdrsp == CLS_TEST_RSP) ? RSPBIT : 0;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	cmdrsp = ((IdStnInd_t *)clsi_msg)->fXIDCmdRsp;
	cmdrsp = (cmdrsp == CLS_XID_RSP) ? RSPBIT : 0;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	cmdrsp = 0;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	cmdrsp = 0;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_cmdrsp_from_clsi", 
                   csm_prim(msg_type));
	cmdrsp = 0;
	break;
    }

    return(cmdrsp);

}




/*
 * Get the pointer to the user data from clsi message
 * Change this later to use the access functions provided by clsi
 */
uchar *get_usrdata_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    uchar        *usr_data;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	usr_data = (uchar *) clsi_msg + 
				((TestStnInd_t *)clsi_msg)->fTESTOffset;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	usr_data = (uchar *) clsi_msg + 
				((IdStnInd_t *)clsi_msg)->fXIDOffset;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	usr_data = (uchar *) clsi_msg + 
				((UDataStnInd_t *)clsi_msg)->fUDataOffset;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	usr_data = (uchar *) clsi_msg + 
				((ConnectStnInd_t *)clsi_msg)->fCONNECTOffset;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_usrdata_from_clsi", 
                   csm_prim(msg_type));
	usr_data = NULL;
	break;
    }

    return(usr_data);

}



/*
 * Get the user data length from clsi message
 * Change this later to use the access functions provided by clsi
 */
int get_usrdata_len_from_clsi (TCLSIMsg *clsi_msg)
{

    ushort       msg_type;
    int          len;


    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	len = ((TestStnInd_t *)clsi_msg)->fTESTLength;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	len = ((IdStnInd_t *)clsi_msg)->fXIDLength;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	len = ((UDataStnInd_t *)clsi_msg)->fUDataLength;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	len = ((ConnectStnInd_t *)clsi_msg)->fCONNECTLength;
	break;

      default:
	CSM_DEBUG("\nCSM: Invalid primitive type %s - get_usrdata_len_from_clsi", 
                   csm_prim(msg_type));
	len = 0;
	break;
    }

    return(len);

}




/*
 * get the mtu size, given the port id
 */
long portid_to_mtu (lportid_t port_id)
{

    if (port_id->mtu_size > 0) {
        return (port_id->mtu_size);
    } else {
        CSMV_DEBUG("\nCSM: mtu size not set portid %s. ret 17800", 
           port_id->swidb->hwptr->hw_namestring);
        return RC_LF17800_VAL;
    }

}




