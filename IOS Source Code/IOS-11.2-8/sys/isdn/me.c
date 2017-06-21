/* $Id: me.c,v 3.3.52.3 1996/09/06 17:08:13 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/me.c,v $
 *------------------------------------------------------------------
 * me.c  ISDN Management Entity
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: me.c,v $
 * Revision 3.3.52.3  1996/09/06  17:08:13  dclare
 * CSCdi68318:  Australia ISDN PRI code changes for homologation
 * Branch: California_branch
 *
 * Revision 3.3.52.2  1996/05/01  14:51:30  hrobison
 * CSCdi46413:  MIP when used as PRI must be in slot 0-4
 * Branch: California_branch
 * Change PRI initialization: for the 7xxx platform the
 * dsl id is now assigned independently of the MIP card
 * slot/subunit location.  The number of PRI interfaces
 * per router is still limited to 10.  The dsl assignment
 * for the 4xxx platform is based on the slot location
 * and corrected to allow only 1 dsl id per slot.
 *
 * Read bug release notes for details on how the ISDN
 * debug output and ISDN show commands have changed.
 * The debug output now uses the hw_short_namestring
 * rather than PRI<dsl #> or BRI<dsl #>.  The show commands
 * now take the interface name or the dsl id as input.
 *
 * Revision 3.3.52.1  1996/04/30  23:07:56  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/21  04:01:01  dclare
 * CSCdi43785:  ISDN BRI for NET3 fails TEI Verify procedures in Taiwain.
 *
 * Revision 3.2  1995/11/17  17:41:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:16:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

* MODULE:       me.c                     
* DESCRIPTION:  ISDN Management Entity                                 
* VERSION:      3.21       

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

ME provides Management Entity functions to Layer 2.  This Management
Entity implementation is based on that of the AT&T 5ESS switch which
is an implementation of Q.920/Q.921.  The DMS-100 switch Management
Entity is a subset of this implementation.

ME provides NO funtions to Layer 1.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged as follows:
  - Management Entity functions (arranged alphabetically)
  - Layer Management SDL functions
  - Layer Management utility functions (arranged alphabetically)
  - Connection Management SDL functions
  - Connection Management utility functions (arranged alphabetically)

*************************************************************************
*************************************************************************
*/

#define     ME_C                                        /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/
#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "tei_ces.h"
#include    "me.h"
#include    "l2.h"
#include    "gp.h"
#include    "ccbri.h"
#include    "../wan/isdn.h"
#include    "../h/interface_private.h"
#include    "../h/logger.h"
#include "../isdn/msg_isdn.c"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* TEI management message types (see Table 8/Q.921)
*/
#define     ID_REQUEST      0x01                            /* request */
#define     ID_ASSIGNED     0x02                           /* assigned */
#define     ID_DENIED       0x03                             /* denied */
#define     ID_CHK_REQ      0x04                      /* check request */
#define     ID_CHK_RESP     0x05                     /* check response */
#define     ID_REMOVE       0x06                             /* remove */
#define     ID_VERIFY       0x07                             /* verify */

/* TEI assignment states (besides UNASSIGNED which is defined in lif.h)
*/
#define     OUTSTANDING     0x01         /* TEI request is outstanding */
#define     ASSIGNED        0x02                       /* TEI assigned */

/* Internal Events -- 1st nibble is 1; 2nd nibble is OSI layer (7)
*/
#define     MC_PARAM_REQ    0x1730          /* LM-CM parameter request */
#define     MC_PARAM_CONF   0x1740          /* CM-LM parameter confirm */
#define     MC_PARAM_ERR    0x1750            /* CM-LM parameter error */
#define     NO_INTERNAL     0x1760               /* no internal events */

#define     MEI             0x0F                    /* mngmt entity ID */

#define     FI              0x82                          /* format ID */
#define     GI              0x80                           /* group ID */
#define     GL0             0x00                       /* group length */
#define     GL1             0x0E
#define     PI0             0x05                     /* size (value 0) */
#define     PL0             0x02                   /* length (value 0) */
#define     PI1             0x06                     /* size (value 1) */
#define     PL1             0x02                   /* length (value 1) */
#define     PI2             0x07                     /* size (value 2) */
#define     PL2             0x01                   /* length (value 2) */
#define     PI3             0x09                     /* size (value 3) */
#define     PL3             0x01                   /* length (value 3) */

/* Random number generater
*/
#define     FIRST_SEED      5
#define     MULTIPLIER      17
#define     INCREMENT       1

/* Other
*/
#define     MAX_ME_CES         128           /* max CESs (connections) */

/* NOTE: An assumption is made that a packets message unit offset is
*        the same for all packets.  Although this is a departure from
*        Q.921, it allows for consistent handling of all packets.
*        The driver is responsible for adjusting incoming and outgoing
*        packets to conform to Q.921.
*/

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

/*
*   Anatomy of a Management Entity Packet (see Table 6 / Q.921):
*
*   Event           SAPI    CES                 Packet Info
*   -----           ----    ---                 -----------
*
*   ME <-> L2
*
*   MDL_ASS_REQ     63      Broadcast CES       SAPI
*   MDL_ASS_IND     ""      ""                   CES
*   MDL_REM_REQ     ""      ""                    TEI
*
*   MDL_ERR_IND     0-62    Non-broadcast CES   Error
*   MDL_ERR_RESP    ""      ""
*
*   MDL_UI_DATA_IND 63      Broadcast CES       ME Identifier
*   MDL_UI_DATA_REQ ""      ""                    Reference Number
*                                                   Message Type
*                                                     Action Indicator/E bit
*
*   MDL_XID_REQ     0-62    Non-broadcast CES   Parameter Info
*   MDL_XID_CONF    ""      ""                  ""
*
*   NOTE:  MDL_ASS/REM/ERR primitives use packets with 'normal' DATA_OFFSETs
*          whereas MDL_UI/XID primitives use packets with UI_DATA_OFFSETs.
*
*   ME <-> ME
*
*   T202_EXPIRY     63      Broadcast CES       ME Identifier
*                                                 Reference Number
*   T204_EXPIRY     0-62    Non-broadcast CES   No info
*
*   NOTE:  T202_EXPIRY primitive uses a packet with 'normal' DATA_OFFSET.
*          T204_EXPIRY primitive has no data packet.
*
*   NOTE:  T204 is not implemented for the VN2 switchtype  
*
*/

/* General ME variables
*/
PRIVATE INT16 ME_Event;                /* current event (primitive id) */
PRIVATE BYTE ME_DSL_Id;                                 /* current DSL */
PRIVATE BYTE ME_Sapi;                                  /* current SAPI */
PRIVATE BYTE ME_Ces;                                    /* current CES */
PRIVATE BYTE ME_State;                               /* LM or CM state */
PRIVATE INT16 ME_InternalEvent;              /* current internal event */

PRIVATE PKT_ID ME_PktOut;                         /* packet to be sent */
PRIVATE BYTE *ME_InfoOut;                 /* info portion of ME_PktOut */
PRIVATE BYTE *ME_InfoIn;            /* info portion of incoming packet */

/* The Layer Management is responsible for TEI Assignment and CES to TEI
 * mapping.  LMTR (layer management tei requests) is an array of structures
 * that maintains information needed to perform these functions.
*/
struct LayerMgmtTeiRequest
   {
   UINT16  Ri;                                     /* reference number */
   BYTE    RC;                                          /* retry count */
   BYTE    Sapi;                            /* service access point ID */
   BYTE    Tei;                               /* terminal equipment ID */
   BYTE    State;                           /* state of the assignment */
   int     T202_Handle;                                  /* T202 timer */
   BOOLEAN me_to_activate;   /* ME will activate, keeps ccbri doing it */
   }
