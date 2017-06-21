/* $Id: l3_sdl_u.c,v 3.7.4.5 1996/08/30 00:12:48 hrobison Exp $
 * $Source: /release/112/cvs/Xsys/isdn/l3_sdl_u.c,v $
 *------------------------------------------------------------------
 * l3_sdl_u.c  ISDN Layer 3 (Network Layer) SDLs (USER side)
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: l3_sdl_u.c,v $
 * Revision 3.7.4.5  1996/08/30  00:12:48  hrobison
 * CSCdi60340:  NET3 cannot handle incomming FACILITY message when call is
 * UP
 * Branch: California_branch
 * Currently we don't support the FACILITY (ETSI Supplementary Services)
 * message type, so the correct action is to ignore it.
 *
 * Revision 3.7.4.4  1996/08/16  20:42:43  dclare
 * CSCdi66164:  ISDN PRI Release message does not get Release Complete on
 * DMS100. Make this switchtype act the same as the other PRI switchetypes.
 * Branch: California_branch
 *
 * Revision 3.7.4.3  1996/07/18  21:23:46  snyder
 * CSCdi63402:  get isdn table out of data space and into text space
 * Branch: California_branch
 *              saves 9788 in RAM on run from flash devices
 *
 * Revision 3.7.4.2  1996/04/30  23:07:47  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.7.4.1  1996/04/11  21:43:59  dclare
 * CSCdi52751:  Make DMS-100 ISDN PRI changes to conform to the
 * Specification
 * Branch: California_branch
 *
 * Revision 3.7  1996/03/08  23:45:01  smackie
 * Fix various signed/unsigned comparison conflicts that cause the
 * 95q4 toolchain to blow it's cookies. (CSCdi49069)
 *
 * Revision 3.6  1996/02/09  16:36:41  dclare
 * CSCdi48422:  ISDN BRI NET3 needs changes to pass France NET3 Delta
 * testing. Allow incoming Alerting and Connect message in state U1.
 *
 * Revision 3.5  1996/01/23  22:22:39  rbeach
 * CSCdi46668:  ISDN: With NET-3 switch NLCBs not released. This
 * is caused after a call has been brought up and we send a
 * layer 3 DISC and receive a REL with various optional IEs,
 * DISPLAY and DATE/TIME being two of them. The NLCB count
 * will increase as well as the PRIM_BTYPE count.
 *
 * Revision 3.4  1996/01/22  06:35:23  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/21  03:52:49  dclare
 * CSCdi42382:  crash in ISDN code in australia. Incorrectly checking the
 * Display IE in an incoming Status Enquiry message.
 *
 * Revision 3.2  1995/11/17  17:41:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/19  16:52:37  dclare
 * CSCdi40646:  ISDN BRI changes for Italy NET3 homologation.
 *
 * Revision 2.1  1995/06/07  21:16:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       l3_sdl_u.c
DESCRIPTION:  ISDN Layer 3 (Network Layer) SDLs (USER side)
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

This module contains Q.931 CCITT user side SDL functions.
Together with the non-SDL layer 3 module (l3.c), it implements layer 3.

ORGANIZATION OF FUNCTIONS

The functions in this module are arranged as follows:
  SDL support functions alphabetically
  SDL functions by state and then alphabetic within a state.

FUNCTION DISPATCH WITHIN THIS MODULE

In general, an NLCB arrives in the L3_ProcessEvent() function within
this module carrying a Q.931 state and an event.  L3_ProcessEvent()
selects a State Dispatch Table to consult by indexing the StateTablePtrs
table with NLCB->State.  The Event column of the selected state table is
searched for a match with NLCB->Event.  If a match is found, the
corresponding EventFunc is called to handle the event.  If the table
is searched without a matching event until the UNEXPECTED event is
encountered, a search of ALL the State Dispatch Tables is conducted
(in L3_DefinedEvent()) for NLCB->Event.

If the event is located anywhere, it is assumed to be defined within
the system, but is assumed to be an event which is unexpected for the
current NLCB state, so the UNEXPECTED EventFunc for NLCB->State in the
originally selected State Dispatch Table is called to handle the event.

If the event is located in NO table for ANY state in the preceeding
search, the event is assumed to be undefined for the system, so the
UNDEFINED EventFunc for NLCB->State in the originally selected State
Dispatch Table is called to handle the event.

The tables and their search method are organized in their present manner
as opposed to a direct X-Y index in order to allow the event to assume any
16 bit value (e.g. a primitive as opposed to a message type) and to still
keep the tables relatively small.  The standard events within a state
are organized by probability of occurrence.

A NOTE ON SDL FUNCTIONS

1. Telenetworks provides SDL solutions for many SwitchTypes through this
   module.  In most SDL functions most SwitchTypes commonly receive the
   default treatment.

2. Telenetworks uses a manufacturing program to selectively remove solutions
   (source code lines delimited by #ifdef (SwitchType) statements) for
   SwitchTypes not purchased by a customer from a customer's source code.

Because of these two considerations, it will be normal for a customer to
find SDL functions which appear as only degenerate switch statements,
lacking any cases but the default.  The other case(s) represent special
actions for SwitchTypes not purchased and thus removed.

For example:

PRIVATE void near Ux_XXXX()
{
   switch ( SwitchType )
      {
      default:
         / * Default actions * /
         .
         .
         .
         break;
      }
}
#ifdef BRI_1TR6

A NOTE ON THE IMPLEMENTATION

In the 1TR6 point to multipoint SDLs, an anomaly is noted in State 7.
The error recovery on reception of SUSP ACK, RES REJ, REG ACK, REG REJ,
CANC ACK and CANC REJ shows a state transition to State 4.  This
transition is not provided in this solution because it is an apparent
document error.  Such a transition would immediately skew the state
tracking with the network.

Also, Telenetworks does not implement the DL_RESET_IND primitive from
Layer 2 to Layer 3.
#endif                                                    / * BRI_1TR6 * /
#ifdef PRI_1TR6

A NOTE ON THE IMPLEMENTATION

In the 1TR6 point to point SDLs, an anomaly is noted in State 12.
The error recovery on reception of REG ACK, REG REJ, CANC ACK and
CANC REJ shows a state transition to State 4.  This transition is not
provided in this solution because it is an apparent document error.  Such
a transition would immediately skew the state tracking with the network
and is patently illogical.

Also, Telenetworks does not implement the DL_RESET_IND primitive from
Layer 2 to Layer 3.
#endif                                                     / * PRI_1TR6 * /

*************************************************************************
*************************************************************************
*/

#define     L3_SDL_U_C		/* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/
#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"
#include    "address.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "ccie.h"
#include    "l3.h"

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/*
 * State/event function pointer.
 */
typedef void (near * EVENT_FUNC) (void);

/*
 * Table entry type for the State Dispatch Tables.
 */
typedef struct {
    int Event;                /* Event to be processed by the function */
    EVENT_FUNC EventFunc;     /* Pointer to function */
    SWITCH_TYPE Restriction;  /* switch it's restricted to === */
}   DISPATCH_TABLE;

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

/*
 * SDL functions - User side
 */
PRIVATE void near U0_BadMsg         (void);
PRIVATE void near U0_BroadcastResp  (void);
PRIVATE void near U0_FacCnclReq     (void);
PRIVATE void near U0_NOP            (void);
PRIVATE void near U0_NLEstReq       (void);
PRIVATE void near U0_RegisterReq    (void);
PRIVATE void near U0_ResumeReq      (void);
PRIVATE void near U0_Setup          (void);
PRIVATE void near U0_SetupReq       (void);
PRIVATE void near U0_Undefined      (void);

PRIVATE void near U1_Alerting       (void);
PRIVATE void near U1_BadMsg         (void);
PRIVATE void near U1_CallProc       (void);
PRIVATE void near U1_Connect        (void);
PRIVATE void near U1_DiscReq        (void);
PRIVATE void near U1_Disconnect     (void);
PRIVATE void near U1_Info           (void);
PRIVATE void near U1_InfoReq        (void);
PRIVATE void near U1_Progress       (void);
PRIVATE void near U1_Release        (void);
PRIVATE void near U1_ReleaseComp    (void);
PRIVATE void near U1_SetupAck       (void);
PRIVATE void near U1_T303Expiry     (void);
PRIVATE void near U1_UserInfo       (void);

PRIVATE void near U2_Alerting       (void);
PRIVATE void near U2_CallProc       (void);
PRIVATE void near U2_Connect        (void);
PRIVATE void near U2_DLEstInd       (void);
PRIVATE void near U2_Info           (void);
PRIVATE void near U2_InfoReq        (void);
PRIVATE void near U2_ReleaseReq     (void);

PRIVATE void near U3_Alerting       (void);
PRIVATE void near U3_CallProc       (void);
PRIVATE void near U3_Connect        (void);
PRIVATE void near U3_Info           (void);
PRIVATE void near U3_Progress       (void);
PRIVATE void near U3_ReleaseReq     (void);
PRIVATE void near U3_T310Expiry     (void);

PRIVATE void near U4_Alerting       (void);
PRIVATE void near U4_Connect        (void);
PRIVATE void near U4_Facility       (void);
#ifdef BRI_1TR6
PRIVATE void near U4_FacilityAck    (void);
PRIVATE void near U4_FacilityRej    (void);
PRIVATE void near U4_FacilityReq    (void);
#endif
#ifdef PRI_1TR6
PRIVATE void near U4_FacilityAck    (void);
PRIVATE void near U4_FacilityRej    (void);
PRIVATE void near U4_FacilityReq    (void);
#endif
PRIVATE void near U4_Progress       (void);
PRIVATE void near U4_ReleaseReq     (void);

PRIVATE void near U6_AlertingReq    (void);
PRIVATE void near U6_BadCrFlag      (void);
PRIVATE void near U6_CallProcReq    (void);
PRIVATE void near U6_Disconnect     (void);
PRIVATE void near U6_DiscReq        (void);
PRIVATE void near U6_Info           (void);
PRIVATE void near U6_MoreInfoReq    (void);
PRIVATE void near U6_Release        (void);
PRIVATE void near U6_ReleaseReq     (void);
PRIVATE void near U6_SetupRejReq    (void);
PRIVATE void near U6_SetupResp      (void);

PRIVATE void near U7_AlertingReq    (void);
PRIVATE void near U7_FacilityAck    (void);
PRIVATE void near U7_FacilityRej    (void);
PRIVATE void near U7_FacilityReq    (void);
#ifdef BRI_1TR6
PRIVATE void near U7_Info           (void);
#endif
PRIVATE void near U7_ProgressReq    (void);
PRIVATE void near U7_ReleaseReq     (void);
PRIVATE void near U7_SetupResp      (void);

PRIVATE void near U8_ConnectAck     (void);
PRIVATE void near U8_ConnectAckReq  (void);
PRIVATE void near U8_ReleaseReq     (void);
PRIVATE void near U8_T313Expiry     (void);
PRIVATE void near U8_Info           (void);

PRIVATE void near U9_AlertingReq    (void);
PRIVATE void near U9_Disconnect     (void);
PRIVATE void near U9_FacilityAck    (void);
PRIVATE void near U9_FacilityRej    (void);
PRIVATE void near U9_ProgressReq    (void);
PRIVATE void near U9_SetupResp      (void);
PRIVATE void near U9_Info           (void);

PRIVATE void near U10_CongCon       (void);
PRIVATE void near U10_ConnectAck    (void);
PRIVATE void near U10_FacAckReq     (void);
PRIVATE void near U10_FacRejReq     (void);
PRIVATE void near U10_Facility      (void);
PRIVATE void near U10_FacilityAck   (void);
PRIVATE void near U10_FacilityRej   (void);
PRIVATE void near U10_FacilityReq   (void);
PRIVATE void near U10_Info          (void);
PRIVATE void near U10_Notify        (void);
PRIVATE void near U10_NotifyReq     (void);
PRIVATE void near U10_Progress      (void);
PRIVATE void near U10_ReleaseReq    (void);
PRIVATE void near U10_SetupResp     (void);
#ifdef BRI_1TR6
PRIVATE void near U10_DetachReq     (void);
#endif
PRIVATE void near U10_SuspendReq    (void);
PRIVATE void near U10_UserInfo      (void);
PRIVATE void near U10_UserInfoReq   (void);

PRIVATE void near U11_BadMsg        (void);
#ifdef BRI_1TR6
PRIVATE void near U11_DetachReq     (void);
#endif
#ifdef PRI_1TR6
PRIVATE void near U11_DetachReq     (void);
#endif
PRIVATE void near U11_Disconnect    (void);
PRIVATE void near U11_FacilityAck   (void);
PRIVATE void near U11_FacilityRej   (void);
PRIVATE void near U11_Info          (void);
PRIVATE void near U11_Release       (void);
PRIVATE void near U11_ReleaseReq    (void);
PRIVATE void near U11_Status        (void);
#ifdef BRI_1TR6
PRIVATE void near U11_SuspendAck    (void);
#endif
PRIVATE void near U11_T305Expiry    (void);

PRIVATE void near U12_DiscReq       (void);
PRIVATE void near U12_Disconnect    (void);
PRIVATE void near U12_FacilityAck   (void);
PRIVATE void near U12_FacilityRej   (void);
PRIVATE void near U12_FacilityReq   (void);
PRIVATE void near U12_Info          (void);
PRIVATE void near U12_Release       (void);
PRIVATE void near U12_ReleaseReq    (void);

#ifdef BRI_1TR6
PRIVATE void near U14_ConnectAck    (void);

#endif
PRIVATE void near U15_Disconnect    (void);
PRIVATE void near U15_DiscReq       (void);
PRIVATE void near U15_Release       (void);
PRIVATE void near U15_ReleaseReq    (void);
PRIVATE void near U15_SuspendAck    (void);
PRIVATE void near U15_SuspendRej    (void);
PRIVATE void near U15_T319Expiry    (void);

PRIVATE void near U17_Disconnect    (void);
PRIVATE void near U17_DiscReq       (void);
PRIVATE void near U17_Info          (void);
PRIVATE void near U17_Release       (void);
PRIVATE void near U17_ReleaseReq    (void);
PRIVATE void near U17_ResumeAck     (void);
PRIVATE void near U17_ResumeRej     (void);
PRIVATE void near U17_T318Expiry    (void);

PRIVATE void near U19_BadMsg        (void);
PRIVATE void near U19_ConnectAck    (void);
PRIVATE void near U19_Disconnect    (void);
PRIVATE void near U19_FacilityAck   (void);
PRIVATE void near U19_FacilityRej   (void);
PRIVATE void near U19_Info          (void);
PRIVATE void near U19_Release       (void);
PRIVATE void near U19_ReleaseComp   (void);
PRIVATE void near U19_RelCompReq    (void);
PRIVATE void near U19_Status        (void);
PRIVATE void near U19_T308Expiry    (void);
PRIVATE void near U19_Undefined     (void);

#ifdef BRI_1TR6
PRIVATE void near U20_RegisterAck   (void);
PRIVATE void near U20_RegisterRej   (void);
#endif
#ifdef PRI_1TR6
PRIVATE void near U20_RegisterAck   (void);
PRIVATE void near U20_RegisterRej   (void);

#endif
#ifdef BRI_1TR6
PRIVATE void near U21_CancelAck     (void);
PRIVATE void near U21_CancelRej     (void);
#endif
#ifdef PRI_1TR6
PRIVATE void near U21_CancelAck     (void);
PRIVATE void near U21_CancelRej     (void);
#endif
PRIVATE void near U25_AlertingReq   (void);
PRIVATE void near U25_CallProcReq   (void);
PRIVATE void near U25_ProgressReq   (void);
PRIVATE void near U25_SetupResp     (void);

PRIVATE void near U98_BroadcastResp (void);
PRIVATE void near U98_NLEstReq      (void);
PRIVATE void near U98_NLRelReq      (void);
PRIVATE void near U98_SetupReq      (void);
PRIVATE void U98_Twait_expiry       (void);
PRIVATE void near U99_AlertingReq   (void);
PRIVATE void near U99_BroadcastResp (void);
PRIVATE void near U99_CallProcReq   (void);
PRIVATE void near U99_DLEstConf     (void);
PRIVATE void near U99_DLEstInd      (void);
PRIVATE void near U99_InfoReq       (void);
PRIVATE void near U99_ReleaseReq    (void);
PRIVATE void near U99_RelCompReq    (void);
PRIVATE void near U99_SetupRejReq   (void);
PRIVATE void near U99_SetupReq      (void);
PRIVATE void near U99_SetupResp     (void);
#ifdef BRI_5ESS
#ifdef 0 /* not currently used */
PRIVATE void near U99_ManReq        (void);
#endif
#endif
PRIVATE void near Ux_BadMsg         (void);
PRIVATE void near Ux_BadMsgAlt      (void);
PRIVATE void near Ux_ChngStateReq   (void);
PRIVATE void near Ux_CongCon        (void);
PRIVATE void near Ux_CongConReq     (void);
PRIVATE void near Ux_ConnectAckReq  (void);
PRIVATE void near Ux_Disconnect     (void);
PRIVATE void near Ux_DiscReq        (void);
PRIVATE void near U8_DiscReq        (void);
PRIVATE void near Ux_DLRelConf      (void);
PRIVATE void near Ux_DLRelInd       (void);
PRIVATE void near Ux_Error          (void);
PRIVATE void near Ux_FacAckReq      (void);
PRIVATE void near Ux_FacRejReq      (void);
PRIVATE void near Ux_FacilityAck    (void);
PRIVATE void near Ux_FacilityRej    (void);
PRIVATE void near Ux_FacilityReq    (void);
PRIVATE void near Ux_Info           (void);
PRIVATE void near Ux_InfoReq        (void);
#ifdef BRI_5ESS
PRIVATE void near Ux_ManInfo        (void);
PRIVATE void near Ux_ManReq         (void);
#endif
PRIVATE void near Ux_NLRelReq       (void);
PRIVATE void near Ux_NOP            (void);
PRIVATE void near Ux_Progress       (void);
PRIVATE void near Ux_Release        (void);
PRIVATE void near Ux_ReleaseComp    (void);
PRIVATE void near Ux_ReleaseReq     (void);
PRIVATE void near Ux_RelCompReq     (void);
#ifdef BRI_1TR6
PRIVATE void near Ux_Setup          (void);
#endif
#ifdef PRI_1TR6
PRIVATE void near Ux_Setup          (void);
#endif
PRIVATE void near Ux_Status         (void);
#ifdef BRI_1TR6
PRIVATE void near Ux_StatusB1TR6    (void);
#endif
#ifdef PRI_1TR6
PRIVATE void near Ux_StatusP1TR6    (void);
#endif
PRIVATE void near Ux_StatusEnq      (void);
PRIVATE void near Ux_StatusReq      (void);
PRIVATE void near Ux_Undefined      (void);
PRIVATE void near Ux_UserInfo       (void);
PRIVATE void near Ux_UserInfoReq    (void);

/*
 * Other PRIVATE functions.
 */
PRIVATE void    near L3_BadState      (void);
PRIVATE BOOLEAN near L3_DefinedEvent  (int);
#ifdef BRI_DMS100
PRIVATE int     near L3_RejectCause   (BYTE *);
#endif
PRIVATE int     near L3_VerifyDisplay (void);
PRIVATE int     near mandatory_ie     (int, int, int);
PRIVATE BOOLEAN near valid_Bchannel   (void);
PRIVATE BOOLEAN valid_cause           (void);
PRIVATE int     near valid_ie_contents (void);

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

/*
 * State Dispatch Tables -
 * 
 * Map an event to the function intended to handle it on a state-basis.
 * 
 * NOTES:
 * 
 * 1. Defined events not found in a table take the UNEXPECTED route near the
 * bottom of each table (see L3_ProcessEvent()). 2. Undefined events take the
 * UNDEFINED route at the bottom of each table (see L3_ProcessEvent()). 3.
 * The earlier events in a table are assumed to occur most often.  This
 * ordering is important for efficiency. 4. An event and its associated
 * function may be reordered within a table freely.  The impact of this will
 * only be a change in the execution time to process the event.  The events
 * and their ordering in one state are decoupled from those of other state.
 * 5. A new message may be added to the tables by specifying the event and an
 * appropriate event function in any state (table) where it is expected. If
 * the event arrives in other states, the UNEXPECTED route is taken. A new
 * message should be inserted in a given table position according to its
 * likeihood of its occurrance relative to the other events in the state. 6.
 * Each table MUST end in the UNEXPECTED and UNDEFINED entries in this order,
 * although the way these cases are handled may be altered by changing the
 * UNEXPECTED and UNDEFINED event functions.
 */

/*  NULL_STATE -- State 0
 *
 * NOTE:
 *
 * In most cases, the complete error handling for unexpected and undefined
 * events to the State 0 call occurs in Ux_UnknownCallRef(), which bypasses 
 * the conventional L3_ProcessEvent entry path.  It is dispatched directly
 * from L3_GetUser_NLCB().
 */
