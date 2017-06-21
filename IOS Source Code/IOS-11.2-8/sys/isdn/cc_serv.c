/* $Id: cc_serv.c,v 3.2.60.1 1996/04/30 23:06:38 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/cc_serv.c,v $
 *------------------------------------------------------------------
 * cc_serv.c  ISDN Call Control Service Message Utilities 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cc_serv.c,v $
 * Revision 3.2.60.1  1996/04/30  23:06:38  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:38:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:12:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       cc_serv.c                                               
DESCRIPTION:  ISDN Call Control Service Message Utilities             
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


 1.  Scope

#ifdef PRI_4ESS
This module is intended to provide a Primary Rate Interface Call
Control Task with support for the maintenance states found in AT&T
Technical Reference (Pub Number 41449) AT&T Integrated Services Digital
Network (ISDN) Primary Rate Interface Specification dated July, 1989,
Part IV, Sec. 10, AT&T Maintenance and Management.

#endif
#ifdef PRI_5ESS
This module is intended to provide a Primary Rate Interface Call
Control Task with support for the maintenance states found in AT&T
Primary Rate Interface Specification (5E5), Document Number 5D5-900-312
Part III Section 1.5 (SERVICE).

#endif
#ifdef PRI_DMS100
This module is intended to provide a Primary Rate Interface Call Control
Task with support for the maintenance function found in Northern Telecom
Primary Rate User-Network Interface Specification NIS A211-1, Ver. 2,
Dec., 1989, Sec. B, Part 6, for the DMS100.

#endif
This module partially implements the Service Message SDL's for the
B-Channel service states.  As the management of B-Channels is seen to
be a Call Control function, these channel states and the utilities
necessary to manipulate them are seen to be functions of Call Control.

The functions contained in this module are designed to be integrated, by
a Telenetworks customer, into his/her Call Control task in whatever manner
seems appropriate.  These utilities may be maintained as a separate
module which may prove advantageous to the user if Telenetworks issues
a maintenance release or enhancements to these routines.


2.  The Code Environment

2.1  The B-Channel Management Table

As other B-Channel management functions must be performed by a Call
Control independently of the Service states (most specifically channel
assignment for new calls), it is assumed by the CC_SERV module that
some form of B-Channel Management Table already exists in the Call
Control.  In this table must be provided a BYTE value for the service
state for each B-Channel and an independent BYTE value for the call state,
or this information must be maintained as an extension to previously-
defined channel states.  Provided within this module (CC_SERV.C) are
certain access routines to this table which exist here as STUB functions.
They assume the usage of the Telenetworks channel table function
implemented by CC_CHAN.C.  IT IS THE RESPONSIBILITY OF THE USER OF THIS
MODULE (CC_SERV) TO COMPLETE THESE FOUR FUNCTIONS IN A MANNER WHICH
MATCHES HIS B-CHANNEL MANAGEMENT DATA STRUCTURES OR OTHER ACCESS IF
CC_CHAN IS NOT USED:

   a.  CC_SERV_GetState
   b.  CC_SERV_SetState
   c.  CC_SERV_GetCallState
   d.  CC_SERV_SetCallState

In addition, it is assumed that the user's Call Control INITIALIZES the
service state for each channel as the system is started up.


2.2  Message Interface

Decoding Maintenance Messages -

In L3.C, maintenance messages arriving from the ISDN peer (i.e. from
below) are now distinguished from call-related messages by a distinct
Protocol Discriminator (03 versus 08), which is the first byte in the
Layer 3 message (the info buffer).  In the functions L3_Go and
L3_GetInfo, the maintenance Protocol Discriminator is detected and the
message is rerouted from the normal path to the function L3_MaintMsg.
In this function, the message primitive is redefined.  If the message
is from Layer 2 (i.e. from the ISDN peer), the message is now assigned
the NL_MAINT_IND primitive and it is mailed to Call Control.  If the
message bears the NL_MAINT_REQ primitive (i.e. it was originated by
the CC_SERV module in Call Control) it is assigned the DL_DATA_REQ
primitive and mailed to Layer 2.  Thus Protocol Control just "readdresses
the envelope and forwards the mail" for maintenance messages.  It does
not change the message type field.

It is the RESPONSIBILITY OF THE TELENETWORKS CUSTOMER'S CALL CONTROL
to decode these messages received from Protocol Control and to dispatch
the SERVICE and SERVICE_ACK messages to the CC_SERV module.

A Protocol Discriminator Issue -

The message may not be decoded within Call Control by examining the
Protocol Discriminator.  This is because L3 Protocol Control overwrites
the Protocol Discriminator to be equal to the message type it received
from Layer 2 when it resends call-related messages to Call Control.  For
example, if Protocol Control receives a SETUP message, it copies the value
for SETUP to the Protocol Discriminator field and rewrites the message
type to CC_SETUP_IND.  This action is performed to give Call Control
additional information in call-related messages.  This action occurs in
the function L3_Mail within the dest_id "switch" statement.  As a result
of this action the Protocol Discriminator may be an unreliable flag to
Call Control and, therefore, two new primitive types have been defined
for maintenance messages between Call Control and Protocol Control
(NL_MAINT_IND and NL_MAINT_REQ).

Call Control MUST DETECT MAINTENANCE MESSAGES by reference to the
message primitive (NL_MAINT_IND) and route them to the CC_SERV module.
The entry function to CC_SERV.C, CC_SERV_ServiceMsg, will decode the
message type for the SERVICE and SERVICE_ACK messages.  SERVICE messages
and solicited SERVICE_ACK messages will be acted upon, unsolicited
SERVICE_ACK messages are treated in compliance with published vendor
specs.  All other message types are rejected (LIF_Error is called) by
the CC_SERV module.  If other maintenance messages are of interest to
Call Control, it should decode them independently and direct them to
routines other than those in this module.

Message Buffer Management -

The CC_SERV utilities will NOT be responsible for freeing the incoming
message primitive or packet.  This is the responsibility of functions
which call these utilities.  If the CC_SERV utilities send mail, they will
get their own packet and primitive to perform this function.  These will
be freed in the normal course of actions in the lower layers as the
I-frame is sent.

Sending Maintenance Messages -

These Service message utilities must send SERVICE and SERVICE_ACK responses
to the ISDN peer.  Thus there must be a capability of building and mailing
them.  As the nature of the mail interface existing in a customer-written
Call Control is unknown to these utilities, they take the liberty of
defining their own (CC_SERV_Mail), which fills out the message header and
uses the standard mail interface, LIF_SendPkt.  If this is not acceptable
to the customer-written Call Control, this function must be performed
in some other way by the user of this code.

Call Reference -

The Call Reference assigned to maintenance massages is the Global Call
Reference.  Counting its length field, the Global Call Reference may be
either two or three bytes long, irrespective of the call reference length
being experienced in the call setup and teardown protocol.  CC_SERV.C
captures the SERVICE message call reference on a per-message basis and
responds with the value supplied by the opposite end.

Flag bit inversion within the Call Reference occurs in Protocol Control
before a maintenance message is forwarded to Call Control.  Call Control
should not tamper with the Call Reference of Service messages as
CC_SERV_Mail copies it for retransmission in its output messages.

In the case of locally-initiated SERVICE messages the defined constant,
GLOBAL_CREF_LEN, is assumed.


3. Maintenance Reports to Call Control

Whenever a valid SERVICE message or a valid, solicited SERVICE_ACK
message is processed, whether or not the state of the channel has
actually changed, the function CC_SERV_ServiceMsg returns the channel
number involved.  This report value is intended as confirmation to Call
Control that the maintenance message was honored.


4. Locally-Initiated Channel Maintenance

This service message module provides the capability for Call Control
to initiate a change in the maintenance condition on a channel.  Also
limited sanity checking has been implemented concerning the call state
of the channel.  Unless the channel is found to be CHANNEL_IDLE or
CHANNEL_MAINT_PEND from a call-perspective, the action is refused.

The action is begun from Call Control by calling CC_SERV_ChangeRequest().
A channel which is undergoing a change in maintenance state has its
CALL STATE re-written from CHANNEL_IDLE to CHANNEL_MAINT_PEND, thus
this implementation assumes the presence the Channel Management Table
from CC_CHAN.C or one offering comparable service and call state fields.
If the request may not be honored, CC_SERV_ChangeRequest() returns
FALSE.  Otherwise the appropriate SERVICE message is composed and sent.

Timing -

If the request may be honored, the "time" parameter is tested.  If TRUE,
an LIF timer is added as required and is started for the channel using 
the "timer ID" from the Channel Management Table (TimerID). The timer is 
initialized specifying CC_ID as the timeout report destination and 
timeouts are handled in the Call Control module along with other mail.  
This method is chosen so that CC_SERV does not need to be a task with a 
mailbox of its own.  This timing function may be used to implement, for 
example, AT&T T3M1.  Access routines are provided in CC_CHAN.C to 
manipulate the timer ID.

Service Ack -

When the SERVICE_ACK message is received, the call state of the channel
is inspected to insure that the state is CHANNEL_MAINT_PEND.  Otherwise
the SERVICE_ACK message is treated as unsolicited.  Upon receiving a
solicited SERVICE_ACK, if the timer had been started previously, it is
stopped.

This state information (CHANNEL_MAINT_PEND) is not coded into the
channel's maintenance state field since there are only three values
defined by the switch vendors and they have end-to-end significance.

If timeout occurs with no far end response, the service state of the 
channel is lowered to that being requested if it is lower than the 
previous state and the SERVICE message is sent periodically, forever,
or until the far end responds.


5.  CCIE.C

The CC_SERV module also uses information element utilities in the module
CCIE.C.  This module must also be linked into any Call Control task
using CC_SERV.C.


6.  Channel ID Info Element

As an addition to this module, certain integrity tests have been added
within the function CC_SERV_ServiceMsg to validate the contents of the
Channel ID info element.  These checks may require minor changes based
upon the application.  The relevant switch vendor specifications should
be consulted if the default values assumed here are inappropriate.

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/

#define     CC_SERV_C                                   /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/
/* """
#include    <stdio.h>
""" */
#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "gp.h"  /*===*/
#include    "ccbri.h"
#include    "cc_chan.h"
#include    "ccpri.h"
#include    "ccie.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
#define GLOBAL_CREF_LEN             1                 /* Assumed value */

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
PRIVATE PKT_ID In_Pkt;              /* Incoming service message packet */
PRIVATE BYTE   DSL_Id;                   /* Digital Subscriber Loop ID */
PRIVATE int    Cref;               /* Storage for call reference value */
PRIVATE int    CrefLen;           /* Storage for call reference length */
BYTE          Int_Id;                          /* Channel Interface ID */

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

