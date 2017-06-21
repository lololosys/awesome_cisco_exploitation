/* $Id: sscop_state.c,v 3.2.56.4 1996/09/06 16:40:22 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop_state.c,v $
 *------------------------------------------------------------------------
 *
 * S S C O P _ S T A T E . C
 *
 * Service Specific Connection Oriented Protocol (SSCOP) State Machine
 *
 * November 1993,    Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *-----------------------------------------------------------------------
 * $Log: sscop_state.c,v $
 * Revision 3.2.56.4  1996/09/06  16:40:22  jwjang
 * CSCdi66728:  ATMSIG: SVCs are not torn-down after peer-switch reload
 * Branch: California_branch
 *
 * Revision 3.2.56.3  1996/06/19  00:57:03  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/05/09  14:11:45  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.56.1.18.1  1996/04/27  06:34:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.56.1.8.1  1996/04/10  23:36:04  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.56.1  1996/03/18  19:00:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:30:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:36:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:47:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:13:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *-----------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "linktype.h"
#include "atmsig_public.h"
#include "sscop_private.h"
#include "sscop_public.h"
#include "mgd_timers.h"



/*
 * s s c o p _ s t a t e T a b l e ( )
 */
void sscop_stateTable (sscopType *sscop)
{

   sscop_statetype prevstate;

   
   prevstate = sscop->state;

   switch (sscop->state) {

      /* Idle State */
      case SSCOP_ST_IDLE:
         sscop_idleState(sscop);
         break;

      /* Outgoing Connect Pending State */
      case SSCOP_ST_OUTCONN_PENDING:
         sscop_outConnPendingState(sscop);
         break;

      /* Data Transfer Ready State */
      case SSCOP_ST_DATA_XFER_READY:
         sscop_dataTransferState(sscop);         
         break;

      /* Outgoing Disconnect Pending State */
      case SSCOP_ST_OUTDISCONN_PENDING:
         sscop_outDisconnPendingState(sscop);        
         break;

      /* Outgoing Resync Pending State */
      case SSCOP_ST_OUTRESYNC_PENDING:
         sscop_outResyncPendingState(sscop);
         break;

      /* Incoming Resync Pending State */
      case SSCOP_ST_INRESYNC_PENDING:
         /*sscop_inResyncPendingState(sscop);*/
         break;

      /* Concurrent Resync Pending State - UNI 3.0 only */
      case SSCOP_ST_CONRESYNC_PENDING:
         /*sscop_concurrentResyncPendingState(sscop);*/
         break;

       case SSCOP_ST_OUTRECOVERY_PENDING:
	 sscop_outRecoveryPendingState(sscop);
	 break;

       case SSCOP_ST_RECOVERYRSP_PENDING:
       case SSCOP_ST_INRECOVERY_PENDING:
      /* Invalid State */
      default:
         break;

   }

   /* state changed? */
   if ( sscop_debugEvent && (prevstate != sscop->state) )
      buginf("\nSSCOP: state changed from %s to %s", sscop_printState(prevstate),
                                                sscop_printState(sscop->state) );

}



/*
 * s s c o p _ i d l e S t a t e ( )
 *
 * Idle State 
 */
