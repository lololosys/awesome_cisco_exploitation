/* $Id: clsi_queue.c,v 3.2 1995/11/17 08:55:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/clsi_queue.c,v $
 *------------------------------------------------------------------
 * Functions that are methods of CLSIQueue class
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains the functions for manipulating CLSIQueue objects.
 * There are functions to create, and destroy the queues.
 * There are functions for accessing various field of the queues.
 *------------------------------------------------------------------
 * $Log: clsi_queue.c,v $
 * Revision 3.2  1995/11/17  08:55:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  05:15:26  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:19:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "cls_private.h"

/* 
 * Methods of TCLSIQInit
 */

TCLSIQInit* CLSIQInitCreate (const char* moduleName, UserPutFuncPtr thePutFuncPtr)
{
    TCLSIQInit* this;
    
    this = malloc(sizeof(TCLSIQInit));
    if (this != NULL)
    {
	sstrncpy(this->fModuleName, moduleName,
		sizeof(this->fModuleName));
	
	this->fPutFunc = thePutFuncPtr;
    }
    return this;
}


void CLSIQInitDestroy (TCLSIQInit* this)
{
    free(this);
}

const char* CLSIQInitGetModuleName (const TCLSIQInit* this)
{
    return this->fModuleName;
}

/*
 * Methods of CLSIQueue
 */

TCLSIQueue* CLSIQueueCreate (TCLSIQInit* theQInit, void* refCon) /* create a queue */
{
    TCLSIQueue* this;
    
    this = malloc(sizeof(TCLSIQueue));
    if (this != NULL)
    {
	this->fQInit = theQInit;
	this->fNext = NULL;
	this->fFirstMsg = NULL;
	this->fLastMsg = NULL;
	this->fRefCon = refCon;
    }
    return this;
}

void CLSIQueueDestroy (TCLSIQueue* this) /* destroy a queue */
{
    free(this);
}
