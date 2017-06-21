/* $Id: unix_sse.h,v 3.2 1995/11/17 17:55:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/unix/unix_sse.h,v $
 *------------------------------------------------------------------
 * unix_sse.h - Definitions for Unix pseudo-support of the SSE
 *
 * May 1993 Tony Li
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_sse.h,v $
 * Revision 3.2  1995/11/17  17:55:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define TABLE(type, hash) (novcache[hash])

extern void unix_sse_chain_init(void);
