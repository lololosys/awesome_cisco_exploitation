/* $Id: dlsw_peer_border.c,v 3.16.6.9 1996/09/09 17:39:54 fbordona Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_border.c,v $
 *------------------------------------------------------------------
 * DLSw Peer Explorer Forwarding Optimization Routines
 *
 * August 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * These routines use cisco's DLX protocol to reach every DLSw
 * peer in the network with a minimal number of explorer packets.
 *------------------------------------------------------------------
 * $Log: dlsw_peer_border.c,v $
 * Revision 3.16.6.9  1996/09/09  17:39:54  fbordona
 * CSCdi66251:  Filtering at DLSw layer will not work on border router
 * Branch: California_branch
 *
 * Revision 3.16.6.8  1996/08/28  12:40:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.16.6.7  1996/08/09  23:43:22  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.16.6.6  1996/08/09  17:30:17  fbordona
 * CSCdi65603:  DLSw: SYS-2-LINKED: Bad enqueue of 60AE6FC0 in queue
 *              Allow DLSw FST over CIP LAN interface.
 * Branch: California_branch
 *
 * Revision 3.16.6.5  1996/08/01  19:04:13  akhanna
 * CSCdi64537:  DLSw peer cost has no impact on peer selection.
 * Branch: California_branch
 *
 * Revision 3.16.6.4  1996/07/23  13:21:42  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.16.6.3  1996/07/09  18:08:28  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.16.6.2  1996/05/21  09:45:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.16.6.1  1996/05/17  10:46:02  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.13.2.5  1996/04/26  07:34:28  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.13.2.4  1996/04/25  19:37:42  ravip
 * CSCdi54131:  DLSw MIB ciscoDlswTConnOper Table returns 0 for stats
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.3  1996/04/24  18:35:20  fbordona
 * CSCdi55588:  DLSw: Use managed timers for connecting dynamic peers
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.2  1996/04/20  02:08:10  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.1  1996/03/17  17:38:07  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.16  1996/03/06  16:55:18  fbordona
 * CSCdi50687:  dlsw peer-on-demand-defaults does not allow the largest
 *              frame lf.
 *
 * Revision 3.15  1996/03/05  18:49:25  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.14  1996/02/27  21:36:21  fbordona
 * CSCdi49949:  DLSw peer-on-demand peers ignore tcp-queue-max.
 *              Improve "show dlsw peers" output.
 *
 * Revision 3.13  1996/02/21  03:35:54  fbordona
 * CSCdi48915:  DLSw+ send frames out out sequence.
 *              Fix deadlock when permitted count reaches 0.
 *
 * Revision 3.12  1996/02/01  06:01:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11  1996/02/01  02:57:00  fbordona
 * CSCdi47930:  DLSw session does not reconnect after peer powered off.
 *              Only attempt one peer connection at at time to avoid
 *              starving the dlsw background process.
 *
 * Revision 3.10  1996/01/29  07:27:19  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.9  1996/01/18  22:24:47  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/15  02:36:06  fbordona
 * CSCdi46772:  Router doing DLSW peering reloads when exposed to traffic
 *
 * Revision 3.7  1996/01/11  14:45:03  fbordona
 * CSCdi46684:  Improve DLSw broadcast handling during heavy wan congestion
 *
 * Revision 3.6  1995/12/17  18:26:05  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/11/30  21:22:49  fbordona
 * CSCdi44759:  DLSw peer-on-demand peers prematurely disconnect.
 *              Fix peer-on-demand crash introduced in 11.1
 *
 * Revision 3.4  1995/11/29  22:06:26  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.3  1995/11/17  09:03:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:24:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:55:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/20  14:10:01  ravip
 * CSCdi40722: Exclusive reachability option does not filter frames
 *             correctly. Fixed problems with deleting reachability cache.
 *
 * Revision 2.2  1995/09/26  16:56:48  fbordona
 * CSCdi39990:  7k crashes for  DLSw+ with Direct transport,
 *              fast s/w and hdlc encap.
 *              Don't explore fast switched peers for non-TR lans.
 *              Remove dead code.
 *
 * Revision 2.1  1995/06/07  20:27:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_priority.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../if/network.h"
#include "../ui/debug.h"

#include "../ibm/netbios.h"
#include "../srt/rsrb.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_cap_exchg.h"
#include "../dlsw/dlsw_debug.h"


/*
 * File Scope Function Prototypes
 */
static void send_to_other_groups (paktype *);
static void send_to_group_members (peerentry *, paktype *);
static void pass_to_core (paktype *);


/*
 ***********************************************************************
 * Border Peer Administrative Functions
 ***********************************************************************
 */

/*
 * bp_fill_pm()
 *
 * fills in the peer_put_t structure for frames being forwarded/relayed
 * by a border peer. peer_put_t requires that pak->info_start points to 
 * the netbios header if the PEER_NETBIOS_UI flag is set. PEER_NETBIOS_UI
 * is used to indicate a UI frame for any SAP.
 */

