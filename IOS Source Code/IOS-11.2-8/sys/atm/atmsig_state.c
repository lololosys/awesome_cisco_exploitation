/* $Id: atmsig_state.c,v 3.4.20.4 1996/07/10 22:20:32 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_state.c,v $
 *------------------------------------------------------------------
 *
 * A T M S I G _ S T A T E . C
 *
 * ATM Signalling (Q.93B) State Machine
 *
 * November  1993,   Percy P. Khabardar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_state.c,v $
 * Revision 3.4.20.4  1996/07/10  22:20:32  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.4.20.3  1996/06/27  22:23:35  jwjang
 * CSCdi61556:  wrong cause in atmsig status msg responding status enquiry
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/06/19  00:56:53  jwjang
 * CSCdi57794:  atmsig setup message for classical ip has different IEs
 * format
 * CSCdi59379
 * Branch: California_branch
 *
 * Revision 3.4.20.1  1996/05/09  14:11:19  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.50.1  1996/04/27  06:33:42  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.40.1  1996/04/10  23:35:39  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.4  1996/02/01  05:59:51  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/12/05  19:46:43  schrupp
 * CSCdi43528:  router fails to respond to status request after clear int
 * Handle RCV_STATUS in NULL state.
 *
 * Revision 3.2  1995/11/17  08:46:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/24  14:38:17  schrupp
 * CSCdi36346:  Spurious access in atmsig_state
 *
 * Revision 2.1  1995/06/07  20:12:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "mgd_timers.h"
#include "address.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/atm.h"
#include "atmsig_api.h"
#include "../if/static_map.h"
#include "atmsig_private.h"


static int getCallState(atmSvcType *, int);

/*
 * a t m S i g _ s t a t e T a b l e ( )
 *
 */
void atmSig_stateTable (atmSvcType *svc)
{

   atmSig_statetype prevstate;

   prevstate = svc->state;

   switch (svc->state) {

      /* Null State */
      case ATMSIG_ST_NULL:
         atmSig_nullState(svc);
         break;

      /* Call Initiated State */
      case ATMSIG_ST_CALL_INITIATED:
         atmSig_callInitiatedState(svc);        
         break;

      /* Outgoing Call Proceeding State */
      case ATMSIG_ST_OUTCALL_PROCEEDING:
         atmSig_outCallProceedingState(svc);         
         break;

      /* Call Present State */
      case ATMSIG_ST_CALL_PRESENT:
         atmSig_callPresentState(svc);
         break;

      /* Connect Request State */
      case ATMSIG_ST_CONNECT_REQ:
         atmSig_connectReqState(svc);         
         break;

      case ATMSIG_ST_INCALL_PROCEEDING:
         atmSig_inCallProceedingState(svc);
         break;

      /* Active State */
      case ATMSIG_ST_ACTIVE:
         atmSig_activeState(svc);         
         break;

      /* Release Request State */
      case ATMSIG_ST_RELEASE_REQ:
         atmSig_releaseReqState(svc);          
         break;

      /* Release Indication State */
      case ATMSIG_ST_RELEASE_IND:
         atmSig_releaseIndState(svc);
         break;
    
      /* Restart State */ 
      case ATMSIG_ST_RESTART_REQ:
         atmSig_restartReqState(svc);
         break;

      /* Invalid State */
      default:
         break;

   }

   /* state changed? */
   if ( (atmSig_debugAll || atmSig_debugEvent) && 
                                     svc && (prevstate != svc->state) )
      buginf("\nATMSIG: state changed from %s to %s", 
          atmSig_printState(prevstate), atmSig_printState(svc->state) );

}
 



/*
 * a t m S i g _ n u l l S t a t e ( ) 
 *
 * A SETUP request causes this state to be entered.  No other Signalling
 * Message will enter in this state since a SETUP message is ENTIRELY
 * processed here and will CHANGE state immediately.
 */
void atmSig_nullState (atmSvcType *svc)
{

   atmSig_msgHeader *hdr;
   atmSvcPartyType *party;
   int result;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: No party in null State");
     svc->freeSvc = TRUE;
     return;
   }

   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
      buginf("\nATMSIG: i %s msg in null State, length %d, call ref %d",
             atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]), 
             GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );        

   switch (svc->event) {

      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

        atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
        break;


      /* invalid messages in NULL state */
      case ATMSIG_RCV_SETUP:
      case ATMSIG_RCV_CALL_PROCEEDING:
      case ATMSIG_RCV_CONNECT:
      case ATMSIG_RCV_CONNECT_ACK:
      case ATMSIG_RCV_RELEASE:
         atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_INCOMPAT_MSG);
         atmSig_statsIgnored++;
         break;


      case ATMSIG_REQ_SETUP:
         if ( atmSig_txSetupMsg(svc) ) {
            atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t303);
            party->retryCount = 0;
            svc->state = ATMSIG_ST_CALL_INITIATED;
         } 
         else {
            /* Setup message not sent - we're out of buffers!! */
            if (atmSig_debugAll || atmSig_debugError)
               buginf("\nATMSIG: nullState: no buffer to send Setup msg");
         }
         break;


      case ATMSIG_REQ_CONNECT:
      case ATMSIG_REQ_RELEASE:
      case ATMSIG_REQ_RELEASE_COMPLETE:
         if (atmSig_debugAll || atmSig_debugError) {
            buginf("\nATMSIG: %s Event in %s state ignored",
               atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_statsIgnored++;
         break;


	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugError) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;

      /* unknown message */
      default:
         atmSig_statsIgnored++;
         break;

   }

}



/*
 * a t m S i g _ c a l l I n i t i a t e d S t a t e ( )
 *
 * This state exists when the user initiated a connection by sending a SETUP message. 
 * A Call Proceeding message received would change state to Outgoing Call Proceeding
 * State (U3).  A Connect message received would notify the client and change state
 * to Active.
 */
