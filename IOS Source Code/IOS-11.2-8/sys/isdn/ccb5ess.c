/* $Id: ccb5ess.c,v 3.3.12.2 1996/07/18 21:23:26 snyder Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccb5ess.c,v $
 *------------------------------------------------------------------
 * ccb5ess.c  ISDN 5ESS BRI Specific Call Control Functions
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccb5ess.c,v $
 * Revision 3.3.12.2  1996/07/18  21:23:26  snyder
 * CSCdi63402:  get isdn table out of data space and into text space
 * Branch: California_branch
 *              saves 9788 in RAM on run from flash devices
 *
 * Revision 3.3.12.1  1996/04/30  23:06:41  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/15  17:27:11  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.2  1995/11/17  17:39:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccb5ess.c
VERSION: 3.21
DESCRIPTION: ISDN 5ESS BRI Specific Call Control Functions

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

This module has been designed to be linked with the "core" BRI Call
Control module (CCBRI.C).


ORGANIZATION OF FUNCTIONS


The COMP_TEST conditional compilation flag is found within this module
wherever modification to the basic Call Control has been made in order
that it may perform some specific test or tests within the AT&T 5E4.2
BRI Test Plan (Jan. 1988).  These modifications generally are necessary
to cause Call Control to assume optional modes of operation rather than
to correct some inherent operational flaws.


#ifdef SBC_INTERFACE

FEATURE DESCRIPTION FOR 5ESS COMPATIBLE CALL CONTROL

BASIC VOICE FEATURES:

The call control will handle basic call setup and tear-down for the
5ESS generic programs starting with 5E4.2 and up to 5E7.
The Call Control will insert endpoint information element into all
outgoing setup messages if the "spid" is not null (i.e.: an endpoint
initialization procedure was successfully completed - see module
MEMIM.C for more details).
The Call Control will verify endpoint information element in all incoming
setup messages is the "spid" is not null.
The Call Control will NOT verify the called local directory number even
if it is provided in incoming setup message.  The endpoint information
element will be verified instead. The calling local directory number
will be inserted in all outgoing setup messages if the "ldn" is not null.

MULTIPLE CALLS OFFERING:

The call control is designed to handle up to two outstanding call
setup requests from the network at a time.  The second call setup
message should not have a channel id information element present. This
second call setup will be treated as a call waiting request which can
be processed only when a B channel becomes available, e.g.: via clearing
of a call that is currently active. This is consistent with AT&T's
multiple call offering feature.

ICLID FEATURE

The incoming calling line identification (ICLID) feature allows the user
to display the directory number of the calling party before the call is
answered.

ICLID feature is provided through the following interface:

 - it is assumed that there is a driver to a device which displays
   the ICLID information. This driver receives only mail in the form
   of packages from this call control. The first byte of the info
   buffer specifies the type of ICLID related message as:

       - ACTIVE_ICLID  (ICLID of an active call)
       - PASSIBE_ICLID (ICLID for a call waiting call)
       - SWITCH_ICLID  (swich the passive/active ICLIDs)

   The first two messages may be followed by:

       - NO_CALLS (clean up the ICLID buffer)
       - incoming call number

   All the strings are null terminated.

 - the ICLID driver may send the folowing messages to (in the form of
   packages) to the call control:

       - CW_CALL_REJECT   (reject the call waiting call without answering)
       - RING_CALL_REJECT (reject ringing call without answering)

SUPPLEMENTARY VOICE FEATURES

The supplementary voice features in this call control follow the AT&T's
type A terminal features. The following features are fully implemented:

    - call hold and recall
    - call conference (including add-on conference)
    - call transfer (supervised and unsupervised)

Note that the drop feature (another type A terminal feature) is not
supported in this version of call control.

#endif


*************************************************************************
*************************************************************************
*/

#define     CCB5ESS_C                                   /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

#include    "flags.h"
#ifdef BRI_5ESS	
#include    "tune.h"
#include    "gendef.h"
#include    "lif.h"
#include	"gp.h"
#include    "ccbri.h"
#include    "ccb5ess.h"
#include    "ccie.h"
#include    "cc_chan.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
enum CCB_State
   {
   ATT_STATE_LOW_LIMIT,                              /* Sizing element */
   ATT_IDLE,                                               /* CCB idle */
   ATT_CONNECTED,                                   /* In conversation */
   ATT_DIALING,                             /* In dial wait or dialing */
   ATT_RINGING,                                       /* Being alerted */
   ATT_RELEASING,      /* Awaiting a response to Disconnect or Release */
   ATT_TONE,                         /* Receiving a call progress tone */
   ATT_DISCONNECTING_HOST,             /* Host Disconnect sent to Host */
   ATT_OFFERED,                     /* Incoming Call forwarded to Host */
   ATT_CONFERENCING,        /* Awaiting response to conference request */
   ATT_CW_RINGING,                         /* In Call Waiting alarting */
   ATT_HELD,                                                /* On hold */
   ATT_RECONNECTING,                         /* Awaiting reconnect ack */
   ATT_STATE_HIGH_LIMIT                              /* Sizing element */
   };

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

/************************************************************************
*             S T A T E   E N T R Y   J U M P   T A B L E               *
*************************************************************************
*/

typedef BOOLEAN (near *CC_STATES)( int * );

/* NOTE: this table MUST strictly correspond to the CCB_State table above !!
*/
static const CC_STATES Ccb_State_Func_5ess[ ATT_STATE_HIGH_LIMIT ] =
   {
   NULL,
   CCB5E_VoiceIdle,
   CCB5E_VoiceConnected,
   CCB5E_VoiceDialing,
   CCB5E_VoiceRinging,
   CCB5E_VoiceReleasing,
   CCB5E_VoiceTone,
   CCB5E_DisconnectingHost,
   CCB5E_CallOffered,
   };

/********************
* CCB5E_CallOffered *
*********************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to State 6 of Q.931.
*
*/
PRIVATE BOOLEAN near CCB5E_CallOffered( event )

int *event;                                /* Event input to the state */

{
   int cause;

   switch ( *event )
      {

      case HOST_DISCONNECT:
         /* acknowledge the disconnect and free CCB
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( FALSE );
            if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CAUSE_IE ) ) != NULL )
               {
               cause = ( int ) IE[2];
               }
            else
               {
               cause = CALL_REJECTED;
               }
            CCBRI_RejectSetup(cause, NO_DIAG);
            }
         CCBRI_RemoveCCB( CCB );
         /* no change in state - obviously (we do not have CCB any more !)
         */
         break;

      case HOST_DISCONNECT_ACK:
         /* should not happen - if it does, free the CCB
         */
         CCBRI_RemoveCCB( CCB );
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;
      case HOST_PROCEEDING:
         /* Indication that the Host is processing the call
         */
         if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
           {
           CCBRI_MailtoL3();                 /* Send CC_BROADCAST_RESP */
           if ( CCBRI_GetOutPkt( CC_CALL_PROC_REQ, FALSE ))
              {
              /* Add CHANNEL ID info element.
               */
               OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                              CHANNELS_LISTED,    /* channel selection */
                              CCB->B_Chan,
                              B_CHANNEL_UNITS,         /* type: H or B */
                              FALSE,                     /* slot map ? */
                              FALSE,                    /* exclusive ? */
                              0,                /* interface id length */
                              NULL );                  /* interface id */

               CCBRI_MailtoL3();
               CCB->State = ATT_RINGING;
               }
           }
         break;
      case HOST_ALERTING:
         /* Indication that the Host has initiated alerting for the call
         */
        if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
           {
           CCBRI_MailtoL3();                 /* Send CC_BROADCAST_RESP */
           }
        if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
           {
           /* Add CHANNEL ID info element.
            */
           OutPkt->InfoLen +=
           CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                              CHANNELS_LISTED,    /* channel selection */
                              CCB->B_Chan,
                              B_CHANNEL_UNITS,         /* type: H or B */
                              FALSE,                     /* slot map ? */
                              FALSE,                    /* exclusive ? */
                              0,                /* interface id length */
                              NULL );                  /* interface id */

           CCBRI_MailtoL3();
           }
        CCB->State = ATT_RINGING;
        break;
      case HOST_CONNECT:
        if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
           {
           CCBRI_MailtoL3();                 /* Send CC_BROADCAST_RESP */
           }
        CCBRI_HostConnect();
        CCB->State = ATT_RINGING;
        break;
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;
      default:
         /* Unexpected message.  No message is expected here from Layer 3
         *  since it will have cleared the Call_ID.  If L3 recieves a new
         *  SETUP, it will be processed in CC_NTKDB in a new CCB in the
         *  IDLE state.
         */
         CCBRI_Error( CCBRI_CALL_LOCKOUT, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/*************************
* CCB5E_DecodeCallState *
**************************
*
* Decode the state of the CCB used in a voice application from
* the CCB state variable.  Dispatch the appropriate state function.
*
* The states are organized as follows:
*           idle            (ATT_IDLE)
*           conversation    (ATT_CONNECTED)
*           call setup      (ATT_DIALING, ATT_TONE, ATT_OFFERED)
*           incoming call   (ATT_RINGING, ATT_CW_RINGING)
*           call teardown   (ATT_RELEASING, ATT_HOST_DISCONNECTING)
*           suppl. services (ATT_HELD, ATT_RECONNECTING, ATT_CONFERENCING)
*
*/
PUBLIC BOOLEAN      CCB5E_DecodeCallState ( event )

BOOLEAN    *event;                         /* Event input to the state */

{
   BOOLEAN  event_pending;
   int      result_code;


   /* Perform a range check on the value of CCB->State to prevent
   *  the strange and unusual.
   */
   if (( CCB->State > ATT_STATE_LOW_LIMIT ) && ( CCB->State < ATT_STATE_HIGH_LIMIT ))
      {
      /* Jump to enter the state function.
      */
      event_pending     = ( *Ccb_State_Func_5ess[ CCB->State ] )( event );
      result_code       = ( event_pending == EVENT_PENDING) ? EVENT_PENDING : NO_ERROR;
      }
   else
      {
      CCBRI_Error( CCBRI_DECODE_CALL_STATE, INVALID_STATE, CCB->State,
                   *event, TRUE );
      result_code = INVALID_STATE;
      }

   return( result_code );

}

/**************************
* CCB5E_DisconnectingHost *
***************************
*
* CCB5E_DisconnectingHost.  This State corresponds to Q.931 State 12
* A Disconnect Indication has been received and HOST_DISCONNECT has been
* mailed to the Host
*
*/
PRIVATE BOOLEAN near CCB5E_DisconnectingHost( event )

int *event;                                /* Event input to the state */

{
  BYTE event_b;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_DISCONNECT_ACK:
         LIF_StopTimer ( CCB->CC5essTimer );
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
           {
             /* Build complaining Cause info element (optional in this message)
            */
           OutPkt->InfoLen +=
           CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                        CCB->Cause, FALSE, 0 );
           CCBRI_MailtoL3();
           CCB->State = ATT_RELEASING;
           }
         break;
      case DISC_HOST_EVENT:                       /* Timeout from Host */
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
           {
             /* Build complaining Cause info element (optional in this message)
            */
           OutPkt->InfoLen +=
           CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                       CCB->Cause, FALSE, 0 );
           CCBRI_MailtoL3();
           event_b = HOST_DISCONNECT_ACK;
           CCBRI_DisconnectHostCall(&event_b, CCB->Cause);
           CCB->State = ATT_RELEASING;
           }
         break;
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;
#endif
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
          LIF_StopTimer ( CCB->CC5essTimer );
          event_b = HOST_DISCONNECT_ACK;
          CCBRI_DisconnectHostCall(&event_b, CCB->Cause);
          CCB5E_ReleaseCall ();
         break;

      default:
         /* Unexpected message.
         *  Note that L3 logic is designed to catch these cases in general
         *  before we receive them, but the inopportune arrival of some
         *  messages may not be detected in L3 because they may not be
         *  inopportune for all applications.
         *  In the call to CCBRI_RejectMsg, the Cause diagnostic is
         *  generated from the Protocol Discriminator byte of the Info
         *  which was overwritten by Layer 3 with the msg_type it received.
         */
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = ATT_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_DISCONNECTING_HOST, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );

}