static void bp_fill_pm (paktype *pak, peer_put_t *pm)
{
    ssp_control_frame_t *ssp_hdr;

    pm->type = PEER_PUT;
    pm->dest_u.peer_handle = (peerhandle_t) NULL;
    pm->lan_port = NULL; /* no port-list checking for relay frames */
    pm->pak = pak;
    pm->pak_cat = PEER_BCAST;

    ssp_hdr = (ssp_control_frame_t *) 
              (pak->datagramstart + sizeof(dlx_bp_hdr_t));
    if (ssp_hdr->frame_direction == SSP_DIRN_FORWARD) {
        pm->smac = ssp_hdr->dlc_id.origin_mac_address;
        pm->dmac = ssp_hdr->dlc_id.target_mac_address;
        pm->ssap = ssp_hdr->dlc_id.origin_link_sap;
        pm->dsap = ssp_hdr->dlc_id.target_link_sap;
    } else {
        pm->smac = ssp_hdr->dlc_id.target_mac_address;
        pm->dmac = ssp_hdr->dlc_id.origin_mac_address;
        pm->ssap = ssp_hdr->dlc_id.target_link_sap;
        pm->dsap = ssp_hdr->dlc_id.origin_link_sap;
    }

    pm->flags = 0;
    if (ssp_hdr->dlc_header_len == SSP_SNA_DLC_HDR_LEN) {
        pm->flags |= PEER_SNA_BCAST;
    } else {
        /*
         * PEER_NETBIOS_UI flag indicates packet is a UI frame for 
         * any SAP.
         */
        pm->flags |= PEER_NETBIOS_UI;
        pak->info_start = (char *) (pak->datagramstart + 
                                    sizeof(dlx_bp_hdr_t) +
                                    sizeof(ssp_control_frame_t) +
                                    TRING_ENCAPBYTES +
                                    SRB_MAXRIF + SAP_HDRBYTES);
    }
}

/*
 * add_border_peer_info()
 *
 * Only called if 'peer' has border peer ability.
 */

border_peer_ret_t add_border_peer_info (peerentry *peer)
{
    peerentry *bp; 
    border_peer_ret_t bp_rc;

    /*
     * Are peers in the same peer group?
     */ 
    if (peer->cap.group == lpeer->cap.group) {
        if (!lpeer->border_peer) {
            lpeer->border_peer = peer;
            bp_rc = BEST_BP;
        } else if (peer->cost != DLSW_NORM_PCOST) {
            if (lpeer->border_peer->cost != DLSW_NORM_PCOST) {
                if (peer->cost < lpeer->border_peer->cost) {
                    lpeer->border_peer = peer;
                    bp_rc = BEST_BP;
                }
                else
                    bp_rc = BACKUP_BP;
            } else {
                if (peer->cost < lpeer->border_peer->cap.cost) {
                    lpeer->border_peer = peer;
                    bp_rc = BEST_BP;
                }
                else
                    bp_rc = BACKUP_BP;
            }
        } else {
            /* peer->cost == DLSW_NORM_PCOST */
            if (lpeer->border_peer->cost != DLSW_NORM_PCOST) {
                if (peer->cap.cost < lpeer->border_peer->cost) {
                    lpeer->border_peer = peer;
                    bp_rc = BEST_BP;
                }
                else
                    bp_rc = BACKUP_BP;
            } else {
                if (peer->cap.cost < lpeer->border_peer->cap.cost) {
                    lpeer->border_peer = peer;
                    bp_rc = BEST_BP;
                }
                else
                    bp_rc = BACKUP_BP;
            }
        }

    } else {
        bp = (peerentry *) conn_dlx_peerQ[peer->cap.group].qhead;
        if (bp) {
            if (!bp->cap.border)
                bp_rc = BEST_BP;
            else if (peer->cost != DLSW_NORM_PCOST) {
                if (bp->cost != DLSW_NORM_PCOST) {
                    if (peer->cost < bp->cost)
                        bp_rc = BEST_BP;
                    else
                        bp_rc = BACKUP_BP;
                } else {
                    if (peer->cost < bp->cap.cost)
                        bp_rc = BEST_BP;
                    else
                        bp_rc = BACKUP_BP;
                }
            } else {
                /* peer->cost == DLSW_NORM_PCOST */
                if (bp->cost != DLSW_NORM_PCOST) {
                    if (peer->cap.cost < bp->cost)
                        bp_rc = BEST_BP;
                    else
                        bp_rc = BACKUP_BP;
                } else {
                    if (peer->cap.cost < bp->cap.cost)
                        bp_rc = BEST_BP;
                    else
                        bp_rc = BACKUP_BP;
                }
            }
        } else {
            bp_rc = BEST_BP;
        }
    }
    return(bp_rc);
}


/*
 * delete_border_peer_info()
 *
 * A border peer is being disconnected. If this router is
 * currently using it as a border peer, we need to replace it
 * with the next best border peer based on peer cost.
 *
 * Caller must have interrupts disabled.
 */

void delete_border_peer_info (peerentry *peer)
{
    peerentry *p;
    peerentry *best_bp = NULL;

    if (peer->cap.group == lpeer->cap.group) {
        if (lpeer->border_peer == peer) {
            p = (peerentry *) conn_dlx_peerQ[lpeer->cap.group].qhead;
            while ((p) && (p->cap.border)) {
                if (p != lpeer->border_peer) {
                    if (!best_bp)
                        best_bp = p;
                    else if (p->cap.cost < best_bp->cap.cost)
                        best_bp = p;
                }
                p = p->next;
            }
            lpeer->border_peer = best_bp;
        }
    } else {
        p = (peerentry *) conn_dlx_peerQ[peer->cap.group].qhead;
        while ((p) && (p->cap.border)) {
            if (p != peer) {
                if (!best_bp)
                    best_bp = p;
                else if (p->cap.cost < best_bp->cap.cost)
                    best_bp = p;
            }
            p = p->next;
        }
        if (best_bp) {
            unqueue(best_bp->qptr, best_bp);
            requeue(best_bp->qptr, best_bp);
        }
    }
}


