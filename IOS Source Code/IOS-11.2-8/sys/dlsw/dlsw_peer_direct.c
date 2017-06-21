/* $Id: dlsw_peer_direct.c,v 3.7.20.10 1996/09/09 19:09:03 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_direct.c,v $
 *------------------------------------------------------------------
 * DLSw Peer Transport Routines for Direct connections.
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Create, destroy, send, and receive routines for DLSw peer
 * connections using a direct interface (such as a serial line). 
 *------------------------------------------------------------------
 * $Log: dlsw_peer_direct.c,v $
 * Revision 3.7.20.10  1996/09/09  19:09:03  akhanna
 * CSCdi65724:  Cannot stop keepalives on promiscous peer
 * Branch: California_branch
 *
 * Revision 3.7.20.9  1996/09/03  22:25:00  kpatel
 * CSCdi67883:  DLSw lite requires flow control to throttle back end
 *              station traffic
 * Branch: California_branch
 *
 * Revision 3.7.20.8  1996/08/28  12:41:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.20.7  1996/08/26  15:05:16  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.7.20.6  1996/07/18  20:27:23  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.7.20.5  1996/07/10  19:09:14  kpatel
 * CSCdi62360:  DLSw+ prom peer does not connect to direct frame-relay
 *              passthru peer
 * Branch: California_branch
 *
 * Revision 3.7.20.4  1996/07/09  18:08:43  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.7.20.3  1996/07/02  23:10:44  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.7.20.2  1996/05/17  10:46:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.6.6  1996/05/14  03:20:39  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.5  1996/05/10  15:00:55  ioakley
 * CSCdi57325:  Remove dlsw reliance on port_number/slot_number
 *              and identify all dlsw ports by associated swidb.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.4  1996/04/26  07:34:41  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.6.3  1996/04/24  06:30:23  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.7.6.2  1996/04/20  02:08:23  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.1  1996/04/03  14:01:53  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.20.1  1996/03/18  19:32:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.2  1996/03/13  01:20:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.1  1996/02/20  00:46:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/01  06:01:36  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/18  22:24:58  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/21  00:36:09  kpatel
 * CSCdi45362:  DLSw: Netbios call doesnt go thru for DLSw over
 *              FrameRelay
 *
 * Revision 3.4  1995/12/15  04:56:37  kpatel
 * CSCdi45411:  DLSw: peer connection doesnt go thru for Direct
 *              Bridge encap
 *
 * Revision 3.3  1995/12/09  16:49:11  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.2  1995/11/17  09:03:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/11/08  20:55:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.12  1995/10/21  02:05:38  kpatel
 * CSCdi42555:  DLSw+ LLC2 peer connection failure/shutdown does
 *              not clean up cep
 *
 * Revision 2.11  1995/10/21  01:24:11  kpatel
 * CSCdi40915:  ACTPU gets trashed in DLSW
 *
 * Revision 2.10  1995/10/09  20:18:46  rnaderi
 * CSCdi41641:  Add sub-interface option to the parser for DLSw+/FR
 * Branch: Ark_Ibm_branch
 *
 * Revision 2.9  1995/08/07  05:31:38  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.8  1995/07/31  21:32:28  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *   Fix crash with DLS+ direct over FrameRelay
 *
 * Revision 2.7  1995/07/31  18:58:59  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *
 * Revision 2.6  1995/06/28  18:51:34  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.5  1995/06/21  08:54:30  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/19  23:58:46  rnaderi
 * CSCdi36049:  mallocs need to check return code
 *
 * Revision 2.3  1995/06/18  21:21:31  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  19:27:32  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  20:27:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "../dlsw/msg_dlsw_core.c"	/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../if/network.h"
#include "../if/rif_util.h"
#include "../ui/debug.h"

#include "../ibm/libibm_externs.h"
#include "../tcp/tcpdriver.h"
#include "../srt/srb_core.h"
#include "../srt/rsrb.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_cap_exchg.h"
#include "../dlsw/dlsw_debug.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_dlu.h"

/* required for FR support */
#include "../wan/frame_relay.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc_public.h"
#include "../cls/dlc.h"
#include "../cls/clsi_queue.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_core_fsm.h" /* re-uses core error messages */

#include "../srt/srt_registry.h"
#include "../cls/dlc_registry.h"
#include "fastswitch_registry.h"


/* Frame Relay related functions */
static int dlsw_fr_peer_init(void);
static void dlsw_fr_disable_port(CLSPortID *);
static void dlsw_fr_enable_port(CLSPortID *);
static void dlsw_peer_send_actring(TCLSIMsg *);
static void dlsw_peer_send_open_stn (peerentry *);
static void dlsw_peer_send_deactring(CLSPortID *);
#ifdef NOBODY_CALLS_ME
static void dlsw_peer_send_disc_req(peerentry *);
#endif
static void dlsw_peer_send_conn_req(peerentry *);
static void dlsw_peer_send_close_stn(peerentry *);
static void dlsw_fr_signal_req(peerentry *);

static void peer_make_cls(union D_primitives *);
static void dlsw_peer_cls(struct strbuf *, struct strbuf *, int *);
static peerentry *clsi2peer (TCLSIMsg *clsi_msg);
static void dlsw_fr_connector(dlsw_ring_t *, peerentry *);
static int dlsw_dlu_peer_rput(TCLSIQueue*, TCLSIMsg*);

static TCLSIMsg* CreatePeerEnableReq(struct D_enable_req *);
static TCLSIMsg* CreatePeerDisableReq(struct D_disable_req *);
static TCLSIMsg* CreatePeerActSapReq(struct D_activatesap_req *);
static TCLSIMsg* CreatePeerDeActSapReq(struct D_deactivatesap_req *);
static TCLSIMsg* CreatePeerReqOpnStnReq(struct D_reqopenstn_req *);
static TCLSIMsg* CreatePeerConnectReq(struct D_connect_req *);
static TCLSIMsg* CreatePeerConnectRsp(struct D_connect_rsp *);
static TCLSIMsg* CreatePeerSignalStnReq(struct D_signalstn_req *);
static TCLSIMsg* CreatePeerDataReq(struct D_data_req *);
static TCLSIMsg* CreatePeerDiscReq(struct D_disconnect_req *);
static TCLSIMsg* CreatePeerDiscRsp(struct D_disconnect_rsp *);
static TCLSIMsg* CreatePeerCloseStnReq(struct D_closestn_req *);
static TCLSIMsg* CreatePeerActRingReq(struct D_activatering_req *);
static TCLSIMsg* CreatePeerDeActRingReq(struct D_deactivatering_req *);
static TCLSIMsg* CreatePeerFlowReq(struct D_flow_req *);

static TCLSIQInit* gDLSWPeerQInit;
static TCLSIQueue* gCLS2DLSWReadPeerQ;
static TCLSIQueue* gDLSW2CLSWritePeerQ;

/*
 * Forward declaration
 */
static void dlsw_if_input (paktype *pak);

