/* $Id: linkdown_event.h,v 3.2 1995/11/17 09:29:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/linkdown_event.h,v $
 *------------------------------------------------------------------
 * SNA Network Management
 * Ethernet, Token Ring and HDLC failure code 
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1988-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: linkdown_event.h,v $
 * Revision 3.2  1995/11/17  09:29:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef linkdown_event_h
#define linkdown_event_h

/* Before this code is implemented, there is no failure cause when a
 * link is down. Most of the failure cause had to be passed from the
 * hardware. This is saved in the hwidb. 
 * All the LINK_DOWN_TRAP for Ethernet, Tokenring and Hdlc are removed
 * from the original code, they are replaced by setting up the error
 * code.
 * The checking of link state changed is done at net_cstate, periodic,
 * this is used to call LinkDownEvent when the failCause is set.
 * Fail cause of keep alive will not be used once there is a real cause.
 * To prevent too many traps and error be sent, same error will not be
 * sent.
 */



typedef enum LinkDownEventType {
    LinkNoError,
    Ether_RemoveStationRcvd,
    Ether_LostCarrier,
    Ether_KeepAliveFailure,
    Ether_FatalTxError,
    Ether_LateCollision,
    Ether_ExcessiveCongestion,
    Ether_OpenFailure,
    AdminDownCommand,
    TR_openFailLobeTest,
    TR_openFailRingBeacon,
    TR_openFailDupAddr,
    TR_openFailRemove,
    TR_openFailUnexpected,
    TR_hardError,
    TR_beacon,
    TR_wireFault,
    TR_remoteRemoval,
    TR_KeepAliveFailure,
    Hdlc_KeepAliveFailure,
    Lapb_KeepAliveFailure,
    Serial_SignalLost
} LinkDownEventType;

void LinkDownEvent(hwidbtype *idb);

#endif

