/* $Id: dlsw_peer_xport.c,v 3.6.6.4 1996/08/28 12:41:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_xport.c,v $
 *------------------------------------------------------------------
 * DLSw Peer Transport External Service Routines 
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Provides DLSw peer transport services to the DLSw sub-system for
 * all transport types.
 *------------------------------------------------------------------
 * $Log: dlsw_peer_xport.c,v $
 * Revision 3.6.6.4  1996/08/28  12:41:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.6.3  1996/07/23  13:21:51  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.6.6.2  1996/07/09  18:08:56  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.6.6.1  1996/05/17  10:46:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.6.6  1996/05/14  03:20:45  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.6.5  1996/04/26  07:34:52  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.6.4  1996/04/25  19:37:53  ravip
 * CSCdi54131:  DLSw MIB ciscoDlswTConnOper Table returns 0 for stats
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.6.3  1996/04/23  22:27:53  fbordona
 * CSCdi54796:  No NetBIOS session between DLSw-peers in different groups
 *              Allow all CSM packets to traverse border relay.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.6.2  1996/04/05  23:12:06  kpatel
 * CSCdi51479:  Memory leak when removing dlsw configuration.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.6.1  1996/03/17  17:38:22  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6  1996/03/05  18:49:39  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.5  1996/02/01  06:01:47  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/11/29  22:06:42  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.3  1995/11/17  09:04:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:25:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/26  16:56:53  fbordona
 * CSCdi39990:  7k crashes for  DLSw+ with Direct transport,
 *              fast s/w and hdlc encap.
 *              Don't explore fast switched peers for non-TR lans.
 *              Remove dead code.
 *
 * Revision 2.3  1995/08/07  05:31:41  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.2  1995/06/18  21:21:36  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:27:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../tcp/tcp.h"
#include "../if/network.h"
#include "../if/tring.h"
#include "../ui/debug.h"

#include "../if/rif_util.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_debug.h"


/*
 * peer_init_remote_xport()
 *
 * If called beccause a remote peer was just configured, it is the 
 * responsibility of the caller to check the configuration commands syntax.
 */

boolean peer_init_remote_xport (peerentry *peer, idbtype *swidb, ulong conf_lf_code)
{
    int media_lf;
    int bytes;
    paktype *pak;
    iphdrtype *iphead;

    media_lf = RC_LFINITIAL;
    switch (peer->xport_type) {
      case DLSW_TCP:
        peer->abort   = dlsw_tcpd_abort;
        peer->connect = dlsw_tcpd_connect;
        peer->sender  = dlsw_tcpd_sender;
        break;

      case DLSW_FST:
        if (peer->cap.local_ack) {
            printf("\n%%Only TCP remote-peers can use local-acknowledgement");
            return(FALSE);
        }

        bytes = IPHEADERBYTES(NOPT);
        pak = getbuffer(bytes);
        if (!pak) {
            return(FALSE);
        }
        pak->datagramstart = pak->network_start;
        pak->datagramsize = bytes;
        ipbuild_head(pak, bytes, FST_DLSW_PROT, lpeer->ipaddr,
                     peer->ipaddr, NULL, &ipdefaultparams);
        iphead = (iphdrtype *) pak->datagramstart;
        iphead->dontfragment = TRUE;
        peer->ip_header = malloc(sizeof(iphdrtype));
        if (peer->ip_header == NULL) {
            return(FALSE);
        }
        *peer->ip_header = *iphead;     /* copy the header */
        retbuffer(pak);                 /* done with the pak, throw it away */

        peer->ipcache_hash = nethash(peer->ipaddr);
        peer->sender  = dlsw_fst_sender;
        peer->abort   = dlsw_fst_abort;
        peer->connect = dlsw_fst_connect;
        media_lf = RC_LF516;
        reg_invoke_dlsw_init_fast_peer((void *) peer);
        break;

      case DLSW_IF:
      case DLSW_FR:
        /*
         * If this interface does not support an adjustable maxdgram size,
         * clean up and return. Error message printed by supports_maxdgram().
         */
        if (!supports_maxdgram(peer->outidb)) {
            return(FALSE);
        }

        peer->sender  = dlsw_if_sender;
        peer->abort   = dlsw_if_abort;
        peer->connect = dlsw_if_connect;
        peer->outidb->dlsw_ifpeer = peer;
        reg_invoke_dlsw_init_fast_peer((void *) peer);
        if ((peer->xport_type == DLSW_FR) && (peer->local_ack))
          peer_llc2_init(peer, swidb, TRUE);
        break;

      case DLSW_IF_LAN:
        /*
         * Direct over lan media not supported.
         * Specifically, IDB_ETHER, IDB_FDDI, and IDB_TR.
         */
        return(FALSE);
    }

    if (conf_lf_code == RC_LFINITIAL) {
        peer->largest_frame = media_lf;
    } else if ((conf_lf_code > media_lf) && (peer->xport_type != DLSW_FST)) {
        printf("%%Media only supports a largest frame of %d.  Lowering lf.\n",
               rc_code_to_bytes(media_lf));
        peer->largest_frame = media_lf;
    } else {
        peer->largest_frame = conf_lf_code;
    }
    peer->largest_frame_def = conf_lf_code;
    return(TRUE);
}


