/* $Id: ccbdms.c,v 3.3.12.1 1996/04/30 23:06:44 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccbdms.c,v $
 *------------------------------------------------------------------
 * ccbdms.c  ISDN DMS-100 BRI Specific Call Control Functions
 *
 * March, 1992  Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccbdms.c,v $
 * Revision 3.3.12.1  1996/04/30  23:06:44  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/15  17:27:13  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.2  1995/11/17  17:39:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccbdms.c
VERSION: 3.21
DESCRIPTION: ISDN DMS-100 BRI Specific Call Control Functions

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
(Basic Rate Access) telephone in a manner compatible with a DMS-100
Central Office switch in accordance with Northern Telecom NIS S208-5,
Issue 1.0 of April 1990.  The telephone is assumed to be implemented
using subscried in-band call progress tones and to send address
information via overlapped sending.

This version does not support any supplementary services.
Various hooks have also been added so that the functionality of this
module may be extended with reasonable effort to support various
supplementary services in the future.  Specifically:

 1. A generalized scheme for managing multiple Call Control Blocks
    (CCBs) exists so that multiple passive calls as well as an active
    call may be manipilated simultaneously, although this functionality
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

Although this implementation relies upon in-band call progress tones, the
function CCBDMS_AcceptSignal is designed to recover the signal value, which
is an initial requirement to build a local tone generation capability.

Various simplifications have been made which reflect the fact that
this is not the software designed to run, as is, a real hardware
device.  For example, the directory number is simply fixed at compile
time, where a real product might be expected to have NV ram for such
volatile information, with a user-programming capability.

#endif

The Call Control is assumed to reside on a point-to-multipoint
interface wherein SETUP is broadcast to the TEs.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.


*************************************************************************
*************************************************************************
*/

