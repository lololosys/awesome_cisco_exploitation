/* $Id: dlsw_peer_fsm.c,v 3.7.20.3 1996/08/28 12:41:17 thille Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_fsm.c,v $
 *------------------------------------------------------------------
 * DLSw peer transport finite state machine
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * DLSw peer transport finite state machine and action routines
 * which process DLSw peer transport events.
 *------------------------------------------------------------------
 * $Log: dlsw_peer_fsm.c,v $
 * Revision 3.7.20.3  1996/08/28  12:41:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.20.2  1996/06/27  15:13:39  fbordona
 * CSCdi61278:  DLSw: System restarted by bus error at PC 0xD0D0D0D
 *              action_b(). Do not free peers if blocked on TCP open.
 *              Connect timer not set properly for priority peers.
 * Branch: California_branch
 *
 * Revision 3.7.20.1  1996/05/17  10:46:27  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.6.3  1996/04/26  07:34:45  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.6.2  1996/04/23  20:13:59  fbordona
 * CSCdi55437:  DLSw peer may take several minutes to connect after reload
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.6.1  1996/04/20  02:08:27  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7  1996/02/01  06:01:40  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/02/01  02:57:10  fbordona
 * CSCdi47930:  DLSw session does not reconnect after peer powered off.
 *              Only attempt one peer connection at at time to avoid
 *              starving the dlsw background process.
 *
 * Revision 3.5  1996/01/04  21:39:45  fbordona
 * CSCdi46273:  DLSw peers cycle CONNECT to DISC after shut no shut on
 *              serial.
 *
 * Revision 3.4  1995/11/29  22:06:36  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.3  1995/11/17  09:03:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:24:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:55:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:27:30  hampton
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
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../tcp/tcp.h"
#include "../if/network.h"
#include "../if/tring.h"
#include "../ui/debug.h"

#include "../ibm/netbios.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_debug.h"


/*
 * File Scope Function Prototypes
 */
static void action_a (peerentry *);
static void action_b (peerentry *);
static void action_c (peerentry *);
static void action_d (peerentry *);
static void action_e (peerentry *);
static void action_f (peerentry *);
static void action_g (peerentry *);


/*
 * Peer Connection State Machine
 *
 */
 
void dlsw_peer_fsm (peerentry *peer, peereventtype event)
{
  
    switch(peer->conn_state) {

      case PEER_DISC:
        switch(event) {
          case OPEN_CONNECTION:
            action_a(peer);
            break;
          case CLOSE_CONNECTION:
            action_d(peer); /* required for admin cleanup of disc'd peers */
            break;
          case READ_PIPE_OPENED:
            action_b(peer);
            break; 
          case CAP_RCVD:
            dlsw_peer_newstate(peer, PEER_WAIT_CAP);
            break; 
          default:
            PDEBUG("\ndlsw_peer_fsm() event %s invalid for state %s",
              event_to_string(event), state_to_string(peer->conn_state));
            break;
        }
        break;

      case PEER_WAIT_READ:
        switch(event) {
          case CLOSE_CONNECTION:
          case READ_OPEN_TIMEOUT:
            action_d(peer);
            break;
          case READ_PIPE_OPENED:
            action_c(peer);
            break;
          default:
            PDEBUG("\ndlsw_peer_fsm() event %s invalid for state %s",
              event_to_string(event), state_to_string(peer->conn_state));
            break;
        }
        break;

      case PEER_WAIT_CAP:
        switch(event) {
          case CLOSE_CONNECTION:
            action_d(peer);
            break;
          case CAP_RCVD:
          case CAP_RSP_RCVD:
            action_e(peer);
            break;
          case CLOSE_WRITE_PIPE:
            action_g(peer);
            break;
          case READ_PIPE_OPENED:
            action_d(peer);
            break;
          default:
            PDEBUG("\ndlsw_peer_fsm() event %s invalid for state %s",
              event_to_string(event), state_to_string(peer->conn_state));
            break;
        }
        break;

      case PEER_CONN:
        switch(event) {
          case CLOSE_CONNECTION:
            action_d(peer);
            break;
          case CAP_RSP_RCVD:
          case CAP_RCVD:
          case UNBUSY_XPORT:
            break; /* NOP */ 
          case CLOSE_READ_PIPE:
            action_f(peer);
            break;
          case CLOSE_WRITE_PIPE:
            action_g(peer);
            break;
          case READ_PIPE_OPENED:
            /* 
             * Partner lost the connection. Disconnect to clean up and then 
             * re-connect on the next try.
             */
            action_d(peer);
            break;
          case BUSY_XPORT:
            peer->conn_state = PEER_XPORT_BUSY;
            break;
          default:
            PDEBUG("\ndlsw_peer_fsm() event %s invalid for state %s",
              event_to_string(event), state_to_string(peer->conn_state));
            break;
        }
        break;

      case PEER_XPORT_BUSY:
        switch(event) {
          case CLOSE_CONNECTION:
            action_d(peer);
            break;
          case CAP_RSP_RCVD:
          case CAP_RCVD:
          case BUSY_XPORT:
            break; /* NOP */ 
          case CLOSE_READ_PIPE:
            action_f(peer);
            break;
          case CLOSE_WRITE_PIPE:
            action_g(peer);
            break;
          case READ_PIPE_OPENED:
            action_d(peer);
            break;
          case UNBUSY_XPORT:
            peer->conn_state = PEER_CONN;
            break;
          default:
            PDEBUG("\ndlsw_peer_fsm() event %s invalid for state %s",
              event_to_string(event), state_to_string(peer->conn_state));
            break;
        }
        break;
    }
}


