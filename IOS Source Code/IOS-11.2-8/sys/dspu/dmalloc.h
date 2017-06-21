/* $Id: dmalloc.h,v 3.2 1995/11/17 09:04:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dmalloc.h,v $
 *------------------------------------------------------------------
 * DSPU malloc exports.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 *    DSPU buffer management.
 *       CISCO malloc uses more than 40 bytes overhead. This module 
 *    replaces malloc and free.
 *
 *    Warning: no free will be called. That is, once buffer is used it will
 *    never return to the system buffer pool.
 *------------------------------------------------------------------
 * $Log: dmalloc.h,v $
 * Revision 3.2  1995/11/17  09:04:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:28:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __dmalloc_h                         
#define __dmalloc_h

#include "dtypes.h"

void DspuBufferInit(void);       /* initialization */

void* DspuMalloc(size_t size);   /* malloc */

void DspuFree(void* bufferP);    /* free   */



#endif

