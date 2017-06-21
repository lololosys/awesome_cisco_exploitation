/* $Id: qllc_event.h,v 3.2 1995/11/17 18:55:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/qllc_event.h,v $
 *------------------------------------------------------------------
 * SNA Network Management QLLC Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllc_event.h,v $
 * Revision 3.2  1995/11/17  18:55:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:01:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef qllc_event_h
#define qllc_event_h

#include "qllc.h"



typedef enum QLLC_EventType {
    QLLC_FRMR_Sent,
    N2_RetryCountExpired,
    QLLC_FRMR_Rcvd
}QLLC_EventType;

void QllcEvent(qllctype *qllc, QLLC_EventType event);

#endif
