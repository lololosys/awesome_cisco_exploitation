/* $Id: ccbnet3.c,v 3.4.12.4 1996/09/13 04:54:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccbnet3.c,v $
 *------------------------------------------------------------------
 * ccbnet3.c  ISDN NET3 BRI Call Control Functions
 *
 * March 1993, Dennis Clare
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccbnet3.c,v $
 * Revision 3.4.12.4  1996/09/13  04:54:32  thille
 * CSCdi68927:  Clean up some things that should be const in ISDN
 * Branch: California_branch
 * Make some more stuff const to save ram.  Also make some more stuff
 * static.
 *
 * Revision 3.4.12.3  1996/08/15  21:29:22  rbeach
 * CSCdi63895:  ISDN connect channel Id will be rejected in specific cases.
 * If a connect is received in CallDialing state and we have already
 * received a message containing a channel id ie we will send a connect
 * ack and then send a release with a cause of Message not compatible with
 * call state. This is not proper behavior.
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/05/21  18:54:38  dclare
 * CSCdi58183:  ISDN BRI NET3 fails Singapore overlap receiving case.
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/04/30  23:06:49  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/15  17:27:14  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.3  1996/02/09  16:36:36  dclare
 * CSCdi48422:  ISDN BRI NET3 needs changes to pass France NET3 Delta
 * testing. Allow incoming Alerting and Connect message in state U1.
 *
 * Revision 3.2  1995/11/17  17:39:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/19  16:52:23  dclare
 * CSCdi40646:  ISDN BRI changes for Italy NET3 homologation.
 *
 * Revision 2.1  1995/06/07  21:13:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccbnet3.c
VERSION: 3.21+
DESCRIPTION: ISDN NET3 BRI Call Control Functions

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

This module started life as a 3.21 version of the ccbvn.c file. It has
been changed to fit the NET3 way of doing things.

The current version of this module is intended to implement the
Call Control functions for a simple user-side Basic Rate Interface
(Basic Rate Access) terminal in a manner compatible with a
Central Office switch in accordance with ETS Specifications ETS 300
102-1, December 1990. The terminal is
assumed to be implemented using subscribed in-band call progress tones
and to send address information via overlapped sending or en-bloc.
A simple keypad interface and a multi-byte message interface from the
user-side application are provided.

OTHER ASSUMPTIONS AND REMARKS PERTAINING TO THE MODULE

0.  The SBC_INTERFACE flag code has been ripped out of here. We never
    used it, so it just cleaned up the listing. So the stuff in section
	1 below is really just info and not relevant. Although we actually
	default to COMMAND_INTERFACE.

1.  Two compile time flags are used to configure the operation of this
    module for different applications interfaces.  The flag SBC_INTERFACE
    is defined when this module is driven by a keystroke interface from
    above (simple phone).  The COMMAND_INTERFACE flag is defined if the
    application is capable of sending and receiving multi-byte messages
    with Call Control.  Both of these flags should not be defined at the
    same time.

    When using the SBC_INTERFACE, the application is assumed to be a simple
    BRI telephone or terminal which can meaningfully handle voice calls for
    simple connection and some basic Supplementary Services.

    When using the COMMAND_INTERFACE, the application is assumed to be a
    processor-based subsystem capable of multi-byte message construction
    and decoding, but allowing it greater call-by-call flexibility.

2.  The information element error checking incompletely implements the
    compatibility checking delineated in the ETS ISDN Specification.
    This specification describes a nearly Herculean requirement for
    error checking, especially with regard to non-mandatory information
    elements.  This requirement greatly exceeds most ISDN product
    requirements for other switch vendors, thus these procedures are
    not assumed to be realistic nor expected of a user product.
    It is hoped that the plan for error checking and recovery therefrom
    is sufficiently demonstrated by example that extension of it will
    be a reasonable undertaking for some currently unsupported optional
    element if the need should arise.
3.  It is assumed that, in the keypad mode, the hardware for this
    application is controlled by some external hardware driver, which
    is implemented as another task.  The interface between these tasks
    is through system mail.  The traffic from the hardware driver is
    single byte commands (SBCs) which represent keystrokes and hookswitch
    reports.  Control of the physical hardware is deliberately left vague
    as this is highly dependent upon the target hardware where this code
    is embedded.
4.  It is assumed that call progress tones will be supplied over the
    B-Channel, from the network, rather than generated locally, or that
    the Host application is in control of this function.
5.  The telephone number for this device, when in keystroke mode, is
    fixed at compile time.  In a real product, this information must
    be made configurable on the device.
6.  The bearer capability of this device, when in keypad mode, is fixed
    at compile time.  In a real product, this information may be required
    to be configurable.
7.  The entry point to the module is CCBNET3_Go.
8.  Throughout the call state functions of the module, when in keypad mode,
    function calls are made to LIF_SendSBC(), sending state updates to
    TRACER_ID inside of the #ifdef SEND_TR_MSG compile flag.  These are
    state updates designed to support the Telenetworks TRACKER.C module in
    its display updates which may be used in test environments with this
    module as a substitute for real application hardware.

This version does not support supplementary services using the multi-byte
message interface (COMMAND_INTERFACE).  The module is capable of managing
multiple concurrent and independent calls in this mode.

Various simplifications have been made which reflect the fact that
this is not the software designed to run, as is, a real hardware
device.  For example, the directory number is simply fixed at compile
time, where a real product might be expected to have NV ram for such
volatile information, with a user-programming capability.

The Call Control is assumed to reside on a point-to-multipoint
interface wherein SETUP is broadcast to the TEs.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*/
/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

/* Standard C library functions/definitions
*/
/* """
#include    <stdio.h>
#include    <string.h>
""" */

#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

/* ISDN includes
*/
#include    "flags.h"
#ifdef BRI_NET3
#include    "tune.h"
#include    "gendef.h"
#include    "lif.h"
#include    "ccie.h"
#include	"gp.h"	/*===*/
#include    "ccbri.h"
#include    "cc_chan.h"

