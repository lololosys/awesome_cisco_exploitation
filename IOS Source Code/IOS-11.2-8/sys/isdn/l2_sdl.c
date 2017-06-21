/* $Id: l2_sdl.c,v 3.2.60.2 1996/07/18 21:23:31 snyder Exp $
 * $Source: /release/112/cvs/Xsys/isdn/l2_sdl.c,v $
 *------------------------------------------------------------------
 * l2.c  ISDN LAPD SDLs (user and network sides) 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: l2_sdl.c,v $
 * Revision 3.2.60.2  1996/07/18  21:23:31  snyder
 * CSCdi63402:  get isdn table out of data space and into text space
 * Branch: California_branch
 *              saves 9788 in RAM on run from flash devices
 *
 * Revision 3.2.60.1  1996/04/30  23:07:35  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:41:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:16:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: l2.c
VERSION: 3.21
DESCRIPTION: ISDN LAPD SDLs (user and network sides)

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

This module contains Q.921 CCITT user and network side SDL functions.
Together with the non-SDL layer 2 module (l2.c), it implements layer 2.

ORGANIZATION OF FUNCTIONS

The functions in this module are arranged as follows:

   - SDL support functions alphabetically
   - SDL functions by STATE

*************************************************************************
*************************************************************************
*/


/************************************************************************
*                  E X T E R N A L   R E F E R E N C E S
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
#include    "l2.h"
#include        "../wan/isdn_debug_flags.h"
#include    "gp.h"

/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/

#define     MAX_L2_EVENT    DIM(L2_Events)               /* max events */
#define     MAX_L2_STATE    8                            /* max states */
/************************************************************************
*                              M A C R O S
*************************************************************************
*/

/* Modulo greater than and less than.
*
*  Due to the nature of the modulo system, comparisons between two
*  values (x and y) only makes any sense if it can be said that the maximum
*  difference between these two values (max) will always be some value less
*  than one half of the modulo.  Also, the modulo value must be a power of 2.
*
*  When comparing values, two cases may exist:
*
*  Case 1:        |________________________x______y___|
*  Case 2:        |___y________________________x______|
*
*  In this example, for both cases, y > x and x < y.
*/
#define MOD_GT(x,y,max) \
   ((x)>(y)?((x)-(y)<=(max>>1)?TRUE : FALSE):((y)-(x)>(max>>1)?TRUE : FALSE))
#define MOD_LT(x,y,max) \
   ((x)<(y)?((y)-(x)<=(max>>1)?TRUE : FALSE):((x)-(y)>(max>>1)?TRUE : FALSE))

/************************************************************************
*                   D A T A   A L L O C A T I O N
*************************************************************************
*/

/* Event translation table - map events to an index value
*
*  A sequential search is performed on this list of events with each
*  layer 2 event.  Therefore, this list is ordered based on the expected
*  occurrence of events in an ordinary layer 2 application.  Priority
*  is given to information transfer in the multi-frame established state.
*/
const INT16 L2_Events [] =
{
   /* information events
   */
   I,                      DL_DATA_REQ,            I_FRAME_QUEUED_UP,
#ifdef L2_FLOW_CTRL
   DL_DATA_RESP,
#endif
#ifdef L2_SREJ
   RR,                     SREJ,                   RNR,
#else
   RR,                     REJ,                    RNR,
#endif
   ACK_PENDING,            T200_EXPIRY,            T203_EXPIRY,
   
   /* unnumbered information events
   */
   UI,                     DL_UI_DATA_REQ,         UI_FRAME_QUEUED_UP,
   XID,                    MDL_XID_REQ,
   
   /* establish/release events
   */
   SABME,                  UA,                     DL_EST_REQ,
   DISC,                   DM,                     DL_REL_REQ,
   
   /* tei events
   */
   MDL_ASS_REQ,            MDL_REM_REQ,            DL_ASS_REQ,
   
   /* error events
   */
   MDL_ERR_RESP,           FRMR,                   PERSISTENT_DEACT,
   SET_OWN_REC_BUSY,       CLEAR_OWN_REC_BUSY,     CNTRL_FIELD_ERR,
   INFO_NOT_PERMITTED,     INCORRECT_LENGTH,       I_FRAME_TOO_LONG,
   INFO_REQUIRED,          UNDEFINED
};

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/* L2 SDL support functions
*/
PRIVATE void    near L2_ClearExceptionConditions (void);
PRIVATE void    near L2_EnquiryResponse      (void);
PRIVATE void    near L2_EstablishDataLink    (void);
PRIVATE INT16   near L2_FindEventIndex       (INT16);
PRIVATE void    near L2_InvokeRetransmission (void);
PRIVATE void    near L2_NR_ErrorRecovery     (void);
PRIVATE void    near L2_Queue                (PKT_ID, INT16, BOOLEAN);
PRIVATE void    near L2_TransmitEnquiry      (void);
PRIVATE BYTE    near L2_ValidNR              (void);

/* L2 SDL functions
*/
PRIVATE void    near NOP                     (void);
PRIVATE void    near S78_CLEAR_OWN_REC_BUSY  (void);
PRIVATE void    near S456_DISC               (void);
PRIVATE void    near S78_DISC                (void);
PRIVATE void    near S1_DL_ASS_REQ           (void);
PRIVATE void    near S5_DL_DATA_REQ          (void);
PRIVATE void    near S78_DL_DATA_REQ         (void);
#ifdef L2_FLOW_CTRL
PRIVATE void    near S78_DL_DATA_RESP        (void);
#endif
PRIVATE void    near S12_DL_EST_REQ          (void);
PRIVATE void    near S3_DL_EST_REQ           (void);
PRIVATE void    near S4578_DL_EST_REQ        (void);
PRIVATE void    near S4_DL_REL_REQ           (void);
PRIVATE void    near S5_DL_REL_REQ           (void);
PRIVATE void    near S78_DL_REL_REQ          (void);
PRIVATE void    near S123_DL_UI_DATA_REQ     (void);
PRIVATE void    near S45678_DL_UI_DATA_REQ   (void);
PRIVATE void    near S4_DM                   (void);
PRIVATE void    near S56_DM                  (void);
PRIVATE void    near S7_DM                   (void);
PRIVATE void    near S8_DM                   (void);
PRIVATE void    near S45678_ERR              (void);
PRIVATE void    near S78_FRMR                (void);
PRIVATE void    near S78_I                   (void);
PRIVATE void    near S5_I_FRAME_QUEUED_UP    (void);
PRIVATE void    near S7_I_FRAME_QUEUED_UP    (void);
PRIVATE void    near S8_I_FRAME_QUEUED_UP    (void);
PRIVATE void    near S4_I_REJ_RNR_RR         (void);
PRIVATE void    near S123_MDL_ASS_REQ        (void);
PRIVATE void    near S23_MDL_ERR_RESP        (void);
PRIVATE void    near S4_MDL_REM_REQ          (void);
PRIVATE void    near S578_MDL_REM_REQ        (void);
PRIVATE void    near S6_MDL_REM_REQ          (void);
PRIVATE void    near S12345678_MDL_XID_REQ   (void);
PRIVATE void    near S7_REJ_RNR_RR           (void);
PRIVATE void    near S8_REJ_RNR_RR           (void);
PRIVATE void    near S4_SABME                (void);
PRIVATE void    near S56_SABME               (void);
PRIVATE void    near S78_SABME               (void);
PRIVATE void    near S78_SET_OWN_REC_BUSY    (void);
#ifdef L2_SREJ
PRIVATE void    near S7_SREJ                 (void);
#endif
PRIVATE void    near S78_ACK_PENDING         (void);
PRIVATE void    near S5_T200_EXPIRY          (void);
PRIVATE void    near S6_T200_EXPIRY          (void);
PRIVATE void    near S7_T200_EXPIRY          (void);
PRIVATE void    near S8_T200_EXPIRY          (void);
PRIVATE void    near S7_T203_EXPIRY          (void);
PRIVATE void    near S5_UA                   (void);
PRIVATE void    near S6_UA                   (void);
PRIVATE void    near S478_UA                 (void);
PRIVATE void    near S45678_UI               (void);
PRIVATE void    near S45_UI_FRAME_QUEUED_UP  (void);
PRIVATE void    near S12345678_XID           (void);
PRIVATE void    L2_ModifyDiscards            (BYTE);

/* State transition function table (see Annex B, C and D/Q.921)
*/
typedef void (near *FSM)(void);                     /* pointer to function */