static boolean dlsw_fr_running = FALSE;

/*
 * peer_direct_init()
 */

void peer_direct_init ()
{
int result;

 reg_add_raw_enqueue(LINK_DLSW, dlsw_if_input, "dlsw_if_input");
    
 result = dlsw_fr_peer_init();
 if (dlsw_peer_debug) 
   if(!result)
      buginf("\nDirect FR registration incomplete.");
}


/*
 * dlsw_if_connect()
 */

boolean dlsw_if_connect (peerentry *peer)
{
dlsw_ring_t *dlsw_fr_ptr;

 if((peer->outidb) && (hw_interface_up(peer->outidb))) {
 
     if((peer->xport_type == DLSW_FR) &&
        (!is_frame_relay(peer->outidb))) {
         PDEBUG("\nDLSw: encap frame-relay not conf on interface %s",
                peer->swidb->namestring);
         return(FALSE);
     }
 
     if (!peer->local_ack) {
         return(TRUE);
     } else {
         dlsw_fr_ptr = dlsw_get_ring_frpeer(peer->swidb);
 
         if(!dlsw_fr_ptr)
            return(FALSE);
 
         if(!dlsw_fr_ptr->enabled)
            return(FALSE);
 
         if(peer->fr_state == FR_CONN)
            return(TRUE);
 
         if(peer->fr_state == FR_DISCONN) {
            dlsw_fr_connector(dlsw_fr_ptr, peer);
            return(FALSE);
         }
 
         if(peer->fr_state == FR_OPEN) {
            if (peer->conn_retries % FR_REEXPLORE)
                dlsw_peer_send_conn_req(peer);
            else
                dlsw_peer_send_close_stn(peer);
         }
     }
 }

 return(FALSE); 

}


/*
 * dlsw_if_abort()
 */

void dlsw_if_abort (peerentry *peer)
{
char buff[DLSW_MAX_PEER_ID];

 /* Only FR/LLC2 encasulation is currently local-acked. */
 if (peer->local_ack) 
   dlsw_peer_send_close_stn(peer);

 PDEBUG("\nDLSw: DIRECT aborting connection for %s", peer_str(peer, buff));
 
}


/*
 * dlsw_if_sender()
 *
 * Just send a packet directly. 
 * datagramsize and * datagramstart are assumed to be set up correctly.
 *
 * Need to process each Direct frame synchronously so disable interrupts. 
 */

core2peer_ret_t dlsw_if_sender (peerentry  *peer, paktype *pak, boolean force,
                                peer_pak_cat category, peer_put_t *dontcare)
{
    char buff[DLSW_MAX_PEER_ID];
    ushort fr_dlci;
    union D_primitives dp;

    /* 
     * If interface not up, discard pak and return. Keepalive failure
     * will bring down the peer.
     */
    if (!hw_interface_up(peer->outidb)) {
       datagram_done(pak);
       peer->drops++;
       return(PEER_UNREACHABLE);
    }

    pak->linktype = LINK_DLSW;
    pak->acct_proto = ACCT_PROTO_SRB;
    pak->if_output = peer->outidb->firstsw;

    if (peer->xport_type == DLSW_FR && !peer->local_ack)
        fr_dlci = peer->fr_dlci;
    else if (peer->xport_type == DLSW_FR && peer->local_ack) {
        fr_dlci = peer->fr_dlci;

        dp.type = D_DATA_REQ;
        dp.disp_data_req.pCepId = peer->p_cep_id;
        dp.disp_data_req.theData = pak->datagramstart;
        dp.disp_data_req.dataLength = pak->datagramsize;
        dp.disp_data_req.pak = pak;

        peer_make_cls(&dp);

        peer->pkts_tx++;

        return(PEER_SUCCESS);
    } else
        fr_dlci = 1;

    if ((peer->outidb->vencap) &&
        ((*peer->outidb->vencap)(pak, ((peer->outidb->status & IDB_IEEEMASK) ?
                                       (ulong) peer->macaddr : (ulong) fr_dlci)))) {
        peer->pkts_tx++;
        datagram_out(pak);
        return(PEER_SUCCESS);
    } else {
        PDEBUG("\nDLSw Direct: vencap failed - dropping packet for %s",
               peer_str(peer, buff));
        datagram_done(pak);
        peer->drops++;
        return(PEER_UNREACHABLE);
    }
}


/*
 * dlsw_if_input()
 *
 * Process direct input from an interface.
 *
 * Interrupts are already disabled.
 * Called from raw_enqueue().
 */

static void dlsw_if_input (paktype *pak)
{
    if ((!lpeer) || (dlsw_disable)) {
        datagram_done(pak);
        return;
    }
    /*
     * Core must process paks at process level.
     * Also, promiscuous peers need to be malloc'd at process level.
     */
    p_enqueue(&direct_inputQ, pak);

}