static int	quiet_release;		/* === */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* Delinition of CCB Call States
*
*  Note: The shared assumptions between this module and CCBRI.C require
*        that the enumerated position of the VN_IDLE constant be fixed
*        in position 1.
*
*  Another Note: The enumerated indices generated here and the function
*                references in the Ccb_State_Func_Net3 table further on must
*                agree in number and position.
*/
enum CCB_State
   {
   NET3_STATE_LOW_LIMIT,                               /* Sizing element */
   NET3_IDLE,                                                /* CCB idle */
   NET3_ALERT,                                          /* Being alerted */
   NET3_CONVERSATION,                                    /* Conversation */
   NET3_DIAL,                                                 /* Dialing */
   NET3_OFFERED,                 /* Await host response to incoming call */
   NET3_ORIGINATE,                     /* Await response to CC_SETUP_REQ */
   NET3_RELEASE,                          /* Waiting for release confirm */
   NET3_RELEASE_ACK,        /* Waiting for release confirm (ack to HOST) */
   NET3_ROUTING,        /* Busy, but neither dialing nor in conversation */
   NET3_STATE_HIGH_LIMIT                               /* Sizing element */
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
*              F U N C T I O N   D E C L A R A T I O N S                *
*************************************************************************
*/
/* Arranged alphabetically.
*/
PRIVATE int      near CCBNET3_CallAlerting     (int *);
PRIVATE int      near CCBNET3_CallConversation (int *);
PRIVATE int      near CCBNET3_CallDialing      (int *);
PRIVATE int      near CCBNET3_CallIdle         (int *);
PRIVATE int      near CCBNET3_CallOffered      (int *);
PRIVATE int      near CCBNET3_CallOriginate    (int *);
PRIVATE int      near CCBNET3_CallRelease      (int *);
PRIVATE int      near CCBNET3_CallReleaseAck   (int *);
PRIVATE int      near CCBNET3_CallRouting      (int *);
PRIVATE int      near CCBNET3_CauseToResult    (BYTE);
PRIVATE BOOLEAN  near CCBNET3_DisconnectInd    (void);
PRIVATE int      near CCBNET3_RejectCall       (BYTE);
PRIVATE void     near CCBNET3_RejectSetup      (BYTE);
PRIVATE void     near CCBNET3_ReleaseChan      (void);
PRIVATE BYTE     near CCBNET3_SetNewChan       (int, int, BOOLEAN);
PRIVATE int      near CCBNET3_SetupInd         (void);
PRIVATE void     near CCBNET3_UnexpectedMsg    (BYTE);
PRIVATE BYTE     near CCBNET3_VerifyDiscIEs    (BYTE *, int);

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
/* State Function Jump Table
*/
typedef int (near *CC_STATES)( int * );
static const CC_STATES Ccb_State_Func_Net3[ NET3_STATE_HIGH_LIMIT ] =
   {
   NULL
   ,
   CCBNET3_CallIdle
   ,
   CCBNET3_CallAlerting
   ,
   CCBNET3_CallConversation
   ,
   CCBNET3_CallDialing
   ,
   CCBNET3_CallOffered
   ,
   CCBNET3_CallOriginate
   ,
   CCBNET3_CallRelease
   ,
   CCBNET3_CallReleaseAck
   ,
   CCBNET3_CallRouting
   };

PRIVATE BYTE    Diag[ 2 ];     /* For passing cause diagnostic to CCIE */
PRIVATE BYTE    ByteTrash;                              /* Byte bucket */

/*
 *	We always must send the REL_COMP.  Unless there was an error and
 *	layer 3 already sent it, in which case quiet release is set.
 *
 *	Entire routine written by rbk ===
 */
static void
CCBNET3_process_releaseind (void)
{
	int cause = 0;		/* initialize to suppress compiler warning */

	if(quiet_release || (cause = CCBRI_AcceptCause((BYTE *)&Trashcan))) { /*===*/
		/* It's bad.  Disconnect with a Release Complete */
		if(CCB->B_Chan)
			CCBNET3_ReleaseChan();
		if(CCBRI_GetOutInfo(HOST_DISCONNECT_ACK)) {
			/* total length of private IE's = 0 */
			OutPkgInfo[ OutPkgInfoLen++ ] = 0;
			CCBRI_MailtoHost(CCB->CopyPublicMsg);
		}
		if(!quiet_release)
			CCBNET3_RejectCall(cause);
	} else {
		if(CCBRI_GetOutPkt(CC_REL_COMP_REQ, FALSE)) {
			/* Used to have a TermCap here, but I think it's unneeded */
			CCBRI_MailtoL3();
		}
	}
}


/************************************************************************
*                C A L L   S T A T E   F U N C T I O N S                *
*************************************************************************
*/
/*********************
* CCBNET3_CallAlerting *
**********************
*
* Process the current event in the context of a voice call in the
* ALERT state.
*
*/
PRIVATE int near CCBNET3_CallAlerting( event )

int *event;                                /* Event input to the state */

{
   int  result_code;

   result_code = NOERR;

   switch ( *event )
      {
      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = NET3_ROUTING;
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );    /* Process disc from HOST. */
         CCB->State = NET3_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );/* Process disc ack from HOST. */
         CCB->State = NET3_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case CC_DISCONNECT_IND:
         /* Distant end disconnect (response to this message will
         *  be received in RELEASING state - see CC_RELEASE_CONF):
         */
         CCBNET3_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBNET3_process_releaseind();
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBNET3_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
/* ===
bailout:
=== */
   return( result_code );
}

/*************************
* CCBNET3_CallConversation *
**************************
*
* Process the current event in the context of a voice call in the
* CONVERSATION state.
*
*/
PRIVATE int near CCBNET3_CallConversation( event )

int *event;                                /* Event input to the state */