void atmSig_callInitiatedState (atmSvcType *svc)
{

   int result;
   atmSig_msgHeader *hdr;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: No party in Call Init State");
     svc->freeSvc = TRUE;
     return;
   }

   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;
 
   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
      buginf("\nATMSIG: i %s msg in call Initiated State, length %d, call ref %d",
                     atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
                     GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );

   switch (svc->event) {

      /* sending of Call Proceeding message is optional. */ 
      case ATMSIG_RCV_CALL_PROCEEDING:
         result = atmSig_processCallProceedingMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */ 
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Change state to Outgoing Call Proceeding. */
         atmSig_stopTimer(party);
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t310);
         svc->state = ATMSIG_ST_OUTCALL_PROCEEDING;
         break;


      /* switch can directly send a Connect msg in response to our Setup msg */
      case ATMSIG_RCV_CONNECT:
         result = atmSig_processConnectMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* message accepted */
         atmSig_stopTimer(party);
         party->retryCount = 0;

         /*
          * Notify client that we've received a Connect message in response to
          * our Setup message.    
          */
         atmSig_notifyConnectEvent(svc);
         break;


      case ATMSIG_RCV_RELEASE:
         result = atmSig_processReleaseMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         atmSig_stopTimer(party);
         party->retryCount = 0;

         /* notify client that we have received a Release message */
         atmSig_notifyReleaseIndication(svc);
         break;


      case ATMSIG_RCV_RELEASE_COMPLETE:
         result = atmSig_processReleaseCompleteMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->state = ATMSIG_ST_NULL;
         svc->freeSvc = TRUE;
         break; 


      case ATMSIG_RCV_RESTART:
         result = atmSig_processRestartMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Transmit RESTART ACK.  Free SVC block */
         atmSig_txRestartAckMsg(svc);

         /* Notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE; 
         break;


      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

        atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
        break;


      case ATMSIG_REQ_RELEASE:
         atmSig_txReleaseMsg(svc, svc->cause_req);
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_RELEASE_REQ;
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
         party->retryCount = 0;
         break;


      case ATMSIG_REQ_CONNECT:
      case ATMSIG_REQ_SETUP:
      case ATMSIG_REQ_RELEASE_COMPLETE:
         if (atmSig_debugAll || atmSig_debugError) {
            buginf("\nATMSIG: %s Event in %s state ignored",
               atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_statsIgnored++;
         break;


	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugError) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


      default:
         atmSig_statsIgnored++;
         break;
   
   }

}
 


/*
 * a t m S i g _ o u t C a l l P r o c e e d i n g S t a t e ( )
 *
 * This state exists for an outgoing call when the user has received a
 * Call Proceeding message (the network has received all call information
 * to effect call establishment).
 *
 * A Connect message received would result in us transmitting a Connect
 * Ack message and changing state to ACTIVE (U10).
 */
void atmSig_outCallProceedingState (atmSvcType *svc)
{

   int result;
   atmSig_msgHeader *hdr;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: No party in Out Call Proc State");
     svc->freeSvc = TRUE;
     return;
   }

   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
      buginf("\nATMSIG: i %s msg in Out Call Proc State, length %d, call ref %d",
             atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
             GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );

   switch (svc->event) {

      case ATMSIG_RCV_CONNECT:
         result = atmSig_processConnectMsg(svc);

         /* Message received with a fatal error.  No action taken on msg. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* message accepted */
         atmSig_stopTimer(party);
         party->retryCount = 0;

         /*
          * Notify Client that we have received a Connect message in response to
          * our Setup message.
          */
         atmSig_notifyConnectEvent(svc);
         break;


      case ATMSIG_RCV_RELEASE:
         result = atmSig_processReleaseMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         atmSig_stopTimer(party);
         party->retryCount = 0;

         /* notify client that we have received a RELEASE message */
         atmSig_notifyReleaseIndication(svc);
         break; 


      /* disconnect the VC, release call reference, stop timers, free SVC  */
      case ATMSIG_RCV_RELEASE_COMPLETE:
         result = atmSig_processReleaseCompleteMsg(svc);
          
         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);
         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_RESTART:
         result = atmSig_processRestartMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Transmit RESTART ACK msg.  Free svc block */
         atmSig_txRestartAckMsg(svc);
         
         /* Notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE; 
         break;


      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

        atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
        break;


      case ATMSIG_REQ_RELEASE:
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_RELEASE_REQ;
         atmSig_txReleaseMsg(svc, svc->cause_req);
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
         party->retryCount = 0;
         break;

      case ATMSIG_REQ_CONNECT:
      case ATMSIG_REQ_SETUP:
      case ATMSIG_REQ_RELEASE_COMPLETE:
         if (atmSig_debugAll || atmSig_debugError) {
            buginf("\nATMSIG: %s Event in %s state ignored",
               atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugError) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


      /* unknown message */
      default:
         atmSig_txStatusMsg(svc, ATM_CAUSE_MSG_NOT_EXISTING);
         atmSig_statsIgnored++;
         break;
   }

}



/*
 * a t m S i g _ c a l l P r e s e n t S t a t e ( )
 *
 * This state exists for an incoming call when the user has already received
 * a Setup message from the network but has not yet responded.  Note that we
 * have only received the Setup message but still need to check it's validity.
 *
 * If the Setup message is valid the user will check with Call Admission Control
 * layer to see if the incoming call can be accepted.
 *
 * If the call is accepted it issues a Call Proceeding message to the source
 * node and tries to open up a svc connection to the destination node.
 */
