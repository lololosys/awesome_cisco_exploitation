/* $Id: abs.c,v 3.1.10.1 1996/03/18 20:49:02 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/abs.c,v $
 *------------------------------------------------------------------
 * abs.c -- integer absolute value (magnitude)
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: abs.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:36  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        abs---integer absolute value (magnitude)

ANSI_SYNOPSIS
        #include <stdlib.h>
        int abs(int i);

DESCRIPTION
        abs returns |x|, the absolute value of i (also called the
        magnitude of i).  That is, if i is negative, the result is the
        opposite of i, but if i is nonnegative the result is i.

        The similar function labs uses and returns long rather than
        int values.

        When compiled with the GNU C compiler, the abs function is a
        macro that expands to an expression.

RETURNS
        The result is a nonnegative integer.

PORTABILITY
        abs is ANSI.

        No supporting OS subroutines are required.
*/

#include <stdlib.h>
#undef abs

int
abs (int i)
{
    return ((i < 0) ? -i : i);
}
