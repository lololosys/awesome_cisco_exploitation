/* $Id: range_private.h,v 3.2 1995/11/17 17:56:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/util/range_private.h,v $
 *------------------------------------------------------------------
 * range_private.h - Private internal definitions for range.c
 *
 * Jan 1995 Tony Li
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: range_private.h,v $
 * Revision 3.2  1995/11/17  17:56:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct rangetype_ {
    avl_node_type avl_node;		/* Tree information */
    uint start, end;			/* start and end of interval */
    int thread;				/* Thead of probably single elems */
} rangetype;

#define RANGE_NOTHREAD -1

struct rangelist_ {
    rangetype *top;			/* top of tree */
    int thread;				/* thread of possible singles */
};
