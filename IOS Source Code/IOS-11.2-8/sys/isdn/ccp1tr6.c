/* $Id: ccp1tr6.c,v 3.2 1995/11/17 17:39:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/ccp1tr6.c,v $
 *------------------------------------------------------------------
 * German primary rate
 *
 * May 1992, Ronnie B. Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccp1tr6.c,v $
 * Revision 3.2  1995/11/17  17:39:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:14:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
*************************************************************************

MODULE:       CCP1TR6.C
DESCRIPTION:  ISDN Primary Rate Interface 1TR6 Compatible Call State
              Module
VERSION:      3.21

*************************************************************************
****copyright_c**********************************************************

                        COPYRIGHT STATEMENT

                         Copyright (c) 1991
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
1TR6 PRI User-side operation (PT-to-PT).  It was developed following 
the specification in: 
                    FTZ-Richtliniensammlung 
                   Technische Forderungen an 
                      digitale Endgerate 
                      mit So-Schnitstelle 
                         Band III 
                           1TR6 
                       D-Kanal-Protocol      
                           1990

The functionality in this module supports basic Call Setup and Teardown
as illustrated in the 1TR21 PRI certification scripts.
  

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

PRIVATE    void             CCP1TR6_CallActive          ARGS(( int  ));
PRIVATE    void             CCP1TR6_CallAlerting        ARGS(( int  ));
PRIVATE    void             CCP1TR6_CallDialing         ARGS(( int  ));
PRIVATE    void             CCP1TR6_CallIdle            ARGS(( int  ));
PRIVATE    void             CCP1TR6_CallOffered         ARGS(( int  ));
PRIVATE    void             CCP1TR6_CallOriginate       ARGS(( int  ));
PRIVATE    void             CCP1TR6_CallOverlapReceiving ARGS(( int ));
PRIVATE    void             CCP1TR6_CallRegMaint        ARGS(( int  )); 
PRIVATE    void             CCP1TR6_CallReleasing       ARGS(( int )); 
PRIVATE    void             CCP1TR6_CallRoutingIn       ARGS(( int  ));
PRIVATE    void             CCP1TR6_CallRoutingOut      ARGS(( int  ));
PRIVATE    BOOLEAN  near    CCP1TR6_FacilityRequest     ARGS(( int ));
PRIVATE    int      near    CCP1TR6_GetCause            ARGS(( int *, BOOLEAN ));
PRIVATE    int      near    CCP1TR6_HostRegisterRequest ARGS(( int  ));
PRIVATE    BOOLEAN  near    CCP1TR6_InCall              ARGS(( void ));
PRIVATE    BOOLEAN          CCP1TR6_OutCall             ARGS(( void ));

#ifdef TRACING
/* NOTE: If a state is added or deleted, CCP1TR6_NumericState() should be
*        updated.
*/

#endif
/************************************************************************
*                    D A T A   A L L O C A T I O N                      *
*************************************************************************
*/


/************************************************************************
*                C A L L   S T A T E   F U N C T I O N S                *
*************************************************************************
*/

/*************************
* CCP1TR6_CallActive     *
**************************
*
* Process the current event in the context of a voice call in the
* CONNECTED state. Corresponds to state T10 of 1TR6
*
*/
PRIVATE void  CCP1TR6_CallActive( event )

int event;                                 /* Event input to the state */

{
   int  recov_cause = 0;                    /* cause from Q931 message */
   int cause = 0;                     /* Locally generated cause value */
   BYTE facilities;                      /* NSF retrieved from message */
   int j;
   int IE_index; 
   switch ( event )
      {

      case CALL_DISC:
         if ( CCP1TR6_GetCause(&recov_cause, FALSE) == ACCEPTED)   
            CCPMSG_DiscPeer( recov_cause );
         else
            CCPMSG_DiscPeer ( LEER_ELEMENT );

         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CALL_FACILITY:
         /* process the Facility message from the HOST.
          */
          CCP1TR6_FacilityRequest( event );
          break;


      case CALL_FACILITY_ACK:
         /* process the Facility Ack from the HOST.
         */
         /* no change in state.
         */
         CCP1TR6_FacilityRequest( event );
         break;

      case CALL_FACILITY_REJ: 
         /* no change in state.
         */
         /* process the Facility Rej from the HOST.
         */
         CCP1TR6_FacilityRequest( event );
         break;

      case CC_FACILITY_ACK:
         CCPMSG_FacAckIn();
         break; 

      case CC_FACILITY_REJ:
         CCPMSG_FacRejIn();
         break; 

   /* Must be SPV */
      case CALL_INFO:
         if ( CCPRI_GetOutPkt( CC_INFO_REQ ))
            {
#ifdef EXTENDED_HOST_MSG
            if ( Host_IEs_Len == 0 )
               {
               CCPRI_CopyPublicAll();
               }
#endif
            CCPRI_Mail( L3_ID, FALSE );
            }
         break;

      case CC_FACILITY_IND:
         if ( CCPRI_GetOutInfo( CALL_FACILITY ))
            {
            if (( cause = CCPRI_AcceptFacilities (&facilities, FALSE, TRUE )) == ACCEPTED )       
               {      
               OutPkgInfo[ OutPkgInfoLen++ ] =  FACILITY_SELECT_IE;
               j =  OutPkgInfoLen++; 
               OutPkgInfo[ OutPkgInfoLen++ ] = IE[3];      /* Fac Code */
               OutPkgInfo[ OutPkgInfoLen++ ] = IE[4];       /* Service */
               OutPkgInfo[ OutPkgInfoLen++ ] = IE[5]; /* additional info */
               IE_index = 6;
               while (IE_index < (IE[1] +2 ) )   /* Pick up Parafelder */
               OutPkgInfo[ OutPkgInfoLen++] = IE[ IE_index++ ];
               CCPRI_Mail( HOST_ID, TRUE ); 
               }
            else 
               {
               CCPRI_FreePkgInfo();
               if ( CCPRI_GetOutPkt( CC_FAC_REJ_REQ ))
                 {
                 Diag[ 0 ] = NO_DIAG;
                 OutPkt->InfoLen +=
                    CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                cause,
                                AM_I_THE_NETWORK_SIDE,
                                Diag );
                 if ( cause != MANDATORY_IE_MISSING )
                    {
                    OutPkt->Info[ OutPkt->InfoLen++ ] =  FACILITY_SELECT_IE;
                    j =  OutPkt->InfoLen++; 
                    OutPkt->Info[ OutPkt->InfoLen++ ] = IE[3]; /* Fac Code */
                    OutPkt->Info[ OutPkt->InfoLen++ ] = IE[4]; /* Service */
                    OutPkt->Info[ j ] = 2;
                    }
                 CCPRI_Mail( L3_ID, FALSE );
                 } 
               }
            }
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_DiscHost( recov_cause );
         CCB->State = CCP1TR6_CallReleasing;

         break;

      case CC_STATUS_IND:
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_SendHostDisc( recov_cause, FALSE );
         CCPRI_ReleaseCall();
         break;

      case CC_INFO_IND:
            if ( CCPRI_GetOutInfo( CALL_INFO ) )
               {
               CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
               }
         break;

      default:
         CCPRI_Error( CCP1TR6_CALL_ACTIVE, UNKNOWN_EVENT, event, FALSE );
         break;
      }  

}

