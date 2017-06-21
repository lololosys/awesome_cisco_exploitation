/* $Id: ccbntkd.c,v 3.3.12.1 1996/04/30 23:06:52 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccbntkd.c,v $
 *------------------------------------------------------------------
 * ccbtkd.c  ISDN NTT/KDD BRI Specific Call Control Functions
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccbntkd.c,v $
 * Revision 3.3.12.1  1996/04/30  23:06:52  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/15  17:27:18  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.2  1995/11/17  17:39:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccbtkd.c
VERSION: 3.21
DESCRIPTION: ISDN NTT/KDD BRI Specific Call Control Functions

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

#ifdef SBC_INTERFACE

This version of this module is intended to implement the
Call Control functions for a simple user-side Basic Rate Interface
(Basic Rate Access) telephone in a manner compatible with a NTT or KDD
Central Office switch in accordance with NTT publication "USER-NETWORK
INTERFACE of INS-NETservice" Version 3 June 1990 and KDD's "INTERNATIONAL
ISDN SERVICE" Version 2.0 April 1990.  The telephone is said to be
using subscried in-band call progress tones and does not send information
by way of overlapped sending

This version does not support any supplementary services.
Various hooks have also been added so that the functionality of this
module may be extended with reasonable effort to support various
supplementary services in the future.  Specifically:

 1. A generalized scheme for managing multiple Call Control Blocks
    (CCBs) exists so that multiple passive calls as well as an active
    call may be manipulated simultaneously, although this functionality
    is not used as this module currently exists except to the extent
    required to reject a second setup while busy.
 2. Each state function is capable of returning an indication of a
    secondary "internal" event which is intended as a method for inter-
    call communication in support of supplementary services.  The
    call returning an internal event may write the CCB pointer to
    the CCB of the call it wishes to activate as its last act before
    returning the internal event, which will be the event processed
    by the second call.  As currently implemented, the internal event
    indication is hard coded in the returns of the state functions
    as FALSE.


Various simplifications have been made which reflect the fact that
this is not the software designed to run, as is, a real hardware
device.  For example, the directory number is simply fixed at compile
time, where a real product might be expected to have NV ram for such
volatile information, with a user-programming capability.

#endif

The Call Control is assumed to reside on a point-to-multipoint
interface wherein SETUP is broadcast to the TEs.

#ifdef COMMAND_INTERFACE

This implementation supports Telenetwork's message-based Call Control
which isdesigned to interface with an intelligent device such as a
PC or PBX. This Call Control has been customized to the following
Japanese  specifications:

   1) NTT's USER-NETWORK INTERFACE of INS-NET Service Version 3 (June 1990)
   2) KDD's INTERNATIONAL ISDN SERVICE - Technical Reference for KDD
      International ISDN User-Network Interface Version 2.0 (April 1990)

Supplementary Services are not currently supported by this module.

#endif


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically within general
groupings.


*************************************************************************
*************************************************************************
*/

#define     CCBNTKD_C                                   /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

/* Standard C library functions/definitions
*/
/* """
#include    <stdio.h>
""" */

#include    "master.h"
#include    "address.h"
#include    "flags.h"
#ifdef KDD_NTT
#include    "tune.h"
#include    "gendef.h"
#include    "lif.h"
#include    "ccie.h"
#include	"gp.h"	/*===*/
#include    "ccbri.h"
#include    "ccbntkd.h"
#include    "cc_chan.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/* This macro detects a dialed digit.
*/
#define DIALED_DIGIT(d) \
 (((d)==KP_DIGIT_0) || ((d)==KP_DIGIT_1) || ((d)==KP_DIGIT_2) || \
  ((d)==KP_DIGIT_3) || ((d)==KP_DIGIT_4) || ((d)==KP_DIGIT_5) || \
  ((d)==KP_DIGIT_6) || ((d)==KP_DIGIT_7) || ((d)==KP_DIGIT_8) || \
  ((d)==KP_DIGIT_9) || ((d)==KP_DIGIT_STAR) || ((d)==KP_DIGIT_POUND))



/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
/* This array "Charges" is used to collect the call charge data from the
*  ADVICE_OF_CHARGE_IE.  This data is currently not transported to
*  the USR, but this need is expected in the future.
*/
PRIVATE BYTE    Charges[15];                              /* Call cost */
PRIVATE BYTE   *Current_IE;            /* Pointer to CCIE info element */
#ifdef SBC_INTERFACE
PRIVATE BYTE   *Cpn_P;                /* Pointer to Called Number buff */
PRIVATE BYTE Called_Party_Num [ PHONE_NUMB_LEN +1];  /* buffer for key */
                                                         /* collection */
