/* $Id: ccbvn.c,v 3.3.12.2 1996/09/13 04:54:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccbvn.c,v $
 *------------------------------------------------------------------
 * ccbvn.c  ISDN French VNX BRI Call Control Functions
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccbvn.c,v $
 * Revision 3.3.12.2  1996/09/13  04:54:38  thille
 * CSCdi68927:  Clean up some things that should be const in ISDN
 * Branch: California_branch
 * Make some more stuff const to save ram.  Also make some more stuff
 * static.
 *
 * Revision 3.3.12.1  1996/04/30  23:06:59  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/15  17:27:25  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.2  1995/11/17  17:39:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccbvn.c
VERSION: 3.21
DESCRIPTION: ISDN French VNX BRI Call Control Functions

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

The current version of this module is intended to implement the
Call Control functions for a simple user-side Basic Rate Interface
(Basic Rate Access) terminal in a manner compatible with a French VN3
Central Office switch in accordance with France Telecom Technical
Specification ST/LAA/RSM/144, Edition 1, Oct. 1988.  The terminal is
assumed to be implemented using subscribed in-band call progress tones
and to send address information via overlapped sending or en-bloc.
A simple keypad interface and a multi-byte message interface from the
user-side application are provided.

OTHER ASSUMPTIONS AND REMARKS PERTAINING TO THE MODULE

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
    compatibility checking delineated in the VN3 ISDN Specification.
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
7.  The entry point to the module is CCBVN_Go.
8.  Throughout the call state functions of the module, when in keypad mode,
    function calls are made to LIF_SendSBC(), sending state updates to
    TRACER_ID inside of the #ifdef SEND_TR_MSG compile flag.  These are
    state updates designed to support the Telenetworks TRACKER.C module in
    its display updates which may be used in test environments with this
    module as a substitute for real application hardware.

#ifdef COMMAND_INTERFACE
This version does not support supplementary services using the multi-byte
message interface (COMMAND_INTERFACE).  The module is capable of managing
multiple concurrent and independent calls in this mode.

#endif
#ifdef SBC_INTERFACE
SUPPLEMENTARY SERVICES (GENERAL)

This version supports several supplementary services using the keystroke
interface when those services have been purchased as an option from
Telenetworks.

The number of calls which may be handled is a compile-time option
(MAX_VOICE_CALLS).  The number of CCBs (Call Control Blocks) should
equal MAX_VOICE_CALLS + 1.  The number of Layer 3 NLCB's should equal
MAX_VOICE_CALLS + 2.  Only one call is permitted in the ALERT state at
one time.  The capability for "en-bloc" digit transmission has also been
implemented.

Some of the above-mentioned functions are not available until certain
Supplementary Services are activated, which are not supported in the
first version of this module, specifically multiple calls.

In order to activate various of the features defined below it is
assumed that certain "feature activation" keys are present on the
terminal.  It may be that in the final implementation of the product
these functions will be activated by some other stimulus such as a
configurable database in a PROM or they may be performed by default.
Since this detail is not known by this author, the "feature key"
approach has been implemented.

SPECIFIC SERVICES PROVIDED IN THIS VERSION

#endif
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

/* ISDN includes
*/
#include    "flags.h"
#ifdef VN2	/* === */
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
*                references in the Ccb_State_Func table further on must
*                agree in number and position.
*/
enum CCB_State
   {
   VN_STATE_LOW_LIMIT,                               /* Sizing element */
   VN_IDLE,                                                /* CCB idle */
   VN_ALERT,                                          /* Being alerted */
   VN_CONVERSATION,                                    /* Conversation */
   VN_DIAL,                                                 /* Dialing */
#ifdef COMMAND_INTERFACE
   VN_DISC_HOST,                                /* Await HOST DISC_ACK */
#endif
#ifdef SBC_INTERFACE
   VN_FACILITY_REQ,              /* Requesting a Supplementary Service */
   VN_LOCKOUT,                            /* Disconnected but off hook */
#endif
#ifdef COMMAND_INTERFACE
   VN_OFFERED,                 /* Await host response to incoming call */
#endif
#ifdef SBC_INTERFACE
   VN_ONHOOK_DIAL,                        /* Dialing while not engaged */
#endif
   VN_ORIGINATE,                     /* Await response to CC_SETUP_REQ */
#ifdef SBC_INTERFACE
   VN_RECONNECT,            /* Reconnecting held or call waiting party */
#endif
   VN_RELEASE,                          /* Waiting for release confirm */
#ifdef COMMAND_INTERFACE
   VN_RELEASE_ACK,        /* Waiting for release confirm (ack to HOST) */
#endif
#ifdef SBC_INTERFACE
#endif
   VN_ROUTING,        /* Busy, but neither dialing nor in conversation */
#ifdef SBC_INTERFACE
#endif
   VN_STATE_HIGH_LIMIT                               /* Sizing element */
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
PRIVATE int      near CCBVN_CallAlerting     (int *);
PRIVATE int      near CCBVN_CallConversation (int *);
PRIVATE int      near CCBVN_CallDialing      (int *);
#ifdef COMMAND_INTERFACE
PRIVATE int      near CCBVN_CallDiscHost     (int *);
#endif
#ifdef SBC_INTERFACE
PRIVATE int      near CCBVN_CallFacility     (int *);
#endif
PRIVATE int      near CCBVN_CallIdle         (int *);
#ifdef SBC_INTERFACE
PRIVATE int      near CCBVN_CallLockout      (int *);
#endif
#ifdef COMMAND_INTERFACE
PRIVATE int      near CCBVN_CallOffered      (int *);
#endif
#ifdef SBC_INTERFACE
PRIVATE int      near CCBVN_CallOnhookDial   (int *);
#endif
PRIVATE int      near CCBVN_CallOriginate    (int *);
#ifdef SBC_INTERFACE
PRIVATE int      near CCBVN_CallReconnecting (int *);
#endif
PRIVATE int      near CCBVN_CallRelease      (int *);
#ifdef COMMAND_INTERFACE
PRIVATE int      near CCBVN_CallReleaseAck   (int *);
#endif
#ifdef SBC_INTERFACE
#endif
PRIVATE int      near CCBVN_CallRouting      (int *);
#ifdef SBC_INTERFACE
#endif
PRIVATE int      near CCBVN_CauseToResult    (BYTE);
PRIVATE int      near CCBVN_DisconnectInd    (void);
#ifdef SBC_INTERFACE
PRIVATE int      near CCBVN_GetCallIndex     (int);
PRIVATE BOOLEAN  near CCBVN_GetHeldChan      (void);
PRIVATE int      near CCBVN_PassCall         (void);
#endif
PRIVATE int      near CCBVN_RejectCall       (BYTE);
PRIVATE void     near CCBVN_RejectSetup      (BYTE);
PRIVATE void     near CCBVN_ReleaseChan      (void);
#ifdef SBC_INTERFACE
#endif
PRIVATE BYTE     near CCBVN_SetNewChan       (int, int, BOOLEAN);
PRIVATE int      near CCBVN_SetupInd         (void);
#ifdef SBC_INTERFACE
PRIVATE int      near CCBVN_StandardHangup   (BYTE);
#endif
PRIVATE void     near CCBVN_UnexpectedMsg    (BYTE);

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
/* State Function Jump Table
*/
typedef int (near *CC_STATES)( int * );
static const CC_STATES Ccb_State_Func[ VN_STATE_HIGH_LIMIT ] =
   {
   NULL
   ,
   CCBVN_CallIdle
   ,
   CCBVN_CallAlerting
   ,
   CCBVN_CallConversation
   ,
   CCBVN_CallDialing
#ifdef COMMAND_INTERFACE
   ,
   CCBVN_CallDiscHost
#endif
#ifdef SBC_INTERFACE
   ,
   CCBVN_CallFacility
   ,
   CCBVN_CallLockout
#endif
#ifdef COMMAND_INTERFACE
   ,
   CCBVN_CallOffered
#endif
#ifdef SBC_INTERFACE
   ,
   CCBVN_CallOnhookDial
#endif
   ,
   CCBVN_CallOriginate
#ifdef SBC_INTERFACE
   ,
   CCBVN_CallReconnecting
#endif
   ,
   CCBVN_CallRelease
#ifdef COMMAND_INTERFACE
   ,
   CCBVN_CallReleaseAck
#endif
#ifdef SBC_INTERFACE
#endif
   ,
   CCBVN_CallRouting
#ifdef SBC_INTERFACE
#endif
   };

PRIVATE BYTE    Diag[ 2 ];     /* For passing cause diagnostic to CCIE */
#ifdef SBC_INTERFACE
PRIVATE char    FacilitySpec[ 20 ];   /* Facility specification buffer */
#endif
PRIVATE BYTE    ByteTrash;                              /* Byte bucket */

/*
 *	We always must send the REL_COMP.  Unless there was an error and
 *	layer 3 already sent it, in which case quiet release is set.
 *
 *	Entire routine written by rbk ===
 */
static void
CCBVN_process_releaseind (void)
{
	int cause = 0;		/* initialize to suppress compiler warning */

	if(quiet_release || (cause = CCBRI_AcceptCause((BYTE*)&Trashcan))) {
		/* It's bad.  Disconnect with a Release Complete */
		if(CCBRI_GetOutInfo(HOST_DISCONNECT_ACK)) {
			/* total length of private IE's = 0 */
			OutPkgInfo[ OutPkgInfoLen++ ] = 0;
			CCBRI_MailtoHost(CCB->CopyPublicMsg);
		}
		if(!quiet_release)
			CCBVN_RejectCall(cause);
		/* CCBVN_ReleaseCall(); === generating an error message */
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
* CCBVN_CallAlerting *
**********************
*
* Process the current event in the context of a voice call in the
* ALERT state.
*
*/
PRIVATE int near CCBVN_CallAlerting( event )

int *event;                                /* Event input to the state */

{
   int  result_code;

   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = VN_ROUTING;
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );    /* Process disc from HOST. */
         CCB->State = VN_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );/* Process disc ack from HOST. */
         CCB->State = VN_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

#endif
#ifdef SBC_INTERFACE
      case KP_OFF_HOOK:
         /* Local off hook - we will answer the incoming call
         *  by sending CC_SETUP_RESP (CONNECT) downwards.
         *  The response to this message (SETUP_COMP_IND) will be
         *  received in the VN_ROUTING state to complete the connection.
         */
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ) )
            {
            CCBRI_MailtoL3();
            }
         else
            {
            result_code = OUT_OF_PKTS;
            }
         Hcb[ Dsl_ID ].Hw_State = OFFHOOK;     /* No chng in CCB state */
         CCB->State = VN_ROUTING;
         CCBRI_TRACE( TR_ROUTING );
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_DISCONNECT_IND:
         /* Distant end disconnect (response to this message will
         *  be received in RELEASING state - see CC_RELEASE_CONF):
         */
