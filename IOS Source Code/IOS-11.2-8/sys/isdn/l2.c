/* $Id: l2.c,v 3.3.42.2 1996/04/30 23:07:32 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/l2.c,v $
 *------------------------------------------------------------------
 * l2.c  ISDN Link Access Procedure on the D-channel (LAPD)
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: l2.c,v $
 * Revision 3.3.42.2  1996/04/30  23:07:32  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3.42.1  1996/04/11  21:43:57  dclare
 * CSCdi52751:  Make DMS-100 ISDN PRI changes to conform to the
 * Specification
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/04  22:11:19  rbeach
 * CSCdi44727:  ISDN MBRI running 5ESS with spids will always use
 * int bri 0 spid value. There was a hardcoded value for the 5ESS
 * switchtype only. It is now dynamic. Also fixed a debug message
 * in l2.c.
 *
 * Revision 3.2  1995/11/17  17:41:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:16:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: l2.c
VERSION: 3.21
DESCRIPTION: ISDN Link Access Procedure on the D-channel (LAPD)

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

The L2 Task implements the following Link Access Procedures based on
CCITT specifications:

   -  LAPB per the X.25  CCITT Specification
   -  LAPD per the Q.921 CCITT Specification (Blue Book)
   -  V120 per the V.120 CCITT Specification (1988)

L2 task communicates with Layer 3, Layer 1, and the Management Entity
by mail messages.

This module contains code to support both the user side and the
network side of the L2 tasks.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.


FLOW CONTROL

Flow control between layer 2 and layer 3 is now available under
compile time flag L2_FLOW_CTRL.  The flow control system is based on
a system of "credits", and required the addition of two new
primitives, DL_DATA_CONF and DL_DATA_RESP.  The names of these two
new primitives were contrived from OSI primitive naming conventions.

In order to use the new flow control mechanisim, the compile time
flag L2_FLOW_CONTROL must be defined.  

It may not be desirable to perform flow control on all data links
(for instance, the Telenetworks Q.931 stack does not understand flow
control at this time, and in fact it is doubtfull it will ever be
needed there).  For this reason, flow control can be enabled on a per
data link basis (and here is yet another configuration problem here,
namely, which DLCB's have flow control enabled, and which do not).

When flow control is to be enabled for a data link, the DLCB boolean
field L3_Flow_Ctrl should be set to TRUE, and the DLCB field L3_RxAck
should be set to L3_QUEUE_SIZE.  When flow control is not to be
performed, L3_Flow_Ctrl should be set to FALSE, and in this case the
value of L3_RxAck is not important.  The L3_Flow_Ctrl field must
never be modified while the data link is in established (that
is, flow control can only be turned on and off when there is no
established data link).

Under flow control, Layer 2 then keeps track of the number of
DL_DATA_IND's sent to Layer 3 by decrementing L3_RxAck once for each
DL_DATA_IND.  When L2_RxAck gets as low as the value K (the Layer 2
window size), Layer 2 begins to send RNR's instead of RR's to it's
peer.  By sending down DL_DATA_RESP primitives to Layer 2, Layer 3
can increment the data link's L3_RxAck value, thus allowing the peer
to send more data.  By withholding DL_DATA_RESP's Layer 3 can prevent
it's peer from sending further data.  Note that Layer 3 must be able
to buffer at least L3_QUEUE_SIZE I frames, and that L3_QUEUE_SIZE
should be bigger than K.

For flow control from Layer 3 to Layer 2, MAX_L2_QUEUE_LEN replaces
L3_QUEUE_SIZE, DL_DATA_CONF replaces DL_DATA_RESP, and DL_DATA_REQ
replaces DL_DATA_IND.  This means that if Layer 3 is using a flow
controlled data link, it must initialize a variable with
MAX_L2_QUEUE_LEN, decrement it's value for each DL_DATA_REQ sent,
increment it's value in response to DL_DATA_CONF's, and stop sending
data when it's value reaches zero.

Let us call L2_RxAck and the Layer 3 equivalent of this variable the
"credit value".  The primitives DL_DATA_RESP and DL_DATA_CONF each
contain a parameter that specifies by how much to increment the
credit value.  The LIF primitive field FreePkt has been "adapted"
(kludeged?) to carry this credit value.


FLOW CONTROL RESET PROBLEMS

Note that when layer 3 receives an indication that a data link has
been reset (via DL_EST_IND), it must reset it's flow control
parameters.  Of course this requires that Layer 3 then send back a
message to handshake the flow control reset so that Layer 2 knows
from what point it must start counting DL_DATA_REQ's again.  For
this, we need to invent a new primitive, and using OSI terminology,
let us call it DL_EST_RESP.  This has not been implemented yet, and
for the time being, if DL_EST_IND is received by Layer 3 for an
established data link, Layer 3 must respond with DL_REL_REQ, that is,
it must drop the data link.


ACTIVATION AND DEACTIVATION PROBLEMS

Activation primitives currently reset the entire Layer 2 task.  This
is poor behaviour for systems that have multiple DSL's.  The state
machinery for handling activation is currently in file l2if.c.
This machinery should probably be integrated with l2_sdl.c.


L2 PROTOCOL PROBLEMS

In processing primitives, Layer 2 deterimines the proper protocol by
taking the value out of the primitive each time.  This has caused
some grief.  Firstly, it requires that Layer 1 know what protocol is
currently begin performed by layer 2.  Secondly, since LIF_Timers do
not handle the Layer 2 protocol value, it requires some execptional
code in the case of timers.


*************************************************************************
*************************************************************************
*/

/* Note: the following symbol must be defined by this module
*  to cause the allocation of the TeiCesTable in tei_ces.h.
*/
#define L2_C


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

#undef      NULL		/* Both master.h and flags.h define it... */

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "gp.h"
#include    "lif.h"
#include    "tei_ces.h"
#include    "l2.h"
#include    "../h/logger.h"
#include    "../wan/isdn.h"
#include    "../h/interface_private.h"
#include    "../isdn/msg_isdn.c"

/************************************************************************
*                F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

PRIVATE DLCB_ID     near L2_AllocDLCB    (void);
PRIVATE void        near L2_Broadcast    (BOOLEAN);
PRIVATE BYTE        near L2_CesToTei     (BYTE, INT16);
PRIVATE void        near L2_FreeDLCB     (DLCB_ID, int);
PRIVATE INT16       near L2_GetAddr      (PKT_ID, BYTE);
PRIVATE INT16       near L2_GetCntrl     (PKT_ID);
PRIVATE DLCB_ID     near L2_GetDLCB      (int, BYTE, BYTE);
PRIVATE BOOLEAN     near L2_GetIntEvent  (void);
PRIVATE INT16       near L2_InitDLCB     (DLCB_ID, int, BYTE, BYTE);
PRIVATE void        near L2_PutAddr      (PKT_ID);
PRIVATE void        near L2_PutCntrl     (PKT_ID, INT16);
PRIVATE void        near L2_Put_FRMR     (BYTE *);
PRIVATE void        near L2_Readjust_Info (PKT_ID, int);
PRIVATE BYTE        near L2_TeiToCes     (BYTE, INT16);
#ifdef BRI_DMS100
PRIVATE void        near L2_TermTest     (INT16, PKT_ID);
#endif


/************************************************************************
*                   D A T A   A L L O C A T I O N
*************************************************************************
*/

/* public data (declared in l2.h)
*/
PKT_ID  L2_PktIn;                                   /* incoming packet */
INT16   L2_Event;                                     /* current event */
BYTE    L2_Sapi;                                       /* current SAPI */
BYTE    L2_Ces;                                         /* current CES */
BYTE    L2_DSL_Id;                       /* digital subscriber loop ID */
INT16   L2_DSL_Chan;                             /* layer 1 channel ID */
INT16   L2_Protocol;                  /* L2P_DSS1, L2P_X25 or L2P_V120 */
BOOLEAN L2_Command;                       /* command or response frame */
BYTE    L2_NR;                              /* receive sequence number */
BYTE    L2_NS;                                 /* send sequence number */
BYTE    L2_F;                                             /* final bit */
BYTE    L2_P;                                              /* poll bit */
#ifdef L2_SREJ
BYTE    L2_SREJ_NR;           /* sequence number to selectively reject */
#endif
#ifdef L2_FLOW_CTRL
int     L2_RxAck;                /* number of packets ack'd by network */
int     L2_TxAck;                   /* number of packets ack'd by user */
#endif
BOOLEAN L2_NeedMail;  /* necessary to read mail to avoid infinite loop */
BOOLEAN L2_FreePktIn;          /* should the incoming packet be freed? */
BOOLEAN L2_FreePktOk;             /* can the incoming packet be freed? */
BYTE    L2_MDL_Error;      /* MDL error indications (Table II-1/Q.921) */
BYTE    L2_FRMR_Cause;                             /* FRMR error cause */
DLCB_ID DLCB;                                 /* pointers current dlcb */

/* Was public data, but not used anywhere else, so made private
*/
PRIVATE PKT_ID  L2_PktOut;                          /* outgoing packet */
PRIVATE BYTE    L2_Tei;                                 /* current TEI */
PRIVATE BYTE    L2_FRMR_Control1;     /* rejected frame control byte 1 */
PRIVATE BYTE    L2_FRMR_Control2;     /* rejected frame control byte 2 */
PRIVATE BYTE    L2_FRMR_Command;       /* rejected frame is a command? */

