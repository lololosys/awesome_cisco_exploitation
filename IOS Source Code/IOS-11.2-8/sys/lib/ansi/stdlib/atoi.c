/* $Id: atoi.c,v 3.1.10.1 1996/03/18 20:49:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/atoi.c,v $
 *------------------------------------------------------------------
 * atoi.c -- ASCII to Integer conversion routine
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atoi.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:37  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        atoi---string to integer

ANSI_SYNOPSIS
        #include <stdlib.h>
        int atoi(const char *s);

DESCRIPTION
        atoi converts the initial portion of a string to an int.

RETURNS
        The functions return the converted value, if any. If no
        conversion was made, 0 is returned.

PORTABILITY
        atoi is ANSI.

        No supporting OS subroutines are required.
*/

#include <stdlib.h>
#include <errno.h>
#include <reent.h>
#include <../reent/reent_private.h>
#include "stdlib_private.h"

/*
 * atoi(), from the Latin, meaning to convert with great exuberance
 * from ASCII to integer
 */
int
_atoi_r (const char *s, int *err)
{
    int result = 0;
    int negate = 0;
    char *str = (char *) s;

    if (!str) {
        *err = ERANGE;
        return(0);              /* *err = ERANGE */
    }

    /* Get rid of leading white space. */
    while(Isspace(*str))
          str++;

    if (*str == '\0') {
        *err = ERANGE;
        return(0);              /* *err = ERANGE */
    }

    if (*str == '-') {
        negate = 1;
        str++;
    }
    else if (*str == '+') {
        str++;
    }

    for (; (*str != '\0') && Isdigit(*str); str++)
        result = (10 * result) + (*str - '0');

    return(negate ? -result : result);
}

int
atoi (const char *s)
{
    int err = 0;
    int result;

    result = _atoi_r(s, &err);

    if (err) {
	_REENT->_errno = err;
    }

    return result;
}