void receive_direct_input (paktype *pak)
{
    peerentry *peer;
    hwaddrtype src_addr;
    int media_lf;
    fr_idb_struct_type *fr_idb;

    if(pak->if_input == NULL) {
       buginf("\nPass-thru, Lack was not configured properly, dropped.");
       datagram_done(pak);
       return;
    }

    pak->acct_proto = ACCT_PROTO_SRB;
    idb_increment_rx_proto_counts(pak, ACCT_PROTO_SRB);

    if (is_frame_relay(pak->if_input->hwptr)) {
       fr_idb = pak->if_input->hwptr->frame_relay_stuff;
       peer = (peerentry *) peer_swidb_dlci_to_handle(pak->if_input,
                                                    DLCI_TO_NUM(pak->fr_dlci));
    } else {
         peer = pak->if_input->hwptr->dlsw_ifpeer;
    }

    if (!peer) {
        /*
         * Only allow direct pods for serial interfaces for release 1.
         */
        if (!(pak->if_input->hwptr->status & IDB_SERIAL)) {
            if (dlsw_peer_debug) {
                buginf("\nDirect pod only permitted on Serial interfaces");
                buginf("\nDropping Direct encaps packet from %s", 
                       pak->if_input->hwptr->hw_namestring);
            }
            datagram_done(pak);
            return;
        }

        /*
         * A non-configured remote peer is connecting in. Accept the
         * connection if peer-on-demmand is enabled.
         */
        if (!lpeer->cap.pod_enabled) {
            if (dlsw_peer_debug)
                buginf("\nDLSw Direct: pod disabled - dropped packet from %s",
                       pak->if_input->hwptr->hw_namestring);
            datagram_done(pak);
            return;
        }

        media_lf = bytes_to_rc_code(pak->if_input->hwptr->maxmtu);
        media_lf = min(media_lf, peer_get_box_largest_frame());
        if (is_frame_relay(pak->if_input->hwptr)) {
          fr_idb = pak->if_input->hwptr->frame_relay_stuff;
            peer = peer_create_remote(DLCI_TO_NUM(pak->fr_dlci), 0, PROM_PEER,
                            	NULL, DLSW_FR, FALSE, FALSE,
                            	lpeer->prompeer_defaults.cost,
                               	lpeer->prompeer_defaults.largest_frame,
                            	lpeer->prompeer_defaults.keepalive_int,
                            	lpeer->prompeer_defaults.dlsw_lsap_out,
                            	lpeer->prompeer_defaults.netbios_h_oacf,
                            	lpeer->prompeer_defaults.netbios_b_oacf,
                            	DLSW_MAX_TCP_QLEN, pak->if_input->hwptr,
                            	NULL, NULL, 0, 0, 0, 0,
                               	lpeer->prompeer_defaults.dlsw_dmac_outlist,
                            	lpeer->prompeer_defaults.dlsw_dmac_out, pak->if_input,
                                NORMAL_MODE);
           }
           else {
             peer = peer_create_remote(0, 0, PROM_PEER, NULL, DLSW_IF, FALSE,
                            	FALSE, lpeer->prompeer_defaults.cost,
                            	lpeer->prompeer_defaults.largest_frame,
                            	lpeer->prompeer_defaults.keepalive_int,
                            	lpeer->prompeer_defaults.dlsw_lsap_out,
                            	lpeer->prompeer_defaults.netbios_h_oacf,
                               	lpeer->prompeer_defaults.netbios_b_oacf,
                            	DLSW_MAX_TCP_QLEN, pak->if_input->hwptr,
                            	NULL, NULL, 0, 0, 0, 0,
                            	lpeer->prompeer_defaults.dlsw_dmac_outlist,
                            	lpeer->prompeer_defaults.dlsw_dmac_out, NULL,
                                NORMAL_MODE);
           }

        if (!peer) {
            if (dlsw_peer_debug)
                buginf("\nDLSw: Direct create p-o-d failure for %s", 
                       pak->if_input->hwptr->hw_namestring);
            datagram_done(pak);
            return;
        }

    } else if (peer->xport_type == DLSW_IF_LAN) {
        /*
         * If LAN transport is used, find the proper peer.
         */
        (* pak->if_input->hwptr->extract_snpa)(pak, &src_addr, SOURCE);
        for ( ; peer; peer = peer->idbnext) {
            if (ieee_equal(peer->macaddr, src_addr.addr))
                break;
        }
    }

    /* Count it for LNM */
    srbV.srb_sb_incr_rx_counters(pak->if_input, pak->datagramsize);

    peer->pkts_rx++;
    pak->datagramstart = pak->network_start;
    peer_input(peer, pak, FALSE);
    return;
}


static void dlsw_fr_connector (dlsw_ring_t *dlsw_fr_ptr, peerentry *peer)
{
   peer->fr_state = FR_OPEN;
   peer->p_sap_id = dlsw_fr_ptr->p_sap_id;
   dlsw_peer_send_open_stn(peer);
}