static const DISPATCH_TABLE StateTable_00[] =
{
/*
 * Event                     Event function		Switch Restriction
 */
   {SETUP,                    U0_Setup,   		NO_ISDN_SWITCH},
   {CC_SETUP_REQ,             U0_SetupReq,		NO_ISDN_SWITCH},
   {CC_BROADCAST_RESP,        U0_BroadcastResp,  	NO_ISDN_SWITCH},
   {CC_RESUME_REQ,            U0_ResumeReq,  		NO_ISDN_SWITCH},
   {CC_REGISTER_REQ,          U0_RegisterReq,    	NO_ISDN_SWITCH},
   {CC_FAC_CANCEL_REQ,        U0_FacCnclReq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               U0_NLEstReq,		NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,    	NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           Ux_NOP,			NO_ISDN_SWITCH},
   {RESTART,                  Ux_NOP,			NO_ISDN_SWITCH},
   {RESTART_ACK,              Ux_NOP,			NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {CANCEL_ACK,               U0_NOP,			PRI_1TR6_STYPE},
   {CANCEL_REJ,               U0_NOP,			PRI_1TR6_STYPE},
   {REGISTER_ACK,             U0_NOP,			PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        U0_NOP,			PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {CANCEL_ACK,               U0_NOP,			BRI_1TR6_STYPE},
   {CANCEL_REJ,               U0_NOP,			BRI_1TR6_STYPE},
   {REGISTER_ACK,             U0_NOP,			BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        U0_NOP,			BRI_1TR6_STYPE},
   {SUSPEND_ACK,              U0_NOP,			BRI_1TR6_STYPE},
   {RESUME_REJ,               U0_NOP,			BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,   		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               U0_BadMsg,   		NO_ISDN_SWITCH},
   {UNDEFINED,                U0_Undefined,		NO_ISDN_SWITCH}
};

/*
 * CALL_INIT -- State 1
 */
static const DISPATCH_TABLE StateTable_01[] =
{
   {CALL_PROC,                U1_CallProc,		NO_ISDN_SWITCH},
   {SETUP_ACK,                U1_SetupAck,		NO_ISDN_SWITCH},
   {ALERTING,                 U1_Alerting,		NO_ISDN_SWITCH},
   {CONNECT,                  U1_Connect,		NO_ISDN_SWITCH},
   {PROGRESS,                 U1_Progress,		NO_ISDN_SWITCH},
   {DISCONNECT,               U1_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        U1_DiscReq,		NO_ISDN_SWITCH},
   {INFO,                     U1_Info,			NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {CC_INFO_REQ,              U1_InfoReq,		NO_ISDN_SWITCH},
   {T303_EXPIRY,              U1_T303Expiry,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,  		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {RELEASE,                  U1_Release,    		NO_ISDN_SWITCH},
   {RELEASE_COMP,             U1_ReleaseComp,    	NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           Ux_ReleaseReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,   	NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {CANCEL_ACK,               Ux_Error,    		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,    		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               U1_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * OVERLAP_SENDING -- State 2
 */
static const DISPATCH_TABLE StateTable_02[] =
{
   {CC_INFO_REQ,              U2_InfoReq,		NO_ISDN_SWITCH},
   {CALL_PROC,                U2_CallProc,		NO_ISDN_SWITCH},
   {ALERTING,                 U2_Alerting,		NO_ISDN_SWITCH},
   {CONNECT,                  U2_Connect,		NO_ISDN_SWITCH},
   {PROGRESS,                 Ux_Progress,		NO_ISDN_SWITCH},
   {DISCONNECT,               Ux_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {INFO,                     U2_Info,			NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,    		VN3_STYPE},
#ifdef BRI_1TR6
   {INFO_1TR6,                Ux_Info,			BRI_1TR6_STYPE},
#endif 
   {STATUS,                   Ux_Status,  		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {RELEASE,                  Ux_Release,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U2_ReleaseReq,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {DL_EST_IND,               U2_DLEstInd,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              U2_DLEstInd,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {CANCEL_ACK,               Ux_Error,   		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,  		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,  		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,  		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {CANCEL_ACK,               Ux_Error,  		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,  		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,  		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,  		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,  		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,  		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,  		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               Ux_BadMsg,  		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
   };

/*
 * OUT_CALL_PROC -- State 3
 */
static const DISPATCH_TABLE StateTable_03[] =
{
   {ALERTING,                 U3_Alerting,		NO_ISDN_SWITCH},
   {CONNECT,                  U3_Connect,		NO_ISDN_SWITCH},
   {DISCONNECT,               Ux_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		NO_ISDN_SWITCH},
   {CALL_PROC,                U3_CallProc,		NO_ISDN_SWITCH},
   {PROGRESS,                 U3_Progress,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {INFO,                     U3_Info,			NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {T310_EXPIRY,              U3_T310Expiry,		NO_ISDN_SWITCH},
#ifdef BRI_1TR6 
   {INFO_1TR6,                U3_Info,   		BRI_1TR6_STYPE},
   {CC_CONNECT_ACK_REQ,       Ux_ConnectAckReq,    	BRI_1TR6_STYPE},
#endif 
#ifdef PRI_1TR6 
   {INFO_1TR6,                U3_Info,   		PRI_1TR6_STYPE},
#endif 
#ifdef PRI_1TR6 
   {INFO_1TR6,                Ux_Info,    		PRI_1TR6_STYPE},
#endif 
   {STATUS,                   Ux_Status,    		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U3_ReleaseReq,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             Ux_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             Ux_FacilityRej,		NO_ISDN_SWITCH},
   {CC_FAC_ACK_REQ,           Ux_FacAckReq,		NO_ISDN_SWITCH},
   {CC_FAC_REJ_REQ,           Ux_FacRejReq,		NO_ISDN_SWITCH},
   {RELEASE,                  Ux_Release,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {CANCEL_ACK,               Ux_Error,   		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,   		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {CANCEL_ACK,               Ux_Error,   		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,   		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,   		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,   		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,   		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,   		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,  		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               Ux_BadMsg,   		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * CALL_DELIVERED -- State 4
 */
static const DISPATCH_TABLE StateTable_04[] =
{
   {CONNECT,                  U4_Connect,		NO_ISDN_SWITCH},
   {ALERTING,                 U4_Alerting,		NO_ISDN_SWITCH},
   {CALL_PROC,                U3_CallProc,		NO_ISDN_SWITCH},
   {PROGRESS,                 U4_Progress,		NO_ISDN_SWITCH},
   {DISCONNECT,               Ux_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {INFO,                     U3_Info,   		NO_ISDN_SWITCH},
#ifdef BRI_1TR6 
   {CC_CONNECT_ACK_REQ,       Ux_ConnectAckReq,   	BRI_1TR6_STYPE},
   {INFO_1TR6,                U3_Info,   		BRI_1TR6_STYPE},
   {FACILITY_ACK_1TR6,        U4_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U4_FacilityRej,		BRI_1TR6_STYPE},
#endif 
#ifdef PRI_1TR6 
   {INFO_1TR6,                U3_Info,   		PRI_1TR6_STYPE},
#endif 
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {USER_INFO,                Ux_UserInfo,		NO_ISDN_SWITCH},
   {CC_USER_INFO_REQ,         Ux_UserInfoReq,		NO_ISDN_SWITCH},
   {CONG_CON,                 Ux_CongCon,		NO_ISDN_SWITCH},
   {CC_CONG_CON_REQ,          Ux_CongConReq,		NO_ISDN_SWITCH},
   {CC_FAC_ACK_REQ,           Ux_FacAckReq,		NO_ISDN_SWITCH},
   {CC_FAC_REJ_REQ,           Ux_FacRejReq,		NO_ISDN_SWITCH},
   {RELEASE,                  Ux_Release,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,  		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U4_ReleaseReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,   	NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
   {FACILITY,                 U4_Facility,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {CC_FACILITY_REQ,          U4_FacilityReq,		PRI_1TR6_STYPE},
   {FACILITY_ACK,             U4_FacilityAck,		PRI_1TR6_STYPE},
   {FACILITY_REJ,             U4_FacilityRej,		PRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,   		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,   		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {CC_FACILITY_REQ,          U4_FacilityReq,		BRI_1TR6_STYPE},
   {FACILITY_ACK,             U4_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ,             U4_FacilityRej,		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,   		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,   		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,   		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,   		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,   		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,   		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,   		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo, 		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               Ux_BadMsg,   		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * CALL_PRESENT -- State 5  Q.931 state 6
 */
static const DISPATCH_TABLE StateTable_06[] =
{
   {CC_CALL_PROC_REQ,         U6_CallProcReq,		NO_ISDN_SWITCH},
   {CC_ALERTING_REQ,          U6_AlertingReq,		NO_ISDN_SWITCH},
   {CC_SETUP_RESP,            U6_SetupResp,		NO_ISDN_SWITCH},
   {CC_SETUP_REJ_REQ,         U6_SetupRejReq,		NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {INFO,                     U6_Info,   		NO_ISDN_SWITCH},
   {CC_MORE_INFO_REQ,         U6_MoreInfoReq,		NO_ISDN_SWITCH},
   {DISCONNECT,               U6_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        U6_DiscReq,		NO_ISDN_SWITCH},
   {RELEASE,                  U6_Release,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U6_ReleaseReq,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,   		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             Ux_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             Ux_FacilityRej,		NO_ISDN_SWITCH},
   {CC_FACILITY_REQ,          Ux_FacilityReq,		NO_ISDN_SWITCH},
   {CC_FAC_ACK_REQ,           Ux_FacAckReq,		NO_ISDN_SWITCH},
   {CC_FAC_REJ_REQ,           Ux_FacRejReq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,    		NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {SETUP,                    Ux_Setup,   		PRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,   		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,   		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {SETUP,                    Ux_Setup,   		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,   		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,   		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,   		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,   		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,   		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,   		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,   		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo, 		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               U1_BadMsg,  		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * CALL_RECEIVED -- State 6  Q.931 state 7
 */
static const DISPATCH_TABLE StateTable_07[] =
{
   {CC_SETUP_RESP,            U7_SetupResp,		NO_ISDN_SWITCH},
   {CC_ALERTING_REQ,          U7_AlertingReq,		NO_ISDN_SWITCH},
   {CC_PROGRESS_REQ,          U7_ProgressReq,		NO_ISDN_SWITCH},
   {DISCONNECT,               Ux_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {USER_INFO,                Ux_UserInfo,		NO_ISDN_SWITCH},
   {CC_USER_INFO_REQ,         Ux_UserInfoReq,		NO_ISDN_SWITCH},
   {CONG_CON,                 Ux_CongCon,		NO_ISDN_SWITCH},
   {CC_CONG_CON_REQ,          Ux_CongConReq,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             U7_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             U7_FacilityRej,		NO_ISDN_SWITCH},
   {CC_FACILITY_REQ,          U7_FacilityReq,		NO_ISDN_SWITCH},
   {CC_FAC_ACK_REQ,           Ux_FacAckReq,		NO_ISDN_SWITCH},
   {CC_FAC_REJ_REQ,           Ux_FacRejReq,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U7_ReleaseReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,  		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {RELEASE,                  Ux_Release,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {SETUP,                    Ux_Setup,   		PRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,   		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,   		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {SETUP,                    Ux_Setup,   		BRI_1TR6_STYPE},
   {FACILITY_ACK_1TR6,        U7_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U7_FacilityRej,		BRI_1TR6_STYPE},
   {INFO_1TR6,                U7_Info,    		BRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusB1TR6,		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,     		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,     		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               Ux_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,    		NO_ISDN_SWITCH}
};

/*
 * CONNECT_REQUEST -- State 8  Q.931 state 8
 */
static const DISPATCH_TABLE StateTable_08[] =
{
   {CONNECT_ACK,              U8_ConnectAck,		NO_ISDN_SWITCH},
   {CC_CONNECT_ACK_REQ,       U8_ConnectAckReq,  	NO_ISDN_SWITCH},
   {DISCONNECT,               Ux_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        U8_DiscReq,		NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {USER_INFO,                Ux_UserInfo,		NO_ISDN_SWITCH},
   {CC_USER_INFO_REQ,         Ux_UserInfoReq,		NO_ISDN_SWITCH},
   {CONG_CON,                 Ux_CongCon,		NO_ISDN_SWITCH},
   {CC_CONG_CON_REQ,          Ux_CongConReq,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             U7_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             U7_FacilityRej,		NO_ISDN_SWITCH},
   {CC_FACILITY_REQ,          U7_FacilityReq,		NO_ISDN_SWITCH},
   {CC_FAC_ACK_REQ,           Ux_FacAckReq,		NO_ISDN_SWITCH},
   {CC_FAC_REJ_REQ,           Ux_FacRejReq,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,  		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {T313_EXPIRY,              U8_T313Expiry,		NO_ISDN_SWITCH},
   {RELEASE,                  Ux_Release,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U8_ReleaseReq,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,    		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {SETUP,                    Ux_Setup,   		PRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,   		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,   		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,   		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {SETUP,                    Ux_Setup,   		BRI_1TR6_STYPE},
   {INFO_1TR6,                U7_Info,   		BRI_1TR6_STYPE},
   {FACILITY_ACK_1TR6,        U7_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U7_FacilityRej,		BRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusB1TR6,		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,    		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,    		BRI_5ESS_STYPE},
#endif
   {INFO,                     U8_Info,    		NO_ISDN_SWITCH},
   {UNEXPECTED,               Ux_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,   		NO_ISDN_SWITCH}
};

/*
 * IN_CALL_PROC -- State 8  Q.931 state 9
 */
static const DISPATCH_TABLE StateTable_09[] =
{                         	
   {CC_ALERTING_REQ,          U9_AlertingReq,		NO_ISDN_SWITCH},
   {CC_PROGRESS_REQ,          U9_ProgressReq,		NO_ISDN_SWITCH},
   {CC_SETUP_RESP,            U9_SetupResp,		NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {USER_INFO,                Ux_UserInfo,		NO_ISDN_SWITCH},
   {CC_USER_INFO_REQ,         Ux_UserInfoReq,		NO_ISDN_SWITCH},
   {DISCONNECT,               U9_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		NO_ISDN_SWITCH},
   {CONG_CON,                 Ux_CongCon,		NO_ISDN_SWITCH},
   {CC_CONG_CON_REQ,          Ux_CongConReq,		NO_ISDN_SWITCH},
   {CC_FAC_ACK_REQ,           Ux_FacAckReq,		NO_ISDN_SWITCH},
   {CC_FAC_REJ_REQ,           Ux_FacRejReq,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             U9_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             U9_FacilityRej,		NO_ISDN_SWITCH},
   {CC_FACILITY_REQ,          Ux_FacilityReq,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,    		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U10_ReleaseReq,		NO_ISDN_SWITCH},
   {RELEASE,                  Ux_Release,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {CANCEL_ACK,               Ux_Error,    		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		PRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,    		BRI_5ESS_STYPE},
#endif
   {INFO,                     U9_Info,    		NO_ISDN_SWITCH},
   {UNEXPECTED,               Ux_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * CALL_ACTIVE -- State 9  Q.931 state 10
 */
static const DISPATCH_TABLE StateTable_10[] =
{
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {USER_INFO,                U10_UserInfo,		NO_ISDN_SWITCH},
   {CC_USER_INFO_REQ,         U10_UserInfoReq,		NO_ISDN_SWITCH},
   {DISCONNECT,               Ux_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		NO_ISDN_SWITCH},
   {CONG_CON,                 U10_CongCon,		NO_ISDN_SWITCH},
   {CC_CONG_CON_REQ,          Ux_CongConReq,		NO_ISDN_SWITCH},
   {CC_SUSPEND_REQ,           U10_SuspendReq,		NO_ISDN_SWITCH},
   {NOTIFY,                   U10_Notify,		NO_ISDN_SWITCH},
   {CC_NOTIFY_REQ,            U10_NotifyReq,		NO_ISDN_SWITCH},
   {CC_SETUP_RESP,            U10_SetupResp,		NO_ISDN_SWITCH},
   {CONNECT_ACK,              U10_ConnectAck,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {INFO,                     U10_Info,    		NO_ISDN_SWITCH},
   {PROGRESS,                 U10_Progress,		NO_ISDN_SWITCH},
   {FACILITY,                 U10_Facility,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             U10_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             U10_FacilityRej,		NO_ISDN_SWITCH},
   {CC_FAC_ACK_REQ,           U10_FacAckReq,		NO_ISDN_SWITCH},
   {CC_FAC_REJ_REQ,           U10_FacRejReq,		NO_ISDN_SWITCH},
   {CC_FACILITY_REQ,          U10_FacilityReq,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,    		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {RELEASE,                  Ux_Release,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U10_ReleaseReq,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {FACILITY_1TR6,            U10_Facility,		PRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusP1TR6,		PRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		PRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U10_FacilityRej,    	PRI_1TR6_STYPE},
   {FACILITY_ACK_1TR6,        U10_FacilityAck,    	PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {FACILITY_1TR6,            U10_Facility,		BRI_1TR6_STYPE},
   {FACILITY_ACK_1TR6,        U10_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U10_FacilityRej,		BRI_1TR6_STYPE},
   {CC_DETACH_REQ,            U10_DetachReq,		BRI_1TR6_STYPE},
   {INFO_1TR6,                U10_Info,    		BRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusB1TR6,		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,    		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,    		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               Ux_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * DISCONNECT_REQUEST -- State 10  Q.931 state 11
 */
static const DISPATCH_TABLE StateTable_11[] =
{
   {RELEASE,                  U11_Release,		NO_ISDN_SWITCH},
   {DISCONNECT,               U11_Disconnect,		NO_ISDN_SWITCH},
   {T305_EXPIRY,              U11_T305Expiry,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {INFO,                     U11_Info,    		NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
#ifdef BRI_1TR6 
   {INFO_1TR6,                U11_Info,    		BRI_1TR6_STYPE},
#endif 
   {STATUS,                   U11_Status,		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             U11_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             U11_FacilityRej,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U11_ReleaseReq,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {ALERTING,                 U11_BadMsg,		NO_ISDN_SWITCH},
   {CALL_PROC,                U11_BadMsg,		NO_ISDN_SWITCH},
   {CONNECT,                  U11_BadMsg,		NO_ISDN_SWITCH},
   {CONNECT_ACK,              U11_BadMsg,		NO_ISDN_SWITCH},
   {FACILITY,                 U11_BadMsg,		NO_ISDN_SWITCH},
   {NOTIFY,                   U11_BadMsg,		NO_ISDN_SWITCH},
   {PROGRESS,                 U11_BadMsg,		NO_ISDN_SWITCH},
   {SETUP_ACK,                U11_BadMsg,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {CC_DETACH_REQ,            U11_DetachReq,		PRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusP1TR6,		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {FACILITY_ACK_1TR6,        U11_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U11_FacilityRej,		BRI_1TR6_STYPE},
   {CC_DETACH_REQ,            U11_DetachReq,		BRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusB1TR6,		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              U11_SuspendAck,		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               Ux_BadMsgAlt,		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * DISCONNECT_IND -- State 11  Q.931 state 12
 */
static const DISPATCH_TABLE StateTable_12[] =
{
   {CC_RELEASE_REQ,           U12_ReleaseReq,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        U12_DiscReq,		NO_ISDN_SWITCH},
   {DISCONNECT,               U12_Disconnect,		NO_ISDN_SWITCH},
   {RELEASE,                  U12_Release,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {INFO,                     U12_Info,    		NO_ISDN_SWITCH},
#ifdef BRI_1TR6 
   {INFO_1TR6,                U12_Info,       		BRI_1TR6_STYPE},
   {FACILITY_ACK_1TR6,        U12_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U12_FacilityRej,   	BRI_1TR6_STYPE},
#endif 
   {STATUS,                   U11_Status,		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             U12_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             U12_FacilityRej,		NO_ISDN_SWITCH},
   {CC_FACILITY_REQ,          U12_FacilityReq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CALL_PROC_REQ,         Ux_NOP,             	NO_ISDN_SWITCH},
   {CC_SETUP_RESP,            Ux_NOP,			NO_ISDN_SWITCH},
   {CC_ALERTING_REQ,          Ux_NOP,             	NO_ISDN_SWITCH},
   {CC_PROGRESS_REQ,          Ux_NOP,             	NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_NOP,			NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
#ifdef PRI_1TR6
   {CC_DETACH_REQ,            U11_DetachReq,		PRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusP1TR6,		PRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {CC_DETACH_REQ,            U11_DetachReq,		BRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusB1TR6,		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,    		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,    		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               Ux_BadMsgAlt,		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};
#ifdef BRI_1TR6

/*
 * CALL_PASSIVE -- 1TR6 State 14
 */
static const DISPATCH_TABLE StateTable_14[] =
{
   {CONNECT_ACK,              U14_ConnectAck,		BRI_1TR6_STYPE},
   {CC_SETUP_RESP,            U9_SetupResp,		BRI_1TR6_STYPE},
   {DISCONNECT,               Ux_Disconnect,		BRI_1TR6_STYPE},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		BRI_1TR6_STYPE},
   {INFO_1TR6,                Ux_Info,			BRI_1TR6_STYPE},
   {FACILITY_ACK_1TR6,        U10_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U10_FacilityRej,		BRI_1TR6_STYPE},
   {STATUS_1TR6,              Ux_StatusB1TR6,		BRI_1TR6_STYPE},
   {RELEASE,                  Ux_Release,		BRI_1TR6_STYPE},
   {CC_RELEASE_REQ,           Ux_ReleaseReq,		BRI_1TR6_STYPE},
   {DL_REL_CONF,              Ux_DLRelConf,		BRI_1TR6_STYPE},
   {DL_REL_IND,               Ux_DLRelInd,		BRI_1TR6_STYPE},
   {NL_EST_REQ,               Ux_NOP,			BRI_1TR6_STYPE},
   {NL_REL_REQ,               Ux_NLRelReq,		BRI_1TR6_STYPE},
   {RELEASE_COMP,             Ux_ReleaseComp,		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,    		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		BRI_1TR6_STYPE},
   {CC_STATUS_REQ,            Ux_StatusReq,		BRI_1TR6_STYPE},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {UNEXPECTED,               Ux_BadMsg,    		BRI_1TR6_STYPE},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};
#endif    /* BRI_1TR6 */

/*
 * SUSPEND_REQUEST -- Q.931 state 15
 */
static const DISPATCH_TABLE StateTable_15[] =
{
   {SUSPEND_ACK,              U15_SuspendAck,		NO_ISDN_SWITCH},
   {SUSPEND_REJ,              U15_SuspendRej,		NO_ISDN_SWITCH},
   {DISCONNECT,               U15_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        U15_DiscReq,		NO_ISDN_SWITCH},
   {T319_EXPIRY,              U15_T319Expiry,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              Ux_InfoReq,		NO_ISDN_SWITCH},
   {INFO,                     Ux_Info,			NO_ISDN_SWITCH},
   {RELEASE,                  U15_Release,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U15_ReleaseReq,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,    		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef BRI_1TR6
   {STATUS_1TR6,              Ux_StatusB1TR6,		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {UNEXPECTED,               Ux_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * RESUME_REQUEST -- Q.931 state 17
 */
static const DISPATCH_TABLE StateTable_17[] =
{
   {RESUME_ACK,               U17_ResumeAck,		NO_ISDN_SWITCH},
   {RESUME_REJ,               U17_ResumeRej,		NO_ISDN_SWITCH},
   {T318_EXPIRY,              U17_T318Expiry,		NO_ISDN_SWITCH},
   {INFO,                     U17_Info,    		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,    		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {DISCONNECT,               U17_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        U17_DiscReq,		NO_ISDN_SWITCH},
   {RELEASE,                  U17_Release,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U17_ReleaseReq,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef BRI_1TR6
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,    		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,    		BRI_5ESS_STYPE},
#endif
   {USER_INFO,                U1_UserInfo,    		VN3_STYPE},
   {UNEXPECTED,               Ux_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * RELEASE_REQUEST -- Q.931 state 19
 *
 * note: Ux_NOP put below by GSD, 2/8/90 at AT&T due to a problem
 *  with call transfer !!!!!!!!!!
 */
static const DISPATCH_TABLE StateTable_19[] =
{
   {RELEASE_COMP,             U19_ReleaseComp,		NO_ISDN_SWITCH},
   {RELEASE,                  U19_Release,		NO_ISDN_SWITCH},
   {DISCONNECT,               U19_Disconnect,		NO_ISDN_SWITCH},
   {FACILITY_ACK,             U19_FacilityAck,		NO_ISDN_SWITCH},
   {FACILITY_REJ,             U19_FacilityRej,		NO_ISDN_SWITCH},
   {INFO,                     U19_Info,    		NO_ISDN_SWITCH},
   {T308_EXPIRY,              U19_T308Expiry,		NO_ISDN_SWITCH},
   {STATUS,                   U19_Status,		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          U19_RelCompReq,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           Ux_NOP,			NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {STATUS_1TR6,              Ux_StatusP1TR6,		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {STATUS_1TR6,              Ux_StatusB1TR6,		BRI_1TR6_STYPE},
   {FACILITY_ACK_1TR6,        U19_FacilityAck,		BRI_1TR6_STYPE},
   {FACILITY_REJ_1TR6,        U19_FacilityRej,		BRI_1TR6_STYPE},
   {INFO_1TR6,                Ux_Info,			BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {CONNECT_ACK,              U19_ConnectAck,		NO_ISDN_SWITCH},
   {UNEXPECTED,               U19_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                U19_Undefined,		NO_ISDN_SWITCH}
};
#ifdef BRI_1TR6

/*
 * REGISTER_REQUEST -- 1TR6 State 20
 */
static const DISPATCH_TABLE StateTable_20[] =
{
   {REGISTER_ACK,             U20_RegisterAck,		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        U20_RegisterRej,		BRI_1TR6_STYPE},
   {DISCONNECT,               U9_Disconnect,		BRI_1TR6_STYPE},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		BRI_1TR6_STYPE},
   {RELEASE,                  Ux_Release,		BRI_1TR6_STYPE},
   {CC_RELEASE_REQ,           Ux_ReleaseReq,		BRI_1TR6_STYPE},
   {RELEASE_COMP,             Ux_ReleaseComp,		BRI_1TR6_STYPE},
   {DL_REL_CONF,              Ux_DLRelConf,		BRI_1TR6_STYPE},
   {DL_REL_IND,               Ux_DLRelInd,		BRI_1TR6_STYPE},
   {NL_EST_REQ,               Ux_NOP,			BRI_1TR6_STYPE},
   {NL_REL_REQ,               Ux_NLRelReq,		BRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,    		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {UNEXPECTED,               Ux_BadMsg,    		BRI_1TR6_STYPE},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * CANCEL_REQUEST -- 1TR6 State 21
 */
static const DISPATCH_TABLE StateTable_21[] =
{
   {CANCEL_ACK,               U21_CancelAck,		BRI_1TR6_STYPE},
   {CANCEL_REJ,               U21_CancelRej,		BRI_1TR6_STYPE},
   {DISCONNECT,               U9_Disconnect,		BRI_1TR6_STYPE},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		BRI_1TR6_STYPE},
   {RELEASE,                  Ux_Release,		BRI_1TR6_STYPE},
   {CC_RELEASE_REQ,           Ux_ReleaseReq,		BRI_1TR6_STYPE},
   {RELEASE_COMP,             Ux_ReleaseComp,		BRI_1TR6_STYPE},
   {DL_REL_CONF,              Ux_DLRelConf,		BRI_1TR6_STYPE},
   {DL_REL_IND,               Ux_DLRelInd,		BRI_1TR6_STYPE},
   {NL_EST_REQ,               Ux_NOP,    		BRI_1TR6_STYPE},
   {NL_REL_REQ,               Ux_NLRelReq,		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,     		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {UNEXPECTED,               Ux_BadMsg,    		BRI_1TR6_STYPE},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};
#endif   /* BRI_1TR6 */
#ifdef PRI_1TR6

/*
 * REGISTER_REQUEST -- 1TR6 State 20
 */
static const DISPATCH_TABLE StateTable_20[] =
{
   {REGISTER_ACK,             U20_RegisterAck,		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        U20_RegisterRej,		PRI_1TR6_STYPE},
   {DISCONNECT,               U9_Disconnect,		PRI_1TR6_STYPE},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		PRI_1TR6_STYPE},
   {RELEASE,                  Ux_Release,		PRI_1TR6_STYPE},
   {CC_RELEASE_REQ,           Ux_ReleaseReq,		PRI_1TR6_STYPE},
   {RELEASE_COMP,             Ux_ReleaseComp,		PRI_1TR6_STYPE},
   {DL_REL_CONF,              Ux_DLRelConf,		PRI_1TR6_STYPE},
   {DL_REL_IND,               Ux_DLRelInd,		PRI_1TR6_STYPE},
   {NL_EST_REQ,               Ux_NOP,			PRI_1TR6_STYPE},
   {NL_REL_REQ,               Ux_NLRelReq,		PRI_1TR6_STYPE},
   {CANCEL_ACK,               Ux_Error,     		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		PRI_1TR6_STYPE},
   {UNEXPECTED,               Ux_BadMsg,    		PRI_1TR6_STYPE},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * CANCEL_REQUEST -- 1TR6 State 21
 */
static const DISPATCH_TABLE StateTable_21[] =
{
   {CANCEL_ACK,               U21_CancelAck,		PRI_1TR6_STYPE},
   {CANCEL_REJ,               U21_CancelRej,		PRI_1TR6_STYPE},
   {DISCONNECT,               U9_Disconnect,		PRI_1TR6_STYPE},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		PRI_1TR6_STYPE},
   {RELEASE,                  Ux_Release,		PRI_1TR6_STYPE},
   {CC_RELEASE_REQ,           Ux_ReleaseReq,		PRI_1TR6_STYPE},
   {RELEASE_COMP,             Ux_ReleaseComp,		PRI_1TR6_STYPE},
   {DL_REL_CONF,              Ux_DLRelConf,		PRI_1TR6_STYPE},
   {DL_REL_IND,               Ux_DLRelInd,		PRI_1TR6_STYPE},
   {NL_EST_REQ,               Ux_NOP,			PRI_1TR6_STYPE},
   {NL_REL_REQ,               Ux_NLRelReq,		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		PRI_1TR6_STYPE},
   {UNEXPECTED,               Ux_BadMsg,    		PRI_1TR6_STYPE},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};
#endif  /* PRI_1TR6 */

/*
 * OVERLAP_RECEIVE -- Q.931 state 25
 */
static const DISPATCH_TABLE StateTable_25[] =
{
   {INFO,                     Ux_Info,			NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {INFO_1TR6,                Ux_Info,			PRI_1TR6_STYPE},
#endif 
   {CC_ALERTING_REQ,          U25_AlertingReq,		NO_ISDN_SWITCH},
   {CC_CALL_PROC_REQ,         U25_CallProcReq,		NO_ISDN_SWITCH},
   {CC_SETUP_RESP,            U25_SetupResp,		NO_ISDN_SWITCH},
   {CC_PROGRESS_REQ,          U25_ProgressReq,		NO_ISDN_SWITCH},
   {DISCONNECT,               U9_Disconnect,		NO_ISDN_SWITCH},
   {CC_DISCONNECT_REQ,        Ux_DiscReq,		NO_ISDN_SWITCH},
   {RELEASE,                  Ux_Release,		NO_ISDN_SWITCH},
   {RELEASE_COMP,             Ux_ReleaseComp,		NO_ISDN_SWITCH},
   {STATUS,                   Ux_Status,    		NO_ISDN_SWITCH},
   {STATUS_ENQ,               Ux_StatusEnq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               Ux_NOP,			NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          Ux_RelCompReq,		NO_ISDN_SWITCH},
#ifdef PRI_1TR6
   {CANCEL_ACK,               Ux_Error,    		PRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		PRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		PRI_1TR6_STYPE},
#endif
#ifdef BRI_1TR6
   {CANCEL_ACK,               Ux_Error,    		BRI_1TR6_STYPE},
   {CANCEL_REJ,               Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_ACK,             Ux_Error,    		BRI_1TR6_STYPE},
   {REGISTER_REJ_1TR6,        Ux_Error,    		BRI_1TR6_STYPE},
   {SUSPEND_ACK,              Ux_Error,     		BRI_1TR6_STYPE},
   {RESUME_REJ,               Ux_Error,     		BRI_1TR6_STYPE},
#endif
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,    		BRI_5ESS_STYPE},
#endif
   {USER_INFO,                U1_UserInfo,		VN3_STYPE},
   {UNEXPECTED,               Ux_BadMsg,    		NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * IDLE_STATE -- State 98  Idle Data Link
 */
static const DISPATCH_TABLE StateTable_98[] =
{
   {CC_BROADCAST_RESP,        U98_BroadcastResp,    	NO_ISDN_SWITCH},
   {CC_RESUME_REQ,            U98_SetupReq,		NO_ISDN_SWITCH},
   {CC_SETUP_REQ,             U98_SetupReq,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_EST_REQ,               U98_NLEstReq,		NO_ISDN_SWITCH},
   {NL_REL_REQ,               U98_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {TWAIT_EXPIRY,             U98_Twait_expiry,         NO_ISDN_SWITCH},
   {UNEXPECTED,               Ux_NOP,			NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * ESTABLISH_WAIT -- State 99  Waiting Establishment
 */
static const DISPATCH_TABLE StateTable_99[] =
{
   {DL_EST_CONF,              U99_DLEstConf,		NO_ISDN_SWITCH},
   {DL_EST_IND,               U99_DLEstInd,		NO_ISDN_SWITCH},
   {CC_SETUP_REQ,             U99_SetupReq,		NO_ISDN_SWITCH},
   {CC_ALERTING_REQ,          U99_AlertingReq,		NO_ISDN_SWITCH},
   {CC_BROADCAST_RESP,        U99_BroadcastResp,   	NO_ISDN_SWITCH},
   {CC_CALL_PROC_REQ,         U99_CallProcReq,		NO_ISDN_SWITCH},
   {CC_INFO_REQ,              U99_InfoReq,		NO_ISDN_SWITCH},
   {CC_RELEASE_REQ,           U99_ReleaseReq,		NO_ISDN_SWITCH},
   {CC_REL_COMP_REQ,          U99_RelCompReq,		VN2_STYPE},
   {CC_REL_COMP_REQ,          U99_RelCompReq,		VN3_STYPE},
   {CC_SETUP_REJ_REQ,         U99_SetupRejReq,		NO_ISDN_SWITCH},
   {CC_SETUP_RESP,            U99_SetupResp,		NO_ISDN_SWITCH},
   {DL_REL_CONF,              Ux_DLRelConf,		NO_ISDN_SWITCH},
   {DL_REL_IND,               Ux_DLRelInd,		NO_ISDN_SWITCH},
   {NL_REL_REQ,               Ux_NLRelReq,		NO_ISDN_SWITCH},
   {CC_CHNG_STATE_REQ,        Ux_ChngStateReq,		NO_ISDN_SWITCH},
   {CC_STATUS_REQ,            Ux_StatusReq,		NO_ISDN_SWITCH},
#ifdef BRI_5ESS
   {MAN_REQ,                  Ux_ManReq,    		BRI_5ESS_STYPE},
   {MAN_INFO,                 Ux_ManInfo,		BRI_5ESS_STYPE},
#endif
   {UNEXPECTED,               Ux_NOP,			NO_ISDN_SWITCH},
   {UNDEFINED,                Ux_Undefined,		NO_ISDN_SWITCH}
};

/*
 * BROADCAST_STATE -- State BCST  Messages are dispatched by another path.
 *  Undefined states accesses end up here.
 */
static const DISPATCH_TABLE StateTable_Illegal[] =
{
   {UNEXPECTED,               L3_BadState,		NO_ISDN_SWITCH},
   {UNDEFINED,                L3_BadState,		NO_ISDN_SWITCH}
};

/*
 * Table of pointers to the State Dispatch Tables (above).
 * 
 * NOTES:
 * 
 * To add a state:
 * 
 * 1. Define the new state in the L3 state enumeration in L3.H. 2. Modify the
 * State Translation Tables and their access as needed in L3.C. 3. Add the
 * new State Dispatch Table to those above. 4. Add the table pointer in the
 * appropriate position in the following pointer table.
 */
static const DISPATCH_TABLE * const StateTablePtrs[] =
{
    StateTable_00,		/* NULL_STATE */
    StateTable_01,		/* CALL_INIT */
    StateTable_02,		/* OVERLAP_SENDING */
    StateTable_03,		/* OUT_CALL_PROC */
    StateTable_04,		/* CALL_DELIVERED */
    StateTable_Illegal,    	/* Undefined state 5 */
    StateTable_06,		/* CALL_PRESENT */
    StateTable_07,		/* CALL_RECEIVED */
    StateTable_08,		/* CONNECT_REQUEST */
    StateTable_09,		/* IN_CALL_PROC */
    StateTable_10,		/* CALL_ACTIVE */
    StateTable_11,		/* DISCONNECT_REQUEST */
    StateTable_12,		/* DISCONNECT_IND */
    StateTable_Illegal,    	/* Undefined state 13 */
#ifdef BRI_1TR6
    StateTable_14,		/* PASSIVE */
#else
    StateTable_Illegal,  	/* Undefined state 14 */
#endif
    StateTable_15,		/* SUSPEND_REQUEST */
    StateTable_Illegal,  	/* Undefined state 16 */
    StateTable_17,		/* RESUME_REQUEST */
    StateTable_Illegal,  	/* Undefined state 18 */
    StateTable_19,		/* RELEASE_REQUEST */
#ifdef BRI
#ifdef BRI_1TR6
    StateTable_20,		/* REGISTER_REQUEST */
    StateTable_21,		/* CANCEL_REQUEST */
#else				/* Not BRI_1TR6 */
    StateTable_Illegal,  	/* Undefined state 20 */
    StateTable_Illegal,  	/* Undefined state 21 */
#endif				/* BRI_1TR6 */
#else				/* PRI */
#ifdef PRI_1TR6
    StateTable_20,		/* REGISTER_REQUEST */
    StateTable_21,		/* CANCEL_REQUEST */
#else				/* Not PRI_1TR6 */
    StateTable_Illegal,  	/* Undefined state 20 */
    StateTable_Illegal,  	/* Undefined state 21 */
#endif				/* PRI_1TR6 */
#endif				/* BRI/PRI */
    StateTable_Illegal,  	/* Undefined state 22 */
    StateTable_Illegal,  	/* Undefined state 23 */
    StateTable_Illegal,  	/* Undefined state 24 */
    StateTable_25,		/* OVERLAP_RECEIVE */
    StateTable_98,		/* IDLE_STATE */
    StateTable_99,		/* ESTABLISH_WAIT */
    StateTable_Illegal,  	/* Undefined 1TR6 state */
    StateTable_Illegal,  	/* Undefined state */
    StateTable_Illegal,  	/* Undefined state */
    StateTable_Illegal,  	/* Undefined state */
    StateTable_Illegal,  	/* Undefined state */
    StateTable_Illegal,  	/* Undefined state */
    StateTable_Illegal,  	/* Undefined state */
    StateTable_Illegal  	/* BROADCAST_STATE not dispatched via this
				 * route */
};


/**************
* L3_BadState *
***************
*
* Report a bad state found in an NLCB.
*
*/
PRIVATE void near L3_BadState()
{
#ifdef DEBUG
    printf("Bad state value found= %d from L3_BadState\n, NLCB->State");
    flush();			/* === */
#endif
#ifdef DISPLAY_LOCAL_ERR
    LIF_Error("l3", "L3_BadState", "Bad state in NLCB.");
#endif
}

/******************
* L3_DefinedEvent *
*******************
*
* Determine if the event is defined for execution anywhere in the SDLs
* by searching each State Dispatch Table for the event.
*
* This is recognized to be an inefficient process, but is tolerated
* because we should not be here unless we are handling an erred event.
*
* Return: TRUE if the event is defined in the table for any state.
*
*/
PRIVATE BOOLEAN near L3_DefinedEvent(event)
int event;			/* event to translate */
{
    int i;			/* Table count */
    const DISPATCH_TABLE *table;	/* State table pointer */
    BOOLEAN success = FALSE;	/* Result */

    for (i = 0; i <= MAX_L3_STATE; i++) {
	table = StateTablePtrs[i];	/* 1st table or next table pointer */
	while (table->Event != UNEXPECTED) {
	    if (table->Event == event &&
		(table->Restriction == NO_ISDN_SWITCH ||
		 table->Restriction == SwitchType)) {
		success = TRUE;	/* Success */
		break;
	    }
	    table++;		/* Next entry within the table */
	}
	if (success) {
	    break;
	}
    }

    return (success);
}

/*****************
* valid_Bchannel *
******************
*
* Added for the France Deltas to Net3, but affects other countries as
* well. For New Zealand have to verify that the Channel Id is included,
* if not, a Status message should be returned and no action taken. This is
* even stricter for Italy, cannot go to call control, have to check it here.
*/
static boolean valid_Bchannel ()
{
    BYTE *chanid_ie;
    BYTE *chanid_ie_fields;
    BOOLEAN struc_err;
    BOOLEAN noerr;

    struc_err = FALSE;
    noerr = TRUE;
    chanid_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen,
                            CHANNEL_ID_IE, CODESET_0, 1);
    if (chanid_ie) {
        /*
         * italy, TC10116, pushed this. Now have to verify that the channel
         * ie is good. If it is not, have to send STATUS and stay
         * in the same state. Can't let this go to call control; the state
         * would change and timer be stopped.
         */
        chanid_ie_fields = CCIE_ExpandChanId_IE(chanid_ie, &struc_err);
        if ((struc_err)                 ||     /* structural error */
            (chanid_ie_fields[0] != 0)  ||     /* Not Implicit interface ID */
            (chanid_ie_fields[1] != 0)  ||     /* Interface type Not BRI */
            (chanid_ie_fields[4] >= 3)  ||     /* Not chan 1 or chan 2 */
            (chanid_ie_fields[3] != 0)) {      /* it is the D-Ch ind */
             NLCB->Cause = INVALID_ELEM_CONTENTS;    /* Cause 100 */
             Diagnostic[0] = CHANNEL_ID_IE;
             Diagnostic[1] = 0;
             L3_Mail(STATUS, L2_ID, TRUE, FALSE);    /* send STATUS */
             noerr = FALSE;     /* no action or state change */
        }
        /*
         * The channel ie is okay, continue and let it go to call control.
         * This will allow the state to change.
         */
    } else {
        /*
         * Channel IE is missing and is required
         */
        NLCB->Cause = MANDATORY_IE_MISSING;
        Diagnostic[0] = CHANNEL_ID_IE;
        Diagnostic[1] = 0;
        L3_Mail(STATUS, L2_ID, TRUE, FALSE);    /* send STATUS */
        noerr = FALSE;          /* no action or state change */
    }
    return(noerr);
}

/**************
* valid_cause *
***************
*
* Added for the custom 5ESS testing. Verify that a valid cause is sent.
* Cannot let this too far thru the state machine or an incorrect call
* state gets returned. 
*/
PRIVATE boolean valid_cause ()
{
    BYTE *cause_ie;
    BYTE *cause_ie_fields;
    BOOLEAN struc_err;
    BOOLEAN valid_ie;

    struc_err = FALSE;
    valid_ie = TRUE;
    cause_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen,
                           CAUSE_IE, CODESET_0, 1);
    if (cause_ie) {
        cause_ie_fields = CCIE_ExpandCause_IE(cause_ie, &struc_err);
        if (struc_err)
             valid_ie = FALSE;     /* no action or state change */
        /*
         * The cause ie is okay, continue and let it go to call control.
         * This will allow the state to change.
         */
    } else
        valid_ie = FALSE;          /* no action or state change */
    return(valid_ie);
}

/*
 * Returns TRUE if the passed IE is mandatory.  This is pretty
 * primitive--many elements are mandatory in some cases and optional in
 * others.  We call all these cases optional.   ===
 */
static int mandatory_ie(codeset, type, ie)
int codeset, type, ie;
{
    /* see table 4-3/Q.931 (Blue Book p75), note 5 */
    if (!(ie & 0xf0) && (codeset == 0))
	return TRUE;

    switch (type) {
      case ALERTING:
      case CALL_PROC:
	return FALSE;

      case CONG_CON:
	/*
	 * This is a lie--but this is never called with single octet
	 * elements, and the only mandatory one for this is Congestion Level.
	 */
	return FALSE;

      case CONNECT:
      case CONNECT_ACK:
	return FALSE;

      case DISCONNECT:
	return codeset == 0 && ie == CAUSE_IE;

      case INFO:
	return FALSE;

      case NOTIFY:
	return codeset == 0 && ie == NOTIFY_IND_IE;

      case PROGRESS:
	return codeset == 0 && ie == PROGRESS_IND_IE;

      case RELEASE:
      case RELEASE_COMP:
      case RESUME:
	return FALSE;

      case RESUME_ACK:
	return codeset == 0 && ie == CHANNEL_ID_IE;

      case RESUME_REJ:
	return codeset == 0 && ie == CAUSE_IE;

      case SETUP:
	if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
	    if (codeset == 0)
		return ie == BEARER_CAPABILITY_IE;
	    else if (codeset == 6)
		return ie == HIGH_LAYER_COMPAT_IE;
	    else
		return FALSE;
	} else
	    return codeset == 0 && ie == BEARER_CAPABILITY_IE;

      case SETUP_ACK:
	return FALSE;

      case STATUS:
	return codeset == 0 && (ie == CAUSE_IE || CALL_STATE_IE);

      case STATUS_ENQ:
      case SUSPEND:
      case SUSPEND_ACK:
	return FALSE;

      case SUSPEND_REJ:
	return codeset == 0 && ie == CAUSE_IE;

      case USER_INFO:
	return codeset == 0 && ie == USER_USER_IE;

      case CANCEL_REJ:
      case FACILITY:
      case FACILITY_ACK:
      case FACILITY_CANCEL:
      case FACILITY_REJ:
      case REGISTER:
      case REGISTER_ACK:
      case REGISTER_REJ:
      case CONG_CON_1TR6:
      case FACILITY_1TR6:
      case FACILITY_ACK_1TR6:
      case FACILITY_REJ_1TR6:
      case INFO_1TR6:
      case REGISTER_REJ_1TR6:
      case STATUS_1TR6:
	return FALSE;	/* I don't want to think about these right now */

      default:
	return FALSE;
    }
}

/*
 * Make sure the IEs are acceptible.  Send out whatever messages are needed
 * in case they are not.  This is to conform to Q.931 (blue book) section
 * 5.8.7.1 (Unrecognized information elements). ===
 * 
 * This routine returns TRUE if the frame should be processed through the SDL.
 */
static int valid_ie_contents ()
{
    BYTE *cur, *end, type;
    int codeset = 0, locked_codeset = 0;
    int good_so_far;
    boolean status_sent;

    if (detail_debug)
	buginf("\nentering valid_ie_contents in l3_sdl_u");
    status_sent = FALSE;
    /*
     * Check for NULL pointer. Causes Spurious Accesses on the 4500.
     */
    if (L3_PktIn == NULL)
      return (FALSE);

    cur = &L3_PktIn->Info[2 + L3_PktIn->Info[1]];
    type = *cur++;
    end = &L3_PktIn->Info[L3_PktIn->InfoLen];
    if (detail_debug)
	buginf("\nvalid_ie_contents cur is %x end is %x", cur, end);
    while (cur < end) {
	/* process the IE */
	/* is it too short? (section 5.8.7.2) */
	if (!(*cur & 0x80) && &cur[1 + cur[1]] > end) {
	    /* yup, too short.  is this a mandatory element? */
	    if (mandatory_ie(codeset, type, *cur)) {
		switch (type) {
		  case SETUP:
		  case RELEASE:
		  default:
                    if (detail_debug)
	                buginf("\nexiting valid_ie_contents short mandatory setup.");
		    Diagnostic[0] = *cur;
		    Diagnostic[1] = 0;
                    /* added ni1 and dms for conformance */
		    if ((SwitchType == BRI_NET3_STYPE) ||
                        (SwitchType == BRI_DMS100_STYPE) ||
                        (SwitchType == BRI_NI1_STYPE)) {

                        /* conformance testing */
                        if ((SwitchType == BRI_NI1_STYPE) ||
                            (SwitchType == BRI_DMS100_STYPE))
			    NLCB->Cause = INVALID_ELEM_CONTENTS;  /* cause 100 */
                        else
			    NLCB->Cause = BAD_INFO_ELEM;          /* cause 99 */

			L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
			L3_CallRelease();
		        return RELEASECOMP_SENT;
		    } else {
			NLCB->Cause = INVALID_ELEM_CONTENTS;
			L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		        return FALSE;
		    }

		  case DISCONNECT:
                    if (detail_debug)
	                buginf("\nexiting valid_ie_contents short mandatory disconnect.");
		    NLCB->Cause = INVALID_ELEM_CONTENTS;
		    Diagnostic[0] = *cur;
		    Diagnostic[1] = 0;
		    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		    break;

		  case RELEASE_COMP:
                    if (detail_debug)
	                buginf("\nexiting valid_ie_contents short mandatory release comp.");
		    break;
		}
	    } else {
		/* optional IE */
                if (detail_debug)
	            buginf("\nexiting valid_ie_contents optional");
		switch (codeset) {
		  case 0:
		    switch (*cur) {
		      case USER_USER_FAC_IE:
		      case LOW_LAYER_COMPAT_IE:
		      case HIGH_LAYER_COMPAT_IE:
		      case USER_USER_IE:
		      case CALLED_SUB_ADDR_IE:
		      case CALLER_SUB_ADDR_IE:
                        if (detail_debug)
	                    buginf("\nexiting valid_ie_contents first optional.");
			NLCB->Cause = ACCESS_INFO_DISCARDED;
			Diagnostic[0] = *cur;
			Diagnostic[1] = 0;
			L3_Mail(STATUS, L2_ID, TRUE, FALSE);
			break;

		      default:
                        if (detail_debug)
	                    buginf("\nexiting valid_ie_contents first default.");
			if ((SwitchType == BRI_TS013_STYPE ||
			     SwitchType == PRI_TS014_STYPE) &&
			    (type == SETUP)) {
			    return (TRUE);
			}
			if ((SwitchType != PRI_4ESS_STYPE) &&
			    (SwitchType != PRI_5ESS_STYPE)) {
                            /*
                             * Added for 5ess conformance testing.
                             */
                            if ((SwitchType == BRI_5ESS_STYPE) ||
                                (SwitchType == BRI_DMS100_STYPE) ||
                                (SwitchType == BRI_NI1_STYPE))
			        NLCB->Cause = INVALID_ELEM_CONTENTS;
                            else
			        NLCB->Cause = BAD_INFO_ELEM;
			    Diagnostic[0] = *cur;
			    Diagnostic[1] = 0;
                            if ((SwitchType == BRI_NI1_STYPE) ||
                                (SwitchType == BRI_DMS100_STYPE)) { 
			        L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
			        L3_CallRelease();
                                return RELEASECOMP_SENT;
                            } else
			        L3_Mail(STATUS, L2_ID, TRUE, FALSE);
                            status_sent = TRUE;
			}
			break;
		    }

		  default:
                    if (detail_debug)
	                buginf("\nexiting valid_ie_contents second default.");
		    if ((SwitchType != PRI_4ESS_STYPE) &&
			(SwitchType != PRI_5ESS_STYPE) && !status_sent) {
                        /*
                         * Added for 5ess conformance testing.
                         */
                        if (SwitchType == BRI_5ESS_STYPE)
			    NLCB->Cause = INVALID_ELEM_CONTENTS;
                        else
			    NLCB->Cause = BAD_INFO_ELEM;
			Diagnostic[0] = CODESET_SHIFT_IE | codeset;
			Diagnostic[1] = *cur;
			Diagnostic[2] = 0;
			L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		    }
		    break;
		}
		break;		/* added for Australia  TS013 */
	    }
	    cur = end;		/* force this loop to end, found at AT&T */
	} else if (!(*cur & 0x80)) {	/* is this a variable length element? */
            if (detail_debug)
                buginf ("\n var ie cur is 80.... ");
	    good_so_far = TRUE;
	    switch (codeset) {
	      case 0:
		switch (*cur) {
		  case SEGMENTED_MSG_IE:
                    if (detail_debug)
                        buginf ("\n var ie segmented.... ");
		    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE ||
			SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;

		  case CHANGE_STATUS_IE:
                    if (detail_debug)
                        buginf ("\n var ie change status.... ");
		    if (SwitchType == BRI_NET3_STYPE ||
			SwitchType == BRI_TS013_STYPE ||
                        SwitchType == PRI_NET5_STYPE ||
                        SwitchType == PRI_TS014_STYPE)
			goto unrec_elem;
		    break;
		  case BEARER_CAPABILITY_IE:
		  case CAUSE_IE:
                    if (detail_debug)
                        buginf ("\n var ie bear cap num.... ");
		    break;
		  case CONNECTED_NUM_IE:
                    if (detail_debug)
                        buginf ("\n var ie connected num.... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case CALL_IDENT_IE:
		  case CALL_STATE_IE:
		  case FACILITY_IE:
		  case PROGRESS_IND_IE:
		  case ENDPOINT_ID_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of others... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case NOTIFY_IND_IE:
		  case DISPLAY_IE:
		  case TIME_DATE_IE:
		  case KEYPAD_FACILITY_IE:
		  case INFO_REQ_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of others and then some... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case SIGNAL_IE:
		  case SWITCHHOOK_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of hook... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case FEATURE_ACTIV_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of feature... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case FEATURE_IND_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of feature ind... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case INFO_RATE_IE:
		  case TRANSIT_DELAY_EE_IE:
		  case TRANSIT_DELAY_SEL_IE:
		  case PACKET_BIN_PARMS_IE:
		  case PACKET_WINDOW_IE:
		  case PACKET_SIZE_IE:
		  case CALLER_NUMBER_IE:
		  case CALLER_SUB_ADDR_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of packet clear... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case CALLED_NUMBER_IE:
                    if (detail_debug)
                        buginf ("\n var ie bear called num.... ");
		    break;
		  case CALLED_SUB_ADDR_IE:
		  case ORIGINAL_CALLED_NUM_IE:
		  case REDIRECTING_NUM_IE:
		  case REDIRECTION_NUM_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of redir... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case TRANSIT_NETWORK_SEL_IE:
		  case RESTART_IND_IE:
		  case USER_USER_FAC_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of U U Ur... ");
		    if (SwitchType == BRI_NET3_STYPE)
			goto unrec_elem;
		    break;
		  case LOW_LAYER_COMPAT_IE:
		  case HIGH_LAYER_COMPAT_IE:
		  case ESCAPE_FOR_EXT_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of LLr... ");
		    if (SwitchType == BRI_TS013_STYPE ||
		        SwitchType == PRI_TS014_STYPE)
			goto unrec_elem;
		    break;

		  case CHANNEL_ID_IE:
		  case NETWORK_FACILITY_IE:
		  case USER_USER_IE:
                    if (detail_debug)
                        buginf ("\n var ie bunch of cug... ");
		    /* these are always known */
		    break;

		  default:
	    unrec_elem:
		    /* we have an unrecognized element */
		    if (detail_debug)
			buginf("\nvalid_ie_contents, unrecognized element in valid_ie");

		    switch (type) {
		      case RELEASE_COMP:
		        if (detail_debug)
                            buginf ("\n var ie bear rel comp.... ");
			break;
		      case DISCONNECT:
                        if (detail_debug)
                            buginf ("\n var ie bear disc.... ");
			if (SwitchType == BRI_TS013_STYPE ||
			    SwitchType == PRI_TS014_STYPE)
			    NLCB->Cause = BAD_INFO_ELEM;
                        /* italy homologation, TC11018 */
                        if (SwitchType == BRI_NET3_STYPE) {
                            NLCB->Cause = MANDATORY_IE_MISSING;
                            Diagnostic[0] = *cur;
                            Diagnostic[1] = 0;
                        }
			break;

                      case CONNECT:
                      case CONNECT_ACK:
                      case CALL_PROC:
                        /*
                         * italy homologation, TC10117, TC10219
                         */ 
                        if (detail_debug)
                            buginf ("\n var ie bear conn.... ");
                        if (!(*cur & 0xf0) &&   /* ===uk */
                            ((cur[1] == 0) || (cur[1] == 1)) &&
                             (SwitchType == PRI_NET5_STYPE ||
                              SwitchType == BRI_NET3_STYPE)) {
                            NLCB->Cause = MANDATORY_IE_MISSING;
                            Diagnostic[0] = *cur;
                            Diagnostic[1] = 0;
                            L3_Mail(STATUS, L2_ID, TRUE, FALSE);
                            return FALSE;
                        }
                        break;

		      case SETUP:
                        if (detail_debug)
                            buginf ("\n var ie setup with f0... ");
			if (!(*cur & 0xf0) &&	/* ===uk */
			    ((cur[1] == 0) || (cur[1] == 1))) {
			    NLCB->Cause = MANDATORY_IE_MISSING;
			    Diagnostic[0] = 0;
			    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
			    L3_CallRelease();
			    return RELEASECOMP_SENT;
			}
			break;

		      case RELEASE:
                        if (detail_debug)
                            buginf ("\n var ie rele with f0... ");
                        if ((SwitchType == PRI_NET5_STYPE) ||
                            (SwitchType == BRI_NET3_STYPE)) {
                            L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
                            /*
                             * comprehension req'd cause 96
                             * comprehension not req'd cause 99
                             */
                            if (*cur & 0x80)
                                NLCB->Cause = BAD_INFO_ELEM;
                            else
                                NLCB->Cause = MANDATORY_IE_MISSING;
                            Diagnostic[0] = *cur;
                            Diagnostic[1] = 0;
                            L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
                            if (SwitchType == PRI_NET5_STYPE)
                                L3_CallRelease();
                            return FALSE;
                        }
			NLCB->Cause = BAD_INFO_ELEM;
			Diagnostic[0] = *cur;
			Diagnostic[1] = 0;
			/* will get tacked to the RELEASE/RELCOM we send */
			if (SwitchType == BRI_TS013_STYPE ||
			    SwitchType == PRI_TS014_STYPE) {
			    L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
			    NLCB->Cause = BAD_INFO_ELEM;
			    Diagnostic[0] = *cur;
			    Diagnostic[1] = 0;
			    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
			    L3_CallRelease();
			    return FALSE;
			}
			break;

		      default:
                        if (detail_debug)
                            buginf ("\n var ie def with f0... ");
			break;
		    }
		}
		break;

	      case 1:		/* codeset 1 */
	      case 2:		/* codeset 2 */
	      case 3:		/* codeset 3 */
	      case 4:		/* codeset 4 */
	      case 5:		/* codeset 5 */
	      case 7:		/* codeset 5 */
	bad_codeset_elem:
                if (detail_debug)
                    buginf ("\n var ie code set... ");
		break;

	      case 6:		/* codeset 6 */
		switch (SwitchType) {
		  case NTT_STYPE:
		  case KDD_STYPE:
		    switch (*cur) {
		      case ADVICE_OF_CHARGE_IE:
		      case USER_CODE_IE:
		      case TRAVEL_CLASSMARK_IE:
		      case ORIG_CALL_APPEAR_IE:
		      case TERMINAL_CAP_IE:
		      case DISPLAY_IE:
		      case OTHER_CALL_REF_IE:
			break;

		      default:
                        if (detail_debug)
                            buginf ("\n var ie bad code set... ");
			goto bad_codeset_elem;
		    }
		    break;

		  case VN2_STYPE:
		  case VN3_STYPE:
		    switch (*cur) {
		      case USER_USER_FAC_IE:
		      case MODE_DE_FONCTIONNEMENT_D_USER:
			break;

		      default:
                        if (detail_debug)
                            buginf ("\n var ie vn other bad code set... ");
			goto bad_codeset_elem;
		    }
		    break;

		    /*
		     * #if defined(BRI_1TR6_STYPE) || defined(PRI_1TR6_STYPE)
		     * ===
		     */
#if defined(BRI_1TR6) || defined(PRI_1TR6)
		  case BRI_1TR6_STYPE:
		  case PRI_1TR6_STYPE:
		    switch (*cur) {
		      case SERVICE_INDICATOR_IE:
		      case CHARGING_INFORMATION_IE:
		      case DATE_IE:
		      case STATUS_FACILITIES_IE:
		      case STATUS_GERUF_TLN_IE:
		      case ADD_TRANSMIT_ATTR_IE:
			break;

		      default:
			goto bad_codeset_elem;
		    }
		    break;
#endif
		  case PRI_4ESS_STYPE:
		  case PRI_5ESS_STYPE:
		    if (detail_debug)
                        buginf ("\n var ie  other ess bad code set... ");
		    break;

		  default:
		    if (detail_debug)
                        buginf ("\n var ie  other def bad code set... ");
		    goto bad_codeset_elem;
		}
		break;
	    }
	    cur = &cur[2 + cur[1]];
	} else {
	    /* it's a single octet IE */
            if (detail_debug)
                buginf ("\n var ie  single... ");
	    if ((*cur & 0xf0) == CODESET_SHIFT_IE) {
		/* we must parse this one */
		if (*cur & NON_LOCKING_SHIFT_MASK)
		    codeset = *cur & 7;
		else
		    locked_codeset = codeset = *cur & 7;
	    } else {
                if (detail_debug)
                    buginf ("\n var ie  single congest... ");
		if ((*cur & 0xf0) == CONGEST_LEVEL_IE ||
		    (*cur & 0xf0) == REPEAT_IND_IE ||
		    *cur == MORE_DATA_IE || *cur == SENDING_COMPLETE_IE) {
		    ;		/* these we always know */
		} else {
		    /* we have an unrecognized element */
		    switch (type) {
		      case STATUS:	/* for test ISTA-Unk1 &&& */
		      case RELEASE_COMP:
                        if (detail_debug)
                            buginf ("\n var ie  single congest status... ");
			break;

		      case DISCONNECT:
                          if (detail_debug)
                              buginf ("\n var ie  single congest disc... ");
                          break;

		      case RELEASE:
                        if (detail_debug)
                            buginf ("\n var ie  single congest rel... ");
                        /*
                         * italy homologation, TC11117, TC1118
                         */
                        if ((SwitchType == PRI_NET5_STYPE) ||
                            (SwitchType == BRI_NET3_STYPE)) {
                            L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
                            /*
                             * comprehension req'd cause 96
                             * comprehension not req'd cause 99
                             */
                            if (*cur & 0x80)
                                NLCB->Cause = BAD_INFO_ELEM;
                            else
                                NLCB->Cause = MANDATORY_IE_MISSING;
                            /*
                             * italy homologation
                             */
                            Diagnostic[0] = *cur;
                            Diagnostic[1] = 0;
                            L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
                            if (SwitchType == PRI_NET5_STYPE)
                               L3_CallRelease();
                            return FALSE;
                        }
                        break;
		      case SETUP:
                        if (detail_debug)
                            buginf ("\n var ie  single setup... ");
			break;

		      default:
                        if (detail_debug)
                            buginf ("\n var ie  single break... ");
			break;
		    }
		}
	    }
	    cur++;
	    codeset = locked_codeset;
	}
    }
    if (detail_debug)
	buginf("\nexiting valid_ie_contents with TRUE.");
    return TRUE;
}

/******************
* L3_ProcessEvent *
*******************
*
* Process an event based on the current state. (see Appendix A-2/Q.931).
* Locate the State Dispatch Table from NLCB->State.  Then locate the event,
* if present, by searching the table from top to bottom.
*
*/
PUBLIC void L3_ProcessEvent()
{
    const DISPATCH_TABLE *table;
    int save_ces, ret_val, check_val;
   
    if (detail_debug) {
	buginf("\nL3 SDL: State=%d  Event=0x%x ces=%x", NLCB->State, NLCB->Event, NLCB->Ces);
	flush();
    }
    if (L3_ValidState((int) NLCB->State)) {
	table = StateTablePtrs[NLCB->State];	/* Get table for state */
    } else {
	table = StateTablePtrs[BROADCAST_STATE];	/* Bad state table */
    }
    for (;;) {			/* Search the state table for the event */
	if (table->Event == NLCB->Event &&
	    (table->Restriction == NO_ISDN_SWITCH ||
	     table->Restriction == SwitchType)) {
	    break;
	} else if (table->Event == UNEXPECTED) {	/* End of table */
	    if (!L3_DefinedEvent(NLCB->Event)) {
		table++;	/* Point to default UNDEFINED entry */
	    }
	    break;
	} else {
	    table++;
	}
    }

    /*
     * Conform to Blue Book Q.931, section 5.8.7.1
     * 
     * The test on 0x80 is based on the fact that all received frames are event
     * numbers less than 0x80.  Internally generated events are greater than
     * 0x80.
     * 
     * We must set the ces to 1 to force any STATUS messages we decide to send
     * to go out on our TEI, not on TEI 127 which results in UI frames that
     * blow up real good.  This is only an issue when we receive poorly
     * formed SETUP messages. Unfortunately, these account for 116 tests in
     * the homologation test suite.			===
     * 
     * Don't call if switchtype is BRI_1TR6_STYPE, network doesn't like the
     * STATUS messages that are returned
     */
    if (SwitchType != BRI_1TR6_STYPE) {
	save_ces = NLCB->Ces;
	NLCB->Ces = 1;		/* This does not scale!!!  === */
        ret_val = valid_ie_contents();
        if (ret_val == RELEASECOMP_SENT)
            check_val = FALSE;
        else 
            check_val = ret_val;
	if (NLCB->Event < 0x80 && !check_val) {
            /*
             * If we are a NET5 or a NET3 switchtype there is a chance
             * that we sent a RELEASE_COMP from valid_ie_contents.
             * If we did in response to a SETUP our Ces value
             * is now 1 and each subsequent incoming SETUP will
             * be unable to find a DLCB so let's set the Cess
             * value back to the initial value. (BROADCAST_CES = 0).
             * If a RELEASE COMPLETE was sent in valid_ie_contents a
             * RELEASECOMP_SENT will be returned from valid_ie_contents.
             */
            if ((SwitchType == PRI_NET5_STYPE || SwitchType == BRI_NET3_STYPE
              || SwitchType == BRI_DMS100_STYPE || SwitchType == BRI_NI1_STYPE) 
               && (ret_val == RELEASECOMP_SENT))
	        NLCB->Ces = BROADCAST_CES;
            else
	        NLCB->Ces = save_ces;
            if (detail_debug) {
                buginf("\nmust be getting bad value from valid_ie <<<<");
                buginf("\n... event is %x", NLCB->Event);
            }
	    return;
	}
	NLCB->Ces = save_ces;
    }
    if (detail_debug)
       buginf("\nL3 jump to table->EventFunc = 0x%x", table->EventFunc); 
    (table->EventFunc) ();	/* Call the event function for the event */
}

/*****************
* L3_RejectCause *
******************
*
* Test the cause information element for its absence and for incorrectness
* in its coding.
*
*/
PRIVATE int near L3_RejectCause(ie_cause)
BYTE *ie_cause;			/* Output cause value from IE */
{
    int cause = 0;		/* Cause for cause info element rejection */
    BYTE *cause_ie;
    int struc_err;		/* === */
    BYTE *unpacked_ie;


    *ie_cause = 0;
    cause_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen,
			   CAUSE_IE, CODESET_0, 1);
    if (cause_ie) {
	struc_err = 0;		/* === */
	unpacked_ie = CCIE_ExpandCause_IE(cause_ie, &struc_err);	/* === */
	if ((SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) && struc_err) {
	    Diagnostic[0] = CAUSE_IE;	/* === */
	    Diagnostic[1] = 0;	/* === */
	    return struc_err;	/* === */
	}			/* === */
	/*
	 * Test every field of the info element for coding correctness.
	 * expanded the Location to include Intern'l, the NET3 code has to
	 * call this routine to save off the Cause value ===
	 */
	if (((unpacked_ie[0] == 0) ||   /* Coding std = CCITT */
	    (unpacked_ie[0] == 3))      /* Coding std = Net-spec */
                                    && 
	    ((unpacked_ie[1] == 0x0) ||	/* Location == User */
	    (unpacked_ie[1] == 0x2) ||	/* Location == Local network */
	    (unpacked_ie[1] == 0x7))) {	/* Location == International */
	    if (!CCIE_CauseValid((int) unpacked_ie[2])) {
                if (detail_debug)
		    buginf ("\n L3_RejectCause setting INVALID 1.");
		cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
		Diagnostic[0] = CAUSE_IE;
		Diagnostic[1] = 0;
	    } else {
		*ie_cause = unpacked_ie[2];	/* Recover the cause value */
		cause = unpacked_ie[2];	/* === */
	    }
	} else {
            if (detail_debug)
                buginf ("\n L3_RejectCause setting INVALID 2.");
	    cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
	    Diagnostic[0] = CAUSE_IE;
	    Diagnostic[1] = 0;
	}
    } else {
        if (detail_debug)
            buginf ("\n L3_RejectCause setting MANDATORY.");
	cause = MANDATORY_IE_MISSING;	/* Cause 96 */
	Diagnostic[0] = CAUSE_IE;
	Diagnostic[1] = 0;
    }

    return (cause);
}

/*******************
* L3_VerifyDisplay *
********************
*
* Find and validate the DISPLAY info element in the incoming message.
* Don't worry about recovering the characters, they are not currently
* used. Needed for Australia TS013 and TS014 homologation.
*/
PRIVATE int L3_VerifyDisplay ()
{
   int  ix;
   int  cause;
   BYTE *ie;

   cause = NOERR;
   ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen, DISPLAY_IE, CODESET_0, 1);
   if (ie) {
      if ((ie[1] > 0) && (ie[1] <= DISPLAY_IE_LEN)) {
         for (ix = 2; ix < ie[1]; ix++) {
            if (ie[ix] & 0x80)
                cause = INVALID_ELEM_CONTENTS;              /* Cause 100 */
         }
      } else
         /* length error */
         cause = INVALID_ELEM_CONTENTS;                     /* Cause 100 */
   }
   return(cause);
}

/*
 * SDL functions by STATE
 */

/* **********************  U 0 :   N U L L   S T A T E  ************** */

/*************
*  U0_BadMsg *
**************
*
* State = 0;  Event = Unexpected Message.
*/
PRIVATE void near U0_BadMsg()
{
    if (L3_MsgSource == L2_ID) {
	switch (SwitchType) {
#ifdef PRI_1TR6
	    case PRI_1TR6_STYPE:
	    break;		/* ignore */
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    break;		/* ignore */
#endif
#ifdef BRI_DMS100
	  case BRI_DMS100_STYPE:
	    NLCB->Cause = WRONG_MSG_FOR_STATE;	/* cause 101 */
	    Diagnostic[0] = NLCB->Event;
	    Diagnostic[1] = 0;
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    L3_CallRelease();	/* State 0 */
	    break;
#endif
	  default:
#ifdef DISPLAY_LOCAL_ERR
	    LIF_Error("l3_sdl_u", "U0_BadMsg", "Bad message received.");
#endif
	    L3_CallRelease();
	    break;
	}
    } else {
	switch (SwitchType) {
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    if (L3_MsgSource == CC_ID)
		L3_Mail(CC_ERROR_IND, CC_ID, FALSE, FALSE);
	    break;
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    if (L3_MsgSource == CC_ID)
		L3_Mail(CC_ERROR_IND, CC_ID, FALSE, FALSE);
	    break;
#endif
	  default:
	    break;
	}
#ifdef DISPLAY_LOCAL_ERR
	LIF_Error("l3_sdl_u", "U0_BadMsg", "Bad message received.");
#endif
	L3_CallRelease();
    }
}

/********************
*  U0_BroadcastResp *
*********************
*
* State = 0;  Event = Call Control Broadcast Setup Response
*
*  Broadcast Call Origination from the NETWORK
*
*  In certain special applications, the Channel ID must be returned in
*  this message in the case where Call Control is unable to respond
*  immediately with any standard State 6 response as an immediate followup
*  to CC_BROADCAST_RESP.  This update is required to associate this NLCB
*  to a B-Channel in the event the network restarts this channel during
*  the time window.  Otherwise the NLCB will be stranded.
*
*/
PRIVATE void near U0_BroadcastResp()
{
#ifdef BRI
    L3_UpdateChannel(L3_PktIn);
#endif
    NLCB->State = CALL_PRESENT;	/* State 6 */
}

/****************
* U0_FacCnclReq *
*****************
*
* State = 0;  Event = Call Control Facility Cancel Request
*
*/
PRIVATE void near U0_FacCnclReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_CallRefSelect(TRUE);
	L3_Mail(FACILITY_CANCEL, L2_ID, FALSE, FALSE);
	NLCB->State = CANCEL_REQUEST;	/* 1TR6 State 21 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_CallRefSelect(TRUE);
	L3_Mail(FACILITY_CANCEL, L2_ID, FALSE, FALSE);
	NLCB->State = CANCEL_REQUEST;	/* 1TR6 State 21 */
	break;
#endif
      default:
	U0_BadMsg();
	break;
    }
}

/* === added the following routine */
/****************
* U0_NLEstReq   *
*****************
*
* State = 0;  Event = NL_EST_REQ in State 0
*
* #ifdef N_AMERICA_FIT ===
* This will allow BRI call control to have less concern about
* the status of the link if SPID/TID initialization request occurs -
* this may happen at any time whether the link is up or down.
* #endif
*/
PRIVATE void near U0_NLEstReq()
{
    switch (SwitchType) {
	case BRI_5ESS_STYPE:
	case BRI_NI1_STYPE:
	case BRI_DMS100_STYPE:
	L3_Mail(NL_EST_CONF, CC_ID, FALSE, FALSE);
	break;
      default:
	Ux_NOP();
	break;
    }
}
/* === added the above routine */

/*********
* U0_NOP *
**********
*
* NOP a message, releasing the NLCB.
*
*/
PRIVATE void near U0_NOP()
{
    L3_CallRelease();
}

/*****************
* U0_RegisterReq *
******************
*
* State = 0;  Event = Call Control Facility Register Request
*
*/
PRIVATE void near U0_RegisterReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_CallRefSelect(TRUE);
	L3_Mail(REGISTER, L2_ID, FALSE, FALSE);
	NLCB->State = REGISTER_REQUEST;	/* 1TR6 State 20 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_CallRefSelect(TRUE);
	L3_Mail(REGISTER, L2_ID, FALSE, FALSE);
	NLCB->State = REGISTER_REQUEST;	/* 1TR6 State 20 */
	break;
#endif
      default:
	U0_BadMsg();
	break;
    }
}

/****************
*  U0_ResumeReq *
*****************
*
* State = 0;  Event = Call Control Resume Request
*
*  Call Resume from the USER TERMINAL
*
*/
PRIVATE void near U0_ResumeReq()
{
    L3_CallRefSelect(TRUE);
    L3_Mail(RESUME, L2_ID, FALSE, FALSE);
    LIF_StartTimer(NLCB->Timer_T318);
    NLCB->State = RESUME_REQUEST;	/* State 17 */
}


/************
*  U0_Setup *
*************
*
* State = 0;  Event = Peer Setup Command
*
* NOTE:  THIS MESSAGE USES THE BROADCAST NLCB IN ORDER TO SEND A MESSAGE
*        TO THE CALL CONTROL.  After the L3_Mail has been sent, the
*        generic BROADCAST NLCB is restored to its standard condition
*        (State and CES).  The call is now disassociated from any
*        NLCB at Protocol Control and will only be reassociated with
*        some other NLCB when Call Control responds to the CC_SETUP_IND.
*        At this point, the new NLCB will assume State 6.
*
* Note that the Call Reference flag bit is not checked here for the
* correct sense.  It is checked in State 6 when Call Control responds
* to the SETUP.
*
*/
PRIVATE void near U0_Setup()
{
    L3_Mail(CC_SETUP_IND, CC_ID, FALSE, FALSE);
    L3_CallRelease();
    NLCB->State = BROADCAST_STATE;	/* restore Broadcast NLCB */
    NLCB->Ces = BROADCAST_CES;
}

/***************
*  U0_SetupReq *
****************
*
* State = 0;  Event = Call Control Setup Request
*
*  Call Origination from the USER TERMINAL
*
*/
PRIVATE void near U0_SetupReq()
{
#ifdef BRI
    L3_UpdateChannel(L3_PktIn);	/* Attempt B-Channel update */
#endif
    L3_CallRefSelect(TRUE);
    L3_SavePkt(&NLCB->Setup, L3_PktIn);	/* save SETUP */
    L3_Mail(SETUP, L2_ID, FALSE, FALSE);
    LIF_StartTimer(NLCB->Timer_T303);
    switch (SwitchType) {
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	NLCB->T303_First = FALSE;	/* One timeout only. */
	NLCB->T303_First = TRUE;/* ===  5.2.2.2 (3)(c) (p93) */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	NLCB->T303_First = FALSE;	/* One timeout only. */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	NLCB->T303_First = FALSE;	/* One timeout only. */
	break;
#endif
#ifdef BRI_NET3
      case BRI_NET3_STYPE:
	NLCB->T303_First = TRUE;
	break;
#endif
#ifdef BRI_TS013
      case BRI_5ESS_STYPE:
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	NLCB->T303_First = FALSE;	/* One timeout only. */
	break;
#endif
      default:
	break;
    }

    NLCB->State = CALL_INIT;	/* State 1 */
}

/****************
*  U0_Undefined *
*****************
*
* State = 0;  Event = Unrecognized Message.
*/
PRIVATE void near U0_Undefined()
{
    if (L3_MsgSource != L2_ID) {
#ifdef DISPLAY_LOCAL_ERR
	LIF_Error("l3_sdl_u", "U0_Undefined", "Bad message received.")
#endif
	;
    }
}


/* ******  U 1 :   C A L L   I N I T I A L I Z E D   S T A T E  ****** */

/***************
*  U1_Alerting *
****************
*
* State = 1;  Event = Peer Alerting Command
*
*/
PRIVATE void near U1_Alerting()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
      case BRI_NET3_STYPE:
        /*
         * Channel is mandatory with this message, but if the IE
         * is incorrect do not allow the state change.
         */
        if (!valid_Bchannel()) {
           break;
        }
        /* fall thru if channel IE is okay */

      default:
	if (NLCB->Setup) {
	    LIF_FreePkt(NLCB->Setup);	/* Discard saved SETUP */
	    NLCB->Setup = NULL;
	}
#ifdef BRI
	L3_UpdateChannel(L3_PktIn);	/* Attempt B-Channel update */
#endif
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_ALERTING_IND, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_DELIVERED;	/* State 4 */
	break;
    }
}

/*************
*  U1_BadMsg *
**************
*
* States = 1,6;  Event = Unexpected Message.
*/
PRIVATE void near U1_BadMsg()
{
    if (L3_MsgSource == L2_ID) {
	switch (SwitchType) {
	  case PRI_4ESS_STYPE:
	  case PRI_5ESS_STYPE:
	  case PRI_NET5_STYPE:
	  case PRI_TS014_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#ifdef BRI_5ESS
	  case BRI_5ESS_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif

#ifdef BRI_NI1			/* === */
	  case BRI_DMS100_STYPE:
	  case BRI_NI1_STYPE:
	    NLCB->Cause = WRONG_MSG_FOR_STATE;  /* Cause 101 */
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef PRI_DMS100
	  case PRI_DMS100_STYPE:
	    Ux_BadMsg();
	    break;
#endif
#ifdef VN2
	  case VN2_STYPE:
	  case VN3_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef BRI_NET3			/* === */
	  case BRI_NET3_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef BRI_TS013
	  case BRI_TS013_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef KDD_NTT
	  case KDD_STYPE:
	  case NTT_STYPE:
	  case PRI_NTT_STYPE:
	    NLCB->Cause = WRONG_MSG_FOR_STATE;	/* cause 101 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    break;		/* Ignore */
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    break;		/* Ignore */
#endif
	  default:
	    L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);
	    break;
	}
    } else {
#ifdef DISPLAY_LOCAL_ERR
	LIF_Error("l3_sdl_u", "U1_BadMsg", "Bad message received.")
#endif
	    ;
    }
}

/***************
*  U1_CallProc *
****************
*
* State = 1;  Event = Peer Call Proceeding
*
*/
PRIVATE void near U1_CallProc()
{
    switch (SwitchType) {
        /*
         * Volcano conformance, have to make sure a B-channel exists,
         * if not then ignore the message.
         */
      case BRI_5ESS_STYPE:
      case BRI_NET3_STYPE:
        /*
         * Channel is mandatory with this message, but if the IE
         * is incorrect do not allow the state change.
         */
        if (!valid_Bchannel()) {
            break;
        }
	/*
	 * fall thru for cases that have a good Channel IE
	 */

      default:
	if (NLCB->Setup) {
	    LIF_FreePkt(NLCB->Setup);	/* Discard saved SETUP */
	    NLCB->Setup = NULL;
	}
	L3_UpdateChannel(L3_PktIn);	/* Attempt B-Channel update */
	LIF_StopTimer(NLCB->Timer_T303);
	if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE ||
	    SwitchType == PRI_4ESS_STYPE || SwitchType == PRI_5ESS_STYPE ||
	    SwitchType == PRI_DMS100_STYPE || SwitchType == PRI_NET5_STYPE ||
            SwitchType == BRI_NET3_STYPE)
	    LIF_StartTimer(NLCB->Timer_T310);
	L3_Mail(CC_CALL_PROC_IND, CC_ID, FALSE, FALSE);
	NLCB->State = OUT_CALL_PROC;	/* State 3 */
	break;
    }
}

/**************
*  U1_Connect *
***************
*
* State = 1;  Event = Peer Connect Command
*
*/
PRIVATE void near U1_Connect()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
      case BRI_NET3_STYPE:
        /*
         * Channel is mandatory with this message, but if the IE
         * is incorrect do not allow the state change.
         */
        if (!valid_Bchannel()) {
           break;
        }
        /* fall thru for good cases */

      default:
	if (NLCB->Setup) {
	    LIF_FreePkt(NLCB->Setup);	/* Discard saved SETUP */
	    NLCB->Setup = NULL;
	}
#ifdef BRI
	L3_UpdateChannel(L3_PktIn);	/* Attempt B-Channel update */
#endif
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_SETUP_CONF, CC_ID, FALSE, FALSE);
	L3_Mail(CONNECT_ACK, L2_ID, TRUE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
    }
}

/****************
* U1_Disconnect *
*****************
*
* State = 1; Event = Peer Disconnect Message
*
*/
PRIVATE void near U1_Disconnect()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	NLCB->State = DISCONNECT_IND;	/* State 12 */
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	NLCB->State = DISCONNECT_IND;	/* State 12 */
	break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	/*
	 * NIS S208-5T issue 1.1 release 2.2 see section 6.1.3.4ab of NIS
	 * S208-5S ISSUE 1.1
	 */
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();		/* Bad message for PRI_DMS100 equipment */
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	NLCB->State = DISCONNECT_IND;	/* State 12 */
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	NLCB->State = DISCONNECT_IND;	/* State 12 */
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	NLCB->State = DISCONNECT_IND;	/* State 12 */
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	NLCB->State = DISCONNECT_IND;	/* State 12 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	NLCB->State = DISCONNECT_IND;	/* State 12 */
	break;
#endif
      default:
	/*
	 * bad message
	 */
	L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);
	break;
    }
}

/**************
*  U1_DiscReq *
***************
*
* State = 1;  Event = Call Control Disconnect Request
*
*/
PRIVATE void near U1_DiscReq()
{
    L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
#ifdef BRI_DMS100
    if (SwitchType == BRI_DMS100_STYPE)
	L3_RejectCause(&NLCB->PreviousCause);	/* Save cause in NLCB */
#endif
    LIF_StopTimer(NLCB->Timer_T303);
    LIF_StartTimer(NLCB->Timer_T305);
    NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
}

/***********
*  U1_Info *
************
*
* State = 1;  Event = Peer Information
*
*/
PRIVATE void near U1_Info()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	U1_BadMsg();
	break;

#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_TS013		/* === */
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
	Ux_BadMsg();
	break;
      case NTT_STYPE:
#ifdef BRI
	L3_UpdateChannel(L3_PktIn);	/* Attempt B-Channel update */
#endif
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* Send up to CC */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
      default:
#ifdef BRI
	L3_UpdateChannel(L3_PktIn);	/* Attempt B-Channel update */
#endif
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* Send up to CC */
	break;
    }
}

/**************
*  U1_InfoReq *
***************
*
* State = 1;  Event = Call Control Information Request
*
*  Cannot process this event until ESTABLISHED and OVER SENDING (state 2).
*  Send this packet to ourselves with a delay
*  (give ourselves time to get to state 2).
*
*/
PRIVATE void near U1_InfoReq()
{
    L3_EnQueue(&NLCB->InfoQ);
}

/****************
*  U1_Progress  *
*****************
*
* State = 1;  Event = Peer Progress Indication
*
*/
PRIVATE void near U1_Progress()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	U1_BadMsg();
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
	if (NLCB->Setup) {
	    LIF_FreePkt(NLCB->Setup);	/* Discard saved SETUP */
	    NLCB->Setup = NULL;
	}
	L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_DELIVERED;	/* State 4 */
	break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:	/* === */
	U1_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	U1_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	break;
    }
}

/**************
*  U1_Release *
***************
*
* State = 1;  Event = Peer Release Message
*                        (with Cause)
*
*/
PRIVATE void near U1_Release()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
        /*
         * changed around for conformance testing 
         */
        NLCB->Cause = L3_RejectCause((BYTE *) & Trashcan);
	if ((NLCB->Cause == INVALID_ELEM_CONTENTS) || (NLCB->Cause == MANDATORY_IE_MISSING))
	    Diagnostic[0] = CAUSE_IE;
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, FALSE, FALSE);	/* === */
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);	/* === */
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
	/*
	 * wait here for Call Control to validate the Cause and Display
	 * information. It will reply with a CC_REL_COMP_REQ message, then
	 * the Release Complete can be sent. ===
	 */
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);	/* === */
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      default:
	L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);	/* bad message */
	break;
    }
}

/******************
*  U1_ReleaseComp *
*******************
*
* State = 1;  Event = Peer Release Complete
*
*/
PRIVATE void near U1_ReleaseComp()
{
    BOOLEAN cause_ie;

    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	Ux_ReleaseComp();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_ReleaseComp();
	break;
#endif
      case BRI_5ESS_STYPE:
        /*
         * Only accept a Release Complete that has a cause value, if a
         * cause is not included, ignore message and stay in the same state.
         */
        cause_ie = valid_cause();
        if (cause_ie) {
            LIF_StopTimer(NLCB->Timer_T303);
            L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
            L3_CallRelease();       /* State 0 */
        }
	break;

      default:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
    }
}

/***************
*  U1_SetupAck *
****************
*
* State = 1;  Event = Peer Setup Acknowledge
*
*/
PRIVATE void near U1_SetupAck()
{
    BYTE *chanid_ie;

    switch (SwitchType) {
      /*
       * Actually, this should not happen as we now send a Sending Complete
       * in the outgoing Setup message, done for cancun === I cannot
       * entirely make out the French, but it appears that receiving
       * SETUPACKs is bad in en-bloc mode.  I don't know how T'works keeps
       * track of whether they are doing en-bloc or not (unless it's the
       * COMMAND_MODE #define).  In any case, we only use en-bloc, so this
       * should be safe.
       * 
       * But having a call to Ux_BadMsg() here breaks INV/U1/ISAc-mis1.  Alas,
       * we are missing the description page for this test, so I don't
       * really know what it's supposed to do.
       */
      case VN2_STYPE:
      case VN3_STYPE:
        /*
         * Verify the ChanId IE is present and well formed (ChanID is
         * mandatory here).
         */
        if ((chanid_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen,
                                     CHANNEL_ID_IE, CODESET_0, 1)) == NULL) {
            NLCB->Cause = MANDATORY_IE_MISSING;
            L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, TRUE, FALSE);
            NLCB->Cause = MANDATORY_IE_MISSING;
            L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
            L3_CallRelease();
            break;
        }
        if (chanid_ie[1] != 1) {
            NLCB->Cause = MANDATORY_IE_LEN_ERR;
            L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, TRUE, FALSE);
            NLCB->Cause = MANDATORY_IE_LEN_ERR;
            L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
            L3_CallRelease();
            break;
        }
        if ((chanid_ie[2] & 0x40) || (chanid_ie[2] & 0x20) ||
            (chanid_ie[2] & 0x10) || (chanid_ie[2] & 0x04)) {
            NLCB->Cause = INVALID_ELEM_CONTENTS;
            L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, TRUE, FALSE);
            NLCB->Cause = INVALID_ELEM_CONTENTS;
            L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
            L3_CallRelease();
            break;
        }
        Ux_BadMsg();
        break;

#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
	Ux_BadMsg();		/* Overlap sending not supported in KDD_NTT */
	break;
#endif

      case BRI_5ESS_STYPE:
        /*
         * Volcano conformance testing, have to make sure a B-channel exists,
         * if not, then ignore the message.
         */
        chanid_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen,
                                CHANNEL_ID_IE, CODESET_0, 1);
        if (chanid_ie) {
            if (NLCB->Setup) {
                LIF_FreePkt(NLCB->Setup);   /* Discard saved SETUP */
                NLCB->Setup = NULL;
            }
            L3_UpdateChannel(L3_PktIn);     /* Attempt B-Channel update */
            LIF_StopTimer(NLCB->Timer_T303);
            L3_Mail(CC_MORE_INFO_IND, CC_ID, FALSE, FALSE);
            NLCB->State = OVERLAP_SENDING;	/* State 2 */
        }
        break;


      default:
	if (NLCB->Setup) {
	    LIF_FreePkt(NLCB->Setup);	/* Discard saved SETUP */
	    NLCB->Setup = NULL;
	}
#ifdef BRI
	L3_UpdateChannel(L3_PktIn);	/* Attempt B-Channel update */
#endif
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_MORE_INFO_IND, CC_ID, FALSE, FALSE);
	NLCB->State = OVERLAP_SENDING;	/* State 2 */
	break;
    }
}

/*****************
*  U1_T303Expiry *
******************
*
* State = 1;  Event = T303 Expiry (no response from network peer)
*
*/
PRIVATE void near U1_T303Expiry()
{
    /* VN2 does not resend, as per 5.2.1.3(4) === */
    if (NLCB->T303_First && SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
	/* ===    try again */
    {
	NLCB->T303_First = FALSE;
	L3_PktIn = NLCB->Setup;
	NLCB->Setup = NULL;
	L3_AvailPktIn = TRUE;	/* make it available */
	L3_Mail(SETUP, L2_ID, FALSE, FALSE);	/* retransmit SETUP */
	LIF_StartTimer(NLCB->Timer_T303);
    } else {			/* call failed */
	/*
	 * Send RELEASE_COMP to the Network conditionally on SwitchType. Then
	 * get a new pkt if required to sent the message.  We have to go
	 * around the usual L3_Mail machinery because we have to build a
	 * channel identification info element in this one case.
	 */
	switch (SwitchType) {
#ifdef PRI_4ESS
          /*  DO we need this for the 5ESS ? */
	  case PRI_4ESS_STYPE:
	  case PRI_NET5_STYPE:
	  case PRI_DMS100_STYPE:
	    NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;
#endif
#ifdef VN2
	  case VN2_STYPE:
	  case VN3_STYPE:
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;		/* No message is sent to the Network */
#endif

#ifdef KDD_NTT
	  case KDD_STYPE:
	  case NTT_STYPE:
	  case PRI_NTT_STYPE:
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;		/* No message is sent to the Network */
#endif
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	    LIF_StartTimer(NLCB->Timer_T305);
	    NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	    LIF_StartTimer(NLCB->Timer_T305);
	    NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;
#endif
#ifdef BRI_NET3			/* === */
	  case BRI_NET3_STYPE:
	    NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;
#endif
#ifdef BRI_TS013
	  case BRI_TS013_STYPE:
	  case PRI_TS014_STYPE:
	    /* no message sent to the network */
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;
#endif
#ifdef BRI_NI1
	  case BRI_5ESS_STYPE:
	  case BRI_NI1_STYPE:	/* === */
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;		/* No message is sent to the Network */
#endif
	  default:
	    NLCB->Cause = UNSPECIFIED_CAUSE;	/* Cause 31 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    L3_PktIn = LIF_GetPkt(SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
	    if (L3_PktIn == NULL)
		LIF_Fatal("l3_sdl_u", "U1_T303Expiry", "LIF_GetPkt failed");
#endif
	    break;
	}

	L3_AvailPktIn = TRUE;	/* Make it available */
	L3_PutInfo(CC_SETUP_CONF, L3_PktIn, TRUE, FALSE);

	/*
	 * To convey the second timeout and the call failure to Call Control,
	 * the Q.931 SDL specifies CC_SETUP_CONF (which is the translation of
	 * the peer CONNECT msg), but with an error indication.  This error
	 * will be conveyed to Call Control through a channel identification
	 * information element, but with channel selection specified as "NO
	 * CHANNEL (=0)." This solution does not coincide with SDL's for all
	 * SwitchTypes implemented but is selected for uniformity.
	 */
	L3_PktIn->InfoLen +=
	    CCIE_BuildChanId_IE(&L3_PktIn->Info[L3_PktIn->InfoLen], 0, 0,
				B_CHANNEL_UNITS, FALSE, TRUE, 0, NULL);

	/*
	 * Send the result to Call Control.
	 */
	LIF_SendPkt(L2P_LAPD, L3_ID,
		    L3_EXT_STATE(NLCB->State), CC_ID,
		    NL_DATA_IND, L3_DSL_Id, Q931_SAPI, NLCB->Ces, 0,
		    NLCB->CallId, Q931_DSL_CHAN_ID, TRUE, L3_PktIn, NOW,
		    REGULAR_MAIL);
	L3_AvailPktIn = FALSE;	/* It has been mailed */

	switch (SwitchType) {
#ifdef PRI_DMS100
	  case PRI_DMS100_STYPE:
	    break;		/* The NLCB is not be released, retaining
				 * State 19 */
#endif
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    break;		/* The NLCB is not be released, retaining
				 * State 11 */
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    break;		/* The NLCB is not be released, retaining
				 * State 11 */
#endif
	  default:
	    L3_CallRelease();	/* State 0 */
	    break;
	}
    }
}

/***************
* U1_UserInfo *
****************
*
* State = 1  Event = Peer User Info
* added for VN3 conformance during homologation testing,
* so no STATUS message is sent out
*/
PRIVATE void near U1_UserInfo()
{
    switch (SwitchType) {
	case VN3_STYPE:
	break;
      default:
	break;
    }
}

/* *******  U 2 :   O V E R L A P   S E N D I N G   S T A T E  ******* */

/***************
*  U2_Alerting *
****************
*
* State = 2;  Event = Peer Alerting Command
*
*/
PRIVATE void near U2_Alerting()
{
    switch (SwitchType) {
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(CC_ALERTING_IND, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_DELIVERED;	/* State 4 */
	break;
    }
}

/***************
*  U2_CallProc *
****************
*
* State = 2;  Event = Peer Call Proceeding
*
*/
PRIVATE void near U2_CallProc()
{
#ifdef VN2
    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
	LIF_StartTimer(NLCB->Timer_T310);
#endif
    L3_Mail(CC_CALL_PROC_IND, CC_ID, FALSE, FALSE);
    NLCB->State = OUT_CALL_PROC;/* State 3 */
}

/***************
*  U2_Connect  *
****************
*
* State = 2;  Event = Peer Connect Command
*
*/
PRIVATE void near U2_Connect()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(CC_SETUP_CONF, CC_ID, FALSE, FALSE);
	L3_Mail(CONNECT_ACK, L2_ID, TRUE, FALSE);
	break;
    }

    NLCB->State = CALL_ACTIVE;	/* State 10 */
}

/***************
*  U2_DLEstInd *
****************
*
* State = 2;  Event = Unexpected DL ESTABLISH INDICATION
*
* As currently written, this is only relevent for NET3.
* Call Control is expected to initiate clearing per NET3 SDLs.
*/
PRIVATE void near U2_DLEstInd()
{
    Ux_BadMsg();		/* as written can't get here */
}

/***********
*  U2_Info *
************
*
* State = 2;  Event = Peer Information
*             added for Australia ===
*/
PRIVATE void near U2_Info()
{
    switch (SwitchType) {
	case BRI_TS013_STYPE:
	case PRI_TS014_STYPE:
	U1_BadMsg();
	break;
      default:
	Ux_Info();
	break;
    }
}

/**************
*  U2_InfoReq *
***************
*
* State = 2;  Event = Call Control Information Request
*
*/
PRIVATE void near U2_InfoReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(INFO_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(INFO_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	L3_Mail(INFO, L2_ID, FALSE, FALSE);
	break;
    }
}

/*****************
*  U2_ReleaseReq *
******************
*
* (aka - Clear Request)
*
* State = 2;  Event = Call Control Release Request
*
*/
PRIVATE void near U2_ReleaseReq()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
      default:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
    }
}

/* ****  U 3 :   O U T G O I N G   C A L L   P R O C E E D I N G  **** */

/***************
*  U3_Alerting *
****************
*
* State = 3;  Event = Peer Alerting Command
*
*/
PRIVATE void near U3_Alerting()
{
    BYTE *signal_ie;

#if defined (VN2) || defined (BRI_NET3)
    if ((SwitchType == VN2_STYPE) ||
	(SwitchType == VN3_STYPE) ||
	(SwitchType == PRI_4ESS_STYPE) ||
	(SwitchType == PRI_5ESS_STYPE) ||
	(SwitchType == PRI_DMS100_STYPE) ||
	(SwitchType == BRI_NET3_STYPE) ||
	(SwitchType == PRI_NET5_STYPE) ||
	(SwitchType == PRI_TS014_STYPE))
	LIF_StopTimer(NLCB->Timer_T310);
#endif

    signal_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen,
                            SIGNAL_IE, CODESET_0, 1);
    if ((SwitchType == BRI_DMS100_STYPE) && (!signal_ie)) {
        NLCB->Cause = MANDATORY_IE_MISSING;
        Diagnostic[0] = SIGNAL_IE;
        L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
        LIF_StartTimer(NLCB->Timer_T305);
        NLCB->State = DISCONNECT_REQUEST;       /* State 11 */
    } else {
        L3_Mail(CC_ALERTING_IND, CC_ID, FALSE, FALSE);
        NLCB->State = CALL_DELIVERED;	/* State 4 */
    }
}

/***************
*  U3_CallProc *
****************
*
* State = 3;  Event = Peer Call Proceeding Command
*
*/
PRIVATE void near U3_CallProc()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
      case BRI_NET3_STYPE:
      case PRI_NET5_STYPE:
	Ux_BadMsg();
	break;

      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
      default:
	break;
    }
}

/***************
*  U3_Connect  *
****************
*
* State = 3;  Event = Peer Connect Command
*
*/
PRIVATE void near U3_Connect()
{
    L3_Mail(CC_SETUP_CONF, CC_ID, FALSE, FALSE);
#if defined (VN2) || defined (BRI_NET3)
    if ((SwitchType == VN2_STYPE) ||
	(SwitchType == VN3_STYPE) ||
	(SwitchType == PRI_4ESS_STYPE) ||
	(SwitchType == PRI_5ESS_STYPE) ||
	(SwitchType == PRI_DMS100_STYPE) ||
	(SwitchType == BRI_NET3_STYPE) ||
	(SwitchType == PRI_NET5_STYPE) ||
	(SwitchType == PRI_TS014_STYPE))
	LIF_StopTimer(NLCB->Timer_T310);
#endif
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	NLCB->State = CONNECT_REQUEST;	/* State 8 */
	break;
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	/*
	 * Although not stated in the specification, it is assumed here that
	 * calls may terminate to automatic equipment which does not sent
	 * alerting before connecting, so this is permitted.
	 */
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
      case BRI_1TR6_STYPE:
	break;			/* handled in CC_CONNECT_ACK_REQ === */

      default:
	L3_Mail(CONNECT_ACK, L2_ID, TRUE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
    }
}

/************
*  U3_Info  *
*************
*
* State = 3;  Event = Peer Info Indication
*
*/
PRIVATE void near U3_Info()
{
    switch (SwitchType) {
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
	Ux_BadMsg();
	break;
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* send it to CC */
	break;
#endif
      case BRI_TS013_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* send it to CC */
	break;

      case PRI_TS014_STYPE:
	U1_BadMsg();
	break;

      default:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* send it to CC */
	break;
    }
}

/****************
*  U3_Progress  *
*****************
*
* State = 3;  Event = Peer Progress Indication
*
*/
PRIVATE void near U3_Progress()
{
    L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
    if ((SwitchType == VN2_STYPE) ||
	(SwitchType == VN3_STYPE) ||
	(SwitchType == PRI_4ESS_STYPE) ||
	(SwitchType == PRI_5ESS_STYPE) ||
	(SwitchType == PRI_DMS100_STYPE) ||
	(SwitchType == BRI_NET3_STYPE) ||
	(SwitchType == PRI_NET5_STYPE) ||
	(SwitchType == PRI_TS014_STYPE))
	    LIF_StopTimer(NLCB->Timer_T310);
    switch (SwitchType) {
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
	NLCB->State = CALL_DELIVERED;	/* State 4 */
	break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	break;
    }
}


/*****************
*  U3_ReleaseReq *
******************
*
* State = 3;  Event = Peer Release Request
*
*/
PRIVATE void near U3_ReleaseReq()
{
#if defined (VN2) || defined (BRI_NET3)
    if ((SwitchType == VN2_STYPE) ||
	(SwitchType == VN3_STYPE) ||
	(SwitchType == BRI_NET3_STYPE))	/* === */
	LIF_StopTimer(NLCB->Timer_T310);
#endif
    L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
    LIF_StartTimer(NLCB->Timer_T308);
    NLCB->State = RELEASE_REQUEST;	/* State 19 */
}

/*****************
*  U3_T310Expiry *
******************
*
* State = 3;  Event = Timer T310 Expiry
*
*/
PRIVATE void near U3_T310Expiry()
{
    switch (SwitchType) {
#ifdef BRI_NET3			/* === */
	case BRI_NET3_STYPE:
#endif
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	/*
	 * Call Control is informed now of the event.
	 */
	L3_StopAllTimers();
	NLCB->Cause = NORMAL_CLEARING;	/* Cause 16 */
	L3_Mail(CC_DISCONNECT_IND, CC_ID, TRUE, TRUE);
	NLCB->Cause = NORMAL_CLEARING;	/* Cause 16 */
	L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_DMS100_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_StopAllTimers();
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;

      default:			/* Not expected for most switchtypes. */
	Ux_BadMsg();
	break;
    }
}

/* *********  U 4 :  C A L L   D E L I V E R E D   S T A T E  ******** */

/***************
*  U4_Alerting *
****************
*
* State = 4;  Event = Peer Alerting Command
*
*/
PRIVATE void near U4_Alerting()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	case BRI_NI1_STYPE:	/* === */
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	L3_Mail(CC_ALERTING_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
	Ux_BadMsg();
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
	L3_Mail(CC_ALERTING_IND, CC_ID, FALSE, FALSE);
	break;
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
      case PRI_NET5_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_ALERTING_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_ALERTING_IND, CC_ID, FALSE, FALSE);
	break;
#endif
      default:			/* Bad message */
	L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);
	break;
    }
}

/***************
*  U4_Connect  *
****************
*
* State = 4;  Event = Peer Connect Command
*
*/
PRIVATE void near U4_Connect()
{
    L3_Mail(CC_SETUP_CONF, CC_ID, FALSE, FALSE);
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_NTT_STYPE:
      case PRI_TS014_STYPE:
	NLCB->State = CONNECT_REQUEST;	/* State 8 */
	break;
#ifdef VN2
      case VN2_STYPE:		/* No CONNECT_ACK is sent. */
      case VN3_STYPE:		/* No CONNECT_ACK is sent. */
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	/* the ConnectAck is sent out from Ux_ConnectAckReq */
	break;
#endif
      default:
	L3_Mail(CONNECT_ACK, L2_ID, TRUE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
    }
}

/***************
* U4_Facility *
****************
*
* State = 4;  Event = Peer Facility
*
*/
PRIVATE void near U4_Facility()
{
    switch (SwitchType) {
#ifdef BRI_NET3
      case BRI_NET3_STYPE:
#endif
      case PRI_NET5_STYPE:
        /* Currently we don't support the FACILITY 
         * (ETSI Supplementary Services) message type,
         * so the correct action is just to ignore it.
         */
         break;
      default:
	Ux_BadMsg();
	break;
    }
}

#ifdef BRI_1TR6
/*****************
* U4_FacilityAck *
******************
*
* State = 4; Event = Peer Facility Acknowledge
*
*/
PRIVATE void near U4_FacilityAck()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
* U4_FacilityRej *
******************
*
* State = 4; Event = Peer Facility Reject
*
*/
PRIVATE void near U4_FacilityRej()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
* U4_FacilityReq *
******************
*
* State = 4; Event = Call Control Facility Request
*
*/
PRIVATE void near U4_FacilityReq()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	L3_Mail(FACILITY_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}
#endif				/* BRI 1TR6 */
#ifdef PRI_1TR6
/*****************
* U4_FacilityAck *
******************
*
* State = 4; Event = Peer Facility Acknowledge
*
*/
PRIVATE void near U4_FacilityAck()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
* U4_FacilityRej *
******************
*
* State = 4; Event = Peer Facility Reject
*
*/
PRIVATE void near U4_FacilityRej()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
* U4_FacilityReq *
******************
*
* State = 4; Event = Call Control Facility Request
*
*/
PRIVATE void near U4_FacilityReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(FACILITY_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}
#endif				/* PRI 1TR6 */
/****************
*  U4_Progress  *
*****************
*
* State = 4;  Event = Peer Progress Indication
*
*/
PRIVATE void near U4_Progress()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
	L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
	break;
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
	L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	/*
	 * Bad message
	 */
	L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);
	break;
    }
}

/*****************
*  U4_ReleaseReq *
******************
*
* State = 4;  Event = Peer Release Request
* added for Australia
*/
PRIVATE void near U4_ReleaseReq()
{
    switch (SwitchType) {
     case BRI_TS013_STYPE:
     case PRI_TS014_STYPE:
	LIF_StopTimer(NLCB->Timer_T310);
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;

      default:
	Ux_ReleaseReq();
	break;
    }
}

/* ***********  U 6 :  C A L L   P R E S E N T   S T A T E  ********** */

/*
 * The detection of the call reference flag bit error is deferred until the
 * response to the call SETUP is received here in State 6.
 */

/******************
*  U6_AlertingReq *
*******************
*
* State = 6;  Event = Call Control Alerting Request
*
*/
PRIVATE void near U6_AlertingReq()
{
    if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
#ifdef BRI
	if (NLCB->ChanId == 0)
	    L3_UpdateChannel(L3_PktIn);
#endif
	L3_Mail(ALERTING, L2_ID, FALSE, FALSE);
	NLCB->State = CALL_RECEIVED;	/* State 7 */
    } else {
	U6_BadCrFlag();
    }
}

/***************
* U6_BadCrFlag *
****************
*
* State 6; Event = CC_ALERTING_REQ, CC_CALL_PROC_REQ, CC_MORE_INFO_REQ,
*                  CC_RELEASE_REQ, CC_SETUP_REJ_REQ, CC_SETUP_RESP
*
* Call Reference flag bit inversion error detected.
*/
PRIVATE void near U6_BadCrFlag()
{
    NLCB->Cause = UNSPECIFIED_CAUSE;	/* cause 31 */
    L3_Mail(CC_RELEASE_CONF, CC_ID, TRUE, TRUE);	/* Release CC */
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
	/*
	 * ignore SETUP.
	 */
	L3_CallRelease();	/* State 0 */
	break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	/*
	 * Per NIS S208-5 Issue 1 Sec. 6.1.5.3.2 (c) - ignore SETUP.
	 */
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	/*
	 * Per NIS A211-1, Ver. 2,  Sec. B, 5.1.5.3 - ignore SETUP.
	 */
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	L3_CallRelease();	/* State 0 */
	break;			/* No response is sent to the NETWORK */
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	L3_CallRelease();	/* State 0 */
	break;			/* No response is sent to the NETWORK */
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	L3_CallRelease();	/* State 0 */
	break;			/* No response is sent to the NETWORK */
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	L3_CallRelease();	/* State 0 */
	break;			/* No response is sent to the NETWORK */
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_CallRelease();	/* State 0 */
	break;			/* No response is sent to the NETWORK */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_CallRelease();	/* State 0 */
	break;
#endif
      default:
	NLCB->Cause = INVALID_CALL_REF;	/* cause 81 */
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
    }
}

/******************
*  U6_CallProcReq *
*******************
*
* State = 6;  Event = Call Control Call Proceeding Request
*
*/
PRIVATE void near U6_CallProcReq()
{
    if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	switch (SwitchType) {
#ifdef BRI_1TR6
	    case BRI_1TR6_STYPE:
	    Ux_Error();
	    break;
#endif
	  default:
#ifdef BRI
	    if (NLCB->ChanId == 0)
		L3_UpdateChannel(L3_PktIn);
#endif
	    L3_Mail(CALL_PROC, L2_ID, FALSE, FALSE);
	    NLCB->State = IN_CALL_PROC;	/* State 9 */
	    break;
	}
    } else {
	U6_BadCrFlag();
    }
}

/****************
* U6_Disconnect *
*****************
*
* State = 6; Event = Peer Disconnect.
*
*/
PRIVATE void near U6_Disconnect()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
      default:
	Ux_Disconnect();
	break;
    }
}

/*************
* U6_DiscReq *
**************
*
* State = 6; Event = Call Control Disconnect Request.
*
*/
PRIVATE void near U6_DiscReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	    Ux_DiscReq();
	} else {
	    U6_BadCrFlag();
	}
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	    Ux_DiscReq();
	} else {
	    U6_BadCrFlag();
	}
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/***********
*  U6_Info *
************
*
* State = 6;  Event = Peer Information
*
*/
PRIVATE void near U6_Info()
{
    switch (SwitchType) {
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#ifdef KDD_NTT
      case KDD_STYPE:
	Ux_BadMsg();
	break;
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* Send up to CC */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
      default:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* Send up to CC */
	break;
    }
}

/******************
*  U6_MoreInfoReq *
*******************
*
* State = 6;  Event = Call Control More Info Request
*
*/
PRIVATE void near U6_MoreInfoReq()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
	Ux_BadMsg();
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
#ifdef BRI
	    if (NLCB->ChanId == 0)
		L3_UpdateChannel(L3_PktIn);
#endif
	    L3_Mail(SETUP_ACK, L2_ID, FALSE, FALSE);
	    NLCB->State = OVERLAP_RECEIVE;	/* State 25 */
	} else {
	    U6_BadCrFlag();
	}
	break;
    }
}

/**************
*  U6_Release *
***************
*
* State = 6;  Event = Peer Release Command
*
*/
PRIVATE void near U6_Release()
{
    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)	/* === */
	L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, FALSE, FALSE);	/* === */
    else			/* === */
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
    switch (SwitchType) {
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	if ((NLCB->Cause = L3_RejectCause((BYTE *) & Trashcan)) != 0)	/* === */
	    Diagnostic[0] = CAUSE_IE;
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	/*
	 * wait here for Call Control to validate the Cause and Display
	 * information. It will reply with a CC_REL_COMP_REQ message, then
	 * the Release Complete can be sent. ===
	 */
	break;

      default:
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
    }
}

/*****************
*  U6_ReleaseReq *
******************
*
* State = 6;  Event = Call Control Release Request
*
*/
PRIVATE void near U6_ReleaseReq()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	/*
	 * Release with call redrection or UUI.
	 */
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	    L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	    LIF_StartTimer(NLCB->Timer_T308);
	    NLCB->State = RELEASE_REQUEST;	/* State 19 */
	} else {
	    U6_BadCrFlag();
	}
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	    Ux_ReleaseReq();
	} else {
	    U6_BadCrFlag();
	}
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	    Ux_ReleaseReq();
	} else {
	    U6_BadCrFlag();
	}
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/******************
*  U6_SetupRejReq *
*******************
*
* State = 6;  Event = Call Control Setup Reject Request
*
*/
PRIVATE void near U6_SetupRejReq()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	    L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
	    LIF_StartTimer(NLCB->Timer_T305);
	    NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	} else {
	    U6_BadCrFlag();
	}
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	    L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
	    LIF_StartTimer(NLCB->Timer_T305);
	    NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	} else {
	    U6_BadCrFlag();
	}
	break;
#endif
      default:
	if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
	    L3_Mail(RELEASE_COMP, L2_ID, FALSE, FALSE);
	    L3_CallRelease();	/* State 0 */
	} else {
	    U6_BadCrFlag();
	}
	break;
    }
}

/****************
*  U6_SetupResp *
*****************
*
* State = 6;  Event = Call Control Setup Response
*
* In this case, a subsequent T313 Expiry may require that the CONNECT
* be resent.  As it may contain Call Control information elements, it must
* be queued in these cases.
*/
PRIVATE void near U6_SetupResp()
{
    if (L3_ValidCallRef(NLCB->CallRefLen, NLCB->CallRef)) {
#ifdef BRI
	if (NLCB->ChanId == 0)
	    L3_UpdateChannel(L3_PktIn);
#endif
	L3_Mail(CONNECT, L2_ID, FALSE, FALSE);
	switch (SwitchType) {
	  case PRI_4ESS_STYPE:
	  case PRI_5ESS_STYPE:
	  case PRI_NET5_STYPE:
	  case PRI_TS014_STYPE:
	    L3_SavePkt(&NLCB->Connect, L3_PktOut);	/* save CONNECT */
	    break;

	  default:
	    break;
	}
	LIF_StartTimer(NLCB->Timer_T313);
	NLCB->State = CONNECT_REQUEST;	/* State 8 */
    } else {			/* Tear down the call because of bad call
				 * reference */
	U6_BadCrFlag();
    }
}

/* *********  U 7 :  C A L L   R E C E I V E D   S T A T E  ********** */

/******************
*  U7_AlertingReq *
*******************
*
* State = 7;  Event = Call Control Alerting Request
*
*             NOTE: This is a valid command for 4/5ESS switch only!
*/
PRIVATE void near U7_AlertingReq()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(ALERTING, L2_ID, FALSE, FALSE);
	break;
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	L3_Mail(ALERTING, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
* U7_FacilityAck *
******************
*
* State = 7, 8; Event = Peer Facility Acknowledge
*
*/
PRIVATE void near U7_FacilityAck()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
* U7_FacilityRej *
******************
*
* State = 7, 8; Event = Peer Facility Reject
*
*/
PRIVATE void near U7_FacilityRej()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
* U7_FacilityReq *
******************
*
* State = 7, 8; Event = Call Control Facility Request
*
*/
PRIVATE void near U7_FacilityReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	/*
	 * It is assumed that Call Control has verified that the facility has
	 * been previously registered.
	 */
	L3_Mail(FACILITY_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	/*
	 * It is assumed that Call Control has verified that the facility has
	 * been previously registered.
	 */
	L3_Mail(FACILITY_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}
#ifdef BRI_1TR6

/**********
* U7_Info *
***********
*
* State = 7, 8;  Event = Peer Info Command
*
*/
PRIVATE void near U7_Info()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}
#endif

/******************
*  U7_ProgressReq *
*******************
*
* State = 7;  Event = Call Control Progress Request
*
*             NOTE: This is a valid command for 4ESS switch only !
*  	note will be verified after homologation.
*/
PRIVATE void near U7_ProgressReq()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_DMS100_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(PROGRESS, L2_ID, FALSE, FALSE);
	break;
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/****************
* U7_ReleaseReq *
*****************
*
* State = 7;  Event = Call Control Release Request
*
*/
PRIVATE void near U7_ReleaseReq()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef VN2
      case VN2_STYPE:		/* May be a UUI exchange */
      case VN3_STYPE:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      default:
	Ux_BadMsg();		/* Bad message */
	break;
    }
}

/****************
*  U7_SetupResp *
*****************
*
* State = 7;  Event = Call Control Setup Response
*
*/
PRIVATE void near U7_SetupResp()
{
    L3_Mail(CONNECT, L2_ID, FALSE, FALSE);
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_SavePkt(&NLCB->Connect, L3_PktOut);	/* save CONNECT */
	break;

      default:
	break;
    }
    LIF_StartTimer(NLCB->Timer_T313);
    NLCB->State = CONNECT_REQUEST;	/* State 8 */
}

/* *******  U 8 :  C O N N E C T   R E Q U E S T   S T A T E  ******** */

/***********
*  U8_Info *
************
*/
PRIVATE void near
    U8_Info()
{
    switch (SwitchType) {
#ifdef BRI_NET3			/* === */
	case BRI_NET3_STYPE:
#endif
	case PRI_NET5_STYPE:
	case VN2_STYPE:
	case VN3_STYPE:
	case BRI_NI1_STYPE:	/* === don't send STATUS */
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
*  U8_ConnectAck *
******************
*
* State = 8;  Event = Peer Connect Acknowledge
*
*/
PRIVATE void near U8_ConnectAck()
{
    LIF_StopTimer(NLCB->Timer_T313);
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	if (NLCB->Connect) {
	    LIF_FreePkt(NLCB->Connect);
	    NLCB->Connect = NULL;
	}
	break;

      default:
	break;
    }
    L3_Mail(CC_SETUP_COMP_IND, CC_ID, FALSE, FALSE);
    NLCB->State = CALL_ACTIVE;	/* State 10 */
}

/********************
*  U8_ConnectAckReq *
*********************
*
* State = 8;  Event = CC Connect Acknowledge Request
*
*/
PRIVATE void near U8_ConnectAckReq()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(CONNECT_ACK, L2_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;

      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
*  U8_ReleaseReq *
******************
*
* State = 8;  Event = Call Control Release Req
*
*/
PRIVATE void near U8_ReleaseReq()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_ReleaseReq();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_ReleaseReq();
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
*  U8_T313Expiry *
******************
*
* State = 8;  Event = T313 Timer Expiry
*
*/
PRIVATE void near U8_T313Expiry()
{
    if (detail_debug)
        buginf ("\n in U8_T313Expiry doing our thing.");
    switch (SwitchType) {
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:
      case BRI_5ESS_STYPE:
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	L3_Mail(CC_SETUP_COMP_IND, CC_ID, TRUE, FALSE);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;

#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
	if (NLCB->T313_First) {
	    NLCB->T313_First = FALSE;
	    L3_PktIn = NLCB->Connect;
	    NLCB->Connect = NULL;
	    L3_AvailPktIn = FALSE;
	    L3_Mail(CONNECT, L2_ID, FALSE, FALSE);
	    LIF_StartTimer(NLCB->Timer_T313);
	    NLCB->State = CONNECT_REQUEST;    /* State 8 */
	} else {
	    NLCB->T313_First = TRUE;
            if (SwitchType == PRI_5ESS_STYPE)
               NLCB->Cause = UNSPECIFIED_CAUSE;    /* Cause 31 */
            else
               NLCB->Cause = TIMER_EXPIRY;    /* Cause 102 */
	    L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	    LIF_StartTimer(NLCB->Timer_T305);
	    NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	}
	break;
#ifdef VN2
	/* case VN2_STYPE: */
      case VN3_STYPE:
	/*
	 * Note that there is disagreement within VN3 specs about this cause
	 * value.  For PRI DP Equipment, Cause 102 may be required, although
	 * this value is not even defined for voice applications.
	 */
	NLCB->Cause = NORMAL_CLEARING;	/* Cause 16 */
	/* #if 0	 === */
	L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	L3_Mail(CC_SETUP_COMP_IND, CC_ID, TRUE, FALSE);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;			/* === */
	/* #endif	 === */
      case VN2_STYPE:
	/*
	 * This can either be a sending of RELEASE or DISCONNECT (and setting
	 * the NLCB->Cause to normal clearing).  Tests ACT/U6/AU6-CN1 and
	 * -CN2 expect RELEASE, -CN3 and -CN4 expect DISCONNECT.  We'll stick
	 * with RELEASE for now, as that is what the VN2 spec calls for (I
	 * think) in section 5.2.2.3(1) in the Note at the end of the
	 * paragraph (p98 of the Aout 1990 edition).
	 */
	NLCB->Cause = NORMAL_CLEARING;	/* Cause 16 */
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);	/* === */
	NLCB->Cause = NORMAL_CLEARING;	/* === */
	Diagnostic[0] = 0;	/* === */
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);	/* === */
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE === */
	LIF_StartTimer(NLCB->Timer_T308);	/* === */
	NLCB->State = RELEASE_REQUEST;	/* State 19 === */
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	NLCB->T313_First = TRUE;
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	Diagnostic[0] = '3';
	Diagnostic[1] = '1';
	Diagnostic[2] = '3';
	L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	L3_Mail(CC_SETUP_COMP_IND, CC_ID, TRUE, FALSE);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_TIMEOUT_IND, CC_ID, TRUE, FALSE);
	NLCB->Cause = PROTOCOL_ERROR;	/* Cause 31 */
	L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:	/* === looks like VN2 changes */
	L3_Mail(CC_TIMEOUT_IND, CC_ID, TRUE, FALSE);
	/* NLCB->Cause = PROTOCOL_ERROR;   Cause 31 === for 1tr6 certif */
	NLCB->Cause = 0;	/* === */
	L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
#ifdef	BRI_NET3		/* === */
      case BRI_NET3_STYPE:
      case PRI_NET5_STYPE:
	NLCB->T313_First = TRUE;
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	L3_Mail(CC_SETUP_COMP_IND, CC_ID, TRUE, FALSE);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
#ifdef	BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	NLCB->T313_First = TRUE;
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	NLCB->PreviousCause = TIMER_EXPIRY;
	L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	L3_Mail(CC_SETUP_COMP_IND, CC_ID, TRUE, FALSE);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
      default:
	NLCB->T313_First = TRUE;
	NLCB->Cause = UNSPECIFIED_CAUSE;	/* Cause 31 */
	L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	L3_Mail(CC_SETUP_COMP_IND, CC_ID, TRUE, FALSE);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
    }
}

/* *****  U 9 :  I N C O M I N G   C A L L   P R O C E E D I N G  **** */

/******************
*  U9_AlertingReq *
*******************
*
* State = 9;  Event = Call Control Alerting Request
*
*/
PRIVATE void near U9_AlertingReq()
{
    L3_Mail(ALERTING, L2_ID, FALSE, FALSE);
    NLCB->State = CALL_RECEIVED;/* State 7 */
}

/****************
* U9_Disconnect *
*****************
*
* State = 9, 20, 25;  Event = Peer Disconnect
*
*/
PRIVATE void near U9_Disconnect()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      default:
	Ux_Disconnect();
	break;
    }
}

/*****************
* U9_FacilityAck *
******************
*
* State = 9;  Event = Peer Facility Acknowledge
*
*/
PRIVATE void near U9_FacilityAck()
{
    switch (SwitchType) {
#ifdef PRI_5ESS
	case PRI_5ESS_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	Ux_FacilityAck();
	break;			/* remain in state 9 */
    }
}

/*****************
* U9_FacilityRej *
******************
*
* State = 9, 10;  Event = Perr Facility Reject
*
*/
PRIVATE void near U9_FacilityRej()
{
    switch (SwitchType) {
#ifdef PRI_5ESS
	case PRI_5ESS_STYPE:
	Ux_BadMsg();
	break;			/* remain in state 9 */
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	Ux_FacilityRej();
	break;			/* remain in state 9 */
    }
}

/******************
*  U9_ProgressReq *
*******************
*
* State = 9;  Event = Call Control Progress Request
*
*/
PRIVATE void near U9_ProgressReq()
{
    switch (SwitchType) {
#ifdef PRI_5ESS
	case PRI_5ESS_STYPE:
	L3_Mail(PROGRESS, L2_ID, FALSE, FALSE);
	NLCB->State = CALL_RECEIVED;	/* State 7 */
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	L3_Mail(PROGRESS, L2_ID, FALSE, FALSE);
	break;			/* remain in state 9 */
    }
}

/****************
*  U9_SetupResp *
*****************
*
* State = 9;  Event = Call Control Setup Response
*
*/
PRIVATE void near U9_SetupResp()
{
    L3_Mail(CONNECT, L2_ID, FALSE, FALSE);
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_SavePkt(&NLCB->Connect, L3_PktOut);	/* save CONNECT */
	break;

      default:
	break;
    }
    if (detail_debug)
        buginf ("\n In U9_SetupResp starting T313");
    LIF_StartTimer(NLCB->Timer_T313);
    NLCB->State = CONNECT_REQUEST;	/* State 8 */
}

/* *******  U 9 :  C O N N E C T   R E Q U E S T   S T A T E  ******** */

/* === added entire routine */
PRIVATE void near
    U9_Info()
{
    switch (SwitchType) {
        case PRI_NET5_STYPE:
        L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
        break;
      default:
        Ux_BadMsg();
        break;
    }
}

/* ***************  U 10 :  A C T I V E   S T A T E  ***************** */

/**************
* U10_CongCon *
***************
*
* State = 10;  Event = Peer Congestion Control Message
*
*/
PRIVATE void near U10_CongCon()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(CC_CONG_CON_IND, CC_ID, FALSE, FALSE);
	break;
    }
}

/******************
*  U10_ConnectAck *
*******************
*
* State = 10;  Event = Peer ConnectAck Message
*
*/
PRIVATE void near U10_ConnectAck()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	break;			/* remain in state 10 */
    }
}