#ifdef 0
/**********************
* CCB5E_PassiveHangup *
***********************
*
* Standard actions for hangup of a passive voice call.  This function
* initiates sending of CC_DISCONNECT_REQ for the PASSIVE (inactive)
* call only if this inactive call is NOT in idle state.  If the passive
* call is in idle state, this function will free the CCB for this call.
*
*/
PRIVATE int near CCB5E_PassiveHangup( ccb )

CCB_ID ccb;

{
   CCB_ID temp;
   int result = NOERR;

   if (ccb->State == ATT_IDLE)
      {
      CCBRI_RemoveCCB( ccb );
      }
   else
      {
      /* store the current ccb pointer.  make the ccb for the passive
      *  call the current ccb.
      */
      temp = CCB;
      CCB = ccb;

      /* send CC_DISCONNECT_REQ:
      */
      if ( CCBRI_GetOutPkt( CC_DISCONNECT_REQ, FALSE ) )
         {
         /* Build mandatory Cause element - normal clearing
         */
         OutPkt->InfoLen +=
            CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
            NORMAL_CLEARING, FALSE, 0 );
         /* Build optional Switchhook State element - onhook (the
         *  phone may not be on hook but from the point of view of
         *  this passive call this is an on hook condition.  Therefore
         *  we will lie to the C.O. and tell it that the passive call was
         *  hung up.
         */
         OutPkt->InfoLen +=
            CCIE_BuildHookState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
            ONHOOK_IDLE);
         CCBRI_MailtoL3();
         }
      else
         {
         result = ERR;
         }

      /* restore the current CCB to what it was before this function was
      *  executed:
      */
      CCB = temp;
      }

   return( result );
}
#endif


/********************
* CCB5E_ReleaseCall *
*********************
*
* Terminate a call and put the CCB to bed.
*
* This function applies to both voice and data calls.
*
*/
PUBLIC void      CCB5E_ReleaseCall()

{

   int    i = 0;

   /* Perform actions specific to the voice call termination.
   */
   if ( CCB->B_Chan )
      {
      if (( CCB->B_Chan != UNDEFINED_B_CHANNEL ) &&
         ( CC_CHAN_GetUserbri(CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID)
              == CCB->Call_ID ))
         CC_CHAN_ReleaseChanbri(CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID);
      CCB->B_Chan = 0;
      }
   /* check if this is an active call:
   */
#ifdef SBC_INTERFACE
   if ( CCB == Hcb[ Dsl_ID ].Ccb_In_Use[ Hcb[ Dsl_ID ].Active_Call ] )
      {
      /* check if the passive call is in call waiting state:
      */
         {
         CCBRI_RemoveCCB( CCB );
         }
      }
   else
#endif
      {
      /* this is a passive CCB - get rid of this CCB only.  Do
      *  not touch any other CCB's:
      */
      CCBRI_RemoveCCB( CCB );
      }

#ifdef SBC_INTERFACE
   if (Hcb[ DEFAULT_DSL ].Hw_State == ONHOOK_IDLE)
      {
      CCBRI_TRACE( TR_IDLE );
      }
   else
      {
      CCBRI_TRACE( TR_LOCKOUT );
      }
#else
   CCBRI_TRACE( TR_IDLE );
#endif

#ifdef COMP_TEST
   /* This is an expedient necessary to perform the AT&T Test Plan
   *  with this Call Control under TaskView.
   */
   Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
#endif
   TheTrash = i;
   return;
}



/****************
* CCB5E_Restart *
*****************
*
* Restart for a B-Channel or the entire interface.
*
*/
PUBLIC void CCB5E_Restart( event, dsl_id )

int  event;                       /* CC_RESTART_IND or CC_RESTART_CONF */
BYTE dsl_id;

{
   int  restart_class;                              /* Type of restart */
   int  b_channel;                               /* Channel to restart */


   /* Look for the Restart Ind info element.
   */
   IE = CCIE_Get_IE ( InPkt->Info, InPkt->InfoLen, RESTART_IND_IE,
      CODESET_0, 1 ) ;
   restart_class = IE[ 2 ] & 0x07;            /* Recover restart class */

   /* Look for the Channel ID info element.
   */
   IE = CCIE_Get_IE ( InPkt->Info, InPkt->InfoLen, CHANNEL_ID_IE,
      CODESET_0, 1 ) ;

   if (( restart_class == RESTART_ALL ) || ( !IE ))
      {
      /* This restart class or the lack of the CHANNEL_ID_IE ==> RESTART ALL.
      *  Test the channel state of channel 0 for correctness.
      *  Note that if Call Control has previously requested restart for the
      *  whole interface, this condition has been been tagged in the state
      *  of the 0th (otherwise unused) channel (CHANNEL_RESTART_PENDING).
      */
      if (( event == CC_RESTART_IND )  ||
          ( event == CC_RESTART_CONF ))
         {
         for (b_channel = MIN_CHANNEL; b_channel <= MAX_CHANNELB; b_channel++)
            CCB5E_RestartChan( b_channel, dsl_id );
#ifdef DEBUG
         printf ( "CCB5E_Restart...ALL channels restarted.\n" );
#endif
         Call_Id_Source = 1;          /* Restore the Call_ID generator */
         }
      }
   else                              /* Restart individual channel(s). */
      {
      IE_Fields = CCIE_ExpandChanId_IE( IE, &Trashcan );
      b_channel = IE_Fields[ 4 ];
      if (( event == CC_RESTART_IND )  ||
         (( event == CC_RESTART_CONF ) &&
         ( CC_CHAN_GetStatebri(b_channel, B_CHANNEL_UNITS, dsl_id)
              == CHANNEL_RESTART_PEND )))
         {
#ifdef DEBUG
         printf ( "CCB5E_Restart...channel %d restarted.\n", b_channel );
#endif
         CCB5E_RestartChan( b_channel, dsl_id );  /* Restart a channel */
         }
      }
}


/********************
* CCB5E_RestartChan *
*********************
*
* Restart an individual B-Channel.
*
*/
PRIVATE void near CCB5E_RestartChan( b_chan, dsl_id )

int  b_chan;                                /* Channel to be restarted */
BYTE dsl_id;

{
   BOOLEAN done = FALSE;
#ifdef SBC_INTERFACE
   int     i;
#endif

#ifdef SBC_INTERFACE
   /* Check if the channel being restarted is in use.
   */
   for ( i = 0; i < MAX_VOICE_CALLS; i++ )
      {
      CCB = Hcb[ dsl_id ].Ccb_In_Use [ i ];
      if (( CCB ) && ( CCB->B_Chan == b_chan ))
         {
         CCB5E_ReleaseCall();                    /* Terminate the call */
         done = TRUE;
         }
      }
#endif
#ifdef COMMAND_INTERFACE
   /* Check if the channel being restarted is in use.
   */
   done = TRUE;
   CCB = CCB_Head[ dsl_id ];
   while ( CCB )
      {
      if ( CCB->B_Chan == b_chan )
         {
         if ( CCB->State != ATT_IDLE )
            {
            CCBRI_ReleaseChan();
#ifdef COMMAND_INTERFACE
            CCBRI_RejectHostCall( TEMPORARY_FAILURE );
            CCBRI_RemoveCCB (CCB);
#endif
#ifdef SBC_INTERFACE
            CCB5E_ReleaseCall;
#endif
            }
         else
            {
            CCBRI_ReleaseChan();
            CCBRI_RemoveCCB( CCB );
            }
         }
      CCB = CCB->Next;
      }
#endif
   if ( !done )
      CC_CHAN_ReleaseChanbri(b_chan, B_CHANNEL_UNITS, dsl_id);
}




#ifdef ICLID
#ifdef TASK_ICLID_DRV
/******************
* CCB5E_SendICLID *
*******************
*
* This function will send a message to the ICLID driver task.
*
*/
PRIVATE void near CCB5E_SendICLID( iclid_string, iclid_id )

BYTE *iclid_string;
BYTE  iclid_id;

{
   int      i;
   PKT_ID   Pkt;

   Pkt = LIF_GetPkt( SML_INFO_BTYPE, DATA_OFFSET, TRUE );
   if ( Pkt )
      {
      Pkt->HeadLen  = DATA_OFFSET;                                /* 4 */
      Pkt->InfoType = SML_INFO_BTYPE;                       /* default */

      switch (iclid_id)
         {
         case SWITCH_ICLID:
            Pkt->Info[ 0 ] = SWITCH_ICLID;
            Pkt->Info[ 1 ] = '\0';
            Pkt->InfoLen = 2;
            break;

         case ACTIVE_ICLID:
            Pkt->Info[ 0 ] = ACTIVE_ICLID;
            if (iclid_string == NULL)
               {
               Pkt->Info[ 1 ] = NO_CALLS;
               Pkt->Info[ 2 ] = '\0';
               Pkt->InfoLen = 3;
               }
            else
               {
               Pkt->InfoLen = 1;
               for (i=0; i<MAX_ICLID; i++)
                  {
                  Pkt->Info[i+1] = iclid_string[i];
                  Pkt->InfoLen++;
                  if (iclid_string[i] == '\0')
                     break;
                  }
               }
            break;

         case PASSIVE_ICLID:
            Pkt->Info[ 0 ] = PASSIVE_ICLID;
            if (iclid_string == NULL)
               {
               Pkt->Info[ 1 ] = NO_CALLS;
               Pkt->Info[ 2 ] = '\0';
               Pkt->InfoLen = 3;
               }
            else
               {
               Pkt->InfoLen = 1;
               for (i=0; i<MAX_ICLID; i++)
                  {
                  Pkt->Info[i+1] = iclid_string[i];
                  Pkt->InfoLen++;
                  if (iclid_string[i] == '\0')
                     break;
                  }
               }
            break;

         default:
            break;
         }

      LIF_SendPkt( L2P_LAPD,
         CC_ID, 0,
         ICLID_DRV_ID,
         ICLID_INFO,
         0,
         0,
         0,
         0,
         1,
         0,
         TRUE,
         Pkt,
         NOW,
         REGULAR_MAIL);
      }
}
#endif
#endif

