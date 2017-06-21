/* $Id: lapb_event.h,v 3.2 1995/11/17 18:06:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/lapb_event.h,v $
 *------------------------------------------------------------------
 * SNA Network Management LAPB Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lapb_event.h,v $
 * Revision 3.2  1995/11/17  18:06:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lapb_event_h
#define lapb_event_h

#include "interface.h"
#include "lapb.h"

typedef enum LAPB_EventType {
    LocalSentFRMR_W,
    LocalSentFRMR_X,
    LocalSentFRMR_Y,
    LocalSentFRMR_Z,
    LocalReceivedFRMR_X,
    LocalReceivedFRMR_W,
    LocalReceivedFRMR_Y,
    LocalReceivedFRMR_Z,
    RetryCountExpired,
    DISCReceived,    
    LocalSentSABM_ReceivedDM
}LAPB_EventType;
void LAPBEvent(hwidbtype *idb, LAPB_EventType event);

#endif
