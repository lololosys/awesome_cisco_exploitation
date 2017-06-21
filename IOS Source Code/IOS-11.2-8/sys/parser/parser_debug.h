/* $Id: parser_debug.h,v 3.2 1995/11/17 18:48:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/parser_debug.h,v $
 *------------------------------------------------------------------
 * parser_debug.h -- Debugging declarations for Command parser
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_debug.h,v $
 * Revision 3.2  1995/11/17  18:48:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up parser debugging */
extern void debug_parser_init(void);

/*
 * The actual debugging flags are defined in parser_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../parser/parser_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../parser/parser_debug_flags.h"