/***********************
* CCP1TR6_CallAlerting *
************************
*
* Process the current event in the context of a voice call in the
* RINGING state.  This terminal is being rung.	This corresponds to
* state T7 of 1TR6 
*
*/
PRIVATE void  CCP1TR6_CallAlerting( event )

int event;                                /* Event input to the state */

{
   int  recov_cause = 0;                    /* cause from Q931 message */
   int cause = 0;                     /* Locally generated cause value */

   switch ( event )
      {
      case CALL_PROGRESS:
         if (( IE = CCPRI_GetPrivate( PROGRESS_IND_IE, CODESET_0 )) != NULL )
            {
            if ( IE[ 2 ] == CALLED_ANSWER )
               {
               if ( CCPRI_GetOutPkt( CC_SETUP_RESP ))     /* "Connect" */
                  {
#ifdef EXTENDED_HOST_MSG
                  /* This element is not supported here as a "private."
                  */
                  CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );
#endif
                  CCPRI_Mail( L3_ID, FALSE );
                  CCB->State = CCP1TR6_CallRoutingIn; /* Expect Connect_Ack */
                  }
               }
            }
         else
            {
            CCPRI_Error( CCP1TR6_CALL_ALERTING, BAD_DATA_FROM_HOST,
                         PROGRESS_IND_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            }
         break;

      case CALL_FACILITY:
         /* process the Facility message from the HOST.
          */
          CCP1TR6_FacilityRequest( event );
          break;

      case CALL_DISC:
         if ( CCP1TR6_GetCause(&recov_cause, FALSE) == ACCEPTED)   
            CCPMSG_DiscPeer( recov_cause );
         else
            CCPMSG_DiscPeer ( LEER_ELEMENT );

         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CALL_INFO:
         if ( CCPRI_GetOutPkt( CC_INFO_REQ ))
            {
#ifdef EXTENDED_HOST_MSG
            if ( Host_IEs_Len == 0 )
               {
               CCPRI_CopyPublicAll();
               }
#endif
            CCPRI_Mail( L3_ID, FALSE );
            }
         break;

      case CC_DISCONNECT_IND:
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_DiscHost( recov_cause );
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_SendHostDisc( recov_cause, FALSE );
         CCPRI_ReleaseCall();
         break;

      case CC_SETUP_IND:             /* Can arrive as a race condition */
         break;

      case CC_STATUS_IND:
         break;


      case CC_FACILITY_REJ:
         CCPMSG_FacRejIn();
         break;

      case CC_FACILITY_ACK:
         CCPMSG_FacAckIn();
         break;
 
      default:

         CCPRI_Error( CCP1TR6_CALL_ALERTING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}
/**********************
* CCP1TR6_CallDialing *
***********************
*
* Process the current event in the context of a voice call in the
* DIALING state. Process overlap sending.  This is state T2 of 1TR6.
*
*/
PRIVATE void  CCP1TR6_CallDialing( event )

int event;                                /* Event input to the state */

{
   int  recov_cause = 0;                    /* cause from Q931 message */
   int cause = 0;                     /* Locally generated cause value */


   switch ( event )
      {

      case CALL_DISC:
         if ( CCP1TR6_GetCause(&recov_cause, FALSE) == ACCEPTED)   
            CCPMSG_DiscPeer( recov_cause );
         else
            CCPMSG_DiscPeer ( LEER_ELEMENT );

         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CALL_INFO:
         if ( CCPRI_GetOutPkt( CC_INFO_REQ ))
            {
#ifdef EXTENDED_HOST_MSG
            if ( Host_IEs_Len == 0 )
               {
               CCPRI_CopyPublicAll();
               }
#endif
            CCPRI_Mail( L3_ID, FALSE );
            }
         break;

      case CC_INFO_IND:
         if ( CCPRI_GetOutInfo( CALL_INFO ) )
            {
            CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
            }
         break;

      case CC_ALERTING_IND:
         CCPMSG_SendHostProgress( CALLED_ALERTING, FALSE, FALSE);   
         CCB->State =  CCP1TR6_CallRoutingOut; 
         break;

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
                  CCPMSG_RejectMsg( CALL_REJECTED_1TR6, 0 );
                  CCB->State = CCP1TR6_CallReleasing;
                  }
               else
                  {
                  CCPMSG_RejectMsg( cause, CHANNEL_ID_IE );
                  CCB->State = CCP1TR6_CallReleasing;
                  }
               /* We expect the network to clear the call as a follow-up.
               */
               CCPRI_FreePkgInfo();
               }
            else
               {
               CCPRI_Mail( HOST_ID, TRUE );        /* Send CALL_ACCEPT */
               CCB->State = CCP1TR6_CallRoutingOut;
               }
            }
         break; 

      case CC_DISCONNECT_IND:
         /* Far end disconnect.  Send CC_RELEASE_REQ and go to the
         *  state RELEASING since we are still off-hook here.
         */
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_DiscHost( recov_cause );
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_SETUP_CONF:                    /* Translation of CONNECT */
         /* CONNECT received from the far end.  This message is expected
         *  for a locally-originated call.
         */
         cause = CCPRI_AcceptChanId( CC_SETUP_CONF, FALSE );
         if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
            {
            /* This element is not mandatory...it's OK if it's not present.
            */
            CCPMSG_RejectMsg( cause, CHANNEL_ID_IE );/* Elem coding err */
            }
         else
            {
            CCPMSG_SendHostProgress( CALLED_ANSWER, FALSE, TRUE );
            }
         CCB->State = CCP1TR6_CallActive;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      default:
         CCPRI_Error( CCP1TR6_CALL_DIALING, UNKNOWN_EVENT, event, FALSE );
         break;
      }

}

/*******************
* CCP1TR6_CallIdle *
********************
*
* Process the current event in the context of a voice call in the
* IDLE state.
*
*/
PRIVATE void  CCP1TR6_CallIdle( event )

int event;                                /* Event input to the state */

{

   switch ( event )
      {
      case CALL_OUTGOING:
         /* Outgoing call request by the host.  This complex function
         *  is implemented independently.
         */
         if ( CCP1TR6_OutCall() )
            CCB->State = CCP1TR6_CallOriginate;
         else
            CCPRI_ReleaseCall();
         break;

      case CALL_FACILITY_REGISTER:
         CCP1TR6_HostRegisterRequest ( CALL_FACILITY_REGISTER );
         break;

      case CALL_FACILITY_CANCEL:
         CCP1TR6_HostRegisterRequest ( CALL_FACILITY_CANCEL );
         break;

      case CC_SETUP_IND:
         if ( CCP1TR6_InCall() )
            CCB->State = CCP1TR6_CallOffered;
         break;

      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
         CCPRI_ReleaseCall();
         break;

      default:
         CCPRI_Error( CCP1TR6_CALL_IDLE, UNKNOWN_EVENT, event, FALSE );
         CCPRI_ReleaseCall();
         break;
      }

}
/**********************
* CCP1TR6_CallOffered *
***********************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to T6 of 1TR6.
*
*/
PRIVATE void  CCP1TR6_CallOffered( event )