static int dlsw_dlu_peer_rput (TCLSIQueue* dluReadQ, TCLSIMsg* clsi_msg)
{
 ushort      primitive;
 ulong       clsi_ret_code;
 DataInd_t   *data_ind;
 paktype     *pak;
 dlsw_ring_t *dlsw_fr_ptr;
 dlsw_sap_t  *dlsw_sap_tmp;
 union D_primitives dp;
 char        *str;
 peerentry   *peer;
ulong       level;
peer_msg_u peer_msg;

 str = string_getnext();

 primitive = CLSIMsgGetPrimitiveCode(clsi_msg);
 clsi_ret_code = CLSIMsgGetReturnCode(clsi_msg);

 switch(primitive)
  {
    case CLS_UDATA          | CLS_INDICATION:
    case CLS_UDATA          | CLS_CONFIRM:
    case CLS_ID_STN         | CLS_INDICATION:
    case CLS_ID             | CLS_CONFIRM:
    case CLS_ID             | CLS_INDICATION:
    case CLS_TEST           | CLS_INDICATION:
    case CLS_TEST_STN       | CLS_CONFIRM:
    case CLS_TEST_STN       | CLS_INDICATION:
    case CLS_UDATA_STN      | CLS_INDICATION:
    case CLS_UDATA_STN      | CLS_CONFIRM:
    case CLS_ACTIVATE_SAP   | CLS_CONFIRM:
    case CLS_DEACTIVATE_SAP | CLS_CONFIRM:
    default:
        CLSIMsgDestroy(clsi_msg);
        return(CLS_OK);
    break;
    case CLS_ENABLE          | CLS_CONFIRM:
        if(dlsw_peer_debug)
           buginf("\ndlsw_dlur_peer_rput, Got Enable Confirm");
        dlsw_peer_send_actring(clsi_msg);
        CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_DISABLE         | CLS_CONFIRM:
        if(dlsw_peer_debug)
           buginf("\ndlsw_dlu_peer_rput, Got Disable Confirm");
        CLSIMsgDestroy(clsi_msg);
    break;

    case CLS_ACTIVATE_RING | CLS_CONFIRM:
        if (dlsw_peer_debug)
           buginf("\n DLSw-FR : Got Activate ring confirm");
        dlsw_fr_ptr = usapid2dgra(clsi_msg->fIdentifier);
        if (dlsw_fr_ptr) {
           dlsw_fr_ptr->p_sap_id = ((ActRngCnf_t *)clsi_msg)->fCLSIHdr.fPSapId;
           dlsw_fr_ptr->enabled = TRUE; /* successfully enabled */
	   if (dlsw_peer_debug)
	            buginf("\n DLSw Got ActRingcnf back for %s - "
			   "port_id = %d, port_type = %d, psap_id = %x",
			   dlsw_fr_ptr->swidb->hwptr->hw_short_namestring,
			   dlsw_fr_ptr->swidb,
			   dlsw_fr_ptr->port_type,
			   dlsw_fr_ptr->p_sap_id);
           dlsw_sap_tmp = dlsw_get_sap(dlsw_fr_ptr, DLSW_FR_SAP);
           if (dlsw_sap_tmp)
                 dlsw_sap_tmp->enabled = TRUE;
        } else
           errmsg(DLSWC_ERR_BADCLSISAP,
                         CLSIMsgSprintfShort(clsi_msg, str),
                         clsi_msg->fIdentifier,
                         "dlsw_pre_proc_clsi - ActRngCnf");
        CLSIMsgDestroy(clsi_msg);

    break;
    case CLS_CONNECT_STN     | CLS_INDICATION:
        if(dlsw_peer_debug)
          buginf("\ndlsw_dlu_peer_rput, ConnectStnInd");
        CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_CONNECT        | CLS_INDICATION:
        if(dlsw_peer_debug)
          buginf("\ndlsw_dlu_peer_rput, ConnectInd");
        peer = clsi2peer(clsi_msg);
        if (peer == NULL)
          return(CLS_OK);
        dp.type = D_CONNECT_RSP;
        dp.disp_connect_req.pCepId = peer->p_cep_id;
        peer->fr_state = FR_CONN;
        peer_make_cls(&dp);
        CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_DEACTIVATE_RING | CLS_CONFIRM:
        /*
        * Do not dequeue the dlsw_ring_t ever.
        * it will get reused.
        */
        if (dlsw_peer_debug)
            buginf("\ndlsw_dlu_peer_rput, Got DeActivate ring Confirm");
        dlsw_fr_ptr = usapid2dgra(clsi_msg->fIdentifier);
        if (dlsw_fr_ptr == NULL) {
                  errmsg(DLSWC_ERR_BADCLSISAP,
                         CLSIMsgSprintfShort(clsi_msg, str),
                         clsi_msg->fIdentifier,
                         "dlsw_pre_proc_clsi- DeActRngCnf");
                  return(CLS_OK);
        }
        dlsw_fr_ptr->enabled = FALSE;
        dlsw_sap_tmp = dlsw_get_sap(dlsw_fr_ptr, DLSW_FR_SAP);
        if (dlsw_sap_tmp)
           dlsw_sap_tmp->enabled = FALSE;
        dp.type = D_DISABLE_REQ;
        dp.disp_disable_req.port_type = dlsw_fr_ptr->port_type;
        dp.disp_disable_req.swidb     = dlsw_fr_ptr->swidb;
        peer_make_cls(&dp);
        CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_REQ_OPNSTN      | CLS_CONFIRM:
       peer = clsi2peer(clsi_msg);
       if (peer == NULL)
         return(CLS_OK);
       if(clsi_ret_code != CLS_OK) {
          if(dlsw_peer_debug)
            buginf("\ndlsw_dlu_peer_rput,
                  ReqOpnStn Did not complete %d ", clsi_ret_code);
          peer->fr_state = FR_DISCONN;
       } else {
          if(dlsw_peer_debug)
            buginf("\ndlsw_dlu_peer_rput, ReqOpnStn complete normally");

          peer->p_cep_id =
	      ((ReqOpnStnCnf_t*) clsi_msg)->fCLSIHdr.fPCepId;
          dp.type = D_CONNECT_REQ;
          dp.disp_connect_req.pCepId = peer->p_cep_id;
          peer_make_cls(&dp);
       }
       CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_CLOSE_STN       | CLS_CONFIRM:
       if(dlsw_peer_debug)
          buginf("\ndlsw_dlu_peer_rput, CloseCnfm is ok");

       peer = clsi2peer(clsi_msg);
       if (peer == NULL)
         return(CLS_OK);

       peer->fr_state = FR_DISCONN;

       CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_CONNECT         | CLS_CONFIRM:
       peer = clsi2peer(clsi_msg);
       if (peer == NULL)
         return(CLS_OK);   
       if(clsi_ret_code != CLS_OK) {
          if(dlsw_peer_debug)
            buginf("\ndlsw_dlu_peer_rput, ConnectCnfm is not ok");
       } else {
          if(dlsw_peer_debug)
            buginf("\ndlsw_dlu_peer_rput, ConnectCnfm is ok");
          peer->fr_state = FR_CONN;
          dlsw_fr_signal_req(peer);

          /* Can improve the start up by kick starting the CAP_EXCHG */
          dlsw_peer_newstate(peer, PEER_WAIT_CAP);
       }

       CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_CONNECTED       | CLS_INDICATION:
       if(dlsw_peer_debug)
         buginf("\ndlsw_dlu_peer_rput, ConnectedCnfm");
       peer = clsi2peer(clsi_msg);
       if (peer == NULL)
         return(CLS_OK);
       peer->fr_state = FR_CONN;
       CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_DISCONNECT      | CLS_INDICATION:
      if(dlsw_peer_debug)
        buginf("\ndlsw_dlu_peer_rput, DisconnectInd");
        CLSIMsgDestroy(clsi_msg);
    break;
    case CLS_DISCONNECT      | CLS_CONFIRM:
        peer = clsi2peer(clsi_msg);
        if (peer == NULL)
          return(CLS_OK);
        dp.type = D_DISCONNECT_RSP;
        dp.disp_disconnect_rsp.pCepId = peer->p_cep_id;
        peer_make_cls(&dp);
        dp.type = D_CLOSESTN_REQ;
        dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
        dp.disp_closestn_req.identifier = peer->p_cep_id;
        peer_make_cls(&dp);        
        CLSIMsgDestroy(clsi_msg);
    break;

    case CLS_FLOW           | CLS_INDICATION:
      peer = clsi2peer(clsi_msg);
      if (peer == NULL)
        return(CLS_OK);
      level = ((FlowIndh_t *)clsi_msg)->fFlowLevel;

      if (level == (DLC_CONGESTION_HIGH | DLC_CONGESTION_INCR)) {
        PDEBUG("\nDLSw: dlsw_dlu_peer_rput() called for new DLC_CONG_HIGH");
        dlsw_peer_fsm(peer, BUSY_XPORT);
        peer_msg.type = PEER_FLOW_CTRL_INDICATION;
        peer_msg.p_flow_ind.peer_handle = (peerhandle_t) peer;
        peer_msg.p_flow_ind.cong_level = 
                            (DLSW_PEER_CONG_HIGH | DLSW_PEER_CONG_INCREASE);
        peer_to_core(&peer_msg);
      } else if ((level == DLC_CONGESTION_LOW) &&
          (peer->conn_state == PEER_XPORT_BUSY)) {
          PDEBUG("\nDLSw: dlsw_dlu_peer_rput() called for new DLC_CONG_LOW");
          dlsw_peer_fsm(peer, UNBUSY_XPORT);
          peer_msg.type = PEER_FLOW_CTRL_INDICATION;
          peer_msg.p_flow_ind.peer_handle = (peerhandle_t) peer;
          peer_msg.p_flow_ind.cong_level = DLSW_PEER_CONG_LOW;
          peer_to_core(&peer_msg);
      }
      break;

    case CLS_DATA  | CLS_INDICATION:
          if(dlsw_peer_debug)
            buginf("\ndlsw_dlu_peer_rput, DataInd");
          peer = clsi2peer(clsi_msg);
          if (peer == NULL)
            return(CLS_OK);    
          if ( (pak = CLSIMsgGetPak(clsi_msg)) == NULL) {
             CLSIMsgDestroy(clsi_msg);
             if(dlsw_peer_debug)
              buginf("\ndlsw_dlu_peer_rput, DataInd, pak is missing.");
          } else {
             data_ind = (DataInd_t*) clsi_msg;

             pak->datagramstart = (uchar *)data_ind->fData +
                                         data_ind->fReserveLength;
             pak->datagramsize = data_ind->fDataLength;

             peer->pkts_rx++;

             pak->datagramstart = pak->network_start;
             pak->peer_ptr = peer;
             p_enqueue(&dlsw_peer_llc2Q, pak);
          }
    break;
   }

 return(CLS_OK);
}


void peer_llc2_init (peerentry *peer, idbtype *idb, boolean sense)
{
CLSPortID  port_id;

 if(!dlsw_fr_running)
  return;

 peer->u_cep_id = (uint)peer;
 port_id.type = CLS_P_TYPE_SWIDB;
 port_id.fTypeInfo.swidb = idb;

 if (sense)
   dlsw_fr_enable_port(&port_id);
 else
   dlsw_fr_disable_port(&port_id);
}



static void dlsw_fr_enable_port (CLSPortID *port_id)
{
union D_primitives dp;

 if (dlsw_peer_debug) 
        buginf("\n DLSw-FR : Sending enable port ; port no : %d",
               CLSIPortIdGetPortNumber(port_id));
 dp.type = D_ENABLE_REQ;
 dp.disp_enable_req.port_type   = CLS_FRAME_RELAY;
 dp.disp_enable_req.swidb       = CLSIPortIdGetSwIdb(port_id);
 peer_make_cls(&dp);
}



static void dlsw_fr_disable_port (CLSPortID *port_id)
{
 dlsw_peer_send_deactring(port_id);
}

static void dlsw_peer_send_deactring (CLSPortID *port_id)
{
union D_primitives dp;
dlsw_ring_t        *dlsw_fr_ptr;
ushort             port_type;
idbtype            *sw_idb;

 port_type   = CLS_FRAME_RELAY;

 sw_idb = CLSIPortIdGetSwIdb(port_id);

 dlsw_fr_ptr = dlsw_get_ring_frpeer(sw_idb);

 if(!dlsw_fr_ptr)
  return;
 if (dlsw_peer_debug)
     buginf("\n DLSw-FR : sending deactivate ring for %s -"
	     " port_id = %d; port_type = %d ",
	     sw_idb->hwptr->hw_short_namestring, sw_idb, port_type);

 dp.type = D_DEACTIVATERING_REQ;
 dp.disp_deactivatering_req.u_sap_id = (uint)dlsw_fr_ptr;
 dp.disp_deactivatering_req.p_sap_id = dlsw_fr_ptr->p_sap_id;
 peer_make_cls(&dp);
}

static int dlsw_fr_peer_init ()
{
 gDLSWPeerQInit = CLSIQInitCreate("DLSWDLUPEER", dlsw_dlu_peer_rput);
 if (gDLSWPeerQInit == NULL)
  return FALSE;

 gCLS2DLSWReadPeerQ = CLSIQueueCreate(gDLSWPeerQInit, NULL);
 if (gCLS2DLSWReadPeerQ == NULL)
  return FALSE;

 gDLSW2CLSWritePeerQ = CLSOpen(gCLS2DLSWReadPeerQ);
 if (gDLSW2CLSWritePeerQ == NULL)
  return FALSE;

 dlsw_fr_running = TRUE;

 return TRUE;
}



/*
 *  peer_make_cls()
 */

static void peer_make_cls (union D_primitives *dp)
{
struct strbuf ctlbuf;
struct strbuf databuf;
int flags;

struct str_core_sndctl sndctlbuf;
TCLSIMsg *themsg;

 sndctlbuf.prim_type = DLSW_FSM_LAN;
 sndctlbuf.msg_type = dp->type;
 ctlbuf.len = sizeof(struct str_core_sndctl);
 ctlbuf.buf = (char *) &sndctlbuf;


 switch(dp->type)
  {
   case D_ENABLE_REQ:
    themsg = CreatePeerEnableReq((struct D_enable_req *) dp);
   break;

   case D_DISABLE_REQ:
    themsg = CreatePeerDisableReq((struct D_disable_req *) dp);
   break;

   case D_ACTIVATESAP_REQ:
    themsg = CreatePeerActSapReq((struct D_activatesap_req *) dp);
   break;

   case D_ACTIVATERING_REQ:
    themsg = CreatePeerActRingReq((struct D_activatering_req *) dp);
   break;

   case D_DEACTIVATESAP_REQ:
    themsg = CreatePeerDeActSapReq((struct D_deactivatesap_req *) dp);
   break;

   case D_DEACTIVATERING_REQ:
    themsg = CreatePeerDeActRingReq((struct D_deactivatering_req *) dp);
   break;

   case D_REQ_OPNSTN_REQ:
    themsg = CreatePeerReqOpnStnReq((struct D_reqopenstn_req *) dp);
   break;

   case D_CLOSESTN_REQ:
    themsg = CreatePeerCloseStnReq((struct D_closestn_req *) dp);
   break;

   case D_CONNECT_REQ:  
    themsg = CreatePeerConnectReq((struct D_connect_req *) dp);
   break;

   case D_CONNECT_RSP:
    themsg = CreatePeerConnectRsp((struct D_connect_rsp *) dp);
   break;

   case D_SIGNALSTN_REQ:
    themsg = CreatePeerSignalStnReq((struct D_signalstn_req *) dp);
   break;

   case D_DATA_REQ:
    themsg = CreatePeerDataReq((struct D_data_req *) dp);
   break;

   case D_FLOW_REQ:
    themsg = CreatePeerFlowReq((struct D_flow_req *) dp);
   break;

   case D_DISCONNECT_REQ:
    themsg = CreatePeerDiscReq((struct D_disconnect_req *) dp);
   break;

   case D_DISCONNECT_RSP:
    themsg = CreatePeerDiscRsp((struct D_disconnect_rsp *) dp);
   break;

   default:
    buginf("can not recognize the message\n");
    themsg = NULL;
    break;

  }
 databuf.buf = (char *) themsg;
 databuf.len = 0;   /* not used, set to 0 for now, future compability */
 flags = 0;
 dlsw_peer_cls(&ctlbuf, &databuf, &flags);

}



static TCLSIMsg* CreatePeerEnableReq (struct D_enable_req *dp)
{
EnableReq_t* enableReq;
uint  parmSize;
    
 switch (dp->port_type)
  {
   case CLS_FRAME_RELAY:	
	parmSize = 0; 
        break;

   default:
	buginf("\nDLSw: CreatePeerEnableReq, invalid port type (%d)", 
                        dp->port_type);
	return (NULL);
	break;
    }
    
   enableReq = (EnableReq_t*) CLSIMsgCreateHeader(CLS_ENABLE | CLS_REQUEST,
					   offsetof(EnableReq_t,fParms)-
					   CLS_ENABLE_REQ_HDR_SZ +
					   parmSize,
					   CLS_LLC_LAYER,
					   CLS_ENTITY_MGR,
					   0);
   if (enableReq == NULL)
	return NULL;

   enableReq->fDLCType = dp->port_type;  
   enableReq->fPortId.type = CLS_P_TYPE_SWIDB;
   enableReq->fPortId.fTypeInfo.swidb = dp->swidb;
   enableReq->fCorrelator = 0x00;     /* 0xdead; */

   if (dp->port_type != CLS_FRAME_RELAY) 
       buginf("\nDLSw: Enabling unknown port type %d", dp->port_type);

    return((TCLSIMsg*) enableReq);
}

static TCLSIMsg* CreatePeerDisableReq (struct D_disable_req *dp)
{
DisableReq_t* disableReq;
   
 disableReq = (DisableReq_t*) CLSIMsgCreateHeader(CLS_DISABLE | CLS_REQUEST,
                                           0,
                                           CLS_LLC_LAYER,
                                           CLS_ENTITY_MGR,
                                           0);
 if (disableReq == NULL)
  return NULL;
 
 disableReq->fPortId.type = CLS_P_TYPE_SWIDB;
 disableReq->fPortId.fTypeInfo.swidb = dp->swidb;
 disableReq->fDLCType = 
                    dp->port_type; 
 disableReq->fCorrelator = 0x00;     /* 0xdead; */
 
 /* Use defaults for everything, for now. */
 return((TCLSIMsg*) disableReq);
 
}

static TCLSIMsg* CreatePeerActSapReq (struct D_activatesap_req *dp)
{
ActSapReq_t* actSapReq;
ActSapLLCParms_t* frParms;
    
 actSapReq = (ActSapReq_t*) CLSIMsgCreateHeader(CLS_ACTIVATE_SAP | CLS_REQUEST,
					   offsetof(ActSapReq_t, fParms) -
					   CLS_ACT_SAP_REQ_HDR_SZ +
					   sizeof(ActSapLLCParms_t),
					   CLS_LLC_LAYER,
					   CLS_ENTITY_MGR,
					   0);
  if (actSapReq == NULL)
   return NULL;

  actSapReq->fCLSIHdr.fUSapId = 0x1234;
    
  actSapReq->fVersion = 1;
  actSapReq->fCorrelator = 0x00;      /*0xdead */
  actSapReq->fPortId.type = CLS_P_TYPE_SWIDB;
  actSapReq->fPortId.fTypeInfo.swidb = dp->swidb;
  actSapReq->fDLCType = dp->port_type;    

  frParms = (ActSapLLCParms_t*) &(actSapReq->fParms);
  frParms->fUserClass = 0x02;
  frParms->fSapValue = dp->sap_value;  
  frParms->fT1 = 0;
  frParms->fT2 = 0;
  frParms->fTi = 0;
  frParms->fMaxOut = 0;
  frParms->fMaxRetryCount = 0;		/* no limit */
  frParms->fMaxIn = 0;
  frParms->fMaxIField = 0;
    
  return((TCLSIMsg*) actSapReq);
}

static TCLSIMsg* CreatePeerDeActSapReq (struct D_deactivatesap_req *dp)
{
DeactSapReq_t* deactSapReq;
   
 deactSapReq = (DeactSapReq_t*) CLSIMsgCreateHeader(CLS_DEACTIVATE_SAP | CLS_REQUEST,
                                           0,
                                           CLS_LLC_LAYER,
                                           CLS_P_SAP_ID,
                                           dp->p_sap_id);
  if (deactSapReq == NULL)
   return NULL;
 
  deactSapReq->fCLSIHdr.fUSapId = 0x1234;
   
  deactSapReq->fVersion = 1;
  deactSapReq->fCorrelator = 0x00;      /*0xdead */
 
  return((TCLSIMsg*) deactSapReq);
}

static TCLSIMsg* CreatePeerReqOpnStnReq (struct D_reqopenstn_req *dp)
{
ReqOpnStnReq_t* reqOpnStnReq;
ReqOpnStnLLCParms_t* frParms;
uchar temp[2];
ushort fr_dlci;
    
 reqOpnStnReq = 
	(ReqOpnStnReq_t*) CLSIMsgCreateHeader(CLS_REQ_OPNSTN | CLS_REQUEST,
				      offsetof(ReqOpnStnReq_t, fParms) -
				      CLS_REQ_OPNSTN_REQ_HDR_SZ +
				      sizeof(ReqOpnStnLLCParms_t),
				      CLS_LLC_LAYER,
				      CLS_P_SAP_ID,
                                      dp->p_sap_id);

 if (reqOpnStnReq == NULL)
  return NULL;
 frParms = (ReqOpnStnLLCParms_t*) &(reqOpnStnReq->fParms);
 
 fr_dlci = dp->fr_dlci;

 temp[0] = fr_dlci >> 8; 
 temp[1] = fr_dlci & 0xFF;
 snpa_init(&frParms->fDestAddr, STATION_FR10, TARGET_UNICAST, temp);

 frParms->fDestSap = dp->rSap;

 snpa_init(&frParms->fSrcAddr, STATION_ILLEGAL, TARGET_UNICAST, temp);
 frParms->fSrcSap = dp->lSap;
 frParms->fRifLength = 0;

 /*
  * These are other configuration parameters. Mostly use defaults.
  */
 frParms->fT1 = 0;
 frParms->fT2 = 0;
 frParms->fTi = 0;
 frParms->fMaxOut = 0;
 frParms->fMaxRetryCount = 0;
 frParms->fMaxIn = 0;
 /*
  frParms->fWindowOption = 0;
 */

 frParms->fMaxOutIncr = 0;
 frParms->fMaxIField = 0;
 frParms->fXIDRetries = 0;
 frParms->fXIDTimer = 0;
 frParms->fExplorerRetries = 0;
 frParms->fExplorerTimer = 0;
 frParms->fExplorerOption = dp->options;
 frParms->fDlci = 0 ;

 reqOpnStnReq->fCLSIHdr.fUCepId = dp->uCepId;
 reqOpnStnReq->fVersion = 1;
 reqOpnStnReq->fCorrelator = 0xdead;
 reqOpnStnReq->fQueuingType = CLS_QUEUING_DEFAULT;

 frParms->fL3Pid =  L3_SNA;

  
 return((TCLSIMsg*) reqOpnStnReq);
}

static TCLSIMsg* CreatePeerConnectReq (struct D_connect_req *dp)
{
ConnectReq_t* connectReq;
size_t parmSize = 0;		/* For now. Do parameters later */
    
 connectReq = (ConnectReq_t*) CLSIMsgCreateHeader(CLS_CONNECT | CLS_REQUEST,
					     offsetof(ConnectReq_t, fParms) -
					     CLS_CONNECT_REQ_HDR_SZ +
					     parmSize,
					     CLS_LLC_LAYER,
					     CLS_P_CEP_ID,
					     dp->pCepId);
 if (connectReq == NULL)
  return NULL;

 /* we might want to lock the Rif here, this will send out a SABME */
 connectReq->fCLSIHdr.fModifyFlags = 0;
 connectReq->fVersion = CLSI_VERSION;
 connectReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) connectReq);
}

