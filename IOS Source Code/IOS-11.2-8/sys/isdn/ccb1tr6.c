/* $Id: ccb1tr6.c,v 3.3.12.3 1996/09/13 04:54:30 thille Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccb1tr6.c,v $
 *------------------------------------------------------------------
 * ccb1tr6.c  ISDN 1TR6 BRI Specific Call Control Functions
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccb1tr6.c,v $
 * Revision 3.3.12.3  1996/09/13  04:54:30  thille
 * CSCdi68927:  Clean up some things that should be const in ISDN
 * Branch: California_branch
 * Make some more stuff const to save ram.  Also make some more stuff
 * static.
 *
 * Revision 3.3.12.2  1996/04/30  23:06:40  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3.12.1  1996/04/12  01:55:09  bdas
 * CSCdi45085:  System restarted by error- Illegal instruction.
 * Remove a routine that was poorly written and causing the router
 * to crash.  This routine is not required for the 1tr6 compliance,
 * and when we received an unexpected message from the switch this
 * routine could cause the router to crash.
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/15  17:27:09  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.2  1995/11/17  17:39:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccb1tr6.c
VERSION: 3.21
DESCRIPTION: ISDN 1TR6 BRI Specific Call Control Functions

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


This version of this module is intended to implement the
Call Control functions for a simple user-side Basic Rate Interface
(Basic Rate Access) telephone in a manner compatible with  
the specfications published by Deutsch Bundespost:

                      FTZ - Richtliniensammlung 
             Technische Forderungen an digitale Endgerate
                      mit So - Schnittstelle
                               1Tr6 
                            Ausgabe 1.90

This version does not support any supplementary services (Dienstmerkmale)
as described in Band 4, Teil 6 of 1TR6.  However a "Registration Maintenance"
state has been created that corresponds to states T20 and T21 in the User-side
SDLs of 1TR6. 

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


Various simplifications have been made which reflect the fact that
this is not the software designed to run, as is, a real hardware
device.  For example, the directory number is simply fixed at compile
time, where a real product might be expected to have NV ram for such
volatile information, with a user-programming capability.


The Call Control is assumed to reside on a point-to-multipoint
interface wherein SETUP is broadcast to the TEs.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.


*************************************************************************
*************************************************************************
*/

