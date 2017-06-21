/* $Id: dlsw_peer_tcp.c,v 3.14.6.9 1996/09/09 19:09:19 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_tcp.c,v $
 *------------------------------------------------------------------
 * DLSw Peer Transport Routines for TCP connections
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Create, destroy, send, and receive routines for DLSw peer
 * connections using a TCP transport. 
 *------------------------------------------------------------------
 * $Log: dlsw_peer_tcp.c,v $
 * Revision 3.14.6.9  1996/09/09  19:09:19  akhanna
 * CSCdi65724:  Cannot stop keepalives on promiscous peer
 * Branch: California_branch
 *
 * Revision 3.14.6.8  1996/09/03  22:25:17  kpatel
 * CSCdi67883:  DLSw lite requires flow control to throttle back end
 *              station traffic
 * Branch: California_branch
 *
 * Revision 3.14.6.7  1996/08/28  12:41:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.6.6  1996/07/23  13:21:48  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.14.6.5  1996/07/09  18:08:53  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.14.6.4  1996/06/27  15:13:42  fbordona
 * CSCdi61278:  DLSw: System restarted by bus error at PC 0xD0D0D0D
 *              action_b(). Do not free peers if blocked on TCP open.
 *              Connect timer not set properly for priority peers.
 * Branch: California_branch
 *
 * Revision 3.14.6.3  1996/05/23  14:56:51  sbales
 * CSCdi55256:  cant happen is continuously displayed on Router
 *              If state occurs, print debug, close peer.
 * Branch: California_branch
 *
 * Revision 3.14.6.2  1996/05/17  10:46:34  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.12.2.6  1996/04/26  07:34:50  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.12.2.5  1996/04/24  18:35:40  fbordona
 * CSCdi55588:  DLSw: Use managed timers for connecting dynamic peers
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.4  1996/04/23  20:14:02  fbordona
 * CSCdi55437:  DLSw peer may take several minutes to connect after reload
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.3  1996/04/20  02:08:32  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.2  1996/04/03  14:01:58  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.12.2.1  1996/03/17  17:38:19  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.14.6.1  1996/03/18  19:32:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.3  1996/03/16  06:39:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.2  1996/03/07  08:44:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  00:46:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/03/05  18:49:37  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.13  1996/02/28  21:37:31  fbordona
 * CSCdi50155:  DLSw crash when priority peer is reloaded
 *
 * Revision 3.12  1996/02/20  22:08:20  fbordona
 * CSCdi47801:  SEgV exception, PC0x60637DFC tcpdriver_fhost.
 *              Use the tcpencaps handle now passed in the tcp
 *              closef vector (CSCdi48380) to determine if the
 *              DLSw read or write pipe has been closed.
 *
 * Revision 3.11  1996/02/08  18:01:54  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.10  1996/02/07  19:09:27  fbordona
 * CSCdi46958:  Router crashes when DLSw peer is reloaded.
 *
 * Revision 3.9  1996/02/01  06:01:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8  1996/01/18  22:25:03  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/16  03:37:16  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.6  1996/01/04  21:39:45  fbordona
 * CSCdi46273:  DLSw peers cycle CONNECT to DISC after shut no shut on
 *              serial.
 *
 * Revision 3.5  1995/11/29  22:06:40  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.4  1995/11/20  23:26:00  fbordona
 * CSCdi42215:  dlsw backup peers broken
 *
 * Revision 3.3  1995/11/17  09:03:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:24:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:56:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/09  20:18:50  rnaderi
 * CSCdi41641:  Add sub-interface option to the parser for DLSw+/FR
 *
 * Revision 2.4  1995/09/01  15:58:39  ppearce
 * CSCdi39719:  CIP support for DLSw+ (TCP encapsulation only)
 *
 * Revision 2.3  1995/08/16  13:54:54  fbordona
 * CSCdi38827:  DLSw prioritization classifies packets incorrectly
 *
 * Revision 2.2  1995/08/07  05:31:39  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.1  1995/06/07  20:27:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */





#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "../cls/dlc_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "config.h"
#include "mgd_timers.h"
#include "parser.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../tcp/tcp.h"
#include "../if/network.h"
#include "../if/tring.h"
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
#include "../dlsw/dlsw_dlx.h"
#include "../ibm/sna_la_pri.h"
#include "../ibm/sna_util.h"
#include "../ibm/libibm_externs.h"