void atmSig_callPresentState (atmSvcType *svc)
{

   int result, call_state, endpoint_state;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: No party in Call Present State");
     svc->freeSvc = TRUE;
     return;
   }

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) ) {
      buginf( "\nATMSIG: %s Event in %s State",
             atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
   }

   switch (svc->event) {

      case ATMSIG_RCV_SETUP:
         result = atmSig_processSetupMsg(svc);

         /* SETUP message received with a fatal error.  Free svc. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
            svc->freeSvc = TRUE;
            return;
         }

         /* notify client of incoming connection if ATM address registered */
         atmSig_notifySetupIndication(svc);
         break;

      /*
       * A received SETUP message causes us to enter this state, so we should be
       * sending a CALL PROCEEDING message not receiving it.  The user node is
       * badly broken (gross protocol violation).  Ignore for now.
       */
      case ATMSIG_RCV_CALL_PROCEEDING:
         break;


      case ATMSIG_RCV_RELEASE:
         result = atmSig_processReleaseMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that we received a Release message */
         atmSig_notifyReleaseIndication(svc);
         break;


      case ATMSIG_RCV_RELEASE_COMPLETE:
         result = atmSig_processReleaseCompleteMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_RESTART:
         result = atmSig_processRestartMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Transmit RESTART ACK.  Free SVC block */
         atmSig_txRestartAckMsg(svc);

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

        atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
        break;


      case ATMSIG_RCV_STATUS:
         result = atmSig_processStatusMsg(svc, &call_state, &endpoint_state);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         break;


      case ATMSIG_REQ_CONNECT:
         atmSig_txConnectMsg(svc);
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t313);
         party->retryCount = 0;
         svc->state = ATMSIG_ST_CONNECT_REQ;
         break;


      case ATMSIG_REQ_RELEASE:
         atmSig_txReleaseMsg(svc, svc->cause_req);
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_RELEASE_REQ;
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
         party->retryCount = 0;
         break;


      case ATMSIG_REQ_RELEASE_COMPLETE:
         atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_INCOMPAT_MSG);
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_NULL;
         svc->freeSvc = TRUE;
         break;


      case ATMSIG_REQ_SETUP:
         if (atmSig_debugAll || atmSig_debugError) {
            buginf( "\nATMSIG: %s Event in %s state ignored",
               atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_statsIgnored++;
         break;


	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugError) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


      default:
         atmSig_statsIgnored++;
         break;
   }

}



/*
 * a t m S i g _ c o n n e c t R e q S t a t e ( )
 *
 * This state exists for an incoming call when the user has answered the call
 * and is waiting to be awarded the call.  CONNECT sent would cause this state to
 * be entered.  A CONNECT_ACK message received would change state to ACTIVE (U10).
 */
void atmSig_connectReqState (atmSvcType *svc)
{

   int result;
   atmSig_msgHeader *hdr;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: No party in Conn Req State");
     svc->freeSvc = TRUE;
     return;
   }

   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
      buginf("\nATMSIG: i %s msg in connect Request State, length %d, call ref %d",
             atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
             GETLONG( &hdr->callref[0]) & ATMSIG_CALLREF_VALUE );

   switch (svc->event) {

      case ATMSIG_RCV_CONNECT_ACK:
         result = atmSig_processConnectAckMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if (result & ATMSIG_IE_FATAL_ERROR)
            return;

         atmSig_stopTimer(party);
         party->retryCount = 0;
         svc->state = ATMSIG_ST_ACTIVE;
	 if (svc->multiPoint)
	   party->partyState = ATMSIG_ST_ACTIVE;

         /* notify client that a Connect Ack message has been received */
         atmSig_notifyConnectAckEvent(svc);
         break;


      case ATMSIG_RCV_RELEASE:
         result = atmSig_processReleaseMsg(svc);

         /* message received with a fatal error.  No action taken on message. */
         if (result & ATMSIG_IE_FATAL_ERROR)
            return;

         atmSig_stopTimer(party);
         party->retryCount = 0;

         /* notify client that we have received a Release message */
         atmSig_notifyReleaseIndication(svc);
         break; 


      case ATMSIG_RCV_RELEASE_COMPLETE:
         result = atmSig_processReleaseCompleteMsg(svc);

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         /* return svc back to free pool */
         svc->freeSvc = TRUE; 
         break;


      case ATMSIG_RCV_RESTART:
         result = atmSig_processRestartMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Transmit RESTART ACK.  Free SVC block */
         atmSig_txRestartAckMsg(svc);

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

        atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
        break;     

      
      /* invalid messages in this state */
      case ATMSIG_RCV_CALL_PROCEEDING:
      case ATMSIG_RCV_CONNECT:
      case ATMSIG_RCV_SETUP:
         atmSig_txStatusMsg(svc, ATM_CAUSE_INCOMPAT_MSG);
         break;


      case ATMSIG_REQ_RELEASE:
         atmSig_txReleaseMsg(svc, svc->cause_req);
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_RELEASE_REQ;
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
         party->retryCount = 0;
         break;


      case ATMSIG_REQ_CONNECT:
      case ATMSIG_REQ_SETUP:
      case ATMSIG_REQ_RELEASE_COMPLETE:
         if (atmSig_debugAll || atmSig_debugError) {
            buginf( "\nATMSIG: %s Event in %s state ignored",
               atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_statsIgnored++;
         break;
 
 
	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugError) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


      default:
         atmSig_txStatusMsg(svc, ATM_CAUSE_MSG_NOT_EXISTING);
         atmSig_statsIgnored++;
         break;
   }

}



/*
 * a t m S i g _ i n C a l l P r o c e e d i n g S t a t e ( )
 *
 * This state exists for an incoming call when the user has already sent
 * a Call Proceeding message indicating that it has received all call info
 * to effect call establishment.  It's now awaiting a Connect
 * message.  A valid Connect message received would change the state of both the
 * source and destination svc to Active.  It will also notify the Call Admission
 * Control layer to directly establish a data path between the input and output
 * ports.
 */
void atmSig_inCallProceedingState (atmSvcType *svc)
{

   int result, call_state, endpoint_state;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: No party in In Call Proc State");
     svc->freeSvc = TRUE;
     return;
   }

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) ) {
      buginf( "\nATMSIG: %s Event in %s State",
              atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
   }

   switch (svc->event) {

       /*
        * Should not get a connect msg in u9 we should be sending the
        * connect msg. Send a Status message.
        */
      case ATMSIG_RCV_CONNECT:
         atmSig_txStatusMsg(svc, ATM_CAUSE_INCOMPAT_MSG);
         break;


      case ATMSIG_RCV_RELEASE:
         result = atmSig_processReleaseMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that we have received a Release message */
         atmSig_notifyReleaseIndication(svc);

         break;


      case ATMSIG_RCV_RELEASE_COMPLETE:
         result = atmSig_processReleaseCompleteMsg(svc);

         /* message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);
         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_RESTART:
         result = atmSig_processRestartMsg(svc);

         /* message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Transmit RESTART ACK message.  Free svc block */
         atmSig_txRestartAckMsg(svc);

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);
         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

        atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
        break;


     case ATMSIG_RCV_STATUS:
         result = atmSig_processStatusMsg(svc, &call_state, &endpoint_state);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         break;


      /* invalid messages received in this state */
      case ATMSIG_RCV_CALL_PROCEEDING:
      case ATMSIG_RCV_SETUP:
         atmSig_txStatusMsg(svc, ATM_CAUSE_INCOMPAT_MSG);
         break;

      case ATMSIG_REQ_CONNECT:
         atmSig_txConnectMsg(svc);
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t313);
         party->retryCount = 0;
         svc->state = ATMSIG_ST_CONNECT_REQ;
         break;


      case ATMSIG_REQ_RELEASE:
         atmSig_txReleaseMsg(svc, svc->cause_req);
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_RELEASE_REQ;
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
         party->retryCount = 0;
         break;


      /* client did not like the Setup message */
      case ATMSIG_REQ_RELEASE_COMPLETE:
         atmSig_txReleaseCompleteMsg(svc, svc->cause_req);
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_NULL;
         svc->freeSvc = TRUE;
         break;


      case ATMSIG_REQ_SETUP:
         if (atmSig_debugAll || atmSig_debugError) {
            buginf("\nATMSIG: %s Event in %s state ignored",
              atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_statsIgnored++;
         break;


	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugError) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


      default:
         atmSig_txStatusMsg(svc, ATM_CAUSE_MSG_NOT_EXISTING);
         atmSig_statsIgnored++;
         break;
   }

}