#define     CCB1TR6_C                                    /* module name */

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
#ifdef BRI_1TR6            /* === */
#include    "tune.h"
#include    "gendef.h"
#include    "lif.h"
#include    "ccie.h"
#include	"gp.h"		/*===*/
#include    "ccbri.h"
#include    "ccb1tr6.h" 
#include    "cc_chan.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* Definitions for CCB Call States
*/
enum CCB_State
   {
   TR6_STATE_LOW_LIMIT,                              /* Sizing element */
   TR6_IDLE,                                               /* CCB idle */
   TR6_RINGING,                                       /* Being alerted */
   TR6_CONNECTING,                                     /* Connect Sent */
   TR6_CALL_ACTIVE,                                    /* Conversation */
   TR6_DIALING,                                             /* Dialing */
   TR6_LOCKOUT,                           /* Disconnected but off hook */
   TR6_OFFHOOK_RELEASING,  /* Waiting for release confirmation on hook */
   TR6_ONHOOK_RELEASING,  /* Waiting for release confirmation off hook */
   TR6_ORIGINATE,                    /* Await response to CC_SETUP_REQ */
   TR6_ROUTING,     /* Engaged but neither dialing nor in conversation */
   TR6_CALL_OFFERED,                           /* Call offered to Host */
/*
   TR6_DISCONNECTING_HOST,	HOST DISCONNECT sent to Host 		=== */
   TR6_CALL_PASSIVE,                             /* Call Passive State */
   TR6_REG_MAINT,                /* Facility Register, Facility Cancel */ 
   TR6_STATE_HIGH_LIMIT                              /* Sizing element */
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
PRIVATE    BOOLEAN  near    CCB1TR6_CallActive          (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallAlerting        (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallConnecting      (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallDialing         (int *);
/* ===
PRIVATE    BOOLEAN  near    CCB1TR6_CallDiscHost        (int *); */
PRIVATE    BOOLEAN  near    CCB1TR6_CallIdle            (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallLockout         (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallOffHookRelease  (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallOnHookRelease   (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallOriginate       (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallPassive         (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallRouting         (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallOffered         (int *);
PRIVATE    BOOLEAN  near    CCB1TR6_CallRegMaint        (int *); 
PRIVATE    int      near    CCB1TR6_ConnectHost         (void);
#ifdef SBC_INTERFACE
PRIVATE    void     near    CCB1TR6_Disconnect          (int);
#endif 
PRIVATE    BOOLEAN  near    CCB1TR6_DisconnectInd       (void);
PRIVATE    void     near    CCB1TR6_DisconnectAckHost   (void);
PRIVATE    void     near    CCB1TR6_DisconnectHostCall  (BYTE *, int); 
/* ===
PRIVATE             near    CCB1TR6_FacilityRequest     (int);
=== */PRIVATE     void    near    CCB1TR6_FacilityRequest     (int);
PRIVATE    int      near    CCB1TR6_GetFacilities       (int *, BOOLEAN);
PRIVATE    int      near    CCB1TR6_GetCause            (int *);
#ifdef 0
PRIVATE    int      near    CCB1TR6_GetCharges          (int *);
#endif
PRIVATE    int      near    CCB1TR6_GetDate             (int *);
PRIVATE    int      near    CCB1TR6_HostCallRequest     (void);
PRIVATE    int      near    CCB1TR6_HostRegisterRequest (int);
PRIVATE    BOOLEAN  near    CCB1TR6_InfoReceived        (void);
PRIVATE    void     near    CCB1TR6_SetupInd            (void);

/************************************************************************
*             S T A T E   E N T R Y   J U M P   T A B L E               *
*************************************************************************
*/

typedef BOOLEAN (near *CC_STATES)( int * );

/* NOTE: this table MUST strictly correspond to the CCB_State table above !!
*/
static const CC_STATES Ccb_State_Func_1TR6[ TR6_STATE_HIGH_LIMIT ] =
   {
   NULL,
   CCB1TR6_CallIdle,
   CCB1TR6_CallAlerting,
   CCB1TR6_CallConnecting, 
   CCB1TR6_CallActive,
   CCB1TR6_CallDialing,
   CCB1TR6_CallLockout,
   CCB1TR6_CallOffHookRelease,
   CCB1TR6_CallOnHookRelease,
   CCB1TR6_CallOriginate,
   CCB1TR6_CallRouting,
   CCB1TR6_CallOffered,
/*   CCB1TR6_CallDiscHost, === */
   CCB1TR6_CallPassive, 
   CCB1TR6_CallRegMaint  
   };




/************************************************************************
*                         F U N C T I O N S                             *
*************************************************************************
*/

/*************************
* CCB1TR6_CallActive     *
**************************
*
* Process the current event in the context of a voice call in the
* CONNECTED state. Corresponds to state T10 of 1TR6
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallActive( event )

int *event;                                /* Event input to the state */

{
   int     k = 0;
   int     length = 0;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_FACILITY:
         /* process the Facility message from the HOST.
          */
          CCB1TR6_FacilityRequest( *event );
          break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_OFFHOOK_RELEASING;
         break;

      case HOST_INFORMATION:
         /* process the information from HOST.
         */
         CCBRI_HostInfo();
         /* no change in state.
         */
         break;

      case HOST_FACILITY_ACK:
         /* process the Facility Ack from the HOST.
         */
         /* no change in state.
         */
         CCB1TR6_FacilityRequest( *event );
         break;

      case HOST_FACILITY_REJ: 
         /* no change in state.
         */
         /* process the Facility Rej from the HOST.
         */
         CCB1TR6_FacilityRequest( *event );
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
         /* Local disconnect (response to this message will be
         *  received in RELEASING state - see CC_RELEASE_IND):
         */
         CCB1TR6_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = TR6_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;
#endif
      case CC_DISCONNECT_IND:                    /* Far end disconnect */
         CCB1TR6_DisconnectInd(); 
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
		/* CCBRI_ReleaseChan();    Disc and release chan if allocated ===*/
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
         CCB->State = TR6_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         CCB1TR6_DisconnectAckHost();
		/* CCBRI_RemoveCCB( CCB );		=== */
		CCBRI_ReleaseCall();
#endif
          
         break;

      case CC_FACILITY_REJ:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_REJ ) )
            {
			OutPkgInfoLen++;	/* === */
            CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break;

      case CC_FACILITY_ACK:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_ACK ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length,  FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break; 

      case CC_FACILITY_IND:
         if ( CCBRI_GetOutInfo( HOST_FACILITY ) )
            {
			OutPkgInfoLen++;    /* === */
            CCB1TR6_GetFacilities( &length,  TRUE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         break;

      case CC_STATUS_IND:
         if ( CCBRI_GetOutInfo( HOST_STATUS ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetCause ( &length );
            k += length;
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            } 
         break;

      case CC_INFO_IND:
         CCB1TR6_InfoReceived();
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); ===*/
            CCBRI_RejectMsg( 0,0); /* === for 1tr6 certif */
            CCB->State = TR6_OFFHOOK_RELEASING;
            }
#ifdef COMMAND_INTERFACE
         else if ( Source == HOST_ID ) 
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_CONNECTED, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/***********************
* CCB1TR6_CallAlerting *
************************
*
* Process the current event in the context of a voice call in the
* RINGING state.  This terminal is being rung.	This corresponds to
* state T7 of 1TR6 
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallAlerting( event )

int *event;                                /* Event input to the state */

{
   int k = 0;
   int length = 0; 

   switch ( *event )
      {

#ifdef COMMAND_INTERFACE

      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = TR6_CONNECTING;
         break;

      case HOST_DISCONNECT:
         /* process the disconnect message from the HOST.
          */
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_ONHOOK_RELEASING;
         break;

      case HOST_FACILITY:
         /* process the Facility message from the HOST.
          */
          CCB1TR6_FacilityRequest( *event );
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
         CCB->State = TR6_CONNECTING;
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
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;        /* Not BELL */
         CCB->State = TR6_ONHOOK_RELEASING;
#endif
         CCB1TR6_DisconnectInd(); 
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
#ifdef COMMAND_INTERFACE
         CCB1TR6_DisconnectAckHost();
#endif
		/* CCB1TR6_ReleaseCall();	=== */
		CCBRI_ReleaseCall();
         break;

      case CC_FACILITY_ACK:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_ACK ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break; 

      case CC_FACILITY_REJ:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_REJ ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break;

      case CC_INFO_IND:
         CCB1TR6_InfoReceived();
         break;

      case CC_STATUS_IND:
         if ( CCBRI_GetOutInfo( HOST_STATUS ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetCause ( &k );
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            } 
         break;

      case CC_SETUP_IND:
         /* Network may resend SETUP to alter Channel.
         */
         CCB1TR6_SetupInd();
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0, 0 ); /* === for 1tr6 certif */
            CCB->State = TR6_ONHOOK_RELEASING;
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


/*********************
* CCB1TR6_Connecting *
**********************
*
* Process the current event in the context of a call in the
* CONNECTING state.  CONNECT has been sent to the Network.  This 
* corresponds to State T8 of 1TR6  
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallConnecting( event )

int *event;                                /* Event input to the state */

{
   int     k = 0;
   int     cause; 
   int     length; 
   PKT_ID  loc_pkt;                  /* Used to test for T313 Timeouts */

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         /* process the disconnect message from the HOST.
          */
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_OFFHOOK_RELEASING;
         break;

      case HOST_FACILITY:
         /* process the Facility message from the HOST.
         */
          CCB1TR6_FacilityRequest( *event );
          break;

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
         *  Note that a T313 timeout must be handled here, that is,
         *  L3 sends T313 Timeouts to us as CC_SETUP_COMP_IND.
         *  So we look at the first byte of the info buffer to see if
         *  it is really a CONNECT_ACK )                         
         */

          loc_pkt = InPkt;
          if ( loc_pkt->Info[0] == CONNECT_ACK )
             {
#ifdef SBC_INTERFACE
#ifdef TASK_KB
             if ( CCB->B_Chan )
                CCBRI_StartConnect( CCB->B_Chan );
#endif
             CCB->State = TR6_CALL_ACTIVE; 
             CCBRI_TRACE( TR_CONVERSATION );
#endif 

#ifdef COMMAND_INTERFACE
             /* inform the HOST.
             */
             if ( ( CCB->B_Chan ) &&  
                 ( CC_CHAN_GetStatebri( CCB->B_Chan,
                                     B_CHANNEL_UNITS,
                                     Dsl_ID ) == CHANNEL_PROPOSED_1TR6 ) )
                 cause = CCB1TR6_ConnectHost(); 
             else
                cause = CALL_REJECTED_1TR6;

             if ( cause != NOERR )
                {
                /* CCBRI_RejectCall ( cause, 0 );  === for 1tr6 certif */
                CCBRI_RejectCall ( CALL_REJECTED_1TR6, 0 );  /* === */
                }
             else
                {
                CCB->State = TR6_CALL_ACTIVE; 
                }
#endif
             }
          else                 /* Not a CONNECT_ACK, must be a Timeout */
             {
             CCB->State = TR6_OFFHOOK_RELEASING;
             }
         break;

      case CC_FACILITY_REJ:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_REJ ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break;

      case CC_FACILITY_ACK:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_ACK ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break; 

      case CC_DISCONNECT_IND:                    /* Far end disconnect */
         CCB1TR6_DisconnectInd(); 
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
		/* CCBRI_ReleaseChan();    Disc and release chan if allocated ===*/
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated ===*/
         CCB->State = TR6_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         CCB1TR6_DisconnectAckHost();
		/* CCBRI_RemoveCCB( CCB );	=== */
		CCBRI_ReleaseCall();
#endif
         break;

      case CC_INFO_IND:
         CCB1TR6_InfoReceived();
         break;

      case CC_STATUS_IND:
         if ( CCBRI_GetOutInfo( HOST_STATUS ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetCause ( &k );
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            } 
         break;

      case CC_SETUP_IND:
         /* Network may resend SETUP to alter Channel.
         */
         CCB1TR6_SetupInd();
         break; 

      case CC_TIMEOUT_IND:
         /* 1 TR6 CONNECT has timeout out.  L3 has sent DISC 
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 );
            CCB->State = TR6_OFFHOOK_RELEASING;
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
* CCB1TR6_CallDialing *
***********************
*
* Process the current event in the context of a voice call in the
* DIALING state. Process overlap sending.  This is state T2 of 1TR6
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallDialing( event )

int *event;                                /* Event input to the state */

{
   int  cause;
#ifdef SBC_INTERFACE
   BYTE digit_as_byte;
#endif 
   BYTE chan_id;
   BYTE exclusive;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_OFFHOOK_RELEASING;
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
         CCB1TR6_Disconnect( NORMAL_CLEARING );
         CCB->State = TR6_ONHOOK_RELEASING;
         CCBRI_TRACE( TR_RELEASING );
         break;
#endif

      case CC_INFO_IND:
         CCB1TR6_InfoReceived();
         break;

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
         /* Status Geruf Tln is Mandatory, but we'll let it go thru
          * anyway.
         */
            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                STATUS_GERUF_TLN_IE, CODESET_6, 1 );
            if (IE)                                    
               {
              /* total length of private IE's = 3
              */
               OutPkgInfo[ OutPkgInfoLen++ ] = 3;
               OutPkgInfo[ OutPkgInfoLen++ ] = STATUS_GERUF_TLN_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;
               OutPkgInfo[ OutPkgInfoLen++ ] = IE[2];
               }
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         CCB->State = TR6_ROUTING;
         break;

      case CC_CALL_PROC_IND:

           cause = (CCBRI_AcceptChanId( &chan_id, &exclusive )); 
           switch ( cause )  
              {

           /* The B Channel will have been received on the SETUP_ACK
            * however it will be resent in CALL_SENT (131217 & 1312416 TR20)
            * It appears that B Channel is mandatory on CALL SENT if we're
            * in this state, and the two test cases seem to confirm this. 
           */
              case MANDATORY_IE_MISSING:
#ifdef COMMAND_INTERFACE
                  CCB1TR6_DisconnectAckHost(); 
#endif
                  CCBRI_RejectCall(0,0); /* === */

				  /* CCB1TR6_ReleaseCall();	=== */
				  CCBRI_ReleaseCall();
                  break; 

              case NOERR: 
                 if ( CCB->B_Chan != chan_id )
                    {
                    CCBRI_RejectCall( 0, 0  );
                    CCB->State = TR6_OFFHOOK_RELEASING;
                    } 
                 else 
                    {
#ifdef TASK_KB
                    CCBRI_StopDigitDetect();
                   /* The B Channel should be connected now, but just to be
                   *  sure
                   */
                    if ( CCB->Chan )
                        CCBRI_StartConnect (CCB->B_Chan);
#endif
                    CCBRI_TRACE( TR_ROUTING );
#ifdef COMMAND_INTERFACE
                     /* inform the HOST.
                     */
                    if ( CCBRI_GetOutInfo( HOST_CALL_SENT ) )
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
                    CCB->State = TR6_ROUTING;
                    }     
                 break; 
              default:        /* Error other than MANDATORY_IE_MISSING */
#ifdef COMMAND_INTERFACE
                 /* CCB1TR6_DisconnectAckHost();*/
#endif 
                 CCBRI_RejectCall( 0, 0  );		/* === */
                 CCB->State = TR6_OFFHOOK_RELEASING;	 /* ===*/

				/* CCB1TR6_ReleaseCall();	=== */
				/* CCBRI_ReleaseCall();		=== */
                 break;
              }                                       /* End of switch */
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
         CCB1TR6_DisconnectInd(); 
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
		/* CCBRI_ReleaseChan();    Disc and release chan if allocated ===*/
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated ===*/
         CCB->State = TR6_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         CCB1TR6_DisconnectAckHost();
		/* CCB1TR6_ReleaseCall();	=== */
		CCBRI_ReleaseCall();
#endif
         break;

      case CC_SETUP_CONF:                    /* Translation of CONNECT */
      case CC_SETUP_COMP_IND:            /* Translation of CONNECT_ACK */
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
         cause = CCB1TR6_ConnectHost();
         if (cause != NOERR)
            {
            CCBRI_RejectCall( 0 , 0 );
			/* CCB1TR6_ReleaseCall();	=== */
			CCBRI_ReleaseCall();
            }
         else 
#endif
         CCB->State = TR6_CALL_ACTIVE;
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
            CCB->State = TR6_OFFHOOK_RELEASING;
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
/* removed this routine === */
#ifdef 0
/****************************
* CCB1TR6_CallDiscHost *
*****************************
*
* CCB1TR6_CallDiscHost.  This State corresponds to Q.931 State 12   
* A Disconnect Indication has been received and HOST_DISCONNECT has been
* mailed to the Host 
*/
PRIVATE BOOLEAN near CCB1TR6_CallDiscHost( event )

int *event;                                /* Event input to the state */

{
  BYTE event_b;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT_ACK:
         LIF_StopTimer ( CCB->CC1TR6Timer );
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ) )
           {
           /* Build complaining Cause info element (optional in this message)
           */
           OutPkt->InfoLen +=
           CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                           CCB->Cause, FALSE, 0 );
           CCBRI_MailtoL3();
           CCB->State = TR6_ONHOOK_RELEASING;
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
           CCB1TR6_DisconnectHostCall(&event_b, CCB->Cause);  
           CCB->State = TR6_ONHOOK_RELEASING;
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
          LIF_StopTimer ( CCB->CC1TR6Timer );
          event_b = HOST_DISCONNECT_ACK;
          CCB1TR6_DisconnectHostCall(&event_b, CCB->Cause);  
          CCB1TR6_ReleaseCall ();
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
            CCB->State = TR6_OFFHOOK_RELEASING;
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
/* removed above routine */

/*******************
* CCB1TR6_CallIdle *
********************
*
* Process the current event in the context of a voice call in the
* IDLE state.
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallIdle( event )

int *event;                                /* Event input to the state */

{
   int cause; 

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_CALL_REQUEST:
         if ( ( cause = CCB1TR6_HostCallRequest() ) != NOERR )
             {
             if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
                {
               /* total length of private IE's = 3.
                */
                OutPkgInfo[ OutPkgInfoLen++ ] = 3;
                OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
                OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                OutPkgInfo[ OutPkgInfoLen++ ] = ( BYTE ) cause;
                CCBRI_MailtoHost( CCB->CopyPublicMsg );
                }
              CCBRI_RemoveCCB( CCB );
             } 
         else 
            {
            CCB->State = TR6_ORIGINATE;
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
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
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

      case HOST_FACILITY_REGISTER:
         if (( cause = CCB1TR6_HostRegisterRequest ( *event)) != NOERR)
            {
            CCBRI_RejectHostMsg( CCB->State );
            CCBRI_RemoveCCB( CCB );
            }
         break; 

      case HOST_FACILITY_CANCEL:
         if (( cause = CCB1TR6_HostRegisterRequest ( *event )) != NOERR)
            {
            CCBRI_RejectHostMsg( CCB->State );
            CCBRI_RemoveCCB( CCB );
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
#ifdef SBC_INTERFACE
      case KP_OFF_HOOK:                               /* Outgoing call */
         /* This is a new outgoing call setup. The CC will send
         *  CC_SETUP_REQ.  If the data link is not established, Layer 3
         *  will detect this and request establishment before the SETUP
         *  is processed.
         */
         Hcb[ DEFAULT_DSL ].Hw_State = OFFHOOK;
#ifdef TASK_KB
         CCBRI_StartTone(DIAL_TONE):
#endif 
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
            CCB->State = TR6_ORIGINATE;
            }
         else
            {
            CCB->State = TR6_LOCKOUT;
            }
         break;
#endif
      case CC_SETUP_IND:
         CCB1TR6_SetupInd();                            /* Incoming call */
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
			/* CCB1TR6_ReleaseCall();	=== */
			CCBRI_ReleaseCall();
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
/**********************
* CCB1TR6_CallLockout *
***********************
*
* Process the current event in the context of a voice call in the
* LOCKOUT (off-hook, but released from the NETWORK) state.  The user
* is "locked-out" from originating again until an on-hook/off-hook
* cycle is completed.  This state is retained after the NETWORK
* has released the call and the user has failed to hang up.
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallLockout( event )

int *event;                                /* Event input to the state */

{
   switch ( *event )
      {
      case KP_ON_HOOK:
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
		/* CCB1TR6_ReleaseCall();   Release from lockout, drop the CCB. ===*/
		 CCBRI_ReleaseCall();
         break;

      default:
         /* Unexpected message.  No message is expected here from Layer 3
         *  since it will have cleared the Call_ID.  If L3 recieves a new
         *  SETUP, it will be processed in CC_DMSB in a new CCB in the
         *  IDLE state.
         */
         if ( Source == L3_ID )
            CCBRI_Error( CCBRI_CALL_LOCKOUT, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}
/**********************
* CCB1TR6_CallOffered *
***********************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to T6 of 1TR6.
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallOffered( event )

int *event;                                /* Event input to the state */

{
   int cause; 

   switch ( *event )
      {
      case HOST_DISCONNECT:
         /* acknowledge the disconnect and free CCB
          * in the 1TR6 world a User may send DISCONNECT when the Host rejects
          * a call or ignore it and free the CCB - we send DISCONNECT 
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CAUSE_IE ) ) != NULL)
               {
               cause = ( int ) IE[2];
               }            
            else
               {
               cause = CALL_REJECTED_1TR6;
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

      case HOST_ALERTING:
         /* Indication that the Host has initiated alerting for the call
         */
        if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
           {
            CCBRI_MailtoL3();                    /* Send CC_BROADCAST_RESP */
            }
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
            {
            CCBRI_MailtoL3(); 
            }
            CCB->State = TR6_RINGING;
         break;

      case HOST_CONNECT:
         if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
            {
             CCBRI_MailtoL3();                    /* Send CC_BROADCAST_RESP */
            }
         CCBRI_HostConnect();
         CCB->State = TR6_CONNECTING;
         break;

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;
      case CC_SETUP_IND:
         /* Network may resend SETUP to alter Channel.
         */
         CCB1TR6_SetupInd();
         break; 
      default:
         /* Unexpected message.  No message is expected here from Layer 3
         *  since it will have cleared the Call_ID.  If L3 recieves a new
         *  SETUP, it will be processed in a new CCB in the	
         *  IDLE state.
         */
         CCBRI_Error( CCBRI_CALL_LOCKOUT, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/*****************************
* CCB1TR6_CallOffHookRelease *
******************************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The local user
* is off-hook. This corresponds to T11 of 1TR6
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallOffHookRelease( event )

int *event;                                /* Event input to the state */

{
   int  k = 0;
   int length = 0; 

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in ONHOOK_RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_ONHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* simply remove the CCB.
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
      case KP_ON_HOOK:
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = TR6_ONHOOK_RELEASING;
         break;
#endif

      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
      case CC_TIMEOUT_IND: 
#ifdef SBC_INTERFACE
         CCB->State = TR6_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         CCB1TR6_DisconnectAckHost();
		/* CCB1TR6_ReleaseCall();	=== */
		CCBRI_ReleaseCall();

#endif
         break;

      case CC_DISCONNECT_IND:
         /* We simply await CC_RELEASE_CONF to follow this message.
         */
         break;

      case CC_FACILITY_ACK:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_ACK ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break; 

      case CC_FACILITY_REJ:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_REJ ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break;

      case CC_INFO_IND:
         CCB1TR6_InfoReceived();
         break;

      case CC_STATUS_IND:
         if ( CCBRI_GetOutInfo( HOST_STATUS ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetCause ( &k );  
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
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
* CCB1TR6_CallOnHookRelease *
*****************************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The local user
* is on-hook.	Corresponds to T19 of 1TR6
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallOnHookRelease( event )

int *event;                                /* Event input to the state */

{
   int  k = 0;
   int  length = 0;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_DISCONNECT_ACK:
         /* simply remove the CCB.
         */
      /*   CCBRI_RemoveCCB( CCB );	=== */
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
      case KP_OFF_HOOK:
         Hcb[ DEFAULT_DSL ].Hw_State = OFFHOOK;
         CCB->State = TR6_OFFHOOK_RELEASING;
         break;
#endif

      case CC_FACILITY_ACK:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_ACK ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break; 

      case CC_FACILITY_REJ:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_REJ ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break;

      case CC_INFO_IND:
         CCB1TR6_InfoReceived();
         break;

      case CC_STATUS_IND:
         if ( CCBRI_GetOutInfo( HOST_STATUS ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetCause ( &k ); 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         break;

      case CC_RELEASE_CONF:
      case CC_RELEASE_IND:
      case CC_TIMEOUT_IND:
#ifdef COMMAND_INTERFACE 
#endif 
		/* CCB1TR6_ReleaseCall();	=== */
		CCBRI_ReleaseCall();
         break;

      case CC_DISCONNECT_IND:
         /* We simply await CC_RELEASE_CONF to follow this message.
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
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
* CCB1TR6_CallOriginate *
*************************
*
* Process the current event in the context of a voice call in the
* ORIGINATE state awaiting the first response to CC_SETUP_REQ.	Corresponds
* to state T1 of 1TR6
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallOriginate( event )

int *event;                                /* Event input to the state */

{
   BYTE    chan_id;
   BYTE    exclusive;
   BOOLEAN struct_err;
   int     k = 0;
   int     length = 0;	/* ===*/

   switch ( *event )
      {

#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_ONHOOK_RELEASING;
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
         /* The caller hangs up.  The current call setup will be
         *  terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         CCB1TR6_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = TR6_ONHOOK_RELEASING;
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
         if (( CCBRI_AcceptChanId( &chan_id, &exclusive )) == NOERR )
            {
            CCBRI_SetNewChan( chan_id, CHANNEL_BUSY, TRUE );
#ifdef TASK_KB
            CCBRI_StartDigitDetect();    /* Condition hdwr for dialing */
            CCBRI_StartConnect( CCB->B_Chan );    /* DIAL TONE in-band */
#endif
            CCB->State = TR6_DIALING;
            CCBRI_TRACE( TR_DIALTONE );
#ifdef COMMAND_INTERFACE
            if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
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
         else
            {
            CCBRI_RejectCall( 0, 0  );
            CCB->State = TR6_OFFHOOK_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
            }
         break;

      case CC_CALL_PROC_IND:
         /* This message indicates that digit transmission (en_bloc) is
         *  complete.  This case is added for completeness.
         */
         if (( CCBRI_AcceptChanId(  &chan_id, &exclusive)) == NOERR )
            {
            /* Connect the B Channel
            */
#ifdef TASK_KB
            CCBRI_StartConnect( CCB->B_Chan );
#endif
            CCB->State = TR6_ROUTING;
            CCBRI_TRACE( TR_ROUTING );
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
               OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;         /* exclusive */
               OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
#endif
            }
         else
            {
            CCBRI_RejectCall( 0, 0 );
            CCB->State = TR6_OFFHOOK_RELEASING;
            CCBRI_TRACE( TR_RELEASING );
            }
         break;

      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated */
#ifdef SBC_INTERFACE
         CCB->State = TR6_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            OutPkgInfoLen++;     /* === */
			CCB1TR6_GetCause ( &length ); 
			k += length;		/* === */
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
			/*  CCBRI_RemoveCCB( CCB );	=== */
			CCBRI_ReleaseCall();	/* === */
            }
#endif
         break;

      case CC_DISCONNECT_IND:
         if ( CCB1TR6_DisconnectInd() )
            {
#ifdef SBC_INTERFACE
            CCB->State = TR6_OFFHOOK_RELEASING;
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
            if ( IE_Fields[ 4 ] == 0 )      /* Test for "No Channel" */
               {
               /* SETUP has timed-out (T303) to NETWORK.  The SETUP failed.
               */
				/* CCBRI_ReleaseChan();  Disc and rls chan if allocated ===*/
#ifdef SBC_INTERFACE
               CCB->State = TR6_LOCKOUT;
#endif
               CCBRI_TRACE( TR_LOCKOUT );
#ifdef COMMAND_INTERFACE
               CCBRI_RejectHostCall (TIMEOUT_1TR6);  
				/* CCBRI_RemoveCCB( CCB );	=== */
				CCB->State = TR6_ONHOOK_RELEASING;	/* === */
#endif
               }
            else
               {
               if ( Source == L3_ID )
                  /* CCBRI_RejectMsg( WRONG_MESSAGE, *event ); === */
                  CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
               CCBRI_Error( CCBRI_CALL_ORIGINATE, UNKNOWN_EVENT, *event, Source, FALSE );
               }
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
            CCB->State = TR6_OFFHOOK_RELEASING;
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
/**********************
* CCB1TR6_CallPassive *
***********************
*
* Process the current event in the context of a voice call in the
* PASSIVE state.  This corresponds to state T14 in 1TR6.  You get
* here by the Host sending DETACH, which is only possible by subscribing
* to the service feature "DienstWechsel".  This has not been developed.
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallPassive( event )

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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
            }
#ifdef COMMAND_INTERFACE
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
#endif
         CCBRI_Error( CCBRI_CALL_PASSIVE, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}

/*************************
* CCB1TR6_CallRegMaint *
**************************
*
* Process the current event in the context of Registration Maintanance.
* This corresponds to States T20 and T21 of 1TR6.
* The states have been consolidated - L3 should reject an inappropriate
* message.
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallRegMaint( event )

int *event;                                /* Event input to the state */

{
   int     k = 0; 
   int     length = 0; 

   switch ( *event )
      {

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_OFFHOOK_RELEASING;
         break;

      case HOST_QUERY:
         /* process the query from the HOST.
         */
         CCBRI_HostQuery();
         /* no change in state.
         */
         break;

      case CC_REGISTER_ACK:

         if ( CCBRI_GetOutInfo( HOST_REGISTER_ACK ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         break; 

      case CC_REGISTER_REJ:
         if ( CCBRI_GetOutInfo( HOST_REGISTER_REJ ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetCause ( &length );
            k += length;
 
            CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 

            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         break; 

      case CC_CANCEL_ACK:
         if  (CCBRI_GetOutInfo( HOST_CANCEL_ACK)) 
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         break; 

      case CC_CANCEL_REJ:
         if ( CCBRI_GetOutInfo( HOST_CANCEL_REJ ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetCause (&length);
            k += length;
            CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
            }
         else if (( Source == HOST_ID ) && ( InPkgInfo ) )
            {
            CCBRI_RejectHostMsg( CCB->State );
            }
         CCBRI_Error( CCBRI_REG_MAINTENANCE, UNKNOWN_EVENT, *event, Source, FALSE );
         break;
      }

   return( FALSE );
}
/**********************
* CCB1TR6_CallRouting *
***********************
*
* Process the current event in the context of a voice call in the
* ROUTING state, the general purpose engaged, but non-CONVERSATION, state.
* This state is typically associated with the presentation of some
* informational tone such as ringback or some other call progress tone
* while the user is standing by. Corresponds to states T3 and T4 of 1TR6
*
*/
PRIVATE BOOLEAN near CCB1TR6_CallRouting( event )

int *event;                                /* Event input to the state */

{
   int  k = 0;
   int  length =0;
   int  cause; 

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE

      case HOST_DISCONNECT:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_OFFHOOK_RELEASING;
         break;

      case HOST_DISCONNECT_ACK:
         /* local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = TR6_ONHOOK_RELEASING;
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
         CCB1TR6_Disconnect( NORMAL_CLEARING );
         Hcb[ DEFAULT_DSL ].Hw_State = ONHOOK_IDLE;
         CCB->State = TR6_ONHOOK_RELEASING;
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
            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen, STATUS_GERUF_TLN_IE,
                             CODESET_0, 1 ) ;
            if (IE) 
               {
               /* total length of private IE's = 3.
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 3; 
               OutPkgInfo[ OutPkgInfoLen++ ] = STATUS_GERUF_TLN_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 01;
               OutPkgInfo[ OutPkgInfoLen++ ] = IE[2];
               }
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         break;

      case CC_INFO_IND:
         CCB1TR6_InfoReceived();
         break;

      case CC_FACILITY_ACK:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_ACK ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break; 

      case CC_FACILITY_REJ:
         if ( CCBRI_GetOutInfo( HOST_FACILITY_REJ ) )
            {
               OutPkgInfoLen++;    /* === */
			   CCB1TR6_GetFacilities( &length, FALSE );
            k+= length; 
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCB->DM_Substate = DM_IDLE;
         break;

      case CC_SETUP_CONF:                    /* Translation of CONNECT */
      case CC_SETUP_COMP_IND:            /* Translation of CONNECT_ACK */
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
         if ((cause = CCB1TR6_ConnectHost()) != NOERR) 
            {
            CCB1TR6_DisconnectAckHost();
            /*CCBRI_RejectCall (cause,0); === for 1tr6 certif */
            CCBRI_RejectCall (0,0); /* === */
			/* CCB1TR6_ReleaseCall();	=== */
			CCBRI_ReleaseCall();
            }
         else {
			/* we have to send Connect_Ack now, === */
		 	if (CCBRI_GetOutPkt (CC_CONNECT_ACK_REQ, FALSE)) {
				CCBRI_MailtoL3 ();
			}
			/* CCB->State = TR6_CALL_ACTIVE;	don't change state here === */
		}
#else	/*===*/
            CCB->State = TR6_CALL_ACTIVE;
#endif	/*===*/
         break;

      case CC_TIMEOUT_IND:
      case CC_RELEASE_IND:
      case CC_RELEASE_CONF:
		/* CCBRI_ReleaseChan();    Disc and release chan if allocated ===*/
#ifdef SBC_INTERFACE
         CCBRI_ReleaseChan();    /* Disc and release chan if allocated ===*/
         CCB->State = TR6_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#endif
#ifdef COMMAND_INTERFACE
         /* inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            OutPkgInfoLen++;    /* === */
			CCB1TR6_GetCause ( &length );
            k += length;
            OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
			/* CCB1TR6_ReleaseCall();	=== */
			CCBRI_ReleaseCall();
            }
#endif
         break;

      case CC_DISCONNECT_IND:
         CCB1TR6_DisconnectInd();
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
            /* CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] ); === */
            CCBRI_RejectMsg( 0,0 ); /* === for 1tr6 certif */
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
/* ===
bailout:
=== */
   return( FALSE );
}

/********************
CCB1TR6_ConnectHost *
*********************
*
* The network has told us we are connected: connect the Host.
* But first we do some basic checks on the IEs returned from the
* Network.
*
*/
PRIVATE int near CCB1TR6_ConnectHost (void)
{
   int     cause; 

/*
 * unused variables, due to following ifdef
 */
#ifdef 0 
   int     k;
   BYTE    addition_info;
   BYTE    service_description;
   int     length = 0;
#endif

   cause = NOERR; 
   if ( CCBRI_GetOutInfo( HOST_CONNECT ) )
      {
/*
 * This section of code is causing problems. When a call comes in on
 * B2, the code incorrectly thinks it is coming in on B1. This occurs
 * in isdnintrfc.c, but the message is built here. ===
 */
#ifdef 0
      OutPkgInfoLen++;

      if  (((  cause = CCBRI_AcceptServiceInd ( &service_description,
                &addition_info, FALSE ))  != INVALID_ELEM_CONTENTS)  &&  (IE)) 
         {
          /* build the CCITT compatible info element.
           */
         OutPkgInfo[ OutPkgInfoLen++ ] = SERVICE_INDICATOR_IE; 
         OutPkgInfo[ OutPkgInfoLen++ ] = 2; 
         OutPkgInfo[ OutPkgInfoLen++ ] = service_description; 
         OutPkgInfo[ OutPkgInfoLen++ ] = addition_info; 
         k+= 4;
         }
       else
          cause = NOERR;                        /* We will let it go * */

      if ((( cause = CCB1TR6_GetFacilities( &length,  TRUE )) == NOERR ) && ( IE ) )
         {
         k+= length; 
         }
       else
          cause = NOERR;                        /* We will let it go * */

      if ( (( cause = CCB1TR6_GetCharges( &length )) == NOERR ) && ( IE ) )
         {
         k+= length; 
         }
      else
         cause = NOERR;                           /* We will let it go */

       /* Date is Mandatory on Connect and Connect_Acks From Network
       *  However 1TR 20 seems to ignore the absence of many such  
       *  mandatory IEs -here we'll check if its present and do a basic
       *  syntax check on the formatted date.
       */

      if ( (( cause = CCB1TR6_GetDate( &length )) == NOERR ) && (IE) )
         {
         k += length; 
         }

/*
 * ===, this is a mandatory IE and we cannot let it go by
 *
 *    else 
 *       cause = NOERR;                          * We will let it go *
 */
		/* have to buildup the channel ie in the message, ===*/
		if (cause == NOERR) {					/*===*/
#endif
        /*
         * set a length of 4 for the isdnintrfc.c routine, then
         * it will be consistent with the rest of the ccb modules 
         * ===
         */
        OutPkgInfo [ OutPkgInfoLen++ ] = 4;
        OutPkgInfo [ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
        OutPkgInfo [ OutPkgInfoLen++ ] = 2;
        OutPkgInfo [ OutPkgInfoLen++ ] = 0x01;          /*exclusive*/
        OutPkgInfo [ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;

        /*OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;    k is length of Private IEs  */
        CCBRI_MailtoHost( CCB->CopyPublicMsg );
     /* } ===*/
      }

/* ===
bailout: 
=== */
 return cause; 
}

/**************************
* CCB1TR6_DecodeCallState *
***************************
*
* Decode the state of the CCB from the CCB state variable.
* Dispatch the appropriate state function through the state function
* jump table.
*
*/
PUBLIC int  CCB1TR6_DecodeCallState( event )

BOOLEAN *event;                            /* Event input to the state */

{
   BOOLEAN event_pending;
   int     result_code;

   /* Perform a range check on the value of CCB->State to prevent
   *  the strange and unusual.
   */
   if (( CCB->State > TR6_STATE_LOW_LIMIT ) && ( CCB->State < TR6_STATE_HIGH_LIMIT ))
      {
      /* Jump to enter the state function.
      */
      event_pending = ( *Ccb_State_Func_1TR6[ CCB->State ] )( event );
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
* CCB1TR6_Disconnect*
*********************
*
* Standard actions for hangup by the voice phone.  This function initiates
* sending of CC_DISCONNECT_REQ.  It is used whenever the voice phone goes
* on hook while engaged.
*
*/
PRIVATE void near CCB1TR6_Disconnect( cause )

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
/****************************
* CCB1TR6_DisconnectAckHost *
*****************************
*
* Send a Host a Disconnect Ack.  In this case a RELEASE has been
* received from the network.
*
*/
PRIVATE  void  near  CCB1TR6_DisconnectAckHost () 
{
   int k = 0; 
   int result_code;
   int length; 

   if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
      {
      OutPkgInfoLen++;    /* === */
      CCB1TR6_GetCause ( &length );
      k += length;   
   /* Note that this is an optional element.  If provided, we attempt
   *  to match numbers.
   */
      if (( result_code = CCBRI_AcceptConnectedAddr( &length, Dsl_ID )) == INVALID_ELEM_CONTENTS )
         {
         /* The element was present, but was coded incorrectly 
         */
		/* CCB1TR6_ReleaseCall();	=== */
		CCBRI_ReleaseCall();
         goto bailout;
         }
      else
         {
         if ( result_code == NOERR )
             k+= length;
         }
#ifdef 0
      if (( CCB1TR6_GetCharges( &length )) == NOERR )
         {
         k+= length; 
         }
#endif
      if ( ((  CCB1TR6_GetDate( &length )) == NOERR ) && (IE) )
         {
         k += length; 
         }

      OutPkgInfo[ OutPkgInfoLen - k -1 ] = k;
      CCBRI_MailtoHost( CCB->CopyPublicMsg );
      }

bailout:
 return;

}
/***************************
* CCB1TR6_DisconnectHostCall *
****************************
*
* Build and send the HOST_DISCONNECT/DISCONNECT_ACK message to the HOST.
*
*/
PUBLIC void near   CCB1TR6_DisconnectHostCall( event, cause )

BYTE    *event;
int     cause;                                 /* Reason for rejection */
                                        	
{
 int     k = 0;
 int     length;

   if ( CCBRI_GetOutInfo( *event  ) )
      {
      OutPkgInfoLen++;  
      if ( !cause )
         {
         CCB1TR6_GetCause (&length);
         k += length;  
         }
      else
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 1;
         OutPkgInfo[ OutPkgInfoLen++ ] = cause; 
         k += 3;
         }  
      IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
          NETWORK_FACILITY_IE, CODESET_0, 1);
      if (IE ) 
        {
        OutPkgInfo[ OutPkgInfoLen++ ] = NETWORK_FACILITY_IE;      
        OutPkgInfo[ OutPkgInfoLen++ ] = 2;
        OutPkgInfo[ OutPkgInfoLen++ ] = IE[3];
        OutPkgInfo[ OutPkgInfoLen++ ] = IE[4];
        k+= 4;
        }

      /*
       * Do not do this, causes calls to not be properly
       * disconnected when the remote side disconnects. Clobbers the
       * call-id field and the B-channel cannot be found. ===
       **
       * CCB1TR6_GetCharges( &length );
       * k+= length;
       */

      CCB1TR6_GetDate( &length );
      k += length; 

 
      OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;  /* k is length of Private IEs  */
      CCBRI_MailtoHost( CCB->CopyPublicMsg );

      }
}


/************************
* CCB1TR6_DisconnectInd *
*************************
*
* Generic action upon receiving the CC_DISCONNECT_IND message
* for a network disconnect.
*
*/
PRIVATE BOOLEAN near CCB1TR6_DisconnectInd()

{
   BYTE event = 0;
   int cause;
   /* BYTE cause_ie;	=== for 1tr6 certif */

#ifdef SBC_INTERFACE
#ifdef TASK_KB
   CCBRI_StopConnect( CCB->B_Chan );
#endif
   /* Far end disconnect.  Send CC_RELEASE_REQ. We will retain connection
   *  to the B-Channel.
   */
   /* Build complaining cause info element (optional in this message) */
      cause = (( CCBRI_AcceptCause (&cause_ie) == MANDATORY_IE_MISSING ) ? 
                    LEER_ELEMENT : cause_ie);

      if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) )
         {
         CCBRI_MailtoL3();
         CCB->State = TR6_OFFHOOK_RELEASING;
         }
#endif 
#ifdef COMMAND_INTERFACE
      cause = 0;                               /* Called extracts cause */

/* === */
/* replaced the following code 							=== */
/*    LIF_StopTimer ( CCB->CC1TR6Timer );				=== */
/*    LIF_UpdateTimer( CCB->CC1TR6Timer, CC_ID, CC_ID,	=== */ 
/*			DISC_HOST_EVENT, 0, CCB->Sapi, CCB->Ces,		=== */ 
/*			CCB->Call_ID, 0, DISC_HOST_TIMEOUT);			===*/
/*    LIF_StartTimer (CCB->CC1TR6Timer); 				=== */
/*    CCB->State  = TR6_DISCONNECTING_HOST; 			=== */
/* replaced the above code with the following */
/* === */

  /* Build complaining cause info element (optional in this message)
  */

  /* === removed at 1tr6 testing, for certif
   * cause = (( CCBRI_AcceptCause (&cause_ie) == MANDATORY_IE_MISSING ) ?
   *        LEER_ELEMENT : cause_ie);
   */

	if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) ) {
	   CCBRI_MailtoL3();
	   CCB->State = TR6_OFFHOOK_RELEASING;
	}
/*
 * added the above code ===
 */

      event       = HOST_DISCONNECT; 
      CCB1TR6_DisconnectHostCall( &event , cause );    
#endif

   TheTrash = ( int ) event; 
   return( TRUE );
}
#ifdef COMMAND_INTERFACE
/************************
CCB1TR6_FacilityRequest *
*************************
*
* Process a Facility Request from HOST.
*
*/
/* ===
PRIVATE     near    CCB1TR6_FacilityRequest( event )
=== */PRIVATE void near    CCB1TR6_FacilityRequest( event )
int  event;
{
   int     k;
   int     j; 
   BYTE    msg_type;
   BYTE    facilities;
   BOOLEAN ack = FALSE; 
   int     IE_index = 0;
   
   if (( CCB->DM_Substate == DM_REQUEST_OUTSTANDING ) && (event == HOST_FACILITY ))
     {
     CCBRI_RejectHostMsg( CCB->State );
     goto bailout;
     }

   if (( CCB->DM_Substate != DM_RESPONSE_EXPECTED) && ((event == HOST_FACILITY_ACK) ||
                                                      (event == HOST_FACILITY_REJ ))) 
     {
     CCBRI_RejectHostMsg( CCB->State );
     goto bailout;
     }

   if ( event == HOST_FACILITY ) 
      msg_type = CC_FACILITY_REQ; 
   else  if ( event == HOST_FACILITY_ACK )
      {
      msg_type = CC_FACILITY_ACK; 
      ack = TRUE;
      }
   else   if ( event == HOST_FACILITY_REJ )
      {
      msg_type = CC_FACILITY_REJ;
      ack = TRUE;
      }
   else
      {
#ifdef DISPLAY_LOCAL_ERR
   LIF_Error ( "CCB1TR6" ,"CCB1TR6_FacilityRequest", "Invalid event");
#endif 
	/* Telenetworks bug -- how to deal w/ msg_type?  === */
/*	return;	 === as good as anything */
	goto bailout;	/* ===, they found their bug */
      }
   if  ( CCBRI_GetOutPkt( msg_type, FALSE ) )
      {
      if (InPkgInfo[ PRIVATE_IE_INDEX ] == 0x00)
         {
        /* simply copy all CCITT compatible info elements from
         *  InPkgInfo to info string for L3. Assume that the HOST
         *  did error checking already.
         */
         k = PRIVATE_IE_INDEX + 1;
         while ( k < (InPkgInfoLen -1))
            {
            OutPkt->Info[ OutPkt->InfoLen++ ] = InPkgInfo[ k++ ];
            }
         }
      else
         {
         if ((  CCBRI_AcceptFacilities( &facilities, TRUE, TRUE )) == NOERR )
            {
            if (IE) 
               {
               k = 0;
               OutPkt->Info[ OutPkt->InfoLen++] = (ack ? FACILITY_SELECT_IE : NETWORK_FACILITY_IE);
               j =  OutPkt->InfoLen++;                  /* placeholder */
               if (!ack) 
                  OutPkt->Info[ OutPkt->InfoLen++] = 0x00;        /* zero */
               OutPkt->Info[ OutPkt->InfoLen++] = facilities; /* Fac Code */
               OutPkt->Info[ OutPkt->InfoLen++] = IE[3];    /* Service */
               OutPkt->Info[ OutPkt->InfoLen++] = IE[4]; /* Add   info */
               k += 4;
               IE_index = 5;
               while (IE_index < (IE[1] +2 )  )  /* Pick up Parafelder */
                  OutPkt->Info[ OutPkt->InfoLen++]  = IE[ IE_index++ ];
               k += (IE_index - 5);
               OutPkt->Info[ j ] = k;
               }
            }
         else
            {
         /* Reject call - Facility Code was invalid
         */
            CCBRI_RejectHostCall (INVALID_ELEM_CONTENTS);
            goto bailout;
            }
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, ORIGINATION_ADDRESS_IE ) ) != NULL )
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
         else
            {
            CCBRI_CopyPublic_IE( ORIGINATION_ADDRESS_IE );
            }
         }
      CCB->DM_Substate = DM_REQUEST_OUTSTANDING;
      CCBRI_MailtoL3();
      }

bailout:
  return; 
}
#endif 
/*******************
CCB1TR6_GetCause *
********************
*
* This routine extracts the cause from an incoming message from the
* network. LEERELEMENT (len == 0) is normal clearing   
*
*/
PRIVATE    int      near    CCB1TR6_GetCause ( length )
 int *length;
{
 int cause = NOERR; 
 BYTE cause_ie;

 cause =  CCBRI_AcceptCause (&cause_ie);
  
 switch ( cause ) 
    {
    case MANDATORY_IE_MISSING:
       *length = 0; 
       break;
    case INVALID_ELEM_CONTENTS:
       *length = 3; 
       OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
       OutPkgInfo[ OutPkgInfoLen++ ] = 1;
       OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_1TR6_INVALID;
       break;
    default:
       if ( cause_ie == LEER_ELEMENT ) 
          {
          *length = 2; 
          OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
          OutPkgInfo[ OutPkgInfoLen++ ] = 0;            /* LeerElement */
          }   
       else
          {
          *length = 3;
          OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
          OutPkgInfo[ OutPkgInfoLen++ ] = 1;            
          OutPkgInfo[ OutPkgInfoLen++ ] =  cause_ie;          
          }                                                             
       break;
    } 
  return cause;
}
/* Removed this function and all of its invocations. */ 
#ifdef 0
/*******************
CCB1TR6_GetCharges *
********************
*
* Extract Charges. If a charge IE is invalid (not ASCII digit), then zero
* length is returned to caller.   
*
*/
PRIVATE    int      near    CCB1TR6_GetCharges ( length )
 int *length;
{
 int cause = NOERR; 
 int j = 0; 
 int IE_index; 
 int save_len;
 BOOLEAN gut = TRUE;

 IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
      CHARGING_INFORMATION_IE, CODESET_6, 1);

 save_len = OutPkgInfoLen;
 if ( IE ) 
    {
    IE_index = 3; 
    OutPkgInfo[ OutPkgInfoLen++ ] = CHARGING_INFORMATION_IE; 
    j  = OutPkgInfoLen++; 
    while ( (IE_index < ( IE[1] + 2 ))   && gut )
       { 
       if  ( (IE[IE_index] >= '0') & (IE[IE_index] <= '9') )         
          OutPkgInfo[ OutPkgInfoLen++] = IE[IE_index++];
       else
          gut = FALSE;
       }
    *length  = IE[1] +1;                              /* Aggregate len */
    OutPkgInfo[ j ] = IE[1] -1;                           /* IE length */
    }

 if (!gut) 
   {
   *length = 0;
   cause = INVALID_ELEM_CONTENTS;
   OutPkgInfoLen = save_len;
#ifdef DISPLAY_LOCAL_ERR
   LIF_Error ( "CCB1TR6" ,"CCB1TR6_GetCharges", "Invalid Charges Character");
#endif 
   }

return cause; 
}
#endif
/************************
CCB1TR6_GetFacilities *
*************************
*
* Extract Network Specific Facilities.
*
*/
PRIVATE    int      near    CCB1TR6_GetFacilities( length, nsf )
 int *length;
 BOOLEAN nsf;
{
 int cause; 
 int j; 
 int IE_index; 
 BYTE facilities;

  if ((( cause = CCBRI_AcceptFacilities( &facilities, FALSE,
       ((nsf) ? TRUE : FALSE) )) == NOERR ) && ( IE ))
     {
     OutPkgInfo[ OutPkgInfoLen++ ] =  NETWORK_FACILITY_IE; 
     j =  OutPkgInfoLen++; 
     if (nsf)
        {
        OutPkgInfo[ OutPkgInfoLen++ ] = IE[3];             /* Fac Code */
        OutPkgInfo[ OutPkgInfoLen++ ] = IE[4];              /* Service */
        OutPkgInfo[ OutPkgInfoLen++ ] = IE[5];      /* additional info */
        IE_index = 6;
        while (IE_index < (IE[1] +2 ) )          /* Pick up Parafelder */
           OutPkgInfo[ OutPkgInfoLen++] = IE[ IE_index++ ];
        *length  = IE[1] +1; 
        OutPkgInfo[ j ] = IE[1] -1;            /* length of Private IE */
        }
     else
        {
        OutPkgInfo[ OutPkgInfoLen++ ] = IE[2];             /* Fac Code */
        OutPkgInfo[ OutPkgInfoLen++ ] = IE[3];              /* Service */
        OutPkgInfo[ j ] = 2;                   /* length of Private IE */
        *length  = IE[1] ; 
        }
     }

return cause; 

}
/*******************
CCB1TR6_GetDate  *
********************
*
* Extract Date. If a date octet is invalid, zero length is returned to
* caller. This routine is not extremely rigorous, but it does screen
* bad characters, which is the degree of validation  1 TR20 seems to call
* for.	===, changes within the routine
*/
PRIVATE    int      near    CCB1TR6_GetDate ( length )
 int *length;
{

 int cause = NOERR; 
 BOOLEAN gut = TRUE;
 int IE_index; 
 BYTE *ie_ptr; 
 int j;

   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
        DATE_IE, CODESET_6, 1);

   if ( ( IE ) && (IE[1] > 0) )
      {
      for ( ie_ptr=&IE[2]; (ie_ptr<(&IE[2] + IE[1])) && (gut); ie_ptr++ )
         {
         switch (ie_ptr - IE)
            {
            case 2:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '3') )       /* Tag */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 3:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '9') )       /* Tag */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 4:
               if ( (*ie_ptr == '.') )
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 5:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '1') )     /* Monat */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 6:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '9') )     /* Monat */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 7:
               if ( (*ie_ptr == '.') )
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 8:
               /*if ( (*ie_ptr == '9') ||  (*ie_ptr == '0') ) === Year 9n-0n */
               if ( (*ie_ptr == '8') || (*ie_ptr == '9') ||  (*ie_ptr == '0') ) /* Year 8n-0n === for 1tr6 certif */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 9:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '9') )      /* Year */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 10:
               if ( (*ie_ptr == '-') )
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 11:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '2') )       /* Std */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 12:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '9') )       /* Std */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 13:
               if ( (*ie_ptr == ':') )
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 14:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '5') )       /* Min */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 15:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '9') )       /* Min */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 16:
               if ( (*ie_ptr == ':') )
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 17:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '5') )      /* Sek */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            case 18:
               if ( (*ie_ptr >= '0') && (*ie_ptr <= '9') )      /* Sek */
                  {
                  break;
                  }
               else
                  {
                  gut = FALSE;
                  }
               break;
            default:
               break;
            }                                 /* End of Case Statement */
         }                                          /* End of For Loop */


    if (gut)
       {
       IE_index = 2; 
       OutPkgInfo[ OutPkgInfoLen++ ] = DATE_IE; 
       j =  OutPkgInfoLen++; 
       while (IE_index < IE[1] +2)
          OutPkgInfo[ OutPkgInfoLen++] = IE[IE_index++ ];
       OutPkgInfo[ j ] = IE[1];
       *length = IE[1] +2; 
       }
    else
       {
       cause = INVALID_ELEM_CONTENTS;  
       *length = 0;
#ifdef DISPLAY_LOCAL_ERR
       LIF_Error ( "CCB1TR6" ,"CCB1TR6_GetDate", "Invalid Date Character");
#endif 
       }
    }
 else
   {
   *length = 0;
   cause = MANDATORY_IE_MISSING;
   }

return cause; 
}
#ifdef COMMAND_INTERFACE
/************************
* CCB1TR6_HostCallRequest *
*************************
*
* Process a request for new call from HOST.
*
*/
PUBLIC int near  CCB1TR6_HostCallRequest()

{
   int k;
   int j;
   int     chan;
   BYTE    service_description;
   BYTE    addition_info;  
   BYTE    facilities;
   BOOLEAN exclusive;
   int result_code = NOERR;
   int IE_index;

   if  ( CCBRI_GetOutPkt( CC_SETUP_REQ, FALSE ) )
      {
      if (InPkgInfo[ PRIVATE_IE_INDEX ] == 0x00)
         {
         /* simply copy all CCITT compatible info elements from
         *  InPkgInfo to info string for L3. Assume that the HOST
         *  did error checking already.
         */
         k = PRIVATE_IE_INDEX + 1;
         while ( k < InPkgInfoLen )
            {
            OutPkt->Info[ OutPkt->InfoLen++ ] = InPkgInfo[ k++ ];
            }
         }
      else
         {
         /* now for several non-mandatory info elements.
         */
         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CHANNEL_ID_IE ) ) != NULL )
            {
            exclusive = IE[ 2 ];
            chan      = IE[ 3 ];
            }
         else if (( IE = CCBRI_GetPublicIE( CHANNEL_ID_IE, CODESET_0 ) ) != NULL)
            {
            IE = CCIE_ExpandChanId_IE( IE, &Trashcan );
            exclusive = IE[ 2 ];
            chan = IE[ 4 ];
            }
         else                                 /* Channel not specified */
            {
            exclusive = FALSE;
            chan = ANY_CHANNEL;
            }

         if ( chan == ANY_CHANNEL )
            {
            result_code =
               CCBRI_SetNewChan( ANY_CHANNEL, CHANNEL_PROPOSED, FALSE );
            }
         else if ( CC_CHAN_GetStatebri( chan,
                                     B_CHANNEL_UNITS,
                                     Dsl_ID ) == CHANNEL_IDLE )
            {
            result_code =
               CCBRI_SetNewChan( chan, CHANNEL_PROPOSED, FALSE );
            }
         else if ( !exclusive )
            {
            result_code =                         /* Pick another one. */
               CCBRI_SetNewChan( ANY_CHANNEL, CHANNEL_PROPOSED, FALSE );
            }
         else
            {
            result_code = NO_CIRCUIT_AVAILABLE;            /* Cause 34 */
            }

         if (( result_code == NOERR ) && ( IE ) ) 
            {
            /* Build the CCITT compatible info element.
            */
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CCB->B_Chan,       /* channel id * */
                                    CCB->B_Chan,              /* ditto */
                                    B_CHANNEL_UNITS,
                                    FALSE,           /* not a slot map */
                                    exclusive,            /* exclusive */
                                    0,      /* no interface identifier */
                                    NULL );

            }
         else if (result_code)                     /* no Chan is legal */
            {
            LIF_FreePkt( OutPkt );     /* Release the outgoing packet. */
            OutPkt = NULL;
            CCBRI_RejectHostCall( result_code );
            goto bailout;
            }

         /* 
         *  Facilities are on optional element.  We check to see if its
         *  present and correctly coded
         */
         if (( CCBRI_AcceptFacilities( &facilities, TRUE, TRUE )) == NOERR )
            {
            if (IE) 
               {
               k = 0;
               OutPkt->Info[ OutPkt->InfoLen++] = NETWORK_FACILITY_IE;
               j =  OutPkt->InfoLen++;                  /* placeholder */
               OutPkt->Info[ OutPkt->InfoLen++] = 0x00;        /* zero */
               OutPkt->Info[ OutPkt->InfoLen++] = facilities; /* Fac Code */
               OutPkt->Info[ OutPkt->InfoLen++] = IE[3];    /* Service */
               OutPkt->Info[ OutPkt->InfoLen++] = IE[4]; /* Addition info */
               k += 4;
               IE_index = 5;
               while (IE_index < (IE[1] +2 )  )  /* Pick up Parafelder */
                  OutPkt->Info[ OutPkt->InfoLen++]  = IE[ IE_index++ ];
               k += (IE_index - 5);
               OutPkt->Info[ j ] = k;
               }
            }
         else
            {
         /* Reject call - Facility Code was invalid
         */
            CCBRI_RejectHostCall (INVALID_ELEM_CONTENTS);
            LIF_FreePkt( OutPkt );   /* Release the outgoing packet. */
            OutPkt = NULL;
            goto bailout;
            }

         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, ORIGINATION_ADDRESS_IE ) ) != NULL )
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
            CCBRI_CopyPublic_IE( ORIGINATION_ADDRESS_IE );
            }

         if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, DESTINATION_ADDRESS_IE ) ) != NULL )
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
         else
            {
            CCBRI_CopyPublic_IE( DESTINATION_ADDRESS_IE );
            }

            /* Service Indicator is MANDATORY.
            */

         if ( ( result_code = CCBRI_AcceptServiceInd ( &service_description, &addition_info, TRUE ) ) == NOERR)
            {
            /* The IE pointer is initialized if it's located.
            */
            if (IE) 
               {
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
               LIF_FreePkt( OutPkt );   /* Release the outgoing packet */
               OutPkt = NULL;
               CCBRI_RejectHostCall (INVALID_ELEM_CONTENTS);
               goto bailout;
               }
            }
         else if ( result_code == INVALID_ELEM_CONTENTS )
            {
            LIF_FreePkt( OutPkt );     /* Release the outgoing packet  */
            OutPkt = NULL;
            CCBRI_RejectHostCall (INVALID_ELEM_CONTENTS);
            goto bailout;
            }
         else if ( CCBRI_CopyPublic_IE( SERVICE_INDICATOR_IE ) != NOERR )
            {
            LIF_FreePkt( OutPkt );     /* Release the outgoing packet. */
            OutPkt = NULL;
            CCBRI_RejectHostCall (INVALID_ELEM_CONTENTS);
            goto bailout;
            }

         }
      /* send the mail now:
      */
      CCBRI_MailtoL3();
      /* NOTE: this function will NOT change the state of the interface.
      *        It is left up to the calling function !
      */
      }
   else
      {
      /* error - no more memory.
      */
      }