#ifdef SBC_INTERFACE
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;  /* Not BELL any longer */
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_RELEASE;
            CCBRI_TRACE( TR_RELEASING );
            }
#else                                             /* COMMAND_INTERFACE */
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_DISC_HOST; /* Wait for HOST_DISCONNECT_ACK */
            }
#endif
         break;

      case CC_RELEASE_IND:
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:
#ifdef SBC_INTERFACE
#ifdef TASK_KB
         CCBRI_StopRing( VOICE_ALERTING );
#endif
#else                                             /* COMMAND_INTERFACE */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
#endif
         CCBVN_ReleaseCall();
         break;

#ifdef SBC_INTERFACE
#endif                                                /* SBC_INTERFACE */
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
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
* CCBVN_CallConversation *
**************************
*
* Process the current event in the context of a voice call in the
* CONVERSATION state.
*
*/
PRIVATE int near CCBVN_CallConversation( event )

int *event;                                /* Event input to the state */

{
   int     result_code;                /* Result code for the function */

#ifdef SBC_INTERFACE
   int     index;                                         /* Hcb index */
   CCB_ID  ccb;                                    /* Passive call CCB */
#endif

   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = VN_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = VN_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

#endif
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* Local disconnect (response to this message will be
         *  received in RELEASING state - see CC_RELEASE_IND):
         */
         result_code = CCBVN_StandardHangup( NORMAL_CLEARING );
         Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         CCB->State = VN_RELEASE;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_DISCONNECT_IND:
#ifdef 0
        /* don't do this, removed during cancun homologation feb94 */
		/* check for missing mandatory elements	=== */
        if(cause = CCBRI_AcceptCause(&garbage)) {               /* === */
			/* It's bad.  Disconnect with a Release Complete */
			if(CCB->B_Chan)										/* === */
				CCBVN_ReleaseChan();							/* === */
			if(CCBRI_GetOutInfo(HOST_DISCONNECT_ACK)) {			/* === */
				/* total length of private IE's = 0 */			/* === */
				OutPkgInfo[ OutPkgInfoLen++ ] = 0;				/* === */
				CCBRI_MailtoHost( CCB->CopyPublicMsg );			/* === */
			}													/* === */
			CCBVN_RejectCall(cause);							/* === */
			CCBVN_ReleaseCall();								/* === */
			break;												/* === */
        }                                                       /* === */
#endif /*===*/

         if ( CCB->B_Chan )
            {
#ifdef SBC_INTERFACE
#ifdef TASK_KB
            CCBRI_StopConnect( CCB->B_Chan );
#endif
#endif
            CCBVN_ReleaseChan();
            }
#ifdef SBC_INTERFACE
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_RELEASE;
            CCBRI_TRACE( TR_RELEASING );
            }
#else                                             /* COMMAND_INTERFACE */
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_DISC_HOST; /* Wait for HOST_DISCONNECT_ACK */
            }
#endif
         break;

#ifdef SBC_INTERFACE
      case KP_HOOK_FLASH:
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_RELEASE_IND:
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
#ifdef SBC_INTERFACE
#ifdef TASK_KB
            CCBRI_StopConnect( CCB->B_Chan );
#endif
#endif
            CCBVN_ReleaseChan();
            }
#ifdef SBC_INTERFACE
         CCB->State = VN_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#else                                             /* COMMAND_INTERFACE */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBVN_ReleaseCall();
#endif
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

/********************
* CCBVN_CallDialing *
*********************
*
* Process the current event in the context of a voice call in the
* DIAL state.
*
*/
PRIVATE int near CCBVN_CallDialing( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   BYTE cause;
   BYTE chan = 0;
#ifdef SBC_INTERFACE
   int  index;
   BYTE digit_as_byte;
#endif


   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_INFORMATION:     /* Process the information from HOST. */
         CCBRI_HostInfo();                      /* No change in state. */
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = VN_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = VN_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
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
         CCBRI_TRACE( TR_DIALING );
         if ( CCBRI_GetOutPkt( CC_INFO_REQ, FALSE ))
            {
            /* This explicit conversion of the event to a BYTE is necessary
            *  to provide portability of this code to various processors.
            */
            digit_as_byte = (BYTE) ( *event );
            /* Send the digit to Layer 3.
            */
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CALLED_NUMBER_IE, OVERLAP_NUMBER,
                                       UNKNOWN_NUMB_PLAN, &digit_as_byte, 1,
                                       TRUE );
            CCBRI_MailtoL3();
            }
         else
            {
            result_code = OUT_OF_PKTS;
            }
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_CALL_PROC_IND:
         /* This message indicates that dialing is complete.
         */
#ifdef SBC_INTERFACE
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
#endif
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
               CCBVN_SetNewChan( chan, CHANNEL_BUSY, TRUE );
            /* The B_Channel is probably connected already...just be sure.
            */
#ifdef SBC_INTERFACE
#ifdef TASK_KB
            if ( CCB->B_Chan )
               CCBRI_StartConnect( CCB->B_Chan );
#endif
            CCBRI_TRACE( TR_ROUTING );