/*
 * File Scope Function Prototypes
 */
static void dlsw_tcpd_readf (tcp_encaps *, tcbtype *, int, void *);
static void dlsw_tcpd_fini (tcp_encaps *, void *);
static void dlsw_tcpd_flow_ctrl (tcp_encaps *, ulong, void *);
static uchar dlsw_packet_classify (paktype *, peer_put_t *);
static peerentry *dlsw_packet_classify_peer (paktype *, peerentry *, 
                                             peer_put_t *);

/*
 * peer_tcp_init()
 */

void peer_tcp_init ()
{
    reg_add_dlsw_running(dlsw_peer_active,"dlsw_peer_active");
    reg_add_tcp_listen(DLSW_RD_PORT, dlsw_tcpd_listen, "dlsw_tcpd_listen");
    reg_add_tcp_listen(DLSW_P1_PORT, dlsw_tcpd_listen, "dlsw_tcpd_listen");
    reg_add_tcp_listen(DLSW_P2_PORT, dlsw_tcpd_listen, "dlsw_tcpd_listen");
    reg_add_tcp_listen(DLSW_P3_PORT, dlsw_tcpd_listen, "dlsw_tcpd_listen");
    reg_add_tcp_access_check(DLSW_RD_PORT,dlsw_peer_active,"dlsw_peer_active");
    reg_add_tcp_access_check(DLSW_P1_PORT,dlsw_peer_active,"dlsw_peer_active");
    reg_add_tcp_access_check(DLSW_P2_PORT,dlsw_peer_active,"dlsw_peer_active");
    reg_add_tcp_access_check(DLSW_P3_PORT,dlsw_peer_active,"dlsw_peer_active");
    reg_invoke_ip_tcpdriver_start();
}


static void dlsw_readf_error (peerentry *peer)
{
    peer->rd_state = RD_INIT;
    if (peer->pak) {
        datagram_done(peer->pak);
        peer->pak = NULL;
    }
    if (peer->priority_top)
        peer = peer->priority_top;

    dlsw_peer_fsm(peer, CLOSE_CONNECTION);
}

/*
 * dlsw_tcpd_readf()
 *
 */
static void dlsw_tcpd_readf (tcp_encaps *t, tcbtype *tcb, 
                             int thresh, void *cookie)
{
    peerentry *peer;
    int bytes, numbytes;
    int rc;
    uchar *data;
    ulong pak_size;
    dlx_header_t *dlx_hdr;

    peer = (peerentry *) cookie;

    if (peer->rd_state == RD_INIT) {
        /* Start reading a header */
        peer->rd_state = RD_HEADER;
        peer->target = (uchar *) &peer->ssp_start;
        peer->sizeleft = sizeof(ssp_hdr_start_t);
        if (peer->pak) {
            /* This message would indicate a bug, as it should never happen */
            buginf("\nDLSw: readf detected packet lying around");
            datagram_done(peer->pak);
            peer->pak = NULL;
        }

        bytes = tcp_unread(tcb);
        if (bytes < sizeof(ssp_hdr_start_t)) {
            PDEBUG("\nDLSw: readf() false alarm, thresh %d, bytes %d",
                   sizeof(ssp_hdr_start_t), bytes);
            return;
        }

    }

    while (TRUE) {
        rc = tcp_getstring(tcb, &data, &bytes, ~0);
        if (rc == NO_DATA) {
            break;
        } else if (rc != OK) {
            PDEBUG("\nDLSw: tcp_getstring() failed - %i(%d) -> %d",
                   tcb->foreignhost, tcb->foreignport, tcb->localport);
            dlsw_readf_error(peer);
            return;
        }

        while (bytes) {
            numbytes = min(bytes, peer->sizeleft);

            /*
             * Copy the data if a target has been set up.
             */
            if (peer->target) {
                bcopy(data, peer->target, numbytes);
                peer->target += numbytes;
            }

            data += numbytes;
            bytes -= numbytes;
            peer->sizeleft -= numbytes;

            if (peer->sizeleft == 0) {
                switch (peer->rd_state) {
                  case RD_INIT:
                    PDEBUG("\nDLSW: Invalid peer rd_state.  Closing peer.");
                    dlsw_peer_fsm(peer, CLOSE_CONNECTION);
                    return;
 
                  case RD_HEADER:
                    peer->pkts_rx++;
                    dlx_hdr = (dlx_header_t *) &peer->ssp_start;
                    if (dlx_hdr->proto_ver_id == DLX_PROTO_VERSION) {
                        pak_size = dlx_hdr->packet_length;
                    } else if (dlx_hdr->proto_ver_id == SSP_VERS) {
                        pak_size = peer->ssp_start.header_len +
                                   peer->ssp_start.message_len;
                    } else {
                        PDEBUG("\nDLSW: Invalid dlsw version %d",
                               dlx_hdr->proto_ver_id);
                        dlsw_readf_error(peer);
                        return;
                    }

                    if (pak_size > PEER_MAX_PAK) {
                        PDEBUG("\nDLSW: Invalid dlsw tcp packet size %d", pak_size);
                        dlsw_readf_error(peer);
                        return;
                    }

                    peer->pak = getbuffer(pak_size);
                    if (!peer->pak) {
                        dlsw_readf_error(peer);
                        return;
                    }
                    peer->pak->datagramsize =  pak_size;
                    bcopy(&peer->ssp_start, peer->pak->datagramstart, 
                          sizeof(ssp_hdr_start_t));  
                    peer->rd_state = RD_DATA; 
                    peer->target = peer->pak->datagramstart + 
                                   sizeof(ssp_hdr_start_t);
                    peer->sizeleft = pak_size - sizeof(ssp_hdr_start_t);     
                    break;

                  case RD_DATA:
                    peer->rd_state = RD_HEADER;
                    peer->target = (uchar *) &peer->ssp_start;
                    peer->sizeleft = sizeof(ssp_hdr_start_t);     
                    peer_input(peer, peer->pak, FALSE);
                    peer->pak = NULL;
                    break;
                }
            }
        }
    }
}


