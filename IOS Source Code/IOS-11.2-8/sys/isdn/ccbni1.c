/* $Id: ccbni1.c,v 3.3.12.2 1996/09/13 04:54:34 thille Exp $ 
 * $Source: /release/112/cvs/Xsys/isdn/ccbni1.c,v $
 *------------------------------------------------------------------
 * ccbni1.c  ISDN National-1 BRI Specific Call Control Functions 
 *
 * February 1993, Dennis Clare 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccbni1.c,v $
 * Revision 3.3.12.2  1996/09/13  04:54:34  thille
 * CSCdi68927:  Clean up some things that should be const in ISDN
 * Branch: California_branch
 * Make some more stuff const to save ram.  Also make some more stuff
 * static.
 *
 * Revision 3.3.12.1  1996/04/30  23:06:51  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/15  17:27:16  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.2  1995/11/17  17:39:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:      CCBNI1.C
DESCRIPTION: National ISDN-1 BRI Specific Call Control Functions
VERSION:     3.32

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
(Basic Rate Access) telephone in a manner compatible with a 
Central Office switch in accordance with the specification: 

             AT&T:                         Bellcore:
               AT&T 235-900-341               Special Report
               ISSUE 1.00                     SR-NWT-001953
               December 1991                  Issue 1, June 1991 
               National ISDN Basic Rate       Generic Guidelines for
               Interface Specification        ISDN Terminal Equipment on
               5E8 Software Release           Basic Access Interfaces
                       
             Northern Telecom:
               NIS S208-6, Issue 1.0
               April 1991 
               ISDN Basic Rate
               User-Network Interface Specification           
                       

This version uses Telenetworks "SBC" interface, assuming the implementation
to be a simple telephone device that would employ "Single Byte Commands" such
as Off-Hook, On-Hook, keypad digits, etc. The developer would interface this Call
Control to local driver module(s) to receive these simple stimuli as well as
generate call progress tones.   
     
This version does not support any supplementary service type features such
as SUSPEND and RESUME, although it would not be difficult to add them.
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
which is designed to interface with an intelligent device such as a
PC or PBX. This Call Control has been customized to the following
specifications:

             AT&T:                         Bellcore:
               AT&T 235-900-341               Special Report
               ISSUE 1.00                     SR-NWT-001953
               December 1991                  Issue 1, June 1991 
               National ISDN Basic Rate       Generic Guidelines for
               Interface Specification        ISDN Terminal Equipment on
               5E8 Software Release           Basic Access Interfaces
                       
             Northern Telecom:
               NIS S208-6, Issue 1.0
               April 1991 
               ISDN Basic Rate
               User-Network Interface Specification           
                       
     
This version does not support any supplementary service type features such
as SUSPEND and RESUME, although it would not be difficult to add them.
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


Supplementary Services are not currently supported by this module.

#endif

ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically within general
groupings.


*************************************************************************
*************************************************************************
*/

#define     CCBNI1_C                                    /* module name */

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
#include    "tune.h"
#include    "gendef.h"
#include    "lif.h"
#include    "ccie.h"
/* #ifdef N_AMERICA_FIT		=== */
/* #include    "ccmim.h"	=== */
/* #endif 					=== */
#include	"gp.h"		/*===*/
#include    "ccbri.h"
#include    "cc_chan.h"
#include "logger.h"
#include "msg_isdn.c"         /* Not a typo, see logger.h */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* Definitions for CCB Call States
*
*
*/
enum CCB_State
   {
   NI1_STATE_LOW_LIMIT,                              /* Sizing element */
   NI1_IDLE,                                               /* CCB idle */
   NI1_RINGING,                                       /* Being alerted */
   NI1_DIALING,                                     /* Overlap sending */
   NI1_CONNECTED,                                      /* Conversation */
   NI1_CONNECTING,                                /* User sent Connect */  
   NI1_LOCKOUT,                           /* Disconnected but off hook */
   NI1_OFFHOOK_RELEASING,  /* Waiting for release confirmation on hook */
   NI1_ONHOOK_RELEASING,  /* Waiting for release confirmation off hook */
   NI1_OFFERED,                          /* In call offered to Host    */                
   NI1_ORIGINATE,                    /* Await response to CC_SETUP_REQ */
   NI1_ROUTINGIN,   /* Engaged but neither dialing nor in conversation */
   NI1_ROUTINGOUT,  /* Engaged but neither dialing nor in conversation */
   NI1_STATE_HIGH_LIMIT                              /* Sizing element */
   };
/* ------------------------------------------------------------------- */
/*                      Miscellaneous Constants                        */
/* ------------------------------------------------------------------- */

/* Various constants.
*/
#define DIAL_KEY              0xEF          /* Request to dial on-hook */
#define EVENT_PENDING            1           /* Internal event pending */
#define VOICE_BEARER_DATA_LEN    3         /* Bearer capability length */
#define NO_ACTIVE_CALLS  MAX_VOICE_CALLS            /* No active calls */
#define NO_STATE_MATCH   MAX_VOICE_CALLS /* No calls of state specified */
#define NO_CALL          MAX_VOICE_CALLS        /* No calls identified */
#define NO_SERVICE_PENDING       0     /* CCB service function default */
#define INTERNAL_RECONNECT       1                   /* Internal event */


/* ------------------------------------------------------------------- */
/*                          Module Error Codes                         */
/* ------------------------------------------------------------------- */

