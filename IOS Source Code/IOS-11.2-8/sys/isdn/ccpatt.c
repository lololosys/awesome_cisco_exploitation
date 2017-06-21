/* $Id: ccpatt.c,v 3.2 1995/11/17 17:39:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/ccpatt.c,v $
 *------------------------------------------------------------------
 * ccpatt.c  ISDN Primary Rate Interface AT&T Compatible Call State Module
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccpatt.c,v $
 * Revision 3.2  1995/11/17  17:39:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:14:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       ccpatt.c                                                
DESCRIPTION:  ISDN Primary Rate Interface AT&T Compatible Call State  
              Module                                                  
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************

GENERAL NOTES

This module implements the Call Control states necessary to provide
AT&T 4/5ESS PRI User-side operation.

This module represents one of several Central Office switch-specific
PRI state solutions which reside under CCPRI.C.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/
/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

/* Standard C library functions/definitions
*/
/* """
#include    <stdio.h>
""" */

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "cc_chan.h"
#include    "ccpri.h"
#include    "ccie.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/************************************************************************
*                            M A C R O S                                *
*************************************************************************
*/
/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S               *
*************************************************************************
*/
PRIVATE void CCPATT_CallConnected     ARGS(( int ));
PRIVATE void CCPATT_CallIdle          ARGS(( int ));
PRIVATE void CCPATT_CallOffered       ARGS(( int ));
PRIVATE void CCPATT_CallOrigination   ARGS(( int ));
PRIVATE void CCPATT_CallRinging       ARGS(( int ));
PRIVATE void CCPATT_CallReleasing     ARGS(( int ));
PRIVATE void CCPATT_CallRoutingIn     ARGS(( int ));
PRIVATE void CCPATT_CallRoutingOut    ARGS(( int ));

#ifdef TRACING
/* NOTE: If a state is added or deleted, CCPATT_NumericState() should be 
*        updated.
*/

#endif
/************************************************************************
*                    D A T A   A L L O C A T I O N                      *
*************************************************************************
*/
PRIVATE BYTE IntID[ 2 ] = {0, 0};       /* For interface ID management */

/************************************************************************
*                C A L L   S T A T E   F U N C T I O N S                *
*************************************************************************
*/

/***********************
* CCPATT_CallConnected *
************************
*
* Process the current event in the context of a call in the CONNECTED
* state.
*
* This state corresponds to L3 Protocol Control State 10.
*
*/
PRIVATE void CCPATT_CallConnected( event )

int event;                                 /* Event input to the state */