/*
 * dlsw_tcpd_fini()
 * routine called when the tcpdriver detects a close on
 * the tcpdriver stream.
 */

void dlsw_tcpd_fini (tcp_encaps *t, void *cookie)
{
    peerentry *peer = (peerentry *) cookie;
    peerentry *top_peer;
    char buff[DLSW_MAX_PEER_ID];

    /*
     * It is possible that after DLSw aborts a tcp connection,
     * the tcp driver may call back the fini vector. This is 
     * because tcp processes the abort in the background and
     * the fini may arrive before the abort is processed.
     * Check peer status to avoid accessing a bogus cookie.
     */
    if ((peer->peer_status != PEER_READY) &&
        (peer->peer_status != PEER_NOT_READY)) {
        if (peer_debug_all)
            buginf("\nDLSw: fini(): invalid peer %x - status %x",
                   peer, peer->peer_status);
        return;
    }

    PDEBUG("\nDLSw: dlsw_tcpd_fini() for peer %i(%d)",
           peer->ipaddr, peer->port);

    /*
     * Special processing required to close the write pipe
     * after capabilities exchange.
     */
    if ((peer->priority_top == NULL) &&
        (lpeer->cap.num_tcp_sessions == CISCO_NUM_TCP_PIPES) &&
        (peer->cap.num_tcp_sessions == CISCO_NUM_TCP_PIPES) &&
        (lpeer->ipaddr < peer->ipaddr) && (peer->tcp_wr_t) &&
        (peer->tcp_rd_t) && (peer->tcp_wr_t == t)) {
        PDEBUG("\nDLSw: dlsw_tcpd_fini() closing write pipe for peer %i",
               peer->ipaddr);
        dlsw_peer_fsm(peer, CLOSE_WRITE_PIPE);
        return;
    }

    if (peer->tcp_wr_t == t)
        peer->tcp_wr_t = NULL;
    else if (peer->tcp_rd_t == t)
        peer->tcp_rd_t = NULL;
    else {
        PDEBUG("\nDLSw: tcp_encaps not found for peer %i(%d)",
               peer->ipaddr, peer->port);
    }

    if (peer->priority_top)
        top_peer = peer->priority_top;
    else
        top_peer = peer;

    if (top_peer->active_open_wait) {
        PDEBUG("\nDLSw: tcp fini for %s while blocking",
               peer_str(peer, buff));
        top_peer->tcp_fini_pending = TRUE;
        return;
    }

    /*
     * Always call the peer fsm with top peer.
     */
    PDEBUG("\nDLSw: tcp fini closing connection for peer %i(%d)",
           top_peer->ipaddr, top_peer->port);
    dlsw_peer_fsm(top_peer, CLOSE_CONNECTION);
}


