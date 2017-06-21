/* $Id: dlsw_core_debug.h,v 3.2 1995/11/17 09:02:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dlsw/dlsw_core_debug.h,v $
 *------------------------------------------------------------------
 * All the core specific debug flags are defined in here
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Includes the function prototypes for debug specific routines
 *------------------------------------------------------------------
 * $Log: dlsw_core_debug.h,v $
 * Revision 3.2  1995/11/17  09:02:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:22:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:54:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:26:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up DLSW debugging */
extern void dlsw_core_debug_init(void);

/*
 * The actual debugging flags are defined in dlsw_core_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../dlsw/dlsw_core_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../dlsw/dlsw_core_debug_flags.h"


extern void dlsw_core_debug_all (boolean flag);
extern void dlsw_core_debug_init (void);

#define DEBUG_DLSW_CORE_SUM      	10
#define DLSW_SHOW_CIRCUITS_FLOW 1