#define     CCBDMS_C                                    /* module name */

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
#ifdef BRI_DMS100          /* === */
#include    "tune.h"
#include    "gendef.h"
#include    "lif.h"
#include    "ccie.h"
#include	"gp.h"		/*===*/
#include    "ccbri.h"
#include    "cc_chan.h"
#include "logger.h"
#include "msg_isdn.c"      /* Not a typo, see logger.h */ 

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* Definitions for CCB Call States
*/
enum CCB_State
   {
   DMS_STATE_LOW_LIMIT,                              /* Sizing element */
   DMS_IDLE,                                               /* CCB idle */
   DMS_RINGING,                                       /* Being alerted */
   DMS_CONNECTED,                                      /* Conversation */
   DMS_DIALING,                                             /* Dialing */
   DMS_LOCKOUT,                           /* Disconnected but off hook */
   DMS_OFFHOOK_RELEASING,  /* Waiting for release confirmation on hook */
   DMS_ONHOOK_RELEASING,  /* Waiting for release confirmation off hook */
   DMS_ORIGINATE,                    /* Await response to CC_SETUP_REQ */
   DMS_ROUTING,     /* Engaged but neither dialing nor in conversation */
   DMS_CALL_OFFERED,                           /* Call offered to Host */
   /* DMS_HOST_DISCONNECTING,      HOST DISCONNECT sent to Host ===    */
   DMS_STATE_HIGH_LIMIT                              /* Sizing element */
   };

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
*              F U N C T I O N   D E C L A R A T I O N S                *
*************************************************************************
*/
PRIVATE    BOOLEAN  near    CCBDMS_CallAlerting        (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CallConversation    (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CallDialing         (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CallIdle            (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CallLockout         (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CallOffHookRelease  (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CallOnHookRelease   (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CallOriginate       (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CallRouting         (int *);
PRIVATE    BOOLEAN  near    CCBDMS_CheckEID            (int *);
/* PRIVATE    void     near    CCBDMS_Disconnect          (int);   */
PRIVATE    BOOLEAN  near    CCBDMS_DisconnectInd       (void);
/* PRIVATE    BOOLEAN  near    CCBDMS_DisconnectingHost   (int *); */
PRIVATE    BOOLEAN  near    CCBDMS_CallOffered         (int *);
PRIVATE    void     near    CCBDMS_RestartChan         (int, BYTE);
PRIVATE    void     near    CCBDMS_SetupInd            (void);

/************************************************************************
*             S T A T E   E N T R Y   J U M P   T A B L E               *
*************************************************************************
*/

typedef BOOLEAN (near *CC_STATES)( int * );

/* NOTE: this table MUST strictly correspond to the CCB_State table above !!
*/
static const CC_STATES Ccb_State_Func_Dms[ DMS_STATE_HIGH_LIMIT ] =
   {
   NULL,
   CCBDMS_CallIdle,
   CCBDMS_CallAlerting,
   CCBDMS_CallConversation,
   CCBDMS_CallDialing,
   CCBDMS_CallLockout,
   CCBDMS_CallOffHookRelease,
   CCBDMS_CallOnHookRelease,
   CCBDMS_CallOriginate,
   CCBDMS_CallRouting,
   CCBDMS_CallOffered
   /* CCBDMS_DisconnectingHost === */
   };

/************************************************************************
*                C A L L   S T A T E   F U N C T I O N S                *
*************************************************************************
*/
/**********************
* CCBDMS_CallAlerting *
***********************
*
* Process the current event in the context of a voice call in the
* RINGING state.  This terminal is being rung.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallAlerting( event )

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

      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = DMS_ROUTING;
         break;

      case HOST_DISCONNECT:
         /* process the disconnect message from the HOST.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = DMS_ONHOOK_RELEASING;
         break;

		/* === added the following state */
       case HOST_DISCONNECT_ACK:
         /* process the disconnect_ack message from the HOST.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = DMS_ONHOOK_RELEASING;
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
      case KP_OFF_HOOK:
         /* Local off hook - we will answer the incoming call
         *  by sending CC_SETUP_RESP (CONNECT) downwards.
         *  The response to this message (CC_SETUP_COMP_IND) will be
         *  received in the ROUTING state to complete the connection.
         */
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ) )
            CCBRI_MailtoL3();
         Hcb[ DEFAULT_DSL ].Hw_State = OFFHOOK;
         CCB->State = DMS_ROUTING;
         CCBRI_TRACE( TR_ROUTING );
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
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;        /* Not BELL */
#endif
         CCBDMS_DisconnectInd();
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         CCBDMS_ReleaseCall();
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
            CCB->State = DMS_ONHOOK_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_ALERTING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/**************************
* CCBDMS_CallConversation *
***************************
*
* Process the current event in the context of a voice call in the
* CONNECTED state.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallConversation( event )

int *event;                                /* Event input to the state */

{
/*   int     index;                                      Hcb index === */


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
         CCB->State = DMS_OFFHOOK_RELEASING;
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
         CCBDMS_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = DMS_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_DISCONNECT_IND:                    /* Far end disconnect */
          CCBDMS_DisconnectInd();
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
#ifdef SBC_INTERFACE
         CCB->State = DMS_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
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
            CCB->State = DMS_OFFHOOK_RELEASING;
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

/*********************
* CCBDMS_CallDialing *
**********************
*
* Process the current event in the context of a voice call in the
* DIALING state. Process overlap sending.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallDialing( event )

int *event;                                /* Event input to the state */

{
#ifdef COMMAND_INTERFACE
   /* int  cause; === */
   int  k;
   BYTE prog_desc;
   BYTE signal;
#endif
#ifdef SBC_INTERFACE
   BYTE digit_as_byte;
#endif
/*   BYTE chan_id;
   BYTE exclusive; === */


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
         CCB->State = DMS_OFFHOOK_RELEASING;
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
               CCIE_BuildKeypad_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    &digit_as_byte, 1, TRUE );
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
         CCBDMS_Disconnect( NORMAL_CLEARING );
         CCB->State = DMS_ONHOOK_RELEASING;
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
         /* if ( CCBRI_GetOutInfo( HOST_CONNECT ) ) === */
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         break;

      case CC_CALL_PROC_IND:
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
         /* The B Channel should be connected now, but just to be
         *  sure
         */
         if ( CCB->Chan )
            CCBRI_StartConnect (CCB->B_Chan);
#endif
         CCB->State = DMS_ROUTING;
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
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         break;

      case CC_ALERTING_IND:
         break;

      case CC_DISCONNECT_IND:
         /* Far end disconnect.  
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
         CCBDMS_DisconnectInd();
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
#ifdef SBC_INTERFACE
         CCB->State = DMS_LOCKOUT;
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
            CCBRI_RemoveCCB( CCB );
            }
#endif
         break;

      case CC_PROGRESS_IND:
         /* May be received if invalid (incomplete) addressing was sent.
         *  It is accompanied by in-band reorder tone.
         */
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_PROGRESS ) )
            {
            k = 0;
            OutPkgInfoLen++;          /* leave space for totoal length */
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
            CCB->State = DMS_OFFHOOK_RELEASING;
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

/******************
* CCBDMS_CallIdle *
*******************
*
* Process the current event in the context of a voice call in the
* IDLE state.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallIdle( event )

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
            /* CCBRI_RemoveCCB( CCB ); === */
			CCBDMS_ReleaseCall();	/* === */
            }
         else
            {
            CCB->State = DMS_ORIGINATE;
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
         /* This is a new outgoing call setup. The CC will send
         *  CC_SETUP_REQ.  If the data link is not established, Layer 3
         *  will detect this and request establishment before the SETUP
         *  is processed.
         */
         Hcb[ DEFAULT_DSL ].Hw_State = OFFHOOK;
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
            CCB->State = DMS_ORIGINATE;
            }
         else
            {
            CCB->State = DMS_LOCKOUT;
            }
         break;

#endif
      case CC_SETUP_IND:
         CCBDMS_SetupInd();                           /* Incoming call */
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         if ( CCB )
            CCBDMS_ReleaseCall();
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
            CCBDMS_ReleaseCall();
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

   return( FALSE );
}

/*********************
* CCBDMS_CallLockout *
**********************
*
* Process the current event in the context of a voice call in the
* LOCKOUT (off-hook, but released from the NETWORK) state.  The user
* is "locked-out" from originating again until an on-hook/off-hook
* cycle is completed.  This state is retained after the NETWORK
* has released the call and the user has failed to hang up.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallLockout( event )

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
         CCBDMS_ReleaseCall();  /* Release from lockout, drop the CCB. */
         break;
#endif

      default:
         /* Unexpected message.  No message is expected here from Layer 3
         *  since it will have cleared the Call_ID.  If L3 recieves a new
         *  SETUP, it will be processed in CC_DMSB in a new CCB in the
         *  IDLE state.
         */
         CCBRI_Error( CCBRI_CALL_LOCKOUT, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/*********************
* CCBDMS_CallOffered *
**********************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to State 6 of Q.931.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallOffered( event )

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
            CCBRI_RejectSetup( cause, 0 );
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
            CCBRI_MailtoL3();                /* Send CC_BROADCAST_RESP */
            if ( CCBRI_GetOutPkt( CC_CALL_PROC_REQ, FALSE ))
               {
               /* Add CHANNEL ID info element.
               */
               OutPkt->InfoLen +=
                  CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CHANNELS_LISTED,    /* chan sel */
                                       /* CCB->B_Chan, ===??? */
									   0,  /* ===???*/
                                       B_CHANNEL_UNITS,/* type: H or B */
                                       FALSE,            /* slot map ? */
                                       FALSE,           /* exclusive ? */
                                       0,       /* interface id length */
                                       NULL );         /* interface id */
               CCBRI_MailtoL3();
               CCB->State = DMS_ROUTING;
               }
            }
         break;

      case HOST_ALERTING:
         /* Indication that the Host has initiated alerting for the call
         */
         if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
            {
            CCBRI_MailtoL3();                /* Send CC_BROADCAST_RESP */
            }
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
            {
            /* Add CHANNEL ID info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CHANNELS_LISTED,/* channel selection */
                                    /* CCB->B_Chan, ===??? */
									0,  /* ===??? */
                                    B_CHANNEL_UNITS,   /* type: H or B */
                                    FALSE,               /* slot map ? */
                                    FALSE,              /* exclusive ? */
                                    0,          /* interface id length */
                                    NULL );            /* interface id */
            CCBRI_MailtoL3();
            }
         CCB->State = DMS_RINGING;
         break;

      case HOST_CONNECT:
         if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
            {
            CCBRI_MailtoL3();                /* Send CC_BROADCAST_RESP */
            }
         CCBRI_HostConnect();
         CCB->State = DMS_ROUTING;
         break;

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         /* Unexpected message.  No message is expected here from Layer 3
         *  since it will have cleared the Call_ID.  If L3 recieves a new
         *  SETUP, it will be processed in CC_DMS in a new CCB in the
         *  IDLE state.
         */
         CCBRI_Error( CCBRI_CALL_LOCKOUT, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/****************************
* CCBDMS_CallOffHookRelease *
*****************************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The local user
* is off-hook.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallOffHookRelease( event )

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

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in ONHOOK_RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = DMS_ONHOOK_RELEASING;
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
         CCB->State = DMS_ONHOOK_RELEASING;
         break;

#endif
      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
      case CC_TIMEOUT_IND:
#ifdef SBC_INTERFACE
         CCB->State = DMS_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            /* CCBRI_RemoveCCB( CCB ); === */
			CCBDMS_ReleaseCall();  /*===*/
            }
#endif
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
         CCBRI_Error( CCBRI_CALL_RELEASING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}


/***************************
* CCBDMS_CallOnHookRelease *
****************************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The local user
* is on-hook.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallOnHookRelease( event )

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
         CCB->State = DMS_OFFHOOK_RELEASING;
         break;

#endif
      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
      case CC_TIMEOUT_IND:
	  /*===*/
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
			}
	  /*===*/
         CCBDMS_ReleaseCall();
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


/***********************
* CCBDMS_CallOriginate *
************************
*
* Process the current event in the context of a voice call in the
* ORIGINATE state awaiting the first response to CC_SETUP_REQ.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallOriginate( event )

int *event;                                /* Event input to the state */

{
   int     cause;
   BYTE    chan_id;
   BYTE    exclusive;
   BOOLEAN struct_err;
   int     state;

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
      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = DMS_ONHOOK_RELEASING;
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
         CCBDMS_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = DMS_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
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
            CCB->State = DMS_DIALING;
            CCBRI_TRACE( TR_DIALTONE );
            }
         else
            {
            CCBRI_RejectCall(cause, NO_DIAG);
            CCB->State = DMS_OFFHOOK_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
            }
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

      case CC_CALL_PROC_IND:
         /*
          * This message indicates that digit transmission (en_bloc) is
          * complete.  This case is added for completeness.
          */
         if ((cause = CCBRI_AcceptChanId(&chan_id, &exclusive)) == NOERR) {
             state = CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, FALSE);
             if (state != NOERR) {
                 CCBRI_RejectCall(REQ_CHANNEL_NOT_AVAIL, NO_DIAG);  /* cause 44 */
                 CCB->State = DMS_OFFHOOK_RELEASING;
             } else {
                /*
                 * Connect the B Channel
                 */
                CCB->State = DMS_ROUTING;
                /*
                 * inform the HOST.
                 */
                if (CCBRI_GetOutInfo(HOST_PROCEEDING)) {
                   /* total length of private IE's = 4 */
                   OutPkgInfo[ OutPkgInfoLen++ ] = 4;
                   OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
                   OutPkgInfo[ OutPkgInfoLen++ ] = 2;
                   OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;      /* exclusive */
                   OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
                   CCBRI_MailtoHost(CCB->CopyPublicMsg);
                }
             }
         } else {
            CCBRI_RejectCall(cause, CHANNEL_ID_IE);
            CCB->State = DMS_OFFHOOK_RELEASING;
         }
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         /* CCBRI_ReleaseChan();  Disc and release chan if allocated === */
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated === */
         CCB->State = DMS_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            /* CCBRI_RemoveCCB( CCB ); === */
			CCBDMS_ReleaseCall();	/*===*/
            }