static const FSM Operation [MAX_L2_STATE] [MAX_L2_EVENT] =
{
   /* TEI_UNASSIGNED -- State 1
   */
   /* information events
   */
   {NOP,                    NOP,                    NOP,
#ifdef L2_FLOW_CTRL
   NOP,
#endif
   NOP,                    NOP,                    NOP,
   NOP,                    NOP,                    NOP,
   /* unnumbered information events
   */
   NOP,                    S123_DL_UI_DATA_REQ,    NOP,
   S12345678_XID,          S12345678_MDL_XID_REQ,
   /* establish/release events
   */
   NOP,                    NOP,                    S12_DL_EST_REQ,
   NOP,                    NOP,                    NOP,
   /* tei events
   */
   S123_MDL_ASS_REQ,       NOP,                    S1_DL_ASS_REQ,
   /* error events
   */
   NOP,                    NOP,                    NOP,
   NOP,                    NOP,                    NOP,
   NOP,                    NOP,                    NOP,
   NOP,                    NOP},
   
   /*
   *  ASSIGN_AWAITING_TEI -- State 2
   */
   /* information events
   */
   {NOP,                    NOP,                    NOP,
#ifdef L2_FLOW_CTRL
   NOP,
#endif
   NOP,                    NOP,                    NOP,
   NOP,                    NOP,                    NOP,
   /* unnumbered information events
   */
   NOP,                    S123_DL_UI_DATA_REQ,    NOP,
   S12345678_XID,          S12345678_MDL_XID_REQ,
   /* establish/release events
   */
   NOP,                    NOP,                    S12_DL_EST_REQ,
   NOP,                    NOP,                    NOP,
   /* tei events
   */
   S123_MDL_ASS_REQ,       NOP,                    NOP,
   /* error events
   */
   S23_MDL_ERR_RESP,       NOP,                    S23_MDL_ERR_RESP,
   NOP,                    NOP,                    NOP,
   NOP,                    NOP,                    NOP,
   NOP,                    NOP},
   
   /*
   *  ESTABLISH_AWAITING_TEI -- State 3
   */
   /* information events
   */
   {NOP,                    NOP,                    NOP,
#ifdef L2_FLOW_CTRL
   NOP,
#endif
   NOP,                    NOP,                    NOP,
   NOP,                    NOP,                    NOP,
   /* unnumbered information events
   */
   NOP,                    S123_DL_UI_DATA_REQ,    NOP,
   S12345678_XID,          S12345678_MDL_XID_REQ,
   /* establish/release events
   */
   NOP,                    NOP,                    S3_DL_EST_REQ,
   NOP,                    NOP,                    NOP,
   /* tei events
   */
   S123_MDL_ASS_REQ,       NOP,                    NOP,
   /* error events
   */
   S23_MDL_ERR_RESP,       NOP,                    S23_MDL_ERR_RESP,
   NOP,                    NOP,                    NOP,
   NOP,                    NOP,                    NOP,
   NOP,                    NOP},
   
   /*
   *  TEI_ASSIGNED -- State 4
   */
   /* information events
   */
   {S4_I_REJ_RNR_RR,        NOP,                    NOP,
#ifdef L2_FLOW_CTRL
   NOP,
#endif
   S4_I_REJ_RNR_RR,        S4_I_REJ_RNR_RR,        S4_I_REJ_RNR_RR,
   NOP,                    NOP,                    NOP,
   /* unnumbered information events
   */
   S45678_UI,              S45678_DL_UI_DATA_REQ,  S45_UI_FRAME_QUEUED_UP,
   S12345678_XID,          S12345678_MDL_XID_REQ,
   /* establish/release events
   */
   S4_SABME,               S478_UA,                S4578_DL_EST_REQ,
   S456_DISC,              S4_DM,                  S4_DL_REL_REQ,
   /* tei events
   */
   NOP,                    S4_MDL_REM_REQ,         NOP,
   /* error events
   */
   NOP,                    NOP,                    S4_MDL_REM_REQ,
   NOP,                    NOP,                    S45678_ERR,
   S45678_ERR,             S45678_ERR,             S45678_ERR,
   NOP,                    NOP},
   
   /*
   *  AWAITING_ESTABLISHMENT -- State 5
   */
   /* information events
   */
   {NOP,                    S5_DL_DATA_REQ,         S5_I_FRAME_QUEUED_UP,
#ifdef L2_FLOW_CTRL
   NOP,
#endif
   NOP,                    NOP,                    NOP,
   NOP,                    S5_T200_EXPIRY,         NOP,
   /* unnumbered information events
   */
   S45678_UI,              S45678_DL_UI_DATA_REQ,  S45_UI_FRAME_QUEUED_UP,
   S12345678_XID,          S12345678_MDL_XID_REQ,
   /* establish/release events
   */
   S56_SABME,              S5_UA,                  S4578_DL_EST_REQ,
   S456_DISC,              S56_DM,                 S5_DL_REL_REQ,
   /* tei events
   */
   NOP,                    S578_MDL_REM_REQ,       NOP,
   /* error events
   */
   NOP,                    NOP,                    S578_MDL_REM_REQ,
   NOP,                    NOP,                    S45678_ERR,
   S45678_ERR,             S45678_ERR,             S45678_ERR,
   NOP,                    NOP},
   
   /*
   *  AWAITING_RELEASE -- State 6
   */
   /* information events
   */
   {NOP,                    NOP,                    NOP,
#ifdef L2_FLOW_CTRL
   NOP,
#endif
   NOP,                    NOP,                    NOP,
   NOP,                    S6_T200_EXPIRY,         NOP,
   /* unnumbered information events
   */
   S45678_UI,              S45678_DL_UI_DATA_REQ,  NOP,
   S12345678_XID,          S12345678_MDL_XID_REQ,
   /* establish/release events
   */
   S56_SABME,              S6_UA,                  NOP,
   S456_DISC,              S56_DM,                 NOP,
   /* tei events
   */
   NOP,                    S6_MDL_REM_REQ,         NOP,
   /* error events
   */
   NOP,                    NOP,                    S6_MDL_REM_REQ,
   NOP,                    NOP,                    S45678_ERR,
   S45678_ERR,             S45678_ERR,             S45678_ERR,
   NOP,                    NOP},
   
   /*
   *  MULTIPLE_FRAME_ESTABLISHED -- State 7
   */
   /* information events
   */
   {S78_I,                  S78_DL_DATA_REQ,        S7_I_FRAME_QUEUED_UP,
#ifdef L2_FLOW_CTRL
   S78_DL_DATA_RESP,
#endif
#ifdef L2_SREJ
   S7_REJ_RNR_RR,          S7_SREJ,                S7_REJ_RNR_RR,
#else
   S7_REJ_RNR_RR,          S7_REJ_RNR_RR,          S7_REJ_RNR_RR,
#endif
   S78_ACK_PENDING,        S7_T200_EXPIRY,         S7_T203_EXPIRY,
   /* unnumbered information events
   */
   S45678_UI,              S45678_DL_UI_DATA_REQ,  NOP,
   S12345678_XID,          S12345678_MDL_XID_REQ,
   /* establish/release events
   */
   S78_SABME,              S478_UA,                S4578_DL_EST_REQ,
   S78_DISC,               S7_DM,                  S78_DL_REL_REQ,
   /* tei events
   */
   NOP,                    S578_MDL_REM_REQ,       NOP,
   /* error events
   */
   NOP,                    S78_FRMR,               S578_MDL_REM_REQ,
   S78_SET_OWN_REC_BUSY,   S78_CLEAR_OWN_REC_BUSY, S45678_ERR,
   S45678_ERR,             S45678_ERR,             S45678_ERR,
   NOP,                    NOP},
   
   /*
   *  TIMER_RECOVERY -- State 8
   */
   /* information events
   */
   {S78_I,                  S78_DL_DATA_REQ,        S8_I_FRAME_QUEUED_UP,
#ifdef L2_FLOW_CTRL
   S78_DL_DATA_RESP,
#endif
#ifdef L2_SREJ
   S8_REJ_RNR_RR,          NOP,                    S8_REJ_RNR_RR,
#else
   S8_REJ_RNR_RR,          S8_REJ_RNR_RR,          S8_REJ_RNR_RR,
#endif
   S78_ACK_PENDING,        S8_T200_EXPIRY,         NOP,
   /* unnumbered information events
   */
   S45678_UI,              S45678_DL_UI_DATA_REQ,  NOP,
   S12345678_XID,          S12345678_MDL_XID_REQ,
   /* establish/release events
   */
   S78_SABME,              S478_UA,                S4578_DL_EST_REQ,
   S78_DISC,               S8_DM,                  S78_DL_REL_REQ,
   /* tei events
   */
   NOP,                    S578_MDL_REM_REQ,       NOP,
   /* error events
   */
   NOP,                    S78_FRMR,               S578_MDL_REM_REQ,
   S78_SET_OWN_REC_BUSY,   S78_CLEAR_OWN_REC_BUSY, S45678_ERR,
   S45678_ERR,             S45678_ERR,             S45678_ERR,
   NOP,                    NOP}
};

/* non-SDL functions alphabetically
*/

/************************************************************************
* L2_AdvanceVA
*************************************************************************
*
* Free ACKed packets and advance VA system variable
* (last I frame ACKed) to NR.
* Note that the packet 'pointed' to by VA has not yet been freed.
*
*/
PUBLIC void L2_AdvanceVA(boolean discard)

{
   if (detail_debug)
       buginf("\n in L2_AdvanceVA");
#ifdef L2_FLOW_CTRL
   L2_RxAck = 0;
#endif

   while (DLCB->VA != L2_NR)
      {
      if (DLCB->TxQ [INDEX(DLCB->VA)].FreePktOk)
         LIF_FreePkt (DLCB->TxQ [INDEX(DLCB->VA)].Pkt);
      if (discard)
         L2_ModifyDiscards(L2_DSL_Id);
      DLCB->TxQ [INDEX(DLCB->VA)].Pkt = NULL;
      DLCB->VA = MOD_INC (DLCB->VA, DLCB->Mod);
#ifdef L2_FLOW_CTRL
      ++L2_RxAck;
#endif
      }
   
#ifdef L2_FLOW_CTRL
   if (DLCB->L3_Flow_Ctrl && L2_RxAck > 0)
      {
      L2_Mail (DL_DATA_CONF, L3_ID);
      }
#endif
}

/************************************************************************
* L2_ClearExceptionConditions
*************************************************************************
*/
PRIVATE void near L2_ClearExceptionConditions ()

{
   if (detail_debug)
       buginf("\n in L2_ClearExceptionConditions");
   DLCB->Peer_Receiver_Busy = FALSE;
   DLCB->Rej_Recovery  = FALSE;
   DLCB->Own_Receiver_Busy  = FALSE;
   DLCB->Ack_Pending = FALSE;
}

/************************************************************************
* L2_EnquiryResponse
*************************************************************************
*
* Respond to a peer enquiry
*
*/
PRIVATE void near L2_EnquiryResponse ()

{
   if (detail_debug)
       buginf("\n in L2_EnquiryResponse");
#ifdef L2_SREJ
   BYTE vi;
   RX_Q *rx_q;
#endif

   L2_F = 1;
   L2_Command = FALSE;
   if (DLCB->Own_Receiver_Busy)
      L2_Mail (RNR, HDLC_D_ID);
   else
      L2_Mail (RR, HDLC_D_ID);
   DLCB->Ack_Pending = FALSE;
#ifdef L2_SREJ
   if (DLCB->Rej_Recovery)
      {
      /* Discard the receive queue
      */
      vi = INDEX(DLCB->VR);
      while (vi != INDEX(DLCB->VR_High))
         {
         rx_q = &DLCB->RxQ[vi];
         if (rx_q->Pkt)
            {
            LIF_FreePkt (rx_q->Pkt);
            rx_q->Pkt = NULL;
            rx_q->Mrej = 0;
            }
         vi = MOD_INC (vi, MAX_L2_QUEUE_LEN);
         }
      DLCB->Rej_Recovery = FALSE;
      }
#endif
}

/************************************************************************
* L2_EstablishDataLink
*************************************************************************
*
* Establish a data link with peer
*
*/
PRIVATE void near L2_EstablishDataLink ()

{
   if (detail_debug)
      buginf("\nentering L2_EstablishDataLink");
   L2_ClearExceptionConditions ();
   DLCB->RC = 0;
   L2_P = 1;
   L2_Command = TRUE;
   L2_Mail (SABME, HDLC_D_ID);
   LIF_StartTimer (DLCB->T200_Handle);
   LIF_StopTimer (DLCB->T203_Handle);
}

/************************************************************************
* L2_FindEventIndex
*************************************************************************
*
* Translate current event into an index to event function table.
*
* Return:   index into event function table.
*
*/
PRIVATE INT16 near L2_FindEventIndex (event)

INT16 event;                                     /* event to translate */
{
   BYTE i;                                               /* loop count */

   if (detail_debug)
       buginf("\n in L2_FindEventIndex");
   for (i=0; (i<MAX_L2_EVENT-1) && (L2_Events [i] != event); i++)
      ;
   return (i);
}

/************************************************************************
* L2_InvokeRetransmission
*************************************************************************
*
* Set up for retransmission; shift frames from retransmission to I queue.
*
*/
PRIVATE void near L2_InvokeRetransmission ()

