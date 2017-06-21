/* $Id: qllc_x25api.c,v 3.1.2.3.2.2 1996/07/23 13:29:10 thille Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllc_x25api.c,v $
 *---------------------------------------------------------------------
 * qllc_x25api.c -- Provides the interface to the X.25 services for QLLC.
 *
 * April 1996, Bob Batz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *---------------------------------------------------------------------
 * $Log: qllc_x25api.c,v $
 * Revision 3.1.2.3.2.2  1996/07/23  13:29:10  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.2.3.2.1  1996/06/28  23:26:14  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/12  04:26:40  gglick
 *         Sync to V112_0_11
 * Branch: X25_II_branch
 *
 * Revision 3.1.2.2  1996/06/06  02:31:35  gglick
 *         Sync to V112_0_1
 * Branch: X25_II_branch
 *
 * Revision 3.1.2.1  1996/05/30  13:48:20  rbatz
 * CSCdi59061:  QLLC rewritten to work with new X.25.
 * Branch: X25_II_branch
 *
 * Revision 3.1  1996/04/12  18:55:46  rbatz
 * Commit the placeholder file.
 *
 *---------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "srt_registry.h"
#include "interface_generic.h"
#include "packet.h"
#include "../os/buffers.h"
#include "mgd_timers.h"
#include "linktype.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "stdarg.h"
#include "../parser/parser_actions.h"
#include "../x25/parser_defs_x25.h"
#include "../x25/ll_ul.h"
#include "../x25/x25_library.h"
#include "../x25/x25_packet.h"
#include "../x25/x25_registry.h"
#include "../llc2/llc2.h"
#include "../util/avl.h"
#include "../ibm/sdlc.h"
#include "../ibm/sna_util.h"
#include "../cls/cls_private.h"
#include "qllc.h"
#include "qllc_private.h"
#include "../srt/lnx.h"
#include "parser_defs_qllc.h"
#include "../ui/debug.h"
#include "../srt/qllc_debug.h"
#include "qllc_event.h"
#include "string.h"
#include "../ibm/libibm_externs.h"


#define QLLC_VC_EVENT_BUF_SIZE  64
#define QLLC_PID_LEN 1
static unsigned char qllc_pid_1980[QLLC_PID_LEN] = {0xC3};
/* static unsigned char qllc_pid_1984[QLLC_PID_LEN] = {0xCB, 0, 0, 0}; */

static x25_packet qllc_x25_vc_pkt;

/*
 * This structure is only need in this module
 */
STRUCTURE_REF(qllcx25_cfg_and_address);
struct qllcx25_cfg_and_address_ {
    x25_config *cfg;
    addrtype   *searchAddr;
};

static void qllcx25_map_add(addrtype *, addrtype *, x25_config *);
static void qllcx25_map_delete(addrtype *, x25_config *);

/*
 ******************************************************************
 * Declarations for the vectors that are called by the X.25 Service
 ******************************************************************
 */
/* 
 * Declarations for qllc_p_vectors
 */
static void qllcx25_call_coll_rx(void *, enum X25_D_STATE, x25_reason *, x25_packet *);
static void qllcx25_enter_p4(void *, enum X25_D_STATE, x25_reason *, x25_packet *);
static void qllcx25_clear_rx(void *, enum X25_D_STATE, x25_reason *, x25_packet *);
static void qllcx25_clear_tx(void *, enum X25_D_STATE, x25_reason *, x25_packet *);
static void qllcx25_pvc_inactive(void *, enum X25_D_STATE, x25_reason *, x25_packet *);
static void qllcx25_deleted(void *, enum X25_D_STATE, x25_reason *, x25_packet *);

static client_p_vectors qllc_p_vectors = {
    qllcx25_call_coll_rx,
    qllcx25_enter_p4,
    qllcx25_clear_rx,
    qllcx25_clear_tx,
    qllcx25_pvc_inactive,
    qllcx25_deleted
};


/* 
 * Declarations for qllc_d_vectors
 */
static void qllcx25_enter_d1(void *, enum X25_D_STATE, x25_reason *, x25_packet *);
static void qllcx25_reset_rx(void *, enum X25_D_STATE, x25_reason *, x25_packet *);
static void qllcx25_reset_tx(void *, enum X25_D_STATE, x25_reason *, x25_packet *);
static void qllcx25_invalid(void *, enum X25_D_STATE, x25_reason *, x25_packet *);

static client_d_vectors qllc_d_vectors = {
    qllcx25_enter_d1,
    qllcx25_reset_rx,
    qllcx25_reset_tx,
    qllcx25_invalid
};


/* 
 * Declarations for qllc_d1_vectors
 */
static void qllcx25_data_rx(void *, x25_packet *);
static void qllcx25_interrupt_rx(void *, x25_packet *);
static void qllcx25_interrupt_conf_rx(void *, x25_packet *);
static void qllcx25_rr_rx(void *, x25_packet *);
static void qllcx25_rnr_rx(void *, x25_packet *);
static void qllcx25_idle(void *, x25_packet *);

static client_d1_vectors qllc_d1_vectors = {
    qllcx25_data_rx,
    qllcx25_interrupt_rx,
    qllcx25_interrupt_conf_rx,
    qllcx25_rr_rx,
    qllcx25_rnr_rx,
    qllcx25_idle
};



/*
 ****************************************************************
 *  Functions to Handle VC Events/Data from X.25 Service.
 ****************************************************************
 */



/*
 * qllcx25_data_rx
 *
 * X.25 service hands us a data packet.
 *
 */
static void qllcx25_data_rx (void *client_context,
                             x25_packet *rx_pak)
{
   qllctype *qllc;
   x25_vc *vc;
   paktype *qllcpak;

   /* 
    * May be needed for some purpose - figure it out later!
    */
   qllc = (qllctype *) client_context;
   if (!qllc)
       return;

   vc = qllc->vc;
   if (!vc)
       return;

   /* 
    * Get the data packet out of the x25_packet and set
    * x25_packet->pak to NULL to keep X.25 from freeing it.
    * Setup some params.
    */
   qllcpak = rx_pak->pak;
   rx_pak->pak = NULL;
   qllcpak->x25_cb = vc;
   if (rx_pak->user_data.q_bit)
       qllcpak->x25_flags |= PAK_X25_QBIT;

#ifdef TO_DO
   if (packet->user_data.m_bit)
       pak->x25_flags &= ???; -- Something here for reassembly???
   -- Do we need to set something with the idb?
#endif

   /* 
    * For now (short-term), send an RR for every packet, 
    * provided that we are not in flow control.
    */
    qllcx25_send_rr(vc);

   /*
    *  Queue it to QLLC and let him handle it and process level!
    */
   qllc_enqueue_data(qllcpak);
   return;
}