#endif
         break;

      case CC_DISCONNECT_IND:
         if ( CCBDMS_DisconnectInd() )
            {
#ifdef SBC_INTERFACE
            CCB->State = DMS_OFFHOOK_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
#endif
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
               /* CCBRI_ReleaseChan();  Disc and rls chan if allocated === */
#ifdef SBC_INTERFACE
               CCBRI_ReleaseChan();  /* Disc and rls chan if allocated === */
               CCB->State = DMS_LOCKOUT;
               CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
                CCBRI_RejectHostCall (TEMPORARY_FAILURE);
                CCBRI_RemoveCCB( CCB );
#endif
               }
            else
               {
               if ( Source == L3_ID )
                  CCBRI_RejectMsg( WRONG_MESSAGE, *event );
               CCBRI_Error( CCBRI_CALL_ORIGINATE, UNKNOWN_EVENT, *event, Source, FALSE );
               }
            }
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
            CCB->State = DMS_OFFHOOK_RELEASING;
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

/*********************
* CCBDMS_CallRouting *
**********************
*
* Process the current event in the context of a voice call in the
* ROUTING state, the general purpose engaged, but non-CONVERSATION, state.
* This state is typically associated with the presentation of some
* informational tone such as ringback or some other call progress tone
* while the user is standing by.
*
*/
PRIVATE BOOLEAN near CCBDMS_CallRouting( event )