/*
 * peer_add_circuit()
 *
 *  -- Notifies the peer manager that it should connect the peer if it
 *     is a peer-on-demand.
 *
 *  -- Unfortunately, we can't connect TCP peers if the thread of
 *     execution is off of a process (such as IP Input) where blocking 
 *     is prohibitted. This is because tcp_driver_active_open() will 
 *     block.  The core calls peer_add_circuit() on the IP Input thread
 *     after receiving certain wan messages, so we must connect tcp pods
 *     in the background.
 */

void peer_add_circuit (peerhandle_t peer_handle)
{
    peerentry *peer = (peerentry *) peer_handle;

    peer->num_ckts++;
    peer->circuit_creates++;

    if ((peer->peer_type == DEMAND_PEER) &&
        (lpeer->conn_mode != PASSIVE_MODE) &&
        (peer->conn_state == PEER_DISC) &&
        (peer->connect_pod == FALSE)) {
        peer->connect_pod = TRUE;
        mgd_timer_start(&peer->conn_timer, 0);
        peer->conn_retries = 0;
    }
}


/*
 * peer_delete_circuit()
 */
void peer_delete_circuit (peerhandle_t peer_handle)
{
    peerentry *peer = (peerentry *) peer_handle;

    /*
     * Sanity check to avoid stale peer handles.
     */
    if (valid_peer(peer)) {
        peer->num_ckts--;
        if (peer->num_ckts == 0)
            GET_TIMESTAMP(peer->zero_ckt_time);
    }
}

/*
 * call to intialize POD peer setup
 */
int peer_new_ckt (peerhandle_t peer_handle)
{
    peerentry *peer = (peerentry *) peer_handle;
    if (!peer_ready_to_send(peer)) 
      return (PEER_INVALID_PEER);
 
    if ((peer->peer_type == DEMAND_PEER) &&
        (lpeer->conn_mode != PASSIVE_MODE) &&
        (peer->conn_state == PEER_DISC) &&
        (peer->connect_pod == FALSE)) {
        peer->connect_pod = TRUE;
        mgd_timer_start(&peer->conn_timer, 0);
        peer->conn_retries = 0;
        return(PEER_WILL_START);
    } else {
        return(PEER_STARTED);
    }
}



/*
 ***********************************************************************
 * Border Peer Frame Receiving Functions
 ***********************************************************************
 */

/*
 * recv_member_to_border()
 */

void recv_member_to_border (peerentry *peer, paktype *pak)
{
    paktype *newpak;
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: recv_member_to_border() from %s", peer_str(peer, buff));
    /*
     * Pass a copy to the DLSw Core
     */
    newpak = pak_duplicate(pak);
    if (newpak)
        pass_to_core(newpak);

    /*
     * Forward this frame to all group members
     * send_to_group_members() does not consume pak.
     */
    send_to_group_members(peer, pak);

    /*
     * Forward this frame to the world.
     * send_to_other_groups() does not consume pak.
     */
    send_to_other_groups(pak);
    datagram_done(pak);
}


/*
 * recv_border_to_member()
 */

void recv_border_to_member (peerentry *peer, paktype *pak)
{
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: recv_border_to_member() from %s", peer_str(peer, buff));
    pass_to_core(pak);
}


/*
 * recv_border_to_border()
 */

void recv_border_to_border (peerentry *peer, paktype *pak)
{
    paktype *newpak;
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: recv_border_to_border() from %s", peer_str(peer, buff));
    /*
     * Pass a copy to the DLSw Core
     */
    newpak = pak_duplicate(pak);
    if (newpak)
        pass_to_core(newpak);

    /*
     * Forward this frame to all group members
     * send_to_group_members() does not consume pak.
     */
    send_to_group_members(peer, pak);
    datagram_done(pak);
}


/*
 * recv_bp_rsp()
 *
 * Border Peer Directed Relay code.
 * -- If the dest peer is this router, pass packet to core.
 * -- Else If this router is a border peer and packet is destined for one of 
 *    my group members, then send the packet to that group member.
 * -- Else If packet is for another group that I can reach through a border
 *    peer, than relay the packet to thhat border peer.
 * -- Else must drop it.
 */

