/* $Id: ccpvn.c,v 3.2 1995/11/17 17:40:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/ccpvn.c,v $
 *------------------------------------------------------------------
 * French primary rate
 *
 * May 1992, Ronnie B. Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccpvn.c,v $
 * Revision 3.2  1995/11/17  17:40:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:14:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
*************************************************************************

MODULE:       ccpvn.c
DESCRIPTION:  ISDN Primary Rate Interface French VN(X) Compatible Call
              State Module
VERSION:      3.21

*************************************************************************
****copyright_c**********************************************************

                          COPYRIGHT STATEMENT

                          Copyright (c) 1992
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
French VN(X) PRI User-side operation.

This module represents one of several Central Office switch-specific
PRI state solutions which reside under CCPRI.C according to the product
developer's requirements.

Any references to VN2 as opposed to later VN versions are purely
historic, as the CCPRI call control has been architected to VN3
Internal Technical Specification, France Telecom/CNET ST/LAA/RSM.144,
Ed. 1, October, 1988 (English Translation) and France Telecom CNET
ST/LAA/ELR/TRA 733 Aug. 1990 (Original French).

The module supports the Supplementary Services Call Deflection and
User to User information.  If this supplementary code was purchased from 
Telenetworks, these features may be selectively activated by enabling the 
following compile-time flags in FLAGS.H:

   VN2_SS_DEFLECTION
   VN2_SS_UUI

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
/* The call control states for a call
*/
PRIVATE void CCPVN_CallConnected     ARGS(( int ));
PRIVATE void CCPVN_CallDialing       ARGS(( int ));
PRIVATE void CCPVN_CallIdle          ARGS(( int ));
PRIVATE void CCPVN_CallOffered       ARGS(( int ));
PRIVATE void CCPVN_CallOrigination   ARGS(( int ));
PRIVATE void CCPVN_CallRinging       ARGS(( int ));
PRIVATE void CCPVN_CallReleasing     ARGS(( int ));
PRIVATE void CCPVN_CallRoutingIn     ARGS(( int ));
PRIVATE void CCPVN_CallRoutingOut    ARGS(( int ));

#ifdef TRACING
/* NOTE: If a state is added or deleted, CCPVN_NumericState() should be
*        updated.
*/

#endif
/************************************************************************
*                    D A T A   A L L O C A T I O N                      *
*************************************************************************
*/
PRIVATE BYTE IntID[ 2 ];                /* For interface ID management */

/************************************************************************
*                C A L L   S T A T E   F U N C T I O N S                *
*************************************************************************
*/
/**********************
* CCPVN_CallConnected *
***********************
*
* Process the current event in the context of a call in the CONNECTED
* state.
*
* This state corresponds to L3 Protocol Control State 10.
*
*/
PRIVATE void CCPVN_CallConnected( event )

int event;                                 /* Event input to the state */