int *event;                                /* Event input to the state */

{
   int  k;
   BYTE prog_desc;
   BYTE signal;
   BYTE chan_id;
   BYTE exclusive;
   int  valid_signal;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_ALERTING:
         /* Indication that the Host has initiated alerting for the call
         */
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
            {
            /* Add CHANNEL ID info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CHANNELS_LISTED, /* chan selection */
                                    /* CCB->B_Chan, ===??? */
									0,	/* ===??? */
                                    B_CHANNEL_UNITS,   /* type: H or B */
                                    FALSE,               /* slot map ? */
                                    FALSE,              /* exclusive ? */
                                    0,          /* interface id length */
                                    NULL );            /* interface id */

            CCBRI_MailtoL3();
            }
         CCB->State = DMS_RINGING;
         break;

      case HOST_DISCONNECT:
      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = DMS_ONHOOK_RELEASING;
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
      case KP_ON_HOOK:
         /* The caller hangs up.  The current call setup
         *  will be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         /* test case 9.6.4.1 & 9.6.4.4 
         */
         CCBDMS_Disconnect( UNSPECIFIED_CAUSE );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = DMS_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif
      case CC_ALERTING_IND:
         /*
          * Notification of far end alerting.
          * The B-Channel should already be connected.  Connect it
          * again just to be sure to receive in-band alerting tone.
          * The signal IE is mandatory for custom DMS100.
          */
         valid_signal = CCBRI_AcceptSignal(&signal);
         if (valid_signal != NOERR) {
             CCBRI_HostDisconnect(HOST_DISCONNECT);
             CCB->State = DMS_ONHOOK_RELEASING;
         } else {
             /*
              * inform the HOST.
              */
             if (CCBRI_GetOutInfo(HOST_ALERTING)) {
                 /* total length of private IE's = 0 */
                OutPkgInfo[ OutPkgInfoLen++ ] = 0;
                CCBRI_MailtoHost( CCB->CopyPublicMsg );
             }
         }
         break;

      case CC_SETUP_CONF:                    /* Translation of CONNECT */
      case CC_SETUP_COMP_IND:            /* Translation of CONNECT_ACK */
         /* The call was connected at the far end.  Connect the call
         *  at this end too (its probably already connected).
         *  The call progresses to the CONNECTED state.
         */
         /* try to get a new channel first, === added following if
         */
         if ( CCBRI_AcceptChanId( &chan_id, &exclusive ) == NOERR )
			{
			CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
			}
		/* added above if statement === */

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
         CCB->State = DMS_CONNECTED;
         break;

      case CC_PROGRESS_IND:
         /* Progress is an indication of interworking or some in-band
         *  call progress tone.
         */
         /* PROGRESS_IND is a mandatory info element here - check
         *  for it first:
         */
