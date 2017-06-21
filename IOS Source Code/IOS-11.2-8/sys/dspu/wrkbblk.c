/* $Id: wrkbblk.c,v 3.2 1995/11/17 09:06:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/wrkbblk.c,v $
 *------------------------------------------------------------------
 * DSPU Work Buffer Control Block Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This contains the work buffer control block init subroutine.
 *------------------------------------------------------------------
 * $Log: wrkbblk.c,v $
 * Revision 3.2  1995/11/17  09:06:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:31:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "dspu.h"
#include "wrkbblk.h"
 
void
InitWrkBBlk (WrkBBlk *workBBlkP
            ,LinkStn *lsP
            )
{
  workBBlkP->next = 0;        /* Init Ptr to next in chain.          */
  workBBlkP->lsP = lsP;       /* Set link station pointer            */       
  workBBlkP->bufReg = FALSE;  /* Set buffer not registered for work. */
}

 