#endif 
/* **********************************************************************
*              F U N C T I O N   D E C L A R A T I O N S                *
*************************************************************************
*/
PRIVATE   int       near    CCBNTKD_AcceptAdviceOfCharge  (BYTE *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallAlerting        (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallConnected       (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallConnecting      (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallIdle            (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallLockout         (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallOffered         (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallOffHookRelease  (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallOnHookRelease   (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallOriginate       (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallRoutingIn       (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_CallRoutingOut      (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_DigitCollection     (int *);
PRIVATE    BOOLEAN  near    CCBNTKD_DisconnectingHost   (int *);
#ifdef SBC_INTERFACE
PRIVATE    void     near    CCBNTKD_Disconnect          (int);
#endif 
PRIVATE    BOOLEAN  near    CCBNTKD_DisconnectInd       (void);
PRIVATE    void     near    CCBNTKD_RestartChan         (int, BYTE);
PRIVATE    void     near    CCBNTKD_SetupInd            (void);

/************************************************************************
*             S T A T E   E N T R Y   J U M P   T A B L E               *
*************************************************************************
*/

typedef BOOLEAN (near *CC_STATES)( int * );

/* NOTE: this table MUST strictly correspond to the CCB_State table in
*  ccbntkd.h
*/
static const CC_STATES Ccb_State_Func_Ntkd[ NTKD_STATE_HIGH_LIMIT ] =
   {
   NULL,
   CCBNTKD_CallIdle,
   CCBNTKD_CallAlerting,
   CCBNTKD_CallConnected,
   CCBNTKD_CallConnecting,
   CCBNTKD_CallLockout,
   CCBNTKD_CallOffHookRelease,
   CCBNTKD_CallOnHookRelease,
   CCBNTKD_CallOffered,
   CCBNTKD_CallOriginate,
   CCBNTKD_CallRoutingIn,
   CCBNTKD_CallRoutingOut,
   CCBNTKD_DigitCollection,
   CCBNTKD_DisconnectingHost
   };


/************************************************************************
*                C A L L   S T A T E   F U N C T I O N S                *
*************************************************************************
*/

/***********************
* CCBNTKD_CallAlerting *
************************
*
* Process the current event in the context of a voice/data call in the
* RINGING state.  The User's ISDN telephone could be RINGING or
* the Host could have sent back HOST_ALERTING.  Corresponds to State 7
* of Q.931
*/
PRIVATE BOOLEAN near CCBNTKD_CallAlerting( event )

int *event;                                /* Event input to the state */

{

   switch ( *event )
      {
      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = NTKD_CONNECTING;
         break;

#ifdef SBC_INTERFACE
      case KP_OFF_HOOK:
         /* Local off hook - we will answer the incoming call
         *  by sending CC_SETUP_RESP (CONNECT) downwards.
         *  The response to this message (CC_SETUP_COMP_IND) will be
         *  received in the CONNECTING state to complete the connection.
         */
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ) )
            CCBRI_MailtoL3();
         Hcb[ DEFAULT_DSL ].Hw_State = OFFHOOK;
         CCB->State = NTKD_CONNECTING;
         CCBRI_TRACE( TR_RINGING );
         break;

#endif
      case CC_DISCONNECT_IND:
         /* Distant end disconnect (response to this message will
         *  be received in ONHOOK_RELEASING state - see CC_RELEASE_CONF):
         */
#ifdef TASK_KB
           CCBRI_StopRing( VOICE_ALERTING );
#endif
#ifdef SBC_INTERFACE
            Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;     /* Not BELL */
            CCB->State = NTKD_ONHOOK_RELEASING;
#endif
#ifdef COMMAND_INTERFACE
            CCBNTKD_DisconnectInd();
#endif
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
#ifdef SBC_INTERFACE
         CCBNTKD_AcceptAdviceOfCharge( Charges );
#endif
         CCBNTKD_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         /* Unexpected message.
         *  Note that L3 logic is designed to catch these cases in general
         *  before we receive them, but the inopportune arrival of some
         *  messages may not be detected in L3 because they may not be
         *  inopportune for all applications.
         */
         if ( ( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ) )
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = NTKD_ONHOOK_RELEASING;
            }
         CCBRI_Error( CCBRI_CALL_ALERTING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/*************************
* CCBNTKD_CallConnected  *
**************************
*
* Process the current event in the context of a voice/data  call in the
* CONNECTED state.  Corresponds to State 10 of Q.931.
*
*/
PRIVATE BOOLEAN near CCBNTKD_CallConnected( event )

int *event;                                /* Event input to the state */

{
   BYTE     cause;                 /* The cause value for bad messages */
   BYTE    event_b;              /* event value for Disconnecting Host */

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_OFFHOOK_RELEASING;
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* Local disconnect (response to this message will be
         *  received in RELEASING state - see CC_RELEASE_IND):
         */
         CCBNTKD_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NTKD_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_DISCONNECT_IND:                    /* Far end disconnect */
         CCBNTKD_DisconnectInd();
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
         CCBRI_AcceptCause( &cause );
#ifdef SBC_INTERFACE
         CCBNTKD_AcceptAdviceOfCharge( Charges );
         CCB->State = NTKD_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         event_b = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall(&event_b, cause);
         CCBRI_RemoveCCB( CCB );
         CCBNTKD_AcceptAdviceOfCharge( Charges );
#endif
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
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
         if ( ( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ) )
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = NTKD_OFFHOOK_RELEASING;
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

   return( FALSE );
}

/*************************
* CCBNTKD_CallConnecting *
**************************
*
* Process the current event in the context of a voice/data  call in the
* CONNECTING state, that is, the user has sent CONNECT to the Network.
* Corresponds to State 8 of Q.931.
*/
PRIVATE BOOLEAN near CCBNTKD_CallConnecting( event )

int *event;                                /* Event input to the state */

{
   BYTE    cause;                  /* The cause value for bad messages */
   BYTE    event_b;
   PKT_ID  loc_pkt;                  /* Used to test for T313 Timeouts */

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

#endif
      case CC_SETUP_COMP_IND:            /* Translation of CONNECT_ACK */
         /* The call was connected at the far end.  Connect the call
         *  at this end too (its probably already connected).
         *  The call progresses to the CONNECTED state.
          (  Note that a T313 timeout must be handled here, that is,
            L3 sends T313 Timeouts to us as CC_SETUP_COMP_IND.
            So we look at the first byte of the info buffer to see if
            it is really a CONNECT_ACK )                         */

          loc_pkt = InPkt;
          if ( loc_pkt->Info[0] == CONNECT_ACK )
             {
#ifdef TASK_KB
             if ( CCB->B_Chan )
                CCBRI_StartConnect( CCB->B_Chan );
#endif
             CCBRI_TRACE( TR_CONVERSATION );
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
                OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;     /* exclusive */
                OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
                CCBRI_MailtoHost( CCB->CopyPublicMsg );
                CCB->State = NTKD_CONNECTED;
                }
#endif
             }
           else
              {
              CCB->State = NTKD_OFFHOOK_RELEASING;
              }
         break;

#ifdef COMMAND_INTERFACE
      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_OFFHOOK_RELEASING;
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* Local disconnect (response to this message will be
         *  received in RELEASING state - see CC_RELEASE_IND):
         */
         CCBNTKD_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NTKD_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_DISCONNECT_IND:                    /* Far end disconnect */
         CCBNTKD_DisconnectInd();
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
         CCBRI_AcceptCause( &cause );

#ifdef SBC_INTERFACE
         CCBNTKD_AcceptAdviceOfCharge( Charges );
         CCB->State = NTKD_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
          event_b = HOST_DISCONNECT_ACK;
          CCBRI_DisconnectHostCall(&event_b, cause);
          CCBRI_RemoveCCB( CCB );
#endif
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
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
         if ( ( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ) )
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = NTKD_OFFHOOK_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_CONNECTING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/*******************
* CCBNTKD_CallIdle *
********************
*
* Process the current event in the context of a voice/data call in the
* IDLE state.
*
*/
PRIVATE BOOLEAN near CCBNTKD_CallIdle( event )

int *event;                                /* Event input to the state */

{
   int cause;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_CALL_REQUEST:
         if ( ( cause = CCBRI_HostCallRequest() ) != NOERR )
            {
            if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
               {
               /* total length of private IE's = 3.
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 3;
               OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;
               OutPkgInfo[ OutPkgInfoLen++ ] = ( BYTE ) cause;
               CCBRI_MailtoHost( FALSE );
               }
            CCBRI_RemoveCCB( CCB );
            }
         else
            {
            CCB->State = NTKD_ORIGINATE;
            }
         break;

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

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_OFF_HOOK:                               /* Outgoing call */
         /* The off-hook puts the phone in Digit Collection state.  Since
         *  overlap sending is not supported, we will collect digits until
         *  a special function key is pushed.
         */
         Hcb[ DEFAULT_DSL ].Hw_State = OFFHOOK;
         CCB->State = NTKD_DIGIT_COLLECT;
#ifdef TASK_KB
         CCBRI_StartTone(DIAL_TONE);
#endif
         memset(Called_Party_Num,'\0', PHONE_NUMB_LEN + 1);
         Cpn_P = Called_Party_Num;
         LIF_UpdateTimer( CCB->NtkdTimer, CC_ID, CC_ID, DIGIT_COLLECT_TIMEOUT,
                    Dsl_ID, CCB->Sapi, CCB->Ces, CCB->Call_ID, 0, DIGIT_COLLECT_DEFAULT);
         LIF_StartTimer (CCB->NtkdTimer);
         break;
#endif
      case CC_SETUP_IND:
         CCBNTKD_SetupInd();                          /* Incoming call */
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         if ( CCB )
            CCBNTKD_ReleaseCall();
         break;

      default:
         /* Unexpected message.
         *  Note that L3 logic is designed to catch these cases in general
         *  before we receive unexpected messages, but the inopportune
         *  arrival of some messages may not be detected in L3 because
         *  they may not be inopportune for all applications.
         */
         if ( Source == L3_ID )
            {
            CCBNTKD_ReleaseCall();
            CCBRI_Error( CCBRI_CALL_IDLE, UNKNOWN_EVENT, *event, Source, FALSE );
            }
#ifdef COMMAND_INTERFACE
         else if ( Source == HOST_ID )
            {
            CCBRI_RejectHostMsg( 0 );                  /* special case */
            CCBRI_RemoveCCB( CCB );                    /* remove CCB */
            }
#endif
         break;
      }

   return( FALSE );
}

/**********************
* CCBNTKD_CallLockout *
***********************
*
* Process the current event in the context of a voice/data call in the
* LOCKOUT (off-hook, but released from the NETWORK) state.  The user
* is "locked-out" from originating again until an on-hook/off-hook
* cycle is completed.  This state is retained after the NETWORK
* has released the call and the user has failed to hang up.
*
*/
PRIVATE BOOLEAN near CCBNTKD_CallLockout( event )

int *event;                                /* Event input to the state */

{
   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      /* NOTE: this is not a valid state for COMMAND_INTERFACE case.
      *        No messages from HOST are expected here.
      */
#endif

#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCBNTKD_ReleaseCall(); /* Release from lockout, drop the CCB. */
         break;
#endif

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


/**********************
* CCBNTKD_CallOffered *
***********************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to State 6 of Q.931.
*
*/
PRIVATE BOOLEAN near CCBNTKD_CallOffered( event )

int *event;                                /* Event input to the state */

{
   int cause;

   switch ( *event )
      {
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

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
            CCBRI_RejectSetup( cause, 0 );
            }
         CCBNTKD_ReleaseCall();
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
                               CHANNELS_LISTED,   /* channel selection */
                               CCB->B_Chan,
                               B_CHANNEL_UNITS,        /* type: H or B */
                               FALSE,                    /* slot map ? */
                               FALSE,                   /* exclusive ? */
                               0,               /* interface id length */
                               NULL );                 /* interface id */

              CCBRI_MailtoL3();
              CCB->State = NTKD_ROUTINGIN;
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
                           CHANNELS_LISTED,       /* channel selection */
                           CCB->B_Chan,
                           B_CHANNEL_UNITS,            /* type: H or B */
                           FALSE,                        /* slot map ? */
                           FALSE,                       /* exclusive ? */
                           0,                   /* interface id length */
                           NULL );                     /* interface id */

           CCBRI_MailtoL3();
           }
        CCB->State = NTKD_RINGING;
        break;

      case HOST_CONNECT:
        if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
           {
           CCBRI_MailtoL3();                 /* Send CC_BROADCAST_RESP */
           }
        CCBRI_HostConnect();
        CCB->State = NTKD_CONNECTING;
        break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
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


/*****************************
* CCBNTKD_CallOffHookRelease *
******************************
*
* Process the current event in the context of a voice/data call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The local user
* is off-hook. In the case of the COMMAND interface, this means that the
* user has originated the Call Clearing sequence by sending a Host Disconnect.
* This corresponds to Q.931 state 11
*/
PRIVATE BOOLEAN near CCBNTKD_CallOffHookRelease( event )

int *event;                                /* Event input to the state */

{
   BYTE event_b;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in ONHOOK_RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_ONHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* simply remove the CCB.
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

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NTKD_ONHOOK_RELEASING;
         break;

#endif
      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
#ifdef SBC_INTERFACE
         CCBNTKD_AcceptAdviceOfCharge( Charges );
         CCB->State = NTKD_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
          event_b = HOST_DISCONNECT_ACK;
          CCBRI_DisconnectHostCall(&event_b, CCB->Cause);
          CCBNTKD_AcceptAdviceOfCharge( Charges );
          CCBNTKD_ReleaseCall();
#endif
         break;

      case CC_DISCONNECT_IND:
         /* We simply await CC_RELEASE_CONF to follow this message.
         */
         break;

      case CC_INFO_IND:
          break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
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
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_RELEASING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/****************************
* CCBNTKD_CallOnHookRelease *
*****************************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The local user
* is on-hook.  In the case of a CMD interface, the Host has sent
* HOST_DISCONNECT_ACK and Call Control has mailed RELEASE to the
* Network.
*
*/
PRIVATE BOOLEAN near CCBNTKD_CallOnHookRelease( event )

int *event;                                /* Event input to the state */

{

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;

      case HOST_DISCONNECT_ACK:
         /* simply remove the CCB.
         */
         CCBRI_RemoveCCB( CCB );
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_OFF_HOOK:
         Hcb[ DEFAULT_DSL ].Hw_State = OFFHOOK;
         CCB->State = NTKD_OFFHOOK_RELEASING;
         break;

#endif
      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
         CCBNTKD_AcceptAdviceOfCharge( Charges );
         CCBNTKD_ReleaseCall();
         break;

      case CC_DISCONNECT_IND:
         /* We simply await CC_RELEASE_CONF to follow this message.
         */
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
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
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_ONHOOK_RELEASING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/************************
* CCBNTKD_CallOriginate *
*************************
*
* Process the current event in the context of a voice/data call in the
* ORIGINATE state awaiting the first response to CC_SETUP_REQ. Corresponds
* to State 1 of Q.931.
*/
PRIVATE BOOLEAN near CCBNTKD_CallOriginate( event )

int *event;                                /* Event input to the state */

{
   BYTE    cause = NOERR;
   BYTE    chan_id;
   BYTE    exclusive;
   BYTE    event_b;
   BOOLEAN struct_err;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_OFFHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_ONHOOK_RELEASING;
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* The caller hangs up.  The current call setup will be
         *  terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         CCBNTKD_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NTKD_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_ALERTING_IND:
         /* ALERTING may be the first network response to SETUP
         *  and it will transport the network's B-channel choice.
         *  The channel will be reported to the USR when the CONNECT
         *  is received.
         */
         if ( CCBRI_AcceptChanId( &chan_id, &exclusive ) == NOERR)
            {
            CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
            CCB->State = NTKD_ROUTINGOUT;
#ifdef COMMAND_INTERFACE
            /* inform the HOST.
            */
            if ( CCBRI_GetOutInfo( HOST_ALERTING ) )
               {
               /* total length of private IE's = 4.
               */
                OutPkgInfo[ OutPkgInfoLen++ ] = 4;
                OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
                OutPkgInfo[ OutPkgInfoLen++ ] = 2;
                OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
                OutPkgInfo[ OutPkgInfoLen++ ] = chan_id;
                CCBRI_MailtoHost( CCB->CopyPublicMsg );
                }
#endif
            }
         break;

      case CC_CALL_PROC_IND:
         /* This message indicates that digit transmission (en_bloc) is
         *  complete.  This case is added for completeness.
         */
         if (( cause = CCBRI_AcceptChanId(  &chan_id, &exclusive)) == NOERR )
            {
            /* The B_Channel should be connected.
            */
#ifdef TASK_KB
            CCBRI_StartConnect( CCB->B_Chan );
#endif
            CCB->State = NTKD_ROUTINGOUT;
            CCBRI_TRACE( TR_ROUTINGOUT );
            CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
#ifdef COMMAND_INTERFACE
            /* inform the HOST.
            */
            if ( CCBRI_GetOutInfo( HOST_PROCEEDING ) )
               {
               /* total length of private IE's = 4.
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 4;
               OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 2;
               OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
               OutPkgInfo[ OutPkgInfoLen++ ] = chan_id;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
#endif
            }
         else
            {
            /* The en-block transmission is complete.  This is the first
            *  response back and there is no Channel Id  - reject the call
            */
            CCBRI_RejectCall( cause, CHANNEL_ID_IE );
            CCB->State = NTKD_OFFHOOK_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
            }
         break;

      case CC_INFO_IND:
         break;

      case CC_PROGRESS_IND:
         /* Note:  L3 protocol control supports this message.  Receipt of
         *  this message may mean the Call Setup is leaving the ISDN, in
         *  which case the B channel would have to be connected to receive
         *  Call Progress tones.
         */
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
#ifdef SBC_INTERFACE
         CCBNTKD_AcceptAdviceOfCharge( Charges );
         CCB->State = NTKD_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         event_b = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall(&event_b, cause);
         CCBRI_RemoveCCB( CCB );
#endif
         break;

      case CC_DISCONNECT_IND:
         if ( CCBNTKD_DisconnectInd() )
            {
#ifdef SBC_INTERFACE
            CCB->State = NTKD_OFFHOOK_RELEASING;
#endif
            CCBRI_TRACE( TR_RELEASING );
            }
         break;

      case CC_SETUP_CONF:
         /* We may have an indication of a second timeout of T303
         *  (L3_SDL_U.C, U1_T303Expiry).  We will study the channel ID
         *  info element to determine this.
         */
         IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                                   CHANNEL_ID_IE, CODESET_0, 1 );
         if ( IE )
            {
            /* Go find the info channel selection field.  An indication of
            *  "No Channel" is a Protocol Control indication of call setup
            *  failure.
            */
            IE_Fields = CCIE_ExpandChanId_IE( IE, &struct_err );
            if ( IE_Fields[ 4 ] == 0 )        /* Test for "No Channel" */
               {
               /* SETUP has timed-out (T303) to NETWORK.  The SETUP failed.
               */
               CCBRI_ReleaseChan();  /* Disc and rls chan if allocated */
#ifdef SBC_INTERFACE
               CCB->State = NTKD_LOCKOUT;
#endif
               CCBRI_TRACE( TR_LOCKOUT );
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
            else                                             /* no T/O */
               {
               CCBRI_SetNewChan( IE_Fields[ 4 ], CHANNEL_BUSY, TRUE );
#ifdef TASK_KB
               if ( CCB->B_Chan )
                  CCBRI_StartConnect( CCB->B_Chan );
#endif
               CCBRI_TRACE( TR_CONVERSATION );
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
               CCB->State = NTKD_CONNECTED;
               }
            }
         break;

#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

#endif
      case CC_STATUS_IND:
         CCBRI_StatusInd();
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
            CCB->State = NTKD_OFFHOOK_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_ORIGINATE, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/************************
* CCBNTKD_CallRoutingIn *
*************************
*
* Process the current event in the context of a voice call in the
* ROUTINGIN state, the general purpose engaged, but non-CONVERSATION, state.
* This state is typically associated with the presentation of some
* informational tone such as ringback or some other call progress tone
* while the user is standing by.  Corresponds to State 9.
*
*/
PRIVATE BOOLEAN near CCBNTKD_CallRoutingIn( event )

int *event;                                /* Event input to the state */

{
   BYTE event_b;
   BYTE cause;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

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

            CCBRI_MailtoL3();                  /* Send CC_ALERTING_REQ */
            CCB->State   = NTKD_RINGING;
            }
         break;


      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = NTKD_CONNECTING;
         break;

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_OFFHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_ONHOOK_RELEASING;
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* The caller hangs up.  The current call setup
         *  will be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         CCBNTKD_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NTKD_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
#ifdef SBC_INTERFACE
         CCBNTKD_AcceptAdviceOfCharge( Charges );
         CCB->State = NTKD_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
          event_b = HOST_DISCONNECT_ACK;
          CCBRI_AcceptCause( &cause );
          CCBRI_DisconnectHostCall(&event_b, cause);
          CCBRI_RemoveCCB( CCB );
#endif
         break;

      case CC_DISCONNECT_IND:
         CCBNTKD_DisconnectInd();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
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
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_ROUTINGIN, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/*************************
* CCBNTKD_CallRoutingOut *
**************************
*
* Process the current event in the context of a voice call in the
* ROUTINGOUT state, the general purpose engaged, but non-CONVERSATION, state.
* This state is typically associated with the presentation of some
* informational tone such as ringback or some other call progress tone
* while the user is standing by. Corresponds to states 3 and 4 of Q.931.
*
*/
PRIVATE BOOLEAN near CCBNTKD_CallRoutingOut( event )

int *event;                                /* Event input to the state */

{
   BYTE cause = 0;
   BYTE event_b;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_OFFHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_ONHOOK_RELEASING;
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* The caller hangs up.  The current call setup
         *  will be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         CCBNTKD_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NTKD_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_ALERTING_IND:
         /* Notification of far end alerting.
         *  The B-Channel should already be connected.  Connect it
         *  again just to be sure to receive in-band alerting tone.
         */
#ifdef TASK_KB
         if ( CCB->B_Chan )
            CCBRI_StartConnect( CCB->B_Chan );
#endif
         CCBRI_TRACE( TR_RINGBACK );
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
         break;

      case CC_SETUP_CONF:                    /* Translation of CONNECT */
         /* The call was connected at the far end.  Connect the call
         *  at this end too (its probably already connected).
         *  The call progresses to the CONNECTED state.
         */
#ifdef TASK_KB
         if ( CCB->B_Chan )
            CCBRI_StartConnect( CCB->B_Chan );
#endif
         CCBRI_TRACE( TR_CONVERSATION );
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
            OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;         /* exclusive */
            OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         CCB->State = NTKD_CONNECTED;
         break;

      case CC_PROGRESS_IND:
         /* Note:  L3 protocol control supports this message.  Receipt of
         *  this message may mean the Call Setup is leaving the ISDN, in
         *  which case the B channel would have to be connected to receive
         *  Call Progress tones.
         */
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
#ifdef SBC_INTERFACE
         CCB->State = NTKD_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
         trash = cause;
#endif
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         event_b = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall(&event_b, cause);
         CCBRI_RemoveCCB( CCB );
#endif
         break;

      case CC_DISCONNECT_IND:
         CCBNTKD_DisconnectInd();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
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
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_ROUTINGOUT, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/**************************
* CCBNTKD_DigitCollection *
***************************
*
* Process the current event in the context of a voice/data call in the
* DigitCollection state.  Since overlap sending is not supported by NTT/KDD,
* this state is entered when digits are being "collected" from keypad interface
*/
PRIVATE BOOLEAN near CCBNTKD_DigitCollection( event )

int *event;                                /* Event input to the state */

{
#ifdef 0 /* not currently used */
   int  cause;
   int  index;
   int  k;
   BYTE prog_desc;
   BYTE digit_as_byte;
#endif


   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NTKD_OFFHOOK_RELEASING;
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;
#endif
#ifdef SBC_INTERFACE
      case KP_DIGIT_0:
      case KP_DIGIT_1:
      case KP_DIGIT_2:
      case KP_DIGIT_3:
      case KP_DIGIT_4:
      case KP_DIGIT_5:
      case KP_DIGIT_6:
      case KP_DIGIT_7:
      case KP_DIGIT_8:
      case KP_DIGIT_9:
      case KP_DIGIT_STAR:
      case KP_DIGIT_POUND:
         /* A dialable digit.
         */
         CCBRI_TRACE( TR_DIALING );             /* DIALTONE -> DIALING */
         if ( strlen ( Called_Party_Num ) > PHONE_NUMB_LEN )
            {
            CCB->State = NTKD_LOCKOUT;
#ifdef TASK_KB
            CCBRI_StartTone ( KP_START_BUSYTONE );
#endif
            LIF_StopTimer ( CCB->NtkdTimer );
            }
         else
            {
            *Cpn_P++ = (BYTE) (*event);
            LIF_StopTimer ( CCB->NtkdTimer );
            LIF_UpdateTimer( CCB->NtkdTimer, CC_ID, CC_ID, DIGIT_COLLECT_TIMEOUT, Dsl_ID, CCB->Sapi,
                      CCB->Ces, CCB->Call_ID, 0, DIGIT_COLLECT_DEFAULT);
            LIF_StartTimer (CCB->NtkdTimer);
            }
         break;

      case DIAL_KEY:
         LIF_StopTimer ( CCB->NtkdTimer );
         CCBRI_TRACE (TR_ORIGINATE);
         if ( CCBRI_GetOutPkt( CC_SETUP_REQ, FALSE ) )
            {
            CCBRI_SetNewChan( chan_id, CHANNEL_PROPOSED, TRUE );
            OutPkt->InfoLen +=
                  CCIE_BuildBearerCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  Hcb[ DEFAULT_DSL].Bearer_Data, strlen( Hcb[ DEFAULT_DSL ].Bearer_Data ));
            OutPkt->InfoLen +=
                  CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                  CCB->B_Chan, 0, B_CHANNEL_UNITS, FALSE, FALSE, 0,
                  NULL );

            if ( strlen( Hcb[ DEFAULT_DSL ].Ldn) >  0 )
               {
               OutPkt->InfoLen +=           /* Calling Number Encoding */
                    CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                    CALLER_NUMBER_IE, UNKNOWN_NUMB_PLAN, UNKNOWN_NUMB_PLAN,
                    Hcb[ DEFAULT_DSL ].Ldn, strlen( Hcb[ DEFAULT_DSL].Ldn ), TRUE );
               }
            OutPkt->InfoLen +=               /* Called Number Encoding */
               CCIE_BuildPartyNumb_IE ( &OutPkt->Info[ OutPkt->InfoLen ],
               CALLED_NUMBER_IE, UNKNOWN_NUMB_PLAN, UNKNOWN_NUMB_PLAN,
               Called_Party_Num, strlen( Called_Party_Num ), TRUE);

            CCBRI_MailtoL3();
            CCB->State = NTKD_ORIGINATE;
            }
         else
            {
            CCBRI_Error( CCBRI_CALL_DIALING, OUT_OF_PKTS, *event, Source, FALSE );
            CCB->State = NTKD_LOCKOUT;
            }
         break;

      case KP_ON_HOOK:
         /* The caller hangs up in dialing.  The current call setup will
         *  be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be ONHOOK_RELEASING since the phone is on-hook.
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCBNTKD_Disconnect( NORMAL_CLEARING );
         CCB->State = NTKD_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;
      case DIGIT_COLLECT_TIMEOUT:

#ifdef TASK_KB
         CCBRI_StopDigitDetect();
         CCBRI_StartTone ( KP_START_BUSYTONE );
#endif
         CCB->State = NTKD_LOCKOUT;
         break;

#endif
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
            CCB->State = NTKD_OFFHOOK_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_DIGIT_COLLECTION, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/****************************
* CCBNTKD_DisconnectingHost.*
*****************************
*
* This State corresponds to Q.931 State 12
* A Disconnect Indication has been received and HOST_DISCONNECT has been
* mailed to the Host
*
*/
PRIVATE BOOLEAN near CCBNTKD_DisconnectingHost( event )

int *event;                                /* Event input to the state */

{
  BYTE event_b;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case HOST_DISCONNECT_ACK:
         LIF_StopTimer ( CCB->NtkdTimer );
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
           {
           /* Build complaining Cause info element (optional in this message)
           */
           OutPkt->InfoLen +=
           CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                           CCB->Cause, FALSE, 0 );
           CCBRI_MailtoL3();
           CCB->State = NTKD_ONHOOK_RELEASING;
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
           CCB->State = NTKD_ONHOOK_RELEASING;
           }
         break;

#endif
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
          LIF_StopTimer ( CCB->NtkdTimer );
          event_b = HOST_DISCONNECT_ACK;
          CCBRI_DisconnectHostCall(&event_b, CCB->Cause);
          CCBNTKD_ReleaseCall ();
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
            CCB->State = NTKD_OFFHOOK_RELEASING;
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

/************************************************************************
*                     O T H E R   F U N C T I O N S                     *
*************************************************************************
*/
/* Note that the data recovered by this function is currently not
*  referenced, waiting in the wings for its curtain call when it is
*  of interest to the SBC application.  With the Command interface,
*  the IE is passed on up as Q.931 public data.
*/
/*******************************
* CCBNTKD_AcceptAdviceOfCharge *
********************************
*
* Find and validate the Advice Of Charge info element in the incoming
* message.
*
*/
PRIVATE int near CCBNTKD_AcceptAdviceOfCharge( charges )

BYTE *charges;
{
   int return_code;

   return_code = NOERR;

   Current_IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
      ADVICE_OF_CHARGE_IE, CODESET_6, 1 );
   if ( Current_IE )
      {
      if (( Current_IE[ 2 ] != 0x82 ) || ( Current_IE[ 1 ] > MAX_CHARGES_LEN ))
         {
         return_code = INVALID_ELEM_CONTENTS;
         }
      else
         {
         strcpy( charges, Current_IE + 3 );
         }
      }
   else
      {
      return_code = MANDATORY_IE_MISSING;
      }

   return ( return_code );
}


/**************************
* CCBNTKD_DecodeCallState *
***************************
*
* Decode the state of the CCB from the CCB state variable.
* Dispatch the appropriate state function through the state function
* jump table.
*
*/
PUBLIC int  CCBNTKD_DecodeCallState( event )

BOOLEAN *event;                            /* Event input to the state */

{
   BOOLEAN event_pending;
   int     result_code;

   /* Perform a range check on the value of CCB->State to prevent
   *  the strange and unusual.
   */
   if (( CCB->State > NTKD_STATE_LOW_LIMIT ) && ( CCB->State < NTKD_STATE_HIGH_LIMIT ))
      {
      /* Jump to enter the state function.
      */
      event_pending     = ( *Ccb_State_Func_Ntkd[ CCB->State ] )( event );
      result_code       = ( event_pending ) ? EVENT_PENDING : NO_ERROR;
      }
   else
      {
      CCBRI_Error( CCBRI_DECODE_CALL_STATE, INVALID_STATE, CCB->State,
                   *event, TRUE );
      result_code = INVALID_STATE;
      }

   return( result_code );
}

#ifdef SBC_INTERFACE
/*********************
* CCBNTKD_Disconnect *
**********************
*
* Standard actions for hangup by the voice phone.  This function initiates
* sending of CC_DISCONNECT_REQ.  It is used whenever the voice phone goes
* on hook while engaged.
*
*/
PRIVATE void near CCBNTKD_Disconnect( cause )

int cause;                      /* Cause for cause information element */
{

#ifdef TASK_KB
   CCBRI_StopConnect( CCB->B_Chan );
#endif
   if ( CCBRI_GetOutPkt( CC_DISCONNECT_REQ, FALSE ))
      {
      /* Build mandatory Cause element.
      */
      OutPkt->InfoLen += CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                             cause, FALSE, 0 );
      CCBRI_MailtoL3();
      }
   return;
}
#endif 

/************************
* CCBNTKD_DisconnectInd *
*************************
*
* Generic action upon receiving the CC_DISCONNECT_IND message
* for a network disconnect.
*
*/
PRIVATE BOOLEAN near CCBNTKD_DisconnectInd()

{
   BYTE cause = 0;
   BYTE event;

#ifdef SBC_INTERFACE
      CCBNTKD_AcceptAdviceOfCharge( Charges );

#ifdef TASK_KB
      CCBRI_StopConnect( CCB->B_Chan );
#endif
   /* Far end disconnect.  Send CC_RELEASE_REQ. We will retain connection
   *  to the B-Channel.
   */
    /* Build complaining cause info element (optional in this message) */
      cause = (( CCBRI_AcceptCause (&cause) == MANDATORY_IE_MISSING ) ?
                    MANDATORY_IE_MISSING : cause);

      if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) )
         {
         CCBRI_MailtoL3();
         CCB->State = NTKD_OFFHOOK_RELEASING;
         }
#endif
#ifdef COMMAND_INTERFACE

    /* Build complaining cause info element (optional in this message) */
      cause = (( CCBRI_AcceptCause (&cause) == MANDATORY_IE_MISSING ) ?
                    MANDATORY_IE_MISSING : cause);

      LIF_UpdateTimer( CCB->NtkdTimer, CC_ID, CC_ID, DISC_HOST_EVENT, Dsl_ID, CCB->Sapi,
                  CCB->Ces, CCB->Call_ID, 0, DISC_HOST_TIMEOUT);
      LIF_StartTimer (CCB->NtkdTimer);
      CCB->State = NTKD_DISCONNECTING_HOST;
      CCB->Cause = (int ) cause;
      event       = HOST_DISCONNECT;
      CCBRI_DisconnectHostCall( &event , cause );
#endif
   return( TRUE );
}


/**********************
* CCBNTKD_ReleaseCall *
***********************
*
* Terminate a call and put the CCB to bed.
*
*/
PUBLIC void CCBNTKD_ReleaseCall()

{
   /* Perform actions specific to the voice call termination.
   */
   CCBRI_ReleaseChan();          /* Disc and release chan if allocated */
   CCBRI_RemoveCCB( CCB );

#ifdef SBC_INTERFACE
   if ( Hcb[ DEFAULT_DSL ].Active_Call == NO_ACTIVE_CALLS )
      {
      CCBRI_TRACE( TR_IDLE );
      }
#endif
   return;
}

/***********************
* CCBNTKD_ReleasedLink *
************************
*
* The data link is down at Layer 2.
* Remove CCBs from any context but LOCKOUT.  Users left OFF HOOK
* must retain a CCB in the LOCKOUT state to process their hangup.
*
*/
PUBLIC void CCBNTKD_ReleasedLink (ces, dsl_id)
BYTE ces;          /* Connection Endpoint Suffix of the released link. */
BYTE dsl_id;                                /* Digital subscriber loop */
{
   BYTE event_b;
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
         event_b = HOST_DISCONNECT_ACK;
         CCB->Cause = TEMPORARY_FAILURE;
         CCBRI_DisconnectHostCall(&event_b,CCB->Cause);
         CCBRI_RemoveCCB(CCB);
      }
      CCB = tmp_ccb;   /* Skip to the next one. */
   }
}

/******************
* CCBNTKD_Restart *
*******************
*
* B-Channel restart indication received.  It is assumed that Layer 3
* Protocol Control (L3_Restart) has already analyzed the message for
* mandatory information element presence and coding correctness.  Therefore
* we will proceed to restart channels without further checking based upon
* the criteria L3 Protocol Control used to restart at that level.
*
*/
PUBLIC void CCBNTKD_Restart( dsl_id )

   BYTE     dsl_id;

{
    int     restart_class;
    int     b_chan;
    BOOLEAN struct_err;


   /* Look for the Restart Ind info element.
   */
   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen, RESTART_IND_IE,
                             CODESET_0, 1 ) ;
   restart_class = IE[ 2 ] & 0x07;            /* Recover restart class */

   /* Look for the Channel ID info element.
   */
   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen, CHANNEL_ID_IE,
                             CODESET_0, 1 ) ;

   if (( restart_class == RESTART_SINGLE_INTERFACE ) || ( !IE ))
      {
      /* This restart class or the lack of the CHANNEL_ID_IE ==> RESTART ALL.
      */
      for ( b_chan = MIN_CHANNEL; b_chan <= MAX_CHANNELB; b_chan++ )
         CCBNTKD_RestartChan( b_chan, dsl_id );
#ifdef DEBUG
      printf ( "CCBNTKD_Restart...ALL channels restarted.\n" );
#endif
#ifdef SBC_INTERFACE
      for ( i = 0; i < MAX_VOICE_CALLS; i++ )
         {
         if ( Hcb[ dsl_id ].Ccb_In_Use[ i ] )
            {
            CCB = Hcb[ dsl_id ].Ccb_In_Use[ i ];
            if (( CCB->State != NTKD_LOCKOUT ) ||
                ( CCB->HcbIndex != Hcb[ dsl_id ].Active_Call ))
               CCBNTKD_ReleaseCall();
            }
         }
#endif
      Call_Id_Source = 1;             /* Restore the Call_ID generator */
      }
   else                              /* Restart individual channel(s). */
      {
      IE_Fields = CCIE_ExpandChanId_IE( IE, &struct_err );
      b_chan = IE_Fields[ 4 ];
      if ( b_chan )
         {
#ifdef DEBUG
         printf ( "CCBNTKD_Restart...channel %d restarted.\n", b_chan );
#endif
         CCBNTKD_RestartChan( b_chan, dsl_id );   /* Restart a channel */
         }
      }
}




/**********************
* CCBNTKD_RestartChan *
***********************
*
* Restart an individual B-Channel.
*
*/
PRIVATE void near CCBNTKD_RestartChan( b_chan, dsl_id )

   int   b_chan;                            /* Channel to be restarted */
   BYTE  dsl_id;                            /* digital subscribel loop */

{
   /* Check if the channel being restarted is assigned to the
   *  active call.
   */
#ifdef SBC_INTERFACE
   if ( Hcb[ dsl_id ].Active_Call != NO_ACTIVE_CALLS )
      {
      CCB = Hcb[ dsl_id ].Ccb_In_Use[ Hcb[ dsl_id ].Active_Call ];
      if (( CCB ) && ( CCB->B_Chan == b_chan ))
         {
         /* Preserve the hardware state of the "phone."
         */
         hw_state = Hcb[ dsl_id ].Hw_State;
#ifdef TASK_KB
         if ( hw_state == BELL )
            CCBRI_StopRing( VOICE_ALERTING );
#endif
         CCBNTKD_ReleaseCall();                  /* Terminate the call */
         if ( hw_state == OFFHOOK )
            {
            /* We reinstall the call into a new CCB so it may hangup.
            */
            Hcb[ dsl_id ].Hw_State = OFFHOOK;      /* Restore hw state */
            if (( index = CCBRI_GetHcbIndex( dsl_id )) < MAX_VOICE_CALLS )
               {
               if ( CCBRI_GetNewCCB( NTKD_IDLE,
                                     NO_CHANNEL,
                                     SIGNALING_SAPI,
                                     SIGNALING_CES,
                                     index,
                                     CCBRI_GetNewCall_Id()))
                  {
                  Hcb[ dsl_id ].Ccb_In_Use[ index ] = CCB;
                  Hcb[ dsl_id ].Active_Call = index;
                  CCB->State = NTKD_LOCKOUT;
                  }
               }
            }
         }
      }
   CC_CHAN_ReleaseChan( b_chan, B_CHANNEL_UNITS, 0 ); /* Remove assign */
#endif
#ifdef COMMAND_INTERFACE
   /* Check if the channel being restarted is in use.
   */
   CCB = CCB_Head[ dsl_id ];
   while ( CCB )
      {
      if ( CCB->B_Chan == b_chan )
         {
         if ( CCB->State != NTKD_IDLE )
            {
            CCBRI_ReleaseChan();
#ifdef COMMAND_INTERFACE
            CCBRI_RejectHostCall( TEMPORARY_FAILURE );
            CCBRI_RemoveCCB (CCB);
#endif
#ifdef SBC_INTERFACE
            CCB->State = NTKD_LOCKOUT;
            CCBRI_TRACE( TR_LOCKOUT );
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

   return;
}


/*******************
* CCBNTKD_SetupInd *
********************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function
* because of its inherent complexity.
*
*/
PRIVATE void near CCBNTKD_SetupInd()

{
#ifdef TASK_KB
   BYTE    ring_type;
#endif 
   int     cause;
   int     length;
   int     k = 0 ;
   BYTE    chan_id;
   BYTE    exclusive;
   BYTE    call_num[ PHONE_NUMB_LEN + 1 ];

   /* A new call SETUP from the far end. The CC may send
   *  ALERTING request to the network instructing it that
   *  the phone is ringing.  This will be done only after
   *  unpacking and checking of info elements in the CC_SETUP_IND
   *  message.
   *
   *  Before sending of ALERTING message, the CC must always mail
   *  CC_BROADCAST_RESP message.  This will assure that the Layer 3
   *  software is in a proper state to receive ALERTING.
   *
   * For the Host interface, CC_BROADCAST_RESP is defered until the
   * CALL_PROCEEDING is returned by the Host.
   *
   */

#ifdef COMMAND_INTERFACE
   /* inform the HOST.
   */
   if ( CCBRI_GetOutInfo( HOST_INCOMING_CALL ) )
      {
      OutPkgInfoLen++;
      k = 5;                      /* bearer capability must be present */
      }
#endif

   /* Look at the bearer capability IE.
   */
   if (( cause = CCBRI_AcceptBearerCap()) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected.
      */
      CCBRI_RejectSetup( cause, BEARER_CAPABILITY_IE );
      CCBNTKD_ReleaseCall();

#ifdef COMMAND_INTERFACE
      CCBRI_FreeCallBufs();
#endif
      goto bailout;
      }

   /* Next we look at the Channel ID.
   *  Note that there is a supplementary services related feature
   *  specified in NTT regarding Incoming Call handling.  When no B-Channel
   *  is idle the SETUP message with no B-channel indication is
   *  supported.  Code would have to be added here to support this feature.
   */
    if (( cause = CCBRI_AcceptChanId( &chan_id, &exclusive)) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected.
      */
      if ( cause == NO_CIRCUIT_AVAILABLE )
         CCBRI_RejectSetup( NO_CIRCUIT_AVAILABLE, 0 );
      else
         CCBRI_RejectSetup( cause, CHANNEL_ID_IE );
      CCBNTKD_ReleaseCall();
#ifdef COMMAND_INTERFACE
      CCBRI_FreeCallBufs();
#endif
      goto bailout;
      }
   else
      {
      if ( (cause = CCBRI_SetNewChan( (int) chan_id,
                                      CHANNEL_BUSY,
                                      TRUE ) ) != NOERR )
        {
        CCBRI_RejectSetup( cause, 0 );
        CCBNTKD_ReleaseCall();
#ifdef COMMAND_INTERFACE
        CCBRI_FreeCallBufs();
#endif
        goto bailout;
        }
#ifdef COMMAND_INTERFACE
      if ( OutPkgInfo )
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 2;
         OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
         OutPkgInfo[ OutPkgInfoLen++ ] = chan_id;
         k += 4;
         }
#endif
      }

   /* Note that this is an optional element.  If provided, we attempt
   *  to match numbers.
   */
   if (( cause = CCBRI_AcceptCallingNumb(call_num,&length )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly or the
      *  called number did not match ours - ignore the setup.
      */
      CCBNTKD_ReleaseCall();
#ifdef COMMAND_INTERFACE
      CCBRI_FreeCallBufs();
#endif
      goto bailout;
      }
#ifdef COMMAND_INTERFACE
   else
      {
      if ( cause == NOERR )
         k+= length;
      }
#endif

   /* Note that this is an optional element.  If provided, we attempt
   *  to match numbers.
   */
   if (( cause = CCBRI_AcceptCalledNumb( &length, Dsl_ID )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly or the
      *  called number did not match ours - ignore the setup.
      */
      CCBNTKD_ReleaseCall();
#ifdef COMMAND_INTERFACE
      CCBRI_FreeCallBufs();
#endif
      goto bailout;
      }
#ifdef COMMAND_INTERFACE
   else
      {
      if ( cause == NOERR )
         k+= length;
      }
#endif


#ifdef SBC_INTERFACE
   /* A subsequent call setup when we are already busy?
   */
   if ( CCB->HcbIndex != Hcb[ DEFAULT_DSL ].Active_Call )
      {
      /* Another call is already busying this terminal.
      *  Note that this is the simplistic action.  Certain
      *  Supplementary Services (e.g. Additional Call Offering)
      *  may require the maintenance (in Hcb.Ccb_In_Use), of multiple
      *  passive calls in addition to an active call.
      */
      CCBRI_RejectSetup( USER_BUSY, 0 );
      CCBNTKD_ReleaseCall();
      goto bailout;
      }
   else if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
      {
      CCBRI_MailtoL3();                      /* Send CC_BROADCAST_RESP */

      if ( CCBRI_GetOutPkt( CC_CALL_PROC_REQ, FALSE ))
         {
         /* Add CHANNEL ID info element.
         */
         OutPkt->InfoLen +=
            CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                 CHANNELS_LISTED, /* channel selection */
                                 CCB->B_Chan,
                                 B_CHANNEL_UNITS,      /* type: H or B */
                                 FALSE,                  /* slot map ? */
                                 FALSE,                 /* exclusive ? */
                                 0,             /* interface id length */
                                 NULL );               /* interface id */

         CCBRI_MailtoL3();                    /* Send CC_CALL_PROC_REQ */
         if ( CCBRI_GetOutPkt ( CC_ALERTING_REQ, FALSE ))
            {
            CCBRI_MailtoL3();
#ifdef TASK_KB
            CCBRI_StartRing( ring_type );      /* Here turn ringing on */
#endif
            Hcb[ DEFAULT_DSL ].Hw_State = BELL;
            CCB->State = NTKD_RINGING;
            CCBRI_TRACE( TR_RINGING );
            }                                          /* Alerting req */
         }                                          /* Call Proceeding */
      }
#endif

#ifdef COMMAND_INTERFACE
   /* inform the HOST
   */
   if ( OutPkgInfo )
      {
      OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
      CCBRI_MailtoHost( CCB->CopyPublicMsg );
      CCB->State = NTKD_OFFERED;
      }
#endif

bailout:

   return;
}


/*******************
* CCBNTKD_Shutdown *
********************
*
* Reset Call Control
*
*/
PUBLIC void  CCBNTKD_Shutdown (dsl)
BYTE dsl;
{
    CCB_ID temp_ccb;

    Dsl_ID = dsl;
    CCB = CCB_Head[dsl];
    while (CCB) {
        temp_ccb = CCB->Next;
        CCBNTKD_ReleaseCall();
        CCB = temp_ccb;
    }
}
#endif