#else                                             /* COMMAND_INTERFACE */
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
#endif
            CCB->State = VN_ROUTING;
            }
         else
            {
            /* Element coding error.
            */
            CCBVN_RejectCall( cause );
            CCBVN_ReleaseCall();
            result_code = CCBVN_CauseToResult( cause );
            }
         break;

#ifdef SBC_INTERFACE
      case KP_HOOK_FLASH:
         break;

      case KP_ON_HOOK:
         /* The caller hangs up in dialing.  The current call setup will
         *  be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING since the phone is on-hook.
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         result_code = CCBVN_StandardHangup( NORMAL_CLEARING );
         CCB->State = VN_RELEASE;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_DISCONNECT_IND:
#ifdef SBC_INTERFACE
         /* Far end disconnect.  Send CC_RELEASE_REQ and go to the
         *  state VN_RELEASE.
         */
#ifdef TASK_KB
         CCBRI_StopDigitDetect();
#endif
#endif
         if ( CCB->B_Chan )
            CCBVN_ReleaseChan();
#ifdef SBC_INTERFACE
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_RELEASE;
            CCBRI_TRACE( TR_RELEASING );
            }
#else                                             /* COMMAND_INTERFACE */
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_DISC_HOST; /* Wait for HOST_DISCONNECT_ACK */
            }
#endif
         break;

      case CC_RELEASE_IND:
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
#ifdef SBC_INTERFACE
#ifdef TASK_KB
            CCBRI_StopDigitDetect();
#endif
#endif
            CCBVN_ReleaseChan();
            }
#ifdef SBC_INTERFACE
         CCB->State = VN_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#else
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBVN_ReleaseCall();
#endif
         break;

#ifdef SBC_INTERFACE
#endif                                                /* SBC_INTERFACE */
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

#ifdef COMMAND_INTERFACE
/*********************
* CCBVN_CallDiscHost *
**********************
*
* Process the current event in the context of a call in the
* DISC_HOST state (awaiting HOST_DISCONNECT_ACK).
*
*/
PRIVATE int near CCBVN_CallDiscHost( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   BYTE event_as_byte;


   result_code = NOERR;

   switch ( *event )
      {
      case HOST_DISCONNECT_ACK:
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ))
            {
            CCBRI_MailtoL3();
            }
         else
            {
            result_code = OUT_OF_PKTS;
            }
         CCB->State = VN_RELEASE;
         break;

      case DISC_HOST_EVENT:                /* Timeout waiting for Host */
         if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ))
            {
            CCBRI_MailtoL3();
            }
         else
            {
            result_code = OUT_OF_PKTS;
            }
         event_as_byte = HOST_DISCONNECT_ACK;
         CCBRI_DisconnectHostCall( &event_as_byte, NORMAL_CLEARING );
         CCB->State = VN_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      /* === added the following cases, similar to CCBVN_CallConversation */
      case CC_RELEASE_IND:                    /* === */ 
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:                   /* === */ 
	 if ( CCB->B_Chan )                   /* === */ 
	    {
#ifdef SBC_INTERFACE                          /* === */ 
#ifdef TASK_KB                                /* === */
	    CCBRI_StopConnect( CCB->B_Chan ); /* === */

#endif                                        /* === */
#endif                                        /* === */
            CCBVN_ReleaseChan();              /* === */
	    }                                 /* === */
#ifdef SBC_INTERFACE                          /* === */
          CCB->State = VN_LOCKOUT;            /* === */
	  CCBRI_TRACE( TR_LOCKOUT );          /* === */
#else                                         /* === COMMAND_INTERFACE */
          if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )  /* === */
	     {
	     /* total length of private IE's = 0 */
	     OutPkgInfo[ OutPkgInfoLen++ ] = 0;        /* === */
	     CCBRI_MailtoHost( CCB->CopyPublicMsg );   /* === */
	     }
	  CCBVN_ReleaseCall();                         /* === */
#endif
	  break;                                       /* === */
	  /* === the above cases were added */
      
      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

#endif                                            /* COMMAND_INTERFACE */
#ifdef SBC_INTERFACE
/*********************
* CCBVN_CallFacility *
**********************
*
* Process the current event in the context of a voice call in the
* FACILITY_REQ state (waiting a response to a FACILITY Supplementary
* Service Request).
*
*/
PRIVATE int near CCBVN_CallFacility( event )

int *event;                                /* Event input to the state */

{
   int    result_code;
   CCB_ID ccb;


   result_code = NOERR;

   switch ( *event )
      {
      case CC_FACILITY_ACK:
         switch ( CCB->Service )
            {
            default:
               break;
            }
         break;

      case CC_FACILITY_REJ:
         switch ( CCB->Service )
            {
            default:
               break;
            }
         CCB->Service = NO_SERVICE_PENDING;
         break;

      case CC_DISCONNECT_IND:
         /* A call requesting HOLD may hang up.
         */
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_RELEASE;
            CCBRI_TRACE( TR_RELEASING );
            }
         break;

      case KP_HOOK_FLASH:
         break;

      case CC_RELEASE_IND:
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:
         if (( Hcb[ Dsl_ID ].Active_Call == CCB->HcbIndex ) &&
             ( Hcb[ Dsl_ID ].Hw_State == OFFHOOK ))
            {
            if ( CCB->B_Chan )
               CCBVN_ReleaseChan();
            CCB->State = VN_LOCKOUT;
            CCBRI_TRACE( TR_LOCKOUT );
            }
         else
            {
            CCBVN_ReleaseCall();
            }
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

#endif                                                /* SBC_INTERFACE */
/*****************
* CCBVN_CallIdle *
******************
*
* Process the current event in the context of a voice call in the
* IDLE state.
*
*/
PRIVATE int near CCBVN_CallIdle( event )

int *event;                                /* Event input to the state */

{
   int result_code;
#ifdef COMMAND_INTERFACE
   int cause;
#else                                                 /* SBC_INTERFACE */
   int addr_len;        /* Count of address digits collected "en_bloc" */
   int sub_addr_len;         /* Sub-address digits collected "en_bloc" */
#endif


   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_CALL_REQUEST:
         if ( ( cause = CCBRI_HostCallRequest() ) == NOERR )
            {
            CCB->State = VN_ORIGINATE;
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
            CCBVN_ReleaseCall();
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
         CCBVN_ReleaseCall();
         break;

      case HOST_DISCONNECT_ACK:
         /* Should not happen - if it does, free the CCB.
         */
         CCBVN_ReleaseCall();
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

#endif
#ifdef SBC_INTERFACE
      case KP_OFF_HOOK:
         /* This is a new outgoing call setup. The CC will send
         *  CC_SETUP_REQ.  If the data link is not established, Layer 3
         *  will detect this and request establishment before the SETUP
         *  is processed.
         *
         */
         Hcb[ Dsl_ID ].Hw_State = OFFHOOK;
         if ( CCBRI_GetOutPkt( CC_SETUP_REQ, FALSE ))
            {
            OutPkt->InfoLen +=
               CCIE_BuildBearerCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       Hcb[ Dsl_ID ].Bearer_Data,
                                       VOICE_BEARER_DATA_LEN );
               {
               OutPkt->InfoLen +=
                  CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       ANY_CHANNEL, 0, B_CHANNEL_UNITS, FALSE,
                                       FALSE, 0, NULL );
               }
            OutPkt->InfoLen +=
               CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CODESET_6, FALSE );
            OutPkt->InfoLen +=
               CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     CEPT_NATIONAL_STANDARD,
                                     NATIONAL_FUNCTION );

            /* Now check to see if digits were collected "en-bloc" in the
            *  CCBVN_CallOnhookDial function for en-bloc transmission.
            */
            if (( addr_len = STRLEN( CCB->Address )) > 0 )
               {
               OutPkt->InfoLen +=
                  CCIE_BuildPartyNumb_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CALLED_NUMBER_IE,
                                          EN_BLOC_NUMBER,
                                          UNKNOWN_NUMB_PLAN,
                                          (BYTE *)CCB->Address,
                                          addr_len,
                                          TRUE );
               }

            OutPkt->InfoLen +=
               CCIE_BuildHighLayer_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CCITT_STANDARD,
                                       CCITT_CEPT_INTERPRETATION,
                                       PROFILE_PRESENTATION,
                                       TELEPHONY_SERVICE );
            CCBRI_MailtoL3();
            CCB->State = VN_ORIGINATE;
            }
         else
            {
            if ( CCB->B_Chan )
               CCBVN_ReleaseChan();
            CCB->State = VN_LOCKOUT;
            result_code = OUT_OF_PKTS;
            }
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_SETUP_IND:
         result_code = CCBVN_SetupInd();
         break;

