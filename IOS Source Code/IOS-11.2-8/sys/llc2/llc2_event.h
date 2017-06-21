/* $Id: llc2_event.h,v 3.2 1995/11/17 17:44:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/llc2/llc2_event.h,v $
 *------------------------------------------------------------------
 * SNA Network Management LLC2 Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llc2_event.h,v $
 * Revision 3.2  1995/11/17  17:44:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:42:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "llc2.h"
#include "interface.h"



typedef enum LLC2_EventType{
    AckTimerExpired,
    DMReceived,
    SABMEReceivedWhileConnected,
    FRMRreceived,
    FRMRsent,
}LLC2_EventType;

void LLC2Event(llctype *llc, LLC2_EventType event);

