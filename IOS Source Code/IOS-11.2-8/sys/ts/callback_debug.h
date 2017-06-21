/* $Id: callback_debug.h,v 3.2 1995/11/17 18:52:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/callback_debug.h,v $
 *------------------------------------------------------------------
 * Callback debug definitions
 *
 * April 1995, Shoou Yiu
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: callback_debug.h,v $
 * Revision 3.2  1995/11/17  18:52:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _CALLBACK_DEBUG_H_
#define _CALLBACK_DEBUG_H_

extern void callback_debug_init (void);

/*
 * The actual debugging flags are defined in chat_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ts/callback_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ts/callback_debug_flags.h"

#endif /* _CALLBACK_DEBUG_H_ */