int event;                                /* Event input to the state */

{
   int  recov_cause = 0;                    /* cause from Q931 message */
   int cause = 0;                     /* Locally generated cause value */

   switch ( event )
      {
      case CALL_ACCEPT:
         /* Prepare to build mandatory CHANNEL ID element required in the
         *  1st response to a SETUP by recovering info sent by the host.
         *  This response is equivalent to CALL_PROCEEDING.
         */
         if ( CCPMSG_GetHostChannel() == NOERR )
            {
            if ( CCPRI_GetOutPkt( CC_BROADCAST_RESP ))
               {
               CCPRI_Mail( L3_ID, FALSE );      /* Send CC_BROADCAST_RESP */
               }
            else
               {
               /* Cannot send a message to L3.
               */
               CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
               CCPRI_ReleaseCall();
               }
            if ( CCPRI_GetOutPkt( CC_CALL_PROC_REQ ))
               {
                OutPkt->InfoLen +=
                    CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                         CHANNELS_LISTED,
                                         CCB->B_Chan, 
                                         B_CHANNEL_UNITS,
                                         FALSE,
                                         CCB->Exclusive,   /* Exclusive */
                                         CCB->Int_Id_Len,/* Intrface ID len */
                                         CCB->Int_Id );  /* Intrface ID ptr */
               CCPRI_Mail( L3_ID, FALSE );      /* Send CC_CALL_PROC */
               CCB->State = CCP1TR6_CallRoutingIn; 
               }
            } 
         else
            {
            /* Cannot send a message to L3.
             */
            CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
            CCPRI_ReleaseCall();
            } 
         break;

      case CALL_PROGRESS:       /* Setup Ack - go to Overlap Receiving */
         if (( IE = CCPRI_GetPrivate ( PROGRESS_IND_IE, CODESET_0 )) != NULL)
            {
            if ( IE[2] == MORE_DIGITS ) 
               {
               if ( CCPMSG_GetHostChannel() == NOERR )
                  {
                  if ( CCPRI_GetOutPkt( CC_BROADCAST_RESP ))
                     {
                     CCPRI_Mail( L3_ID, FALSE );    /* Send CC_BROADCAST_RESP */
                     if ( CCPRI_GetOutPkt( CC_MORE_INFO_REQ ))
                        {
                        OutPkt->InfoLen +=
                            CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                             CHANNELS_LISTED,
                                             CCB->B_Chan, 
                                             B_CHANNEL_UNITS,
                                             FALSE,
                                             CCB->Exclusive,   /* Exclusive */
                                             CCB->Int_Id_Len,/* Intrface ID len */
                                             CCB->Int_Id );  /* Intrface ID ptr */
                        CCPRI_Mail( L3_ID, FALSE ); /* Send CC_MORE_INFO_REQ */
                        CCB->State = CCP1TR6_CallOverlapReceiving; 
                        }
                     else
                        {
                        CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
                        CCPRI_ReleaseCall();
                        }  
                     }
                  else
                     {
                     CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
                     CCPRI_ReleaseCall();
                     } 
                  } 
               else 
                  {
                  CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
                  CCPRI_ReleaseCall();
                  }
               }
            else if ( IE[2] == CALLED_ALERTING )
               {
               if ( CCPMSG_GetHostChannel() == NOERR )
                  {
                  if ( CCPRI_GetOutPkt( CC_BROADCAST_RESP ))
                     {
                     CCPRI_Mail( L3_ID, FALSE );    /* Send CC_BROADCAST_RESP */
                     if ( CCPRI_GetOutPkt( CC_ALERTING_REQ  ))
                        {
                        OutPkt->InfoLen +=
                            CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                             CHANNELS_LISTED,
                                             CCB->B_Chan, 
                                             B_CHANNEL_UNITS,
                                             FALSE,
                                             CCB->Exclusive,   /* Exclusive */
                                             CCB->Int_Id_Len,/* Intrface ID len */
                                             CCB->Int_Id );  /* Intrface ID ptr */
                        CCPRI_Mail( L3_ID, FALSE ); /* Send CC_MORE_INFO_REQ */
                        CCB->State = CCP1TR6_CallAlerting; 
                        }
                     else
                        {
                        CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
                        CCPRI_ReleaseCall();
                        }  
                     }
                  else
                     {
                     CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
                     CCPRI_ReleaseCall();
                     } 
                  } 
               else 
                  {
                  CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
                  CCPRI_ReleaseCall();
                  }
               }
            else
               {
               CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
               CCPRI_ReleaseCall();
               }
            }
         else 
            {
            CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );/* Cause 41 */
            CCPRI_ReleaseCall();
            }  
         break;

      case CALL_REJECTION:
         CCPMSG_RejectInCall( 0, 0 );
         CCPRI_ReleaseCall();
         break;


      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_SendHostDisc( recov_cause, FALSE );
         CCPRI_ReleaseCall();
         break;

      case CC_STATUS_IND:
         break;

      default:
         CCPRI_Error( CCP1TR6_CALL_OFFERED, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}
/*******************************
* CCP1TR6_CallOverlapReceiving *
********************************
*
* Process the current event in the context of a voice call in the
* Overlap Receiving State. This corresponds to T25 of 1TR6.  Our application
* has sent the Channel on the Setup Ack message.
*/
PRIVATE void  CCP1TR6_CallOverlapReceiving( event )

int event;                                /* Event input to the state */

{
   int  recov_cause = 0;                    /* cause from Q931 message */

   switch ( event )
      {
      case CALL_ACCEPT:
         if ( CCPRI_GetOutPkt( CC_CALL_PROC_REQ ) )
            {
            CCPRI_Mail( L3_ID, FALSE );              /* Send CALL_PROC */
            CCB->State = CCP1TR6_CallRoutingIn;
            }
         break;

      case CALL_INFO:
         if ( CCPRI_GetOutPkt( CC_INFO_REQ ))
            {
#ifdef EXTENDED_HOST_MSG
            if ( Host_IEs_Len == 0 )
               {
               CCPRI_CopyPublicAll();
               }
#endif
            CCPRI_Mail( L3_ID, FALSE );
            }
         break;

      case CALL_PROGRESS:
         if (( IE = CCPRI_GetPrivate( PROGRESS_IND_IE, CODESET_0 )) != NULL )
            {
            if ( IE[ 2 ] == CALLED_ANSWER )
               {
               if ( CCPRI_GetOutPkt( CC_SETUP_RESP ))     /* "Connect" */
                  {
#ifdef EXTENDED_HOST_MSG
                  /* This element is not supported here as a "private."
                  */
                  CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );
#endif

                  CCPRI_Mail( L3_ID, FALSE );
                  CCB->State = CCP1TR6_CallRoutingIn; /* Expect Connect_Ack */
                  }
               }
            else if ( IE[ 2 ] == CALLED_ALERTING )
               {
               if ( CCPRI_GetOutPkt( CC_ALERTING_REQ ))       /* Alert */
                  {
                  CCPRI_Mail( L3_ID, FALSE );
                  CCB->State = CCP1TR6_CallAlerting; 
                  }
               }
            else 
               {
               CCPRI_Error( CCP1TR6_CALL_ALERTING, BAD_DATA_FROM_HOST,
                         PROGRESS_IND_IE, FALSE );
               CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
               }
            }
         else
            {
            CCPRI_Error( CCP1TR6_CALL_ALERTING, BAD_DATA_FROM_HOST,
                         PROGRESS_IND_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            }
         break;

      case CALL_DISC:
         if ( CCP1TR6_GetCause(&recov_cause, FALSE) == ACCEPTED)   
            CCPMSG_DiscPeer( recov_cause );
         else
            CCPMSG_DiscPeer ( LEER_ELEMENT );
         CCB->State = CCP1TR6_CallReleasing;
         break;


      case CC_INFO_IND:
         if ( CCPRI_GetOutInfo( CALL_INFO ))
            {
            CCPRI_Mail( HOST_ID, TRUE );  /* Relay the result to the HOST */
            }
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      default:
         CCPRI_Error( CCP1TR6_CALL_OVERLAP_RECEIVING, UNKNOWN_EVENT, event, FALSE );
         break;
      }

}