PRIVATE BOOLEAN near CC_SERV_Complete      (BYTE, BYTE, BYTE);
PRIVATE BYTE    near CC_SERV_GetCallState  (BYTE);
PRIVATE BYTE    near CC_SERV_GetState      (BYTE);
PRIVATE BOOLEAN near CC_SERV_InServ        (BYTE, BYTE, BYTE);
PRIVATE void    near CC_SERV_Mail          (PKT_ID, BYTE);
PRIVATE BOOLEAN near CC_SERV_Maint         (BYTE, BYTE, BYTE);
PRIVATE BOOLEAN near CC_SERV_OutOfServ     (BYTE, BYTE, BYTE);
PRIVATE void    near CC_SERV_SetCallState  (BYTE, BYTE);
PRIVATE BOOLEAN near CC_SERV_SetState      (BYTE, BYTE);


/************************
* CC_SERV_ChangeRequest *
*************************
*
* Call Control-initiated channel maintenance action.
*
*/
PUBLIC BOOLEAN CC_SERV_ChangeRequest( int_id, chan, dsl_id, new_state, time, 
                                      event, interval )

BYTE    int_id;                                        /* Interface ID */
int     chan;                                    /* B-channel to alter */
int     dsl_id;                          /* Digital Subscriber Loop ID */
int     new_state;                                /* New service state */
BOOLEAN time;                              /* Start maintenance timer? */
INT16   event;                            /* Reported event on timeout */
UINT32  interval;                                   /* Timing interval */

