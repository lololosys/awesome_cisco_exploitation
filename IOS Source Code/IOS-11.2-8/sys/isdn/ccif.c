/* $Id: ccif.c,v 3.2.60.1 1996/04/30 23:07:06 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccif.c,v $
 *------------------------------------------------------------------
 * ccif.c  Executive interface functions for Call Control
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccif.c,v $
 * Revision 3.2.60.1  1996/04/30  23:07:06  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:39:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:44:01  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:14:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:      ccif.c
VERSION:     3.21
DESCRIPTION: Executive interface functions for Call Control

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

This interface module contains functions to interface a Call Control
Task with the MultiTasking EXecutive (MTEX) of other executive through
LIF.


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

ISSUES:

No Shutdown is implemented.

#ifdef VN2
#ifdef BRI

Starting in version 3.20 of the software,  activation and deactivation
primitives will be recognized by the Call Control Interface module.
The operation of this features is as follows:

   - The software starts in deactivated state (NOT_ACTIVE)
   - if PU_ACTIVATE is TRUE, this module will send a new primitive
     - MAL_ACT_REQ to ME and will enter a WAITING state.
   - Receiving MAL_ACT_IND will put the software in ACTIVE state, regardless
     of its previous state.
   - Receiving MAL_DEACT_IND will put the software in NOT_ACTIVE state,
     regardless of ots previous state.
   - Any message from HOST or KB_ID received in ACTIVE state will be processed
     in normal way.
   - Any message from HOST or KB_ID in NOT_ACTIVE state will be stored in 
     message queue (the size of this queue is given by MAX_ACT_QUEUE_LEN).
     Additionally, this module will send MAL_ACT_REQ to Management Entity 
     and will enter state WAITING. When MAL_ACT_IND is received, all
     of the messages in the queue will be processed in the order they
     were received. If MAL_DEACT_IND is received isntead, the message 
     queue will be purged.
   - Any message from HOST or KB_ID in WAITING state will be stored in the
     message queue.

#endif
#endif

*************************************************************************
*************************************************************************
*/

#define     CCIF_C                                      /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

#include    "master.h"

#include    "flags.h"
#include    "tune.h"
#include    "gendef.h"
#include    "exec.h"
#include    "execuser.h"
#include    "lif.h"
#include    "gp.h"
#include    "ccbri.h"

#ifdef PRI
#include    "cc_chan.h"
#include    "ccpri.h"
#endif

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

#define CCB_LEN BLKSIZE( sizeof ( struct CallControlBlock ) )

/************************************************************************
*                   D A T A    A L L O C A T I O N                      *
*************************************************************************
*/

#ifdef VN2
#ifdef BRI
/*
 * Activation Queue and activation state
 */
#define MAX_ACT_QUEUE_LEN     2

typedef enum {
   NOT_ACTIVE,
   WAITING,               
   ACTIVE
} ACT_STATE;

typedef struct {
    PKG_ID Pkg [MAX_ACT_QUEUE_LEN];                   /* saved packages */
    int PkgCnt;                         /* number of packages on queue */
} CCIF_ACT_Q;

PRIVATE ACT_STATE VN2_Act_State;                   /* activation state */
PRIVATE CCIF_ACT_Q ActQueue;                      /* package/sbc queue */

#endif                                                          /* BRI */
#endif                                                          /* VN2 */


/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

#ifdef VN2
#ifdef BRI
/* Function Declarations and Definitions for activation queue management:
*/

PRIVATE  void   near CCIF_EnQueue            (BYTE *);
PRIVATE  BYTE * near CCIF_DeQueue            (BOOLEAN);
PRIVATE  void   near CCIF_DiscardQueue       (void);
PRIVATE  void   near CCIF_InitQueue          (void);


/***************
* CCIF_EnQueue *
****************
*
* Queue incoming package or SBC on a queue.
*
*/
PRIVATE void near CCIF_EnQueue (addr)

BYTE  *addr;                      /* generic address of the mail piece */
{
   if (((PKG_ID) addr)->BufType == PKG_BTYPE) {
      if (ActQueue.PkgCnt == MAX_ACT_QUEUE_LEN) {
         LIF_Error ("ccif", "CCIF_EnQueue", "Queue Full.");
      } else {
         ActQueue.Pkg[ ActQueue.PkgCnt ] = (PKG_ID) addr ;
         ActQueue.PkgCnt++;
      }
   }
}