/* 
 * qllcx25_svc_announce
 * 
 * When X.25 service receives a call, it notifies the clients 
 * through invocations to registry _x25_svc_announce.  This is 
 * the function that handles this for QLLC.
 *
 */
boolean qllcx25_svc_announce (x25_vc *vc, 
                              x25_packet *call, 
                              service_call_accept_t *last_resort)
{
    /*
     * We need to tell X.25 whether or not we want this call.
     *
     * Simply look at the Call User Data Field.  If first byte
     * in the CUD is 0xC3, then tell X.25 that we want it.
     *
     */
     if (!call->d_bit &&
         (call->user_data.data_len == QLLC_PID_LEN) &&
         !bcmp(call->user_data.data, qllc_pid_1980, QLLC_PID_LEN))
     {
         /*
          * Enqueue this into QLLC's Call Queue.
          */
         qllc_enqueue_call(call->pak);
         call->pak = NULL;
         return(TRUE);
     }
     return (FALSE);
}

/*
 * qllcx25_enter_d1
 *
 * Event occurs when the VC enters state D1.
 * We interpret this as a VC_UP event.
 *
 */
static void qllcx25_enter_d1 (void *client_context,
                              enum X25_D_STATE d_was,
                              x25_reason *reason, 
                              x25_packet *packet)
{
   qllc_vc_event_input(client_context, QLLC_X25_VC_UP_EVENT);
   return;
}

/*
 * qllcx25_rr_rx
 *
 * Event occurs when X.25 receives an RR packet.
 */
static void qllcx25_rr_rx (void *client_context, 
                           x25_packet *packet)
{
   qllc_vc_event_input(client_context, QLLC_X25_RR_EVENT);
   return;
}

/*
 * qllcx25_rnr_rx
 *
 * Event occurs when X.25 receives an RNR packet.
 */
static void qllcx25_rnr_rx (void *client_context, 
                            x25_packet *packet)
{
   qllc_vc_event_input(client_context, QLLC_X25_RNR_EVENT);
   return;
}

/*
 * qllcx25_reset_rx
 *
 * Event occurs when X.25 receives a RESET packet.
 * What does this mean?
 */
static void qllcx25_reset_rx (void *client_context,
                              enum X25_D_STATE d_was,
                              x25_reason *reason,
                              x25_packet *packet)
{
   qllctype *qllc = (qllctype *) client_context;
   if (!qllc)
       return;

   if (qllc->qllc_ispvc)
       qllc->qllc_x25_reset_cause = packet->cause_diag.cause;

   qllc_vc_event_input(qllc, QLLC_X25_RESET_IND_EVENT);
   return;
}

/*
 * qllcx25_clear_rx
 *
 * Event occurs when X.25 receives a CLEAR packet on the VC.
 *
 */
static void qllcx25_clear_rx (void *client_context,
                              enum X25_D_STATE d_was,
                              x25_reason *reason, 
                              x25_packet *packet)
{
   qllc_vc_event_input(client_context, QLLC_X25_CLEAR_IND_EVENT);
   return;
}

/*
 * qllcx25_deleted
 *
 * Event occurs when X.25 ???.
 * What does this mean?
 */
static void qllcx25_deleted (void *client_context,
                             enum X25_D_STATE d_was,
                             x25_reason *reason, 
                             x25_packet *packet)
{
   qllc_vc_event_input(client_context, QLLC_X25_DELETED_EVENT);
   return;
}

/*
 * qllcx25_invalid
 *
 * Event occurs when X.25 ???
 * What does this mean?
 */

static void qllcx25_invalid (void *client_context,
                             enum X25_D_STATE d_was, 
                             x25_reason *reason, 
                             x25_packet *packet)
{
   qllc_vc_event_input(client_context, QLLC_X25_INVALID_EVENT);
   return;
}


/*
 * qllcx25_call_coll_rx
 *
 * Event occurs when clients calls a remote X.121 address at the
 * same time that the the device with that X.121 address calls us.
 *
 */
static void qllcx25_call_coll_rx (void *client_context,
                                  enum X25_D_STATE d_was,
                                  x25_reason *reason, 
                                  x25_packet *packet)
{
   qllc_vc_event_input(client_context, QLLC_X25_CALL_COLLISION_EVENT);
   return;
}

/*
 * qllcx25_pvc_inactive
 *
 * Event occurs when X.25 service decides the PVC 
 * is inactive.
 *
 */
static void qllcx25_pvc_inactive (void *client_context,
                                  enum X25_D_STATE d_was,
                                  x25_reason *reason, 
                                  x25_packet *packet)
{
   qllc_vc_event_input(client_context, QLLC_X25_PVC_INACTIVE_EVENT);
   return;
}

/*
 * qllcx25_interrupt_rx
 *
 * Event occurs when an Interrupt Packet is received.
 * We don't expect this in QLLC.  But we will send
 * back an Interrupt Confirm Packet.
 */
static void qllcx25_interrupt_rx (void *client_context, 
                                  x25_packet *packet)
{
   paktype  *callpak = NULL;
   qllctype *qllc;
   enum X25LIB_REQ_RESULT req_result;

   /* 
    * Allocate a pak for the X.25 service usage
    */
   callpak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
   if (!callpak)
       return;

   qllc = (qllctype *) client_context;
   if (!qllc || !qllc->vc)
       return;

   /* 
    * Build the Interrupt Confirm packet.
    */
   memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
   qllc_x25_vc_pkt.pak  = callpak;
   qllc_x25_vc_pkt.type = X25PID_INTERRUPT_CONF;
   (*(qllc->vc)->x25.d1_vect->interrupt_conf_tx)((qllc->vc)->x25.context, &qllc_x25_vc_pkt,
                                                 &req_result);
   return;
}

/*
 * qllcx25_enter_p4
 *
 * Event occurs when X.25 VC enters state P4.
 * We ignore interpreting these as "VC_UP" events
 * in favor of waiting to enter the "D1" state.
 *
 */
static void qllcx25_enter_p4 (void *client_context,
                              enum X25_D_STATE d_was,
                              x25_reason *reason, 
                              x25_packet *packet)
{
   return;
}

/*
 * qllcx25_clear_tx
 *
 * X.25 service invokes this to tell us that it
 * transmitted a Clear Packet.  
 *
 * Currently we ignore, but maybe we should care.
 * Can X.25 send this without us initiating?
 *
 */
static void qllcx25_clear_tx (void *client_context,
                              enum X25_D_STATE d_was,
                              x25_reason *reason, 
                              x25_packet *packet)
{
   return;
}

/*
 * qllcx25_reset_tx
 *
 * X.25 service invokes this to tell us that it
 * transmitted a Reset Packet.
 *
 * Currently, we ignore, but maybe we should care.
 * Could X.25 have sent this without us initiating?
 *
 */
static void qllcx25_reset_tx (void *client_context,
                              enum X25_D_STATE d_was,
                              x25_reason *reason, 
                              x25_packet *packet)
{
   return;
}