#ifdef SBC_INTERFACE
      case DIAL_KEY:                   /* Collect digits while on hook */
         CCBRI_TRACE( TR_DIALING );
         Hcb[ Dsl_ID ].Hw_State = DIAL_ONHOOK;
         CCB->State = VN_ONHOOK_DIAL;
         break;

      case KP_ON_HOOK:
         /* It is unclear why the CC should receive ON HOOK in idle
         *  state.  However, a proper thing to do if it does happen
         *  is to simply release the CCB.  There is no need to
         *  initiate a hangup procedure since L3 should be in
         *  NULL state ( state 0) at this point.
         */
         CCBVN_ReleaseCall();
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         if ( CCB )
            CCBVN_ReleaseCall();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         CCBVN_ReleaseCall();
         break;
      }

   return( result_code );
}

#ifdef SBC_INTERFACE
/********************
* CCBVN_CallLockout *
*********************
*
* Process the current event in the context of a call in the
* LOCKOUT (off-hook but released from the NETWORK) state.  The user
* is "locked-out" from originating again until an on-hook/off-hook
* cycle is completed.  This state is retained even after the NETWORK
* has released the call if the user has failed to hang up.
*
*/
PRIVATE int near CCBVN_CallLockout( event )

int *event;                                /* Event input to the state */

{
   int    result_code;
   int    index;


   result_code = NOERR;

   switch ( *event )
      {
      case KP_ON_HOOK:
         Hcb[ Dsl_ID ].Active_Call = NO_ACTIVE_CALLS;
         Hcb[ Dsl_ID ].Hw_State    = ONHOOK_IDLE;
         CCBVN_ReleaseCall();                 /* Release from lockout. */
         break;

      case DIAL_KEY:
         /* From the LOCKOUT state, a caller may re-originate (draw dial
         *  tone through a new SETUP) by pressing the DIAL_KEY if another
         *  call is already on hold (if we have preserved the B-channel).
         *  If on-hook dialing (en-bloc digit collection) is desired, the
         *  caller should hang up first.
         */
         CCB->State = VN_IDLE;
         *event = KP_OFF_HOOK;
         result_code = EVENT_PENDING;
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

#endif                                                /* SBC_INTERFACE */
#ifdef COMMAND_INTERFACE
/********************
* CCBVN_CallOffered *
*********************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to State 6 of Q.931.
*
* Note that the HOST must accept the network's channel choice.
*/
PRIVATE int near CCBVN_CallOffered( event )

int *event;                                /* Event input to the state */
{
   BYTE cause;
   int  result_code;
   int  k;


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
            /* Build Terminal Capabilities IE preceeded by codeset shift.
            */
            OutPkt->InfoLen +=
               CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CODESET_6, FALSE );
            OutPkt->InfoLen +=
               CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     CEPT_NATIONAL_STANDARD,
                                     NATIONAL_FUNCTION );
            CCBRI_MailtoL3();
            CCB->State = VN_ROUTING;
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
            /* Build Terminal Capabilities IE preceeded by codeset shift.
            */
            OutPkt->InfoLen +=
               CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CODESET_6, FALSE );
            OutPkt->InfoLen +=
               CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     CEPT_NATIONAL_STANDARD,
                                     NATIONAL_FUNCTION );
            CCBRI_MailtoL3();
            }
         CCB->State = VN_ALERT;
         break;

      case HOST_CONNECT:
         if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
            {
            CCBRI_MailtoL3();                /* Send CC_BROADCAST_RESP */
            }
         if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ) )
            {
            if ( InPkgInfo[ PRIVATE_IE_INDEX ] == 0x00 )
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
               /* Build Terminal Capabilities IE preceeded by codeset shift.
               */
               OutPkt->InfoLen +=
                  CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                             CODESET_6, FALSE );
               OutPkt->InfoLen +=
                  CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                        CEPT_NATIONAL_STANDARD,
                                        NATIONAL_FUNCTION );
               }
            CCBRI_MailtoL3();                     /* Send the mail now */
            }
         CCB->State = VN_ROUTING;        /* Wait for CC_SETUP_COMP_IND */
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
            CCBVN_RejectSetup( cause );
            }
         CCBVN_ReleaseCall();
         break;

      case HOST_DISCONNECT_ACK:
         /* Should not happen - if it does, free the CCB.
         */
         CCBVN_ReleaseCall();
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      case CC_DISCONNECT_IND:             /* Hang-up by the held party */
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            CCB->State = VN_DISC_HOST; /* Wait for HOST_DISCONNECT_ACK */
         break;

      case CC_RELEASE_IND:
		CCBVN_process_releaseind();	/* === */
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
         CCBVN_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

#endif                                            /* COMMAND_INTERFACE */
#ifdef SBC_INTERFACE
/***********************
* CCBVN_CallOnhookDial *
************************
*
* Process the current event in the context of a voice call in the
* ONHOOK_DIAL state.  This state is activated to allow dialing of digits
* for en-bloc transmission in call SETUP or related to a Supplementary
* Service activation.  It is entered when the DIAL_KEY is received in
* the IDLE state.
*
*/
PRIVATE int near CCBVN_CallOnhookDial( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   char event_as_string[ 2 ];


   result_code = NOERR;

   switch ( *event )
      {
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
         if ( CCB->CollectAddress )
            {
            if ( STRLEN( CCB->Address ) < MAX_ADDRESS_DIGITS )
               {
               /* Append the digit to the Address buffer.
               */
               event_as_string[ 0 ] = *event;
               event_as_string[ 1 ] = 0;
               STRCAT( CCB->Address, event_as_string );
               }
            else
               {
               result_code = DIGIT_BUFFER_OVERFLOW;
               }
            }
         break;

      case CANCEL_KEY:
         /* The CANCEL_KEY is used to cancel the call while still on-hook.
         */
         if ( Hcb[ Dsl_ID ].Hw_State == ONHOOK_IDLE )
            CCBVN_ReleaseCall();
         break;

      case KP_OFF_HOOK:
         /* This action will cause origination and en-bloc transmission
         *  of any address and sub-address digits already collected.
         */
         CCB->State = VN_IDLE;
         *event = KP_OFF_HOOK;
         result_code = EVENT_PENDING;
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

#endif                                                /* SBC_INTERFACE */
/**********************
* CCBVN_CallOriginate *
***********************
*
* Process the current event in the context of a voice call in the
* ORIGINATE state awaiting the first response to CC_SETUP_REQ.
*
*/
PRIVATE int near CCBVN_CallOriginate( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   BYTE cause;
   BYTE chan;
#ifdef SBC_INTERFACE
   int  index;
#endif

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
            CCBVN_RejectCall( cause );
            CCBVN_ReleaseCall();
            result_code = CCBVN_CauseToResult( cause );
            }
         else
            {
            if ( chan )
               CCBVN_SetNewChan( chan, CHANNEL_BUSY, TRUE );
#ifdef SBC_INTERFACE
            {
#ifdef TASK_KB
               CCBRI_StartDigitDetect();     /* Condition the hardware */
               CCBRI_StartConnect( CCB->B_Chan );     /* Rcv DIAL TONE */
#endif
               CCBRI_TRACE( TR_DIALTONE );
               CCB->State = VN_DIAL;
               }
            }
#else                                             /* COMMAND_INTERFACE */
            if ( CCBRI_GetOutInfo( HOST_INFORMATION ) )
               {
               /* total length of private IE's = 0
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 0;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               CCB->State = VN_DIAL;
               }
            }
#endif
         break;

      case CC_CALL_PROC_IND:
         /* This message indicates that dialing (en_bloc) is complete.
         */
         if (( cause = CCBRI_AcceptChanId( &chan, &ByteTrash )) != NOERR )
            {
            CCBVN_RejectCall( cause );
            CCBVN_ReleaseCall();
            result_code = CCBVN_CauseToResult( cause );
            }
         else
            {
            /* The B_Channel may be connected already...just be sure.
            */
            if ( chan )
               CCBVN_SetNewChan( chan, CHANNEL_BUSY, TRUE );
#ifdef SBC_INTERFACE
#ifdef TASK_KB
            if ( CCB->B_Chan )
               CCBRI_StartConnect( CCB->B_Chan );
#endif
            CCBRI_TRACE( TR_ROUTING );
#else                                             /* COMMAND_INTERFACE */
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
#endif
            }
         CCB->State = VN_ROUTING;
         break;

      case CC_SETUP_CONF:
         /* We may have an indication of a timeout of T303.  We will
         *  study the channel ID info element to determine this.
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
                  CCBVN_ReleaseChan();

			   /* Send a RELEASE.  Apparantly violate 5.2.1.3(4) === */
/* === */	   if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE )) {
/* === */			/* Build Cause info element (optional in this message) */
/* === */			Diag[ 0 ] = 0;
/* === */			OutPkt->InfoLen +=
/* === */				CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
/* === */						NORMAL_CLEARING, FALSE, Diag );
/* === */			CCBRI_MailtoL3();
/* === */	   }

