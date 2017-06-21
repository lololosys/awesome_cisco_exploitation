/* $Id: ccbts013.c,v 3.3.12.2 1996/09/13 04:54:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccbts013.c,v $
 *------------------------------------------------------------------
 * ccbts013.c  ISDN Australia BRI Call Control Functions
 *
 * February 1994, Dennis Clare
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * ccbts013.c,v 
 *------------------------------------------------------------------
 * $Log: ccbts013.c,v $
 * Revision 3.3.12.2  1996/09/13  04:54:36  thille
 * CSCdi68927:  Clean up some things that should be const in ISDN
 * Branch: California_branch
 * Make some more stuff const to save ram.  Also make some more stuff
 * static.
 *
 * Revision 3.3.12.1  1996/04/30  23:06:58  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/15  17:27:24  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.2  1995/11/17  17:39:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/23  15:14:08  dclare
 * CSCdi36325:  Australian ISDN PRI implementation for the 7000. Also get
 * rid of some homologation defines that became too awkward to use. These
 * are the final changes found during homologation.
 *
 * Revision 2.1  1995/06/07  21:13:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccbts013.c
VERSION: 3.21+
DESCRIPTION: ISDN Australia BRI Call Control Functions

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

This module started life as a 3.21 version of the ccbnet3.c file. It has
been changed to fit the Australia, TS013 (previously called TPH_1962) way
of doing things.

The current version of this module is intended to implement the
Call Control functions for a simple user-side Basic Rate Interface
(Basic Rate Access) terminal in a manner compatible with a
Central Office switch in accordance with Australian Specifications TS 013 
ISDN Basic Rate. The terminal is
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
7.  The entry point to the module is CCBTS013_Go.
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

/* ISDN includes
*/
#include    "flags.h"
#ifdef BRI_TS013
#include    "tune.h"
#include    "gendef.h"
#include    "lif.h"
#include    "ccie.h"
#include	"gp.h"
#include    "ccbri.h"
#include    "cc_chan.h"

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
*                references in the Ccb_State_Func_Ts013 table further on
*                must agree in number and position.
*/
enum CCB_State
   {
   TS013_STATE_LOW_LIMIT,                               /* Sizing element */
   TS013_IDLE,                                                /* CCB idle */
   TS013_ALERT,                                          /* Being alerted */
   TS013_CONVERSATION,                                    /* Conversation */
   TS013_DIAL,                                                 /* Dialing */
   TS013_OFFERED,                 /* Await host response to incoming call */
   TS013_ORIGINATE,                     /* Await response to CC_SETUP_REQ */
   TS013_RELEASE,                          /* Waiting for release confirm */
   TS013_RELEASE_ACK,        /* Waiting for release confirm (ack to HOST) */
   TS013_ROUTING,        /* Busy, but neither dialing nor in conversation */
   TS013_STATE_HIGH_LIMIT                               /* Sizing element */
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
*              F U N C T I O N   D E C L A R A T I O N S                *
*************************************************************************
*/
/* Arranged alphabetically.
*/
PRIVATE int      near CCBTS013_CallAlerting     (int *);
PRIVATE int      near CCBTS013_CallConversation (int *);
PRIVATE int      near CCBTS013_CallDialing      (int *);
PRIVATE int      near CCBTS013_CallIdle         (int *);
PRIVATE int      near CCBTS013_CallOffered      (int *);
PRIVATE int      near CCBTS013_CallOriginate    (int *);
PRIVATE int      near CCBTS013_CallRelease      (int *);
PRIVATE int      near CCBTS013_CallReleaseAck   (int *);
PRIVATE int      near CCBTS013_CallRouting      (int *);
PRIVATE int      near CCBTS013_CauseToResult    (BYTE);
PRIVATE BOOLEAN  near CCBTS013_DisconnectInd    (void);
PRIVATE int      near CCBTS013_RejectCall       (BYTE);
PRIVATE void     near CCBTS013_RejectSetup      (BYTE);
PRIVATE void     near CCBTS013_ReleaseChan      (void);
PRIVATE int      near CCBTS013_SendRelease      (BYTE);
PRIVATE BYTE     near CCBTS013_SetNewChan       (int, int, BOOLEAN);
PRIVATE int      near CCBTS013_SetupInd         (void);
PRIVATE void     near CCBTS013_UnexpectedMsg    (BYTE);
PRIVATE BYTE     near CCBTS013_VerifyDiscIEs    (BYTE *, int);

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
/* State Function Jump Table
*/
typedef int (near *CC_STATES)( int * );
static const CC_STATES Ccb_State_Func_Ts013[ TS013_STATE_HIGH_LIMIT ] =
   {
   NULL
   ,
   CCBTS013_CallIdle          /* L3 State 0 */
   ,
   CCBTS013_CallAlerting      /* L3 State 4 */
   ,
   CCBTS013_CallConversation  /* L3 State 10 */
   ,
   CCBTS013_CallDialing       /* L3 State 2 */
   ,
   CCBTS013_CallOffered       /* L3 State 6 */
   ,
   CCBTS013_CallOriginate     /* L3 State 1 */
   ,
   CCBTS013_CallRelease       /* L3 State 12 */
   ,
   CCBTS013_CallReleaseAck    /* L3 State 11 */
   ,
   CCBTS013_CallRouting       /* L3 State 3, 8 */
   };

PRIVATE BYTE    Diag[ 2 ];     /* For passing cause diagnostic to CCIE */
PRIVATE BYTE    ByteTrash;                              /* Byte bucket */

/*
 * We always must send the REL_COMP. First must check the Cause ie
 * and then the Display ie. The Cause is mandatory only if it is the
 * first disconnecting message. L3 doesn't check the IEs real well,
 * so it stays in the same state and waits for the CC_REL_COMP_REQ
 * message before it will send out the RELEASE_COMPLETE message.
 */
static void
CCBTS013_process_releaseind (BOOLEAN cause_mandatory)
{
    char display_chars;
    BYTE cause;
    BYTE diag [2];

    /*
     * check if the Cause ie is missing or corrupted. Only
     * then should the display be checked
     */
    cause = CCBRI_AcceptCause( &cause );
    diag[ 0 ] = CAUSE_IE;
    diag[ 1 ] = 0;
    if ((cause_mandatory) && (cause == MANDATORY_IE_MISSING ||
         cause == INVALID_ELEM_CONTENTS)) {
       /*
        * tell L3 to send out the RELEASE_COMPLETE
        */
       if(CCBRI_GetOutPkt(CC_REL_COMP_REQ, FALSE)) {
          OutPkt->InfoLen +=
            CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
            cause,
            FALSE,          /* not network side */
            diag);
          CCBRI_MailtoL3();
       }

    } else {
      /*
       * cause is not mandatory, may have invalid ie for cause
       */
      if (cause == NOERR || cause == MANDATORY_IE_MISSING) {
        cause = CCBRI_AcceptDisplay((char *)  &display_chars );
        diag[0] = DISPLAY_IE;
        diag[1] = 0;
      }

      /*
       * tell L3 to send out the RELEASE_COMPLETE
       */
      if(CCBRI_GetOutPkt(CC_REL_COMP_REQ, FALSE)) {
         if (cause != NOERR) {
             OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
               cause,
               FALSE,          /* not network side */
               diag);
         }
         CCBRI_MailtoL3();
      }
    }
}