#ifdef SBC_INTERFACE
         if ( CCBRI_AcceptProgress( &prog_desc ) == NOERR )
            {
            /* Check for an expected progress indicator.
            */
            if (( prog_desc == CALL_NOT_END_TO_END_ISDN ) ||/* ind = 1 */
                ( prog_desc == IN_BAND_INFO ))              /* ind = 8 */
               {
               /* Connect the B_Chan to be sure it is connected to
               *  receive the call progress tone (NIS S208-5 6.1.1.2.4).
               */
#ifdef TASK_KB
               if ( CCB->B_Chan )
                  CCBRI_StartConnect( CCB->B_Chan );
#endif
               }
            }
#endif
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_PROGRESS ) )
            {
            k = 0;
            OutPkgInfoLen++;          /* leave space for totoal elngth */
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

      case CC_TIMEOUT_IND:
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
#ifdef SBC_INTERFACE
         CCB->State = DMS_LOCKOUT;
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
            CCBRI_RemoveCCB( CCB );
            }
#endif
         break;

      case CC_DISCONNECT_IND:
         CCBDMS_DisconnectInd();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_INFO_IND:
          /* ignore */
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
         CCBRI_Error( CCBRI_CALL_ROUTING, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}
#ifdef 0
/***************************
* CCBDMS_DisconnectingHost *
****************************
*
* CCBDMS_DisconnectingHost.  This State corresponds to Q.931 State 12
* A Disconnect Indication has been received and HOST_DISCONNECT has been
* mailed to the Host
*/
PRIVATE BOOLEAN near CCBDMS_DisconnectingHost( event )

int *event;                                /* Event input to the state */

{
  BYTE event_b;
  BYTE diag[1];

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
         LIF_StopTimer ( CCB->CCDMSTimer );
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
           {
           /* Build complaining Cause info element (optional in this message)
           */
           diag[ 0 ] = 0;
           OutPkt->InfoLen +=
              CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                  CCB->Cause, FALSE, diag );
           CCBRI_MailtoL3();
           CCB->State = DMS_ONHOOK_RELEASING;
           }
         break;

      case DISC_HOST_EVENT:                       /* Timeout from Host */
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
           {
           /* Build complaining Cause info element (optional in this message)
           */
           diag[0] = 0;
           OutPkt->InfoLen +=
              CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   CCB->Cause, FALSE, diag );
           CCBRI_MailtoL3();
           event_b = HOST_DISCONNECT_ACK;
           CCBRI_DisconnectHostCall(&event_b, CCB->Cause);
           CCB->State = DMS_ONHOOK_RELEASING;
           }
         break;

#endif
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
          LIF_StopTimer ( CCB->CCDMSTimer );
          event_b = HOST_DISCONNECT_ACK;
          CCBRI_DisconnectHostCall(&event_b, CCB->Cause);
          CCBDMS_ReleaseCall();
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
            CCB->State = DMS_OFFHOOK_RELEASING;
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
#endif

/************************************************************************
*                     O T H E R   F U N C T I O N S                     *
*************************************************************************
*/