/************************
* CCP1TR6_CallReleasing *
*************************
*
* Process the current event in the context of a call in the
* RELEASING (waiting for CC_RELEASE_IND or CC_RELEASE_CONF) state.
*
* This state corresponds to L3 Protocol Control States T11, T12 and T19.
*
*/
PRIVATE void  CCP1TR6_CallReleasing( event )

int event;                                /* Event input to the state */

{
#ifdef DEBUG
   printf( "CCP1TR6 executing with event = %x in state = CALL RELEASING\n", event );
#endif

   switch ( event )
      {

      case CALL_FACILITY:
         /* process the Facility message from the HOST.
          */
          CCPMSG_FacReqOut();
          break;

      case CC_RELEASE_IND:
/*         if ( CCPRI_GetOutPkt( CC_REL_COMP_REQ ))
            CCPRI_Mail( L3_ID, FALSE ); */
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

      case CC_INFO_IND:
         if ( CCPRI_GetOutInfo( CALL_INFO ) )
            {
            CCPRI_Mail( HOST_ID, TRUE );
            }
         break;

      case CC_STATUS_IND:
         break;

      case CC_FACILITY_REJ: 
         /* no change in state.
         */
         /* process the Facility Rej from the Network.
         */
         CCPMSG_FacRejIn();
         break;

      case CC_FACILITY_ACK:
         CCPMSG_FacAckIn();
         break; 

      default:
         CCPRI_Error( CCP1TR6_CALL_RELEASING, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}

/************************
* CCP1TR6_CallOriginate *
*************************
*
* Process the current event in the context of a voice call in the
* ORIGINATE state awaiting the first response to CC_SETUP_REQ.	Corresponds
* to state T1 of 1TR6
*
*/
PRIVATE void  CCP1TR6_CallOriginate( event )

int event;                                /* Event input to the state */

{
   int  recov_cause = 0;                    /* cause from Q931 message */
   int cause = 0;                     /* Locally generated cause value */


#ifdef DEBUG
   printf( "CCP1TR6 executing with event = %x in state = CALL ORIGINATION\n", event );
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
               /* We expect the network to send RELEASE_ACK
                */
               CCPMSG_RejectMsg( CALL_REJECTED_1TR6, 0 );
               CCPRI_FreePkgInfo();
               }
            else
               {
               CCPRI_Mail( HOST_ID, TRUE );        /* Send CALL_ACCEPT */
               CCB->State = CCP1TR6_CallRoutingOut;
               }
            }
         break;

      case CALL_DISC:                               /* Host disconnect */
         if ( CCP1TR6_GetCause(&recov_cause, FALSE) == ACCEPTED)   
            CCPMSG_DiscPeer( recov_cause );
         else
            CCPMSG_DiscPeer ( LEER_ELEMENT );
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_DiscHost( recov_cause );
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         CCPMSG_RejectOutCall( 0, TRUE );
         CCPRI_ReleaseCall();
         break;

      case CC_STATUS_IND:
         break;

      case CC_MORE_INFO_IND:               /* Translation of SETUP_ACK */
         /* This is the first response to SETUP request message sent
         *  in IDLE state.  This message will cause change of state to
         *  DIALING state for overlap sending.  DIAL TONE will be sent by
         *  the NETWORK on the selected B-Channel.  We will connect to it.
         *
         *  NOTE: The channel ID info element is a mandatory info element
         *  here. Check for it first:
         */
         if ( CCPRI_GetOutInfo( CALL_PROGRESS ))
            {
            /* Receive final B-Chan choice or reject the message.  Note
            *  that it is essential to return the channel to the HOST
            *  now since the channel the network selects may not be the
            *  same channel the user proposed in the SETUP.  The HOST
            *  should connect to this channel when the CALL_ACCEPT
            *  message is received.
            */
            if (( cause = CCPRI_AcceptChanId( CC_MORE_INFO_IND,
                                              TRUE )) != ACCEPTED )
               {
               /* We expect the network to send RELEASE_ACK
                */
               CCPMSG_RejectMsg( CALL_REJECTED_1TR6, 0 );
               CCPRI_FreePkgInfo();
               }
            else
               {
               OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;
               OutPkgInfo[ OutPkgInfoLen++ ] = MORE_DIGITS;
               CCPRI_Mail( HOST_ID, TRUE );          /* Send CALL_PROG */
               CCB->State = CCP1TR6_CallDialing;
               }
            }
         break;

      case CC_SETUP_CONF:               
            if (( cause = CCPRI_AcceptChanId( CC_SETUP_CONF,
                                              TRUE )) != ACCEPTED )
               {
               /* The SETUP timed out
                */
               CCPMSG_DiscHost( (int) TIMEOUT_1TR6  );
               CCPRI_FreePkgInfo();
               }   
         break; 

      default:
         CCPRI_Error( CCP1TR6_CALL_ORIGINATION, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}
/************************
* CCP1TR6_CallRoutingIn *
*************************
*
* Process the current event in the context of a call in the ROUTING_IN
* state which is the generic busy state, but not in conversation, for the
* incoming call.  This state corresponds to Layer 3 states T8 and T9 (user).
*
*/
PRIVATE void CCP1TR6_CallRoutingIn( event )

int event;                                 /* Event input to the state */

{
   BYTE *prog_ie;                  /* Pointer to prog_ind info element */
   int  recov_cause = 0;                    /* cause from Q931 message */
   int cause = 0;                     /* Locally generated cause value */


#ifdef DEBUG
   printf( "CCP1TR6 executing with event = %x in state = CALL ROUTING_IN\n", event );
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
                  CCPRI_Mail( L3_ID, FALSE );
                  CCB->State = CCP1TR6_CallAlerting;
                  }
               }
            else if ( prog_ie[ 2 ] == CALLED_ANSWER )
               {
               if ( CCPRI_GetOutPkt( CC_SETUP_RESP ))     /* "Connect" */
                  {
                  CCPRI_Mail( L3_ID, FALSE );
                  /* Expect CONNECT_ACK (CC_SETUP_COMP_IND) before we
                  *  transit to the CONNECTED state.
                  */
                  }
               }
            }
         else
            {
            CCPRI_Error( CCP1TR6_CALL_ROUTING_IN, BAD_DATA_FROM_HOST,
                         PROGRESS_IND_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            }
         break;

      case CALL_INFO:
         if ( CCPRI_GetOutPkt( CC_INFO_REQ ))
            {
#ifdef EXTENDED_HOST_MSG
            if ( Host_IEs_Len == 0 )
               {
               CCPRI_CopyPublicAll();
               }
#endif
            CCPRI_Mail( L3_ID, FALSE );
            }
         break;

      case CC_SETUP_COMP_IND:
         /* CONNECT_ACK received from the far end for a peer-initiated call.
         */
         CCB->State = CCP1TR6_CallActive;
         CCPMSG_SendHostProgress( CALL_CONNECTED, FALSE, FALSE );
         break;

      case CALL_FACILITY:
         /* process the Facility message from the HOST.
          */
          CCPMSG_FacReqOut();
          break;

      case CALL_DISC:
         if ( CCP1TR6_GetCause(&recov_cause, FALSE) == ACCEPTED)   
            CCPMSG_DiscPeer( recov_cause );
         else
            CCPMSG_DiscPeer ( LEER_ELEMENT );

         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_FACILITY_REJ: 
         /* no change in state.
         */
         /* process the Facility Rej from the Network.
         */
         CCPMSG_FacRejIn();
         break;

      case CC_FACILITY_ACK:
         CCPMSG_FacAckIn();
         break; 

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_DiscHost( recov_cause );
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_SendHostDisc( recov_cause, FALSE );
         CCPRI_ReleaseCall();
         break;

      case CC_STATUS_IND:
         break;

      case CC_SETUP_IND:             /* Can arrive as a race condition */
         break;

      default:
         CCPRI_Error( CCP1TR6_CALL_ROUTING_IN, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}
/*************************
* CCP1TR6_CallRegMaint   *
**************************
*
* Process the current event in the context of Registration Maintanance.
* This corresponds to States T20 and T21 of 1TR6.
* The states have been consolidated - L3 should reject an inappropriate
* message.
*
*/
PRIVATE void  CCP1TR6_CallRegMaint( event )

int event;                                /* Event input to the state */

{
   int recov_cause = 0;
   int cause = 0;

   switch ( event )
      {

      case CALL_DISC:
         if ( CCP1TR6_GetCause(&recov_cause, FALSE) == ACCEPTED)   
            CCPMSG_DiscPeer( recov_cause );
         else
            CCPMSG_DiscPeer ( LEER_ELEMENT );
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_REGISTER_ACK:
         if ( CCPRI_GetOutInfo( CALL_REGISTER_ACK ))
            {
            CCPRI_GetFacilities ( FALSE ); 
            CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
            CCB->DM_Substate = FALSE;
            CCB->State = CCP1TR6_CallIdle;
            }
         break; 

      case CC_REGISTER_REJ:
         if ( CCPRI_GetOutInfo( CALL_REGISTER_REJ ))
            {
            if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
               recov_cause = cause; 
            OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
            OutPkgInfo[ OutPkgInfoLen++ ] = 1;
            OutPkgInfo[ OutPkgInfoLen++ ] = recov_cause;
            CCPRI_GetFacilities ( FALSE ); 
            CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
            CCB->DM_Substate = FALSE;
            CCB->State = CCP1TR6_CallIdle;
            }
         break; 

      case CC_CANCEL_ACK:
         if ( CCPRI_GetOutInfo( CALL_CANCEL_ACK ))
            {
            CCPRI_GetFacilities ( FALSE ); 
            CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
            CCB->DM_Substate = FALSE;
            CCB->State = CCP1TR6_CallIdle;
            }
         break; 

      case CC_CANCEL_REJ:
         if ( CCPRI_GetOutInfo( CALL_CANCEL_REJ ))
            {
            if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
               recov_cause = cause; 
            OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
            OutPkgInfo[ OutPkgInfoLen++ ] = 1;
            OutPkgInfo[ OutPkgInfoLen++ ] = recov_cause;
            CCPRI_GetFacilities ( FALSE ); 
            CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
            CCB->DM_Substate = FALSE;
            CCB->State = CCP1TR6_CallIdle;
            }
         break; 

      default:
         CCPRI_Error( CCP1TR6_CALL_REG, UNKNOWN_EVENT, event, FALSE );
         break;
      }

}

/*************************
* CCP1TR6_CallRoutingOut *
**************************
*
* Process the current event in the context of a voice call in the
* ROUTING state, the general purpose engaged, but non-CONVERSATION, state.
* This state is typically associated with the presentation of some
* informational tone such as ringback or some other call progress tone
* while the user is standing by. Corresponds to states T3 and T4 of 1TR6
*
*/
PRIVATE void  CCP1TR6_CallRoutingOut( event )

int event;                                /* Event input to the state */

{
   int  cause = 0; 
   int  recov_cause = 0;                    /* cause from Q931 message */

#ifdef DEBUG
   printf( "CCP1TR6 executing with event = %x in state = CALL ROUTING_OUT\n", event );
#endif

   switch ( event )
      {
      case CC_ALERTING_IND:                    /* Called is being rung */
         CCPMSG_SendHostProgress( CALLED_ALERTING, FALSE, FALSE);   
         CCB->State =  CCP1TR6_CallRoutingOut; 
         break;

      case CC_SETUP_CONF:
         /* CONNECT received from the far end.  This message is expected
         *  for a locally-originated call.
         */
         cause = CCPRI_AcceptChanId( CC_SETUP_CONF, FALSE );
         if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
            {
            /* This element is not mandatory...it's OK if it's not present.
            */
            CCPMSG_RejectMsg( cause, CHANNEL_ID_IE );/* Elem coding err */
            }
         else
            {
            CCPMSG_SendHostProgress( CALLED_ANSWER, FALSE, TRUE );
            }
        
         CCB->State = CCP1TR6_CallActive;
         break;

      case CALL_DISC:
         if ( CCP1TR6_GetCause(&recov_cause, FALSE) == ACCEPTED)   
            CCPMSG_DiscPeer( recov_cause );
         else
            CCPMSG_DiscPeer ( LEER_ELEMENT );
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_DISCONNECT_IND:                    /* Network disconnect */
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_DiscHost( recov_cause );
         CCB->State = CCP1TR6_CallReleasing;
         break;

      case CC_RELEASE_IND:
         CCPMSG_ReleaseInd();
         break;

      case CC_RELEASE_CONF:
         if (( cause =  CCP1TR6_GetCause(&recov_cause, TRUE)) != ACCEPTED )
            recov_cause = cause; 
         CCPMSG_SendHostDisc( recov_cause, FALSE );
         CCPRI_ReleaseCall();
         break;

      case CC_STATUS_IND:
         break;

      case CC_INFO_IND:
         if ( CCPRI_GetOutInfo( CALL_INFO ) )
            {
            CCPRI_Mail( HOST_ID, TRUE );
            }
         break;

      default:
         CCPRI_Error( CCP1TR6_CALL_ROUTING_OUT, UNKNOWN_EVENT, event, FALSE );
         break;
      }
}
/************************
CCP1TR6_FacilityRequest *
*************************
*
* Process a Facility Request from HOST.
*
*/
PRIVATE    BOOLEAN  near    CCP1TR6_FacilityRequest( event )
int  event;
{
   BYTE    msg_type;
   BYTE    facilities;
   BOOLEAN ack = FALSE; 
   int     cause = 0;
   

   if (( CCB->DM_Substate == DM_REQUEST_OUTSTANDING ) && (event == CALL_FACILITY ))
     {
     CCPMSG_RejectMsg( cause, NETWORK_FACILITY_IE );
     goto bailout;
     }

   if (( CCB->DM_Substate != DM_REQUEST_OUTSTANDING) && ((event == CALL_FACILITY_ACK) ||
                                                      (event == CALL_FACILITY_REJ ))) 
     {
     CCPMSG_RejectMsg( cause, NETWORK_FACILITY_IE );
     goto bailout;
     }

   if ( event == CALL_FACILITY ) 
      msg_type = CC_FACILITY_REQ; 
   else  if ( event == CALL_FACILITY_ACK )
      {
      msg_type = CC_FACILITY_ACK; 
      ack = TRUE;
      }
   else if ( event == CALL_FACILITY_REJ )
      {
      msg_type = CC_FACILITY_REJ;
      ack = TRUE;
      }

   if  ( CCPRI_GetOutPkt( msg_type ) )
      {
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {
         if (((  CCPRI_AcceptFacilities( &facilities, TRUE, TRUE )) == NOERR ) &&
             ( IE ) )
            {
            CCIE_BuildNetFac_IE_1TR6( &OutPkt->Info[ OutPkt->InfoLen ],
                      facilities,
                      ((ack) ? TRUE : FALSE),
                      IE ); 
            }
         else
            {
         /* Reject call - Facility Code was invalid
         */
            CCPMSG_RejectMsg( INVALID_ELEM_CONTENTS, NETWORK_FACILITY_IE );
            goto bailout;
            }
         }
      CCB->DM_Substate = DM_REQUEST_OUTSTANDING;
      CCPRI_Mail( L3_ID, FALSE );
      }

bailout:
  return; 
}
/******************************
* CCP1TR6_HostRegisterRequest *
*******************************
*
* Registration Maintenance activities are initiated here.
*
*/
PRIVATE  int near  CCP1TR6_HostRegisterRequest ( event )

int  event;
{
   int     cause = NOERR;
   BYTE    msg_type;
   BYTE    facilities;

   if (event == CALL_FACILITY_REGISTER )
      msg_type = CC_REGISTER_REQ; 
   else if (event == CALL_FACILITY_CANCEL )
      msg_type = CC_FAC_CANCEL_REQ; 

   if ((cause =  CCPRI_AcceptFacilities ( &facilities, TRUE, TRUE )) == NOERR ) 
      {
      if ( CCPRI_GetOutPkt( msg_type ))
         {
         CCIE_BuildNetFac_IE_1TR6 ( &OutPkt->Info[ OutPkt->InfoLen], facilities, FALSE, IE); 
         }
      else
         {
         CCPMSG_RejectMsg( TEMPORARY_FAILURE, 0 );
         goto bailout;
         }  
      }
   else
      {
      /* Reject call - Facility Code was invalid
       */
      CCPMSG_RejectMsg( cause, NETWORK_FACILITY_IE );
      goto bailout;
      }

    if ( ( IE = CCPRI_GetPrivate ( ORIGINATION_ADDRESS_IE, CODESET_0 ) ) != NULL )
       {
         /* build the CCITT compatible info element.
          */
        OutPkt->InfoLen +=
            CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
	                   ORIGINATION_ADDRESS_IE,   
                      IE[ 2 ],        /* number type */
                      IE[ 3 ],        /* number plan */
                      &IE[ 4 ],             /* digits */
                      IE[ 1 ] - 2,   /* number length */
                      TRUE );        /* ASCII digits */
       }
    CCB->State = CCP1TR6_CallRegMaint; 
    CCB->DM_Substate = DM_REQUEST_OUTSTANDING;
    CCPRI_Mail( L3_ID, FALSE );
bailout:
  return cause;

}
/*******************
* CCP1TR6_InCall   *
********************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function
* because of its inherent complexity.
*
*/
PRIVATE BOOLEAN near CCP1TR6_InCall()