void recv_bp_rsp (peerentry *peer, paktype *pak)
{
    peerentry *p;
    dlx_bp_hdr_t *bp_hdr;
    peer_put_t pm_data;

    /*
     * If dest is this router, pass it to core.
     */
    bp_hdr = (dlx_bp_hdr_t *) pak->datagramstart;
    if (lpeer->ipaddr == bp_hdr->dest_ipaddr) {
        pass_to_core(pak);
    } else if (!lpeer->cap.border) {
        /*
         * If we are not a border peer, we can't do anything with it.
         */
        if (dlsw_peer_debug)
            buginf("\nDLSW: Local not BP: Can't relay BP pak to %i g:%d",
                   bp_hdr->dest_ipaddr, bp_hdr->dest_group);
        datagram_done(pak);
    } else if (lpeer->cap.group == bp_hdr->dest_group) {
        /*
         * If dest is a member of our group, send it to our group member.
         */
        peer = get_conn_peer_by_ip(bp_hdr->dest_ipaddr);
        if (peer) {
            if (dlsw_peer_debug)
                buginf("\nDLSW: relaying pak to member %i in group %d", 
                       bp_hdr->dest_ipaddr, bp_hdr->dest_group);
            bp_fill_pm(pak, &pm_data);
            peer_put_pak(peer, &pm_data);
        } else {
            if (dlsw_peer_debug)
                buginf("\nDLSW: Unknown member: Can't relay BP pak to %i g:%d",
                       bp_hdr->dest_ipaddr, bp_hdr->dest_group);
                buginf("\nDLSW: Can't relay BP pak to %i", bp_hdr->dest_ipaddr);
            datagram_done(pak);
        }
    } else {
        /*
         * Try to reach another border peer in the destination group.
         */
        p = (peerentry *) conn_dlx_peerQ[bp_hdr->dest_group].qhead;
        if ((p) && (p->cap.border)) {
            if (dlsw_peer_debug)
                buginf("\nDLSW: relaying pak to BP %i in group %d", 
                       p->ipaddr, p->cap.group);
            bp_fill_pm(pak, &pm_data);
            peer_put_pak(p, &pm_data);
        } else {
            if (dlsw_peer_debug)
                buginf("\nDLSW: Can't relay BP pak to %i g:%d", 
                       bp_hdr->dest_ipaddr, bp_hdr->dest_group);
            datagram_done(pak);
        }
    }
}


/*
 ***********************************************************************
 * Border Peer Frame Sending Functions
 ***********************************************************************
 */

/*
 * peer_lan_media_check()
 *
 * For direct passthough or fst peers, don't send acket unless
 * lan media is a physical token ring or a cip lan.
 */

static boolean peer_lan_media_check (peerentry *peer, hwidbtype *hwidb)
{
    char buff[DLSW_MAX_PEER_ID];

    if (!is_fast_peer((peerhandle_t) peer))
        return(TRUE);

    if ((hwidb->status & IDB_TR) &&
        !(hwidb->status & IDB_VIRTUAL))
        return(TRUE);

    if (is_cip_lan(hwidb))
        return(TRUE);

    PDEBUG("\nDLSw: not exploring to %s from lan %s",
           peer_str(peer, buff), hwidb->hw_namestring);
    return(FALSE);
}


static boolean bandwidth_to_send_bcast (peerentry *peer)
{
    ulong ii, depth;
    char buff[DLSW_MAX_PEER_ID];

    if (peer->xport_type != DLSW_TCP)
        return(TRUE);

    /*
     * If the peer's tcp queue has exceeded the tcp_explorer_hiwater
     * mark, then do not send broadcast frames. All priority peers
     * must be checked. In any have exceeded the threahold, do
     * not send the broadcast.
     *
     * First check the high priority peer.
     */
    depth = reg_invoke_ip_tcpdriver_oqlen((tcp_encaps *) peer->tcp_wr_t);
    if (depth > peer->tcp_explorer_hiwater) {
        PDEBUG("\nDLSw: TCP pipe congested (%d) - Not sending bcast to %s",
               depth, peer_str(peer, buff));
        return(FALSE);
    }

    if (peer->priority) {
        for (ii = 1; ii <= DLSW_PRIORITY_COUNT; ii++) {
            depth = reg_invoke_ip_tcpdriver_oqlen((tcp_encaps *)
                    peer->priority_peers[ii]->tcp_wr_t);
            if (depth > peer->priority_peers[ii]->tcp_explorer_hiwater) {
                PDEBUG("\nDLSw: TCP pipe congested (%d) - NOT sending bcast to %s",
                       depth, peer_str(peer->priority_peers[ii], buff));
                return(FALSE);
            }
        }
    }
    return(TRUE);
}


static boolean pass_bcast_filter (peerentry *peer,
                                  peer_put_t *pm, paktype *pak)
{
    netbios_header      *nb_hdr;

    if (pm->lan_port && pm->lan_port->swidb &&
	(!peer_lan_media_check(peer, pm->lan_port->swidb->hwptr))) {
	PDEBUG("\nbroadcast filter failed media check");
        return(FALSE);
    }

    /*
     * check for mac address exclusivity filtering
     * if it is a not a group address
     */
    if ((pm->dmac) && (!(pm->dmac[0] & TR_GROUP)) &&
        (!(csm_peer_mac_excl_pass((peerhandle_t)peer, pm->dmac)))) {
	PDEBUG("\nbroadcast filter failed mac check");
        return(FALSE);
    }

    /*
     * if netbios and frame types with dname, check
     * for netbios exclusivity filter.
     */
    if ((pm->dsap == 0xF0) || (pm->ssap == 0xF0)) {
        nb_hdr = (netbios_header *) pak->info_start;
        switch (nb_hdr->command) {
	  case NETBIOS_DATAGRAM:
          case NETBIOS_NAME_QUERY:
          case NETBIOS_NAME_RECOGNIZED:
          case NETBIOS_STATUS_QUERY:
            if (!(csm_peer_nb_excl_pass((peerhandle_t)peer,
                                nb_hdr->destname))) {
		PDEBUG("\nbroadcast filter failed exclusivity check");
                return(FALSE);
            }
        }
    }
    return(TRUE);
}


