/* $Id: tclMtherr.c,v 3.2 1995/11/17 18:46:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcl/tclMtherr.c,v $
 *------------------------------------------------------------------
 * tclMatherr.c --
 *
 *      This function provides a default implementation of the
 *      "matherr" function, for SYS-V systems where it's needed.
 * April 1994, Brad Benson
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tclMtherr.c,v $
 * Revision 3.2  1995/11/17  18:46:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:04:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Copyright (c) 1993 The Regents of the University of California.
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
 */

#include "tclInt.h"

#ifndef TCL_GENERIC_ONLY
#include "tclUnix.h"
#else
#define NO_ERRNO_H
#endif

#ifdef NO_ERRNO_H
extern int errno;			/* Use errno from tclExpr.c. */
#define EDOM 33
#define ERANGE 34
#endif

/*
 * The following variable is secretly shared with Tcl so we can
 * tell if expression evaluation is in progress.  If not, matherr
 * just emulates the default behavior, which includes printing
 * a message.
 */

extern int tcl_MathInProgress;


/*
 *----------------------------------------------------------------------
 *
 * matherr --
 *
 *	This procedure is invoked on Sys-V systems when certain
 *	errors occur in mathematical functions.  Type "man matherr"
 *	for more information on how this function works.
 *
 * Results:
 *	Returns 1 to indicate that we've handled the error
 *	locally.
 *
 * Side effects:
 *	Sets errno based on what's in xPtr.
 *
 *----------------------------------------------------------------------
 */

int
matherr( struct exception *xPtr )	/* Describes error that occurred. */
{
    if (!tcl_MathInProgress) {
	return 0;
    }
    if ((xPtr->type == DOMAIN) || (xPtr->type == SING)) {
	errno = EDOM;
    } else {
	errno = ERANGE;
    }
    return 1;
}
