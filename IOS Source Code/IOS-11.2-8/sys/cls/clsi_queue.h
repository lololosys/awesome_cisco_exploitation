/* $Id: clsi_queue.h,v 3.2 1995/11/17 08:55:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/clsi_queue.h,v $
 *------------------------------------------------------------------
 * Cisco Link Services (CLS) interface file
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Defines the CLSIQueue class which is the channel for communications
 * between the DLU (Data Link User) and CLS (Cisco Link Services). The
 * communications uses a STREAMS-like interface model for passing
 * messages. The messages themselves are defined in clsi_msg.h.
 *
 * TCLSIQInit is another class that contains information on the 
 * module and its entry points. A module will typically only have
 * a single TCLSIQInit object associated with it.
 *------------------------------------------------------------------
 * $Log: clsi_queue.h,v $
 * Revision 3.2  1995/11/17  08:55:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:20:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CLSI_QUEUE_H__
#define __CLSI_QUEUE_H__

STRUCTURE_REF(TCLSIQueue);
STRUCTURE_REF(TCLSIQInit);

struct TCLSIQueue_
{
    TCLSIQInit*    fQInit;
    TCLSIQueue*    fNext;
    TCLSIMsg*  	   fFirstMsg;
    TCLSIMsg*  	   fLastMsg;
    void*          fRefCon;
};

/* Prototype for User put() function */
typedef int (*UserPutFuncPtr)(TCLSIQueue*, TCLSIMsg*);

struct TCLSIQInit_
{
    UserPutFuncPtr fPutFunc;
    char fModuleName[50];
};

/*
 * Methods of TCLSIQInit
 */

TCLSIQInit* CLSIQInitCreate(const char* moduleName, UserPutFuncPtr thePutFuncPtr);
void CLSIQInitDestroy(TCLSIQInit* this);
const char* CLSIQInitGetModuleName(const TCLSIQInit* this);

/*
 * Methods of CLSIQueue
 */

TCLSIQueue* CLSIQueueCreate(TCLSIQInit* theQInit, void* refCon);
	/* create a queue */

void CLSIQueueDestroy(TCLSIQueue* this);
	/* destroy a queue */

static inline int CLSIQueuePut(TCLSIQueue* this, TCLSIMsg* theMsg);
	/* put something to the queue */

static inline void* CLSIQueueGetRefCon(const TCLSIQueue* this);
	/* get refCon value from queue */

static inline TCLSIQInit* CLSIQueueGetQInit(const TCLSIQueue* this);

/*
 * Calls to exchange queue pointers with CLS. Called by DLU to
 * establish connection.
 */

TCLSIQueue* CLSOpen(TCLSIQueue* dluReadQueuePtr);
void CLSClose(TCLSIQueue* clsWriteQueuePtr);

/*
 * Implementation of static inline functions
 */

static inline int CLSIQueuePut (TCLSIQueue* this, TCLSIMsg* theMsg)
{
    return((*this->fQInit->fPutFunc)(this, theMsg));
}

static inline void* CLSIQueueGetRefCon (const TCLSIQueue* this)
{
    return(this->fRefCon);
}

static inline TCLSIQInit* CLSIQueueGetQInit (const TCLSIQueue* this)
{
    return(this->fQInit);
}

#endif __CLSIQUEUE_H__
