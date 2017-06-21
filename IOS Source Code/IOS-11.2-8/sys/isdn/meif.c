/* $Id: meif.c,v 3.2.58.2 1996/04/30 23:07:59 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/meif.c,v $
 *------------------------------------------------------------------
 * meif.c  MTEX interface functions for the Management Entity 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: meif.c,v $
 * Revision 3.2.58.2  1996/04/30  23:07:59  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/04/09  02:34:56  rbeach
 * CSCdi53168:  SPIDS on an MBRI for 5ESS always get sent out BRI0.
 * The spid and eid were being sent out on BRI 0 for all the other
 * BRI interfaces. This was caused by an incorrect dsl value. This
 * only occurs with a switchtype of basic-5ess.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:42:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:44:21  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:17:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       meif.c                                                  
DESCRIPTION:  MTEX interface functions for the Management Entity      
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

The Management Entity Interface module contains functions to interface
the Management Entity with the Real-time MultiTasking EXecutive (MTEX).


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/

#define     MEIF_C                                      /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/
#include    "master.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "exec.h"
#include    "execuser.h"
#include    "lif.h"
#include    "me.h"
#include    "l2.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

PRIVATE BOOLEAN MEIF_Activated;                     /* link activated? */

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

/***************
* MEIF_StartME *
****************
*
* Management Entity Interface module
*
*/
PUBLIC void MEIF_StartME ()

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
   PRIM    prim_copy;                       /* temp copy of incoming prim */
   PRIM_ID prim_in;                              /* incoming primitive */
   BOOLEAN junk;
   BYTE *  addr, dsl;                            /* generic address of mail */

   ME_Init();      /* Initialize ME */
   for (dsl=0; dsl <MAX_DSL; dsl++)
       assignment_state[dsl] = ASSIGNMENT_NOT_STARTED;  
   for (;;) {
      /*
       * Activate link(s)
       */
      MEIF_Activated = TRUE;

      while (MEIF_Activated) {
         /* Read mail from our mailbox
          */
         addr = LIF_ReadMail(ME_ID);

         if (((PRIM_ID) addr)->BufType == PRIM_BTYPE) {
            prim_in = (PRIM_ID) addr;
            prim_copy = *prim_in; /* Structure copy */

            switch (prim_in->Prim_ID) {
               case MPH_DEACT_IND:     /* deactivation */
                   /* NOTE: at this point the ME_ShutDown will execute and
                    *       will send proper deactivation messages to all
                    *       layers. Nothing has to be done here.
                    */
                   ME_ShutDown(prim_in->Dsl_ID);
                   break;

               case MAL_ACT_REQ:
                   /* already activated - tell it to the Call Control.  All other
                    *  layers should be active already.
                    */
                   LIF_SendPkt(L2P_LAPD, ME_ID, 0, CC_ID, MAL_ACT_IND, 
			       prim_in->Dsl_ID, 0, 1, 0, 0, 0, TRUE, NULL, NOW,
                               REGULAR_MAIL);
                   break;

               case MNL_MIM_IND:
                   junk = ME_got_mnl_mim_ind(prim_in->Dsl_ID, prim_in->Ces,
                            prim_in->Call_ID, prim_in->Pkt);
                   /*
                    * turn off the spid timer
                    */
                   CCBFIT_TerminalInit(MNL_MIM_IND, prim_in->Ces, prim_in->Dsl_ID);
                   break;

               case MPH_ACT_IND:
                   /* just ignore this */
                   break;

               default:
                   ME_Go(prim_in->Prim_ID, prim_in->Dsl_ID, prim_in->Sapi,
                         prim_in->Ces, prim_in->Pkt);
                   break;
            }
            /*
             * NEVER free timer primitives! Check the copy not the original
             * since it could have been freed & re-used when processed if it
             * was a timer.
             */
            if ((prim_copy.Prim_ID & 0xFF00) != TIMER_MASK_ME)
               LIF_FreePrim (prim_in);
        } else {
            LIF_Dispose(addr);
        }
      }

      /* Let's get closure, shall we?
       */
      ME_ShutDown(ALL_ISDN_INTERFACES);
   }
}