/*
 * dlsw_tcpd_flow_ctrl()
 *
 */
static void dlsw_tcpd_flow_ctrl (tcp_encaps *t, ulong level, void *cookie)
{
    peerentry *peer;
    peer_msg_u peer_msg;

    peer = (peerentry *) cookie;
    if (!peer)
        return;

    if (peer->priority_top)
        peer = peer->priority_top;

    if (level == (TCP_CONG_HIGH | TCP_CONG_INCREASE)) {
        PDEBUG("\nDLSw: dlsw_tcpd_flow_ctrl() called for new TCP_CONG_HIGH");
        dlsw_peer_fsm(peer, BUSY_XPORT);
        peer_msg.type = PEER_FLOW_CTRL_INDICATION;
        peer_msg.p_flow_ind.peer_handle = (peerhandle_t) peer;
        peer_msg.p_flow_ind.cong_level =
                             (DLSW_PEER_CONG_HIGH | DLSW_PEER_CONG_INCREASE);
        peer_to_core(&peer_msg);
    } else if ((level == TCP_CONG_LOW) &&
        (peer->conn_state == PEER_XPORT_BUSY)) {
        PDEBUG("\nDLSw: dlsw_tcpd_flow_ctrl() called for new TCP_CONG_LOW");
        dlsw_peer_fsm(peer, UNBUSY_XPORT);
        peer_msg.type = PEER_FLOW_CTRL_INDICATION;
        peer_msg.p_flow_ind.peer_handle = (peerhandle_t) peer;
        peer_msg.p_flow_ind.cong_level = DLSW_PEER_CONG_LOW;
        peer_to_core(&peer_msg);
    }
}


/*
 * dlsw_tcpd_connect()
 *
 * Opens this peer's write TCP session - local port 2067.
 */

boolean dlsw_tcpd_connect (peerentry *peer)
{
    int openstat;
    tcp_encaps tmp;
    peerentry *top_peer;

    if (peer->tcp_wr_t) {
        PDEBUG("\nDLSw: write pipe already opened for peer %i", peer->ipaddr);
        return(TRUE);
    }

    if (peer->priority_top)
        top_peer = peer->priority_top;
    else
        top_peer = peer;

    top_peer->active_open_wait = TRUE;
    openstat = -1;
    /*
     * tcpdriver_active_open will block waiting for the connection
     * to open.
     */
    memset(&tmp, 0, sizeof(tmp));
    tmp.readf = (tcpdriver_readf) dlsw_tcpd_readf;
    tmp.closef = (tcpdriver_closef) dlsw_tcpd_fini;
    tmp.flushf = NULL;
    tmp.flow_ctrl = (tcpdriver_flow_ctrl) dlsw_tcpd_flow_ctrl;
    tmp.inthresh = 4;
    tmp.cookie = peer;

    /* For TCP window, allow 4 large frames in transit + overhead (20480)*/
    peer->tcp_wr_t = reg_invoke_ip_tcpdriver_active_open(peer->ipaddr, 
                     lpeer->ipaddr, peer->port, 20480, &tmp, &openstat);

    top_peer->active_open_wait = FALSE;
    if (top_peer->tcp_fini_pending == TRUE) {
        top_peer->tcp_fini_pending = FALSE;
        if (peer->tcp_wr_t) {
            reg_invoke_ip_tcpdriver_update(peer->tcp_wr_t, NULL, 0, NULL);
            reg_invoke_ip_tcpdriver_abort(peer->tcp_wr_t);
            peer->tcp_wr_t = NULL;
        }
        if (peer->tcp_rd_t) {
            reg_invoke_ip_tcpdriver_update(peer->tcp_rd_t, NULL, 0, NULL);
            reg_invoke_ip_tcpdriver_abort(peer->tcp_rd_t);
            peer->tcp_rd_t = NULL;
        }
        PDEBUG("\nDLSw: CONN: peer %i open failed due to partner close",
               peer->ipaddr);
        return(FALSE);
    }

    if (peer->tcp_wr_t) {
        peer->tcp_wr_t->tcb->giveuptime = peer->tcp_timeout;
	reg_invoke_ip_tcpdriver_keepalive(peer->tcp_wr_t,
					  TCPD_KP_ENABLE,
					  peer->keepalive_int);
        if (peer->keepalive_int) {
	     reg_invoke_ip_tcpdriver_keepalive(peer->tcp_wr_t, 
					       TCPD_KP_INTERVAL,
					       peer->keepalive_int);
             reg_invoke_ip_tcpdriver_keepalive(peer->tcp_wr_t, 
					       TCPD_KP_ATTEMPTS,
					       DLSW_KEEP_CNT_THOLD);
	}
        reg_invoke_ip_tcpdriver_set_oqmax(peer->tcp_wr_t, peer->max_tcp_qlen);
        reg_invoke_ip_tcpdriver_set_flow_ctrl_levels(peer->tcp_wr_t, 
                     ((peer->max_tcp_qlen * DLSW_TCP_HI_WATER_PERCENT) / 100),
                     ((peer->max_tcp_qlen * DLSW_TCP_LO_WATER_PERCENT) / 100));
        reg_invoke_ip_tcpdriver_change_timeout(peer->tcp_wr_t,TRUE); 
        return(TRUE);
    } else {
        PDEBUG("\nDLSw: CONN: peer %i open failed, %s [%d]",
               peer->ipaddr, tcpuser_open_fail_str(openstat), openstat);
        return(FALSE);
    } 
}


