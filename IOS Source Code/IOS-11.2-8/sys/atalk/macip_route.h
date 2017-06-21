/* $Id: macip_route.h,v 3.2 1995/11/17 08:45:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/macip_route.h,v $
 *------------------------------------------------------------------
 * macip_route.h -- information for users of MacIP Routing
 *
 * September 1991, Russ Tremain
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * prereqs:
 *	atalk.h
 *	macip_util.h
 *	../ip/ip.h
 *------------------------------------------------------------------
 * $Log: macip_route.h,v $
 * Revision 3.2  1995/11/17  08:45:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:10:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__macip_route_h__
#define	__macip_route_h__

extern void macip_to_IP(paktype *pak);
extern void macip_to_DDP(paktype *pak);

#endif
