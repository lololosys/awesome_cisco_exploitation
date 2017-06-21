/* $Id: ncia_client.c,v 3.1.2.10 1996/08/31 05:20:16 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_client.c,v $
 *------------------------------------------------------------------
 * ncia_client.c - NCIA Client handling module
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_client.c,v $
 * Revision 3.1.2.10  1996/08/31  05:20:16  schiang
 * CSCdi67779:  debug ncia client message does not display all packet types
 * Branch: California_branch
 *
 * Revision 3.1.2.9  1996/07/23  13:20:50  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.2.8  1996/07/11  21:13:47  jolee
 * CSCdi62553:  Move externs from .c to .h that was lost in CSCdi61974
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/07/03  06:34:30  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/06/18  19:01:27  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/12  06:51:45  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/04  05:58:09  schiang
 * CSCdi59005:  ncia client output queue full with duplicated pkts if
 * client is down
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/01  21:49:20  jolee
 * CSCdi59245:  Added clear ncia client registered & show ncia client
 * saplist
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/22  18:06:17  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * corruppted -> corrupted
 * occured -> occurred
 *
 * Revision 3.1.2.1  1996/05/17  10:43:27  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.3  1996/05/14  04:31:50  jolee
 * CSCdi57570:  Added clear client support, removed backup mac suppport
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/05/03  22:06:21  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:41  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:15:25  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "../srt/srt_registry.h"
#include "../cls/dlc_registry.h"
#include "../partner/partner_registry.h"
#include "../hes/sse_registry.h"
#include "globs.h"
#include "interface_private.h"
#include "packet.h"
#include "../os/buffers.h"
#include "mgd_timers.h"
#include "config.h"
#include "parser.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../srt/rsrb.h"
#include "../srt/srb.h"
#include "../srt/srt_debug.h"
#include "../tcp/tcpdriver.h"
#include "../cls/clsi_msg.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../cls/dlc.h"
#include "../cls/cls_assert.h"

#include "../cls/ncia.h"
#include "../cls/ncia_client.h"
#include "../cls/ncia_server_fsm.h"
#include "ncia_debug_flags.h"
#include "ncia_debug.h"

/* NCIA forward references for internal functions */

static void ncia_tcpd_fini(tcp_encaps *t, void *cookie);
static void ncia_tcpd_readf(tcp_encaps *t, tcbtype *tcb, int thresh, 
                            void *cookie);
static void ncia_tcpd_flow_ctrl(tcp_encaps *t, ulong level, void *cookie);
static client_ret_t ncia_tcpd_sender(nciapeertype *ncia_client,
                                     paktype *pak, boolean force);
static void ncia_tcpd_slam(tcp_encaps *t, nciapeertype *ncia_client);
static void ncia_tcpd_abort(tcp_encaps  *t);

static ncia_servertype *ncia_find_server_by_ipaddr(ipaddrtype server_ipaddr);
static nciapeertype *ncia_client_create(tcp_encaps *t, tcbtype *tcb,
                                      ncia_servertype *ncia_server);
static void ncia_client_input(nciapeertype  *ncia_client);
static void ncia_init_tcp_session_parameter(nciapeertype *ncia_client);
static process ncia_conn_out_proc(void);
static ushort ncia_hash_ipaddr(ipaddrtype ipaddr);
static void ncia_adopt_passive_open_sess(nciapeertype *ncia_client, 
                                         tcp_encaps *t);
static void ncia_dump_client_packet(uchar *ndlc_packet, boolean send_rcv_flag);


#ifdef NCIA_TEST_ROUTER_TO_ROUTER
static void ncia_modify_in_msg(nciapeertype *ncia_client);
static void ncia_modify_out_msg(nciapeertype *ncia_client);
#endif  

/*
 * ncia_tcpd_connect()
 *
 * Opens a TCP session to a peer client - local port NCIA_PORT
 */

boolean ncia_tcpd_connect (nciapeertype *ncia_client)
{
    int openstat;
    tcp_encaps tmp;
    tcp_encaps *t;

    if (ncia_client->t) {
        if ((((ncia_client_debug >> 12) << 12) == NciaClientDebugALL) ||
            (ncia_client_debug & NciaClientDebugERROR))
      buginf ("\nNCIA: ncia_tcpd_connect pipe already opened to peer client %i",
                 ncia_client->ipaddr);
        return(FALSE);
    }
    openstat = -1;
    /*
     * tcpdriver_active_open will block waiting for the connection
     * to open.
     */
    bzero(&tmp, sizeof(tmp));
    tmp.readf = (tcpdriver_readf) ncia_tcpd_readf;
    tmp.closef = (tcpdriver_closef) ncia_tcpd_fini;
    tmp.flushf = NULL;
    tmp.flow_ctrl = (tcpdriver_flow_ctrl) ncia_tcpd_flow_ctrl;

    tmp.inthresh = sizeof(ndlc_header_t);
    tmp.cookie = ncia_client;

    /* For TCP window, allow 4 large frames in transit + overhead (20480)*/
    ncia_client->in_active_open = TRUE;

    t = reg_invoke_ip_tcpdriver_active_open(ncia_client->ipaddr,
            ncia_client->ncia_server->ipaddr, NCIA2_PORT, 20480,
            &tmp, &openstat); 

    ncia_client->in_active_open = FALSE;

    if (!t) {
        /* active open fails, empty the output queue */
        paktype *pak;

        if ((((ncia_client_debug >> 12) << 12) == NciaClientDebugALL) ||
            (ncia_client_debug & NciaClientDebugERROR)) {
             buginf("\nNCIA: Active open to peer %i failed, %s [%d]",
                    ncia_client->ipaddr, tcpuser_open_fail_str(openstat),
                    openstat);
	}

        while ((pak = dequeue(&ncia_client->OutputQ)))
            datagram_done(pak);
 
        return(FALSE);
    }
    else {

        if (ncia_client-> client_state != NCIA_OPEN_WAIT) {
                          /* notified by passive open to abort */
            ncia_tcpd_abort(t);
            return(TRUE);
        }
        else 
            ncia_client->t = t;   /* adopt active opened connection */
    }

    ncia_client->t->tcb->giveuptime = ncia_client->tcp_timeout;


    reg_invoke_ip_tcpdriver_keepalive(ncia_client->t, TCPD_KP_ENABLE,
                               ncia_client->ncia_server->tcp_keepalive_int);
    reg_invoke_ip_tcpdriver_keepalive(ncia_client->t, TCPD_KP_INTERVAL,
                               ncia_client->ncia_server->tcp_keepalive_int);
    reg_invoke_ip_tcpdriver_keepalive(ncia_client->t, TCPD_KP_ATTEMPTS, 
                                      NCIA_KEEP_CNT_THOLD);

    reg_invoke_ip_tcpdriver_set_oqmax(ncia_client->t,
                                      ncia_client->max_tcp_qlen);
    reg_invoke_ip_tcpdriver_set_flow_ctrl_levels(ncia_client->t,
        ((ncia_client->max_tcp_qlen * NCIA_TCP_HI_WATER_PERCENT) / 100),
        ((ncia_client->max_tcp_qlen * NCIA_TCP_LO_WATER_PERCENT) / 100));
    reg_invoke_ip_tcpdriver_change_timeout(ncia_client->t,TRUE);
    
    if ((((ncia_client_debug >> 12) << 12) == NciaClientDebugALL) ||
        (ncia_client_debug & NciaClientDebugEVENT))
         buginf("\nNCIA: Conn to peer %i -> %d established",
                ncia_client->ipaddr, NCIA2_PORT);

    return(TRUE);
 
}