void sscop_idleState (sscopType *sscop)
{

   boolean result = TRUE;

   
   switch (sscop->event) {

      case SSCOP_RCV_BGN:
         if ( sscop_isV31(sscop) ) {
	   if ( sscop_processBeginPdu(sscop) == SSCOP_RESULT_OK ) {
	     sscop_makeConnInConnPending(sscop);	     
	   }
	   else 
	     (void)sscop_transmitBeginRejPdu(sscop);

	   break;
	 }

         sscop_initSscopCb(sscop);
         result = sscop_processBeginPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
            /* 
             * transmit Begin Ack pdu and change state to data transfer ready if
             * successful transmission, otherwise remain in idle state.
             */
            result = sscop_transmitBeginAckPdu(sscop);
            if (result == SSCOP_RESULT_OK)
               sscop->state = SSCOP_ST_DATA_XFER_READY;
         }
         else {
            /* received invalid Begin pdu, reject request - remain in idle state */
            (void)sscop_transmitBeginRejPdu(sscop);
         }
         break;


      case SSCOP_TX_BEGIN:
         /*
          * transmit Begin Pdu and change state to Outgoing Connection Pending
          * after successful transmission.  Start timer.
          */

	 /* do it for both versions to save a version checking. uni 3.0 
	    won't use it anyway */
	 SSCOP_INC_VTSQ(sscop->vtSq);

         result = sscop_transmitBeginPdu(sscop);
         /* no free buffers will result in failure of sending a Begin pdu */ 
         if (result == SSCOP_RESULT_OK) {
            sscop->state = SSCOP_ST_OUTCONN_PENDING;
            sscop_startTimer(sscop, sscop->ccTimerValue);
            sscop->retryCount = 0;
         }
         break;


      case SSCOP_RCV_END:
         /* 
          * send an End Ack pdu to an illegal End pdu received in idle state,
          * no state change - state still remains idle. 
          */
         result = sscop_processEndPdu(sscop);
         if (result == SSCOP_RESULT_OK)
            sscop_transmitEndAckPdu(sscop);
         break;


      /* ignore EndAck and Begin Rej pdu's - illegal in idle state */
      case SSCOP_RCV_BGREJ:
	 if ( sscop_isV31(sscop) )
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_D,sscop->index);
	 /* let it fall through */
	 
      case SSCOP_RCV_ENDAK:
         SSCOP_STATS(sscop->stats.Ignored);
         break;


      /* illegal pdu's in idle state - transmit an End pdu */
      case SSCOP_RCV_POLL:
	 if ( sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_G,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */

      case SSCOP_RCV_ER:
	 if ( result && sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_L,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */

      case SSCOP_RCV_SD:
	 if ( result && sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_A,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */

      case SSCOP_RCV_BGAK:
	 if ( result && sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_C,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */

      case SSCOP_RCV_STAT:
	 if ( result && sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_H,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */

      case SSCOP_RCV_USTAT:
	 if ( result && sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_I,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */

      case SSCOP_RCV_RS:
	 if ( result && sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_J,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */

      case SSCOP_RCV_SDP:
	 if ( result && sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_L,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */

      case SSCOP_RCV_RSAK:
	 if ( result && sscop_isV31(sscop) ) {
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_K,sscop->index);
	   result = FALSE;
	 }
	 /* let it fall through */


      case SSCOP_RCV_ERAK:
	 if ( result && sscop_isV31(sscop) ) 
	   sscop_maaErrorIndication(sscop->state,MAA_ERROR_M,sscop->index);
	 /* let it fall through */


      default:
         sscop_transmitEndPdu(sscop);
         SSCOP_STATS(sscop->stats.Ignored);
         break;

   }

}



/*
 * s s c o p _ o u t C o n n P e n d i n g S t a t e ( )
 *
 * Outgoing Connection Pending State
 */
void sscop_outConnPendingState (sscopType *sscop)
{

   boolean result;


   switch (sscop->event) {

      /* received Begin Ack pdu - valid response in this state */ 
      case SSCOP_RCV_BGAK:

         if ( sscop_isV31(sscop) ) {
	   if (sscop_processBeginAckPdu(sscop) == SSCOP_RESULT_OK) {
	     sscop_enterDataXferReady(sscop);
	   }
	   break;
	 }

         sscop_initSscopCb(sscop);
         result = sscop_processBeginAckPdu(sscop);
         /* 
          * change state to Data Transfer Ready after validating Begin Ack.
          * Sscop was initialized when Begin Pdu was transmitted, so no need
          * to initialize it here again. 
          */
         if (result == SSCOP_RESULT_OK) {
            sscop->state = SSCOP_ST_DATA_XFER_READY;
            /* notify higher layers, stop connection control timer */
            sscop_stopTimer(sscop);
            sscop->retryCount = 0;
         } 
         break;


      /* 
       * call collision - transmit a Begin Ack pdu.
       * The peer transmitter must respond to our Begin pdu with a Begin Ack pdu. 
       */
      case SSCOP_RCV_BGN:

	 if ( sscop_isV31(sscop) ) {
	   if (sscop_processBeginPdu(sscop) == SSCOP_RESULT_OK) {
	     SSCOP_INIT_VRMR(sscop->vmr);
	     sscop_transmitBeginAckPdu(sscop);
	     sscop_enterDataXferReady(sscop);
	   }
	   break;
	 }

         sscop_initSscopCb(sscop);
         result = sscop_processBeginPdu(sscop);
         /* transmit a Begin Ack pdu after validation */ 
         if (result == SSCOP_RESULT_OK) {
            sscop_transmitBeginAckPdu(sscop);
            sscop->state = SSCOP_ST_DATA_XFER_READY;
            /* notify higher layers, stop connection control timer */
            sscop_stopTimer(sscop);
            sscop->retryCount = 0;
         }
         break;


      /* peer rejected our call request - go back to idle state */
      case SSCOP_RCV_BGREJ:
         result = sscop_processBeginRejPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
	   if ( sscop_isV31(sscop) ) {
	     sscop_stopTimer(sscop);
	     /* if we receive too many begin rej pdu in this state,
		increase Vt(sq) to avoid dead lock */
	     sscop->begin_rej_count++;
	     if (sscop->begin_rej_count++ > SSCOP_BEGINREJ_MAX) {
	       SSCOP_INC_VTSQ(sscop->vtSq);
	       sscop->begin_rej_count = 0;
	     }
	     else
	       sscop->vtSq = 0;
	     sscop->state = SSCOP_ST_IDLE;
	   }
	   else
	     sscop_initSscopCb(sscop);
         }
         break;

 
      /* invalid pdu's in outgoing connection pending state - ignore */ 

     case SSCOP_RCV_ER:
	 if ( !sscop_isV31(sscop) ) {
	   if (sscop_debugError)
	     buginf("\nSCPERR(%d) - i ER Pdu in UNI 3.0",sscop->index);
	 /* else let it fall through */
	   break;
	 }

     case SSCOP_RCV_ERAK:
	 if ( !sscop_isV31(sscop) )
	   if (sscop_debugError)
	     buginf("\nSCPERR(%d) - i ERAK Pdu in UNI 3.0",sscop->index);
	 /* else let it fall through */


      case SSCOP_RCV_SDP:

	 if ( sscop_isV31(sscop) ) 
	   if (sscop_debugError)
	     buginf("\nSCPERR(%d) - i SDP Pdu in UNI 3.1",sscop->index);
	 /* else let it fall through */

      case SSCOP_RCV_ENDAK:
      case SSCOP_RCV_SD:
      case SSCOP_RCV_END:
      case SSCOP_RCV_STAT:
      case SSCOP_RCV_USTAT:
      case SSCOP_RCV_POLL:
      case SSCOP_RCV_RS:
      case SSCOP_RCV_RSAK:
      default:
         SSCOP_STATS(sscop->stats.Ignored);
         break;

   }
 
}



/*
 * s s c o p _ d a t a T r a n s f e r S t a t e ( )
 *
 * Data Transfer State
 */
void sscop_dataTransferState (sscopType *sscop)
{

   boolean result;


   switch (sscop->event) {

      /* higher layer wants to restart the connection */
      case SSCOP_TX_BEGIN:
         /*
          * transmit Begin pdu and change state to Outgoing Connection Pending
          * after successful transmission.  Start connection control timer.
          */

         /* it should never happen in uni 3.1. and it does
	    not seem to occur in 3.0 in current implementation */
         if ( sscop_isV31(sscop) ) {
	   if (sscop_debugEvent)
	     buginf("\nSCPEVNT(%d) - establish request in DataXferReady state",
		    sscop->index);
	   
	   break;
	 }

	 result = sscop_transmitBeginPdu(sscop);
	 sscop_initSscopCb(sscop);     /* initialize sscop control block */

         if (result == SSCOP_RESULT_OK) {
            sscop->state = SSCOP_ST_OUTCONN_PENDING;
            sscop_startTimer(sscop, sscop->ccTimerValue);
            sscop->retryCount = 0;
         }
         break;

 
      /* higher layer wants us to resync our transmission side */ 
      case SSCOP_TX_RESYNC:

	 if ( sscop_isV31(sscop) ) {
	   sscop_resetDataXferTimers(sscop);
	   SSCOP_INC_VTSQ(sscop->vtSq);
	   SSCOP_INIT_VRMR(sscop->vmr);
	   if (sscop_transmitResyncPdu(sscop) == SSCOP_RESULT_OK)
	     sscop->retryCount = 1;
	   else
	     sscop->retryCount = 0;
	   sscop_releaseBuffers(sscop);
	   /* start Timer_CC */
	   sscop_startTimer(sscop,sscop->ccTimerValue);  
	   sscop->state = SSCOP_ST_OUTRESYNC_PENDING;
	   break;
	 }

         sscop_resetTransmitter(sscop);
         sscop_transmitResyncPdu(sscop);
         sscop->state = SSCOP_ST_OUTRESYNC_PENDING;
         /* start connection control timer */
         sscop_startTimer(sscop, sscop->ccTimerValue);
         sscop->retryCount = 0;
         break;


      /* 
       * higher layer wants us to close the connection, after a valid transmission
       * of an End pdu change state to outgoing disconnect pending.  Start timer.
       * timer. 
       */
      case SSCOP_TX_END:

	 if ( sscop_isV31(sscop) ) {
	   sscop_resetDataXferTimers(sscop);
	   if (sscop_transmitEndPdu(sscop) == SSCOP_RESULT_OK) 
	     sscop->retryCount = 1;
	   else  /* if fail to send end pdu, send it next time */
	     sscop->retryCount = 0;

	   /* prepare for retrieval */
	   sscop_releaseBuffers(sscop);
	   /* start Timer_CC */
	   sscop_startTimer(sscop,sscop->ccTimerValue);  
	   sscop->state = SSCOP_ST_OUTDISCONN_PENDING;
	   break;
	 }

         result = sscop_transmitEndPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
            sscop->state = SSCOP_ST_OUTDISCONN_PENDING;
            sscop_startTimer(sscop, sscop->ccTimerValue);
            sscop->retryCount = 0;
         }
         else {
            /*
             * Unsucessful transmission of an End pdu means we are out of buffers,
             * close the connection by initializing sscop and going back to the
             * idle state.
             */
         } 
         break;


      /* our peer wants to restart the connection */ 
      case SSCOP_RCV_BGN:

         if ( sscop_isV31(sscop) ) {
           result = sscop_processBeginPdu(sscop);
           switch (result) {
           case SSCOP_PDU_RETRANSMIT:
	     sscop_stopTimerNoResponse(sscop);
	     sscop_transmitBeginAckPdu(sscop);
	     sscop_startTimerNoResponse(sscop);
             process_send_message(atmsig_timer_pid, ATMSIG_SSCOP_ESTABLISH, sscop, 0);
             break;

           case SSCOP_RESULT_OK:
	     /* reset timers, infor the upper layer 
		and go to inConnPending state */
             sscop_aaReleaseIndication(sscop);
             sscop_resetDataXferTimers(sscop);
             sscop_deliverData(sscop); /* clear receive buffer */
             sscop_makeConnInConnPending(sscop);
	     break;

           case SSCOP_PDU_NEW:
             /* we treat it as a special case. when we receive nsq=1 in the ready
                state, we start from scratch instead of polling status enquiry */
             sscop_initSscopCb(sscop);
             break;

           case SSCOP_PDU_BAD:
           default:
             break;
	   }
	 }

         result = sscop_processBeginPdu(sscop);
         sscop_initSscopCb(sscop);     /* initialize sscop control block */
         if (result == SSCOP_RESULT_OK) {
            result = sscop_transmitBeginAckPdu(sscop);
            if (result == SSCOP_RESULT_OK)
               sscop->state = SSCOP_ST_DATA_XFER_READY;
            else
               sscop->state = SSCOP_ST_IDLE;
         }
         break;


      case SSCOP_RCV_POLL:
         sscop_processPollPdu(sscop);
         break;


      case SSCOP_RCV_SD:
	 sscop_processSdpPdu(sscop,FALSE);
	 break;


      case SSCOP_RCV_END:
         result = sscop_processEndPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
	   sscop_transmitEndAckPdu(sscop);
	   sscop_initSscopCb(sscop); 
	 }
         break;


      case SSCOP_TX_DATA:
         sscop_sendSdPdu(sscop);
         break;


      case SSCOP_TX_KEEPALIVE:
	 /* this should never happen in 3.1 code */
	 if ( !sscop_isV31(sscop) ) {
	   sscop_stopTimer(sscop);
	   if (sscop->retryCount < sscop->maxRetryCount) {
	     mgd_timer_stop(&sscop->keepalive_timer);
	     sscop_transmitPollPdu(sscop);
	     ++sscop->retryCount;
	     sscop_startTimer(sscop, sscop->pollTimerValue);
	   }
	   else {
	     sscop_transmitEndPdu(sscop);
	     sscop_initSscopCb(sscop);
	   } 
	 }
	 else if (sscop_debugError)  
	   buginf("\nSCPERR(%d): SSCOP_TX_KEEPALIVE in 3.1",sscop->index);

	 break;


      case SSCOP_RCV_RS:
	 if ( sscop_isV31(sscop) ) {
	   if (sscop_processResyncPdu(sscop) != SSCOP_RESULT_OK) {
	     /* bad or retransmission */
	     sscop_stopTimerNoResponse(sscop);
	     sscop_transmitResyncAckPdu(sscop);
	     sscop_startTimerNoResponse(sscop);
	   }
	   else {
	     sscop_resetDataXferTimers(sscop);

	     /* go to inResyncPending State */
	     SSCOP_INIT_VRMR(sscop->vmr);
	     sscop_transmitResyncAckPdu(sscop);

	     /* prepare retriveal */
	     sscop_releaseBuffers(sscop);	     
	     sscop_initStateVars(sscop);
	     sscop_setDataXferTimers(sscop);
	   }

	   /* stay in the SSCOP_ST_DATA_XFER_READY */
	   break;
	 }

         result = sscop_processResyncPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
            sscop_resetReceiver(sscop);
            sscop_transmitResyncAckPdu(sscop);
         }
         break;


      case SSCOP_RCV_SDP:
         sscop->retryCount = 0;
         sscop_stopTimer(sscop);
         sscop_processSdpPdu(sscop, TRUE);
         break;


      case SSCOP_RCV_STAT:
	 if ( sscop_isV31(sscop) ) {
	   sscop_processStatPdu(sscop);
	   break;
	 }

         sscop->retryCount = 0;
         sscop_stopTimer(sscop);
         sscop_processStatPdu(sscop);
         break;

      case SSCOP_RCV_USTAT:
         sscop_processUstatPdu(sscop);
         break;

       case SSCOP_RCV_ER:
	 if ( !sscop_isV31(sscop) ) 
	   if (sscop_debugError)
	     buginf("\nSCPERR(%d) - rcv ER Pdu in uni 3.0",sscop->index);
	 else {
	   if (sscop_processErPdu(sscop) != SSCOP_RESULT_OK) {
	     /* retransmit or bad */
	     sscop_startTimerNoResponse(sscop);
	     sscop_transmitErAckPdu(sscop);
	   }
	   else {
	     /* don't call sscop_initSscopCb() because we don't want
		to inform atmsig the sscop failure & want to stay in
		this state */
	     sscop_resetDataXferTimers(sscop);
	     
	     /* go to incoming Recovery Pending state & finish things */
	     sscop_clearTransmitter(sscop);
	     SSCOP_INIT_VRMR(sscop->vmr);
	     sscop_transmitErAckPdu(sscop);
	     sscop_initStateVars(sscop);
	     sscop_setDataXferTimers(sscop);
	     /* stay in data xfer ready state */
	   }
	 }

	 break;

      /* ignore - illegal pdu's in data transfer state */ 
      case SSCOP_RCV_ENDAK:
	 if ( sscop_isV31(sscop) ) {
	   if (sscop_processEndPdu(sscop) == SSCOP_RESULT_OK) {
	     sscop_maaErrorIndication(sscop->state,MAA_ERROR_F,sscop->index);
	     sscop_aaReleaseIndication(sscop);
	     sscop_requestConnection(sscop);
	     break;
	   }
	 }
	 /* else it's uni 3.0. let it fall through */


      case SSCOP_RCV_BGREJ:
         result = sscop_processBeginRejPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
           if ( sscop_isV31(sscop) ) {
             sscop_maaErrorIndication(sscop->index,MAA_ERROR_D,sscop->index);
	     sscop_aaReleaseIndication(sscop);
	     sscop_requestConnection(sscop);
             break;
           }
           sscop_initSscopCb(sscop);
         }
         break;

      case SSCOP_RCV_ERAK:
      case SSCOP_RCV_RSAK:
      case SSCOP_RCV_BGAK:
      default:
         SSCOP_STATS(sscop->stats.Ignored);
         break;

   }

}



