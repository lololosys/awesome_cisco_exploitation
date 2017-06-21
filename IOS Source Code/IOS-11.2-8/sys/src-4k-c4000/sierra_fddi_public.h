/* $Id: sierra_fddi_public.h,v 3.3 1995/11/17 18:41:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra_fddi_public.h,v $
 *------------------------------------------------------------------
 * External Prototypes for the Cisco Sierra FDDI Module.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: sierra_fddi_public.h,v $
 * Revision 3.3  1995/11/17  18:41:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SIERRA_FDDI_PUBLIC_H__
#define __SIERRA_FDDI_PUBLIC_H__

void rt_timer_start(void);
void csp_rt_timer_int_hand(uchar *nullptr0,  uchar *nullptr1, uchar *nullptr2);

#endif __SIERRA_FDDI_PUBLIC_H__
