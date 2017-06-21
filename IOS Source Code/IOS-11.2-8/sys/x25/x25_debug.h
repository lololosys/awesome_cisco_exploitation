/* $Id: x25_debug.h,v 3.2 1995/11/17 18:07:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/x25_debug.h,v $
 *------------------------------------------------------------------
 * x25_debug.h -- Debugging declarations for X.25
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_debug.h,v $
 * Revision 3.2  1995/11/17  18:07:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:22:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up X.25 debugging */
extern void x25_debug_init(void);

/*
 * It's possible to restrict X.25 debugging to a single VC. This variable
 * says which one.
 */
extern int x25_vcnum;

/*
 * The DEBUG_X25VC item lies in the same code space as the flag codes
 * defined in x25_debug_flags.h... it's passed to debug_x25_command as
 * a csb->which value. The regular flags start from zero, so this one
 * is negative. DEBUG_X25VC is used to set up debugging for a selected
 * X.25 LCN.
 */

#define DEBUG_X25VC -1

/*
 * The actual debugging flags are defined in x25_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../x25/x25_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../x25/x25_debug_flags.h"