/*
 * s s c o p _ o u t D i s c o n n P e n d i n g S t a t e ( )
 *
 * Outgoing Disconnect Pending State.  Entered when an End Pdu is transmitted.
 */
void sscop_outDisconnPendingState (sscopType *sscop)
{

   boolean result;


   switch (sscop->event) {

      case SSCOP_TX_BEGIN:
     /* this will never happen in current implementation */
         if ( sscop_isV31(sscop) ) {
	   sscop_initSscopCb(sscop);
	   SSCOP_INC_VTSQ(sscop->vtSq);
	   (void)sscop_transmitBeginPdu(sscop);
	 }
	 else	 {
         (void)sscop_transmitBeginPdu(sscop);
         sscop_initSscopCb(sscop);
         }

         sscop->state = SSCOP_ST_OUTCONN_PENDING;
         /* start connection control Timer */
         sscop_startTimer(sscop, sscop->ccTimerValue);
         sscop->retryCount = 0;
         break;

      /* call clear collision */
      case SSCOP_RCV_END:
         result = sscop_processEndPdu(sscop);
         if (result == SSCOP_RESULT_OK) { 
            result = sscop_transmitEndAckPdu(sscop);

	    /* if fail to send EndAck, still go into idle
	       state and reply the next end pdu from there */
	    if ( sscop_isV31(sscop) ) {
	      sscop_initSscopCb(sscop);
	      break;
	    }

            if (result == SSCOP_RESULT_OK) {
               /* stop cc timer, notify higher layers */
               sscop->state = SSCOP_ST_IDLE;
            }
         }
         break;


      case SSCOP_RCV_BGN:
	 if ( sscop_isV31(sscop) ) {
	   if (sscop_processBeginPdu(sscop) != SSCOP_RESULT_OK) {
	     /* must be retransmission, stay in this state */
	     sscop_transmitBeginAckPdu(sscop);
	     sscop_transmitEndPdu(sscop);
	   }
	   else {
	     sscop_stopTimer(sscop);  /* reset Timer_CC */
	     /* go to SSCOP_ST_DATA_XFER_READY through Incoming Conn 
		Pending state */
	     sscop_makeConnInConnPending(sscop);
	   }
	   break;
	 }

         sscop_initSscopCb(sscop);
         result = sscop_processBeginPdu(sscop);
         if (result == SSCOP_RESULT_OK) { 
            /* stop CC timer, notify higher layers */
            sscop_stopTimer(sscop);
            sscop->retryCount = 0; 
            result = sscop_transmitBeginAckPdu(sscop);
            if (result == SSCOP_RESULT_OK)
               sscop->state = SSCOP_ST_DATA_XFER_READY;
         }
         break;


      case SSCOP_RCV_ENDAK:
         result = sscop_processEndAckPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
            /* stop connection control timer, notify higher layers */
            sscop_stopTimer(sscop);
            sscop_initSscopCb(sscop);
         } 
         break;


      case SSCOP_RCV_BGREJ:
	 if ( sscop_isV31(sscop) ) {
	   /* unnecessary to inform atmsig to tear down svc because
	      we never reach data xfer ready state.
	    */
	   if (sscop_processBeginRejPdu(sscop) == SSCOP_RESULT_OK)  {
	     sscop_stopTimer(sscop);  /* stop Timer_CC */
	     sscop->state = SSCOP_ST_IDLE;
	   }
	 }
	 else  /* UNI 3.0 */
	   sscop_stopTimer(sscop);    /* stop Timer_CC */
	 break;


      /* ignore the following Pdu's */
      case SSCOP_RCV_ER:
      case SSCOP_RCV_ERAK:
      case SSCOP_RCV_SD:
      case SSCOP_RCV_BGAK:
      case SSCOP_RCV_POLL:
      case SSCOP_RCV_STAT:
      case SSCOP_RCV_USTAT:
      case SSCOP_RCV_SDP:
      case SSCOP_RCV_RS:
      case SSCOP_RCV_RSAK:
      default:
         SSCOP_STATS(sscop->stats.Ignored);
         break;

   }

}



