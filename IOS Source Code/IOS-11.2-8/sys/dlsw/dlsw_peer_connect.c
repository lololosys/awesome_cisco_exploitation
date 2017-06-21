/* $Id: dlsw_peer_connect.c,v 3.16.6.4 1996/08/28 12:41:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_connect.c,v $
 *------------------------------------------------------------------
 * DLSw Peer Background Processing
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Responsible for creating, maintaining, and destroying peer
 * connections. 
 *------------------------------------------------------------------
 * $Log: dlsw_peer_connect.c,v $
 * Revision 3.16.6.4  1996/08/28  12:41:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.16.6.3  1996/06/08  14:58:55  fbordona
 * CSCdi58842:  System restarted by bus error at PC 0xD0D0D0D, addr 0x0
 *              Do not clean up peer if blocked on TCP open.
 *              keepalive_count not initialized properly causing false
 *              keepalive failure.
 * Branch: California_branch
 *
 * Revision 3.16.6.2  1996/05/17  10:46:10  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.12.2.6  1996/04/26  07:34:35  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.12.2.5  1996/04/25  19:37:45  ravip
 * CSCdi54131:  DLSw MIB ciscoDlswTConnOper Table returns 0 for stats
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.4  1996/04/24  18:35:30  fbordona
 * CSCdi55588:  DLSw: Use managed timers for connecting dynamic peers
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.3  1996/04/20  02:08:16  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.2  1996/04/03  14:01:47  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.12.2.1  1996/03/17  17:38:12  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.16.6.1  1996/03/18  19:32:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.3  1996/03/16  06:39:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.12.2  1996/03/07  08:44:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.12.1  1996/02/20  00:46:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.16  1996/03/05  18:49:30  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.15  1996/03/01  17:57:42  kmoberg
 * CSCdi47312:  Router crashes testing dlsw direct/hdlc/fast
 *
 * Revision 3.14  1996/02/28  21:37:25  fbordona
 * CSCdi50155:  DLSw crash when priority peer is reloaded
 *
 * Revision 3.13  1996/02/27  21:36:27  fbordona
 * CSCdi49949:  DLSw peer-on-demand peers ignore tcp-queue-max.
 *              Improve "show dlsw peers" output.
 *
 * Revision 3.12  1996/02/20  22:08:14  fbordona
 * CSCdi47801:  SEgV exception, PC0x60637DFC tcpdriver_fhost.
 *              Use the tcpencaps handle now passed in the tcp
 *              closef vector (CSCdi48380) to determine if the
 *              DLSw read or write pipe has been closed.
 *
 * Revision 3.11  1996/02/07  19:09:21  fbordona
 * CSCdi46958:  Router crashes when DLSw peer is reloaded.
 *
 * Revision 3.10  1996/02/01  06:01:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/02/01  02:57:05  fbordona
 * CSCdi47930:  DLSw session does not reconnect after peer powered off.
 *              Only attempt one peer connection at at time to avoid
 *              starving the dlsw background process.
 *
 * Revision 3.8  1996/01/04  21:39:39  fbordona
 * CSCdi46273:  DLSw peers cycle CONNECT to DISC after shut no shut on
 *              serial.
 *
 * Revision 3.7  1995/12/11  14:50:17  fbordona
 * CSCdi45349:  Clean up DLSw+ Peer timer debug for sys_timestamp change
 *
 * Revision 3.6  1995/11/30  21:22:53  fbordona
 * CSCdi44759:  DLSw peer-on-demand peers prematurely disconnect.
 *              Fix peer-on-demand crash introduced in 11.1
 *
 * Revision 3.5  1995/11/29  22:06:31  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.4  1995/11/20  23:25:51  fbordona
 * CSCdi42215:  dlsw backup peers broken
 *
 * Revision 3.3  1995/11/17  09:03:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:24:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:55:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/07/26  19:12:19  hampton
 * Convert DLSW to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37708]
 *
 * Revision 2.3  1995/07/24 13:05:35  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.2  1995/06/28  18:50:57  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.1  1995/06/07  20:27:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
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
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_cap_exchg.h"
#include "../dlsw/dlsw_debug.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/dlsw_csm_externs.h"


/*
 * File Scope variables.
 */
