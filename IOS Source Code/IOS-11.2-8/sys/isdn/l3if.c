/* $Id: l3if.c,v 3.2.60.1 1996/04/30 23:07:50 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/l3if.c,v $
 *------------------------------------------------------------------
 * l3if.c
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: l3if.c,v $
 * Revision 3.2.60.1  1996/04/30  23:07:50  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:41:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:44:20  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 21:16:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       l3if.c                                                  
DESCRIPTION:  MTEX interface functions for Layer 3                    
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

Layer 3 Interface module contains functions to interface Layer 3
Controller with the MultiTasking EXecutive (MTEX).


ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.


*************************************************************************
*************************************************************************
*/

#define     L3IF_C                                      /* module name */

/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

#include    "master.h"
#undef      NULL

#include    "flags.h"
#include    "tune.h"
#include    "gendef.h"
#include    "exec.h"
#include    "execuser.h"
#include    "lif.h"
#include    "ccie.h"
#include    "l3.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

#define     TIMER_MASK_L3  0x1300                  /* time out message */
#define     NLCB_LEN BLKSIZE( sizeof ( struct NetworkLinkControlBlock ) )

/************************************************************************
*                   D A T A    A L L O C A T I O N                      *
*************************************************************************
*/

PRIVATE BOOLEAN L3IF_Activated;

/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/

/***************
* L3IF_StartL3 *
****************
*
* Layer 3 Interface module
*
*/
PUBLIC void L3IF_StartL3 ()
{
   BYTE *  addr;                            /* generic address of mail */
   PRIM_ID prim_in;                              /* incoming primitive */

   for (;;) {
      /*
       * Initialize L3
       */
      L3_Init();

      L3IF_Activated = TRUE;
      while (L3IF_Activated) {
          /*
           * Read mail from our mailbox
           */
          addr = LIF_ReadMail(L3_ID);

          if (((PRIM_ID) addr)->BufType == PRIM_BTYPE) {
              prim_in = (PRIM_ID) addr;
              if (prim_in->Prim_ID == MNL_RESET_REQ)
                  L3IF_Activated = FALSE;
              else if (prim_in->Prim_ID == MNL_DEACT_IND)
				L3_ShutDown(prim_in->Dsl_ID);
              else if (prim_in->Prim_ID == MNL_ACT_IND)
                  /* do nothing - already activated */
                  ;
              else
                  L3_Go(prim_in->Source_ID, prim_in->Prim_ID,
                        prim_in->Dsl_ID, prim_in->Ces, prim_in->Call_ID,
                        prim_in->Pkt);
              /*
               * NEVER free timer primitives!
               */
              if ((prim_in->Prim_ID & 0xFF00) != TIMER_MASK_L3)
                  LIF_FreePrim(prim_in);
          } else                                 /* Some unknown envelope */
              LIF_Dispose(addr);
      }

      /*
       * Let's get closure, shall we?
       */
      L3_ShutDown(ALL_ISDN_INTERFACES);
   }
}
