/* $Id: qsort.h,v 3.1 1995/11/09 13:43:26 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/util/qsort.h,v $
 *------------------------------------------------------------------
 * Quicksort declarations
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: qsort.h,v $
 * Revision 3.1  1995/11/09  13:43:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/09  16:18:43  jjohnson
 * CSCdi38326:  quicksort should be a utility function
 * delete arap_qsort from the arap subsystem
 * add new utility function qsort to libutil.a
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void qsort(void *, size_t, size_t, int (*) (const void *, const void *));