static core2peer_ret_t do_grouped_bcast (peer_put_t *pm, paktype *opak)
{
    ulong i;
    peerentry *peer;
    core2peer_ret_t rc = PEER_NO_CONNECTIONS;
    core2peer_ret_t temp_rc;
    char buff[DLSW_MAX_PEER_ID];

    /*
     * Send to all cisco no-pod peers in a group.
     */
    for (i = 1; i < DLSW_MAX_PGROUP; i++) {
        peer = (peerentry *) conn_dlx_peerQ[i].qhead;
        for (; peer; peer = peer->next) {
            if (peer->peer_type == DEMAND_PEER)
                continue;

            if (!pass_bcast_filter(peer, pm, opak))
                continue;

            if (bandwidth_to_send_bcast(peer)) {
                PDEBUG("\nDLSw: peer bcast() to %s", peer_str(peer, buff));
                pm->pak = pak_duplicate(opak);
                if (!pm->pak)
                    return(rc);

                temp_rc = peer_put_pak(peer, pm);
                if ((temp_rc == PEER_SUCCESS) ||
                    (temp_rc == PEER_SUCCESS_AND_BUSY))
                    rc = PEER_SUCCESS;
                else if (rc != PEER_SUCCESS)
                    rc = temp_rc;
            }
        }
    }
    return(rc);
}


static core2peer_ret_t do_non_grouped_bcast (peer_put_t *pm, paktype *opak)
{
    peerentry *peer;
    core2peer_ret_t rc = PEER_NO_CONNECTIONS;
    core2peer_ret_t temp_rc;
    ssp_hdr_t *ssp_hdr;
    char buff[DLSW_MAX_PEER_ID];

    /*
     * Send to cisco peers not in a group (group 0).
     */
    peer = (peerentry *) conn_dlx_peerQ[0].qhead;
    for (; peer; peer = peer->next) {
        if (!pass_bcast_filter(peer, pm, opak))
            continue;

        if (bandwidth_to_send_bcast(peer)) {
            pm->pak = pak_duplicate(opak);
            if (!pm->pak)
                return(rc);

            temp_rc = peer_put_pak(peer, pm);
            if ((temp_rc == PEER_SUCCESS) ||
                (temp_rc == PEER_SUCCESS_AND_BUSY))
                rc = PEER_SUCCESS;
            else if (rc != PEER_SUCCESS)
                rc = temp_rc;
        }
    }
    /*
     * Send to non-cisco peers.
     */
    peer = (peerentry *) conn_dlsw_peerQ.qhead;
    for (; peer; peer = peer->next) {
        if (!pass_bcast_filter(peer, pm, opak))
            continue;

        if (bandwidth_to_send_bcast(peer)) {
            pm->pak = pak_duplicate(opak);
            if (!pm->pak)
                return(rc);

            /*
             * Must convert ssp lf field to be 1795 compliant.
             */
            ssp_hdr = (ssp_hdr_t *) pm->pak->datagramstart;
            if (((ssp_hdr->ssp_flags & SSP_FLAGS_EX) &&
                 (ssp_hdr->ssp_msg_type == SSP_OP_canureach)) ||
                (ssp_hdr->ssp_msg_type == SSP_OP_netbios_nq) ||
                (ssp_hdr->ssp_msg_type == SSP_OP_netbios_anq) ||
                (ssp_hdr->ssp_msg_type == SSP_OP_dataframe)) {
                ssp_hdr->ssp_lf = cisco_to_1795_lf_bits(ssp_hdr->ssp_lf);
                ssp_hdr->ssp_lf |= SSP_LF_IGNORE_BIT;
            }

            temp_rc = peer_put_pak(peer, pm);
            if ((temp_rc == PEER_SUCCESS) ||
                (temp_rc == PEER_SUCCESS_AND_BUSY))
                rc = PEER_SUCCESS;
            else if (rc != PEER_SUCCESS)
                rc = temp_rc;
        }
    }

    /*
     * Try to send bcast to disconnected Dynamic peers.
     * Bcasts that pass filter test will trigger opening of peer.
     */
    peer = (peerentry *) disc_peerQ.qhead;
    for (; peer; peer = peer->next) {
        if (peer->peer_type != DYNAMIC_PEER)
            continue;

        if (!pass_bcast_filter(peer, pm, opak))
            continue;

        PDEBUG("\nDLSw: attempting peer_put_bcast() to dynamic %s",
               peer_str(peer, buff));
        pm->pak = pak_duplicate(opak);
        if (pm->pak)
            peer_put_pak(peer, pm); /* No need to check return code */
    }
    return(rc);
}