/*
 * qllcx25_idle
 *
 * Event occurs when X.25 detects an idle period on the
 * SVC.  Some clients may choose to Clear an SVC at this
 * time.  We do not want to take down our SNA session.
 */
static void qllcx25_idle (void *client_context, 
                          x25_packet *packet)
{
   return;
}

/*
 * qllcx25_interrupt_conf_rx
 *
 * Event occurs when X.25 receives an Interrupt Confirm Packet
 * This should not happen since QLLC will nevers send an
 * Interrupt Packet.
 */

static void qllcx25_interrupt_conf_rx (void *client_context, 
                                       x25_packet *packet)
{
   return;
}


/****************************************************************
 *  Functions to Send VC Events/Data to the X.25 Service.
 ****************************************************************/

/*
 * qllcx25_xmitter
 *
 * This is the nomination vector that is called when the packet
 * queued by qllcx25_data_tx reached the head of its queue and
 * the X.25 services tells us we can send it.
 */
static enum TX_DQ_RESULT qllcx25_xmitter (hwidbtype *idb,
                                          enum TX_DQ_OPERATION op,
                                          paktype **pak_ptr,
                                          paktype **fragment_of)
{
    static x25_packet qllc_x25_tx_pkt;
    enum X25LIB_REQ_RESULT result;
    x25_vc *vc;
    ushort frag_size ;
    paktype *frag_buff = NULL;

    vc = (x25_vc *) (*pak_ptr)->tx_nominate_context;



    switch (op) {
        case TXOP_NOMINATION:		/* data nominated to send */
            if (!x25lib_vc_can_send_inline(vc)) {
                return(TX_SKIP);
            }

            /*
             *  Send the packet.
             */
            memset(&qllc_x25_tx_pkt, 0, sizeof(x25_packet));
            (*pak_ptr)->tx_nominate          = NULL;

            qllc_x25_tx_pkt.type             = X25PID_DATA;
            qllc_x25_tx_pkt.d_bit            = FALSE;
            qllc_x25_tx_pkt.flow.pr          = x25lib_vc_tx_ack_everything_inline(vc);
            qllc_x25_tx_pkt.user_data.q_bit  = (*pak_ptr)->x25_flags & PAK_X25_QBIT;

 
            /*
             * Do I need to fragment?
             */
            frag_size = qllcx25_get_maxdataout(vc);
            if ((*pak_ptr)->datagramsize <= frag_size) {

                qllc_x25_tx_pkt.pak              = *pak_ptr;
                qllc_x25_tx_pkt.user_data.m_bit  = FALSE;
                (*vc->x25.d1_vect->data_form)(vc->x25.context, &qllc_x25_tx_pkt, 
                                              &result);
                if (result == X25LIB_REQ_OK) {
                    qllc_x25_tx_pkt.pak = NULL;
                    return(TX_PACKET);
                } else {
                    if (lnx_error_debug)
                        buginf("\nqllcx25_xmitter error: Bad result = %d from data_form",
                                  result);
                    return(TX_SKIP);
                }

            } else {
                /*
                 * Yes, I do need to fragment this puppy!
                 */
/* TO_DO - Should already have buffers allocated and stored in qllc structure
           before getting here.
*/
                frag_buff = getbuffer(frag_size + *(vc->x25.data_overhead));
                if (!frag_buff) {
                    return(TX_SKIP);
                }
                frag_buff->datagramstart += *(vc->x25.data_overhead);
                frag_buff->datagramsize   = frag_size;
                bcopy((*pak_ptr)->datagramstart, frag_buff->datagramstart, frag_size);
                (*pak_ptr)->datagramstart += frag_size;
                (*pak_ptr)->datagramsize  -= frag_size;
                qllc_x25_tx_pkt.pak        = frag_buff;
                qllc_x25_tx_pkt.user_data.m_bit  = TRUE;

                (*vc->x25.d1_vect->data_form)(vc->x25.context, 
                                              &qllc_x25_tx_pkt, &result);

                if (result == X25LIB_REQ_OK) {
                    qllc_x25_tx_pkt.pak = NULL;
                    *fragment_of = *pak_ptr;
                    *pak_ptr = frag_buff;
                    return(TX_FRAGMENT);
                } else {
                    if (lnx_error_debug)
                        buginf("\nqllcx25_xmitter error: Bad result = %d from data_form",
                                  result);
                    return(TX_SKIP);
                }              
            }

       case TXOP_WALK:			/* someone's walking the tx queues */
            return(TX_SKIP);

       case TXOP_FLUSH:
           /*  TO_DO
            *  this data packet is being flushed from the transmit queues;
            *  either initiate a VC Reset procedure (probably best) or
            *  enqueue the packet privately for requeueing later (?)
            */
            return(TX_SKIP);

       case TXOP_REQUEUE:
       default:
            return(TX_SKIP);
    }
}

boolean qllcx25_send_data (qllctype *qllc,
                           paktype *pak)
{
    x25_vc *vc = qllc->vc;

    if (!vc) {
        if (lnx_error_debug)
            buginf("\nqllcx25_send_data: No VC ptr - packet dropped!");
	if (pak)
 	    datagram_done(pak);
        return(FALSE);
    }

    if (pak) {
        pak->tx_nominate_context = vc;
        if (qllc_packet_debug)
            QllcPrPacket(qllc, pak, 'O');
    }

    if (qllc->x25_vc_busy) {
        /*
         * If X.25 is busy, then enqueue this to the overflow queue.
         */
        if (pak)
	    p_enqueue(&qllc->vc_overflowQ, pak);

    } else if (pak && QUEUEEMPTY(&qllc->vc_overflowQ)) {
        /*
         * If the overflow queue is empty, try to enqueue this to X.25.
         * If it fails, put it in the overflowQ.  Need pak_lock() to let us do so.
         */
        pak_lock(pak);
        if (!(*vc->x25.d1_vect->data_enqueue)(vc->x25.context, pak, qllc->txqid_x25,
                                             (tx_nominate_t) qllcx25_xmitter))
        {
            p_enqueue(&qllc->vc_overflowQ, pak);
        } else {
            datagram_done(pak);         /* Undo the pak_lock() */
        }
    } else {
        /*
         * Put this pak at the tail of the vc_overflowQ.
         * Then enqueue as many as possibly in the X.25 queue.
         */
        if (pak)
	    p_enqueue(&qllc->vc_overflowQ, pak);
				
	while (!QUEUEEMPTY(&qllc->vc_overflowQ)) {
	    pak = p_dequeue(&qllc->vc_overflowQ);
            pak_lock(pak);
            if (!(*vc->x25.d1_vect->data_enqueue)(vc->x25.context, pak, qllc->txqid_x25,
                                            (tx_nominate_t) qllcx25_xmitter))
            {
                p_requeue(&qllc->vc_overflowQ, pak);
                break;
            }
            else {
                datagram_done(pak);    /* Undo the pak_lock() */
            }
	}
    }
				
/* rbatz - What TO_DO for CLS? */
    /* if we reach any threshold values, send appropriate messages */
    if (QllcIsCls(qllc)) {
	CepFlowControlInd(qllc->fUCepId, &qllc->fCep);
	return(TRUE);
    }

/* rbatz - TO_DO: Don't hard code values here */

    if ( (QUEUESIZE(&qllc->vc_overflowQ) > 7) && 
         !qllc->net_quenched)  {
          qllc->net_quenched = TRUE;
          lnx_quench(qllc->lnx, TRUE);
    } else if ((QUEUESIZE(&qllc->vc_overflowQ) < 2) && 
         !qllc->x25_vc_busy && 
          qllc->net_quenched)  {
          qllc->net_quenched = FALSE;
          lnx_quench(qllc->lnx, FALSE);
    }

    return (TRUE);
}