/*****************
* CCB5E_SetupInd *
******************
*
* A Setup is received in Voice Idle state
*
*/
PRIVATE int near CCB5E_SetupInd ()
{
   int     k = 0;
   int     length;
   int     result_code;
   int     cause;
   BYTE    chan_id;
   BYTE    exclusive;
   int     tmp_usid, our_usid;
   int     tmp_tid, our_tid;
   int     tmp_inter, our_inter;
   BOOLEAN call_allowed;
   BYTE    calling_number[ MAX_ADDRESS_DIGITS ];
   result_code = NOERR;

         /* A new call setup from the far end. The CC will send
         *  ALERTING request to the network instructing it that
         *  the phone is ringing.  This will be done only after
         *  unpacking and checking of info elements in the SETUP_IND
         *  message.  The hardware state of the phone will determine
         *  treatment of this message.  The following cases are
         *  tested:
         *             - Hardware IDLE, SETUP includes B channel (use
         *               the B channel indicated in order to set up
         *               an active incoming call, start ringing)
         *
         *             - Hardware IDLE, SETUP without B channel (use
         *               the default available B channel in the ALERTING
         *               message, setup incoming active call, start
         *               ringing)
         *
         *             - Hardware OFF HOOK, SETUP includes B channel (error
         *               condition, reject the setup)
         *
         *             - Hardware OFF HOOK, SETUP without B channel (setup
         *               incoming passive call, send ALERTING (no B channel)
         *               start call waiting alerting).
         *
         *  The active/passive state of the current CCB must also be examined.
         *
         *  Before sending of ALERTING message, the CC must always mail
         *  BROADCAST response message.  This will assure that the Layer 3
         *  software is in a proper state to receive the ALERTING.
         */
         /* Bearer capability is the only truly mandatory info element
         *  here.  According to AT&T 5E4.2 Test Plan (Illegal Message Tests
         *  Test 4.1) a missing Bearer Capability info element is grounds
         *  to IGNORE the message.  In this case we will go ahead and
         *  overtly reject it (CC_SETUP_REJECT_REQ).
         */
         /* NOTE: get the calling number for the incoming call here.
         *        Also note whether the incoming call is for an
         *        active or a passive call appearance.  This
         *        information will be passed to ICLID program in
         *        the future (*** FFS ***).
         */
         /* To force a terminal on a broadcast data link to initiate
         *  data link establishment, AT&T has been known to send a
         *  broadcast SETUP with the call reference deliberately incorrect.
         *  The error is in the call reference flag bit (MSB), which is
         *  sent inverted (asserted) with call reference 0x7F.  L3 always
         *  inverts the call reference flag bit on all incoming message as
         *  one of its first actions (L3_GetInfo()), so the (bad) 0xFF call
         *  reference is converted back to 0x7F before reaching here (CC).
         *  When L3 handles the setup reject we are about to generate, it
         *  will be forced to initiate data link establishment with L2
         *  before it may respond to the bad SETUP message with RELEASE_COMP,
         *  which is what AT&T was scheming all along.
         */
         if ( ( CCB->Call_ID == 0 ) ||
            ( CCB->CallRef == 0x7F ) )
            {
            CCBRI_RejectSetup(INVALID_CALL_REF, NO_DIAG);
            }
         else
            {
#ifdef COMMAND_INTERFACE
            /* inform the HOST.
            */
            if ( CCBRI_GetOutInfo( HOST_INCOMING_CALL ) )
               {
               OutPkgInfoLen++;
               k = 5;             /* bearer capability must be present */
               }
#endif
         /* Look at the bearer capability IE.
         */
            if (( cause = CCBRI_AcceptBearerCap()) != NOERR )
               {
                 /* This is a mandatory element.  The call must be rejected.
                 */
                   CCBRI_RejectSetup( cause, BEARER_CAPABILITY_IE );
                   CCB5E_ReleaseCall();
#ifdef COMMAND_INTERFACE
                   CCBRI_FreeCallBufs();
#endif
                   goto bailout;
                }
            /*
             * Next we look for channel id IE. Have to reject the
             * call here because the state changes.
             */
            cause = CCBRI_AcceptChanId(&chan_id, &exclusive);
            if (cause == NOERR) {
               if (CCBRI_SetNewChan(chan_id, CHANNEL_BUSY, FALSE) != NOERR) {
                   CCBRI_RejectSetup(REQ_CHANNEL_NOT_AVAIL, CHANNEL_ID_IE);
                   CCB5E_ReleaseCall();
                   CCBRI_FreeCallBufs();
                   goto bailout;
               }
               if ( OutPkgInfo )
                  {
                  OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 2;
                  OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
                  OutPkgInfo[ OutPkgInfoLen++ ] = chan_id;
                  k += 4;
                  }
            } else {
                /*
                 * This is a mandatory element.  The call must be rejected.
                 */
                CCBRI_RejectSetup(cause, CHANNEL_ID_IE);
                CCB5E_ReleaseCall();
                CCBRI_FreeCallBufs();
                goto bailout;
            }

            /*
             * Verify endpoint id IE
             */
            CCBFIT_GetEpid(&our_usid, &our_tid, &our_inter, SPECIFIER_0);
            if (CCBFIT_AcceptEndpointId(&tmp_usid, &tmp_tid, &tmp_inter) == NOERR) {
               /*
                * verify USID/TID match
                */
               if ((tmp_usid == our_usid) ||
                   (tmp_usid == 0) || (tmp_usid == BROADCAST_USID_5ESS)) {
                  if (((tmp_inter == 0) &&
                      ((tmp_tid  == our_tid) || (tmp_tid == BROADCAST_TID))) ||
                     ((tmp_inter == 1) && (tmp_tid != our_tid)))
                     /*
                      * USID/TID test positive, the call will proceed.
                      */
                     call_allowed = TRUE;
                  else
                     call_allowed = FALSE;
               } else {
                  /*
                   * no match in TID or USID - the call will not proceed.
                   */
                  call_allowed = FALSE;
               }
            } else {
               /*
                * Either we are not on a multipoint, or we do not have
                * our own USID/TID or the incoming message does not carry
                * USID/TID - we will let the call through.
                */
               call_allowed = TRUE;
            }

            /*
             * Now get the calling number (not a mandatory IE).
             * NOTE: the AcceptCallingNumb function will return
             * the calling number or NO_ICLID in the first byte if there
             * is no calling number IE.
             */
            if (CCBRI_AcceptCallingNumb(&calling_number[0], &length) == NOERR) {
               k+= length;
            }

#if defined (X31_DEV) || defined (TASK_V3)
            if (CCBRI_AcceptCalledNumb( &length, Dsl_ID ) == NOERR) {
               k += length;
            }
#endif

            if ( ( result_code  == NOERR ) &&
               ( call_allowed == TRUE ) )
               {
#ifdef COMMAND_INTERFACE
               /* inform the HOST
               */
               if ( OutPkgInfo )
                  {
                  OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
                  CCBRI_MailtoHost( CCB->CopyPublicMsg );
                  CCB->State = ATT_OFFERED;
                  goto bailout;

                  }
#endif
               if ( ( Hcb[ Dsl_ID ].Hw_State == ONHOOK_IDLE ) ||
                  ( Hcb[ Dsl_ID ].Hw_State == BELL ) )
                  {
                  if (CCB->HcbIndex == Hcb[ Dsl_ID ].Active_Call)
                  {
                     if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ) )
                        CCBRI_MailtoL3();
#ifdef COMP_TEST
                     if ( QuickConn )
                        {
                        if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ) )
                           {
                           /* we will ALWAYS send channel ID info element with
                           *  this message (i.e. with CONNECT message, as this
                           *  SETUP response message becomes a CONNECT message
                           *  at Layer 3.
                           */
                           OutPkt->InfoLen +=
                              CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                              CCB->B_Chan, 0, B_CHANNEL_UNITS, FALSE,
                              FALSE, 0, NULL );
                           OutPkt->InfoLen +=
                              CCIE_BuildHookState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                              OFFHOOK);
                           CCBRI_MailtoL3();
                           }
                        Hcb[ Dsl_ID ].Hw_State = OFFHOOK;
                        /* Assume RINGING CCB state as if ALERTING had been
                        *  answered.
                        */
                        CCB->State = ATT_RINGING;
                        break;
                        }

                     if ( QuickProceed )
                        {
                        if ( CCBRI_GetOutPkt( CC_CALL_PROC_REQ, FALSE ) )
                           {
                           /* we will ALWAYS send channel ID info element with
                           *  this message (i.e. with CONNECT message, as this
                           *  SETUP response message becomes a CONNECT message
                           *  at Layer 3.
                           */
                           OutPkt->InfoLen +=
                              CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                              CCB->B_Chan, 0, B_CHANNEL_UNITS, FALSE,
                              FALSE, 0, NULL );
                           CCBRI_MailtoL3();
                           }
                        CCB->State = ATT_RINGING;
                        break;
                        }
#endif                                             /* End of Comp Test */
                     if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ) )
                        {
                        /* Build mandatory/optional CHANNEL ID element
                        */
                        OutPkt->InfoLen +=
                           CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                           (CCB->B_Chan == UNDEFINED_B_CHANNEL) ?
                              ANY_CHANNEL : CCB->B_Chan,
                           0, B_CHANNEL_UNITS, FALSE, FALSE, 0, NULL );
                        CCBRI_MailtoL3();
                        }
                                               /* here turn ringing on */
#ifdef TASK_KB
                     CCBRI_StartRing( VOICE_ALERTING );
#endif
#ifdef ICLID
                     CCB5E_SendICLID( calling_number, ACTIVE_ICLID );