{
   if (detail_debug)
       buginf("\n in L2_InvokeRetransmission");
   while (DLCB->VS != L2_NR)
      {
      DLCB->VS = MOD_DEC (DLCB->VS, DLCB->Mod);
      ++DLCB->I_Queue_Len;
      }
}

/************************************************************************
* L2_NR_ErrorRecovery
*************************************************************************
*
* Recover from a receive sequence number error
*
*/
PRIVATE void near L2_NR_ErrorRecovery ()

{
   if (detail_debug)
       buginf("\n in L2_NR_ErrorRecovery");
   L2_MDL_Error = 'J';                                   /* N(R) Error */
   L2_Mail (MDL_ERR_IND, ME_ID);
   if ((SwitchType == PRI_4ESS_STYPE) ||
       (SwitchType == PRI_5ESS_STYPE) || (SwitchType == BRI_5ESS_STYPE))
         L2_DiscardQs();

   /*
    * changed from #ifndef's ===
    */
   if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE &&
       SwitchType != BRI_NI1_STYPE && SwitchType != BRI_DMS100_STYPE &&
       SwitchType != BRI_NET3_STYPE && SwitchType != BRI_TS013_STYPE &&
       SwitchType != PRI_1TR6_STYPE && SwitchType != BRI_1TR6_STYPE &&
       SwitchType != PRI_TS014_STYPE && SwitchType != PRI_NET5_STYPE) {
          L2_F = 0;
          L2_Command = FALSE;
          L2_FRMR_Cause = FRMR_Z;
          L2_Mail (FRMR, HDLC_D_ID);
   }
   L2_EstablishDataLink();
   DLCB->L3_Initiated = FALSE;
}

/************************************************************************
* L2_ProcessEvent
*************************************************************************
*
* Process an event based on the current state
* (see Annex B, C and D/Q.921)
*
*/
PUBLIC void L2_ProcessEvent ()

{
   if (detail_debug)
       buginf("\n in L2_ProcessEvent");
#ifdef  DEBUG
   printf("\nL2 SDL : State=%d, Event=%d",
      DLCB->State, L2_FindEventIndex (L2_Event)+1);
   flush();	/* === */
#endif
   (*Operation [DLCB->State-1] [L2_FindEventIndex (L2_Event)] ) ();
}

/************************************************************************
* L2_Queue
*************************************************************************
*
* Put a packet on the queue.
*
*/
PRIVATE void near L2_Queue (pkt, frame, free_pkt_ok)

PKT_ID pkt;                                         /* packet to queue */
INT16 frame;                                         /* I or UI frame? */
BOOLEAN free_pkt_ok;                       /* can we free this packet? */
{
   BYTE va, vs, vi;                              /* indexes into queue */
   int q_len;                                          /* queue length */

   if (detail_debug)
       buginf("\n in L2_Queue");
   va = INDEX(DLCB->VA);
   vs = INDEX(DLCB->VS);
   q_len = ((frame == I) ? DLCB->I_Queue_Len : DLCB->UI_Queue_Len);
   if ((vi = vs + q_len) > MAX_L2_QUEUE_LEN-1)
      vi -= MAX_L2_QUEUE_LEN;

   /* Check for queue overflow.
    */
   if (vs < va)
      vs += MAX_L2_QUEUE_LEN;

   if ((vs - va + q_len) >= MAX_L2_QUEUE_LEN)
      {
      /* NOTE: No error condition is generated on queue overflow.
      *        Frames are simply freed.  (FFS)
      */
      if (free_pkt_ok)
         LIF_FreePkt (pkt);
      }
   else                                       /* store packet in queue */
      {
      DLCB->TxQ[vi].FreePktOk = free_pkt_ok;
      DLCB->TxQ[vi].Pkt = pkt;
      if (frame == I)
         ++DLCB->I_Queue_Len;
      else
         ++DLCB->UI_Queue_Len;
      }
}

/************************************************************************
* L2_TransmitEnquiry
*************************************************************************
*
* Enquire of peer
*
*/
PRIVATE void near L2_TransmitEnquiry ()

{
   if (detail_debug)
       buginf("\n in L2_TransmitEnquiry");
   L2_P = 1;
   L2_Command = TRUE;
   if (DLCB->Own_Receiver_Busy)
      L2_Mail (RNR, HDLC_D_ID);
   else
      L2_Mail (RR, HDLC_D_ID);
   DLCB->Ack_Pending = FALSE;
   LIF_StartTimer (DLCB->T200_Handle);
}

/************************************************************************
* L2_ValidNR
*************************************************************************
*
* Is NR valid?  ValidNRis defined as >=VA && <= VS
*
* Due to modulo arithmetic, the following valid cases may exist:
*
* Case 1:        |________________________VA___NR___VS___|
* Case 2:        |___VS________________________VA___NR___|
* Case 3:        |___NR___VS________________________VA___|
*
* For cases 2 and 3, VS (and NR) are 'normalized' to >= LAPD_MOD
*
*/
PRIVATE BYTE near L2_ValidNR ()

{
   BYTE Tmp_VS;                                /* temporary VS storage */
   BYTE Tmp_NR;                                /* temporary NR storage */

   if (detail_debug)
       buginf("\n in L2_ValidNR");
   if (DLCB->VS < DLCB->VA)
      {
      Tmp_VS = DLCB->VS + DLCB->Mod;
      if (L2_NR <= DLCB->VS)                                 /* case 3 */
         Tmp_NR = L2_NR + DLCB->Mod;
      else                                                   /* case 2 */
         Tmp_NR = L2_NR;
      }
   else                                                      /* case 1 */
      {
      Tmp_VS = DLCB->VS;
      Tmp_NR = L2_NR;
      }

   return ((DLCB->VA <= Tmp_NR) && (Tmp_NR <= Tmp_VS) ? TRUE : FALSE);
}

/* SDL functions by EVENT
*/

/************************************************************************
* NOP
*************************************************************************
*
* No SDL is defined for this event / state pair.
*
*/
PRIVATE void near NOP ()

{
}

/************************************************************************
* S78_CLEAR_OWN_REC_BUSY
*************************************************************************
*
* State = 7,8; Event = CLEAR_OWN_REC_BUSY
*
*/
PRIVATE void near S78_CLEAR_OWN_REC_BUSY ()

{
   if (detail_debug)
       buginf("\n in S78_CLEAR_OWN_REC_BUSY");
   if (DLCB->Own_Receiver_Busy)
      {
      DLCB->Own_Receiver_Busy = FALSE;
      L2_F = 0;
      L2_Command = FALSE;
      L2_Mail (RR, HDLC_D_ID);
      DLCB->Ack_Pending = FALSE;
      }
}

/************************************************************************
* S456_DISC
*************************************************************************
*
* State = 4,5,6; Event = DISC
*
*/
PRIVATE void near S456_DISC ()

{
   if (detail_debug)
       buginf("\n in S456_DISC");
   L2_F = L2_P;
   L2_Command = FALSE;
   if (DLCB->State == AWAITING_RELEASE)
      L2_Mail (UA, HDLC_D_ID);
   else
      L2_Mail (DM, HDLC_D_ID);
}

/************************************************************************
* S78_DISC
*************************************************************************
*
* State = 7,8; Event = DISC
*
*/
PRIVATE void near S78_DISC ()

{
   if (detail_debug)
       buginf("\n in S78_DISC");
   L2_DiscardQs ();
   L2_F = L2_P;
   L2_Command = FALSE;
   L2_Mail (UA, HDLC_D_ID);
   L2_Mail (DL_REL_IND, L3_ID);
   LIF_StopTimer (DLCB->T200_Handle);
   if (DLCB->State == MULTIPLE_FRAME_ESTABLISHED)
      LIF_StopTimer (DLCB->T203_Handle);
   DLCB->State = TEI_ASSIGNED;
}

/************************************************************************
* S1_DL_ASS_REQ
*************************************************************************
*
* State = 1; Event = DL_ASS_REQ
*
*/
PRIVATE void near S1_DL_ASS_REQ ()

{
   if(detail_debug)
      buginf("\n entering S1_DL_ASS_REQ");
   L2_Mail (MDL_ASS_IND, ME_ID);
   DLCB->State = ASSIGN_AWAITING_TEI;
}

/************************************************************************
* S5_DL_DATA_REQ
*************************************************************************
*
* State = 5; Event = DL_DATA_REQ
*
*/
PRIVATE void near S5_DL_DATA_REQ ()

{
   if (detail_debug)
       buginf("\n in S5_DL_DATA_REQ");
   if ((SwitchType != PRI_4ESS_STYPE) &&
       (SwitchType != PRI_5ESS_STYPE) &&
       (SwitchType != BRI_5ESS_STYPE) &&
       (!DLCB->L3_Initiated))
      {
      L2_FreePktIn = FALSE;                      /* keep packet around */
      L2_Queue (L2_PktIn, I, L2_FreePktOk);
      }
}

/************************************************************************
* S78_DL_DATA_REQ
*************************************************************************
*
* State = 7,8; Event = DL_DATA_REQ
*
*/
PRIVATE void near S78_DL_DATA_REQ ()

{
   if (detail_debug)
       buginf("\n in S78_DL_DATA_REQ");
   L2_FreePktIn = FALSE;                         /* keep packet around */
   L2_Queue (L2_PktIn, I, L2_FreePktOk);
}

#ifdef L2_FLOW_CTRL
/************************************************************************
* S78_DL_DATA_RESP
*************************************************************************
*
* State = 7,8; Event = DL_DATA_RESP
*
*/
PRIVATE void near S78_DL_DATA_RESP ()

{
   if (detail_debug)
       buginf("\n in S78_DL_DATA_RESP");
   if (DLCB->L3_Flow_Ctrl)
      {
      /* The user says it can handle some more data.
      *  Just how much more has been placed in L2_TxAck.
      */
      DLCB->L3_RxAck += L2_TxAck;

      /* Check that the flow control parameters are still making
      *  sense.  If the amount of data the user says it can
      *  handle is now bigger than L3_QUEUE_SIZE, something
      *  has gone very wrong.
      */
      if (DLCB->L3_RxAck > L3_QUEUE_SIZE)
         LIF_Fatal ("l2_sdl", "S78_DL_DATA_RESP", "flow control failure.");

      /* Can the user's rx queue now handle a window full of data?
      */
      if (DLCB->L3_RxAck >= DLCB->K)
         {
         /* Was the peer previously flow controlled?
         *  If so, enable the peer's transmissions.
         */
         if (DLCB->Own_Receiver_Busy)
            {
            /* FFS: could cause CLEAR_OWN_REC_BUSY event
            */
            DLCB->Own_Receiver_Busy = FALSE;
            L2_F = 0;
            L2_Command = FALSE;
            L2_Mail (RR, HDLC_D_ID);
            DLCB->Ack_Pending = FALSE;
            }
         }
      }
}
#endif

/************************************************************************
* S12_DL_EST_REQ
*************************************************************************
*
* State = 1,2; Event = DL_EST_REQ
*
*/
PRIVATE void near S12_DL_EST_REQ ()