/* ncia_tcpd_listen
 * Given a tcb opened by tcp_listen upon an incoming connection, insert
 * it into the tcpdriver structures.  We don't have enough information
 * here to easily validate so we wait for an incoming packet.  If prior
 * to receiving that first packet this side tries to open it's open
 */

void ncia_tcpd_listen (tcbtype *tcb)
{
    ncia_servertype  *ncia_server;
    nciapeertype *ncia_client;
    ipaddrtype server_ipaddr, client_ipaddr;
    tcp_encaps *t;
    tcp_encaps tmp;

    bzero(&tmp, sizeof(tcp_encaps));
    tmp.readf = (tcpdriver_readf) ncia_tcpd_readf;
    tmp.closef = (tcpdriver_closef) ncia_tcpd_fini;
    tmp.flushf = NULL;
    tmp.flow_ctrl = (tcpdriver_flow_ctrl) ncia_tcpd_flow_ctrl;
    tmp.inthresh = sizeof(ndlc_header_t); /* the first packet size we expect */
    tmp.cookie = NULL;	

    t = reg_invoke_ip_tcpdriver_passive_open(tcb, NCIA_TCP_WINDOW_SIZE, &tmp);

    if (!t) {
        if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
            (ncia_client_debug & NciaClientDebugERROR)) 
	     buginf("\nNCIA: Passive open from %i(%d) -> %d failed",
		    tcb->foreignhost.ip_addr, tcb->foreignport, tcb->localport);
	tcp_close(tcb);
    }
    else {
        if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
            (ncia_client_debug & NciaClientDebugEVENT))
	    buginf("\nNCIA: Passive open %i(%d) -> %d",
		   tcb->foreignhost.ip_addr, tcb->foreignport, tcb->localport);

        server_ipaddr = reg_invoke_ip_tcpdriver_lhost(t); 
        if (!server_ipaddr) {
            if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client_debug & NciaClientDebugERROR))
                buginf("\nNCIA Error: no ipaddr from tcpdriver_lhost()");
            reg_invoke_ip_tcpdriver_abort(t); 
            return;
        }

        if (!(ncia_server = ncia_find_server_by_ipaddr(server_ipaddr))) {
        /* serach for a configured server control block failed */

            if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client_debug & NciaClientDebugERROR)) 
               buginf("\nNCIA: Client %i using incorrect server ip address %i",
                       reg_invoke_ip_tcpdriver_fhost(t), server_ipaddr);
            reg_invoke_ip_tcpdriver_abort(t);
            return;
        }

        if (!(ncia_server->flags & NCIA_SERVER_CONFIGURED) || 
            !(ncia_server->flags & NCIA_SERVER_ACTIVE) ||
             (ncia_server->flags & NCIA_SERVER_STOP)) {
            if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client_debug & NciaClientDebugERROR)) 
                buginf("\nNCIA: Ncia server is inactive, terminate incoming connection from %i",
                       reg_invoke_ip_tcpdriver_fhost(t));
            reg_invoke_ip_tcpdriver_abort(t);
            return;
        }

        /* 
         * Record the client info.
         */
        client_ipaddr = reg_invoke_ip_tcpdriver_fhost(t); 
        if (!client_ipaddr) {
            if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client_debug & NciaClientDebugERROR))
                buginf("\nNCIA: No ipaddr from client tcpdriver_fhost()");
            reg_invoke_ip_tcpdriver_abort(t);
            return;
        }

        ncia_client = ncia_find_client_by_ipaddr(ncia_server, client_ipaddr);
        if (!ncia_client) {          /* initial client call in */
    	    ncia_client = ncia_client_create(t, tcb, ncia_server);
            if (!ncia_client) {
                if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
                    (ncia_client_debug & NciaClientDebugERROR)) 
                buginf ("\nNCIA: Create ncia client cb at passive open failed");
                reg_invoke_ip_tcpdriver_abort(t);
                return; 
            }
            else { 
              ncia_init_tcp_session_parameter(ncia_client);
              ncia_chain_client_to_hash_tbl(ncia_client);
            }  

        }
        else {    /* Client Ctl Blk  already exists */

           /* check for TCP session open collision  */
           if (ncia_client->client_state == NCIA_OPEN_WAIT) {
                  /* conn collision from a client */
               if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
                   (ncia_client_debug & NciaClientDebugEVENT))
              buginf("\nNCIA: Conn collision from client %i(%d)", client_ipaddr,
                          tcb->localport);

               if ((ulong) reg_invoke_ip_tcpdriver_lhost(t) >
                   (ulong) ncia_client->ipaddr) {
            /*
             * our active wins.  passive abort.  ncia_client->t not set yet.
             * Abort passive open
             */
                   if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
                       (ncia_client_debug & NciaClientDebugEVENT)) 
       buginf("\nNCIA: Abort passive open, client %i using lower ip address %i",
                         ncia_client->ipaddr, reg_invoke_ip_tcpdriver_lhost(t));
                   reg_invoke_ip_tcpdriver_abort(t);
                   return;
               }   
               else {
                   ncia_adopt_passive_open_sess(ncia_client, t);
               } 
           }
           else {
               if (!ncia_client->t) {
                  if (ncia_client->client_state == NCIA_SHUTDOWN_PENDING) {
                      ncia_tcpd_abort(t);
                      return;
                  }

                  /* client registered or configured, but no TCP session */
                   ncia_adopt_passive_open_sess(ncia_client, t);
               } 
               else
               if (ncia_client->t != t) {    /* TCP session by ACTIVE OPEN */
                                             /* this should not occur */
                   if (ncia_client->client_state == NCIA_OPEN_WAIT) {
                       if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                           (ncia_client->NciaFlags & NciaClientDebugEVENT)) 
                           buginf("\nNCIA: Client %i TCP session by active open
                                  pending open, abort it", client_ipaddr);
                       ncia_tcpd_abort(ncia_client->t);
                       /* abort pending ACTIVE OPEN tcp session */
                       ncia_adopt_passive_open_sess(ncia_client, t);
	           }
                   else
                   if (ncia_client->client_state == NCIA_CLOSE_WAIT) {
                                           /* CLOSE_CONN already issued */

                       if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                           (ncia_client->NciaFlags & NciaClientDebugEVENT)) 
                           buginf("\nNCIA: Event %s pending, abort ACTIVE OPEN session client %i",
                                   ncia_event_to_string(CLOSE_CONN), client_ipaddr);
                       ncia_tcpd_abort(ncia_client->t);
                       ncia_pre_server_fsm(ncia_client, NDLC_TCP_DOWN);

                       ncia_adopt_passive_open_sess(ncia_client, t);
                   }
                   else {  /* TCP active open session in progress */
                       if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                           (ncia_client->NciaFlags & NciaClientDebugEVENT)) 
                            buginf("\nNCIA: ACTIVE OPEN session in progress for client %i, abort Passive Open session",
                            client_ipaddr);
                       ncia_tcpd_abort(t);
                       return;
	           }
	       }
               else {  
                /* ncia_client->t == t, TCP session by PASSIVE OPEN exists */
                /* weird error, peer client passive opend sess already exists */
                   if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) ||
                       (ncia_client_debug & NciaClientDebugERROR)) 
                buginf("\nNCIA: Error, client passive open tcp session exists");
                       ncia_tcpd_abort(t);
                       return;
	       }
	   }
        }

        ncia_client_fsm(ncia_client, PASSIVE_OPEN);
    }
}