#ifdef BRI_1TR6
/****************
* U10_DetachReq *
*****************
*
* State = 10;  Event = Call Control Detach Request
*
*/
PRIVATE void near U10_DetachReq()
{
    L3_Mail(DETACH, L2_ID, FALSE, FALSE);
    NLCB->State = CALL_PASSIVE;	/* State 14 */
}

#endif				/* BRI_1TR6 */
/****************
* U10_FacAckReq *
*****************
*
* State = 10;  Event = Call Control Facility Acknowledge Request
*
*/
PRIVATE void near U10_FacAckReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(FACILITY_ACK_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(FACILITY_ACK_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_FacAckReq();
	break;
    }
}

/****************
* U10_FacRejReq *
*****************
*
* State = 10;  Event = Call Control Facility Reject Request
*
*/
PRIVATE void near U10_FacRejReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(FACILITY_REJ_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(FACILITY_REJ_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_FacRejReq();
	break;
    }
}

/***************
* U10_Facility *
****************
*
* State = 10;  Event = Peer Facility
*
*/
PRIVATE void near U10_Facility()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	/*
	 * just ignore the FACILITY message for now, not too sure what to do
	 * with it ===		L3_Mail( CC_FACILITY_IND, CC_ID, FALSE, FALSE
	 * );
	 */
	break;
