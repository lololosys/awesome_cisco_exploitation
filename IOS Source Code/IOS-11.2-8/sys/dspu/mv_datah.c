/* $Id: mv_datah.c,v 3.2.58.1 1996/06/28 23:05:53 hampton Exp $
 * $Source: /release/112/cvs/Xsys/dspu/mv_datah.c,v $
 *------------------------------------------------------------------
 * SNA Network Management message header
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_datah.c,v $
 * Revision 3.2.58.1  1996/06/28  23:05:53  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:05:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "mv_datah.h"

static queuetype emptyDataQ;

/* assign an array of data which should never run out */
#define MaxDataHeader 32

DataHeader* DataHeader_new(void)
{
   return p_dequeue(&emptyDataQ);
}

void DataHeaderInit(void)
{
   DataHeader* dataP;
   queue_init(&emptyDataQ, 0);
   dataP = malloc(sizeof(DataHeader)*MaxDataHeader);
   if (dataP){
        hword i;
        for (i = 0; i < MaxDataHeader; i++)
            p_enqueue(&emptyDataQ, &dataP[i]);
   }
}

void DataHeader_delete(DataHeader* dataP)
{
   dataP->next = 0;
   p_enqueue(&emptyDataQ, dataP);
}

void DataHeader_deleteChain(DataHeader* dataP)
{
   while (dataP){
      DataHeader* temp = dataP;
      dataP = dataP->next;
      temp->next = 0;         /* to satisfy p_enqueue */ 
      p_enqueue(&emptyDataQ, temp);
   }
}

hword DataHeaderFindTotalLen (DataHeader* dataP)
{
    hword len;
    for (len = 0; dataP; dataP = dataP->next)
        len += dataP->dataLength;
    return len;
}

