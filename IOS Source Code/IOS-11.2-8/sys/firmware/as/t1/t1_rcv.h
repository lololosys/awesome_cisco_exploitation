/* $Id: t1rcv.h,v 1.7 1996/05/06 06:01:08 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/t1rcv.h,v $
 *------------------------------------------------------------------
 * framer.h  NEAT framing algorithm inline  routines
 *
 * October, 1995, Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: t1rcv.h,v $
 * Revision 1.7  1996/05/06 06:01:08  jliu
 *     o Cleaned some code for LED on and off
 *     o Set DSX1_XMT_AIS in dsx1LineStatus after sending AIS
 *
 * Revision 1.6  1996/04/08  18:18:01  jliu
 *     o cleanup some code
 *     o modified code for digits collection
 *
 * Revision 1.5  1996/03/19  03:54:50  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.4  1996/03/02  03:51:24  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.3  1996/02/26  06:32:16  jliu
 *     o removed SetRxingRemoteAlarm macro since only one place use it
 *
 * Revision 1.2  1996/02/22  22:27:52  jliu
 *     o update thge current T1 firmware for starting channelized T1 coding
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef HAVE_T1RCV_H
#define HAVE_T1RCV_H

static inline void ClearRxingRemoteAlarm (T1InfoType *t1)
{
    t1->Fr.RxingRemAlarm = FALSE;
    ClearMIBLineStatus(&t1->Mib->Config.dsx1LineStatus, DSX1_RCV_REM_ALM);
    if (!t1->unit) 
        NEAT_LED &= ~REM_ALARM_0;
    else
        NEAT_LED &= ~REM_ALARM_1;
}

extern void interface_init(T1InfoType *t1);
extern void T1_Framing_Process(T1InfoType *t1, int unit);


#endif   /* HAVE_T1RCV_H */