/***************
* CCIF_DeQueue *
****************
*
* DeQueue package or SBC from queue.
*
*/
PRIVATE BYTE * near CCIF_DeQueue (is_package) 
BOOLEAN is_package;
{
   int    i;
   BYTE * return_addr;
   /* Remove the first packet from the head of the queue.  Then scoot all
   *  the remaining packets up one in the queue, setting the last element
   *  to NULL.
   *
   *  Finally just decrement the number of packets in the queue, and
   *  return the one pulled off of the front.
   */
   
   return_addr = (BYTE *) ActQueue.Pkg[0];
   if (is_package) {
      return_addr = (BYTE *) ActQueue.Pkg[0];
      for (i = 0; i < MAX_ACT_QUEUE_LEN - 1; i++) {
         ActQueue.Pkg[i] = ActQueue.Pkg[i + 1];
      }
      ActQueue.Pkg[MAX_ACT_QUEUE_LEN - 1] = NULL;
      ActQueue.PkgCnt--;
   }
   return (return_addr);
}


/********************
* CCIF_DiscardQueue *
*********************
*
* Discard packet queue.
*
*/
PRIVATE void near CCIF_DiscardQueue ()

{
   while (ActQueue.PkgCnt)
      LIF_Dispose(CCIF_DeQueue(TRUE));
}

/*****************
* CCIF_InitQueue *
******************
*
* Initialize the Activation Queue
*
*/
PRIVATE void near CCIF_InitQueue ()

{
   int i;

   for (i = 0; i < MAX_ACT_QUEUE_LEN - 1; i++) {
      ActQueue.Pkg[i] = NULL;
   }
   ActQueue.PkgCnt = 0;
}
#endif    /* BRI */
#endif    /* VN2 */


