/* $Id: t1fdl.h,v 1.6 1996/07/10 06:44:23 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/t1fdl.h,v $
 *------------------------------------------------------------------
 * t1fdl.h  Facility Data Link Message definitions
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: t1fdl.h,v $
 * Revision 1.6  1996/07/10 06:44:23  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.5  1996/06/07  21:40:31  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.4  1996/05/09  21:25:54  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.3  1996/03/19  03:54:45  jliu
 *    o change the Tx clock source to internal by default
 *    o always use XBCKI for slip/transmit buffer clock which is derived
 *      from TDM bus 2.044 Mhz
 *    o when external network loopback is detected, need to change the transmit
 *      clock to line and change it back to internal when that loopback
 *      request goes away
 *    o need to initialize Rx PCM control registers
 *    o modify some internal registers setup for tone generation
 *
 * Revision 1.2  1996/03/02  03:51:22  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.1.1.1  1996/02/10  03:22:53  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef HAVE_T1FDL_H
#define HAVE_T1FDL_H

#define ESF_DL_RESPONSE_GOOD_CMD 0
#define ESF_DL_RESPONSE_BAD_CMD 0x08

#define LINELOOP_ACT            0x0e
#define LINELOOP_DEACT          0x38
#define PAYLOOP_ACT             0x14
#define PAYLOOP_DEACT           0x32

#define NUM_T1_CHANNELS		24
#define NUM_UNUSED_CHANNELS	8

#define ANSI_PRM_LENGTH 0xb

#define CRC_1		0x10;
#define CRC_1to5  	0x04;
#define CRC_5to10       0x80;
#define CRC_10to100     0x20;
#define CRC_100to319    0x04;
#define CRC_320more     0x01;
#define FE              0x80;
#define SE              0x40;
#define LB              0x20;
#define LV              0x40;
#define SL              0x02;

#define TYPE_ES 0
#define TYPE_UAS 1
#define TYPE_BES 2
#define TYPE_SES 3
#define TYPE_CSS_LOFC 4
#define PUTSHORT(ptr, val)  (*((ushort *) (ptr)) = (ushort) (val))

extern void t1_nloop(T1InfoType *t1, bool flag);
extern void fdl_req_process(T1InfoType *t1);
extern void fdl_reply_process(T1InfoType *t1, frm_info *frm);
extern void bzero(void *dummy, int count);
extern void payloadloop_pro(T1InfoType *t1, bool flag);
extern void netloop_pro(T1InfoType *t1, bool flag);

#endif   /* HAVE_T1FDL_H */