/*
 * a t m S i g _ a c t i v e S t a t e ( )
 *
 * This state exists for an incoming call when the user has received an acknowledgment
 * from the network that the user has been awarded the call.  This state exists for
 * an outgoing call when the user has received an indication that the remote user has
 * answered the call. 
 */
void atmSig_activeState (atmSvcType *svc)
{

   int result, call_state, endpoint_state;
   atmSig_msgHeader *hdr;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: No party in Active State");
     svc->freeSvc = TRUE;
     return;
   }

   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
      buginf("\nATMSIG: i %s msg in active State, length %d, call ref %d",
             atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
             GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );


   switch (svc->event) {

      case ATMSIG_RCV_RELEASE:
         result = atmSig_processReleaseMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that we have received a Release message */
         atmSig_notifyReleaseIndication(svc);

         break;

      case ATMSIG_RCV_RELEASE_COMPLETE:
         result = atmSig_processReleaseCompleteMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);
         svc->freeSvc = TRUE; 
         break;


      case ATMSIG_RCV_RESTART:
         result = atmSig_processRestartMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Transmit RESTART ACK.  Free SVC block */
         atmSig_txRestartAckMsg(svc);

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
         break;
         

      case ATMSIG_RCV_CALL_PROCEEDING:
      case ATMSIG_RCV_CONNECT:
      case ATMSIG_RCV_CONNECT_ACK:
      case ATMSIG_RCV_SETUP:
         atmSig_txStatusMsg(svc, ATM_CAUSE_INCOMPAT_MSG);
         break;


      case ATMSIG_RCV_STATUS:
	 if (mgd_timer_running(&svc->statusEnqTimer))
             mgd_timer_stop(&svc->statusEnqTimer);
	 
         if ( atmSig_processStatusMsg(svc, &call_state, &endpoint_state)
             & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
           break;
         else {
	   result = getCallState(svc,call_state);
	   if ( result != svc->state ) {
	     if (atmSig_debugEvent)
	       buginf("\ncall-ref %d, incompatible state %s in U10",
		      svc->callRef,atmSig_printState(result));
	       
	     atmSig_freeSvcBlock(svc);
	   }
	   else {
	     if (atmSig_debugEvent) 
	       buginf("\ncall-ref %d, compatible state in U10",
		      svc->callRef);
	   }
	 }

         break;


      /* Do we send a Restart message? */
      case ATMSIG_RCV_RESTART_ACK:
         break;

      /* if MTP, flush any existing parties before sending release */
      case ATMSIG_REQ_RELEASE:
	 if (svc->multiPoint)
	   atmSig_flushPartyQ(svc);
         atmSig_txReleaseMsg(svc, svc->cause_req);
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_RELEASE_REQ;
         atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t308);
         party->retryCount = 0;
         break;

      case ATMSIG_REQ_CONNECT:
      case ATMSIG_REQ_SETUP:
      case ATMSIG_REQ_RELEASE_COMPLETE:
         if (atmSig_debugAll || atmSig_debugError) {
            buginf( "\nATMSIG: %s Event in %s state ignored",
               atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_statsIgnored++;
         break;


	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugError) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


      /* unknown message */
      default:
         atmSig_txStatusMsg(svc, ATM_CAUSE_MSG_NOT_EXISTING);
         atmSig_statsIgnored++;
         break;
   }

}



/*
 * a t m S i g _ r e l e a s e R e q S t a t e ( )
 *
 * This state exists when the user has requested the network to clear the end-to-end
 * connection (if any) by sending a Release message and is waiting a Release
 * Complete response. 
 */
void atmSig_releaseReqState (atmSvcType *svc)
{

   int result;
   atmSig_msgHeader *hdr;


   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
      buginf("\nATMSIG: i %s msg in release Req State, length %d, call ref %d",
             atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
             GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );


   switch (svc->event) {

      case ATMSIG_RCV_RELEASE_COMPLETE:
         result = atmSig_processReleaseCompleteMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);
         svc->freeSvc = TRUE; 
         break;


      case ATMSIG_RCV_RESTART:
         result = atmSig_processRestartMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Transmit RESTART ACK.  Free SVC block */
         atmSig_txRestartAckMsg(svc);

         /* Notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE; 
         break;


      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

        atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
        break;

      
      /* messages invalid in this state */
      case ATMSIG_RCV_CALL_PROCEEDING:
      case ATMSIG_RCV_CONNECT:
      case ATMSIG_RCV_CONNECT_ACK:
      case ATMSIG_RCV_SETUP:
         atmSig_txStatusMsg(svc, ATM_CAUSE_INCOMPAT_MSG);
         break;


      case ATMSIG_RCV_STATUS:
         break;


     case ATMSIG_RCV_RELEASE:
         /*
          * 5.5.4.5 Clear Collision
          * If the user receives a RELEASE message while in the Release
          * Request state, the network shall: stop timer T308; release
          * release the call reference and virtual channel; and enter the
          * Null state (without sending or receiving a RELEASE COMPLETE
          * message).
          */
         result = atmSig_processReleaseMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE;
         break;


      case ATMSIG_REQ_CONNECT:
      case ATMSIG_REQ_SETUP:
      case ATMSIG_REQ_RELEASE:
      case ATMSIG_REQ_RELEASE_COMPLETE:
         if (atmSig_debugAll || atmSig_debugEvent) {
            buginf( "\nATMSIG: %s Event in %s state ignored",
             atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_statsIgnored++;
         break;


	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugEvent) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


      /* unknown message */
      default:
         atmSig_txStatusMsg(svc, ATM_CAUSE_MSG_NOT_EXISTING);
         atmSig_statsIgnored++;
         break;
   } 

}