#endif
#ifdef BRI_NET3
      case BRI_NET3_STYPE:
#endif
      case PRI_NET5_STYPE:
        /* we don't support the FACILITY message 
         * (ETSI Supplementary Services) type yet.
         * so just ignore this message for now.
         */
        break;
      default:
	Ux_BadMsg();
	break;
    }
}

/******************
* U10_FacilityAck *
*******************
*
* State = 10;  Event = Peer Facility Acknowledge
*
*/
PRIVATE void near U10_FacilityAck()
{
    switch (SwitchType) {
#ifdef PRI_5ESS
	case PRI_5ESS_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_FacilityAck();
	break;			/* remain in state 9 */
    }
}

/******************
* U10_FacilityRej *
*******************
*
* State = 10;  Event = Peer Facility Reject
*
*/
PRIVATE void near U10_FacilityRej()
{
    switch (SwitchType) {
#ifdef PRI_5ESS
	case PRI_5ESS_STYPE:
	Ux_BadMsg();
	break;			/* remain in state 9 */
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_FacilityRej();
	break;			/* Remain in state 10 */
    }
}

/******************
* U10_FacilityReq *
*******************
*
* State = 10;  Event = Call Control Facility Request
*
*/
PRIVATE void near U10_FacilityReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	/*
	 * It is assumed that Call Control has verified that the facility has
	 * been previously registered.
	 */
	L3_Mail(FACILITY_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	/*
	 * It is assumed that Call Control has verified that the facility has
	 * been previously registered.
	 */
	L3_Mail(FACILITY_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_FacilityReq();
	break;
    }
}