{
   int     j = 0;
   BYTE    service; 
   BYTE    add_info;
   BYTE    facilities;
   int     IE_index = 0;
   BOOLEAN success; 
   int     cause = 0;


   /* inform the HOST.
   */
   /* Start off by allocating an info buffer in which the HOST's
   *  CALL_INCOMING will be constructed on-the-fly.
   */
   if ( !CCPRI_GetOutInfo( CALL_INCOMING ))
      {
      CCPMSG_RejectInCall( CALL_REJECTED, 0 );
      success = FALSE;
      goto bailout;
      }

   if ((  CCPRI_AcceptServiceInd(&service, &add_info, FALSE)) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected - by ignoring
      * the message 
      */                    	
      success = FALSE; 
      CCPRI_FreePkgInfo();
      CCB->ClearHost = FALSE; 
      CCPRI_ReleaseCall(); 
      goto bailout;
      }
   else
      {
      OutPkgInfo[ OutPkgInfoLen++ ] = SERVICE_INDICATOR_IE;
      OutPkgInfo[ OutPkgInfoLen++ ] = 2;
      OutPkgInfo[ OutPkgInfoLen++ ] = service;
      OutPkgInfo[ OutPkgInfoLen++ ] = add_info;
      }

   /* Next we look at the Channel ID.
   */

   if (( cause = CCPRI_AcceptChanId( CC_SETUP_IND, TRUE )) != ACCEPTED )
      {
      CCPMSG_RejectInCall( CALL_REJECTED_1TR6, 0);  
      success = FALSE;
      CCB->State = CCP1TR6_CallReleasing;
      goto bailout;
      }

   /* Note that this is an optional element.  If provided, we verify
   *  it. If its coded incorrectly, we just skip it and don't release the
   *  call.
   */

   if ((( cause = CCPRI_AcceptFacilities(
                 &facilities, FALSE, TRUE )) != INVALID_ELEM_CONTENTS ) && (IE) )
      {
      OutPkgInfo[ OutPkgInfoLen++ ] = NETWORK_FACILITY_IE;
      j =  OutPkgInfoLen++; 
      OutPkgInfo[ OutPkgInfoLen++ ] = IE[3];               /* Fac Code */
      OutPkgInfo[ OutPkgInfoLen++ ] = IE[4];                /* Service */
      OutPkgInfo[ OutPkgInfoLen++ ] = IE[5];          /* Addition Info */
      IE_index = 6;
      while (IE_index < (IE[1] +2 ) )            /* Pick up Parafelder */
         OutPkgInfo[ OutPkgInfoLen++] = IE[ IE_index++ ];
      OutPkgInfo[ j ] = IE[1] -1;              /* length of Private IE */
      }

   /* Note that ORIGINATION ADDRESS is an optional element.  If provided, 
   *  attempt to match numbers.
   */
   if (( cause = CCPRI_AcceptCrNumb()) != ACCEPTED )
      {
      /* The element was present, but was coded incorrectly or the
      *  ignore the setup. 1TR20 is pretty loose about invalidly
      *  coded IEs - it may be enough to ignore this IE
      */
      ; 
      }

   /* Note that this is an optional element.  If provided, we attempt
   *  to match numbers.
   */
   if (( cause = CCPRI_AcceptCdNumb()) != ACCEPTED )
      {
      /* The element was present, but was coded incorrectly or the
      *  called number did not match ours - ignore the setup.
      */
      ;
      }
   
   CCB->ClearHost = TRUE;
   CCPRI_Mail( HOST_ID, TRUE );                  /* Send CALL_INCOMING */

bailout:

   return success;
}