/*
 * a t m S i g _ r e l e a s e I n d S t a t e ( )
 *
 * This state exists when the network has disconnected the end-to-end connection
 * (if any) and has sent an invitation to diconnect the user-network connection.
 * Signalling is waiting for the signalling user above it to send a Release 
 * Complete message.
 */
void atmSig_releaseIndState (atmSvcType *svc)
{

   int result, call_state, endpoint_state;
   atmSvcPartyType *party;


   party = svc->rootParty;

   if (!party){
     if (atmSig_debugAll || atmSig_debugError)
       buginf("\nATMSIGERR: No party in Release Ind State");
     svc->freeSvc = TRUE;
     return;
   }

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) ) {
      buginf( "\nATMSIG: %s Event in %s State",
        atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
   }

   switch (svc->event) {

      case ATMSIG_RCV_RELEASE_COMPLETE:
         result = atmSig_processReleaseCompleteMsg(svc);

         /* message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         /* return svc back to the free pool */
         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_RESTART:
         result = atmSig_processRestartMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         /* Message accepted.  Transmit RESTART ACK.  Free SVC block */
         atmSig_txRestartAckMsg(svc);

         /* notify client that this connection went away */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE;
         break;


      case ATMSIG_RCV_STATUS_ENQUIRY:
         result = atmSig_processStatusEnquiryMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

        atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
        break;


     case ATMSIG_RCV_STATUS:
         result = atmSig_processStatusMsg(svc, &call_state, &endpoint_state);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         break;


     case ATMSIG_RCV_RELEASE:
         /*
          * We are in Release Indication State becuase we have already
          * received a RELEASE from the Network. Getting another RELEASE
          * could only mean that the signalling user above us failed to
          * transmit a RELEASE COMPLETE. Therefore we do so here, and
          * notify the signalling user via the api call.
          */
         result = atmSig_processReleaseMsg(svc);

         /* Message received with a fatal error.  No action taken on message. */
         if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) )
            return;

         atmSig_txReleaseCompleteMsg(svc, ATM_CAUSE_NORMAL_UNSPECIFIED);

         /* to the client, we pretend as though we rcvd a Release Complete msg */
         atmSig_notifyReleaseCompleteIndication(svc);

         svc->freeSvc = TRUE;

         break;


      /* messages invalid in this state */
      case ATMSIG_RCV_CALL_PROCEEDING:
      case ATMSIG_RCV_CONNECT:
      case ATMSIG_RCV_CONNECT_ACK:
      case ATMSIG_RCV_SETUP:
         atmSig_txStatusMsg(svc, ATM_CAUSE_INCOMPAT_MSG);
         break;


      case ATMSIG_REQ_RELEASE_COMPLETE:
         atmSig_txReleaseCompleteMsg(svc, svc->cause_req);
         atmSig_stopTimer(party);
         svc->state = ATMSIG_ST_NULL;
         svc->freeSvc = TRUE;
         break;


      case ATMSIG_REQ_CONNECT:
      case ATMSIG_REQ_SETUP:
         if (atmSig_debugAll || atmSig_debugError) {
            buginf("\nATMSIG: %s Event in %s state ignored",
              atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_statsIgnored++;
         break;


      /*
       * This is a case of Release Collision.  We got a Release msg from the
       * switch and notified the client.  We also got a Release msg from the
       * client - internally send a Release Complete msg to the client.  But
       * wait for a Release Complete request from the client.  Upon getting
       * the Release Complete req or timer expiry we will send a Release
       * Complete msg to the switch.
       */
      case ATMSIG_REQ_RELEASE:
         if (atmSig_debugAll || atmSig_debugEvent) {
            buginf("\nATMSIG: %s Event in %s state, Release Collision",
              atmSig_printEventType(svc->event), atmSig_printState(svc->state) );
         }
         atmSig_notifyReleaseCompleteIndication(svc);
         /* 
          * Do not free svc here - await Release Complete req from client or
          * timer expiry.
          */  
         break;


	 /* 
	  * Drop all multipoint Requests and Messages.
	  * Shouldn't get these in stateTable.
	  */
       case ATMSIG_REQ_ADD_PARTY:
       case ATMSIG_REQ_DROP_PARTY:
       case ATMSIG_REQ_ADD_PARTY_ACK:
       case ATMSIG_REQ_ADD_PARTY_REJ:
       case ATMSIG_REQ_DROP_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY:
       case ATMSIG_RCV_ADD_PARTY_ACK:
       case ATMSIG_RCV_ADD_PARTY_REJ:
       case ATMSIG_RCV_DROP_PARTY:
       case ATMSIG_RCV_DROP_PARTY_ACK:
         if (atmSig_debugAll || atmSig_debugError) {
	   buginf("\nATMSIG: %s Event in %s state ignored",
	     atmSig_printEventType(svc->event), atmSig_printState(svc->state));
         }
         atmSig_statsIgnored++;
         break;


      /* unknown message */
      default:
         atmSig_txStatusMsg(svc, ATM_CAUSE_MSG_NOT_EXISTING);
         atmSig_statsIgnored++;
         break;
   }

}



/*
 * a t m S i g _ r e s t a r t R e q S t a t e ( )
 *
 * This state exists when we receive _global_ Restart messages. 
 * The Restart messages have global scope and always uses the global svc
 * block for processing.  Local Restart messages are processed by their 
 * respective svc blocks.
 */