#define INVALID_STATE           -1
#define UNEXPECTED_MSG          -2
#define MISSING_MANDATORY_IE    -3
#define INVALID_INFO_ELEMENT    -4
#define UNACCEPTABLE_CHANNEL    -5
#define UNKNOWN_MSG_SOURCE      -6
#define NO_CALL_SLOT            -7
#define OUT_OF_CCBS             -8
#define BAD_CALLED_ADDR         -9
#define BAD_BEARER_CAP         -10
#define OUT_OF_PKTS            -11
#define MAND_IE_LEN_ERR        -12
#define DIGIT_BUFFER_OVERFLOW  -13

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
/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/
PRIVATE    BOOLEAN  near    CCBNI1_CallAlerting        (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallConnected       (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallConnecting      (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallDialing         (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallIdle            (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallLockout         (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallOffered         (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallOffHookRelease  (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallOnHookRelease   (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallOriginate       (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallRoutingIn       (int *);
PRIVATE    BOOLEAN  near    CCBNI1_CallRoutingOut      (int *);
#ifdef SBC_INTERFACE
PRIVATE    void     near    CCBNI1_Disconnect          (int);
#endif 
PRIVATE    BOOLEAN  near    CCBNI1_CheckEID            (int *); 
PRIVATE    BOOLEAN  near    CCBNI1_DisconnectInd       (void);
PRIVATE    void     near    CCBNI1_RestartChan         (int, BYTE);
PRIVATE    void     near    CCBNI1_SetupInd            (void);


/************************************************************************
*             S T A T E   E N T R Y   J U M P   T A B L E               *
*************************************************************************
*/

typedef BOOLEAN (near *CC_STATES)( int * );

/* NOTE: this table MUST strictly correspond to the CCB_State table 
*/
static const CC_STATES Ccb_State_Func_NI1[ NI1_STATE_HIGH_LIMIT ] =
   {
   NULL,
   CCBNI1_CallIdle,    
   CCBNI1_CallAlerting,  
   CCBNI1_CallDialing,
   CCBNI1_CallConnected, 
   CCBNI1_CallConnecting,
   CCBNI1_CallLockout, 
   CCBNI1_CallOffHookRelease, 
   CCBNI1_CallOnHookRelease, 
   CCBNI1_CallOffered,  
   CCBNI1_CallOriginate,
   CCBNI1_CallRoutingIn,
   CCBNI1_CallRoutingOut
   };


/************************************************************************
*                C A L L   S T A T E   F U N C T I O N S                *
*************************************************************************
*/

/***********************
* CCBNI1_CallAlerting *
************************
*
* Process the current event in the context of a voice/data call in the
* RINGING state.  The User's ISDN telephone could be RINGING or
* the Host could have sent back HOST_ALERTING.  Corresponds to State 7
* of Q.931
*/
PRIVATE BOOLEAN near CCBNI1_CallAlerting( event )

int *event;                                /* Event input to the state */

{
   BYTE event_b;
   BYTE cause;

   switch ( *event )
      {
      case HOST_CONNECT:                  
         CCBRI_HostConnect();
         CCB->State = NI1_CONNECTING;
         break;

      case HOST_DISCONNECT_ACK:                
         /* process the disconnect_ack message from the HOST. This
         *  sends RELEASE to the Network
         */ 
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_ONHOOK_RELEASING;
         break;

      case HOST_DISCONNECT:                
         /* process the disconnect message from the HOST. This sends
         *  DISCONNECT to the Network.
         */ 
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_OFFHOOK_RELEASING;
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
         CCB->State = NI1_CONNECTING;
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
            CCB->State = NI1_ONHOOK_RELEASING;
#endif
#ifdef COMMAND_INTERFACE
            CCBNI1_DisconnectInd();
#endif
         break;

      case CC_RELEASE_IND:       
      case CC_RELEASE_CONF:
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         CCBRI_AcceptCause( &cause );
#ifdef SBC_INTERFACE
         CCB->State = NI1_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         event_b = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall(&event_b, cause);
         CCBRI_ReleaseCall();
#endif 
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_INFO_IND:
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0.
             */
             CCBRI_MailtoHost( TRUE );
            }
         break; 

      case CC_KEY_HOLD_IND:
         if ( CCBRI_GetOutInfo( HOST_KEY_HOLD ) )
            {
            /* total length of private IE's = 0. Just copy in the
             * public IEs.
             */
             CCBRI_MailtoHost( TRUE );
            }
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
            CCB->State = NI1_ONHOOK_RELEASING;
            }
         CCBRI_Error( CCBRI_CALL_ALERTING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/*************************
* CCBNI1_CallConnected   *
**************************
*
* Process the current event in the context of a voice/data  call in the
* CONNECTED state.  Corresponds to State 10 of Q.931.
*
*/
PRIVATE BOOLEAN near CCBNI1_CallConnected( event )

int *event;                                /* Event input to the state */

{
   BYTE     cause;                 /* The cause value for bad messages */
   BYTE     event_b;              /* event value for Disconnecting Host */
   BYTE     prog_desc;
   BYTE     signal;
   int      k;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_OFFHOOK_RELEASING;
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
         CCBNI1_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NI1_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_DISCONNECT_IND:                    /* Far end disconnect */
         CCBNI1_DisconnectInd();
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_AcceptCause( &cause );
#ifdef SBC_INTERFACE
         CCB->State = NI1_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         event_b = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall(&event_b, cause);
         CCBRI_ReleaseCall();
#endif
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_INFO_IND:
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0. Just copy in the
            * public IEs.
            */
            CCBRI_MailtoHost( TRUE );
            }
         break; 

      case CC_PROGRESS_IND:
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            k = 0;
            OutPkgInfoLen++;           /* leave space for total length */
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

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
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
            CCB->State = NI1_OFFHOOK_RELEASING;
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
* CCBNI1_CallConnecting  *
**************************
*
* Process the current event in the context of a voice/data  call in the
* CONNECTING state, that is, the user has sent CONNECT to the Network.
* Corresponds to State 8 of Q.931.
*/
PRIVATE BOOLEAN near CCBNI1_CallConnecting( event )

int *event;                                /* Event input to the state */

{
   BYTE    cause;                  /* The cause value for bad messages */
   BYTE    event_b;
   PKT_ID  loc_pkt;                  /* Used to test for T313 Timeouts */

   switch ( *event )
      {
      case CC_SETUP_COMP_IND:            /* Translation of CONNECT_ACK */
         /* The call was connected at the far end.  Connect the call
         *  at this end too (its probably already connected).
         *  The call progresses to the CONNECTED state.
         * (  Note that a T313 timeout must be handled here, that is,
         *   L3 sends T313 Timeouts to us as CC_SETUP_COMP_IND.
         *   So we look at the first byte of the info buffer to see if
         *   it is really a CONNECT_ACK )  
         */

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
                CCB->State = NI1_CONNECTED;
                }
#endif
             }
           else
              {
              CCB->State = NI1_OFFHOOK_RELEASING;
              }
         break;

#ifdef COMMAND_INTERFACE
      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_OFFHOOK_RELEASING;
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* Local disconnect (response to this message will be
         *  received in RELEASING state - see CC_RELEASE_IND):
         */
         CCBNI1_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NI1_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_DISCONNECT_IND:                    /* Far end disconnect */
         CCBNI1_DisconnectInd();
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_AcceptCause( &cause );

#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
         CCB->State = NI1_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
          event_b = HOST_DISCONNECT_ACK;
          CCBRI_DisconnectHostCall(&event_b, cause);
          CCBRI_ReleaseCall();
#endif
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_INFO_IND:

#ifdef COMMAND_INTERFACE
         /* must inform the host.
         */
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
         /* no change in state.
         */
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
         if ( ( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ) )
            {
            CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
            CCB->State = NI1_OFFHOOK_RELEASING;
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
/**********************
* CCBNI1_CallDialing  *
***********************
*
* Process the current event in the context of a voice call in the
* DIALING state. Process overlap sending State 2.
*
*/
PRIVATE BOOLEAN near CCBNI1_CallDialing( event )

int *event;                                /* Event input to the state */

{
   int 	 k;
   BYTE signal;
   BYTE prog_desc;
   int  valid_signal;

#ifdef SBC_INTERFACE
   BYTE digit_as_byte;
#endif 

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_OFFHOOK_RELEASING;
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
         if ( CCBRI_GetOutPkt( CC_INFO_REQ, FALSE ))
            {
            /* The following step is required for portability to some
            *  680XX compilers.
            */
            digit_as_byte = (BYTE) ( *event );
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CALLED_NUMBER_IE,
                                       0x00,                   /* type */
                                       0x00,                   /* plan */
                                       &digit_as_byte,       /* digits */
                                       1,             /* number length */
                                       TRUE );         /* ASCII digits */
            CCBRI_MailtoL3();  /* Send the single digit to the Network */
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
         CCBNI1_Disconnect( NORMAL_CLEARING );
         CCB->State = NI1_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;
#endif

      case CC_INFO_IND:
         /* This message may be expected as an indication to terminate
         *  dial tone after the first digit if it were being generated
         *  locally.
         */
#ifdef COMMAND_INTERFACE
         /* must inform the host.
         */
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0
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
         CCB->State = NI1_CONNECTED;
         break;

      case CC_CALL_PROC_IND:

#ifdef TASK_KB
             CCBRI_StopDigitDetect();
             /* The B Channel should be connected now, but just to be
                sure
             */
             if ( CCB->Chan )
                CCBRI_StartConnect (CCB->B_Chan);
#endif
             CCB->State = NI1_ROUTINGOUT;
             CCBRI_TRACE( TR_ROUTING );
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
                OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;         /* exclusive */
                OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;

                 /*  Attach Progress Ind to message if present 
                 */
                if ( CCBRI_AcceptProgress( &prog_desc ) == NOERR )
                   {
                   OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
                   OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                   OutPkgInfo[ OutPkgInfoLen++ ] = prog_desc;
                   }
                CCBRI_MailtoHost( CCB->CopyPublicMsg );
                }
#endif
         break;

      case CC_ALERTING_IND:
#ifdef TASK_KB
             CCBRI_StopDigitDetect();
             /* The B Channel should be connected now, but just to be
                sure
             */
             if ( CCB->Chan )
                CCBRI_StartConnect (CCB->B_Chan);
#endif
             /*
              * The signal IE is mandatory, reject the call if it is missing.
              */
             valid_signal = CCBRI_AcceptSignal(&signal);
             if (valid_signal != NOERR) {
                 CCBRI_RejectCall(valid_signal, SIGNAL_IE);
                 CCB->State = NI1_OFFHOOK_RELEASING;
             } else { 
                 CCB->State = NI1_ROUTINGOUT;
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
                    OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;         /* exclusive */
                    OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
                    CCBRI_MailtoHost( CCB->CopyPublicMsg );
                    }
#endif
             }
         break;

      case CC_DISCONNECT_IND:
         /* Far end disconnect.  Send CC_RELEASE_REQ and go to the
         *  state RELEASING since we are still off-hook here.
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
         /* CAUSE is a mandatory info element here - check it.
         */
         CCBNI1_DisconnectInd(); 
         break;

      case CC_ERROR_IND:
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
         CCB->State = NI1_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            CCBRI_ReleaseCall();
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
               OutPkgInfoLen++;       /* leave space for totoal length */
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

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
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
            CCB->State = NI1_OFFHOOK_RELEASING;
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

   return( FALSE );
}

/*******************
* CCBNI1_CallIdle  *
********************
*
* Process the current event in the context of a voice/data call in the
* IDLE state.
*
*/
PRIVATE BOOLEAN near CCBNI1_CallIdle( event )

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
            CCBRI_ReleaseCall();
            }
         else
            {
            CCB->State = NI1_ORIGINATE;
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
         /* The off-hook puts the phone in Digit Collection state.  
         */
         Hcb[ DEFAULT_DSL ].Hw_State = OFFHOOK;
         CCB->State = NI1_ORIGINATE;
         if ( CCBRI_GetOutPkt( CC_SETUP_REQ, FALSE ))
            {
            OutPkt->InfoLen +=
               CCIE_BuildBearerCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       Hcb[ DEFAULT_DSL ].Bearer_Data,
                                       MAX_BEARER_DATA_SIZE );
            /* The channel ID info element is not included, indicating
            *  "any channel" is acceptable.
            *  Depending upon the application, additional information
            *  elements may be required in the SETUP message and may be
            *  included at this point.
            */
            CCBRI_MailtoL3();
            }
#ifdef TASK_KB
         CCBRI_StartTone(DIAL_TONE);
#endif
         break;
#endif
      case CC_SETUP_IND:
         CCBNI1_SetupInd();                          /* Incoming call */
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
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
            CCBRI_ReleaseCall();
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
* CCBNI1_CallLockout  *
***********************
*
* Process the current event in the context of a voice/data call in the
* LOCKOUT (off-hook, but released from the NETWORK) state.  The user
* is "locked-out" from originating again until an on-hook/off-hook
* cycle is completed.  This state is retained after the NETWORK
* has released the call and the user has failed to hang up.
*
*/
PRIVATE BOOLEAN near CCBNI1_CallLockout( event )

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
         CCBRI_ReleaseCall(); /* Release from lockout, drop the CCB. */
         break;
#endif

      default:
         /* Unexpected message.  No message is expected here from Layer 3
         *  since it will have cleared the Call_ID.  If L3 recieves a new
         *  SETUP, it will be processed in CC_BRI in a new CCB in the
         *  IDLE state.
         */
         CCBRI_Error( CCBRI_CALL_LOCKOUT, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/**********************
* CCBNI1_CallOffered  *
***********************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to State 6 of Q.931.
*
*/
PRIVATE BOOLEAN near CCBNI1_CallOffered( event )

int *event;                                /* Event input to the state */

{

   int        chan_id   = 0;
   BOOLEAN    exclusive = TRUE;
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
            CCBRI_RejectSetup( cause, 0 );
            }
         CCBRI_ReleaseCall();
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
              /* Add CHANNEL ID info element - is the Host sending it?.
              */
              if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CHANNEL_ID_IE ) ) != NULL )
                 {
                 exclusive = IE[ 2 ];
                 chan_id   = IE[ 3 ];
                 }

              OutPkt->InfoLen +=
              CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                               (( chan_id ) ? chan_id : CCB->B_Chan),
                               0,
                               B_CHANNEL_UNITS,        /* type: H or B */
                               FALSE,                    /* slot map ? */
                               exclusive,               /* exclusive ? */
                               0,               /* interface id length */
                               NULL );                 /* interface id */

              CCBRI_MailtoL3();
              CCB->State = NI1_ROUTINGIN;
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
           if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CHANNEL_ID_IE ) ) != NULL )
              {
              exclusive = IE[ 2 ];
              chan_id   = IE[ 3 ];
              }
           /* Add CHANNEL ID info element.
           */
           OutPkt->InfoLen +=
           CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                           (( chan_id ) ? chan_id : CCB->B_Chan),
                           0,                           
                           B_CHANNEL_UNITS,            /* type: H or B */
                           FALSE,                        /* slot map ? */
                           exclusive,                   /* exclusive ? */
                           0,                   /* interface id length */
                           NULL );                     /* interface id */

           CCBRI_MailtoL3();
           }
        CCB->State = NI1_RINGING;
        break;

      case HOST_CONNECT:
         if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
            {
            CCBRI_MailtoL3();                /* Send CC_BROADCAST_RESP */
            }
         CCBRI_HostConnect();
         CCB->State = NI1_CONNECTING;
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_DISCONNECT_IND:
         CCBNI1_DisconnectInd(); 
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         CCBRI_ReleaseCall();
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
         *  SETUP, it will be processed in CC_NI1B in a new CCB in the
         *  IDLE state.
         */
         CCBRI_Error( CCBRI_CALL_LOCKOUT, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/*****************************
* CCBNI1_CallOffHookRelease  *
******************************
*
* Process the current event in the context of a voice/data call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The local user
* is off-hook. In the case of the COMMAND interface, this means that the
* user has originated the Call Clearing sequence by sending a Host Disconnect.
* This corresponds to Q.931 state 11
*/
PRIVATE BOOLEAN near CCBNI1_CallOffHookRelease( event )

int *event;                                /* Event input to the state */

{
   BYTE event_b;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in ONHOOK_RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_ONHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* simply remove the CCB.
         */
         CCBRI_RemoveCCB( CCB );
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NI1_ONHOOK_RELEASING;
         break;

#endif
      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
#ifdef SBC_INTERFACE
         CCB->State = NI1_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         event_b = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall(&event_b, CCB->Cause);
         CCBRI_ReleaseCall();
#endif
         break;

      case CC_DISCONNECT_IND:
         /* We simply await CC_RELEASE_CONF to follow this message.
         */
         break;

      case CC_INFO_IND:
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0.
             */
             CCBRI_MailtoHost( TRUE );
            }
         break; 

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
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
* CCBNI1_CallOnHookRelease  *
*****************************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The local user
* is on-hook.  In the case of a CMD interface, the Host has sent
* HOST_DISCONNECT and Call Control has mailed RELEASE to the
* Network. 
*/
PRIVATE BOOLEAN near CCBNI1_CallOnHookRelease( event )

int *event;                                /* Event input to the state */

{

   switch ( *event )
      {

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;

      case HOST_DISCONNECT_ACK:
         /* simply remove the ccb === 
         */
		 CCBRI_RemoveCCB( CCB );
         break;

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
         CCBRI_ReleaseCall();
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
* CCBNI1_CallOriginate  *
*************************
*
* Process the current event in the context of a voice/data call in the
* ORIGINATE state awaiting the first response to CC_SETUP_REQ. Corresponds
* to State 1 of Q.931.
*/
PRIVATE BOOLEAN near CCBNI1_CallOriginate( event )

int *event;                                /* Event input to the state */

{
   int     cause = 0;
   BYTE    cause_ie; 
   BYTE    chan_id;
   BYTE    exclusive;
   BYTE    event_b;
   BOOLEAN struct_err;
   int     valid_signal;
   BYTE    signal;
   int     state;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.)
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_OFFHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_ONHOOK_RELEASING;
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
         CCBNI1_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NI1_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_ALERTING_IND:
         /* ALERTING may be the first network response to SETUP
         *  and it will transport the network's B-channel choice.
         *  The channel will be reported to the USR when the CONNECT
         *  is received.
         */
         if (CCBRI_AcceptChanId( &chan_id, &exclusive) == NOERR) {
             CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
             CCB->State = NI1_ROUTINGOUT;
             /*
              * Signal IE is mandatory, reject the call if missing. 
              */
             valid_signal = CCBRI_AcceptSignal(&signal);
             if (valid_signal != NOERR) {
                 CCBRI_RejectCall(valid_signal, SIGNAL_IE);
                 CCB->State = NI1_OFFHOOK_RELEASING;
             } else {
                /*
                 * inform the HOST.
                 */
                if (CCBRI_GetOutInfo(HOST_ALERTING)) {
                   /*
                    * total length of private IE's = 4.
                    */
                    OutPkgInfo[ OutPkgInfoLen++ ] = 4;
                    OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
                    OutPkgInfo[ OutPkgInfoLen++ ] = 2;
                    OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
                    OutPkgInfo[ OutPkgInfoLen++ ] = chan_id;
                    CCBRI_MailtoHost( CCB->CopyPublicMsg );
                }
             }
         }
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
         if (( cause = CCBRI_AcceptChanId( &chan_id, &exclusive )) == NOERR )
            {
            CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
#ifdef TASK_KB
            CCBRI_StartDigitDetect();    /* Condition hdwr for dialing */
            CCBRI_StartConnect( CCB->B_Chan );    /* DIAL TONE in-band */
#endif
            CCB->State = NI1_DIALING;
            CCBRI_TRACE( TR_DIALTONE );
            }
         else
            {
            CCBRI_RejectCall( cause, CHANNEL_ID_IE );
            CCB->State = NI1_OFFHOOK_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
            }
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
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
            CCB->State = NI1_ROUTINGOUT;
            CCBRI_TRACE( TR_ROUTINGOUT );
            /*
             * Make sure the B-channel is not already in use.
             */
            state = CCBRI_SetNewChan(chan_id, CHANNEL_BUSY, FALSE);
            if (state != NOERR) {
                CCBRI_RejectCall(REQ_CHANNEL_NOT_AVAIL, NO_DIAG);
                CCB->State = NI1_OFFHOOK_RELEASING;
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
                    OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
                    OutPkgInfo[ OutPkgInfoLen++ ] = chan_id;
                    CCBRI_MailtoHost( CCB->CopyPublicMsg );
                }
            }
         } else {
            /*
             * The en-block transmission is complete.  This is the first
             * response back and there is no Channel Id  - reject the call
             * Including the diagnostic causes problems when timeouts occur,
             * specifically T308, causes extra octets to be sent out. 
             */
            CCBRI_RejectCall(cause, NO_DIAG);
            CCB->State = NI1_OFFHOOK_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
         }
         break;

      case CC_INFO_IND:
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
         CCB->State = NI1_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE

         cause = CCBRI_AcceptCause ( &cause_ie );
         if ( cause_ie != 0x00 )
            cause = cause_ie;
         event_b = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall(&event_b, cause);
         CCBRI_ReleaseCall();

         /*
          * if the cause is "requested facility not subscribed", the
          * router could send the spid, but will not do this now.
          */
         if (isdn_debug_ok(q931_debug, Dsl_ID) || isdn_debug) {
             buginf("\nISDN %s: Setup was rejected, cause = %x",
                    DSL_TO_NAME(Dsl_ID), cause);
         }
#endif
         break;

      case CC_DISCONNECT_IND:
         if ( CCBNI1_DisconnectInd() )
            {
#ifdef SBC_INTERFACE
            CCB->State = NI1_OFFHOOK_RELEASING;
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
               CCB->State = NI1_LOCKOUT;
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
               CCB->State = NI1_CONNECTED;
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
            CCB->State = NI1_OFFHOOK_RELEASING;
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
* CCBNI1_CallRoutingIn  *
*************************
*
* Process the current event in the context of a voice call in the
* ROUTINGIN state, the general purpose engaged, but non-CONVERSATION, state.
* This state is typically associated with the presentation of some
* informational tone such as ringback or some other call progress tone
* while the user is standing by.  Corresponds to State 9 of Q.931.
*
*/
PRIVATE BOOLEAN near CCBNI1_CallRoutingIn( event )

int *event;                                /* Event input to the state */

{
   BYTE event_b;
   BYTE cause;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_ALERTING:
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
            {
            /* Add CHANNEL ID info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CCB->B_Chan,
                                    0,                           
                                    B_CHANNEL_UNITS,   /* type: H or B */
                                    FALSE,               /* slot map ? */
                                    FALSE,              /* exclusive ? */
                                    0,          /* interface id length */
                                    NULL );            /* interface id */

            CCBRI_MailtoL3();                  /* Send CC_ALERTING_REQ */
            CCB->State   = NI1_RINGING;
            }
         break;

      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = NI1_CONNECTING;
         break;

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_OFFHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_ONHOOK_RELEASING;
         break;


#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* The caller hangs up.  The current call setup
         *  will be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         CCBNI1_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NI1_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
         CCB->State = NI1_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
          event_b = HOST_DISCONNECT_ACK;
          CCBRI_AcceptCause( &cause );
          CCBRI_DisconnectHostCall(&event_b, cause);
          CCBRI_ReleaseCall();
#endif
         break;

      case CC_DISCONNECT_IND:
         CCBNI1_DisconnectInd();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_INFO_IND:
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0.
             */
             CCBRI_MailtoHost( TRUE );
            }
         break;

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
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
* CCBNI1_CallRoutingOut  * 
**************************
*
* Process the current event in the context of a voice call in the
* ROUTINGOUT state, the general purpose engaged, but non-CONVERSATION, state.
* This state is typically associated with the presentation of some
* informational tone such as ringback or some other call progress tone
* while the user is standing by. Corresponds to states 3 and 4 of Q.931.
*
*/
PRIVATE BOOLEAN near CCBNI1_CallRoutingOut( event )

int *event;                                /* Event input to the state */

{
   BYTE cause = 0;
   BYTE event_b;
#ifdef SBC_INTERFACE
   BYTE trash;
#endif 
   BYTE   prog_desc;
   BYTE   signal;
   int    k;    
   int    valid_signal;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_OFFHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NI1_ONHOOK_RELEASING;
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
         CCBNI1_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = NI1_ONHOOK_RELEASING;
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

         valid_signal = CCBRI_AcceptSignal(&signal);
         if (valid_signal != NOERR) {
             CCBRI_RejectCall(valid_signal, SIGNAL_IE);
             CCB->State = NI1_OFFHOOK_RELEASING;
         } else {

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
         }  
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
         CCB->State = NI1_CONNECTED;
         break;


      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
         CCB->State = NI1_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
         trash = cause;
#endif
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         event_b = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall(&event_b, cause);
         CCBRI_ReleaseCall();
#endif
         break;

      case CC_DISCONNECT_IND:
         CCBNI1_DisconnectInd();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
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

      case CC_INFO_IND:
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0.
             */
             CCBRI_MailtoHost( TRUE );
            }
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


/************************************************************************
*                     O T H E R   F U N C T I O N S                     *
*************************************************************************
*/

/*********************
* CCBNI1_CheckEID    *
**********************
*
* EID checks are performed for Initializing Terminals, that is the 
* EID on the incoming message is compared with the EID stored in the
* Host Table. 
* 
*/
PRIVATE BOOLEAN near CCBNI1_CheckEID (ces)
int *ces;                     /* we return the ces value to the caller */
{
  BOOLEAN result = FALSE; 
  int     tmp_usid, our_usid;
  int     tmp_tid, our_tid;
  int     tmp_inter, our_inter, ix;
  int     cause;

  /*
   * Need to know if the eid matches so when comparing directory numbers.
   */
  CCB->Eid_matched = FALSE;
  for (ix = 0; ix < MAX_EID; ix++) {
     CCBFIT_GetEpid(&our_usid, &our_tid, &our_inter, ix);
     cause = CCBFIT_AcceptEndpointId(&tmp_usid, &tmp_tid, &tmp_inter);
     if (cause == NOERR) {
        /*
         * verify USID/TID match
         */
        if ((tmp_usid == our_usid) ||
            (tmp_usid == 0) || tmp_usid == BROADCAST_USID_NI1) {
            if (((tmp_inter == 0) &&
                ((tmp_tid == our_tid) || (tmp_tid == BROADCAST_TID_NI1))) ||
                ((tmp_inter == 1) && (tmp_tid != our_tid))) {
               /*
                * USID/TID test positive, the call will proceed.
                */
               result = TRUE;
               /*
                * Have to mark that the CES has been matched, when check the
                * DN, it may incorrectly swap the ces.
                */ 
               CCB->Eid_matched = TRUE;
               break;
           } else {
               /*
                * If our spid has not answered, allow the call, could be a NIT
                */
               if (our_usid == EID_NOTASSIGNED) {
                   result = TRUE;
                   break;
               } else
                   result = FALSE;
           }
        } else {
           /*
            * No match in TID or USID - the call
            * will not proceed. But have to make sure that the eid is valid
            * in the router. It may be a NIT, but only check the first eid,
            * the logic gets too complicated.
            */
           if (our_usid == EID_NOTASSIGNED) {
               result = TRUE;
               break;
           } else
               result = FALSE;
        }
     } else {
        /*
         * Either we are not on a multipoint, or we do not have
         * our own USID/TID or the incoming message does not carry
         * USID/TID - we will let the call through.
         */
        result = TRUE;
        break; 
     }
  }
  if (result == TRUE) {
    *ces = ix + 1;
  }
  return (result);
}

/**************************
* CCBNI1_DecodeCallState  *
***************************
*
* Decode the state of the CCB from the CCB state variable.
* Dispatch the appropriate state function through the state function
* jump table.
*
*/
PUBLIC int  CCBNI1_DecodeCallState( event )

BOOLEAN *event;                            /* Event input to the state */

{
   BOOLEAN event_pending;
   int     result_code;

   /* Perform a range check on the value of CCB->State to prevent
   *  the strange and unusual.
   */
   if (( CCB->State > NI1_STATE_LOW_LIMIT ) && ( CCB->State < NI1_STATE_HIGH_LIMIT ))
      {
      /* Jump to enter the state function.
      */
      event_pending     = ( *Ccb_State_Func_NI1[ CCB->State ] )( event );
      result_code       = ( event_pending ) ? EVENT_PENDING : NO_ERROR;
      }
   else
      {
      CCBRI_Error( CCBRI_DECODE_CALL_STATE, INVALID_STATE, CCB->State,
                   *event, FALSE );
                   /* *event, TRUE );	=== */
      result_code = INVALID_STATE;
      }

   return( result_code );
}

#ifdef SBC_INTERFACE
/*********************
* CCBNI1_Disconnect  *
**********************
*
* Standard actions for hangup by the voice phone.  This function initiates
* sending of CC_DISCONNECT_REQ.  It is used whenever the voice phone goes
* on hook while engaged.
*
*/
PRIVATE void near CCBNI1_Disconnect( cause )

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
* CCBNI1_DisconnectInd  *
*************************
*
* Generic action upon receiving the CC_DISCONNECT_IND message
* for a network disconnect.
*
*/
PRIVATE BOOLEAN near CCBNI1_DisconnectInd()

{
   BYTE cause = 0;
   BYTE event;

    /* Build complaining cause info element (optional in this message)
    */
      cause = (( CCBRI_AcceptCause (&cause) == MANDATORY_IE_MISSING ) ?
                    MANDATORY_IE_MISSING : cause);

   /* Far end disconnect.  Send CC_RELEASE_REQ. We will retain connection
   *  to the B-Channel.
   */

      if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) )
         {
         CCBRI_MailtoL3();
         CCB->State = NI1_ONHOOK_RELEASING;
         }

      CCB->Cause = (int ) cause;
      event       = HOST_DISCONNECT;
      CCBRI_DisconnectHostCall( &event , cause );

   return( TRUE );
}