PRIVATE LMTR [MAX_DSL][MAX_ME_CES];

PRIVATE UINT16 LM_ParamErr;                 /* parameter error counter */
PRIVATE UINT16 LM_Seed;                 /* seed for random number gen. */
PRIVATE BOOLEAN LM_FirstTime;          /* one time initialization flag */

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

PRIVATE void        near ME_Mail         (INT16, INT16);
PRIVATE void        near ME_Remove_TEI   (BYTE);
PRIVATE void	    near ME_Verify_TEI   (BYTE);

PRIVATE void        near LM_Go           (void);
PRIVATE void        near LM_MDL_ASS_IND  (void);
PRIVATE void        near LM_MDL_UI_DATA_IND (void);
PRIVATE void        near LM_MC_PARAM_CONF (void);
PRIVATE void        near LM_MC_PARAM_ERR (void);
PRIVATE void        near LM_T202_EXPIRY  (void);

PRIVATE BYTE        near LM_GetCes       (BYTE);
PRIVATE void        near LM_PutTeiMsg    (UINT16, BYTE, BYTE);
PRIVATE UINT16      near LM_GetRi        (void);
PRIVATE BYTE        near LM_Search       (UINT16);
#ifdef DEBUG
PRIVATE void        near LM_ShowTei      (void);
#endif

PRIVATE void        near CM_Go           (void);
PRIVATE void        near CM_MC_PARAM_REQ (void);
PRIVATE void        near CM_MDL_ERR_IND  (void);
PRIVATE void        near CM_MDL_XID_CONF (void);
PRIVATE void        near CM_T204_EXPIRY  (void);

PRIVATE void        near CM_GetParmMsg   (INT16 *, INT16 *, BYTE *, UINT32 *);
PRIVATE void        near CM_PutParmMsg   (INT16, INT16, BYTE, UINT32);

/*****************
* me_activate_ok * 
******************
*
* Returns flag to indicate if the ME Task will activate Layer 2 after
* an ID Verify or if CCBRI will do the activation.
*/
PUBLIC boolean me_activate_ok (int dsl, int ces)
{
    return (LMTR[dsl][ces].me_to_activate);
}

/***********************
* me_set_activate_flag *
************************
*
* Sets flag to TRUE if ME Task is going to activate, this will keep ccbri
* from attempting * to activate the interface.
*     T - ME to activate
*     F - ccbri to activate
*/
PUBLIC void me_set_activate_flag (int dsl, int ces, boolean act_flag)
{
    LMTR[dsl][ces].me_to_activate = act_flag;
}

/********
* ME_Go *
*********
*
* Process a management entity message.  Pass it either to the Layer
* Management or the Connection Management.  Afterward, process any
* internal events that may have occurred.
*
*/
PUBLIC void ME_Go (event, dsl_id, sapi, ces, pkt)

INT16 event;                      /* the event which has just occurred */
BYTE dsl_id;                             /* Digital Subscriber Loop ID */
BYTE sapi;                                                     /* SAPI */
BYTE ces;                                /* connection endpoint suffix */
PKT_ID pkt;                                         /* incoming packet */

{
   /* Initialize global variables
    */
   ME_Event = event;
   ME_Sapi = sapi;
   ME_Ces = ces;
   if (pkt != NULL)
      ME_InfoIn = pkt->Info;
   ME_DSL_Id = dsl_id;

   /* Packets for the Layer Management always have a SAPI == ME_SAPI.
    *  Packets for the Connection Management always have a SAPI != ME_SAPI.
    */
   if (sapi == ME_SAPI)
      LM_Go ();
   else
      CM_Go ();

   /* An internal event MAY occur as the result of LM_Go () or CM_Go ().
    */
   while (ME_InternalEvent != NO_INTERNAL)
      {
      ME_Event = ME_InternalEvent;
      switch (ME_InternalEvent)
         {
      case MC_PARAM_REQ:
         CM_Go ();
         break;

      case MC_PARAM_CONF:
         LM_Go ();
         break;

      case MC_PARAM_ERR:
         LM_Go ();
         break;

      default:
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error ("me", "ME_Go", "Unknown internal event.");
#endif
         ;
         }
      }

   /* NOTE: Incoming packets are freed (unless a timer), therefore,
    *        if a packet is to be mailed during the above processing,
    *        it must get a new packet and not use the incoming one.
    */
   if ( pkt && ((event & 0xFF00) != TIMER_MASK_ME) )
      LIF_FreePkt (pkt);
}

/**********
* ME_Init *
***********
*
* Initialize the Management Entity.
*
*/
PUBLIC void ME_Init ()
{
   BYTE  i;
   INT16 j;

   ME_InternalEvent = NO_INTERNAL;

   /* Initialize Layer Management
    */
   for (i=0; i<MAX_DSL; i++)
      {
      for (j=0; j<MAX_ME_CES; j++)
         {
         LMTR [i][j].Ri = 0;
         LMTR [i][j].RC = 0;
         LMTR [i][j].Sapi = UNASSIGNED;
         LMTR [i][j].Tei = UNASSIGNED;
         LMTR [i][j].State = UNASSIGNED;
         LMTR [i][j].T202_Handle = 0;
         }
      LMTR [i][BROADCAST_CES].Tei = BROADCAST_TEI;
      LMTR [i][BROADCAST_CES].State = ASSIGNED;
      }

#ifdef NON_AUTO
#ifdef USER
   /* NOTE:    Non-automatic TEI assignment is done here.
    *          In the future, these should be set by reading hardware
    *          addresses (equipment may have configurable DIP switch).
    */
   for (i=0; i<MAX_DSL; i++)
      {
#ifdef KDD_NTT
      LMTR [i][1].Tei = 0;      /* user equipment with CES=1 has TEI=0 */
      LMTR [i][1].Sapi = 0;
      LMTR [i][1].State = ASSIGNED;
      LMTR [i][2].Tei = 1;      /* user equipment with CES=2 has TEI=1 */
      LMTR [i][1].Sapi = 0;
      LMTR [i][2].State = ASSIGNED;
#else
      LMTR [i][1].Tei = 1;      /* user equipment with CES=1 has TEI=1 */
      LMTR [i][1].Sapi = 0;
      LMTR [i][1].State = ASSIGNED;
      LMTR [i][2].Tei = 2;      /* user equipment with CES=2 has TEI=2 */
      LMTR [i][1].Sapi = 0;
      LMTR [i][2].State = ASSIGNED;
#endif
      LMTR [i][63].Tei = 63;  /* user equipment with CES=63 has TEI=63 */
      LMTR [i][1].Sapi = 0;
      LMTR [i][63].State = ASSIGNED;
      }
#endif
#endif

   /*
    * Primary Rate does not support TEI assignment.  Here, we assign
    *  a TEI.  The TEI value is 0 as per 41449 III-7.  The CES is 1.
    */
   if ((SwitchType == PRI_5ESS_STYPE) || (SwitchType == PRI_DMS100_STYPE)
        || (SwitchType == PRI_4ESS_STYPE) || (SwitchType == PRI_NTT_STYPE)
        || (SwitchType == PRI_NET5_STYPE) || (SwitchType == PRI_TS014_STYPE)) {
     for (i=0; i<MAX_DSL; i++)
        {
        LMTR [i][1].Tei = 0;      /* user equipment with CES=1 has TEI=0 */
        LMTR [i][1].Sapi = 0;
        LMTR [i][1].State = ASSIGNED;
        }
   }
   LM_ParamErr = 0;
   LM_FirstTime = TRUE;
}

