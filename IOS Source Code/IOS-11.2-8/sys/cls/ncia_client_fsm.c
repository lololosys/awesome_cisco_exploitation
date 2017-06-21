/* $Id: ncia_client_fsm.c,v 3.1.2.8 1996/08/31 05:20:21 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_client_fsm.c,v $
 *------------------------------------------------------------------
 * ncia_client_fsm.c - NCIA Client FSM
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_client_fsm.c,v $
 * Revision 3.1.2.8  1996/08/31  05:20:21  schiang
 * CSCdi67779:  debug ncia client message does not display all packet types
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/07/23  13:20:52  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.2.6  1996/07/11  21:13:50  jolee
 * CSCdi62553:  Move externs from .c to .h that was lost in CSCdi61974
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/07/03  06:34:32  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/18  19:01:34  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/01  21:49:22  jolee
 * CSCdi59245:  Added clear ncia client registered & show ncia client
 * saplist
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/22  18:06:19  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * corruppted -> corrupted
 * occured -> occurred
 *
 * Revision 3.1.2.1  1996/05/17  10:43:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.5  1996/05/14  18:42:20  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.4  1996/05/14  04:31:52  jolee
 * CSCdi57570:  Added clear client support, removed backup mac suppport
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.3  1996/05/03  22:06:24  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/04/18  01:21:10  jolee
 * CSCdi55000:  Reset NciaFlags when TCP_LISTEN_MODE not on
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/05  06:05:43  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:16:36  schiang
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
#include "../os/signal.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "../srt/srt_registry.h"
#include "../cls/dlc_registry.h"
#include "../partner/partner_registry.h"
#include "../hes/sse_registry.h"
#include "types.h"
#include "globs.h"
#include "interface_private.h"
#include "packet.h"
#include "../os/buffers.h"
#include "mgd_timers.h"
#include "config.h"
#include "parser.h"
#include "../h/pak_api.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../srt/rsrb.h"
#include "../srt/srb.h"
#include "../srt/srt_debug.h"
#include "../tcp/tcpdriver.h"
#include "../tcp/tcp.h"
#include "../if/if_vidb.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc.h"
#include "../cls/clsi_msg.h"
#include "../cls/cls_assert.h"
#include "../cls/ncia.h"
#include "../cls/ncia_client.h"
#include "../cls/ncia_server_fsm.h"
#include "ncia_debug_flags.h"
#include "ncia_debug.h"


/*
 * File Scope Function Prototypes
 */
static boolean ncia_client_action_02(nciapeertype *ncia_client);
static void ncia_client_action_06(nciapeertype *ncia_client);
static client_ret_t ncia_client_action_07(nciapeertype *ncia_client);
static client_ret_t ncia_client_action_08(nciapeertype *ncia_client);
static client_ret_t ncia_client_action_09(nciapeertype *ncia_client);
static client_ret_t ncia_client_action_11(nciapeertype *ncia_client);
static client_ret_t ncia_client_action_12(nciapeertype *ncia_client);
static client_ret_t ncia_client_action_15(nciapeertype *ncia_client, 
                                          uchar reason_for_close);
static client_ret_t ncia_client_action_16(nciapeertype *ncia_client);
static void ncia_client_action_error(nciapeertype *ncia_client);

/* forward references for utility functions */

static paktype *ncia_make_packet(uchar msg_type, ulong pak_size);
static void ncia_append_cap_xchg_cv_to_packet(nciapeertype * ncia_client);
static boolean ncia_add_cap_xchg_to_packet(nciapeertype * ncia_client, 
                                           boolean cmd_flag);
static boolean ncia_assign_macaddr_to_client(nciapeertype *ncia_client);
static cap_cmd_ret_t ncia_check_cap_cmd_rcvd(nciapeertype *ncia_client);
static void ncia_build_cap_xchg_cmd(nciapeertype *ncia_client);
static void ncia_build_cap_xchg_rsp(nciapeertype *ncia_client);
static boolean ncia_check_if_client_mac_valid(uchar *peer_client_mac, 
                                              nciapeertype *ncia_client);
static void ncia_set_sap_list_to_zero(nciapeertype *ncia_client);
static void ncia_check_output_return(nciapeertype *ncia_client,
                client_ret_t rtn_code, ncia_client_eventtype_t event);
static void ncia_print_event_state(nciapeertype *ncia_client, 
                                   ncia_client_eventtype_t event);
static void ncia_print_error_event_state(nciapeertype *ncia_client, 
                                         ncia_client_eventtype_t event);
static void ncia_free_inpak_terminate(nciapeertype *ncia_client);
static void ncia_process_rcvd_cap_cmd(nciapeertype *ncia_client);
static void ncia_prt_bld_no_mem(nciapeertype *ncia_client, ndlc_msg_type_t msgtype);
static boolean ncia_process_client_time_out(nciapeertype *ncia_client);


/*
 * NCIA client Connection State Machine
 *
 */
 