/**********************
* CCNI1_ReleaseCall   *
***********************
*
* Terminate a call and put the CCB to bed.
*
*/
PUBLIC void CCBNI1_ReleaseCall()

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
* CCBNI1_ReleasedLink  *
************************
*
* The data link is down at Layer 2.
* Remove CCBs from any context but LOCKOUT.  Users left OFF HOOK
* must retain a CCB in the LOCKOUT state to process their hangup.
*
*/
PUBLIC void CCBNI1_ReleasedLink (ces, dsl_id)
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
      /*
       * a ces == 0 (BROADCAST) means the Dsl has been deactivated
       */
      if ((CCB->Ces == ces) || (ces == BROADCAST_CES)) {
         /* notify the HOST:
         */
         if (CCB->B_Chan)
            CCBRI_ReleaseChan();
         event_b = HOST_DISCONNECT_ACK;
         CCB->Cause = TEMPORARY_FAILURE;
         CCBRI_DisconnectHostCall(&event_b,CCB->Cause);
         CCBRI_RemoveCCB(CCB);
      }
      CCB = tmp_ccb;
   }
}

/******************
* CCBNI1_Restart  *
*******************
*
* B-Channel restart indication received.  It is assumed that Layer 3
* Protocol Control (L3_Restart) has already analyzed the message for
* mandatory information element presence and coding correctness.  Therefore
* we will proceed to restart channels without further checking based upon
* the criteria L3 Protocol Control used to restart at that level.
*
*/
PUBLIC void CCBNI1_Restart( dsl_id )

   BYTE     dsl_id;