/*
 * ncia_tcpd_fini
 * routine called when the tcpdriver detects a close on
 * the tcpdriver stream.
 */

static
void ncia_tcpd_fini (tcp_encaps *t, void *cookie)
{
    nciapeertype *ncia_client;
 
    ncia_client = (nciapeertype *) cookie;
    if ((ulong) ncia_client == NCIA_FREED_CLIENT)
        return;

    reg_invoke_ip_tcpdriver_update(ncia_client->t, NULL, 1, 
                                   (void *) NCIA_FREED_CLIENT);

    reg_invoke_ip_tcpdriver_close(ncia_client->t);
    ncia_client->t = (tcp_encaps *) NULL;
    mgd_timer_stop(&ncia_client->timer);     

    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
        (ncia_client->NciaFlags & NciaClientDebugEVENT)) { 
        buginf("\nNCIA: Client %i closef scheduled, last state %s",
               ncia_client->ipaddr,
               ncia_state_to_string(ncia_client->client_state));
    }
    ncia_client_fsm(ncia_client, CONN_CLOSED);
}

/*
 * search through the client Queue to see if this client has a connection
 */

ncia_servertype *ncia_find_server_by_ipaddr(ipaddrtype ipaddr)
{
    ncia_servertype *ncia_server;

    ncia_server = (ncia_servertype *) NULL;
    ForEachNciaServer(ncia_group, ncia_server)
	    if (ncia_server->ipaddr == ipaddr)
		return (ncia_server);
    return((ncia_servertype *) NULL);
}

/*
 * search through the client Queue to see if this client has a connection
 */

nciapeertype *ncia_find_client_by_ipaddr(ncia_servertype *ncia_server, 
                                         ipaddrtype ipaddr)
{
    nciapeertype *ncia_client;
    ushort        index;

    ncia_client = (nciapeertype *) NULL;
    index = ncia_hash_ipaddr(ipaddr);
    for (ncia_client = ncia_server->hash_tbl[index].qhead; ncia_client;
         ncia_client = ncia_client->fnext)
	 if (ncia_client->ipaddr == ipaddr)
		return (ncia_client);
    return((nciapeertype *) NULL);
}