boolean csm_filter_check;

/*
 * File scope function prototypes.
 */
static void peer_remover(void);
static boolean send_keepalive(peerentry *peer, paktype *, uchar);
static void peer_act_on_capabilities (void);
static void peer_process_queued_input (void);
static boolean dlsw_peerBLOCK (void);

/*
 * Externs.
 */
extern mgd_timer dlsw_peer_master_timer;
extern mgd_timer dlsw_peer_conn_timer;
extern mgd_timer dlsw_peer_keep_timer;

/*
 * peer_next_conn_interval()
 *
 * Implements a "slow-down like" tcp connect algorithm for the next time
 * an attttempt should be made to establish a  tcp connection.
 */
ulong peer_next_conn_interval (peerentry *peer)
{
    if (peer->conn_retries > TCP_RETRY_THOLD_3)
        return(20 * DLSW_DEF_CONNECT_INT); /* conn every 5 minutes */
    else if (peer->conn_retries > TCP_RETRY_THOLD_2)
        return(4 * DLSW_DEF_CONNECT_INT);  /* conn every 1 minute */
    else if (peer->conn_retries > TCP_RETRY_THOLD_1)
        return(2 * DLSW_DEF_CONNECT_INT);  /* conn every 30 seconds */
    else
        return(DLSW_DEF_CONNECT_INT);        /* conn every 15 seconds */
}


/*
 * dlsw_peer_background()
 *
 * Called every 1 second by dlsw_background() in dlsw_core.c
 */
void dlsw_peer_background (void)
{
    mgd_timer *expired_timer;
    sys_timestamp time_spent;
    peerentry *peer;
    char buff[DLSW_MAX_PEER_ID];

    GET_TIMESTAMP(time_spent);
    dlsw_fct_house_keeping();

    while (mgd_timer_expired(&dlsw_peer_master_timer)) {
        expired_timer = mgd_timer_first_expired(&dlsw_peer_master_timer);
        peer = mgd_timer_context(expired_timer);
        switch (mgd_timer_type(expired_timer)) {
          case PEER_CONN_TIMER :
            mgd_timer_stop(&peer->conn_timer);
            peer->conn_retries++;
            dlsw_peer_fsm(peer, OPEN_CONNECTION);
            peer->connect_dynam = FALSE;
            break;

          case PEER_KEEP_TIMER :
            if (peer->keepalive_count >= DLSW_KEEP_CNT_THOLD) {
                PDEBUG("\nDLSw: keepalive failure for %s",
                       peer_str(peer, buff));
                mgd_timer_stop(&peer->keep_timer);
                dlsw_peer_fsm(peer, CLOSE_CONNECTION);
            } else  {
                send_keepalive(peer, NULL, DLX_PEER_TEST_REQ);
                peer->keepalive_count++;
                mgd_timer_start(&peer->keep_timer, peer->keepalive_int);
            }
            break;

          default :
            mgd_timer_stop(expired_timer);
            break;
        }
        if (ELAPSED_TIME(time_spent) > ONESEC) {
              break;
          }
    }

    peer_remover();
}

static boolean dlsw_peerBLOCK ()
{
    if (wait_conn_peerQ.qhead || 
        direct_inputQ.qhead || 
        dlsw_peer_llc2Q.qhead) {
        return FALSE;
    }
    return TRUE;
}
 

forktype dlsw_peer_process (void)
{
    ulong count;
    paktype *pak;

    edisms(systeminitBLOCK, 0);
 
    while (TRUE) {
        if (wait_conn_peerQ.qhead)
            peer_act_on_capabilities();

        if (direct_inputQ.qhead)
            peer_process_queued_input();

        if (dlsw_peer_llc2Q.qhead) {
            count = PAK_SWITCHCOUNT;
            while (TRUE) {
                if (--count == 0)
                  break;

                pak = p_dequeue(&dlsw_peer_llc2Q);
                if (pak == NULL )
                    break;

                peer_input(pak->peer_ptr, pak, FALSE);
            }
        }

        edisms((blockproc *)dlsw_peerBLOCK, 0);
    }
}