/************************************************************************
*                C A L L   S T A T E   F U N C T I O N S                *
*************************************************************************
*/
/**************************
* CCBTS013_CallAlerting *
***************************
*
* Process the current event in the context of a voice call in the
* ALERT state. L3 State= 1.
*
*/
PRIVATE int near CCBTS013_CallAlerting (int * event )
{
   int  result_code;

   result_code = NOERR;

   switch ( *event )
      {
      case HOST_CONNECT:
         CCBRI_HostConnect();
         CCB->State = TS013_ROUTING;
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );    /* Process disc from HOST. */
         CCB->State = TS013_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );/* Process disc ack from HOST. */
         CCB->State = TS013_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case CC_DISCONNECT_IND:
         /* Distant end disconnect (response to this message will
         *  be received in RELEASING state - see CC_RELEASE_CONF):
         */
         CCBTS013_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBTS013_process_releaseind(TRUE);
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBTS013_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
/* ===
bailout:
=== */
   return( result_code );
}

/******************************
* CCBTS013_CallConversation *
*******************************
*
* Process the current event in the context of a voice call in the
* CONVERSATION state.
*  L3 State 10
*/
PRIVATE int near CCBTS013_CallConversation( event )

int *event;                                /* Event input to the state */

{
   int     result_code;                /* Result code for the function */

   result_code = NOERR;

   switch ( *event )
      {
      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = TS013_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = TS013_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      case CC_DISCONNECT_IND:
         if ( CCB->B_Chan )
            CCBTS013_ReleaseChan();
         CCBTS013_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBTS013_process_releaseind(TRUE);
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBTS013_ReleaseChan();
            }
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBTS013_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

/*************************
* CCBTS013_CallDialing *
**************************
*
* Process the current event in the context of a voice call in the
* DIAL state. L3 State= 2.
* 
*/
PRIVATE int near CCBTS013_CallDialing( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   BYTE cause;
   BYTE chan = 0;
   BYTE prog_desc;

   result_code = NOERR;

   switch ( *event )
      {
      case HOST_INFORMATION:     /* Process the information from HOST. */
         CCBRI_HostInfo();                      /* No change in state. */
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = TS013_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = TS013_RELEASE;
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
               CCBTS013_SetNewChan( chan, CHANNEL_BUSY, TRUE );
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
            CCB->State = TS013_ROUTING;
            }
         else
            {
            /* Element coding error.
            */
            CCBTS013_RejectCall( cause );
            CCBTS013_ReleaseCall();
            result_code = CCBTS013_CauseToResult( cause );
            }
         break;

      case CC_DISCONNECT_IND:
         if ( CCB->B_Chan )
            CCBTS013_ReleaseChan();
         CCBTS013_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBTS013_process_releaseind(TRUE);
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBTS013_ReleaseChan();
            }
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBTS013_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      case CC_PROGRESS_IND:
         /* just allow the message to come in, don't change state
          * or do anything else. Added this state
          *
          * now have to check the PROGRESS, it is optional, but we Australia
          * tests this and will send an invalid value.
          */
         if ((cause = CCBRI_AcceptProgress( &prog_desc )) != NOERR) {
            CCBTS013_SendRelease( cause );
            CCBTS013_ReleaseCall();
            result_code = CCBTS013_CauseToResult( cause );
            CCBRI_FreeCallBufs();

           /* added to remove the dialer carrier timer */
           if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) ) {
              /* total length of private IE's = 0 */
              OutPkgInfo[ OutPkgInfoLen++ ] = 0;
              CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }

            CCB->State = TS013_RELEASE;
         }
         break;

      case CC_ALERTING_IND:
        /* Notification of far end alerting.
         * The B-Channel should already be connected.
         * Inform the HOST. Added this state.
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
         /* The call was connected at the far end.  Connect the call
         *  at this end too (its probably already connected) and the
         *  call will transfered to state CONNECTED.
         *
         * NOTE: these messages correspond to CONNECT_ACK and CONNECT
         *  messages (respectivelly) and we should not have mandatory info
         *  elements to examine here.
         */
		 if(!CCB->B_Chan) {		/* only check if mandatory */
			/* check for poorly formatted ChanID IE. */
			BOOLEAN struc_err = FALSE;

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

         /* Inform the HOST  */
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
         CCB->State = TS013_CONVERSATION;
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}
/**********************
* CCBTS013_CallIdle *
***********************
*
* Process the current event in the context of a voice call in the
* IDLE state. State = 0.
*
*/
PRIVATE int near CCBTS013_CallIdle( event )

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
            CCB->State = TS013_ORIGINATE;
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
            CCBTS013_ReleaseCall();
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
         CCBTS013_ReleaseCall();
         break;

      case HOST_DISCONNECT_ACK:
         /* Should not happen - if it does, free the CCB.
         */
         CCBTS013_ReleaseCall();
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case CC_SETUP_IND:
         result_code = CCBTS013_SetupInd();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         if ( CCB )
            CCBTS013_ReleaseCall();
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         CCBTS013_ReleaseCall();
         break;
      }

   return( result_code );
}