#ifdef SBC_INTERFACE
               CCB->State = VN_LOCKOUT;
               CCBRI_TRACE( TR_LOCKOUT );
#else                                             /* COMMAND_INTERFACE */
               if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
                  {
                  /* Total length of private IE's = 0
                  */
                  OutPkgInfo[ OutPkgInfoLen++ ] = 0;
                  CCBRI_MailtoHost( FALSE );
                  CCBVN_ReleaseCall();
                  }
#endif
               }
            else
               {
               if ( Source == L3_ID )
                  {
                  CCBRI_RejectMsg( WRONG_MESSAGE, InPkt->Info[ 0 ] );
                  }
               result_code = UNEXPECTED_MSG;
               }
            }
         break;

#ifdef COMMAND_INTERFACE
      case HOST_DISCONNECT:
         /* Local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = VN_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         /* Local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = VN_RELEASE;
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

#endif                                            /* COMMAND_INTERFACE */
#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* The caller hangs up.  The current call setup
         *  will be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         result_code = CCBVN_StandardHangup( NORMAL_CLEARING );
         CCB->State = VN_RELEASE;
         CCBRI_TRACE( TR_RELEASING );
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_DISCONNECT_IND:
         /* Far end disconnect.  Send CC_RELEASE_REQ and go to the
         *  state VN_RELEASE.
         */
         if ( CCB->B_Chan )
            CCBVN_ReleaseChan();
#ifdef SBC_INTERFACE
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_RELEASE;
            CCBRI_TRACE( TR_RELEASING );
            }
#else                                             /* COMMAND_INTERFACE */
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_DISC_HOST; /* Wait for HOST_DISCONNECT_ACK */
            }
#endif
         break;

      case CC_RELEASE_IND:
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBVN_ReleaseChan();
            }
#ifdef SBC_INTERFACE
         CCB->State = VN_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#else
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBVN_ReleaseCall();
#endif
         break;

#ifdef SBC_INTERFACE
#endif                                                /* SBC_INTERFACE */
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

#ifdef SBC_INTERFACE
/*************************
* CCBVN_CallReconnecting *
**************************
*
* Process the current event in the context of a voice call in the
* RECONNECT (waiting for INTERNAL_RECONNECT) state.  The local user
* is off-hook.  The INTERNAL_RECONNECT is an indication that the call
* which initiated a HOLD or CALL WAITING retrieve has been released.
*
*/
PRIVATE int near CCBVN_CallReconnecting( event )

int *event;                                /* Event input to the state */

{
   int result_code;


   result_code = NOERR;

   switch ( *event )
      {
      case INTERNAL_RECONNECT:
         /* The formerly active call has been released by the Network.
         *  We interlock this event through this internal event.
         */

         /* Pick up the held or call waiting call by sending CC_SETUP_RESP
         *  downwards (CONNECT at L3).  The response to this message
         *  (CC_SETUP_COMP_IND) will be received in the VN_ROUTING state to
         *  complete the connection.
         */
         if ( CCBRI_GetOutPkt( CC_SETUP_RESP, FALSE ))
            {
            OutPkt->InfoLen +=
               CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CCB->B_Chan, 0, B_CHANNEL_UNITS, FALSE,
                                    TRUE, 0, NULL );
            OutPkt->InfoLen +=
               CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CODESET_6, FALSE );
            OutPkt->InfoLen +=
               CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     CEPT_NATIONAL_STANDARD,
                                     NATIONAL_FUNCTION );
            CCBRI_MailtoL3();                    /* Send CC_SETUP_RESP */
            Hcb[ Dsl_ID ].Hw_State = OFFHOOK;
            CCB->State = VN_ROUTING;
            CCBRI_TRACE( TR_ROUTING );
            }
         break;

      case KP_ON_HOOK:
         /* The caller hangs up.  The current call setup
         *  will be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         result_code = CCBVN_StandardHangup( NORMAL_CLEARING );
         CCB->State = VN_RELEASE;
         CCBRI_TRACE( TR_RELEASING );
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return ( result_code );
}

#endif                                                /* SBC_INTERFACE */
/********************
* CCBVN_CallRelease *
*********************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The HOST
* is already cleared for this call if there is a HOST.
*
*/
PRIVATE int near CCBVN_CallRelease( event )

int *event;                                /* Event input to the state */

