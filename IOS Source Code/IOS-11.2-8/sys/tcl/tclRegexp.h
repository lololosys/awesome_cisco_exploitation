/* $Id: tclRegexp.h,v 3.2 1995/11/17 18:46:59 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcl/tclRegexp.h,v $
 *------------------------------------------------------------------
 * tclRegexp.h - Definitions etc. for regexp(3) routines.
 * April 1994, Brad Benson
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tclRegexp.h,v $
 * Revision 3.2  1995/11/17  18:46:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:04:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _TCL
#include "tcl.h"
#endif
#ifndef _REGEXP
#define _REGEXP 1

#define NSUBEXP  10
typedef struct TclRegexp {
	char *startp[NSUBEXP];
	char *endp[NSUBEXP];
	char regstart;		/* Internal use only. */
	char reganch;		/* Internal use only. */
	char *regmust;		/* Internal use only. */
	int regmlen;		/* Internal use only. */
	char program[1];	/* Unwarranted chumminess with compiler. */
} TclRegexp;

extern TclRegexp *TclRegComp _ANSI_ARGS_((char *exp));
extern int TclRegExec _ANSI_ARGS_((TclRegexp *prog, char *string, char *start));
extern void TclRegSub _ANSI_ARGS_((TclRegexp *prog, char *source, char *dest));
extern void TclRegError _ANSI_ARGS_((char *msg));

#endif /* REGEXP */