/*****************
* CCP1TR6_OutCall *
******************
*
* Process the CALL_OUTGOING message from the HOST.
*
* Return TRUE if the process completed with success.
*
*/
PUBLIC BOOLEAN CCP1TR6_OutCall()

{
   int      cause = 0;                   /* Cause for unacceptable IEs */
   int      chan, hi_chan;                              /* B/H-channel */
   int      int_id;                                    /* Interface ID */
   BOOLEAN  exclusive;                           /* Channel exclusive? */
   BOOLEAN  success = TRUE;                /* Result for this function */
   BYTE facilities; 
   int      k; 
   int      j;
   int      IE_index; 
   BYTE     service_description; 
   BYTE     addition_info; 
   BOOLEAN  private_chan_id = FALSE; 


   CCB->ClearHost = TRUE;       /* The call will be cleared regardless */
   /* We start off getting the OutPkt since we will be building the
   *  CC_SETUP_REQ message as we go through the IEs of the HOST's message.
   */
   if ( CCPRI_GetOutPkt( CC_SETUP_REQ ))
      {
      /* One of these must be provided.
      *  We must determine the channel situation before we proceed since
      *  the selected channel is stored away in the CCB and the Channel
      *  Management Table.  This element will have to be reconsidered in
      *  its correct position when the CC_SETUP_REQ message is being built.
      */
      if (( IE = CCPRI_GetPrivate( CHANNEL_ID_IE, CODESET_0 )) != NULL )
         {
         private_chan_id = TRUE;
         chan      = IE[ 4 ];
         exclusive = IE[ 2 ];
         int_id    = ( MultiInterface ) ? IE[ 5 ] : 0;
         IE_Fields = NULL;
         }
#ifdef EXTENDED_HOST_MSG
      else if (( IE = CCPRI_GetPublic( CHANNEL_ID_IE, CODESET_0 )) != NULL )
         {
         IE_Fields = CCIE_ExpandChanId_IE( IE, &Trashcan );
         chan      = IE_Fields[ 9 ];
         exclusive = IE_Fields[ 2 ];
         int_id    = 
            (IE_Fields[0] && MultiInterface) ? IE_Fields[IE_Fields[8] ] : 0;
         }
#endif
      else
         {
         CCPRI_Error( CCPMSG_OUT_CALL, BAD_DATA_FROM_HOST,
                      CHANNEL_ID_IE, FALSE );
         CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
         cause = MANDATORY_IE_MISSING;
         goto out_call_failed;
         }
#ifdef CHANNEL_TABLE
      /* Check if the HOST is requesting an UNavailable channel or is 
      *  browsing for a channel.
      */
      if ( chan == ANY_CHANNEL )
         {
         exclusive = FALSE;
         }
      if (( chan == ANY_CHANNEL )                                       ||
          ( CC_CHAN_GetState( int_id, chan, 
                              CCB->ChanType, Dsl_Id ) != CHANNEL_IDLE ) ||
          ( CC_CHAN_GetServState( int_id, chan, 
                                  CCB->ChanType, Dsl_Id ) != IN_SERVICE ))
         {
         if ( exclusive )                        /* Exclusive request? */
            {
            cause = REQ_CHANNEL_NOT_AVAIL;                 /* Cause 44 */
            goto out_call_failed;
            }
         /* Select another channel for proposal to the ISDN peer.
         *  Reset the int_id to the lowest one possible for the chan_type.
         *  It is assumed that the D-channel is in the MIN_INT_ID and 
         *  thus this interface is unavailable for H11 allocation.
         */
         int_id = ( CCB->ChanType == H11_CHANNEL_UNITS ) ? MIN_INT_ID + 1 : 
                                                           MIN_INT_ID;
         switch ( SwitchType )
            {
            default:
               while (( !CCB->B_Chan ) && ( int_id <= MAX_INT_ID ))
                  {
                  /* We ASSUME here - Only 23 chans in int_id = 0 
                  */
                  hi_chan = ( MultiInterface && (int_id==MIN_INT_ID)) ?
                               MAX_CHANNEL - 1 : MAX_CHANNEL;
                  if (( CCB->B_Chan = CC_CHAN_GetIdleChan( int_id,
                                                           MIN_CHANNEL,
                                                           hi_chan,
                                                           CCB->ChanType,
                                                           Dsl_Id )) == 0 )
                     int_id++;
                  }
               break;
            }
         if ( CCB->B_Chan == 0 )
            {
            cause = NO_CIRCUIT_AVAILABLE;                  /* Cause 34 */
            goto out_call_failed;
            }
         }
      else
         {
         CCB->B_Chan = chan;       /* Save the original channel number */
         }
#else                                             /* Not CHANNEL TABLE */
      CCB->B_Chan      = chan;              /* Save the channel number */
#endif                                                /* CHANNEL_TABLE */
      CCB->Int_Id[ 0 ] = int_id;              /* Save the interface ID */

#ifdef CHANNEL_TABLE
      /* Set channel parameters now.
      */
      CC_CHAN_SetUser( CCB->Int_Id[ 0 ], CCB->B_Chan, 
                       CCB->ChanType, CCB->Call_ID, Dsl_Id );
      CC_CHAN_SetProtocol( CCB->Int_Id[ 0 ], CCB->B_Chan, 
                           CCB->ChanType, L2P_LAPD, Dsl_Id );
      CC_CHAN_SetState( CCB->Int_Id[ 0 ], CCB->B_Chan, 
                        CCB->ChanType, CHANNEL_PROPOSED, Dsl_Id );
#endif
      CCB->Exclusive = exclusive;      /* Save the exclusive indicator */

      /* Now that the channel study is over, we may proceed.
      */
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )                 /* No private elements? */
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {  
         /* Build the Channel IE
         */
         if ( private_chan_id )    /* Priv CHANNEL_ID_IE version used? */
            {
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CHANNELS_LISTED,
                                    CCB->B_Chan,
                                    CCB->ChanType,
                                    ( CCB->ChanType == H0_CHANNEL_UNITS ),
                                    CCB->Exclusive,       /* Exclusive */
                                    CCB->Int_Id_Len,/* Intrface ID len */
                                    CCB->Int_Id );  /* Intrface ID ptr */
            }
#ifdef EXTENDED_HOST_MSG
          else
             {
             CCPRI_CopyPublic( CHANNEL_ID_IE, CODESET_0 );
             }
#endif 
         /* 
         *  Facilities are on optional element.  We check to see if its
         *  present and correctly coded
         */
         if ((( CCPRI_AcceptFacilities( &facilities, TRUE, TRUE )) == NOERR ) &&
            ( IE ))
            {
            k = 0;
            OutPkt->Info[ OutPkt->InfoLen++] = NETWORK_FACILITY_IE;
            j =  OutPkt->InfoLen++;                     /* placeholder */
            OutPkt->Info[ OutPkt->InfoLen++] = 0x00;           /* zero */
            OutPkt->Info[ OutPkt->InfoLen++] = facilities; /* Fac Code */
            OutPkt->Info[ OutPkt->InfoLen++] = IE[3];       /* Service */
            OutPkt->Info[ OutPkt->InfoLen++] = IE[4]; /* Addition info */
            k += 4;
            IE_index = 5;
            while (IE_index < (IE[1] +2 )  )     /* Pick up Parafelder */
               OutPkt->Info[ OutPkt->InfoLen++]  = IE[ IE_index++ ];
            k += (IE_index - 5);
            OutPkt->Info[ j ] = k;
            }
         else if ( cause != NOERR ) 
            {
         /* Reject call - Facility Code was invalid
         */
            cause = INVALID_ELEM_CONTENTS;
            CCPRI_Error( CCPMSG_OUT_CALL, BAD_DATA_FROM_HOST,
                          NETWORK_FACILITY_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            goto out_call_failed;
            }

         if ( ( IE = CCPRI_GetPrivate ( ORIGINATION_ADDRESS_IE, CODESET_0 ) ) != NULL )
            {
            /* build the CCITT compatible info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       ORIGINATION_ADDRESS_IE,   
                                       IE[ 2 ],         /* number type */
                                       IE[ 3 ],         /* number plan */
                                       &IE[ 4 ],             /* digits */
                                       IE[ 1 ] - 2,   /* number length */
                                       TRUE );         /* ASCII digits */
            }
         else
            {
            cause = INVALID_ELEM_CONTENTS;
            CCPRI_Error( CCPMSG_OUT_CALL, BAD_DATA_FROM_HOST,
                          ORIGINATION_ADDRESS_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            cause = MANDATORY_IE_MISSING;
            goto out_call_failed;
            }   

         if ( ( IE = CCPRI_GetPrivate ( DESTINATION_ADDRESS_IE, CODESET_0 ) ) != NULL )
            {
            /* build the CCITT compatible info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       DESTINATION_ADDRESS_IE,
                                       IE[ 2 ],         /* number type */
                                       IE[ 3 ],         /* number plan */
                                       &IE[ 4 ],             /* digits */
                                       IE[ 1 ] - 2,   /* number length */
                                       TRUE );         /* ASCII digits */
            }

            /* Service Indicator is MANDATORY.
            */

         if ((( CCPRI_AcceptServiceInd ( &service_description, &addition_info, TRUE ) ) == NOERR)
            && ( IE ))
            {
            /* The IE pointer is initialized if it's located.
            */
            /* build the CCITT compatible info element.
            */
            OutPkt->Info[ OutPkt->InfoLen++] = 0x96;  /* escape to Codeset 6 */
            OutPkt->Info[ OutPkt->InfoLen++] = SERVICE_INDICATOR_IE;
            OutPkt->Info[ OutPkt->InfoLen++] = 2;            /* length */
            OutPkt->Info[ OutPkt->InfoLen++] = service_description; 
            OutPkt->Info[ OutPkt->InfoLen++] = addition_info;
            }
         else 
            {
            CCPRI_Error( CCPMSG_OUT_CALL, BAD_DATA_FROM_HOST,
                         SERVICE_INDICATOR_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            cause = INVALID_ELEM_CONTENTS;
            goto out_call_failed;
            }
         }
      /* send the mail now:
      */
      CCPRI_Mail( L3_ID, FALSE );
      }