static 
void ncia_tcpd_readf(tcp_encaps *t, tcbtype *tcb, 
                     int thresh, void *cookie)
{
    nciapeertype *ncia_client;
    int bytes, numbytes;
    int rc;
    uchar *data;
    ulong pak_size;
    ndlc_header_t *ndlc_hdr;

    ncia_client = (nciapeertype *) cookie;

    if (!ncia_client) {
         buginf ("\nNCIA: Error, NULL client ctl blk ptr in _readf");
         return; 
    }
    if ((ulong) ncia_client == NCIA_FREED_CLIENT) {
        /*
         * flush the TCP driver's bufferr
         */
        do {
            if (tcp_getstring(tcb, &data, &bytes, 0x1000) != OK)
                break;
	} while (bytes != 0);
        return;
    }
    /*
     *  sanity check of the requested count
     */
    if (thresh < sizeof(ndlc_header_t)) {
        ncia_shutdown_client(ncia_client, TRUE);
        return;
    }

    if (ncia_client->rd_state == RD_INIT) {
        /* Start reading a header */
        ncia_client->rd_state = RD_HEADER;
        ncia_client->target = (uchar *) &ncia_client->ndlc_start;
        ncia_client->sizeleft = sizeof(ndlc_header_t);

        if (ncia_client->in_pak) {
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                buginf ("\nNCIA: Error, in_pak not consumed yet");
                        datagram_done(ncia_client->in_pak);
            ncia_client->in_pak = NULL;
        }

        bytes = tcp_unread(tcb);
        if (bytes < thresh) {
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                buginf("\nNCIA: readf() false alarm, thresh %d, bytes %d",
                       sizeof(ndlc_header_t), bytes);
            reg_invoke_ip_tcpdriver_update(t, NULL, thresh, ncia_client);
            return;
        }
    }

    while (TRUE) {
        rc = tcp_getstring(tcb, &data, &bytes, ~0);
        if (rc == NO_DATA) {
            break;
        }
        else
        if (rc != OK) {
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                 buginf("\nNCIA: tcp_getstring() failed - %i(%d) -> %d",
                    tcb->foreignhost, tcb->foreignport, tcb->localport);
            ncia_client->rd_state = RD_INIT;

            if (ncia_client->in_pak) {
                if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                   (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                   buginf ("\nNCIA: Error, in_pak not consumed yet");
                       /*freebuf (ncia_client->in_pak); */
               datagram_done(ncia_client->in_pak);
               ncia_client->in_pak = NULL;
            }
            ncia_shutdown_client(ncia_client, TRUE);
            return;
        }

        while (bytes) {
            numbytes = min(bytes, ncia_client->sizeleft);

            /*
             * Copy the data if a target has been set up.
             */
            if (ncia_client->target) {
                bcopy(data, ncia_client->target, numbytes);
                ncia_client->target += numbytes;
            }

            data += numbytes;
            bytes -= numbytes;
            ncia_client->sizeleft -= numbytes;

            if (ncia_client->sizeleft == 0) {
                switch (ncia_client->rd_state) {
                  case RD_INIT:
                    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                        (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                        buginf("\nNCIA: Error, RD_INIT with no data left");
                    break;
 
                  case RD_HEADER:
                    ndlc_hdr = (ndlc_header_t *) &ncia_client->ndlc_start;
                    if (ndlc_hdr->proto_ver_id != 
                        (NCIA_PROTOCOL_ID | NCIA_PROTO_VERSION)) {
                        ncia_client->rd_state = RD_INIT;
                        ncia_client_fsm(ncia_client, CLOSE_CONN);
                        return;
                    }

                    pak_size = ndlc_hdr->packet_length;
                    if (ncia_client->in_pak) {
                        if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                            (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                            buginf ("\nNCIA: Error, in_pak not consumed yet");
                        datagram_done(ncia_client->in_pak);
                        ncia_client->in_pak = NULL;
		    }
                    ncia_client->in_pak = getbuffer(pak_size);
                    if (!ncia_client->in_pak) {
                        ncia_client->rd_state = RD_INIT;
                        ncia_shutdown_client(ncia_client, TRUE);
                        return;
                    }

                    ncia_client->pkts_rx++;
                    ncia_client->in_pak->datagramsize = pak_size;
                    bcopy(&ncia_client->ndlc_start,
                          ncia_client->in_pak->datagramstart, 
                          sizeof(ndlc_header_t));  
                    ncia_client->rd_state = RD_DATA; 
                    ncia_client->target = ncia_client->in_pak->datagramstart + 
                                   sizeof(ndlc_header_t);
                    ncia_client->sizeleft = pak_size - sizeof(ndlc_header_t);
     
                    break;

                  case RD_DATA:
                    ncia_client->rd_state = RD_HEADER;
                    ncia_client->target = (uchar *) &ncia_client->ndlc_start;
                    ncia_client->sizeleft = sizeof(ndlc_header_t);

#ifdef NCIA_TEST_ROUTER_TO_ROUTER
                    if (ncia_client->NciaFlags & NCIA_CLIENT_CONFIGED)
                        ncia_modify_in_msg(ncia_client);
#endif
                    ncia_client_input(ncia_client);

                    break;
                  default:
                    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                        (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                      buginf("\nNCIA: Error, invalid read_state in tcp packet");
                    if (ncia_client->in_pak) {
                        datagram_done(ncia_client->in_pak);
                        ncia_client->in_pak = NULL;
		    }
	          break;
                }
            }
        }
    }
}



#ifdef NCIA_TEST_ROUTER_TO_ROUTER

/*
 * this change only works for one pair of PC.  Their MAC
 * addresses are defined as the folowing.  We assume they
 * use the same sap.
 */

/* 
DSPU 
char host_addr[IEEEBYTES] = {0x10, 0x00, 0x31, 0x74, 0x00, 0x01};

MVS  
char host_addr[IEEEBYTES] = {0x40, 0x00, 0x10, 0x60, 0x10, 0x00};
*/

char host_addr[IEEEBYTES];

static char client_addr[IEEEBYTES] = {0x40, 0x00, 0x30, 0x00, 0x00, 0x03};

static 
inline void ncia_print_modify_msg(char *s, int ndlc)
{
    switch(ndlc) {
    case NDLC_CAN_U_REACH:
         buginf("%s change NDLC_CAN_U_REACH", s);
         break;
    case NDLC_I_CAN_REACH:
         buginf("%s change NDLC_I_CAN_REACH", s);
         break;
    case NDLC_I_CANNOT_REACH:
         buginf("%s change NDLC_I_CANNOT_REACH", s);
         break;
    case NDLC_START_DL:
         buginf("%s change NDLC_START_DL", s);
         break;
    case NDLC_DL_STARTED:
         buginf("%s change NDLC_DL_STARTED", s);
         break;
    case NDLC_START_DL_FAILED:
         buginf("%s change NDLC_START_DL_FAILED", s);
         break;
    case NDLC_CAP_XCHG:
         buginf("%s change NDLC_CAP_XCHG", s);
         break;
    default:
         break;
    }
}

static 
void ncia_modify_in_msg(nciapeertype *ncia_client)
{
    int                 ndlc;
    ndlc_packet_t      *ndlc_packet;
    ndlc_can_u_reach_t *can_u_reach;
    ndlc_start_dl_t    *start_dl;
    ndlc_cap_xchg_t    *cap_xchg;
    uchar               sap;

    ndlc_packet = (ndlc_packet_t *) ncia_client->in_pak->datagramstart;
    ndlc = ndlc_packet->ndlc_header.message_type;

    ncia_print_modify_msg("\nNCIA: ncia_modify_in_msg", ndlc);

    switch(ndlc) {
    case NDLC_CAN_U_REACH:
    case NDLC_I_CAN_REACH:
    case NDLC_I_CANNOT_REACH:
         can_u_reach = &ndlc_packet->ndlc_data.can_u_reach;
         ieee_copy(&client_addr[0], &can_u_reach->target_mac[0]);
         break;
    case NDLC_START_DL:
    case NDLC_DL_STARTED:
    case NDLC_START_DL_FAILED:
         start_dl = &ndlc_packet->ndlc_data.start_dl;
         ieee_copy(&client_addr[0], &start_dl->target_mac[0]);
         sap = start_dl->target_sap;
         start_dl->target_sap = start_dl->client_sap;
         start_dl->client_sap = sap;
         break;
    case NDLC_CAP_XCHG:
         cap_xchg = &ndlc_packet->ndlc_data.cap_xchg;
         ieee_copy(&host_addr[0], &cap_xchg->peer_client_mac[0]);
         break;
    default:
         break;
    }
}


static 
void ncia_modify_out_msg(nciapeertype *ncia_client)
{
    int                 ndlc;
    ndlc_packet_t      *ndlc_packet;
    ndlc_can_u_reach_t *can_u_reach;
    ndlc_start_dl_t    *start_dl;
    ndlc_cap_xchg_t    *cap_xchg;
    uchar               sap;

    ndlc_packet = (ndlc_packet_t *) ncia_client->out_pak->datagramstart;
    ndlc = ndlc_packet->ndlc_header.message_type;

    ncia_print_modify_msg("\nNCIA: ncia_modify_out_msg:", ndlc);

    switch(ndlc) {
    case NDLC_CAN_U_REACH:
    case NDLC_I_CAN_REACH:
    case NDLC_I_CANNOT_REACH:
         can_u_reach = &ndlc_packet->ndlc_data.can_u_reach;
         ieee_copy(&host_addr[0], &can_u_reach->target_mac[0]);
         break;
    case NDLC_START_DL:
    case NDLC_DL_STARTED:
    case NDLC_START_DL_FAILED:
         start_dl = &ndlc_packet->ndlc_data.start_dl;
         ieee_copy(&host_addr[0], &start_dl->target_mac[0]);
         sap = start_dl->target_sap;
         start_dl->target_sap = start_dl->client_sap;
         start_dl->client_sap = sap;
         break;
    case NDLC_CAP_XCHG:
         cap_xchg = &ndlc_packet->ndlc_data.cap_xchg;
         ieee_copy(&client_addr[0], &cap_xchg->peer_client_mac[0]);
         break;
    default:
         break;
    }
}
#endif


/*
 * Routine to send out a TCP packet for a client 
 */

client_ret_t ncia_send_from_out_pak(nciapeertype *ncia_client)
{
    client_ret_t    rtn_code;
    ndlc_header_t *ndlc_hdr;

    rtn_code = CLIENT_SUCCESS;

#ifdef NCIA_TEST_ROUTER_TO_ROUTER
    /* 
     * modify the mac addr so that we can let two router talk to 
     * each other.  We only do it at the router which the client
     * is configured.
     */

    if (ncia_client->NciaFlags & NCIA_CLIENT_CONFIGED)
        ncia_modify_out_msg(ncia_client);

#endif

    ndlc_hdr = (ndlc_header_t *) ncia_client->out_pak->datagramstart;

    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugEVENT))
         buginf("\nNCIA: Sent %s in tcp packet to client %i (%d bytes)\n",
                ncia_print_ndlc(ndlc_hdr->message_type), ncia_client->ipaddr,
                ndlc_hdr->packet_length);

    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugMSGS))
         ncia_dump_client_packet((uchar *) ndlc_hdr, TRUE); 

    if  ((rtn_code = ncia_tcpd_sender(ncia_client,
         ncia_client->out_pak, TRUE)) == CLIENT_SUCCESS) { 
         ncia_client->out_pak = NULL;
    }
    return(rtn_code);   
}        


