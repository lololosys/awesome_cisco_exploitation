/* $Id: isis_nlsp_proc_nums.h,v 3.3 1995/12/11 04:45:36 sluong Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/clns/isis_nlsp_proc_nums.h,v $
 *------------------------------------------------------------------
 * Definitions for ISIS/NLSP process numbers
 *
 * November 1994, Dave Katz
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: isis_nlsp_proc_nums.h,v $
 * Revision 3.3  1995/12/11  04:45:36  sluong
 * CSCdi45316:  Add route-aggregation command for nlsp
 * change to not automatically generate aggregate routes.
 * up the maximum number of nlsp instances to 28.
 *
 * Revision 3.2  1995/11/17  08:54:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:52:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:19:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ISIS_NLSP_PROC_NUMS_H_
#define _ISIS_NLSP_PROC_NUMS_H_

/*
 * Process numbers for NLSP and ISIS.  These are used to distinguish
 * ISIS and NLSP as protocols, *not* as instances of protocols.
 *
 * Used as indices in debug flags and traffic flags, and in clnspdb->proc_type
 * among other places.
 */

typedef enum {
    PROC_TYPE_ISIS,			/* IS-IS */
    PROC_TYPE_NLSP,			/* NLSP */
    ISIS_NUM_PROC_TYPE			/* How many do we have? */
} isis_proc_type;

#endif
