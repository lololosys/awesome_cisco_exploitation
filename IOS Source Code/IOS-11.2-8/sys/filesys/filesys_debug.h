/* $Id: filesys_debug.h,v 3.2 1995/11/17 09:07:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/filesys_debug.h,v $
 *------------------------------------------------------------------
 * Headers for routines used for File system debugging.
 *
 * Oct 1995, Yan Ke
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filesys_debug.h,v $
 * Revision 3.2  1995/11/17  09:07:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  17:46:20  yanke
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up Filesys debugging */
extern void filesys_debug_init(void);

/*
 * The actual debugging flags are defined in filesys_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../filesys/filesys_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../filesys/filesys_debug_flags.h"