{
   if(detail_debug)
      buginf("\n L2 getting a DL_EST_REQ in S12");
   if (DLCB->State == TEI_UNASSIGNED)                       /* State 1 */
      L2_Mail (MDL_ASS_IND, ME_ID);
   DLCB->State = ESTABLISH_AWAITING_TEI;
}

/************************************************************************
* S3_DL_EST_REQ
*************************************************************************
*
* State = 3; Event = DL_EST_REQ
*
*/
PRIVATE void near S3_DL_EST_REQ ()

{
    if (detail_debug)
        buginf("\n in S3_DL_EST_REQ");
    L2_Mail (MDL_ASS_IND, ME_ID);
    DLCB->State = ESTABLISH_AWAITING_TEI;
}

/************************************************************************
* S4578_DL_EST_REQ
*************************************************************************
*
* State = 4,5,7,8; Event = DL_EST_REQ
*
*/
PRIVATE void near S4578_DL_EST_REQ ()

{
   if (detail_debug)
       buginf("\n in S4578_DL_EST_REQ");
   if (DLCB->State != TEI_ASSIGNED)
      L2_DiscardQs ();
   if (DLCB->State != AWAITING_ESTABLISHMENT)
      L2_EstablishDataLink ();
   DLCB->L3_Initiated = TRUE;
   DLCB->State = AWAITING_ESTABLISHMENT;
}

/************************************************************************
* S4_DL_REL_REQ
*************************************************************************
*
* State = 4; Event = DL_REL_REQ
*
*/
PRIVATE void near S4_DL_REL_REQ ()

{
   if (detail_debug)
       buginf("\n in S4_DL_REL_REQ");
   L2_Mail (DL_REL_CONF, L3_ID);
}

/************************************************************************
* S5_DL_REL_REQ
*************************************************************************
*
* State = 5; Event = DL_REL_REQ
*
*/
PRIVATE void near S5_DL_REL_REQ ()

{
   if (detail_debug)
       buginf("\n in S5_DL_REL_REQ");
   DLCB->DL_Release_Request = TRUE;
}

/************************************************************************
* S78_DL_REL_REQ
*************************************************************************
*
* State = 7,8; Event = DL_REL_REQ
*
*/
PRIVATE void near S78_DL_REL_REQ ()

{
   if (detail_debug)
       buginf("\n in S78_DL_REL_REQ");
   L2_DiscardQs ();
   DLCB->RC = 0;
   L2_P = 1;
   L2_Command = TRUE;
   L2_Mail (DISC, HDLC_D_ID);
   if (DLCB->State == MULTIPLE_FRAME_ESTABLISHED)
      LIF_StopTimer (DLCB->T203_Handle);
   LIF_StartTimer (DLCB->T200_Handle);
   DLCB->State = AWAITING_RELEASE;
}

/************************************************************************
* S123_DL_UI_DATA_REQ
*************************************************************************
*
* State = 1,2,3; Event = DL_UI_DATA_REQ
*
*/
PRIVATE void near S123_DL_UI_DATA_REQ ()

{
   if (detail_debug)
       buginf("\n in  S123_DL_UI_DATA_REQ");
   L2_FreePktIn = FALSE;                         /* keep packet around */

   if (DLCB->State == TEI_UNASSIGNED)
      L2_Mail (MDL_ASS_IND, ME_ID);

   L2_Queue (L2_PktIn, UI, L2_FreePktOk);

   if (DLCB->State == TEI_UNASSIGNED)
      DLCB->State = ASSIGN_AWAITING_TEI;
}

/************************************************************************
* S45678_DL_UI_DATA_REQ
*************************************************************************
*
* State = 4,5,6,7,8; Event = DL_UI_DATA_REQ
*
*/
PRIVATE void near S45678_DL_UI_DATA_REQ ()

{
   if (detail_debug)
       buginf("\n in  S45678_DL_UI_DATA_REQ");
   L2_FreePktIn = FALSE;                         /* keep packet around */

   /* Go ahead and mail it; don't bother queueing and signaling.
    */
   L2_P = 0;
   L2_Command = TRUE;
   L2_Mail (UI, HDLC_D_ID);
}

/************************************************************************
* S4_DM
*************************************************************************
*
* State = 4; Event = DM
*
*/
PRIVATE void near S4_DM ()

{
/* ===
#ifndef VN2
=== */
   if (detail_debug)
       buginf("\n in  S4_DM");
#if 1	/* === */
   /* check for the equipment type first
    */
   if ((SwitchType == PRI_4ESS_STYPE) ||
       (SwitchType == PRI_5ESS_STYPE) ||
       (SwitchType == BRI_5ESS_STYPE)) {
       if (L2_F) {
           L2_MDL_Error = 'D';                   /* FFS - the error type */
           L2_Mail (MDL_ERR_IND, ME_ID);
       }
   } else {
       /*
        * DM with F=0 is sent to force establish.
        * Ignore this for US switches.
        */
       if (!L2_F) {
           if ((SwitchType != BRI_5ESS_STYPE) &&
               (SwitchType != BRI_DMS100_STYPE) && (SwitchType != BRI_NI1_STYPE)) {  
                  if (DLCB->Able_to_Establish) {
                      L2_EstablishDataLink ();
                      DLCB->L3_Initiated = FALSE;
                      DLCB->State = AWAITING_ESTABLISHMENT;
                  }
              }
          }
      }
#endif
}

/************************************************************************
* S56_DM
*************************************************************************
*
* State = 5,6; Event = DM
*
*/
PRIVATE void near S56_DM ()

{
   if (detail_debug)
       buginf("\n in  S56_DM");
   if (L2_F)
      {
      if (DLCB->State == AWAITING_ESTABLISHMENT)
         {
   if (detail_debug)
       buginf("\n in  S56_DM discard q");
         L2_DiscardQs ();
         if ((SwitchType == PRI_4ESS_STYPE) ||
             (SwitchType == PRI_5ESS_STYPE) ||
             (SwitchType == BRI_5ESS_STYPE))
            {
            L2_MDL_Error = 'D';                /* FFS - the error type */
            L2_Mail (MDL_ERR_IND, ME_ID);
            }
         L2_Mail (DL_REL_IND, L3_ID);
         }
      else
         {
   if (detail_debug)
       buginf("\n in  S56_DM mail dl_rel_conf");
         L2_Mail (DL_REL_CONF, L3_ID);
         }
   if (detail_debug)
       buginf("\n in  S56_DM stop t200");
      LIF_StopTimer (DLCB->T200_Handle);
      DLCB->State = TEI_ASSIGNED;
      }
}

/************************************************************************
* S7_DM
*************************************************************************
*
* State = 7; Event = DM
*
*/
PRIVATE void near S7_DM ()

{
   if (detail_debug)
       buginf("\n in  S7_DM");
   if (L2_F) {
       if ((SwitchType != PRI_4ESS_STYPE) && (SwitchType != PRI_5ESS_STYPE)) {
          L2_MDL_Error = 'B';                /* unsolicited resp., DM, F=1 */
          L2_Mail (MDL_ERR_IND, ME_ID);
       }
       if (SwitchType == BRI_5ESS_STYPE) {
           L2_DiscardQs ();
           L2_EstablishDataLink ();
           DLCB->L3_Initiated = FALSE;
           DLCB->State = AWAITING_ESTABLISHMENT;
       }
   } else {
      /*
       * DM response with F=0
       * Ignore for US switches, 5ess, dms100 and ni1.
       */
      if ((SwitchType != BRI_5ESS_STYPE) &&
          (SwitchType != BRI_DMS100_STYPE) && (SwitchType != BRI_NI1_STYPE)) { 
          L2_MDL_Error = 'E';                      /* DM response with F=0 */
          L2_Mail (MDL_ERR_IND, ME_ID);
          if ((SwitchType == PRI_4ESS_STYPE) ||
              (SwitchType == PRI_5ESS_STYPE) || (SwitchType == BRI_5ESS_STYPE))
              L2_DiscardQs ();
          L2_EstablishDataLink ();
          DLCB->L3_Initiated = FALSE;
          DLCB->State = AWAITING_ESTABLISHMENT;
      }
   }
}

/************************************************************************
* S8_DM
*************************************************************************
*
* State = 8; Event = DM
*
*/
PRIVATE void near S8_DM ()

{
   if (detail_debug)
       buginf("\n in  S8_DM");
   /*
    * Error B = unsolicited DM with F=1, Error E = DM with F=0.
    * Ignore for US switches, 5ess, dms100, ni1.
    */
   if (L2_F) {
       L2_MDL_Error = (L2_F ? 'B' : 'E');
       L2_Mail (MDL_ERR_IND, ME_ID);
       if ((SwitchType == PRI_4ESS_STYPE) ||
           (SwitchType == PRI_5ESS_STYPE) || (SwitchType == BRI_5ESS_STYPE))
           L2_DiscardQs ();
       L2_EstablishDataLink ();
       DLCB->L3_Initiated = FALSE;
       DLCB->State = AWAITING_ESTABLISHMENT;
   } else {
       /*
        * Ignore for US switchtypes, dms100 and ni.
        */
       if ((SwitchType != BRI_DMS100_STYPE) && (SwitchType != BRI_NI1_STYPE)) {
           L2_MDL_Error = (L2_F ? 'B' : 'E');
           L2_Mail (MDL_ERR_IND, ME_ID);
           if ((SwitchType == PRI_4ESS_STYPE) ||
               (SwitchType == PRI_5ESS_STYPE) || (SwitchType == BRI_5ESS_STYPE))
               L2_DiscardQs ();
           L2_EstablishDataLink ();
           DLCB->L3_Initiated = FALSE;
           DLCB->State = AWAITING_ESTABLISHMENT;
       }
   }
}

/************************************************************************
* S45678_ERR
*************************************************************************
*
* State = 4,5,6,7,8; Event = CNTRL_FIELD_ERR, INFO_NOT_PERMITTED,
*                            INCORRECT_LENGTH, I_FRAME_TOO_LONG
*
*/
PRIVATE void near S45678_ERR ()