/*************************
* CCBTS013_CallOffered *
**************************
*
* Process the current event in the context of a voice/data call which
* has been offered to the Host. Corresponds to State 6 of Q.931.
* L3 State = 6.
* Note that the HOST must accept the network's channel choice.
*/
PRIVATE int near CCBTS013_CallOffered( event )

int *event;                                /* Event input to the state */
{
   BYTE cause = NOERR;
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
            CCB->State = TS013_ROUTING;
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
         CCB->State = TS013_ALERT;
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
         CCB->State = TS013_ROUTING;        /* Wait for CC_SETUP_COMP_IND */
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
            CCBTS013_RejectSetup( cause );
            }
         CCBTS013_ReleaseCall();
         break;

      case HOST_DISCONNECT_ACK:
         /* Should not happen - if it does, free the CCB.
         */
         CCBTS013_ReleaseCall();
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      case CC_DISCONNECT_IND:             /* Hang-up by the held party */
         if ( CCB->B_Chan )
			CCBTS013_ReleaseChan();
         CCBTS013_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBTS013_process_releaseind(TRUE);
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            OutPkgInfo[ OutPkgInfoLen++ ] = 3;    /* Private IE length */
            OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
            OutPkgInfo[ OutPkgInfoLen++ ] = 1;
            OutPkgInfo[ OutPkgInfoLen++ ] = cause;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBTS013_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      /*
       * ignore info messages
       */
      case CC_INFO_IND:
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

/***************************
* CCBTS013_CallOriginate *
****************************
*
* Process the current event in the context of a voice call in the
* ORIGINATE state awaiting the first response to CC_SETUP_REQ.
* L3 State = 1.
*/
PRIVATE int near CCBTS013_CallOriginate( event )