/*************
*  U10_Info  *
**************
*
* State = 10;  Event = Peer Info Indication
*
*/
PRIVATE void near U10_Info()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
    }
}

/**************
*  U10_Notify *
***************
*
* State = 10;  Event = Network Peer Notify
*
*/
PRIVATE void near U10_Notify()
{
    switch (SwitchType) {
	case BRI_DMS100_STYPE:
	case PRI_NET5_STYPE:
	break;			/* Ignore */
      case KDD_STYPE:
	Ux_BadMsg();
	break;
      case NTT_STYPE:
	L3_Mail(CC_NOTIFY_IND, L2_ID, FALSE, FALSE);
	break;
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	L3_Mail(CC_NOTIFY_IND, CC_ID, FALSE, FALSE);
	break;
    }
}

/*****************
*  U10_NotifyReq *
******************
*
* State = 10;  Event = Call Control Notify
*
*/
PRIVATE void near U10_NotifyReq()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
	Ux_BadMsg();
	break;
      case NTT_STYPE:
	L3_Mail(NOTIFY, L2_ID, FALSE, FALSE);
	break;
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif				/* KDD_NTT */
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	L3_Mail(NOTIFY, L2_ID, FALSE, FALSE);
	break;
    }
}

/****************
*  U10_Progress *
*****************
*
* State = 10;  Event = Peer Progress Message
*
*/
PRIVATE void near U10_Progress()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	Ux_Progress();
	break;			/* Remain in state 9 */
    }
}

/******************
*  U10_ReleaseReq *
*******************
*
* State = 9, 10;  Event = Call Control Release Request
*
*/
PRIVATE void near U10_ReleaseReq()
{
    switch (SwitchType) {
#ifdef BRI_DMS100

	case BRI_DMS100_STYPE:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef BRI_NET3
      case BRI_NET3_STYPE:
#endif
      case PRI_NET5_STYPE:
        /* Currently we don't support the FACILITY 
         * (ETSI Supplementary Services) message type,
         * so the correct action is just to ignore it.
         */
         break;
      default:
	Ux_BadMsg();
	break;
    }
}

/****************
* U10_SetupResp *
*****************
*
* State = 10;  Event = Call Control Setup Response
*
*/
PRIVATE void near U10_SetupResp()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	Ux_BadMsg();
	break;			/* remain in state 10 */
    }
}

/******************
*  U10_SuspendReq *
*******************
*
* State = 10;  Event = Call Control Suspend Request
*
*/
PRIVATE void near U10_SuspendReq()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	/*
	 * Note that the VN2 Specification indicates T318 as the SUSPEND
	 * request timer and T319 as the RESUME request timer. It is felt
	 * that this is an error being that this is the opposite of CCITT
	 * Rec. Q.931.  The CCITT assignment is implemented.
	 */
	L3_Mail(SUSPEND, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T319);
	NLCB->State = SUSPEND_REQUEST;	/* State 15 */
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
	Ux_BadMsg();
	break;
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
      case NTT_STYPE:
	L3_Mail(SUSPEND, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T319);
	NLCB->State = SUSPEND_REQUEST;	/* State 15 */
	break;
#endif				/* KDD_NTT */
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(SUSPEND, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T319);
	NLCB->State = SUSPEND_REQUEST;	/* State 15 */
	break;
#endif
#ifdef CCITT
      case CCITT_STYPE:
	L3_Mail(SUSPEND, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T319);
	NLCB->State = SUSPEND_REQUEST;	/* State 15 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/***************
* U10_UserInfo *
****************
*
* State = 10  Event = Peer User Info
*
*/
PRIVATE void near U10_UserInfo()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(CC_USER_INFO_IND, CC_ID, FALSE, FALSE);
	break;
    }
}

/******************
* U10_UserInfoReq *
*******************
*
* State = 10;  Event = Call Control User Info Request
*
*/
PRIVATE void near U10_UserInfoReq()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(USER_INFO, L2_ID, FALSE, FALSE);
	break;
    }
}

/* ****  U 11 :  D I S C O N N E C T   R E Q U E S T   S T A T E  **** */

/**************
*  U11_BadMsg *
***************
*
* State = 11;  Event = Unexpected message.
*
*/
PRIVATE void near U11_BadMsg()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_TS014_STYPE:
	break;			/* do nothing */
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	break;			/* do nothing */
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* do nothing */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* do nothing */
#endif
      default:
	Ux_BadMsg();
	break;
    }
}
#ifdef BRI_1TR6

/****************
* U11_DetachReq *
*****************
*
* State = 11, 12; Call Control Detach Request
*
*/
PRIVATE void near U11_DetachReq()
{
    L3_Mail(DETACH, L2_ID, FALSE, FALSE);
}
#endif
#ifdef PRI_1TR6

/****************
* U11_DetachReq *
*****************
*
* State = 11; Call Control Detach Request
*
*/
PRIVATE void near U11_DetachReq()
{
    L3_Mail(DETACH, L2_ID, FALSE, FALSE);
}
#endif