/**********
* CC_Task *
***********
*
* Call Control Interface module
*
*/
PUBLIC void CC_Task ()
{
   BYTE *  addr;                            /* Generic address of mail */
   PRIM_ID prim_in;                              /* Incoming primitive */
   PKG_ID  pkg;                                    /* Incoming package */
   BOOLEAN activated;                               /* Link activated? */

   for (;;) {
      activated = TRUE;

      /* Initialize CC
      */
      switch (SwitchType) {
         case BRI_5ESS_STYPE:
         case BRI_DMS100_STYPE:
         case NTT_STYPE:
         case BRI_1TR6_STYPE:
         case BRI_NI1_STYPE:
         case BRI_NET3_STYPE:
         case BRI_TS013_STYPE:         
            CCBRI_Init();
            break;
#ifdef PRI
         case PRI_4ESS_STYPE:
         case PRI_NET5_STYPE:
         case PRI_NTT_STYPE:
         case PRI_TS014_STYPE:
         case PRI_5ESS_STYPE:
         case PRI_DMS100_STYPE:
            CCPRI_Init();
            break;
#endif                                                     /* PRI_4ESS */

         case VN2_STYPE:
         case VN3_STYPE:
            CCBRI_Init();
            CCIF_InitQueue();
            VN2_Act_State = ACTIVE;            
            break;
         default:                                  /* Should not occur */
            break;
         }

      while (activated) {
         /*
          * Read mail from our mailbox
          */
         addr = LIF_ReadMail(CC_ID);

         if (((PRIM_ID) addr)->BufType == PRIM_BTYPE) {
            prim_in = (PRIM_ID) addr;
            switch (SwitchType) {
               case BRI_5ESS_STYPE:
               case BRI_DMS100_STYPE:
               case NTT_STYPE:
               case BRI_NI1_STYPE:
               case BRI_NET3_STYPE:
               case BRI_TS013_STYPE:
               case BRI_1TR6_STYPE:
                  CCBRI_Go(prim_in->Source_ID, prim_in->SourceState,
                           prim_in->Prim_ID, prim_in->Pkt, prim_in->Call_ID,
                           prim_in->Dsl_ID, prim_in->Sapi, prim_in->Ces);
                  break;

#ifdef PRI
               case PRI_4ESS_STYPE:
               case PRI_NET5_STYPE:
               case PRI_NTT_STYPE:
               case PRI_TS014_STYPE:
               case PRI_5ESS_STYPE:
               case PRI_DMS100_STYPE:
                  CCPRI_Go( prim_in->Source_ID, prim_in->Prim_ID,
                            prim_in->Pkt, prim_in->Call_ID,
                            prim_in->SourceState, prim_in->Dsl_ID,
                            prim_in->Ces );
                  break;
#endif                                                     /* PRI_4ESS */

               case VN2_STYPE:
               case VN3_STYPE:
                  /* see if this is activation/deactivation primitive
                  */
                  switch (prim_in->Prim_ID) {
                     case MAL_ACT_IND:
                        while (ActQueue.PkgCnt) {
                           pkg = (PKG_ID) CCIF_DeQueue ( TRUE );
                           CCBRI_Go( pkg->SourceID, 0, pkg->MsgType, (PKT_ID)pkg, 0,
                              0, 0, 0 );
                           LIF_Dispose( pkg );
                        }
                        VN2_Act_State = ACTIVE;
                        break;

                     case MAL_DEACT_IND:
                        VN2_Act_State = NOT_ACTIVE;
                        CCIF_DiscardQueue();
                        break;

                     default:
                        CCBRI_Go(prim_in->Source_ID, prim_in->SourceState,
                                 prim_in->Prim_ID, prim_in->Pkt, prim_in->Call_ID,
                                 prim_in->Dsl_ID, prim_in->Sapi, prim_in->Ces);
                        break;
                  }
                  break;

               default:                            /* Should not occur */
                  break;
               }
               /*
                * NEVER free timer primitives!
                */
               if ((prim_in->Prim_ID & 0xFF00) != TIMER_MASK_CC)
                   LIF_FreePrim(prim_in);
         } else if (((PKG_ID) addr)->BufType == PKG_BTYPE) {
            pkg = (PKG_ID) addr;
            switch (SwitchType) {
#ifdef PRI
               case PRI_4ESS_STYPE:
               case PRI_NET5_STYPE:
               case PRI_NTT_STYPE:
               case PRI_TS014_STYPE:
               case PRI_5ESS_STYPE:
               case PRI_DMS100_STYPE:
                  CCPRI_Go(pkg->SourceID, pkg->MsgType, (PKT_ID)pkg, 0,
                           pkg->PkgInfo[1], 0, 0);
                  LIF_Dispose(pkg);
                  break;
#endif                                                     /* PRI_4ESS */
               case BRI_5ESS_STYPE:
               case BRI_DMS100_STYPE:
               case NTT_STYPE:
               case BRI_NI1_STYPE:
               case BRI_1TR6_STYPE:
               case BRI_NET3_STYPE:
               case BRI_TS013_STYPE:
                  CCBRI_Go(pkg->SourceID, 0, pkg->MsgType, (PKT_ID)pkg, 0,
                           pkg->PkgInfo[1], 0, 0);
                  LIF_Dispose(pkg);
                  break;
               case VN2_STYPE:
               case VN3_STYPE:
                  if (VN2_Act_State == ACTIVE) {
                     CCBRI_Go(pkg->SourceID, 0, pkg->MsgType, (PKT_ID)pkg, 0,
                              pkg->PkgInfo[1], 0, 0);
                     LIF_Dispose(pkg);
                  } else {
                     if (VN2_Act_State == NOT_ACTIVE) {
                        LIF_SendPkt(L2P_LAPD, CC_ID, 0, ME_ID, MAL_ACT_REQ,  
                                    pkg->PkgInfo[1], Q931_SAPI, SIGNALING_CES, 0, 1,
                                    0, FALSE, NULL, NOW, REGULAR_MAIL);
                        VN2_Act_State = WAITING;
                     }
                     /* save the incoming package in a queue if there is
                      * room. Otherwise, drop it.
                      */
                     CCIF_EnQueue(addr);
                  }
                  break;
               default:                            /* Should not occur */
                  LIF_Dispose(pkg);
                  break;
               }
            } else {                                       /* Unknown event */
                LIF_Dispose(addr);
            }
         }
      }
}
