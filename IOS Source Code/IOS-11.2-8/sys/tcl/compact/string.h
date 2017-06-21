/* $Id: string.h,v 3.2 1995/11/17 18:47:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcl/compact/string.h,v $ 
 *------------------------------------------------------------------
 * string.h
 * April 1994, Brad Benson
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: string.h,v $
 * Revision 3.2  1995/11/17  18:47:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/12  19:00:17  cparrott
 * CSCdi35565:  TCL image doesnt compile.
 *
 * Revision 2.1  1995/06/07  23:05:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * string.h --
 *
 *	Declarations of ANSI C library procedures for string handling.
 *
 * Copyright (c) 1991-1993 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 */

#ifndef _STRING
#define _STRING


extern void *		memchr _ANSI_ARGS_((CONST VOID *s, int c, size_t n));
extern void *		memmove _ANSI_ARGS_((VOID *t, CONST VOID *f,
			    size_t n));

extern int		strcasecmp _ANSI_ARGS_((CONST char *s1,
			    CONST char *s2));
extern int		strcmp _ANSI_ARGS_((CONST char *s1, CONST char *s2));
extern char *		strcpy _ANSI_ARGS_((char *dst, CONST char *src));
extern char *		strdup _ANSI_ARGS_((CONST char *string));
extern char *		strerror _ANSI_ARGS_((int error));
extern size_t		strlen _ANSI_ARGS_((CONST char *string));
extern char *		strrchr _ANSI_ARGS_((CONST char *string, int c));
extern size_t		strspn _ANSI_ARGS_((CONST char *string,
			    CONST char *chars));
extern char *		strstr _ANSI_ARGS_((CONST char *string,
			    CONST char *substring));

#endif /* _STRING */