#endif
                     CCB->State = ATT_RINGING;
                     Hcb[ Dsl_ID ].Hw_State = BELL;
                     CCBRI_TRACE( TR_RINGING );
                     }
                  else
                     {
                     /* passive call CCB but the hardware is on hook.  This means that
                     *  there is a call in RINGING state already.  We will send ALERTING
                     *  message (no B channel) and will turn call waiting alerting on.
                     *  The new state for this CCB will be CW_RINGING.  Check here
                     *  that there is no B channel in SETUP message, if this is not
                     *  true - reject the setup.
                     */
                     if ( CCB->B_Chan == UNDEFINED_B_CHANNEL )
                        {
                        if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ) )
                           CCBRI_MailtoL3();
                        if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ) )
                           {
                           CCBRI_MailtoL3();
                           }
                        CCB->State = ATT_CW_RINGING;
#ifdef TASK_KB
                        CCBRI_StartRing( CW_ALERTING );
#endif
#ifdef ICLID
                        CCB5E_SendICLID( calling_number, PASSIVE_ICLID );
#endif
                        }
                     else
                        {
                        /* active call setup attempt while a call is already
                        *  active.  Ignore it but release the resources!
                        */
                        CCB5E_ReleaseCall();
                        }
                     }
                  }                       /* the phone is on-hook idle */
               else                    /* the phone is not ONHOOK_IDLE */
                  {
                  /* The phone is in an OFF HOOK state.  This must be a call
                  *  setup for a call waiting call.  Check that:
                  *       1. this is not an active CCB
                  *       2. there is no B channel in  the setup message
                  * If the above is true, send alerting (no B channel), start
                  * call waiting alerting.  Otherwise, reject the setup.
                  */
                  if ( ( CCB->HcbIndex != Hcb[ Dsl_ID ].Active_Call ) &&
                     ( CCB->B_Chan == UNDEFINED_B_CHANNEL ) )
                     {
                     if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ) )
                        CCBRI_MailtoL3();
                     if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ) )
                        {
                        CCBRI_MailtoL3();
                        }
                     CCB->State = ATT_CW_RINGING;
#ifdef TASK_KB
                     CCBRI_StartRing( CW_ALERTING );
#endif
#ifdef ICLID
                     CCB5E_SendICLID( calling_number, PASSIVE_ICLID );
#endif
                     }
                  else
                     {
                     /* active call setup attempt while a call is already
                     *  active.  Ignore it but release the resources!
                     */
#ifdef SEND_TR_MSG
                     /* Note that this action in only necessary to improve
                     *  the performance of the DEMOs.  An actual 5ESS switch
                     *  would not have gotten us here in the first place
                     *  and ignoring it would be an adequate response anyway.
                     */
                     if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ) )
                        CCBRI_MailtoL3();
                     if ( CCBRI_GetOutPkt( CC_SETUP_REJ_REQ, FALSE ))
                         {
                         OutPkt->InfoLen +=
                            CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                            USER_BUSY, FALSE, 0 );
                         CCBRI_MailtoL3();
                         }
#endif
                     CCB5E_ReleaseCall();
                     }
                  }                               /* phone is off hook */
               }                 /* no missing mandatory info elements */
            }
   TheTrash = k;
bailout:
  return result_code;
}

#ifdef 0
/***********************
* CCB5E_StdVoiceHangup *
************************
*
* Standard actions for hangup by the voice phone.  This function initiates
* sending of CC_DISCONNECT_REQ.  It should be used whenever the voice
* phone goes on hook.
*
*/
PRIVATE void near CCB5E_StdVoiceHangup( cause )

int cause;                      /* Cause for cause information element */
{
#ifdef TASK_KB
   CCBRI_StartTone( SILENT_TONE );
#endif

#ifdef COMP_TEST
   if ( ReleaseActive )
      {
      /* Send RELEASE instead of DISCONNECT as an option to perform
      *  AT&T 5E4.2 Inopportune Messages Test 60.
      */
      if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
         {
         OutPkt->InfoLen +=
            CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
            cause, FALSE, 0 );
         CCBRI_MailtoL3();
         }
      }
   else
#endif
   if ( CCBRI_GetOutPkt( CC_DISCONNECT_REQ, FALSE ) )
      {
      /* Build mandatory Cause element - normal clearing
      */
      OutPkt->InfoLen +=
         CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
         cause, FALSE, 0 );
      /* Build optional Switchhook State element - onhook
      */
      OutPkt->InfoLen +=
         CCIE_BuildHookState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
         ONHOOK_IDLE);
      CCBRI_MailtoL3();
      }

   return;
}
#endif


/***********************
* CCB5E_VoiceConnected *
************************
*
*
* Process the current event in the context of a voice call in the
* CONNECTED (in conversation) state.
*
* This is a state for an active call.
*
*/
PRIVATE int near CCB5E_VoiceConnected( event )

int    *event;                             /* Event input to the state */

{
   int    result_code;                 /* result code for the function */
   BYTE   cause_ie;
   int    cause;
#ifdef COMMAND_INTERFACE
   int    k;
   BYTE   prog_desc;
   BYTE   signal;
   BYTE   event_b;
#endif

   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_DISCONNECT:
      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = ATT_RELEASING;
         break;
#endif
#ifdef SBC_INTERFACE
      case  KP_ON_HOOK:
         /* local disconnect (response to this message will be
         *  received in RELEASING state - see CC_RELEASE_IND):
         */
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
         CCB5E_StdVoiceHangup( NORMAL_CLEARING );

#ifdef COMP_TEST
         /* The active_call identity is retained to allow messages
         *  to be received in the RELEASING state.
         */
#else
         Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
#endif
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         CCB->State = ATT_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif

      case CC_DISCONNECT_IND:
#ifdef COMP_TEST
         if ( SuspendDiscon )
         /* We will ignore this message and rely upon subsequent
         *  USER hangup.
         */
            break;
#endif
         CCB5E_DisconnectInd();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         /* The RELEASE_CONF message will be received if L3 received
         *  RELEASE_COMP in State 10.
         *  The RELEASE_IND message may be received in any state.
         *  Either should clear the CCB associated with this call only
         *  (any calls in passive states will not be affected by
         *  these messages).
         */
         /* NOTE: the cause info element is mandatory in these messages
         *        as they are the first clearing messages.
         */
#ifdef COMMAND_INTERFACE
         cause = (( CCBRI_AcceptCause (&cause_ie) == MANDATORY_IE_MISSING ) ?
                     MANDATORY_IE_MISSING : cause_ie);
         event_b       = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall( &event_b , cause );
#endif
#ifdef SBC_INTERFACE
         Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
#endif
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
         CCB5E_ReleaseCall();
         break;

      case CC_SETUP_IND:
         /* the network attempts to set up a new call with the same
         *  call id as the call presently in conversation.  Reject
         *  this call.
         */
         /* NOTE: we will ignore this call setup attempt so there
         *  is no need to verify its mandatory info elements.
         */
         /* NOTE: this CANNOT be a call waiting call setup because
         *  it has the call_id of an active call in progress.
         */
         break;



      case CC_INFO_IND:
         /* ignore this message, unles a host interface is present.
         */
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         break;
      case CC_PROGRESS_IND:
#ifdef COMMAND_INTERFACE
            /* inform the HOST.
            */
            if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
               {
               k = 0;
               OutPkgInfoLen++;       /* leave space for totoal elngth */
               if ( CCBRI_AcceptProgress( &prog_desc ) == NOERR )
                  {
                  OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                  OutPkgInfo[ OutPkgInfoLen++ ] = prog_desc;
                  k += 3;
                  }

               if ( CCBRI_AcceptSignal( &signal ) == NOERR )
                  {
                  OutPkgInfo[ OutPkgInfoLen++ ] = SIGNAL_IE;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                  OutPkgInfo[ OutPkgInfoLen++ ] = signal;
                  k += 3;
                  }

               /* total length of private info elements:
               */
               OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
#endif
         break;

#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         break;
#endif
      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = ATT_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_CONNECTED, UNKNOWN_EVENT, *event, Source, FALSE );
         break;

      }

   return( result_code );
}




/*********************
* CCB5E_VoiceDialing *
**********************
*
* Process the current event in the context of a voice call in the
* DIALING state.
*
* This is a state for an active call.
*
*/
PRIVATE int near CCB5E_VoiceDialing( event )

int    *event;                             /* Event input to the state */

{
   int   k = 0;
   int   result_code;
   BYTE  chan_id;
   BYTE  exclusive;
   BYTE  cause_ie;
   int   cause;
   BYTE  prog_desc = 0;
   BYTE  signal = 0;
#ifdef SBC_INTERFACE
   BYTE  event_as_byte;
#endif

   result_code = NOERR;
   cause       = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = ATT_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         break;

#endif
#ifdef SBC_INTERFACE
      case  KP_DIGIT_0:
      case  KP_DIGIT_1:
      case  KP_DIGIT_2:
      case  KP_DIGIT_3:
      case  KP_DIGIT_4:
      case  KP_DIGIT_5:
      case  KP_DIGIT_6:
      case  KP_DIGIT_7:
      case  KP_DIGIT_8:
      case  KP_DIGIT_9:
      case  KP_DIGIT_STAR:
      case  KP_DIGIT_POUND:
         /* Digits dialed by the user are processed here.  We assume
         *  dial tone had been already turned on.  After the first digit
         *  is dialed - the dial tone will be turned off.
         */
         if ( CCB->FirstDigit )
            {
            CCBRI_TRACE( TR_DIALING );
#ifdef TASK_KB
            CCBRI_StartTone( SILENT_TONE );
#endif
            CCB->FirstDigit = FALSE;
            }
         /* the dialed digits will be mailed to the network one by
         *  one as they are dialed (overlap sending).  The network
         *  may send: 1. ALERTING (turn the ringer on)
         *            2. PROGRESS (go to connect mode if not end-to-end ISDN)
         *            3. SETUP_COMP (go to CONNECTED mode)
         */
         if ( CCBRI_GetOutPkt( CC_INFO_REQ, FALSE ))
            {
            /* Send the digit to Layer 3
            */
            /* The following step is required for portability to some
            *  680XX compilers.
            */
            event_as_byte = (BYTE) ( *event );
            OutPkt->InfoLen +=
               CCIE_BuildKeypad_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               &event_as_byte, 1, TRUE );
            CCBRI_MailtoL3();
            }
         break;

      case  KP_ON_HOOK:
         /* The caller hangs up in dialing.  The current call setup
         *  will be terminated by sending of DISCONNECT_REQ.  The new
         *  state will be RELEASING.  In this state any passive calls
         *  (except for calls in CALL WAITING state) will be released
         *  too. Any calls in call waiting state will be moved to
         *  RINGING state.
         */