int *event;                                /* Event input to the state */

{
   int  result_code = NOERR;
   BYTE cause;
   BYTE chan;
   BYTE prog_desc;

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
            CCBTS013_RejectCall( cause );
            CCBTS013_ReleaseCall();
            result_code = CCBTS013_CauseToResult( cause );
            /*
             * Added to stop the dialer carrier timer.
             */
            if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) ) {
               /* total length of private IE's = 0 */
               OutPkgInfo[ OutPkgInfoLen++ ] = 0;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
            break;
            }
         if ((cause = CCBRI_AcceptProgress( &prog_desc )) == INVALID_ELEM_CONTENTS) {
            CCBTS013_RejectCall( cause );
            CCBTS013_ReleaseCall();
            result_code = CCBTS013_CauseToResult( cause );
            /*
             * Added to stop the dialer carrier timer.
             */
            if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) ) {
               /* total length of private IE's = 0 */
               OutPkgInfo[ OutPkgInfoLen++ ] = 0;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }

            break;
            }
         if ( chan )
            CCBTS013_SetNewChan( chan, CHANNEL_BUSY, TRUE );
         if ( CCBRI_GetOutInfo( HOST_INFORMATION ) ) {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            CCB->State = TS013_DIAL;
            }
         break;

      case CC_CALL_PROC_IND:
         /* This message indicates that dialing (en_bloc) is complete.
         */
         if (( cause = CCBRI_AcceptChanId( &chan, &ByteTrash )) != NOERR )
            {
            CCBTS013_RejectCall( cause );
            CCBTS013_ReleaseCall();
            result_code = CCBTS013_CauseToResult( cause );
            /*
             * Added to stop the dialer carrier timer.
             */
            if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) ) {
               /* total length of private IE's = 0 */
               OutPkgInfo[ OutPkgInfoLen++ ] = 0;
               CCBRI_MailtoHost( CCB->CopyPublicMsg );
               }
            }
         else
            {
            /* The B_Channel may be connected already...just be sure.
            */
            if ( chan )
               CCBTS013_SetNewChan( chan, CHANNEL_BUSY, TRUE );
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
             CCB->State = TS013_ROUTING;
            }
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
                  CCBTS013_ReleaseChan();

			   /* Send a RELEASE.  Apparantly violate 5.2.1.3(4) */
         	   if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE )) {
         			/* Build Cause info element (optional in this message) */
         			Diag[ 0 ] = 0;
         			OutPkt->InfoLen +=
         				CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
         					NORMAL_CLEARING, FALSE, Diag );
         			CCBRI_MailtoL3();
         	   }

               if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
                  {
                  /* Total length of private IE's = 0
                  */
                  OutPkgInfo[ OutPkgInfoLen++ ] = 0;
                  CCBRI_MailtoHost( FALSE );
                  CCBTS013_ReleaseCall();
                  }
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

      case HOST_DISCONNECT:
         /* Local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = TS013_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         /* Local disconnect from HOST (response to this message will be
         *  received in RELEASING state.
         */
         CCBRI_HostDisconnect( *event );
         CCB->State = TS013_RELEASE;
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case CC_DISCONNECT_IND:
        /* Far end disconnect.  Send CC_RELEASE_REQ and go to the
         * state TS013_RELEASE.
         */
         if ( CCB->B_Chan )
            CCBTS013_ReleaseChan();
         CCBTS013_DisconnectInd();
         break;

      case CC_RELEASE_IND:
		CCBTS013_process_releaseind(TRUE);
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBTS013_ReleaseChan();
            }
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBTS013_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      /*
       * ignore info messages
       */
      case CC_INFO_IND:
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }

   return( result_code );
}

/*************************
* CCBTS013_CallRelease *
**************************
*
* Process the current event in the context of a voice call in the
* RELEASING (waiting for CC_RELEASE_CONF) state.  The HOST
* is already cleared for this call if there is a HOST.
*
*/
PRIVATE int near CCBTS013_CallRelease( event )

int *event;                                /* Event input to the state */

{
   int result_code = NOERR;

   switch ( *event )
      {
      case CC_RELEASE_IND:
		 CCBTS013_process_releaseind(FALSE);
		/* fall through... */
      case CC_RELEASE_CONF:
         /* added 24feb to get the carrier_wait timer to be destroyed
          */
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) ) {
             /* Total length of private IE's = 0
              */
             OutPkgInfo[ OutPkgInfoLen++ ] = 0;
             CCBRI_MailtoHost( CCB->CopyPublicMsg );
         }
         /* */

         CCBTS013_ReleaseCall();
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
        /*  Local disconnect or disconnect ack from HOST.
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
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

/****************************
* CCBTS013_CallReleaseAck *
*****************************
*
* Process the current event in the context of a voice call in the
* RELEASING_ACK (waiting for CC_RELEASE_CONF) state.  The local user
* is to have the release acknowledged via HOST_DISCONNECT_ACK message
* in this case.
*
*/
PRIVATE int near CCBTS013_CallReleaseAck( event )