{
   int call_state;
   int timer_id;

   DSL_Id  = dsl_id;
   Int_Id  = int_id;
   Cref    = 0;
   CrefLen = GLOBAL_CREF_LEN;

   call_state = CC_SERV_GetCallState( chan );
   if ((( call_state == CHANNEL_IDLE ) ||
        ( call_state == CHANNEL_MAINT_PEND )) &&
       ( CC_SERV_Complete( SERVICE, chan, new_state ) ))
      {
      CC_SERV_SetCallState( chan, CHANNEL_MAINT_PEND );
      if ( time )
         {
         if (( timer_id = CC_CHAN_GetTimerID( Int_Id, chan, DSL_Id )) == -1 )
            {
            if (( timer_id = LIF_AddTimer( CC_ID, CC_ID, event,
                                           DSL_Id, 0, Int_Id, chan, 0,
                                           interval )) != ERR )
               {
               CC_CHAN_SetTimerID( Int_Id, chan, timer_id, DSL_Id );
               LIF_StartTimer( timer_id );
               }
            else
               {
               chan = 0;                                    /* Failure */
               }
            }
         else
            {
            LIF_StartTimer( timer_id );
            }
         }
      else if (( timer_id = CC_CHAN_GetTimerID( Int_Id, 
                                                chan, 
                                                DSL_Id )) != NOT_TIMING )
         {
         if ( LIF_TimerRunning( timer_id ) )
            LIF_StopTimer( timer_id );
         LIF_RemoveTimer( timer_id );
         CC_CHAN_SetTimerID( Int_Id, chan, NOT_TIMING, DSL_Id );
         }
      }
   else                                        /* Cannot alter channel */
      {
      chan = 0;
      }

   return( chan != 0 );
}


