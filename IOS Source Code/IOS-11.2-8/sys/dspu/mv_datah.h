/* $Id: mv_datah.h,v 3.2 1995/11/17 09:05:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/mv_datah.h,v $
 *------------------------------------------------------------------
 * SNA Network Management message header
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_datah.h,v $
 * Revision 3.2  1995/11/17  09:05:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#ifndef mv_datah_h
#define mv_datah_h

#include "nmtype.h"

struct DataHeader {
    DataHeader* next;        /* pointer to next header */
    byte*       dataBytes;   /* pointer to data extent */
    hword       dataLength;  /* length of extent       */
    hword       pad;         /* make it word align     */
 };

DataHeader* DataHeader_new(void);

void DataHeader_delete(DataHeader* dataP);

void DataHeader_deleteChain(DataHeader* dataP);

void DataHeaderInit(void);
hword DataHeaderFindTotalLen(DataHeader* dataP);
#endif