{
    int     restart_class;
    int     b_chan;
#ifdef SBC_INTERFACE  
    int     i;
#endif 
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
         CCBNI1_RestartChan( b_chan, dsl_id );
#ifdef DEBUG
      printf ( "CCBNI1_Restart...ALL channels restarted.\n" );
#endif
#ifdef SBC_INTERFACE
      for ( i = 0; i < MAX_VOICE_CALLS; i++ )
         {
         if ( Hcb[ dsl_id ].Ccb_In_Use[ i ] )
            {
            CCB = Hcb[ dsl_id ].Ccb_In_Use[ i ];
            if (( CCB->State != NI1_LOCKOUT ) ||
                ( CCB->HcbIndex != Hcb[ dsl_id ].Active_Call ))
               CCBRI_ReleaseCall();
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
         printf ( "CCBNI1_Restart...channel %d restarted.\n", b_chan );
#endif
         CCBNI1_RestartChan( b_chan, dsl_id );   /* Restart a channel */
         }
      }
}




/**********************
* CCBNI1_RestartChan  *
***********************
*
* Restart an individual B-Channel.
*
*/
PRIVATE void near CCBNI1_RestartChan( b_chan, dsl_id )

   int   b_chan;                            /* Channel to be restarted */
   BYTE  dsl_id;                            /* digital subscribel loop */