/*********************
* CCBDMS_CheckEID    *
**********************
*
* EID checks are performed for initializing terminals, that is the 
* EID on the incoming message is compared with the EID stored in the
* Host Table. 
* 
*/
PRIVATE BOOLEAN near CCBDMS_CheckEID (ces)
int *ces;                     /* we return the ces value to the caller */
{
    BOOLEAN result = FALSE; 
    int     tmp_usid, our_usid;
    int     tmp_tid, our_tid;
    int     tmp_inter, our_inter, ix;
    int     cause;

    for (ix = 0; ix < MAX_EID; ix++) {
        CCBFIT_GetEpid(&our_usid, &our_tid, &our_inter, ix);
        cause = CCBFIT_AcceptEndpointId(&tmp_usid, &tmp_tid, &tmp_inter);
        if (cause == NOERR) {
            /*
             * verify USID/TID match
             */
            if ((tmp_usid == our_usid) ||
                (tmp_usid == 0) || (tmp_usid == BROADCAST_USID_NI1)) {
                if (((tmp_inter == 0) &&
                    ((tmp_tid == our_tid) || (tmp_tid == BROADCAST_TID_NI1))) ||
                    ((tmp_inter == 1) && (tmp_tid != our_tid))) {
                    /*
                     * USID/TID test positive, the call will proceed.
                     */
                    result = TRUE;
                    break;
                } else
                    result = FALSE;
           } else {
               /*
                * no match in TID or USID - the call
                * will not proceed.
                */
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

/*************************
* CCBDMS_DecodeCallState *
**************************
*
* Decode the state of the CCB from the CCB state variable.
* Dispatch the appropriate state function through the state function
* jump table.
*
*/
PUBLIC int  CCBDMS_DecodeCallState( event )

BOOLEAN *event;                            /* Event input to the state */

{
   BOOLEAN event_pending;
   int     result_code;

   /* Perform a range check on the value of CCB->State to prevent
   *  the strange and unusual.
   */
   if (( CCB->State > DMS_STATE_LOW_LIMIT ) && ( CCB->State < DMS_STATE_HIGH_LIMIT ))
      {
      /* Jump to enter the state function.
      */
      event_pending = ( *Ccb_State_Func_Dms[ CCB->State ] )( event );
      result_code = ( event_pending ) ? EVENT_PENDING : NO_ERROR;
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
/********************
* CCBDMS_Disconnect *
*********************
*
* Standard actions for hangup by the voice phone.  This function initiates
* sending of CC_DISCONNECT_REQ.  It is used whenever the voice phone goes
* on hook while engaged.
*
*/
PRIVATE void near CCBDMS_Disconnect( cause )

int cause;                      /* Cause for cause information element */
{
   BYTE diag[1];

#ifdef TASK_KB
   CCBRI_StopConnect( CCB->B_Chan );
#endif
   if ( CCBRI_GetOutPkt( CC_DISCONNECT_REQ, FALSE ))
      {
      /* Build mandatory Cause element.
      */
      diag[ 0 ] = 0;
      OutPkt->InfoLen += CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                             cause, FALSE, diag );
      CCBRI_MailtoL3();
      }
   return;
}
#endif		/* === SBC_INTERFACE */

/***********************
* CCBDMS_DisconnectInd *
************************
*
* Generic action upon receiving the CC_DISCONNECT_IND message
* for a network disconnect.
*
*/
PRIVATE BOOLEAN near CCBDMS_DisconnectInd()

{
   BYTE cause = 0;
#ifdef COMMAND_INTERFACE
   BYTE event = 0;
#endif

#ifdef SBC_INTERFACE
#ifdef TASK_KB
   CCBRI_StopConnect( CCB->B_Chan );
#endif
   /* Far end disconnect.  Send CC_RELEASE_REQ. We will retain connection
   *  to the B-Channel.
   */
   /* Build complaining cause info element (optional in this message) 
   */
   cause = (( CCBRI_AcceptCause (&cause) == MANDATORY_IE_MISSING ) ?
                 MANDATORY_IE_MISSING : cause);

   if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) )
      {
      CCBRI_MailtoL3();
      CCB->State = DMS_OFFHOOK_RELEASING;
      }
#endif
#ifdef COMMAND_INTERFACE
/* === replaced the following code 
 * LIF_UpdateTimer( CCB->CCDMSTimer, CC_ID, CC_ID, DISC_HOST_EVENT, 0, 
 *                  CCB->Sapi, CCB->Ces, CCB->Call_ID, 0, DISC_HOST_TIMEOUT);
 * LIF_StartTimer (CCB->CCDMSTimer);
 * CCB->State  = DMS_HOST_DISCONNECTING;
 */
   if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) )
	  {
	  CCBRI_MailtoL3();
	  CCB->State = DMS_ONHOOK_RELEASING;
	  }
/* === end of replaced code */	
   CCB->Cause  = (int ) cause;
   event       = HOST_DISCONNECT;
   CCBRI_DisconnectHostCall( &event , cause );
#endif

   return( TRUE );
}


/*********************
* CCBDMS_ReleaseCall *
**********************
*
* Terminate a call and put the CCB to bed.
*
*/
PUBLIC void CCBDMS_ReleaseCall()

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

/**********************
* CCBDMS_ReleasedLink *
***********************
*
* The data link is down at Layer 2.
* Remove CCBs from any context but LOCKOUT.  Users left OFF HOOK
* must retain a CCB in the LOCKOUT state to process their hangup.
*
*/
PUBLIC void CCBDMS_ReleasedLink (ces, dsl_id)
BYTE ces;          /* Connection Endpoint Suffix of the released link. */
BYTE dsl_id;                                /* Digital subscriber loop */
{
   CCB_ID tmp_ccb;

   /*
    * Start at the head and remove all impacted CCBs.
    */
   CCB = CCB_Head[dsl_id];
   /*
    * a ces == 0 (BROADCAST) means the entire DSL has been deactivated
    */
   while (CCB) {
      tmp_ccb = CCB->Next;
      if ((CCB->Ces == ces) || (ces == BROADCAST_CES)) {
         /*
          * notify the HOST
          */
         if (CCB->B_Chan)
            CCBRI_ReleaseChan();
         CCBRI_RejectHostCall(TEMPORARY_FAILURE);
         CCBDMS_ReleaseCall();
      }
      CCB = tmp_ccb;   /* Skip to the next one. */
   }
}

/*****************
* CCBDMS_Restart *
******************
*
* B-Channel restart indication received.  It is assumed that Layer 3
* Protocol Control (L3_Restart) has already analyzed the message for
* mandatory information element presence and coding correctness.  Therefore
* we will proceed to restart channels without further checking based upon
* the criteria L3 Protocol Control used to restart at that level.
*
*/
PUBLIC void CCBDMS_Restart( dsl_id )

BYTE dsl_id;                             /* Digital subscriber loop ID */

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
         CCBDMS_RestartChan( b_chan, dsl_id );
#ifdef DEBUG
      printf ( "CCBDMS_Restart...ALL channels restarted.\n" );
#endif
#ifdef SBC_INTERFACE
      for ( i = 0; i < MAX_VOICE_CALLS; i++ )
         {
         if ( Hcb[ dsl_id ].Ccb_In_Use[ i ] )
            {
            CCB = Hcb[ dsl_id ].Ccb_In_Use[ i ];
            if (( CCB->State != DMS_LOCKOUT ) ||
                ( CCB->HcbIndex != Hcb[ dsl_id ].Active_Call ))
               CCBDMS_ReleaseCall();
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
         printf ( "CCBDMS_Restart...channel %d restarted.\n", b_chan );
#endif
         CCBDMS_RestartChan( b_chan, dsl_id );    /* Restart a channel */
         }
      }
}


