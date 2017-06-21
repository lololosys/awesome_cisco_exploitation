/* $Id: labs.c,v 3.1.10.1 1996/03/18 20:49:08 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/labs.c,v $
 *------------------------------------------------------------------
 * labs.c -- integer absolute value (magnitude)
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: labs.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:41  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        labs---integer absolute value (magnitude)

ANSI_SYNOPSIS
        #include <stdlib.h>
        long int abs(long int i);

DESCRIPTION
        labs returns |x|, the absolute value of i (also called the
        magnitude of i).  That is, if i is negative, the result is the
        opposite of i, but if i is nonnegative the result is i.

        The similar function labs uses and returns long rather than
        int values.

RETURNS
        The result is a nonnegative integer.

PORTABILITY
        abs is ANSI.

        No supporting OS subroutines are required.
*/

#include <stdlib.h>
#undef labs

long int
labs (long int j)
{
    return ((j < 0) ? -j : j);
}