void ncia_client_fsm (nciapeertype *ncia_client, ncia_client_eventtype_t event)
{
    client_ret_t  rtn_code;

    rtn_code = CLIENT_UNREACHABLE;
    switch(ncia_client->client_state) {

      case NCIA_CLOSED:
        switch(event) {
          case ACTIVE_OPEN:
            /*
             * from NDLC fsm
             */
            ncia_print_event_state(ncia_client, ACTIVE_OPEN);
            ncia_client->client_state = NCIA_OPEN_WAIT;
            if (ncia_client_action_02(ncia_client)) { 
                             /* Active Open TCP session  successfuly */
                if (ncia_client->client_state == NCIA_CAP_WAIT) {
                             /* Being overriden Passive Open */             
                   if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                       (ncia_client->NciaFlags & NciaClientDebugEVENT))
                       buginf("\nNCIA: Active open TCP pipe overridden by "
                               "client %i passive open", ncia_client->ipaddr);
		}
                else { 
                   if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                       (ncia_client->NciaFlags & NciaClientDebugEVENT))
                       buginf("\nNCIA: Active open for client %i completed",
                               ncia_client->ipaddr);
                   ncia_client_fsm(ncia_client, CONN_OPENED);
		}
  	    }
            else {
                if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                    (ncia_client->NciaFlags & NciaClientDebugEVENT))
                     buginf("\nNCIA: Active open failed, ctl blk = %x, state = %s",
                             ncia_client, ncia_state_to_string(ncia_client->client_state));
                ncia_client_destroy(ncia_client); 
            }      
            break;

          case PASSIVE_OPEN:
   
            ncia_print_event_state(ncia_client, PASSIVE_OPEN);
            ncia_client->client_state = NCIA_CAP_WAIT;
            mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);
            break;

          case CLOSE_CONN:  

            ncia_print_event_state(ncia_client, CLOSE_CONN);
            break;

          case CONN_OPENED:

            ncia_print_error_event_state(ncia_client, CONN_OPENED);
            break;

          case CONN_CLOSED:

            ncia_print_error_event_state(ncia_client, CONN_CLOSED);
            break;
          case BUSY_XPORT:

            ncia_print_error_event_state(ncia_client, BUSY_XPORT);
            break;

          case UNBUSY_XPORT:

            ncia_print_error_event_state(ncia_client, UNBUSY_XPORT);
            break;

          case TIME_OUT:
            break;

          case CAP_RCVD:
          case CAP_RSP_RCVD:
          case KEEPALIVE_RCVD:
          case KEEPALIVE_RSP_RCVD:
          case NDLC_PRIMITIVES:
          case CLOSE_PEER_REQ_RCVD:
          case CLOSE_PEER_RSP_RCVD:
            break;

          case NDLC_FSM_INPUT:

            /* This should not occur.  the client FSM will be called with
             * NDLC_FSM_INPUT only if a client is not in NCIA_CLOSED state,
             * Otherwise, the FSM should be called with ACTIVE_OPEN first
             */ 
            ncia_print_error_event_state(ncia_client, NDLC_FSM_INPUT);
            break;     

          case CIRCUIT_FREED:

            ncia_print_event_state(ncia_client, CIRCUIT_FREED);
            if (!ncia_client->NciaCircuit.qhead)    /* no circuit left */
                ncia_client_destroy(ncia_client);
            break;
          default:
            /*
             * something wrong seriously.
             */
            ncia_client_action_error(ncia_client);
            break;
        }
        break;

      case NCIA_OPEN_WAIT:
        switch(event) {
          case ACTIVE_OPEN: 
            ncia_print_error_event_state(ncia_client, ACTIVE_OPEN);
            break;

          case PASSIVE_OPEN:
            ncia_print_event_state(ncia_client, PASSIVE_OPEN);
            ncia_client->client_state = NCIA_CAP_WAIT;
            /*
             * connect-in won because client has the higher ip address.
             *  the active open will check the state to avoid contention
             */

            mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);   
            break;

          case CLOSE_CONN:

            ncia_print_event_state(ncia_client, CLOSE_CONN);
            ncia_client->client_state = NCIA_CLOSE_WAIT;
            /* If problem occurs, add a flag in ncia_client indicating close 
             * con requested during active_open, and add code in active_open
             * to abort TCP session right after it comes up */
            break;

          case CONN_OPENED:

            ncia_print_event_state(ncia_client, CONN_OPENED);
            rtn_code = ncia_client_action_08(ncia_client);  
            /* send cap exchange */
            ncia_check_output_return(ncia_client, rtn_code, CONN_OPENED);
            if ((rtn_code == NO_MEMORY_FOR_PACKET) || 
                (rtn_code == CLIENT_UNREACHABLE)) {
               ncia_shutdown_client(ncia_client, TRUE);
            }
            else { 
               ncia_client->client_state = NCIA_CAP_NEG;
               mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);   
	    }
            break;

          case TIME_OUT:

            ncia_print_error_event_state(ncia_client, TIME_OUT);
            ncia_client->client_state = NCIA_CLOSED;
            ncia_client->NciaFlags &= ~NCIA_CLIENT_ACTIVE;
            break;

          case BUSY_XPORT:
          case UNBUSY_XPORT:
            break;

          case CAP_RCVD:
          case CAP_RSP_RCVD:
          case KEEPALIVE_RCVD:
          case KEEPALIVE_RSP_RCVD:
          case NDLC_PRIMITIVES:

            break;

          case NDLC_FSM_INPUT:
            ncia_print_error_event_state(ncia_client, NDLC_FSM_INPUT);
            break;

	  case CONN_CLOSED:
            ncia_print_event_state(ncia_client, CONN_CLOSED);
            ncia_terminate_client(ncia_client);
            break;

          case CLOSE_PEER_REQ_RCVD:
            ncia_print_error_event_state(ncia_client, CLOSE_PEER_REQ_RCVD); 
            rtn_code = ncia_client_action_16(ncia_client);/* send clo peer rsp*/
            ncia_free_inpak_terminate(ncia_client);
            break;
      
          case CLOSE_PEER_RSP_RCVD:

            ncia_print_error_event_state(ncia_client, CLOSE_PEER_RSP_RCVD); 
            ncia_free_inpak_terminate(ncia_client);
            break;
          case CIRCUIT_FREED:

            ncia_print_event_state(ncia_client, CIRCUIT_FREED);
            if (!ncia_client->NciaCircuit.qhead)    /* no circuit left */
                ncia_client_destroy(ncia_client);
            break;
          default:
            /*
             * something wrong seriously.
             */
            ncia_client_action_error(ncia_client);
            break;
        }
        break;

      case NCIA_CAP_WAIT:
        switch(event) {
          case ACTIVE_OPEN:
            break;

          case PASSIVE_OPEN: 
            /* Should never occur, the new remote TCP session should be
               aborted in the ncia_tcpd_listen */

            ncia_print_error_event_state(ncia_client, PASSIVE_OPEN); 
            break;

          case CLOSE_CONN:

            ncia_print_event_state(ncia_client, CLOSE_CONN); 
            rtn_code = ncia_client_action_15(ncia_client, NCIA_CLO_SYS_DOWN);
                                                /* send close peer req */
            ncia_check_output_return(ncia_client, rtn_code, CLOSE_CONN); 
            if ((rtn_code == NO_MEMORY_FOR_PACKET) || 
                (rtn_code == CLIENT_UNREACHABLE)) {
               ncia_shutdown_client(ncia_client, TRUE);
            }
            else { 
               ncia_client->client_state = NCIA_CLOSE_WAIT;
               mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);
	    }

            break;

          case CONN_OPENED:

            ncia_print_error_event_state(ncia_client, CONN_OPENED); 
            break;

          case CAP_RCVD:

            ncia_process_rcvd_cap_cmd(ncia_client);
            break;

          case CAP_RSP_RCVD:

            ncia_print_error_event_state(ncia_client, CAP_RSP_RCVD); 
            break;

          case KEEPALIVE_RCVD: 

            ncia_print_error_event_state(ncia_client, KEEPALIVE_RCVD); 
            break;

          case KEEPALIVE_RSP_RCVD: 

            ncia_print_error_event_state(ncia_client, KEEPALIVE_RSP_RCVD); 
            break;

          case TIME_OUT:   
            /*  we are waiting for a CAP_CMD from the peer, a timer was
             *  started to keep track of if we received it in time
             */                

            ncia_process_client_time_out(ncia_client);
            break;

          case BUSY_XPORT:  
                          
            mgd_timer_stop(&ncia_client->timer);     
            ncia_print_event_state(ncia_client, BUSY_XPORT); 
                           /* just wait for quiet down */ 
            break;

          case UNBUSY_XPORT:
                            
            ncia_print_event_state(ncia_client, UNBUSY_XPORT); 
            mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);
            break;

          case NDLC_FSM_INPUT: 

            ncia_print_error_event_state(ncia_client, NDLC_FSM_INPUT); 
            break;

          case NDLC_PRIMITIVES:

            ncia_print_error_event_state(ncia_client, NDLC_PRIMITIVES); 
            break;

	  case CONN_CLOSED:

            ncia_print_event_state(ncia_client, CONN_CLOSED); 
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugEVENT))
                 buginf("\nNCIA: Timer value = %x (%x)", 
                        &ncia_client->timer, ncia_client->timer);
            ncia_terminate_client(ncia_client);
            break;

          case CLOSE_PEER_REQ_RCVD:

            ncia_print_event_state(ncia_client, CLOSE_PEER_REQ_RCVD); 
            rtn_code = ncia_client_action_16(ncia_client); 
            /* send close peer rsp */
            ncia_check_output_return(ncia_client, rtn_code, 
                                     CLOSE_PEER_REQ_RCVD);
            ncia_free_inpak_terminate(ncia_client);

            break;
      
          case CLOSE_PEER_RSP_RCVD:

            ncia_print_event_state(ncia_client, CLOSE_PEER_RSP_RCVD); 
            ncia_free_inpak_terminate(ncia_client);
            break;

          case CIRCUIT_FREED:

            ncia_print_error_event_state(ncia_client, CIRCUIT_FREED);
            break;

          default:
            /*
             * something wrong seriously.
             */
            ncia_client_action_error(ncia_client);
            break;
        }
        break;

      case NCIA_CAP_NEG:
        switch(event) {
          case ACTIVE_OPEN:

            ncia_print_error_event_state(ncia_client, ACTIVE_OPEN);
            break;

          case PASSIVE_OPEN:
            /* Abort remote TCP ??? */
            ncia_print_error_event_state(ncia_client, PASSIVE_OPEN); 
            break;

          case CLOSE_CONN:

            ncia_print_event_state(ncia_client, CLOSE_CONN);
            rtn_code = ncia_client_action_15(ncia_client, NCIA_CLO_SYS_DOWN);
            /* send close peer req */
            ncia_check_output_return(ncia_client, rtn_code, CLOSE_CONN); 
            if ((rtn_code == NO_MEMORY_FOR_PACKET) || 
                (rtn_code == CLIENT_UNREACHABLE)) {
               ncia_shutdown_client(ncia_client, TRUE);
            }
            else { 
               ncia_client->client_state = NCIA_CLOSE_WAIT;
               mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT); 
	    }
            break;

          case CONN_OPENED:
            break;

          case CAP_RCVD:

            ncia_process_rcvd_cap_cmd(ncia_client);
            break;

          case CAP_RSP_RCVD:

            mgd_timer_stop(&ncia_client->timer);         
            ncia_print_event_state(ncia_client, CAP_RSP_RCVD);
            ncia_client->client_state = NCIA_OPENED;
            ncia_client->NciaFlags |= NCIA_CLIENT_ACTIVE;
            ncia_client->cur_cap_xchg_count = 0; /* reset for if reconnect */
            ncia_client->keepalive_count = 0; /* kick in keepalive */
            ncia_pre_server_fsm(ncia_client, CONN_OPENED);
            if (ncia_client->ncia_server->ndlc_keepalive_int)
                mgd_timer_start(&ncia_client->timer,
                                ncia_client->ncia_server->ndlc_keepalive_int);

            break;

          case KEEPALIVE_RCVD:
          case KEEPALIVE_RSP_RCVD:
            break;

          case TIME_OUT:
            /*
             * keepalive time out
             */

            if (ncia_process_client_time_out(ncia_client)) {
                rtn_code = ncia_client_action_08(ncia_client);
                if ((rtn_code == NO_MEMORY_FOR_PACKET) || 
                    (rtn_code == CLIENT_UNREACHABLE)) {
                    ncia_shutdown_client(ncia_client, TRUE);
                }
                else {
                    /*ncia_print_event_state(ncia_client, TIME_OUT); */
                    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                        (ncia_client->NciaFlags & NciaClientDebugEVENT))
                         buginf("\nNCIA: cap xchg req sent to client %i",
                                   ncia_client->ipaddr);
                    mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);
                }
	    }
            break;
 
          case BUSY_XPORT:

            mgd_timer_stop(&ncia_client->timer);     
            ncia_print_event_state(ncia_client, BUSY_XPORT);
            break;

          case UNBUSY_XPORT:

            ncia_print_event_state(ncia_client, UNBUSY_XPORT);
            mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);
            break;

          case NDLC_FSM_INPUT:           /* NDLC primitives from host side */

            ncia_print_error_event_state(ncia_client, NDLC_FSM_INPUT); 
            break;

          case NDLC_PRIMITIVES:

            ncia_print_error_event_state(ncia_client, NDLC_PRIMITIVES);
            break;

	  case CONN_CLOSED:

            ncia_print_event_state(ncia_client, CONN_CLOSED);
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugEVENT))
                 buginf("\nNCIA: Timer value = %x (%x)", 
                         &ncia_client->timer, ncia_client->timer);
            ncia_terminate_client(ncia_client);
            break;

          case CLOSE_PEER_REQ_RCVD:

            ncia_print_event_state(ncia_client, CLOSE_PEER_REQ_RCVD);
            rtn_code = ncia_client_action_16(ncia_client);  
            /* send clo peer rsp */ 
            ncia_check_output_return(ncia_client, rtn_code,
                                     CLOSE_PEER_REQ_RCVD);
            ncia_free_inpak_terminate(ncia_client);  

            break;
      
          case CLOSE_PEER_RSP_RCVD:

            ncia_print_event_state(ncia_client, CLOSE_PEER_RSP_RCVD);
            ncia_free_inpak_terminate(ncia_client);

            break;

          case CIRCUIT_FREED:

            ncia_print_error_event_state(ncia_client, CIRCUIT_FREED);
            break;

          default:
            /*
             * something wrong seriously.
             */
            ncia_client_action_error(ncia_client);
            break;
        }
        break;

      case NCIA_OPENED:
        switch(event) {
          case ACTIVE_OPEN:
            ncia_print_error_event_state(ncia_client, ACTIVE_OPEN);
            break;

          case PASSIVE_OPEN:
            /*
             * this client is in connect state already
             */
            ncia_print_error_event_state(ncia_client, PASSIVE_OPEN);
            /*
             *   the new incoming sess by PASSIVE OPEN is aborted 
             *   in ncia_client_input
             */

            break;

          case CLOSE_CONN:

            ncia_print_event_state(ncia_client, CLOSE_CONN);
            rtn_code = ncia_client_action_15(ncia_client, NCIA_CLO_SYS_DOWN);
                                        /* send close peer req */
            ncia_check_output_return(ncia_client, rtn_code, CLOSE_CONN);
            if ((rtn_code == NO_MEMORY_FOR_PACKET) ||
                (rtn_code == CLIENT_UNREACHABLE)) {
               ncia_shutdown_client(ncia_client, TRUE);
            }
            else { 
               ncia_client->client_state = NCIA_CLOSE_WAIT;
               mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT); 
	    }
            break;

          case CONN_OPENED: 

            ncia_print_error_event_state(ncia_client, CONN_OPENED);
            break;
 
          case CAP_RCVD: 

            ncia_print_error_event_state(ncia_client, CAP_RCVD);
            break;

          case CAP_RSP_RCVD: 

            ncia_print_error_event_state(ncia_client, CAP_RSP_RCVD);
            break;

          case KEEPALIVE_RCVD:

            ncia_print_event_state(ncia_client, KEEPALIVE_RCVD);
            rtn_code = ncia_client_action_12(ncia_client); 
            ncia_check_output_return(ncia_client, rtn_code,
                                        KEEPALIVE_RCVD);
            if ((rtn_code == NO_MEMORY_FOR_PACKET) ||
                (rtn_code == CLIENT_UNREACHABLE)) {
               ncia_shutdown_client(ncia_client, TRUE);
            }
            else
                if (ncia_client->ncia_server->ndlc_keepalive_int)
                    mgd_timer_start(&ncia_client->timer,
                                ncia_client->ncia_server->ndlc_keepalive_int);
            break;

          case KEEPALIVE_RSP_RCVD:

            ncia_print_event_state(ncia_client, KEEPALIVE_RSP_RCVD);
            ncia_client->keepalive_count = 0;
            if (ncia_client->ncia_server->ndlc_keepalive_int)
                mgd_timer_start(&ncia_client->timer,
                                ncia_client->ncia_server->ndlc_keepalive_int);
            break;

          case TIME_OUT: 
            /*
             * keepalive time out
             */

            if (ncia_process_client_time_out(ncia_client)) {
                rtn_code = ncia_client_action_11(ncia_client);
                if ((rtn_code == NO_MEMORY_FOR_PACKET) ||
                    (rtn_code == CLIENT_UNREACHABLE)) {
                    ncia_shutdown_client(ncia_client, TRUE);
                }
                else {
                    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                        (ncia_client->NciaFlags & NciaClientDebugEVENT))
                         buginf("\nNCIA: keepalive req sent to client %i",
                                   ncia_client->ipaddr);
                    if (ncia_client->ncia_server->ndlc_keepalive_int)
                        mgd_timer_start(&ncia_client->timer,
                                ncia_client->ncia_server->ndlc_keepalive_int);
                }
	    }
            break;

          case BUSY_XPORT:

            mgd_timer_stop(&ncia_client->timer);     
            ncia_print_event_state(ncia_client, BUSY_XPORT);
            ncia_client->client_state = NCIA_BUSY;
            break;

          case UNBUSY_XPORT:  

            ncia_print_error_event_state(ncia_client, UNBUSY_XPORT);
            break;

          case NDLC_FSM_INPUT:
            ncia_print_event_state(ncia_client, NDLC_FSM_INPUT);
            if (!ncia_client->out_pak) {
                if (ncia_client->NciaFlags & NciaClientDebugEVENT) 
                 buginf("\nNCIA: event %s, state %s, for client %i, no out_pak",
                        ncia_event_to_string(NDLC_FSM_INPUT),
                        ncia_state_to_string(ncia_client->client_state), 
                        ncia_client->ipaddr);
                return;
            }
            rtn_code = ncia_client_action_07(ncia_client);
            /* forward to peer */
            ncia_check_output_return(ncia_client, rtn_code, NDLC_FSM_INPUT); 
            if ((rtn_code == NO_MEMORY_FOR_PACKET) ||
                (rtn_code == CLIENT_UNREACHABLE)) {
                ncia_shutdown_client(ncia_client, TRUE);
            }
            else
                if (ncia_client->ncia_server->ndlc_keepalive_int)
                    mgd_timer_start(&ncia_client->timer,
                                ncia_client->ncia_server->ndlc_keepalive_int);
           
            break;

          case NDLC_PRIMITIVES:
                                             /* Forward to NDLC_FSM */
            ncia_print_event_state(ncia_client, NDLC_PRIMITIVES);
            if (ncia_client->in_pak) {
                ncia_client_action_06(ncia_client);
                if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                    (ncia_client->NciaFlags & NciaClientDebugEVENT))

                    buginf("\nNCIA: Forwarding input from peer client %i"
                            " to host", ncia_client->ipaddr);
            }
            else {
              if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                  (ncia_client->NciaFlags & NciaClientDebugERROR))

                 buginf("\nNCIA: Error, no input pak for client %i",
                           ncia_client->ipaddr);
            }
      
            break;

	  case CONN_CLOSED:

            ncia_print_event_state(ncia_client, CONN_CLOSED);
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugEVENT))
                 buginf("\nNCIA: Timer value = %x (%x)",
                         &ncia_client->timer, ncia_client->timer);
            ncia_terminate_client(ncia_client);
            break;

          case CLOSE_PEER_REQ_RCVD:

            ncia_print_event_state(ncia_client, CLOSE_PEER_REQ_RCVD);
            rtn_code = ncia_client_action_16(ncia_client); 
            /* send clo peer rsp */ 
            ncia_check_output_return(ncia_client, rtn_code,
                                     CLOSE_PEER_REQ_RCVD);
            ncia_free_inpak_terminate(ncia_client);
            break;
      
          case CLOSE_PEER_RSP_RCVD:

            ncia_print_event_state(ncia_client, CLOSE_PEER_RSP_RCVD);
            ncia_free_inpak_terminate(ncia_client);
            break;

          case CIRCUIT_FREED:

            ncia_print_event_state(ncia_client, CIRCUIT_FREED);
            break;

          default:
            /*
             * something wrong seriously.
             */
            ncia_client_action_error(ncia_client);
            break;

        }
        break;

      case NCIA_BUSY:
        switch(event) {
          case ACTIVE_OPEN:
            break;

          case PASSIVE_OPEN:
            /* No need to Abort remote TCP, it's done in ncia_client_input */
            break;

          case CLOSE_CONN:

            ncia_print_event_state(ncia_client, CLOSE_CONN);
            rtn_code = ncia_client_action_15(ncia_client, NCIA_CLO_SYS_DOWN);
            /* send close peer req */
            ncia_check_output_return(ncia_client, rtn_code, CLOSE_CONN);
            if ((rtn_code == NO_MEMORY_FOR_PACKET) || 
                (rtn_code == CLIENT_UNREACHABLE)) {
               ncia_shutdown_client(ncia_client, TRUE);
            }
            else { 
               ncia_client->client_state = NCIA_CLOSE_WAIT;
               mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);
	    }

            break;

          case CONN_OPENED:
            break;

          case CAP_RCVD:
          case CAP_RSP_RCVD:
            break;

          case KEEPALIVE_RCVD:   

            ncia_print_error_event_state(ncia_client, KEEPALIVE_RCVD);
            rtn_code = ncia_client_action_12(ncia_client); 
            ncia_check_output_return(ncia_client, rtn_code,
                                        KEEPALIVE_RCVD);
            if ((rtn_code == NO_MEMORY_FOR_PACKET) ||
                (rtn_code == CLIENT_UNREACHABLE)) {
               ncia_shutdown_client(ncia_client, TRUE);
            }
            else 
                if (ncia_client->ncia_server->ndlc_keepalive_int)
                    mgd_timer_start(&ncia_client->timer,
                                ncia_client->ncia_server->ndlc_keepalive_int);
            break;
 
         case KEEPALIVE_RSP_RCVD:
                                 
            ncia_print_error_event_state(ncia_client, KEEPALIVE_RSP_RCVD);
            ncia_client->keepalive_count = 0;
            if (ncia_client->ncia_server->ndlc_keepalive_int)
                mgd_timer_start(&ncia_client->timer,
                                ncia_client->ncia_server->ndlc_keepalive_int);
            break;

          case TIME_OUT:  
            /*              
             * keepalive time out
             */

            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugEVENT))
                buginf("\nNCIA: event %s, state %s, for client %i,"
                        " keepalive_count = %d",
                        ncia_event_to_string(TIME_OUT),
                        ncia_state_to_string(ncia_client->client_state), 
                        ncia_client->ipaddr,
                        ncia_client->keepalive_count);
            break;

          case BUSY_XPORT:

            ncia_print_error_event_state(ncia_client, BUSY_XPORT);
            break;

          case UNBUSY_XPORT:

            ncia_print_event_state(ncia_client, UNBUSY_XPORT);
            ncia_client->client_state = NCIA_OPENED;
            rtn_code = ncia_client_action_07(ncia_client);  
            /* forward to peer */
            ncia_check_output_return(ncia_client, rtn_code, UNBUSY_XPORT); 
            if ((rtn_code == NO_MEMORY_FOR_PACKET) ||
                (rtn_code == CLIENT_UNREACHABLE)) {
               ncia_shutdown_client(ncia_client, TRUE);
            }
            else
                if (ncia_client->ncia_server->ndlc_keepalive_int)
                    mgd_timer_start(&ncia_client->timer,
                              ncia_client->ncia_server->ndlc_keepalive_int);

            break;

          case NDLC_FSM_INPUT:

            ncia_print_error_event_state(ncia_client, NDLC_FSM_INPUT);

            break;

          case NDLC_PRIMITIVES:
    
            ncia_print_error_event_state(ncia_client, NDLC_PRIMITIVES);
            ncia_client_action_06(ncia_client);    /* forward to NDLC_FSM */
     
            break;

          case CONN_CLOSED:

            ncia_print_event_state(ncia_client, CONN_CLOSED);
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugEVENT))
                 buginf("\nNCIA: Timer value = %x (%x)", 
                           &ncia_client->timer, ncia_client->timer);
            ncia_terminate_client(ncia_client);
            break;

          case CLOSE_PEER_REQ_RCVD:
               
            ncia_print_error_event_state(ncia_client, CLOSE_PEER_REQ_RCVD);
            rtn_code = ncia_client_action_16(ncia_client); 
            /* send clo peer rsp */ 
            ncia_check_output_return(ncia_client, rtn_code,
                                     CLOSE_PEER_REQ_RCVD);
            ncia_free_inpak_terminate(ncia_client);
            break;
      
          case CLOSE_PEER_RSP_RCVD:

            ncia_print_error_event_state(ncia_client, CLOSE_PEER_RSP_RCVD);
            ncia_free_inpak_terminate(ncia_client);
            break;

          case CIRCUIT_FREED:

            ncia_print_error_event_state(ncia_client, CIRCUIT_FREED);
            break;
          default:
            /*
             * something wrong seriously.
             */
            ncia_client_action_error(ncia_client);
            break;
        }
        break;

      case NCIA_CLOSE_WAIT:
        switch(event) {
          case ACTIVE_OPEN:
            break;

          case PASSIVE_OPEN: 
 
            /* Should never occur, the new remote TCP session should 
               have been aborted in the ncia_tcpd_listen */

            break;

          case CLOSE_CONN:
            break;

          case CONN_OPENED:
            break;

          case CAP_RCVD:
            break;

          case CAP_RSP_RCVD:
             break;

          case KEEPALIVE_RCVD:
          case KEEPALIVE_RSP_RCVD:
            break;

          case TIME_OUT:
            
            if (ncia_process_client_time_out(ncia_client)) {
                rtn_code = ncia_client_action_15(ncia_client, NCIA_CLO_SUSPEND);
                if ((rtn_code == NO_MEMORY_FOR_PACKET) ||
                    (rtn_code == CLIENT_UNREACHABLE)) {
                    ncia_shutdown_client(ncia_client, TRUE);
                }
                else {
                    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                        (ncia_client->NciaFlags & NciaClientDebugEVENT))
                         buginf("\nNCIA: close peer req sent to client %i",
                                   ncia_client->ipaddr);
                    mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);
                }
	    }
            break;

          case BUSY_XPORT:
               
            ncia_print_error_event_state(ncia_client, BUSY_XPORT);
            break;

          case UNBUSY_XPORT:
               
            ncia_print_event_state(ncia_client, UNBUSY_XPORT);
            break;

          case NDLC_FSM_INPUT:
               
            ncia_print_event_state(ncia_client, NDLC_FSM_INPUT);
            break;
               
          case NDLC_PRIMITIVES:

            ncia_print_error_event_state(ncia_client, NDLC_PRIMITIVES);
            break;

	  case CONN_CLOSED:

            ncia_print_event_state(ncia_client, CONN_CLOSED);
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugEVENT))
                 buginf("\nNCIA: Timer value = %x (%x)",
                        &ncia_client->timer, ncia_client->timer);
            ncia_terminate_client(ncia_client);
            break;

          case CLOSE_PEER_REQ_RCVD:

            ncia_print_event_state(ncia_client, CLOSE_PEER_REQ_RCVD);
            rtn_code = ncia_client_action_16(ncia_client); /*send clo peer rsp*/ 
            ncia_check_output_return(ncia_client, rtn_code,
                                     CLOSE_PEER_REQ_RCVD);
            ncia_free_inpak_terminate(ncia_client);

            break;
      
          case CLOSE_PEER_RSP_RCVD:

            ncia_print_event_state(ncia_client, CLOSE_PEER_RSP_RCVD);
            ncia_free_inpak_terminate(ncia_client);

            break;

          case CIRCUIT_FREED:

            ncia_print_event_state(ncia_client, CIRCUIT_FREED);
            break;
          default:
            /*
             * something wrong seriously.
             */
            ncia_client_action_error(ncia_client);
            break;
        }
        break;

      case NCIA_SHUTDOWN_PENDING:
        switch(event) {
          case ACTIVE_OPEN:
            /*
             * from NDLC fsm
             */
            ncia_print_error_event_state(ncia_client, ACTIVE_OPEN);
            break;

          case PASSIVE_OPEN:
   
            ncia_print_error_event_state(ncia_client, PASSIVE_OPEN);
            break;

          case CLOSE_CONN:  

            ncia_print_error_event_state(ncia_client, CLOSE_CONN);
            break;

          case CONN_OPENED:

            ncia_print_error_event_state(ncia_client, CONN_OPENED);
            break;

          case CONN_CLOSED:

            ncia_print_error_event_state(ncia_client, CONN_CLOSED);
            break;
          case BUSY_XPORT:

            ncia_print_error_event_state(ncia_client, BUSY_XPORT);
            break;

          case UNBUSY_XPORT:

            ncia_print_error_event_state(ncia_client, UNBUSY_XPORT);
            break;

          case TIME_OUT:

            ncia_print_error_event_state(ncia_client, TIME_OUT);
            break;

          case CAP_RCVD:
          case CAP_RSP_RCVD:
          case KEEPALIVE_RCVD:
          case KEEPALIVE_RSP_RCVD:
          case NDLC_PRIMITIVES:
          case CLOSE_PEER_REQ_RCVD:
          case CLOSE_PEER_RSP_RCVD:
            break;

          case NDLC_FSM_INPUT:

            ncia_print_error_event_state(ncia_client, NDLC_FSM_INPUT);
            break;     

          case CIRCUIT_FREED:

            ncia_print_event_state(ncia_client, CIRCUIT_FREED);
            ncia_client->client_state = NCIA_CLOSED;
            ncia_client->NciaFlags &= ~NCIA_CLIENT_ACTIVE;
            ncia_client_destroy(ncia_client);
            break;

          default:
            /*
             * something wrong seriously.
             */
            ncia_client_action_error(ncia_client);
            break;

        }
        break;
    }
}