void atmSig_restartReqState (atmSvcType *svc)
{

   atmSig_msgHeader *hdr;
   boolean result = ATMSIG_MSG_OK;


   if (!svc->inbuf)
     return;

   hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
      buginf("\nATMSIG: i %s msg in restart Request State, length %d, call ref %d",
             atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
             GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE ); 

   /* 
    * Global Restart message received - reset all the svc's associated with
    * the particular port and reply with a Restart Ack message.
    */ 
   if (hdr->msgtype == ATMSIG_MT_RESTART) {

      result = atmSig_processRestartMsg(svc);

      /* No action taken on messages with fatal error */
      if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) 
         return;

      /* reply with Restart Ack and change state on successful transmission */
      if ( atmSig_txRestartAckMsg(svc) ) {
         /* put the global svc block back in idle state */
         svc->state = ATMSIG_ST_NULL;
      }
      else {
         if (atmSig_debugAll || atmSig_debugAll)
            buginf("\nATMSIG: no memory to send global Restart Ack message");
      }

      /* reset svc's tied to a particular port */
      atmSig_interfaceReset(svc->sig_idb->hwptr);

   }

   /* 
    * We received a global Restart Ack message, it's probably due to our sending
    * a global Restart message - nothing more to do except maybe log it, the system
    * input routine will free the buffer.
    */ 
   if (hdr->msgtype == ATMSIG_MT_RESTART_ACK) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: Restart Ack msg rcvd"); 
   }

   /*
    * We received a global Status message, it's probably due to our sending
    * a Status Enq message - nothing more to do except maybe log it, the
    * system input routine will free the buffer.
    */
   if (hdr->msgtype == ATMSIG_MT_STATUS) {
      if (atmSig_debugAll || atmSig_debugEvent)
         buginf("\nATMSIG: global Status msg rcvd"); 
   }

}



/*
 * a t m S i g _ m u l t i P a r t y S t a t e T a b l e ( )
 *
 * Multipoint State Table
 * Multipoint messages are handled in this table.
 * Should not enter this table unless link already active and call
 * previously setup.
 */
void atmSig_multiPointStateTable (atmSvcType *svc, atmSvcPartyType *party)
{

   atmSig_statetype prevstate;


   prevstate = party->partyState;

   switch (party->partyState) {

      /* Null State */
      case ATMSIG_PST_NULL:
         atmSig_multiNullState(svc, party);
         break;

      /* Add Party Initiated State */
      case ATMSIG_PST_ADD_PARTY_INITIATED:
         atmSig_addPartyInitiatedState(svc, party);        
         break;

      /* Add Party Received State */
      case ATMSIG_PST_ADD_PARTY_RECEIVED:
         atmSig_addPartyReceivedState(svc, party);         
         break;

      /* Drop Party Initiated State */
      case ATMSIG_PST_DROP_PARTY_INITIATED:
         atmSig_dropPartyInitiatedState(svc, party);        
         break;

      /* Drop Party Received State */
      case ATMSIG_PST_DROP_PARTY_RECEIVED:
         atmSig_dropPartyReceivedState(svc, party);        
         break;

      /* Active State */
      case ATMSIG_PST_ACTIVE:
         atmSig_multiPointActiveState(svc, party);         
         break;

      /* Invalid State */
      default:
         break;

   }

   /* multiState changed? */
   if ( (atmSig_debugAll || atmSig_debugEvent) && party && 
                                             (prevstate != party->partyState) )
     buginf("\nATMSIG: party state changed from %s to %s", 
	    atmSig_printState(prevstate), atmSig_printState(party->partyState) );

}
 

/*
 * a t m S i g _ m u l t i N u l l S t a t e ( ) 
 *
 * An ADD PARTY request causes this state to be entered.  
 * This will only occur if we have already sent a SETUP for the first
 * party. 
 *
 */
void atmSig_multiNullState (atmSvcType *svc, atmSvcPartyType *party)
{

   int result;
   atmSig_msgHeader *hdr;


   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
     buginf("\nATMSIG: i %s msg in Multi Null State, length %d, call ref %d",
	    atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]), 
	    GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );        


   switch (svc->event) {

   case ATMSIG_RCV_ADD_PARTY:
     result = atmSig_processAddPartyMsg(svc, party); 

     /* ADD_PARTY message received with a fatal error.  Free Party. */ 
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       party->freeParty = TRUE;
       atmSig_statsIgnored++;
       return;
     }
    
     /* 
      * ADD_PARTY accepted, notify client
      */
     party->partyState = ATMSIG_PST_ADD_PARTY_RECEIVED;
     atmSig_notifyAddPartyEvent(svc, party);

     break;
     
     /* 
      * Process Multi Requests from api
      */
   case ATMSIG_REQ_ADD_PARTY:
     atmSig_txAddPartyMsg(svc, party);
     atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t399);
     party->retryCount = 0;
     party->partyState = ATMSIG_PST_ADD_PARTY_INITIATED;

     break;


     /* 
      * Invalid Multi Messages and Requests
      * Send buginf
      */
   case ATMSIG_RCV_ADD_PARTY_ACK:
   case ATMSIG_RCV_ADD_PARTY_REJ:
   case ATMSIG_RCV_DROP_PARTY:
   case ATMSIG_RCV_DROP_PARTY_ACK:
   case ATMSIG_REQ_ADD_PARTY_ACK:
   case ATMSIG_REQ_ADD_PARTY_REJ:
   case ATMSIG_REQ_DROP_PARTY:
   case ATMSIG_REQ_DROP_PARTY_ACK:
   default:
     atmSig_statsIgnored++;
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: %s Event in %s Party state ignored",
	      atmSig_printEventType(svc->event), 
	      atmSig_printState(party->partyState) );
     break;
    

   }
  
}



/*
 * a t m S i g _ a d d P a r t y I n i t i a t e d S t a t e ( )
 *
 * This state exists when the user initiates a connection by sending 
 * an ADD_PARTY message. This will only be sent if a SETUP has already
 * been sent and there is one party active.
 */