/*
 * qllcx25_setup_client_ctxt
 *
 * Function is called from the X.25 Protocol Engine to
 * have the client fill in his client context, etc...
 * before a Call Confirm is received.
 */
void qllcx25_setup_client_ctxt(void *client_context, x25_vc *vc)
{
    qllctype *qllc = (qllctype *) client_context;
    qllcx25_bind_vc(qllc, vc);
    qllc_setstate(qllc, QST_ADM);
    return;
}

/*
 * qllcx25_place_call
 *
 * Tell X.25 service to place a call (activate an SVC).
 *
 */
x25_vc * qllcx25_place_call (x25_config *x25_cfg,
                             addrtype *src_x121,
                             addrtype *dst_x121,
                             qllctype *qllc)
{
    x25_vc              *vc_ptr = NULL;
    x25_ctxt_id         ctxt_id;
    x25_connect_data    x25_conn_info;
    paktype             *callpak;
    enum X25REG_CREATE_SVC_RESULT result;

    /* 
     * Build the X.25 context id based on the x25 configuration.
     * Make sure the X.25 context is active.
     */
    if (x25_cfg->id.type != X25SERVICE_X25)
        return NULL;
    ctxt_id.type = X25SERVICE_X25;
    ctxt_id.id_x25.hwidb = x25_cfg->id.hwidb;
#ifdef TO_DO
    if (???(ctxt_id) != ???UP)
        return NULL;
#endif
    
    /* 
     * Allocate a pak for the X.25 service usage.
     */
    callpak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
    if (!callpak)
         return NULL;

    /* 
     * Build the call packet.
     */
    memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
    qllc_x25_vc_pkt.pak      = callpak;
    qllc_x25_vc_pkt.type     = X25PID_CALL;
    qllc_x25_vc_pkt.d_bit    = FALSE;

   /*
    * Specify the Call User Data
    */
    qllc_x25_vc_pkt.user_data.data_len   = QLLC_PID_LEN;
    qllc_x25_vc_pkt.user_data.data       = qllc_pid_1980;

    /*
     * Specify the Address Block.
     */
    qllc_x25_vc_pkt.a_block.specified   = TRUE;
    qllc_x25_vc_pkt.a_block.a_bit       = 0;
    qllc_x25_vc_pkt.a_block.src.type    = X25_ADDR_X121;
    qllc_x25_vc_pkt.a_block.src.digits  = src_x121->length;
    memcpy(qllc_x25_vc_pkt.a_block.src.addr.x121, src_x121->x121_addr, 
            src_x121->length);
    qllc_x25_vc_pkt.a_block.dst.type   = X25_ADDR_X121;
    qllc_x25_vc_pkt.a_block.dst.digits = dst_x121->length;
    memcpy(qllc_x25_vc_pkt.a_block.dst.addr.x121, dst_x121->x121_addr, 
            dst_x121->length);

#ifdef TO_DO
    /*
     * Specify the Facilities Block
     */
    x25lib_facility_fillin(&qllc_x25_vc_packet,
                           ??? x25_facility_defaults,
                           ??? x25_facility_explicit_config);
#endif

    /*
     * Fill in the X25 Connection Information.
     */
    x25_conn_info.ctxt_id = ctxt_id;
    x25_conn_info.client_setup_vector = qllcx25_setup_client_ctxt;
    x25_conn_info.client_context = qllc;

    /* 
     * Fill in the Facilities Block
     */
/* TO_DO More for the Facilities Block */
    qllc_x25_vc_pkt.f_block.specified    = TRUE;

    /* 
     * Make the call
     */
    reg_invoke_x25_svc_create(&x25_conn_info, &qllc_x25_vc_pkt,
                              &vc_ptr, &result);

    if (!vc_ptr) {
        buginf("\nFailed to create an SVC");
        return (NULL);
    }
    return(vc_ptr);
}

/* 
 * qllcx25_send_call_confirm
 *
 * QLLC invokes this to send a Call Confirm Packet to an SVC.
 *
 */
boolean qllcx25_send_call_confirm (x25_vc *vc)
{
    paktype     *callpak;
    enum X25LIB_REQ_RESULT result;
    
    /* 
     * Allocate a pak for the X.25 service usage
     */
    callpak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
    if (!callpak)
         return FALSE;

    /* 
     * Build the Call Confirm packet.
     */
    memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
    qllc_x25_vc_pkt.pak      = callpak;
    qllc_x25_vc_pkt.type     = X25PID_CALL_CONF;
    qllc_x25_vc_pkt.d_bit    = FALSE;

    /*
     * Specify a zero-length address block.
     */
    qllc_x25_vc_pkt.a_block.specified   = TRUE;
    qllc_x25_vc_pkt.a_block.a_bit       = 0;
    qllc_x25_vc_pkt.a_block.src.type    = X25_ADDR_X121;
    qllc_x25_vc_pkt.a_block.src.digits  = 0;
    qllc_x25_vc_pkt.a_block.dst.type    = X25_ADDR_X121;
    qllc_x25_vc_pkt.a_block.dst.digits  = 0;

    qllc_x25_vc_pkt.f_block.specified    = TRUE;


#ifdef TO_DO
    /*
     * Specify the Facilities Block
     */
    x25lib_facility_fillin(&x25_vc_packet,
                           ??? x25_facility_defaults,
                               x25-facility_explicit_config);
#endif

    /* 
     * Send the Call Confirm Packet.
     */
    (*vc->x25.p_vect->call_conf) (vc->x25.context, &qllc_x25_vc_pkt, &result);

#ifdef TO_DO
    if (result != ) {
        return FALSE;
    }
#endif
    return(TRUE);
}

/* 
 * qllcx25_send_rr
 *
 * QLLC invokes this to send an X.25 Level RR Packet.
 *
 */