/*******************
* CC_SERV_Complete *
********************
*
* Complete the processing for a SERVICE or SERVICE_ACK message,
* building and sending the request or response.
*
*/
PRIVATE BOOLEAN near CC_SERV_Complete( msg, data1, data2 )

BYTE msg;                                      /* Message type to send */
BYTE data1;                            /* Generic data for the message */
BYTE data2;                            /* Generic data for the message */

{
    BOOLEAN no_error;                                    /* Error flag */
    PKT_ID  pkt;                       /* The outgoing message packet. */
    BYTE    diag[ 2 ];

    /* Get a packet for the response.
    */
    no_error =
        (( pkt = LIF_GetPkt( SML_INFO_BTYPE, DATA_OFFSET, TRUE )) != NULL );

    if ( no_error )                      /* Start building the message */
        {
        pkt->InfoType  = SML_INFO_BTYPE;
        if ( CrefLen == 2 )
           pkt->InfoLen = 5;             /* Account for message header */
        else
           pkt->InfoLen = 4;             /* Account for message header */

        if (( msg == SERVICE ) || ( msg == SERVICE_ACK ))
            {
            /* Build mandatory Change Status and Channel ID info elements.
            *  data1 = channel.
            *  data2 = new state.
            */
            pkt->InfoLen +=
                CCIE_BuildChangeStatus_IE( &pkt->Info[ pkt->InfoLen ],
                                           CHANNEL_PREF, data2 );
            pkt->InfoLen +=
                CCIE_BuildChanId_IE( &pkt->Info[ pkt->InfoLen ], 1, data1,
                                     B_CHANNEL_UNITS, FALSE, TRUE, 1, 
                                     &Int_Id );
#ifdef DEBUG
            printf( "CC_SERV_Complete:   Channel = %d, Final state   = %d\n",
                    data1, data2 );
#endif
            }
        else                /* msg == CC_STATUS_REQ or CC_REL_COMP_REQ */
            {
            /* The cause value has been passed in the "new_state."
            *  data1 = cause.
            *  data2 = cause diagnostic.
            */
            diag[ 0 ] = data2;
            diag[ 1 ] = 0;
            pkt->InfoLen += CCIE_BuildCause_IE( &pkt->Info[ pkt->InfoLen ],
                                                data1, FALSE, diag );
            if ( msg == CC_STATUS_REQ )
               {
               pkt->InfoLen += 
                  CCIE_BuildCallState_IE( &pkt->Info[ pkt->InfoLen ], 0 );
               }            
            }
        CC_SERV_Mail( pkt, msg );                 /* Send the response */
        }
    else
        {
#ifdef DISPLAY_LOCAL_ERR
        LIF_Error ("cc_serv", "CC_SERV_Complete", "Couldn't get a packet.")
#endif
        ;
        }
    return ( no_error );
}


/***********************
* CC_SERV_GetCallState *
************************
*
* Recover the service state of a channel.
*
* NOTE: This function is a STUB and must be completed for
*       each application based upon the channel management
*       datastructures in use.
*
*/
PRIVATE BYTE near CC_SERV_GetCallState( chan )

BYTE chan;                                    /* B-Channel of interest */

{
    return ( CC_CHAN_GetStatepri( Int_Id, chan, B_CHANNEL_UNITS, DSL_Id ) );
}


/*******************
* CC_SERV_GetState *
********************
*
* Recover the service state of a channel.
*
* NOTE: This function is a STUB and must be completed for
*       each application based upon the channel management
*       datastructures in use.   The valid states are:
*
*       IN_SERVICE     - 0
*       MAINTENANCE    - 1
*       OUT_OF_SERVICE - 2
*
*/
PRIVATE BYTE near CC_SERV_GetState( chan )

BYTE chan;                                    /* B-Channel of interest */

{
    return ( CC_CHAN_GetServState( Int_Id, chan, B_CHANNEL_UNITS, DSL_Id ) );
}


/*****************
* CC_SERV_InServ *
******************
*
* The in-service state function for a channel.
*
*/
PRIVATE BOOLEAN near CC_SERV_InServ( msg, chan, new_state )

BYTE msg;                                 /* The incoming message type */
BYTE chan;                                       /* In service channel */
BYTE new_state;                   /* New service state for the channel */