#ifdef TASK_KB
         CCBRI_StartTone( SILENT_TONE );
         CCBRI_StopDigitDetect();
#endif
         CCB5E_StdVoiceHangup( NORMAL_CLEARING );
         CCB->State = ATT_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         CCB->FirstDigit = TRUE;
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         break;

#endif

      case CC_ALERTING_IND:
         /* C.O. informs us to turn the ringing on.
         *  This message causes us to leave the present state
         *  so that continued inappropriate dialing by the USER
         *  is not conveyed to L3.
         *  NOTE: channel ID info element may be mandatory here
         *  under certain conditions.  We will not check for it.
         */
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_ALERTING ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         CCB->FirstDigit = TRUE;
         /* start ringback tone here:
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
         CCBRI_StartTone( RINGBACK_TONE );
#endif
         CCB->State = ATT_TONE;
         CCBRI_TRACE( TR_RINGBACK );
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         /* The RELEASE_CONF message will be received if L3 received
         *  RELEASE_COMP in State 1.
         *  The RELEASE_IND message may be received in any state.
         *  Either should clear the CCB associated with this call only
         *  (any calls in passive states will not be affected by this
         *  these messages).
         */
         /* NOTE: the cause info element is mandatory in these messages
         *        as they are the first clearing messages.
         */
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
#ifdef SBC_INTERFACE
         Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
#endif
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
         CCBRI_StartTone( SILENT_TONE );
         CCBRI_StopDigitDetect();
#endif
         CCB5E_ReleaseCall();
         break;

      case CC_DISCONNECT_IND:
#ifdef COMP_TEST
         if ( SuspendDiscon )
            break;                                 /* To match Test 23 */
#endif
         /* Far end disconnect during dialing.  Send RELEASE_REQ
         *  and go to state RELEASING. Any possible calls in passive
         *  states will be treated there.
         */
         /* CAUSE is a mandatory info element here - check
         *  for it first:
         */
         if ( ( cause = CCBRI_AcceptCause( &cause_ie ) ) == NOERR )
            {
#ifdef COMMAND_INTERFACE
            /* inform the HOST.
            */
            CCBRI_RejectHostCall( cause );
#endif
            if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
               {
               /* Build Cause info element (optional in this message)
               */
               OutPkt->InfoLen +=
                  CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  NORMAL_CLEARING, FALSE, 0 );
               CCBRI_MailtoL3();
               }
            CCB->State = ATT_RELEASING;
            }
         else
            {
            CCBRI_RejectMsg( MANDATORY_IE_MISSING, 0 );
            }
#ifdef TASK_KB
         CCBRI_StartTone( SILENT_TONE );
         CCBRI_StopDigitDetect();
         CCBRI_StopConnect( CCB->B_Chan );
#endif
         break;

      case CC_CALL_PROC_IND:
         /* This message causes us to leave the present state
         *  so that continued inappropriate dialing by the USER
         *  is not conveyed to L3.
         *  NOTE: channel ID info element may be mandatory here
         *  under certain conditions.  We will not check for it.
         *  We will simply try to get the Channel ID.
         */
         if ( CCBRI_AcceptChanId( &chan_id, &exclusive ) == NOERR)
            {
            CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
            }

#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_PROCEEDING ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         CCB->State = ATT_TONE;
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
#ifdef COMP_TEST
         /* NOTE: taken out at AT&T (2/8/90) by GSD, caused problems
         *  with call progress message !!!
         *  NOTE ALSO: Restored as a COMP_TEST option (07/06/90, VNB) as
         *  required for AT&T 5E4.2 Compatability Test Plan, Illegal Message
         *  Test 16.
         */
         CCB->State = ATT_TONE;
#endif
         break;

      case CC_PROGRESS_IND:
#ifdef COMP_TEST
         break;   /* We do not want to progress to the CONNECTED state */
#else
         /* the PROGRESS message must have the PROGRESS_INDICATOR info
         *  element.  The progress description in this info element will
         *  indicate whether this is an end-to-end ISDN call.  If NOT -
         *  further call progress information will be provided in-band.
         *  In such case, the B channel will be connected immediatly.
         *  If this is an end-to-end ISDN call, this message will have
         *  no effect.
         */
         /* while testing at AT&T: the progress is sent to indicate:
         *        BUSY (signal - congestion tone on)
         *        INTERDIGIT TO (signal - congestion tone on)
         *  Eventualy, we need to read the signal IE and turn proper
         *  tone on (also to turn the voice path on if non end-to-end
         *  ISDN call).  For the time being, we simply turn the voice path
         *  on whenever this message is received (and we have a B channel).
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
#ifdef COMMAND_INTERFACE
            /* inform the HOST.
            */
            if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
               {
               k = 0;
               OutPkgInfoLen++;       /* leave space for totoal elngth */
               if ( CCBRI_AcceptProgress( &prog_desc ) == NOERR )
                  {
                  OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                  OutPkgInfo[ OutPkgInfoLen++ ] = prog_desc;
                  k += 3;
                  }

               if ( CCBRI_AcceptSignal( &signal ) == NOERR )
                  {
                  OutPkgInfo[ OutPkgInfoLen++ ] = SIGNAL_IE;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                  OutPkgInfo[ OutPkgInfoLen++ ] = signal;
                  k += 3;
                  }

               /* total length of private info elements:
               */
               OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
#else
         if ((CCB->B_Chan != 0) &&
            (CCB->B_Chan != UNDEFINED_B_CHANNEL))
            {
                  /*
                   *  Note that a Call Progress generally means that the
                   *  call is leaving the ISDN - we must connect the B
                   *  channel to receive in-band tones.  Above we have
                   *  notified the Host
                   */
#ifdef TASK_KB

#endif

#ifdef TASK_KB
            CCBRI_StartConnect( CCB->B_Chan );
#endif
            }
         else
            {
            /* may have to unpack signal here and turn a proper
            *  tone on.
            */
            ;
            }
#endif
         break;
#endif

      case CC_SETUP_COMP_IND:
      case CC_SETUP_CONF:
         /* The call was connected at the far end.  Connect the call
         *  at this end too.  First we will stop the ringback tone
         *  by providing silent tone.  Next, the proper B
         *  channel will be connected and the call will transfer to
         *  state CONNECTED.  Note that this will occur only if the
         *  current call is an active call.  If it is not, no action
         *  will be taken (however, this should never happen).
         */
         /* NOTE: these messages correspond to CONNECT_ACK and CONNECT
         *  messages (respectivelly) and they do not have mandatory info
         *  elements.
         */
         if (( cause = CCBRI_AcceptChanId( &chan_id, &exclusive )) != NOERR)
            {
            if ( cause != MANDATORY_IE_MISSING)
               {
               CCB5E_ReleaseCall();
               CCB->State = ATT_RELEASING;
#ifdef COMMAND_INTERFACE
               if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
                  {
                  /* total length of private IE's = 0
                  */
                  OutPkgInfo[ OutPkgInfoLen++ ] = 0;
                  CCBRI_MailtoHost( CCB->CopyPublicMsg );
                  CCBRI_RemoveCCB( CCB );
                  }
#endif
               }
            }
         else
            {
            if (chan_id != 0)
               CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
#ifdef TASK_KB
            CCBRI_StartTone( SILENT_TONE );
#endif
            if (
#ifdef SBC_INTERFACE
               ( CCB == Hcb[ DEFAULT_DSL ].Ccb_In_Use[ Hcb[ DEFAULT_DSL ].Active_Call ] ) &&
#endif
               ( CCB->B_Chan != UNDEFINED_B_CHANNEL) &&
               ( CCB->B_Chan != 0))
               {
#ifdef TASK_KB
               CCBRI_StartConnect( CCB->B_Chan );
               CCBRI_StopDigitDetect();
#endif
#ifdef COMMAND_INTERFACE
            /* inform the HOST.
            */
               if ( CCBRI_GetOutInfo( HOST_CONNECT ) )
                  {
                  /* total length of private IE's = 4.
                  */
                  OutPkgInfo[ OutPkgInfoLen++ ] = 4;
                  OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 2;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;   /* exclusive */
                  OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
                  CCBRI_MailtoHost( CCB->CopyPublicMsg );
                  }
#endif
               CCB->State = ATT_CONNECTED;
               CCBRI_TRACE( TR_CONVERSATION );
               }
            }
         break;

      case CC_SETUP_IND:
         /* The SETUP_IND should never be received in this state.
         *  The network cannot attempt to set up a new incoming call
         *  while an outgoing call is in process.  We will reject
         *  this setup message.
         */
         /* NOTE: we do not check for mandatory info elements
         *  here since the setup attempt will be ignored.
         */
         break;

      case CC_INFO_IND:
         /* ignore this message, unles a host interface is present.
         */
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         break;

#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         break;

#endif
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = ATT_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_DIALING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }
   TheTrash = signal;                            /* eliminate warnings */
   TheTrash = prog_desc;
   TheTrash = k;
   return( result_code );
}




/******************
* CCB5E_VoiceIdle *
*******************
*
* Process the current event in the context of a voice call in the
* IDLE state.
*
* This is a state for an active call.
*
*/
PRIVATE int near CCB5E_VoiceIdle( event )

int    *event;                             /* Event input to the state */

{
   int     result_code;
   int     cause;
#ifdef SBC_INTERFACE
   BYTE   *ldn;
   int     our_usid;
   int     our_tid;
   int     our_inter;
#endif

   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         /* acknowledge the disconnect and free CCB
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( FALSE );
            }
         CCBRI_RemoveCCB( CCB );
         CCBRI_TRACE( TR_RELEASING );
         break;

      case HOST_DISCONNECT_ACK:
         /* should not happen - if it does, free the CCB
         */
         CCBRI_RemoveCCB( CCB );
         CCBRI_TRACE( TR_IDLE );
         break;

      case HOST_CALL_REQUEST:
         cause = CCBRI_HostCallRequest();
         if (cause != NOERR)
            {
            CCBRI_RejectHostCall (cause);
            CCBRI_RemoveCCB( CCB );
            }
         else
            {
            CCB->State = ATT_TONE;
            CCBRI_TRACE( TR_DIALTONE );
            }
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         break;
#endif

#ifdef SBC_INTERFACE
      case  KP_OFF_HOOK:
         /* This is a new outgoing call setup. The CC will send
         *  SETUP request to the network.  The call will transition
         *  TONE state.  In this state we expect to receive MORE_INFO_IND
         *  (this is a request for digits to be dialed) or SETUP_CONF
         *  (if the call may be connected without any additional
         *  data.
         */
         CCB->FirstDigit = TRUE;
         if ( CCBRI_GetOutPkt( CC_SETUP_REQ, FALSE ) )
            {
            CCBRI_SetNewChan( ANY_CHANNEL, CHANNEL_PROPOSED, FALSE );
            OutPkt->InfoLen +=
               CCIE_BuildBearerCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               Hcb[ Dsl_ID ].Bearer_Data,
               MAX_BEARER_DATA_SIZE );
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               CCB->B_Chan, 0, B_CHANNEL_UNITS, FALSE, FALSE, 0, NULL );
#ifdef COMP_TEST
            if ( EnBlocSetup )    /* Add enbloc digits to the message. */
               OutPkt->InfoLen +=
                  CCIE_BuildKeypad_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  &EnBlocDigits[0], 7, TRUE );
