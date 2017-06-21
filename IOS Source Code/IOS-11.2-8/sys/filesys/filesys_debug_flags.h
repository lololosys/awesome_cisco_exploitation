/* $Id: filesys_debug_flags.h,v 3.2 1995/11/17 09:07:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/filesys_debug_flags.h,v $
 *------------------------------------------------------------------
 * debugging flags for file system.
 *
 * Oct 1995, Yan Ke
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filesys_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:07:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  17:46:22  yanke
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from Filesys code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug.h"
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(filesys_debug_arr)

DEBUG_FLAG(filesys_debug, DEBUG_FILESYS, "File system error messages")

DEBUG_ARRDONE
