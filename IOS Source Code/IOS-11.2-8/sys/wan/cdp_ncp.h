/* $Id: cdp_ncp.h,v 3.2 1995/11/17 18:00:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/cdp_ncp.h,v $
 *------------------------------------------------------------------
 * CDP support over PPP
 *
 * March 1995, Arun Sastry
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cdp_ncp.h,v $
 * Revision 3.2  1995/11/17  18:00:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:17:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CDP_NCP_H__
#define __CDP_NCP_H__

extern uchar cdp_reqci(fsm *, uchar *, short *);
extern int cdp_ackci(fsm *, uchar *, int);

#endif __CDP_NCP_H__ 