/******************
*  U11_Disconnect *
*******************
*
* State = 11;  Event = Peer Disconnect Command
*
* This is a standard clearing collision where the local Call Control has
* sent CC_DISCONNECT_REQ and the Network sends DISCONNECT due to a
* coincidental remote hangup before it processes the DISCONNECT from the
* User.
*
*/
PRIVATE void near U11_Disconnect()
{
    BYTE cause;
    BYTE *ie, *my_Get_IE_pkt(PKT_ID, BYTE);

    cause = 0;
    LIF_StopTimer(NLCB->Timer_T305);
    switch (SwitchType) {
      case BRI_5ESS_STYPE:
            /*
             * Network will release the call on a Disconnect collision.
             */
	break;			/* do nothing */
      case BRI_DMS100_STYPE:
	NLCB->Cause = 0;	/* Clear possible local cause value */
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;

#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	{
	    /*
	     * Why do we send the release directly here, instead of from call
	     * control, where CCIE_ExpandCause_IE() would do this for us?
	     */
	    if ((ie = my_Get_IE_pkt(L3_PktIn, CAUSE_IE)) == NULL) {
		NLCB->Cause = MANDATORY_IE_MISSING;
		L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
		L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, FALSE, FALSE);
		L3_CallRelease();
		break;
	    } else if ((ie[2] & 0x60) != 0) {
		NLCB->Cause = INVALID_ELEM_CONTENTS;
		/*
		 * do not send the Release Complete here, let the Disconnect
		 * message go out === L3_Mail (RELEASE_COMP, L2_ID, TRUE,
		 * FALSE); L3_Mail (CC_RELEASE_IND_QUIET, CC_ID, FALSE,
		 * FALSE); L3_CallRelease (); break;
		 */
	    }
	    L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	    LIF_StartTimer(NLCB->Timer_T308);
	    NLCB->State = RELEASE_REQUEST;	/* State 19 */
	}
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* do nothing */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	/* === we have to ignore it, but restart the timer */
	/* L3_Mail (RELEASE, L2_ID, TRUE, FALSE);	   === */
	/* L3_SavePkt (&NLCB->Release, L3_PktOut); === *//* save RELEASE */
	/* LIF_StartTimer (NLCB->Timer_T308);      === */
	/* NLCB->State = RELEASE_REQUEST;          === *//* State 19 */
	LIF_StartTimer(NLCB->Timer_T305);	/* === */
	break;
#endif
      case PRI_NET5_STYPE:
      case BRI_NET3_STYPE:
        /*
         * Added for NET5 test TC11115, we receive a disc with the
         * mandatory cause field missing. We need to send a Release
         * and go to state 19.
         * Added net3 for italy homologation, TC11115, TC11116
         */
        {
            if ((ie = my_Get_IE_pkt(L3_PktIn, CAUSE_IE)) == NULL)
                NLCB->Cause = MANDATORY_IE_MISSING;
            else if ((ie[1] == 2) && !(ie[3] & 0x80))
                NLCB->Cause = INVALID_ELEM_CONTENTS;
 
            L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
            L3_SavePkt(&NLCB->Release, L3_PktOut);  /* save RELEASE */
            LIF_StartTimer(NLCB->Timer_T308);
            NLCB->State = RELEASE_REQUEST;  /* State 19 */
            break;
        }
	/*
	 * Added for Australia test #T3-11-039, not sending 
	 * out the cause in the Release.
	 */
      case BRI_TS013_STYPE:
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	/* stay in the same state */
	break;

      case PRI_TS014_STYPE:
        /* 
         * Change per the TS014 - 1995 spec, section 5.4.2.4.4.5
         */
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
        L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
        L3_SavePkt(&NLCB->Release, L3_PktOut);  /* save RELEASE */
        LIF_StartTimer(NLCB->Timer_T308);
        NLCB->State = RELEASE_REQUEST;  /* State 19 */
	break;

      default:
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
    }
}

/******************
* U11_FacilityAck *
*******************
*
* State = 11;  Event = Peer Facility Acknowledge
*
*/
PRIVATE void near U11_FacilityAck()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/******************
* U11_FacilityRej *
*******************
*
* State = 11;  Event = Peer Facility Reject
*
*/
PRIVATE void near U11_FacilityRej()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/************
*  U11_Info *
*************
*
* State = 11;  Event = Peer Info Command
*
*/
PRIVATE void near U11_Info()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	/*
	 * Send post-hangup call charges information to Call Control.
	 */
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
	Ux_BadMsg();
	break;
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* send it to CC */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
      case PRI_NET5_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	U11_BadMsg();
	break;
    }
}

/***************
*  U11_Release *
****************
*
* State = 11;  Event = Peer Release Command
*
*/
PRIVATE void near U11_Release()
{
#ifdef BRI_DMS100
    if (SwitchType == BRI_DMS100_STYPE)
	NLCB->Cause = 0;	/* Clear possible local cause value */
#endif
    LIF_StopTimer(NLCB->Timer_T305);
    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)	/* === */
	L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, FALSE, FALSE);	/* === */
    else			/* === */
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
    switch (SwitchType) {
      case BRI_TS013_STYPE:
	/*
	 * Wait here for Call Control to validate the Cause and Display
	 * information. CC will reply with a CC_REL_COMP_REQ message, then
	 * the Release Complete can be sent. ===
	 */
	break;

      default:
	NLCB->Cause = 0;	/* no cause IE */
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
    }
}

/*****************
* U11_ReleaseReq *
******************
*
* State = 11;  Event = Call Control Release Request
*
*/
PRIVATE void near U11_ReleaseReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	Ux_ReleaseReq();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_ReleaseReq();
	break;
#endif
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	Ux_ReleaseReq();
	break;
      default:
	break;
    }
}

/*************
* U11_Status *
**************
*
* State = 11, 12.  Event = Network Peer Status
*
*/
PRIVATE void near U11_Status()
{
    switch (SwitchType) {
	/*
	 * We are allowed to ignore this, but it is preferred that we do not
	 */
#ifdef 0
	case VN2_STYPE:		/* Ignore in clearing states per */
	case VN3_STYPE:
	break;			/* ST/LAA/RSM/144 5.1.8.9 (2b.) */
	/* in ST/LAA/ELR/TRA/743 this is in section 5.2.8.9 === */
#endif
      default:
	Ux_Status();
	break;
    }
}

#ifdef BRI_1TR6
/******************
*  U11_SuspendAck *
*******************
*
* State = 11;  Event = Network Peer Suspend Acknowledge Command
*
*/
PRIVATE void near U11_SuspendAck()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	L3_Mail(CC_SUSPEND_CONF, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

#endif
/******************
*  U11_T305Expiry *
*******************
*
* State = 11;  Event = Timer 305 Expiry
*
*/
PRIVATE void near U11_T305Expiry()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	break;
#ifdef VN2
      case VN2_STYPE:
	break;			/* No cause */

      case VN3_STYPE:
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	Diagnostic[0] = '3';
	Diagnostic[1] = '0';
	Diagnostic[2] = '5';
	break;
#endif
      case BRI_1TR6_STYPE:
	break;			/* === no cause for 1tr6 certif */

      case BRI_NET3_STYPE:
      case PRI_NET5_STYPE:
      case BRI_5ESS_STYPE:
      case BRI_DMS100_STYPE:
      case BRI_NI1_STYPE:
	/*
	 * Have to send the same Cause that went out with the DISCONNECT
	 * message, initially failed on cancun homologation because setting
	 * the cause value to TIMER_EXPIRY, Cause 102.
	 */
        if (detail_debug)
            buginf("\n U11_T305Expiry prev = %x, cause = %x", NLCB->PreviousCause, NLCB->Cause); 

	if (NLCB->PreviousCause != 0)	/* === */
	    NLCB->Cause = NLCB->PreviousCause;	/* Cause from CC_DISC_REQ */
	break;

      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	/*
	 * have to send the same Cause that went out with the DISCONNECT
	 * message.
	 * 
	 * NLCB->Cause = TIMER_EXPIRY;  * Cause 102 *
	 */
	if (NLCB->PreviousCause != 0)
	    NLCB->Cause = NLCB->PreviousCause;	/* Cause from CC_DISC_REQ */
	break;

      default:
	NLCB->Cause = PROTOCOL_ERROR;	/* Cause 111 */
	break;
    }
    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
    LIF_StartTimer(NLCB->Timer_T308);
    switch (SwitchType) {
      case BRI_DMS100_STYPE:
	L3_Mail(CC_RELEASE_IND, CC_ID, TRUE, FALSE);
	break;
      case PRI_DMS100_STYPE:
	NLCB->Cause = PROTOCOL_ERROR;	/* Cause 111 */
	/*
	 * The DMS SDLs specify CC_RELEASE_IND here, which is an exception to
	 * the way this message is otherwise used.  Normally it causes Call
	 * Control to issue a CC_REL_COMP_REQ (RELEASE_RESP) which will be
	 * inappropriate in L3 since we next expect network RELEASE_COMP
	 * rather than the user to issue RELEASE_COMP from State 19.
	 */
	L3_Mail(CC_RELEASE_CONF, CC_ID, TRUE, FALSE);
	break;
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_TIMEOUT_IND, CC_ID, TRUE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_TIMEOUT_IND, CC_ID, TRUE, FALSE);
	break;
#endif
      default:
	break;
    }
    NLCB->State = RELEASE_REQUEST;	/* State 19 */
}

/* ****  U 12 :  D I S C O N N E C T   R E Q U E S T   S T A T E  **** */

/******************
*  U12_Disconnect *
*******************
*
* State = 12;  Event = Peer Disconnect Command
*
*/
PRIVATE void near U12_Disconnect()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/***************
*  U12_DiscReq *
****************
*
* State = 12;  Event = Call Control Disconnect Request
*
* This is a standard clearing collision where the Network has sent
* DISCONNECT and the local Call Control sends a susequent CC_DISCONNECT_REQ
* due to a coincidental local hangup before it processes the
* CC_DISCONNECT_IND.
*
*/
PRIVATE void near U12_DiscReq()
{
    switch (SwitchType) {
	case BRI_5ESS_STYPE:
	Ux_DiscReq();
	break;
      case BRI_DMS100_STYPE:
	Ux_DiscReq();
	break;
      case PRI_DMS100_STYPE:
	Ux_DiscReq();
	break;
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_DiscReq();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*******************
*  U12_FacilityAck *
********************
*
* State = 12;  Event = Peer Facility Acknowledge.
*
*/
PRIVATE void near U12_FacilityAck()
{

    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*******************
*  U12_FacilityRej *
********************
*
* State = 12;  Event = Peer Facility Reject.
*
*/
PRIVATE void near U12_FacilityRej()
{

    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*******************
*  U12_FacilityReq *
********************
*
* State = 12;  Event = Call Control Facility Request.
*
*/
PRIVATE void near U12_FacilityReq()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	/*
	 * User may send FACILITY request for supplemental service such as
	 * malicious call tracing.
	 */
	L3_Mail(FACILITY, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(FACILITY_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(FACILITY_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/***********
* U12_Info *
************
*
* State = 12;  Event = Peer Info Command
*
*/
PRIVATE void near U12_Info()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
      case VN2_STYPE:		/* === */
      case VN3_STYPE:		/* === */
	break;			/* === */

#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsgAlt();
	break;
    }
}

/***************
*  U12_Release *
****************
*
* State = 12;  Event = Peer Release Command
*
*/
PRIVATE void near U12_Release()
{
    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)	/* === */
	L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, FALSE, FALSE);	/* === */
    else			/* === */
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
    switch (SwitchType) {
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	/*
	 * Wait here for Call Control to validate the Cause and Display
	 * information. CC will reply with a CC_REL_COMP_REQ message, then
	 * the Release Complete can be sent. ===
	 */
	break;

      default:
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
    }
}

/******************
*  U12_ReleaseReq *
*******************
*
* State = 12;  Event = Call Control Release Request
*
*/
PRIVATE void near U12_ReleaseReq()
{
    if (detail_debug)
        buginf ("\n sending release U12 rel req");
    L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
    LIF_StartTimer(NLCB->Timer_T308);
    NLCB->State = RELEASE_REQUEST;	/* State 19 */
}
#ifdef BRI_1TR6

/* **********  U 14 :  C A L L  P A S S I V E   S T A T E  *********** */

/*****************
* U14_ConnectAck *
******************
*
* State = 14;  Event = Peer Connect Acknowledge
*
*/
PRIVATE void near U14_ConnectAck()
{
    if (SwitchType == BRI_1TR6_STYPE) {
	L3_Mail(CC_SETUP_COMP_IND, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
    } else {
	Ux_BadMsg();
    }
}

#endif				/* BRI_1TR6 */
/* *******  U 15 :  S U S P E N D   R E Q U E S T   S T A T E  ******* */

/******************
*  U15_Disconnect *
*******************
*
* State = 15;  Event = Network Peer Disconnect Command
*
*/
PRIVATE void near U15_Disconnect()
{
    LIF_StopTimer(NLCB->Timer_T319);
    L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
    NLCB->State = DISCONNECT_IND;	/* State 12 */
}

/***************
*  U15_DiscReq *
****************
*
* State = 15;  Event = Call Control Disconnect Request
*
*/
PRIVATE void near U15_DiscReq()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/***************
*  U15_Release *
****************
*
* State = 15;  Event = Network Peer Release Command
*
*/
PRIVATE void near U15_Release()
{
    LIF_StopTimer(NLCB->Timer_T319);
    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)	/* === */
	L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, FALSE, FALSE);	/* === */
    else			/* === */
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
    L3_CallRelease();		/* State 0 */
}

/******************
*  U15_ReleaseReq *
*******************
*
* State = 15;  Event = Call Control Release Request
*
*/
PRIVATE void near U15_ReleaseReq()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/******************
*  U15_SuspendAck *
*******************
*
* State = 15;  Event = Network Peer Suspend Acknowledge Command
*
*/
PRIVATE void near U15_SuspendAck()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef KDD_NTT
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_SUSPEND_CONF, CC_ID, FALSE, FALSE);
	LIF_StopTimer(NLCB->Timer_T319);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef CCITT
      case CCITT_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/******************
*  U15_SuspendRej *
*******************
*
* State = 15;  Event = Network Peer Suspend Reject Command
*
*/
PRIVATE void near U15_SuspendRej()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_REJ, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef KDD_NTT
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_REJ, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_SUSPEND_REJ, CC_ID, FALSE, FALSE);
	LIF_StopTimer(NLCB->Timer_T319);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef CCITT
      case CCITT_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_REJ, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_REJ, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	LIF_StopTimer(NLCB->Timer_T319);
	L3_Mail(CC_SUSPEND_REJ, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/******************
*  U15_T319Expiry *
*******************
*
* State = 15;  Event = Timer T319 Expiry
*
*/
PRIVATE void near U15_T319Expiry()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	L3_Mail(CC_ERROR_IND, CC_ID, TRUE, FALSE);
	break;
#endif
      default:
	L3_Mail(CC_SUSPEND_REJ, CC_ID, TRUE, FALSE);
	break;
    }
    NLCB->State = CALL_ACTIVE;	/* State 10 */
}

/* ********  U 17 :  R E S U M E   R E Q U E S T   S T A T E  ******** */

/*****************
* U17_Disconnect *
******************
*
* State = 17;  Event = Peer Disconnect Command
*
*/
PRIVATE void near U17_Disconnect()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/**************
* U17_DiscReq *
***************
*
* State = 17;  Event = Call Control Disconnect Request
*
*/
PRIVATE void near U17_DiscReq()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
	LIF_StartTimer(NLCB->Timer_T305);
	NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/***********
* U17_Info *
************
*
* State = 17;  Event = Network Peer Info Command
*
*/
PRIVATE void near U17_Info()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
      default:
	Ux_Info();
	break;
    }
}

/***************
*  U17_Release *
****************
*
* State = 17;  Event = Network Peer Release Command
*
*/
PRIVATE void near U17_Release()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/******************
*  U17_ReleaseReq *
*******************
*
* State = 17;  Event = Call Control Release Request
*
*/
PRIVATE void near U17_ReleaseReq()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
*  U17_ResumeAck *
******************
*
* State = 17;  Event = Network Peer Resume Acknowledge Command
*
*/
PRIVATE void near U17_ResumeAck()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_CONF, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef KDD_NTT
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_CONF, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_RESUME_CONF, CC_ID, FALSE, FALSE);
	LIF_StopTimer(NLCB->Timer_T318);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef CCITT
      case CCITT_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_CONF, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_CONF, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_CONF, CC_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*****************
*  U17_ResumeRej *
******************
*
* State = 17;  Event = Network Peer Resume Reject Command
*
*/
PRIVATE void near U17_ResumeRej()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef KDD_NTT
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef CCITT
      case CCITT_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	LIF_StopTimer(NLCB->Timer_T318);
	L3_Mail(CC_RESUME_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/******************
*  U17_T318Expiry *
*******************
*
* State = 17;  Event = Timer T318 Expiry
*
*/
PRIVATE void near U17_T318Expiry()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	L3_Mail(CC_RESUME_REJ, CC_ID, TRUE, FALSE);
	NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	Diagnostic[0] = '3';
	Diagnostic[1] = '1';
	Diagnostic[2] = '8';
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_TIMEOUT_IND, CC_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      default:
	L3_Mail(CC_RESUME_REJ, CC_ID, TRUE, FALSE);
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
    }
}

/* *******  U 19 :  R E L E A S E   R E Q U E S T   S T A T E  ******* */

/**************
*  U19_BadMsg *
***************
*
*   State 19;  Event = Unexpected or Unknown Message
*
*/
PRIVATE void near U19_BadMsg()
{
    if (L3_MsgSource == L2_ID) {
	switch (SwitchType) {
#ifdef PRI_DMS100
	    case PRI_DMS100_STYPE:
	    break;
#endif
#ifdef BRI_DMS100
	  case BRI_DMS100_STYPE:
	    break;		/* do nothing */
#endif
	  case PRI_4ESS_STYPE:
	  case PRI_5ESS_STYPE:
	    if ((NLCB->Event == RESTART) || (NLCB->Event == RESTART_ACK))
		Ux_BadMsg();
	    break;
#ifdef BRI_5ESS
	  case BRI_5ESS_STYPE:
	    Ux_BadMsg();
	    break;

#endif
#ifdef VN2
	  case VN2_STYPE:
	  case VN3_STYPE:
	    Ux_BadMsg();
	    break;
#endif
#ifdef BRI_NI1			/* === */
	  case BRI_NI1_STYPE:
	    Ux_BadMsg();
	    break;
#endif
#ifdef KDD_NTT
	  case KDD_STYPE:
	  case NTT_STYPE:
	  case PRI_NTT_STYPE:
	    Ux_BadMsg();
	    break;
#endif
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    Ux_BadMsg();
	    break;
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    Ux_BadMsg();
	    break;
#endif
#ifdef BRI_NET3			/* === */
	  case BRI_NET3_STYPE:
	  case PRI_NET5_STYPE:
	    Ux_BadMsg();
	    break;
#endif
#ifdef BRI_TS013
	  case BRI_TS013_STYPE:
	  case PRI_TS014_STYPE:
	    Ux_BadMsg();
	    break;
#endif
	  default:
	    L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);
	    break;
	}
    } else {
	switch (SwitchType) {
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    Ux_BadMsg();
	    break;
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    Ux_BadMsg();
	    break;
#endif
	  default:
#ifdef DISPLAY_LOCAL_ERR
	    LIF_Error("l3_sdl_u", "U19_BadMsg", "Bad message received.");
#endif
	    break;
	}
    }
}

/******************
*  U19_ConnectAck *
*******************
*
* State = 19;  Event = Connect Ack.
* for TS014 should ignore.
*
*/
PRIVATE void near U19_ConnectAck()
{
    switch (SwitchType) {
      case PRI_TS014_STYPE:
        break;                  /* do nothing */
      default:
        U19_BadMsg();
        break;
    }
}

/*****************
* U19_Disconnect *
******************
*
* State = 19;  Event = Network Peer Disconnect Command
*
*/
PRIVATE void near U19_Disconnect()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	break;			/* do nothing */
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	break;			/* do nothing */
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_5ESS
      case BRI_5ESS_STYPE:
	Ux_BadMsg();
	break;			/* complain */
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	break;			/* do nothing */
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	break;			/* do nothing */
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
	break;			/* do nothing */
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;
#endif
      default:
	L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);	/* bad message */
	break;
    }
}

/******************
* U19_FacilityAck *
*******************
*
* State = 19; Event = Peer Facility Acknowledge
*
*/
PRIVATE void near U19_FacilityAck()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	U19_BadMsg();
	break;
    }
}

/******************
* U19_FacilityRej *
*******************
*
* State = 19; Event = Peer Facility Reject
*
*/
PRIVATE void near U19_FacilityRej()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	U19_BadMsg();
	break;
    }
}

/***********
* U19_Info *
************
*
* State = 19;  Event = Peer Info Command
*
*/
PRIVATE void near U19_Info()
{
    switch (SwitchType) {
#ifdef VN2			/* === */
	case VN2_STYPE:		/* === */
	case VN3_STYPE:		/* === */
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* === */
	break;			/* === */
#endif				/* === */
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
      case BRI_NET3_STYPE:
      case PRI_NET5_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);
	break;
#endif
      default:
	U19_BadMsg();
	break;
    }
}

/***************
*  U19_Release *
****************
*
* State = 19;  Event = Peer Release Command
*/
PRIVATE void near U19_Release()
{
    switch (SwitchType) {
#ifdef BRI_NI1			/* === */
      case BRI_5ESS_STYPE:
      case BRI_NI1_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
      case PRI_NET5_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      default:
	L3_CallRelease();	/* State 0 */
	break;			/* do nothing */
    }
}

/*******************
*  U19_ReleaseComp *
********************
*
* State = 19;  Event = Peer Release Complete Command
*/
PRIVATE void near U19_ReleaseComp()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	/*
	 * This message is an addition to the published SDLs, but it is
	 * required to clear the call in Call Control when it has issued
	 * CC_RELEASE_REQ.
	 */
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;

#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      default:
	LIF_StopTimer(NLCB->Timer_T308);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
    }
}

/*****************
* U19_RelCompReq *
******************
*
* State = 19; Event = Call Control Release Complete Request.
*
*/
PRIVATE void near U19_RelCompReq()
{
    switch (SwitchType) {
#ifdef PRI_DMS100
	case PRI_DMS100_STYPE:
	L3_Mail(RELEASE_COMP, L2_ID, FALSE, FALSE);
	L3_CallRelease();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	L3_Mail(RELEASE_COMP, L2_ID, FALSE, FALSE);
	L3_CallRelease();
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/**************
*  U19_Status *
***************
*
* State = 19;  Event = L2 Status Indication
*
*/
PRIVATE void near U19_Status()
{
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
	break;			/* Ignore message */
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	break;			/* Ignore message */
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	Ux_Status();
	break;
#endif

	/*
	 * We are allowed to ignore this, but it is preferred that we do not
	 */
#if 0
      case VN2_STYPE:
      case VN3_STYPE:
	break;
#endif

#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	/*
	 * Ignore unless the received call state is null; if null then
	 * release all resources and go to null state.
	 */
	if (L3_GetState(L3_PktIn->Info,
			L3_PktIn->InfoLen,
			&NLCB->Cause) == NULL_STATE) {
	    /*
	     * Added this for Australia test #T3-19-071. ===
	     */
	    if (NLCB->Cause == MANDATORY_IE_MISSING) {
		/*
		 * Just reply to the STATUS wo the Call State with a RELEASE
		 * message, the T308 is already running and already in State
		 * 19, so let it continue.
		 */
		Diagnostic[0] = CALL_STATE_IE;
		Diagnostic[1] = 0;
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	    } else {
		NLCB->State = NULL_STATE;	/* Report NULL_STATE to CC */
		LIF_StopTimer(NLCB->Timer_T308);
		L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
		L3_CallRelease();	/* State 0 */
	    }
	}
	break;
#endif
      case BRI_NET3_STYPE:
      case PRI_NET5_STYPE:
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
#endif
      default:
	if (L3_GetState(L3_PktIn->Info,
			L3_PktIn->InfoLen,
			&NLCB->Cause) == NULL_STATE) {
	    NLCB->State = NULL_STATE;	/* Report NULL_STATE to CC */
	    L3_Mail(CC_STATUS_IND, CC_ID, FALSE, FALSE);
	    L3_CallRelease();	/* State 0 */
	}
	break;
    }
}

/******************
*  U19_T308Expiry *
*******************
*
* State = 19;  Event = Timer T308 Expiry
*
*/
PRIVATE void near U19_T308Expiry()
{
    if (NLCB->T308_First) {	/* try again */
	NLCB->T308_First = FALSE;

        /*
         * Found during conformance testing. If the diagnostic byte is set on
         * the previous message, then it is cleared here, but still goes out
         * because the old message is still around.
         */
        L3_PktIn = NLCB->Release;
        NLCB->Release = NULL;
        L3_AvailPktIn = TRUE;	/* Make it available */
	L3_Mail(RELEASE, L2_ID, FALSE, TRUE);	/* Retransmit RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
    } else {			/* call failed */
	switch (SwitchType) {
	  case PRI_4ESS_STYPE:
	  case PRI_5ESS_STYPE:
	  case PRI_NET5_STYPE:
	    NLCB->Cause = TIMER_EXPIRY;	/* Cause 102 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    L3_Mail(CC_RELEASE_CONF, CC_ID, TRUE, FALSE);
	    break;
#ifdef BRI_5ESS
	  case BRI_5ESS_STYPE:
	    L3_Mail(CC_RELEASE_CONF, CC_ID, TRUE, FALSE);
	    break;
#endif
#ifdef BRI_DMS100
	  case BRI_DMS100_STYPE:
	    L3_Mail(CC_TIMEOUT_IND, CC_ID, FALSE, FALSE);
	    break;
#endif
#ifdef PRI_DMS100
	  case PRI_DMS100_STYPE:
	    /*
	     * Allow Call Control to conduct channel restart.
	     */
	    L3_Mail(CC_TIMEOUT_IND, CC_ID, TRUE, FALSE);
	    break;
#endif
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    L3_Mail(CC_TIMEOUT_IND, CC_ID, TRUE, FALSE);
	    break;
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    L3_Mail(CC_TIMEOUT_IND, CC_ID, TRUE, FALSE);
	    L3_Mail(DL_REL_REQ, L2_ID, TRUE, FALSE);	/* === for 1tr6 certif */
	    break;
#endif
	  default:
	    L3_Mail(CC_RELEASE_CONF, CC_ID, TRUE, FALSE);
	    break;
	}
	L3_CallRelease();	/* State 0 */
    }
}

/*****************
*  U19_Undefined *
******************
*
* State = 19;  Event = Undefined message type.
*
*/
PRIVATE void near U19_Undefined()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	break;
#endif
      default:
	Ux_Undefined();
	break;
    }
}

#ifdef BRI_1TR6
/* ******  U 20 :  R E G I S T E R   R E Q U E S T   S T A T E  ****** */

/******************
* U20_RegisterAck *
*******************
*
* State = 20;  Event = Peer Register Acknowledge
*
*/
PRIVATE void near U20_RegisterAck()
{
    if (SwitchType == BRI_1TR6_STYPE) {
	L3_Mail(CC_REGISTER_ACK, CC_ID, FALSE, FALSE);
	L3_CallRelease();
    } else {
	Ux_BadMsg();
    }
}

/******************
* U20_RegisterRej *
*******************
*
* State = 20;  Event = Peer Register Reject
*
*/
PRIVATE void near U20_RegisterRej()
{
    if (SwitchType == BRI_1TR6_STYPE) {
	L3_Mail(CC_REGISTER_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();
    } else {
	Ux_BadMsg();
    }
}
#endif				/* BRI_1TR6 */
#ifdef PRI_1TR6

/* ******  U 20 :  R E G I S T E R   R E Q U E S T   S T A T E  ****** */

/******************
* U20_RegisterAck *
*******************
*
* State = 20;  Event = Peer Register Acknowledge
*
*/
PRIVATE void near U20_RegisterAck()
{
    if (SwitchType == PRI_1TR6_STYPE) {
	L3_Mail(CC_REGISTER_ACK, CC_ID, FALSE, FALSE);
	L3_CallRelease();
    } else {
	Ux_BadMsg();
    }
}

/******************
* U20_RegisterRej *
*******************
*
* State = 20;  Event = Peer Register Reject
*
*/
PRIVATE void near U20_RegisterRej()
{
    if (SwitchType == PRI_1TR6_STYPE) {
	L3_Mail(CC_REGISTER_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();
    } else {
	Ux_BadMsg();
    }
}

#endif				/* PRI_1TR6 */
#ifdef BRI_1TR6
/* ********  U 21 :  C A N C E L   R E Q U E S T   S T A T E  ******** */

/****************
* U21_CancelAck *
*****************
*
* State = 21;  Event = Peer Cancel Acknowledge
*
*/
PRIVATE void near U21_CancelAck()
{
    if (SwitchType == BRI_1TR6_STYPE) {
	L3_Mail(CC_CANCEL_ACK, CC_ID, FALSE, FALSE);
	L3_CallRelease();
    } else {
	Ux_BadMsg();
    }
}

/****************
* U21_CancelRej *
*****************
*
* State = 21;  Event = Peer Cancel Reject
*
*/
PRIVATE void near U21_CancelRej()
{
    if (SwitchType == BRI_1TR6_STYPE) {
	L3_Mail(CC_CANCEL_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();
    } else {
	Ux_BadMsg();
    }
}

#endif				/* BRI_1TR6 */
#ifdef PRI_1TR6
/* ********  U 21 :  C A N C E L   R E Q U E S T   S T A T E  ******** */

/****************
* U21_CancelAck *
*****************
*
* State = 21;  Event = Peer Cancel Acknowledge
*
*/
PRIVATE void near U21_CancelAck()
{
    if (SwitchType == PRI_1TR6_STYPE) {
	L3_Mail(CC_CANCEL_ACK, CC_ID, FALSE, FALSE);
	L3_CallRelease();
    } else {
	Ux_BadMsg();
    }
}

/****************
* U21_CancelRej *
*****************
*
* State = 21;  Event = Peer Cancel Reject
*
*/
PRIVATE void near U21_CancelRej()
{
    if (SwitchType == PRI_1TR6_STYPE) {
	L3_Mail(CC_CANCEL_REJ, CC_ID, FALSE, FALSE);
	L3_CallRelease();
    } else {
	Ux_BadMsg();
    }
}
#endif				/* PRI_1TR6 */

/* *******  U 25 :  O V E R L A P   R E C E I V E   S T A T E  ******* */

/*******************
*  U25_AlertingReq *
********************
*
* State = 25;  Event = CC Alerting Request
*
*/
PRIVATE void near U25_AlertingReq()
{
    L3_Mail(ALERTING, L2_ID, FALSE, FALSE);
    NLCB->State = CALL_RECEIVED;/* State 7 */
}

/*******************
*  U25_CallProcReq *
********************
*
* State = 25;  Event = CC Call Proceeding Request
*
*/
PRIVATE void near U25_CallProcReq()
{
    L3_Mail(CALL_PROC, L2_ID, FALSE, FALSE);
    NLCB->State = IN_CALL_PROC;	/* State 9 */
}

/*******************
*  U25_ProgressReq *
********************
*
* State = 25;  Event = CC Progress Request
*
*/
PRIVATE void near U25_ProgressReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
      default:
	L3_Mail(PROGRESS, L2_ID, FALSE, FALSE);
	break;
    }
}

/*****************
*  U25_SetupResp *
******************
*
* State = 25;  Event = CC Setup Response
*
*/
PRIVATE void near U25_SetupResp()
{
    L3_Mail(CONNECT, L2_ID, FALSE, FALSE);
    switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_SavePkt(&NLCB->Connect, L3_PktOut);	/* save CONNECT */
	break;

      default:
	break;
    }
    LIF_StartTimer(NLCB->Timer_T313);
    NLCB->State = CONNECT_REQUEST;	/* State 8 */
}

