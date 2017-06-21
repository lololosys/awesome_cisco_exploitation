/* $Id: lex_debug_flags.h,v 3.2 1995/11/17 17:43:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_debug_flags.h,v $
 *------------------------------------------------------------------
 * lex_debug_flags.h -- Debugging flag declarations for LEX
 *
 * Feb. 1994, David Coli
 *
 * Copyright (c) 1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lex_debug_flags.h,v $
 * Revision 3.2  1995/11/17  17:43:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:40:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from LEX code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(lex_debug_arr)

DEBUG_FLAG(lex_rcmd_debug,DEBUG_LEX_RCMD,"LEX Remote Commands")

DEBUG_ARRDONE