char *ncia_state_to_string (ncia_client_state_t state)
{
    switch (state) {
      case NCIA_CLOSED:           return("NCIA_CLOSED");
        break;
      case NCIA_OPEN_WAIT:        return("NCIA_OPEN_WAIT");
        break;
      case NCIA_CAP_WAIT:         return("NCIA_CAP_WAIT");
        break;
      case NCIA_CAP_NEG:          return("NCIA_CAP_NEG");
        break;
      case NCIA_OPENED:           return("NCIA_OPENED");
        break;
      case NCIA_BUSY:             return("NCIA_BUSY");
        break;
      case NCIA_CLOSE_WAIT:       return("NCIA_CLOSE_WAIT");
        break;
      case NCIA_SHUTDOWN_PENDING: return("NCIA_SHUTDOWN_PENDING");
        break;
      default:                    return("UNKNOWN");
        break;
    }
}


char *ncia_event_to_string (ncia_client_eventtype_t event)
{
    switch (event) {
      case ACTIVE_OPEN:         return("ACTIVE_OPEN");
        break;
      case PASSIVE_OPEN:        return("PASSIVE_OPEN");
        break;
      case CLOSE_CONN:          return("CLOSE_CONNECTION");
        break;
      case CONN_OPENED:         return("CONNECTION_OPENED");
        break;
      case CAP_RCVD:            return("CAP_CMD_RCVD");
        break;
      case CAP_RSP_RCVD:        return("CAP_RSP_RCVD");
        break;
      case KEEPALIVE_RCVD:      return("KEEPALIVE_RCVD");
        break;
      case KEEPALIVE_RSP_RCVD:  return("KEEPALIVE_RSP_RCVD");
        break;
      case TIME_OUT:            return("TIME_OUT");
         break; 
      case BUSY_XPORT:          return("XPORT_BUSY");
        break;
      case UNBUSY_XPORT:        return("XPORT_NOT_BUSY");
        break;
      case NDLC_FSM_INPUT:      return("NDLC_FSM_INPUT");
        break;
      case NDLC_PRIMITIVES:     return("NDLC_PRIMITIVES");
        break;
      case CONN_CLOSED:         return("CONNECTION_CLOSED");
        break;
      case CLOSE_PEER_REQ_RCVD: return("CLOSE_PEER_REQ_RCVD");
        break;
      case CLOSE_PEER_RSP_RCVD: return("CLOSE_PEER_RSP_RCVD");
        break;
      case CIRCUIT_FREED:       return("CIRCUIT_FREED");
        break;
      default:                  return("UNKNOWN");
        break;
    }
}