static TCLSIMsg* CreatePeerConnectRsp (struct D_connect_rsp *dp)
{
ConnectRsp_t* connectRsp;
 size_t parmSize = 0;		/* For now. Do parameters later */
    
 connectRsp = (ConnectRsp_t*) CLSIMsgCreateHeader(CLS_CONNECT | CLS_RESPONSE,
					     offsetof(ConnectRsp_t, fParms) -
					     CLS_CONNECT_RSP_HDR_SZ +
					     parmSize,
					     CLS_LLC_LAYER,
					     CLS_P_CEP_ID,
					     dp->pCepId);
 if (connectRsp == NULL)
  return NULL;

 connectRsp->fCLSIHdr.fActionCode = 0;
 connectRsp->fCLSIHdr.fModifyFlags = 0;
 connectRsp->fVersion = CLSI_VERSION;
 connectRsp->fCorrelator = 0xdead;
 connectRsp->fFlag = dp->flag;

 return((TCLSIMsg*) connectRsp);
}

static TCLSIMsg* CreatePeerSignalStnReq (struct D_signalstn_req *dp)
{
SignalStnReq_t* sigStnReq;
    
 sigStnReq = (SignalStnReq_t*)
 CLSIMsgCreateHeader(CLS_SIGNAL_STN | CLS_REQUEST,
		    sizeof(SignalStnReq_t) -
		    CLS_SIGNAL_STN_REQ_HDR_SZ,
		    CLS_LLC_LAYER,
		    CLS_P_CEP_ID,
		    dp->pCepId);
 if (sigStnReq == NULL)
  return NULL;
    
 sigStnReq->fCLSIHdr.fActionCode = 0; /* start data (exit local */
				 /* busy state) */
 sigStnReq->fVersion = CLSI_VERSION;
 sigStnReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) sigStnReq);
}