/*
 * ncia_tcpd_flow_ctrl()
 *
 */
static 
void ncia_tcpd_flow_ctrl (tcp_encaps *t, ulong level, void *cookie)
{
    nciapeertype *ncia_client;
 
    ncia_client = (nciapeertype *) cookie;
    if (!ncia_client)
        return;
    if (level == (TCP_CONG_HIGH | TCP_CONG_INCREASE)) {
        if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
            (ncia_client->NciaFlags & NciaClientDebugEVENT))
            buginf ("\nNCIA: ncia_tcpd_flow_ctrl called for new TCP_CONG_HIGH");
        ncia_client_fsm(ncia_client, BUSY_XPORT);
        if (ncia_client->client_state == NCIA_OPENED) {
            ncia_pre_server_fsm(ncia_client, BUSY_XPORT);
	}
    }
    else
    if ((level == TCP_CONG_LOW) &&
        (ncia_client->client_state == NCIA_BUSY)) {
        ncia_client_fsm(ncia_client, UNBUSY_XPORT);
        ncia_pre_server_fsm(ncia_client, UNBUSY_XPORT);
    }
}

/*
 * ncia_hash_ipaddr
 *
 * Generate hash table index for client's ipaddr
 */
static
ushort ncia_hash_ipaddr(ipaddrtype ipaddr)
{
    return((ushort) ipaddr & NCIA_HASH_MASK);
}
 
/*
 * ncia_tcpd_sender()
 */
 
static
client_ret_t ncia_tcpd_sender (nciapeertype *ncia_client,
                               paktype *pak, boolean force)
{
    int result;

    if (!ncia_client->t) {
        ncia_client->drops++;
        datagram_done(pak);
        return(CLIENT_UNREACHABLE);
    }
 
    result = reg_invoke_ip_tcpdriver_send(ncia_client->t, pak, force);
    if (TCPDSEND_OK(result)) {
        ncia_client->pkts_tx++;
        return(CLIENT_SUCCESS);
    }
 
    if (result == TCPDSEND_QFULL) {
        if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
            (ncia_client->NciaFlags & NciaClientDebugEVENT))
             buginf("\nNCIA: Outbound tcp queue full (%d) for client %i",
                    QUEUESIZE(&ncia_client->t->outputq), ncia_client->ipaddr);

        return(CLIENT_BUSY);
    }
 
    /*
     * Otherwise result == TCPDSEND_CLOSED
     */
    ncia_client->drops++;
    return(CLIENT_UNREACHABLE);
}
 

/*
 *  ncia_tcpd_close
 *  close the TCP connection for ncia and drains any input.
 */

void ncia_tcpd_close (
    tcp_encaps   *t,
    paktype      *pak)
{
    int status;

    if (!t) {
        return;
    }

    /*
     *  a final message may be sent before closing a connection
     */
    if (pak) {
        status = reg_invoke_ip_tcpdriver_send(t, pak, FALSE);
        if (TCPDSEND_ERROR(status))
            datagram_done(pak);
    }

    reg_invoke_ip_tcpdriver_update(t, NULL, 1, (void *) NCIA_FREED_CLIENT);
    reg_invoke_ip_tcpdriver_close(t);
}

/*
 * ncia_tcpd_slam
 * Slam a TCP connection due to an error.
 */
static
void ncia_tcpd_slam (
    tcp_encaps *t,
    nciapeertype *ncia_client)