{
   int result_code;


   result_code = NOERR;

   switch ( *event )
      {
      case CC_RELEASE_IND:
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:
#ifdef SBC_INTERFACE
         if (( CCB->OtherCallIndex != NO_CALL ) &&
             ( CCB->OtherCallIndex == Hcb[ Dsl_ID ].Active_Call ))
            {
            CCBVN_ReleaseCall();
            /* It is possible that the user has hung-up, clearing the
            *  reconnecting (active) call.
            */
            if (( CCB = Hcb[ Dsl_ID ].Ccb_In_Use[ Hcb[ Dsl_ID ].Active_Call ] ) != NULL )
               {
               result_code = EVENT_PENDING;   /* Activate reconnection */
               *event = INTERNAL_RECONNECT;
               }
            }
         else
            {
            CCBVN_ReleaseCall();
            }
#else                                             /* COMMAND_INTERFACE */
         CCBVN_ReleaseCall();
#endif
         break;

      case CC_DISCONNECT_IND:
#ifdef SBC_INTERFACE
#endif
         /* We simply await CC_RELEASE_CONF to follow this message.
         */
         break;

#ifdef COMMAND_INTERFACE
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

#endif
#ifdef SBC_INTERFACE
#endif                                                /* SBC_INTERFACE */
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

#ifdef COMMAND_INTERFACE
/***********************
* CCBVN_CallReleaseAck *
************************
*
* Process the current event in the context of a voice call in the
* RELEASING_ACK (waiting for CC_RELEASE_CONF) state.  The local user
* is to have the release acknowledged via HOST_DISCONNECT_ACK message
* in this case.
*
*/
PRIVATE int near CCBVN_CallReleaseAck( event )

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
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* Total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBVN_ReleaseCall();
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
         CCB->State = VN_RELEASE;
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
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

#endif
#ifdef SBC_INTERFACE
#endif                                                /* SBC_INTERFACE */
/********************
* CCBVN_CallRouting *
*********************
*
* Process the current event in the context of a voice call in the
* routing state, the general purpose busy, but not CONVERSATION, state.
*
*/
PRIVATE int near CCBVN_CallRouting( event )

int *event;                                /* Event input to the state */

{
   int  result_code;

   result_code = NOERR;

   switch ( *event )
      {
#ifdef COMMAND_INTERFACE
      case HOST_ALERTING:
         /* Indication that the Host has initiated alerting for the call
         */
         if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
            {
            /* Build Terminal Capabilities IE preceeded by codeset shift.
            */
            OutPkt->InfoLen +=
               CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CODESET_6, FALSE );
            OutPkt->InfoLen +=
               CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     CEPT_NATIONAL_STANDARD,
                                     NATIONAL_FUNCTION );
            CCBRI_MailtoL3();
            }
         CCB->State = VN_ALERT;
         break;

      case HOST_INFORMATION:     /* Process the information from HOST. */
         CCBRI_HostInfo();                      /* No change in state. */
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = VN_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = VN_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

#endif                                            /* COMMAND_INTERFACE */
      case CC_ALERTING_IND:
         /* Notification of far end alerting.
         *  The B-Channel should already be connected.
         */
#ifdef SBC_INTERFACE
         /* Connect it again just to be sure.
         */
#ifdef TASK_KB
         if ( CCB->B_Chan )
            CCBRI_StartConnect( CCB->B_Chan );
#endif
         CCBRI_TRACE( TR_RINGBACK );
#else                                             /* COMMAND_INTERFACE */
         /* Inform the HOST.
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

      case CC_SETUP_COMP_IND:
      case CC_SETUP_CONF:
         /* The call was connected at the far end.  Connect the call
         *  at this end too (its probably already connected) and the
         *  call will transfered to state CONNECTED.
         */
         /* NOTE: these messages correspond to CONNECT_ACK and CONNECT
         *  messages (respectivelly) and we should not have mandatory info
         *  elements to examine here.
         */

		if(!CCB->B_Chan) {		/* only check if mandatory */
			/* check for poorly formatted ChanID IE.  === */
			BOOLEAN struc_err;

			if((IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen, CHANNEL_ID_IE,
					CODESET_0, 1)) != NULL) {
				IE_Fields = CCIE_ExpandChanId_IE(IE, &struc_err);
				if(struc_err) {
					char diag[2];
					if ( CCBRI_GetOutPkt( CC_REL_COMP_REQ, FALSE )) {
						diag[ 0 ] = 0;
						OutPkt->InfoLen +=
							CCIE_BuildCause_IE( &OutPkt->Info[OutPkt->InfoLen],
							INVALID_ELEM_CONTENTS, FALSE, diag);
						CCBRI_MailtoL3();
					}
				}
			}
		}

#ifdef SBC_INTERFACE
#ifdef TASK_KB
         if ( CCB->B_Chan )
            CCBRI_StartConnect( CCB->B_Chan );
#endif
         CCBRI_TRACE( TR_CONVERSATION );
         CCB->State = VN_CONVERSATION;
#else                                             /* COMMAND_INTERFACE */
         /* Inform the HOST.
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
         CCB->State = VN_CONVERSATION;
#endif
         break;

#ifdef SBC_INTERFACE
      case KP_ON_HOOK:
         /* The caller hangs up.  The current call setup
         *  will be terminated by sending of CC_DISCONNECT_REQ.  The new
         *  state will be RELEASING.
         */
         result_code = CCBVN_StandardHangup( NORMAL_CLEARING );
         Hcb[ Dsl_ID ].Hw_State = ONHOOK_IDLE;
         CCB->State = VN_RELEASE;
         CCBRI_TRACE( TR_RELEASING );
         break;

#endif                                                /* SBC_INTERFACE */
      case CC_DISCONNECT_IND:
         /* Far end disconnect.  Send CC_RELEASE_REQ and go to the
         *  state VN_RELEASE.
         */
         if ( CCB->B_Chan )
            {
#ifdef SBC_INTERFACE
#ifdef TASK_KB
            CCBRI_StopConnect( CCB->B_Chan );
#endif
#endif
            CCBVN_ReleaseChan();
            }
#ifdef SBC_INTERFACE
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_RELEASE;
            CCBRI_TRACE( TR_RELEASING );
            }
#else                                             /* COMMAND_INTERFACE */
         if (( result_code = CCBVN_DisconnectInd()) == NOERR )
            {
            CCB->State = VN_DISC_HOST; /* Wait for HOST_DISCONNECT_ACK */
            }
#endif
         break;

      case CC_PROGRESS_IND:
         /* CAUSE is a mandatory info element here - check
         *  for it first:
         */
		 /* Cause is not mandatory === */
#ifdef SBC_INTERFACE
#ifdef TASK_KB
            /* Progress is an indication of some call progress tone.
            *  Connect the B_Chan to be sure it is connected to receive
            *  the call progress tone.
            */
            if ( CCB->B_Chan )
               CCBRI_StartConnect( CCB->B_Chan )
#endif
#endif
         break;

      case CC_RELEASE_IND:
		CCBVN_process_releaseind();	/* === */
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
#ifdef SBC_INTERFACE
#ifdef TASK_KB
            CCBRI_StopConnect( CCB->B_Chan );
#endif
#endif
            CCBVN_ReleaseChan();
            }
#ifdef SBC_INTERFACE
         CCB->State = VN_LOCKOUT;
         CCBRI_TRACE( TR_LOCKOUT );
#else                                             /* COMMAND_INTERFACE */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBVN_ReleaseCall();
#endif
         break;

#ifdef SBC_INTERFACE
#endif                                                /* SBC_INTERFACE */
      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBVN_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

#ifdef SBC_INTERFACE
#endif                                                /* SBC_INTERFACE */
/************************************************************************
*                     O T H E R   F U N C T I O N S                     *
*************************************************************************
*/
/**********************
* CCBVN_CauseToResult *
***********************
*
* Convert a CCITT cause value into an error result code.
* Note that all legal causes are not supported, only those generated.
*/
PRIVATE int near CCBVN_CauseToResult( cause )

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
* CCBVN_DecodeCallState *
*************************
*
* Decode the state of the CCB from the CCB state variable.
* Dispatch the appropriate state function through the state function
* jump table.
*
*/
PUBLIC int CCBVN_DecodeCallState( event )

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
   if (( CCB->State > VN_STATE_LOW_LIMIT ) &&
       ( CCB->State < VN_STATE_HIGH_LIMIT ))
      {
      /* Enter state function.
      */
      result_code = ( *Ccb_State_Func[ CCB->State ] )( event );
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
#endif                                                /* SBC_INTERFACE */
/**********************
* CCBVN_DisconnectInd *
***********************
*
* Generic action upon receiving the CC_DISCONNECT_IND message.
*
*/
PRIVATE int near CCBVN_DisconnectInd()

{
   int  result_code;
   BYTE cause;                         /* Locally-detected error cause */
   BYTE errcause;
#ifdef COMMAND_INTERFACE
   BYTE host_event = HOST_DISCONNECT;      /* Event to be sent to HOST */
#endif

   result_code = NOERR;
#ifdef COMMAND_INTERFACE
   if ( (errcause = CCBRI_AcceptCause( &cause )) == MANDATORY_IE_MISSING ) {
		/* Used to just set cause = 0  */
		/* It's bad.  Disconnect with a Release Complete */
		if(CCB->B_Chan)										/* === */
			CCBVN_ReleaseChan();							/* === */
		if(CCBRI_GetOutInfo(HOST_DISCONNECT_ACK)) {			/* === */
			/* total length of private IE's = 0 */			/* === */
			OutPkgInfo[ OutPkgInfoLen++ ] = 0;				/* === */
			CCBRI_MailtoHost( CCB->CopyPublicMsg );			/* === */
		}													/* === */
		CCBVN_RejectCall(errcause);							/* === */
		CCBVN_ReleaseCall();								/* === */
		return MISSING_MANDATORY_IE;						/* === */
	}                                                       /* === */
 cause = errcause;

 if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) )
     {
      if (cause != NOERR) {
        Diag[ 0 ] = 0;
        OutPkt->InfoLen +=
           CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                cause, FALSE, Diag );
      }
      CCBRI_MailtoL3();
      CCB->State = VN_RELEASE;
      }
   CCB->Cause  = (int ) cause;
   CCBRI_DisconnectHostCall( &host_event, cause );