{
   if (detail_debug)
       buginf("\n in  S45678_ERR");
   switch (L2_Event)
      {
   case CNTRL_FIELD_ERR:
      L2_MDL_Error = 'L';                     /* non-implemented frame */
      L2_F = 0;
      L2_FRMR_Cause = FRMR_W;
      break;

   case INFO_NOT_PERMITTED:
      L2_MDL_Error = 'M';                     /* I field not permitted */
      if (L2_Command)
         L2_F = L2_P;
      L2_FRMR_Cause = FRMR_X;
      break;

   case INCORRECT_LENGTH:
      L2_MDL_Error = 'N';                     /* Frame with wrong size */
      if (L2_Command)
         L2_F = L2_P;
      L2_FRMR_Cause = FRMR_X;
      break;

   case I_FRAME_TOO_LONG:
      L2_MDL_Error = 'O';                                /* N201 error */
      if (L2_Command)
         L2_F = L2_P;
      L2_FRMR_Cause = FRMR_Y;
      break;

   default:
      L2_MDL_Error = '?';                                /* who knows? */
      L2_FRMR_Cause = '?';
      }
   L2_Mail (MDL_ERR_IND, ME_ID);
   if ((DLCB->State == MULTIPLE_FRAME_ESTABLISHED) ||
       (DLCB->State == TIMER_RECOVERY)) {
         if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE &&
             SwitchType != BRI_NI1_STYPE && SwitchType != BRI_DMS100_STYPE &&
             SwitchType != PRI_1TR6_STYPE && SwitchType != BRI_NET3_STYPE &&
             SwitchType != BRI_TS013_STYPE && SwitchType != PRI_TS014_STYPE &&
             SwitchType != BRI_1TR6_STYPE && SwitchType != PRI_NET5_STYPE) {
                 L2_Command = FALSE;
                 L2_Mail(FRMR, HDLC_D_ID);
        }
        L2_EstablishDataLink();
        DLCB->L3_Initiated = FALSE;
        DLCB->State = AWAITING_ESTABLISHMENT;
   }
}

/************************************************************************
* S78_FRMR
*************************************************************************
*
* State = 7,8; Event = FRMR
*
*/
PRIVATE void near S78_FRMR ()

{
   if (detail_debug)
       buginf("\n in  S78_FRMR");
   if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE &&
       SwitchType != BRI_TS013_STYPE && SwitchType != BRI_NET3_STYPE &&
       SwitchType != PRI_TS014_STYPE && SwitchType != PRI_NET5_STYPE) {
           L2_MDL_Error = 'K';             /* receipt of FRMR response */
           L2_Mail (MDL_ERR_IND, ME_ID);
   }
   L2_EstablishDataLink();
   DLCB->L3_Initiated = FALSE;
   DLCB->State = AWAITING_ESTABLISHMENT;
}

/************************************************************************
* S78_I
*************************************************************************
*
* State = 7,8; Event = I
*
*/
PRIVATE void near S78_I ()

{
#ifdef L2_SREJ
   RX_Q *rx_q = DLCB->RxQ;
#endif                                                      /* L2_SREJ */
#ifdef VN2
   /* MODIFICATIONS FAX: (per LIR/FRANCE Dec 1991 - GSD) 
   *  add Boolean FrameRR_Send to test if RR is sent.
   */
   BOOLEAN FrameRR_Send;
   FrameRR_Send = FALSE;
#endif

   if (detail_debug)
       buginf("\n in  S78_I");
   L2_FreePktIn = FALSE;                         /* keep packet around */

   if (DLCB->Own_Receiver_Busy)
      {
      if (L2_FreePktOk)
         {
         LIF_FreePkt (L2_PktIn);
         }
      if (L2_P)
         {
         L2_F = 1;
         L2_Command = FALSE;
         L2_Mail (RNR, HDLC_D_ID);
         DLCB->Ack_Pending = FALSE;
         }
      }
   else
#ifdef L2_SREJ
      {
      if (!DLCB->Rej_Recovery)
         {
         /* Not in reject condition
         */
         if (L2_NS == DLCB->VR)
            {
            /* ACK normally.
            */
            DLCB->VR = MOD_INC (DLCB->VR, DLCB->Mod);
            L2_Mail (DL_DATA_IND, L3_ID);
            DLCB->Ack_Pending = TRUE;
            }
         else if (MOD_GT(L2_NS, DLCB->VR, DLCB->Mod))
            {
            /* New gap here.
            *  Add this frame to the RxQ.
            *  Multi-reject with an SREJ frame for each missing
            *  frame 'below' this one.
            *  Enter reject condition.
            */
            rx_q[INDEX(L2_NS)].Pkt = L2_PktIn;
            rx_q[INDEX(L2_NS)].Mrej = 0;
            for (L2_SREJ_NR=DLCB->VR; L2_SREJ_NR != L2_NS; L2_SREJ_NR++)
               {
               L2_F = 0;
               L2_Command = FALSE;
               L2_Mail (SREJ, HDLC_D_ID);
               rx_q[INDEX(L2_SREJ_NR)].Mrej = 1;
               }
            /* Assign the new VR_High.
            */
            DLCB->VR_High = L2_NS;
            DLCB->Rej_Recovery = TRUE;
            }
         else
            {
            /* NS is out of range!
            */
            if (L2_FreePktOk)
               LIF_FreePkt (L2_PktIn);
            }
         }
      else
         {
         /* In reject condition
         */
         if (L2_NS == DLCB->VR)
            {
            /* A gap has been filled; send DL_DATA(s) up and
            *  ACK up to next gap.
            */
            rx_q[INDEX(L2_NS)].Pkt = L2_PktIn;
            rx_q[INDEX(L2_NS)].Mrej = 0;
            while (rx_q[INDEX(DLCB->VR)].Pkt != NULL)
               {
               L2_PktIn = rx_q[INDEX(DLCB->VR)].Pkt;
               rx_q[INDEX(DLCB->VR)].Pkt = NULL;
               rx_q[INDEX(DLCB->VR)].Mrej = 0;
               DLCB->VR = MOD_INC (DLCB->VR, DLCB->Mod);
               L2_Mail (DL_DATA_IND, L3_ID);
               if (DLCB->VR == MOD_INC(DLCB->VR_High, DLCB->Mod))
                  DLCB->Rej_Recovery = FALSE;       /* all gaps filled */
               }
            DLCB->Ack_Pending = TRUE;
            }
         else if (MOD_GT(L2_NS, DLCB->VR, DLCB->Mod))
            {
            if (rx_q[INDEX(L2_NS)].Pkt == NULL)
               {
               /* Add this frame to the RxQ.
               *  Resend an SREJ for every frame 'below' this one that has
               *  an Mrej value less than or equal to the Mrej value of
               *  the received frame.
               */
               rx_q[INDEX(L2_NS)].Pkt = L2_PktIn;
               for (L2_SREJ_NR=DLCB->VR; L2_SREJ_NR != L2_NS; L2_SREJ_NR++)
                  {
                  if ((rx_q[INDEX(L2_SREJ_NR)].Pkt == NULL) &&
                     (rx_q[INDEX(L2_SREJ_NR)].Mrej <=
                     rx_q[INDEX(L2_NS)].Mrej))
                     {
                     L2_F = 0;
                     L2_Command = FALSE;
                     L2_Mail (SREJ, HDLC_D_ID);
                     ++rx_q[INDEX(L2_SREJ_NR)].Mrej;
                     }
                  }
               rx_q[INDEX(L2_NS)].Mrej = 0;
               if (MOD_GT(L2_NS, DLCB->VR_High, DLCB->Mod))
                  DLCB->VR_High = L2_NS;    /* Assign the new VR_High. */
               }
            else
               {
               /* Protocol error - already received this frame!
               */
               if (L2_FreePktOk)
                  LIF_FreePkt (L2_PktIn);
               }
            }
         else
            {
            /* NS is out of range!
            */
            if (L2_FreePktOk)
               LIF_FreePkt (L2_PktIn);
            }
         }
      }
#else                                                       /* L2_SREJ */
      {
      if (L2_NS == DLCB->VR)
         {
         DLCB->VR = MOD_INC (DLCB->VR, DLCB->Mod);
         DLCB->Rej_Recovery = FALSE;
         L2_Mail (DL_DATA_IND, L3_ID);
#ifdef L2_FLOW_CTRL
         if (DLCB->L3_Flow_Ctrl)
            {
            /* Check that the network has not overrun us.
            */
            if (DLCB->L3_RxAck <= 0)
               LIF_Fatal ("l2_sdl", "S78_I", "flow control failure.");

            /* Can the user's rx queue handle another window full of data?
            *  if not, set Own_Receiver_Busy to TRUE.
            */
            if (--DLCB->L3_RxAck < DLCB->K)
               DLCB->Own_Receiver_Busy = TRUE;
            }
#endif                                                 /* L2_FLOW_CTRL */
         if (L2_P)
            {                                          /* par. 5.6.2.1 */
            L2_F = 1;
            L2_Command = FALSE;
            if (DLCB->Own_Receiver_Busy)
               {
               L2_Mail (RNR, HDLC_D_ID);
               }
            else
               {
#ifdef VN2
               /* MODIFICATIONS FAX (per LIR/FRANCE - GSD Dec/1991)
               */
               if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)		/* === */
					FrameRR_Send = TRUE;
#endif
               L2_Mail (RR, HDLC_D_ID);
               }
            DLCB->Ack_Pending = FALSE;
            }
         else
            {                                          /* par. 5.6.2.2 */
            DLCB->Ack_Pending = TRUE;
            }
         }
      else
         {
         if (L2_FreePktOk)
            LIF_FreePkt (L2_PktIn);

         if (DLCB->Rej_Recovery)
            {
#ifdef L2_MREJ
            /* NOTE: Reject recovery implemented as per Appendix I in Q.921.
            */
            if ((L2_NS == DLCB->VM) || (MOD_LT(L2_NS, DLCB->VM, DLCB->Mod)))
               {
               L2_F = L2_P;
               L2_Command = FALSE;
               L2_Mail (REJ, HDLC_D_ID);
               DLCB->Ack_Pending = FALSE;
               L2_MDL_Error = '?';                       /* who knows? */
               L2_Mail (MDL_ERR_IND, ME_ID);
               }
            else
               {
               if (L2_P)
                  {
                  L2_F = L2_P;
                  L2_Command = FALSE;
                  L2_Mail (RR, HDLC_D_ID);
                  DLCB->Ack_Pending = FALSE;
                  }
               }
            DLCB->VM = L2_NS;
#else                                                       /* L2_MREJ */
            if (L2_P)
               {
               L2_F = L2_P;
               L2_Command = FALSE;
               L2_Mail (RR, HDLC_D_ID);
               DLCB->Ack_Pending = FALSE;
               }
#endif                                                      /* L2_MREJ */
            }
         else
            {
            DLCB->Rej_Recovery = TRUE;
            L2_F = L2_P;
            L2_Command = FALSE;
            L2_Mail (REJ, HDLC_D_ID);
            DLCB->Ack_Pending = FALSE;
#ifdef L2_MREJ
            DLCB->VM = L2_NS;
#endif                                                      /* L2_MREJ */
            }
         }
      }