/**********
* ME_Mail *
***********
*
*  Mail an event to a destination.
*
*/
PRIVATE void near ME_Mail (event, dest_id)

INT16 event;                                          /* event to mail */
INT16 dest_id;                                 /* destination layer ID */
{
   LIF_SendPkt (L2P_LAPD, ME_ID, ME_State, dest_id, event, ME_DSL_Id, ME_Sapi,
      ME_Ces, 0, 0, 0, TRUE, ME_PktOut, NOW, REGULAR_MAIL);
}

/****************
* ME_Remove_TEI *
*****************
*
*  Remove a TEI.
*
*/
PRIVATE void near ME_Remove_TEI (ces)

BYTE ces;                                      /* ces of tei to remove */
{
    BYTE sapi;

    /*
     * Never remove a TEI in PRI
     * Only remove a TEI if it is an automatic TEI
     * and a BRI
     */
    if(detail_debug)
        buginf("\nentering ME_Remove_TEI");

    /*
     * Both PRI and BRI (static and automatic TEI's) need
     * to notify Layer 2 that we are going down. PRI was
     * leaving T203 running and the timer would fire after
     * PRI was deconfigured and we would crash. BRI was
     * fine all along.
     * CSCdi33719.
     */

    /*
     * prepare out packet
     */
    ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);

    if (ME_PktOut == NULL)
        LIF_Fatal("me", "ME_Remove_TEI", "LIF_GetPkt failed");

    ME_InfoOut = ME_PktOut->Info;
    ME_InfoOut [0] = LMTR [ME_DSL_Id][ces].Sapi;
    ME_InfoOut [1] = ces;
    ME_InfoOut [2] = LMTR [ME_DSL_Id][ces].Tei;
    ME_PktOut->InfoLen = 3;

    /*
     * prepare other mail variables
     */
    ME_Sapi = ME_SAPI;
    ME_Ces = BROADCAST_CES;
    ME_State = CM_TEI_MANAGE;

    /*
     * send remove request to L2
     */
    ME_Mail (MDL_REM_REQ, L2_ID);

    if (AUTO_TEI (LMTR [ME_DSL_Id][ces].Tei)) {
        /*
         * Clear the me/ccbri activation flag and make sure that
         * the T202 timer is stopped.
         */
        LMTR[ME_DSL_Id][ces].me_to_activate = FALSE;
        if (LMTR [ME_DSL_Id][ces].T202_Handle) {
            LIF_StopTimer(LMTR [ME_DSL_Id][ces].T202_Handle);
	    LIF_RemoveTimer(LMTR [ME_DSL_Id][ces].T202_Handle);
            LMTR[ME_DSL_Id][ces].T202_Handle = 0;
        }
        /*
         * unassign in layer management
         */
        errmsg(&msgsym(LAYER2DOWN, ISDN),
             isdn_idblist[ME_DSL_Id]->hw_namestring,
             LMTR [ME_DSL_Id][ces].Tei);
        sapi = LMTR [ME_DSL_Id][ces].Sapi;
        LMTR [ME_DSL_Id][ces].Ri = 0;
        LMTR [ME_DSL_Id][ces].RC = 0;
        LMTR [ME_DSL_Id][ces].Sapi = UNASSIGNED;
        LMTR [ME_DSL_Id][ces].Tei = UNASSIGNED;
        LMTR [ME_DSL_Id][ces].State = UNASSIGNED;
        LMTR [ME_DSL_Id][ces].T202_Handle = 0;

        /*
         * initiate automatic TEI assignment for a new TEI value.
         */
        assignment_state[ME_DSL_Id] = ASSIGNMENT_NOT_STARTED;
        CCBFIT_TerminalDown(ME_DSL_Id, ces);
    } else {
        /*
         * Non-automatic TEI assignment
         * Notify the equipment user for the need for corrective action.
         */
    }
}

/**************
* ME_ShutDown *
***************
*
* Shutdown the Management Entity.
*
*/
PUBLIC void ME_ShutDown (dsl)
int dsl;
{
   int ix;

   if (detail_debug)
       buginf("\n Entering ME_Shutdown");
   if(dsl != ALL_ISDN_INTERFACES)
       ME_DSL_Id = dsl;
   if(isdn_debug)
       buginf("\nISDN %s: Shutting down ME", 
	  DSL_TO_NAME(ME_DSL_Id));

   for (ix=0; ix<MAX_ME_CES; ix++) {
       /*
        * make sure that no timers have been allocated, this will
        * leak memory.
        */
       if (LMTR [ME_DSL_Id][ix].T202_Handle) {
          LIF_StopTimer(LMTR [ME_DSL_Id][ix].T202_Handle);
          LIF_RemoveTimer(LMTR [ME_DSL_Id][ix].T202_Handle);
          LMTR[ME_DSL_Id][ix].T202_Handle = 0;
       }
    }
    /*
     * VN2 keeps TEI around Layer 1 problems,
     * now keep the TEI around for ni1, dms and 5ess as well.
     */
   if(SwitchType != VN2_STYPE &&
      SwitchType != VN3_STYPE && SwitchType != BRI_NI1_STYPE &&
      SwitchType != BRI_5ESS_STYPE && SwitchType != BRI_DMS100_STYPE) {
      for (ix=0; ix<MAX_ME_CES; ix++) {
         if ( LMTR [ME_DSL_Id][ix].State == ASSIGNED &&
              LMTR [ME_DSL_Id][ix].Tei   != BROADCAST_TEI ) {
            ME_Remove_TEI (ix);
         }
      }
   }
   
   /*
    * This will clean up the LMTR (layer management tei requests)
    * data structure if outstanding TEI requests occur.
    */
    LM_PersDeact(dsl);

#ifdef EURO_ACT
	ME_PktOut = (PKT_ID) NULL; /* LIR engineers claim this is needed ?? */

   /* send proper deactivation messages to Layer 3 and Call Control.
   *  Use express mail in order to expedite treatment of deactivation.
   */
   if (detail_debug)
       buginf("\n sending MNL_DEACT_IND from me_shut");
   LIF_SendPkt (L2P_LAPD, ME_ID, 0, L3_ID, MNL_DEACT_IND, ME_DSL_Id, 0,
           1, 0, 0, 0, FALSE, ME_PktOut, NOW, EXPRESS_MAIL);

   if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
	   LIF_SendPkt (L2P_LAPD, ME_ID, 0, CC_ID, MAL_DEACT_IND, ME_DSL_Id, 0,
		   1, 0, 0, 0, FALSE, ME_PktOut, NOW, EXPRESS_MAIL);
#endif EURO_ACT
}

/* Layer Management SDLs
*/

/********
* LM_Go *
*********
*
* Layer Management (USER Side).
* FFS RESET primitive: reset all layers (MPH/DL/NL_RESET_REQ)
*/
PRIVATE void near LM_Go ()

{
   switch (ME_Event)
      {
   case MDL_ASS_IND:
      LM_MDL_ASS_IND ();
      break;

   case MDL_UI_DATA_IND:
      LM_MDL_UI_DATA_IND ();
      break;

   case MC_PARAM_CONF:
      LM_MC_PARAM_CONF ();
      break;

   case MC_PARAM_ERR:
      LM_MC_PARAM_ERR ();
      break;

   case T202_EXPIRY:
      LM_T202_EXPIRY ();
      break;

   default:
      break;
      }
}

