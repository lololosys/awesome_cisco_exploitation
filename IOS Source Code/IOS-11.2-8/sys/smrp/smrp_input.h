/* $Id: smrp_input.h,v 3.2 1995/11/17 18:53:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_input.h,v $
 *------------------------------------------------------------------
 * Input processing routines for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_input.h,v $
 * Revision 3.2  1995/11/17  18:53:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:45:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_INPUT_H__
#define __SMRP_INPUT_H__

/*
 *------------------------------------------------------------------
 * SMRP Network Layer Entry Points defined by smrp_input.
 *------------------------------------------------------------------
 */
#if NOT_USED
void smrp_EnqueuePacket(SMRPPortID, paktype*, uchar*, int);
#endif

/*
 *------------------------------------------------------------------
 * SMRP Internal Entry Points defined by smrp_input.
 *------------------------------------------------------------------
 */
extern void smrp_input_Initialize(void);

extern void smrp_input(void);
extern boolean smrp_FastProcessForward(paktype *);

extern paktype *smrp_CreatePacketWithData(SMRPPort *, MCHeader *, uint);
extern paktype *smrp_CreatePacketWithHeader(SMRPPort *, MCHeader *, uint);

extern void smrp_Dispose(int, paktype*);

#endif __SMRP_INPUT_H__

