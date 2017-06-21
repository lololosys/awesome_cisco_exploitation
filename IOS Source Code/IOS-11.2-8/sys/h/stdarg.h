/* $Id: stdarg.h,v 3.2 1995/11/17 09:13:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/stdarg.h,v $
 *------------------------------------------------------------------
 * stdarg.h -- macros for writing portable routines with variable arguments
 *
 * 28-December-1991, Kirk Lougheed
 *
 * Converted from varargs.h to stdarg.h (ANSI compliance) by Kevin Herbert
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 *------------------------------------------------------------------
 * $Log: stdarg.h,v $
 * Revision 3.2  1995/11/17  09:13:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:36:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __STDARG_H__
#define __STDARG_H__

#ifdef __sparc__
#define va_start(AP, LASTARG)    \
 (__builtin_saveregs (),   \
  AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))
#else
#define va_start(AP, LASTARG)  \
  ( AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))
#endif

#define va_end(AP)

#define __va_rounded_size(TYPE)  \
  (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#ifdef __mips
#define va_arg(AP, TYPE)						\
    ((TYPE *) (void *)(AP = (char *) (__alignof(TYPE) > 4		\
		   ? ((int)AP + 2*8 - 1) & -8	                        \
		   : ((int)AP + 2*4 - 1) & -4)))[-1]

#else

#define va_arg(AP, TYPE)  \
 (AP += __va_rounded_size (TYPE),    \
  *((TYPE *) (AP - ((sizeof(TYPE) < 4)  \
		    ? sizeof(TYPE)  \
		    : __va_rounded_size (TYPE)))))

#endif __mips

#endif __STDARG_H__