{
    if (t) {
        reg_invoke_ip_tcpdriver_update(t, NULL, 1, (void *) NCIA_FREED_CLIENT);
        reg_invoke_ip_tcpdriver_close(t);
        if (!ncia_client)
            buginf("\nNCIA: Client ctl block corrupted, at tcpd_slam?");
    }

}

/*
 * ncia_tcpd_abort
 * aborts the TCP connection for ncia immediately
 */

static
void ncia_tcpd_abort (tcp_encaps  *t)
{
    if (t) {
        reg_invoke_ip_tcpdriver_update(t, NULL, 0, NULL);
        reg_invoke_ip_tcpdriver_abort(t);
    }
}

static
void ncia_init_tcp_session_parameter (nciapeertype *ncia_client)
{

        /*if (ncia_client->ncia_server->tcp_keepalive_int) { */
            reg_invoke_ip_tcpdriver_keepalive(ncia_client->t, TCPD_KP_ENABLE,
                                ncia_client->ncia_server->tcp_keepalive_int);
            reg_invoke_ip_tcpdriver_keepalive(ncia_client->t, TCPD_KP_INTERVAL,
                                   ncia_client->ncia_server->tcp_keepalive_int);
             reg_invoke_ip_tcpdriver_keepalive(ncia_client->t, TCPD_KP_ATTEMPTS,
                                               NCIA_KEEP_CNT_THOLD);
      /*  }
        else
            reg_invoke_ip_tcpdriver_keepalive(ncia_client->t, TCPD_KP_ENABLE, 0); */

        ncia_client->t->tcb->giveuptime = ncia_client->tcp_timeout;
        reg_invoke_ip_tcpdriver_update(ncia_client->t, NULL, 0, ncia_client);
               
	reg_invoke_ip_tcpdriver_set_oqmax(ncia_client->t, 
                                          ncia_client->max_tcp_qlen);
        reg_invoke_ip_tcpdriver_set_flow_ctrl_levels(ncia_client->t,
               ((ncia_client->max_tcp_qlen * NCIA_TCP_HI_WATER_PERCENT) / 100),
               ((ncia_client->max_tcp_qlen * NCIA_TCP_LO_WATER_PERCENT) / 100));

	reg_invoke_ip_tcpdriver_change_timeout(ncia_client->t, TRUE);

}

static
void ncia_client_input (nciapeertype *ncia_client)
{
     
    ndlc_cap_xchg_t *ndlc_cap_xchg_hdr;
    ndlc_header_t *ndlc_hdr;

    ndlc_hdr = (ndlc_header_t *) ncia_client->in_pak->datagramstart;

    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugEVENT))
         buginf("\nNCIA: Rcvd %s in tcp packet for client %i (%d bytes)\n",
                ncia_print_ndlc(ndlc_hdr->message_type), ncia_client->ipaddr,
                ndlc_hdr->packet_length);

    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugMSGS))
         ncia_dump_client_packet((uchar *) ndlc_hdr, FALSE); 

    switch (ndlc_hdr->message_type) {

      case NDLC_START_DL:
      case NDLC_DL_STARTED:
      case NDLC_START_DL_FAILED:
      case NDLC_HALT_DL:
      case NDLC_HALT_DL_NOACK:
      case NDLC_DL_HALTED:
      case NDLC_CONTACT_STN:
      case NDLC_STN_CONTACTED:
      case NDLC_XID_FRAME:
      case NDLC_FCM_FRAME:
      case NDLC_DGRM_FRAME:
      case NDLC_INFO_FRAME:
      case NDLC_DATA_FRAME:
      case NDLC_CAN_U_REACH:
      case NDLC_I_CAN_REACH:
      case NDLC_I_CANNOT_REACH:
        ncia_pre_server_fsm(ncia_client, NDLC_PRIMITIVES);
        break;

      case NDLC_CAP_XCHG:
        ndlc_cap_xchg_hdr = (ndlc_cap_xchg_t *) 
                   ((uchar *) ncia_client->in_pak->datagramstart + NDLC_HDR_SZ);
        if ((ndlc_cap_xchg_hdr->cap_xchg_flag & NCIA_CAP_XCHG_CMD))
            ncia_client_fsm(ncia_client, CAP_RCVD);
        else
            ncia_client_fsm(ncia_client, CAP_RSP_RCVD);
        break;

      case NDLC_PEER_TEST_REQ:
        if (ndlc_hdr->packet_length == sizeof(ndlc_header_t))
            ncia_client_fsm(ncia_client, KEEPALIVE_RCVD);
        else 
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                buginf("\nNCIA: Error, invalid packet length in keepalive req, %d",
                        ndlc_hdr->packet_length);
        break;

      case NDLC_PEER_TEST_RSP:
        if (ndlc_hdr->packet_length == sizeof(ndlc_header_t))
            ncia_client_fsm(ncia_client, KEEPALIVE_RSP_RCVD);
        else 
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                buginf("\nNCIA: Error, invalid packet length in keepalive rsp, %d",
                        ndlc_hdr->packet_length);
        break;

      case NDLC_CLOSE_PEER_REQ:
        ncia_client_fsm(ncia_client, CLOSE_PEER_REQ_RCVD);
        return;
        break;

      case NDLC_CLOSE_PEER_RSP:
        if (ndlc_hdr->packet_length == sizeof(ndlc_header_t)) {
            ncia_client_fsm(ncia_client, CLOSE_PEER_RSP_RCVD);
            return;
	}
        else 
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                 buginf("\nNCIA: Error, invalid packet length in close peer rsp, %d",
                         ndlc_hdr->packet_length);
        break;
              
      default:
        if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
            (ncia_client->NciaFlags & NciaClientDebugERROR))
            buginf("\nNCIA: Error, invalid msg type %d in rcvd tcp packet",
                    ndlc_hdr->message_type);
	break;
    }
    if (ncia_client->in_pak) {
        datagram_done(ncia_client->in_pak);
        ncia_client->in_pak = NULL;
    }
    
}

/*
 * ncia_client_destroy
 */