#endif                                                    /* COMP_TEST */
            OutPkt->InfoLen +=
               CCIE_BuildHookState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               OFFHOOK);
            if ((ldn = gpGetLdn()) != NULL)
               {
               OutPkt->InfoLen +=
                  CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  CALLER_NUMBER_IE, LOC_NUMBER, ISDN_NUMB_PLAN, ldn,
                  strlen( ldn ), FALSE );
               }
            OutPkt->InfoLen +=
               CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               CODESET_6, TRUE);
            OutPkt->InfoLen +=
               CCIE_BuildCallAppear_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               CCB->CallAppearance);
            /* build an Endpoint Identification IE here if applicable:
            */
            if (gpGetEpid(&our_usid, &our_tid, &our_inter))
               {
               OutPkt->InfoLen +=
                  CCIE_BuildEndpointId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  our_usid, our_tid, our_inter );
               }
            CCBRI_MailtoL3();
            }
         Hcb[ Dsl_ID ].Hw_State = OFFHOOK;
#ifdef TASK_KB
         CCBRI_StartTone( SILENT_TONE );
#endif
         CCB->State = ATT_TONE;
         break;

      case KP_ON_HOOK:
         /* It is unclear why the CC should receive ON HOOK in idle
         *  state.  However, a proper thing to do if it does happen
         *  is to simply release the CCB.  There is no need to
         *  initiate a hangup procedure since L3 should be in
         *  NULL state ( state 0) at this point.
         */
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         CCB5E_ReleaseCall();
         break;
#endif

      case CC_SETUP_IND:
         CCB5E_SetupInd();
         break;

      case CC_INFO_IND:
      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
         /* Invalid messages - clear the CCB associated with this
         *  call. Do not send any response to Layer 3.
         */
#ifdef COMMAND_INTERFACE
         /* inform the HOST
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
#ifdef SBC_INTERFACE
         Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
#endif
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
         CCB5E_ReleaseCall();
         break;

#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         break;

#endif
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         if ( CCB )
            CCB5E_ReleaseCall();
         break;

      default:
         if ( Source == L3_ID )
            {
            CCB5E_ReleaseCall();
            CCBRI_Error( CCBRI_CALL_IDLE, UNKNOWN_EVENT, *event, Source, FALSE );
            }
#ifdef COMMAND_INTERFACE
         else if ( Source == HOST_ID )
            {
            CCBRI_RejectHostMsg( 0 );                  /* special case */
            CCBRI_RemoveCCB( CCB );                      /* remove CCB */
            }
#endif
         break;
      }

   return( result_code );
}





/***********************
* CCB5E_VoiceReleasing *
************************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_IND) state.
*
*/
PRIVATE int near CCB5E_VoiceReleasing( event )

int    *event;                             /* Event input to the state */

{
   int  result_code;
   BYTE cause_ie;

   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
      case HOST_DISCONNECT_ACK:
         /* process the disconnect message from the HOST.
         */
                                    /* CCBRI_HostDisconnect( *event ); */
         CCB->State = ATT_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif

#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* the local end hangs up.  Note that this could only occur
         *  after the far end disconnect (otherwise the phone must have
         *  already been off hook).  There is no need to send any
         *  disconnect messages in this case.  We simply get rid of the
         *  CCB for the active call.  Any possible passive HELD calls
         *  will also be disconnected.
         */
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         CCB5E_ReleaseCall();
         break;
#endif

      case CC_RELEASE_IND:
         /* local disconnect - response to DISCONNECT message.  Note
         *  that in the case of local disconnect we will free the
         *  CCB's in HELD state too.  ReleaseCall procedure will
         *  do it automatically.
         */
         /* NOTE: there are no mandatory info elements in this message.
         */
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
#ifdef SBC_INTERFACE
         if ( ( CCB == Hcb[ Dsl_ID ].Ccb_In_Use[ Hcb[ Dsl_ID ].Active_Call ] ) &&
            ( Hcb[ Dsl_ID ].Hw_State == OFFHOOK ) )
            {
            CCB->State = ATT_IDLE;                /* set state to IDLE */
            CCB->Call_ID = CCBRI_GetNewCall_Id();
            *event = KP_OFF_HOOK;    /* set internal event to OFF HOOK */
            result_code = EVENT_PENDING;
            }
         else
#endif
            {
            CCB5E_ReleaseCall();
            }
         break;

      case CC_RELEASE_CONF:
         /* far end disconnect - response to RELEASE message.  Note
         *  that in the case of far end disconnect we will not free
         *  the passive CCB's that may be in the HELD state.  We will
         *  also NOT connect any calls that might be in Call Waiting
         *  state.  Any possible passive calls will not be changed
         *  by the far end disconnect as long as the local user is
         *  off hook.
         *  The following actions will be taken in the case of far end
         *  disconnect:
         *      1. the local user will be given dial tone and may
         *         start a new call.
         *      2. any new incoming call will be treated as call
         *         waiting call setup.
         */
         /* NOTE: there are no mandatory info elements in this message.
         */
#ifdef COMMAND_INTERFACE
         /*
          * added the following message to be sent to the host,
          * not properly disconnecting the call ===
          */
          if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
             {
             /* total length of private IE's = 0
			 */
             OutPkgInfo[ OutPkgInfoLen++ ] = 0;
             CCBRI_MailtoHost( CCB->CopyPublicMsg );
           }
         CCB5E_ReleaseCall();
#endif
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
#ifdef SBC_INTERFACE
         if ( ( CCB == Hcb[ Dsl_ID ].Ccb_In_Use[ Hcb[ Dsl_ID ].Active_Call ] ) &&
            ( Hcb[ Dsl_ID ].Hw_State == OFFHOOK ) )
            {
#ifdef COMP_TEST
            CCB5E_ReleaseCall();
#else
#ifdef SEND_TR_MSG              /* This is some other demo invironment */
            CCBRI_TRACE( TR_LOCKOUT );
#else
            /* give dial tone and enable a new call setup by setting the
            *  CCB state to idle and by setting the internal event to
            *  KP_OFF_HOOK.  Note that call_id in the CCB must be updated
            *  (this will be a new call setup). This CCB continues
            *  to be the active CCB.
            */
            CCB->State = ATT_IDLE;            /* set state to ATT_IDLE */
            CCB->Call_ID = CCBRI_GetNewCall_Id();
            *event = KP_OFF_HOOK;    /* set internal event to OFF HOOK */
            result_code = EVENT_PENDING;
#endif
#endif
            }
#endif
         break;

      case CC_DISCONNECT_IND:
         /* far end disconnect while waiting for a response to
         *  DISCONNECT message.  The response to this message
         *  will be treated in this state - see CC_RELEASE_CONF.
         */
         /* CAUSE is a mandatory info element here - check
         *  for it first:
         */
         if ( CCBRI_AcceptCause( &cause_ie ) == NOERR )
            {
#ifdef COMMAND_INTERFACE
            /* inform the HOST
            */
            CCBRI_RejectHostCall( (int) cause_ie );
#endif
            if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
               {
               /* Build Cause info element (optional in this message)
               */
               OutPkt->InfoLen +=
                  CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  NORMAL_CLEARING, FALSE, 0 );
               CCBRI_MailtoL3();
               }
            }
         else
            {
            CCBRI_RejectMsg( MANDATORY_IE_MISSING, 0 );
            }
         break;



      case CC_INFO_IND:
         /* ignore this message.
         */
#ifdef COMMAND_INTERFACE
         /* inform the HOST
         */
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         break;
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         break;

#endif
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = ATT_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_DIALING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( result_code );
}


/*********************
* CCB5E_VoiceRinging *
**********************
*
* Process the current event in the context of a voice call in the
* RINGING state.
*
* This is a state for an active call.
*
*/
PRIVATE int near CCB5E_VoiceRinging( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   BYTE chan_id;
   BYTE exclusive;
   BYTE cause_ie;
   PKT_ID  loc_pkt;                  /* Used to test for T313 Timeouts */

   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      /* process the connect message from the HOST.
      */
      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = ATT_RINGING;
         CCBRI_TRACE( TR_CONVERSATION );
         break;

      /* process the Alerting message from the HOST.
      */
      case HOST_ALERTING:
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
            {
            /* Add CHANNEL ID info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CHANNELS_LISTED, /* chan selection */
                                    CCB->B_Chan,
                                    B_CHANNEL_UNITS,   /* type: H or B */
                                    FALSE,               /* slot map ? */
                                    FALSE,              /* exclusive ? */
                                    0,          /* interface id length */
                                    NULL );            /* interface id */

            CCBRI_MailtoL3();
            }
         CCB->State = ATT_RINGING;
         break;

      case HOST_DISCONNECT:
      case HOST_DISCONNECT_ACK:
         /* process the disconnect message from the HOST.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = ATT_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         break;
#endif

#ifdef SBC_INTERFACE
      case  KP_OFF_HOOK:
         /* local off hook - this will answer the incoming call
         *  by sending SETUP_RESPONSE to the distant end.
         *  This message will include channel ID and switchhook
         *  state info elements.
         *  The response to this message (SETUP_COMP_IND) will be
         *  received in this state too.
         */
         if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ) )
            {
            /* we will ALWAYS send channel ID info element with
            *  this message (i.e. with CONNECT message, as this
            *  SETUP response message becomes a CONNECT message at
            *  Layer 3.
            */
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               CCB->B_Chan, 0, B_CHANNEL_UNITS, FALSE, FALSE, 0, NULL );
            OutPkt->InfoLen +=
               CCIE_BuildHookState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               OFFHOOK);
            CCBRI_MailtoL3();
            }
         Hcb[ Dsl_ID ].Hw_State = OFFHOOK;       /* no change in state */
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         break;

      case  KP_ON_HOOK:
         /* local disconnect (response to this message will be
         *  received in RELEASING state - see CC_RELEASE_IND).
         *  Note that this on hook  message received here means
         *  that the user hung up before the SETUP_COMP message
         *  was received from the network.
         */
         CCB5E_StdVoiceHangup( NORMAL_CLEARING );
         CCB->State = ATT_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         break;