boolean qllcx25_send_rr (x25_vc *vc)
{
    paktype *rr_pak;
    enum X25LIB_REQ_RESULT result;

    /* 
     * Allocate a pak for the X.25 service usage
     */
    rr_pak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
    if (!rr_pak)
         return(FALSE);

    memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
    qllc_x25_vc_pkt.pak              = rr_pak;
    qllc_x25_vc_pkt.type             = X25PID_RR;
    qllc_x25_vc_pkt.d_bit            = FALSE;
    qllc_x25_vc_pkt.flow.pr          = x25lib_vc_tx_ack_everything_inline(vc);
    qllc_x25_vc_pkt.user_data.q_bit  = FALSE;
    qllc_x25_vc_pkt.user_data.m_bit  = FALSE;
    (*vc->x25.d1_vect->rr_tx)(vc->x25.context, &qllc_x25_vc_pkt, &result);
    qllc_x25_vc_pkt.pak              = NULL;

    return (TRUE);
}

/* 
 * qllcx25_send_rnr
 *
 * QLLC invokes this to send an X.25 Level RNR Packet.
 *
 */
boolean qllcx25_send_rnr (x25_vc *vc)
{
    paktype *rnr_pak;
    enum X25LIB_REQ_RESULT result;

    /* 
     * Allocate a pak for the X.25 service usage
     */
    rnr_pak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
    if (!rnr_pak)
         return(FALSE);

    memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
    qllc_x25_vc_pkt.pak              = rnr_pak;
    qllc_x25_vc_pkt.type             = X25PID_RNR;
    qllc_x25_vc_pkt.d_bit            = FALSE;
    qllc_x25_vc_pkt.flow.pr          = x25lib_vc_tx_ack_everything_inline(vc); /* TO_DO wrong! */
    qllc_x25_vc_pkt.user_data.q_bit  = FALSE;
    qllc_x25_vc_pkt.user_data.m_bit  = FALSE;
    (*vc->x25.d1_vect->rr_tx)(vc->x25.context, &qllc_x25_vc_pkt, &result);
    qllc_x25_vc_pkt.pak              = NULL;

    return (TRUE);
}

/* 
 * qllcx25_send_clear
 *
 * QLLC invokes this to send a Clear Packet to deactivate an SVC.
 *
 */
boolean qllcx25_send_clear (x25_vc *vc, uchar cause, uchar diag)
{
    paktype     *callpak;
    enum X25LIB_REQ_RESULT result;

    /* 
     * Allocate a pak for the X.25 service usage
     */
    callpak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
    if (!callpak)
         return(FALSE);

    /* 
     * Build the Clear packet.
     */
    memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
    qllc_x25_vc_pkt.pak = callpak;
    qllc_x25_vc_pkt.type = X25PID_CLEAR;

    /*
     * Specify the Cause Diagnostics Structure.
     */
    qllc_x25_vc_pkt.cause_diag.cause = cause;
    if (diag == X25_DIAG_NO_INFO) {
        qllc_x25_vc_pkt.cause_diag.diag_specified = FALSE;
    } 
    else {
        qllc_x25_vc_pkt.cause_diag.diag_specified = TRUE;
        qllc_x25_vc_pkt.cause_diag.diag = diag;
    }


    /* 
     * Send the Clear Packet.
     */
    (*vc->x25.p_vect->clear) (vc->x25.context, &qllc_x25_vc_pkt, &result);
    buginf("\nSend clear - result = %d", result);
#ifdef TO_DO
    if (result != ) {
        return FALSE;
    }
#endif
    return(TRUE);
}

/* 
 * qllcx25_send_clear_confirm
 *
 * QLLC invokes this to send a Clear Confirm Packet.
 *
 */
boolean qllcx25_send_clear_confirm (x25_vc *vc)
{
    paktype     *callpak;
    enum X25LIB_REQ_RESULT result;

    /* 
     * Allocate a pak for the X.25 service usage
     */
    callpak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
    if (!callpak)
         return FALSE;

    /* 
     * Build the Clear Confirm packet.
     */
    memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
    qllc_x25_vc_pkt.pak      = callpak;
    qllc_x25_vc_pkt.type     = X25PID_CLEAR_CONF;

    /* 
     * Send the Clear Confirm Packet.
     */
    (*vc->x25.p_vect->clear_conf) (vc->x25.context, &qllc_x25_vc_pkt, &result);
#ifdef TO_DO
    if (result != ) {
        return FALSE;
    }
#endif
    return(TRUE);
}

/* 
 * qllcx25_send_reset
 *
 * QLLC invokes this to send a Reset Packet to ??? a ???VC.
 *
 */
boolean qllcx25_send_reset (x25_vc *vc, uchar cause, uchar diag)
{
    paktype     *callpak;
    enum X25LIB_REQ_RESULT result;

    /* 
     * Allocate a pak for the X.25 service usage.
     */
    callpak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
    if (!callpak)
         return(FALSE);

    /*
     * Build the Reset packet.
     */
    memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
    qllc_x25_vc_pkt.pak      = callpak;
    qllc_x25_vc_pkt.type     = X25PID_RESET;

    /*
     * Specify the Cause Diagnostics Structure.
     */
    qllc_x25_vc_pkt.cause_diag.cause = cause;
    if (diag == X25_DIAG_NO_INFO) {
        qllc_x25_vc_pkt.cause_diag.diag_specified = FALSE;
    } 
    else {
        qllc_x25_vc_pkt.cause_diag.diag_specified = TRUE;
        qllc_x25_vc_pkt.cause_diag.diag = diag;
    }

    /* 
     * Send the Reset Packet.
     */
    (*vc->x25.d_vect->reset) (vc->x25.context, &qllc_x25_vc_pkt, &result);
#ifdef TO_DO
    if (result != ) {
        return(FALSE);
    }
#endif
    return(TRUE);
}

/* 
 * qllcx25_send_reset_confirm
 *
 * QLLC invokes this to send a Reset Confirm Packet to ??? a ???VC.
 *
 */
boolean qllcx25_send_reset_confirm (x25_vc *vc)
{
   paktype     *callpak;
   enum X25LIB_REQ_RESULT result;

   /* 
    * Allocate a pak for the X.25 service usage
    */
   callpak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
   if (!callpak)
       return(FALSE);

   /* 
    * Build the Reset Confirm packet.
    */
   memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
   qllc_x25_vc_pkt.pak      = callpak;
   qllc_x25_vc_pkt.type     = X25PID_RESET_CONF;

   (*vc->x25.d_vect->reset_conf) (vc->x25.context, &qllc_x25_vc_pkt, &result);
#ifdef TO_DO
   if (result != ) {
       return FALSE;
   }
#endif
   return TRUE;
}

/* 
 * qllcx25_quench
 *
 * QLLC invokes this to send:
 *      an RNR packet to turn flow control on, or
 *      an RR  packet to turn flow control off.
 *
 */