{
    BOOLEAN no_error;
    int     timer_id;


    if ( msg == SERVICE )
        {
        switch ( new_state )
            {
            case IN_SERVICE:
                no_error = CC_SERV_Complete( SERVICE_ACK, chan, IN_SERVICE );
                break;
            case MAINTENANCE:
            case OUT_OF_SERVICE:
                CC_SERV_SetState( chan, new_state );
                no_error = CC_SERV_Complete( SERVICE_ACK, chan, new_state );
                break;
            default:                                  /* Unknown state */
                no_error = FALSE;
                break;
            }
        }
    /* msg == SERVICE_ACK
    */
    else if ( CC_SERV_GetCallState( chan ) == CHANNEL_MAINT_PEND )
        {
        /* Response to SERVICE message request.
        */
        CC_SERV_SetCallState( chan, CHANNEL_IDLE );
        CC_SERV_SetState( chan, new_state );
        if (( timer_id = CC_CHAN_GetTimerID( Int_Id, 
                                             chan, DSL_Id )) != NOT_TIMING )
           {
           LIF_StopTimer( timer_id );
           LIF_RemoveTimer( timer_id );
           CC_CHAN_SetTimerID( Int_Id, chan, ERR, DSL_Id );
           }
        no_error = TRUE;
        }
    else
        {
        /* msg == unsolicited SERVICE_ACK
        */
        switch ( new_state )
            {
            case IN_SERVICE:
                no_error = TRUE;                         /* Just a NOP */
                break;
            case MAINTENANCE:
            case OUT_OF_SERVICE:
                no_error = CC_SERV_Complete( SERVICE, chan, IN_SERVICE );
                break;
            default:                                  /* Unknown state */
                no_error = FALSE;
                break;
            }
        }

    return ( no_error );
}


/***************
* CC_SERV_Mail *
****************
*
* Send a message to Layer 3.
* All the fields subsequent to the msg_type have already been composed.
* The InfoLen should already be correct.
*
*/
PRIVATE void near CC_SERV_Mail( pkt, msg )

PKT_ID pkt;                                     /* The outgoing packet */
BYTE   msg;                                            /* The msg_type */

{
   pkt->HeadLen   = DATA_OFFSET;                                  /* 4 */
   /* Write Protocol Discriminator 
   */
   pkt->Info[ 0 ] = 
      (( msg == SERVICE ) || ( msg == SERVICE_ACK )) ? MaintProto : Qdot931;
   pkt->Info[ 1 ] = (BYTE) CrefLen;              /* CR length = 1 or 2 */
   if ( CrefLen == 2 )
      {
      pkt->Info[ 2 ] = (BYTE) ( Cref >> 8 );         /* Call reference */
      pkt->Info[ 3 ] = (BYTE) Cref;                  /* Call reference */
      pkt->Info[ 4 ] = msg;                              /* Message ID */
      }
   else                         /* Single byte (Global) call reference */
      {
      pkt->Info[ 2 ] = (BYTE) Cref;                  /* Call reference */
      pkt->Info[ 3 ] = msg;                              /* Message ID */
      }

   /* Mail the packet.
   */
   LIF_SendPkt( L2P_LAPD, CC_ID, 0, L3_ID, 
                ((msg==SERVICE) || (msg==SERVICE_ACK)) ?
                   NL_MAINT_REQ : NL_DATA_REQ, 
                DSL_Id,
                Q931_SAPI, 1, 0, 0, Q931_DSL_CHAN_ID, TRUE, pkt, NOW,
                REGULAR_MAIL);
}


/****************
* CC_SERV_Maint *
*****************
*
* The maintenance state function for a channel.
*
*/
PRIVATE BOOLEAN near CC_SERV_Maint( msg, chan, new_state )

BYTE msg;                                 /* The incoming message type */
BYTE chan;                                   /* Channel in maintenance */
BYTE new_state;                   /* New service state for the channel */

{
    BOOLEAN no_error;
    int     timer_id;


    if ( msg == SERVICE )
        {
        switch ( new_state )
            {
            case IN_SERVICE:
                /* Attempt to restore service...the Call Control may
                *  reject this plan.
                */
                if ( !CC_SERV_SetState( chan, IN_SERVICE ) )
                    new_state = MAINTENANCE;
                no_error = CC_SERV_Complete( SERVICE_ACK, chan, new_state );
                break;
            case MAINTENANCE:
                no_error = CC_SERV_Complete( SERVICE_ACK, chan, MAINTENANCE );
                break;
            case OUT_OF_SERVICE:
                CC_SERV_SetState( chan, OUT_OF_SERVICE );
                no_error = CC_SERV_Complete( SERVICE_ACK, chan, OUT_OF_SERVICE );
                break;
            default:                                  /* Unknown state */
                no_error = FALSE;
                break;
            }
        }
    /* msg == SERVICE_ACK
    */
    else if ( CC_SERV_GetCallState( chan ) == CHANNEL_MAINT_PEND )
        {
        /* Response to SERVICE message request.
        */
        CC_SERV_SetCallState( chan, CHANNEL_IDLE );
        CC_SERV_SetState( chan, new_state );
        if (( timer_id = CC_CHAN_GetTimerID( Int_Id, 
                                             chan, DSL_Id )) != NOT_TIMING )
           {
           LIF_StopTimer( timer_id );
           LIF_RemoveTimer( timer_id );
           CC_CHAN_SetTimerID( Int_Id, chan, ERR, DSL_Id );
           }
        no_error = TRUE;
        }
    else
        {
        /* msg == unsolicited SERVICE_ACK
        */
        switch ( new_state )
            {
            case MAINTENANCE:
                no_error = TRUE;                         /* Just a NOP */
                break;
            case IN_SERVICE:
            case OUT_OF_SERVICE:
                no_error = CC_SERV_Complete( SERVICE, chan, MAINTENANCE );
                break;
            default:                                  /* Unknown state */
                no_error = FALSE;
                break;
            }
        }

    return ( no_error );
}