#endif

#ifdef ICLID
      case RING_CALL_REJECT:
         /* call reject for MICOM ICLID feature
         */

         if ( CCBRI_GetOutPkt( CC_DISCONNECT_REQ, FALSE ) )
            {
            /* Build mandatory Cause element - normal clearing
            */
            OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               CALL_REJECTED, FALSE, 0 );
            /* Build optional Switchhook State element - onhook
            */
            OutPkt->InfoLen +=
               CCIE_BuildHookState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               ONHOOK_IDLE);
            CCBRI_MailtoL3();
#ifdef TASK_KB
            CCBRI_StopRing( VOICE_ALERTING );
#endif
            CCB->State = ATT_RELEASING;
            }
         break;
#endif
      case CC_DISCONNECT_IND:
         /* distant end disconnect (response to this message will
         *  be received in RELEASING state - see CC_RELEASE_CONF):
         */
         /* CAUSE is a mandatory info element here - check
         *  for it first:
         */
#ifdef COMP_TEST
         if ( SuspendDiscon )
            break;
#endif
         if ( CCBRI_AcceptCause( &cause_ie ) == NOERR )
            {
#ifdef COMMAND_INTERFACE
            CCBRI_RejectHostCall( (int) cause_ie );
#endif
            if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
               {
               /* Build Cause info element (optional in this message)
               */
               OutPkt->InfoLen +=
                  CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  NORMAL_CLEARING, FALSE, 0 );
               CCBRI_MailtoL3();
               }
            CCBRI_TRACE( TR_RELEASING );
#ifdef SBC_INTERFACE
            Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
#endif
            CCB->State = ATT_RELEASING;
            }
         else
            {
            if ( CCBRI_GetOutPkt( CC_CHNG_STATE_REQ, FALSE ))
              {
              OutPkt->Info[ OutPkt->InfoLen++ ] = L3_State;
              CCBRI_MailtoL3();          /* Mail the change state msg. */
              }
            if ( CCBRI_GetOutPkt( CC_DISCONNECT_REQ, FALSE ) )
               {
              /* Build mandatory Cause element - normal clearing
              */
               OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  MANDATORY_IE_MISSING, FALSE, 0 );
               CCBRI_MailtoL3();
               CCB->State = ATT_RELEASING;
               }
            }
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         break;

      case CC_RELEASE_CONF:
         /* The RELEASE_CONF message will be received if L3 received
         *  RELEASE_COMP in State 12 when the call has been left in this
         *  state in CC due to the SuspendDiscon control.
         */
      case CC_RELEASE_IND:
         /* RELEASE_IND message may be received in any state.  It will
         *  clear the CCB associated with this call only (any calls
         *  in passive states will not be affected by this message).
         */
         /* NOTE: there are no mandatory info elements in this message.
         */
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
#ifdef SBC_INTERFACE
         Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
#endif
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         CCB5E_ReleaseCall();
         break;

      case CC_SETUP_COMP_IND:
         /* This is the network's response to SETUP_RESP sent from
         *  this state when the user goes off hook.  We will
         *  connect the call only if this is an active call CCB.
         *  If this is not an active call - this message will be
         *  ignored.
         *
         *  *** FFS *** - should we send a disconnect if this is not an
         *        active call CCB?  This clearly is an error condition.
         */
         /* NOTE: there are no mandatory info elements in CONNECT_ACK
         *  (or SETUP_COMP_IND) message.
         */
         /* Here we test for T313 timeouts, which come to us as
         *  CC_SETUP_COMP_IND as well.
         */
         loc_pkt = InPkt;
         if ( loc_pkt->Info[0] == CONNECT_ACK )
            {
            if ( CCBRI_AcceptChanId( &chan_id, &exclusive ) == NOERR)
               {
               CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
               }
#ifdef COMMAND_INTERFACE
            if ( CCBRI_GetOutInfo( HOST_CONNECT ) )
               {
               /* total length of private IE's = 4
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 4;
               OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 2;
               OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;      /* exclusive */
               OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
#endif

            if (
#ifdef SBC_INTERFACE
                ( CCB == Hcb[ Dsl_ID ].Ccb_In_Use[ Hcb[ Dsl_ID ].Active_Call ] ) &&
#endif
                ( CCB->B_Chan != UNDEFINED_B_CHANNEL ) &&
                ( CCB->B_Chan != 0 ) )
               {
#ifdef TASK_KB
               CCBRI_StartConnect( CCB->B_Chan );
#endif
               CCB->State = ATT_CONNECTED;
               CCBRI_TRACE( TR_CONVERSATION );
               }
            }
         break;

#ifdef COMP_TEST
      case DIGIT_COMP_TEST_5:
         /* This special function key is issued to cause CC to generate
         *  a CC_ALERTING_REQ after the call was suspended when CC sent
         *  CC_CALL_PROC_IND under the QuickProceed control.  This function
         *  is necessary to perform AT&T Compatibility Test 46.  Chan_ID
         *  was already sent in the CC_CALL_PROC_IND message.
         */
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ) )
            CCBRI_MailtoL3();
         Hcb[ DEFAULT_DSL ].Hw_State = BELL;
         break;

      case DIGIT_COMP_TEST_6:
         /* This special function key is issued to cause CC to generate
         *  a CC_DISCONNECT_REQ after the call has entered the alerting
         *  condition.  This conforms with AT&T Test Plan Test 57.
         */
         CCB5E_StdVoiceHangup( NORMAL_CLEARING );
         CCB->State = ATT_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         break;

#endif
      case CC_INFO_IND:
         /* ignore this message.
         */
