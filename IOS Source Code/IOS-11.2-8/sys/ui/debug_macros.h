/* $Id: debug_macros.h,v 3.3.62.1 1996/07/23 18:32:52 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ui/debug_macros.h,v $
 *------------------------------------------------------------------
 * debug_kludge.h -- Macros for generic debug DEBUG_FLAG declaration
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug_macros.h,v $
 * Revision 3.3.62.1  1996/07/23  18:32:52  snyder
 * CSCdi63791:  debugging arrays should be const
 *              4112 out of data space, 556 out of image
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  17:47:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:46:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:40:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:11:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug.h"

/* This perverted abuse of the C preprocessor is designed to keep us from
 * having to keep too many files consistent to make debugging DEBUG_FLAGs work.
 * To make a standard debugging DEBUG_FLAG, we have to create 3 things:
 *
 * o A boolean flag variable (tested by the routines that print the actual
 *   debugging output)
 *
 * o An array element that associates that variable with a textual
 *   description (used by the parsing and printing routines to talk about
 *   the flag)
 *
 * o A symbolic name for the array index (used by parse chains to designate
 *   which flag is to be set or cleared within an array).
 *
 * Rather than ask a human being to type in all three definitions, we use
 * the macros below and some include file hackery to create them all from
 * the same data. If the magic symbol "__DECLARE_DEBUG_ARR__" is defined,
 * we assume that whatever file includes us wants to actually declare
 * initialized data... presumably we've been included by an xxx_debug.c
 * module. If "__DECLARE_DEBUG_NUMS__" is defined instead, we assume
 * that an include file wants to create the symbolic names for array
 * offsets. If neither magic symbol is defined, we assume that an include
 * file (probably the same one) wants to create external declarations
 * for the booleans themselves, as well as an external declaration for
 * the array.
 *
 * There are some instances in which a debug flag is defined that should
 * not be declared by us;  in such cases the DEBUG_FLAG_NO_DECL macro
 * should be used, rather than the DEBUG_FLAG macro.
 */

/* We've probably been called before, so let's clean up what we did then */
#undef DEBUG_ARRDECL
#undef DEBUG_FLAG
#undef DEBUG_ARRDONE
#undef DEBUG_FLAG_NO_DECL

#if defined(__DECLARE_DEBUG_ARR__)

#define DEBUG_ARRDECL(arrname) const debug_item_type arrname[] = {
#define DEBUG_FLAG(varname,codename,textname) {&varname, textname},
#define DEBUG_FLAG_NO_DECL(varname,codename,textname) {&varname, textname},
#define DEBUG_ARRDONE {(boolean *) NULL, (char *) NULL} };

#elif defined(__DECLARE_DEBUG_NUMS__)

#define DEBUG_ARRDECL(arrname) enum { 
#define DEBUG_FLAG(varname,codename,textname) codename,
#define DEBUG_FLAG_NO_DECL(varname,codename,textname) codename,
#define DEBUG_ARRDONE };

#else

#define DEBUG_ARRDECL(arrname) extern const debug_item_type arrname[];
#define DEBUG_FLAG(varname,codename,textname) boolean varname;
#define DEBUG_FLAG_NO_DECL(varname,codename,textname) /* Nothing */
#define DEBUG_ARRDONE /* Nothing */

#endif

/* Some other include file might want to reuse us... */
#undef __DECLARE_DEBUG_ARR__
#undef __DECLARE_DEBUG_NUMS__
