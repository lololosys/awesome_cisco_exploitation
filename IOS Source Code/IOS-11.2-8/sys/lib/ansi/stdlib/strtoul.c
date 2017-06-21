/* $Id: strtoul.c,v 3.1.10.1 1996/03/18 20:49:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/strtoul.c,v $
 *------------------------------------------------------------------
 * strtoul.c -- convert an unsigned number string to unsigned long
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strtoul.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.2  1996/03/08  19:49:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 * The POSIX standard says strtoul() MUST allow an optional
 * leading + or - sign.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:44  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strtoul---string to unsigned long

ANSI_SYNOPSIS
        #include <stdlib.h>
        unsigned long strtoul(const char *s, char **ptr,
                              int base);

        unsigned long _strtoul_r(void *reent, const char *s,
                              char **ptr, int base);

DESCRIPTION

        The function strtoul converts the string *s to an unsigned
        long. First, it breaks down the string into three parts:
        leading whitespace, which is ignored; a subject string
        consisting of the digits meaningful in the radix specified by
        base (for example, 0 through 7 if the value of base is 8); and
        a trailing portion consisting of one or more unparseable
        characters, which always includes the terminating null
        character. Then, it attempts to convert the subject string
        into an unsigned long integer, and returns the result.

        If the value of base is zero, the subject string is expected
        to look like a normal C integer constant (save that no
        optional sign is permitted): a possible 0x indicating
        hexadecimal radix, and a number.  If base is between 2 and 36,
        the expected form of the subject is a sequence of digits
        (which may include letters, depending on the base)
        representing an integer in the radix specified by base.  The
        letters a--z (or A--Z) are used as digits valued from 10 to
        35. If base is 16, a leading 0x is permitted.

        The subject sequence is the longest initial sequence of the
        input string that has the expected form, starting with the
        first non-whitespace character.  If the string is empty or
        consists entirely of whitespace, or if the first
        non-whitespace character is not a permissible digit, the
        subject string is empty.

        If the subject string is acceptable, and the value of base is
        zero, strtoul attempts to determine the radix from the input
        string. A string with a leading 0x is treated as a hexadecimal
        value; a string with a leading 0 and no x is treated as octal;
        all other strings are treated as decimal. If base is between 2
        and 36, it is used as the conversion radix, as described
        above. Finally, a pointer to the first character past the
        converted subject string is stored in ptr, if ptr is not NULL.

        If the subject string is empty (that is, if *s does not start
        with a substring in acceptable form), no conversion is
        performed and the value of s is stored in ptr (if ptr is not
        NULL).

        The alternate function _strtoul_r is a reentrant version.  The
        extra argument reent is a pointer to a reentrancy structure.

        The strtoul() function converts the string pointed to by s to
        long using the following algorithm:

            1. Remove leading whie space by testing characters with
               the isspace() function.

            2. Remove an optional + or - character. That's right, a
               minus sign is legal!

            3. If base is zero, remove a constant using the rules of
               the C programming language (initial 0x or 0X for hex,
               initial 0 for octal, etc.).

            4. If base is between 2 and 36, scan for a series of
               characters in the specified radix. The characters a (or
               A) to z (or Z) are used for 10 to 35.

            5. If endptr is not NULL, store a pointer to the first
               unrecognized character (which may be the final null
               character) in the pointer pointed to by ptr.

            6. Convert the characters scanned in steps 2 to 4 to long
               and return the result.


RETURNS
        strtoul returns the converted value, if any. If no conversion
        was made, 0 is returned.

        strtoul returns ULONG_MAX if the magnitude of the converted
        value is too large, and sets errno to ERANGE.

PORTABILITY
        strtoul is ANSI.

        strtoul requires no supporting OS subroutines.
*/

/*
 * Andy Wilson, 2-Oct-89.
 */

#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <reent.h>
#include <../reent/reent_private.h>
#include "stdlib_private.h"

unsigned long
_strtoul_r (const char *s, char **ptr, int base, int *err)
{
    unsigned long total = 0;
    unsigned int digit;
    int radix;
    const char *start = s;
    int did_conversion = 0;
    int overflow = 0;
    int minus = 0;
    unsigned long maxdiv, maxrem;

    if (s == NULL) {
        *err = ERANGE;
        if (!ptr)
            *ptr = (char *) start;
        return(0L);
    }

    while (Isspace (*s))
        s++;

    /*
     * yes, this is silly, but the standard says that leading sign is
     * legal.
     */
    if (*s == '-') {
      s++;
      minus = 1;
    }
    else if (*s == '+')
      s++;

    radix = base;
    if (base == 0 || base == 16) {
        /*
         * try to infer radix from the string (assume decimal).
         * accept leading 0x or 0X for base 16.
         */
        if (*s == '0') {
            did_conversion = 1;
            if (base == 0)
                radix = 8;              /* guess it's octal */
            s++;                        /* (but check for hex) */
            if (*s == 'X' || *s == 'x') {
                did_conversion = 0;
                s++;
                radix = 16;
            }
        }
    }
    if (radix == 0)
        radix = 10;

    maxdiv = ULONG_MAX / radix;
    maxrem = ULONG_MAX % radix;

    while ((digit = *s) != 0) {
        if (digit >= '0' && digit <= '9' && digit < ('0' + radix))
            digit -= '0';
        else if (radix > 10) {
            if (digit >= 'a' && digit < ('a' + radix - 10))
                digit = digit - 'a' + 10;
            else if (digit >= 'A' && digit < ('A' + radix - 10))
                digit = digit - 'A' + 10;
            else
                break;
        }
        else
            break;
        did_conversion = 1;
        if (total > maxdiv
            || (total == maxdiv && digit > maxrem))
            overflow = 1;
        total = (total * radix) + digit;
        s++;
    }
    if (overflow) {
        *err = ERANGE;
        if (ptr != NULL)
            *ptr = (char *) s;
        return(ULONG_MAX);
    }
    if (ptr != NULL)
        *ptr = (char *) ((did_conversion) ? (char *) s : start);

    return(minus ? - total : total);
}

unsigned long
strtoul (const char *s, char **ptr, int base)
{
    int err = 0;
    unsigned long result;

    result = _strtoul_r(s, ptr, base, &err);

    if (err) {
        _REENT->_errno = err;
    }

    return(result);
}
