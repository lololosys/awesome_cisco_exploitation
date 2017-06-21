/* $Id: range.h,v 3.2 1995/11/17 17:56:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/util/range.h,v $
 *------------------------------------------------------------------
 * range.h - Definitions for range arithmetic
 *
 * Sept., 1992 Tony Li
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: range.h,v $
 * Revision 3.2  1995/11/17  17:56:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * A range is an closed interval of integers, such as [1..10].
 * A range list is a just a list of ranges: [1..10], [13..40], [51].
 * Range lists are now implemented as trees, but you don't know this.
 * Range lists are kept in sorted order.
 */

typedef struct rangelist_ rangelist;

/*
 * range.c
 */ 
extern rangelist *rangelist_create(void);
extern void rangelist_destroy(rangelist *rl);
extern void rangelist_init(rangelist *rl, uint start, uint end);
extern boolean rangelist_malloc_one(rangelist *rl, uint *retval);
extern boolean rangelist_malloc_block(rangelist *rl, uint count, uint *lower);
extern boolean rangelist_return_one(rangelist *rl, uint value);
extern boolean rangelist_return_block(rangelist *rl, uint low, uint high);
extern void show_rangelist(rangelist *rl);



