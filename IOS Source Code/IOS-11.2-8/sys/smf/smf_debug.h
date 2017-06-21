/* $Id: smf_debug.h,v 3.2 1995/11/17 18:52:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smf/smf_debug.h,v $
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
 * $Log: smf_debug.h,v $
 * Revision 3.2  1995/11/17  18:52:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:44:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up SMF debugging */
extern void smf_debug_init(void);

/*
 * The actual debugging flags are defined in smf_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "smf_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "smf_debug_flags.h"