/*
 * action_a()
 *
 * For each transport type, this routine attempts to actively open 
 * the peer session. For TCP connections, called when the read pipe
 * is closed.
 */

static void action_a (peerentry *peer)
{
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: action_a() attempting to connect %s",
	   peer_str(peer, buff));    
    GET_TIMESTAMP(peer->conn_start_time);
    if (peer->xport_type == DLSW_TCP) {
        if (peer->tcp_wr_t) {
            PDEBUG("\nDLSw: action_a(): write pipe already open");
            reg_invoke_ip_tcpdriver_abort(peer->tcp_wr_t);
            peer->tcp_wr_t = NULL;
            dlsw_peer_newstate(peer, PEER_DISC);
        } else if ((*peer->connect)(peer)) {
            PDEBUG("\nDLSw: action_a(): Write pipe opened for %s", 
                   peer_str(peer, buff));
            GET_TIMESTAMP(peer->conn_start_time);
            dlsw_peer_newstate(peer, PEER_WAIT_READ);
        } else {
            PDEBUG("\nDLSw: action_a(): CONN failed - retries %d",
                   peer->conn_retries);
            dlsw_peer_newstate(peer, PEER_DISC);
        }
    } else {
        if ((*peer->connect)(peer)) {
            PDEBUG("\nDLSw: action_a(): Connection opened for %s", 
                   peer_str(peer, buff));
            dlsw_peer_newstate(peer, PEER_WAIT_CAP);
        } else {
            PDEBUG("\nDLSw: action_a(): CONN failed - retries %d",
                   peer->conn_retries);
            dlsw_peer_newstate(peer, PEER_DISC);
        }
    }
}


/*
 * action_b()
 *
 * For TCP conections only.
 * Opens the TCP write pipe after the read pipe is already open.
 */