/*****************
* LM_MDL_ASS_IND *
******************
*
* TEI assignment indication.
*
*/
PRIVATE void near LM_MDL_ASS_IND ()

{
   BYTE sapi;
   BYTE ces;
   UINT16 ri;
   PKT_ID timer_pkt;                              /* timed mail packet */

   sapi = ME_InfoIn [0];
   ces = ME_InfoIn [1];

   switch (LMTR [ME_DSL_Id][ces].State)
      {
   case UNASSIGNED:                        /* automatic TEI assignment */
		 if(detail_debug)
		 	buginf("\nLM_MDL_ASS_IND, sending out an ID_REQuest");
         {
         /* prepare timer packet
         */
         timer_pkt = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
         if (timer_pkt == NULL)
            LIF_Fatal("me", "LM_MDL_ASS_IND", "LIF_GetPkt failed");
#endif
         LMTR [ME_DSL_Id][ces].Ri = ri = LM_GetRi ();
         timer_pkt->Info [0] = MEI;
         timer_pkt->Info [1] = (BYTE) (ri >> 8);          /* high bits */
         timer_pkt->Info [2] = (BYTE) (ri & 0x00FF);       /* low bits */
         timer_pkt->InfoLen = 3;

         /* prepare timer/mail variables
         */
         ME_Sapi = ME_SAPI;
         ME_Ces = BROADCAST_CES;
         ME_State = CM_TEI_MANAGE;

         LMTR [ME_DSL_Id][ces].T202_Handle = LIF_AddTimerP (ME_ID, ME_ID,
            T202_EXPIRY, ME_DSL_Id, ME_Sapi, ME_Ces, 0, 0, DEFAULT_T202,
            timer_pkt);
         LIF_StartTimer( LMTR [ME_DSL_Id][ces].T202_Handle );

         if (detail_debug)
             buginf("\n  would be starting T202 here, handle %3d", LMTR [ME_DSL_Id][ces].T202_Handle);

         /* prepare out packet
         */
         ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
         if (ME_PktOut == NULL)
            LIF_Fatal("me", "LM_MDL_ASS_IND", "LIF_GetPkt failed");
#endif
         ME_InfoOut = ME_PktOut->Info;
         LM_PutTeiMsg(ri, ID_REQUEST, BROADCAST_TEI);
         ME_PktOut->InfoLen = 5;
#ifdef EURO_ACT
         assignment_state[ME_DSL_Id] = ASSIGNMENT_IN_PROGRESS;
#endif EURO_ACT
         ME_Mail (MDL_UI_DATA_REQ, L2_ID);

         LMTR [ME_DSL_Id][ces].Sapi = sapi;
         LMTR [ME_DSL_Id][ces].State = OUTSTANDING;
         }
      break;

   case OUTSTANDING:
      /*
       * TEI request already in progress. Just ignore.
       */
      if (detail_debug)
          buginf("\nme, TEI OUTSTANDING");
      break;

   case ASSIGNED:       /* non-automatic or repeat auto TEI assignment */
      /*
       *  Save the TEI and begin parameter negotiation.
       */
	  if(detail_debug)
	      buginf("\nme, TEI assigned");
      if ((DLCB = L2_SearchDLCB (ME_DSL_Id, sapi, ces,
         SIGNAL_CHAN_ID)) == NULL)
         {
#ifdef DISPLAY_LOCAL_ERR
         LIF_Error ("me", "LM_MDL_ASS_IND", "Can't find DLCB.");
#endif
         break;
         }
      DLCB->Tei = LMTR [ME_DSL_Id][ces].Tei;
      L2_AddTeiCes (ME_DSL_Id, DLCB->Tei, ces, SIGNAL_CHAN_ID);

      ME_Sapi = sapi;
      ME_Ces = ces;
#ifdef SUPPRESS_XID
      ME_InternalEvent = MC_PARAM_CONF;
#else
      ME_InternalEvent = MC_PARAM_REQ;
#endif
      break;

   default:                                      /* should never occur */
      break;
      }
}

/*********************
* LM_MDL_UI_DATA_IND *
**********************
*
* Unit data indication (response from TEI request for the USER side
* or USER request for a TEI.
*
*/
PRIVATE void near LM_MDL_UI_DATA_IND ()

