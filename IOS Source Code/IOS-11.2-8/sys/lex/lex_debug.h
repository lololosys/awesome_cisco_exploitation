/* $Id: lex_debug.h,v 3.2 1995/11/17 17:43:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_debug.h,v $
 *------------------------------------------------------------------
 * lex_debug.h -- Debugging declarations for LEX
 *
 * Feb. 1994, David Coli
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex_debug.h,v $
 * Revision 3.2  1995/11/17  17:43:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:40:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Function to fire up LEX debugging */
extern void lex_debug_init(void);

/*
 * The actual debugging flags are defined in lex_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../lex/lex_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../lex/lex_debug_flags.h"