/*
 * peer_process_queued_input()
 *
 * Direct encaps paks are received at interrupt level. The
 * core must process paks at process level and the peer manager 
 * needs to malloc structs for promiscuous peers at process level.
 * Process direct input that is not fastswitched in the background.
 */

static void peer_process_queued_input ()
{
    paktype *pak;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);

    while((pak = p_dequeue(&direct_inputQ)))
        receive_direct_input(pak);

    reset_interrupt_level(status);
}


/*
 * peer_act_on_capabilities()
 */

static void peer_act_on_capabilities ()
{
    char buff[DLSW_MAX_PEER_ID];
    peerentry *peer;
    peerentry *next;
    queuetype *new_qptr;
    border_peer_ret_t bp_rc;
    ulong ii;

    peer = (peerentry *) wait_conn_peerQ.qhead;
    while (peer) {
        next = peer->next;
        PDEBUG("\nDLSw: peer_act_on_capabilities() for %s",
               peer_str(peer, buff));
        if (peer->xport_type == DLSW_TCP) {
            if ((lpeer->cap.num_tcp_sessions == CISCO_NUM_TCP_PIPES) &&
                (peer->cap.num_tcp_sessions == CISCO_NUM_TCP_PIPES)) {
                if (lpeer->ipaddr > peer->ipaddr) {
                    dlsw_peer_fsm(peer, CLOSE_READ_PIPE);
                } else if (peer->tcp_rd_t && peer->tcp_wr_t) {
                    /*
                     * We have the lower ip address and our partner has
                     * not closed our write pipe. Set flag so peer is not
                     * used until this event occurs.
                     */
                    peer->close_write_pending = TRUE;
                }
            }

            if ((peer->priority) && (peer->cap.priority)) {
                if (lpeer->ipaddr > peer->ipaddr) {
                    if (!connect_priority_peers(peer)) {
                        PDEBUG("\npriority connect %s failed", 
                               peer_str(peer, buff));
                        return;
                    }
                }
            } else if (peer->priority) {
                errmsg(PEER_BADCAP, "priority set on local-peer - not set on",
                       peer_str(peer, buff));
            } else if (peer->cap.priority) {
                errmsg(PEER_BADCAP, "priority not set on local-peer - set on",
                       peer_str(peer, buff));
            }
        }

        if (lpeer->cap.spoof_and_seg != peer->cap.spoof_and_seg) {
            if (lpeer->cap.spoof_and_seg)
               errmsg(PEER_BADCAP, "biu-segment configured locally - not configured on",
                      peer_str(peer, buff));
            else
               errmsg(PEER_BADCAP, "biu-segment not configured locally - configured on",
                      peer_str(peer, buff));
        }

        bp_rc = NOT_BP;
        if (!bcmp(lpeer->cap.vendor_id,peer->cap.vendor_id,SNAP_OUIBYTES)) {
            new_qptr = &conn_dlx_peerQ[peer->cap.group];
            if ((lpeer->cap.group) && (peer->cap.border) &&
                (peer->peer_type != DEMAND_PEER))
                bp_rc = add_border_peer_info(peer);
        } else
            new_qptr = &conn_dlsw_peerQ;

        unqueue(peer->qptr, peer);
        peer->qptr = new_qptr;
        if (bp_rc == BEST_BP)
            requeue(peer->qptr, peer);
        else if (bp_rc == BACKUP_BP)
            insqueue (peer->qptr, peer, conn_dlx_peerQ[peer->cap.group].qhead);
        else
            enqueue(peer->qptr, peer);
        peer->peer_status = PEER_READY;
        GET_TIMESTAMP(peer->uptime);
        GET_TIMESTAMP(peer->zero_ckt_time);
        if ((peer->keepalive_int) &&
            (peer_get_vendor_type((peerhandle_t) peer) == PEER_CISCO)) {
            mgd_timer_start(&peer->keep_timer, peer->keepalive_int);
        }
        if (peer->priority) {
            for (ii = 1; ii <= DLSW_PRIORITY_COUNT; ii++) {
                if (peer->priority_peers[ii])
                    peer->priority_peers[ii]->peer_status = PEER_READY;
            }
        }
        peer = next;
    }
}