{
   UINT16 ri;
   BYTE msg_type;
   BYTE ai;
   BYTE ces;
   INT16 i;
   boolean keepL2_up;

   keepL2_up = FALSE;
   ri = ((((UINT16) (ME_InfoIn [1])) << 8) | ((UINT16) (ME_InfoIn [2])));
   msg_type = ME_InfoIn [3];
   ai = ME_InfoIn [4] >> 1;

   switch (msg_type)
      {
   case ID_ASSIGNED:
      /*
       *  Ignore if not an automatic TEI.
       */
      if ( AUTO_TEI( ai ) )
         {
	 /*
	  *  First, check if this means we have to nuke an established TEI.
	  */
         if ((ces = LM_GetCes (ai)) != MAX_ME_CES) {
            if (LMTR [ME_DSL_Id][ces].State == ASSIGNED &&
                LMTR [ME_DSL_Id][ces].Tei   != BROADCAST_TEI) {
               ME_Remove_TEI (ces);
            }
         }

         /*
          *  Ignore if cannot find LMTR record.
          */
         if ((ces = LM_Search (ri)) != MAX_ME_CES)
            {
            /*
             *  If TEI is already assigned, remove.
             */
            if (LMTR [ME_DSL_Id][ces].State != ASSIGNED)
               {
               /*
                *  Assign TEI
                */
               LMTR [ME_DSL_Id][ces].Tei = ai;
               LMTR [ME_DSL_Id][ces].State = ASSIGNED;
               LIF_RemoveTimer( LMTR [ME_DSL_Id][ces].T202_Handle );
               LMTR [ME_DSL_Id][ces].T202_Handle = 0;
               LMTR[ME_DSL_Id][ces].me_to_activate = FALSE;
               /*
                *  Save the TEI and begin parameter negotiation.
                */
               ME_Sapi = LMTR [ME_DSL_Id][ces].Sapi;
               ME_Ces = ces;

               if ((DLCB = L2_SearchDLCB (ME_DSL_Id, ME_Sapi, ME_Ces,
                  SIGNAL_CHAN_ID)) == NULL)
                  {
#ifdef DISPLAY_LOCAL_ERR
                  LIF_Error ("me", "LM_MDL_UI_DATA_IND",
                             "Can't find DLCB.");
#endif
                  break;
                  }
               DLCB->Tei = ai;
               L2_AddTeiCes (ME_DSL_Id, ai, ces, SIGNAL_CHAN_ID);
#ifdef EURO_ACT
               assignment_state[ME_DSL_Id] = ASSIGNMENT_DONE;
#endif EURO_ACT
#ifdef SUPPRESS_XID
               ME_InternalEvent = MC_PARAM_CONF;
#else
               ME_InternalEvent = MC_PARAM_REQ;
#endif
               }
#ifdef VN2
            /* MODIFICATIONS FAX  -- ce qui suit en commentaire 
           else
               {
               ME_Remove_TEI (ces);
               }
            ** fin MODIFICATIONS FAX */ 
#else
            else
               {
               ME_Remove_TEI (ces);
               }
#endif EURO_ACT
            }
         }
      break;

   case ID_DENIED:                                       /* do nothing */
      break;                          /* try again with expiry of T202 */

   case ID_REMOVE:
      /*
       * For US switches, 5ess, dms100 and ni1, if the TEI is removed ask
       * for a new one and bring the link back up.
       */
      if (isdn_pu_establish && ((SwitchType == BRI_5ESS_STYPE) ||
         (SwitchType == BRI_NI1_STYPE) || (SwitchType == BRI_DMS100_STYPE))) {
          keepL2_up = TRUE;
      }

      if (ai == BROADCAST_TEI) {           /* remove ALL assigned TEIs */
          /*
           * Reverse the order of removal, this will allow the ces 1 to be the
           * last removed and the first one started.
           */
          for (i= MAX_ME_CES-1; i>=0; i--) {
            if (LMTR [ME_DSL_Id][i].State == ASSIGNED &&
                LMTR [ME_DSL_Id][i].Tei != BROADCAST_TEI) {
                ME_Remove_TEI(i);
            }
         }
         /*
          * Ask for a new TEI outside the loop, that way only get one.
          */
         if (keepL2_up) {
             LMTR[ME_DSL_Id][SIGNALING_CES].me_to_activate = TRUE;
             CCBFIT_TerminalInit(NL_PU_EST_IND, SIGNALING_CES, ME_DSL_Id);
         } else {
             LMTR[ME_DSL_Id][SIGNALING_CES].me_to_activate = FALSE;
         }
      } else {
         if ((ces = LM_GetCes (ai)) != MAX_ME_CES) {
            if (LMTR [ME_DSL_Id][ces].State == ASSIGNED &&
                LMTR [ME_DSL_Id][ces].Tei != BROADCAST_TEI) {
                /*
                 * Remove the tei here, ccbri will ask for a new one.
                 */
                ME_Remove_TEI(ces);
                if (keepL2_up) {
                    LMTR[ME_DSL_Id][ces].me_to_activate = TRUE;
                    CCBFIT_TerminalInit(NL_PU_EST_IND, ces, ME_DSL_Id);
                } else { 
                    LMTR[ME_DSL_Id][ces].me_to_activate = FALSE;
                }
            }
         }
      }
      break;

   case ID_CHK_REQ:
      if (ai == BROADCAST_TEI) {      /* respond for ALL assigned TEIs */
         /*
          * for mars ts014 homologation, ignore id_check_req
          */
         if (SwitchType != PRI_TS014_STYPE) { 
             for (i=0; i<MAX_ME_CES; i++) {
                if ((LMTR [ME_DSL_Id][i].Tei != UNASSIGNED) &&
                    (LMTR [ME_DSL_Id][i].Tei != BROADCAST_TEI)) {
                    /*
                     * prepare out packet
                     */
                    ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
                    if (ME_PktOut == NULL)
                        LIF_Fatal("me", "LM_MDL_UI_DATA_IND", "LIF_GetPkt failed");
#endif
                    ME_InfoOut = ME_PktOut->Info;
                    LMTR [ME_DSL_Id][i].Ri = ri = LM_GetRi ();
                    LM_PutTeiMsg(ri, ID_CHK_RESP, LMTR [ME_DSL_Id][i].Tei);
                    ME_PktOut->InfoLen = 5;

                    /*
                     * prepare other mail variables
                     */
                    ME_Sapi = ME_SAPI;
                    ME_Ces = BROADCAST_CES;
                    ME_State = CM_TEI_MANAGE;

                    ME_Mail (MDL_UI_DATA_REQ, L2_ID);
                }
            }
         }
      } else {
         if ((ces = LM_GetCes (ai)) != MAX_ME_CES)
            {
            if (LMTR [ME_DSL_Id][ces].Tei != UNASSIGNED)
               {
               if (LMTR [ME_DSL_Id][ces].T202_Handle) {
                  LIF_StopTimer(LMTR [ME_DSL_Id][ces].T202_Handle);
		  LIF_RemoveTimer(LMTR [ME_DSL_Id][ces].T202_Handle);
                  LMTR[ME_DSL_Id][ces].T202_Handle = 0;
               }
               /* prepare out packet
                */
               ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
               if (ME_PktOut == NULL)
                  LIF_Fatal("me", "LM_MDL_UI_DATA_IND", "LIF_GetPkt failed");
#endif
               ME_InfoOut = ME_PktOut->Info;
               LMTR [ME_DSL_Id][ces].Ri = ri = LM_GetRi ();
               LM_PutTeiMsg(ri, ID_CHK_RESP, LMTR [ME_DSL_Id][ces].Tei);
               ME_PktOut->InfoLen = 5;

               /* prepare other mail variables
                */
               ME_Sapi = ME_SAPI;
               ME_Ces = BROADCAST_CES;
               ME_State = CM_TEI_MANAGE;

               ME_Mail (MDL_UI_DATA_REQ, L2_ID);
               }
            }
      }
      break;

   default:
      break;
      }
}

/*******************
* LM_MC_PARAM_CONF *
********************
*
* Parameter negotiation confirmed.
*
*/
PRIVATE void near LM_MC_PARAM_CONF ()

{
   ME_InternalEvent = NO_INTERNAL;             /* reset internal event */


   /* prepare out packet
    */
   ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
   if (ME_PktOut == NULL)
      LIF_Fatal("me", "LM_MC_PARAM_CONF", "LIF_GetPkt failed");
#endif
   ME_InfoOut = ME_PktOut->Info;
   ME_InfoOut [0] = ME_Sapi;
   ME_InfoOut [1] = ME_Ces;
   ME_InfoOut [2] = LMTR [ME_DSL_Id][ME_Ces].Tei;
   ME_PktOut->InfoLen = 3;

   /* prepare other mail variables
    */
   ME_Sapi = ME_SAPI;
   ME_Ces = BROADCAST_CES;
   ME_State = CM_TEI_MANAGE;

   ME_Mail (MDL_ASS_REQ, L2_ID);
}

/******************
* LM_MC_PARAM_ERR *
*******************
*
* Parameter negotiation error (default values are used).
*
*/
PRIVATE void near LM_MC_PARAM_ERR ()

{
   ME_InternalEvent = NO_INTERNAL;             /* reset internal event */

   ++LM_ParamErr;                                         /* log error */

   /* prepare out packet
    */
   ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
   if (ME_PktOut == NULL)
      LIF_Fatal("me", "LM_MC_PARAM_ERR", "LIF_GetPkt failed");
#endif
   ME_InfoOut = ME_PktOut->Info;
   ME_InfoOut [0] = ME_Sapi;
   ME_InfoOut [1] = ME_Ces;
   ME_InfoOut [2] = LMTR [ME_DSL_Id][ME_Ces].Tei;
   ME_PktOut->InfoLen = 3;

   /* prepare other mail variables
    */
   ME_Sapi = ME_SAPI;
   ME_Ces = BROADCAST_CES;
   ME_State = CM_TEI_MANAGE;

   ME_Mail (MDL_ASS_REQ, L2_ID);
}

/*
 *	Added for persistant deact.  Note that this kills all ces's.  The
 *	problem is that finding the right RI seems difficult, and I just spent
 *	3 hours figuring out how to get the PERSIST_DEACT event to the layer 2
 *	state machine.  I don't have the patience to go through that again.
 */
void
LM_PersDeact (dsl)
int dsl;
{
   int i;

   ME_DSL_Id = dsl;
   for (i=0; i<MAX_ME_CES; i++) {
       if(LMTR[ME_DSL_Id][i].State == OUTSTANDING) {
           /*
            * Clear out LMTR entry
            */
           LMTR [ME_DSL_Id][i].Ri = 0;
           LMTR [ME_DSL_Id][i].RC = 0;
           LMTR [ME_DSL_Id][i].Tei = UNASSIGNED;
           LMTR [ME_DSL_Id][i].State = UNASSIGNED;
           LIF_RemoveTimer( LMTR [ME_DSL_Id][i].T202_Handle );
           LMTR [ME_DSL_Id][i].T202_Handle = 0;
       }
   }
}