#ifdef COMMAND_INTERFACE
         /* must inform the host.
         */
         if ( CCBRI_GetOutInfo( HOST_CONNECT ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         break;

#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         break;

#endif
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = ATT_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_DIALING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   /* if the call leaves the ringing state - send a cancel message to
   *  the ICLID driver.
   */
#ifdef ICLID
   if (CCB->State != ATT_RINGING)
      CCB5E_SendICLID( NULL, ACTIVE_ICLID );
#endif

   return( result_code );
}


/******************
* CCB5E_VoiceTone *
*******************
*
* Process the current event in the context of a voice call in the
* TONE state.
*
* This is a state for an active call.
*
*/
PRIVATE int near CCB5E_VoiceTone( event )

int *event;                                /* Event input to the state */

{
   int    result_code;
   BYTE   chan_id = 0;
   BYTE   exclusive;
   BYTE   cause_ie;
   PKT_ID loc_pkt;                   /* Used to test for T303 Timeouts */
   int    cause = NOERR;
   int    k;
   BYTE   prog_desc;
   BYTE   signal;
   int    state;

   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
      case HOST_DISCONNECT_ACK:
         /* process the disconnect message from the HOST.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = ATT_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         break;
#endif

#ifdef SBC_INTERFACE
      case  KP_ON_HOOK:
         /* The caller hangs up in dialing.  The current call setup
         *  will be terminated by sending of DISCONNECT_REQ.  The new
         *  state will be RELEASING.  In this state any passive calls
         *  (except for calls in CALL WAITING state) will be released
         *  too. Any calls in call waiting state will be moved to
         *  RINGING state.
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
         if ( Hcb[ Dsl_ID ].Hw_State == OFFHOOK )
            {
            CCB5E_StdVoiceHangup( NORMAL_CLEARING );
            CCB->State = ATT_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
            CCB->FirstDigit = TRUE;
            Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
            }
         break;

      case KP_HOOK_FLASH:
         /* The user hook flashes in dialing (before  dialing the
         *  digits).  The current call setup will be terminated by
         *  sending of DISCONNECT_REQ (in CCB5E_StdVoiceHangup).  The new
         *  state will be RECONNECTING if there is a passive call in
         *  either HOLD or CALL WAITING states.  In RECONNECTING state,
         *  the passive call will be activated as soon as a RELEASE
         *  confirmation is received for the active call.  If there is
         *  no passive call, the new state will be RELEASING.
         */
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
         CCB5E_StdVoiceHangup( NORMAL_CLEARING );
            CCB->State = ATT_RELEASING;

         CCBRI_TRACE( TR_RELEASING );
         CCB->FirstDigit = TRUE;
         break;
#endif

      case CC_MORE_INFO_IND:
         /* This is a response to SETUP request message mailed
         *  in IDLE state.  This massage will cause change of state
         *  to dialing state. The dial tone will be turned on.
         */
         /* NOTE: this is a SETUP_ACK message from the peer.  The channel ID
         *  info element is a mandatory info element here. Check for it first:
         */
#ifdef SBC_INTERFACE
         Hcb[ Dsl_ID ].Hw_State = OFFHOOK;
#endif
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         if ( ( cause = CCBRI_AcceptChanId( &chan_id, &exclusive) ) == NOERR )
            {
            CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
            /* see if there is a held party
            */
            /* note - turning dial tone or special dial tone on will
            *  always start DTMF receiver (digit detection)
            */
#ifdef TASK_KB
               CCBRI_StartTone( DIAL_TONE );
#endif

            CCB->State = ATT_DIALING;
            CCBRI_TRACE( TR_DIALTONE );
#ifdef SBC_INTERFACE
            Hcb[ Dsl_ID ].Active_Call = CCB->HcbIndex;
#endif
            }
#ifdef COMP_TEST
         else if ( cause == UNACCEPTABLE_CHANNEL )
            {
            CCB5E_StdVoiceHangup( NO_CIRCUIT_AVAILABLE );
            CCB->State = ATT_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
            CCB->FirstDigit = TRUE;
            Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
            }
#else
         Trashcan = cause;                /* Prevent compiler warnings */
#endif
         break;

      case CC_SETUP_COMP_IND:
      case CC_SETUP_CONF:
         /* The call was connected at the far end.  Connect the call
         *  at this end too.  The proper B channel will be connected
         *  and the call will transfer to state CONNECTED.
         *  Note that this will occur only if the current call is an
         *  active call.  If it is not, no action will be taken
         *  (however, this should never happen).
         */
         /* NOTE: these messages correspond to CONNECT_ACK and CONNECT
         *  messages (respectivelly) and they do not have mandatory info
         *  elements.
         *
         *  NOTE: T303 timeouts are handled here by making sure the first
         *  byte of the info buffer indicates the message is from the network
         *  and not a layer 3 timeout
         */
         loc_pkt = InPkt;
         if (( loc_pkt->Info[0] != CONNECT ) &&
            (  loc_pkt->Info[0] != CONNECT_ACK ))
           {
#ifdef COMMAND_INTERFACE
            CCBRI_RejectHostCall ( TEMPORARY_FAILURE );

#endif
#ifdef SBC_INTERFACE
            Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
#endif
#ifdef TASK_KB
            CCBRI_StopConnect( CCB->B_Chan );
#endif
            CCB5E_ReleaseCall();
#ifdef SEND_TR_MSG
            CCBRI_TRACE( TR_LOCKOUT );
#endif
           }
         else
           {
           if ( CCBRI_AcceptChanId( &chan_id, &exclusive ) == NOERR )
              {
              CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
              }
#ifdef COMMAND_INTERFACE
           if ( CCBRI_GetOutInfo( HOST_CONNECT ) )
               {
               /* total length of private IE's = 4
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 4;
               OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 2;
               OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
               OutPkgInfo[ OutPkgInfoLen++ ] = CCB->B_Chan;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
#endif
           CCB->State = ATT_CONNECTED;
           CCBRI_TRACE( TR_CONVERSATION );
#ifdef TASK_KB
            CCBRI_StartTone( SILENT_TONE );
           if (
              ( CCB == Hcb[ Dsl_ID ].Ccb_In_Use[ Hcb[ Dsl_ID ].Active_Call ] ) &&
              ( CCB->B_Chan != UNDEFINED_B_CHANNEL ) &&
              ( CCB->B_Chan != 0 ) )
              {
              CCBRI_StartConnect( CCB->B_Chan );
              }
#endif
           }
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         /* RELEASE_IND message may be received in any state.  It will
         *  clear the CCB associated with this call only (any calls
         *  in passive states will not be affected by this message).
         */
         /* NOTE: there are no mandatory info elements in this message.
         */
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
#ifdef SBC_INTERFACE
         Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
#endif
#ifdef TASK_KB
         CCBRI_StopConnect( CCB->B_Chan );
#endif
         CCB5E_ReleaseCall();
#ifdef SEND_TR_MSG
         IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                                  CAUSE_IE, CODESET_0, 1 );
         if (( IE ) && ( IE[ 3 ] == USER_BUSY ))
            CCBRI_TRACE( TR_BUSY );
#endif
         break;

      case CC_DISCONNECT_IND:
#ifdef COMP_TEST
         if ( SuspendDiscon )
            break;
#endif
         /* Far end disconnect during dialing.  Send RELEASE_REQ
         *  and go to state RELEASING. Any possible calls in passive
         *  states will be treated there.
         */
         /* CAUSE is a mandatory info element here - check
         *  for it first:
         */
         if ( ( cause = CCBRI_AcceptCause( &cause_ie ) ) == NOERR )
            {
#ifdef COMMAND_INTERFACE
            CCBRI_RejectHostCall( (int) cause_ie );
#endif
            if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
               {
               /* Build Cause info element (optional in this message)
               */
               OutPkt->InfoLen +=
                  CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  NORMAL_CLEARING, FALSE, 0 );
               CCBRI_MailtoL3();
               }
            CCB->State = ATT_RELEASING;
            }
         else
            {
            CCBRI_RejectMsg( MANDATORY_IE_MISSING, 0 );
            }
         break;

      case CC_CALL_PROC_IND:
         /*
          * Ignore this message and stay in the present state.
          *
          * NOTE: channel ID info element may be mandatory here
          * under certain conditions (AT&T Test 19). We will simply try to
          * get the Channel ID. But, if the B-channel is in use, then the
          * call will be disconnected.
          */
         if ((cause = CCBRI_AcceptChanId(&chan_id, &exclusive) != NOERR)) {
             CCBRI_RejectMsg(cause, CHANNEL_ID_IE);
             CCB->State = ATT_RELEASING;
         } else {
             state = CCBRI_SetNewChan(chan_id, CHANNEL_BUSY, FALSE);
             if (state != NOERR) {
                CCBRI_RejectCall(REQ_CHANNEL_NOT_AVAIL, NO_DIAG);
                CCB->State = ATT_RELEASING;
             } else {
                 /*
                  * inform the HOST.
                  */
                 if (CCBRI_GetOutInfo(HOST_PROCEEDING)) {
                     /*
                      * total length of private IE's = 4.
                      */
                     OutPkgInfo[ OutPkgInfoLen++ ] = 4;
                     OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
                     OutPkgInfo[ OutPkgInfoLen++ ] = 2;
                     OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;         /* exclusive */
                     OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
                     CCBRI_MailtoHost( CCB->CopyPublicMsg );
                 }
             }
#ifdef COMP_TEST
             cause = CCBRI_AcceptChanId( &chan_id, &exclusive );
             if ( cause == UNACCEPTABLE_CHANNEL ) {
                 CCB5E_StdVoiceHangup( NO_CIRCUIT_AVAILABLE );
                 CCB->State = ATT_RELEASING;
                 CCBRI_TRACE( TR_RELEASING );
                 CCB->FirstDigit = TRUE;
                 Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
             }
#endif
          }
          break;

      case CC_ALERTING_IND:
         /* C.O. informs us to turn the ringing on.
         *  NOTE: channel ID info element may be mandatory here
         *  under certain conditions.  We will not check for it.
         */
         /* start ringback tone here:
         */
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_ALERTING ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
#ifdef TASK_KB
         CCBRI_StartTone( RINGBACK_TONE );
#endif
         CCBRI_TRACE( TR_RINGBACK );
         break;

      case CC_INFO_IND:
         /* ignore this message.
         */
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_CONNECT ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         break;

      case CC_PROGRESS_IND:
#ifdef COMP_TEST
         break;   /* We do not want to progress to the CONNECTED state */
#else
         /* the PROGRESS message must have the PROGRESS_INDICATOR info
         *  element.  The progress description in this info element will
         *  indicate whether this is an end-to-end ISDN call.  If NOT -
         *  further call progress information will be provided in-band.
         *  In such case, the B channel will be connected immediatly.
         *  If this is an end-to-end ISDN call, this message will have
         *  no effect.
         */
         /* while testing at AT&T: the progress is sent to indicate:
         *        BUSY (signal - congestion tone on)
         *        INTERDIGIT TO (signal - congestion tone on)
         *  Eventualy, we need to read the signal IE and turn proper
         *  tone on (also to turn the voice path on if non end-to-end
         *  ISDN call).  For the time being, we simply turn the voice path
         *  on whenever this message is received (and we have a B channel).
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
#ifdef COMMAND_INTERFACE
            /* inform the HOST.
            */
            if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
               {
               k = 0;
               OutPkgInfoLen++;       /* leave space for totoal elngth */
               if ( CCBRI_AcceptProgress( &prog_desc ) == NOERR )
                  {
                  OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                  OutPkgInfo[ OutPkgInfoLen++ ] = prog_desc;
                  k += 3;
                  }

               if ( CCBRI_AcceptSignal( &signal ) == NOERR )
                  {
                  OutPkgInfo[ OutPkgInfoLen++ ] = SIGNAL_IE;
                  OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                  OutPkgInfo[ OutPkgInfoLen++ ] = signal;
                  k += 3;
                  }

               /* total length of private info elements:
               */
               OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
#else
         if ((CCB->B_Chan != 0) &&
            (CCB->B_Chan != UNDEFINED_B_CHANNEL))
            {
#ifdef TASK_KB
#endif
#ifdef TASK_KB
            CCBRI_StartConnect( CCB->B_Chan );
#endif
            }
         else
            {
            /* may have to unpack signal here and turn a proper
            *  tone on.
            */
            ;
            }
#endif
         break;
#endif
#ifdef COMMAND_INTERFACE

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         break;

#endif
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = ATT_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_DIALING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( result_code );
}

/*********************
* CCB5E_ReleasedLink *
**********************
*
* The data link is down at Layer 2.
* Remove CCBs from any context but LOCKOUT.  Users left OFF HOOK
* must retain a CCB in the LOCKOUT state to process their hangup.
*
*/
PUBLIC void CCB5E_ReleasedLink (ces, dsl_id)
BYTE ces;          /* Connection Endpoint Suffix of the released link. */
BYTE dsl_id;                                /* Digital subscriber loop */
{
   CCB_ID tmp_ccb;

   /*
    * Start at the head and remove all impacted CCBs.
    */
   CCB = CCB_Head[dsl_id];
   while (CCB) {
      tmp_ccb = CCB->Next;
      if (CCB->Ces == ces) {
         /* notify the HOST:
         */
         if (CCB->B_Chan)
            CCBRI_ReleaseChan();
         CCBRI_RejectHostCall(TEMPORARY_FAILURE);
         CCBRI_ReleaseCall();
      }
      CCB = tmp_ccb;
   }
}

/**********************
* CCB5E_DisconnectInd *
***********************
*
* Generic action upon receiving the CC_DISCONNECT_IND message
* for a network disconnect.
*
*/
PRIVATE BOOLEAN near CCB5E_DisconnectInd()

{
   BYTE event = 0;
#ifdef COMMAND_INTERFACE
   BYTE cause_ie = 0;
   int cause;
#endif

#ifdef SBC_INTERFACE
#ifdef TASK_KB
      CCBRI_StopConnect( CCB->B_Chan );
#endif
   /* Far end disconnect.  Send CC_RELEASE_REQ. We will retain connection
   *  to the B-Channel.
   */
   if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) )
      {
      CCBRI_MailtoL3();
      CCB->State = ATT_RELEASING;
      }
#endif
#ifdef COMMAND_INTERFACE
      cause = (( CCBRI_AcceptCause (&cause_ie) == MANDATORY_IE_MISSING ) ?
                  MANDATORY_IE_MISSING : cause_ie);
      LIF_UpdateTimer( CCB->CC5essTimer, CC_ID, CC_ID, DISC_HOST_EVENT, Dsl_ID, CCB->Sapi,
                  CCB->Ces, CCB->Call_ID, 0, DISC_HOST_TIMEOUT);
      LIF_StartTimer (CCB->CC5essTimer);
      CCB->State = ATT_DISCONNECTING_HOST;
      CCB->Cause = (int ) cause;
      event       = HOST_DISCONNECT;
      CCBRI_DisconnectHostCall( &event , cause );
#endif
   TheTrash = ( int )event;
   return( TRUE );
}
/*******************
* CCB5E_Shutdown *
********************
*
* Reset Call Control
*
*/
PUBLIC void CCB5E_Shutdown (dsl)
BYTE dsl;
{
    int i;
    CCB_ID temp_ccb;

    Dsl_ID = dsl;
    CCB = CCB_Head[dsl];
    while (CCB) {
        temp_ccb = CCB->Next;
        CCB5E_ReleaseCall();
        CCB = temp_ccb;
    }

    for (i=0; i < MAX_HOSTS; i++)
        LIF_StopTimer (Host_CB[dsl][ i ].Timer_T415 );
}
/* End of module
*/
#endif