static TCLSIMsg* CreatePeerDataReq (struct D_data_req *dp)
{
DataReq_t* dataReq;
hword reservedSize;

    
 dataReq = (DataReq_t*) CLSIMsgCreateFromPak(dp->pak,
                                       CLS_DATA | CLS_REQUEST,
				       CLS_LLC_LAYER,
				       CLS_P_CEP_ID,
				       dp->pCepId,
                                       0,
                                       &reservedSize);
 if (dataReq == NULL)
  return NULL;

 dataReq->fVersion = CLSI_VERSION;
 dataReq->fCorrelator = 0xdead;
 dataReq->fReserveLength = reservedSize;
 return((TCLSIMsg*) dataReq);
}

static TCLSIMsg* CreatePeerDiscReq (struct D_disconnect_req *dp)
{
DisconnectReq_t* disconnectReq;
 size_t parmSize = 0;		/* For now. Do parameters later */
    
 disconnectReq = (DisconnectReq_t*) CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_REQUEST,
			offsetof(DisconnectReq_t, fCorrelator) -
			CLS_DISCONNECT_REQ_HDR_SZ +
			parmSize,
			CLS_LLC_LAYER,
			CLS_P_CEP_ID,
			dp->pCepId);
 if (disconnectReq == NULL)
  return NULL;

 disconnectReq->fVersion = CLSI_VERSION;
 disconnectReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) disconnectReq);
}