out_call_failed:
      if ( cause )  /* A cause of zero means everything went correctly */
         {
         if ( OutPkt )           /* A half-built CC_SETUP_REQ message? */
            {
            LIF_FreePkt( OutPkt );
            OutPkt = NULL;
            }
         CCPMSG_RejectOutCall( cause, FALSE ); /* Send failure to HOST */
         success = FALSE;
         }
   return( success );
}
/************************************************************************
*                   U T I L I T I E S                                   *
*************************************************************************
*/
/*******************
CCP1TR6_GetCause *
********************
*
* This routine extracts the cause from an incoming message from the
* network or the Host  
*
*/
PRIVATE    int      near    CCP1TR6_GetCause ( recov_cause, from_network )
 int *recov_cause;                                  /* recovered cause */
 BOOLEAN from_network;                         /* from network or user */
{
 int cause = NOERR; 
 int cause_ie;

   if ( from_network  )
      {
      cause = CCPRI_AcceptCause (&cause_ie);
      if ( cause == ACCEPTED ) 
         *recov_cause = cause_ie; 
      }                                                /* from network */
   else
      {
      if (( IE = CCPRI_GetPrivate( CAUSE_IE, CODESET_0 )) != NULL )
         {
         if ( IE[1] == 0 ) 
            {
            *recov_cause = LEER_ELEMENT; 
            cause = ACCEPTED; 
            }
         else
            {
            *recov_cause = IE[2];
            cause = ACCEPTED;  
            }
         }

      }
   return ( cause );
}
/************************************************************************
*                   C C B   I N I T I A L I Z A T I O N                 *
*************************************************************************
*/
/**********************
* CCP1TR6_InitialState *
***********************
*
* Provide access to CCPRI.C for CCB initialization.
*
*/
PUBLIC CC_STATE CCP1TR6_InitialState()