/*
 * s s c o p _ o u t R e s y n c P e n d i n g S t a t e ( )
 *
 * Outgoing Resync Pending State
 */
void sscop_outResyncPendingState (sscopType *sscop) 
{

   boolean result;
   int errCode = 0;


   switch (sscop->event) {

      /*
       * Begin Pdu received when Resync Ack was expected, clean up the connection
       * and go back in Data Transfer state after transmitting Begin Ack pdu.
       */
      case SSCOP_RCV_BGN:
         if ( sscop_isV31(sscop) ) {
	   if (sscop_processBeginPdu(sscop) != SSCOP_RESULT_OK) {
	     /* retransmission */
	     sscop_transmitBeginAckPdu(sscop);
	     sscop_transmitResyncPdu(sscop);
	   }
	   else {
	     sscop_stopTimer(sscop);  /* reset Timer_CC */
	     sscop_aaReleaseIndication(sscop);
	     sscop_makeConnInConnPending(sscop);
	   }

	   break;
	 }

         result = sscop_processBeginPdu(sscop);
         sscop_initSscopCb(sscop);     /* initialize sscop control block */
         if (result == SSCOP_RESULT_OK) {
            sscop_initSscopCb(sscop);
            (void)sscop_transmitBeginAckPdu(sscop);
            sscop->state = SSCOP_ST_DATA_XFER_READY;
         } 
         break;


      case SSCOP_RCV_ENDAK:
	 errCode = MAA_ERROR_F;

      case SSCOP_RCV_BGREJ:
	 if (!errCode)
	   errCode = MAA_ERROR_D;

         result = sscop_processEndAckPdu(sscop);
         if (result == SSCOP_RESULT_OK)  {
	   if ( sscop_isV31(sscop) )
	     sscop_maaErrorIndication(sscop->state,errCode,sscop->index);

	   sscop_initSscopCb(sscop);
	 }
         break;


      case SSCOP_RCV_END:
         result = sscop_processEndPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
            sscop_transmitEndAckPdu(sscop);
            sscop_initSscopCb(sscop);
         }
         break;


      case SSCOP_RCV_RSAK:
         /* stop timer CC */
         result = sscop_processResyncAckPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
	   sscop_stopTimer(sscop);
	   if (sscop_isV31(sscop)) {
	     sscop_enterDataXferReady(sscop);
	     break;
	   }
	   else
	     sscop->retryCount = 0;
            sscop->state = SSCOP_ST_DATA_XFER_READY;
         }
         break;


      /*
       * Resync Pdu collision - reset all Receive state entities and transmit a
       * Resync Ack pdu.  Note that we completely process a Resync Pdu here itself,
       * instead of going into a Concurrent Resync Pending State.
       */ 
      case SSCOP_RCV_RS:
         result = sscop_processResyncPdu(sscop);
         if (result == SSCOP_RESULT_OK) {
	   if ( sscop_isV31(sscop) ) {
	     SSCOP_INIT_VRMR(sscop->vmr);
	     sscop_transmitResyncAckPdu(sscop);
	     sscop_enterDataXferReady(sscop);
	     break;
	   }

            sscop_resetReceiver(sscop);
            (void)sscop_transmitResyncPdu(sscop);
         }
         break; 


      case SSCOP_RCV_SD:
         sscop_processSdpPdu(sscop, FALSE);
         break;


      case SSCOP_RCV_SDP:
         sscop_processSdpPdu(sscop, TRUE);
         break;


      case SSCOP_RCV_POLL:
	 /* don't respond to the poll because not in data xfer ready state */
	 if ( !sscop_isV31(sscop) )
	   sscop_processPollPdu(sscop);
         break;


      /* higher layers want to restart the connection */
      case SSCOP_TX_BEGIN:
	 if ( sscop_isV31(sscop) ) {
	   /* not defined in uni 3.1, ignore it */
	   if (sscop_debugEvent) 
	     buginf("\nSCPEVNT(%d) - conn req in outResyncPending, uni 3.1",sscop->index);

	   break;
	 }

         /*
          * Transmit Begin Pdu and change state to Outgoing Connection Pending
          * after successful transmission.  Start Timer.
          */
         result = sscop_transmitBeginPdu(sscop);
         sscop_initSscopCb(sscop);     /* initialize sscop control block */
         if (result == SSCOP_RESULT_OK) {
            sscop->state = SSCOP_ST_OUTCONN_PENDING;
            sscop_startTimer(sscop, sscop->ccTimerValue);
            sscop->retryCount = 0;   
         }
         break;
 
      case SSCOP_RCV_ER:
      case SSCOP_RCV_ERAK:
      case SSCOP_RCV_BGAK:
      case SSCOP_RCV_STAT:
      case SSCOP_RCV_USTAT:
      default:
         SSCOP_STATS(sscop->stats.Ignored);
         break;

   }


}