{
#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL CONNECTED\n", event );
#endif

   switch ( event )
      {
      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CALL_FACILITY:
         CCPMSG_FacReqOut();          /* Host requests a Peer Facility */
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CC_INFO_IND:
         CCPMSG_SendHostInfo();
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
         CCPRI_Error( CCPVN_CALL_CONNECTED, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/********************
* CCPVN_CallDialing *
*********************
*
* Process the current event in the context of a call in the DIALING state.
*
* This state corresponds to L3 Protocol Control State 2.
*
* NT2 are not explicitly forbidden to perform overlap sending.
*
*/
PRIVATE void CCPVN_CallDialing( event )

int event;                                 /* Event input to the state */
{

#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL DIALING\n", event );
#endif

   switch ( event )
      {
      case CALL_INFO:                                 /* Host digit(s) */
         if ( CCPRI_GetOutPkt( CC_INFO_REQ ))
            {
            if (( IE = CCPRI_GetPrivate( CALLED_NUMBER_IE, CODESET_0 )) != NULL )
               {
               OutPkt->InfoLen +=
                  CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CALLED_NUMBER_IE,
                                          IE[ 2 ],    /* Number type * */
                                          IE[ 3 ],      /* Number plan */
                                         &IE[ 4 ],     /* Digit buffer */
                                          IE[ 1 ] - 2,/* Num of digits */
                                          TRUE );             /* ASCII */
               }
#ifdef EXTENDED_HOST_MSG
            else                         /* Look for "public" version. */
               {
               CCPRI_CopyPublic( CALLED_NUMBER_IE, CODESET_0 );
               }
#endif
            }
         CCPRI_Mail( L3_ID, FALSE );               /* Send CC_INFO_REQ */
         break;

      case CC_CALL_PROC_IND:                  /* Called is progressing */
         CCB->State = CCPVN_CallRoutingOut;
         break;

      case CC_ALERTING_IND:                    /* Called is being rung */
         CCPVN_CallRoutingOut( CC_ALERTING_IND );
         CCB->State = CCPVN_CallRoutingOut;
         break;

      case CC_SETUP_CONF:
      case CC_PROGRESS_IND:
         CCPVN_CallRoutingOut( event );
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPVN_CallReleasing;
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
         CCPRI_Error( CCPVN_CALL_DIALING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/*****************
* CCPVN_CallIdle *
******************
*
* Process the current event in the context of a call in the IDLE state.
*
* This state corresponds to L3 Protocol Control State 0.
*
*/
PRIVATE void CCPVN_CallIdle( event )

int event;                                 /* Event input to the state */
{

#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL IDLE\n", event );
#endif

   switch ( event )
      {
      case CALL_OUTGOING:
         /* Outgoing call request by the host.  This complex function
         *  is implemented independently.
         */
         CCB->Responded = TRUE;
         if ( CCPMSG_OutCall() )
            CCB->State = CCPVN_CallOrigination;
         else
            CCPRI_ReleaseCall();
         break;

      case CC_SETUP_IND:
         if ( CCPMSG_InCall() )
            CCB->State = CCPVN_CallOffered;
        else
            CCPRI_ReleaseCall();
         break;

      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
         CCPRI_ReleaseCall();
         break;

      default:
         CCPRI_Error( CCPVN_CALL_IDLE, UNKNOWN_EVENT, event, FALSE );
         CCPRI_ReleaseCall();
         break;
      }
}

/********************
* CCPVN_CallOffered *
*********************
*
* Process the current event in the context of a call in the OFFERED
* state.  This state is invoked when an INCOMING_CALL has been offered
* to the HOST and a response has not yet been received.
*
* This state corresponds to L3 Protocol Control State 6.
*
*/
PRIVATE void CCPVN_CallOffered( event )

int event;                                 /* Event input to the state */
{
   int chan;                             /* Channel from CHANNEL_ID_IE */
   BYTE int_id;                                        /* Interface ID */

#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL OFFERED\n", event );
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
            /* Note that we do not expect this case since the user is
            *  expected to use tthe channel offered by the network.
            */
            if ( CCB->B_Chan )
               CCPRI_ReleaseChan();                /* Free the old one */
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
         if (( !CCB->Responded ) && ( CCPRI_GetOutPkt( CC_BROADCAST_RESP )))
            {
            CCPRI_Mail( L3_ID, FALSE );      /* Send CC_BROADCAST_RESP */
            CCB->Responded = TRUE;
            }
         if (( IE = CCPRI_GetPrivate( PROGRESS_IND_IE, CODESET_0 )) != NULL )
            {
            /* The inclusion of this IE in this context indicates that
            *  the Host is reporting that the call is already ALERTING or
            *  CONNECTED.  We will skip sending CC_CALL_PROC_REQ and go
            *  directly to ALERTING or CONNECT phases.
            */
            CCPVN_CallRoutingIn( CALL_ACCEPT );
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
                  IntID[ 1 ] = 0;
                  OutPkt->InfoLen += CCIE_BuildChanId_IE(
                                        &OutPkt->Info[ OutPkt->InfoLen ],
                                        CHANNELS_LISTED,
                                        CCB->B_Chan,  /* Lowest B_Chan */
                                        CCB->ChanType,       /* B-chan */
                                        CHANNEL_NUMBERS,/* No slot map */
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
               /* Add TERMINAL_CAPABILITY_IE non-locking in Codeset 6.
               */
               OutPkt->InfoLen +=
                  CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                             CODESET_6, FALSE );
               OutPkt->InfoLen +=
                  CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                        CEPT_NATIONAL_STANDARD,
                                        NATIONAL_FUNCTION );
               }
            CCPRI_Mail( L3_ID, FALSE );              /* Send CALL_PROC */
            CCB->State = CCPVN_CallRoutingIn;
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

      case CALL_RELEASE:
         /* Call rejection, but with UUI or redirection.
         */
         if ( CCPRI_GetOutPkt( CC_BROADCAST_RESP ))
            CCPRI_Mail( L3_ID, FALSE );
         if ( CCPRI_GetOutPkt( CC_RELEASE_REQ ))
            {
            /* Build Cause info element.
            */
            if (( IE = CCPRI_GetPrivate( CAUSE_IE, CODESET_0 )) != NULL )
               {
               Diag[ 0 ] = NO_DIAG;
               OutPkt->InfoLen +=
                  CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                      IE[ 2 ],
                                      AM_I_THE_NETWORK_SIDE,
                                      Diag );
               }
#ifdef EXTENDED_HOST_MSG
            else
               {
               CCPRI_CopyPublic( CAUSE_IE, CODESET_0 );
               }
#endif
            /* Add TERMINAL_CAPABILITY_IE non-locking in Codeset 6.
            */
            OutPkt->InfoLen +=
               CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CODESET_6, FALSE );
            OutPkt->InfoLen +=
               CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     CEPT_NATIONAL_STANDARD,
                                     NATIONAL_FUNCTION );
            CCPRI_Mail( L3_ID, FALSE );
            CCB->State = CCPVN_CallReleasing;
            }
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPVN_CallReleasing;
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
         CCPRI_Error( CCPVN_CALL_OFFERED, UNKNOWN_EVENT, event, FALSE );
         break;
      }

in_call_failed:
   return;
}