int *event;                                /* Event input to the state */
{
   int result_code;


   result_code = NOERR;

   switch ( *event )
      {
	  case CC_DISCONNECT_IND:
		/* we had a DISCONNECT collision */
               if ( CCB->B_Chan )
                  CCBTS013_ReleaseChan();
               CCBTS013_DisconnectInd();
		break;

      case CC_RELEASE_IND:
		CCBTS013_process_releaseind(FALSE);
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* Total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBTS013_ReleaseCall();
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;                                 /* No change in state. */

      case HOST_DISCONNECT:
        /*  Local disconnect from HOST. This is an apparent mutual
         *  disconnect race condition. We will simply ignore it.
         */
         break;

      case HOST_DISCONNECT_ACK:
        /*  Local disconnect ack from HOST. This is an unexpected message.
         *  We will simply change states since the HOST is cleared.
         */
         CCB->State = TS013_RELEASE;
         break;

      case HOST_INFORMATION:
         CCBRI_HostInfo();       /* Process the information from HOST. */
         break;                                 /* No change in state. */

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      /*
       * ignore info messages
       */
      case CC_INFO_IND:
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
            }
         result_code = UNEXPECTED_MSG;
         break;
      }
   return( result_code );
}

/*************************
* CCBTS013_CallRouting *
**************************
*
* Process the current event in the context of a voice call in the
* routing state, the general purpose busy, but not CONVERSATION, state.
* L3 State = 3.
*/
PRIVATE int near CCBTS013_CallRouting( event )

int *event;                                /* Event input to the state */