/*
 * dlsw_tcpd_listen()
 */

void dlsw_tcpd_listen (tcbtype *tcb)
{
    tcp_encaps *t;
    tcp_encaps tmp;
    ipaddrtype ipaddr;
    ipaddrtype destipaddr;
    peerentry *peer;
    peerentry *top_peer;

    memset(&tmp, 0, sizeof(tmp));
    tmp.readf = (tcpdriver_readf) dlsw_tcpd_readf;
    tmp.closef = (tcpdriver_closef) dlsw_tcpd_fini;
    tmp.flushf = NULL;
    tmp.flow_ctrl = (tcpdriver_flow_ctrl) dlsw_tcpd_flow_ctrl;
    tmp.inthresh = 4;
    tmp.cookie = NULL;

    if ((!lpeer) || (dlsw_disable) || (!lpeer->ipaddr)) {
        tcp_close(tcb);
        return;
    }

    /*
     * Disable NET interrupts so both tcp pipes can open before 
     * we receive a message from our peer.
     */ 
    t = reg_invoke_ip_tcpdriver_passive_open(tcb, 20480, &tmp);
    if (!t) {
        errmsg(PEER_PASSIVE, tcb->foreignhost.ip_addr, 
                             tcb->foreignport, tcb->localport);
        tcp_close(tcb);
    } else {
        destipaddr = reg_invoke_ip_tcpdriver_lhost(t); 
        if (destipaddr != lpeer->ipaddr) {
            if (dlsw_peer_debug) {
                buginf("\nDLSW: peer %i using incorrect ip address %i",
                       reg_invoke_ip_tcpdriver_fhost(t), destipaddr);
                buginf("\n      should match local-peer peer-id %i",
                       lpeer->ipaddr);
            }
            reg_invoke_ip_tcpdriver_abort(t);
            return;
        }
        /* 
         * Find Remote Peer that opened this connection.
         */
        ipaddr = reg_invoke_ip_tcpdriver_fhost(t); 
        if (!ipaddr) {
            if (dlsw_peer_debug)
                buginf("\nDLSW: no ipaddr from tcpdriver_fhost()");
            reg_invoke_ip_tcpdriver_abort(t);
            return;
        }

        peer = find_priority_peer(ipaddr, tcb->localport);
        if (!peer) {
            /*
             * Only create new prom peer if high priority port
             * is connecting in.
             */
            if (tcb->localport != DLSW_RD_PORT) {
                if (dlsw_peer_debug)
                    buginf("\nDLSw: Not port 2065 - Rej conn from %i(%d)",
                           ipaddr, tcb->localport);
                reg_invoke_ip_tcpdriver_abort(t);
                return;
            }

            /*
             * A non-configured remote peer is connecting in. Accept the 
             * connection if peer-on-demmand is enabled.
             */
            if (!lpeer->cap.pod_enabled) {
                if (dlsw_peer_debug)
                    buginf("\nDLSw: Not promiscuous - Rej conn from %i(%d)",
                           ipaddr, tcb->localport);
                reg_invoke_ip_tcpdriver_abort(t);
                return;
            } 

            peer = peer_create_remote(0, ipaddr, PROM_PEER, NULL, DLSW_TCP,
                               	TRUE, FALSE, lpeer->prompeer_defaults.cost,
                                lpeer->prompeer_defaults.largest_frame,
                                lpeer->prompeer_defaults.keepalive_int,
                                lpeer->prompeer_defaults.dlsw_lsap_out,
                                lpeer->prompeer_defaults.netbios_h_oacf,
                                lpeer->prompeer_defaults.netbios_b_oacf,
                                lpeer->prompeer_defaults.max_tcp_qlen, NULL,
                                NULL, NULL, 0, 0, 0, 0,
                                lpeer->prompeer_defaults.dlsw_dmac_outlist,
                                lpeer->prompeer_defaults.dlsw_dmac_out, NULL,
                                NORMAL_MODE);

            if (!peer) {
                reg_invoke_ip_tcpdriver_abort(t);
                return; 
            }
        } else if (peer->xport_type != DLSW_TCP) {
            PDEBUG("\nDLSw: Found non-TCP peer with same ipaddr %i", ipaddr);
            reg_invoke_ip_tcpdriver_abort(t);
            return;
        } else if (TIMER_RUNNING_AND_SLEEPING(peer->last_passive_open)) {
            /*
             * First a few definitions :
             *  - initial connection : the TCP connection first opened 
             *    by a peer wishing to establish a DLSw 2 pipe conn.
             *  - response connection : the 2nd TCP pipe opened in response
             *    to the initial connection.
             *
             * There is a race condition between Peer A closing it's
             * initial connection due to a timeout and Peer B 
             * successfully connecting it's response connection. If 
             * Peer A disconnects it's initial conn, and then it receives
             * Peer B's response conn, Peer A will misinterpret Peer B's 
             * response conn as an initial conn and Peer A will attempt 
             * to open a response conn back to Peer B. Wait it gets
             * worse! During this time, Peer B detected that peer A aborted
             * it's initial conn while Peer B was connecting it's response 
             * conn. Due to receiving this abort, Peer B sets the
             * tcp_fini_pending flag. Then after unblocking and  Peer B's
             * response conn successfully connects, Peer B disconnects
             * it due to the latent tcp_fini_pending event. 
             *   
             * While Peer B closes it response conn due to the fini
             * event, Peer A is connecting it's response conn. By the time
             * Peer A connects it;s response conn, peer B has already
             * cleaned up it's response conn, and Peer B misinterprets
             * Peer A's connect as an initial connection. These events
             * ignites a thrashing sequence until the timing is such that 
             * one peer disconnects it's pipe before the other peer can
             * connect it's response pipe. This may take several minutes!
             */ 
            PDEBUG("\nDLSw: Rejecting looped open from peer %i (%d->%d)",
                   ipaddr, tcb->foreignport, tcb->localport);
            reg_invoke_ip_tcpdriver_abort(t);
            TIMER_START(peer->last_passive_open, TCP_PASSIVE_OPEN_WINDOW);
            return;
        } else {
            if (peer->priority_top)
                top_peer = peer->priority_top;
            else
                top_peer = peer;

            if (top_peer->active_open_wait) {
                PDEBUG("\nDLSw: Conn collision - Rej conn from %i(%d)",
                       ipaddr, tcb->localport);
                reg_invoke_ip_tcpdriver_abort(t);
                top_peer->tcp_fini_pending = TRUE; /* force immediate cleanup */
                TIMER_START(peer->last_passive_open, TCP_PASSIVE_OPEN_WINDOW);
                return;
            }
        }
	reg_invoke_ip_tcpdriver_keepalive(t, TCPD_KP_ENABLE,
					  peer->keepalive_int);
	if (peer->keepalive_int) {
	     reg_invoke_ip_tcpdriver_keepalive(t, TCPD_KP_INTERVAL,
					       peer->keepalive_int);
	     reg_invoke_ip_tcpdriver_keepalive(t, TCPD_KP_ATTEMPTS,
					       DLSW_KEEP_CNT_THOLD);
	}

        TIMER_START(peer->last_passive_open, TCP_PASSIVE_OPEN_WINDOW);
	t->tcb->giveuptime = peer->tcp_timeout;
        reg_invoke_ip_tcpdriver_update(t, NULL, 0, peer);
        reg_invoke_ip_tcpdriver_set_oqmax(t, peer->max_tcp_qlen);
        reg_invoke_ip_tcpdriver_set_flow_ctrl_levels(t,
                     ((peer->max_tcp_qlen * DLSW_TCP_HI_WATER_PERCENT) / 100),
                     ((peer->max_tcp_qlen * DLSW_TCP_LO_WATER_PERCENT) / 100));
        reg_invoke_ip_tcpdriver_change_timeout(t,TRUE);
        reg_invoke_ip_tcpdriver_change_timeout(t, TRUE);
        PDEBUG("\nDLSw: passive open %i(%d) -> %d",
               tcb->foreignhost.ip_addr, tcb->foreignport, tcb->localport);
        if (!peer->priority_top) {
            peer->tcp_rd_t = t;
            dlsw_peer_fsm(peer, READ_PIPE_OPENED);
        } else {
            GET_TIMESTAMP(peer->uptime);
            peer->peer_status = PEER_READY;
            peer->tcp_wr_t = t;
            peer->conn_state = PEER_CONN;

            /*
             * If all priority peers are connected, then
             * release the waiting flag.
             */
            if (tcb->localport == DLSW_P3_PORT)
                peer->priority_top->wait_pri_connect = FALSE;
        }
    }
}


