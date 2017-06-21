/* $Id: mailbox_debug.h,v 3.2 1995/11/17 17:41:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/mailbox_debug.h,v $
 *------------------------------------------------------------------
 * mailbox_debug.h -- Debugging declarations for Chipcom mailbox
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mailbox_debug.h,v $
 * Revision 3.2  1995/11/17  17:41:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:38:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up mailbox debugging */
extern void mailbox_debug_init(void);

/*
 * The actual debugging flags are defined in mailbox_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../les/mailbox_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../les/mailbox_debug_flags.h"
