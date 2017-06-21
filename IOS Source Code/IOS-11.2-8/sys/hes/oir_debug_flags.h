/* $Id: oir_debug_flags.h,v 3.2 1995/11/17 09:19:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/oir_debug_flags.h,v $
 *------------------------------------------------------------------
 * oir_debug_flags.h -- Debugging flag declarations for OIR
 *
 * November 1994, William H. Palmer
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: oir_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:19:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(oir_debug_arr)

DEBUG_FLAG(oir_debug,DEBUG_OIR,"Online Insertion and Removal")

DEBUG_ARRDONE

extern void oir_debug_init(void);
extern void oir_parser_init(void);