/* private data
*/
PRIVATE BYTE    L2_Network;              /* acting as the network side */
PRIVATE DLCB_ID DLCB_Head[MAX_DSL];      /* heads of dlcb linked lists */
PRIVATE DLCB_ID DLCB_Tail[MAX_DSL];      /* tails of dlcb linked lists */
int Used_dlcb;    /* counter for showing memory usage */

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/**************
 * show_dlcbs *
 **************
 *
 * Show the values of the active DLCBs for an interface
 */
PUBLIC void show_dlcbs (int dsl)
{
    DLCB_ID dlcb;

    printf("\n    Layer 2 Status:");
    dlcb = DLCB_Head[dsl];
    if (dlcb) {
        while (dlcb) {
	    printf("\n\tTEI = %d, State = ", (int) dlcb->Tei);
            switch (dlcb->State) {
	        case 0x01:
                    printf("TEI_UNASSIGNED");
                    break;
                case 0x02:
                    printf("ASSIGN_AWAITING_TEI");
                    break;
                case 0x03:
                    printf("ESTABLISH_AWAITING_TEI");
                    break;
                case 0x04:
                    printf("TEI_ASSIGNED");
                    break;
                case 0x05:
                    printf("AWAITING_ESTABLISHMENT");
                    break;
                case 0x06:
                    printf("AWAITING_RELEASE");
                    break;
                case 0x07:
                    printf("MULTIPLE_FRAME_ESTABLISHED");
                    break;
                case 0x08:
                    printf("TIMER_RECOVERY");
                    break;
                default:
                    printf("Unknown State");
                    break;
            }
            dlcb = dlcb->Next;
        }
    } else
        printf("\n\tLayer 2 NOT Activated");
}

/************************************************************************
* L2_AddTeiCes
*************************************************************************
*
* Add an entry to the TeiCesTable - find the first unused space in
*   the table and add the new entry there.
*
*/
PUBLIC void L2_AddTeiCes (dsl_id, tei, ces, chan)

int  dsl_id;
BYTE tei;
BYTE ces;
INT16 chan;
{
   INT16 i;

   /* look for an unused entry in TEI/CES/Chan table
   */
#ifdef USER
   for (i=0; i<MAX_LOGICAL_LINKS; i++)
      {
      if (TeiCesTable[dsl_id][i].Tei == UNASSIGNED)
         {
         /* put the current TEI, CES and physical channel into the table
         */
         TeiCesTable[dsl_id][i].Tei = tei;
         TeiCesTable[dsl_id][i].Ces = ces;
         TeiCesTable[dsl_id][i].DSL_Chan = chan;
         break;
         }
      }
#endif
}

/************************************************************************
* L2_AllocDLCB
*************************************************************************
*
* Allocate a DLCB memory block.
*
* Return:   Address of the DLCB if possible, NULL otherwise.
*
* NOTE:  This function is factored out in order to enable a change from
*        dynamic (present) to static allocation of DLCB blocks.
*
*/
PRIVATE DLCB_ID near L2_AllocDLCB()

{
   return(malloc_named(sizeof(struct DataLinkControlBlock), "isdn_dlcb"));
}

/************************************************************************
* L2_Broadcast
*************************************************************************
*
* Pass along an UI frame.
*
*/
PRIVATE void near L2_Broadcast (from_hdlc)

BOOLEAN from_hdlc;
{
   L2_FreePktIn = FALSE;     /* UI packets are passed along, not freed */

   if (from_hdlc)
      {
      L2_Ces = BROADCAST_CES;
      if (L2_Sapi == ME_SAPI)
         L2_Mail (MDL_UI_DATA_IND, ME_ID);           /* to Mgmt Entity */
      else
         L2_Mail (DL_UI_DATA_IND, L3_ID);                /* to Layer 3 */
      }
   else                                                     /* UI data */
      {
      L2_P = 0;
      L2_Command = TRUE;
      L2_Tei = BROADCAST_TEI;
      L2_Mail (UI, HDLC_D_ID);                           /* to Layer 1 */
      }

   DLCB = NULL;               /* Terminate message processing in L2_Go */
}

/************************************************************************
* L2_CesToTei
*************************************************************************
*
* Convert a CES into its corresponding TEI.
*
* Return:   corresponding TEI, if found;
*           UNASSIGNED, otherwise.
*
*/
PRIVATE BYTE near L2_CesToTei (ces, chan)

BYTE ces;                                                       /* CES */
INT16 chan;                                      /* layer 1 channel ID */
{
   BYTE tei;
   INT16 i;

   if (L2_Protocol == L2P_V120)
      return ces;

   tei = UNASSIGNED;

   /* go through the TEI/CES/Chan table looking for a match for the
   *  given CES and physical channel.
   */
   for (i=0; i<MAX_LOGICAL_LINKS; i++)
      {
      if ((TeiCesTable[L2_DSL_Id][i].Ces == ces) &&
         (TeiCesTable[L2_DSL_Id][i].DSL_Chan == chan))
         {
         /* found the match - get the TEI
         */
         tei = TeiCesTable[L2_DSL_Id][i].Tei;
         break;
         }
      }

   return (tei);
}

/************************************************************************
* L2_DiscardQs
*************************************************************************
*
* Discard packets on the queues.
*
* The UI queue begins at VS and is UI_Queue_Len long.
*
* The I queue conceptually consists of 2 queues.  The transmit queue
* contains packets that have not yet been sent; it begins at VS and is
* is I_Queue_Len long.  The retransmit queue contains packets that have
* been sent but not yet ACKed; it begins at VA and goes up to VS-1.
*
#ifdef L2_SREJ
* The receive queue begins at VR and goes up to VR_High.
*
#endif
*/
PUBLIC void L2_DiscardQs ()