{
   return ( CCP1TR6_CallIdle );
}


#ifdef TRACING                      /* Taskview or Tracker environment */
/************************************************************************
*                  C C B   S T A T E   T R A S L A T I O N              *
*************************************************************************
*/
/**********************
* CCP1TR6_NumericState *
***********************
*
* Translate the state to a numeric index.  This function is only intended
* as a support for Tracker and Taskview demo and test systems.  It is
* not normally used in a working system because it is inefficient and
* consumes memory.
*
*/
PUBLIC BYTE CCP1TR6_NumericState( ccb_state )

CC_STATE ccb_state;                                 /* State of a call */
{
   static CC_STATE call_state[] = { NULL,                         /* 0 */
                                    CCP1TR6_CallActive,           /* 1 */
                                    CCP1TR6_CallIdle,             /* 2 */
                                    CCP1TR6_CallOffered,          /* 3 */
                                    CCP1TR6_CallOriginate,        /* 4 */
                                    CCP1TR6_CallDialing,          /* 5 */
                                    CCP1TR6_CallAlerting,         /* 6 */
                                    CCP1TR6_CallReleasing,        /* 7 */
                                    CCP1TR6_CallRoutingIn,        /* 8 */
                                    CCP1TR6_CallRoutingOut,       /* 9 */
                                    CCP1TR6_CallOverlapReceiving /* 10 */
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