#endif                                                      /* L2_SREJ */

   if (L2_ValidNR ())
      {
      if ((DLCB->Peer_Receiver_Busy) || (DLCB->State == TIMER_RECOVERY))
         {
         L2_AdvanceVA(FALSE);
         }
      else
         {
         if (L2_NR == DLCB->VS)
            {
            L2_AdvanceVA(FALSE);
            LIF_StopTimer (DLCB->T200_Handle);
            LIF_StartTimer (DLCB->T203_Handle);
            }
         else
            {
            if (L2_NR != DLCB->VA)
               {
               L2_AdvanceVA(FALSE);
               LIF_StartTimer (DLCB->T200_Handle);
#ifdef VN2
               /* MODIFICATIONS FAX (per LIR/FRANCE, GSD - Dec 1991)
               */
	           LIF_StopTimer (DLCB->T203_Handle);
#endif               
               }
            }
         }
      }
   else
      {
#ifdef VN2
	  if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {		/* === */
          /* MODIFICATIONS FAX (per LIR/FRANCE, GSD - Dec 1991)
          * add test if RR or REJ have been sent, no sent RR 
          */
          if ((DLCB->Rej_Recovery == FALSE) && (FrameRR_Send == FALSE))
             {
             L2_F = L2_P;
             L2_Command = FALSE;
             if (DLCB->Own_Receiver_Busy)
                L2_Mail (RNR, HDLC_D_ID);
             else
                L2_Mail (RR, HDLC_D_ID);
             DLCB->Ack_Pending = FALSE;
             }
	  }							/* === */
#endif                                                          /* VN2 */
      L2_NR_ErrorRecovery ();
      DLCB->State = AWAITING_ESTABLISHMENT;
      }
}

/************************************************************************
* S5_I_FRAME_QUEUED_UP
*************************************************************************
*
* State = 5; Event = I_FRAME_QUEUED_UP
*
*/
PRIVATE void near S5_I_FRAME_QUEUED_UP ()

{
   /* need more mail independent of Layer 3 initialization
    */
   if (detail_debug)
       buginf("\n in S5_I_FRAME_QUEUED_UP ");
   L2_NeedMail = TRUE;
}

/************************************************************************
* S7_I_FRAME_QUEUED_UP
*************************************************************************
*
* State = 7; Event = I_FRAME_QUEUED_UP
*
*/
PRIVATE void near S7_I_FRAME_QUEUED_UP ()

{
   BYTE va, vs;                          /* indexes into I frame queue */

   if (detail_debug)
       buginf("\n in S7_I_FRAME_QUEUED_UP ");
   va = INDEX(DLCB->VA);
   vs = INDEX(DLCB->VS);

   if (DLCB->Peer_Receiver_Busy)
      {
      L2_NeedMail = TRUE;              /* need more mail to avoid loop */
      }
   else
      {
      if (vs < va)
         {
         vs += MAX_L2_QUEUE_LEN;
         }
      if (vs == va + DLCB->K)
         {
         L2_NeedMail = TRUE;                         /* need more mail */
         }
      else
         {
         DLCB->I_Queue_Len--;
         L2_P = 0;
         L2_Command = TRUE;
         L2_Mail (I, HDLC_D_ID);
         DLCB->VS = MOD_INC (DLCB->VS, DLCB->Mod);
         DLCB->Ack_Pending = FALSE;
         if (!LIF_TimerRunning (DLCB->T200_Handle))
            {
            LIF_StopTimer (DLCB->T203_Handle);
            LIF_StartTimer (DLCB->T200_Handle);
            }
         }
      }
}

/************************************************************************
* S8_I_FRAME_QUEUED_UP
*************************************************************************
*
* State = 8; Event = I_FRAME_QUEUED_UP
*
*/
PRIVATE void near S8_I_FRAME_QUEUED_UP ()

{
   if (detail_debug)
       buginf("\n in S8_I_FRAME_QUEUED_UP ");
   L2_NeedMail = TRUE;                 /* need more mail to avoid loop */
}

/************************************************************************
* S4_I_REJ_RNR_RR
*************************************************************************
*
* State = 4; Event = I, RR, REJ or RNR
* (NOTE: the special treatment of I frames and supervisory commands
*        applies only to 4ESS and 5ESS equipment)
*
*/
PRIVATE void near S4_I_REJ_RNR_RR ()

{
   if (detail_debug)
       buginf("\n in S4_I_REJ_RNR_RR ");
   if (((SwitchType == PRI_4ESS_STYPE) ||
        (SwitchType == PRI_5ESS_STYPE) ||
        (SwitchType == BRI_5ESS_STYPE)) &&
		 (L2_P))
      {
      L2_F = 1;
      L2_Command = FALSE;
      L2_Mail (DM, HDLC_D_ID);
      }
}

/************************************************************************
* S123_MDL_ASS_REQ
*************************************************************************
*
* State = 1,2,3; Event = MDL_ASS_REQ
*
*/
PRIVATE void near S123_MDL_ASS_REQ ()

{
   if (detail_debug)
       buginf("\n in S123_MDL_ASS_REQ ");
#ifdef DEBUG
   printf("Begin of MDL_ASS_REQ():\n");
   L2_DispDLCB(DLCB);
#endif

   /*  Update data link timers; this DLCB may be re-used.
   */
   LIF_UpdateTimer( DLCB->T200_Handle, L2_ID, L2_ID, T200_EXPIRY, L2_DSL_Id,
                    L2_Sapi, L2_Ces, 0, L2_DSL_Chan, 
					isdn_sys_constants[L2_DSL_Id].T200 );
   LIF_UpdateTimer( DLCB->T203_Handle, L2_ID, L2_ID, T203_EXPIRY, L2_DSL_Id,
                    L2_Sapi, L2_Ces, 0, L2_DSL_Chan, DEFAULT_T203 );
   if(SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)	/* === */
	   LIF_UpdateTimer( DLCB->T204_Handle, ME_ID, ME_ID, T204_EXPIRY, L2_DSL_Id,
                    L2_Sapi, L2_Ces, 0, L2_DSL_Chan, DEFAULT_T204 );

   if (DLCB->State == ESTABLISH_AWAITING_TEI)
      {
      L2_EstablishDataLink();
      DLCB->L3_Initiated = TRUE;
      DLCB->State = AWAITING_ESTABLISHMENT;
      }
   else
      {
      DLCB->State = TEI_ASSIGNED;
      }
#ifdef DEBUG
   printf("End of MDL_ASS_REQ():\n");
   L2_DispDLCB(DLCB);
#endif
}

/************************************************************************
* S23_MDL_ERR_RESP
*************************************************************************
*
* State = 2,3; Event = MDL_ERR_RESP, PERSISTENT_DEACT
*
*/
PRIVATE void near S23_MDL_ERR_RESP ()

{
   if (detail_debug)
       buginf("\n in S23_MDL_ERR_RESP ");
   L2_DiscardQs ();
   if (DLCB->State == ESTABLISH_AWAITING_TEI)               /* State 3 */
      L2_Mail (DL_REL_IND, L3_ID);
   DLCB->State = TEI_UNASSIGNED;
}

/************************************************************************
* S4_MDL_REM_REQ
*************************************************************************
*
* State = 4; Event = MDL_REM_REQ, PERSISTENT_DEACT
*
*/
PRIVATE void near S4_MDL_REM_REQ ()

{
   if (detail_debug)
       buginf("\n in S4_MDL_REM_REQ ");
   L2_DiscardQs ();
   /*
    * PRI TEI should never go to state TEI_UNASSIGNED.
    */
   if (L2_Event == MDL_REM_REQ && !is_pri)
      {
      DLCB->State = TEI_UNASSIGNED;
      L2_RemoveTeiCes(DLCB->Tei, DLCB->Ces, DLCB->DSL_Chan);
      }
   else
      {
      DLCB->State = TEI_ASSIGNED;
      }
}

/************************************************************************
* S578_MDL_REM_REQ
*************************************************************************
*
* State = 5,7,8; Event = MDL_REM_REQ, PERSISTENT_DEACT
*
*/
PRIVATE void near S578_MDL_REM_REQ ()

{
   if (detail_debug)
       buginf("\n in S578_MDL_REM_REQ ");
   L2_DiscardQs ();
   L2_Mail (DL_REL_IND, L3_ID);
   LIF_StopTimer (DLCB->T200_Handle);
   if (DLCB->State == MULTIPLE_FRAME_ESTABLISHED)
      LIF_StopTimer (DLCB->T203_Handle);
   /*
    * PRI TEI should never go to state TEI_UNASSIGNED.
    */
   if (L2_Event == MDL_REM_REQ && !is_pri)
      {
      DLCB->State = TEI_UNASSIGNED;
      L2_RemoveTeiCes(DLCB->Tei, DLCB->Ces, DLCB->DSL_Chan);
      }
   else
      {
      DLCB->State = TEI_ASSIGNED;
      }
}

/************************************************************************
* S6_MDL_REM_REQ
*************************************************************************
*
* State = 6; Event = MDL_REM_REQ, PERSISTENT_DEACT
*
*/
PRIVATE void near S6_MDL_REM_REQ ()

{
   if (detail_debug)
       buginf("\n in S6_MDL_REM_REQ ");
   L2_DiscardQs ();
   L2_Mail (DL_REL_CONF, L3_ID);
   LIF_StopTimer (DLCB->T200_Handle);
   /*
    * PRI TEI should never go to state TEI_UNASSIGNED.
    */
   if (L2_Event == MDL_REM_REQ && !is_pri)
      {
      DLCB->State = TEI_UNASSIGNED;
      L2_RemoveTeiCes(DLCB->Tei, DLCB->Ces, DLCB->DSL_Chan);
      }
   else
      {
      DLCB->State = TEI_ASSIGNED;
      }
}

/************************************************************************
* S12345678_MDL_XID_REQ
*************************************************************************
*
* State = 1,2,3,4,5,6,7,8; Event = MDL_XID_REQ
*
*/
PRIVATE void near S12345678_MDL_XID_REQ ()

{
   if (detail_debug)
       buginf("\n in S12345678_MDL_XID_REQ ");
   L2_FreePktIn = FALSE;                         /* keep packet around */

   /* Just go ahead and mail it.
    */
   L2_P = 0;
   L2_Command = TRUE;
   L2_Mail (XID, HDLC_D_ID);
}

/************************************************************************
* S7_REJ_RNR_RR
*************************************************************************
*
* State = 7; Event = RR, RNR, REJ
*
*/
PRIVATE void near S7_REJ_RNR_RR ()

{
   if (detail_debug)
       buginf("\n in S7_REJ_RNR_RR ");
   DLCB->Peer_Receiver_Busy = ((L2_Event == RNR) ? TRUE : FALSE);

   if (L2_Command)
      {
      if (L2_P)
         L2_EnquiryResponse ();
      }
   else
      {
      if (L2_F)
         {
         L2_MDL_Error = 'A';                   /* supervisory with F=1 */
         L2_Mail (MDL_ERR_IND, ME_ID);
         }
      }

   if (L2_ValidNR ())
      {
      if (L2_Event == RR)
         {
         if (L2_NR == DLCB->VS)
            {
            L2_AdvanceVA(FALSE);
            if (detail_debug)
                buginf("\n in S7_REJ_RNR_RR stop 200 start 203");
            LIF_StopTimer (DLCB->T200_Handle);
            LIF_StartTimer (DLCB->T203_Handle);
            }
         else
            {
            if (L2_NR != DLCB->VA)
               {
               L2_AdvanceVA(FALSE);
               if (detail_debug)
                   buginf("\n in S7_REJ_RNR_RR start 200");
               LIF_StartTimer (DLCB->T200_Handle);
               }
            }
         }
      else if (L2_Event == RNR)
         {
         L2_AdvanceVA(FALSE);
         LIF_StopTimer (DLCB->T203_Handle);
         LIF_StartTimer (DLCB->T200_Handle);
         }
      else
         {
#ifndef L2_SREJ
         /* REJ
         */
         L2_AdvanceVA(FALSE);
         LIF_StopTimer (DLCB->T200_Handle);
         LIF_StartTimer (DLCB->T203_Handle);
         L2_InvokeRetransmission ();
#endif
         }
      }
   else
      {
      L2_NR_ErrorRecovery ();
      DLCB->State = AWAITING_ESTABLISHMENT;
      }
}