/*
 * core_to_peer()
 *
 * Do not consume the packet if parameter error.
 */
core2peer_ret_t core_to_peer (peer_msg_u *msg)
{
    peerentry *peer;
    core2peer_ret_t rc;
    ssp_hdr_t *ssp_hdr;

    if (!lpeer) {
        datagram_done(msg->p_put.pak);
        msg->p_put.pak = NULL;
        return(PEER_NO_CONNECTIONS);
    }

    switch (msg->type) {
      case PEER_PUT:
	peer = (peerentry *) msg->put_handle;

        /*
         * Too many crashes due to invalid peer handles so here's the peer
         * validation check until the core and peer play nicely.
         */
        if (peer_ready_to_send(peer)) {
            if ((peer->peer_type == DEMAND_PEER) &&
                (peer->conn_state != PEER_CONN)) {
                ssp_hdr = (ssp_hdr_t *) msg->p_put.pak->datagramstart;
                if (((ssp_hdr->ssp_flags & SSP_FLAGS_EX) &&
                     ((ssp_hdr->ssp_msg_type == SSP_OP_canureach) ||
                      (ssp_hdr->ssp_msg_type == SSP_OP_icanreach))) ||
                    (ssp_hdr->ssp_msg_type == SSP_OP_netbios_nq) ||
                    (ssp_hdr->ssp_msg_type == SSP_OP_netbios_nr) ||
                    (ssp_hdr->ssp_msg_type == SSP_OP_netbios_anq) ||
                    (ssp_hdr->ssp_msg_type == SSP_OP_netbios_anr) ||
                    (ssp_hdr->ssp_msg_type == SSP_OP_dataframe)) {
                    rc = send_bp_resp(msg);
                } else {
                    rc = PEER_UNREACHABLE;
                    datagram_done(msg->p_put.pak);
                    msg->p_put.pak = NULL;
                }
            } else
                rc = peer_put_pak(peer, &msg->p_put);
        } else {
            rc = PEER_INVALID_HANDLE;
            datagram_done(msg->p_put.pak);
            msg->p_put.pak = NULL;
        }
        break;

      case PEER_PUT_BCAST:
        rc = do_bcast(&msg->p_put); 
        break;

      case PEER_PUT_GROUP:
        rc = send_peer_group(msg);
        break;

      case PEER_PUT_POD_RSP:
        rc = send_bp_resp(msg);
        break;

      default:
        /*
         * Do not consume caller's packet */
        errmsg(PEER_BADVALUE, "core_to_peer()", "type", msg->type); 
        rc = PEER_PARAMETER_ERROR;
        break;
    }
    return(rc);
}


/* 
 * peer_put_pak() 
 *
 * CSM will be adjusting the maximum packet size if necessary.
 * Don't close the peer connection on send failure; Let keepalives
 * take care of this.
 *
 * If CSM wants the Peer Mgr to validate that the destination peer(s)
 * have access to the LAN input ring, CSM will pass in a Non-zero
 * lan input ring in the peer_put_t. If CSM does not want this checking,
 * it will pass in a zero ring number.
 *
 * Consumes the pak in all cases. 
 */
