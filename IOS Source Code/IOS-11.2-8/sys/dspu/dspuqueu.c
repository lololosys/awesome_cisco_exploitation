/* $Id: dspuqueu.c,v 3.2.60.1 1996/09/13 08:31:27 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspuqueu.c,v $
 *------------------------------------------------------------------
 * DSPU Queue Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This contains queue manipulation routines for various DSPU modules.
 * Note that the enqueue/dequeue routines inhibit and enable interrupts.
 * This is because the private DSPU queue which holds CLSI buffers may be
 * accessed concurrently.
 *------------------------------------------------------------------
 * $Log: dspuqueu.c,v $
 * Revision 3.2.60.1  1996/09/13  08:31:27  lcox
 * CSCdi61967:  dspu - retries & retry-timeout dont work like doc says
 * Branch: California_branch
 *   retry-timeout range changed in code to agree with user
 *   documentation  (was 0..600, is now 1..600
 *
 * Revision 3.2  1995/11/17  09:05:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:03:26  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/06/07 20:28:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "dspu.h"
#include "dspu_interr.h"
#include "dspuqueu.h"


typedef struct Link {
        struct Link* next;
        } Link;


/*============================================================================*
* DspuQueueIsEmpty: *
*============================================================================*
*
* Description:
* Is the specified DSPU queue empty?
*
**/


boolean
DspuQueueIsEmpty (DspuQueueHeader *dspuQHdr)

{


return (dspuQHdr->first == (void *) NULL);

}  /* DspuQueueIsEmpty. */


/*============================================================================*
* DspuGetFirstQelement: *
*============================================================================*
*
* Description:
* Return first element from specified queue.
*
**/


void *
DspuGetFirstQelement (DspuQueueHeader *dspuQHdr)

{


return (dspuQHdr->first);

}  /* DspuGetFirstQelement. */


/*============================================================================*
* DspuGetLastQelement: *
*============================================================================*
*
* Description:
* Return last element from specified queue.
*
**/


void *
DspuGetLastQelement (DspuQueueHeader *dspuQHdr)

{

return (dspuQHdr->last);

}  /* DspuGetLastQelement. */


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
          )
{
    boolean retval = TRUE;

    DISABLE_INTERRUPT                 /* No interruptions please. */

    if (((Link*)bufferP)->next != 0) {
        retval =  FALSE;
    } else {
        if (queueP->first == 0) {
            /* Insert the first bufferP in the queue */
            queueP->first = bufferP;
            queueP->last  = bufferP;
        } else if (queueP->last != bufferP) {
            /* Insert bufferP at the end of the queue */
            ((Link*)queueP->last)->next = bufferP;
            queueP->last = bufferP;
        } else {
            retval = FALSE;
        }
    }

    ENABLE_INTERRUPT                  /* Allow interruptions again. */
  
    return retval;
}

/*=====================================================================*
* DspuDequeue
*======================================================================*
*
* Description:
*
**/
void* DspuDequeue
        (DspuQueueHeader * queueP 
        )
{
  Link* bufferP;

  DISABLE_INTERRUPT                 /* No interruptions please. */

  if (queueP->first == 0) {
    /* Empty queue */
    bufferP = 0;
  }
  else {    
    /* Delete the first queue element */
    bufferP = queueP->first;  
    queueP->first = bufferP->next;
    if (queueP->first == 0)
      queueP->last = 0;
    bufferP->next = 0; 
  }  

  ENABLE_INTERRUPT                  /* Allow interruptions again. */
  
  return (void*) bufferP;
}

/*=====================================================================*
* DspuQueueInit
*======================================================================*
*
* Description:
*
**/
void DspuQueueInit
       (DspuQueueHeader * queueP 
       )
{
  queueP->first = queueP->last = 0;  
}


/*=====================================================================*
* DspuDeleteQueueElement
*======================================================================*
*
* Description:
*
**/
boolean DspuDeleteQueueElement(
          DspuQueueHeader * queueP,
          void            * bufferP
          )
{
  void* previousP;

  previousP = queueP->first;
  
  if (previousP != 0)           
  {
    /* Queue not empty */
    if (bufferP == queueP->first)
    {
      /* Delete first queue element */
      DspuDequeue(queueP);
    }
    else
    {
      /* Find the previous ptr */
      while ((previousP != 0) && (((Link*)previousP)->next != bufferP))
        previousP = ((Link*)previousP)->next;    
      
      if (previousP == 0)
        return FALSE;  /* bufferP does no exist */
        
      if (queueP->last == bufferP)
      {
        /* Delete last queue element */
        queueP->last = previousP;
        ((Link*)previousP)->next = 0;
      }
      else
      {
        ((Link*)previousP)->next = ((Link*)bufferP)->next;
        ((Link*)bufferP)->next = 0;
      }
    }        
    return TRUE;
  }
  else
    return FALSE;
}