/*********************
* CCBDMS_RestartChan *
**********************
*
* Restart an individual B-Channel.
*
*/
PRIVATE void near CCBDMS_RestartChan( b_chan, dsl_id )

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
         CCBDMS_ReleaseCall();                   /* Terminate the call */
         if ( hw_state == OFFHOOK )
            {
            /* We reinstall the call into a new CCB so it may hangup.
            */
            Hcb[ dsl_id ].Hw_State = OFFHOOK;      /* Restore hw state */
            if (( index = CCBRI_GetHcbIndex( dsl_id )) < MAX_VOICE_CALLS )
               {
               if ( CCBRI_GetNewCCB( DMS_IDLE,
                                     NO_CHANNEL,
                                     SIGNALING_SAPI,
                                     SIGNALING_CES,
                                     index,
                                     CCBRI_GetNewCall_Id()))
                  {
                  Hcb[ dsl_id ].Ccb_In_Use[ index ] = CCB;
                  Hcb[ dsl_id ].Active_Call = index;
                  CCB->State = DMS_LOCKOUT;
                  }
               }
            }
         }
      }
   CC_CHAN_ReleaseChan( b_chan, B_CHANNEL_UNITS, Dsl_ID );
#endif                                                /* SBC_INTERFACE */
#ifdef COMMAND_INTERFACE
   /* Check if the channel being restarted is in use.
   */
   CCB = CCB_Head[ dsl_id ];
   while ( CCB )
      {
      if ( CCB->B_Chan == b_chan )
         {
         if ( CCB->State != DMS_IDLE )
            {
            CCBRI_ReleaseChan();
#ifdef COMMAND_INTERFACE
            CCBRI_RejectHostCall( TEMPORARY_FAILURE );
            CCBRI_RemoveCCB( CCB );
#endif
#ifdef SBC_INTERFACE
            CCB->State = DMS_LOCKOUT;
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


/******************
* CCBDMS_SetupInd *
*******************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function
* because of its inherent complexity.
*
*/
PRIVATE void near CCBDMS_SetupInd ()
{
   int     cause;
   int     length;
   BYTE    ring_type;
   BYTE    chan_id;
   BYTE    exclusive;
   int     k;
   int     ces;
   int     state;

   /*
    * A new call SETUP from the far end. The CC may send
    * ALERTING request to the network instructing it that
    * the phone is ringing.  This will be done only after
    * unpacking and checking of info elements in the CC_SETUP_IND
    * message.
    *
    * Before sending of ALERTING message, the CC must always mail
    * CC_BROADCAST_RESP message.  This will assure that the Layer 3
    * software is in a proper state to receive ALERTING.
    */
   k = 0;
   ces = 0;

   /*
    * The first thing checked is the EID. If the SETUP is incorrectly 
    * addressed ignore it.
    */
   if ((CCBDMS_CheckEID(&ces)) == FALSE) {
       /*
        * Notify users of this since this is a change in functionality
        * to conform with specification.
        */
       errmsg(&msgsym(INVALID_EID, ISDN), DSL_TO_NAME(Dsl_ID));
       if (isdn_debug_ok(q931_debug, Dsl_ID) || isdn_debug)
           buginf("\nISDN %s: Ignoring incoming call, EndPoint ID mismatch",
                  DSL_TO_NAME(Dsl_ID));
       CCBDMS_ReleaseCall();
       goto bailout;
    } else {
       if (ces) 
          CCB->Ces = ces; 
    }

   /* inform the HOST.
   */
   if ( CCBRI_GetOutInfo( HOST_INCOMING_CALL ) )
      {
      OutPkgInfoLen++;
      k = 5;                      /* bearer capability must be present */
      }

   /* Look at the bearer capability IE.
   */
   if (( cause = CCBRI_AcceptBearerCap()) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected.
      */
      CCBRI_RejectSetup( cause, BEARER_CAPABILITY_IE );
      CCBDMS_ReleaseCall();
#ifdef COMMAND_INTERFACE
      CCBRI_FreeCallBufs();
#endif
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
      CCBDMS_ReleaseCall();
#ifdef COMMAND_INTERFACE
      CCBRI_FreeCallBufs();
#endif
      goto bailout;
      }
   else
      {
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

   /*
    * If provided, we attempt to match numbers. Note: This is where we have
    * to determine which ces value to use (1 or 2). The DMS uses 2 TEIs and
    * 2 ces values as well. If number not present, release the call.
    */
   cause = CCBRI_AcceptCalledNumb(&length, Dsl_ID); 
   if ((cause == INVALID_ELEM_CONTENTS) || (cause == MANDATORY_IE_MISSING)) {
      /*
       * The element was present, but was coded incorrectly or the
       * called number did not match ours - reject the setup. Also, notify
       * the user since this changes the implementation to conform to the
       * spec.
       */
      errmsg(&msgsym(INVALID_CALLEDNUMBER, ISDN), DSL_TO_NAME(Dsl_ID));
      if (isdn_debug_ok(q931_debug, Dsl_ID) || isdn_debug) {
          buginf("\nISDN %s: Rejecting the call, no Called Party number",
                 DSL_TO_NAME(Dsl_ID));
      }
      CCBRI_RejectSetup(cause, CALLED_NUMBER_IE);
      CCBDMS_ReleaseCall();
      CCBRI_FreeCallBufs();
      goto bailout;
   } else {
      if (cause == NOERR)
         k+= length;
   }

   /*
    * This is where the ces going to process_rxstate has to be adjusted.
    * The CCB->Ces value is set in CCBRI_AcceptCalledNumber and will 
    * determine which ces (1 or 2) to use. The default value is 1.
    */
   OutPkgInfo[2] = CCB->Ces;

   /* Next receive the ringing pattern.
   */
   if ((cause = CCBRI_AcceptSignal(&ring_type)) == INVALID_ELEM_CONTENTS) {
      CCBRI_RejectSetup(cause, SIGNAL_IE);
      CCBDMS_ReleaseCall();
      CCBRI_FreeCallBufs();
      goto bailout;
   } else {
      if (cause == NOERR) {
         OutPkgInfo[ OutPkgInfoLen++ ] = SIGNAL_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 1;
         OutPkgInfo[ OutPkgInfoLen++ ] = ring_type;
         k += 3;
      }
   }

   state = CCBRI_SetNewChan(chan_id, CHANNEL_BUSY, TRUE);
   if (state != NOERR) {
      CCBRI_RejectSetup(REQ_CHANNEL_NOT_AVAIL, NO_DIAG);
      CCBDMS_ReleaseCall();
      CCBRI_FreeCallBufs();
   } else {
      /* inform the HOST
      */
      if (OutPkgInfo) {
         OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
         CCBRI_MailtoHost( CCB->CopyPublicMsg );
         CCB->State = DMS_CALL_OFFERED;
      }
    }
bailout:
   return;
}

/******************
* CCBDMS_Shutdown *
*******************
*
* Reset Call Control
*
*/
PUBLIC void CCBDMS_Shutdown (dsl)
BYTE dsl;
{
    int	jx;
    CCB_ID temp_ccb;

    Dsl_ID = dsl;
    CCB = CCB_Head[dsl];
    while ( CCB ) {
        temp_ccb = CCB->Next;
        CCBDMS_ReleaseCall();
        CCB = temp_ccb;
    }

    for (jx=0; jx < MAX_HOSTS; jx++)
        LIF_StopTimer (Host_CB[dsl][ jx ].Timer_T415 );
}

#endif	/* === */
