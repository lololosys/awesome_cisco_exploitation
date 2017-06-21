/* $Id: dspuqueu.h,v 3.2 1995/11/17 09:05:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspuqueu.h,v $
 *------------------------------------------------------------------
 * DSPU Queue Routines.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This contains the DSPU Queue routine definitions.
 *------------------------------------------------------------------
 * $Log: dspuqueu.h,v $
 * Revision 3.2  1995/11/17  09:05:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:28:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef dspuqueu_h
#define dspuqueu_h

#include "dtypes.h"


typedef struct DspuQueueHeader{
                 void*  first;
                 void*  last;
                 } DspuQueueHeader;


boolean
DspuQueueIsEmpty (DspuQueueHeader *dspuQHdr);

void *
DspuGetFirstQelement (DspuQueueHeader *dspuQHdr);

void *
DspuGetLastQelement (DspuQueueHeader *dspuQHdr);


/*=====================================================================*
* DspuEnqueue
*======================================================================*
*
* Description:
*   The object to be enqueued is rejected if its next ptr is not zero.
*
**/
boolean DspuEnqueue
          (DspuQueueHeader * queueP
          ,void            * bufferP
          );
       
/*=====================================================================*
* DspuDequeue
*======================================================================*
*
* Description:
*
**/
void* DspuDequeue
        (DspuQueueHeader * queueP 
        );
            
/*=====================================================================*
* DspuQueueInit
*======================================================================*
*
* Description:
*
**/
void DspuQueueInit
       (DspuQueueHeader * queueP 
       );

/*=====================================================================*
* DspuDeleteQueueElement
*======================================================================*
*
* Description:
*
**/
boolean DspuDeleteQueueElement
          (DspuQueueHeader * queueP
          ,void            * buffer
          );
          
typedef struct QueueElement{
    void* next;
} QueueElement;


#define NextQueueElement(p) ((QueueElement*)p)->next
     
#define IsEmpty(queueHeader) (queueHeader.first == 0)
#define GetFirstQelement(queueHeader) (queueHeader.first)
#define GetFirstQueue(queueHeader) (queueHeader->first)
#define GetLastQelement(queueHeader) (queueHeader.last)
/* boolean IsEmpty(QueueHeader q) {return q.first == 0;} */
/* void*  GetFirstQelement(QueueHeader q) {return q.first;} */

#endif
