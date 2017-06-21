/* $Id: dmalloc.c,v 3.3.52.2 1996/08/28 12:41:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dmalloc.c,v $
 *------------------------------------------------------------------
 * DSPU malloc Services. 
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 *    DSPU buffer management.
 *       CISCO malloc uses more than 40 bytes overhead. This module 
 *    replaces malloc and free.
 *
 * DSPU calls malloc to get 2 K of memory to use as a heap. There are 16
 * buffer pools. Each buffer in the same pool will hold buffer of the
 * same size. Each buffer allocated is a multiple of 8 bytes and the
 * pools hold buffers from 8 bytes to 256 bytes. The size of the
 * buffer is the index to the buffer pool which is headed by a queue.
 *
 * DspuMalloc tries to get buffer from the pool. If none is available,
 * it then gets it from the heap.
 * DspuFree always put buffer back to the buffer pool;
 *------------------------------------------------------------------
 * $Log: dmalloc.c,v $
 * Revision 3.3.52.2  1996/08/28  12:41:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.52.1  1996/05/17  10:47:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.40.1  1996/03/22  18:50:16  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.3  1995/11/30  19:07:29  ppearce
 * CSCdi44393:  DSPU/SDLC memory leak when SDLC device disconnected
 *   (1) SDLC sends DISC_IND when it shouldn't
 *   (2) DSPU memory heap management isn't working properly
 *
 * Revision 3.2  1995/11/17  09:04:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  21:45:13  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:28:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */

#include "dspu.h"
#include "dmalloc.h"
#include "dspuqueu.h"

/* 32 buffer pools of 8 bytes each */

#define BufferResolution 8
#define MaxBufferPool    32
#define HeapSize         2048
#define MagicNumber      0xa5

static DspuQueueHeader  bufferPool[MaxBufferPool+1];
static byte*            dspuHeap;
static hword            bufferOffset;
static hword            numberOfBuffersUsed;     /* for statistic only */


/*=====================================================================*
* DspuBufferInit
*======================================================================*
*
* Description:
*    Initialise buffer pools and heap management
*   
**/
void DspuBufferInit(void)
{
   hword i;
   for (i = 0; i <= MaxBufferPool; i++)
      DspuQueueInit(&bufferPool[i]);
   
   dspuHeap = malloc(HeapSize);    /* get a buffer */

   bufferOffset = 0;
   numberOfBuffersUsed = 0;
}

/*=====================================================================*
* GetHeap
*======================================================================*
*
* Description:
*   heap management mallocs 2 k buffer and uses an offset to get the
* next memory.
*   
**/
static byte* GetHeap(hword index)
{
    byte* bufferP = NULL;
    hword bufferSize = (index + 1)*BufferResolution;
    
    if (bufferOffset + bufferSize >=  HeapSize) {
        if (bufferSize > HeapSize)
            return NULL; /* too big for intended use */
        dspuHeap = malloc(HeapSize);    /* get a buffer */
        if (dspuHeap) {
            bufferOffset = 0;
            numberOfBuffersUsed++;         /* for statistics only */
        }
    }
    if (dspuHeap) {
        bufferP = &dspuHeap[bufferOffset];
        bufferOffset += bufferSize;
    }
    return bufferP;
}

/*=====================================================================*
* DspuMalloc
*======================================================================*
*
* Description:
*   Get a buffer. The first 2 bytes of the buffer saves the buffer index
* Only the 3rd byte of the buffer returned to the user.
*   
*   
**/

void* DspuMalloc(size_t size)
{
   hword            index;
   byte*            buffer;
   
   /* index = 0 -> size 0..4,  bufferSize = 8
    *         1 -> 5..12,      bufferSize = 16
    *         i -> 8*i-3..8*(i+1)-5 bufferSize = 8 *(i + 1)
    */
   
   index = (size + 3)/BufferResolution;
   
    
   if (index > MaxBufferPool){
       /* 
        * Did not expect to get a buffer of more than 256 bytes. 
        *
        * If so, get it from the heap 
        *   and return the buffer to > 256 bytes pool
        *
        * Output debug message to signal investigation needed
        *
        */
      buffer = GetHeap(index);
      index = MaxBufferPool;
      if (dspu.debugLevel) {
        buginf("DSPU: buffer > 256 bytes required\n");
   }
   } else if ((buffer = DspuDequeue(&bufferPool[index])) == 0) {
      buffer = GetHeap(index);
   } else {
      /* 
       * DspuDequeue() must have returned a buffer from the pool
       *   so it is not necessary to reset index and MagicNumber 
       *     -- just return the buffer --
       *
       */
      return(buffer);
   }

   if (!buffer)               /* GetHeap failed                    */
     return(NULL);            /*   ensure DspuMalloc fails as well */

   *buffer = (byte) index;
   buffer[1] = MagicNumber;   /* for checking */
   return &buffer[4];    /* return the 5th byte */
       
}

/*=====================================================================*
* DspuFree
*======================================================================*
*
* Description:
*   
**/
void DspuFree(void* bufferP)
{
   byte* bufP = (byte*)bufferP - 4;
   
   if (bufP[0] > MaxBufferPool || bufP[1] != MagicNumber){
       errmsg(&msgsym(DSPU_BufferCorrupted, DSPU));
   } else {
       hword index = bufP[0];
       DspuEnqueue(&bufferPool[index], bufferP);
   }
}