{
   int  result_code;
   BYTE   prog_desc;
   BYTE   cause;

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
         CCB->State = TS013_ALERT;
         break;

      case HOST_INFORMATION:     /* Process the information from HOST. */
         CCBRI_HostInfo();                      /* No change in state. */
         break;

      case HOST_DISCONNECT:
         CCBRI_HostDisconnect( *event );
         CCB->State = TS013_RELEASE_ACK;
         break;

      case HOST_DISCONNECT_ACK:
         CCBRI_HostDisconnect( *event );
         CCB->State = TS013_RELEASE;
         break;

      case HOST_QUERY:
         CCBRI_HostQuery();        /* Process the query from the HOST. */
         break;

      case CC_ALERTING_IND:
         /* Notification of far end alerting.
          * The B-Channel should already be connected.
          *
          * Now have to check the PROGRESS, it is optional, but we Australia
          * tests this and will send an invalid value.
          */
         if ((cause = CCBRI_AcceptProgress( &prog_desc )) == INVALID_ELEM_CONTENTS) {
            CCBTS013_SendRelease( cause );
            CCBTS013_ReleaseCall();
            result_code = CCBTS013_CauseToResult( cause );
            CCBRI_FreeCallBufs();
            CCB->State = TS013_RELEASE;
            /* Added to remove dialer carrier timer. */
            if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) ) {
              /* total length of private IE's = 0 */
              OutPkgInfo[ OutPkgInfoLen++ ] = 0;
              CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }

            break;
         }

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
		/*  NOTE: the T313 Timeout is also handled here */
        if(InPkt->Info[0] != CONNECT_ACK ) {
           CCB->State = TS013_RELEASE_ACK;
           break;
        }
        /* fall thru */
      case CC_SETUP_CONF:
         /* The call was connected at the far end.  Connect the call
         *  at this end too (its probably already connected) and the
         *  call will transfered to state CONNECTED.
         */
         /* NOTE: these messages correspond to CONNECT_ACK and CONNECT
         *  messages (respectivelly) and we should not have mandatory info
         *  elements to examine here.
		 *  NOTE: the T313 Timeout is also handled above
         */

		if(!CCB->B_Chan) {		/* only check if mandatory */
			/* check for poorly formatted ChanID IE. */
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

         /* Inform the HOST.  */
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
         CCB->State = TS013_CONVERSATION;
         break;

      case CC_DISCONNECT_IND:
         /* Far end disconnect.
         *  state TS013_RELEASE.
         */
         if ( CCB->B_Chan )
             CCBTS013_ReleaseChan();
         CCBTS013_DisconnectInd();
         break;

      case CC_PROGRESS_IND:
         /* Cause is not mandatory,
          * now have to check the PROGRESS, it is optional, but we Australia
          * tests this and will send an invalid value.
          */
		 if ((cause = CCBRI_AcceptProgress( &prog_desc )) != NOERR) {
            CCBTS013_SendRelease( cause );
            CCBTS013_ReleaseCall();
            result_code = CCBTS013_CauseToResult( cause );
            CCBRI_FreeCallBufs();
           /* Added to remove dialer carrier timer. */
           if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) ) {
              /* total length of private IE's = 0 */
              OutPkgInfo[ OutPkgInfoLen++ ] = 0;
              CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }

            CCB->State = TS013_RELEASE;
         }
         break;

      case CC_RELEASE_IND:
		CCBTS013_process_releaseind(TRUE);
		/* fall through... */
      case CC_RELEASE_CONF:
         if ( CCB->B_Chan )
            {
            CCBTS013_ReleaseChan();
            }
         if ( CCBRI_GetOutInfo( HOST_DISCONNECT_ACK ) )
            {
            /* total length of private IE's = 0
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = 0;
            CCBRI_MailtoHost( CCB->CopyPublicMsg );
            }
         CCBTS013_ReleaseCall();
         break;

      case CC_STATUS_IND:
         CCBRI_StatusInd();
         break;

      /*
       * ignore info messages
       */
      case CC_INFO_IND:
         break;

      default:
         if (( Source == L3_ID ) && ( InPkt ) && ( InPkt->Info ))
            {
            CCBTS013_UnexpectedMsg( InPkt->Info[ 0 ] );
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
/*************************
* CCBTS013_CauseToResult *
**************************
*
* Convert a CCITT cause value into an error result code.
* Note that all legal causes are not supported, only those generated.
*/
PRIVATE int near CCBTS013_CauseToResult( cause )

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

/***************************
* CCBTS013_DecodeCallState *
****************************
*
* Decode the state of the CCB from the CCB state variable.
* Dispatch the appropriate state function through the state function
* jump table.
*
*/
PUBLIC int CCBTS013_DecodeCallState( event )

int *event;                                /* Event input to the state */

{
   int result_code;

  /*  Perform a range check on the value of CCB->State to prevent
   *  the strange and unusual.
   */
   if (( CCB->State > TS013_STATE_LOW_LIMIT ) &&
       ( CCB->State < TS013_STATE_HIGH_LIMIT ))
      {
      /* Enter state function.
      */
      result_code = ( *Ccb_State_Func_Ts013[ CCB->State ] )( event );
      }
   else
      {
      CCBRI_Error( CCBRI_DECODE_CALL_STATE, INVALID_STATE, CCB->State,
                   *event, TRUE );
      result_code = INVALID_STATE;
      }

   return( result_code );
}

/*************************
* CCBTS013_DisconnectInd *
**************************
*
* Generic action upon receiving the CC_DISCONNECT_IND message.
*
*/
PRIVATE BOOLEAN near CCBTS013_DisconnectInd()

{
   BYTE cause = 0;                     /* Locally-detected error cause */
   BYTE host_event = HOST_DISCONNECT;      /* Event to be sent to HOST */
   BYTE valid_ie = 0;                /* for Australian test #T3-10-112 */

/* Let's change the order here. We will send the CC_RELEASE_REQ first,
 * then the STATUS. Having problems with the call state and knowing
 * what it should actually be. If we request the RELEASE to be sent out,
 * then we almost for sure should be in state 19 (RELEASE_REQUEST).
 * First, we will check the cause. If is is bad, send the status, if no
 * don't send it.
 *
 * Have to check for unimplemented IEs in the message.
 */
   if ((valid_ie = CCBTS013_VerifyDiscIEs (InPkt->Info, InPkt->InfoLen)) != NOERR) {
       cause = BAD_INFO_ELEM;
   } else {
       /*
        * Fix to only send a RELEASE COMPLETE with the proper cause value,
        * don't send the STATUS message after the Release Complete.
        */
       cause = CCBRI_AcceptCause( &cause );
       valid_ie = CAUSE_IE;
   }

   if ( CCBRI_GetOutPkt ( CC_RELEASE_REQ, FALSE ) )
     {
     /* code to add the cause value */
      if (cause != NOERR) {
        Diag[ 0 ] = valid_ie;
        Diag[ 1 ] = 0;
        OutPkt->InfoLen +=
           CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                cause, FALSE, Diag );
      }

	  CCBRI_MailtoL3();
	  CCB->State = TS013_RELEASE;
	  }

   CCB->Cause  = (int ) cause;
   CCBRI_DisconnectHostCall( &host_event, cause );

   return ( TRUE );
}

/************************
* CCBTS013_RejectCall *
*************************
*
* Reject a call due to flaws or incompatibilities in the associated message
* received from the peer.  The function CCBTS013_RejectSetup is only used for
* rejecting the CC_SETUP_IND and this function is used otherwise.
*
*/
PRIVATE int near CCBTS013_RejectCall( cause )

BYTE cause;                                    /* Reason for rejection */

{
   int result_code;
   result_code = NOERR;

   /* We request L3 to send RELEASE_COMP directly.
    */
   if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ))
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