/* ********  U 98 :   I D L E   D A T A   L I N K   S T A T E  ******* */

/*********************
*  U98_BroadcastResp *
**********************
*
* State = 98;  Event = Call Control Broadcast Setup Response
*
*  Broadcast Call Origination from the NETWORK
*
*  Cannot process this event until ESTABLISHED (state 0).
*  Send this packet to ourselves with a delay
*  (give ourselves time to get to state 0).
*
*/
PRIVATE void near U98_BroadcastResp()
{
    LIF_StopTimer(NLCB->Timer_twait);
    L3_EnQueue(&NLCB->MsgQ);
    L3_Mail(DL_EST_REQ, L2_ID, TRUE, FALSE);
    NLCB->State = ESTABLISH_WAIT;	/* State 99 */
}

/****************
*  U98_NLEstReq *
*****************
*
* State = 98;  Event = Call Control NL_Establish_Req
*
*/
PRIVATE void near U98_NLEstReq ()
{
    int random;

    random = 0;
    if (SwitchType == BRI_NI1_STYPE) {
        LIF_StopTimer(NLCB->Timer_twait);
        random = L3_start_twait_timer(L3_DSL_Id);
    }

    if (random != 0) {
        if (isdn_debug || isdn_debug_ok(q921_debug, L3_DSL_Id)) {
            buginf("\nISDN %s: Start twait timer, for %d seconds",
               DSL_TO_NAME(L3_DSL_Id), random/ONESEC);
        }
        LIF_UpdateTimer(NLCB->Timer_twait, L3_ID, L3_ID, TWAIT_EXPIRY,
                        L3_DSL_Id, Q931_SAPI, NLCB->Ces, 0, 0, random);
        LIF_StartTimer(NLCB->Timer_twait);
    } else {
        L3_Mail(DL_EST_REQ, L2_ID, FALSE, FALSE);
        NLCB->State = ESTABLISH_WAIT;	/* State 99 */
        NLCB->CallId = 0;
    }
}

/********************
*  U98_Twait_expiry *
*********************
*
* State = 98;  Event = Expiry of National ISDN timer to delay activation
*
*/
PRIVATE void U98_Twait_expiry ()
{
    LIF_StopTimer(NLCB->Timer_twait);
    if (isdn_debug || isdn_debug_ok(q921_debug || q931_debug, L3_DSL_Id)) {
        buginf("\nISDN %s: Twait timer expired, Activate Layer 2",
               DSL_TO_NAME(L3_DSL_Id));
    }
    L3_Mail(DL_EST_REQ, L2_ID, FALSE, FALSE);
    NLCB->State = ESTABLISH_WAIT;	/* State 99 */
    NLCB->CallId = 0;
} 

/****************
*  U98_NLEstReq *
*****************
*
* State = 98;  Event = Call Control NL_Release_Req
*
*/
PRIVATE void near U98_NLRelReq()
{
    /*
     * Confirm to Call Control that no data link exists.
     */
    L3_Mail(NL_REL_CONF, CC_ID, TRUE, FALSE);
    L3_Free_NLCB(NLCB);
}

/****************
*  U98_SetupReq *
*****************
*
* State = 98;  Event = Call Control Setup Request
*
*  Call Origination from the USER TERMINAL
*
*  Cannot process this event until ESTABLISHED (state 0).
*
*  NOTE: This function is also used to process CC_RESUME_REQ.
*/
PRIVATE void near U98_SetupReq()
{
    NLCB_ID nlcb;

    L3_EnQueue(&NLCB->MsgQ);
    /*
     * don't send the DL_EST_REQ message if there already is an NLCB in state
     * 99 for the same CES.
     */
    nlcb = NLCB_Head[L3_DSL_Id];
    while ((nlcb) && ((nlcb->Ces != NLCB->Ces) ||
		      (nlcb->State != ESTABLISH_WAIT)))
	nlcb = nlcb->Next;

    if (nlcb == NULL)
	L3_Mail(DL_EST_REQ, L2_ID, TRUE, FALSE);

    LIF_StopTimer(NLCB->Timer_twait);
    NLCB->State = ESTABLISH_WAIT;	/* State 99 */
}

/* *******  U 99 :   E S T A B L I S H    W A I T    S T A T E  ****** */

/*******************
*  U99_AlertingReq *
********************
*
* State = 99;  Event = Call Control Alerting Request
*
*  Cannot process this event until ESTABLISHED and CALL PRESENT (state 6).
*  We will shelve the message for recovery subsequent to completion of
*  data link establishment.
*
*/
PRIVATE void near U99_AlertingReq()
{
    L3_EnQueue(&NLCB->MsgQ);
}

/*********************
*  U99_BroadcastResp *
**********************
*
* State = 99;  Event = Call Control Broadcast Response.
*
*  Cannot process this event until ESTABLISHED and in NULL_STATE (State 0).
*  This message may result if, as data link establishment is in progress
*  due to an NL_EST_REQ from Call Control, a BROADCAST SETUP from below is
*  responded to by Call Control.  We will shelve the message for recovery
*  subsequent to completion of data link establishment.
*
*/
PRIVATE void near U99_BroadcastResp()
{
    L3_EnQueue(&NLCB->MsgQ);
}

/*******************
*  U99_CallProcReq *
********************
*
* State = 99;  Event = Call Control Call Proceeding Request
*
*  Cannot process this event until ESTABLISHED and CALL PRESENT (state 6).
*  We will shelve the message for recovery subsequent to completion of
*  data link establishment.
*
*/
PRIVATE void near U99_CallProcReq()
{
    L3_EnQueue(&NLCB->MsgQ);
}

/*****************
*  U99_DLEstConf *
******************
*
* State = 99;  Event = Peer Establish Confirm
*
*/
PRIVATE void near U99_DLEstConf()
{
    NLCB_ID nlcb;

    nlcb = NLCB;		/* Save entry nlcb.  We'll need NLCB for
				 * L3_Mail */
    nlcb->State = NULL_STATE;	/* Update state to State 0 in this NLCB */

    /*
     * Now we must locate all the other nlcb's which may have been in the
     * process of setting up the same data link.
     */
    NLCB = NLCB_Head[L3_DSL_Id];
    while (NLCB) {
	if ((NLCB->Ces == nlcb->Ces) && (NLCB->State == ESTABLISH_WAIT)) {
	    /*
	     * Somebody else trying to establish the same data link.
	     */
	    NLCB->State = NULL_STATE;	/* Data link is now established */

	    /*
	     * Either this NLCB has a pending CC_SETUP_REQ or it was driven
	     * by a NL_EST_REQ from its Call Control.
	     */
	    if (NLCB->Internal == TRUE)	/* Pending SETUP */
		/*
		 * We deliberately send the NLCB a message which is a NOP in
		 * State 0.  The effect of this will simply be to wake it up
		 * so that we flush the internal (SETUP) mail pending through
		 * a call to L3_ProcessInternal after the primary event is
		 * NOP'ed.
		 */
		L3_Mail(NL_EST_REQ, L3_ID, FALSE, FALSE);
	}
        /*
         * Stop the NI1 twait timer if it is running, L2 is up.
         */
        if (SwitchType == BRI_NI1_STYPE)
            LIF_StopTimer(NLCB->Timer_twait);

	NLCB = NLCB->Next;
    }

    NLCB = nlcb;		/* Restore entry nlcb */

    /*
     * Possible NL_EST_REQ case - send confirmation to CC.  The establishment
     * is confirmed to CC whether or not it requested it overtly.
     */
    L3_Mail(NL_EST_CONF, CC_ID, FALSE, FALSE);

    /*
     * If this NLCB has a pending SETUP message for Layer 2 this will
     * recovered in L3_ProcessInternal.
     */
}

/****************
*  U99_DLEstInd *
*****************
*
* State = 99;  Event = Peer Establish Indication
*
*/
PRIVATE void near U99_DLEstInd()
{
    NLCB_ID nlcb;

    NLCB->State = NULL_STATE;	/* State 0 */

    nlcb = NLCB_Head[L3_DSL_Id];
    /* Move any and all NLCBs waiting on this establishment to State 0. */
    while (nlcb) {
	if ((nlcb->Ces == NLCB->Ces) && (nlcb->State == ESTABLISH_WAIT))
	    nlcb->State = NULL_STATE;
	nlcb = nlcb->Next;
    }
    L3_Mail(NL_EST_CONF, CC_ID, FALSE, FALSE);
}

/***************
*  U99_InfoReq *
****************
*
* State = 99;  Event = Call Control Information Request
*
*  Cannot process this event until ESTABLISHED and OVER SENDING (State 6).
*  We will shelve the message for recovery subsequent to completion of
*  data link establishment.
*
*/
PRIVATE void near U99_InfoReq()
{
    L3_EnQueue(&NLCB->MsgQ);
}

/******************
*  U99_ReleaseReq *
*******************
*
* State = 99;  Event = Call Control Call Release Request.
*
*  Cannot process this event until ESTABLISHED and CALL PRESENT (state 6).
*  We will shelve the message for recovery subsequent to completion of
*  data link establishment.
*
*/
PRIVATE void near U99_ReleaseReq()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	L3_EnQueue(&NLCB->MsgQ);/* Release for deflection or UUI */
	break;
#endif
      default:
	break;
    }
}

/* same as above, but for Release Completes.   routine added === */
PRIVATE void near U99_RelCompReq()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	L3_EnQueue(&NLCB->MsgQ);/* Release for deflection or UUI */
	break;
#endif
      default:			/* this should never happen */
	break;
    }
}


/*******************
*  U99_SetupRejReq *
********************
*
* State = 99;  Event = Call Control Setup Reject Request
*
*  Cannot process this event until ESTABLISHED and CALL PRESENT (state 6).
*  We will shelve the message for recovery subsequent to completion of
*  data link establishment.
*
*/
PRIVATE void near U99_SetupRejReq()
{
    L3_EnQueue(&NLCB->MsgQ);
}

/****************
*  U99_SetupReq *
*****************
*
* State = 99;  Event = Call Control Alerting Request
*
*  Cannot process this event until ESTABLISHED and in State 0.
*  We will shelve the message for recovery subsequent to completion of
*  data link establishment.
*
*/
PRIVATE void near U99_SetupReq()
{
    L3_EnQueue(&NLCB->MsgQ);
}

/*****************
*  U99_SetupResp *
******************
*
* State = 99;  Event = Call Control Call Setup Response.
*
*  Cannot process this event until ESTABLISHED and CALL PRESENT (state 6).
*  We will shelve the message for recovery subsequent to completion of
*  data link establishment.
*
*/
PRIVATE void near U99_SetupResp()
{
    L3_EnQueue(&NLCB->MsgQ);
}

/* *********  M U L T I P L E   S T A T E   M E S S A G E S  ********* */

/*************
*  Ux_BadMsg *
**************
*
*   State = Any except 98,99,0,1,6,19;  Event = Unexpected Message
*
*/
PRIVATE void near Ux_BadMsg()
{
    extern void itoa(int, char*, int);
 
#ifdef DISPLAY_LOCAL_ERR
    char err_text[256];		/* made array bigger === */
    char err_event[32];		/* === */
    char err_src[32];		/* === */
#endif

    if (L3_MsgSource == L2_ID) {
	switch (SwitchType) {
	  case BRI_DMS100_STYPE:
	  case BRI_NI1_STYPE:
	    NLCB->Cause = WRONG_MSG_FOR_STATE;	/* Cause 101 */
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;

	  case PRI_4ESS_STYPE:
	  case PRI_5ESS_STYPE:
	  case PRI_NET5_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;

          /*
           * changes for mars homologation
           */
          case PRI_TS014_STYPE:
            NLCB->Cause = WRONG_MSG_FOR_STATE;  /* Cause 101 */
            Diagnostic[0] = NLCB->Event;
            L3_Mail(STATUS, L2_ID, TRUE, FALSE);
            break;

#ifdef BRI_5ESS
	  case BRI_5ESS_STYPE:
	    /*
	     * STATUS option provided in AT&T 5D5-900-311 Para. 3.5.2.
	     */
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef PRI_DMS100
	  case PRI_DMS100_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save RELEASE */
	    LIF_StartTimer(NLCB->Timer_T308);
	    NLCB->State = RELEASE_REQUEST;	/* State 19 */
	    break;
#endif
#ifdef VN2
	  case VN2_STYPE:
	  case VN3_STYPE:
	    /*
	     * ST/LAA/RSM/ 133 Section 5.1.8.4.
	     */
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    Diagnostic[1] = 0;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef KDD_NTT
	  case KDD_STYPE:
	  case NTT_STYPE:
	  case PRI_NTT_STYPE:
	    NLCB->Cause = WRONG_MSG_FOR_STATE;	/* Cause 101 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    break;		/* Ignore */
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    break;		/* Ignore */
#endif
#ifdef BRI_NET3			/* === */
	  case BRI_NET3_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    Diagnostic[1] = 0;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef BRI_TS013
	  case BRI_TS013_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    Diagnostic[1] = 0;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
	  default:
	    L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);
	    break;
	}
    } else {
	switch (SwitchType) {
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    if (L3_MsgSource == CC_ID)
		Ux_Error();
	    break;
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    if (L3_MsgSource == CC_ID)
		Ux_Error();
	    break;
#endif
	  default:
	    break;
	}

#ifdef DISPLAY_LOCAL_ERR
	itoa(L3_MsgSource, err_src, 16);
	itoa(NLCB->Event, err_event, 16);
	strcpy(err_text, "Source ID = ");
	strcat(err_text, err_src);
	strcat(err_text, "  Event = ");
	strcat(err_text, err_event);
	LIF_Error("l3_sdl_u", "Ux_BadMsg", err_text);
#endif
    }
}

/****************
*  Ux_BadMsgAlt *
*****************
*
* State = any;  Event = Unexpected message, alternate treatment.
*
* This function exists to provide differing actions required, on a
* switch type basis, among various switch types.  It is an alternate
* to Ux_BadMsg.
*
*/
PRIVATE void near Ux_BadMsgAlt()
{
    switch (SwitchType) {
#ifdef BRI_DMS100
	case BRI_DMS100_STYPE:
	break;			/* do nothing */
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

/*******************
*  Ux_ChngStateReq *
********************
*
* State = Any State:  Event = Call Control Change State Req
*
* This message is provided so that Call Control, upon analyzing a
* message and discovering that it should be rejected due to flaws in
* its information elements, may redirect Protocol Control to assume
* the correct state (typically its previous state) as a partial response
* to the message rejection process.  The Protocol Control state in
* which the flawed message was processed was the SourceState in the
* primitive in which the flawed message was resent to Call Control.
* The current protocol state may no longer agree with this value since
* the superficially appropriate action for Protocol Control to perform
* when processing the flawed message may have been to execute a state
* change (e.g CALL_PROCEEDING received in State 1 with missing mandatory
* CHANNEL_ID_IE).
*
* Typically Call Control may also generate a CC_STATUS_REQ message which
* Protocol Control will convert to a STATUS message as an independent
* action through the function Ux_StatusReq.
*
*/
PRIVATE void near Ux_ChngStateReq()
{
    /*
     * The correct state for the interface is known to the Call Control and
     * is returned in the Info.
     */
    if (NLCB->CallRefLen == 2)	/* 2 byte CR */
	NLCB->State = (L3_STATE) L3_PktIn->Info[5];
    else			/* 1 byte CR */
	NLCB->State = (L3_STATE) L3_PktIn->Info[4];
}

/*************
* Ux_CongCon *
**************
*
* State = 4,7,8 and 9;  Event = Peer Congestion Control Message
*
*/
PRIVATE void near Ux_CongCon()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(CC_CONG_CON_IND, CC_ID, FALSE, FALSE);
	break;
    }
}

/****************
* Ux_CongConReq *
*****************
*
* State = 4,7,8,9,10;  Event = Call Control Congestion Control Message
*
*/
PRIVATE void near Ux_CongConReq()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(CONG_CON, L2_ID, FALSE, FALSE);
	break;
    }
}

/********************
*  Ux_ConnectAckReq *
*********************
*
* State = 3,4 for BRI_1TR6_STYPE only
* === new routine added
*
*/
PRIVATE void near Ux_ConnectAckReq()
{
    switch (SwitchType) {
#ifdef BRI_1TR6
	case BRI_1TR6_STYPE:
	L3_Mail(CONNECT_ACK, L2_ID, FALSE, FALSE);
	NLCB->State = CALL_ACTIVE;	/* State 10 */
	break;
#endif
      default:
	break;
    }
}

/*****************
*  Ux_Disconnect *
******************
*
* State = 2,3,4,6,7,8,9,10;  Event = Peer Disconnect Message
*
*/
PRIVATE void near Ux_Disconnect()
{
    L3_StopAllTimers();
    L3_Mail(CC_DISCONNECT_IND, CC_ID, FALSE, FALSE);
    NLCB->State = DISCONNECT_IND;	/* State 12 */
}

/* Written by rbk === */
PRIVATE void near
    U8_DiscReq()
{
    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
	U8_ReleaseReq();
    else
	Ux_DiscReq();
}

/**************
*  Ux_DiscReq *
***************
*
* State = 2,3,4,7,8,9,10,12,20;  Event = Call Control Disconnect Request
*
*/
PRIVATE void near Ux_DiscReq()
{
    L3_Mail(DISCONNECT, L2_ID, FALSE, FALSE);
    /*
     * Have to save off the Cause value for NET3, the Release has to send out
     * the same Cause value.
     */
    if (SwitchType == BRI_NET3_STYPE ||
        SwitchType == PRI_NET5_STYPE ||
        SwitchType == BRI_TS013_STYPE ||
        SwitchType == PRI_TS014_STYPE ||
        SwitchType == BRI_5ESS_STYPE ||
        SwitchType == BRI_DMS100_STYPE || SwitchType == BRI_NI1_STYPE) {
	NLCB->PreviousCause = L3_RejectCause((BYTE *) & Trashcan);
 }

    LIF_StartTimer(NLCB->Timer_T305);
    NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
}

/***************
* Ux_DLRelConf *
****************
*
* State = Any;  Event = Peer Release Confirm
*
*/
PRIVATE void near Ux_DLRelConf()
{
    NLCB_ID nlcb;
    NLCB_ID next_nlcb;
    BYTE remove_ces;		/* CES of data link which released */

    nlcb = NLCB;		/* Save the incoming NLCB. */
    remove_ces = nlcb->Ces;

    /*
     * Inform Call Control of the breakdown of data link establishment.
     */
    L3_Mail(NL_REL_CONF, CC_ID, TRUE, FALSE);

    /*
     * Locate every active nlcb with this CES and remove it.
     */
    NLCB = NLCB_Head[L3_DSL_Id];
    while (NLCB) {
	next_nlcb = NLCB->Next;
	if (NLCB->Ces == remove_ces)
	    L3_Free_NLCB(NLCB);
	NLCB = next_nlcb;
    }

    /*
     * As NLCB is now a NULL pointer, it is essential to restore it to some
     * real (though freed) nlcb to allow continuation processing in L3_Go.
     */
    NLCB = nlcb;
}

/**************
* Ux_DLRelInd *
***************
*
* State = Any;  Event = Peer Release Indication
*
*/
PRIVATE void near Ux_DLRelInd()
{
    NLCB_ID nlcb;
    NLCB_ID next_nlcb;
    BYTE remove_ces;		/* CES of data link which released */

    /*
     * Inform Call Control of the breakdown of data link establishment.
     */
    L3_Mail(NL_REL_IND, CC_ID, TRUE, FALSE);

    /* ===??? not sure this if test is valid */
#ifdef 0
    if (SwitchType != BRI_DMS100_STYPE) {	/* NIS S208-5 Sec. 6.1.5.9 */
#endif
	nlcb = NLCB;		/* Save the incoming NLCB. */
	remove_ces = nlcb->Ces;
	/*
	 * Locate every active nlcb with this CES and remove it.
	 */
	NLCB = NLCB_Head[L3_DSL_Id];
	while (NLCB) {
	    next_nlcb = NLCB->Next;
	    if (NLCB->Ces == remove_ces)
		if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
		    if (NLCB->State == CALL_ACTIVE) {	/* State 10 */
			/*
			 * The correct cause value here is in some doubt.
			 */
			NLCB->Cause = PROTOCOL_ERROR;	/* Cause 111 */
			L3_Mail(STATUS, L2_ID, TRUE, FALSE);
			/*
			 * T309 (optional) is not implemented.
			 */
		    } else {
			L3_Free_NLCB(NLCB);
		    }
		} else {
		    L3_Free_NLCB(NLCB);
		}
	    NLCB = next_nlcb;
	}
	/*
	 * As NLCB is now a NULL pointer, it is essential to restore it to
	 * some real (though freed) nlcb to allow continuation processing in
	 * L3_Go.
	 */
	NLCB = nlcb;
#ifdef 0			/* ===??? */
    }
#endif				/* ===??? */
}

/***********
* Ux_Error *
************
*
* State = Any state;  Event = Many events requiring error indication to CC.
*
#ifdef PRI_1TR6
* Also, for PRI_1TR6, this function is used extensively from any states for
* cases requiring an error indication to Call Control and otherwise no
* error action.
#endif
#ifdef BRI_1TR6
* Also, for BRI_1TR6, this function is used extensively from any states for
* cases requiring an error indication to Call Control and otherwise no
* error action.
#endif
*
*/
PRIVATE void near Ux_Error()
{
    L3_Mail(CC_ERROR_IND, CC_ID, TRUE, FALSE);
}

/***************
* Ux_FacAckReq *
****************
*
* State = 4,6,7,8,9,10; Event = Call Control Facility Ack Request
*
*/
PRIVATE void near Ux_FacAckReq()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Error();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Error();
	break;
#endif
      default:
	L3_Mail(FACILITY_ACK, L2_ID, FALSE, FALSE);
	break;
    }
}

/***************
* Ux_FacRejReq *
****************
*
* State = 4,6,7,8,9,10; Event = Call Control Facility Rej Request
*
*/
PRIVATE void near Ux_FacRejReq()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Error();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Error();
	break;
#endif
      default:
	L3_Mail(FACILITY_REJ, L2_ID, FALSE, FALSE);
	break;
    }
}

/*****************
* Ux_FacilityAck *
******************
*
* State = 6, 9, 10;  Event = Facility Acknowledge from peer
*
*/
PRIVATE void near Ux_FacilityAck()
{
    switch (SwitchType) {
#ifdef PRI_DMS100
	case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;
#endif
      default:
	/*
	 * send FACILITY_ACK to CC
	 */
	L3_Mail(CC_FACILITY_ACK, CC_ID, FALSE, FALSE);
	break;
    }
}

/*****************
* Ux_FacilityRej *
******************
*
* State = 6, 9, 10;  Event = Facility Reject from peer
*
*/
PRIVATE void near Ux_FacilityRej()
{
    switch (SwitchType) {
#ifdef PRI_DMS100
	case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;
#endif
      default:
	/*
	 * send FACILITY_REJ to CC
	 */
	L3_Mail(CC_FACILITY_REJ, CC_ID, FALSE, FALSE);
	break;
    }
}

/*****************
* Ux_FacilityReq *
******************
*
* State = 6, 9, 10;  Event = Facility Request from Call Control
*
*/
PRIVATE void near Ux_FacilityReq()
{
    switch (SwitchType) {
#ifdef PRI_DMS100
	case PRI_DMS100_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	/*
	 * send FACILITY msg to L2
	 */
	L3_Mail(FACILITY, L2_ID, FALSE, FALSE);
	break;
    }
}

/**********
* Ux_Info *
***********
*
* State = 2, 3, 4, 10, 15, 25; Event = L2 Info Indication
*
#ifdef PRI_1TR6
* In the PRI_1TR6 case, if the INFO indicates to call control a re-direction
* of the Protocol Control state to T3, call control may direct the state
* advance via a CC_CHNG_STATE_REQ.
#endif
#ifdef BRI_1TR6
* In the BRI_1TR6 case, if the INFO indicates to call control a re-direction
* of the Protocol Control state to T3, call control may direct the state
* advance via a CC_CHNG_STATE_REQ.
#endif
*
*/
PRIVATE void near Ux_Info()
{
    switch (SwitchType) {
#ifdef PRI_DMS100
	case PRI_DMS100_STYPE:
	Ux_Undefined();
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
	Ux_BadMsg();
	break;
#endif
      default:
	L3_Mail(CC_INFO_IND, CC_ID, FALSE, FALSE);	/* send it to CC */
	break;
    }
}