static 
char *ncia_clientevent_to_packetstring(ncia_client_eventtype_t event)
{
    switch (event) {
      case CLOSE_CONN:          return("NDLC_CLOSE_PEER_REQ");
        break;
      case CONN_OPENED:         return("NDLC_CAP_XCHG_CMD");
        break;
      case KEEPALIVE_RCVD:      return("NDLC_PEER_TEST_RSP");
        break;
      case UNBUSY_XPORT:        return("NDLC_INFO_FRAME");
        break;
      case NDLC_FSM_INPUT:      return("NDLC_INFO_FRAME");
        break;
      case CLOSE_PEER_REQ_RCVD: return("NDLC_CLOSE_PEER_RSP");
        break;
      default:                  return("NO_PACKET_BE_SENT?");
        break;
    }
}

static
paktype *ncia_make_packet (uchar msg_type, ulong pak_size)
{
    paktype *pak;
    ndlc_header_t *ndlc_ptr;
 
    pak = getbuffer(pak_size);
    if (!pak) {
        return(NULL);
    }

    memset(pak, 0, pak_size);
    pak->datagramsize = pak_size; 
    ndlc_ptr = (ndlc_header_t *) pak->datagramstart;
    bzero (ndlc_ptr, pak_size);
    ndlc_ptr->proto_ver_id  = NCIA_PROTO_VERSION | NCIA_PROTOCOL_ID;
    ndlc_ptr->message_type  = msg_type;
    ndlc_ptr->packet_length = pak_size;
    return(pak);
}