/************************************************************************
* S8_REJ_RNR_RR
*************************************************************************
*
* State = 8; Event = RR,REJ,RNR
*
*/
PRIVATE void near S8_REJ_RNR_RR ()

{
   if (detail_debug)
       buginf("\n in S8_REJ_RNR_RR ");
   DLCB->Peer_Receiver_Busy = ((L2_Event == RNR) ? TRUE : FALSE);

   if (L2_Command)
      {
      if (L2_P)
         L2_EnquiryResponse ();
      if (L2_ValidNR ())
         {
         L2_AdvanceVA(FALSE);
         }
      else
         {
         L2_NR_ErrorRecovery ();
         DLCB->State = AWAITING_ESTABLISHMENT;
         }
      }
   else
      {
      if (L2_F)
         {
         if (L2_ValidNR ())
            {
            L2_AdvanceVA(FALSE);
            if (L2_Event != RNR)
               {
               LIF_StopTimer (DLCB->T200_Handle);
               LIF_StartTimer (DLCB->T203_Handle);
               }
            else
               {
               LIF_StartTimer (DLCB->T200_Handle);
               }
            L2_InvokeRetransmission ();
            DLCB->State = MULTIPLE_FRAME_ESTABLISHED;
            }
         else
            {
            L2_NR_ErrorRecovery ();
            DLCB->State = AWAITING_ESTABLISHMENT;
            }
         }
      else
         {
         if (L2_ValidNR ())
            {
            L2_AdvanceVA(FALSE);
            }
         else
            {
            L2_NR_ErrorRecovery ();
            DLCB->State = AWAITING_ESTABLISHMENT;
            }
         }
      }
}

/************************************************************************
* S4_SABME
*************************************************************************
*
* State = 4; Event = SABME
*
*/
PRIVATE void near S4_SABME ()

{
   if (detail_debug)
       buginf("\n in S4_SABME ");
   /* NOTE: Guard here against the possibility of the peer being configured
   *        the same as this L2 (USER-USER or NETWORK-NETWORK).
   */
   if (!L2_Command)
      {
#ifdef DISPLAY_LOCAL_ERR
      LIF_Error ("l2", "S4_SABME", "Peer is also configured as USER.");
#endif
      }
   else
      {
      L2_F = L2_P;
      L2_Command = FALSE;
      if (DLCB->Able_to_Establish)
         {
         L2_Mail (UA, HDLC_D_ID);
         L2_ClearExceptionConditions ();
#ifdef L2_FLOW_CTRL
         DLCB->L3_RxAck = L3_QUEUE_SIZE;
#endif      
         DLCB->VS = DLCB->VA = DLCB->VR = 0;
#ifdef L2_MREJ
         DLCB->VM = 0;
#endif
#ifdef L2_SREJ
         DLCB->VR_High = 0;
#endif

         /*
          * If an interface gets cleared, the switch may send a SABME before
          * the router. Have to make sure that spid stuff gets handled correctly.
          */
         if (SwitchType == BRI_5ESS_STYPE || SwitchType == BRI_DMS100_STYPE ||
             SwitchType == BRI_NI1_STYPE) {
             CCBFIT_TerminalInit(MNL_EST_IND, DLCB->Ces, L2_DSL_Id);
         }
         L2_Mail (DL_EST_IND, L3_ID);
         LIF_StopTimer (DLCB->T200_Handle);
         LIF_StartTimer (DLCB->T203_Handle);
/* HACK ALERT */
	if (detail_debug)
	buginf ("\n in S4_SABME setting state to MULT fr est");
         DLCB->State = MULTIPLE_FRAME_ESTABLISHED;
         }
      else
         {
         L2_Mail (DM, HDLC_D_ID);
         }
      }
}

/************************************************************************
* S56_SABME
*************************************************************************
*
* State = 5,6; Event = SABME
*
*/
PRIVATE void near S56_SABME ()

{
   if (detail_debug)
       buginf("\n in S56_SABME ");
   L2_F = L2_P;
   L2_Command = FALSE;
   if (DLCB->State == AWAITING_ESTABLISHMENT)
      {
#ifdef BRI_DMS100
      if (SwitchType == BRI_DMS100_STYPE)
         {
         /* if responding to SABME's then go to MFE
         *  to satisfy TEI Management test cases.
         */
         L2_Mail (UA, HDLC_D_ID);
         L2_ClearExceptionConditions ();
#ifdef L2_FLOW_CTRL
         DLCB->L3_RxAck = L3_QUEUE_SIZE;
#endif      
         DLCB->VS = DLCB->VA = DLCB->VR = 0;
         L2_Mail (DL_EST_IND, L3_ID);
         LIF_StopTimer (DLCB->T200_Handle);
         LIF_StartTimer (DLCB->T203_Handle);
         DLCB->State = MULTIPLE_FRAME_ESTABLISHED;
         }
      else
         {
         L2_Mail (UA, HDLC_D_ID);
         }
#else
      L2_Mail (UA, HDLC_D_ID);
#endif
      }
   else
      {
      L2_Mail (DM, HDLC_D_ID);
      }
}

/************************************************************************
* S78_SABME
*************************************************************************
*
* State = 7,8; Event = SABME
*
*/
PRIVATE void near S78_SABME ()

{
   if (detail_debug)
       buginf("\n in S78_SABME ");
   L2_F = L2_P;
   L2_Command = FALSE;
   L2_Mail (UA, HDLC_D_ID);
   L2_ClearExceptionConditions ();
   if ((SwitchType != PRI_4ESS_STYPE) &&
      (SwitchType != PRI_5ESS_STYPE))
      {
      L2_MDL_Error = 'F';                 /* unsuccessful retran SABME */
      L2_Mail (MDL_ERR_IND, ME_ID);
      }
   if ((DLCB->VS != DLCB->VA) ||
      ((SwitchType == PRI_4ESS_STYPE) ||
      (SwitchType == PRI_5ESS_STYPE) ||
      (SwitchType == BRI_5ESS_STYPE)))
      L2_Mail (DL_EST_IND, L3_ID);
   LIF_StopTimer (DLCB->T200_Handle);
   LIF_StartTimer (DLCB->T203_Handle);
   L2_DiscardQs ();
#ifdef L2_FLOW_CTRL
   DLCB->L3_RxAck = L3_QUEUE_SIZE;
#endif
   DLCB->VS = DLCB->VA = DLCB->VR = 0;
#ifdef L2_MREJ
   DLCB->VM = 0;
#endif
#ifdef L2_SREJ
   DLCB->VR_High = 0;
#endif
   if (DLCB->State != MULTIPLE_FRAME_ESTABLISHED)
      DLCB->State = MULTIPLE_FRAME_ESTABLISHED;
}

/************************************************************************
* S78_SET_OWN_REC_BUSY
*************************************************************************
*
* State = 7,8; Event = SET_OWN_REC_BUSY
*
*/
PRIVATE void near S78_SET_OWN_REC_BUSY ()

{
   if (detail_debug)
       buginf("\n in S78_SET_OWN_REC_BUSY ");
   if (!DLCB->Own_Receiver_Busy)
      {
      DLCB->Own_Receiver_Busy = TRUE;
      L2_F = 0;
      L2_Command = FALSE;
      L2_Mail (RNR, HDLC_D_ID);
      DLCB->Ack_Pending = FALSE;
      }
}

#ifdef L2_SREJ
/************************************************************************
* S7_SREJ
*************************************************************************
*
* State = 7; Event = SREJ
*
*/
PRIVATE void near S7_SREJ ()

{
   if (detail_debug)
       buginf("\n in S7_SREJ ");
   /* Retransmit the I frame with the sequence number L2_NR.
   *  DLCB->VS is assumed to be the next I frame to send, therefore
   *  set this value to L2_NR, send the frame and reset DLCB->VS to
   *  its original value.
   */
   BYTE vs;

   vs = DLCB->VS;
   DLCB->VS = L2_NR;
   L2_P = 0;
   L2_Command = TRUE;
   L2_Mail (I, HDLC_D_ID);
   DLCB->VS = vs;
   DLCB->Ack_Pending = FALSE;
   LIF_StartTimer (DLCB->T200_Handle);
}
#endif

/************************************************************************
* S78_ACK_PENDING
*************************************************************************
*
* State = 7,8; Event = ACK_PENDING
*
*/
PRIVATE void near S78_ACK_PENDING ()

{
   if (detail_debug)
       buginf("\n in  S78_ACK_PENDING ");
   if (DLCB->Ack_Pending)
      {
      DLCB->Ack_Pending = FALSE;
      L2_F = 0;
      L2_Command = FALSE;
      L2_Protocol = DLCB->L2_Protocol;
      L2_DSL_Chan = DLCB->DSL_Chan;
      if (DLCB->Own_Receiver_Busy)
         L2_Mail (RNR, HDLC_D_ID);
      else
         L2_Mail (RR, HDLC_D_ID);
      }
}

/************************************************************************
* S5_T200_EXPIRY
*************************************************************************
*
* State = 5; Event = T200_EXPIRY
*
*/
PRIVATE void near S5_T200_EXPIRY ()

