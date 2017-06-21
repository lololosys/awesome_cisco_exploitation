/* $Id: x25_event.h,v 3.2 1995/11/17 18:07:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/x25_event.h,v $
 *------------------------------------------------------------------
 * SNA Network Management X25 Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_event.h,v $
 * Revision 3.2  1995/11/17  18:07:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:22:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef x25_event_h
#define x25_event_h

#include "interface.h"
#include "x25.h"
#include "../dspu/nmtype.h"


typedef enum X25_EventType{
    X25Clear_Sent,
    X25Reset_Sent,
    X25Restart_Sent,
    CMNS_Restart_Sent,
    X25Clear_Rcvd,
    X25Reset_Rcvd,
    X25Restart_Rcvd,
    ProtocolViolation,
    DiagnosticPakSentOrRcvd
}X25_EventType;


/* There are alerts which have same alert ID but different subfield. The subfield is set
 * to 82211A00, where 1A is Clear Packet. This is the default, but can be changed to
 * 1B (Restart) or 1C Reset. That is the field is changed to 1A + AlertType.
 * The alerts that will be converted are: 
 * 1 reset/clear indication received by DTE
 * 20 reset/clear/restart sent by DCE
 * 21 reset/clear/restart received by DCE
 */

enum AlertType{
    AlertClear,
    AlertReset,
    AlertRestart
};


struct X25_EventData_ {
    uchar  diag;
    uchar  cause;
    uchar  alertType;     /* dce indication/request type */
    uchar  gfiLcg;
    uchar  lcn;
    uchar  pti;
 };


void X25Event(lcitype   *lci, 
              hwidbtype *idb,
              X25_EventData* eventData,
              X25_EventType event);

/* Received a Reset, restart or reset */
void X25EventRx(hwidbtype* idb, lcitype* lci, packettype* p); 

#endif