/*
 * send_keepalive()
 */

static boolean send_keepalive (peerentry *peer, paktype *pak, uchar op)
{
    dlx_header_t *dlx_hdr;
    char buff[DLSW_MAX_PEER_ID];

    if (!pak) {
        pak = get_dlx_packet(op, sizeof(dlx_header_t), 0, 0);
        if (!pak)
            return(peer->keepalive_int);
    } else {
        dlx_hdr = (dlx_header_t *) pak->datagramstart;
        dlx_hdr->message_type  = op;
    }
    if ((*peer->sender)(peer,pak,TRUE,PEER_CONN_ADMIN,NULL)==PEER_SUCCESS) {
        PDEBUG("\nDLSw: Keepalive %s sent to %s)",  
                   (op == DLX_PEER_TEST_REQ) ? "Request" : "Response",
                    peer_str(peer, buff));
        return(TRUE);
    } else {
        PDEBUG("\nDLSw: Keepalive %s send failed to %s",
                   (op == DLX_PEER_TEST_REQ) ? "Request" : "Response",
                    peer_str(peer, buff));
        return(FALSE);
    }
}


/*
 * peer_remover()
 */

static void peer_remover ()
{
    peerentry *peer;
    peerentry *next;
    ulong i;
    leveltype status;
    char buff[DLSW_MAX_PEER_ID];

    status = raise_interrupt_level(ALL_DISABLE);

    /*
     * Clean up disconnected peers and clean up
     * peer-on-demand peers that did not connect.
     */
    peer = (peerentry *) disc_peerQ.qhead;
    while (peer) {
        next = peer->next;
        if (peer->remove) {
            peer_nuke(peer);
        } else if (peer->peer_type == DEMAND_PEER) {
            if (((peer->conn_state == PEER_DISC) ||
                 (peer->conn_state == PEER_WAIT_CAP)) &&
                (peer->active_open_wait == FALSE) &&
                (CLOCK_OUTSIDE_INTERVAL(peer->zero_ckt_time,
                                        DLSW_DEF_CONNECT_INT))) {
                /*
                 * Clean up any disconnected and inactive pods.
                 * These pods are a result of sending an ICR, but the
                 * origin peer did not chose this peer for the circuit.
                 * For FST/Direct peers in PEER_WAIT_CAP state, there 
                 * was a capabilities exhange failure.
                 */
                PDEBUG("\nDLSw: removing unused pod %i", peer->ipaddr);
                dlsw_peer_fsm(peer, CLOSE_CONNECTION);
            }
        } else if (((peer->conn_state == PEER_WAIT_READ) ||
              (peer->conn_state == PEER_WAIT_CAP) ||
              ((peer->conn_state == PEER_DISC) &&
               (peer->peer_type == PROM_PEER))) &&
             (peer->active_open_wait == FALSE) &&
             (CLOCK_OUTSIDE_INTERVAL(peer->conn_start_time, DLSW_CONN_WINDOW))) {
            /*
             * Clean up any connections that did not completely open
             * in the last connection window. Prom FST/Direct peers get 
             * created when a packet arrives. If the connect fails and the
             * remote stops retrying the connection, the DISC PROM peers
             * need to be cleanup up in the background. This does not
             * apply to TCP peers since TCP events inform us when the
             * remote peer gives up and close the connection.
             */
            PDEBUG("\nDLSw: %s did not connect", peer_str(peer, buff));
            dlsw_peer_fsm(peer, CLOSE_CONNECTION);
        }
        peer = next;
    }

    /*
     * Mosh through all cisco connected peers to remove inactive pods.
     */
    for (i = 0; i < DLSW_MAX_PGROUP; i++) {
        peer = (peerentry *) conn_dlx_peerQ[i].qhead;
        while (peer) {
            next = peer->next;
            if ((peer->peer_type == DEMAND_PEER) &&
                (peer->num_ckts == 0) &&
                (lpeer->pod_defaults.pod_inactivity != 0) &&
                (CLOCK_OUTSIDE_INTERVAL(peer->zero_ckt_time,
                               lpeer->pod_defaults.pod_inactivity))) {
                PDEBUG("\nDLSw: closing conn for pod %i", peer->ipaddr);
                dlsw_peer_fsm(peer, CLOSE_CONNECTION);
                peer_nuke(peer);
            } else if (peer->peer_type == DYNAMIC_PEER) {
                 /*
                  * If inactivity configured, close dynamic peer
                  * connection regardless of how many ckts it carries.
                  */
                 if ((peer->dynamic_inactivity != -1) &&
                     (CLOCK_OUTSIDE_INTERVAL(peer->last_non_peer_traffic,
                                             peer->dynamic_inactivity))) {
                      PDEBUG("\nDLSw: Inactivity - closing dynamic conn for %i",
                             peer->ipaddr);
                      dlsw_peer_fsm(peer, CLOSE_CONNECTION);
                 } else if ((peer->num_ckts == 0) &&
                    (CLOCK_OUTSIDE_INTERVAL(peer->zero_ckt_time,
                                            peer->no_llc))) {
                    PDEBUG("\nDLSw: closing conn for dynamic peer %i",
                           peer->ipaddr);
                    dlsw_peer_fsm(peer, CLOSE_CONNECTION);
                }
            } else if (peer->primary_peer) {
                 if ((peer->primary_peer->conn_state == PEER_CONN) &&
                     TIMER_RUNNING_AND_AWAKE(peer->linger_timer)) {
                      PDEBUG("\nDLSw: closing backup peer; linger expired");
                      dlsw_peer_fsm(peer, CLOSE_CONNECTION);
                 } else if ((peer->primary_peer->conn_state == PEER_CONN) &&
                            peer->backup_linger == -1) {
                      if ((CLOCK_OUTSIDE_INTERVAL(peer->zero_ckt_time,
                                                  DLSW_DEF_KEEP_INT))
                          && (peer->num_ckts == 0)) {
                           PDEBUG("\nDLSw: closing backup peer %i; no ckts",
                                  peer->ipaddr);
                           dlsw_peer_fsm(peer, CLOSE_CONNECTION);
                      }
                 }
            }
            peer = next;
        }
    }
    reset_interrupt_level(status);
}


