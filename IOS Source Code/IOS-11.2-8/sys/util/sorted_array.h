/* $Id: sorted_array.h,v 3.2 1995/11/17 17:56:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/util/sorted_array.h,v $
 *------------------------------------------------------------------
 * sorted_array.h -- interface to sorted array manipulation routines
 *
 * January 1995, Ravishankar Chandrasekaran
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sorted_array.h,v $
 * Revision 3.2  1995/11/17  17:56:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/06  14:23:56  jacobt
 * CSCdi36607:  need show memory summary command
 *
 * Revision 2.1  1995/06/07  23:13:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SORTED_ARRAY_H__
#define	__SORTED_ARRAY_H__

extern void sorted_array_add(ulong *, int *, ulong);
extern boolean sorted_array_partial_compare(ulong *, int, ulong *, int);
extern boolean sorted_array_get_index (ulong *array, int count,
				       int *pindex, ulong number);
extern boolean sorted_array_get_index_2(ulong *array_1, ulong *array_2,
					int count, int *pindex,
					ulong number_1, ulong number_2);
extern void sorted_array_add_2 (ulong *array_1, ulong *array_2,
				int *pcount,
				ulong number_1, ulong number_2);
#endif __SORTED_ARRAY_H__