{
   BYTE vi;
   TX_Q *tx_q;
#ifdef L2_SREJ
   RX_Q *rx_q;
#endif

   /* Discard the UI queue
   */
   vi = INDEX(DLCB->VS);
   while (DLCB->UI_Queue_Len)
      {
      tx_q = &DLCB->TxQ[vi];
      if (tx_q->Pkt != NULL)
         {
         if (tx_q->FreePktOk)
            LIF_FreePkt (tx_q->Pkt);
         tx_q->Pkt = NULL;                /* another task will free it */
         }
      vi = MOD_INC (vi, MAX_L2_QUEUE_LEN);
      --DLCB->UI_Queue_Len;
      }

   /* Discard the I queue - transmit part
   */
   vi = INDEX(DLCB->VS);
   while (DLCB->I_Queue_Len)
      {
      tx_q = &DLCB->TxQ[vi];
      if (tx_q->Pkt != NULL)
         {
         if (tx_q->FreePktOk)
            LIF_FreePkt (tx_q->Pkt);
         tx_q->Pkt = NULL;                /* another task will free it */
         }
      vi = MOD_INC (vi, MAX_L2_QUEUE_LEN);
      --DLCB->I_Queue_Len;
      }

   /* Discard the I queue - retransmit part
   */
   L2_NR = DLCB->VS;               /* pretend that all have been ACKed */
   L2_AdvanceVA(TRUE);

#ifdef L2_SREJ
   if (DLCB->Rej_Recovery)
      {
      /* Discard the receive queue
      */
      vi = INDEX(DLCB->VR);
      while (vi != INDEX(DLCB->VR_High))
         {
         rx_q = &DLCB->RxQ[vi];
         if (rx_q->Pkt != NULL)
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

#ifdef DEBUG
/************************************************************************
* L2_DispDLCB
*************************************************************************
*
* Display a DLCB
*
*/
PUBLIC void L2_DispDLCB (dlcb)

DLCB_ID dlcb;                                       /* DLCB to display */
{
   printf("DLCB: State=%d, Sapi=%d, Ces=%d, Tei=%d\n", (int) dlcb->State,
      (int) dlcb->Sapi, (int) dlcb->Ces, (int) dlcb->Tei);
}
#endif

/************************************************************************
* L2_FreeDLCB
*************************************************************************
*
* Remove data link control block from list of DLCBs and deallocate
* its storage.
*
*/
PRIVATE void near L2_FreeDLCB (dlcb, dsl)

DLCB_ID dlcb;                                          /* DLCB to free */
int dsl;
{
   DLCB_ID temp;

   DLCB = dlcb;                                 /* DiscardQs uses DLCB */

   /* Search / Remove DLCB from Linked List
   */
   if (dlcb == DLCB_Head[dsl]) {
      /* first link in list */
      temp = NULL;
      DLCB_Head[dsl] = dlcb->Next;
   } else {
      /* search for link; logically delete */
      temp = DLCB_Head[dsl];
      while (temp->Next != dlcb) {
         temp = temp->Next;
         if (temp == NULL)                                /* not found */
            goto bailout;
      }
      temp->Next = dlcb->Next;
   }
   if (dlcb == DLCB_Tail[dsl])              /* last link in list */
      DLCB_Tail[dsl] = temp;

   if (dlcb->L2_Protocol == L2P_LAPD)       /* get rid of TeiCes entry */
      L2_RemoveTeiCes(dlcb->Tei, dlcb->Ces, dlcb->DSL_Chan);

   /* Stop / Remove Timers
   */
   LIF_StopTimer(dlcb->T200_Handle);
   LIF_StopTimer(dlcb->T203_Handle);
   if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
       LIF_StopTimer(dlcb->T204_Handle);
   LIF_RemoveTimer(dlcb->T200_Handle);
   LIF_RemoveTimer(dlcb->T203_Handle);
   if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
       LIF_RemoveTimer(dlcb->T204_Handle);

   L2_DiscardQs();                   /* discard any packets on queues */

   /*
    * Deallocate link
    */
   Used_dlcb--;
   free(dlcb);

bailout:
   return;
}

/************************************************************************
* L2_GetAddr
*************************************************************************
*
* Get address fields from packet frame header.
* (see Figure 5 and Table 1/Q.921 or X.25/2.4.2)
*
* Return:   NOERR if all went well;
*           ERR, otherwise.
*/
PRIVATE INT16 near L2_GetAddr (pkt, network)

PKT_ID pkt;                                                  /* packet */
BYTE network;                                     /* network/user flag */
{
   BYTE *h;                             /* frame header part of packet */
   INT16 ret_code;                                /* return/error code */

   h = pkt->Header;
   ret_code = NOERR;

   switch (L2_Protocol)
      {
      case L2P_LAPD:
         /* channel must be the signalling channel for LAPD
         */
         if (L2_DSL_Chan != DSS1_D_CHANNEL)
            ret_code = ERR;
         /* octet 2 EA bit must be clear and octet 3 EA bit must be set
         */
         if ( ( ( h[0] & 0x01 ) == 0x01 ) || ( ( h[1] & 0x01 ) == 0x00 ) )
            ret_code = ERR;
         L2_Sapi = h[0] >> 2;
         if (( L2_Sapi != Q931_SAPI) &&
             ( L2_Sapi != X25_SAPI ) &&
#ifdef BRI_DMS100
             ( L2_Sapi != TERM_LOOP_SAPI) &&
#endif
             ( L2_Sapi != ME_SAPI ))
                ret_code = ERR;
         L2_Tei = h[1] >> 1;
         L2_Ces = L2_TeiToCes (L2_Tei, L2_DSL_Chan);
         break;
      case L2P_LAPB:
         /* octet 2 EA bit must be set for LAPB
         */
         if ( ( h[0] & 0x01 ) == 0x00 )
            ret_code = ERR;
         L2_Sapi = LAPB_SAPI;                     /* Use 'fake' values */
         L2_Tei = LAPB_TEI;
         L2_Ces = LAPB_CES;
         break;
      case L2P_V120:
         /* octet 2 EA bit must be clear and octet 3 EA bit must be set
         */
         if ( ( ( h[0] & 0x01 ) == 0x01 ) || ( ( h[1] & 0x01 ) == 0x00 ) )
            ret_code = ERR;
         L2_Sapi = h[0] >> 2;
         L2_Tei = h[1] >> 1;
         L2_Ces = L2_Tei;
         break;
      default:
         ret_code = ERR;
         break;
      }

  /*  Symmetric command/response bit; the user and network both
   *  interpret this bit the same.
   */
   if(Network_side) {
      L2_Command = (h[0] & 0x02) ? FALSE : TRUE;
   } else {
      L2_Command = (h[0] & 0x02) ? TRUE : FALSE;
   }

   /* If an FRMR is sent, this info will again be needed; save it.
   */
   L2_FRMR_Command = L2_Command;

#ifdef DEBUG
   printf("L2_GetAddr() : SAPI=%d, TEI=%d, CES=%d\n", L2_Sapi, L2_Tei, L2_Ces);
#endif

   return (ret_code);
}

/************************************************************************
* L2_GetCntrl
*************************************************************************
*
* Get control fields from packet.
* (see Table 5/Q.921 or X.25)
*
* Return:   event of this command / response.
*
*/
PRIVATE INT16 near L2_GetCntrl (pkt)

PKT_ID pkt;                                                  /* packet */
{
   BYTE *h;                             /* frame header part of packet */
   BYTE *pf;                                      /* poll or final bit */
   BYTE format;                                        /* frame format */
   INT16 temp_event;                                     /* temp event */
   INT16 head_len;                           /* expected header length */

   h = pkt->Header;
   pf = (L2_Command ? &L2_P : &L2_F);           /* command or response */

   /* If an FRMR is sent, this info will again be needed; save it.
   */
   L2_FRMR_Control1 = h[2];            /* received frame control field */
   L2_FRMR_Control2 = h[3];                   /* assume NOT unnumbered */

   format = ((L2_Protocol == L2P_LAPB) ? (h[1] & 0x03) : (h[2] & 0x03));

   switch (format)
      {
      case SUPERVISORY:
         if (L2_Protocol == L2P_LAPB)
            {
            L2_Readjust_Info (pkt, 2);
            head_len = 2;
            temp_event = (INT16) (h[1] & 0x0F);
            L2_NR = h[1] >> 5;
            *pf = (h[1] >> 4) & 0x01;
            }
         else
            {		/* needed for 1tr6 conformance testing */
			if (h[2] & 0xf0) {	/* upper byte is set incorrectly === */
				temp_event = CNTRL_FIELD_ERR;
				return (temp_event);
			} else {
            	head_len = 4;
            	temp_event = (INT16) h[2];
            	L2_NR = h[3] >> 1;
            	*pf = h[3] & 0x01;
            }
		}
#ifdef L2_SREJ
         if ((temp_event != RR) && (temp_event != RNR) && (temp_event != SREJ))
#else
         if ((temp_event != RR) && (temp_event != RNR) && (temp_event != REJ))
#endif
            temp_event = CNTRL_FIELD_ERR;
         else if (pkt->Info != NULL)
            temp_event = INFO_NOT_PERMITTED;
         break;

      case UNNUMBERED:
         L2_FRMR_Control2 = 0;
         if (L2_Protocol == L2P_LAPB)
            {
            L2_Readjust_Info (pkt, 2);
            head_len = 2;
            temp_event = (INT16) (h[1] & 0xEF);
            if (temp_event == SABM)
               temp_event = SABME;          /* translate SABM to SABME */
            *pf = (h[1] >> 4) & 0x01;
            }
         else
            {
            head_len = 3;
            temp_event = (INT16) (h[2] & 0xEF);
            *pf = (h[2] >> 4) & 0x01;
            }
         switch (temp_event)
            {
            case SABME:
               if (!L2_Command)
                  temp_event = CNTRL_FIELD_ERR;
               else if (pkt->Info != NULL)
                  temp_event = INFO_NOT_PERMITTED;
               break;

            case DM:
               if (L2_Command)
                  temp_event = CNTRL_FIELD_ERR;
               else if (pkt->Info != NULL)
                  temp_event = INFO_NOT_PERMITTED;
               break;

            case DISC:
               if (!L2_Command)
                  temp_event = CNTRL_FIELD_ERR;
               else if (pkt->Info != NULL)
                  temp_event = INFO_NOT_PERMITTED;
               break;

            case UA:
               if (L2_Command)
                  temp_event = CNTRL_FIELD_ERR;
               else if (pkt->Info != NULL)
                  temp_event = INFO_NOT_PERMITTED;
               break;

            case UI:
               if (L2_Protocol != L2P_LAPB)
                  L2_Readjust_Info (pkt, 1);
               if (!L2_Command)
                  temp_event = CNTRL_FIELD_ERR;
               else if (pkt->Info == NULL)
                  temp_event = INFO_REQUIRED;
               else if (pkt->InfoLen > DEFAULT_N201)
                  temp_event = I_FRAME_TOO_LONG;
               break;

            case FRMR:
               if (L2_Protocol != L2P_LAPB)
                  L2_Readjust_Info (pkt, 1);
               if (L2_Command)
                  temp_event = CNTRL_FIELD_ERR;
               else if (pkt->Info == NULL)
                  temp_event = INFO_REQUIRED;
               else if (pkt->InfoLen > DEFAULT_N201)
                  temp_event = I_FRAME_TOO_LONG;
               break;

            case XID:
               if (L2_Protocol != L2P_LAPB)
                  L2_Readjust_Info (pkt, 1);
               if (pkt->Info == NULL)
                  temp_event = INFO_REQUIRED;
               else if (pkt->InfoLen > DEFAULT_N201)
                  temp_event = I_FRAME_TOO_LONG;
#ifdef PRI
               /* Primary Rate doesn't understand nor respond to XIDs.
               */
               temp_event = UNDEFINED;
#endif
               break;

            default:
               temp_event = CNTRL_FIELD_ERR;
               break;
            }
         break;

      default:                                          /* information */
         temp_event = I;
         if (L2_Protocol == L2P_LAPB)
            {
            L2_Readjust_Info (pkt, 2);
            head_len = 2;
            L2_NS = (h[1] >> 1) & 0x07;
            L2_NR = h[1] >> 5;
            *pf = (h[1] >> 4) & 0x01;
            }
         else
            {
            head_len = 4;
            L2_NS = h[2] >> 1;
            L2_NR = h[3] >> 1;
            *pf = h[3] & 0x01;
            }
         if (!L2_Command)
            {
            temp_event = CNTRL_FIELD_ERR;
            }
         else if (pkt->Info == NULL)
            {
               if ( SwitchType == BRI_DMS100_STYPE ||
                    SwitchType == BRI_NET3_STYPE ||
                    SwitchType == PRI_NET5_STYPE ||
                    SwitchType == BRI_TS013_STYPE ||
                    SwitchType == PRI_TS014_STYPE ||
                    SwitchType == BRI_NI1_STYPE ||
                    SwitchType == VN2_STYPE ||
                    SwitchType == VN3_STYPE) {
               /* for short I-frame, acknowledge as usual
               *  NIS 208-4T - test case 4.19 LAPD test
               */
               }
            else
               {
               temp_event = INFO_REQUIRED;
               }
            }
         else if (pkt->InfoLen > DEFAULT_N201)
            {
            temp_event = I_FRAME_TOO_LONG;
            }
         break;
      }

   if (pkt->HeadLen > head_len)
      temp_event = INCORRECT_LENGTH;
   else if (pkt->HeadLen < head_len)
      temp_event = UNDEFINED;

   return (temp_event);
}

/************************************************************************
* L2_GetDLCB
*************************************************************************
*
* Allocate and initialize a DLCB.
*
* Return:   Address of the DLCB, if allocated
*           NULL, otherwise
*
*/
PRIVATE DLCB_ID near L2_GetDLCB (dsl_id, ces, network)

int  dsl_id;                             /* Digital Subscriber Loop Id */
BYTE ces;                                         /* CES to search for */
BYTE network;                                     /* network/user flag */
{
    DLCB_ID dlcb;    /* DLCB address */

    if ((dlcb = L2_AllocDLCB()) != NULL) {
        if ((L2_InitDLCB (dlcb, dsl_id, ces, network) ) == ERR) {
            free(dlcb);
            dlcb = NULL;
        } else {
            Used_dlcb++;
            if (DLCB_Head[dsl_id] != NULL)
                DLCB_Tail[dsl_id]->Next = dlcb;
            else
                DLCB_Head[dsl_id] = dlcb;
            DLCB_Tail[dsl_id] = dlcb;
        }
    } else {
        if (detail_debug)
            buginf("\nCan't allocate DLC");
           LIF_Error ("l2", "L2_GetDLCB", "Can't allocate DLCB.");
    }
   return (dlcb);
}

/************************************************************************
* L2_GetIntEvent
*************************************************************************
*
* Determine if an internal "event" has occured
* Return: TRUE, if there is an internal event, FALSE otherwise
*
*/
PRIVATE BOOLEAN near L2_GetIntEvent()

{
   BOOLEAN more_event = TRUE;    /* is there another event to process? */

   if (DLCB->UI_Queue_Len &&
      ((DLCB->State == TEI_ASSIGNED) ||
      (DLCB->State == AWAITING_ESTABLISHMENT)))
      {
      L2_Event = UI_FRAME_QUEUED_UP;
      }
   else if (DLCB->I_Queue_Len && !L2_NeedMail)
      {
      L2_Event = I_FRAME_QUEUED_UP;
      }
   else if (DLCB->Ack_Pending)
      {
      L2_Event = ACK_PENDING;
      }
   else if (DLCB->DL_Release_Request &&
      ((DLCB->State == MULTIPLE_FRAME_ESTABLISHED) ||
      (DLCB->State == TIMER_RECOVERY)))
      {
      L2_Event = DL_REL_REQ;
      DLCB->DL_Release_Request = FALSE;
      }
   else
      {
      more_event = FALSE;
      }

   return (more_event);
}

/************************************************************************
* L2_Go
*************************************************************************
*
* Process a L2 event.  Because layer 2 implements many protocols and
* can receive events from many sources, it is important to understand
* which parameters are applicable for each source/protocol pair.  The
* following table lists the applicable parameters for each pair along
* with its meaningful values.  For example, if the protocol is V120 and
* the event came from 'above' (L3_V), the only meaningful address parameter
* is the LLI.
*
*
* Protocol: L2P_LAPD       L2P_LAPD       L2P_LAPB       L2P_V120
*
*           ----------     ----------     ----------     ----------
*   L3      |L3-Q931 |     | P3-X25 |     | P3-X25 |     |L3-V120 |
*           ----------     ----------     ----------     ----------
*
*           SAPI=0         SAPI=16       SAPI=NA        SAPI=NA
*           TEI =NA        TEI =NA       TEI =NA        TEI =NA
*           LLI =NA        LLI =NA       LLI =NA        LLI =256-2047
*           CES =0-127     CES =0-127    CES =NA        CES =NA
*
*                               -----------
*   L2                          | L2      |
*                               -----------
*
*           SAPI=0,1,63    SAPI=16       SAPI=NA        SAPI=NA
*           TEI =0-127     TEI =0-127    TEI =NA        TEI =NA
*           LLI =NA        LLI =NA       LLI =NA        LLI =256-2047
*           CES =NA        CES =NA       CES =NA        CES =NA
*
*           ----------     ----------    ----------     ----------
*   HDLC    | HDLC_D |     | HDLC_D |    | HDLC_B |     | HDLC_V |
*           ----------     ----------    ----------     ----------
*
*   NA = Not Applicable
*
*/
PUBLIC void L2_Go (proto, source_id, event, dsl_id, sapi, ces, lli, chan,
   pkt, network, free_pkt_ok)

INT16   proto;                                             /* protocol */
INT16   source_id;                                   /* mail source ID */
INT16   event;                                      /* primitive event */
int     dsl_id;                             /* Digital Subscriber Loop */
BYTE    sapi;                                                  /* SAPI */
BYTE    ces;                                                    /* CES */
INT16   lli;                                        /* logical link id */
INT16   chan;                                    /* layer 1 channel ID */
PKT_ID  pkt;                                            /* data packet */
BYTE    network;                               /* network or user flag */
BOOLEAN free_pkt_ok;                      /* can this packet be freed? */
{
   BOOLEAN from_hdlc = FALSE;       /* source is a layer 2 driver task */
   BOOLEAN from_l3   = FALSE;              /* source is a layer 3 task */
   BOOLEAN find_dlcb = TRUE;                    /* must dlcb be found? */

   /* 
    * Initialize global variables.
    */
   if (detail_debug)
      buginf("\n L2_Go(): DSL_Id=%d, SAPI=%d, CES=%d, Event=%x",
         dsl_id, sapi, ces, event);
   L2_Protocol = proto;
   L2_PktIn = pkt;
   L2_FreePktIn = (pkt ? TRUE : FALSE);
   L2_FreePktOk = free_pkt_ok;
#ifdef L2_FLOW_CTRL
   /* The flow control parameter of a DL_DATA_CONF primitive
   *  is placed in the "free_pkt_ok" field of the primitive.
   */
   L2_TxAck = free_pkt_ok;
#endif
   L2_DSL_Chan = chan;
   L2_DSL_Id = dsl_id;
   L2_Network = network;
   L2_NeedMail = FALSE;

   /* Set the global variables L2_Sapi, L2_Ces, L2_Tei and L2_Event
   *  based on the source_id of the packet
   */
   if (detail_debug)
       buginf("\n in L2_Go source_id is %x ", source_id);
   switch (source_id)
      {
      case HDLC_V_ID:
      case HDLC_D_ID:
      case HDLC_B1_ID:
      case HDLC_B2_ID:
         from_hdlc = TRUE;
		 if ( L2_GetAddr (L2_PktIn, network) == ERR )   /* unpack addr */
            {
            L2_Event = UNDEFINED;                      /* ignore frame */
            }
         else
            {
			L2_Event = L2_GetCntrl (L2_PktIn); /* unpack control field */
            }
         break;
      case ME_ID:
         /* NOTE:  MDL_ASS/REM_REQ primitives are a special case in
         *         that the SAPI, CES and TEI are packed in the info
         *         portion of their packets.  Unpack them here.
         */
         if (event == MDL_ASS_REQ || event == MDL_REM_REQ)
            {
            L2_Sapi = L2_PktIn->Info [0];
            L2_Ces = L2_PktIn->Info [1];
            L2_Tei = L2_PktIn->Info [2];
            L2_Event = event;
            }
         else
            {
            L2_Sapi = sapi;
            L2_Ces = ces;
            L2_Tei = L2_CesToTei (L2_Ces, L2_DSL_Chan);
            L2_Event = event;
            }
         break;
      case V3_ID:
         from_l3 = TRUE;
         L2_Sapi = (BYTE) (lli >> 7);           /* convert LLI to SAPI */
         L2_Ces = (BYTE) (lli & 0x007F);                    /* and CES */
         L2_Tei = L2_Ces;                                   /* and TEI */
         L2_Event = event;
         break;
      case L3_ID:
         from_l3 = TRUE;
         L2_Sapi = Q931_SAPI;
         L2_Ces = ces;
         L2_Tei = L2_CesToTei (L2_Ces, L2_DSL_Chan);
         L2_Event = event;
         break;
      case P3_ID:
         from_l3 = TRUE;
         if (L2_Protocol == L2P_LAPB)             /* X.25 on B channel */
            {
            L2_Sapi = LAPB_SAPI;                          /* fake SAPI */
            L2_Ces = LAPB_CES;                             /* fake CES */
            L2_Tei = LAPB_TEI;                             /* fake TEI */
            L2_Event = event;
            }
         else                      /* must be LAPD (X.25 on D channel) */
            {
            L2_Sapi = X25_SAPI;
            L2_Ces = ces;
            L2_Tei = L2_CesToTei (L2_Ces, L2_DSL_Chan);
            L2_Event = event;
            }
         break;
      case L2_ID:
         /* The mail is a timer from ourselves, and because of some
         *  design problems we need to do some fancy foot work here.
         *  Here are the facts of the problem:
         *
         *  1) Layer 2 uses a global variable, "L2_Protocol", to
         *  determine the protocol being used (LAPB, LABD, V120).
         *  This variable is set to the value taken from each
         *  primitive mailed to Layer 2 (which requires everyone
         *  that talks to layer 2 to know  what layer 2 protocol
         *  is being used).
         *
         *  2) LIF timers do no accept a "protocol" field like
         *  other mailed primitives.
         *
         *  This means that at this point, the variable L2_Protocol,
         *  which was set above, is full of garbage.  Fortunatly
         *  the protocol value is stored in each dlcb.  Therefore,
         *  We will perform a search for the dlcb here.  In addition
         *  to taking the L2_Protocol out of the dlcb, we will also
         *  take the TEI value from it.
         *
         */
         L2_Sapi = sapi;
         L2_Ces = ces;
         DLCB = L2_SearchDLCB (L2_DSL_Id, L2_Sapi, L2_Ces, L2_DSL_Chan);
         if (DLCB == NULL)
            {
            L2_Event = UNDEFINED;      /* no dlcb found - ignore frame */
            }
         else
            {
            L2_Event = event;
            L2_Protocol = DLCB->L2_Protocol;
            L2_Tei = DLCB->Tei;
            }
         /* signal that a dlcb has already been located
         */
         find_dlcb = FALSE;
         break;
      default:
         LIF_Fatal("L2", "L2_Go", "UNKNOWN SOURCE ID");
         break;
      }

   /* Terminal loopback test will be executed by the network if SAPI = 17
    * and switch type is DMS100.
    */
   if (((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE))
                                && (L2_Sapi == TERM_LOOP_SAPI)) /*===*/
      {
      L2_TermTest (source_id, L2_PktIn);
      }
   else
      {
#ifdef DEBUG
      printf("L2_Go(): DSL_Id=%d, SAPI=%d, CES=%d, TEI=%d, Event=%x\n",
         L2_DSL_Id, L2_Sapi, L2_Ces, L2_Tei, L2_Event);
#endif
      if (L2_Tei == BROADCAST_TEI) {
         /*
          * ts014 mars homologation, should ignore frames coming in
          * on TEI 127
          */ 
         if (SwitchType == PRI_TS014_STYPE) {
             DLCB = NULL;
             goto bailout;
         } 
#ifdef VN2
         if (from_hdlc)
            {
            switch (L2_Event)
               {
               case CNTRL_FIELD_ERR:
               case INFO_NOT_PERMITTED:
               case I_FRAME_TOO_LONG:
               case INCORRECT_LENGTH:
                  DLCB=NULL;
                  break;
               case UI:
               default:
                  L2_Broadcast (from_hdlc);
                  break;
               }
            }
         else
           	L2_Broadcast (from_hdlc);
#else
         L2_Broadcast (from_hdlc);
#endif
         }
      else
         {
		 /* === added for 1tr6 certif, ignore a UI w/out TEI == 127 */
         if ((SwitchType == BRI_1TR6_STYPE) && (L2_Event == UI)) {
              DLCB=NULL;
              goto bailout;
			}

         /* if find_dlcb is true, we must try to locate a dlcb now.
         *  if the flag is false, the search has already been performed.
         */
         if (find_dlcb)
            {
            DLCB = L2_SearchDLCB (L2_DSL_Id, L2_Sapi, L2_Ces, L2_DSL_Chan);
            }
         if (DLCB == NULL)
            {
            /* If the event is from a Layer 3 task, a new DLCB is allocated.
             * Otherwise, check to see if we know about this TEI/CES.  If so,
             * a new DLCB is also allocated and the state is advanced to
             * TEI_ASSIGNED.
             */
            if ( from_l3 )
               {
               DLCB = L2_GetDLCB (L2_DSL_Id, L2_Ces, network);
               }
            else if (( L2_Tei != UNASSIGNED ) && ( L2_Ces != UNASSIGNED ) &&
               ( L2_Event != UNDEFINED ))
               {
               if (( DLCB = L2_GetDLCB( L2_DSL_Id, L2_Ces, network )) != NULL )
                  DLCB->State = TEI_ASSIGNED;
               }
            }
         }
/* === for 1tr6 certif */
bailout:
      if (detail_debug)
          buginf("\n in L2_Go at bailout DLCB is  %x ", DLCB);
      if (DLCB != NULL)
         {
         /*
          * process external event (may cause internal event)
          */
         if (detail_debug)
             buginf("\n in L2_Go going to process event");
         L2_ProcessEvent ();

         /* process internal event(s)
         */
         while (L2_GetIntEvent ())
            L2_ProcessEvent ();

         /* if data link is now idle, free it
         */
         switch (L2_Protocol)
            {
            case L2P_V120:
               if (DLCB->State == TEI_ASSIGNED)
                  {
                  L2_FreeDLCB(DLCB, dsl_id);
                  DLCB = NULL;
                  }
               break;
            default:
               if (DLCB->State == TEI_UNASSIGNED)
                  {
                  L2_FreeDLCB(DLCB, dsl_id);
                  DLCB = NULL;
                  }
               break;
            }
         }

      if (L2_FreePktIn && L2_FreePktOk)
         {
#ifdef DEBUG
         printf("FREE Incoming Packet\n");
#endif
         /* release the packet
         */
         LIF_FreePkt (L2_PktIn);
         }
#ifdef BRI_DMS100
      }
#endif

#ifdef DEBUG
   if (DLCB == NULL)
      printf("L2_Go() end: no dlcb\n");
   else
      printf("L2_Go() end: State=%d\n", DLCB->State);
#endif
}

/************************************************************************
* L2_Init
*************************************************************************
*
* Initialize L2 variables
*
*/
PUBLIC void L2_Init ()

{
   INT16 ix;                                            /* loop counter */
   BOOLEAN is_pri = TRUE;

   /*
    * initialize protocol defaults
    */
   DEFAULT_T200 = 1000;
   DEFAULT_N200 = 3;

   switch(SwitchType) {
      case PRI_1TR6_STYPE:
      case PRI_4ESS_STYPE:
      case PRI_NTT_STYPE:
          DEFAULT_T203 = 30000;
          DEFAULT_K = 7;
          break;

      case PRI_TS014_STYPE: 
      case PRI_NET5_STYPE:
      case PRI_DMS100_STYPE:
          DEFAULT_T203 = 10000;
          DEFAULT_K = 7;
          break;

      case PRI_5ESS_STYPE:
          DEFAULT_T203 = 30000;
          DEFAULT_K = 1;
          break;

      case BRI_5ESS_STYPE:
      case BRI_NI1_STYPE:
          DEFAULT_T203 = 30000;
          DEFAULT_K = 1;
          is_pri = FALSE;
          break;

      case BRI_NET3_STYPE:
      case BRI_DMS100_STYPE:
      case VN2_STYPE:
      case VN3_STYPE:
      case KDD_STYPE:
      case NTT_STYPE:
      case BRI_1TR6_STYPE:
      case BRI_TS013_STYPE:
      default:
          DEFAULT_T203 = 10000;
          DEFAULT_K = 1;
          is_pri = FALSE;
          break;
   }

   for (ix=0; ix<MAX_DSL; ix++) {
      /* Initialize DLCB headcell array.
       */
      DLCB_Head[ix] = NULL;
      DLCB_Tail[ix] = NULL;

      /*
       * Setup the Layer defaults
       */
      isdn_sys_constants[ix].T200 = DEFAULT_T200;
      isdn_sys_constants[ix].N200 = DEFAULT_N200;
   }
   DLCB        = NULL;
   L2_NeedMail = TRUE;

#ifdef USER
   /* initialize the TEI_to_CES conversion table (defined in TEI_CES.H)
   */
   for (L2_DSL_Id = 0; L2_DSL_Id < MAX_DSL; L2_DSL_Id++) {
      for (ix=0; ix<MAX_LOGICAL_LINKS; ix++) {
         TeiCesTable[L2_DSL_Id][ix].Ces      = UNASSIGNED;
         TeiCesTable[L2_DSL_Id][ix].Tei      = UNASSIGNED;
         TeiCesTable[L2_DSL_Id][ix].DSL_Chan = UNASSIGNED;
      }
      /* Initialize broadcast CES/TEI for each DSL -
       * assume only SIGNALING channel
       */
      L2_AddTeiCes (L2_DSL_Id, BROADCAST_TEI, BROADCAST_CES, SIGNAL_CHAN_ID);
   }
#endif


#ifdef BRI
#ifdef USER
#ifndef TASK_ME
/* If no Management Entity is linked with this module during
 *  application testing, no TEI assignment functionality is available.
 *  Here, the USER side gets a DLCB that is already in TEI_ASSIGNED state.
 */
 if (!is_pri) {
   L2_Sapi = Q931_SAPI;
   L2_Ces = 1;
   L2_Tei = 1;
   L2_DSL_Chan = SIGNAL_CHAN_ID;
   for (L2_DSL_Id = 0; L2_DSL_Id < MAX_DSL; L2_DSL_Id++) {
      DLCB = L2_GetDLCB (L2_DSL_Id, L2_Ces, FALSE);
      DLCB->State = TEI_ASSIGNED;
      L2_AddTeiCes (L2_DSL_Id, L2_Tei, L2_Ces, SIGNAL_CHAN_ID);
   }
 }
#endif                                                      /* TASK_ME */
#ifdef TIMING_TESTS
   L2_Sapi = Q931_SAPI;
   L2_Ces = 1;
   L2_Tei = 1;
   L2_DSL_Chan = SIGNAL_CHAN_ID;
   for (L2_DSL_Id = 0; L2_DSL_Id < MAX_DSL; L2_DSL_Id++) {
      DLCB = L2_GetDLCB (L2_DSL_Id, L2_Ces, FALSE);
      DLCB->State = MULTIPLE_FRAME_ESTABLISHED;
      L2_AddTeiCes (L2_DSL_Id, L2_Tei, L2_Ces, SIGNAL_CHAN_ID);
   }
#endif                                                 /* TIMING_TESTS */
#endif                                                         /* USER */
#endif                                                          /* BRI */

#ifdef PRI
/*
 * Primary Rate does not support TEI assignment.  Here, we get a DLCB
 *  that is already in TEI_ASSIGNED state.  For Primary Rate, the TEI
 *  value is 0 as per 41449 III-7.
 */
 if (is_pri) {
   L2_Sapi = Q931_SAPI;
   L2_Ces = 1;                        /* Rule: CES is always 1 for PRI */
   /*
    * NOTE: this value is based on subscription. It agrees with 41449
    */
   L2_Tei = 0;
   L2_DSL_Chan = SIGNAL_CHAN_ID;

   /*
    * Initialize one DLCB for each subscriber interface.
    */
   for ( L2_DSL_Id=0; L2_DSL_Id<MAX_DSL; L2_DSL_Id++ ) {
      /*
       * Get one DLCB for each DSL
       */
      DLCB = L2_GetDLCB (L2_DSL_Id, L2_Ces, FALSE);
      DLCB->State = TEI_ASSIGNED;
      L2_AddTeiCes (L2_DSL_Id, L2_Tei, L2_Ces, SIGNAL_CHAN_ID);
   }
  }   
#endif

   L2_DSL_Id = 0;
}

/************************************************************************
* L2_InitDLCB
*************************************************************************
*
* Initialize a data link entity structure
*
* Return:   NOERR if all went well;
*           ERR, otherwise.
*/
PRIVATE INT16 near L2_InitDLCB (dlcb, dsl_id, ces, network)

DLCB_ID dlcb;                              /* address of DLCB to init. */
int  dsl_id;                             /* Digital Subscriber Loop ID */
BYTE ces;                                                       /* CES */
BYTE network;                                     /* network/user flag */
{
   int i;

   /* Initialize all timers to ERR value, which will flag the
   *  timers as not allocated if we reach the "err_no_timer"
   *  error handler.
   */
   dlcb->T200_Handle = ERR;
   dlcb->T203_Handle = ERR;
   if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
       dlcb->T204_Handle = ERR;

   if ((dlcb->T200_Handle = LIF_AddTimer (L2_ID, L2_ID, T200_EXPIRY,
      dsl_id, L2_Sapi, L2_Ces, 0, L2_DSL_Chan, 
	  isdn_sys_constants[dsl_id].T200)) == ERR)
      goto err_no_timer;
   if ((dlcb->T203_Handle = LIF_AddTimer (L2_ID, L2_ID, T203_EXPIRY,
      dsl_id, L2_Sapi, L2_Ces, 0, L2_DSL_Chan, DEFAULT_T203)) == ERR)
      goto err_no_timer;
   if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE) {
       if ((dlcb->T204_Handle = LIF_AddTimer (ME_ID, ME_ID, T204_EXPIRY,
          dsl_id, L2_Sapi, L2_Ces, 0, L2_DSL_Chan, DEFAULT_T204)) == ERR)
          goto err_no_timer;
   }

   /* L2P_LAPB and L2P_V120 have no TEI assignment;
   *  begin in TEI_ASSIGNED
   */
   switch (L2_Protocol)
      {
      case L2P_LAPB:
         dlcb->L2_Protocol = L2P_LAPB;
         dlcb->Mod = LAPB_MOD;
         dlcb->State = TEI_ASSIGNED;
         break;
      case L2P_V120:
         dlcb->L2_Protocol = L2P_V120;
         dlcb->Mod = LAPD_MOD;
         dlcb->State = TEI_ASSIGNED;
         break;
      case L2P_LAPD:
         dlcb->L2_Protocol = L2P_LAPD;
         dlcb->Mod = LAPD_MOD;
         dlcb->State = TEI_UNASSIGNED;
         break;
      default:
         break;
      }

   dlcb->Sapi     = L2_Sapi;
   dlcb->Tei      = L2_Tei;
   dlcb->DSL_Chan = L2_DSL_Chan;
   dlcb->Ces      = ces;
   dlcb->Network  = network;
   dlcb->K        = (dlcb->Sapi == X25_SAPI) ? DEFAULT_X25_K : DEFAULT_K;
   dlcb->N200     = (BYTE) isdn_sys_constants[dsl_id].N200;
   dlcb->RC       = 0;
   dlcb->VA       = 0;
   dlcb->VS       = 0;
   dlcb->VR       = 0;
#ifdef L2_MREJ
   dlcb->VM       = 0;
#endif
#ifdef L2_SREJ
   dlcb->VR_High  = 0;
#endif
   dlcb->Own_Receiver_Busy  = FALSE;
   dlcb->Peer_Receiver_Busy = FALSE;
   dlcb->Rej_Recovery       = FALSE;
   dlcb->Ack_Pending        = FALSE;
#ifdef L2_FLOW_CTRL
   if (L2_Protocol == L2P_V120)
      dlcb->L3_Flow_Ctrl    = TRUE;
   else
      dlcb->L3_Flow_Ctrl    = FALSE;
   dlcb->L3_RxAck           = L3_QUEUE_SIZE;
#endif
   dlcb->DL_Release_Request = FALSE;
   dlcb->L3_Initiated       = FALSE;
   dlcb->Able_to_Establish  = TRUE;      /* FFS: is this ever cleared? */

   /* initialize I/UI tx queue
   */
   dlcb->UI_Queue_Len = 0;
   dlcb->I_Queue_Len  = 0;

   /* initialize connection management fields
   */
   dlcb->CM_State  = CM_IDLE;
   dlcb->CM_RC     = 0;
   for (i=0; i<MAX_CM_ERR; i++)
      dlcb->CM_Err_Count [i] = 0;
   dlcb->N201_Rcv  = DEFAULT_N201_R;
   dlcb->N201_Xmit = DEFAULT_N201_X;
   dlcb->T200      = isdn_sys_constants[dsl_id].T200;

   dlcb->Next      = NULL;

   return NOERR;

err_no_timer:

#ifdef DISPLAY_LOCAL_ERR
   LIF_Error ("l2", "L2_InitDLCB", "Can't add timer.");
#endif
   if (dlcb->T200_Handle == ERR)
      LIF_RemoveTimer(dlcb->T200_Handle);
   if (dlcb->T203_Handle == ERR)
      LIF_RemoveTimer(dlcb->T203_Handle);
   if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE) {
       if (dlcb->T204_Handle == ERR)
          LIF_RemoveTimer(dlcb->T204_Handle);
   }
   return ERR;
}

/************************************************************************
* L2_Mail
*************************************************************************
*
* Mail an event to another task.
*
* Parameter dest_id is not necessarily the id of the task to receive
* the event; this routine may modify dest_id depending on the value
* of the globals L2_Protocol and L2_Sapi (which where taken out of
* the primitive that was passed to L2_Go, the main entry point of
* the Layer 2 module).
*
* If the dest_id passed in is L3_ID, then it is possible that the
* true destination of the event is P3_ID or V3_ID (both of which
* are higher layers).
*
* If the dest_id is HDLC_D_ID, then it is possible that the true
* destination of the event is HDLC_V_ID or HDLC_B1_ID or HDLC_B2_ID
* (all of which are lower layers).
*
* Notice that L3_ID just means 'up', and HDLC_D_ID means 'down'.
* Besides L3_ID and HDLC_D_ID, the only other dest_id that should
* be passed in is ME_ID.
*
* It would probably be best to eliminate the dest_id parameter and
* let routine determine the dest_id itself, based on the event type,
* L2_Protocol and L2_Sapi.
*
*/
PUBLIC void L2_Mail (event, dest_id)

INT16 event;                                          /* event to mail */
INT16 dest_id;                                 /* destination layer ID */
{
   BOOLEAN free;                       /* may destination free packet? */
   BYTE state;

   if (L2_Protocol == L2P_V120 && dest_id == ME_ID)
      return;

   switch (event)
      {
      case I:
         /* retrieve the packet from the DLCB I queue
         */
         L2_PktOut = DLCB->TxQ [INDEX(DLCB->VS)].Pkt;
		 if(L2_PktOut == NULL) {		/* test added === */
			if (isdn_debug_ok(q921_debug, L2_DSL_Id))
				buginf("\nISDN Error: Missing TxQ Packet for VS %d (index %d)",
						DLCB->VS, INDEX(DLCB->VS));
			return;
		 }
         if ( L2_Protocol == L2P_LAPB )
            L2_PktOut->HeadLen = 2;
         else
            L2_PktOut->HeadLen = 4;
#ifdef DEBUG
         LIF_DispPkt (L2_PktOut);
#endif
         free = FALSE;
         break;

      case UI:
      case XID:
         /* simply pass along the packet
         */
         L2_PktOut = L2_PktIn;
         L2_PktOut->HeadLen = 3;
         free = TRUE;
         break;

      case DL_UI_DATA_IND:
         /* simply pass along the packet
         */
         L2_PktOut = L2_PktIn;
         free = TRUE;
         break;

#ifdef L2_FLOW_CTRL
      case DL_DATA_CONF:
         /* There is no data packet to pass along.
         *
         *  L2_RxAck is a global computed by function L2_AdvanceVA,
         *  and it represents the number of packets ack'd by network.
         *  We are improvising here and using the "free packet" field
         *  of the primitive as a means to pass this value to the user.
         */
         L2_PktOut = NULL;
         free = L2_RxAck;
         break;
#endif

      case DL_DATA_IND:
      case MDL_UI_DATA_IND:
      case MDL_XID_IND:
      case MDL_XID_CONF:
         /* simply pass along the packet
         */
         L2_PktOut = L2_PktIn;
         free = TRUE;
         break;

      case MDL_ASS_IND:
         /* SAPI is 1st octet, CES to assign is 2nd octet
         */
         L2_PktOut = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
         if (L2_PktOut == NULL)
            LIF_Fatal ("l2", "L2_Mail", "LIF_GetPkt failed");
         L2_PktOut->Info [0] = L2_Sapi;
         L2_PktOut->Info [1] = L2_Ces;
         L2_PktOut->InfoLen = 2;
         L2_Sapi = ME_SAPI;
         L2_Ces = BROADCAST_CES;
         free = TRUE;
         break;

   case MDL_ERR_IND:
         /* MDL error letter is 1st octet
         */
         L2_PktOut = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
         if (L2_PktOut == NULL)
            LIF_Fatal ("l2", "L2_Mail", "LIF_GetPkt failed");
         L2_PktOut->Info [0] = L2_MDL_Error;
         L2_PktOut->InfoLen = 1;
         free = TRUE;
         break;

      case RR:
      case RNR:
#ifdef L2_SREJ
      case SREJ:
#else
      case REJ:
#endif
         L2_PktOut = LIF_GetPkt (HEADER_BTYPE, DATA_OFFSET, TRUE);
         if (L2_PktOut == NULL)
            LIF_Fatal ("l2", "L2_Mail", "LIF_GetPkt failed");
         if ( L2_Protocol == L2P_LAPB )
            L2_PktOut->HeadLen = 2;
         else
            L2_PktOut->HeadLen = 4;
         free = TRUE;
         break;

      case SABME:
      case DM:
      case DISC:
      case UA:
         L2_PktOut = LIF_GetPkt (HEADER_BTYPE, UI_DATA_OFFSET, TRUE);
         if (L2_PktOut == NULL)
            LIF_Fatal ("l2", "L2_Mail", "LIF_GetPkt failed");
         free = TRUE;
         if ( L2_Protocol == L2P_LAPB )
            {
            if ( event == SABME )
               event = SABM;
            L2_PktOut->HeadLen = 2;
            }
         else
            {
            L2_PktOut->HeadLen = 3;
            }
         break;

      case FRMR:
         L2_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
         if (L2_PktOut == NULL)
            LIF_Fatal ("l2", "L2_Mail", "LIF_GetPkt failed");
         if ( L2_Protocol == L2P_LAPB )
            L2_PktOut->HeadLen = 2;
         else
            L2_PktOut->HeadLen = 3;
         L2_Put_FRMR (L2_PktOut->Info);
         L2_PktOut->InfoLen = 5;
         free = TRUE;
         break;

      case DL_EST_IND:
      case DL_EST_CONF:
         errmsg(&msgsym(LAYER2UP, ISDN),
               isdn_idblist[L2_DSL_Id]->hw_namestring, L2_Tei);
         L2_PktOut = NULL;
         free = FALSE;
         break;

      case DL_REL_IND:
      case DL_REL_CONF:
         errmsg(&msgsym(LAYER2DOWN, ISDN),
               isdn_idblist[L2_DSL_Id]->hw_namestring, L2_Tei);
         L2_PktOut = NULL;
         free = FALSE;
         break;

      default:
         free = FALSE;/* Make all compilers happy about initialization */
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error ("l2", "L2_Mail", "Unknown event.");
#endif
         break;
      }

   if (dest_id == HDLC_D_ID)                                  /* frame */
      {
      L2_PutAddr (L2_PktOut);                    /* pack address field */
      L2_PutCntrl (L2_PktOut, event);            /* pack control field */
      event = PH_DATA_REQ;          /* all frames become a PH_DATA_REQ */
      if (L2_Protocol == L2P_LAPD)
         dest_id = LIF_GetDriverID (L2P_LAPD, L2_DSL_Id, L2_DSL_Chan);
      }

   if (L2_Protocol == L2P_LAPB)                /* X25 on the B channel */
      {
      if (dest_id == L3_ID)
         dest_id = P3_ID;
      else if (dest_id == HDLC_D_ID)
         dest_id = LIF_GetDriverID (L2P_LAPB, L2_DSL_Id, L2_DSL_Chan);
      }
   else if (L2_Protocol == L2P_V120)
      {
      if (dest_id == L3_ID)
         dest_id = V3_ID;
      else if (dest_id == HDLC_D_ID)
         dest_id = LIF_GetDriverID (L2P_V120, L2_DSL_Id, L2_DSL_Chan);
      }
   else if (L2_Sapi == X25_SAPI)            /* L2_Protocol == L2P_LAPD */
      {
      if (dest_id == L3_ID)
         dest_id = P3_ID;                  /* must be X25 on D Channel */
      }

   /* send the mail via the layer interface.
   *  Note: LIF_SendPkt()'s return value is not checked for ERR/NOERR.
   */
   state = ( L2_Tei == BROADCAST_TEI ) ? 0 : DLCB->State;
   if(detail_debug)
   	  buginf("\n Sending data on BRI/PRI %d to %x", L2_DSL_Id, dest_id);
   LIF_SendPkt (L2_Protocol, L2_ID, state, dest_id, event, L2_DSL_Id,
      L2_Sapi, L2_Ces, ((L2_Sapi << 7) | L2_Tei), 0, L2_DSL_Chan,
      free, L2_PktOut, NOW, REGULAR_MAIL);
}

/************************************************************************
* L2_PutAddr
*************************************************************************
*
* Put address fields into packet frame header.
* (see Figure 5 and Table 1/Q.921 or X.25/2.4.2)
*
*/
PRIVATE void near L2_PutAddr (pkt)

PKT_ID pkt;                                                  /* packet */
{
   BYTE *h;                             /* frame header part of packet */

   h = pkt->Header;

   if (L2_Protocol == L2P_LAPB)
      {
      h[0] = 0x01;                                   /* set 'LAPB' bit */
      }
   else
      {
      h[0] = L2_Sapi << 2;
      h[1] = (L2_Tei << 1) | 0x01;
      }

   /* Symmetric command/response bit; the user and network both
   *  set this bit the same.
   */
   if(Network_side) {
      if(L2_Command)
         h[0] |= 0x02;                                     /* set C/R bit */
   } else {
      if(L2_Protocol == L2P_V120) {
         if(!L2_Command)
            h[0] |= 0x02;                                  /* set C/R bit */
      } else {
         if(L2_Network) {                                      /* network */
            if(L2_Command)
               h[0] |= 0x02;                               /* set C/R bit */
         } else {                                                    /* user */
            if(!L2_Command)
               h[0] |= 0x02;                               /* set C/R bit */
         }
      }
   }
}

/************************************************************************
* L2_PutCntrl
*************************************************************************
*
* Put the control byte(s) of a command / response.
* (see Table 5/Q.921 or X.25)
*
*/
PRIVATE void near L2_PutCntrl (pkt, event)

PKT_ID pkt;                                                  /* packet */
INT16 event;                                  /* command/response code */
{
   BYTE *h;                             /* frame header part of packet */
   BYTE *pf;                                      /* poll or final bit */
   BYTE n_of_r;                                                /* N(R) */

   h = pkt->Header;

   pf = (L2_Command ? &L2_P : &L2_F);           /* command or response */

   switch (event)
      {
      case I:                                           /* information */
      case RR:                                          /* supervisory */
      case RNR:
#ifdef L2_SREJ
      case SREJ:
#else
      case REJ:
#endif
#ifdef L2_SREJ
         n_of_r = (event == SREJ) ? L2_SREJ_NR : DLCB->VR;
#else
         n_of_r = DLCB->VR;
#endif
         if (L2_Protocol == L2P_LAPB)
            {
            h[1] = (event == I) ? (DLCB->VS << 1) : ((BYTE) event);
            h[1] |= n_of_r << 5;
            if (*pf)
               h[1] |= 0x10;
            }
         else
            {
            h[2] = (event == I) ? (DLCB->VS << 1) : ((BYTE) event);
            h[3] = n_of_r << 1;
            if (*pf)
               h[3] |= 0x01;
            }
         break;

      case SABME:                                        /* unnumbered */
      case SABM:
      case DM:
      case UI:
      case DISC:
      case UA:
      case FRMR:
      case XID:
         if (L2_Protocol == L2P_LAPB)
            {
            h[1] = (BYTE) event;
            if (*pf)
               h[1] |= 0x10;
            }
         else
            {
            h[2] = (BYTE) event;
            if (*pf)
               h[2] |= 0x10;
            }
         break;

      default:
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error ("l2", "L2_PutCntrl", "Unknown event.");
#endif
         break;
      }
}

/************************************************************************
* L2_Put_FRMR
*************************************************************************
*
* Put the information field byte(s) of an FRMR.
* (see Figure 6/Q.921)
*
*/
PRIVATE void near L2_Put_FRMR (info)

BYTE *info;                                   /* address of info field */
{
   *info++ = L2_FRMR_Control1;
   *info++ = L2_FRMR_Control2;
   *info++ = DLCB->VS << 1;
   *info   = DLCB->VR << 1;
   if (L2_FRMR_Command)
      info++;
   else
      *info++ |= 1;
   *info = L2_FRMR_Cause;
}

/************************************************************************
* L2_Readjust_Info
*************************************************************************
*
* Frames received by the drivers assume a header of 4 bytes.  In the
* case of unnumbered frames, the header is only 3 bytes.  In the case
* of LAPB, the header is only 2 bytes.  Therefore, if the unnumbered
* or the LAPB frame has info, some adjustment needs to be made.
* The necessary adjustment depends on whether the packet is in a
* SPLIT_FRAME or not.
*
* In SPLIT_FRAME mode, the leading byte(s) of the info is actually in
* the final bytes of the header.  To correct, shift the info down
* (correction) bytes and copy the last (correction) bytes of the header
* into the first (correction) bytes of the info.
*
* Otherwise, the pointer to the info points (correction) bytes into the
* info.  Subtract (correction) from the pointer.
*
* In both cases, subtract (correction) from the packet header length and
* add (correction) to the packet info length.
*
*/
PRIVATE void near L2_Readjust_Info (pkt, correction)

PKT_ID pkt;            /* address of packet whose info needs adjusting */
int correction;                                    /* Amount to adjust */
{

   /* Is info present (assuming the real HeadLen >= 2 and correction < 3) ?
   */
   if (pkt->HeadLen > 4-correction)
      {
      if (pkt->Info != NULL)
         pkt->Info -= correction;
      else                                        /* 3 or 4 byte frame */
         pkt->Info = &pkt->Header[pkt->HeadLen-correction];

      pkt->HeadLen -= correction;
      pkt->InfoLen += correction;
      }
   else
      {
      pkt->HeadLen = 4-correction;
      pkt->Info = NULL;
      }
}

/************************************************************************
* L2_RemoveTeiCes
*************************************************************************
*
* Remove an entry from the TeiCesTable.
*
*/
PUBLIC void  L2_RemoveTeiCes (tei, ces, chan)

BYTE tei;
BYTE ces;
INT16 chan;
{
   INT16 i;

   for (i=0; i<MAX_LOGICAL_LINKS; i++)
      {
      if ((TeiCesTable[L2_DSL_Id][i].Tei == tei)
         && (TeiCesTable[L2_DSL_Id][i].Ces == ces)
         && (TeiCesTable[L2_DSL_Id][i].DSL_Chan == chan))
         {
#ifdef USER
         TeiCesTable[L2_DSL_Id][i].Tei  = UNASSIGNED;
         TeiCesTable[L2_DSL_Id][i].Ces  = UNASSIGNED;
#endif
         TeiCesTable[L2_DSL_Id][i].DSL_Chan = UNASSIGNED;
         break;
         }
      }
}

/***************
* L2_ResetLink *
****************
*
* Reset all the DLCBs to TEI_ASSIGNED state.
*
*/
PUBLIC void L2_ResetLink (dsl_num)
int dsl_num;
{
    DLCB_ID dlcb, next;

    if (dsl_num >= MAX_DSL) {
        if (isdn_debug_ok(q921_debug, dsl_num))
            buginf("\nISDN ResetLink Error: dsl value exceeds MAX_DSL");
    } else {
        DLCB = DLCB_Head[dsl_num];
        while (DLCB) {
            dlcb = DLCB->Next;
            /*
             * For a shutdown on an interface keep the TEI assigned
             * since these switchtypes keep the TEI all the time.
             */
            if ((SwitchType == BRI_5ESS_STYPE) ||
                (SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) {
                DLCB->State = TEI_ASSIGNED;
            } else {
                if (DLCB->State >= TEI_ASSIGNED)
                    DLCB->State = TEI_UNASSIGNED;
            }

            LIF_StopTimer(DLCB->T200_Handle);
            LIF_StopTimer(DLCB->T203_Handle);
            L2_DiscardQs();
            DLCB->RC = 0;
            DLCB->VA = 0;
            DLCB->VS = 0;
            DLCB->VR = 0;
            DLCB->Own_Receiver_Busy = FALSE;
            DLCB->Peer_Receiver_Busy = FALSE;
            DLCB->L3_Initiated = FALSE;
            DLCB = dlcb;
        }
        /*
         * This to make sure that no DLCB are left hanging. It
         * can happen that Layer 1 doesn't come up and ME shuts
         * down. This strands DLCBs and leaks memory.
         */
        dlcb = DLCB_Head[dsl_num];
        while (dlcb != NULL) {
            next = dlcb->Next;
            if (dlcb->Tei >= BROADCAST_TEI)
                L2_FreeDLCB(dlcb, dsl_num);
            dlcb = next;
         }
    } 
}

/************************************************************************
* L2_SearchDLCB
*************************************************************************
*
* Search for the DLCB associated with the given SAPI and CES.
*
* Return:   Address of the DLCB, if found
*           NULL, otherwise
*
*/
PUBLIC DLCB_ID L2_SearchDLCB (dsl_id, sapi, ces, chan)

int  dsl_id;                             /* Digital Subscriber Loop Id */
BYTE sapi;                                       /* SAPI to search for */
BYTE ces;                                         /* CES to search for */
INT16 chan;                          /* physical channel to search for */
{
   DLCB_ID dlcb;                                       /* DLCB address */

   dlcb = DLCB_Head[dsl_id];

   while (dlcb != NULL &&
      ! (dlcb->Sapi == sapi && dlcb->Ces == ces && dlcb->DSL_Chan == chan))
      {
      dlcb = dlcb->Next;
      }

   return (dlcb);
}

/************************************************************************
* L2_ShutDown
*************************************************************************
*
* Shut down L2
*
*/
PUBLIC void L2_ShutDown ()

{
   DLCB_ID next;
   int l2_dsl_id;

   if (detail_debug)
       buginf("\n entering L2_ShutDown");
   for (l2_dsl_id=0; l2_dsl_id<MAX_DSL; l2_dsl_id++)
      {
      DLCB=DLCB_Head[l2_dsl_id];
      while (DLCB != NULL)
         {
         next = DLCB->Next;
         L2_FreeDLCB (DLCB, l2_dsl_id);
         DLCB = next;
         }
      DLCB = DLCB_Head[l2_dsl_id] = DLCB_Tail[l2_dsl_id] = NULL;
      }
}

/************************************************************************
* L2_TeiToCes
*************************************************************************
*
* Convert a TEI into its coresponding CES.
*
* Return:   coresponding CES, if found;
*           UNASSIGNED, otherwise.
*
*/
PRIVATE BYTE near L2_TeiToCes (tei, chan)

BYTE tei;                                                       /* TEI */
INT16 chan;                                      /* layer 1 channel ID */
{
   BYTE ces;
   INT16 i;

   if (L2_Protocol == L2P_V120)
      return tei;

   ces = UNASSIGNED;

   for (i=0; i<MAX_LOGICAL_LINKS; i++)
      {
      if ((TeiCesTable[L2_DSL_Id][i].Tei == tei) &&
         (TeiCesTable[L2_DSL_Id][i].DSL_Chan == chan))
         {
         ces = TeiCesTable[L2_DSL_Id][i].Ces;
         break;
         }
      }

   return (ces);
}

#ifdef BRI_DMS100
/************************************************************************
* L2_TermTest
*************************************************************************
*
* Perform the terminal loopback test (applies only to DMS100 equipment
* (SAPI = 17)).  This is assumed to be a loopback message destined from
* or to the USER Management Entity.
*
*/
PRIVATE void near L2_TermTest (source_id, pkt)

INT16 source_id;                       /* the ID of the source's layer */
PKT_ID pkt;                                      /* packet to loopback */
{
   int dest_id;                                           /* addressee */
   int event;                                         /* event to mail */

   L2_PktOut = pkt;

   if ( source_id == ME_ID )
      {
      event = PH_DATA_REQ;
      dest_id = HDLC_D_ID;
      }
   else                                         /* source == HDLC_D_ID */
      {
      event = MDL_DATA_REQ;
      dest_id = ME_ID;
      }

   /* Send the packet to the network or the ME.
   *  Note: LIF_SendPkt()'s return value is not checked for ERR/NOERR.
   */
   LIF_SendPkt (L2_Protocol, L2_ID, TEI_UNASSIGNED, dest_id, event,
      L2_DSL_Id, L2_Sapi, L2_Ces, ((L2_Sapi << 7) | L2_Tei),
      0, L2_DSL_Chan, TRUE, L2_PktOut, NOW, REGULAR_MAIL);
}
#endif                                                   /* BRI_DMS100 */

/*eof*/