static
void ncia_append_cap_xchg_cv_to_packet (nciapeertype * ncia_client)
{
    paktype *pak;    
    ndlc_cap_xchg_cv_t *ndlc_cap_cv_ptr;
    short    i, num_saps;
  
    pak = (paktype *) ncia_client->out_pak; 
    ndlc_cap_cv_ptr = (ndlc_cap_xchg_cv_t *) ((uchar *) pak->datagramstart +
                       NDLC_HDR_SZ + NDLC_CAP_XCHG_SZ);
    
    num_saps =  ncia_client->cap_xchg_ctl_vec.ctl_vec_len - NCIA_CAP_CONST_TWO;
    if (num_saps < 0)
        num_saps = 0;    /* protection from null cap_xchg_cv */
    ndlc_cap_cv_ptr->ctl_vec_len = ncia_client->cap_xchg_ctl_vec.ctl_vec_len;
    ndlc_cap_cv_ptr->ctl_vec_typ = ncia_client->cap_xchg_ctl_vec.ctl_vec_typ;

    for (i = 0; i <= num_saps; i++) {
        ndlc_cap_cv_ptr->sap_list[i] = 
            ncia_client->cap_xchg_ctl_vec.sap_list[i]; 
    }
}    

static
boolean ncia_add_cap_xchg_to_packet (nciapeertype * ncia_client, 
                                     boolean cmd_flag)
{
    paktype *pak;  
    ndlc_cap_xchg_t *ndlc_cap_ptr;
  
    pak = (paktype *) ncia_client->out_pak; 
    ndlc_cap_ptr = (ndlc_cap_xchg_t *) (pak->datagramstart + NDLC_HDR_SZ);
    if (is_ieee_zero(ncia_client->Vmac)) { 
        if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
            (ncia_client->NciaFlags & NciaClientDebugERROR))
 buginf("\nNCIA: Build cap xchg cmd or rsp with zero Mac address for client %i",
                   ncia_client->ipaddr);  
        return(FALSE);
    } 
    else {              /* Mac addr already in ctl blk */
        ieee_copy (ncia_client->Vmac, ndlc_cap_ptr->peer_client_mac);
        if (cmd_flag)
           ndlc_cap_ptr->cap_xchg_flag |= NCIA_CAP_XCHG_CMD; /* CAP Xchg cmd */
        else  
           ndlc_cap_ptr->cap_xchg_flag &= ~NCIA_CAP_XCHG_CMD; /* CAP Xchg rsp*/
        ndlc_cap_ptr->reserved = 0;
        return (TRUE);
    }
}    