core2peer_ret_t peer_put_pak (peerentry *peer, peer_put_t *pm)
{
    peer2core_ret_t rc;
    netbios_header *nbh;
    char buff[DLSW_MAX_PEER_ID];

    if (!pm->pak)
        return(PEER_PARAMETER_ERROR);
        
    /* If backup peer and primary connected, do not send */
    if ((peer->primary_peer) &&
        (peer->primary_peer->conn_state == PEER_CONN) &&
        (pm->lan_port) && (pm->type == PEER_PUT_BCAST)) {
        PDEBUG("\nDLSw: %s is backup peer, primary peer %i already connected",
                peer_str(peer, buff), peer->primary_peer->ipaddr);
        datagram_done(pm->pak);
        pm->pak = NULL;
        return(PEER_FILTERED);
    }

    if (pm->lan_port &&
        !peer_pass_port_filter((peerhandle_t) peer, pm->lan_port)) {
	PDEBUG("\nDLSw: %s not accessible from lan input port %s",
	       peer_str(peer, buff), 
               pm->lan_port->swidb->hwptr->hw_namestring);
        datagram_done(pm->pak);
	pm->pak = NULL;
        return(PEER_FILTERED);
    }

    if (CHECK_ON(pm->flags, FILTER_ME)) {
        /*
         * Exclusivity and Supported Sap Filtering.
         */
        rc = peer_cap_filter(peer, pm->ssap, pm->dsap);
        if (rc != PEER_PASS) {
            PDEBUG("\nDLSw: frame cap filtered (%d) to %s",
                   rc, peer_str(peer, buff));
            datagram_done(pm->pak);
	    pm->pak = NULL;
            return(PEER_FILTERED);
        }

        if (CHECK_ON(pm->flags, PEER_NETBIOS_UI))
            nbh = (netbios_header *) pm->pak->info_start;
        else
            nbh = NULL;

        /*
         * LSAP, netbios host, dmac, and netbios bytes output access list
         * filtering.
         */
        if (!peer_out_filter(peer, pm->dsap, pm->ssap, nbh, pm->dmac)) {
            PDEBUG("\nDLSw: frame output access list filtered to %s",
                   peer_str(peer, buff));
            datagram_done(pm->pak);
	    pm->pak = NULL;
            return(PEER_FILTERED);
        }
    }

    /*
     * If the packet originated from a LAN on this router and it is
     * a broadcast, then the peer must adjust down the SSP lf, if
     * necessary, for each peer it sends to. CSM can not do this since
     * it does not know which peers are receiving the bcast.
     */
    if (pm->lan_port && (pm->type == PEER_PUT_BCAST)) {
        dlx_bp_hdr_t *bp_hdr;
        ssp_hdr_t *ssp_hdr;
        long mtu_bytes;
        uchar flag_bit;
         
        bp_hdr = (dlx_bp_hdr_t *) pm->pak->datagramstart;
        if ((bp_hdr->proto_ver_id == DLX_PROTO_VERSION) &&
            (is_dlx_bp_op(bp_hdr->message_type))) {
            ssp_hdr = (ssp_hdr_t *) (pm->pak->datagramstart +
                                     sizeof(dlx_bp_hdr_t)); 
        } else if (bp_hdr->proto_ver_id == SSP_VERS) {
            ssp_hdr = (ssp_hdr_t *) pm->pak->datagramstart;
        } else {
            ssp_hdr = NULL;
        } 

        if ((ssp_hdr) && (ssp_hdr->ssp_flags & SSP_FLAGS_EX) &&
            ((ssp_hdr->ssp_msg_type == SSP_OP_canureach) ||
             (ssp_hdr->ssp_msg_type == SSP_OP_netbios_nq))) {
            flag_bit = ssp_hdr->ssp_lf & SSP_LF_IGNORE_BIT;
            mtu_bytes = min(peerid_to_mtu((peerhandle_t) peer),
                            rc_code_to_bytes(ssp_hdr->ssp_lf &
                                             ~SSP_LF_IGNORE_BIT));
            ssp_hdr->ssp_lf = bytes_to_rc_code(mtu_bytes);
            ssp_hdr->ssp_lf |= flag_bit;
        }
    }

    peer_update_out_stat(peer, pm->pak);
    rc = (*peer->sender)(peer, pm->pak, TRUE, pm->pak_cat, pm);
    if ((rc != PEER_SUCCESS) && (rc != PEER_SUCCESS_AND_BUSY)) {
        PDEBUG("\nDLSw: frame failed (%d) to be sent to %s",
               rc, peer_str(peer, buff));
    }
    return(rc);
}

/*
 * connect_priority_peers()
 */

boolean connect_priority_peers (peerentry *peer)
{
    uchar i;
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: connect_priority_peers() for %s", peer_str(peer, buff));
    for (i = 1; i <= DLSW_PRIORITY_COUNT; i++) {
        if (!(*peer->connect)(peer->priority_peers[i])) {
            PDEBUG("\nDLSw: connecting priority %s - %d failed", 
                   peer_str(peer, buff), i);
            dlsw_peer_fsm(peer, CLOSE_CONNECTION);
            return(FALSE);
        } 
        GET_TIMESTAMP(peer->priority_peers[i]->uptime);
        peer->priority_peers[i]->conn_state = PEER_CONN;
    }
    return(TRUE);
}