{
#ifdef DEBUG
   printf( "CCPATT executing with event = %x in state = CALL CONNECTED\n", event );
#endif

   switch ( event )
      {
      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CALL_UUI:
         CCPMSG_SendPeerUUI();    /* HOST UUI transmission to the peer */
         break;

      case CC_USER_INFO_IND:
         CCPMSG_SendHostUUI();    /* Peer UUI transmission to the HOST */
         break;

      case CALL_CONGESTION:
         CCPMSG_SendPeerCongest();       /* Congestion in UUI transmit */
         break;

      case CC_CONG_CON_IND:
         CCPMSG_SendHostCongest();       /* Congestion in UUI transmit */
         break;

      case CALL_FACILITY:
         /* Host requests a Peer Facility
         */
         CCPMSG_FacReqOut();
         break;

      case CC_FACILITY_IND:
         /* Peer requests a Host Facility
         */
         CCPMSG_FacReqIn();
         break;

      case CALL_FACILITY_ACK:
         /* Host acknowledgess a Peer Facility request
         */
         CCPMSG_FacAckOut();
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CALL_FACILITY_REJ:
         /* Host rejects a Peer Facility request
         */
         CCPMSG_FacRejOut();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CC_STATUS_IND:
         if ( SourceState == 0 )    /* The call no longer exists at L3 */
            {
            CCPMSG_SendHostDisc( 0, FALSE );
            CCPRI_ReleaseCall();
            }
         else
            {
            CCPMSG_SendHostProgress( STATUS_REPORT, TRUE, FALSE );
            }
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_SendHostDisc( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      default:
         CCPRI_Error( CCPATT_CALL_CONNECTED, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/******************
* CCPATT_CallIdle *
*******************
*
* Process the current event in the context of a call in the IDLE state.
*
* This state corresponds to L3 Protocol Control State 0.
*
*/
PRIVATE void CCPATT_CallIdle( event )

int event;                                 /* Event input to the state */
{

#ifdef DEBUG
   printf( "CCPATT executing with event = %x in state = CALL IDLE\n", event );
#endif

   switch ( event )
      {
      case CALL_OUTGOING:
         /* Outgoing call request by the host.  This complex function
         *  is implemented independently.
         */
         if ( CCPMSG_OutCall() )
            CCB->State = CCPATT_CallOrigination;
         else 
            CCPRI_ReleaseCall();
         break;

      case CC_SETUP_IND:
         if ( CCPMSG_InCall() )
            CCB->State = CCPATT_CallOffered;
         else 
            CCPRI_ReleaseCall();
         break;

      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
         CCPRI_ReleaseCall();
         break;

      default:
         CCPRI_Error( CCPATT_CALL_IDLE, UNKNOWN_EVENT, event, FALSE );
         CCPRI_ReleaseCall();
         break;
      }
}


/*********************
* CCPATT_CallOffered *
**********************
*
* Process the current event in the context of a call in the OFFERED
* state.  This state is invoked when an INCOMING_CALL has been offered
* to the HOST and a response has not yet been received.
*
* This state corresponds to L3 Protocol Control State 6.
*
*/
PRIVATE void CCPATT_CallOffered( event )

int event;                                 /* Event input to the state */
{
   int chan;                             /* Channel from CHANNEL_ID_IE */
   BYTE int_id;                                        /* Interface ID */

#ifdef DEBUG
   printf( "CCPATT executing with event = %x in state = CALL OFFERED\n", event );
#endif

   switch ( event )
      {
      case CALL_ACCEPT:
         /* Build mandatory CHANNEL ID element required in the first
         *  response to a SETUP by recovering information sent by the host.
         *  This response is equivalent to CALL_PROCEEDING.
         */
         if (( IE = CCPRI_GetPrivate( CHANNEL_ID_IE, CODESET_0 )) != NULL )
            {
            IE_Fields = NULL;  /* This indicates private version found */
            CCB->Exclusive = IE[ 2 ];
            chan      = IE[ 4 ];
            int_id    = ( MultiInterface ) ? IE[ 5 ] : 0;
            }
#ifdef EXTENDED_HOST_MSG
         else if (( IE = CCPRI_GetPublic( CHANNEL_ID_IE, CODESET_0 )) != NULL )
            {
            IE_Fields = CCIE_ExpandChanId_IE( IE, &Trashcan );
            CCB->Exclusive = IE_Fields[ 2 ];
            chan      = IE_Fields[ 9 ];
            int_id    = ( IE_Fields[ 0 ] && MultiInterface ) ? 
                           IE_Fields[ IE_Fields[ 8 ] ] : 0;
            }
#endif
         else
            {
            CCPMSG_SendHostDisc( MANDATORY_IE_MISSING, FALSE );
            CCPMSG_RejectInCall( TEMPORARY_FAILURE, 0 );
            CCPRI_ReleaseCall();
            goto in_call_failed;
            }

         /* Check for different channel numb or same channel in a different
         *  interface.
         */
         if (( CCB->B_Chan != chan ) || ( CCB->Int_Id[ 0 ] != int_id ))
            {
#ifdef CHANNEL_TABLE
            CCPRI_ReleaseChan();                   /* Free the old one */
            if (( CC_CHAN_GetState( int_id, chan, 
                                    CCB->ChanType, Dsl_Id ) == CHANNEL_IDLE ) &&
                ( CC_CHAN_GetServState( int_id, chan, 
                                        CCB->ChanType, Dsl_Id ) != OUT_OF_SERVICE ))
               {
               CCB->B_Chan      = chan;                   /* Save chan */
               CCB->Int_Id[ 0 ] = int_id;              /* Interface ID */
               CC_CHAN_SetState( int_id, chan, CCB->ChanType, 
                                 CHANNEL_BUSY, Dsl_Id );
               CC_CHAN_SetProtocol( int_id, chan, CCB->ChanType, 
                                    L2P_LAPD, Dsl_Id );
               CC_CHAN_SetUser( int_id, chan, CCB->ChanType, 
                                CCB->Call_ID, Dsl_Id );
               }
            else
               /* We are at an impass; the call cannot proceed. Send Cause 34.
               */
               {
               CCPMSG_SendHostDisc( NO_CIRCUIT_AVAILABLE, FALSE );
               CCPMSG_RejectInCall( NO_CIRCUIT_AVAILABLE, 0 );
               CCPRI_ReleaseCall();
               goto in_call_failed;
               }
#else                                             /* Not CHANNEL_TABLE */
            CCB->B_Chan = chan;                           /* Save chan */
            CCB->Int_Id[ 0 ] = int_id;                 /* Interface ID */
#endif                                                /* CHANNEL_TABLE */
            }
         if ( CCPRI_GetOutPkt( CC_BROADCAST_RESP ))
            {
            CCPRI_Mail( L3_ID, FALSE );      /* Send CC_BROADCAST_RESP */
            }
         if (( IE = CCPRI_GetPrivate( PROGRESS_IND_IE, CODESET_0 )) != NULL )
            {
            /* The call is already ALERTING or CONNECTED
            */
            CCPATT_CallRoutingIn( CALL_ACCEPT );
            }
         else if ( CCPRI_GetOutPkt( CC_CALL_PROC_REQ ))
            {
#ifdef EXTENDED_HOST_MSG
            if ( Host_IEs_Len == 0 )
               {
               CCPRI_CopyPublicAll();
               }
            else
#endif
               {
               if ( IE_Fields == NULL )  /* Already located private IE */
                  {
                  IntID[ 0 ] = CCPRI_MapIntId( CCB->Int_Id[ 0 ], TRUE );
                  OutPkt->InfoLen += CCIE_BuildChanId_IE(
                                        &OutPkt->Info[ OutPkt->InfoLen ],
                                        CHANNELS_LISTED,
                                        CCB->B_Chan,  /* Lowest B_Chan */
                                        CCB->ChanType, /* B, H0 or H11 */
                                        ( CCB->ChanType == H0_CHANNEL_UNITS ) ?
                                           CHANNEL_SLOT_MAP : CHANNEL_NUMBERS,
                                        CCB->Exclusive,   /* Exclusive */
                                        CCB->Int_Id_Len, /* Int ID len */
                                        IntID );             /* Int ID */
                  }
#ifdef EXTENDED_HOST_MSG
               else
                  {
                  CCPRI_CopyPublic( CHANNEL_ID_IE, CODESET_0 );
                  }
#endif
               }
            CCPRI_Mail( L3_ID, FALSE );              /* Send CALL_PROC */
            CCB->State = CCPATT_CallRoutingIn;
            }
         else
            {
            /* Cannot send a message to L3.
            */
            CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
            CCPRI_ReleaseCall();
            }
         break;

      case CALL_REJECTION:
         CCPMSG_RejectInCall( 0, 0 );
         CCPRI_ReleaseCall();
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_SendHostDisc( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      case CC_STATUS_IND:
         if ( SourceState == 0 )    /* The call no longer exists at L3 */
            {
            CCPMSG_SendHostDisc( 0, FALSE );
            CCPRI_ReleaseCall();
            }
         else
            {
            CCPMSG_SendHostProgress( STATUS_REPORT, TRUE, FALSE );
            }
         break;

      default:
         CCPRI_Error( CCPATT_CALL_OFFERED, UNKNOWN_EVENT, event, FALSE );
         break;
      }

in_call_failed:
   return;
}


/*************************
* CCPATT_CallOrigination *
**************************
*
* Process the current event in the context of a call in the ORIGINATION
* state.
*
* Primarily we expect CALL_PROCEEDING from the network here.
*
* This state corresponds to L3 Protocol Control State 1.
*
*/
PRIVATE void CCPATT_CallOrigination( event )

int event;                                 /* Event input to the state */

{
   int cause;                         /* Locally generated cause value */
   int prog_ind;                                 /* Progress indicator */
   int ie_cause;                                /* Cause from CAUSE_IE */


#ifdef DEBUG
   printf( "CCPATT executing with event = %x in state = CALL ORIGINATION\n", event );
#endif

   switch ( event )
      {
      case CC_CALL_PROC_IND:     /* CALL_PROCEEDING (translated by L3) */
         if ( CCPRI_GetOutInfo( CALL_ACCEPT ))
            {
            /* Receive final B-Chan choice or reject the message.  Note
            *  that it is essential to return the channel to the HOST
            *  now since the channel the network selects may not be the
            *  same channel the user proposed in the SETUP.  The HOST
            *  should connect to this channel when the CALL_ACCEPT
            *  message is received.
            */
            if (( cause = CCPRI_AcceptChanId( CC_CALL_PROC_IND, 
                                              TRUE )) != ACCEPTED )
               {
               if (( cause == NO_CIRCUIT_AVAILABLE ) ||
                   ( cause == REQ_CHANNEL_NOT_AVAIL ))
                  {
                  CCPMSG_RejectMsg( cause, 0 );
                  }
               else                    /* Missing or mis-coded element */
                  {
                  CCPMSG_RejectMsg( cause, CHANNEL_ID_IE );
                  }
               /* We expect the network to clear the call as a follow-up.
               */
               CCPRI_FreePkgInfo();
               }
            else
               {
               CCPRI_Mail( HOST_ID, TRUE );        /* Send CALL_ACCEPT */
               CCB->State = CCPATT_CallRoutingOut;
               }
            }
         break;

      case CC_SETUP_CONF:                          /* CONNECT received */
         if ( CCPRI_GetOutInfo( CALL_ACCEPT ))
            {
            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                              CHANNEL_ID_IE, CODESET_0, 1 );
            if ( IE )
               {
               /* Go find the info channel selection field.  An indication of
               *  "No Channel" is a Protocol Control indication of call setup
               *  failure.
               */
               IE_Fields = CCIE_ExpandChanId_IE( IE, &Trashcan );
               if ( IE_Fields[ 4 ] == 0 )     /* Test for "No Channel" */
                  {
                  /* SETUP timed-out (2nd T303) to NETWORK.  The SETUP failed.
                  */
                  CCPRI_ReleaseChan();
                  CCPRI_FreePkgInfo();
                  CCPMSG_RejectOutCall( TIMER_EXPIRY, FALSE );
                  CCPRI_ReleaseCall();
                  }
               else
                  {
                  /* Receive final B-Chan choice or reject the message.
                  */
                  if (( cause = CCPRI_AcceptChanId( CC_SETUP_CONF, 
                                                    TRUE )) != ACCEPTED )
                     {
                     if (( cause == NO_CIRCUIT_AVAILABLE ) ||
                         ( cause == REQ_CHANNEL_NOT_AVAIL ))
                        {
                        CCPMSG_RejectMsg( cause, 0 );
                        }
                     else
                        {
                        CCPMSG_RejectMsg( cause, CHANNEL_ID_IE );
                        }
                     CCPRI_FreePkgInfo();
                     }
                  else
                     {
                     CCPRI_Mail( HOST_ID, TRUE );  /* Send CALL_ACCEPT */
                     CCPMSG_SendHostProgress( CALLED_ANSWER, FALSE, TRUE );
                     CCB->State = CCPATT_CallConnected;
                     }
                  }
               }
            }
         break;

      case CC_ALERTING_IND:
         /* Network claims the far end is ringing.
         */
         if ( CCPRI_GetOutInfo( CALL_ACCEPT ))
            {
            cause = CCPRI_AcceptChanId( CC_ALERTING_IND, FALSE );
            if ( cause != ACCEPTED )
               {
               /* This element is mandatory in this context.
               */
               if (( cause == NO_CIRCUIT_AVAILABLE )   ||
                   ( cause == REQ_CHANNEL_NOT_AVAIL ))
                  {
                  CCPMSG_RejectMsg( cause, 0 );
                  }
               else
                  {
                  CCPMSG_RejectMsg( cause, CHANNEL_ID_IE );
                  }
               CCPRI_FreePkgInfo();
               }
            else
               {
               CCPRI_Mail( HOST_ID, TRUE );        /* Send CALL_ACCEPT */
               CCPMSG_SendHostProgress( CALLED_ALERTING, FALSE, TRUE );
               CCB->State = CCPATT_CallRoutingOut;
               }
            }
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_RejectOutCall( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      case CC_PROGRESS_IND:
         if (( cause = CCPRI_AcceptProgressInd( &prog_ind )) != ACCEPTED )
            CCPMSG_RejectMsg( cause, PROGRESS_IND_IE );

         /* Note that the CAUSE IE is not mandatory in this message.
         */
         else if (( cause = CCPRI_AcceptCause( &ie_cause ))
                               != MANDATORY_IE_MISSING )
            {
            CCPMSG_RejectMsg( cause, CAUSE_IE );
            }
         else
            {
            if (( prog_ind == CALL_NOT_END_TO_END_ISDN ) ||
                ( prog_ind == IN_BAND_INFO ))
               CCPMSG_SendHostProgress( prog_ind, TRUE, FALSE );
            CCB->State = CCPATT_CallRoutingOut;
            }
         break;

      case CC_STATUS_IND:
         if ( SourceState == 0 )    /* The call no longer exists at L3 */
            {
            CCPMSG_SendHostDisc( 0, FALSE );
            CCPRI_ReleaseCall();
            }
         else
            {
            CCPMSG_SendHostProgress( STATUS_REPORT, TRUE, FALSE );
            }
         break;

      default:
         CCPRI_Error( CCPATT_CALL_ORIGINATION, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/***********************
* CCPATT_CallReleasing *
************************
*
* Process the current event in the context of a call in the
* RELEASING (waiting for CC_RELEASE_IND or CC_RELEASE_CONF) state.
*
* This state corresponds to L3 Protocol Control States 11, 12 and 19.
*
*/
PRIVATE void CCPATT_CallReleasing( event )

int  event;                                /* Event input to the state */

{
#ifdef DEBUG
   printf( "CCPATT executing with event = %x in state = CALL RELEASINGn", event );
#endif

   switch ( event )
      {
      case CC_RELEASE_IND:
         CCPRI_ReleaseCall();
         break;

      case CC_RELEASE_CONF:
         /* We choose not to examine the message contents and
         *  simply release the call.
         */
         CCPRI_ReleaseCall();
         break;

      case CALL_DISC:                         /* Normal race condition */
      case CC_DISCONNECT_IND:
         break;

      case CC_STATUS_IND:
         if ( SourceState == 0 )
            CCPRI_ReleaseCall();
         break;

      default:
         CCPRI_Error( CCPATT_CALL_RELEASING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/*********************
* CCPATT_CallRinging *
**********************
*
* Process the current event in the context of a call in the RINGING state.
*
* This state corresponds to L3 Protocol Control State 7.
*
*/
PRIVATE void CCPATT_CallRinging( event )

int event;                                 /* Event input to the state */

{
#ifdef DEBUG
   printf( "CCPATT executing with event = %x in state = CALL RINGING\n", event );
#endif

   switch ( event )
      {
      case CALL_PROGRESS:
         if (( IE = CCPRI_GetPrivate( PROGRESS_IND_IE, CODESET_0 )) != NULL )
            {
            if ( IE[ 2 ] == CALLED_ANSWER )
               {
               if ( CCPRI_GetOutPkt( CC_SETUP_RESP ))
                  {
#ifdef EXTENDED_HOST_MSG
                  CCPRI_CopyPublic( CONNECTED_NUM_IE, CODESET_0 );
                  /* Channel ID should already be resolved.
                  */
                  CCPRI_CopyPublic( PROGRESS_IND_IE, CODESET_0 );
                  CCPRI_CopyPublic( REDIRECTING_NUM_IE, CODESET_0 );
#endif
                  /* Consider if UUI should be added from the HOST.
                  */
                  if (( IE = CCPRI_GetPrivate( USER_USER_IE,
                                               CODESET_0 )) != NULL )
                     {
                     OutPkt->InfoLen +=
                        CCIE_BuildUser_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                           USER_USER_IE, IE[ 2 ], &IE[ 3 ],
                                           IE[ 1 ] - 1 );
                     }
#ifdef EXTENDED_HOST_MSG
                  else
                     {
                     CCPRI_CopyPublic( USER_USER_IE, CODESET_0 );
                     }
#endif
                  CCPRI_Mail( L3_ID, FALSE );
                  CCB->State = CCPATT_CallRoutingIn;/* Expect Connect_Ack */
                  }
               }
            }
         break;

      case CALL_UUI:
         CCPMSG_SendPeerUUI();   /* HOST UUI transmission to the peer. */
         break;

      case CC_USER_INFO_IND:
         CCPMSG_SendHostUUI();   /* Peer UUI transmission to the HOST. */
         break;

      case CALL_CONGESTION:
         CCPMSG_SendPeerCongest();       /* Congestion in UUI transmit */
         break;

      case CC_CONG_CON_IND:
         CCPMSG_SendHostCongest();       /* Congestion in UUI transmit */
         break;

      case CALL_FACILITY:
         /* Host requests a Peer Facility
         */
         CCPMSG_FacReqOut();
         break;

      case CC_FACILITY_IND:
         /* Peer requests a Host Facility
         */
         CCPMSG_FacReqIn();
         break;

      case CALL_FACILITY_ACK:
         /* Host acknowledgess a Peer Facility request
         */
         CCPMSG_FacAckOut();
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CALL_FACILITY_REJ:
         /* Host rejects a Peer Facility request
         */
         CCPMSG_FacRejOut();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_DISCONNECT_IND:
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_SendHostDisc( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      case CC_SETUP_IND:             /* Can arrive as a race condition */
         break;

      case CC_STATUS_IND:
         if ( SourceState == 0 )    /* The call no longer exists at L3 */
            {
            CCPMSG_SendHostDisc( 0, FALSE );
            CCPRI_ReleaseCall();
            }
         else
            {
            CCPMSG_SendHostProgress( STATUS_REPORT, TRUE, FALSE );
            }
         break;

      default:
         CCPRI_Error( CCPATT_CALL_RINGING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/***********************
* CCPATT_CallRoutingIn *
************************
*
* Process the current event in the context of a call in the ROUTING_IN
* state which is the generic busy state, but not in conversation, for the
* incoming call.  This state corresponds to Layer 3 states 8 and 9 (user).
*
* This state corresponds to L3 Protocol Control States 8 and 9.
*
*/
PRIVATE void CCPATT_CallRoutingIn( event )

int event;                                 /* Event input to the state */

{
   BYTE *prog_ie;                  /* Pointer to prog_ind info element */


#ifdef DEBUG
   printf( "CCPATT executing with event = %x in state = CALL ROUTING_IN\n", event );
#endif

   switch ( event )
      {
      case CALL_PROGRESS:
      case CALL_ACCEPT:
         /* These messages require at least the private PROGRESS_IND_IE
         *  since this IE is used to specify ALERTING vs CONNECT.
         */
         if (( prog_ie = CCPRI_GetPrivate( PROGRESS_IND_IE,
                                           CODESET_0 )) != NULL )
            {
            if ( prog_ie[ 2 ] == CALLED_ALERTING )
               {
               if ( CCPRI_GetOutPkt( CC_ALERTING_REQ ))
                  {
                  if ( event == CALL_ACCEPT )
                     {
                     /* The channel id needs to be provided as the first
                     *  response to a SETUP.  Exclusive was established
                     *  in CCPATT_CallOffered() from CHANNEL_ID_IE.
                     */
                     IntID[ 0 ] = CCPRI_MapIntId( CCB->Int_Id[ 0 ], TRUE );
                     OutPkt->InfoLen += CCIE_BuildChanId_IE(
                                           &OutPkt->Info[ OutPkt->InfoLen ],
                                           CHANNELS_LISTED,
                                           CCB->B_Chan,
                                           CCB->ChanType,
                                           ( CCB->ChanType == H0_CHANNEL_UNITS ) ?
                                              CHANNEL_SLOT_MAP : CHANNEL_NUMBERS,
                                           CCB->Exclusive,
                                           CCB->Int_Id_Len,
                                           IntID );
                     }
#ifdef EXTENDED_HOST_MSG
                  CCPRI_CopyPublic( PROGRESS_IND_IE, CODESET_0 );
                  CCPRI_CopyPublic( REDIRECTING_NUM_IE, CODESET_0 );
#endif
                  /* Consider if UUI should be added from the HOST.
                  */
                  if (( IE = CCPRI_GetPrivate( USER_USER_IE,
                                               CODESET_0 )) != NULL )
                     {
                     OutPkt->InfoLen +=
                        CCIE_BuildUser_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                           USER_USER_IE, IE[ 2 ], &IE[ 3 ],
                                           IE[ 1 ] - 1 );
                     }
#ifdef EXTENDED_HOST_MSG
                  else
                     {
                     CCPRI_CopyPublic( USER_USER_IE, CODESET_0 );
                     }
#endif
                  CCPRI_Mail( L3_ID, FALSE );
                  CCB->State = CCPATT_CallRinging;
                  }
               }
            else if ( prog_ie[ 2 ] == CALLED_ANSWER )
               {
               if ( CCPRI_GetOutPkt( CC_SETUP_RESP ))
                  {
                  if ( event == CALL_ACCEPT )
                     {
                     /* The channel id needs to be provided as the first
                     *  response to a SETUP.
                     */
                     IntID[ 0 ] = CCPRI_MapIntId( CCB->Int_Id[ 0 ], TRUE );
                     OutPkt->InfoLen += CCIE_BuildChanId_IE(
                                           &OutPkt->Info[ OutPkt->InfoLen ],
                                           CHANNELS_LISTED,
                                           CCB->B_Chan,
                                           CCB->ChanType,
                                           ( CCB->ChanType == H0_CHANNEL_UNITS ) ?
                                              CHANNEL_SLOT_MAP : CHANNEL_NUMBERS,
                                           CCB->Exclusive,
                                           CCB->Int_Id_Len,
                                           IntID );
                     }
#ifdef EXTENDED_HOST_MSG
                  CCPRI_CopyPublic( PROGRESS_IND_IE, CODESET_0 );
                  CCPRI_CopyPublic( REDIRECTING_NUM_IE, CODESET_0 );
#endif
                  /* Consider if UUI should be added from the HOST.
                  */
                  if (( IE = CCPRI_GetPrivate( USER_USER_IE,
                                               CODESET_0 )) != NULL )
                     {
                     OutPkt->InfoLen +=
                        CCIE_BuildUser_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                           USER_USER_IE, IE[ 2 ], &IE[ 3 ],
                                           IE[ 1 ] - 1 );
                     }
#ifdef EXTENDED_HOST_MSG
                  else
                     {
                     CCPRI_CopyPublic( USER_USER_IE, CODESET_0 );
                     }
#endif
                  CCPRI_Mail( L3_ID, FALSE );
                  /* Expect CONNECT_ACK (CC_SETUP_COMP_IND) before we
                  *  transit to the CONNECTED state.
                  */
                  }
               }
            }
         else
            {
            CCPRI_Error( CCPATT_CALL_IN_ROUTING, BAD_DATA_FROM_HOST,
                         PROGRESS_IND_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            }
         break;

      case CC_SETUP_COMP_IND:
         /* CONNECT_ACK received from the far end for a peer-initiated call.
         */
         CCB->State = CCPATT_CallConnected;
         break;

      case CALL_UUI:
         CCPMSG_SendPeerUUI();   /* HOST UUI transmission to the peer. */
         break;

      case CC_USER_INFO_IND:
         CCPMSG_SendHostUUI();   /* Peer UUI transmission to the HOST. */
         break;

      case CALL_CONGESTION:
         CCPMSG_SendPeerCongest();       /* Congestion in UUI transmit */
         break;

      case CC_CONG_CON_IND:
         CCPMSG_SendHostCongest();       /* Congestion in UUI transmit */
         break;

      case CALL_FACILITY:
         /* Host requests a Peer Facility
         */
         CCPMSG_FacReqOut();
         break;

      case CC_FACILITY_IND:
         /* Peer requests a Host Facility
         */
         CCPMSG_FacReqIn();
         break;

      case CALL_FACILITY_ACK:
         /* Host acknowledgess a Peer Facility request
         */
         CCPMSG_FacAckOut();
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CALL_FACILITY_REJ:
         /* Host rejects a Peer Facility request
         */
         CCPMSG_FacRejOut();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_SendHostDisc( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      case CC_STATUS_IND:
         if ( SourceState == 0 )    /* The call no longer exists at L3 */
            {
            CCPMSG_SendHostDisc( 0, FALSE );
            CCPRI_ReleaseCall();
            }
         else
            {
            CCPMSG_SendHostProgress( STATUS_REPORT, TRUE, FALSE );
            }
         break;

      case CC_SETUP_IND:             /* Can arrive as a race condition */
         break;

      default:
         CCPRI_Error( CCPATT_CALL_IN_ROUTING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/************************
* CCPATT_CallRoutingOut *
*************************
*
* Process the current event in the context of a call in the ROUTING_OUT
* state which is the generic busy state, but not in conversation, for the
* outgoing call.  This state corresponds to Layer 3 states 3 and 4 (user).
*
* This state corresponds to L3 Protocol Control States 3 and 4.
*
*/
PRIVATE void CCPATT_CallRoutingOut( event )

int event;                                 /* Event input to the state */

{
   int cause;                         /* Locally generated cause value */
   int ie_cause;                      /* Cause recovered from cause ie */
   int prog_ind;                                 /* Progress indicator */


#ifdef DEBUG
   printf( "CCPATT executing with event = %x in state = CALL ROUTING_OUT\n", event );
#endif

   switch ( event )
      {
      case CC_ALERTING_IND:                    /* Called is being rung */
         /* The CHANNEL_ID_IE is prodably not present since it was probably
         *  sent across in the CALL_PROCEEDING message.  B-channel is of 
         *  no concern since it should have been negotiated already.
         */
         CCPMSG_SendHostProgress( CALLED_ALERTING, FALSE, TRUE );
         break;

      case CC_SETUP_CONF:
         /* CONNECT received from the far end.  This message is expected
         *  for a locally-originated call.  B-channel is of no concern
         *  since it should have been negotiated already.
         */
         CCPMSG_SendHostProgress( CALLED_ANSWER, FALSE, TRUE );
         if ( CCPRI_GetOutPkt( CC_CONNECT_ACK_REQ )) /* CONNECT_ACK */
            CCPRI_Mail( L3_ID, FALSE );
         CCB->State = CCPATT_CallConnected;    /* Enter conversation */
         break;

      case CC_PROGRESS_IND:                  /* PROGRESS from the peer */
         if (( cause = CCPRI_AcceptProgressInd( &prog_ind )) != ACCEPTED )
            {
            CCPMSG_RejectMsg( cause, PROGRESS_IND_IE );
            }
         else
            {
            /* Note that the CAUSE IE is not mandatory in this message.
            */
            cause = CCPRI_AcceptCause( &ie_cause );
   
            if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
               {
               SourceState = 4;                      /* L3 Call Delivered */
               CCPMSG_RejectMsg( cause, CAUSE_IE );
               }
            else if (( prog_ind == CALL_NOT_END_TO_END_ISDN ) ||
                     ( prog_ind == IN_BAND_INFO ))
               {
               CCPMSG_SendHostProgress( prog_ind, TRUE, FALSE );
               }
            }
         break;

      case CALL_UUI:
         CCPMSG_SendPeerUUI();   /* HOST UUI transmission to the peer. */
         break;

      case CC_USER_INFO_IND:
         CCPMSG_SendHostUUI();   /* Peer UUI transmission to the HOST. */
         break;

      case CALL_CONGESTION:
         CCPMSG_SendPeerCongest();       /* Congestion in UUI transmit */
         break;

      case CC_CONG_CON_IND:
         CCPMSG_SendHostCongest();       /* Congestion in UUI transmit */
         break;

      case CALL_FACILITY:
         /* Host requests a Peer Facility
         */
         CCPMSG_FacReqOut();
         break;

      case CC_FACILITY_IND:
         /* Peer requests a Host Facility
         */
         CCPMSG_FacReqIn();
         break;

      case CALL_FACILITY_ACK:
         /* Host acknowledgess a Peer Facility request
         */
         CCPMSG_FacAckOut();
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CALL_FACILITY_REJ:
         /* Host rejects a Peer Facility request
         */
         CCPMSG_FacRejOut();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPATT_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_SendHostDisc( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      case CC_STATUS_IND:
         if ( SourceState == 0 )    /* The call no longer exists at L3 */
            {
            CCPMSG_SendHostDisc( 0, FALSE );
            CCPRI_ReleaseCall();
            }
         else
            {
            CCPMSG_SendHostProgress( STATUS_REPORT, TRUE, FALSE );
            }
         break;

      default:
         CCPRI_Error( CCPATT_CALL_OUT_ROUTING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/************************************************************************
*                   C C B   I N I T I A L I Z A T I O N                 *
*************************************************************************
*/
/**********************
* CCPATT_InitialState *
***********************
*
* Provide access to CCPRI.C for CCB initialization.
*
*/
PUBLIC CC_STATE CCPATT_InitialState()

{
   return ( CCPATT_CallIdle );
}


#ifdef TRACING                      /* Taskview or Tracker environment */
/************************************************************************
*                  C C B   S T A T E   T R A S L A T I O N              *
*************************************************************************
*/
/**********************
* CCPATT_NumericState *
***********************
*
* Translate the state to a numeric index.  This function is only intended
* as a support for Tracker and Taskview demo and test systems.  It is
* not normally used in a working system because it is inefficient and
* consumes memory for marginal value.
*
*/
PUBLIC BYTE CCPATT_NumericState( ccb_state )

CC_STATE ccb_state;                                 /* State of a call */
{
   static CC_STATE call_state[] = { NULL,                         /* 0 */
                                    CCPATT_CallConnected,         /* 1 */
                                    CCPATT_CallIdle,              /* 2 */
                                    CCPATT_CallOffered,           /* 3 */
                                    CCPATT_CallOrigination,       /* 4 */
                                    CCPATT_CallRinging,           /* 5 */
                                    CCPATT_CallReleasing,         /* 6 */
                                    CCPATT_CallRoutingIn,         /* 7 */
                                    CCPATT_CallRoutingOut         /* 8 */
                                  };/* Change here if a state is added */
   BYTE i; 

   for ( i = 0; i < 9; i++ )        /* Change here if a state is added */
      {
      if ( ccb_state == call_state[ i ] )
         goto found;
      }
   i = 0;

found:
   return ( i );
}
#endif                                                      /* TRACING */
/* End of module
*/