{
   int     result_code;                /* Result code for the function */

   result_code = NOERR;

   switch ( *event )
      {
      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NET3_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = NET3_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      case CC_DISCONNECT_IND:
         if ( CCB->B_Chan )
            CCBNET3_ReleaseChan();
         CCBNET3_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBNET3_process_releaseind();
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBNET3_ReleaseChan();
            }
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBNET3_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

/********************
* CCBNET3_CallDialing *
*********************
*
* Process the current event in the context of a voice call in the
* DIAL state.
*
*/
PRIVATE int near CCBNET3_CallDialing( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   BYTE cause;
   BYTE chan = 0;

   result_code = NOERR;

   switch ( *event )
      {
      case HOST_INFORMATION:     /* Process the information from HOST. */
         CCBRI_HostInfo();                      /* No change in state. */
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NET3_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = NET3_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      case CC_CALL_PROC_IND:
         /* This message indicates that dialing is complete.
         */
         if ( CCB->B_Chan == 0 )             /* Should not be the case */
            cause = CCBRI_AcceptChanId( &chan, &ByteTrash );
         else
            cause = NOERR;

         /* The Channel ID element is not mandatory in this message thus
         *  we are only concerned if it is present and coded incorrectly.
         */
         if (( cause == NOERR ) || ( cause == MANDATORY_IE_MISSING ))
            {
            if ( chan )
               CCBNET3_SetNewChan( chan, CHANNEL_BUSY, TRUE );
            /* The B_Channel is probably connected already...just be sure.
            */
             /* inform the HOST.
             */
             if ( CCBRI_GetOutInfo( HOST_PROCEEDING ) )
                {
                /* total length of private IE's = 4.
                */
                OutPkgInfo[ OutPkgInfoLen++ ] = 4;
                OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
                OutPkgInfo[ OutPkgInfoLen++ ] = 2;
                OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;     /* exclusive */
                OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
                CCBRI_MailtoHost( CCB->CopyPublicMsg );
                }
            CCB->State = NET3_ROUTING;
            }
         else
            {
            /* Element coding error.
            */
            CCBNET3_RejectCall( cause );
            CCBNET3_ReleaseCall();
            result_code = CCBNET3_CauseToResult( cause );
            }
         break;

      case CC_DISCONNECT_IND:
         if ( CCB->B_Chan )
            CCBNET3_ReleaseChan();
         CCBNET3_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBNET3_process_releaseind();
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBNET3_ReleaseChan();
            }
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBNET3_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_PROGRESS_IND:
         /* just allow the message to come in, don't change state
         *  or do anything else. Added this state ===
         */
         break;

      case CC_ALERTING_IND:
         /* Notification of far end alerting.
         *  The B-Channel should already be connected.
         * Inform the HOST. Added this state ===
         */
         if ( CCBRI_GetOutInfo( HOST_ALERTING ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         break;

      case CC_SETUP_CONF:
         /* We may have an indication of a timeout of T303.  We will
         *  study the channel ID info element to determine this. 
         *  Added this state ===
         */
         IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                           CHANNEL_ID_IE, CODESET_0, 1 );
         if ( IE )
            {
            /* Go find the info channel selection field.  An indication of
            *  "No Channel" is a Protocol Control indication of call setup
            *  failure.
            */
            IE = CCIE_ExpandChanId_IE( IE, &Trashcan );
            if ( IE[ 4 ] == 0 )               /* Test for "No Channel" */
               {
               /* SETUP has timed-out (T303) to NETWORK.  The SETUP failed.
               */
               if ( CCB->B_Chan )
                  CCBNET3_ReleaseChan();

			   /* Send a RELEASE.  Apparantly violate 5.2.1.3(4) === */
/* === */	   if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE )) {
/* === */			/* Build Cause info element (optional in this message) */
/* === */			Diag[ 0 ] = 0;
/* === */			OutPkt->InfoLen +=
/* === */				CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
/* === */					NORMAL_CLEARING, FALSE, Diag );
/* === */			CCBRI_MailtoL3();
/* === */	   }

               if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
                  {
                  /* Total length of private IE's = 0
                  */
                  OutPkgInfo[ OutPkgInfoLen++ ] = 0;
                  CCBRI_MailtoHost( FALSE );
                  CCBNET3_ReleaseCall();
                  }
               }
            }

         /* Inform the HOST so can properly connect to
          * the B-channel
          */
         if (( *event == CC_SETUP_CONF ) &&
             ( CCBRI_GetOutInfo( HOST_CONNECT )))
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
         CCB->State = NET3_CONVERSATION;

         break;

      case CC_INFO_IND:
         /*
          * Allow this message, added for Singapore. We do not do
          * Overlap sending/receiving, but Singapore still tests it.
          */
         if (CCBRI_GetOutInfo(HOST_INFORMATION)) {
             OutPkgInfo[OutPkgInfoLen++] = 0;
             CCBRI_MailtoHost(CCB->CopyPublicMsg);
         }
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}
/*****************
* CCBNET3_CallIdle *
******************
*
* Process the current event in the context of a voice call in the
* IDLE state. State 0.
*
*/
PRIVATE int near CCBNET3_CallIdle( event )

int *event;                                /* Event input to the state */

