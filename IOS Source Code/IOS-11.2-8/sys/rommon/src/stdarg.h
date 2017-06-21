/* $Id: stdarg.h,v 3.2 1995/11/17 18:43:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/stdarg.h,v $
 *------------------------------------------------------------------
 * stdarg.h
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Standard Variable Argument macros for M68k/MIPS.
 *------------------------------------------------------------------
 * $Log: stdarg.h,v $
 * Revision 3.2  1995/11/17  18:43:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __STDARG_H__
#define __STDARG_H__

/*
 * Note : va_rollback is a non-standard macro. It is used by query_user()
 * to allow the user to re-specify the size of an operation.
 */

/*
 * va_list variables are pointer to char
 */
typedef char *va_list;

/*
 * If things are being compiled with -mips3, then everything
 * gets stacked as 8byte quantities. Otherwise they are stacked
 * as 4byte quantities
 */
#if __mips == 3
#define VA_ARG_MULTI	4
#else
#define VA_ARG_MULTI	2
#endif

/* 
 * Amount of space required in an argument list for an arg of type TYPE.
 * TYPE may alternatively be an expression whose type is used.  
 */
#define __va_rounded_size(TYPE)                                         \
    (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

/*
 * Initialize AP, va list pointer
 */
#define va_start(AP, LASTARG) 						\
    (AP = ((char *) &(LASTARG) + __va_rounded_size (LASTARG)))

/*
 * End va list processing
 */
#define va_end(AP)

#ifndef __mips

/*
 * Return present argument and increment AP
 */
#define va_arg(AP, TYPE)						\
 (AP += __va_rounded_size (TYPE),					\
  *((TYPE *) (AP - __va_rounded_size (TYPE))))

/*
 * Rollback AP to point to the previous argument
 */
#define va_rollback(AP, TYPE)                                           \
  (AP -= __va_rounded_size (TYPE))

#else  /* __mips */

/*
 * Return present argument and increment AP
 */
#define va_arg(AP, TYPE)						\
    ((TYPE *) (void *)(AP = (char *) (__alignof(TYPE) > 4		\
		   ? ((int)AP + VA_ARG_MULTI*8 - 1) &                   \
				      (-4 * VA_ARG_MULTI)	        \
		   : ((int)AP + VA_ARG_MULTI*4 - 1) &                   \
				      (-2 * VA_ARG_MULTI))))[-1]

/*
 * Rollback AP to point to the previous argument
 */
#define va_rollback(AP, TYPE)                                           \
    (AP = (char *)(__alignof(TYPE) > 4                                  \
		   ? ((int)AP - VA_ARG_MULTI*4 + 1) &                   \
		                     (-4 * VA_ARG_MULTI)                \
		   : ((int)AP - VA_ARG_MULTI*2 + 1) &                   \
		                     (-2 * VA_ARG_MULTI)))
		   
#endif /* __mips */

#endif  /* __STDARG_H__ */