/********************
* CC_SERV_OutOfServ *
*********************
*
* The out-of-service state function for a channel.
*
*/
PRIVATE BOOLEAN near CC_SERV_OutOfServ( msg, chan, new_state )

BYTE msg;                                 /* The incoming message type */
BYTE chan;                                   /* Out of service channel */
BYTE new_state;                   /* New service state for the channel */

{
    BOOLEAN no_error;
    int     timer_id;


    if ( msg == SERVICE )
        {
        switch ( new_state )
            {
            case IN_SERVICE:
                /* Attempt to restore service...the Call Control may
                *  reject this plan.
                */
                if ( !CC_SERV_SetState( chan, IN_SERVICE ) )
                    {
                    if ( CC_SERV_SetState( chan, MAINTENANCE ) )
                        new_state = MAINTENANCE;
                    else
                        new_state = OUT_OF_SERVICE;
                    }
                no_error = CC_SERV_Complete( SERVICE_ACK, chan, new_state );
                break;
            case MAINTENANCE:
                /* Attempt to restore to maintenance...the Call Control
                *  may reject this plan also.
                */
                if ( !CC_SERV_SetState( chan, MAINTENANCE ) )
                        new_state = OUT_OF_SERVICE;
                no_error = CC_SERV_Complete( SERVICE_ACK, chan, new_state );
                break;
            case OUT_OF_SERVICE:
                no_error = CC_SERV_Complete( SERVICE_ACK, chan, OUT_OF_SERVICE );
                break;
            default:                                 /* Unknown status */
                no_error = FALSE;
                break;
            }
        }
    /* msg == SERVICE_ACK
    */
    else if ( CC_SERV_GetCallState( chan ) == CHANNEL_MAINT_PEND )
        {
        /* Response to SERVICE message request.
        */
        CC_SERV_SetCallState( chan, CHANNEL_IDLE );
        CC_SERV_SetState( chan, new_state );
        if (( timer_id = CC_CHAN_GetTimerID( Int_Id, 
                                             chan, DSL_Id )) != NOT_TIMING )
           {
           LIF_StopTimer( timer_id );
           LIF_RemoveTimer( timer_id );
           CC_CHAN_SetTimerID( Int_Id, chan, ERR, DSL_Id );
           }
        no_error = TRUE;
        }
    else
        {
        /* msg == unsolicited SERVICE_ACK
        */
        switch ( new_state )
            {
            case OUT_OF_SERVICE:
                no_error = TRUE;                         /* Just a NOP */
                break;
            case MAINTENANCE:
            case IN_SERVICE:
                no_error = CC_SERV_Complete( SERVICE, chan, OUT_OF_SERVICE );
                break;
            default:                                  /* Unknown state */
                no_error = FALSE;
                break;
            }
        }

    return ( no_error );
}


/*********************
* CC_SERV_ServiceMsg *
**********************
*
* The entry function for these service message utilities.
*
*/
PUBLIC int CC_SERV_ServiceMsg( pkt, dsl_id, int_id )

