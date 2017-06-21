/* $Id: memtype.h,v 3.2 1995/11/17 18:51:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tn3270/memtype.h,v $
 *------------------------------------------------------------------
 * M E M T Y P E . H
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: memtype.h,v $
 * Revision 3.2  1995/11/17  18:51:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * These definitions must match the order of the elements in the size
 * array in memory.c.
 */

#define MEM_TYPE_FIELD	 (0x01)
#define MEM_TYPE_SEGMENT (0x02)

/*
 * Function definitions...
 */

void *memory_allocate   (struct screen_type_ *screen,char type);
void  memory_deallocate (struct screen_type_ *screen,void *mem);
