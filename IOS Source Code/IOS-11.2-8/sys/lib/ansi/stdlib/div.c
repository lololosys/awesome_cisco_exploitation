/* $Id: div.c,v 3.1.10.1 1996/03/18 20:49:07 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/div.c,v $
 *------------------------------------------------------------------
 * div.c -- divide two integers
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: div.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:40  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        div---divide two integers

ANSI_SYNOPSIS
        #include <stdlib.h>
        div_t div(int n, int d);

DESCRIPTION
        Divide n/d, returning quotient and remainder as two integers
        in a structure div_t.

RETURNS
        The result is represented with the structure

        typedef struct {
            int quot;
            int rem;
        } div_t;

        where the quot field represents the quotient, and rem the
        remainder.  For nonzero d, if `r = div(n,d);' then
        n equals `r.rem + d*r.quot'.

        When d is zero, the quot member of the result has the same
        sign as n and the largest representable magnitude.

        To divide long rather than int values, use the similar
        function ldiv.

PORTABILITY
        div is ANSI, but the behavior for zero d is not specified by
        the standard.

        No supporting OS subroutines are required.
*/

#include <limits.h>
#include <stdlib.h>

div_t
div (int n, int d)
{
    div_t res;

    if (d) {
        res.quot = abs (n) / abs (d);
        res.rem = abs (n) % abs (d);

        if ((n < 0 && d > 0) || (n >= 0 && d < 0))
            res.quot = -res.quot;
        if (n < 0)
            res.rem = -res.rem;
    }
    else {
        if (n < 0)
            res.quot = INT_MIN;
        else
            res.quot = INT_MAX;

        res.rem = 0;
    }

    return res;
}