{
   int result_code;
   int cause;

   result_code = NOERR;
   switch ( *event )
      {
      case HOST_CALL_REQUEST:
         if ( ( cause = CCBRI_HostCallRequest() ) == NOERR )
            {
            CCB->State = NET3_ORIGINATE;
            }
         else
            {
            if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
               {
               /* Total length of private IE's = 3.
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 3;
               OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;
               OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) cause;
               CCBRI_MailtoHost( FALSE );
               }
            CCBNET3_ReleaseCall();
            }
         break;

      case HOST_DISCONNECT:
         /* Should not happen - if it does, acknowledge and free the CCB.
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ))
            {
            /* Total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( FALSE );
            }
         CCBNET3_ReleaseCall();
         break;

      case HOST_DISCONNECT_ACK:
         /* Should not happen - if it does, free the CCB.
         */
         CCBNET3_ReleaseCall();
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case CC_SETUP_IND:
         result_code = CCBNET3_SetupInd();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         if ( CCB )
            CCBNET3_ReleaseCall();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         CCBNET3_ReleaseCall();
         break;
      }

   return( result_code );
}

/********************
* CCBNET3_CallOffered *
*********************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to State 6 of Q.931.
*
* Note that the HOST must accept the network's channel choice.
*/
PRIVATE int near CCBNET3_CallOffered( event )

int *event;                                /* Event input to the state */
{
   BYTE cause;
   int  result_code;

   result_code = NOERR;

   switch ( *event )
      {
      case HOST_PROCEEDING:
        /* Indicate that the Host is processing the call
         */
         if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
            {
            CCBRI_MailtoL3();                /* Send CC_BROADCAST_RESP */
            }
         if ( CCBRI_GetOutPkt( CC_CALL_PROC_REQ, FALSE ))
            {
            CCBRI_MailtoL3();
            CCB->State = NET3_ROUTING;
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
            CCBRI_MailtoL3();
            }
         CCB->State = NET3_ALERT;
         break;

      case HOST_CONNECT:
         if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
            {
            CCBRI_MailtoL3();                /* Send CC_BROADCAST_RESP */
            }
         if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ) )
            {
            CCBRI_MailtoL3();                     /* Send the mail now */
            }
         CCB->State = NET3_ROUTING;        /* Wait for CC_SETUP_COMP_IND */
         break;

      case HOST_DISCONNECT:
         /* Acknowledge the disconnect and free the CCB.
         */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( FALSE );
            if ( ( IE = CCBRI_GetHost_IE ( InPkgInfo, CAUSE_IE ) ) != NULL)
               {
               cause = IE[ 2 ];
               }
            else
               {
               cause = CALL_REJECTED;                      /* Cause 21 */
               }
            CCBNET3_RejectSetup( cause );
            }
         CCBNET3_ReleaseCall();
         break;

      case HOST_DISCONNECT_ACK:
         /* Should not happen - if it does, free the CCB.
         */
         CCBNET3_ReleaseCall();
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      case CC_DISCONNECT_IND:             /* Hang-up by the held party */
         if ( CCB->B_Chan )
			CCBNET3_ReleaseChan();
         CCBNET3_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBNET3_process_releaseind();
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCBRI_AcceptCause( &cause ) != NOERR )
            cause = NORMAL_CLEARING;

         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            OutPkgInfo[ OutPkgInfoLen++ ] = 3;    /* Private IE length */
            OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
            OutPkgInfo[ OutPkgInfoLen++ ] = 1;
            OutPkgInfo[ OutPkgInfoLen++ ] = cause;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBNET3_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