static
boolean ncia_assign_macaddr_to_client (nciapeertype * ncia_client)
{
    ncia_servertype  *ncia_server;

    /* in case a client encountered time out and send the same cap_xchg_cmd */
    /* the 2nd time, after the Mac addr was already assigned during the     */
    /* process of the 1st cap_xchg_cmd                                      */
    if (!is_ieee_zero(ncia_client->Vmac)) { 
        if (ncia_client_mac_in_range(ncia_client))     /* free mac assigned */
            ncia_add_to_free_mac_index_chain(ncia_client);
    } 
    ncia_server = ncia_client->ncia_server; 
    if (ncia_server->ncia_free_client_index >= ncia_server->vmac_range) {
        NciaClientDebugAll("\nNCIA: Mac %e for client %i mac range used up",
                            ncia_client->Vmac, ncia_client->ipaddr);
        return(FALSE);
    }
    else {
        ncia_client->VmacIndex = ncia_server->ncia_free_client_index;
        ncia_server->ncia_free_client_index = 
                                 ncia_server->vmac_list[ncia_client->VmacIndex];
        ieee_add(ncia_server->vmac_start, ncia_client->VmacIndex, 
                 ncia_client->Vmac);
        ncia_server->vmac_list[ncia_client->VmacIndex] = NCIA_MAC_INDEX_USED;
    }
    return(TRUE);
}


/*
 *   Routine to execute ACTIVE_OPEN by calling ncia_tcpd_connect
 */
static
boolean ncia_client_action_02 (nciapeertype *ncia_client)
{
     return(ncia_tcpd_connect(ncia_client));
} 

/*
 *  routine to forward rcvd NDLC primitives from client to server FSM
 */

static
void ncia_client_action_06 (nciapeertype *ncia_client)
{
     ncia_pre_server_fsm(ncia_client, NDLC_PRIMITIVES);
} 
 

/*
 * Routine to send out an NDLC primitives to a peer client
 */ 

static
client_ret_t ncia_client_action_07 (nciapeertype *ncia_client)
{
     return(ncia_send_from_out_pak(ncia_client));
}

/*
 * Routine to build and send out Cap Xchg Req to a peer client
 */

static
client_ret_t ncia_client_action_08 (nciapeertype *ncia_client)
{
    ncia_build_cap_xchg_cmd(ncia_client); 
    if (ncia_client->out_pak)
        return(ncia_send_from_out_pak(ncia_client));
    else {
        ncia_prt_bld_no_mem(ncia_client, NDLC_CAP_XCHG);
        return (NO_MEMORY_FOR_PACKET);    
    }
}        
       
/*
 * Routine to build and send out Cap Xchg Rsp to a peer client
 */

client_ret_t  ncia_client_action_09 (nciapeertype *ncia_client)
{
    ncia_build_cap_xchg_rsp(ncia_client); /* with rcvd mac fr cmd */
    if (ncia_client->out_pak)
        return(ncia_send_from_out_pak(ncia_client));
    else {
        ncia_prt_bld_no_mem(ncia_client, NDLC_CAP_XCHG);
        return (NO_MEMORY_FOR_PACKET);    
    }
}        
   
      
/*
 * Routine to send out NDLC_PEER_TEST_REQ (kepalive req) to a peer client
 */

static
client_ret_t ncia_client_action_11 (nciapeertype *ncia_client)
{
    if ((ncia_client->out_pak = ncia_make_packet(NDLC_PEER_TEST_REQ,
        NDLC_HDR_SZ)))
        return(ncia_send_from_out_pak(ncia_client));
    else {
        ncia_prt_bld_no_mem(ncia_client, NDLC_PEER_TEST_REQ);
        return (NO_MEMORY_FOR_PACKET);   
    }
} 

/*
 * Routine to send out NDLC_PEER_TEST_RSP (keepalive rsp) to a peer client
 */

static
client_ret_t ncia_client_action_12 (nciapeertype *ncia_client)
{
    if ((ncia_client->out_pak = ncia_make_packet(NDLC_PEER_TEST_RSP,
        NDLC_HDR_SZ))) {
        return(ncia_send_from_out_pak(ncia_client)); 

    }
    else {
        ncia_prt_bld_no_mem(ncia_client, NDLC_PEER_TEST_RSP);
        return (NO_MEMORY_FOR_PACKET);   
    }
}

 
/*
 * Routine to build and send a close peer request to a client 
 */


/* build and send CLOSE_PEER_REQ */ 

static
client_ret_t ncia_client_action_15 (nciapeertype *ncia_client, 
                                    uchar reason_for_close)
{

    paktype *pak;
    ndlc_clo_peer_req_t *ndlc_clo_peer_ptr;

    if ((ncia_client->out_pak = ncia_make_packet(NDLC_CLOSE_PEER_REQ,
                  NDLC_HDR_SZ + NDLC_CLOSE_PEER_REQ_SZ))) {

        pak = (paktype *) ncia_client->out_pak;
        ndlc_clo_peer_ptr = (ndlc_clo_peer_req_t *) 
                            (pak->datagramstart + NDLC_HDR_SZ);
        ndlc_clo_peer_ptr->reason_code = reason_for_close;      
        return(ncia_send_from_out_pak(ncia_client));

    }
    else {
        ncia_prt_bld_no_mem(ncia_client, NDLC_CLOSE_PEER_REQ);
        return (NO_MEMORY_FOR_PACKET);
    }
}

/* build and send CLOSE_PEER_RSP */

static
client_ret_t ncia_client_action_16 (nciapeertype *ncia_client)
{
    if ((ncia_client->out_pak = ncia_make_packet(NDLC_CLOSE_PEER_RSP,
                      NDLC_HDR_SZ))) {
        return(ncia_send_from_out_pak(ncia_client));
    }
    else {
        ncia_prt_bld_no_mem(ncia_client, NDLC_CLOSE_PEER_RSP);
        return (NO_MEMORY_FOR_PACKET);
    }
}