/*************************
* CCBTS013_RejectSetup *
**************************
*
* This function is called as the first response to the CC_SETUP_IND
* when the call is being rejected.
*
*/
PRIVATE void near CCBTS013_RejectSetup( cause )

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

/*************************
* CCBTS013_ReleaseCall *
**************************
*
* Terminate a call and put the CCB to bed.
*
*/
PUBLIC void CCBTS013_ReleaseCall()

{

   /* Perform actions specific to the voice call termination.
   */
   if ( CCB->B_Chan )
      CCBTS013_ReleaseChan();

   CCBRI_RemoveCCB( CCB );

   return;
}

/*************************
* CCBTS013_ReleaseChan *
**************************
*
* Remove a B-Channel from allocation by a CCB.
*
*/
PRIVATE void near CCBTS013_ReleaseChan()

{
   if ( CC_CHAN_GetUserbri( CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID ) == CCB->Call_ID )
      {
      CC_CHAN_ReleaseChanbri( CCB->B_Chan, B_CHANNEL_UNITS, Dsl_ID );
      }
   CCB->B_Chan = 0;
}

/**************************
* CCBTS013_ReleasedLink *
***************************
*
* The data link is down at Layer 2.
* Remove CCBs from any context but LOCKOUT.  Users left OFF HOOK
* must retain a CCB in the LOCKOUT state to process their hangup.
*
* It is specified that calls in the ACTIVE state should not be disturbed.
*
*/
PUBLIC void CCBTS013_ReleasedLink (ces, dsl_id)
BYTE ces;          /* Connection Endpoint Suffix of the released link. */
BYTE dsl_id;                                /* Digital Subscriber Loop */
{
   CCB_ID tmp_ccb;

   Dsl_ID = dsl_id; /* This assumption is made in CCBTS013_ReleaseCall(). */
   /*
    * Start at the head and remove all impacted CCBs.
    */
   CCB = CCB_Head[dsl_id];
   while ( CCB ) {
      tmp_ccb = CCB->Next;
      if (CCB->Ces == ces) {
         if (CCB->State != TS013_CONVERSATION) {
            if (CCBRI_GetOutInfo( HOST_DISCONNECT_ACK)) {
               /* total length of private IE's = 0
               */
               OutPkgInfo[OutPkgInfoLen++] = 0;
               CCBRI_MailtoHost(FALSE);
            }
            CCBTS013_ReleaseCall();
         }
      }
      CCB = tmp_ccb;   /* Skip to the next one. */
   }
}

/*************************
* CCBTS013_SendRelease *
**************************
*
* Reject a call due to flaws or incompatibilities in the associated message
* received from the peer. Will request L3 to send out a RELEASE message.
* Added for Australia.
*/
PRIVATE int near CCBTS013_SendRelease( cause )

BYTE cause;                                    /* Reason for rejection */

{
   int result_code;

   result_code = NOERR;

   /* We request L3 to send RELEASE directly.
   */
   if ( CCBRI_GetOutPkt( CC_RELEASE_REQ, FALSE ))
      {
      /* Build the RELEASE message.
      */
      Diag[ 0 ] = 0;  /* Diagnostic is only relevant in STATUS message */
      OutPkt->InfoLen +=
         CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ], cause,
                             FALSE, Diag );
      CCBRI_MailtoL3();                        /* Send CC_RELEASE_REQ */
      }
   else
      {
      result_code = OUT_OF_PKTS;
      }

   return ( result_code );
}

/************************
* CCBTS013_SetNewChan *
*************************
*
* Establish a new B-Channel in the CCB.
*
*/
PRIVATE BYTE near CCBTS013_SetNewChan( b_chan, state, overwrite )

int     b_chan;                                    /* B-Channel to set */
int     state;                                /* New state for channel */
BOOLEAN overwrite; /* May a chan in the PROPOSED state be overwritten? */
{
   BYTE   cause;

   cause = CCBRI_SetNewChan( b_chan, state, overwrite );

   return( cause );
}


/**********************
* CCBTS013_SetupInd *
***********************
*
* Handle the CC_SETUP_IND message while IDLE.
* This process has been made into an independent function because
* of its inherent complexity.  This process is complicated by the
* possible presence and interraction of the Call Waiting and Call
* Deflection and UUI Supplementary Services where they are activated.
*
*/
PRIVATE int near CCBTS013_SetupInd()