/*
 * dlsw_abort_one()
 */

static void dlsw_abort_one (peerentry *peer)
{

    if (peer->tcp_wr_t) {
        PDEBUG("\nDLSw: aborting tcp connection for peer %i(%d)",
               reg_invoke_ip_tcpdriver_fhost(peer->tcp_wr_t),
               reg_invoke_ip_tcpdriver_fport(peer->tcp_wr_t));
        reg_invoke_ip_tcpdriver_update(peer->tcp_wr_t, NULL, 0, NULL);
        reg_invoke_ip_tcpdriver_abort(peer->tcp_wr_t);
        peer->tcp_wr_t = NULL;
    }
    if (peer->tcp_rd_t) {
        PDEBUG("\nDLSw: aborting tcp connection for peer %i(%d)",
               reg_invoke_ip_tcpdriver_fhost(peer->tcp_rd_t),
               reg_invoke_ip_tcpdriver_fport(peer->tcp_rd_t));
        reg_invoke_ip_tcpdriver_update(peer->tcp_rd_t, NULL, 0, NULL);
        reg_invoke_ip_tcpdriver_abort(peer->tcp_rd_t);
        peer->tcp_rd_t = NULL;
    }
}


/*
 * dlsw_tcpd_abort
 * nuke with extreme prejudice a tcpdriver peer.  this aborts the
 * connection and drains any input.
 */