PKT_ID  pkt;                         /* Incoming service message packet */
BYTE    dsl_id;                 /* Digital Subscriber Loop ID (network) */
BYTE   *int_id;                               /* Interface ID (output) */
{
   BOOLEAN  no_error = TRUE;
   BYTE     msg;                          /* The incoming message type */
   BYTE     chan = 0;                        /* The referenced channel */
   BYTE     current_state;           /* The referenced channel's state */
   BYTE     new_state;            /* New service state for the channel */
   BYTE    *chan_id_ie;                    /* Pointer to Channel ID IE */
   BYTE    *change_status_ie;           /* Pointer to Change Status IE */
   BYTE    *unpacked_ie;   /* Pointer to info element unpacking buffer */
   BOOLEAN  struc_err;                       /* Structural error in IE */


   In_Pkt = pkt;                           /* Save incoming parameters */
   DSL_Id = dsl_id;

   /* Check out the incoming packet for integrity.
   */
   if (( In_Pkt ) && ( In_Pkt->Info ))
      {
      CrefLen = In_Pkt->Info[ 1 ];
      if ( CrefLen == 1 )                      /* Short call reference */
         {
         Cref = In_Pkt->Info[ 2 ];
         msg = In_Pkt->Info[ 3 ];                     /* Read msg_type */
         }
      else                                      /* Long call reference */
         {
         Cref = ( In_Pkt->Info[ 2 ] << 8 ) + In_Pkt->Info[ 3 ];
         msg = In_Pkt->Info[ 4 ];                     /* Read msg_type */
         }

      if (( msg == SERVICE ) || ( msg == SERVICE_ACK ))
         {
         /* Locate essential info elements.
         */
         chan_id_ie = CCIE_Get_IE( In_Pkt->Info, In_Pkt->InfoLen,
                                   CHANNEL_ID_IE, CODESET_0, 1 );
         change_status_ie = CCIE_Get_IE( In_Pkt->Info, In_Pkt->InfoLen,
                                         CHANGE_STATUS_IE, CODESET_0, 1 );
         no_error = (( chan_id_ie != NULL ) &&    /* Missing Mand. ie? */
                     ( change_status_ie != NULL ));
         if ( no_error )
            {
            /* Check call reference, masking flag bit.
            */
            no_error = ( GLOBAL_CALL_REF( Cref, CrefLen ));
            if ( no_error )
               {
               /* Get channel ID
               */
               unpacked_ie = CCIE_ExpandChanId_IE( chan_id_ie, &struc_err );
               if ( unpacked_ie[ 0 ] )
                  Int_Id 
                     = CCPRI_MapIntId( unpacked_ie[ unpacked_ie[8] ], FALSE );
               else
                  Int_Id = 0;
               *int_id = Int_Id;                             /* Output */
               chan = unpacked_ie[ 9 ];                /* Read channel */

               /* Check the contents of the channel ID element.  The
               *  exact values to be expected here may vary depending
               *  upon the application.
               */
               no_error =
                  (( unpacked_ie[ 1 ] == 1 ) && /* Test interface type */
                   ( unpacked_ie[ 4 ] == 1 ) &&      /* Test info chan */
                   ( unpacked_ie[ 5 ] == 0 ) &&     /* Test coding std */
                   ( unpacked_ie[ 6 ] == 0 ) &&       /* Test numb/map */
                   ( unpacked_ie[ 7 ] == 3 ) &&      /* Test chan type */
                   ( !struc_err )            && /* No structural error */
                   ( VALID_CHANNELP( chan ) ) &&   /* Test channel numb */
                   ( Int_Id <= MAX_INT_IDP));                /* Int ID */
               if (( no_error ) &&
                   ( unpacked_ie[ 3 ] == 0 ))       /* Test D-Chan ind */
                  {
                 /* Get channel states.
                  */
                  current_state = CC_SERV_GetState( chan );
                  unpacked_ie = CCIE_ExpandChangeStatus_IE( change_status_ie,
                                                            &Trashcan );
                  /* Check channel vs interface preference.
                  */
                  no_error = ( unpacked_ie[ 0 ] );
                  if ( no_error )                /* Channel preference */
                     {
                     new_state = unpacked_ie[ 1 ];
#ifdef DEBUG
                     printf( "CC_SERV_ServiceMsg: Channel = %d, Initial state = %d\n",
                              chan, current_state );
#endif
                     /* Dispatch by current chan state.
                     */
                     switch ( current_state )
                        {
                        case IN_SERVICE:
                           no_error = CC_SERV_InServ( msg, chan, new_state );
                           break;
                        case MAINTENANCE:
                           no_error = CC_SERV_Maint( msg, chan, new_state );
                           break;
                        case OUT_OF_SERVICE:
                           no_error = CC_SERV_OutOfServ( msg, chan, new_state );
                           break;
                        default:
                           no_error = FALSE;
                           CC_SERV_Complete( CC_STATUS_REQ, 
                                             INVALID_ELEM_CONTENTS,
                                             CHANGE_STATUS_IE );
#ifdef DISPLAY_LOCAL_ERR
                           LIF_Error ( "cc_serv", "CC_SERV_ServiceMsg",
                                       "Unknown channel state.");
#endif
                           break;
                        }
                     }
                  else                 /* Interface preference (error) */
                     {
                     no_error = FALSE;
                     CC_SERV_Complete( CC_STATUS_REQ, 
                                       INVALID_ELEM_CONTENTS,
                                       CHANGE_STATUS_IE );
                     }
                  }
               else if ( msg == SERVICE )
                  {
                  if ( unpacked_ie[ 3 ] == 0 )      
                     /* Test D-Chan ind.  If the D-channel indicator is set
                     *  then the message is ignored.
                     */
                     CC_SERV_Complete( CC_STATUS_REQ, 
                                       INVALID_ELEM_CONTENTS,
                                       CHANNEL_ID_IE );
                  no_error = FALSE;
                  }
               else 
                  {
                  /* Unsolicited SERVICE_ACK w/bad Chan id ie - NOP
                  */
                  no_error = TRUE;
                  }
               }
            else
               {
               CC_SERV_Complete( CC_REL_COMP_REQ, INVALID_CALL_REF, 0 );
               }
            }
         }
      else                              /* Other maintenance messages? */
         {
         no_error = FALSE;
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error ("cc_serv", "CC_SERV_ServiceMsg",
                    "Unknown message received.");
#endif
         }
      }
   else
      {
#ifdef DISPLAY_LOCAL_ERR
      
LIF_Error ("cc_serv", "CC_SERV_ServiceMsg",
                 "Bad message received - missing buffer elements.");
#endif
      }

   if ( !no_error )
      chan = 0;

   return ( chan );
}