{
   if(detail_debug)
       buginf("\n Expiry TIMER 200");

   L2_Protocol = DLCB->L2_Protocol;
   L2_DSL_Chan = DLCB->DSL_Chan;
   if (DLCB->RC == DLCB->N200)
      {
      L2_DiscardQs ();
      /*
       * Have to set the me_activate flag so that ccbri will not attempt to
       * reactivate Layer 2 while the ME task does the TEI id_verify.
       */
      if ((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) {
          me_set_activate_flag(L2_DSL_Id, DLCB->Ces, TRUE);
      }
      L2_MDL_Error = 'G';                /* unsuccessful retrans SABME */
      L2_Mail (MDL_ERR_IND, ME_ID);
      L2_Mail (DL_REL_IND, L3_ID);
      DLCB->State = TEI_ASSIGNED;
      }
   else
      {
      ++DLCB->RC;
      L2_P = 1;
      L2_Command = TRUE;
      L2_Mail (SABME, HDLC_D_ID);
      LIF_StartTimer (DLCB->T200_Handle);
      }
}

/************************************************************************
* S6_T200_EXPIRY
*************************************************************************
*
* State = 6; Event = T200_EXPIRY
*
*/
PRIVATE void near S6_T200_EXPIRY ()

{
   if (detail_debug)
       buginf("\n in  S6_T200_EXPIRY ");
#ifdef DEBUG
   printf("EXPIRY  TIMER 200\n");
#endif

   L2_Protocol = DLCB->L2_Protocol;
   L2_DSL_Chan = DLCB->DSL_Chan;
   if (DLCB->RC == DLCB->N200)
      {
      L2_MDL_Error = 'H';                 /* Unsuccessful retrans DISC */
      L2_Mail (MDL_ERR_IND, ME_ID);
      L2_Mail (DL_REL_CONF, L3_ID);
      DLCB->State = TEI_ASSIGNED;
      }
   else
      {
      ++DLCB->RC;
      L2_P = 1;
      L2_Command = TRUE;
      L2_Mail (DISC, HDLC_D_ID);
      LIF_StartTimer (DLCB->T200_Handle);
      }
}

/************************************************************************
* S7_T200_EXPIRY
*************************************************************************
*
* State = 7; Event = T200_EXPIRY
*
*/
PRIVATE void near S7_T200_EXPIRY ()

{
   if (detail_debug)
       buginf("\n in  S7_T200_EXPIRY ");
#ifdef DEBUG
   printf("EXPIRY  TIMER 200\n");
#endif

   L2_Protocol = DLCB->L2_Protocol;
   L2_DSL_Chan = DLCB->DSL_Chan;

   DLCB->RC = 0;
   /* FFS implement dependent
    */
#ifdef KDD_NTT
   if (( SwitchType == KDD_STYPE ) || (DLCB->Peer_Receiver_Busy))
#else
   if (DLCB->Peer_Receiver_Busy)
#endif
      {
      L2_TransmitEnquiry ();
      }
   else
      {
      /* Get last transmitted I frame.
       */
      DLCB->VS = MOD_DEC (DLCB->VS, DLCB->Mod);
      L2_P = 1;
      L2_Command = TRUE;
      L2_Mail (I, HDLC_D_ID);
      DLCB->VS = MOD_INC (DLCB->VS, DLCB->Mod);
      DLCB->Ack_Pending = FALSE;
      LIF_StartTimer (DLCB->T200_Handle);
      }
   ++DLCB->RC;
   DLCB->State = TIMER_RECOVERY;
}

/************************************************************************
* S8_T200_EXPIRY
*************************************************************************
*
* State = 8; Event = T200_EXPIRY
*
*/
PRIVATE void near S8_T200_EXPIRY ()

{
   if (detail_debug)
       buginf("\n in  S8_T200_EXPIRY ");
#ifdef DEBUG
   printf("EXPIRY  TIMER 200\n");
#endif

   L2_Protocol = DLCB->L2_Protocol;
   L2_DSL_Chan = DLCB->DSL_Chan;
   if (DLCB->RC == DLCB->N200)
      {
      if ((SwitchType == PRI_4ESS_STYPE) ||
          (SwitchType == PRI_5ESS_STYPE) ||
			 (SwitchType == BRI_5ESS_STYPE))
         L2_DiscardQs ();
      L2_MDL_Error = 'I';               /* unsuccessful retrans - stat */
      L2_Mail (MDL_ERR_IND, ME_ID);
      L2_EstablishDataLink ();
      DLCB->L3_Initiated = FALSE;
      DLCB->State = AWAITING_ESTABLISHMENT;
      }
   else
      {
      if (DLCB->VS == DLCB->VA)
         {
         L2_TransmitEnquiry ();
         }
      else
         {
         /* FFS implement dependent
          */
#ifdef KDD_NTT
         if (( SwitchType == KDD_STYPE ) || (DLCB->Peer_Receiver_Busy))
#else
         if (DLCB->Peer_Receiver_Busy)
#endif
            {
            L2_TransmitEnquiry ();
            }
         else
            {
            /* Get last transmitted I frame.
             */
            DLCB->VS = MOD_DEC (DLCB->VS, DLCB->Mod);
            L2_P = 1;
            L2_Command = TRUE;
            L2_Mail (I, HDLC_D_ID);
            DLCB->VS = MOD_INC (DLCB->VS, DLCB->Mod);
            DLCB->Ack_Pending = FALSE;
            LIF_StartTimer (DLCB->T200_Handle);
            }
         }
      ++DLCB->RC;
      }
}

/************************************************************************
* S7_T203_EXPIRY
*************************************************************************
*
* State = 7; Event = T203_EXPIRY
*
*/
PRIVATE void near S7_T203_EXPIRY ()

{
   if (detail_debug)
       buginf("\n in  S7_T203_EXPIRY ");
#ifdef DEBUG
   printf("EXPIRY  TIMER 203\n");
#endif

   L2_Protocol = DLCB->L2_Protocol;
   L2_DSL_Chan = DLCB->DSL_Chan;
   L2_TransmitEnquiry ();
   DLCB->RC = 0;
   DLCB->State = TIMER_RECOVERY;
}

/************************************************************************
* S5_UA
*************************************************************************
*
* State = 5; Event = UA
*
*/
PRIVATE void near S5_UA ()

{
   if (detail_debug)
       buginf("\n in  S5_UA ");
   if (L2_F)
      {
      if (DLCB->L3_Initiated)
         {
         L2_Mail (DL_EST_CONF, L3_ID);
         }
      else if (DLCB->VS != DLCB->VA)
         {
         L2_DiscardQs ();
         L2_Mail (DL_EST_IND, L3_ID);
         }

      /*
       * Make sure that a spid gets sent out well before a Setup message.
       * This has to be done here, otherwise the MTEX mail will allow the
       * Setup to go out before the spid.
       */
      if (SwitchType == BRI_5ESS_STYPE || SwitchType == BRI_DMS100_STYPE ||
          SwitchType == BRI_NI1_STYPE) {
          CCBFIT_TerminalInit(NL_EST_CONF, DLCB->Ces, L2_DSL_Id);
      }

      LIF_StopTimer (DLCB->T200_Handle);
      LIF_StartTimer (DLCB->T203_Handle);
#ifdef L2_FLOW_CTRL
      DLCB->L3_RxAck = L3_QUEUE_SIZE;
#endif
      DLCB->VS = DLCB->VA = DLCB->VR = 0;
#ifdef L2_MREJ
      DLCB->VM = 0;
#endif
#ifdef L2_SREJ
      DLCB->VR_High = 0;
#endif
      DLCB->State = MULTIPLE_FRAME_ESTABLISHED;
      }
   else
      {
      L2_MDL_Error = 'D';                   /* unsolicited UA with F=0 */
      L2_Mail (MDL_ERR_IND, ME_ID);
      }
}

/************************************************************************
* S6_UA
*************************************************************************
*
* State = 6; Event = UA
*
*/
PRIVATE void near S6_UA ()

{
   if (detail_debug)
       buginf("\n in  S6_UA ");
   if (L2_F)
      {
      L2_Mail (DL_REL_CONF, L3_ID);
      LIF_StopTimer (DLCB->T200_Handle);
      DLCB->State = TEI_ASSIGNED;
      }
   else
      {
      if (SwitchType != BRI_5ESS_STYPE)
         {
         L2_MDL_Error = 'D';                /* unsolicited UA with F=0 */
         L2_Mail (MDL_ERR_IND, ME_ID);
         }
      }
}

/************************************************************************
* S478_UA
*************************************************************************
*
* State = 4,7,8; Event = UA
*
*/
PRIVATE void near S478_UA ()

{
   if (detail_debug)
       buginf("\n in  S478_UA ");
   /* Error C = unsolicited UA with F=1, Error D = unsolicited UA with F=0
    */
   L2_MDL_Error = (L2_F ? 'C' : 'D');
   L2_Mail (MDL_ERR_IND, ME_ID);
   if (DLCB->State != TEI_ASSIGNED)                    /* state 7 or 8 */
      {
      if (SwitchType == BRI_5ESS_STYPE)
         {
         L2_DiscardQs ();
         L2_EstablishDataLink ();
         if (DLCB->State == MULTIPLE_FRAME_ESTABLISHED)
            DLCB->L3_Initiated = FALSE;
         DLCB->State = AWAITING_ESTABLISHMENT;
         }
      else if (((SwitchType == PRI_4ESS_STYPE) ||
                (SwitchType == PRI_5ESS_STYPE)) &&
					L2_F)
         {
         L2_EstablishDataLink ();
         L2_DiscardQs ();
         DLCB->L3_Initiated = FALSE;
         DLCB->State = AWAITING_ESTABLISHMENT;
         }
      }
}

/************************************************************************
* S45678_UI
*************************************************************************
*
* State = 4,5,6,7,8; Event = UI
*
*/
PRIVATE void near S45678_UI ()

{
   if (detail_debug)
       buginf("\n in  S45678_UI ");
   L2_FreePktIn = FALSE;                         /* keep packet around */

   L2_Mail (DL_UI_DATA_IND, L3_ID);
}

/************************************************************************
* S45_UI_FRAME_QUEUED_UP
*************************************************************************
*
* State = 4,5; Event = UI_FRAME_QUEUED_UP
*
*/
PRIVATE void near S45_UI_FRAME_QUEUED_UP ()

{
   if (detail_debug)
       buginf("\n in  S45_UI_FRAME_QUEUED_UP ");
   --DLCB->UI_Queue_Len;
   L2_P = 0;
   L2_Command = TRUE;
   L2_PktIn = DLCB->TxQ [INDEX(DLCB->VS)].Pkt;       /* dequeue packet */
   DLCB->TxQ [INDEX(DLCB->VS)].Pkt = NULL;
   L2_Mail (UI, HDLC_D_ID);
   DLCB->VS = MOD_INC (DLCB->VS, DLCB->Mod);
}

/************************************************************************
* S12345678_XID
*************************************************************************
*
* State = 1,2,3,4,5,6,7,8; Event = XID
*
*/
PRIVATE void near S12345678_XID ()

{
   if (detail_debug)
       buginf("\n in  S12345678_XID ");
   switch ( SwitchType )
      {
#ifdef KDD_NTT
      case KDD_STYPE:
      case NTT_STYPE:
         break;                                        /* Ignore frame */
#endif
      default:
         L2_FreePktIn = FALSE;                   /* keep packet around */

         if (L2_Command)                          /* XID command frame */
            L2_Mail (MDL_XID_IND, ME_ID);
         else                                    /* XID response frame */
            L2_Mail (MDL_XID_CONF, ME_ID);
         break;
      }
}


PRIVATE void L2_ModifyDiscards (BYTE dsl)
{

   if (L2_Discards[dsl] == 0xffffffff) {
      /* 
       * Take Care of Wrap around case 
       */
      L2_Discards[dsl] -= L2D_Task_Counter[dsl];
      L2D_Task_Counter[dsl] = 0;
   }
   L2_Discards[dsl]++;
}