void dlsw_tcpd_abort (peerentry *peer)
{
    uchar i;

    if (peer->priority_top)
        peer = peer->priority_top;

    dlsw_abort_one(peer);
    if (peer->priority) {
        for (i = 1; i <= DLSW_PRIORITY_COUNT; i++) {
            if (peer->priority_peers[i]) {
                dlsw_abort_one(peer->priority_peers[i]);
                peer->priority_peers[i]->conn_state = PEER_DISC;
            }
        }
    }
}


/*
 * dlsw_tcpd_sender()
 *
 * if 'pm' is NULL, packet will not be prioritized.
 */

peer2core_ret_t dlsw_tcpd_sender (peerentry  *peer, paktype *pak,
                                  boolean force, peer_pak_cat category,
                                  peer_put_t *pm)
{
    int result;
    int priority;

    if (!peer->tcp_wr_t) {
        if (peer->peer_type == DYNAMIC_PEER &&
            peer->conn_state == PEER_DISC &&
            peer->connect_dynam == FALSE) {
            PDEBUG("\nDLSw: Packet for disconnected dynamic peer %i",
                   peer->ipaddr);
            peer->connect_dynam = TRUE;
            if (!pak_enqueue(&peer->drpQ, pak))
                datagram_done(pak);
            peer->conn_retries = 0;
            mgd_timer_start(&peer->conn_timer, 0);
            return(PEER_NO_CONNECTIONS);
	 } else {
	      peer->drops++;
	      datagram_done(pak);
	      return(PEER_UNREACHABLE);
	 }
    }
       
    /*
     * A peer waiting for priority peers to connect,
     * can not be used.
     */
    if ((peer->wait_pri_connect == TRUE) ||
         ((peer->priority_top) &&
         (peer->priority_top->wait_pri_connect == TRUE))) {
        datagram_done(pak);
        return(PEER_UNREACHABLE);
    }

    /*
     * Can not send if we are waiting for our partner to complete
     * the cap ex and close our write pipe.
     */
    if (peer->close_write_pending) {
        datagram_done(pak);
        return(PEER_UNREACHABLE);
    }

    if (peer->priority) {
        if ((category == PEER_USER_INFO) && (pm) && (pm->lan_port)) {
            peer = dlsw_packet_classify_peer(pak, peer, pm);
        } else {
            priority = peer_get_pri_by_cat(category);
            if (priority != PRIORITY_HIGH)
                peer = peer->priority_peers[priority];
        }
    }

    pak->acct_proto = ACCT_PROTO_SRB;
    peer->pkts_tx++;

    /*
     * Do not count peer generated packets for dynamic peer inactivity.
     */
    if ((peer->peer_type == DYNAMIC_PEER) &&
        (category != PEER_CONN_ADMIN) && (category != PEER_CONN_SETUP))
        GET_TIMESTAMP(peer->last_non_peer_traffic);

    result = reg_invoke_ip_tcpdriver_send(peer->tcp_wr_t, pak, force);
    if (TCPDSEND_OK(result)) {
        clear_if_input(pak);
        if (peer->conn_state == PEER_XPORT_BUSY) {
            return(PEER_SUCCESS_AND_BUSY);
        } else {
            return(PEER_SUCCESS);
        }
    } else {
        datagram_done(pak);
        peer->drops++;
    }


    if (result == TCPDSEND_QFULL) {
        PDEBUG("\nDLSw: outbound tcp queue full (%d) for peer %i",
               QUEUESIZE(&peer->tcp_wr_t->outputq), peer->ipaddr);
        return(PEER_BUSY);
    }
  
    /* 
     * Otherwise result == TCPDSEND_CLOSED
     */
    if (peer->priority_top)
        peer = peer->priority_top;

    PDEBUG("\nDLSw: connection closed for peer %i", peer->ipaddr);
    dlsw_peer_fsm(peer, CLOSE_CONNECTION);
    return(PEER_UNREACHABLE);
}