void qllcx25_quench (qllctype *qllc, boolean quench_on)
{
    paktype     *pak;
    enum X25LIB_REQ_RESULT result;
    x25_vc      *vc = NULL;

    if (qllc_event_debug)
	buginf("\nQLLC: X25 QUENCH %s", quench_on ? "ON": "OFF");

/* TO_DO : ERROR MESSAGE
    if (!qllc)
        errmsg(&msgsym(NULLPTR, QLLC),"qllc");
*/
    qllc->net_busy = quench_on;

    vc = qllc->vc;
    if (!vc) {
/* TO_DO errmsg(&msgsym(NULLPTR, QLLC), "qllc->vc"); -  check out this error msg */
        return;
    }

    /* 
     * Allocate a pak for the X.25 service usage.
     */
    pak = getbuffer(QLLC_VC_EVENT_BUF_SIZE);
    if (!pak)
         return;

    /* 
     * Build the RR or RNR packet.
     */
    memset(&qllc_x25_vc_pkt, 0, sizeof(x25_packet));
    qllc_x25_vc_pkt.pak      = pak;

    if (quench_on) {
        qllc_x25_vc_pkt.type = X25PID_RNR;
        (*vc->x25.d1_vect->rnr_tx)(vc->x25.context, &qllc_x25_vc_pkt, &result);
        qllc->qllcStat.qllc_quenchon++;
    } else {
        qllc_x25_vc_pkt.type = X25PID_RR;
        (*vc->x25.d1_vect->rr_tx)(vc->x25.context, &qllc_x25_vc_pkt, &result);
        qllc->qllcStat.qllc_quenchoff++;
    }
 
    /* 
     * If we are not quenched, send packets to lnx_output.
     */
    while (!qllc->net_busy) {
        pak = p_dequeue(&qllc->qllc_lnxQ);
        if (pak)
            lnx_output(pak, qllc);
        else
            break;
    }

    return;
}



/***********************************************************
 * Configuration Handling Functions
 ***********************************************************/
void qllcx25_config_created (x25_config *cfg,
                             parseinfo *csb,
                             boolean *error)
{
   qllcx25_cfg_ctxt  *qllc_cfg;

   if (cfg->id.type != X25SERVICE_X25)
       return;

   qllc_cfg = malloc_named(sizeof(qllcx25_cfg_ctxt),
			   "QLLC client configuration");

   if (!qllc_cfg) {
       *error = TRUE;
       return;
   }

   cfg->client.cc.client_ctxt[X25CLIENT_QLLC] = qllc_cfg;
/*
   reg_invoke_x25_config_lock(x25_config_id *id, x25_config **cfg_ptr);
*/


   return;
}

void qllcx25_config_deleted (x25_config *cfg)
{
   /* TO_DO: Destroy things (qllc_cfg, related map_root, others
    */

/*
   reg_invoke_x25_config_unlock(cfg);
*/

   return;
}

/*
 *  qllcx25_client_config_map
 *
 *  Called (via registry) when the "x25 map qllc ... " command is entered.
 *  The X.25 service will pass the following to us in this map command: 
 *
 *  csb->uiarray[0] is set to the permissible map options.
 *  csb->uiarray[1] is set to LINK_QLLC.
 *  OBJ(paddr,1) is set to the QLLC address, if specified.
 *  OBJ(paddr,2) is set to the remote host's X.121 address.
 */
void qllcx25_client_config_map (parseinfo *csb, 
                                x25_subconfig *subcfg,
                                char **msg,
                                boolean *error)
{
    addrtype vmac, x121;

    memset(&vmac, 0, sizeof(addrtype));
    memset(&x121, 0, sizeof(addrtype));

    /*
     * Was the Virtual Mac address entered (optional)?
     */
    if (GETOBJ(paddr,1) && GETOBJ(paddr,1)->length) {
        vmac.type   = ADDR_QLLC;
        vmac.length = GETOBJ(paddr,1)->length;
        memcpy(vmac.qllc_addr, GETOBJ(paddr, 1)->qllc_addr, GETOBJ(paddr,1)->length);
    }

    /*
     * If the X121 address was not entered, then this is screwed up.
     */
    if (GETOBJ(paddr,2) && (GETOBJ(paddr,2)->length)) {
        x121.type   = ADDR_X121;
        x121.length = GETOBJ(paddr,2)->length;
        memcpy(x121.x121_addr, GETOBJ(paddr,2)->qllc_addr, GETOBJ(paddr,2)->length);
    }
    else {
        *error = TRUE;
        return;
    }
    
    if (csb->sense) {
        qllcx25_map_add(&x121, &vmac, subcfg->config);
    } else {
        qllcx25_map_delete(&x121, subcfg->config);
    }

/*
 * TO_DO: More Error checking Here! csb->nvgen?
 */
    return;
}

void qllcx25_client_config_map_nvgen (parseinfo *csb, 
                                      x25_subconfig *subcfg)
{
   qllcx25_map_addr *printAddr = NULL;
   qllcx25_cfg_ctxt *qllc_cfg  = NULL;
   int n;

   /*
    * Find the map_root.
    */
   qllc_cfg = (qllcx25_cfg_ctxt *) subcfg->config->client.cc.client_ctxt[X25CLIENT_QLLC];
   if (!qllc_cfg)
        return;

   /*
    *  Print all map statements associated with this config.
    */
   for (n = 0; n < QLLCX25_HASH_SIZE; n++) {
        printAddr = qllc_cfg->anchor[n];
        while (printAddr) {
           if (printAddr->hashAddr.type == ADDR_X121) {
               if (printAddr->map->vmac == NULL) {
                   /*
                    * Just print the X.121 address
                    */
                   nv_write(TRUE, "x25 map qllc %s", printAddr->hashAddr.x121_addr);
               }
               else {
                   nv_write(TRUE, "x25 map qllc %e %s", printAddr->map->vmac->hashAddr.qllc_addr,
                                                        printAddr->hashAddr.x121_addr);
               }
           }
           printAddr = printAddr->next;
        }
   }
   return;
}



/***********************************************************
 * Map Handling Functions
 ***********************************************************/

static inline int qllcx25_hash (addrtype *search)
{
    /*
     * If vmac is passed, it is OK since search.x121_addr and
     * search.qllc_addr point at the same thing (in a union).
     */
    return(xnethash(search->x121_addr, search->length) & (QLLCX25_HASH_SIZE-1));
}

