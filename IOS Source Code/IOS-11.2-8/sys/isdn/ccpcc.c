/* $Id: ccpcc.c,v 3.3.52.1 1996/04/30 23:07:07 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccpcc.c,v $
 *------------------------------------------------------------------
 * ccpcc.c  ISDN Primary Rate Interface Compatible Call State Module
 *          this will handle all the switchtypes, AT&T, DMS
 *
 * March 1994, Dennis Clare 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccpcc.c,v $
 * Revision 3.3.52.1  1996/04/30  23:07:07  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/28  22:51:52  dclare
 * CSCdi44526:  ISDN PRI NET5 changes for Italian homologation. Changes to
 * handle corner test cases. Restart handling changed to handle interfaces,
 * a channel, and multiple channels.
 *
 * Revision 3.2  1995/11/17  17:39:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:14:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       ccpcc.c                                                
DESCRIPTION:  ISDN Primary Rate Interface Compatible Call State  
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

This module implements the Call Control states necessary to provide User-
side operation. It is based on the AT&T 4/5ESS PRI module and will support
the call control for all primary rate switchtypes.


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
#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "gp.h"
#include    "ccbri.h"
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
PUBLIC BYTE CCPCC_VerifyDiscIEs      (BYTE *, int);
PUBLIC BYTE CCPCC_VerifyRestartIEs   (BYTE *, int, int *);

PRIVATE void CCPCC_CallConnected     (int);
PRIVATE void CCPCC_CallIdle          (int);
PRIVATE void CCPCC_CallOffered       (int);
PRIVATE void CCPCC_CallOrigination   (int);
PRIVATE void CCPCC_CallRinging       (int);
PRIVATE void CCPCC_CallReleasing     (int);
PRIVATE void CCPCC_CallRoutingIn     (int);
PRIVATE void CCPCC_CallRoutingOut    (int);

#ifdef TRACING
/* NOTE: If a state is added or deleted, CCPCC_NumericState() should be 
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

/**********************
* CCPCC_CallConnected *
***********************
*
* Process the current event in the context of a call in the CONNECTED
* state.
*
* This state corresponds to L3 Protocol Control State 10.
*
*/
PRIVATE void CCPCC_CallConnected( int event )
{
   if (detail_debug)
      buginf("\nCCPCC executing with event = %x in state = CALL CONNECTED", event);
   switch ( event )
      {
      case CALL_DISC:
         CCPMSG_DiscPeer( 0 );
         CCB->PriState = CCPCC_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->PriState = CCPCC_CallReleasing;
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

      case CALL_NOTIFY:
         CCPMSG_NotifyOut();
         break;

      case CC_NOTIFY_IND:
         CCPMSG_NotifyIn();
         break;

      case CC_INFO_IND:
         /*
          * This is for TS014.
          */
         CCPMSG_InfoIn();
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
         CCPRI_Error( CCPCC_CALL_CONNECTED, P_UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/*****************
* CCPCC_CallIdle *
******************
*
* Process the current event in the context of a call in the IDLE state.
*
* This state corresponds to L3 Protocol Control State 0.
*
*/
PRIVATE void CCPCC_CallIdle( int event )
{
   if (detail_debug)
       buginf("\nCCPCC executing with event = %x in state = CALL IDLE", event);
   switch ( event )
      {
      case CALL_OUTGOING:
         /* Outgoing call request by the host.  This complex function
          * is implemented independently.
          */
         if ( CCPMSG_OutCall() )
            CCB->PriState = CCPCC_CallOrigination;
         else {
            CCPRI_ReleaseCall();
         }
         break;

      case CC_SETUP_IND:
         if (detail_debug)
            buginf("\nCallIdle, CCB->Call_ID = %x", CCB->Call_ID);
         if ( CCPMSG_InCall() )
            CCB->PriState = CCPCC_CallOffered;
         else 
            CCPRI_ReleaseCall();
         break;

      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
         CCPRI_ReleaseCall();
         break;

      default:
         CCPRI_Error( CCPCC_CALL_IDLE, P_UNKNOWN_EVENT, event, FALSE );
         CCPRI_ReleaseCall();
         break;
      }
}


/********************
* CCPCC_CallOffered *
*********************
*
* Process the current event in the context of a call in the OFFERED
* state.  This state is invoked when an INCOMING_CALL has been offered
* to the HOST and a response has not yet been received.
*
* This state corresponds to L3 Protocol Control State 6.
*
*/
PRIVATE void CCPCC_CallOffered( int event )
{
   int chan;                             /* Channel from CHANNEL_ID_IE */
   BYTE int_id;                                        /* Interface ID */

   if (detail_debug)
       buginf("\nCCPCC executing with event = %x in state = CALL OFFERED", event);
   switch ( event )
      {
      case CALL_ACCEPT:
         /* Build mandatory CHANNEL ID element required in the first
          * response to a SETUP by recovering information sent by the host.
          * This response is equivalent to CALL_PROCEEDING.
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
          * interface.
          */
         if (( CCB->B_Chan != chan ) || ( CCB->Int_Id[ 0 ] != int_id ))
            {
#ifdef CHANNEL_TABLE
            CCPRI_ReleaseChan();                   /* Free the old one */
            /*
             * This only seems to apply to at&t, not to the dms
             */
            if (( CC_CHAN_GetStatepri( int_id, chan, 
                                    CCB->ChanType, Dsl_Id ) == CHANNEL_IDLE ) &&
                ( CC_CHAN_GetServState( int_id, chan, 
                                        CCB->ChanType, Dsl_Id ) != OUT_OF_SERVICE ))
               {
               CCB->B_Chan      = chan;                   /* Save chan */
               CCB->Int_Id[ 0 ] = int_id;              /* Interface ID */
               CC_CHAN_SetStatepri( int_id, chan, CCB->ChanType, 
                                 CHANNEL_BUSY, Dsl_Id );
               CC_CHAN_SetProtocolpri( int_id, chan, CCB->ChanType, 
                                    L2P_LAPD, Dsl_Id );
               CC_CHAN_SetUserpri( int_id, chan, CCB->ChanType, 
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
         if ( CCPRI_GetOutPkt( CC_BROADCAST_RESP )) {
            CCPRI_Mail( L3_ID, FALSE );      /* Send CC_BROADCAST_RESP */
         } else {
            /* Cannot send a message to L3.
             */
            CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
            CCPRI_ReleaseCall();
            goto in_call_failed;
         }

         if (( IE = CCPRI_GetPrivate( PROGRESS_IND_IE, CODESET_0 )) != NULL )
            {
            /* The call is already ALERTING or CONNECTED
             */
            CCPCC_CallRoutingIn( CALL_ACCEPT );
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
            CCB->PriState = CCPCC_CallRoutingIn;
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
         CCB->PriState = CCPCC_CallReleasing;
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
         CCPRI_Error( CCPCC_CALL_OFFERED, P_UNKNOWN_EVENT, event, FALSE );
         break;
      }

in_call_failed:
   return;
}


/************************
* CCPCC_CallOrigination *
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
PRIVATE void CCPCC_CallOrigination( int event )
{
   int cause;                         /* Locally generated cause value */
   int prog_ind;                                 /* Progress indicator */
   int ie_cause;                                /* Cause from CAUSE_IE */

   if (detail_debug)
       buginf("\nCCPCC executing with event = %x in state = CALL ORIGINATION", event);
   switch ( event )
      {
      case CC_CALL_PROC_IND:     /* CALL_PROCEEDING (translated by L3) */
      case CC_MORE_INFO_IND:  /* added for NET5 TC10201 */
         if ( CCPRI_GetOutInfo( CALL_ACCEPT ))
            {
            /* Receive final B-Chan choice or reject the message.  Note
             * that it is essential to return the channel to the HOST
             * now since the channel the network selects may not be the
             * same channel the user proposed in the SETUP.  The HOST
             * should connect to this channel when the CALL_ACCEPT
             * message is received.
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
               CCB->PriState = CCPCC_CallRoutingOut;
               }
            }
         break;

      case CC_SETUP_CONF:                          /* CONNECT received */
         if ( CCPRI_GetOutInfo( CALL_CONNECT ))
            {
            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                              CHANNEL_ID_IE, CODESET_0, 1 );
            if ( IE )
               {
               /* Go find the info channel selection field.  An indication of
                * "No Channel" is a Protocol Control indication of call setup
                * failure.
                */
               IE_Fields = CCIE_ExpandChanId_IE( IE, &Trashcan );
               if ( IE_Fields[ 4 ] == 0 )     /* Test for "No Channel" */
                  {
                  /* SETUP timed-out (2nd T303) to NETWORK.  The SETUP failed.
                   */
                  CCPRI_ReleaseChan();
                  CCPRI_FreePkgInfo();
                  CCPMSG_RejectOutCall( TIMER_EXPIRY, FALSE );
                  /*
                   * the dms may expect a CC_RELEASE_CONF
                   * before the call is actually cleared, it will not
                   * do the following line of code. ===
                   */
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
                     CCB->PriState = CCPCC_CallConnected;
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
               CCB->PriState = CCPCC_CallRoutingOut;
               }
            }
         break;

      case CALL_DISC:
         CCPMSG_DiscPeer(0);
         CCPRI_ReleaseCall();  /* make sure that the channel gets freed */
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->PriState = CCPCC_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPRI_ReleaseCall();
         CCPMSG_RejectOutCall( 0, TRUE );
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
            CCB->PriState = CCPCC_CallRoutingOut;
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
         CCPRI_Error( CCPCC_CALL_ORIGINATION, P_UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/**********************
* CCPCC_CallReleasing *
***********************
*
* Process the current event in the context of a call in the
* RELEASING (waiting for CC_RELEASE_IND or CC_RELEASE_CONF) state.
*
* This state corresponds to L3 Protocol Control States 11, 12 and 19.
*
*/
PRIVATE void CCPCC_CallReleasing( int event )
{
   if (detail_debug)
       buginf("\nCCPCC executing with event = %x in state = CALL RELEASING", event);
   switch ( event )
      {
      case CC_RELEASE_IND:
         /*
          * May have to do the following code
          * this is in the dms code ===
          *
          * if ( CCPRI_GetOutPkt( CC_REL_COMP_REQ ))
          *   CCPRI_Mail( L3_ID, FALSE );
          */
         CCPRI_ReleaseCall();
         break;

      case CC_RELEASE_CONF:
         /* We choose not to examine the message contents and
          * simply release the call.
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
         CCPRI_Error( CCPCC_CALL_RELEASING, P_UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/********************
* CCPCC_CallRinging *
*********************
*
* Process the current event in the context of a call in the RINGING state.
*
* This state corresponds to L3 Protocol Control State 7.
*
*/
PRIVATE void CCPCC_CallRinging( int event )
{
   if (detail_debug)
      buginf("\nCCPCC executing with event = %x in state = CALL RINGING", event);
   switch ( event )
      {
      case CALL_PROGRESS:
      case CALL_ACCEPT:
         /*
          * These messages require at least the private PROGRESS_IND_IE
          * since this IE is used to specify ALERTING vs CONNECT.
          */
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
                  CCB->PriState = CCPCC_CallRoutingIn;/* Expect Connect_Ack */
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
         CCB->PriState = CCPCC_CallReleasing;
         break;

      case CC_DISCONNECT_IND:
         CCPMSG_DiscHost( 0 );
         CCB->PriState = CCPCC_CallReleasing;
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
         CCPRI_Error( CCPCC_CALL_RINGING, P_UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/**********************
* CCPCC_CallRoutingIn *
***********************
*
* Process the current event in the context of a call in the ROUTING_IN
* state which is the generic busy state, but not in conversation, for the
* incoming call.  This state corresponds to Layer 3 states 8 and 9 (user).
*
* This state corresponds to L3 Protocol Control States 8 and 9.
*
*/
PRIVATE void CCPCC_CallRoutingIn( int event )
{
   BYTE *prog_ie;                  /* Pointer to prog_ind info element */

   if (detail_debug)
       buginf("\nCCPCC executing with event = %x in state = CALL ROUTING_IN", event);
   switch ( event )
      {
      case CALL_PROGRESS:
      case CALL_ACCEPT:
         /* These messages require at least the private PROGRESS_IND_IE
          * since this IE is used to specify ALERTING vs CONNECT.
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
                      * response to a SETUP.  Exclusive was established
                      * in CCPCC_CallOffered() from CHANNEL_ID_IE.
                      */
                     IntID[ 0 ] = CCPRI_MapIntId( CCB->Int_Id[ 0 ], TRUE );
                     IntID[ 1 ] = 0;
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
                  /*
                   * may have to add for the dms ===
                   *
                   * This element is not supported here as a "private."
                   * CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );
                   */
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
                  CCB->PriState = CCPCC_CallRinging;
                  }
               }
            else if ( prog_ie[ 2 ] == CALLED_ANSWER )
               {
               if ( CCPRI_GetOutPkt( CC_SETUP_RESP ))
                  {
                  if ( event == CALL_ACCEPT )
                     {
                     /* The channel id needs to be provided as the first
                      * response to a SETUP.
                      */
                     IntID[ 0 ] = CCPRI_MapIntId( CCB->Int_Id[ 0 ], TRUE );
                     IntID[ 1 ] = 0;
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
                  /*
                   * May have to add for dms ===
                   *
                   * This element is not supported here as a "private."
                   * CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );
                   */
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
                   * transit to the CONNECTED state.
                   */
                  }
               }
            }
         else
            {
            CCPRI_Error( CCPCC_CALL_IN_ROUTING, P_BAD_DATA_FROM_HOST,
                         PROGRESS_IND_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            }
         break;

      case CC_SETUP_COMP_IND:
         /* CONNECT_ACK received from the far end for a peer-initiated call.
          */
         CCB->PriState = CCPCC_CallConnected;
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
         CCB->PriState = CCPCC_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->PriState = CCPCC_CallReleasing;
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
         CCPRI_Error( CCPCC_CALL_IN_ROUTING, P_UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/***********************
* CCPCC_CallRoutingOut *
************************
*
* Process the current event in the context of a call in the ROUTING_OUT
* state which is the generic busy state, but not in conversation, for the
* outgoing call.  This state corresponds to Layer 3 states 3 and 4 (user).
*
* This state corresponds to L3 Protocol Control States 2, 3 and 4.
*
*/
PRIVATE void CCPCC_CallRoutingOut( int event )
{
   int cause;                         /* Locally generated cause value */
   int ie_cause;                      /* Cause recovered from cause ie */
   int prog_ind;                                 /* Progress indicator */

   if (detail_debug)
       buginf("\nCCPCC executing with event = %x in state = CALL ROUTING_OUT", event);
   switch ( event )
      {
      case CC_ALERTING_IND:                    /* Called is being rung */
         /* The CHANNEL_ID_IE is prodably not present since it was probably
          *  sent across in the CALL_PROCEEDING message.  B-channel is of 
          *  no concern since it should have been negotiated already.
          *
          * It seems that AT&T just wants you to ignore Bad Channel stuff
          * and continue processing. Added at AT&T in SF
          */
         cause = CCPRI_AcceptChanId( CC_ALERTING_IND, FALSE );
         if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ) &&
             (( cause != INVALID_ELEM_CONTENTS) && (SwitchType == PRI_4ESS_STYPE)))
            {
            /* This element is not mandatory...it's OK if it's not present.
             */
            CCPMSG_RejectMsg( cause, CHANNEL_ID_IE );
            }
         else
            {
            CCPMSG_SendHostProgress( CALLED_ALERTING, FALSE, TRUE );
            }
         break;

      case CC_SETUP_CONF:
         /* CONNECT received from the far end.  This message is expected
          * for a locally-originated call.  B-channel is of no concern
          * since it should have been negotiated already. ===
          */
         if ( CCPRI_GetOutInfo( CALL_CONNECT )) {
            OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
            OutPkgInfo[ OutPkgInfoLen++ ] = 4;
            OutPkgInfo[ OutPkgInfoLen++ ] = CCB->Exclusive | 0xE1;; /* Excl */
            OutPkgInfo[ OutPkgInfoLen++ ] = CCB->Int_Id[ 0 ] | 0x80;
            OutPkgInfo[ OutPkgInfoLen++ ] = CCB->ChanType | 0x80;    /* Typ */
            OutPkgInfo[ OutPkgInfoLen++ ] = CCB->B_Chan | 0x80;     /* Chan */

            CCPRI_Mail( HOST_ID, TRUE );  /* Send CALL_CONNECT */
         }
         CCPMSG_SendHostProgress( CALLED_ANSWER, FALSE, TRUE );
         if ( CCPRI_GetOutPkt( CC_CONNECT_ACK_REQ )) /* CONNECT_ACK */
            CCPRI_Mail( L3_ID, FALSE );
         CCB->PriState = CCPCC_CallConnected;    /* Enter conversation */
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
         CCB->PriState = CCPCC_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCPMSG_DiscHost( 0 );
         CCB->PriState = CCPCC_CallReleasing;
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

      case CC_CALL_PROC_IND:
         /*
          * Ignore a Call Proceeding after get a Setup Ack,
          * TS014 tests it this way.
          */ 
         break;

      default:
         CCPRI_Error( CCPCC_CALL_OUT_ROUTING, P_UNKNOWN_EVENT, event, FALSE );
         break;
      }
}


/************************************************************************
*                   C C B   I N I T I A L I Z A T I O N                 *
*************************************************************************
*/
/*********************
* CCPCC_InitialState *
**********************
*
* Provide access to CCPRI.C for CCB initialization.
*
*/
PUBLIC CC_STATE CCPCC_InitialState()
{
   return ( CCPCC_CallIdle );
}

/**********************
* CCPCC_VerifyDiscIEs *
***********************
*
* This function returns 0 if all info elements were properly recognized.
* Otherwise, it returns the IE_ID of the FIRST unrecognized info element
* in the info buffer.
*
*/
PUBLIC BYTE CCPCC_VerifyDiscIEs (info_buffer, info_length)

   BYTE    *info_buffer;
   int      info_length;

{
   BOOLEAN recognized = TRUE;
   BYTE    current_ie = 0;
   int     current_offset;        /* pointer to a current info element */

   if (detail_debug)
       buginf ("\n Entering VerifyDiscIEs... ");
   current_offset  = CCIE_LenOf_L3_Msg_Header( info_buffer );

   while (recognized && (current_offset < info_length)) {
      current_ie = info_buffer[ current_offset ];
      switch (current_ie) {
        case CAUSE_IE:
        case PROGRESS_IND_IE:
        case DISPLAY_IE:
        case USER_USER_IE:
        if (detail_debug)
            buginf ("\n In VerifyDiscIEs in cause et al . ");
            break;
        default:
        if (detail_debug)
            buginf ("\n In VerifyDiscIEs have unrecognized. ");
            recognized = FALSE;
            break;
      }
      if (current_ie & 0x80) {
         /*
          * single octet
          */
        if (detail_debug)
            buginf ("\n In VerifyDiscIEs have unrecognized single. ");
         current_offset++;
      } else {
         /*
          * variable length octet
          */
        if (detail_debug)
            buginf ("\n In VerifyDiscIEs have unrecognized variable. ");
         current_offset++;
         current_offset += info_buffer[ current_offset ] + 1;
      }
      /*
       * If found an unrecognized ie, then want to return that
       * value. So don't update to the next one.
       */
      if (recognized)
         current_ie = info_buffer[ current_offset ];
   }

   if (detail_debug)
       buginf ("\n VerifyDiscIEs recognized is %x current is %x", recognized, current_ie);
   return( recognized ? 0 : current_ie );
}

/*************************
* CCPCC_VerifyRestartIEs *
**************************
*
* This function returns 0 if all info elements were properly recognized.
* Otherwise, it returns the IE_ID of the FIRST unrecognized info element
* in the info buffer.
*
*/
PUBLIC BYTE CCPCC_VerifyRestartIEs (info_buffer, info_length, bcount)

   BYTE    *info_buffer;
   int      info_length;
   int     *bcount;

{
   BOOLEAN recognized = TRUE;
   BYTE    current_ie = 0;
   int     current_offset;        /* pointer to a current info element */
   int     channel_count = 0; 

   if (detail_debug)
       buginf ("\n Entering VerifyRestartIEs... ");
   current_offset  = CCIE_LenOf_L3_Msg_Header( info_buffer );

   while (recognized && (current_offset < info_length)) {
      current_ie = info_buffer[ current_offset ];
      switch (current_ie) {
        case CHANNEL_ID_IE:
            channel_count++;
            break;
        case RESTART_IND_IE:
        case DISPLAY_IE:
        if (detail_debug)
            buginf ("\n In VerifyRestartIEs in cause et al . ");
            break;
        default:
        if (detail_debug)
            buginf ("\n In VerifyRestartIEs have default ");
            recognized = FALSE;
            break;
      }
      if (current_ie & 0x80) {
         /*
          * single octet
          */
        if (detail_debug)
            buginf ("\n In VerifyRestartIEs have single octet");
         current_offset++;
      } else {
         /*
          * variable length octet
          */
        if (detail_debug)
            buginf ("\n In VerifyRestartIEs have variable octet");
         current_offset++;
         current_offset += info_buffer[ current_offset ] + 1;
      }
      /*
       * If found an unrecognized ie, then want to return that
       * value. So don't update to the next one.
       */
      if (recognized)
         current_ie = info_buffer[ current_offset ];
   }

   if (detail_debug)
       buginf ("\n VerifyRestartIEs recognized is %x current is %x",
               recognized, current_ie);
   *bcount = channel_count;
   return( recognized ? 0 : current_ie );
}

#ifdef TRACING                      /* Taskview or Tracker environment */
/************************************************************************
*                  C C B   S T A T E   T R A S L A T I O N              *
*************************************************************************
*/
/*********************
* CCPCC_NumericState *
**********************
*
* Translate the state to a numeric index.  This function is only intended
* as a support for Tracker and Taskview demo and test systems.  It is
* not normally used in a working system because it is inefficient and
* consumes memory for marginal value.
*
*/
PUBLIC BYTE CCPCC_NumericState( CC_STATE ccb_state )
{
   static int call_state[] = { NULL,                        /* 0 */
                               CCPCC_CallConnected,         /* 1 */
                               CCPCC_CallIdle,              /* 2 */
                               CCPCC_CallOffered,           /* 3 */
                               CCPCC_CallOrigination,       /* 4 */
                               CCPCC_CallRinging,           /* 5 */
                               CCPCC_CallReleasing,         /* 6 */
                               CCPCC_CallRoutingIn,         /* 7 */
                               CCPCC_CallRoutingOut         /* 8 */
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
#endif         /* TRACING */
/* End of module */