static core2peer_ret_t do_bcast_to_border (peer_put_t *pm, paktype *opak)
{
    dlx_bp_hdr_t *bp_hdr;
    core2peer_ret_t rc = PEER_NO_CONNECTIONS;
    char buff[DLSW_MAX_PEER_ID];

    /*
     * Send to our BP.
     */
    if (bandwidth_to_send_bcast(lpeer->border_peer)) {
        if (dlsw_peer_debug)
            buginf("\nDLSw: sending bcast to BP %s",
                   peer_str(lpeer->border_peer, buff));
        pm->pak = pak_duplicate(opak);
        if (!pm->pak)
            return(rc);

        pm->pak->datagramstart -= sizeof(dlx_bp_hdr_t);
        pm->pak->datagramsize += sizeof(dlx_bp_hdr_t);
        bp_hdr = (dlx_bp_hdr_t *) pm->pak->datagramstart;
        bp_hdr->proto_ver_id = DLX_PROTO_VERSION;
        bp_hdr->message_type = DLX_MEMBER_TO_BP;
        bp_hdr->packet_length = pm->pak->datagramsize;
        bp_hdr->orig_ipaddr = lpeer->ipaddr;
        bp_hdr->dest_ipaddr = 0;
        bp_hdr->orig_group = lpeer->cap.group;
        bp_hdr->dest_group = 0;
        if (lpeer->pod_defaults.xport_type == DLSW_FST)
            bp_hdr->flags = BP_FLAG_POD_FST;
        else
            bp_hdr->flags = 0;
        rc = peer_put_pak(lpeer->border_peer, pm);

        /*
         * If the send to our bp failed, send to all grouped
         * non-pod peers.
         */
        if ((rc != PEER_SUCCESS) && (rc != PEER_SUCCESS_AND_BUSY))
             rc = do_grouped_bcast(pm, opak);
        else
             rc = PEER_SUCCESS;
    } else {
        rc = PEER_BUSY;
    }
    return(rc);
}


static core2peer_ret_t do_border_bcast (peer_put_t *pm, paktype *opak)
{
    ulong i;
    peerentry *peer;
    dlx_bp_hdr_t *bp_hdr;
    core2peer_ret_t rc = PEER_NO_CONNECTIONS;
    core2peer_ret_t temp_rc;
    char buff[DLSW_MAX_PEER_ID];

    /*
     * We are a border peer. Perform the broadcast.
     * Send to our group members and send to 1 border
     * peer in every other reachable group.
     */
    for (i = 1; i < DLSW_MAX_PGROUP; i++) {
        peer = (peerentry *) conn_dlx_peerQ[i].qhead;
        for (; peer; peer = peer->next) {
            if ((i == lpeer->cap.group) || (peer->cap.border)) {
                /*
                 * The peer is in our group or it's
                 * a border peer from a diff group.
                 */
                if (i == lpeer->cap.group) {
                    /*
                     * check for peer broadcast filters,
                     * since packet will not be forwarded
                     * by destination peer
                     */
                    if (!pass_bcast_filter(peer, pm, opak))
                        continue;
                }

                if (bandwidth_to_send_bcast(peer)) {
                    if (i == lpeer->cap.group) {
                        /*
                         * Send directly to peer.
                         */
                        PDEBUG("\nDLSw: peer bcast() to %s",
                               peer_str(peer, buff));
                        pm->pak = pak_duplicate(opak);
                        if (!pm->pak)
                            return(rc);

                        temp_rc = peer_put_pak(peer, pm);
                        if ((temp_rc == PEER_SUCCESS) ||
                            (temp_rc == PEER_SUCCESS_AND_BUSY))
                             rc = PEER_SUCCESS;
                        else if (rc != PEER_SUCCESS)
                             rc = temp_rc;
                    } else {
                        PDEBUG("\nDLSw: peer bcast to BP %s",
                               peer_str(peer, buff));
                        pm->pak = pak_duplicate(opak);
                        if (!pm->pak)
                            return(rc);

                        pm->pak->datagramstart -= sizeof(dlx_bp_hdr_t);
                        pm->pak->datagramsize += sizeof(dlx_bp_hdr_t);
                        bp_hdr = (dlx_bp_hdr_t *) pm->pak->datagramstart;
                        bp_hdr->proto_ver_id = DLX_PROTO_VERSION;
                        bp_hdr->message_type = DLX_BP_TO_BP;
                        bp_hdr->packet_length = pm->pak->datagramsize;
                        bp_hdr->orig_ipaddr = lpeer->ipaddr;
                        bp_hdr->dest_ipaddr = 0;
                        bp_hdr->orig_group = lpeer->cap.group;
                        bp_hdr->dest_group = 0;
                        if (lpeer->pod_defaults.xport_type == DLSW_FST)
                            bp_hdr->flags = BP_FLAG_POD_FST;
                        else
                            bp_hdr->flags = 0;
                        temp_rc = peer_put_pak(peer, pm);
                        if ((temp_rc == PEER_SUCCESS) ||
                            (temp_rc == PEER_SUCCESS_AND_BUSY))
                             rc = PEER_SUCCESS;
                        else if (rc != PEER_SUCCESS)
                             rc = temp_rc;
                        /*
                         * Only send to 1 bp in other groups.
                         */
                        if (rc == PEER_SUCCESS)
                            break;
                    }
                }
            } else {
              /*
               * Peer not in our group and no border peer known
               * for this group. Break peer loop and move on to
               * next group.
               */
              break;
            }
        }
    }
    return(rc);
}


/*
 * do_bcast()
 *
 * Performs filtering/prioritization as appropraite through peer_put_pak().
 * Consumes pak.
 *
 * Do not print error messages on low memory conditions when
 * sending explorers.
 *
 * If we ever implement DLX, check pm->flags for PEER_DLX_TYPE.
 */