static void qllcx25_map_add (addrtype *x121,
                             addrtype *vmac,
                             x25_config *x25_cfg)
{
   qllcx25_map *qllcmap = NULL;
   qllcx25_cfg_ctxt *qllc_cfg;
   int n;
   leveltype level;

   /* 
    * Find the map_root.
    */
   qllc_cfg = (qllcx25_cfg_ctxt *) x25_cfg->client.cc.client_ctxt[X25CLIENT_QLLC];
   if (!qllc_cfg)
        return;

   /* 
    * Check to see it if already exists via X.121.
    */
   qllcmap = qllcx25_map_lookup(x121, x25_cfg);
   if (qllcmap) {
       printf("\nDuplicate X121 address for this interface - ignoring!");
       return;
   }

   /* 
    * Check to see it if already exists via VMAC as well.
    */
   if (vmac && vmac->length) {
       qllcmap = qllcx25_map_lookup(vmac, x25_cfg);
       if (qllcmap) {
           printf("\nDuplicate Virtual Mac address for this interface - ignoring!");
           return;
       }
   }

   /*
    * Allocate the map structure, the map_address structures,
    * and fill them in.
    */
   qllcmap = malloc(sizeof(qllcx25_map));
   if (qllcmap) {
       qllcmap->x121 = malloc(sizeof(qllcx25_map_addr));
       if (qllcmap->x121) {
           qllcmap->x121->next = NULL;
           qllcmap->x121->map = qllcmap;
           bcopy(x121, &(qllcmap->x121->hashAddr), sizeof(addrtype));
       }
       else {
           buginf("\nqllcx25_map_add failure - No free memory");
           free(qllcmap);
           return;
       }
       if (vmac && vmac->length) {
           qllcmap->vmac = malloc(sizeof(qllcx25_map_addr));
           if (qllcmap->vmac) {
               qllcmap->vmac->next = NULL;
               qllcmap->vmac->map = qllcmap;
               bcopy(vmac, &(qllcmap->vmac->hashAddr), sizeof(addrtype));
           }
           else {
               buginf("\nqllcx25_map_add failure - No free memory");
               free(qllcmap->x121);
               free(qllcmap);
               return;
           }
       }
       else {
          qllcmap->vmac = NULL;
       }
   }

   /*
    * Insert the hash addresses (x121 and vmac) into the hash table.
    */
   level = raise_interrupt_level(NETS_DISABLE);
   n = qllcx25_hash(x121);
   qllcmap->x121->next = qllc_cfg->anchor[n];
   qllc_cfg->anchor[n] = qllcmap->x121;
   if (qllcmap->vmac) {
       n = qllcx25_hash(vmac);
       qllcmap->vmac->next = qllc_cfg->anchor[n];
       qllc_cfg->anchor[n] = qllcmap->vmac;
   }
   reset_interrupt_level(level);
   
   return;
}

qllcx25_map * qllcx25_map_lookup (addrtype *searchAddr,
                                  x25_config *x25_cfg)
{
   qllcx25_cfg_ctxt *qllc_cfg;
   qllcx25_map_addr *hashee;

   /* 
    * Find the map_root.
    */
   qllc_cfg = (qllcx25_cfg_ctxt *) x25_cfg->client.cc.client_ctxt[X25CLIENT_QLLC];
   if (!qllc_cfg)
       return(NULL);

   /*
    * Search the hash tables via the searchAddr.
    */
   hashee = qllc_cfg->anchor[qllcx25_hash(searchAddr)];
   while (hashee) {
      if ( (searchAddr->type   == hashee->hashAddr.type) &&
           (searchAddr->length == hashee->hashAddr.length) &&
           (!bcmp(searchAddr->x121_addr, hashee->hashAddr.x121_addr, searchAddr->length)) ) {
            break;
      }
      hashee = hashee->next;
   }

   if (hashee)
       return(hashee->map);
   
   return(NULL);
}

static qllcx25_map_addr * qllcx25_remove_hash (qllcx25_map_addr **anchor,
                                               addrtype *removeAddr)
{
   leveltype level;
   qllcx25_map_addr *prev, *hashee;

   prev = NULL;
   level = raise_interrupt_level(NETS_DISABLE);
   hashee = *anchor;
   while (hashee) {
      if ( (removeAddr->type == hashee->hashAddr.type) &&
           (removeAddr->length == hashee->hashAddr.length) &&
           (!bcmp(removeAddr->x121_addr, hashee->hashAddr.x121_addr, removeAddr->length)) )
      {
           if (hashee == *anchor) {
               *anchor = hashee->next;
           } else {
               prev->next = hashee->next;
           }
           hashee->next = NULL;
           break;
      }
      prev = hashee;
      hashee = hashee->next;
   }
   reset_interrupt_level(level);
   return(hashee);
}

static void qllcx25_map_delete (addrtype *x121,
                                x25_config *x25_cfg)
{
   qllcx25_cfg_ctxt *qllc_cfg = NULL;
   qllcx25_map *map = NULL; 
   qllcx25_map_addr *mapAddr = NULL;
   addrtype *vmac = NULL;

   /* 
    * Find the map_root.
    */
   qllc_cfg = (qllcx25_cfg_ctxt *) x25_cfg->client.cc.client_ctxt[X25CLIENT_QLLC];
   if (!qllc_cfg)
        return;

   /*
    * Remove the X121 address from the hash tables.
    */
   mapAddr = qllcx25_remove_hash(&qllc_cfg->anchor[qllcx25_hash(x121)], x121);

   /*
    * Get the map, and clean up the x121 map address.
    */
   if (!mapAddr)
       return;
   map = mapAddr->map;
   map->x121 = NULL;
   free(mapAddr);

   /*
    * If there is a map entry for vmac as well, then we need to
    * remove the vmac entry from the hash table and clean up.
    */
   if (map->vmac) {
       vmac = &(map->vmac->hashAddr);
       mapAddr = qllcx25_remove_hash(&qllc_cfg->anchor[qllcx25_hash(vmac)], vmac);

       /*
        * If maps do not match, this is a bug.
        */
       if (mapAddr->map != map) {
           buginf("\nERROR with maps in qllc_x25_map_delete!");
       }
       else {
           map->vmac = NULL;
           free(mapAddr);
       }
   }

   /*
    * Finally, free the map!
    */
   free(map);
   return;
}

/****************************************************************
 * QLLC Utility Functions
 ****************************************************************/

/*
 * qllcx25_lock_vc
 */
void qllcx25_lock_vc (x25_vc *vc)
{
    x25_vc *locked_vc;

    reg_invoke_x25_vc_lock(&vc->x25.vc_id, FALSE, &locked_vc);
    if (locked_vc != vc) {
        buginf("\nError in qllcx25_lock_vc");
        return;
    }
    return;
}

/*
 * qllcx25_unlock_vc
 */
void qllcx25_unlock_vc (x25_vc *vc)
{
   vc->x25.vc_unlock(vc);
   return;
}

/*
 * qllcx25_bind_vc
 *
 * Link qllc and vc.  Also, get the qllc->vc_holdQ ptr from X.25.
 */