{
   BYTE   cause;
   BYTE   chan;
   int    length = 0;
   BYTE   exclusive = 0;
   int    result_code = NOERR;
   int    k;
   BYTE   prog_desc;

  /*  A new call SETUP from the far end. The CC may send
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
   if (( cause = CCBRI_AcceptBearerCap()) != NOERR )
      {
         /* This is a mandatory element.  The call must be rejected.
         */
         CCBTS013_RejectSetup( cause );
         CCBTS013_ReleaseCall();
         result_code = CCBTS013_CauseToResult( cause );
		 CCBRI_FreeCallBufs();
         goto in_call_failed;
      }

   /* Next we look at the Channel ID.
   */
   if (( cause = CCBRI_AcceptChanId( &chan, &exclusive )) != NOERR )
      {
      CCBTS013_RejectSetup( cause );

      CCBTS013_ReleaseCall();
      result_code = CCBTS013_CauseToResult( cause );
	  CCBRI_FreeCallBufs();
      goto in_call_failed;
      }
	
   if ( chan )
      {
      CCBTS013_SetNewChan( chan, CHANNEL_BUSY, TRUE );
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
	  *	Except that NO_CHANNEL is defined for the Chan ID info element,
	  *	not Cause.  They want NO_CIRCUIT_AVAILABLE, so I changed it.
      */
      CCBRI_RejectCall(NO_CIRCUIT_AVAILABLE, 0);	/* see comment above */
      CCBTS013_ReleaseCall();
      result_code = CCBTS013_CauseToResult( cause );
	  CCBRI_FreeCallBufs();
      goto in_call_failed;
      }

   /* Now have to check the PROGRESS, it is optional, but we Australia
    * tests this and will send an invalid value.
    */
   if ((cause = CCBRI_AcceptProgress( &prog_desc )) == INVALID_ELEM_CONTENTS)
      {
      CCBTS013_RejectSetup( cause );
      CCBTS013_ReleaseCall();
      result_code = CCBTS013_CauseToResult( cause );
	  CCBRI_FreeCallBufs();
      goto in_call_failed;
      }
   k+= length;

   /*  Next attempt to match the called address to our own or to recover it
    *  for the Host.
    */
   if (( cause = CCBRI_AcceptCalledNumb( &length, Dsl_ID )) != NOERR )
      {
      /* This is an optional element, but if present, the number must
      *  match the terminal's number and the element must be coded correctly.
      *  Otherwise ignore the call.
      */
      if ( cause != MANDATORY_IE_MISSING )     /* Element coding error */
         {
         CCBTS013_RejectSetup( cause );
         CCBTS013_ReleaseCall();
         result_code = CCBTS013_CauseToResult( cause );
	     CCBRI_FreeCallBufs();
         goto in_call_failed;
         }
   } else
         k+= length;

  /* NOTE: Here tests may be inserted for checking the optional
   *  Lower Layer and Higher Layer compatability information elements
   *  and any other elements of concern.
   */

   OutPkgInfo[ OutPkgInfoLen - k - 1 ] = k;
   CCBRI_MailtoHost( CCB->CopyPublicMsg );
   CCB->State = TS013_OFFERED;

in_call_failed:
   return ( result_code );
}

/**********************
* CCBTS013_Shutdown *
***********************
*
* Reset Call Control
*
*/
PUBLIC void CCBTS013_Shutdown (dsl)
BYTE dsl;
{
    CCB_ID temp_ccb;

    Dsl_ID = dsl;
    CCB = CCB_Head[dsl];
    while (CCB) {
        temp_ccb = CCB->Next;
        CCBTS013_ReleaseCall();
        CCB = temp_ccb;
    }
}

/*************************
* CCBTS013_VerifyDiscIEs *
**************************
*
* This function returns 0 if all info elements were properly recognized.
* Otherwise, it returns the IE_ID of the FIRST unrecognized info element
* in the info buffer.
*
*/
PUBLIC BYTE CCBTS013_VerifyDiscIEs( info_buffer, info_length )

   BYTE    *info_buffer;
   int      info_length;

{
   BOOLEAN recognized = TRUE;
   BYTE    current_ie = 0;
   int     current_offset;        /* pointer to a current info element */

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

/***************************
* CCBTS013_UnexpectedMsg *
****************************
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
PRIVATE void near CCBTS013_UnexpectedMsg( diagnostic )

BYTE diagnostic;                    /* The message type received by L3 */
{
   CCBRI_RejectMsg( WRONG_MESSAGE, diagnostic );
   /* Other action(s) appropriate to the specific application may be added
   *  here.
   */
   return;
}
#endif