void ncia_client_destroy (nciapeertype *ncia_client)
{    

    if (ncia_client) {
        if (ncia_client->NciaCircuit.qhead)   /* circuit left */
           return;  

        if (ncia_client->in_pak) {
            datagram_done(ncia_client->in_pak);
            ncia_client->in_pak = NULL;
	}
        if (ncia_client->out_pak) {
            datagram_done(ncia_client->out_pak);
            ncia_client->out_pak = NULL;
	}
   
        if (ncia_client->in_active_open) 
            return;           /* Active Open pending, leave everything untouched */
        if ((ncia_client->NciaFlags & NCIA_CLIENT_CONFIGED) ||
            (ncia_client->NciaFlags & NCIA_CLIENT_REGISTERED)) {
            /*
             * the control block needs to be kept
             */
            ncia_client->client_state = NCIA_CLOSED;
            ncia_client->rd_state = RD_INIT;
            ncia_client->pkts_rx = 0;
            ncia_client->pkts_tx = 0;
            ncia_client->drops   = 0;
        } else { 

            if (ncia_client_mac_in_range(ncia_client))
                ncia_add_to_free_mac_index_chain(ncia_client);

            ncia_unchain_client_from_hash_tbl(ncia_client);
            free(ncia_client);
        }
    }
    else
        if (((ncia_client_debug & NciaClientDebugALL) == NciaClientDebugALL) || 
            (ncia_client_debug & NciaClientDebugERROR)) 
           buginf("\nNCIA: Error, client destroy - NULL ncia client pointer!!");
}

void ncia_terminate_client (nciapeertype *ncia_client)
{

    if (ncia_client->NciaCircuit.qhead) {   /* still circuit left */
        ncia_client->client_state = NCIA_SHUTDOWN_PENDING;
        ncia_pre_server_fsm(ncia_client, NDLC_TCP_DOWN);
    }
    else {  
        ncia_client->client_state = NCIA_CLOSED;
        ncia_client->NciaFlags &= ~NCIA_CLIENT_ACTIVE; 
        ncia_client_destroy(ncia_client);
    } 
}

static
nciapeertype *ncia_client_create (tcp_encaps *t, tcbtype *tcb,
                                      ncia_servertype *ncia_server)
{
    nciapeertype 	*ncia_client;

    ncia_client = malloc(sizeof(nciapeertype));

    if (!ncia_client) {
        printf(nomemory);
	return(NULL);
    }
        
    /* Initialize nciapeertype data structures */
    bzero(ncia_client, sizeof(nciapeertype)); 
    
    queue_init(&ncia_client->OutputQ, 0);
    queue_init(&ncia_client->NciaCircuit, 0);
   
    ncia_client->ipaddr = tcb->foreignhost.ip_addr;
    ncia_client->t = t;
    ncia_client->ncia_server = ncia_server;
    ncia_client->rd_state = RD_INIT;
    ncia_client->client_state = NCIA_CLOSED;
    ncia_client->hash_index = NCIA_NOT_IN_HASH;
    ncia_client->cur_cap_xchg_count = 0;

    ncia_init_tcp_value_in_client_cb(ncia_client);

    return(ncia_client);
}


/*
 * do connect out through ACTIVE_OPEN
 */
static
process ncia_conn_out_proc (void)
{
    nciapeertype  *ncia_client;

    if (process_get_arg_ptr((void**)&ncia_client)) {
       ncia_client_fsm (ncia_client, ACTIVE_OPEN);
    }
    else {
        if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
            (ncia_client->NciaFlags & NciaClientDebugERROR)) 
             buginf ("\nNCIA: Error, get argument ptr failed in conn_out "
                  "background process for client %i", ncia_client->ipaddr);
    }
    process_kill(THIS_PROCESS);
}

void ncia_shutdown_client (nciapeertype  *ncia_client, boolean slam_flag)
{
    mgd_timer_stop(&ncia_client->timer);     
    if (!slam_flag) {
        ncia_tcpd_close(ncia_client->t, ncia_client->out_pak);
        ncia_client->out_pak = NULL;
    }
    else
        ncia_tcpd_slam(ncia_client->t, ncia_client);
    ncia_client->t = NULL;  
    ncia_terminate_client(ncia_client);  
}

/*
 * start a background process  to initiate Connect_Out
 */

void ncia_pre_client_fsm (nciapeertype *ncia_client, 
                          ncia_client_eventtype_t event)
{
  
    if (event == ACTIVE_OPEN) {
        if (ncia_client->client_state == NCIA_CLOSED) {
            if (((ncia_client->NciaFlags & NCIA_CLIENT_CONFIGED) || 
                (ncia_client->NciaFlags & NCIA_CLIENT_REGISTERED)) &&
               (!(ncia_client->NciaFlags & NCIA_CLIENT_ACTIVE))) {
               
        
                bzero(&ncia_client->pid, sizeof(pid_t)); 
                ncia_client->pid = process_create (&ncia_conn_out_proc,
                     "NCIA_Conn_Out_Proc", NORMAL_STACK, PRIO_NORMAL);
                process_set_arg_ptr(ncia_client->pid, ncia_client);
	    }
            else {
                if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                    (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                     buginf ("\nNCIA: Error, active_open for pre_client_fsm"
                        " while client %i is active or not configured, registered",
                          ncia_client->ipaddr);

                ncia_client_destroy(ncia_client);  /* free client ctl blk if no circuit */
                return;
	    }
	}
        else {
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                (ncia_client->NciaFlags & NciaClientDebugERROR)) 
                buginf ("\nNCIA: Error, active_open for pre_client_fsm"
                        " while state!= NCIA_CLOSED for client %i",
                          ncia_client->ipaddr);
	}
    }
    else
    if (event == NDLC_FSM_INPUT) {                 
        ncia_client_fsm(ncia_client, NDLC_FSM_INPUT);
    }
    else {
       if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
           (ncia_client->NciaFlags & NciaClientDebugERROR)) 
            buginf ("\nNCIA: Error, invalid event rcvd in pre_client_fsm"
                    " for client %i", ncia_client->ipaddr);
    }
}

/*
 * ncia_client_active
 * Return an indication if the ncia client manager is enabled.
 * Primarily for TCP usage.
 */

boolean ncia_client_active (ulong sport, ulong dport, ipaddrtype fhost)
{
    return(TRUE);      /* ncia_running was turned on in ncia_start */
}

void ncia_init_tcp_value_in_client_cb (nciapeertype *ncia_client)
{

    ncia_client->drops   = 0;
    ncia_client->pkts_rx = 0;
    ncia_client->pkts_tx = 0;

    ncia_client->keepalive_count = MAX_TIMEOUT;
    ncia_client->tcp_timeout     = NCIA_KEEP_CNT_THOLD * 
                                   ncia_client->ncia_server->tcp_keepalive_int;
    ncia_client->max_tcp_qlen    = NCIA_MAX_TCP_QLEN;

    if (ncia_client_debug)
        NciaSetClientDebug(ncia_client, ncia_client_debug);
                             
    mgd_timer_init_leaf(&ncia_client->timer, &ncia_master_timer,
                        NCIA_CLIENT_TIMER, ncia_client, FALSE);
}

