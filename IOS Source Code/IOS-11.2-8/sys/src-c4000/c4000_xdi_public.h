/* $Id: c4000_xdi_public.h,v 3.3 1995/11/17 18:45:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-c4000/c4000_xdi_public.h,v $
 *------------------------------------------------------------------
 * External prototypes for BSI FDDI driver
 *
 * May 1993, Scott Mackie
 * March 1994, Scott Mackie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: c4000_xdi_public.h,v $
 * Revision 3.3  1995/11/17  18:45:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:32  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:56:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __C4000_XDI_PUBLIC_H__
#define __C4000_XDI_PUBLIC_H__

extern void CSPInterruptHandler(uInt16 stnID);
extern Flag StationPathTestMulti(uInt16 stnID);
extern void BypassRequestMulti(uInt16 stnID, uInt16 bypassState);
extern void CSPHandlePHYInterrupts(uInt16 stnID, uInt16 portID);
extern void CSPHandleMACInterrupts(uInt16 stnID, uInt16 macID);
extern Flag LEMInterrupt(uInt16 stnID, uInt16 portID);

#endif __C4000_XDI_PUBLIC_H__