/**********************
* CC_SERV_ServTimeout *
***********************
*
* Timeout of SERVICE_ACK response timer (AT&T T3M1).
*
* The timer data for the channel is in the Channel Management Table.
* The target maintenance state has been preserved "borrowing" the
* channel protocol entry in the table.  
*
*/
PUBLIC void CC_SERV_ServTimeout( int_id, chan, dsl_id, event, interval )

BYTE   int_id;                                 /* Channel interface ID */
int    chan;                                  /* Channel under service */
int    dsl_id;                           /* Digital Subscriber Loop ID */
INT16  event;                             /* Reported event on timeout */
UINT32 interval;                                    /* Timing interval */
{
   int new_maint_state;                    /* Target maintenance state */
   int old_maint_state;                  /* Previous maintenance state */


   DSL_Id = dsl_id;
   Int_Id = int_id;

   /* Recover the target state.
   */
   new_maint_state = CC_CHAN_GetProtocolpri( Int_Id, chan, 
                                          B_CHANNEL_UNITS, dsl_id );

   /* Request a new SERVICE message be sent.
   */
   CC_SERV_ChangeRequest( Int_Id, chan, DSL_Id, new_maint_state, TRUE, 
                          event, interval );

   /* After one unsuccessful attempt to change the service state of the
   *  channel, we force the service state to that we are requesting if
   *  the new level of service is less available than the previous level
   *  (see 41449, Sec. 10.1.5.1).
   */
   old_maint_state = CC_CHAN_GetServState( Int_Id, chan, 
                                           B_CHANNEL_UNITS, dsl_id );
   switch ( old_maint_state )
      {
      case IN_SERVICE:
         CC_CHAN_SetServState( Int_Id, chan, B_CHANNEL_UNITS, 
                               new_maint_state, dsl_id );
         break;
      case MAINTENANCE:
         if ( new_maint_state == OUT_OF_SERVICE ) 
            CC_CHAN_SetServState( Int_Id, chan, B_CHANNEL_UNITS, 
                                  OUT_OF_SERVICE, dsl_id );
         break;
      case OUT_OF_SERVICE:
      default:
         break;
      }
}


/***********************
* CC_SERV_SetCallState *
************************
*
* Write the call state of a channel.
*
* NOTE: This function is a STUB and must be completed for
*       each application based upon the channel management
*       data structures in use.
*/
PRIVATE void near CC_SERV_SetCallState( chan, new_state )

BYTE chan;                                    /* B-Channel of interest */
BYTE new_state;                             /* B-Channel service state */

{
    CC_CHAN_SetStatepri( Int_Id, chan, B_CHANNEL_UNITS, new_state, DSL_Id );
}


/*******************
* CC_SERV_SetState *
********************
*
* Write the service state of a channel.
*
* NOTE 1: This function is a STUB and must be completed for
*         each application based upon the channel management
*         data structures in use.
*
* NOTE 2: As an option, the Call Control may refuse to change the
*         state of a channel (most specifically refusing to put it
*         back into a higher level of service from a lower one) and may
*         report this refusal through the return value for this function.
*
* NOTE 3: It is up to the Call Control whether or not to allow one-way
*         usage of channels in lowered states of service.
*/
PRIVATE BOOLEAN near CC_SERV_SetState( chan, new_state )

BYTE chan;                                    /* B-Channel of interest */
BYTE new_state;                             /* B-Channel service state */

{
    CC_CHAN_SetServState( Int_Id, chan, B_CHANNEL_UNITS, new_state, DSL_Id );

    return ( TRUE );
}
/* End of module
*/