/*
 * s s c o p _ i n R e s y n c P e n d i n g S t a t e ( )
 *
 *  In Resync Pending State
 */
void sscop_inResyncPendingState (sscopType *sscop)
{


}



void sscop_outRecoveryPendingState (sscopType *sscop)
{

  /* ORDER IS IMPORTANT IN THIS ROUTINE. DO NOT CHANGE IT UNLESS
     YOU KNOW WHAT YOU ARE DOING */
  switch (sscop->event) {

  case SSCOP_RCV_RS:
    if (sscop_processResyncPdu(sscop) != SSCOP_RESULT_OK)
	sscop_maaErrorIndication(sscop->state, MAA_ERROR_J, sscop->index);
    else {
      SSCOP_INIT_VRMR(sscop->vmr);
      sscop_transmitResyncAckPdu(sscop);
      sscop_clearTransmitter(sscop);
      sscop_enterDataXferReady(sscop);
    }    
    break;

  case SSCOP_RCV_ER:
    if (sscop_processErPdu(sscop) != SSCOP_RESULT_OK)
	sscop_maaErrorIndication(sscop->state, MAA_ERROR_L, sscop->index);
    else {
      SSCOP_INIT_VRMR(sscop->vmr);
      sscop_transmitErPdu(sscop);
      sscop_deliverData(sscop);
      sscop_enterDataXferReady(sscop);
    }
    break;

  case SSCOP_RCV_BGN:
    if (sscop_processBeginPdu(sscop) != SSCOP_RESULT_OK)  /* retransmission */
	sscop_maaErrorIndication(sscop->state, MAA_ERROR_B, sscop->index);
    else {
      sscop_stopTimer(sscop);  /* reset Timer_CC */
      sscop_aaReleaseIndication(sscop);
      sscop_deliverData(sscop); /* clear receive buffer */
      sscop_makeConnInConnPending(sscop);
    }
    break;

  case SSCOP_RCV_ENDAK:
    if (sscop_processEndAckPdu(sscop) != SSCOP_RESULT_OK)
      break;
    sscop_maaErrorIndication(sscop->state, MAA_ERROR_C, sscop->index);
    sscop_initSscopCb(sscop);
    break;

  case SSCOP_RCV_BGREJ:
    if (sscop_processBeginRejPdu(sscop) != SSCOP_RESULT_OK)
      break;

    sscop_maaErrorIndication(sscop->state, MAA_ERROR_C, sscop->index);
    sscop_initSscopCb(sscop);
    break;

  case SSCOP_RCV_END:
    if (sscop_processEndPdu(sscop) == SSCOP_RESULT_OK) {
      sscop_transmitEndAckPdu(sscop);
      sscop_initSscopCb(sscop);
    }
    break;

  case SSCOP_RCV_ERAK:
    if (sscop_processErAckPdu(sscop) == SSCOP_RESULT_OK) {
      sscop_deliverData(sscop);
      /* go thru Recovery Response Pending State */
      sscop_enterDataXferReady(sscop);
    }
    break;

  case SSCOP_RCV_BGAK:
    sscop_maaErrorIndication(sscop->state, MAA_ERROR_C, sscop->index);
    SSCOP_STATS(sscop->stats.Ignored);
    break;

  case SSCOP_RCV_RSAK:
    sscop_maaErrorIndication(sscop->state, MAA_ERROR_K, sscop->index);

  case SSCOP_RCV_STAT:
  case SSCOP_RCV_USTAT:
  case SSCOP_RCV_POLL:
  case SSCOP_RCV_SD:
  default:
    SSCOP_STATS(sscop->stats.Ignored);
    break;

  }
}