/************************
* CCPVN_CallOrigination *
*************************
*
* Process the current event in the context of a call in the ORIGINATION
* state.
*
* Primarily we expect CALL_PROCEEDING from the network here.
*
* This state corresponds to L3 Protocol Control State 1.
*
*/
PRIVATE void CCPVN_CallOrigination( event )

int event;                                 /* Event input to the state */

{
   int cause;                         /* Locally generated cause value */
   int ie_cause;                                /* Cause from CAUSE_IE */


#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL ORIGINATION\n", event );
#endif

   switch ( event )
      {
      case CC_CALL_PROC_IND:       /* CALL_PROCEEDING translated by L3 */
      case CC_MORE_INFO_IND:               /* SETUP_ACK received by L3 */
         if ( CCPRI_GetOutInfo( CALL_ACCEPT ))
            {
            /* Receive final B-Chan choice or reject the call.  Note
            *  that it is essential to return the channel to the HOST
            *  now since the channel the network selects may not be the
            *  same channel the user proposed in the SETUP.  The HOST
            *  should connect to this channel when the CALL_ACCEPT
            *  message is received.
            */
            if (( cause = CCPRI_AcceptChanId( CC_CALL_PROC_IND,
                                              TRUE )) != ACCEPTED )
               {
               CCPRI_FreePkgInfo();                /* Free CALL_ACCEPT */
               CCPMSG_RejectResponse( cause, 0 );
               CCPMSG_RejectOutCall( 0, FALSE );
               CCPRI_ReleaseCall();
               }
            else
               {
               CCPRI_Mail( HOST_ID, TRUE );        /* Send CALL_ACCEPT */
               if ( event == CC_CALL_PROC_IND )
                  {
                  CCB->State = CCPVN_CallRoutingOut;
                  }
               else
                  {
                  CCPMSG_SendHostProgress( MORE_DIGITS, FALSE, FALSE );
                  CCB->State = CCPVN_CallDialing;
                  }
               }
            }
         break;

      case CC_SETUP_CONF:                  /* "Quick" CONNECT received */
         if ( CCPRI_GetOutInfo( CALL_ACCEPT ))
            {
            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                              CHANNEL_ID_IE, CODESET_0, 1 );
            if ( IE )
               {
               /* Go find the info channel selection field.  An indication
               *  of "No Channel" is a Protocol Control indication of call
               *  setup failure.
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
                     CCPRI_FreePkgInfo();          /* Free CALL_ACCEPT */
                     CCPMSG_RejectResponse( cause, 0 );
                     CCPMSG_RejectOutCall( 0, FALSE );
                     CCPRI_ReleaseCall();
                     }
                  else
                     {
                     CCPRI_Mail( HOST_ID, TRUE );  /* Send CALL_ACCEPT */
                     CCPMSG_SendHostProgress( CALLED_ANSWER, FALSE, TRUE );
                     CCB->State = CCPVN_CallConnected;
                     }
                  }
               }
            }
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_RejectOutCall( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      case CC_PROGRESS_IND:
         /* Note that the CAUSE IE is mandatory in this message.
         */
         if (( cause = CCPRI_AcceptCause( &ie_cause )) != ACCEPTED )
            {
            CCPMSG_RejectResponse( cause, 0 );
            CCPMSG_RejectOutCall( 0, FALSE );
            CCPRI_ReleaseCall();
            }
         else
            {
            CCPMSG_SendHostProgress( 0, TRUE, FALSE );
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
         CCPRI_Error( CCPVN_CALL_ORIGINATION, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/**********************
* CCPVN_CallReleasing *
***********************
*
* Process the current event in the context of a call in the
* RELEASING (waiting for CC_RELEASE_IND or CC_RELEASE_CONF) state.
*
* This state corresponds to L3 Protocol Control States 11, 12 and 19.
*
*/
PRIVATE void CCPVN_CallReleasing( event )

int  event;                                /* Event input to the state */

{
#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL RELEASINGn", event );
#endif

   switch ( event )
      {
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         /* We explicitly force the the sending of the CALL_CLEARED
         *  message to append the Q.931 elements here so that information
         *  conveyed in the network's RELEASE message (such as call charge
         *  information) will make it to the HOST.
         */
         if (( CCB->ClearHost ) && ( CCPRI_GetOutInfo( CALL_CLEARED )))
            CCPRI_Mail( HOST_ID, TRUE );
         CCB->ClearHost = FALSE;
         CCPRI_ReleaseCall();
         break;

      case CALL_DISC:                        /* Normal race conditions */
      case CC_DISCONNECT_IND:
         break;

      case CALL_FACILITY:
         CCPMSG_FacReqOut();   /* Host requests peer fac, e.g. charges */
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CC_INFO_IND:
         CCPMSG_SendHostInfo();
         break;

      case CC_STATUS_IND:
         if ( SourceState == 0 )
            CCPRI_ReleaseCall();
         break;

      default:
         CCPRI_Error( CCPVN_CALL_RELEASING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/********************
* CCPVN_CallRinging *
*********************
*
* Process the current event in the context of a call in the RINGING state.
*
* This state corresponds to L3 Protocol Control State 7.
*
*/
PRIVATE void CCPVN_CallRinging( event )

int event;                                 /* Event input to the state */

{
#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL RINGING\n", event );
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
                  CCPRI_Mail( L3_ID, FALSE );
                  CCB->State = CCPVN_CallRoutingIn;/* Expect Connect_Ack */
                  }
               }
            }
         break;

      case CALL_FACILITY:
         CCPMSG_FacReqOut();          /* Host requests a Peer Facility */
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_DISCONNECT_IND:
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_SendHostDisc( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      case CC_INFO_IND:
         CCPMSG_SendHostInfo();
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
         CCPRI_Error( CCPVN_CALL_RINGING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/**********************
* CCPVN_CallRoutingIn *
***********************
*
* Process the current event in the context of a call in the ROUTING_IN
* state which is the generic busy state, but not in conversation, for the
* incoming call.  This state corresponds to Layer 3 states 8 and 9 (user).
*
* This state corresponds to L3 Protocol Control States 8 and 9.
*
*/
PRIVATE void CCPVN_CallRoutingIn( event )

int event;                                 /* Event input to the state */

{
   BYTE *prog_ie;                  /* Pointer to prog_ind info element */


#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL ROUTING_IN\n", event );
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
                  /* The Host may add a CAUSE_IE to this msg (USER_BUSY).
                  */
                  if (( IE = CCPRI_GetPrivate( CAUSE_IE, CODESET_0 )) != NULL )
                     {
                     Diag[ 0 ] = NO_DIAG;
                     OutPkt->InfoLen +=
                        CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                            IE[ 2 ],
                                            AM_I_THE_NETWORK_SIDE,
                                            Diag );
                     }
#ifdef EXTENDED_HOST_MSG
                  else
                     {
                     CCPRI_CopyPublic( CAUSE_IE, CODESET_0 );
                     }
#endif
                  if ( event == CALL_ACCEPT )        /* First response */
                     {
                     /* Add TERMINAL_CAPABILITY_IE non-locking in Codeset 6.
                     */
                     OutPkt->InfoLen +=
                        CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                                   CODESET_6, FALSE );
                     OutPkt->InfoLen +=
                        CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                              CEPT_NATIONAL_STANDARD,
                                               NATIONAL_FUNCTION );
                     }
                  CCPRI_Mail( L3_ID, FALSE );
                  CCB->State = CCPVN_CallRinging;
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
                     IntID[ 1 ] = 0;
                     OutPkt->InfoLen += CCIE_BuildChanId_IE(
                                           &OutPkt->Info[ OutPkt->InfoLen ],
                                           CHANNELS_LISTED,
                                           CCB->B_Chan,
                                           CCB->ChanType,
                                           CHANNEL_NUMBERS,
                                           CCB->Exclusive,
                                           CCB->Int_Id_Len,
                                           IntID );
                     /* Add TERMINAL_CAPABILITY_IE non-locking in Codeset 6.
                     */
                     OutPkt->InfoLen +=
                        CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                                   CODESET_6, FALSE );
                     OutPkt->InfoLen +=
                        CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                              CEPT_NATIONAL_STANDARD,
                                               NATIONAL_FUNCTION );
                     }
                  CCPRI_Mail( L3_ID, FALSE );
                  /* Expect CONNECT_ACK (CC_SETUP_COMP_IND) before we
                  *  transit to the CONNECTED state.
                  */
                  }
               }
            else     /* We don't generate PROGRESS towards the network */
               {
               CCPRI_Error( CCPVN_CALL_IN_ROUTING, BAD_DATA_FROM_HOST,
                            PROGRESS_IND_IE, FALSE );
               CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
               }
            }
         else
            {
            CCPRI_Error( CCPVN_CALL_IN_ROUTING, BAD_DATA_FROM_HOST,
                         PROGRESS_IND_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            }
         break;

      case CC_SETUP_COMP_IND:
         /* CONNECT_ACK received from the far end for a peer-initiated call.
         */
         CCB->State = CCPVN_CallConnected;
         break;

      case CALL_FACILITY:
         /* Host requests a Peer Facility
         */
         CCPMSG_FacReqOut();
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_INFO_IND:
         CCPMSG_SendHostInfo();
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
         CCPRI_Error( CCPVN_CALL_IN_ROUTING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/***********************
* CCPVN_CallRoutingOut *
************************
*
* Process the current event in the context of a call in the ROUTING_OUT
* state which is the generic busy state, but not in conversation, for the
* outgoing call.
*
* This state corresponds to L3 Protocol Control States 3 and 4.
*
* WARNING:
* Be aware that this state is called in certain cases from
* CCPVN_CallDialing() to share its functionality for certain messages
* common to both.
*/
PRIVATE void CCPVN_CallRoutingOut( event )

int event;                                 /* Event input to the state */

{
   int cause;                         /* Locally generated cause value */
   int ie_cause;                      /* Cause recovered from cause ie */


#ifdef DEBUG
   printf( "CCPVN executing with event = %x in state = CALL ROUTING_OUT\n", event );
#endif

   switch ( event )
      {
      case CC_ALERTING_IND:                    /* Called is being rung */
         CCPMSG_SendHostProgress( CALLED_ALERTING, FALSE, TRUE );
         break;

      case CC_SETUP_CONF:
         /* CONNECT received from the far end.  This message is expected
         *  for a locally-originated call.  B-channel is of no concern
         *  since it should have been negotiated already.
         */
         CCPMSG_SendHostProgress( CALLED_ANSWER, FALSE, TRUE );
         CCB->State = CCPVN_CallConnected;       /* Enter conversation */
         break;

      case CC_PROGRESS_IND:                  /* PROGRESS from the peer */
         /* Note that the CAUSE IE is mandatory in this message.
         */
         if (( cause = CCPRI_AcceptCause( &ie_cause )) != ACCEPTED )
            {
            CCPMSG_RejectResponse( cause, 0 );
            CCPMSG_SendHostDisc( 0, FALSE );
            CCPRI_ReleaseCall();
            }
         else
            {
            CCPMSG_SendHostProgress( 0, TRUE, FALSE );
            }
         break;

      case CALL_FACILITY:
         CCPMSG_FacReqOut();          /* Host requests a Peer Facility */
         break;

      case CC_FACILITY_ACK:
         /* Peer acknowledgess a Host Facility request
         */
         CCPMSG_FacAckIn();
         break;

      case CC_FACILITY_REJ:
         /* Peer rejects a Host Facility request
         */
         CCPMSG_FacRejIn();
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->State = CCPVN_CallReleasing;
         break;

      case CC_INFO_IND:
         CCPMSG_SendHostInfo();
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
         CCPRI_Error( CCPVN_CALL_OUT_ROUTING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/************************************************************************
*                   C C B   I N I T I A L I Z A T I O N                 *
*************************************************************************
*/
/*********************
* CCPVN_InitialState *
**********************
*
* Provide access to this module for CCPRI.C through CCB initialization.
* A new CCB will be assigned this state.  Subsequently the CCB state is
* only altered within this module.
*
*/
PUBLIC CC_STATE CCPVN_InitialState()

{
   return ( CCPVN_CallIdle );
}

#ifdef TRACING                      /* Taskview or Tracker environment */
/************************************************************************
*                 C C B   S T A T E   T R A N S L A T I O N             *
*************************************************************************
*/
/*********************
* CCPVN_NumericState *
**********************
*
* Translate the state address to a numeric index.  This function is only
* intended as a support for Tracker and Taskview demo and test systems.
* It is not normally used in a working system because it is inefficient
* and consumes memory for marginal value.  The returned value is intended
* for screen displays.
*
*/
PUBLIC BYTE CCPVN_NumericState( ccb_state )

CC_STATE ccb_state;                                 /* State of a call */
{
   static CC_STATE call_state[] = { NULL,                         /* 0 */
                                    CCPVN_CallConnected,          /* 1 */
                                    CCPVN_CallDialing,            /* 2 */
                                    CCPVN_CallIdle,               /* 3 */
                                    CCPVN_CallOffered,            /* 4 */
                                    CCPVN_CallOrigination,        /* 5 */
                                    CCPVN_CallRinging,            /* 6 */
                                    CCPVN_CallReleasing,          /* 7 */
                                    CCPVN_CallRoutingIn,          /* 8 */
                                    CCPVN_CallRoutingOut          /* 9 */
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