/*
 * peer_input()
 */
void peer_input (peerentry *peer, paktype *pak, boolean border_relay)
{
    dlx_header_t *dlx_hdr;
    ssp_hdr_t *ssp_ptr;
    peer_msg_u peer_msg;
    peer_filter_msg_u filter_msg;
    char buff[DLSW_MAX_PEER_ID];

    /*
     * Always process packet like it was received from
     * highest priority peer.
     */
    if (peer->priority_top) {
        peer = peer->priority_top;
    }

    dlx_hdr = (dlx_header_t *) pak->datagramstart;
    if (dlx_hdr->proto_ver_id == DLX_PROTO_VERSION) {
        if ((!border_relay) && (peer->keepalive_int) &&
            (peer_get_vendor_type((peerhandle_t) peer) == PEER_CISCO)) {
            peer->keepalive_count = 0;
            mgd_timer_start(&peer->keep_timer, peer->keepalive_int);
        }
        switch (dlx_hdr->message_type) {
          case DLX_PEER_TEST_REQ:
            if (peer->conn_state == PEER_CONN)
                send_keepalive(peer, pak, DLX_PEER_TEST_RSP);
            break;

          case DLX_PEER_TEST_RSP:
            PDEBUG("\nDLSw: Keepalive Response from %s",
                   peer_str(peer, buff));
            datagram_done(pak);
            break;

          case DLX_MEMBER_TO_BP:
            PDEBUG("\nDLSw: Pak from %s with op DLX_MEMBER_TO_BP",
                   peer_str(peer, buff));
            recv_member_to_border(peer, pak);
            break;

          case DLX_BP_TO_MEMBER:
            PDEBUG("\nDLSw: Pak from %s with op DLX_BP_TO_MEMBER",
                   peer_str(peer, buff));
            recv_border_to_member(peer, pak);
            break;

          case DLX_BP_TO_BP:
            PDEBUG("\nDLSw: Pak from %s with op DLX_BP_TO_BP",
                   peer_str(peer, buff));
            recv_border_to_border(peer, pak);
            break;

          case DLX_RELAY_RSP:
            PDEBUG("\nDLSw: Pak from %s with op DLX_RELAY_RSP",
                   peer_str(peer, buff));
            recv_bp_rsp(peer, pak);
            break;

          default:
            /*
             * Peers are capable of receiving packets before the peer
             * connection process is complete. Specifically, do not pass
             * packets to core/csm until capabilities are acted upon.
             */
            if ((peer->peer_status != PEER_READY) &&
                (peer->peer_type != DEMAND_PEER)) {
                 PDEBUG("\nDLSw: Peer is not connected yet - dropping packet");
                 datagram_done(pak);
                 return;
            }

	    if ((is_dlx_csm_op((dlx_header_t *) pak->datagramstart)) &&
		(csm_filter_check)) {
	        filter_msg.type = PEER_CSM_ACCEPT_CHECK;
	        filter_msg.csm_acpt.csm_header_start = (void *) 
						       pak->datagramstart;
                if (!peer_to_csm_filter(&filter_msg))
		    break;
            }
            GET_TIMESTAMP(peer->last_non_peer_traffic);
            peer_msg.type = PEER_RECV_PAK;
            peer_msg.p_recv.header_type = PEER_DLX_TYPE;
            peer_msg.p_recv.peer_handle = (peerhandle_t) peer;
            peer_msg.p_recv.pak = pak;
            peer_to_core(&peer_msg);
            break;
        }
        return;
    }

    /*
     * Peer Manager processes SSP Capability Exchange messages.
     */
    ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
    if (ssp_ptr->ssp_msg_type == SSP_OP_capability_xchg) {
        peer_cap_exchg_input(peer, pak);
    } else {
        /*
         * Peers are capable of receiving packets before the peer
         * connection process is complete. Specifically, do not pass
         * packets to core/csm until capabilities are acted upon.
         */
        if ((peer->peer_status != PEER_READY) &&
            (peer->peer_type != DEMAND_PEER)) {
             PDEBUG("\nDLSw: Peer is not connected yet - dropping packet");
             datagram_done(pak);
             return;
        }

        if ((!border_relay) && (peer->keepalive_int) &&
            (peer_get_vendor_type((peerhandle_t) peer) == PEER_CISCO)) {
            peer->keepalive_count = 0;
            mgd_timer_start(&peer->keep_timer, peer->keepalive_int);
        }
        peer_update_in_stat(peer, pak);

        /* Some vendors might send standards based transport keepalive */
        /* packet. Do nothing with the packet..yes this is what the    */
        /* standard says.                                              */
        if (ssp_ptr->ssp_msg_type == SSP_OP_keepalive) {
          PDEBUG("\nDLSw: Standards based transport keepalive received from %s", peer_str(peer, buff)); 
          datagram_done(pak);
          return;
        }

	if ((is_ssp_csm_op(((ssp_hdr_t *)pak->datagramstart)->ssp_msg_type)) &&
            (csm_filter_check)) {
            filter_msg.type = PEER_CSM_ACCEPT_CHECK;
            filter_msg.csm_acpt.csm_header_start = (void *) pak->datagramstart;
            if (!peer_to_csm_filter(&filter_msg))
	        return;
        }
        GET_TIMESTAMP(peer->last_non_peer_traffic);
        peer_msg.type = PEER_RECV_PAK;
        peer_msg.p_recv.header_type = PEER_SSP_TYPE;
        peer_msg.p_recv.peer_handle = (peerhandle_t) peer;
        peer_msg.p_recv.pak = pak;
        peer_to_core(&peer_msg);
    }
}