static TCLSIMsg* CreatePeerDiscRsp (struct D_disconnect_rsp *dp)
{
DisconnectRsp_t* disconnectRsp;
 size_t parmSize = 0;		/* For now. Do parameters later */
    
 disconnectRsp = (DisconnectRsp_t*) CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_RESPONSE,
				    offsetof(DisconnectReq_t, fCorrelator) -
				    CLS_DISCONNECT_RSP_HDR_SZ +
				    parmSize,
				    CLS_LLC_LAYER,
				    CLS_P_CEP_ID,
				    dp->pCepId);
 if (disconnectRsp == NULL)
  return NULL;

 disconnectRsp->fVersion = CLSI_VERSION;
 disconnectRsp->fCorrelator = 0xdead;

 return((TCLSIMsg*) disconnectRsp);
}

static TCLSIMsg* CreatePeerCloseStnReq (struct D_closestn_req *dp)
{
CloseStnReq_t* closeStnReq;
    
 closeStnReq = (CloseStnReq_t*) CLSIMsgCreateHeader(CLS_CLOSE_STN | CLS_REQUEST,
				     offsetof(CloseStnReq_t, fCorrelator) -
				     CLS_CLOSE_STN_REQ_HDR_SZ,
				     CLS_LLC_LAYER,
				     dp->identifierType,
				     dp->identifier);
 if (closeStnReq == NULL)
  return NULL;

 closeStnReq->fCLSIHdr.fUCepId = 0x00abcdef;
 closeStnReq->fCLSIHdr.fActionCode = CLS_ACTION_FORCED;
    
 closeStnReq->fVersion = CLSI_VERSION;
 closeStnReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) closeStnReq);
}


/*
 * Activate Ring Request.
 */

static TCLSIMsg* CreatePeerActRingReq (struct D_activatering_req *dp)
{
ActRngReq_t* actringReq;
ActRngTRParms_t* frParms;

 actringReq = (ActRngReq_t*) CLSIMsgCreateHeader(CLS_ACTIVATE_RING | CLS_REQUEST,
  					   offsetof(ActRngReq_t,fParms) -
					   CLS_ACT_RING_REQ_HDR_SZ +
					   sizeof(ActRngTRParms_t),
					   CLS_LLC_LAYER,
					   CLS_ENTITY_MGR,
					   0);

 if (actringReq == NULL)
  return NULL;

 actringReq->fCLSIHdr.fUSapId = dp->u_sap_id;

 actringReq->fVersion    = 0;

 actringReq->fDLCType = dp->port_type;  
 actringReq->fPortId.type = CLS_P_TYPE_SWIDB;
 actringReq->fPortId.fTypeInfo.swidb = dp->swidb;

 frParms = (ActRngTRParms_t*) &(actringReq->fParms);

 frParms->fT1 = 0;
 frParms->fT2 = 0;
 frParms->fTi = 0;
 frParms->fMaxOut = 0;
 frParms->fMaxRetryCount = 0;          /* no limit */
 frParms->fMaxIn = 0;
 frParms->fMaxIField = 0;
 
 return((TCLSIMsg*) actringReq);
}

/*
 * Deactivate Ring Request.
 */
static TCLSIMsg* CreatePeerDeActRingReq (struct D_deactivatering_req *dp)
{
DeActRngReq_t* deactringReq;
    
 deactringReq = (DeActRngReq_t*) CLSIMsgCreateHeader(CLS_DEACTIVATE_RING | CLS_REQUEST,
						     0,
						     CLS_LLC_LAYER,
						     CLS_P_SAP_ID,
						     dp->p_sap_id);
 if (deactringReq == NULL)
  return NULL;

 deactringReq->fVersion  =  1;
 deactringReq->fCorrelator = 0xdead;
 deactringReq->fCLSIHdr.fUSapId = dp->u_sap_id;

 return((TCLSIMsg*) deactringReq);
    
}

/*
 * Flow request.
 */

static TCLSIMsg* CreatePeerFlowReq (struct D_flow_req *dp)
{
FlowReq_t* flowReq;
    
 flowReq = (FlowReq_t*) CLSIMsgCreateHeader(CLS_FLOW | CLS_REQUEST,
                                       0,
				       CLS_LLC_LAYER,
				       CLS_P_CEP_ID,
				       dp->pCepId);
 if (flowReq == NULL)
  return NULL;

 flowReq->fVersion = CLSI_VERSION;
 flowReq->fCLSIHdr.fFlowAction = dp -> action;
 return((TCLSIMsg*) flowReq);
}

