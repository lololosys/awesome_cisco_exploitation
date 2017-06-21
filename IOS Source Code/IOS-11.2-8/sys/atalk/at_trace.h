/* $Id: at_trace.h,v 3.2 1995/11/17 08:44:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/at_trace.h,v $
 *------------------------------------------------------------------
 * AppleTalk Trace command implementation header file.
 *
 * February 1993, David S.A. Stine, on a whim.
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_trace.h,v $
 * Revision 3.2  1995/11/17  08:44:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:09:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_TRACE_H__
#define __AT_TRACE_H__

typedef struct ddpecho_trace_ {
    uchar echofunc;		/* DDP_ECHO_{REQUEST,REPLY,TRACE,TREPLY}  */
    ulong echoseq;		/* echo seq number -- we use 0 */
    uchar  pad;			/* MBZ */
    ushort net;			/* Network of destination node */
    uchar  node;		/* Node # of destination node */
    ulong  start_time;		/* Time at origin of trace */
} ddpecho_trace;  

#endif