core2peer_ret_t do_bcast (peer_put_t *pm)
{
    paktype *origpak;
    core2peer_ret_t rc;
    core2peer_ret_t temp_rc;

    origpak = pm->pak;
    pm->pak = NULL;
    /*
     * Send to non-cisco peers and non-grouped cisco peers.
     */
    rc = do_non_grouped_bcast(pm, origpak);
    pm->pak = NULL;

    if (lpeer->cap.border) {
        /*
         * Local peer is a border peer. Send to our group
         * members and 1 border peer in every other reachable group.
         */
        temp_rc = do_border_bcast(pm, origpak);
    } else if (lpeer->border_peer) {
        /*
         * Local peer knows of a border peer in his group.
         * Send solely to the border peer.
         */
        temp_rc = do_bcast_to_border(pm, origpak);
    } else {
        /*
         * Local peer is not a border peer and does not
         * know of a border peer in his group.
         */
        temp_rc = do_grouped_bcast(pm, origpak);
    }

    /*
     * The above functions will not return PEER_SUCCESS_AND_BUSY.
     */
    if (rc != PEER_SUCCESS)
        rc = temp_rc;

    datagram_done(origpak);
    pm->pak = NULL;
    return(rc);
}


/*
 * pass_to_core()
 */

static void pass_to_core (paktype *pak)
{
    peerentry *peer;
    dlx_bp_hdr_t *bp_hdr;

    bp_hdr =  (dlx_bp_hdr_t *) pak->datagramstart;
    pak->datagramstart += sizeof(dlx_bp_hdr_t);
    pak->datagramsize -= sizeof(dlx_bp_hdr_t);

    /*
     * If the originating peer is a peer-on-demand, the core doesn't 
     * need to know that this frame was sent through border peers.
     */
    peer = get_pod_peer_by_ip(bp_hdr->orig_ipaddr);
    if (!peer) {
        /*
         * Accept DLX_RELAY_RSP from unknown peers because we
         * initiated the connection (we sent the firest frame and this
         * is the response - initialting peer does not have to be
         * promiscuous.
         */
        if ((lpeer->cap.pod_enabled) || 
            (bp_hdr->message_type == DLX_RELAY_RSP)) {
            /*
             * make sure peer's pod transport and the local
             * pod transport match.
             */
            if (((lpeer->pod_defaults.xport_type == DLSW_TCP) &&
                 (bp_hdr->flags & BP_FLAG_POD_FST)) ||
                ((lpeer->pod_defaults.xport_type == DLSW_FST) &&
                 (!(bp_hdr->flags & BP_FLAG_POD_FST)))) {
                if (dlsw_peer_debug)
                    buginf("\nDLSw: pod transport mismatch with peer %i",
                           bp_hdr->orig_ipaddr);
                datagram_done(pak);
                return;
            }

            /*
             * create a pod peer to shelter the core/csm from border peers.
             */
            if (dlsw_peer_debug)
                buginf("\nDLSW: creating a peer-on-demand for %i",
                       bp_hdr->orig_ipaddr);

            peer = new_remote_peer(0, bp_hdr->orig_ipaddr, DEMAND_PEER,
                               lpeer->pod_defaults.ple,
                               lpeer->pod_defaults.xport_type,
                               lpeer->pod_defaults.local_ack,
                               lpeer->pod_defaults.priority,
                               lpeer->pod_defaults.cost,
                               lpeer->pod_defaults.largest_frame,
                               lpeer->pod_defaults.keepalive_int,
                               lpeer->pod_defaults.dlsw_lsap_out,
                               lpeer->pod_defaults.netbios_h_oacf,
                               lpeer->pod_defaults.netbios_b_oacf,
                               lpeer->pod_defaults.max_tcp_qlen,
                               NULL, NULL, NULL, 0, -1, -1, 0, 
			       lpeer->pod_defaults.dlsw_dmac_outlist,
			       lpeer->pod_defaults.dlsw_dmac_out, NULL,
                               NORMAL_MODE);

            if (!peer) {
                datagram_done(pak);
                return;
            }
            peer->cap.group = bp_hdr->orig_group;
            if (lpeer->pod_defaults.keepalive_int != -1) {
                peer->keepalive_int = lpeer->pod_defaults.keepalive_int;
                peer->took_def_keep = FALSE;
            }
        } else {
            if (dlsw_peer_debug)
                buginf("\nDLSw: not promiscuous, core will not see pak from %i",
                       bp_hdr->orig_ipaddr);
            datagram_done(pak);
            return;
        }
    } else if (peer->peer_type == DEMAND_PEER) {
        /*
         * make sure previously created peer's transport and the remote
         * peer's transport match. 
         */
        if (((peer->xport_type == DLSW_TCP) &&
             (bp_hdr->flags & BP_FLAG_POD_FST)) ||
            ((peer->xport_type == DLSW_FST) &&
             (!(bp_hdr->flags & BP_FLAG_POD_FST)))) {
            if (dlsw_peer_debug)
                buginf("\nDLSw: transport mismatch with peer %i",
                       bp_hdr->orig_ipaddr);
            datagram_done(pak);
            return;
        }
    }
    PDEBUG("\nDLSw: passing pak to core originally from %i in group %d",
           bp_hdr->orig_ipaddr, bp_hdr->orig_group);
    peer_input(peer, pak, TRUE);
}


