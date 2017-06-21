/* $Id: wrkbblk.h,v 3.2 1995/11/17 09:06:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/wrkbblk.h,v $
 *------------------------------------------------------------------
 * DSPU Work Buffer Control Block.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This contains the work buffer control block definition.
 *------------------------------------------------------------------
 * $Log: wrkbblk.h,v $
 * Revision 3.2  1995/11/17  09:06:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:31:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __WRKBBLK_H
#define __WRKBBLK_H

#include "dtypes.h"

/*---- The following structure(s) are exported. */

struct WrkBBlk {        /* Work Buffer control Block. */
        struct WrkBBlk *next;   /* Ptr to next in chain. */
        LinkStn        *lsP;    /* link station pointer */       
        boolean        bufReg;  /* Buffer is registered for work. */
        } ;

/*---- The following interfaces are exported. */

void
InitWrkBBlk (WrkBBlk *workBBlkP  /* Ptr to work buffer block. (Input). */
            ,LinkStn *lsP
            );


#endif /* __WRKBBLK_H. */
