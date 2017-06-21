/* $Id: clnsfast_sse.h,v 3.2 1995/11/17 08:52:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/clnsfast_sse.h,v $
 *------------------------------------------------------------------
 * clnsfast_sse.h -- Definitions for CLNS SSE support
 *
 * October 1993 Tony Li
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clnsfast_sse.h,v $
 * Revision 3.2  1995/11/17  08:52:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:18:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

struct sse_clnsinfo_ {
    int cache_version;			/* clns_cache_version */
    int cache_changes;			/* clns_cache_changes */
};