void atmSig_addPartyInitiatedState (atmSvcType *svc, atmSvcPartyType *party)
{

   int result;
   atmSig_msgHeader *hdr;


   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;
 
   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
     buginf("\nATMSIG: i %s msg in Add Party Initiated State, length %d, call ref %d",
	   atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
	   GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );

   switch (svc->event) {

   case ATMSIG_RCV_ADD_PARTY_ACK:
     result = atmSig_processAddPartyAckMsg(svc, party);
    
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     /* 
      * Message accepted.  Notify Client.
      */
     atmSig_stopTimer(party);
     party->retryCount = 0;
     atmSig_notifyAddPartyAckEvent(svc, party);
     break;
    
   case ATMSIG_RCV_ADD_PARTY_REJ:
     result = atmSig_processAddPartyRejMsg(svc, party);
    
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }
     /* 
      * Message accepted.  Notify Client.
      */
     atmSig_stopTimer(party);
     party->retryCount = 0;

     atmSig_notifyAddPartyRejEvent(svc, party);
     party->partyState = ATMSIG_PST_NULL;
     party->freeParty = TRUE;
     break;

   case ATMSIG_RCV_DROP_PARTY:
     result = atmSig_processDropPartyMsg(svc, party);
     
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_stopTimer(party);
     party->retryCount = 0;
     party->partyState = ATMSIG_PST_DROP_PARTY_RECEIVED;

     atmSig_notifyDropPartyEvent(svc, party);
     break;

   case ATMSIG_RCV_STATUS_ENQUIRY:
     result = atmSig_processStatusEnquiryMsg(svc);
    
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
     break;


     /* 
      * Process Multi Requests from api
      */
   case ATMSIG_REQ_DROP_PARTY:
     atmSig_stopTimer(party);
     atmSig_txDropPartyMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
     party->partyState = ATMSIG_PST_DROP_PARTY_INITIATED;
     atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t398);	 
     party->retryCount = 0;
     break;


     /* 
      * Invalid Multi Messages and Requests
      * Send buginf
      */
   case ATMSIG_RCV_DROP_PARTY_ACK:
   case ATMSIG_REQ_ADD_PARTY_ACK:
   case ATMSIG_REQ_ADD_PARTY_REJ:
   case ATMSIG_REQ_DROP_PARTY_ACK:
   default:
     atmSig_statsIgnored++;
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: %s Event in %s Party state ignored",
	   atmSig_printEventType(svc->event), atmSig_printState(party->partyState) );
     break;
    

   }
   
}
 


/*
 * a t m S i g _ a d d P a r t y R e c e i v e d S t a t e ( )
 *
 * This state results from accepting an AddParty message.
 *
 */
void atmSig_addPartyReceivedState (atmSvcType *svc, atmSvcPartyType *party)
{

   int result;
   atmSig_msgHeader *hdr;


   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
     buginf("\nATMSIG: i %s msg in Add Party Received State, length %d, call ref %d",
	    atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
	    GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );

   switch (svc->event) {

     /* sending party now wants to drop us before we acked */
   case ATMSIG_RCV_DROP_PARTY:
     result = atmSig_processDropPartyMsg(svc, party);
     
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     party->partyState = ATMSIG_PST_DROP_PARTY_RECEIVED;

     atmSig_notifyDropPartyEvent(svc, party);
     
     break;


   case ATMSIG_RCV_STATUS_ENQUIRY:
     result = atmSig_processStatusEnquiryMsg(svc);

     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
     break;     


     /* 
      * Process Multi Requests from api
      */
   case ATMSIG_REQ_ADD_PARTY_ACK:
     atmSig_txAddPartyAckMsg(svc, party);
     party->partyState = ATMSIG_PST_ACTIVE;
     break;


   case ATMSIG_REQ_ADD_PARTY_REJ:
     atmSig_txAddPartyRejMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
     party->partyState = ATMSIG_PST_NULL;
     party->freeParty = TRUE;
     break;


     /* 
      * Invalid Multi Messages and Requests
      * Send buginf
      */
   case ATMSIG_RCV_ADD_PARTY_ACK:
   case ATMSIG_RCV_ADD_PARTY_REJ:
   case ATMSIG_RCV_DROP_PARTY_ACK:
   case ATMSIG_REQ_DROP_PARTY:
   case ATMSIG_REQ_DROP_PARTY_ACK:
   default:
     atmSig_statsIgnored++;
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: %s Event in %s Party state ignored",
	   atmSig_printEventType(svc->event), atmSig_printState(party->partyState) );
     break;
    

   }

}



/*
 * a t m S i g _ m u l t i P a r t y A c t i v e S t a t e ( )
 *
 * This state exists for a call when the user has received an 
 * indication that the remote user has answered the call. 
 */
void atmSig_multiPointActiveState (atmSvcType *svc, atmSvcPartyType *party)
{

   int result;
   atmSig_msgHeader *hdr;


   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;
   
   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
     buginf("\nATMSIG: i %s msg in party active State, length %d, call ref %d",
	    atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
	    GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );

   switch (svc->event) {
     
   case ATMSIG_RCV_DROP_PARTY:
     result = atmSig_processDropPartyMsg(svc, party);
     
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     party->partyState = ATMSIG_PST_DROP_PARTY_RECEIVED;

     atmSig_notifyDropPartyEvent(svc, party);
     break;


   case ATMSIG_RCV_STATUS_ENQUIRY:
     result = atmSig_processStatusEnquiryMsg(svc);

     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
     break;


     /* 
      * Process Multi Requests from api
      */
   case ATMSIG_REQ_DROP_PARTY:
     atmSig_txDropPartyMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
     atmSig_startTimer(party, svc->sig_idb->hwptr->atm_db->timer_t398);	 
     party->retryCount = 0;
     party->partyState = ATMSIG_PST_DROP_PARTY_INITIATED;
     break;



     /* 
      * Invalid Multi Messages and Requests
      * Send buginf
      */
   case ATMSIG_RCV_ADD_PARTY_ACK:
   case ATMSIG_RCV_ADD_PARTY_REJ:
   case ATMSIG_RCV_DROP_PARTY_ACK:
   case ATMSIG_REQ_ADD_PARTY_ACK:
   case ATMSIG_REQ_ADD_PARTY_REJ:
   case ATMSIG_REQ_DROP_PARTY_ACK:
   default:
     atmSig_statsIgnored++;
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: %s Event in %s Party state ignored",
	   atmSig_printEventType(svc->event), atmSig_printState(party->partyState) );
     break;
    

   }

}



/*
 * a t m S i g _ d r o p P a r t y I n i t i a t e d S t a t e ( )
 *
 * This state exists when the user has requested the network to clear the 
 * end-to-end connection (if any) by sending a DropParty message and is waiting
 * for a DropPartyAck response. 
 */