static
boolean ncia_check_if_client_mac_valid (uchar *client_mac_ptr, 
                                        nciapeertype *ncia_client)
{
    ncia_servertype   *ncia_server;   
    nciapeertype  *client_ptr;
    boolean       mac_exists;

    mac_exists = FALSE;
    ncia_server = (ncia_servertype *)ncia_client->ncia_server;

    ForEachNciaClient(ncia_server, client_ptr)
        if ((ieee_equal (client_mac_ptr, (uchar *) &client_ptr->Vmac)) &&
            (client_ptr != ncia_client))    { 
            mac_exists = TRUE;
            goto END_OF_CLIENT_SEARCH2;   /* exit fr two level loop */
        }
    END_OF_CLIENT_SEARCH2:
    return (mac_exists ? FALSE : TRUE);
} 

static cap_cmd_ret_t ncia_check_cap_cmd_rcvd (nciapeertype *ncia_client)
{
    ncia_servertype  *ncia_server;
    paktype *in_pak;
    ndlc_header_t *ndlc_ptr;
    ndlc_cap_xchg_t *ndlc_cap_ptr;
    ndlc_cap_xchg_cv_t *ndlc_cap_cv_ptr;
    short   i, num_saps = 0;
    cap_cmd_ret_t rtn_rsp;

    ncia_server = ncia_client->ncia_server;
    in_pak = (paktype *) ncia_client->in_pak; 
    ndlc_ptr = (ndlc_header_t *) in_pak->datagramstart;
    rtn_rsp = CAP_CMD_MAC_NO;    

    if (ndlc_ptr->packet_length < NCIA_CAP_MIN_LEN) {
        if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
            (ncia_client->NciaFlags & NciaClientDebugERROR))
            buginf("\nNCIA: Cap xchg cmd packet len %d < 12, for client %i(%d)",
                    ndlc_ptr->packet_length, 
                    reg_invoke_ip_tcpdriver_fhost(ncia_client->t),
                    reg_invoke_ip_tcpdriver_fport(ncia_client->t));
        ncia_set_sap_list_to_zero(ncia_client);
        return(rtn_rsp);                       
    }
    else {
        ndlc_cap_ptr = (ndlc_cap_xchg_t *) 
                       (in_pak->datagramstart + NDLC_HDR_SZ);
        if ((ndlc_ptr->packet_length >= NCIA_CAP_ONE_SAP_LEN) &&
            (ndlc_ptr->packet_length <= NCIA_CAP_MAX_LEN)) {
            ndlc_cap_cv_ptr = (ndlc_cap_xchg_cv_t *) 
                       (in_pak->datagramstart + NDLC_HDR_SZ + NDLC_CAP_XCHG_SZ);

                                          /* pt to 1st byte of cap_xchg_cv_t */
            if (ndlc_cap_cv_ptr->ctl_vec_len != 0) {  /* SAP list appended */
                ncia_client->cap_xchg_ctl_vec.ctl_vec_len = 
                                                   ndlc_cap_cv_ptr->ctl_vec_len;
                num_saps = ndlc_cap_cv_ptr->ctl_vec_len - NCIA_CAP_CONST_TWO;
                /* num of saps = length - 2 */
                ncia_client->cap_xchg_ctl_vec.ctl_vec_typ = 
                                                   ndlc_cap_cv_ptr->ctl_vec_typ;
                for (i = 0; i < num_saps; i++)  {
                     ncia_client->cap_xchg_ctl_vec.sap_list[i] = 
                                                   ndlc_cap_cv_ptr->sap_list[i];
	        /* SAP list has been saved in the ncia_client ctl blk */
	        }
	    }
            else {
                if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                    (ncia_client->NciaFlags & NciaClientDebugERROR))
                    buginf("\nNCIA: Ctl vec len %d while packet len > NCIA_CAP_MIN_LEN, for client %i(%d)",
                            ndlc_cap_cv_ptr->ctl_vec_len, 
                            reg_invoke_ip_tcpdriver_fhost(ncia_client->t),
                            reg_invoke_ip_tcpdriver_fport(ncia_client->t));
                ncia_set_sap_list_to_zero(ncia_client);
            }
	}
        else {
            if (ndlc_ptr->packet_length > NCIA_CAP_MAX_LEN) {
                if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                    (ncia_client->NciaFlags & NciaClientDebugERROR))
         buginf("\nNCIA: Invalid cap xchg cmd packet len %d, for client %i(%d)",
                            ndlc_ptr->packet_length, 
                            reg_invoke_ip_tcpdriver_fhost(ncia_client->t),
                            reg_invoke_ip_tcpdriver_fport(ncia_client->t));
	    }
            ncia_set_sap_list_to_zero(ncia_client);
	}
    }

    if (!(ncia_client->NciaFlags & NCIA_CLIENT_CONFIGED)) {
        if (!is_ieee_zero(ndlc_cap_ptr->peer_client_mac) &&
            ncia_check_if_client_mac_valid(
                       (uchar *)&ndlc_cap_ptr->peer_client_mac, ncia_client)) {
            
            if (!is_ieee_zero(ncia_client->Vmac)) {  
                /* free current mac assigned */ 
                if (ncia_client_mac_in_range(ncia_client)) 
                    ncia_add_to_free_mac_index_chain(ncia_client);
            } 
            ieee_copy (ndlc_cap_ptr->peer_client_mac, ncia_client->Vmac);
            if (ncia_client_mac_in_range(ncia_client)) {
                ncia_client->VmacIndex = 
                         ncia_subtract_macaddr_for_vmacindex(ncia_client->Vmac,
                         ncia_server->vmac_start, ncia_server->vmac_range);
                if (ncia_server->ncia_free_client_index >= 
                    ncia_server->vmac_range) {
                    /* Should never occur, already checked for mac valid */
            NciaClientDebugAll("\nNCIA: mac %e for client %i mac range used up",
                               ncia_client->Vmac, ncia_client->ipaddr);
                    rtn_rsp = CAP_CMD_MAC_OUT;
	        }
                else {
                    ncia_remove_from_free_mac_index_chain(ncia_client);
                    rtn_rsp = CAP_CMD_MAC_OK;
		}
	    }
            else {
                ncia_client->VmacIndex = NCIA_OUT_MAC_RANGE;
                rtn_rsp = CAP_CMD_MAC_OK; 
	    }
	}
        else {
            if (!(ncia_client->NciaFlags & NCIA_CLIENT_REGISTERED))
                if (ncia_assign_macaddr_to_client(ncia_client))
                    rtn_rsp = CAP_CMD_MAC_NO;
                else
                    rtn_rsp = CAP_CMD_MAC_OUT;
	}
    } else
        rtn_rsp = CAP_CMD_MAC_NO;/* force client to accept the configured mac */
  
    ncia_client->CapXchgFlags |= ndlc_cap_ptr->cap_xchg_flag;
    if (ncia_client->CapXchgFlags & NCIA_TCP_LISTEN_MODE) 
        ncia_client->NciaFlags |= NCIA_CLIENT_REGISTERED;
    else
        ncia_client->NciaFlags &= ~NCIA_CLIENT_REGISTERED;
    if (ncia_client->CapXchgFlags & NCIA_NETBIOS_IND)
        ncia_client->NciaFlags |= NCIA_CLIENT_NETBIOS;
    else
        ncia_client->NciaFlags &= ~NCIA_CLIENT_NETBIOS;

    return(rtn_rsp);
}    

static
void ncia_build_cap_xchg_cmd (nciapeertype *ncia_client)
    /* check cap cmd rcvd failed, reassign mac and send out a cap xchg cmd */

{
    if ((ncia_client->out_pak = ncia_make_packet(NDLC_CAP_XCHG, NDLC_HDR_SZ + 
        NDLC_CAP_XCHG_SZ + ncia_client->cap_xchg_ctl_vec.ctl_vec_len))) {
        ncia_add_cap_xchg_to_packet(ncia_client, NCIA_CAP_XCHG_CMD);
        ncia_append_cap_xchg_cv_to_packet(ncia_client);
    }
}

/* check cap xchg cmd rcvd succesful, reply with a cap xchg response */ 
static
void ncia_build_cap_xchg_rsp (nciapeertype *ncia_client)
{

    if ((ncia_client->out_pak = ncia_make_packet(NDLC_CAP_XCHG, NDLC_HDR_SZ + 
        NDLC_CAP_XCHG_SZ + ncia_client->cap_xchg_ctl_vec.ctl_vec_len))) {
        ncia_add_cap_xchg_to_packet(ncia_client, NCIA_CAP_XCHG_RSP);
        ncia_append_cap_xchg_cv_to_packet(ncia_client);
    }
}