/**********************
* CCBNET3_CallOriginate *
***********************
*
* Process the current event in the context of a voice call in the
* ORIGINATE state awaiting the first response to CC_SETUP_REQ.
*
*/
PRIVATE int near CCBNET3_CallOriginate( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   BYTE cause;
   BYTE chan;
   result_code = NOERR;
   switch ( *event )
      {
      case CC_MORE_INFO_IND:
         /* This is the first response to SETUP request message sent
         *  in IDLE state.  This message will cause change of state
         *  to DIALING state.  DIAL TONE will be sent by the NETWORK
         *  on the selected B-Channel.  We will connect to it.
         *
         *  NOTE: this is a SETUP_ACK message from the peer.  The channel ID
         *  info element is a mandatory info element here. Check for it first:
         */
         if (( cause = CCBRI_AcceptChanId( &chan, &ByteTrash )) != NOERR )
            {
            CCBNET3_RejectCall( cause );
            CCBNET3_ReleaseCall();
            result_code = CCBNET3_CauseToResult( cause );
            }
         else
            {
            if ( chan )
               CCBNET3_SetNewChan( chan, CHANNEL_BUSY, TRUE );
            if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
               {
               /* total length of private IE's = 0
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 0;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               CCB->State = NET3_DIAL;
               }
            }
         break;

      case CC_ALERTING_IND:
         /*
          * Notification of far end alerting.
          * Added for the French deltas. Inform the HOST of both the Alerting
          * and the Info. The Info processes the B-channel information.
          */
         if ((cause = CCBRI_AcceptChanId(&chan, &ByteTrash)) != NOERR) {
            CCBNET3_RejectCall(cause);
            CCBNET3_ReleaseCall();
            result_code = CCBNET3_CauseToResult(cause);
         } else {
            if (chan)
               CCBNET3_SetNewChan(chan, CHANNEL_BUSY, TRUE);
            if (CCBRI_GetOutInfo( HOST_ALERTING)) {
               /* total length of private IE's = 0.
                */
               OutPkgInfo[OutPkgInfoLen++] = 0;
               CCBRI_MailtoHost(CCB->CopyPublicMsg);
               CCB->State = NET3_DIAL;
            }
            if (CCBRI_GetOutInfo(HOST_INFORMATION)) {
               /* total length of private IE's = 0
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 0;
               CCBRI_MailtoHost(CCB->CopyPublicMsg);
            }
         }
         break;

      case CC_CALL_PROC_IND:
         /* This message indicates that dialing (en_bloc) is complete.
         */
         if (( cause = CCBRI_AcceptChanId( &chan, &ByteTrash )) != NOERR )
            {
            CCBNET3_RejectCall( cause );
            CCBNET3_ReleaseCall();
            result_code = CCBNET3_CauseToResult( cause );
            }
         else
            {
            /* The B_Channel may be connected already...just be sure.
            */
            if ( chan )
               CCBNET3_SetNewChan( chan, CHANNEL_BUSY, TRUE );
            /* Inform the HOST.
            */
            if ( CCBRI_GetOutInfo( HOST_PROCEEDING ) )
               {
               /* Total length of private IE's = 4.
                */
               OutPkgInfo[ OutPkgInfoLen++ ] = 4;
               OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 2;
               OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;      /* exclusive */
               OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
            }
         CCB->State = NET3_ROUTING;
         break;

      case CC_SETUP_CONF:
         /*
          * We may have an indication of a timeout of T303.  We will
          * study the channel ID info element to determine this.
          */
         IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
                           CHANNEL_ID_IE, CODESET_0, 1);
         if (IE) {
            /* Go find the info channel selection field.  An indication of
             * "No Channel" is a Protocol Control indication of call setup
             * failure.
             */
            IE = CCIE_ExpandChanId_IE(IE, &Trashcan);
            if (IE[4] == 0) {             /* Test for "No Channel" */
               /* SETUP has timed-out (T303) to NETWORK.  The SETUP failed.
                */
               if (CCB->B_Chan)
                  CCBNET3_ReleaseChan();

               /* Send a RELEASE.  Apparantly violate 5.2.1.3(4) === */
               if (CCBRI_GetOutPkt(CC_RELEASE_REQ, FALSE)) {
                  /* Build Cause info element (optional in this message) */
                  Diag[0] = 0;
                  OutPkt->InfoLen +=
                          CCIE_BuildCause_IE(&OutPkt->Info[OutPkt->InfoLen],
                          NORMAL_CLEARING, FALSE, Diag);
                  CCBRI_MailtoL3();
               }

               if (CCBRI_GetOutInfo(HOST_DISCONNECT_ACK)) {
                  /* Total length of private IE's = 0
                   */
                  OutPkgInfo[OutPkgInfoLen++] = 0;
                  CCBRI_MailtoHost(FALSE);
                  CCBNET3_ReleaseCall();
               }
            } else {
               /*
                * There is a B-channel. New for this state, added for
                * the French net3 deltas.
                */
               if (chan)
                  CCBNET3_SetNewChan(chan, CHANNEL_BUSY, TRUE);
               /* Inform the HOST.
                */
               if (CCBRI_GetOutInfo(HOST_CONNECT)) {
                   /* total length of private IE's = 4.  */
                   OutPkgInfo[OutPkgInfoLen++] = 4;
                   OutPkgInfo[OutPkgInfoLen++] = CHANNEL_ID_IE;
                   OutPkgInfo[OutPkgInfoLen++] = 2;
                   OutPkgInfo[OutPkgInfoLen++] = 0x01;         /* exclusive */
                   OutPkgInfo[OutPkgInfoLen++] = (BYTE) CCB->B_Chan;
                   CCBRI_MailtoHost(CCB->CopyPublicMsg);
               }
               CCB->State = NET3_CONVERSATION;
            }
         }
         break;

      case HOST_DISCONNECT:
         /* Local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NET3_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         /* Local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = NET3_RELEASE;
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case CC_DISCONNECT_IND:
         /* Far end disconnect.  Send CC_RELEASE_REQ and go to the
         *  state NET3_RELEASE.
         */
         if ( CCB->B_Chan )
            CCBNET3_ReleaseChan();
         CCBNET3_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBNET3_process_releaseind();
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBNET3_ReleaseChan();
            }
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBNET3_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

/********************
* CCBNET3_CallRelease *
*********************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The HOST
* is already cleared for this call if there is a HOST.
*
*/
PRIVATE int near CCBNET3_CallRelease( event )

int *event;                                /* Event input to the state */

{
   int result_code;


   result_code = NOERR;

   switch ( *event )
      {
      case CC_RELEASE_IND:
		CCBNET3_process_releaseind();
		/* fall through... */
      case CC_RELEASE_CONF:
         CCBNET3_ReleaseCall();
         break;

      case CC_DISCONNECT_IND:
         /* We simply await CC_RELEASE_CONF to follow this message.
         */
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case HOST_DISCONNECT_ACK:
      case HOST_DISCONNECT:
         /* Local disconnect or disconnect ack from HOST.
         *  This is an apparent mutual disconnect race condition.
         *  We will simply ignore it.
         */
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

/***********************
* CCBNET3_CallReleaseAck *
************************
*
* Process the current event in the context of a voice call in the
* RELEASING_ACK (waiting for CC_RELEASE_CONF) state.  The local user
* is to have the release acknowledged via HOST_DISCONNECT_ACK message
* in this case.
*
*/
PRIVATE int near CCBNET3_CallReleaseAck( event )

int *event;                                /* Event input to the state */
{
   int result_code;


   result_code = NOERR;

   switch ( *event )
      {
	  case CC_DISCONNECT_IND:		/* === */
		/* we had a DISCONNECT collision === */
		break;						/* === */

      case CC_RELEASE_IND:
		CCBNET3_process_releaseind();
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* Total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBNET3_ReleaseCall();
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case HOST_DISCONNECT:
         /* Local disconnect from HOST.  This is an apparent mutual
         *  disconnect race condition.  We will simply ignore it.
         */
         break;

      case HOST_DISCONNECT_ACK:
         /* Local disconnect ack from HOST.  This is an unexpected message.
         *  We will simply change states since the HOST is cleared.
         */
         CCB->State = NET3_RELEASE;
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;                                 /* No change in state. */

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

/********************
* CCBNET3_CallRouting *
*********************
*
* Process the current event in the context of a voice call in the
* routing state, the general purpose busy, but not CONVERSATION, state.
* State 8.
*/
PRIVATE int near CCBNET3_CallRouting( event )

int *event;                                /* Event input to the state */

{
   int  result_code;

   result_code = NOERR;

   switch ( *event )
      {
      case HOST_ALERTING:
         /* Indication that the Host has initiated alerting for the call
         */
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
            {
            CCBRI_MailtoL3();
            }
         CCB->State = NET3_ALERT;
         break;

      case HOST_INFORMATION:     /* Process the information from HOST. */
         CCBRI_HostInfo();                      /* No change in state. */
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = NET3_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = NET3_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      case CC_ALERTING_IND:
         /* Notification of far end alerting.
         *  The B-Channel should already be connected.
         */
         /* Inform the HOST.
         */
         if ( CCBRI_GetOutInfo( HOST_ALERTING ) )
            {
            /* total length of private IE's = 0.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         break;

      case CC_SETUP_COMP_IND:
        /*  NOTE: the T313 Timeout is also handled here ===*/
        if(InPkt->Info[0] != CONNECT_ACK ) {
            CCB->State = NET3_RELEASE_ACK;
            break;
        }
         /* fall thru */
      case CC_SETUP_CONF:
         /* The call was connected at the far end.  Connect the call
          *  at this end too (its probably already connected) and the
          *  call will transfered to state CONNECTED.
          *
          * NOTE: these messages correspond to CONNECT_ACK and CONNECT
          *  messages (respectivelly) and we should not have mandatory info
          *  elements to examine here.
          */

         if(!CCB->B_Chan) {    /* only check if mandatory */
             /* check for poorly formatted ChanID IE.  === */
             BOOLEAN struc_err;

             if((IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen, CHANNEL_ID_IE,
                                   CODESET_0, 1)) != NULL) {
                  IE_Fields = CCIE_ExpandChanId_IE(IE, &struc_err);
                  if(struc_err) {
                      char diag[2];
                      if (CCBRI_GetOutPkt(CC_REL_COMP_REQ, FALSE)) {
                          diag[ 0 ] = 0;
                          OutPkt->InfoLen +=
                              CCIE_BuildCause_IE(&OutPkt->Info[OutPkt->InfoLen],
                                                 INVALID_ELEM_CONTENTS, FALSE, diag);
                          CCBRI_MailtoL3();
                      }
                  }
              }
          }

          /* Inform the HOST.
           */
          if (( *event == CC_SETUP_CONF ) &&
              ( CCBRI_GetOutInfo( HOST_CONNECT ))) {
              /* total length of private IE's = 4.  */
              OutPkgInfo[ OutPkgInfoLen++ ] = 4;
              OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
              OutPkgInfo[ OutPkgInfoLen++ ] = 2;
              OutPkgInfo[ OutPkgInfoLen++ ] = 0x01;         /* exclusive */
              OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) CCB->B_Chan;
              CCBRI_MailtoHost( CCB->CopyPublicMsg );
          }
          CCB->State = NET3_CONVERSATION;
          break;

      case CC_DISCONNECT_IND:
         /* Far end disconnect.
         *  state NET3_RELEASE.
         */
		 if ( CCB->B_Chan )
			CCBNET3_ReleaseChan();
         CCBNET3_DisconnectInd();	/*===*/
         break;

      case CC_PROGRESS_IND:
         /* CAUSE is a mandatory info element here - check
         *  for it first:
         */
		 /* Cause is not mandatory === */
         break;

      case CC_RELEASE_IND:
		CCBNET3_process_releaseind();
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBNET3_ReleaseChan();
            }
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBNET3_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_INFO_IND:
         /*
          * ignore this message. Added for italy homologation.
          * TC10304
          */
         if (CCBRI_GetOutInfo(HOST_INFORMATION)) {
            OutPkgInfo[OutPkgInfoLen++] = 0;
            CCBRI_MailtoHost(CCB->CopyPublicMsg);
         }
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBNET3_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

/************************************************************************
*                     O T H E R   F U N C T I O N S                     *
*************************************************************************
*/
/**********************
* CCBNET3_CauseToResult *
***********************
*
* Convert a CCITT cause value into an error result code.
* Note that all legal causes are not supported, only those generated.
*/
PRIVATE int near CCBNET3_CauseToResult( cause )

BYTE cause;

{
   int result_code;


   switch ( cause )
      {
      case CAP_NOT_IMPLEMENTED:                            /* Cause 65 */
         result_code = BAD_BEARER_CAP;
         break;
      case INVALID_ELEM_CONTENTS:                         /* Cause 100 */
         result_code = INVALID_INFO_ELEMENT;
         break;
      case MANDATORY_IE_MISSING:                           /* Cause 96 */
         result_code = MISSING_MANDATORY_IE;
         break;
      case MANDATORY_IE_LEN_ERR:                          /* Cause 103 */
         result_code = MAND_IE_LEN_ERR;
         break;
      case CALL_REJECTED:                                  /* Cause 21 */
         result_code = BAD_CALLED_ADDR;
         break;
      case NO_CIRCUIT_AVAILABLE:                           /* Cause 34 */
         result_code = UNACCEPTABLE_CHANNEL;
         break;
      default:
         result_code = NOERR;
         break;
      }

   return ( result_code );
}

/************************
* CCBNET3_DecodeCallState *
*************************
*
* Decode the state of the CCB from the CCB state variable.
* Dispatch the appropriate state function through the state function
* jump table.
*
*/
PUBLIC int CCBNET3_DecodeCallState( event )

int *event;                                /* Event input to the state */

{
   int result_code;

	if(*event == CC_RELEASE_IND_QUIET) {	/* === */
		quiet_release = TRUE;				/* === */
		*event = CC_RELEASE_IND;			/* === */
	} else									/* === */
		quiet_release = FALSE;				/* === */

   /* Perform a range check on the value of CCB->State to prevent
   *  the strange and unusual.
   */
   if (( CCB->State > NET3_STATE_LOW_LIMIT ) &&
       ( CCB->State < NET3_STATE_HIGH_LIMIT ))
      {
      /* Enter state function.
      */
      result_code = ( *Ccb_State_Func_Net3[ CCB->State ] )( event );
      }
   else
      {
      CCBRI_Error( CCBRI_DECODE_CALL_STATE, INVALID_STATE, CCB->State,
                   *event, TRUE );
      result_code = INVALID_STATE;
      }

   return( result_code );
}

/************************
* CCBNET3_DisconnectInd *
*************************
*
* Generic action upon receiving the CC_DISCONNECT_IND message.
*
*/
PRIVATE BOOLEAN near CCBNET3_DisconnectInd ()
{
   BYTE cause;                             /* Locally-detected error cause */
   BYTE host_event;                        /* Event to be sent to HOST */
   BYTE valid_ie;                          /* for Italy, TC11019 */

   cause = 0;
   host_event = HOST_DISCONNECT;
   valid_ie = 0;
   /*
    * Have to check for unimplemented IEs in the message. This was changed for
    * the Italy homologation. TC11018, TC11019
    */
   if ((valid_ie = CCBNET3_VerifyDiscIEs(InPkt->Info, InPkt->InfoLen)) != NOERR) {
       /*
        * check for comprehension required, can't be done in valid_ie(), so has
        * to be done here. For Italy, TC11018
        */
       if (valid_ie & 0xf0)
           cause = BAD_INFO_ELEM;        /* cause 99 */
       else
           cause = MANDATORY_IE_MISSING; /* cause 96 */
   } else {
       cause = CCBRI_AcceptCause(&cause);
       valid_ie = CAUSE_IE;
   }

   if (CCBRI_GetOutPkt(CC_RELEASE_REQ, FALSE)) {
       /*
        * add the cause value
        * for italy, TC1022
        */
       if (cause != NOERR) {
           Diag[0] = valid_ie;
           Diag[1] = 0;
           OutPkt->InfoLen +=
           CCIE_BuildCause_IE(&OutPkt->Info[OutPkt->InfoLen], cause, FALSE, Diag);
       }
       CCBRI_MailtoL3();
       CCB->State = NET3_RELEASE;
   }
   CCB->Cause  = (int)cause;
   CCBRI_DisconnectHostCall(&host_event, cause);

   return(TRUE);
}

/*******************
* CCBNET3_RejectCall *
********************
*
* Reject a call due to flaws or incompatibilities in the associated message
* received from the peer.  The function CCBNET3_RejectSetup is only used for
* rejecting the CC_SETUP_IND and this function is used otherwise.
*
*/
PRIVATE int near CCBNET3_RejectCall( cause )

BYTE cause;                                    /* Reason for rejection */

{
   int result_code;


   result_code = NOERR;

   /* We request L3 to send RELEASE_COMP directly.
   */
   if ( CCBRI_GetOutPkt( CC_REL_COMP_REQ, FALSE ))
      {
      /* Build the RELEASE_COMP message.
      */
      Diag[ 0 ] = 0;  /* Diagnostic is only relevant in STATUS message */
      OutPkt->InfoLen +=
         CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ], cause,
                             FALSE, Diag );
      CCBRI_MailtoL3();                        /* Send CC_REL_COMP_REQ */
      }
   else
      {
      result_code = OUT_OF_PKTS;
      }

   return ( result_code );
}

/********************
* CCBNET3_RejectSetup *
*********************
*
* This function is called as the first response to the CC_SETUP_IND
* when the call is being rejected.
*
*/
PRIVATE void near CCBNET3_RejectSetup( cause )

BYTE cause;                                      /* Cause for CAUSE_IE */

{
   if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
      {
      CCBRI_MailtoL3();                         /* Send BROADCAST_RESP */
      if ( CCBRI_GetOutPkt( CC_SETUP_REJ_REQ, FALSE ))
         {
         /* Build the message.
         */
         Diag[ 0 ] = 0;     /* Diag is only relevant in STATUS message */
         OutPkt->InfoLen +=
            CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ], cause,
                                FALSE, Diag );
         CCBRI_MailtoL3();                    /* Send CC_SETUP_REJ_REQ */
         }
      }
   return;
}

/********************
* CCBNET3_ReleaseCall *
*********************
*
* Terminate a call and put the CCB to bed.
*
*/
PUBLIC void CCBNET3_ReleaseCall()

{

   /* Perform actions specific to the voice call termination.
   */
   if ( CCB->B_Chan )
      CCBNET3_ReleaseChan();

   CCBRI_RemoveCCB( CCB );

   return;
}

/********************
* CCBNET3_ReleaseChan *
*********************
*
* Remove a B-Channel from allocation by a CCB.
*
*/
PRIVATE void near CCBNET3_ReleaseChan()

{
   if ( CC_CHAN_GetUserbri( CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID ) == CCB->Call_ID )
      {
      CC_CHAN_ReleaseChanbri( CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID );
      }
   CCB->B_Chan = 0;
}

/***********************
* CCBNET3_ReleasedLink *
************************
*
* The data link is down at Layer 2.
* Remove CCBs from any context but LOCKOUT.  Users left OFF HOOK
* must retain a CCB in the LOCKOUT state to process their hangup.
*
* It is specified that calls in the ACTIVE state should not be disturbed.
*
*/
PUBLIC void CCBNET3_ReleasedLink (ces, dsl_id)
BYTE ces;          /* Connection Endpoint Suffix of the released link. */
BYTE dsl_id;                                /* Digital Subscriber Loop */
{
   CCB_ID tmp_ccb;

   Dsl_ID = dsl_id; /* This assumption is made in CCBNET3_ReleaseCall(). */
   /*
    * Start at the head and remove all impacted CCBs.
    */
   CCB = CCB_Head[dsl_id];
   while (CCB) {
      tmp_ccb = CCB->Next; 
      if (CCB->Ces == ces) {
         if (CCB->State != NET3_CONVERSATION) {
            if (CCBRI_GetOutInfo(HOST_DISCONNECT_ACK)) {
               /*
                * total length of private IE's = 0
                */
               OutPkgInfo[OutPkgInfoLen++] = 0;
               CCBRI_MailtoHost(FALSE);
            }
            CCBNET3_ReleaseCall();
         }
      }
      CCB = tmp_ccb;    /* Skip to the next one. */
   }
}

/*******************
* CCBNET3_SetNewChan *
********************
*
* Establish a new B-Channel in the CCB.
*
*/
PRIVATE BYTE near CCBNET3_SetNewChan( b_chan, state, overwrite )

int     b_chan;                                    /* B-Channel to set */
int     state;                                /* New state for channel */
BOOLEAN overwrite; /* May a chan in the PROPOSED state be overwritten? */
{
   BYTE   cause;
   CCB_ID ccb;


   cause = CCBRI_SetNewChan( b_chan, state, overwrite );
   if ( !CCB->B_Chan )
      {
      cause = USER_BUSY;                                   /* Cause 17 */
      if ( b_chan == NO_CHANNEL )
         {
         ccb = Hcb[ Dsl_ID ].Ccb_In_Use[ Hcb[ Dsl_ID ].Active_Call ];
         if (( !ccb ) || ( !ccb->B_Chan ))
            /* Another call is not active or no B-channel.
            */
            cause = NO_CIRCUIT_AVAILABLE;                  /* Cause 34 */
         }
      }

   return( cause );
}


/*****************
* CCBNET3_SetupInd *
******************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function because
* of its inherent complexity.  This process is complicated by the
* possible presence and interraction of the Call Waiting and Call
* Deflection and UUI Supplementary Services where they are activated.
*
*/
PRIVATE int near CCBNET3_SetupInd()

{
   BYTE   cause;
   BYTE   chan;
   int    length = 0;
   BYTE   exclusive = 0;
   int    result_code = NOERR;
   int    k;

   /* A new call SETUP from the far end. The CC may send
   *  ALERTING request to the network instructing it that
   *  the phone is ringing.  This will be done only after
   *  unpacking and checking of info elements in the CC_SETUP_IND
   *  message, and upon consideration of any related Supplementary
   *  Services.
   *
   *  Before sending of ALERTING message, the CC must always mail
   *  CC_BROADCAST_RESP message.  This will assure that the Layer 3
   *  software is in a proper state to receive ALERTING.
   */

   /* Prepare to inform the HOST.
   */
   if ( !CCBRI_GetOutInfo( HOST_INCOMING_CALL ) )
      {
      goto in_call_failed;
      }
   OutPkgInfoLen++;
   k = 5;

   /* Look at the bearer capability IE.
   */
   if (( cause = CCBRI_AcceptBearerCap()) != NOERR ) {
      if ((cause == CAP_NOT_IMPLEMENTED)   ||            /* Cause 65 */
          (cause == MANDATORY_IE_LEN_ERR)) {            /* Cause 103 */
         /*
          * changes put in for italy homologation, TC10026
          *
          * We do not respond in this case.
          */
         CCBNET3_ReleaseCall();
         result_code = CCBNET3_CauseToResult( cause );
         goto in_call_failed;
      } else {
         /* This is a mandatory element.  The call must be rejected.
         */
         CCBNET3_RejectSetup( cause );
         CCBNET3_ReleaseCall();
         result_code = CCBNET3_CauseToResult( cause );
         goto in_call_failed;
      }
   }

   /* Next we look at the Channel ID.
   */
   if (( cause = CCBRI_AcceptChanId( &chan, &exclusive )) != NOERR )
      {
      /*
       * This is a mandatory element.  The call must be rejected.
       * italy homologation, TC10026
       */
      CCBNET3_RejectSetup( cause );
      CCBNET3_ReleaseCall();
      result_code = CCBNET3_CauseToResult( cause );
      goto in_call_failed;
      }
	
   if ( chan )
      {
      CCBNET3_SetNewChan( chan, CHANNEL_BUSY, TRUE );
      OutPkgInfo[ OutPkgInfoLen++ ] = CHANNEL_ID_IE;
      OutPkgInfo[ OutPkgInfoLen++ ] = 2;
      OutPkgInfo[ OutPkgInfoLen++ ] = exclusive;
      OutPkgInfo[ OutPkgInfoLen++ ] = chan;
      k += 4;
      }
   else
      {
      /* Without the possibility in COMMAND_INTERFACE for a call on hold at
      * the present time, the network should not offer "NO_CHANNEL."
	  *
	  *		Except that NO_CHANNEL is defined for the Chan ID info element,
	  *	not Cause.  They want NO_CIRCUIT_AVAILABLE, so I changed it.  ===
      */
      CCBRI_RejectCall(NO_CIRCUIT_AVAILABLE, 0);	/* see comment above === */
      CCBNET3_ReleaseCall();
      result_code = CCBNET3_CauseToResult( cause );
      goto in_call_failed;
      }

   /* Next attempt to match the called address to our own or to recover it
   *  for the Host.
   */
#ifdef 0		/*===*/
   if (( cause = CCBRI_AcceptCalledNumb( &length, Dsl_ID )) != NOERR )
      {
      /* This is an optional element, but if present, the number must
      *  match the terminal's number and the element must be coded correctly.
      *  Otherwise ignore the call.
      */
      if ( cause != MANDATORY_IE_MISSING )     /* Element coding error */
         {
         CCBNET3_ReleaseCall();
         result_code = CCBVN_CauseToResult( cause );
         goto in_call_failed;
         }
      }
#endif

   /* NOTE: Here tests may be inserted for checking the optional
   *  Lower Layer and Higher Layer compatability information elements
   *  and any other elements of concern.
   */

   k += length;
   OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
   CCBRI_MailtoHost( CCB->CopyPublicMsg );
   CCB->State = NET3_OFFERED;

in_call_failed:
   return ( result_code );
}

/*******************
* CCBNET3_Shutdown *
********************
*
* Reset Call Control
*
*/
PUBLIC void CCBNET3_Shutdown (dsl)
BYTE dsl;
{
    CCB_ID temp_ccb;

    Dsl_ID = dsl;
    CCB = CCB_Head[dsl];
    while (CCB) {
        temp_ccb = CCB->Next;
        CCBNET3_ReleaseCall();
        CCB = temp_ccb;
    }
}

/**********************
* CCBNET3_UnexpectedMsg *
***********************
*
* Unexpected message in a Call Control state from L3 Protocol Control.
*
* Note that L3 logic is designed to catch these cases in general
* before we receive them, but the inopportune arrival of some
* messages may not be detected in L3 because they may not be
* inopportune for all applications.
* In the call to CCBRI_RejectMsg, the Cause diagnostic is
* generated from the Protocol Discriminator byte of the Info
* which was overwritten by Layer 3 with the msg_type it received.
* This value is passed to this function from the Call Control states.
*
*/
PRIVATE void near CCBNET3_UnexpectedMsg( diagnostic )

BYTE diagnostic;                    /* The message type received by L3 */
{
   CCBRI_RejectMsg( WRONG_MESSAGE, diagnostic );
   /* Other action(s) appropriate to the specific application may be added
   *  here.
   */
   return;
}

/***********************
* CCBNET3VerifyDiscIEs *
************************
*
* Verifies what information elements are legal in a Disconnect message.
* This function returns 0 if all info elements were properly recognized.
* Otherwise, it returns the IE_ID of the FIRST unrecognized info element
* in the info buffer. This routine was added to pass the italy homologation.
*/
PRIVATE BYTE CCBNET3_VerifyDiscIEs (info_buffer, info_length)

BYTE    *info_buffer;
int      info_length;
{
   BOOLEAN recognized;
   BYTE    current_ie;
   int     current_offset;        /* pointer to a current info element */

   recognized = TRUE;
   current_ie = 0;
   current_offset  = CCIE_LenOf_L3_Msg_Header( info_buffer );

   while (recognized && (current_offset < info_length)) {
      current_ie = info_buffer[ current_offset ];
      switch (current_ie) {
        case CAUSE_IE:
        case PROGRESS_IND_IE:
        case DISPLAY_IE:
            break;
        default:
            recognized = FALSE;
            break;
      }

      if (current_ie & 0x80) {
          /* single octet */
          current_offset++;
      } else {
          /* variable length */
          current_offset++;
          current_offset += info_buffer[ current_offset ] + 1;
      }
      /*
       * if found an unrecognized ie, then want to return that
       * value. so don't update to the next one.
       */
      if (recognized)
         current_ie = info_buffer[ current_offset ];
   }

   return( recognized ? 0 : current_ie );
}
#endif