/*************
* Ux_InfoReq *
**************
*
* State = 2,3,4,7,8,9,10,11,12  Event = Call Control Info Request
*
*/
PRIVATE void near Ux_InfoReq()
{
    switch (SwitchType) {
#ifdef BRI_5ESS			/* === */
	case BRI_5ESS_STYPE:
	L3_Mail(INFO, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_DMS100		/* === */
      case BRI_DMS100_STYPE:
	L3_Mail(INFO, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	L3_Mail(INFO, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef KDD_NTT
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	L3_Mail(INFO, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef CCITT
      case CCITT_STYPE:
	L3_Mail(INFO, L2_ID, FALSE, FALSE);
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_Mail(INFO_1TR6, L2_ID, FALSE, FALSE);
	break;
#endif
      default:
	Ux_BadMsg();
	break;
    }
}

#ifdef BRI_5ESS
/*************
* Ux_ManInfo *
**************
*
* State = any; Event = Management Information from L2
*
*/
PRIVATE void near Ux_ManInfo()
{
    /*
     * LIF_SendPkt (L2P_LAPD, L3_ID, 0, ME_ID, MNL_MIM_IND, L3_DSL_Id,
     * Q931_SAPI, VOICE_DEFAULT_CES, 0, 0, Q931_DSL_CHAN_ID, TRUE,
     * L3_PktIn,===
     */

    LIF_SendPkt(L2P_LAPD, L3_ID, 0, ME_ID, MNL_MIM_IND, L3_DSL_Id,
		Q931_SAPI, NLCB->Ces, 0, 0, Q931_DSL_CHAN_ID, TRUE, L3_PktIn,
		NOW, REGULAR_MAIL);
    L3_AvailPktIn = FALSE;	/* The L3_PktIn has been remailed */
    L3_PktIn = NULL;
}

/************
* Ux_ManReq *
*************
*
* State = any; Event = Management Information from ME
*
*/
PRIVATE void near Ux_ManReq()
{
    LIF_SendPkt(L2P_LAPD, L3_ID, 0, L2_ID, DL_DATA_REQ, L3_DSL_Id,
    /* Q931_SAPI, VOICE_DEFAULT_CES, 0, 0, Q931_DSL_CHAN_ID, TRUE, L3_PktIn, */
		Q931_SAPI, NLCB->Ces, 0, 0, Q931_DSL_CHAN_ID, TRUE, L3_PktIn,
		NOW, REGULAR_MAIL);
    L3_AvailPktIn = FALSE;	/* The L3_PktIn has been remailed */
    L3_PktIn = NULL;
}
#endif				/* BRI_5ESS */

/**************
* Ux_NLRelReq *
***************
*
* State = Any;  Event = Call Control Data Link Release Request
*
*/
PRIVATE void near Ux_NLRelReq()
{
    /*
     * Inform Layer 2 to breakdown the data link.
     */
    L3_Mail(DL_REL_REQ, L2_ID, TRUE, FALSE);
}

/*********
* Ux_NOP *
**********
*
* State = 98, 99;  Event = Unknown
*
*/
PRIVATE void near Ux_NOP()
{
    /*
     * Do nothing
     */
}

/***************
*  Ux_Progress *
****************
*
* State = 2, 10, (and 30-10, 31-10, 32-10, 33-10, 36-10 ifdef ATT_TYPE_A)
* Event = Peer Progress
*
*/
PRIVATE void near Ux_Progress()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	Ux_Undefined();
	break;
#endif
	/* #ifdef BRI_1TR6	=== */
	/* case BRI_1TR6_STYPE: === */
	/* Ux_Undefined();	=== */
	/* break;			=== */
	/* #endif === */
      default:
	L3_Mail(CC_PROGRESS_IND, CC_ID, FALSE, FALSE);
	break;
    }
}

/****************
* Ux_RelCompReq *
*****************
*
* State = Any active call setup state.
* Event = Call Control RelCompReq.
*
* This function is specific to the Alcatel VN2 ISDN implementation for
* error recovery.  As such, other users of this code for other SwitchTypes
* should not be reaching this function.
*
*/
PRIVATE void near Ux_RelCompReq()
{
    switch (SwitchType) {
#ifdef VN2
	case VN2_STYPE:
	case VN3_STYPE:
	L3_Mail(RELEASE_COMP, L2_ID, FALSE, FALSE);
	L3_CallRelease();
	break;
#endif
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	/*
         * Added to keep from sending a Release Complete
         */
	if (NLCB->State == NULL_STATE)
	    Ux_BadMsg();
	else
	    L3_Mail(RELEASE_COMP, L2_ID, FALSE, FALSE);
	L3_CallRelease();
	break;

      case BRI_NET3_STYPE:
        /*
         * Make all the net3 switchtypes act consistenly.
         */
        if (NLCB->State == NULL_STATE)
            Ux_BadMsg();
        else
            L3_Mail(RELEASE_COMP, L2_ID, FALSE, FALSE);
        L3_CallRelease();
        break;

      default:
	Ux_BadMsg();
	break;
    }
}

/**************
*  Ux_Release *
***************
*
* State = 2,3,4,6,7,8,9,10,15,25;  Event = Peer Release Message
*                                          (with Cause)
*/
PRIVATE void near Ux_Release()
{
    L3_Mail(CC_RELEASE_IND, CC_ID, FALSE, FALSE);
    switch (SwitchType) {
      case VN2_STYPE:		/* === */
      case VN3_STYPE:
	/* The Release Complete will be sent from Call Control */
	break;			/* === */

      case BRI_DMS100_STYPE:
        /*
         * Cannot do this here, causing a diagnostic to go out for all
         * of the Release Complete messages.
         *
         * We have a special problem here because the protocol demands CAUSE
         * information element checking which cannot be performed in Call
         * Control.  We must do it here in Layer 3 since we must progress to
         * State 0 directly.
         */
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	/*
	 * Whenever CC_RELEASE_IND is sent to Call Control we expect A
	 * CC_REL_COMP_REQ (RELEASE_CONF) response in State 19 before
	 * RELEASE_COMP is issued to the network to conform with the SDLs.
	 */
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      case BRI_TS013_STYPE:
	/*
	 * wait here for Call Control to validate the Cause and Display
	 * information. It will reply with a CC_REL_COMP_REQ message, then
	 * the Release Complete can be sent.
	 */
	break;

      default:
	L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
    }
}

/******************
*  Ux_ReleaseComp *
*******************
*
* State = any state except 0, 1 and 19;  Event = Peer Release Complete
*
*/
PRIVATE void near Ux_ReleaseComp()
{
    switch (SwitchType) {
#ifdef BRI_5ESS
	case BRI_5ESS_STYPE:
	LIF_StopTimer(NLCB->Timer_T303);
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_NI1			/* === */
      case BRI_NI1_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
      case PRI_DMS100_STYPE:
      case PRI_NET5_STYPE:
      case PRI_TS014_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_NET3			/* === */
      case BRI_NET3_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef BRI_TS013
      case BRI_TS013_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
      case PRI_NTT_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_CallRelease();	/* State 0 */
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_StopAllTimers();
	L3_Mail(CC_RELEASE_CONF, CC_ID, FALSE, FALSE);
	L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      default:
	L3_Mail(STATUS_ENQ, L2_ID, TRUE, FALSE);	/* bad message */
	break;
    }
}

/*****************
*  Ux_ReleaseReq *
******************
*
* State = 1,2,3,4,6,7;  Event = Call Control Release Request
*
*/
PRIVATE void near Ux_ReleaseReq()
{
    switch (SwitchType) {
#ifdef PRI_1TR6
	case PRI_1TR6_STYPE:
	L3_StopAllTimers();
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	L3_StopAllTimers();
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;
#endif
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	L3_Mail(RELEASE, L2_ID, FALSE, FALSE);
	L3_SavePkt(&NLCB->Release, L3_PktOut);	/* save RELEASE */
	LIF_StartTimer(NLCB->Timer_T308);
	NLCB->State = RELEASE_REQUEST;	/* State 19 */
	break;

      default:
	Ux_BadMsg();
	break;
    }
}
#ifdef BRI_1TR6
/************
*  Ux_Setup *
*************
*
* State = 6, 7, 8;  Event = 1TR6  Setup Retransmission
* State remains unchanged.
*
*/
PRIVATE void near Ux_Setup()
{
    L3_Mail(CC_SETUP_IND, CC_ID, FALSE, FALSE);
}
#endif

#ifdef PRI_1TR6
/************
*  Ux_Setup *
*************
*
* State = 6, 7, 8;  Event = 1TR6  Setup Retransmission
* State remains unchanged.
*
*/
PRIVATE void near Ux_Setup()
{
    L3_Mail(CC_SETUP_IND, CC_ID, FALSE, FALSE);
}
#endif
/*************
*  Ux_Status *
**************
*
* State = Any State, except -3,-2,-1,0,19:  Event = Peer Status
*
*/
PRIVATE void near Ux_Status()
{
    L3_STATE state;
    BYTE *cause_ie;		/* Pointer to cause info element */
    BYTE *unpacked_ie;		/* Unpacked version of cause info element */
    BYTE cause;			/* Cause value */
    int struct_err;		/* === */
#ifdef VN2
    BYTE PBstate;
#endif

    switch (SwitchType) {
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* Ignore undefined message - see
				 * Ux_StatusP1TR6 */
#endif				/* PRI_1TR6 */
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* Ignore undefined message - see
				 * Ux_StatusB1TR6 */
#endif				/* BRI_1TR6 */
      case BRI_5ESS_STYPE:
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	/*
	 * Can't be the same as default because a check for the Cause field
	 * must be done before the check for the Call State. A test for a
	 * missing cause info element or an invalid cause value will
	 * otherwise fail.
	 */
	if ((cause_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen,
				    CAUSE_IE, CODESET_0, 1)) == NULL) {
	    NLCB->Cause = MANDATORY_IE_MISSING;	/* Cause 96 */
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    goto bailout;
	} else {
            /* Cause element is present.  Check the cause
             * value.
             */
	    unpacked_ie = CCIE_ExpandCause_IE(cause_ie, &struct_err);
	    cause = unpacked_ie[2];
	    if (!CCIE_CauseValid(cause)) {
		NLCB->Cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
		L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		goto bailout;
	    }
	}
	/*
	 * Recover the peer state from the incoming message.
	 */
	state = L3_GetState(L3_PktIn->Info, L3_PktIn->InfoLen, &NLCB->Cause);

	/*
	 * Test if L3_GetState was unhappy, that is, a problem was found with
	 * the CALL_STATE information element.
	 */
	if (NLCB->Cause) {
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);	/* Send STATUS */
	    goto bailout;	/* Do no more for now. */
	}
	/*
	 * Test if the peer reports State 0.
	 */
	else if (state == NULL_STATE) {
	    NLCB->State = NULL_STATE;
	    L3_Mail(CC_STATUS_IND, CC_ID, FALSE, FALSE);
	    L3_CallRelease();	/* State 0 */
	    goto bailout;
	}
	/*
	 * Test if the peer state and the NLCB state fail to match.
	 */
	else if (state != L3_EXT_STATE(NLCB->State))
	    /* mustn't goto bailout === */
	{
	    NLCB->Cause = WRONG_MSG_FOR_STATE;	/* Cause 101 */
	    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
	    LIF_StartTimer(NLCB->Timer_T308);
	    NLCB->State = RELEASE_REQUEST;	/* State 19 */
	    goto bailout;
	}
	break;

      default:
	/*
	 * Recover the peer state from the incoming message.
	 */
	state = L3_GetState(L3_PktIn->Info, L3_PktIn->InfoLen, &NLCB->Cause);

	/*
	 * Test if L3_GetState was unhappy, that is, a problem was found with
	 * the CALL_STATE information element.
	 */
	if (NLCB->Cause) {
	    switch (SwitchType) {
#ifdef PRI_DMS100
	      case PRI_DMS100_STYPE:
		Diagnostic[0] = CALL_STATE_IE;
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		LIF_StartTimer(NLCB->Timer_T308);
		NLCB->State = RELEASE_REQUEST;	/* State 19 */
		break;
#endif				/* PRI_DMS100 */
#ifdef BRI_DMS100
	      case BRI_DMS100_STYPE:
		Diagnostic[0] = CALL_STATE_IE;
		Diagnostic[1] = 0;
		/*
		 * Check for state 1 & 6, re: NIS S208-5, 6.1.3.2a
		 */
		if ((NLCB->State == CALL_INIT) ||
		    (NLCB->State == CALL_PRESENT)) {
		    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
		    NLCB->Cause = PROTOCOL_ERROR;	/* Cause 111 */
		    L3_Mail(CC_RELEASE_IND, CC_ID, TRUE, FALSE);
		    L3_CallRelease();	/* State 0 */
		} else {
		    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		    LIF_StartTimer(NLCB->Timer_T308);
		    NLCB->State = RELEASE_REQUEST;	/* State 19 */
		}
		break;
#endif				/* BRI_DMS100 */
#ifdef VN2
	      case VN2_STYPE:
	      case VN3_STYPE:
		/*
		 * For VN2, if CALL_STATE_IE is absent or not CCITT state,
		 * send RELEASE_COMP w/cause 96 or 100 and diag =
		 * CALL_STATE_IE. Diagnostic is not added because this is not
		 * a STATUS msg.
		 */
		/*
		 * if call state missiing send a RC because mandatory, else
		 * ignore
		 */
		if (NLCB->Cause == MANDATORY_IE_MISSING) {
		    if (NLCB->State != DISCONNECT_REQUEST) {
			/*
			 * 4000 homologation : if in state 11, ignore cause
			 * 96
			 */
			PBstate = NLCB->Cause;
			L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, TRUE, FALSE);	/* === */
			NLCB->Cause = PBstate;
			L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
			L3_CallRelease();
		    }
		}
		/*
		 * ignore invalid state, for cancun L3_Mail (RELEASE, L2_ID,
		 * TRUE, FALSE); L3_SavePkt (&NLCB->Release, L3_PktOut); *
		 * Save REL * LIF_StartTimer(NLCB->Timer_T308); NLCB->State =
		 * RELEASE_REQUEST;         * State 19 *
		 */

		break;
#endif				/* VN2 */
#ifdef KDD_NTT
	      case KDD_STYPE:
	      case NTT_STYPE:
	      case PRI_NTT_STYPE:
		Diagnostic[0] = CALL_STATE_IE;
		L3_Mail(STATUS, L2_ID, TRUE, FALSE);	/* Send STATUS */
		break;
#endif				/* KDD_NTT */
	      default:
		L3_Mail(STATUS, L2_ID, TRUE, FALSE);	/* Send STATUS */
		break;
	    }
	    goto bailout;	/* Do no more for now. */
	}
	/* this used to be in an #ifndef VN2 === */
	/*
	 * Test if the peer reports State 0.
	 */
	else if (state == NULL_STATE) {
            /*
             * Make sure a cause value is here, fails conformance testing 
             */
            cause_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen, CAUSE_IE,
                                   CODESET_0, 1);
            /*
             * Don't think that this is correct for all switches, have to check the
             * Cause first then the State.
             */
            if (((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) &&
                 (cause_ie == NULL)) {
                NLCB->Cause = MANDATORY_IE_MISSING;     /* Cause 96 */
                L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
                L3_SavePkt(&NLCB->Release, L3_PktOut);  /* Save REL */
                LIF_StartTimer(NLCB->Timer_T308);
                NLCB->State = RELEASE_REQUEST;  /* State 19 */
            } else {
                /*
                 * All other switches will ignore
                 */
	        NLCB->State = NULL_STATE;
	        L3_Mail(CC_STATUS_IND, CC_ID, FALSE, FALSE);
	        L3_CallRelease();	/* State 0 */
            }
	    goto bailout;
	}
	/*
	 * Test if the peer state and the NLCB state fail to match.
	 */
	else if (state != L3_EXT_STATE(NLCB->State) &&
		 SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
	    /* mustn't goto bailout === */
	{
	    switch (SwitchType) {
	      case PRI_4ESS_STYPE:
	      case PRI_5ESS_STYPE:
	      case PRI_NET5_STYPE:
		/*
		 * This solution is a best match to the AT&T PRI Test Plan
		 * while not violating 41449.
		 */
		if (NLCB->State != DISCONNECT_REQUEST) {
		    L3_StopAllTimers();
		    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
		    L3_Mail(DISCONNECT, L2_ID, TRUE, FALSE);
		    LIF_StartTimer(NLCB->Timer_T305);
		    NLCB->State = DISCONNECT_REQUEST;	/* State 11 */
		}
		break;
#ifdef VN2
	      case VN2_STYPE:
	      case VN3_STYPE:
		break;		/* No verification in VN2 */
#endif				/* VN2 */
#ifdef KDD_NTT
	      case KDD_STYPE:
	      case NTT_STYPE:
	      case PRI_NTT_STYPE:
		NLCB->Cause = WRONG_MSG_FOR_STATE;	/* Cause 101 */
		Diagnostic[0] = STATUS;
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		LIF_StartTimer(NLCB->Timer_T308);
		NLCB->State = RELEASE_REQUEST;	/* State 19 */
		break;
#endif				/* KDD_NTT */
	      case BRI_TS013_STYPE:
	      case PRI_TS014_STYPE:
		NLCB->Cause = WRONG_MSG_FOR_STATE;	/* Cause 101 */
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		LIF_StartTimer(NLCB->Timer_T308);
		NLCB->State = RELEASE_REQUEST;	/* State 19 */
		break;

	      case BRI_NI1_STYPE:
	      case BRI_DMS100_STYPE:
		NLCB->Cause = MANDATORY_IE_MISSING;     /* Cause 96 */
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		LIF_StartTimer(NLCB->Timer_T308);
		NLCB->State = RELEASE_REQUEST;	/* State 19 */
		break;

	      default:
		NLCB->Cause = PROTOCOL_ERROR;	/* Cause 111 */
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		LIF_StartTimer(NLCB->Timer_T308);
		NLCB->State = RELEASE_REQUEST;	/* State 19 */
		break;
	    }
	    goto bailout;
	}
	/*
	 * Test for a missing cause info element or an invalid cause value.
	 */
	if ((cause_ie = CCIE_Get_IE(L3_PktIn->Info, L3_PktIn->InfoLen,
				    CAUSE_IE, CODESET_0, 1)) == NULL) {
	    NLCB->Cause = MANDATORY_IE_MISSING;	/* Cause 96 */
	    switch (SwitchType) {
#ifdef PRI_DMS100
	      case PRI_DMS100_STYPE:
		Diagnostic[0] = CAUSE_IE;
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		LIF_StartTimer(NLCB->Timer_T308);
		NLCB->State = RELEASE_REQUEST;	/* State 19 */
		break;
#endif				/* PRI_DMS100 */
#ifdef BRI_DMS100
	      case BRI_DMS100_STYPE:
		Diagnostic[0] = CAUSE_IE;
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		LIF_StartTimer(NLCB->Timer_T308);
		NLCB->State = RELEASE_REQUEST;	/* State 19 */
		break;
#endif				/* BRI_DMS100 */
#ifdef VN2
	      case VN2_STYPE:
	      case VN3_STYPE:
		/*
		 * if cause is missing, then send RC because mandatory
		 */

		L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, TRUE, FALSE);	/* === */
		NLCB->Cause = MANDATORY_IE_MISSING;	/* Cause 96 */
		L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
		L3_CallRelease();
		break;
#endif				/* VN2 */
#ifdef KDD_NTT
	      case KDD_STYPE:
	      case NTT_STYPE:
	      case PRI_NTT_STYPE:
		Diagnostic[0] = CAUSE_IE;
		L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		break;
#endif				/* KDD_NTT */
	      default:
		L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		break;
	    }
	} else {		/* Cause element is present.  Check the cause
				 * value. */
	    unpacked_ie = CCIE_ExpandCause_IE(cause_ie, &struct_err);
	    cause = unpacked_ie[2];
	    if (!CCIE_CauseValid(cause)) {
		NLCB->Cause = INVALID_ELEM_CONTENTS;	/* Cause 100 */
		switch (SwitchType) {
#ifdef PRI_DMS100
		  case PRI_DMS100_STYPE:
		    Diagnostic[0] = CAUSE_IE;
		    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		    LIF_StartTimer(NLCB->Timer_T308);
		    NLCB->State = RELEASE_REQUEST;	/* State 19 */
		    break;
#endif				/* PRI_DMS100 */
#ifdef BRI_DMS100
		  case BRI_DMS100_STYPE:
		    Diagnostic[0] = CAUSE_IE;
		    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		    L3_SavePkt(&NLCB->Release, L3_PktOut);	/* Save REL */
		    LIF_StartTimer(NLCB->Timer_T308);
		    NLCB->State = RELEASE_REQUEST;	/* State 19 */
		    break;
#endif				/* BRI_DMS100 */
#ifdef VN2
		  case VN2_STYPE:
		  case VN3_STYPE:
		    /*
		     * For VN3, if cause is invalid, just ignore
		     */
		    break;
#endif				/* VN2 */
#ifdef KDD_NTT
		  case KDD_STYPE:
		  case NTT_STYPE:
		  case PRI_NTT_STYPE:
		    Diagnostic[0] = CAUSE_IE;
		    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		    break;
#endif				/* KDD_NTT */
		  default:
		    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		    break;
		}
	    } else {
		switch (SwitchType) {
#ifdef VN2
		  case VN2_STYPE:
		  case VN3_STYPE:
		    if (cause == MANDATORY_IE_MISSING) {	/* Cause 96 */
			/*
			 * 4000 homologation : if in state 11, ignore cause
			 * 96
			 */
			if (NLCB->State != DISCONNECT_REQUEST) {
			    /*
			     * Tear down the call per ST/LAA/RSM/ 133,
			     * Section 5.1.8.9 (2).
			     */
			    L3_Mail(CC_RELEASE_IND_QUIET, CC_ID, TRUE, FALSE);	/* === */
			    NLCB->Cause = MANDATORY_IE_MISSING;	/* Cause 111 */
			    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
			    L3_CallRelease();
			}
		    }
		    break;
#endif				/* VN2 */
#ifdef KDD_NTT
		  case KDD_STYPE:
		  case NTT_STYPE:
		  case PRI_NTT_STYPE:
		    if ((cause == MANDATORY_IE_MISSING) ||	/* Cause 96 */
			(cause == NONEXISTENT_MSG) ||	/* Cause 97 */
			(cause == BAD_INFO_ELEM) ||	/* Cause 99 */
			(cause == INVALID_ELEM_CONTENTS)) {	/* Cause 100 */
			NLCB->Cause = cause;
			Diagnostic[0] = unpacked_ie[3];
			L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
			L3_SavePkt(&NLCB->Release, L3_PktOut);
			LIF_StartTimer(NLCB->Timer_T308);
			NLCB->State = RELEASE_REQUEST;	/* State 19 */
		    }
		    break;
#endif				/* KDD_NTT */
		  case PRI_4ESS_STYPE:
		  case PRI_5ESS_STYPE:
		  case PRI_NET5_STYPE:
		    /*
		     * This message may be an indication from AT&T that
		     * MA-UUI was not passed end to end, which may be of
		     * concern to the application.
		     */
		    if (cause == ACCESS_INFO_DISCARDED)
			L3_Mail(CC_STATUS_IND, CC_ID, FALSE, FALSE);
		    break;

		  default:
		    break;
		}
	    }
	    /*
	     * else...Do no more.  Everything checks out......Whew!
	     */
	}
	break;			/* End outermost default case */
    }
bailout:
    return;
}

#ifdef BRI_1TR6
/*****************
* Ux_StatusB1TR6 *
******************
*
* State = 10, 11, 12, 14, 15 and 19;  Event = Peer Status for BRI_1TR6
*                                             switch types only.
*/
PRIVATE void near Ux_StatusB1TR6()
{
    L3_Mail(CC_STATUS_IND, CC_ID, FALSE, FALSE);
}

#endif				/* BRI_1TR6 */
#ifdef PRI_1TR6
/*****************
* Ux_StatusP1TR6 *
******************
*
* State = 10, 11, 12 and 19;  Event = Peer Status for PRI_1TR6
*                                     switch types only.
*
*/
PRIVATE void near Ux_StatusP1TR6()
{
    L3_Mail(CC_STATUS_IND, CC_ID, FALSE, FALSE);
}

#endif				/* PRI_1TR6 */
/****************
*  Ux_StatusEnq *
*****************
*
* State = Any State, except 0, 98, 99:  Event = Peer Status Enquiry
*
*/
PRIVATE void near Ux_StatusEnq()
{
    int cause;

    switch (SwitchType) {
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;			/* Ignore */
#endif
      case BRI_TS013_STYPE:
      case PRI_TS014_STYPE:
	NLCB->Cause = RESP_TO_STAT_ENQ;
	L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	/*
	 * Have to check to see if the display is valid for Australia.
	 */
	cause = L3_VerifyDisplay();
	if (cause != NOERR) {
	    NLCB->Cause = cause;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	}
	break;

      default:
	NLCB->Cause = RESP_TO_STAT_ENQ;
	L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	break;
    }
}

/****************
*  Ux_StatusReq *
*****************
*
* State = Any State:  Event = Call Control Status Req
*
* Relay a Call Control-generated CC_STATUS_REQ message as a STATUS message.
* Rebuild the information elements locally so that the NLCB state is
* recovered.
*/
PRIVATE void near Ux_StatusReq()
{
    NLCB->Cause = L3_GetCause(L3_PktIn);
    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
}

/****************
*  Ux_Undefined *
*****************
*
*   State = Any except 98,99,0;  Event = Unrecognized Message
*
*/
PRIVATE void near Ux_Undefined()
{
    if (L3_MsgSource == L2_ID) {
	switch (SwitchType) {
#ifdef BRI_DMS100
	    case BRI_DMS100_STYPE:
	    case BRI_NI1_STYPE:	/* === */
	    NLCB->Cause = NONEXISTENT_MSG;	/* Cause 97 */
	    Diagnostic[0] = NLCB->Event;
	    Diagnostic[1] = 0;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
          case BRI_5ESS_STYPE:
            /*
             * STATUS option provided in AT&T 5D5-900-311 Para. 3.5.2.
             */
            NLCB->Cause = WRONG_MESSAGE;        /* Cause 98 */
            Diagnostic[0] = NLCB->Event;
            L3_Mail(STATUS, L2_ID, TRUE, FALSE);
            break;
#ifdef PRI_DMS100
	  case PRI_DMS100_STYPE:
	    NLCB->Cause = NONEXISTENT_MSG;	/* Cause 97 */
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
	  case PRI_4ESS_STYPE:
	  case PRI_5ESS_STYPE:
	  case PRI_NET5_STYPE:
	    NLCB->Cause = NONEXISTENT_MSG;	/* Cause 97 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#ifdef VN2
	  case VN2_STYPE:
	  case VN3_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    Diagnostic[1] = 0;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef KDD_NTT
	  case KDD_STYPE:
	  case NTT_STYPE:
	  case PRI_NTT_STYPE:
	    NLCB->Cause = NONEXISTENT_MSG;	/* Cause 97 */
	    Diagnostic[0] = NLCB->Event;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef PRI_1TR6
	  case PRI_1TR6_STYPE:
	    break;
#endif
#ifdef BRI_1TR6
	  case BRI_1TR6_STYPE:
	    break;
#endif
#ifdef BRI_NET3			/* === */
	  case BRI_NET3_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    Diagnostic[1] = 0;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
#ifdef BRI_TS013
	  case BRI_TS013_STYPE:
	  case PRI_TS014_STYPE:
	    NLCB->Cause = WRONG_MESSAGE;	/* Cause 98 */
	    Diagnostic[0] = NLCB->Event;
	    Diagnostic[1] = 0;
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
#endif
	  default:
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
	}
    } else {
#ifdef DISPLAY_LOCAL_ERR
	LIF_Error("l3_sdl_u", "Ux_Undefined", "Bad message received.")
#endif
	    ;
    }
}

/*********************
*  Ux_UnknownCallRef *
**********************
*
* Handle a message to an unrecognized call reference.  For instance,
* if the code has just initialized, and no call reference has ever
* been assigned, but a message (other than SETUP) has arrived
* from the network.
*
* Any State, the state is unknown since the NLCB could not be located.
* A new NLCB was fetched just to handle this event.
*
* Each SwitchType has its own flavor of handling messages to unrecognized
* call references.
*
* In the Telenetworks implementation, a call in State 0 is a non-existant
* call where no NLCB is currently allocated and no static data exists.
* Thus, an unexpected message to a State 0 call is indistinguishable from
* a message to an existing call with an unrecognized call reference.
*
* An idle NLCB was activated to process this event.  In most cases, it is
* released as this function completes.  Some protocols demand the sending
* RELEASE and transition to STATE 19 for unexpected messages.  In these
* cases, the NLCB is retained (for T308 and RELEASE resend eventualities).
*
* Check for STATUS_ENQ (for the outside world it means the system is in
* state 0, even though for us it is state 98 or possibly 99).
*
* Return:  This BOOLEAN function returns TRUE if processing of this message
*          is complete.  The return of FALSE will cause the message to
*          be redirected into the STATE 0 SDLs (currently only for 1TR6).
*/
PUBLIC BOOLEAN Ux_UnknownCallRef()
{
    BYTE cause;
    L3_STATE state;
    BOOLEAN done = TRUE;	/* Initialize output */

    if (detail_debug)
        buginf("\n entering Ux_UnknownCallRef");
    state = NULL_STATE;
    switch (SwitchType) {
      case BRI_5ESS_STYPE:
	switch (NLCB->Event) {
	  case STATUS_ENQ:
	    NLCB->Cause = RESP_TO_STAT_ENQ;	/* Cause #30 */
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
	  case RELEASE_COMP:
	    break;

	  case RELEASE:
	    NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    break;

          /*
           * Added the Status and Info cases for the PBX. Helps
           * prevent endless Layer 3 message exchanges and problems on
           * router reloads, happening on the global call references.
           */
          case STATUS:
          case INFO:
          default:
            NLCB->Cause = INVALID_CALL_REF;    /* Cause #81 */
            if (GLOBAL_CALL_REF(NLCB->CallRef, NLCB->CallRefLen))
	        L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
            else
	        L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	    break;
	}
	break;
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
	switch (NLCB->Event) {
	  case STATUS_ENQ:
	    NLCB->Cause = RESP_TO_STAT_ENQ;	/* Cause #30 */
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
	  case RELEASE_COMP:
	    break;
	  default:
	    if (!L3_DefinedEvent(NLCB->Event)) {
		NLCB->Cause = NONEXISTENT_MSG;	/* Cause 97 */
		if (GLOBAL_CALL_REF(NLCB->CallRef, NLCB->CallRefLen))
		    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		else
		    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    } else {
		NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
		if (GLOBAL_CALL_REF(NLCB->CallRef, NLCB->CallRefLen))
		    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
		else
		    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    }
	    break;
	}
	break;

      case PRI_NET5_STYPE:
	switch (NLCB->Event) {
	  case STATUS_ENQ:
	    NLCB->Cause = INVALID_CALL_REF; /* Cause #81 */ 
            if (GLOBAL_CALL_REF(NLCB->CallRef, NLCB->CallRefLen))
	        L3_Mail(STATUS, L2_ID, TRUE, FALSE);
            else
	        L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	    break;
	  case STATUS:
            /*
             * Recover the peer state from the incoming message.
             */
            state = L3_GetState(L3_PktIn->Info, L3_PktIn->InfoLen, &NLCB->Cause);
            /*
             * Test TC19002. Ignore STATUS messages with global cr.
             */
            if (GLOBAL_CALL_REF(NLCB->CallRef, NLCB->CallRefLen))
                break;
            if (state != NULL_STATE) {
	        NLCB->Cause = WRONG_MSG_FOR_STATE; /* Cause #101 */ 
	        L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
            }
	    break;
            /* 
             * TC10001 A RELEASE_COMPLETE with unknown cr should
             * be ignore.
             * TC11915 A RELEASE_COMPLETE with a global cr should
             * be responded to with a STATUS.
             */
          case RELEASE_COMP:
            if (GLOBAL_CALL_REF(NLCB->CallRef, NLCB->CallRefLen)) {
	        NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
                L3_Mail(STATUS, L2_ID, TRUE, FALSE);
            }
            break;
	  default:
            NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
	    if (GLOBAL_CALL_REF(NLCB->CallRef, NLCB->CallRefLen))
	        L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    else
	        L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    break;
	}
	break;

      case BRI_NI1_STYPE:	/* === */
	switch (NLCB->Event) {
	  case STATUS_ENQ:
	    NLCB->Cause = RESP_TO_STAT_ENQ;	/* Cause #30 */
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    break;
	  case STATUS:
	    /*
	     * This code is commented out for now because the NI1 test suite
	     * states that we should toss any STATUS message whether the
	     * call_state is 0 or not. This differs from Q.931 where the
	     * message would be tossed only if call_state = 0. ===
	     * 
	     * if ( L3_GetState( L3_PktIn->Info, L3_PktIn->InfoLen, &cause ) !=
	     * NULL_STATE ) { NLCB->Cause = WRONG_MSG_FOR_STATE;     Cause
	     * #101 Diagnostic[ 0 ] = STATUS; L3_Mail (RELEASE_COMP, L2_ID,
	     * TRUE, FALSE); }
	     */
	    break;
	  case RELEASE_COMP:
	    break;
	  case RELEASE:
	    NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    break;
	  default:
	    NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);	/* === */
	    break;
	}
	break;
#ifdef BRI_DMS100
      case BRI_DMS100_STYPE:
	switch (NLCB->Event) {
	  case RELEASE_COMP:
	    /*
	     * Per NIS S208-5 Issue 1 Sec. 6.1.5.3.2 (b).
	     */
	    break;
	  case NOTIFY:
	    break;		/* Ignore */
	  case RESTART:
	    NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    break;
	  default:
	    if (L3_DefinedEvent(NLCB->Event)) {
		/*
		 * Per NIS S208-5 Issue 1 Sec. 6.1.5.3.2 (a).
		 */
		NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
		L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    }
	    /*
	     * Else unknown, ignore (NIS S208-5 Issue 1 Sec. 6.1.5.4.1)
	     */
	    break;
	}
	break;
#endif
#ifdef PRI_DMS100
      case PRI_DMS100_STYPE:
	switch (NLCB->Event) {
	  case RELEASE_COMP:
	    break;
	  default:
	    /*
	     * Test if the message is an undefined type.
	     */
	    if (!L3_DefinedEvent(NLCB->Event)) {
		NLCB->Cause = NONEXISTENT_MSG;	/* Cause 97 */
		L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	    } else {
		NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
		L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    }
	    break;
	}
	break;
#endif
#ifdef VN2
      case VN2_STYPE:
      case VN3_STYPE:
	switch (NLCB->Event) {
	  case RELEASE_COMP:
	    break;
	  default:
	    NLCB->Cause = INVALID_CALL_REF;
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    break;
	}
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	switch (NLCB->Event) {
	  case RELEASE:
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    break;
	  case RELEASE_COMP:
	    break;
	  default:
	    /* === added for 1tr6 certif, previously ignored */
	    NLCB->Cause = 0;	/* === */
	    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);	/* === */


	    break;
	}
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	switch (NLCB->Event) {
	  case RELEASE:
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    break;
	  case DISCONNECT:
	    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	    break;
	  case RELEASE_COMP:
	    break;
	  default:
	    if (!L3_DefinedEvent(NLCB->Event)) {
		L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
	    }
	    break;
	}
	break;
#endif
      default:			/* CCITT, KDD_NTT, BRI_TS013, and others */
	if (NLCB->Event == STATUS) {
	    if (L3_GetState(L3_PktIn->Info,
			    L3_PktIn->InfoLen,
			    &cause) != NULL_STATE) {
		NLCB->Cause = WRONG_MSG_FOR_STATE;	/* Cause #101 */
		Diagnostic[0] = STATUS;
		/*
		 * have to do this for Norway, phase 1
		 */
		if ((SwitchType == BRI_NET3_STYPE) &&
		    (SubSwitchType == BRI_NWNET3_STYPE))
		    L3_Mail(RELEASE, L2_ID, TRUE, FALSE);
		else
		    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	    }
	} else if (GLOBAL_CALL_REF(NLCB->CallRef, NLCB->CallRefLen)) {
	    NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
	    L3_Mail(STATUS, L2_ID, TRUE, FALSE);
	} else if (NLCB->Event != RELEASE_COMP) {
	    NLCB->Cause = INVALID_CALL_REF;	/* Cause #81 */
	    L3_Mail(RELEASE_COMP, L2_ID, TRUE, FALSE);
	}
	break;
    }

    if (done)
	L3_CallRelease();

    return (done);
}


/***************
*  Ux_UserInfo *
****************
*
* State = 3,4,6,7,8 and 9;  Event = Peer User Info
*
*/
PRIVATE void near Ux_UserInfo()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	break;
#endif
      default:
	L3_Mail(CC_USER_INFO_IND, CC_ID, FALSE, FALSE);
	break;
    }
}

/******************
*  Ux_UserInfoReq *
*******************
*
* State = 3,4,6,7,8,9 and 10  Event = Call Control User Info Request
*
*/
PRIVATE void near Ux_UserInfoReq()
{
    switch (SwitchType) {
#ifdef KDD_NTT
	case KDD_STYPE:
	case NTT_STYPE:
	case PRI_NTT_STYPE:
	Ux_BadMsg();
	break;
#endif
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
	Ux_Error();
	break;
#endif
#ifdef BRI_1TR6
      case BRI_1TR6_STYPE:
	Ux_Error();
	break;
#endif
      default:
	L3_Mail(USER_INFO, L2_ID, FALSE, FALSE);
	break;
    }
}