static
void ncia_set_sap_list_to_zero (nciapeertype *ncia_client)
{
    ncia_client->cap_xchg_ctl_vec.ctl_vec_len = 0;
    ncia_client->cap_xchg_ctl_vec.ctl_vec_typ = 0;
    bzero (ncia_client->cap_xchg_ctl_vec.sap_list, NCIA_MAX_SAPS);
}


static
void ncia_client_action_error (nciapeertype *ncia_client)
{
    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
        (ncia_client->NciaFlags & NciaClientDebugERROR))
        buginf("\nNCIA: Unrecognized event in state %s, for client %i(%d)",
                ncia_state_to_string (ncia_client->client_state), 
                reg_invoke_ip_tcpdriver_fhost(ncia_client->t),
                reg_invoke_ip_tcpdriver_fport(ncia_client->t));
}

static
void ncia_check_output_return (nciapeertype *ncia_client,
                client_ret_t rtn_code, ncia_client_eventtype_t event)
{
     if (rtn_code == CLIENT_BUSY) {
         if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
             (ncia_client->NciaFlags & NciaClientDebugERROR)) 
              buginf ("\nNCIA: Error, send packet %s to client %i, halted, TCP queue full",
                      ncia_clientevent_to_packetstring(event),
                      ncia_client->ipaddr);

	      /*   packet was sent since force=TRUE option was used */
         ncia_pre_server_fsm(ncia_client, BUSY_XPORT);
     }
     else {
     if (rtn_code == CLIENT_UNREACHABLE)
        if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
            (ncia_client->NciaFlags & NciaClientDebugERROR)) 
             buginf ("\nNCIA: Error, send packet %s to client %i, failed, client unreachable",
                     ncia_clientevent_to_packetstring(event),
                     ncia_client->ipaddr);
     }
                             /* NO_MEMORY_FOR_PACKET msg already printed */
}

static
void ncia_print_event_state (nciapeertype *ncia_client, 
                             ncia_client_eventtype_t event)
{
    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
        (ncia_client->NciaFlags & NciaClientDebugEVENT))

        buginf("\nNCIA: event %s, state %s for client %i",
                ncia_event_to_string(event),
                ncia_state_to_string(ncia_client->client_state),
                ncia_client->ipaddr);
}

static
void ncia_print_error_event_state (nciapeertype *ncia_client, 
                                   ncia_client_eventtype_t event)
{
    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
        (ncia_client->NciaFlags & NciaClientDebugERROR))
         buginf("\nNCIA: event %s, state %s, for client %i, should not have occurred",
                 ncia_event_to_string(event),
                ncia_state_to_string(ncia_client->client_state),
                ncia_client->ipaddr);
}

static
void ncia_free_inpak_terminate (nciapeertype *ncia_client)
{
    if (ncia_client->in_pak) {
        datagram_done(ncia_client->in_pak);
        ncia_client->in_pak = NULL;
    }	
    ncia_shutdown_client(ncia_client, FALSE);
}

static void ncia_process_rcvd_cap_cmd (nciapeertype *ncia_client)
{
    client_ret_t  rtn_code;

    mgd_timer_stop(&ncia_client->timer);         
    ncia_client->cur_cap_xchg_count++;
    if (ncia_check_cap_cmd_rcvd(ncia_client) == CAP_CMD_MAC_OK) {
     
        ncia_client->cur_cap_xchg_count= 0;
        rtn_code = ncia_client_action_09(ncia_client); /*agreed, rtn CAP rsp*/
        if ((rtn_code == NO_MEMORY_FOR_PACKET) ||
            (rtn_code == CLIENT_UNREACHABLE))
            ncia_shutdown_client(ncia_client, TRUE);
        else {
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                (ncia_client->NciaFlags & NciaClientDebugEVENT))
                 buginf("\nNCIA: event %s, state %s, for client %i,"
                         " cap xchg rsp sent",
                           ncia_event_to_string(CAP_RCVD),
                           ncia_state_to_string(ncia_client->client_state), 
                           ncia_client->ipaddr);
            ncia_client->client_state = NCIA_OPENED;
            ncia_client->NciaFlags |= NCIA_CLIENT_ACTIVE;
            ncia_client->keepalive_count = 0; /* kick in keepalive */
            ncia_pre_server_fsm(ncia_client, CONN_OPENED);

               /* Attention, CONN_OPENED was used to inform Server that
                * a client state is ready for transporrt 
                */      
            if (ncia_client->ncia_server->ndlc_keepalive_int)
                mgd_timer_start(&ncia_client->timer,
                                ncia_client->ncia_server->ndlc_keepalive_int);
	}
    }
    else 
    if (ncia_check_cap_cmd_rcvd(ncia_client) == CAP_CMD_MAC_NO) {
        if (ncia_client->cur_cap_xchg_count < NCIA_MAX_CAP_XCHG) { 
            rtn_code = ncia_client_action_08(ncia_client); 
                                            /* send CAP_REQ, new mac */
            if ((rtn_code == NO_MEMORY_FOR_PACKET) || 
                (rtn_code == CLIENT_UNREACHABLE)) 
                ncia_shutdown_client(ncia_client, TRUE);
            else {
                if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
                    (ncia_client->NciaFlags & NciaClientDebugEVENT))
                    buginf("\nNCIA: event %s, state %s, for client %i,"
                            " cap xchg cmd sent",
                              ncia_event_to_string(CAP_RCVD),
                              ncia_state_to_string(ncia_client->client_state),
                              ncia_client->ipaddr);
                ncia_client->client_state = NCIA_CAP_NEG;
                mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT);        
            }   
        }
        else {
            if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) || 
                (ncia_client->NciaFlags & NciaClientDebugERROR))
                 buginf("\nNCIA: Error, event %s, state %s,"
                         " for client %i cap neg exceeded limit?",
                           ncia_event_to_string(CAP_RCVD),
                           ncia_state_to_string(ncia_client->client_state), 
                           ncia_client->ipaddr);
             ncia_shutdown_client(ncia_client, FALSE);
	}
    }
    else { 
        rtn_code = ncia_client_action_15(ncia_client, NCIA_CLO_NO_MAC); 
                                            /* send close peer req */
        ncia_check_output_return(ncia_client, rtn_code, CLOSE_CONN); 
        if ((rtn_code == NO_MEMORY_FOR_PACKET) || 
            (rtn_code == CLIENT_UNREACHABLE)) 
            ncia_shutdown_client(ncia_client, TRUE);
        else { 
            ncia_client->client_state = NCIA_CLOSE_WAIT;
            mgd_timer_start(&ncia_client->timer, NCIA_CLIENT_TIMER_INT); 
	}
    }
}


static void ncia_prt_bld_no_mem(nciapeertype *ncia_client, ndlc_msg_type_t msgtype)
{
    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
        (ncia_client->NciaFlags & NciaClientDebugERROR))
        buginf ("\nNCIA: Error, state %s, for client %i,"
                " build %s packet, no memory",
                  ncia_state_to_string(ncia_client->client_state),
                  ncia_client->ipaddr,
                  ncia_print_ndlc(msgtype));
}

static boolean ncia_process_client_time_out (nciapeertype *ncia_client)
{

    if (((ncia_client->NciaFlags & NciaClientDebugALL) == NciaClientDebugALL) ||
        (ncia_client->NciaFlags & NciaClientDebugEVENT))
       buginf("\nNCIA: event %s, state %s, for client %i, keepalive_count = %d",
                  ncia_event_to_string(TIME_OUT),
                  ncia_state_to_string(ncia_client->client_state), 
                  ncia_client->ipaddr,
                  ncia_client->keepalive_count);

    ncia_client->keepalive_count++;
    if (ncia_client->keepalive_count > MAX_TIMEOUT) {
        ncia_shutdown_client(ncia_client, FALSE);
        return(FALSE);
    }
    else 
        return(TRUE);
}

