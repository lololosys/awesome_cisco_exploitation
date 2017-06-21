/* $Id: smf_debug_flags.h,v 3.2 1995/11/17 18:52:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smf/smf_debug_flags.h,v $
 *------------------------------------------------------------------
 * Software MAC Address Filter (SMF) debug flags.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The SMF is intended for use when an interface is listening
 * promiscuously to match and dispose of received packets based
 * upon their destination MAC addresses in an efficient way.
 * In particular, it provides a facility that may be used on a
 * per-interface basis in support of Concurrent Routing and
 * Bridging (CRB) and eventually Integrated Routing and Bridging (IRB).
 *
 *------------------------------------------------------------------
 * $Log: smf_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:52:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/14  20:39:51  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * filesys/fslib.c            suported -> supported
 * os/msg_system.c            unititialized -> uninitialized
 * parser/cfg_routemap_set.h  substract -> subtract
 * smf/smf_debug_flags.h      missses -> misses
 *
 * Revision 2.1  1995/06/07  22:44:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from TBRIDGE code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(smf_debug_arr)

DEBUG_FLAG(smf_debug_update, DEBUG_SMF_UPDATE,
	   "Software MAC filter address insertions and deletions")
DEBUG_FLAG(smf_debug_match, DEBUG_SMF_MATCH,
	   "Software MAC filter address matches")
DEBUG_FLAG(smf_debug_miss, DEBUG_SMF_MISS,
	   "Software MAC filter address misses")

DEBUG_ARRDONE

/*
 * Defining SMF_DEBUG will enable a bunch of sfw MAC filter debugging code
 * that should never be included in customer images of any description.
 * It will impair performance and produce a potentially huge number of
 * debug messages.  Define this ONLY for debugging development images.
 * The commands to enable the different levels of debugging are hidden.

#define SMF_DEBUG

 */