{
#ifdef SBC_INTERFACE
   int   hw_state;
   int   index;
#endif 
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
         CCBRI_ReleaseCall();                  /* Terminate the call */
         if ( hw_state == OFFHOOK )
            {
            /* We reinstall the call into a new CCB so it may hangup.
            */
            Hcb[ dsl_id ].Hw_State = OFFHOOK;      /* Restore hw state */
            if (( index = CCBRI_GetHcbIndex( dsl_id )) < MAX_VOICE_CALLS )
               {
               if ( CCBRI_GetNewCCB( NI1_IDLE,
                                     NO_CHANNEL,
                                     SIGNALING_SAPI,
                                     SIGNALING_CES,
                                     index,
                                     CCBRI_GetNewCall_Id()))
                  {
                  Hcb[ dsl_id ].Ccb_In_Use[ index ] = CCB;
                  Hcb[ dsl_id ].Active_Call = index;
                  CCB->State = NI1_LOCKOUT;
                  }
               }
            }
         }
      }
   CC_CHAN_ReleaseChanbri( b_chan, B_CHANNEL_UNITS, Dsl_ID ); /* Remove assign */
#endif
#ifdef COMMAND_INTERFACE
   /* Check if the channel being restarted is in use.
   */
   CCB = CCB_Head[ dsl_id ];
   while ( CCB )
      {
      if ( CCB->B_Chan == b_chan )
         {
         if ( CCB->State != NI1_IDLE )
            {
            CCBRI_ReleaseChan();
#ifdef COMMAND_INTERFACE
            CCBRI_RejectHostCall( TEMPORARY_FAILURE );
            CCBRI_RemoveCCB (CCB);
#endif
#ifdef SBC_INTERFACE
            CCB->State = NI1_LOCKOUT;
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
#ifdef COMMAND_INTERFACE
/*******************
* CCBNI1_SetupInd  *
********************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function
* because of its inherent complexity.
*
*/
PRIVATE void near CCBNI1_SetupInd()

{
   int     cause;
   int     length;
   int     k;
   BYTE    chan_id;
   BYTE    exclusive;
   BYTE    call_num[ PHONE_NUMB_LEN + 1 ];
   BYTE    progress_description;
   H_ID  hostcb;
   int   ces; 

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

   /* inform the HOST.
   */
   if ( CCBRI_GetOutInfo( HOST_INCOMING_CALL ) )
      {
      OutPkgInfoLen++;
      k = 5;                      /* bearer capability must be present */
      }
   else
      {
      goto bailout;
      } 

   /* Look at the bearer capability IE.
   */
   if (( cause = CCBRI_AcceptBearerCap()) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected.
      */
      CCBRI_RejectSetup( cause, BEARER_CAPABILITY_IE );
      CCBNI1_ReleaseCall();
      CCBRI_FreeCallBufs();
      goto bailout;
      }

   /* Next we look at the Channel ID.
   */
   if (( cause = CCBRI_AcceptChanId( &chan_id, &exclusive)) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected.
      */
      if ( cause == NO_CIRCUIT_AVAILABLE )
         CCBRI_RejectSetup( NO_CIRCUIT_AVAILABLE, 0 );
      else
         CCBRI_RejectSetup( cause, CHANNEL_ID_IE );
      CCBNI1_ReleaseCall();
      CCBRI_FreeCallBufs();
      goto bailout;
      }
   else
      {
      if ((cause = CCBRI_SetNewChan((int) chan_id, CHANNEL_BUSY, FALSE)) != NOERR) {
        CCBRI_RejectSetup(cause, NO_DIAG);
        CCBNI1_ReleaseCall();
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
      }

    /*
     * The first thing checked is the EID. If the SETUP is incorrectly 
     * addressed ignore it.
     */
    hostcb = &Host_CB [Dsl_ID] [ CCB->Host_Specifier ];
    if ((hostcb->Initializing_Terminal) && (CCBNI1_CheckEID(&ces) == FALSE)) {
        /*
         * Inform the user, since conforming to the spec changed functionality
         * for some configs.
         */ 
        errmsg(&msgsym(INVALID_EID, ISDN), DSL_TO_NAME(Dsl_ID));
        if (isdn_debug_ok(q931_debug, Dsl_ID) || isdn_debug)
            buginf("\nISDN %s: Ignoring incoming call, EndPoint ID mismatch",
                   DSL_TO_NAME(Dsl_ID));
        CCBNI1_ReleaseCall();
        CCBRI_FreeCallBufs();
        goto bailout;
    }  else {
       CCB->Ces = ces; 
    }

   /* Note that this is an optional element.  If provided, we attempt
    * to match numbers. Note: This is where we have to determine which ces
    * value to use (1 or 2). The DMS uses 2 TEIs and 2 ces values as well.
    */
   cause = CCBRI_AcceptCalledNumb( &length, Dsl_ID );
   /*
    * If the element was present, but was coded incorrectly or the
    * called number did not match ours - ignore this ie.
    */
   if (cause != INVALID_ELEM_CONTENTS) {
       if (cause == NOERR) {
           k+= length;
       } else {
           errmsg(&msgsym(INVALID_CALLEDNUMBER, ISDN), DSL_TO_NAME(Dsl_ID));
           if (isdn_debug_ok(q931_debug, Dsl_ID) || isdn_debug)
               buginf("\nISDN %s: Ignoring incoming call, Called Party Number "
                      "mismatch", DSL_TO_NAME(Dsl_ID));
           CCBNI1_ReleaseCall();
           CCBRI_FreeCallBufs();
           goto bailout;
       }
   }

   /* 
    * Compare DN with Bearer Capability
    * Note that this is an optional element.  If provided, we attempt
    * to match numbers.
    */
   if (( cause = CCBRI_AcceptCallingNumb(call_num,&length )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly. We still process 
      *  the call. 
      */
      ;
      }
   else
      {
      if ( cause == NOERR )
         k+= length;
      }

   if ((( cause = CCBRI_AcceptProgress( &progress_description )) == NOERR )
        && ( IE ) )
      {
      /* The element was present - attach it 
      */

      OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
      OutPkgInfo[ OutPkgInfoLen++ ] = 2;
      OutPkgInfo[ OutPkgInfoLen++ ] = 0x00;    /* Location is not used */
      OutPkgInfo[ OutPkgInfoLen++ ] = progress_description;
      k+= 4;   
      }  

   /* This is where the ces going to process_rxstate has to be adjusted.
    * The CCB->Ces value is set in CCBRI_AcceptCalledNumber and will 
    * determine which ces (1 or 2) to use. The default is 1. ===
    */
   OutPkgInfo[2] = CCB->Ces;

   /* inform the HOST
   */
   if ( OutPkgInfo )
      {
      OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
      CCBRI_MailtoHost( CCB->CopyPublicMsg );
      CCB->State = NI1_OFFERED;
      }

bailout:
   return;
}
#endif 

#ifdef  SBC_INTERFACE
/*******************
* CCBNI1_SetupInd  *
********************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function
* because of its inherent complexity.
*
*/
PRIVATE void near CCBNI1_SetupInd()

{
   int     cause;
   int     length;
   int     k;
#ifdef TASK_KB
   BYTE    ring_type;
#endif
   BYTE    chan_id;
   BYTE    exclusive;
   CCB_ID  active_ccb;
   BYTE    call_num[ PHONE_NUMB_LEN + 1 ];
   BYTE    sending_complete;
   BYTE    progress_description;
#ifdef N_AMERICA_FIT
   int     ces = 0; 
#endif 
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
   */

   /* The first thing we check is EID.  If the SETUP is incorrectly 
   *  addressed we ignore it.
   */
#ifdef N_AMERICA_FIT 
    hostcb = &Host_CB [Dsl_ID] [ CCB->Host_Specifier ];
    if ( (hostcb->Initializing_Terminal) && (CCBNI1_CheckEID(@ces) == FALSE) )  
       {
       CCBNI1_ReleaseCall();
       CCBRI_FreeCallBufs();
       goto bailout;
       }  
    else
       {
       if ( ces ) 
          CCB->Ces = ces; 
       }
#endif 

   /* Note that this is an optional element.  If provided, we attempt
   *  to match numbers.
   */
   if (( cause = CCBRI_AcceptCalledNumb( &length, Dsl_ID )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly or the
      *  called number did not match ours - ignore this ie.
      */
      ;
      }

   /* Look at the bearer capability IE.
   */
   if (( cause = CCBRI_AcceptBearerCap()) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected.
      */
      CCBRI_RejectSetup( cause, BEARER_CAPABILITY_IE );
      CCBRI_ReleaseCall();

      goto bailout;
      }

    /* 
     *  Compare DN with Bearer Capability
    */

   /* Next we look at the Channel ID.
   */
    if (( cause = CCBRI_AcceptChanId( &chan_id, &exclusive)) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected.
      */
      if ( cause == NO_CIRCUIT_AVAILABLE )
         CCBRI_RejectSetup( NO_CIRCUIT_AVAILABLE, 0 );
      else
         CCBRI_RejectSetup( cause, CHANNEL_ID_IE );
      CCBRI_ReleaseCall();
      goto bailout;
      }
   else
      {
#ifdef CHANNEL_TABLE
      if ( (cause = CCBRI_SetNewChan( (int) chan_id,
                                      CHANNEL_BUSY,
                                      TRUE ) ) != NOERR )
        {
        CCBRI_RejectSetup( cause, 0 );
        CCBRI_ReleaseCall();
        goto bailout;
        }
#endif
      }
   /* Note that this is an optional element.  If provided, we attempt
   *  to match numbers.
   */
   if (( cause = CCBRI_AcceptCallingNumb(call_num,&length )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly. We still process 
      *  the call. 
      */
      ;
      }


   if ((( cause = CCBRI_AcceptProgress( &progress_description )) == NOERR )
        && ( IE ) )
      {
      }  


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
      CCBRI_ReleaseCall();
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
                                 CCB->B_Chan,
                                 0,
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
            CCB->State = NI1_RINGING;
            CCBRI_TRACE( TR_RINGING );
            }                                          /* Alerting req */
         }                                          /* Call Proceeding */
      }


bailout:

   return;
}
#endif
/*******************
* CCBNI1_Shutdown  *
********************
*
* Reset Call Control
*
*/
PUBLIC void CCBNI1_Shutdown (dsl)
BYTE dsl;
{
    CCB_ID temp_ccb;
    int i;

    Dsl_ID = dsl;
    CCB = CCB_Head[dsl];
    while (CCB) {
        temp_ccb = CCB->Next;
        CCBNI1_ReleaseCall();
        CCB = temp_ccb;
    }

    for (i=0; i < MAX_HOSTS; i++)
        LIF_StopTimer (Host_CB[dsl][ i ].Timer_T415 );
}