bailout:
   return( result_code );
}
#endif 
#ifdef COMMAND_INTERFACE
/******************************
* CCB1TR6_HostRegisterRequest *
*******************************
*
* Registration Maintenance activities are initiated here.
*
*/
PRIVATE  int near  CCB1TR6_HostRegisterRequest ( event )

int  event;
{
   int     k;
   int     j;
   int     IE_index;
   int     cause;
   BYTE    msg_type;
   BYTE    facilities;

   if (event == HOST_FACILITY_REGISTER )
      msg_type = CC_REGISTER_REQ; 
   else if (event == HOST_FACILITY_CANCEL )
      msg_type = CC_FAC_CANCEL_REQ; 
   else
      {
#ifdef DISPLAY_LOCAL_ERR
   LIF_Error ( "CCB1TR6" ,"CCB1TR6_RegisterRequest", "Invalid event");
#endif 
	/* Telenetworks bug -- how to deal w/ msg_type? === */
/*	return ERR;		 === as good as anything */
	goto bailout;	/* ===, they found their bug */
      }


   if ( CCBRI_GetOutPkt( msg_type, FALSE ) )
      {             	
      if (InPkgInfo[ PRIVATE_IE_INDEX ] == 0x00)
         {
         /* simply copy all CCITT compatible info elements from
          *  InPkgInfo to info string for L3. Assume that the HOST
          *  did error checking already.
          */
         k = PRIVATE_IE_INDEX + 1;
         while ( k < (InPkgInfoLen -1) )
            {
            OutPkt->Info[ OutPkt->InfoLen++ ] = InPkgInfo[ k++ ];
            }
         }
       else
         {
         if ((( CCBRI_AcceptFacilities( &facilities, TRUE, TRUE )) == NOERR )
            && (IE) ) 
            {
            k = 0;
            OutPkt->Info[ OutPkt->InfoLen++] = NETWORK_FACILITY_IE;
            j =  OutPkt->InfoLen++;                  /* placeholder */
            OutPkt->Info[ OutPkt->InfoLen++] = 0x00;        /* zero */
            OutPkt->Info[ OutPkt->InfoLen++] = facilities; /* Fac Code */
            OutPkt->Info[ OutPkt->InfoLen++] = IE[3];    /* Service */
            OutPkt->Info[ OutPkt->InfoLen++] = IE[4]; /* Addition info */
            k += 4;
            IE_index = 5;
            while (IE_index < (IE[1] +2 )  )  /* Pick up Parafelder */
                 OutPkt->Info[ OutPkt->InfoLen++]  = IE[ IE_index++ ];
            k += (IE_index - 5);
            OutPkt->Info[ j ] = k;
            }
         }
         CCBRI_MailtoL3();
         CCB->State = TR6_REG_MAINT;
      }
bailout:	/* === */
   return (cause = NOERR);
}
#endif 
/***********************
* CCB1TR6_InfoReceived *
************************
*
* Info Indication has been received from Network. Pass it up to the Host
*
*/
PRIVATE    BOOLEAN  near    CCB1TR6_InfoReceived()
{
   int k = 0;
   int cause;
   BYTE facilities; 
#ifdef 0
   int length;
#endif
   int IE_index = 0;

   /* inform the HOST.
   */
   if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
      {
       /* Look at Network Specific Facilities;
        */

      OutPkgInfoLen++;		/* === */	
      if ((( CCBRI_AcceptFacilities(&facilities, FALSE, TRUE)) == NOERR )
           && (IE)) 
          {
          /* build the CCITT compatible info element.
          */
          OutPkt->Info[ OutPkt->InfoLen++] = NETWORK_FACILITY_IE;
          k =  OutPkgInfoLen++ ; 
          OutPkt->Info[ OutPkt->InfoLen++] = IE[2];        /* Fac Code */
          OutPkt->Info[ OutPkt->InfoLen++] = IE[3];         /* Service */
          OutPkt->Info[ OutPkt->InfoLen++] = IE[4];   /* Addition info */
          k += 3;
          while (IE_index < (IE[1] -3) )         /* Pick up Parafelder */
             OutPkgInfo[ OutPkgInfoLen++] = InPkt->Info[IE_index++ ];
		    k+= IE_index; 

          if  ((CCB->State == TR6_DIALING) && (IE[2] == ANZEIGE_UMGELEITETER_RUF ))
            CCB->State = TR6_ROUTING;
          }
/* 
 * Don't invoke the next function as it may return without initializing the
 * length. The length may contain garbage and will cause a crash while
 * subtracting k from OutPkgInfoLen. 
 */
#ifdef 0
      if (( CCB1TR6_GetCharges( &length )) == NOERR )
         {
         k+= length; 
         }
#endif
      OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
      CCBRI_MailtoHost( CCB->CopyPublicMsg );
      } 

  return  (cause = NOERR);
}
/**********************
* CCB1TR6_ReleaseCall *
***********************
*
* Terminate a call and put the CCB to bed.
*
*/
PUBLIC void CCB1TR6_ReleaseCall()

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
* CCB1TR6_ReleasedLink *
************************
*
* The data link is down at Layer 2.
* Remove CCBs from any context but LOCKOUT.  Users left OFF HOOK
* must retain a CCB in the LOCKOUT state to process their hangup.
*
*/
PUBLIC void CCB1TR6_ReleasedLink (ces, dsl_id)
BYTE ces;           /* Connection Endpoint Suffix of the released link. */
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