#else                             /* SBC_INTERFACE */
   /* Far end disconnect.  Send CC_RELEASE_REQ. We will retain connection
   *  to the B-Channel for in-band tones or announcements.
   *
   * CAUSE is a mandatory info element here - check
   *  for it first:
   */
   if (( cause = CCBRI_AcceptCause( &ByteTrash )) == NOERR )
      {
      if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ))
         {
         /* Build Cause info element (optional in this message)
         */
         Diag[ 0 ] = 0;
         OutPkt->InfoLen +=
            CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                NORMAL_CLEARING, FALSE, Diag );
         CCBRI_MailtoL3();
         }
      else
         {
         result_code = OUT_OF_PKTS;
         }
      }
   else
      {
      CCBVN_RejectCall( cause );
      CCBVN_ReleaseCall();
      result_code = CCBVN_CauseToResult( cause );
      }
#endif

   return ( result_code );
}

#ifdef SBC_INTERFACE
/*********************
* CCBVN_GetCallIndex *
**********************
*
* Recover the index of the first CCB from the Hcb with a matching state.
*
*/
PRIVATE int near CCBVN_GetCallIndex( state )

int state;                                           /* State to match */

{
   int    i;
   int    result;
   CCB_ID ccb;


   result = NO_STATE_MATCH;                              /* Initialize */
   for ( i = 0; i < MAX_VOICE_CALLS; i++ ) /* Search all the Hcb slots */
      {
      ccb = Hcb[ Dsl_ID ].Ccb_In_Use[ i ];
      if (( ccb ) && ( ccb->State == state ))
         {
         result = i;
         break;
         }
      }

   return ( result );
}

/*****************
* CCBVN_PassCall *
******************
*
* Pass the ownership of the active call slot to a call which is on HOLD
* or in CALL WAITING.  This is a prelude to the reconnection with this
* call after the currently active call has disconnected and has been
* released by the Network.
*
*/
PRIVATE int near CCBVN_PassCall()

{
   CCB_ID ccb;
   int    result_code;
   int    channel;


   result_code = NOERR;
   ccb = CCB;           /* Save the CCB of the call being reconnected. */

   /* Check for an active call.
   */
   if ( Hcb[ Dsl_ID ].Hw_State == OFFHOOK )
      {
      CCB = Hcb[ Dsl_ID ].Ccb_In_Use[ Hcb[ Dsl_ID ].Active_Call ];

      /* The channel of the active call is re-used by the held call.
      */
      channel = CCB->B_Chan;

      if ( channel )
         {
         CCBVN_ReleaseChan();
         if ( ccb->B_Chan == NO_CHANNEL )               /* A held call */
            {
            /* The channel of the active call is re-used to connect the
            *  held call.
            */
            ccb->B_Chan = channel;      /* Save the channel for re-use */
            CC_CHAN_SetStatebri( channel, B_CHANNEL_UNITS, CHANNEL_RESERVED, Dsl_ID );
            CC_CHAN_SetProtocolbri( channel, B_CHANNEL_UNITS, L2P_LAPD, Dsl_ID );
            CC_CHAN_SetUser( channel, B_CHANNEL_UNITS, CCB->Call_ID, Dsl_ID );
            }
         }

      if ( CCB->State == VN_LOCKOUT )
         {
         CCBVN_ReleaseCall();           /* Release the active call CCB */
         CCB = ccb;     /* The call waiting ccb becomes the acitve CCB */
         Hcb[ Dsl_ID ].Active_Call = CCB->HcbIndex;
         CCB->State = VN_RECONNECT;
         result_code = EVENT_PENDING;         /* Activate reconnection */
         }

      else if ((result_code = CCBVN_StandardHangup(NORMAL_CLEARING)) == NOERR)
         {
         /* We force the active call to disconnect.  The reconnection will
         *  be resumed when the active call is released through the internal
         *  event INTERNAL_RECONNECT generated in the RELEASING state.
         */
         /* Save the index of the reconnecting call.
         */
         CCB->OtherCallIndex = ccb->HcbIndex;
         CCB->State = VN_RELEASE;

         /* The reconnecting call becomes the new active call.
         */
         Hcb[ Dsl_ID ].Active_Call = ccb->HcbIndex;
         ccb->State = VN_RECONNECT;
         }
      else
         {
         result_code = OUT_OF_PKTS;
         }
      }

   return( result_code );
}

#endif                                                /* SBC_INTERFACE */
/*******************
* CCBVN_RejectCall *
********************
*
* Reject a call due to flaws or incompatibilities in the associated message
* received from the peer.  The function CCBVN_RejectSetup is only used for
* rejecting the CC_SETUP_IND and this function is used otherwise.
*
*/
PRIVATE int near CCBVN_RejectCall( cause )

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
      if(SwitchType != VN3_STYPE) { /* added at vn3 homologation testing */
          OutPkt->InfoLen +=
             CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    CODESET_6, FALSE );
          OutPkt->InfoLen +=
             CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                               CEPT_NATIONAL_STANDARD,
                               NATIONAL_FUNCTION );
      }
      CCBRI_MailtoL3();                        /* Send CC_REL_COMP_REQ */
      }
   else
      {
      result_code = OUT_OF_PKTS;
      }

   return ( result_code );
}

/********************
* CCBVN_RejectSetup *
*********************
*
* This function is called as the first response to the CC_SETUP_IND
* when the call is being rejected.
*
*/
PRIVATE void near CCBVN_RejectSetup( cause )

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
         OutPkt->InfoLen +=
            CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CODESET_6, FALSE );
         OutPkt->InfoLen +=
            CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                  CEPT_NATIONAL_STANDARD,
                                  NATIONAL_FUNCTION );
         CCBRI_MailtoL3();                    /* Send CC_SETUP_REJ_REQ */
         }
      }
   return;
}

/********************
* CCBVN_ReleaseCall *
*********************
*
* Terminate a call and put the CCB to bed.
*
*/
PUBLIC void CCBVN_ReleaseCall()

{
#ifdef SBC_INTERFACE
   int index;
#endif

   /* Perform actions specific to the voice call termination.
   */
   if ( CCB->B_Chan )
      CCBVN_ReleaseChan();

#ifdef SBC_INTERFACE
   if (( Hcb[ Dsl_ID ].Active_Call == CCB->HcbIndex ) &&
       ( Hcb[ Dsl_ID ].Hw_State == OFFHOOK ))
      {
      CCBRI_TRACE( TR_LOCKOUT );
      CCB->State = VN_LOCKOUT;
      }
   else
      {
      CCBRI_RemoveCCB( CCB );
      }
#else                                             /* COMMAND_INTERFACE */
   CCBRI_RemoveCCB( CCB );
#endif

#ifdef SBC_INTERFACE
   if ( Hcb[ Dsl_ID ].Active_Call == NO_ACTIVE_CALLS )
      {
      CCBRI_TRACE( TR_IDLE );
      }
#endif                                                /* SBC_INTERFACE */

   return;
}

/********************
* CCBVN_ReleaseChan *
*********************
*
* Remove a B-Channel from allocation by a CCB.
*
*/
PRIVATE void near CCBVN_ReleaseChan()

{
   if ( CC_CHAN_GetUserbri( CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID ) == CCB->Call_ID )
      {
#ifdef TASK_KB
      CCBRI_StopConnect( CCB->B_Chan );
#endif
      CC_CHAN_ReleaseChanbri( CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID );
      }
   CCB->B_Chan = 0;
}

