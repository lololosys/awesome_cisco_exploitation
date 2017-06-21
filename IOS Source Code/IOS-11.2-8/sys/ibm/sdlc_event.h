/* $Id: sdlc_event.h,v 3.2 1995/11/17 09:23:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/sdlc_event.h,v $
 *------------------------------------------------------------------
 * SNA Network Management SDLC Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdlc_event.h,v $
 * Revision 3.2  1995/11/17  09:23:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef sdlc_event_h
#define sdlc_event_h


/*
 * Network Management Event Hooks
 */
typedef enum _sdlc_nm_event_hook_t {
    SDLC_MaxIFieldExceeded,
    SDLC_RxFrmrNoReason,
    SDLC_RxUFrameWithIField,
    SDLC_RxSnrmInNrm,
    SDLC_RxFrmrWandX,
    SDLC_RxFrmrW,
    SDLC_RxFrmrY,
    SDLC_RxFrmrZ,
    SDLC_RxDm,
    SDLC_RxInvalidUFrame,
    SDLC_RxSFrameWithIField,
    SDLC_RxInvalidSFrame,
    SDLC_RxFrmrNoReasonInErrorState,
    SDLC_RxUFrameWithIFieldInErrorState,
    SDLC_RxFrmrInErrorState,
    SDLC_RxDmInErrorState,
    SDLC_RxInvalidNr,
    SDLC_PollRetriesExhausted_NoResponse,
    SDLC_PollRetriesExhausted_IFrameReSent,
    SDLC_InactivityTimerExpired,
    SDLC_XidRetriesExhausted,
    SDLC_CarrierDown,
    SDLC_RNRLimit
} sdlc_nm_event_hook_t;

void SdlcNmEvent(sdlc_nm_event_hook_t eventType,
                 hwidbtype *idb, 
                 sdlc_data_block *sdb);


#endif