#ifdef SBC_INTERFACE 
/*******************
* CCB1TR6_SetupInd *
********************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function
* because of its inherent complexity.
*
*/
PRIVATE void near CCB1TR6_SetupInd()

{
   int     cause;
   int     length;
   int     k = 0;
   int     j = 0;
   BYTE    chan_id;
   BYTE    exclusive;
   BYTE    service; 
   BYTE    add_info;
   BYTE    facilities;
   BYTE    numb[ MAX_ADDRESS_DIGITS ];
   int     IE_index = 0;
#ifdef TASK_KB
   BYTE    ring_type;
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
   */
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
      CCBRI_RejectSetup( USER_BUSY_1TR6, 0 );
	/* CCB1TR6_ReleaseCall();	=== */
	  CCBRI_ReleaseCall();
      goto bailout;
      }
       /* Look at the Service Ind IE: a mandatory element;
        */

   if (( cause = CCBRI_AcceptServiceInd(&service, &add_info, FALSE)) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected - by ignoring
      * the message per 1TR20
      */
     /*  CCBRI_RejectSetup( cause, 0 );  */
	/* CCB1TR6_ReleaseCall();	=== */
		CCBRI_ReleaseCall();
      goto bailout;
      }

   /* Next we look at the Channel ID.
   */
   if (( cause = CCBRI_AcceptChanId( &chan_id, &exclusive)) != NOERR )
      {
      /* In 1TR6 the SETUP can be resent, with an available channel
      */
      if (  cause != MANDATORY_IE_MISSING  )
         { 
         CCBRI_RejectSetup( cause, 0 );
         CCB1TR6_ReleaseCall();
         goto bailout;
         }
      }
   else
      {
      if ( (cause = CCBRI_SetNewChan( (int) chan_id,
                                      CHANNEL_PROPOSED_1TR6,
                                      TRUE ) ) != NOERR )
        {
        CCB1TR6_ReleaseCall();
        goto bailout;
        }
      }

   /* Note that this is an optional element.  If provided, we verify
   *  it. If its coded incorrectly, we just skip it and don't release the
   *  call.
   */

   if (( cause = CCBRI_AcceptFacilities( &facilities, FALSE, TRUE )) != INVALID_ELEM_CONTENTS )
      {
      ;
      }
   /* Note that ORIGINATION ADDRESS is an optional element.  If provided, 
   *  attempt to match numbers.
   */
    if (( cause = CCBRI_AcceptCallingNumb(numb , &length )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly or the
      *  ignore the setup. 1TR20 is pretty loose about invalidly
      *  coded IEs - it may be enough to ignore this IE  (See Page 43 - 1TR6 )
      */
      ;
      }     

   /* Note that this is an optional element.  If provided, we attempt
   *  to match numbers.
   */
   if (( cause = CCBRI_AcceptCalledNumb( &length, Dsl_ID )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly or the
      *  called number did not match ours - ignore the setup else the following:
      */
      ;
      }
      /* Setup can be resent to update the Channel Id - we
      *  only want to send Broadcast if its first time 
      */
 
   if (CCB->State == TR6_IDLE)
      {
      if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
         {
         CCBRI_MailtoL3();                      /* Send CC_BROADCAST_RESP */
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
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

            CCBRI_MailtoL3();                     /* Send CC_ALERTING_REQ */
#ifdef TASK_KB
            CCBRI_StartRing( ring_type );         /* Here turn ringing on */
#endif
            Hcb[ DEFAULT_DSL ].Hw_State = BELL;
            if (CCB->State == TR6_IDLE)
               CCB->State = TR6_RINGING; 
            CCBRI_TRACE( TR_RINGING );
            }
         }
      }

bailout:

   return;
}