void atmSig_dropPartyInitiatedState (atmSvcType *svc, atmSvcPartyType *party)
{

   int result;
   atmSig_msgHeader *hdr;


   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
     buginf("\nATMSIG: i %s msg in Drop Party Initiated State, len %d, call ref %d",
	    atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
	    GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );

   switch (svc->event) {

   case ATMSIG_RCV_DROP_PARTY_ACK:
     result = atmSig_processDropPartyAckMsg(svc, party);
     
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_stopTimer(party);
     party->retryCount = 0;

     atmSig_notifyDropPartyAckEvent(svc, party);     
     party->partyState = ATMSIG_PST_NULL;
     party->freeParty = TRUE; 
     break;
     
     
     /* Receive DROP_PARTY in this state is a drop collision
      * Notify client. Send DROP_PARTY_ACK and clear call. 
      */
   case ATMSIG_RCV_DROP_PARTY:
     result = atmSig_processDropPartyMsg(svc, party);
     
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_stopTimer(party);
     party->retryCount = 0;

     atmSig_notifyDropPartyAckEvent(svc, party);     
     atmSig_txDropPartyAckMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
     party->partyState = ATMSIG_PST_NULL;
     party->freeParty = TRUE; 
     break;
     
     
   case ATMSIG_RCV_STATUS_ENQUIRY:
     result = atmSig_processStatusEnquiryMsg(svc);
     
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
     break;


     /* 
      * Invalid Multi Messages and Requests
      * Send buginf
      */
   case ATMSIG_RCV_ADD_PARTY_ACK:
   case ATMSIG_RCV_ADD_PARTY_REJ:
   case ATMSIG_REQ_ADD_PARTY_ACK:
   case ATMSIG_REQ_ADD_PARTY_REJ:
   case ATMSIG_REQ_DROP_PARTY:
   case ATMSIG_REQ_DROP_PARTY_ACK:
   default:
     atmSig_statsIgnored++;
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: %s Event in %s Party state ignored",
	   atmSig_printEventType(svc->event), atmSig_printState(party->partyState) );
     break;
    

   }
   
}



/*
 * a t m S i g _ d r o p P a r t y R e c e i v e d S t a t e ( )
 *
 * This state exists when the network has requested the user to clear the 
 * end-to-end connection. The user has received a DropParty message and should
 * send a DropPartyAck response. 
 */
void atmSig_dropPartyReceivedState (atmSvcType *svc, atmSvcPartyType *party)
{

   int result;
   atmSig_msgHeader *hdr;


   if (svc->inbuf)
     hdr = (atmSig_msgHeader *)svc->inbuf->datagramstart;

   if ( svc->inbuf && (atmSig_debugAll || atmSig_debugEvent) )
     buginf("\nATMSIG: i %s msg in Drop Party Initiated State, len %d, call ref %d",
	    atmSig_printEventType(svc->event), GETSHORT(&hdr->length[0]),
	    GETLONG(&hdr->callref[0]) & ATMSIG_CALLREF_VALUE );

   switch (svc->event) {

   case ATMSIG_RCV_DROP_PARTY_ACK:
     result = atmSig_processDropPartyAckMsg(svc, party);
     
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_notifyDropPartyAckEvent(svc, party);     
     party->partyState = ATMSIG_PST_NULL;
     party->freeParty = TRUE; 
     break;


   case ATMSIG_RCV_STATUS_ENQUIRY:
     result = atmSig_processStatusEnquiryMsg(svc);
     
     /* Message received with a fatal error.  No action taken on message. */
     if ( result & (ATMSIG_MANDATORY_IE_MISSING | ATMSIG_IE_FATAL_ERROR) ) {
       atmSig_statsIgnored++;
       return;
     }

     atmSig_txStatusMsg(svc, ATM_CAUSE_STATUSENQ_RSP);
     break;
     
     
     /* 
      * Process Multi Requests from api
      */
   case ATMSIG_REQ_DROP_PARTY_ACK:
     atmSig_stopTimer(party);
     atmSig_txDropPartyAckMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
     party->partyState = ATMSIG_PST_NULL;
     party->freeParty = TRUE;
     break;


     /* Drop Party collision 
      * Notify client. 
      */
   case ATMSIG_REQ_DROP_PARTY:
     atmSig_stopTimer(party);
     atmSig_notifyDropPartyAckEvent(svc, party);     
     atmSig_txDropPartyAckMsg(svc, party, ATM_CAUSE_NORMAL_UNSPECIFIED);
     party->partyState = ATMSIG_PST_NULL;
     party->freeParty = TRUE;
     break;


     /* 
      * Invalid Multi Messages and Requests
      * Send buginf
      */
   case ATMSIG_RCV_ADD_PARTY_ACK:
   case ATMSIG_RCV_ADD_PARTY_REJ:
   case ATMSIG_RCV_DROP_PARTY:
   case ATMSIG_REQ_ADD_PARTY_ACK:
   case ATMSIG_REQ_ADD_PARTY_REJ:
   default:
     atmSig_statsIgnored++;
     if (atmSig_debugAll || atmSig_debugEvent)
       buginf("\nATMSIG: %s Event in %s Party state ignored",
	   atmSig_printEventType(svc->event), atmSig_printState(party->partyState) );
     break;
    

   }

}

static int getCallState (atmSvcType *svc, int state)
{
  int st = 0;

  switch (state) {
  case 0:
    st = ATMSIG_ST_NULL;
    break;
  case 1: 
    st = ATMSIG_ST_CALL_INITIATED;
    break;
  case 3:
    st = ATMSIG_ST_OUTCALL_PROCEEDING;
      break;
  case 6:
    st = ATMSIG_ST_CALL_PRESENT;
    break;
  case 8:
    st = ATMSIG_ST_CONNECT_REQ;
    break;
  case 9:
    st = ATMSIG_ST_INCALL_PROCEEDING;
    break;
  case 10:
    st = ATMSIG_ST_ACTIVE;
    break;
  case 11:
    st = ATMSIG_ST_RELEASE_REQ;
    break;
  case 12:
    st = ATMSIG_ST_RELEASE_IND;
    break;
  default:
    if (atmSig_debugError)
	buginf("\nunknown UNI state %d", state);
  }  

  return (st);
}