/*********************
* CCBVN_ReleasedLink *
**********************
*
* The data link is down at Layer 2.
* Remove CCBs from any context but LOCKOUT.  Users left OFF HOOK
* must retain a CCB in the LOCKOUT state to process their hangup.
*
* It is specified that calls in the ACTIVE state should not be disturbed.
*
*/
PUBLIC void CCBVN_ReleasedLink (ces, dsl_id)
BYTE ces;          /* Connection Endpoint Suffix of the released link. */
BYTE dsl_id;                                /* Digital Subscriber Loop */
{
   CCB_ID tmp_ccb;

   Dsl_ID = dsl_id; /* This assumption is made in CCBVN_ReleaseCall(). */
   /*
    * Start at the head and remove all impacted CCBs.
    */
   CCB = CCB_Head[dsl_id];
   while (CCB) {
      tmp_ccb = CCB->Next;
      if (CCB->Ces == ces) {
         if (CCB->State != VN_CONVERSATION) {
            if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) ) {
               /* total length of private IE's = 0
               */
               OutPkgInfo[OutPkgInfoLen++] = 0;
               CCBRI_MailtoHost(FALSE);
            }
            CCBVN_ReleaseCall();
         }
      }
      CCB = tmp_ccb;   /* Skip to the next one. */
   }
}

/*******************
* CCBVN_SetNewChan *
********************
*
* Establish a new B-Channel in the CCB.
*
*/
PRIVATE BYTE near CCBVN_SetNewChan( b_chan, state, overwrite )

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
* CCBVN_SetupInd *
******************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function because
* of its inherent complexity.  This process is complicated by the
* possible presence and interraction of the Call Waiting and Call
* Deflection and UUI Supplementary Services where they are activated.
*
*/
PRIVATE int near CCBVN_SetupInd()

{
   BYTE   cause;
   BYTE   chan;
   int    length = 0;
   BYTE   exclusive = 0;
   int    result_code = NOERR;
#ifdef TASK_KB
   CCB_ID active_ccb;
#endif
#ifdef COMMAND_INTERFACE
   int    k;
#endif

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

#ifdef COMMAND_INTERFACE
   /* Prepare to inform the HOST.
   */
   if ( !CCBRI_GetOutInfo( HOST_INCOMING_CALL ) )
      {
      goto in_call_failed;
      }
   OutPkgInfoLen++;
   k = 5;
#endif

   /* Look at the bearer capability IE.
   */
   if (( cause = CCBRI_AcceptBearerCap()) != NOERR )
      {
      if (( cause == CAP_NOT_IMPLEMENTED )   ||            /* Cause 65 */
          ( cause == INVALID_ELEM_CONTENTS ) ||           /* Cause 100 */
          ( cause == MANDATORY_IE_LEN_ERR ))              /* Cause 103 */
         {
         /* We do not respond in this case.
         */
         CCBVN_ReleaseCall();
         result_code = CCBVN_CauseToResult( cause );
         goto in_call_failed;
         }
      else
         {
         /* This is a mandatory element.  The call must be rejected.
         */
         CCBVN_RejectSetup( cause );
         CCBVN_ReleaseCall();
         result_code = CCBVN_CauseToResult( cause );
         goto in_call_failed;
         }
      }

   /* Next we look at the Channel ID.
   */
   if (( cause = CCBRI_AcceptChanId( &chan, &exclusive )) != NOERR )
      {
      /* This is a mandatory element.  The call must be rejected.
      */
      if (( cause != INVALID_ELEM_CONTENTS ) &&           /* Cause 100 */
          ( cause != MANDATORY_IE_LEN_ERR ))              /* Cause 103 */
         {
         CCBVN_RejectSetup( cause );
         }
      CCBVN_ReleaseCall();
      result_code = CCBVN_CauseToResult( cause );
      goto in_call_failed;
      }
	
#ifdef COMMAND_INTERFACE
   if ( chan )
      {
      CCBVN_SetNewChan( chan, CHANNEL_BUSY, TRUE );
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
      CCBVN_ReleaseCall();
      result_code = CCBVN_CauseToResult( cause );
      goto in_call_failed;
      }
#endif

   /* deleted test on Called Number IE   === */

   /* NOTE: Here tests may be inserted for checking the optional
   *  Lower Layer and Higher Layer compatability information elements
   *  and any other elements of concern.
   */

#ifdef COMMAND_INTERFACE
   k += length;
   OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
   CCBRI_MailtoHost( CCB->CopyPublicMsg );
   CCB->State = VN_OFFERED;

#else                                                 /* SBC_INTERFACE */
   /* A subsequent call setup when we are already busy?
   */
   if ( CCB->HcbIndex != Hcb[ Dsl_ID ].Active_Call )
      {
      CCBVN_RejectSetup( USER_BUSY );
      result_code = CCBVN_CauseToResult( cause );
      CCBVN_ReleaseCall();
      goto in_call_failed;
      }

   if ( CCBVN_GetCallIndex( VN_ALERT ) != NO_STATE_MATCH )
      {
      /* Another call in already ringing to this terminal.
      */
      CCBVN_RejectSetup( USER_BUSY );
      result_code = CCBVN_CauseToResult( cause );
      CCBVN_ReleaseCall();
      goto in_call_failed;
      }

   if ( CCBRI_GetOutPkt( CC_BROADCAST_RESP, FALSE ))
      {
      CCBRI_MailtoL3();                      /* Send CC_BROADCAST_RESP */

      if ( CCBRI_GetOutPkt( CC_ALERTING_REQ, FALSE ))
         {
         CCBVN_SetNewChan( chan, CHANNEL_BUSY, TRUE );

         /* Build Terminal Capabilities IE preceeded by codeset shift.
         */
         OutPkt->InfoLen +=
            CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CODESET_6, FALSE );
         OutPkt->InfoLen +=
            CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                  CEPT_NATIONAL_STANDARD,
                                  NATIONAL_FUNCTION );
         CCBRI_MailtoL3();                     /* Send CC_ALERTING_REQ */

         if ( TRUE )
            {
            /* Here turn ringing on.
            */
#ifdef TASK_KB
            CCBRI_StartRing( SIMPLE_RING );
#endif
            CCB->State = VN_ALERT;
            CCBRI_TRACE( TR_RINGING );
            Hcb[ Dsl_ID ].Hw_State = BELL;
            }
         }
      else                            /* Couldn't send CC_ALERTING_REQ */
         {
         CCBVN_ReleaseCall();
         result_code = OUT_OF_PKTS;
         goto in_call_failed;
         }
      }
   else                             /* Couldn't send CC_BROADCAST_RESP */
      {
      CCBVN_ReleaseCall();
      result_code = OUT_OF_PKTS;
      goto in_call_failed;
      }
#endif                                                /* SBC_INTERFACE */

in_call_failed:
   return ( result_code );
}

/*****************
* CCBVN_Shutdown *
******************
*
* Reset Call Control
*
*/
PUBLIC void CCBVN_Shutdown (dsl)
BYTE dsl;
{
    CCB_ID temp_ccb;

    Dsl_ID = dsl;
    CCB = CCB_Head[dsl];
    while (CCB) {
        temp_ccb = CCB->Next;
        CCBVN_ReleaseCall();
        CCB = temp_ccb;
    }
}

#ifdef SBC_INTERFACE
/***********************
* CCBVN_StandardHangup *
************************
*
* Standard actions for hangup by the voice phone.  This function initiates
* sending of CC_DISCONNECT_REQ.  It should be used whenever the voice
* phone goes on hook while engaged.
*
*/
PRIVATE int near CCBVN_StandardHangup( cause )

BYTE cause;                     /* Cause for cause information element */
{
   int result_code;


#ifdef TASK_KB
   CCBRI_StopConnect( CCB->B_Chan );
#endif
   if ( CCBRI_GetOutPkt( CC_DISCONNECT_REQ, FALSE ))
      {
      /* Build mandatory Cause element.
      */
      Diag[ 0 ] = 0;
      OutPkt->InfoLen +=
         CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                             cause, FALSE, Diag );
      CCBRI_MailtoL3();
      result_code = NOERR;
      }
   else
      {
      result_code = OUT_OF_PKTS;
      }

   return( result_code );
}

#endif
/**********************
* CCBVN_UnexpectedMsg *
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
PRIVATE void near CCBVN_UnexpectedMsg( diagnostic )

BYTE diagnostic;                    /* The message type received by L3 */
{
   CCBRI_RejectMsg( WRONG_MESSAGE, diagnostic );
   /* Other action(s) appropriate to the specific application may be added
   *  here.
   */
   return;
}

#endif  /* === */