#endif 
#ifdef COMMAND_INTERFACE 
/*******************
* CCB1TR6_SetupInd *
********************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function
* because of its inherent complexity.
*
*/
PRIVATE void near CCB1TR6_SetupInd()

{
   int     cause;
   int     length;
   int     k = 0;
   int     j = 0;
   BYTE    chan_id;
   BYTE    exclusive;
   BYTE    service; 
   BYTE    add_info;
   BYTE    facilities;
   BYTE    numb[ MAX_ADDRESS_DIGITS ];
   int     IE_index = 0;

   /* A new call SETUP from the far end. The CC may send
   *  ALERTING request to the network instructing it that
   *  the phone is ringing.  This will be done only after
   *  unpacking and checking of info elements in the CC_SETUP_IND
   *  message.
   *
   *  Before sending of ALERTING message, the CC must always mail
   *  CC_BROADCAST_RESP message.  This will assure that the Layer 3
   *  software is in a proper state to receive ALERTING.
   */

   /* inform the HOST.
   */

   if ( CCBRI_GetOutInfo( HOST_INCOMING_CALL ) )
      {
      OutPkgInfoLen++;
       /* Look at the Service Ind IE: a mandatory element;
        */

      if (( cause = CCBRI_AcceptServiceInd(&service, &add_info, FALSE)) != NOERR )
         {
      /* This is a mandatory element.  The call must be rejected - by ignoring
      * the message 
      */                    	
         /* CCB1TR6_ReleaseCall();	=== */
		 CCBRI_ReleaseCall();
         CCBRI_FreeCallBufs();  
         goto bailout;
         }
      else
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = SERVICE_INDICATOR_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 2;
         OutPkgInfo[ OutPkgInfoLen++ ] = service;
         OutPkgInfo[ OutPkgInfoLen++ ] = add_info;
         k += 4;
         }

      }
   else
      {
      /* CCB1TR6_ReleaseCall();	=== */
	  CCBRI_ReleaseCall();
      goto bailout;
      }
   /* Next we look at the Channel ID.
   */
    if (( cause = CCBRI_AcceptChanId( &chan_id, &exclusive)) != NOERR )
      {
      /* In 1TR6 the SETUP can be resent, with an available channel
      */
      if ( cause != MANDATORY_IE_MISSING ) 
         { 
         CCBRI_RejectSetup( cause, 0 );
         /* CCB1TR6_ReleaseCall();	=== */
		 CCBRI_ReleaseCall();
         CCBRI_FreeCallBufs();  
         goto bailout;
         }
      }
   else
      {
      if ( (cause = CCBRI_SetNewChan( (int) chan_id,
                                      CHANNEL_PROPOSED_1TR6,
                                      TRUE ) ) != NOERR )
         {
      /* In 1TR6 certain DM such as Anklopfen must customize channel management
       * Since we don't support Anklopfen we'll ignore the SETUP 
       * per Test 1.3.1.3.2 of 1 TR20 option 3i		===???
      */
         /* CCB1TR6_ReleaseCall();	=== */
		 CCBRI_ReleaseCall();
         CCBRI_FreeCallBufs();
         goto bailout;
         }
      else 
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 2;
         OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
         OutPkgInfo[ OutPkgInfoLen++ ] = chan_id;
         k += 4;
         }
      }
   /* Note that this is an optional element.  If provided, we verify
   *  it. If its coded incorrectly, we just skip it and don't release the
   *  call.
   */


   if ((( cause = CCBRI_AcceptFacilities(
                 &facilities, FALSE, TRUE )) != INVALID_ELEM_CONTENTS ) && (IE) )
      {
      OutPkgInfo[ OutPkgInfoLen++ ] = NETWORK_FACILITY_IE;
      j =  OutPkgInfoLen++; 
      OutPkgInfo[ OutPkgInfoLen++ ] = IE[3];               /* Fac Code */
      OutPkgInfo[ OutPkgInfoLen++ ] = IE[4];                /* Service */
      OutPkgInfo[ OutPkgInfoLen++ ] = IE[5];          /* Addition Info */
      k += 5;
      IE_index = 6;
      while (IE_index < (IE[1] +2 ) )            /* Pick up Parafelder */
         OutPkgInfo[ OutPkgInfoLen++] = IE[ IE_index++ ];
      k+= IE_index -6 ; 
      OutPkgInfo[ j ] = IE[1] -1;              /* length of Private IE */
      }

   /* Note that ORIGINATION ADDRESS is an optional element.  If provided, 
   *  attempt to match numbers.
   */
   if (( cause = CCBRI_AcceptCallingNumb( &numb[0] , &length )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly or the
      *  ignore the setup. 1TR20 is pretty loose about invalidly
      *  coded IEs - it may be enough to ignore this IE
      */
      ; 
      }
   else
      {
      if ( cause == NOERR )
         k+= length;
      }

   /* Note that this is an optional element.  If provided, we attempt
   *  to match numbers.
   */
   if (( cause = CCBRI_AcceptCalledNumb( &length, Dsl_ID )) == INVALID_ELEM_CONTENTS )
      {
      /* The element was present, but was coded incorrectly or the
      *  called number did not match ours - ignore the setup.
      */
      ;
      }
   else
      {
      if ( cause == NOERR )
         k+= length;
      }

      /* inform the HOST
      */
      if ( OutPkgInfo )
         {
         OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
         CCBRI_MailtoHost( CCB->CopyPublicMsg );

      /* Only advance the state if its a first time SETUP
      */
         if (CCB->State == TR6_IDLE)
            CCB->State = TR6_CALL_OFFERED; 
         }

bailout:

   return;
}

#endif 
/*****************
* CCB1TR6_Shutdown *
******************
*
* Reset Call Control
*
*/
PUBLIC void  CCB1TR6_Shutdown (dsl)
BYTE dsl;
{
    CCB_ID temp_ccb;

    Dsl_ID = dsl;
    CCB = CCB_Head[dsl];
    while (CCB) {
        temp_ccb = CCB->Next;
        CCBRI_ReleaseCall();
        CCB = temp_ccb; 
    }
}

#endif	/* === */
