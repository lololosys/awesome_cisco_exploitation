/* $Id: qllc_debug.h,v 3.2 1995/11/17 18:55:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/qllc_debug.h,v $
 *------------------------------------------------------------------
 * QLLC: define debug flags
 *
 * April, 1994          Phil Morton
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllc_debug.h,v $
 * Revision 3.2  1995/11/17  18:55:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:01:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* function to start qllc debugging */
extern void qllc_debug_init(void); 

/* parser function for qllc debugging commands */
extern void qllc_debug_command(parseinfo *csb);

/* the actual debugging flags are defined in qllc_debug_flags.h    */
/* the file is included twice, once to define the flags themselves */
/* and once to define the indices that the parser uses             */

#include "../srt/qllc_debug_flags.h" 
#define __DECLARE_DEBUG_NUMS__ 
#include "../srt/qllc_debug_flags.h"
			     









