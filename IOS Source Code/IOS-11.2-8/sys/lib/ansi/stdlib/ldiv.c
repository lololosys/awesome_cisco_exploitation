/* $Id: ldiv.c,v 3.1.10.1 1996/03/18 20:49:09 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/ldiv.c,v $
 *------------------------------------------------------------------
 * ldiv.c -- divide two long integers
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ldiv.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:42  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ldiv.c
 * Original Author:     G. Haley
 *
 * Computes the quotient and remainder of the division of the numerator n by
 * the denominator d. If the division is inexact, the sign of the quotient is
 * that of the mathematical quotient, and the magnitude of the quotient is the
 * largest integer less than the magnitude of the mathematical quotient. If the
 * result cannot be represented, the behaviour is undefined. Returns the
 * results in a structure of type ldiv_t, comprising the following members:
 *
 *      long quot;
 *      long rem;
 */

/*
FUNCTION
        ldiv---divide two long integers

ANSI_SYNOPSIS
        #include <stdlib.h>
        ldiv_t ldiv(long n, long d);

DESCRIPTION
        Divide n/d, returning quotient and remainder as two integers
        in a structure ldiv_t.

RETURNS
        The result is represented with the structure

        typedef struct {
            long quot;
            long rem;
        } ldiv_t;

        where the quot field represents the quotient, and rem the
        remainder.  For nonzero d, if `r = ldiv(n,d);' then
        n equals `r.rem + d*r.quot'.

        When d is zero, the quot member of the result has the same
        sign as n and the largest representable magnitude.

        To divide int rather than long values, use the similar
        function div.

PORTABILITY

        ldiv is ANSI, but the behavior for zero d is not specified by
        the standard.

        No supporting OS subroutines are required.
*/

#include <limits.h>
#include <stdlib.h>

ldiv_t
ldiv (long n, long d)
{
    ldiv_t res;

    if (d) {
        res.quot = labs (n) / labs (d);
        res.rem = labs (n) % labs (d);

        if ((n < 0 && d > 0) || (n >= 0 && d < 0))
            res.quot = -res.quot;
        if (n < 0)
            res.rem = -res.rem;
    }
    else {
        if (n < 0)
            res.quot = LONG_MIN;
        else
            res.quot = LONG_MAX;

        res.rem = 0;
    }

    return res;
}