/*
 * send_to_group_members()
 *
 * Does not consume pak.
 */

static void send_to_group_members (peerentry *peer, paktype *pak)
{
    dlx_bp_hdr_t *bp_hdr;
    peerentry *p;
    peer_put_t pm_data;

    bp_hdr =  (dlx_bp_hdr_t *) pak->datagramstart;
    bp_hdr->message_type = DLX_BP_TO_MEMBER;
    bp_fill_pm(pak, &pm_data);
    if ((p= (peerentry *) conn_dlx_peerQ[lpeer->cap.group].qhead)) {
        for (; p; p= p->next) {
            if (p != peer) {
		if (pass_bcast_filter(p, &pm_data, pak)) {
		    pm_data.pak = pak_duplicate(pak);
		    if (!pm_data.pak) {
			errmsg(PEER_NOMEM, "send to group members");
			continue;
		    }
		    PDEBUG("\nsend_to_group_members(): copy to peer %i",
			p->ipaddr);
		    peer_put_pak(p, &pm_data);
		    pm_data.pak = pak;
		}
            }
        }
    }
}


/*
 * send_to_other_groups()
 *
 * Does not consume pak.
 */

static void send_to_other_groups (paktype *pak)
{
    register ulong i;
    dlx_bp_hdr_t *bp_hdr;
    peerentry *peer;
    peer_put_t pm_data;

    /*
     * Forward this frame to border peers of other groups.
     * If this router is not aware of a border peer for a
     * group, then individually send to each peer in that group.
     */
    bp_hdr = (dlx_bp_hdr_t *) pak->datagramstart;
    bp_hdr->message_type = DLX_BP_TO_BP;
    bp_fill_pm(pak, &pm_data);
    for (i = 0; i < DLSW_MAX_PGROUP; i++) {
        if ((i != lpeer->cap.group) &&
	    (peer = (peerentry *) conn_dlx_peerQ[i].qhead) &&
	    (peer->cap.border)) {
	     pm_data.pak = pak_duplicate(pak);
	     if (!pm_data.pak) {
	         errmsg(PEER_NOMEM, "send to other groups");
		 continue;
	     }
	     PDEBUG("\nsend_to_other_groups(): copy to peer %i",
	            peer->ipaddr);
	     peer_put_pak(peer, &pm_data);
	     pm_data.pak = pak;
        }
    }
}


/*
 * send_peer_group()
 *
 * Does not consume pak.
 */

core2peer_ret_t send_peer_group (peer_msg_u *msg)
{
    peerentry *peer;
    paktype *origpak = msg->p_put.pak;


    peer = (peerentry *) conn_dlx_peerQ[msg->put_group].qhead;
    if (!peer)
        return(PEER_UNREACHABLE); 

    for (; peer; peer = peer->next) {
        msg->p_put.pak = pak_duplicate(origpak);
        if (msg->p_put.pak)
            peer_put_pak(peer, &msg->p_put);
    }
    return(PEER_SUCCESS); 
}


/*
 * send_bp_resp()
 *
 * Consumes the pak.
 */

core2peer_ret_t send_bp_resp (peer_msg_u *msg)
{
    dlx_bp_hdr_t *bp_hdr;
    peerentry *peer;
    core2peer_ret_t rc;

    peer = (peerentry *) msg->put_handle;
    msg->p_put.pak->datagramstart -= sizeof(dlx_bp_hdr_t);
    msg->p_put.pak->datagramsize += sizeof(dlx_bp_hdr_t);
    bp_hdr =  (dlx_bp_hdr_t *) msg->p_put.pak->datagramstart;
    bp_hdr->proto_ver_id = DLX_PROTO_VERSION;
    bp_hdr->message_type = DLX_RELAY_RSP;
    bp_hdr->packet_length = msg->p_put.pak->datagramsize;
    bp_hdr->orig_group = lpeer->cap.group;
    bp_hdr->dest_group = peer->cap.group;
    bp_hdr->orig_ipaddr = lpeer->ipaddr;
    bp_hdr->dest_ipaddr = peer->ipaddr;
    if (lpeer->pod_defaults.xport_type == DLSW_FST)
        bp_hdr->flags = BP_FLAG_POD_FST;
    else
        bp_hdr->flags = 0;

    /*
     * If this group member is not a border peer and it
     * knows of a border peer for its group, then send
     * the message to its border peer.
     */
    if (!lpeer->cap.border) {
        if (lpeer->border_peer) {
            rc = peer_put_pak(lpeer->border_peer, &msg->p_put);
            msg->p_put.pak = NULL;
            return(rc);
        } else {
            /*
             * Nothing a group memeber with no border-peer can do
             * with this frame.
             */
            datagram_done(msg->p_put.pak);
            msg->p_put.pak = NULL;
	    return(PEER_UNREACHABLE); 
        }
    }

    /*
     * We ARE border peer capable.
     * Attempt to send to the border peer in the destination group.
     */
    peer = (peerentry *) conn_dlx_peerQ[bp_hdr->dest_group].qhead;
    if ((peer) && (peer->cap.border)) {
        rc = peer_put_pak(peer, &msg->p_put);
        msg->p_put.pak = NULL;
        return(rc);
    } else {
        datagram_done(msg->p_put.pak);
        msg->p_put.pak = NULL;
	return(PEER_UNREACHABLE); 
    }
}