/*
 * dlsw_packet_classify_peer()
 */

peerentry *dlsw_packet_classify_peer (paktype *pak, peerentry *rootpeer,
                                      peer_put_t *pm)
{
    int priority, i;

    if (!rootpeer->priority)
        return(rootpeer);

    priority = dlsw_packet_classify(pak, pm);
    if (priority != 0) {
        for (i = priority; i <= DLSW_PRIORITY_COUNT; i++) {
             if (rootpeer->priority_peers[i])
                 return(rootpeer->priority_peers[i]);
        }
    }
    return(rootpeer);
}


/*
 * dlsw_packet_classify()
 *
 * Classify a packet for its priority.
 * If it is a FID2 TH do SNA Local LU Address prioritization.
 * If it is an SNA FID2 frame, classify by the local address priority.
 */

static uchar dlsw_packet_classify (paktype *pak, peer_put_t *pm)
{
    if (!pm->lan_port->swidb) {
        if (dlsw_peer_debug)
            buginf("\nDLSw: dlsw_packet_classify(): NULL input port");
        return(PRIORITY_NORMAL);
    }

    /* 
     * SNA FID2 Local LU address prioritization 
     */
    if ((pm->lan_port->swidb->hwptr->sna_la_prilist) &&
        (((pm->dsap & ~SAP_GROUP) == SAP_IBM_SNA) ||
         ((pm->ssap & ~SAP_GROUP) == SAP_IBM_SNA))) {
        /*
         * pak->info_start points to SAP header.
         * TH header starts after 4 byte SAP header.
         */
        if ((pak->info_start[4] & SNA_TH_FID_MASK) == SNA_TH_FID2_VALUE)
            return(general_fid2_pri_classify(pm->lan_port->swidb->hwptr,
                                             pm->dmac, pm->smac, &pm->dsap,
                                             &pm->ssap, &pak->info_start[4]));
    }

    if (pm->lan_port->swidb->hwptr->sap_prilist) {
        return(general_sap_pri_classify(pm->lan_port->swidb->hwptr, pm->dmac,
                                        pm->smac, &pm->dsap, &pm->ssap));
    } else {
        return(PRIORITY_NORMAL);
    }
}