static void action_b (peerentry *peer)
{
    char buff[DLSW_MAX_PEER_ID];
    boolean conn_rc;

    /*
     * Since the remote peer is actively opening the connection,
     * stop the connect timer. If this connect attempt fails,
     * dlsw_peer_newstate() will restart the connect timer if
     * necessary.
     */
    mgd_timer_stop(&peer->conn_timer);
    if (!peer->priority_top) {
        if (peer->tcp_wr_t) {
            PDEBUG("\nDLSw: action_b(): write pipe for %s already open,
                   peer_str(peer, buff)");
            dlsw_peer_newstate(peer, PEER_WAIT_CAP);
        } else if (!peer->tcp_rd_t) {
            PDEBUG("\nDLSw: action_b(): read pipe for %s is NOT open",
                   peer_str(peer, buff));
            (*peer->abort)(peer);
            dlsw_peer_newstate(peer, PEER_DISC);
        } else {
            PDEBUG("\nDLSw: action_b(): opening write pipe for %s",
                   peer_str(peer, buff));
            conn_rc = (*peer->connect)(peer);
            if ((peer->remove) ||
                ((peer->peer_status != PEER_READY) &&
                 (peer->peer_status != PEER_NOT_READY))) {
                errmsg(PEER_GENERAL, "Action B: Peer has been freed");
                return;
            }
            if (conn_rc) {
                dlsw_peer_newstate(peer, PEER_WAIT_CAP);
            } else {
                (*peer->abort)(peer);
                dlsw_peer_newstate(peer, PEER_DISC);
            }
        }
    } else {
        /*
         * This is a priority peer.
         */
        PDEBUG("\nDLSw: action_b(): connect state for priority %s",
               peer_str(peer, buff));
        dlsw_peer_newstate(peer, PEER_CONN);
    }
}


/*
 * action_c()
 *
 * For TCP conections only.
 * Performs administrative duties in response to a TCP read pipe
 * opening after the write pipe is already opened.
 */

static void action_c (peerentry *peer)
{
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: action_c(): for %s", peer_str(peer, buff));
    if ((!peer->tcp_wr_t) || (!peer->tcp_rd_t)) {
        PDEBUG("\nDLSw: action_c(): read/write pipe is NOT open");
    } else
        dlsw_peer_newstate(peer, PEER_WAIT_CAP);
}


/*
 * action_d()
 */

static void action_d (peerentry *peer)
{
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: action_d(): for %s", peer_str(peer, buff));
    (*peer->abort)(peer);
    dlsw_peer_newstate(peer, PEER_DISC);
}


/*
 * action_e()
 *
 * Capabilities Exchange Msg Received.
 */

static void action_e (peerentry *peer)
{
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: action_e(): for %s", peer_str(peer, buff));
    if ((peer->cap.cap_known) && (peer->cap.cap_ack)) {
        if ((peer->xport_type == DLSW_TCP) &&
            (peer->priority) && (peer->cap.priority) &&
            (lpeer->ipaddr < peer->ipaddr)) {
            /*
             * The remote peer will actively establish the priority
             * peer connections. Since this peer will move into a ready
             * to send position, set flag so there will be no sending
             * until all priority peers are connected.
             */
            PDEBUG("\nDLSw: waiting for priority peers to connect");
            peer->wait_pri_connect = TRUE;
        }
        dlsw_peer_newstate(peer, PEER_CONN);
    }
}


/*
 * action_f()
 *
 * Only use 1 TCP connection fo rthis peer session. Close our
 * local read pipe (DLSW_RD_PORT).
 */

static void action_f (peerentry *peer)
{
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: action_f(): for %s", peer_str(peer, buff));
    if (peer->tcp_rd_t) {
        PDEBUG("\nDLSw: closing read pipe tcp connection for peer %i(%d)",
               peer->ipaddr, peer->port);
        reg_invoke_ip_tcpdriver_update(peer->tcp_rd_t, NULL, 0, NULL);
        reg_invoke_ip_tcpdriver_abort(peer->tcp_rd_t);
        peer->tcp_rd_t = NULL;
    }
}


/*
 * action_g()
 *
 * Only use 1 TCP connection for this peer session. Close our
 * local write pipe (port 2067).
 */

static void action_g (peerentry *peer)
{
    char buff[DLSW_MAX_PEER_ID];

    PDEBUG("\nDLSw: action_g(): for %s", peer_str(peer, buff));
    if (peer->tcp_wr_t) {
        PDEBUG("\nDLSw: closing write pipe tcp connection for peer %i(%d)",
               reg_invoke_ip_tcpdriver_fhost(peer->tcp_wr_t),
               reg_invoke_ip_tcpdriver_fport(peer->tcp_wr_t));
        reg_invoke_ip_tcpdriver_update(peer->tcp_wr_t, NULL, 0, NULL);
        reg_invoke_ip_tcpdriver_abort(peer->tcp_wr_t);
        peer->close_write_pending = FALSE;
        peer->tcp_wr_t = peer->tcp_rd_t;
        peer->tcp_rd_t = NULL;
    }
}


char *state_to_string (peerstatetype state)
{
    switch (state) {
      case PEER_DISC:           return("DISCONN");
      case PEER_WAIT_READ:      return("WAIT_RD");
      case PEER_WAIT_CAP:       return("CAP_EXG");
      case PEER_CONN:           return("CONNECT");
      case PEER_XPORT_BUSY:     return("WAN_BUSY");
      default:                  return("UNKNOWN");
    }
}


char *event_to_string (peereventtype event)
{
    switch (event) {
      case OPEN_CONNECTION:     return("OPEN_CONNECTION");
      case CLOSE_CONNECTION:    return("CLOSE_CONNECTION");
      case READ_PIPE_OPENED:    return("READ_PIPE_OPENED");
      case READ_OPEN_TIMEOUT:   return("READ_OPEN_TIMEOUT");
      case CAP_RCVD:            return("CA_RCVD");
      case CAP_RSP_RCVD:        return("CAP_RSP_RCVD");
      case CAP_TIMEOUT:         return("CAP_TIMEOUT");
      case CLOSE_READ_PIPE:     return("CLOSE_READ_PIPE");
      case CLOSE_WRITE_PIPE:    return("CLOSE_WRITE_PIPE");
      case BUSY_XPORT:          return("BUSY_XPORT");
      case UNBUSY_XPORT:        return("UNBUSY_XPORT");
      default:                  return("UNKNOWN");
    }
}