/*
 * Process messages coming from the CORE.
 */
static void dlsw_peer_cls (struct strbuf *ctlbuf, struct strbuf *databuf, int *flags)
{
char 		*str;
    
str = string_getnext();
 
 if (dlsw_peer_debug)
    buginf("\n PEER-DISP Sent : %s ",
	 CLSIMsgSprintfShort((TCLSIMsg *)databuf->buf, str));
 CLSIQueuePut(gDLSW2CLSWritePeerQ, (TCLSIMsg*) databuf -> buf);

}



/*********************/
/* Utility functions */
/*********************/

static void dlsw_peer_send_actring (TCLSIMsg *clsi_msg)
{
union         D_primitives dp;
dlsw_ring_t   *dlsw_fr_ptr;
dlsw_sap_t    *dlsw_sap_tmp;
CLSPortID     port_id;
ushort        port_type;
hwidbtype     *hw_idb;
idbtype       *sw_idb;
char   *str;
 
 str = string_getnext();
 
 port_id   = ((EnableCnf_t *)clsi_msg)->fPortId;
 port_type   =  CLS_FRAME_RELAY;
 
 sw_idb = CLSIPortIdGetSwIdb(&port_id);
 if (sw_idb == NULL ) {
      errmsg(DLSWC_ERR_BADCLSI,
             CLSIMsgSprintfShort(clsi_msg, str),
             "Sending Act Ring", "sw_idb is NULL");
      return;
 }
 hw_idb = sw_idb->hwptr;
 if (hw_idb == NULL ) {
      errmsg(DLSWC_ERR_BADCLSI,CLSIMsgSprintfShort(clsi_msg, str),
             "Sending Act Ring", "hw_idb is NULL ");
      return;
 }
 dlsw_fr_ptr = dlsw_get_ring_frpeer(sw_idb);

 if (!dlsw_fr_ptr) {
    dlsw_fr_ptr = malloc_named(sizeof(dlsw_ring_t), "DLSW-RING");
    if (!dlsw_fr_ptr) {
      errmsg(PEER_NOMEM,"activate ring");
      return;
    }
    dlsw_fr_ptr->enabled = FALSE;
    enqueue(&dlsw_ringQ, dlsw_fr_ptr);
 }
 dlsw_sap_tmp = dlsw_get_sap(dlsw_fr_ptr, DLSW_FR_SAP);
 if (!dlsw_sap_tmp) {
    dlsw_sap_tmp = malloc_named(sizeof(dlsw_sap_t), "DLSW-SAP");
    if (!dlsw_sap_tmp) {
      errmsg(PEER_NOMEM,"activate sap");
      return;
    }
    dlsw_sap_tmp->sap = DLSW_FR_SAP;
    dlsw_sap_tmp->enabled = FALSE;
    enqueue(&(dlsw_fr_ptr->dlsw_sapQ), dlsw_sap_tmp);
 }

 if ((dlsw_fr_ptr->enabled)&&(dlsw_sap_tmp->enabled))
   return;
 
 dlsw_fr_ptr->port_type   = port_type;
 dlsw_fr_ptr->mtu_size  = llc_mtu(hw_idb);
 dlsw_fr_ptr->enabled = FALSE; /* set to TRUE when cnf comes back */
 dlsw_fr_ptr->swidb = sw_idb;
 
 dp.type = D_ACTIVATERING_REQ;
 dp.disp_activatering_req.u_sap_id = (uint)dlsw_fr_ptr;
 dp.disp_activatering_req.bridge_number = 0;
 dp.disp_activatering_req.source_ring   = 0;
 dp.disp_activatering_req.target_ring   = 0;
 dp.disp_activatering_req.port_type     = port_type;
 dp.disp_activatering_req.swidb         = sw_idb;
 if (dlsw_peer_debug)
    buginf("\n DLSw-FR : Sending activate ring for %s - port_id = %d
           port_type = %d dgra(UsapID) = %x",
	   sw_idb->hwptr->hw_short_namestring, sw_idb, port_type, dlsw_fr_ptr);
 peer_make_cls(&dp);

}


static void dlsw_peer_send_open_stn (peerentry *peer)
{
union D_primitives  dp;

 dp.type = D_REQ_OPNSTN_REQ;
 dp.disp_reqopenstn_req.uCepId = peer->u_cep_id;
 dp.disp_reqopenstn_req.fr_dlci =  peer->fr_dlci;
 dp.disp_reqopenstn_req.rSap = DLSW_FR_SAP;
 dp.disp_reqopenstn_req.lSap = DLSW_FR_SAP;
 dp.disp_reqopenstn_req.rif = NULL;

 dp.disp_reqopenstn_req.options = 0;

 dp.disp_reqopenstn_req.p_sap_id = peer->p_sap_id;
 peer_make_cls(&dp);

}

#ifdef NOBODY_CALLS_ME
static void dlsw_peer_send_disc_req (peerentry *peer)
{
union D_primitives dp;

 dp.type = D_DISCONNECT_REQ;
 dp.disp_disconnect_req.pCepId = peer->p_cep_id;
 peer_make_cls(&dp);
}
#endif

static void dlsw_peer_send_close_stn (peerentry *peer)
{
union D_primitives dp;

 dp.type = D_CLOSESTN_REQ;
 dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
 dp.disp_closestn_req.identifier = peer->p_cep_id;
 peer_make_cls(&dp);
}

static void dlsw_peer_send_conn_req (peerentry *peer)
{
union D_primitives dp;

 dp.type = D_CONNECT_REQ;
 dp.disp_connect_req.pCepId = peer->p_cep_id;
 peer_make_cls(&dp);
}

static void dlsw_fr_signal_req (peerentry *peer)
{
union D_primitives dp;

 dp.type = D_SIGNALSTN_REQ;
 dp.disp_signalstn_req.pCepId = peer->p_cep_id;
 peer_make_cls(&dp);
}

static peerentry *clsi2peer (TCLSIMsg *clsi_msg)
{
    peerentry *peer = NULL;

    if ((clsi_msg->fIdentifierType == CLS_U_CEP_ID) &&
        (clsi_msg->fIdentifier != CLS_NO_CEP)) {
        peer = (peerentry *) clsi_msg->fIdentifier;
        if (peer == (peerentry *) peer->u_cep_id)
          return(peer);
    }

    if (peer_debug_all) {
      if (peer == 0)
        buginf("\nDLSw: Invalid_peer(): peer:%x", peer);
      else
        buginf("\nDLSw: Invalid_peer(): peer:%x rem:%d status:%x type:%x",
                   peer, peer->remove, peer->peer_status, peer->peer_type);
      errmsg(PEER_GENERAL, "Invalid_peer");
    }

    return(NULL);
}

