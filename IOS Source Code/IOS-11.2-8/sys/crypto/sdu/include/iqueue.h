/* $Id: iqueue.h,v 1.1.68.1 1996/04/19 14:57:41 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/iqueue.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: iqueue.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iqueue.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:41  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:39  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:19  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/Common/IQUEUE.H 6     8/30/95 1:32p Venkat 
*
* DESCRIPTION:	This file contains the macros and type definitions to support
*				queuing operations of far pointers between near and far modules.
*
* NOTES:		These queues use a circular buffer and 8-bit PUT/GET indices.
*				Interrupts may either PUT or GET into a queue, but not both.
*				Tasks (or other interrupts) may either PUT or GET into a queue,
*				but not both.  The Interrupt and Task may not perform the same
*				operation to the same queue.
*
*				The IQueues may be defined in either far or near space since the
*				operation is the same.  In fact, if the Queues are defined in 
*				the near data space of the interrupt layer this will optimize 
*				queue manipulation during time critical interrupt processing.
*
*				The test program for these macros is called IQ-TEST. The IDE+C 
*				files should be check in under $/SecureLAN/Common/Test/IQueue.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: IQUEUE.H 
* 
* *****************  Version 6  *****************
* User: Venkat       Date: 8/30/95    Time: 1:32p
* Updated in $/SecureLAN/Common
* Fix for NO_IQENTRY() macro; Code generation problem fixed; My feeling
* is it is general enough, any body can use it!
* 
* *****************  Version 5  *****************
* User: Venkat       Date: 8/25/95    Time: 4:39p
* Updated in $/SecureLAN/Common
* Macro IQ_NOENTRY(q) is added for use in SFU.
* 
* *****************  Version 4  *****************
* User: Croyle       Date: 10/13/94   Time: 1:16p
* Updated in $/SecureLAN/Common
* Added #define IQ_INI macro to be used as a declarative initializer so
* that a queue can be put into the BSS (initialized data) segment and
* doesn't require a function to execute the IQ_INIT() macro.
* 
* *****************  Version 3  *****************
* User: Croyle       Date: 10/06/94   Time: 1:37p
* Updated in $/SecureLAN/Common
* Cleanuped up tab alignment since group standardized on 4space TABs (was
* using 3space TABs).
* 
* *****************  Version 2  *****************
* User: Croyle       Date: 9/23/94    Time: 2:43p
* Updated in $/SecureLAN/Common
* First check-in which conforms to Coding Standard
*******************************************************************************/

#ifndef IQUEUE_H
#define IQUEUE_H

/*****************************************************************************
							 INCLUDE FILES
*****************************************************************************/

#include "../crypto/sdu/include/cylink.h"

/*****************************************************************************
							 IQUEUE DEFINITIONS
*****************************************************************************/

#define IQ_SUCCESS	 0
#define IQ_BADSIZE	-1
#define IQ_NOMEM  	-2
#define IQ_FULL		-3
#define IQ_EMPTY  	-4

/*****************************************************************************
							 TYPE DEFINITIONS
*****************************************************************************/

/* Queue Definition */

typedef struct						/* Queue Structure */
{
	uchar		put;				/* buffer index: where to put new entry */
	uchar		get;				/* buffer index: where to get next entry */
	ushort		pad;        		/* pad structure to long word boundary */
	void far*	buf[256];			/* queue buffer (far pointer entries) */
}	IQueue;

/*****************************************************************************
							 INLINE QUEUE MACROS
*****************************************************************************/
/*****************************************************************************
Macro:	IQ_INIT
Note:	This macro initializes the buffer indeces.
Parms:	q...queue info structure
*****************************************************************************/

#define IQ_INI			{ 0, 255, 0xbabe }
#define IQ_INIT(q)		{ q.put=0; q.get=255; }
#define IQ_NOENTRY(q) 	( q.put == (uchar)(q.get+1) )

/*****************************************************************************
Macro:	IQ_PUT,  and IQ_PUT_nc [ no confirmation i.e., no stat ]
Note:	puts an entry into a queue. A macro was chosen to maximize the
		processing speed since these functions operate with the 64us interrupt.
Parms:	q......IQueue structure
		dat....data to post to queue
		stat...result of operation (IQ_FULL or IQ_SUCCESS) >>>IQ_PUT ONLY<<<
*****************************************************************************/

#define IQ_PUT(q,dat,stat)                                             		\
{                                                     						\
	REG uchar e = q.put;					/* current put index		*/	\
	REG uchar nxt = e+1;					/* next put index 			*/	\
																			\
	if (nxt != q.get)	{					/* compare with q.get value */	\
		q.put = nxt;	  					/* must be atomic operation!*/	\
		q.buf[e] = (void far*) dat; 		/* write data to queue 		*/	\
		stat=IQ_SUCCESS;                                                    \
	}																		\
	else  																	\
		stat=IQ_FULL;						/* geti=puti, queue is full	*/ 	\
}

#define IQ_PUTnc(q,dat)                                             		\
{                                                     						\
	REG uchar e = q.put;					/* current put index 		*/	\
	REG uchar nxt = e+1;					/* next put index 			*/	\
	if (nxt != q.get)	{					/* compare with q.get value */	\
		q.put = nxt;	  					/* must be atomic operation!*/	\
		q.buf[e] = (void far*) dat;  		/* write data to queue 		*/	\
	}																		\
}

/*****************************************************************************
Macro:	IQ_GET
Note:	gets an entry from a queue. A macro was chosen to maximize processing
		speed since these functions operate within the 64us interrupt.
Parms:	q.......IQueue structure
		entry...data retrieved from queue (NULL if queue is empty)
*****************************************************************************/

#define IQ_GET(q,entry)														\
{																			\
	REG uchar e = q.get+1;					/* current put index   		*/	\
	if (e != q.put) 														\
	{																		\
		q.get = e;							/* must be atomic operation	*/	\
		entry = q.buf[e];													\
	}																		\
	else  																	\
		entry = NULL;						/* queue is empty 			*/	\
}

#endif

/*------------------------- iqueue.h END OF FILE ------------------------------ */