/*****************
* LM_T202_EXPIRY *
******************
*
* Timer T202 expiry (no response to TEI request).
*
*/
PRIVATE void near LM_T202_EXPIRY ()

{
   BYTE ces;
   UINT16 ri;
   PKT_ID timer_pkt;                              /* timed mail packet */

   if (detail_debug)
       buginf("\n entering LM_T202_EXPIRY");

   ri = ((((UINT16) (ME_InfoIn [1])) << 8) | ((UINT16) (ME_InfoIn [2])));

   /*
    * Do not call LM_Search with an ri == 0.  The LMTR table
    * is initailized to have an ri = 0 and an incorrect CES will
    * be returned.
    */
   if ((ri != 0) && ((ces = LM_Search (ri)) != MAX_ME_CES))
      {
      if (LMTR [ME_DSL_Id][ces].State == OUTSTANDING)
         {
         /* 'nuf already!
          */
/*         if (LMTR [ME_DSL_Id][ces].RC == DEFAULT_N202)  === */
         if (LMTR [ME_DSL_Id][ces].RC ==
			 ((SwitchType == BRI_1TR6_STYPE) ? DEFAULT_N202+1 : DEFAULT_N202)) /* === for 1tr6 certif */
            {
            /* prepare out packet
             */
            ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
            if (ME_PktOut == NULL)
               LIF_Fatal("me", "LM_T202_EXPIRY", "LIF_GetPkt failed");
#endif
            ME_InfoOut = ME_PktOut->Info;
            ME_InfoOut [0] = 'E';             /* FFS which error here? */
            ME_PktOut->InfoLen = 1;

            /* prepare other mail variables
             */
            ME_Sapi = LMTR [ME_DSL_Id][ces].Sapi;
            ME_Ces = ces;
            ME_State = CM_TEI_MANAGE;

            ME_Mail (MDL_ERR_RESP, L2_ID);

            /*  Clear out LMTR entry
             */
            LMTR [ME_DSL_Id][ces].Ri = 0;
            LMTR [ME_DSL_Id][ces].RC = 0;
            LMTR [ME_DSL_Id][ces].Tei = UNASSIGNED;
            LMTR [ME_DSL_Id][ces].State = UNASSIGNED;
            LIF_RemoveTimer( LMTR [ME_DSL_Id][ces].T202_Handle );
            LMTR [ME_DSL_Id][ces].T202_Handle = 0;
            }
         else                                       /* let's try again */
            {
            /* Use new value of Ri on retry; because this value is in the
            *  timer primitive info, the timer must be removed, rebuilt
            *  and again be added and started.
            */
            LIF_RemoveTimer( LMTR [ME_DSL_Id][ces].T202_Handle );
            timer_pkt = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
            if (timer_pkt == NULL)
               LIF_Fatal("me", "LM_T202_EXPIRY", "LIF_GetPkt failed");
#endif
            LMTR [ME_DSL_Id][ces].Ri = ri = LM_GetRi ();
            timer_pkt->Info [0] = MEI;
            timer_pkt->Info [1] = (BYTE) (ri >> 8);       /* high bits */
            timer_pkt->Info [2] = (BYTE) (ri & 0x00FF);    /* low bits */
            timer_pkt->InfoLen = 3;

            /* prepare timer/mail variables
            */
            ME_Sapi = ME_SAPI;
            ME_Ces = BROADCAST_CES;
            ME_State = CM_TEI_MANAGE;

            LMTR [ME_DSL_Id][ces].T202_Handle = LIF_AddTimerP (ME_ID, ME_ID,
               T202_EXPIRY, ME_DSL_Id, ME_Sapi, ME_Ces, 0, 0, DEFAULT_T202,
               timer_pkt);
            LIF_StartTimer( LMTR [ME_DSL_Id][ces].T202_Handle );

            /* prepare out packet
             */
            ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
            if (ME_PktOut == NULL)
               LIF_Fatal("me", "LM_T202_EXPIRY", "LIF_GetPkt failed");
#endif
            ME_InfoOut = ME_PktOut->Info;
            LM_PutTeiMsg (ri, ID_REQUEST, BROADCAST_TEI);
            ME_PktOut->InfoLen = 5;
#ifdef EURO_ACT
            assignment_state[ME_DSL_Id] = ASSIGNMENT_IN_PROGRESS;
#endif EURO_ACT
            ME_Mail (MDL_UI_DATA_REQ, L2_ID);

            ++LMTR [ME_DSL_Id][ces].RC;
            }
         }
      } else if (ri == 0) {
          /*
           * If ri == 0 this must be an Verify TEI.  Try N202 times
           * and then throw the TEI away.  But for US switches don't
           * throw it away, instead, try to bring Layer 2 up again.
           */

          LIF_RemoveTimer( LMTR [ME_DSL_Id][ME_Ces].T202_Handle );
          LMTR[ME_DSL_Id][ME_Ces].T202_Handle = 0;

	  if (detail_debug)
	      buginf("\nN202 = %d\n", LMTR[ME_DSL_Id][ME_Ces].RC);
	  if (LMTR[ME_DSL_Id][ME_Ces].RC == DEFAULT_N202) {
              if ((SwitchType != BRI_5ESS_STYPE) && (SwitchType != BRI_NI1_STYPE) &&
                      (SwitchType != BRI_DMS100_STYPE)) {
	          ME_Remove_TEI(ME_Ces);
              } else {
                  LMTR[ME_DSL_Id][ME_Ces].RC = 0;
                  /*
                   * Have to check the me_verify flag since ccbri could try to bring
                   * Layer 2 back up which would interfere with the id_verify.
                   */ 
                  if (LMTR[ME_DSL_Id][ME_Ces].me_to_activate) {
                      LMTR[ME_DSL_Id][ME_Ces].me_to_activate = FALSE;
                      CCBFIT_TerminalInit(NL_PU_EST_IND, ME_Ces, ME_DSL_Id);
                  }
              }
	  } else {
	      ++LMTR[ME_DSL_Id][ME_Ces].RC;
	      ME_Verify_TEI(ME_Ces);
	  }
      }
}

/* Layer Management Utilities
*/

/************
* LM_GetCes *
*************
*
* Get the CES that corresponds with the given TEI.
*
* Returns:  Corresponding CES, if successful; MAX_ME_CES, otherwise.
*
*/
PRIVATE BYTE near LM_GetCes (tei)

BYTE tei;                                          /* tei to translate */
{
   INT16 i;

   for (i=0; i<MAX_ME_CES; i++)
      if (LMTR [ME_DSL_Id][i].Tei == tei)
         break;

   return (i);
}

/***************
* LM_PutTeiMsg *
****************
*
* Build a TEI assignment message.
*
*/
PRIVATE void near LM_PutTeiMsg (ri, msg_type, ai)

UINT16 ri;                                         /* reference number */
BYTE msg_type;                                         /* message type */
BYTE ai;                                           /* action indicator */
{
   ME_InfoOut [0] = MEI;
   ME_InfoOut [1] = (BYTE) (ri >> 8);                     /* high bits */
   ME_InfoOut [2] = (BYTE) (ri & 0x00FF);                  /* low bits */
   ME_InfoOut [3] = msg_type;
   ME_InfoOut [4] = (ai << 1) | 0x01;           /* OR in extension bit */
}

/***********
* LM_GetRi *
************
*
* Generate a random number between 0 and FFFF.
*
* Return:   random number
*
*/
PRIVATE UINT16 near LM_GetRi ()