boolean ncia_client_mac_in_range (nciapeertype *ncia_client)
{
    ncia_servertype *ncia_server;

    ncia_server = ncia_client->ncia_server;
    return(ncia_compare_macaddr(ncia_client->Vmac, ncia_server->vmac_start, 
                                ncia_server->vmac_range) ? TRUE: FALSE);
}

boolean ncia_mac_in_range_but_taken (nciapeertype *ncia_client)
{
    ncia_servertype *ncia_server;
    nciapeertype    *client_ptr;

    ncia_server = ncia_client->ncia_server;

    ForEachNciaClient(ncia_server, client_ptr)  
        if (ieee_equal(client_ptr->Vmac, ncia_client->Vmac) && 
            ncia_client->ipaddr != client_ptr->ipaddr)  
            return(TRUE);

    return(FALSE);
}

void ncia_add_to_free_mac_index_chain (nciapeertype *ncia_client)
{
    ncia_servertype *ncia_server;

    ncia_server = ncia_client->ncia_server;

    if (ncia_server->ncia_free_client_index < ncia_server->vmac_range) {
        ncia_server->vmac_list[ncia_client->VmacIndex] = 
                                            ncia_server->ncia_free_client_index;
    }
    else {
        ncia_server->vmac_list[ncia_client->VmacIndex] = 
                                            ncia_server->vmac_range;
    }
    ncia_server->ncia_free_client_index = ncia_client->VmacIndex;
}

void ncia_remove_from_free_mac_index_chain (nciapeertype *ncia_client)
{
    ncia_servertype *ncia_server;
    int              index;

    ncia_server = ncia_client->ncia_server;
    if (ncia_server->ncia_free_client_index == ncia_client->VmacIndex) {
        ncia_server->ncia_free_client_index = 
                                 ncia_server->vmac_list[ncia_client->VmacIndex];
    }
    else {
        for (index = ncia_server->ncia_free_client_index; 
             index <= ncia_server->vmac_range;
             index = ncia_server->vmac_list[index]) {
             if (ncia_server->vmac_list[index] == ncia_client->VmacIndex) { 
                 /* remove slot from free list */
                 ncia_server->vmac_list[index] = 
                                 ncia_server->vmac_list[ncia_client->VmacIndex];
                 break;
	     }
	}
    }
    ncia_server->vmac_list[ncia_client->VmacIndex] = NCIA_MAC_INDEX_USED;
}

boolean ncia_compare_macaddr (uchar *addr1, uchar *addr2,  ushort mac_range)
{
    ulong diff;
   
    diff = GETSHORT(&addr1[0]) - GETSHORT(&addr2[0]);
    if (diff)
        return(FALSE);
    diff = GETLONG(&addr1[2]) - GETLONG(&addr2[2]);
    if (diff < mac_range)
        return(TRUE);
    else 
        return(FALSE);
}

ushort ncia_subtract_macaddr_for_vmacindex (uchar *addr1, uchar *addr2, 
                                            ushort mac_range)
{
    ulong diff;
   
    diff = GETSHORT(&addr1[0]) - GETSHORT(&addr2[0]);
    if (diff) {
  buginf ("\nNCIA: subtract mac addr for index over range after verification?");
        return(mac_range);
    }
    diff = GETLONG(&addr1[2]) - GETLONG(&addr2[2]);
    if (diff >= mac_range) {
  buginf ("\nNCIA: subtract mac addr for index over range after verification?");
        return(mac_range);
    }
    else 
        return((ushort) diff);

}

void ncia_chain_client_to_hash_tbl (nciapeertype *ncia_client)
{
    ncia_servertype  *ncia_server;
    ushort    index;

    ncia_server = ncia_client->ncia_server;
    index = ncia_hash_ipaddr(ncia_client->ipaddr);
    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugERROR))
         buginf("\nNCIA: index for client hash queue is %d", index);

    p_enqueue(&ncia_server->hash_tbl[index], ncia_client);
    ncia_server->hash_tbl_element[index]++;
    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugERROR))
         buginf("\nNCIA: number of element in client hash queue %d is %d",
                 index, ncia_server->hash_tbl_element[index]);
    ncia_client->hash_index = index; 
}


void ncia_unchain_client_from_hash_tbl (nciapeertype *ncia_client)
{
    ncia_servertype *ncia_server;
    ushort    index;

    ncia_server = ncia_client->ncia_server;
    index = ncia_hash_ipaddr(ncia_client->ipaddr);
    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugERROR))
         buginf("\nNCIA: index for client hash queue is %d", index);
    p_unqueue(&ncia_server->hash_tbl[index], ncia_client);
    ncia_server->hash_tbl_element[index]--; 
    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugERROR))
         buginf("\nNCIA: number of element in client hash queue %d is %d",
                 index, ncia_server->hash_tbl_element[index]);
    ncia_client->hash_index = NCIA_NOT_IN_HASH; 
}

static
void ncia_adopt_passive_open_sess (nciapeertype *ncia_client, tcp_encaps *t)
{
    ncia_client->t = t;        /* use to TCP sess by PASSIVE OPEN */
    ncia_client->NciaFlags &= ~NCIA_CLIENT_ACTIVE;
    ncia_client->client_state = NCIA_CLOSED;
    ncia_init_tcp_session_parameter(ncia_client);
}

static
void ncia_dump_client_packet (uchar *ndlc_packet, boolean send_rcv_flag)
{
    char buffer[80];
    char *p = buffer;
    short   i, num_byte;
    ndlc_header_t  *ndlc_hdr;

    ndlc_hdr = (ndlc_header_t *) ndlc_packet;
    num_byte = ndlc_hdr->packet_length;

    p = buffer;

    for (i = 0; i < num_byte; i++) {
        p += sprintf(p, "%-02x", (*(uchar *) (ndlc_packet + i)));
        if (((i+1)%8) == 0) {
            *p = 0x00;
            if (((i+1)%32) == 0 || i == (num_byte-1))
                buginf(" %s\n", buffer);
            else
                buginf(" %s", buffer);
            p = buffer;
        }
    }
    *p = 0x00;
    if (i%8)
        buginf(" %s\n", buffer);
}
