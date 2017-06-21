/* $Id: l2if.c,v 3.2.60.1 1996/04/30 23:07:37 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/l2if.c,v $
 *------------------------------------------------------------------
 * l2if.c  MTEX interface functions for Layer 2
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: l2if.c,v $
 * Revision 3.2.60.1  1996/04/30  23:07:37  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:41:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:44:18  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:16:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: l2if.c
VERSION: 3.21
DESCRIPTION: MTEX interface functions for Layer 2

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

Layer 2 Interface module contains functions to activate and initialize
Layer 2 as well as enabling it to pick up its mail using MTEX.

ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/


/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 
*************************************************************************
*/

#include    "master.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "exec.h"
#include    "execuser.h"
#include    "lif.h"
#include    "l2.h"


/************************************************************************
*                           C O N S T A N T S                           
*************************************************************************
*/

/* timer primitive id's have 0x12 in the upper byte
*/
#define TIMER_MASK_L2   0x1200

/* size of dlcb memory pool blocks
*/
#define DLCB_LEN BLKSIZE(sizeof(struct DataLinkControlBlock))


/************************************************************************
*                F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

PRIVATE void near L2IF_Activate(void);


/************************************************************************
*                     D A T A   A L L O C A T I O N                     
*************************************************************************
*/


/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                
*************************************************************************
*/


/************************************************************************
* L2IF_Activate
*************************************************************************
*
* Await physical layer activation.
*
*/
PRIVATE void near L2IF_Activate ()

{
#ifdef TASK_L1
   PRIM_ID prim_in;                              /* incoming primitive */
   BYTE lr_act_req;              /* local or remote activation request */
   BOOLEAN waiting;
   BYTE *mail;

   waiting = TRUE;
   
   while (waiting)                                /* await MDL_ACT_REQ */
      {
      mail = LIF_ReadMail (L2_ID);

      if (((PRIM *) mail)->BufType == PRIM_BTYPE)
         {
         prim_in = (PRIM *) mail;
         
         switch (prim_in->Prim_ID)
            {
            case MDL_ACT_REQ:
               waiting = FALSE;
               lr_act_req = prim_in->SourceState;
               LIF_FreePrim (prim_in);
               break;
            default:
               /* Send back to ourselves.  Commonly, a DL_EST_REQ
               *  has been sent by L3; we don't want to deal with
               *  it until after activation and we don't want to
               *  throw it away.
               */
               LIF_MailPrim( prim_in, L2_ID, L2_ID,
                  NOW, REGULAR_MAIL, MAIL_TO_DEST );
               break;
            }
         }
      else
         {
         LIF_Fatal ("L2IF", "L2IF_Activate 1", "bad mail type");
         }
      }

   /* Send PH_ACT_REQ to L1
   */
   LIF_SendPkt (L2P_LAPD, L2_ID, lr_act_req, L1_ID, PH_ACT_REQ,
      0, 0, 0, 0, 0, 0, FALSE, NULL, NOW, REGULAR_MAIL);

   waiting = TRUE;
   
   while (waiting)                                 /* await PH_ACT_IND */
      {
      mail = LIF_ReadMail (L2_ID);
      
      if (((PRIM *) mail)->BufType == PRIM_BTYPE)
         {
         prim_in = (PRIM *) mail;
         
         switch (prim_in->Prim_ID)
            {
            case PH_ACT_IND:
               waiting = FALSE;
               LIF_FreePrim (prim_in);
               break;
            default:
               /* Send back to ourselves.  Commonly, a DL_EST_REQ
               *  has been sent by L3; we don't want to deal with
               *  it until after activation and we don't want to
               *  throw it away.
               */
               LIF_MailPrim( prim_in, L2_ID, L2_ID,
                  NOW, REGULAR_MAIL, MAIL_TO_DEST );
               break;
            }
         }
      else
         {
         LIF_Fatal ("L2IF", "L2IF_Activate 2", "bad mail type");
         }
      }
#endif TASK_L1
}

/************************************************************************
* L2IF_StartL2
*************************************************************************
*
* Layer 2 Interface module
*
*/
PUBLIC void L2IF_StartL2()

{
   /*
    * A local copy is made of each primitive coming in on the "mail", since it
    * can be freed during processing in another part of the code.  A check is
    * made before the primitive is freed here, to see if it was a timer.  If
    * the primitive had been freed during processing, it may have had it's
    * fields cleared or been reallocated before this timer check.  So the check
    * is done on the copy not the original. This was problem that appeared in
    * France.
    */
   PRIM    prim_copy;                    /* Copy of original primitive */
   PRIM_ID prim_in;                              /* incoming primitive */
   BOOLEAN network;                                /* L2 network side? */
   BOOLEAN running;                          /* is L2 software running */
   BOOLEAN activated;                              /* is l1 activated? */
   BYTE *mail;
   BYTE dsl = 0;				/* Local copy of DSL */

   /* Initialize the network variable based on compile time option
   */
   if(Network_side)
      network = TRUE;
   else
      network = FALSE;

   for (;;)
      {
      L2_Init ();                                     /* Initialize L2 */
      running = TRUE;

      while (running)
         {
         L2IF_Activate ();                      /* wait for activation */
         activated = TRUE;                       /* indicate activated */

         while (activated)
            {
            /* Read mail from our mailbox.
            */
            mail = LIF_ReadMail (L2_ID);
            
            if (((PRIM *) mail)->BufType == PRIM_BTYPE)
               {
               prim_in = (PRIM *) mail;
               prim_copy = *prim_in;

               switch (prim_copy.Prim_ID)
                  {
                  case PH_DEACT_IND:                   /* deactivation */
                     break;
                  case MDL_RESET_REQ:                   /* major reset */
                     activated = FALSE;
                     running = FALSE;
                     dsl = prim_in->Dsl_ID;
                     break;
                  case PERSISTENT_DEACT:
		     /* have to overwrite all the stuff that I couldn't
		      * figure out how to have the MTEX do right in the
		      * first place.    ===
		      */
                     activated = FALSE;
                     prim_in->Sapi = 0;
                     prim_in->Ces = 1;
                     dsl = prim_in->Dsl_ID;
                     /* fall through... */
                  default:
                     L2_Go (prim_copy.L2_Protocol, prim_copy.Source_ID,
                        prim_copy.Prim_ID, prim_copy.Dsl_ID,
                        prim_copy.Sapi, prim_copy.Ces,
                        prim_copy.LLI, prim_copy.DSL_Chan_ID,
                        prim_copy.Pkt, network, prim_copy.FreePkt);
                     break;
                  }

               /* NEVER free timer primitives!
               */
               if ((prim_copy.Prim_ID & 0xFF00) != TIMER_MASK_L2)
                  LIF_FreePrim (prim_in);
               }
            else
               {
	       LIF_Dispose((PRIM *)mail);
               }
            }
         /* Reset all of the DLCB's back to TEI_ASSIGNED state. Note that
          *  if the link was not established, there will be no DLCBs so the
          *  following function will have no effect.
          */
         L2_ResetLink(dsl);
         }
      /* L2_ShutDown must be called before L2_Init is called again
      */      
      L2_ShutDown ();
      }
}

/*eof*/