{

   if (LM_FirstTime)
      {
      LM_Seed = FIRST_SEED;             /* initialize with fixed value */
      LM_FirstTime = FALSE;                  /* don't initialize again */
      }
   LM_Seed = (LM_Seed * MULTIPLIER + INCREMENT);    /* generate number */

   LM_Seed = ( LM_Seed & 0xffff );     /* insure only 16 bits returned */
   return LM_Seed;
}

/************
* LM_Search *
*************
*
* Search for a TEI request given its reference number.
*
* Returns:  Corresponding CES, if successful; MAX_ME_CES, otherwise.
*
*/
PRIVATE BYTE near LM_Search (ri)

UINT16 ri;                           /* reference number to search for */
{
   INT16 i;

   for (i=0; i<MAX_ME_CES; i++)
      if (LMTR [ME_DSL_Id][i].Ri == ri)
         break;

   return (i);
}

#ifdef DEBUG
/*************
* LM_ShowTei *
**************
*
* Show CES to TEI mapping.
*
*/
PRIVATE void near LM_ShowTei ()

{
   INT16 i;

   for (i=0; i<MAX_ME_CES; i++)
      printf("CES=%d   TEI=%d\n", i, LMTR [ME_DSL_Id][i].Tei);
}
#endif

/* Connection Management SDLs
*/

/********
* CM_Go *
*********
*
* Connection Management (USER Side).
*
* NOTE: Test procedures are not supported.
*
*/
PRIVATE void near CM_Go ()

{
   /* Get DLCB associated with this SAPI/TEI
    */
   if ((DLCB = L2_SearchDLCB (ME_DSL_Id, ME_Sapi, ME_Ces, SIGNAL_CHAN_ID))
      == NULL)
      {
#ifdef DISPLAY_LOCAL_ERR
      if ( ME_Ces != LAPB_CES )
         LIF_Error ("me", "CM_Go", "Can't find DLCB.");
#endif
      goto Bailout;
      }

   if (DLCB->CM_State == CM_IDLE)
      {
      switch (ME_Event)
         {
      case MC_PARAM_REQ:
         CM_MC_PARAM_REQ ();
         break;

      case MDL_ERR_IND:
         CM_MDL_ERR_IND ();
         break;

      default:
         break;
         }
      }
   else                                       /* must be CM_AWAIT_CONF */
      {
      switch (ME_Event)
         {
      case MDL_XID_CONF:
         CM_MDL_XID_CONF ();
         break;

      case T204_EXPIRY:
		 if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
             CM_T204_EXPIRY ();
         break;

      default:
         break;
         }
      }

Bailout:

   return;
}

/******************
* CM_MC_PARAM_REQ *
*******************
*
* Parameter negotiation request.
*
*/
PRIVATE void near CM_MC_PARAM_REQ ()

{
   ME_InternalEvent = NO_INTERNAL;             /* reset internal event */

#ifdef SUPPRESS_XID
   /* Assign parameters to DLCB
    */
   DLCB->K         = (BYTE) DEFAULT_K;
   DLCB->N201_Rcv  = DEFAULT_N201_R;
   DLCB->N201_Xmit = DEFAULT_N201_X;
   DLCB->T200      = isdn_sys_constants[ME_DSL_Id].T200;
   DLCB->CM_State  = CM_IDLE;
   ME_InternalEvent = MC_PARAM_CONF;

#else
   /* auto TEIs negotiate parms
    */
   if (AUTO_TEI (LMTR [ME_DSL_Id][DLCB->Ces].Tei))
      {
      /* prepare out packet
       */
      ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
      if (ME_PktOut == NULL)
         LIF_Fatal("me", "CM_MC_PARAM_REQ", "LIF_GetPkt failed");
#endif
      ME_InfoOut = ME_PktOut->Info;
      CM_PutParmMsg (DEFAULT_N201_X, DEFAULT_N201_R, (BYTE) DEFAULT_K,
         isdn_sys_constants[ME_DSL_Id].T200);
      ME_PktOut->InfoLen = 18;

      /* prepare other mail variables
       */
      ME_Sapi = DLCB->Sapi;
      ME_Ces = DLCB->Ces;
      ME_State = DLCB->State;

      ME_Mail (MDL_XID_REQ, L2_ID);

	  if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
          LIF_StartTimer (DLCB->T204_Handle);
      DLCB->CM_State = CM_AWAIT_CONF;
      }
   else                        /* non-auto TEIs use default parameters */
      {
      /* Assign parameters to DLCB
       */
      DLCB->K         = (BYTE) DEFAULT_K;
      DLCB->N201_Rcv  = DEFAULT_N201_R;
      DLCB->N201_Xmit = DEFAULT_N201_X;
      DLCB->T200      = isdn_sys_constants[ME_DSL_Id].T200;
      DLCB->CM_State  = CM_IDLE;
      ME_InternalEvent = MC_PARAM_CONF;
      }
#endif
}

/*****************
* CM_MDL_ERR_IND *
******************
*
* Error indication.
*
*/
PRIVATE void near CM_MDL_ERR_IND ()

