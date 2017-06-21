/* $Id: os.h,v 3.2.62.1 1996/03/18 22:56:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xremote/os.h,v $
 *------------------------------------------------------------------
 * os.h -- OS-specific X definitions for Xremote
 *
 * September 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: os.h,v $
 * Revision 3.2.62.1  1996/03/18  22:56:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:23:19  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  22:02:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:26:37  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/25  11:46:01  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 23:31:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef OS_H
#define OS_H
#include "misc.h"

#define Xalloc(x) malloc(x)
#define Xfree(x) free(x)

#include <errno.h>

#endif OS_H