void qllcx25_bind_vc (qllctype *qllc, x25_vc *vc)
{
    if (!qllc || !vc) 
        return;

    qllcx25_lock_vc(vc);
    vc->client.context = qllc;
    qllc->vc = vc;

    /*
     * Fill in the vc client vectors.
     */
    vc->client.p_vect   = &qllc_p_vectors;
    vc->client.d_vect   = &qllc_d_vectors;
    vc->client.d1_vect  = &qllc_d1_vectors;
#ifdef TO_DO
    vc->client.d1_idle_msec = 0; -- Set to something (0) so we never time out!
#endif

    qllc->txqid_x25     = DEFAULT_QUEUE(qllc->idb->hwptr);

#ifdef TO_DO
    -- Here is where I want to get the qllc->vc_holdQ
       by making an invocation to the X.25 service!
#endif
}

/* 
 * qllcx25_unbind_vc
 *
 * Unlink qllc and vc.
 */
void qllcx25_unbind_vc (qllctype *qllc)
{
    x25_vc *vc;
    leveltype	level;
	
    if (qllc == NULL)
	return;
    
    level = raise_interrupt_level(NETS_DISABLE);
    vc = qllc->vc;
    qllc->vc = NULL;
		
    if (vc) {
        vc->client.context = NULL;
        qllcx25_unlock_vc(vc);
    }
    reset_interrupt_level(level);
    return;
}


/*
 * qllcx25_get_dest_address
 *
 * Get the destination X.121 address from the VC.
 */
void qllcx25_get_dst_address (x25_vc *vc, addrtype *x121_dst)
{
    memset(x121_dst, 0, sizeof(addrtype));
    x121_dst->type   = ADDR_X121;
    x121_dst->length = vc->x25.p.dest.digits;
    memcpy(x121_dst->x121_addr, vc->x25.p.dest.addr.x121, x121_dst->length);
    return;
}

/*
 * qllcx25_get_src_address
 *
 * Get the source X.121 address from the VC.
 */
void qllcx25_get_src_address (x25_vc *vc, addrtype *x121_src)
{
    memset(x121_src, 0, sizeof(addrtype));
    x121_src->type   = ADDR_X121;
    x121_src->length = vc->x25.p.source.digits;
    memcpy(x121_src->addr.x121_address, vc->x25.p.source.addr.x121, x121_src->length);
    return;
}

/*
 * qllcx25_get_if_x25_address
 *
 * Get the destination X.121 address for the VC.
 */
void qllcx25_get_if_x25_address (x25_config *cfg, addrtype *ifAddr)
{
    memset(ifAddr, 0, sizeof(addrtype));
    ifAddr->type   = ADDR_X121;
    ifAddr->length = cfg->first_subcfg.address.digits;
    memcpy(ifAddr->x121_addr, cfg->first_subcfg.address.addr.x121,
           ifAddr->length);
    return;
}

/*
 * qllcx25_get_maxdataout
 *
 * Return the Maximum Packet Size for xmits on this vc.
 *
 */
ushort qllcx25_get_maxdataout (x25_vc *vc)
{
   return (vc->x25.p.max_data_tx);
}  

/*
 * qllcx25_get_lcn
 *
 * Given the vc, return the Logical Channel Number.
 *
 */
ushort qllcx25_get_lcn (x25_vc *vc)
{
    return(vc->x25.p.vc_number);
}

/*
 * qllcx25_get_swidb_from_config
 *
 * Get a software IDB from the config
 */
idbtype *qllcx25_get_swidb_from_config(x25_config *cfg)
{
/*
 * TO_DO: What if we have multiple subinterfaces!
 */
   return(cfg->first_subcfg.swidb);
}

/*
 * qllcx25_get_swidb_from_vc
 *
 * Get a software IDB from the VC.
 */
idbtype * qllcx25_get_swidb_from_vc(x25_vc *vc)
{
   return(qllcx25_get_swidb_from_config(vc->x25.config));
}

/*
 * qllcx25_get_x25config_from_vc
 *
 * Given a vc, return the pertient x25_config
 *
 */
x25_config * qllcx25_get_config_from_vc (x25_vc *vc)
{
    return(vc->x25.config);
}

/*
 * qllcx25_get_config_from_hwidb
 *
 * Given a hwidb, find the appropriate x25_config
 *
 */
x25_config * qllcx25_get_config_from_hwidb (hwidbtype * hwidb)
{
    x25_config_id   cfg_id;
    x25_config      *cfg = NULL;

    cfg_id.type     = X25SERVICE_X25;
    cfg_id.hwidb    = hwidb;
    reg_invoke_x25_config_find(&cfg_id, &cfg);
    return(cfg);
}

/*
 * qllcx25_search_config_maps
 *
 * Only used as a callback from the X.25 code as
 * the internals of a "for all x25_configs".
 *
 */
boolean qllcx25_search_config_maps (x25_config *cfg, void *param)
{
    qllcx25_cfg_and_address *cfg_plus_addr = (qllcx25_cfg_and_address *) param;
    if (cfg_plus_addr->cfg == NULL) {
        if (qllcx25_map_lookup(cfg_plus_addr->searchAddr, cfg)) {
            cfg_plus_addr->cfg = cfg;
            return(FALSE);
        }
        else
            return(TRUE);
    }
    return(FALSE);
}

/*
 * qllcx25_get_config_from_addr
 *
 * Given a vmac or x121, return the x25_config.
 *
 */
x25_config * qllcx25_get_config_from_addr (addrtype *searchAddr)
{
    qllcx25_cfg_and_address cfg_and_address;

    cfg_and_address.cfg         = NULL;
    cfg_and_address.searchAddr  = searchAddr;
    reg_invoke_x25_config_forall(qllcx25_search_config_maps, &cfg_and_address);
    return(cfg_and_address.cfg);
}


qllctype * qllcx25_get_qllc_from_vc (x25_vc *vc)
{
    if (!vc)
        return NULL;

    return((qllctype *) vc->client.context);
}

boolean qllcx25_vc_is_pvc(x25_vc *vc)
{
/* TO_DO - implement */
    return FALSE;
}


boolean qllcx25_x25service_up (qllctype *qllc)
{
#if TO_DO
    /* Really what we want here is to find the X25 context from
     * based on the qllc pointer and determine the state of that 
     * context - For now just return TRUE is qllc exist */
#endif

    if (qllc)
        return TRUE;
    else
        return FALSE;
}

#ifdef TO_DO
static void qllcx25_pvc_setup (addrtype *proto_addr, 
                                lcitype *lci, 
                                boolean sense)
{
    lnx_pvc_setup(proto_addr, lci, sense);
}

#endif

/****************** SNMP support functions **********************/
x25_vc * qllcx25_vc_lookup_next(hwidbtype *idb, long vc_num, int dummy)
{
    return NULL;
}

x25_vc * qllcx25_vc_lookup(hwidbtype *idb, long vc_num, int dummy)
{
    return NULL;
}