{
   BYTE error;

   /* Log error
    */
   error = ME_InfoIn [0];
   ++DLCB->CM_Err_Count [error-'A'];

   if(detail_debug)
       buginf("\nCM_MDL_ERR_IND, got an MDL_ERR_IND, error = %c", error);

   /*
    * According to Q.921, Annex II, Table II-1/Q.921, we have
    * a choice here of verifying the link or taking it down.
    * For switch type NET3 a verify is sent for all other types
    * the tei is removed.
    */
   if ((error == 'C') || (error == 'D') ||
       (error == 'H') || (error == 'G')) {
       /*
        * Added for Sapphire NET3 testing.  Test TC25005 requires
        * either we remove our tei and send an id request or we
        * verify the tei.  The verify was chosen as the lesser of
        * two evil's. Also, need to verify the TEI for custom DMS and
        * National ISDN switches.
        */
       if ((SwitchType == BRI_NET3_STYPE) ||
           (SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) {
           ME_Verify_TEI(ME_Ces);
       } else {
           /*
            * Australia TS014 switchtypes should ignore error 'D'
            * and stay in the same state, see TABLE 6/C2 of the TS014
            * specification for further details. All other switchtypes
            * will remove the TEI. Except the 5ESS custom, it will not
            * remove it's TEI and does not support the TEI verify. 
            */
           if (((SwitchType != PRI_TS014_STYPE) && (error != 'D')) &&
                (SwitchType != BRI_5ESS_STYPE)) {
               ME_Remove_TEI(ME_Ces);
           }
       }
   }

   /* FFS Layer 2 flow control based on error (frame queue full).
    */
   if ((error == 'P') || (error == 'Q'))
      {
      }
}

/******************
* CM_MDL_XID_CONF *
*******************
*
* Parameter negotiation confirmation.
*
*/
PRIVATE void near CM_MDL_XID_CONF ()

{
   INT16 n201_x;
   INT16 n201_r;
   BYTE k;
   UINT32 t200;

   if (SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
       LIF_StopTimer (DLCB->T204_Handle);

/* FFS
    if (not test)
*/
   if (TRUE)
      {
#ifdef FFS
/* FFS
      if (zero length I field)
         use default values
*/
      if (FALSE)
         {
         n201_x = DEFAULT_N201_X;
         n201_r = DEFAULT_N201_R;
         k      = (BYTE) DEFAULT_K;
         t200   = isdn_sys_constants[ME_DSL_Id].T200;
         }
      else                                     /* use confirmed values */
#endif
         {
         CM_GetParmMsg (&n201_x, &n201_r, &k, &t200);
         }

      /* Assign parameters to DLCB
       */
      DLCB->K         = k;
      DLCB->N201_Rcv  = n201_r;
      DLCB->N201_Xmit = n201_x;
      DLCB->T200      = t200;
      DLCB->CM_State  = CM_IDLE;

      ME_InternalEvent = MC_PARAM_CONF;
      }
}

/*****************
* CM_T204_EXPIRY *
******************
*
* Timer T204 expiry (no response to XID request).
*
*/
PRIVATE void near CM_T204_EXPIRY ()

{
   if (DLCB->CM_RC == DEFAULT_N204)                     /* try no more */
      {
      /* prepare out packet
       */
/* FFS: XID_RESET??
      ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
      if (ME_PktOut == NULL)
         LIF_Fatal("me", "CM_T204_EXPIRY", "LIF_GetPkt failed");
#endif
      ME_InfoOut = ME_PktOut->Info;
      CM_PutParmMsg (DEFAULT_N201_X, DEFAULT_N201_R, (BYTE) DEFAULT_K,
                     isdn_sys_constants[ME_DSL_Id].T200);
      ME_PktOut->InfoLen = 18;
*/
      /* prepare other mail variables
       */
      ME_Sapi = DLCB->Sapi;
      ME_Ces = DLCB->Ces;
/* FFS:
      ME_State = DLCB->CM_State;
      ME_Mail (MDL_XID_RESET???, L2_ID);
*/
      ME_InternalEvent = MC_PARAM_ERR;
      DLCB->CM_State = CM_IDLE;
      }
   else                                                   /* try again */
      {
      /* prepare out packet
       */
      ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
#ifdef FFS_CHECK_ERRORS
      if (ME_PktOut == NULL)
         LIF_Fatal("me", "CM_T204_EXPIRY", "LIF_GetPkt failed");
#endif
      ME_InfoOut = ME_PktOut->Info;
      CM_PutParmMsg (DEFAULT_N201_X, DEFAULT_N201_R, (BYTE) DEFAULT_K,
                     isdn_sys_constants[ME_DSL_Id].T200);
      ME_PktOut->InfoLen = 18;

      /* prepare other mail variables
       */
      ME_Sapi = DLCB->Sapi;
      ME_Ces = DLCB->Ces;
      ME_State = DLCB->CM_State;

      ME_Mail (MDL_XID_REQ, L2_ID);

      LIF_StartTimer (DLCB->T204_Handle);
      ++DLCB->CM_RC;
      }
}

/* Connection Management Utilities
*/

/****************
* CM_GetParmMsg *
*****************
*
* Get values from confirmed parameter negotiation message.
*
*/
PRIVATE void near CM_GetParmMsg (n201_x, n201_r, k, t200)

INT16 *n201_x;
INT16 *n201_r;
BYTE *k;
UINT32 *t200;
{
   *n201_x = ((((UINT16) (ME_InfoIn [6])) << 8) | ((UINT16) (ME_InfoIn [7])));
   *n201_r = ((((UINT16) (ME_InfoIn [10])) << 8) | ((UINT16) (ME_InfoIn [11])));
   *k = ME_InfoIn [14];
#ifdef FFS
   /* The T200 byte value in ME_InfoIn is in .1 secs while the t200
    * UINT32 value is in .001 secs.  The byte value should be multiplied
    * by 100 to get msecs rather than tenths of seconds.
   */
   *t200 = (UINT32) ME_InfoIn [17] * 100;
#endif
   *t200 = isdn_sys_constants[ME_DSL_Id].T200;
   }

/****************
* CM_PutParmMsg *
*****************
*
* Build a parameter negotiation message.
*
*/
PRIVATE void near CM_PutParmMsg (n201_x, n201_r, k, t200)

INT16 n201_x;
INT16 n201_r;
BYTE k;
UINT32 t200;
{
   ME_InfoOut [0]  = FI;
   ME_InfoOut [1]  = GI;
   ME_InfoOut [2]  = GL0;
   ME_InfoOut [3]  = GL1;
   ME_InfoOut [4]  = PI0;
   ME_InfoOut [5]  = PL0;
   ME_InfoOut [6] = (BYTE) (n201_x >> 8);                 /* high bits */
   ME_InfoOut [7] = (BYTE) (n201_x & 0x00FF);              /* low bits */
   ME_InfoOut [8]  = PI1;
   ME_InfoOut [9]  = PL1;
   ME_InfoOut [10] = (BYTE) (n201_r >> 8);                /* high bits */
   ME_InfoOut [11] = (BYTE) (n201_r & 0x00FF);             /* low bits */
   ME_InfoOut [12] = PI2;
   ME_InfoOut [13] = PL2;
   ME_InfoOut [14] = k;
   ME_InfoOut [15] = PI3;
   ME_InfoOut [16] = PL3;
#ifdef FFS
   /* The T200 byte value in ME_InfoOut is in .1 secs while the t200
    * UINT32 value is in .001 secs.  The UINT32 value should be divided
    * by 100 to get tenths of seconds rather than msecs.
   */
   ME_InfoOut [17] = (BYTE)(t200 / 100);
#endif
   ME_InfoOut [17] = 0;
}

/*
 * Verify the TEI.  This will eleviate the requirement to
 * remove the tei during NET3 testing.
 */
PRIVATE void near ME_Verify_TEI (ces)
BYTE ces;
{
    BYTE tei;
    PKT_ID timer_pkt;    

 
    /*
     * Make sure there are no pending timers. 
     */
 
    if (LMTR [ME_DSL_Id][ces].T202_Handle) {
        LIF_StopTimer(LMTR [ME_DSL_Id][ces].T202_Handle);
        LIF_RemoveTimer(LMTR [ME_DSL_Id][ces].T202_Handle);
        LMTR[ME_DSL_Id][ces].T202_Handle = 0;
    }

    timer_pkt = LIF_GetPkt (SML_INFO_BTYPE, DATA_OFFSET, TRUE);
    if (timer_pkt == NULL)
	LIF_Fatal("me", "ME_Verify_TEI", "LIF_GetPkt failed");
    timer_pkt->Info [0] = MEI;
    timer_pkt->Info [1] = 0;	/* RI is set to 0 (unused for verify) */
    timer_pkt->Info [2] = 0;
    timer_pkt->InfoLen = 3;

    /*
     * prepare timer/mail variables
     */
    ME_Sapi = ME_SAPI;
    ME_Ces = BROADCAST_CES;
    ME_State = CM_TEI_MANAGE;

    LMTR [ME_DSL_Id][ces].T202_Handle = LIF_AddTimerP (ME_ID, ME_ID,
         T202_EXPIRY, ME_DSL_Id, ME_Sapi, ces, 0, 0, DEFAULT_T202,
	 timer_pkt);
    LIF_StartTimer(LMTR[ME_DSL_Id][ces].T202_Handle);

    /*
     * Build Verify message.
     */
    tei = LMTR[ME_DSL_Id][ces].Tei;
    
    ME_PktOut = LIF_GetPkt (SML_INFO_BTYPE, UI_DATA_OFFSET, TRUE);
    if (ME_PktOut == NULL)
	LIF_Fatal("me", "ME_Verify_TEI", "LIF_GetPkt failed");
    ME_InfoOut = ME_PktOut->Info;
    LM_PutTeiMsg(0, ID_VERIFY, tei);
    ME_PktOut->InfoLen = 5;

    ME_Mail (MDL_UI_DATA_REQ, L2_ID);
}
