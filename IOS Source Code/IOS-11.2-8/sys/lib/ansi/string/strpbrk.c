/* $Id: strpbrk.c,v 3.1.10.1 1996/03/18 20:49:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strpbrk.c,v $
 *------------------------------------------------------------------
 * strpbrk.c -- find chars in string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strpbrk.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:56  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:10  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strpbrk---find chars in string

ANSI_SYNOPSIS
        #include <string.h>
        char *strpbrk(const char *s1, const char *s2);

DESCRIPTION
        This function locates the first occurence in the string
        pointed to by s1 of any character in string pointed to by s2
        (excluding the terminating null character).

RETURNS
        strpbrk returns a pointer to the character found in s1, or a
        null pointer if no character from s2 occurs in s1.

PORTABILITY
        strpbrk is ANSI C.

        strpbrk requires no supporting OS subroutines.
*/

#include <string.h>

char *
strpbrk (const char *s1, const char *s2)
{
    const char *c = s2;

    if (!*s1)
        return (char *) NULL;

    while (*s1) {
        for (c = s2; *c; c++) {
            if (*s1 == *c)
                break;
        }
        if (*c)
            break;
        s1++;
    }

    if (*c == '\0')
        s1 = NULL;

    return (char *) s1;
}

#if 0
/* previous cisco implementation */
/*
 * strpbrk - return pointer to first occurrence in string cs of any character
 * of string ct, or NULL if none are present
 */
char *
strpbrk(char *cs, char *ct)
{
    char *s1=cs, *scnp;
    int c,sc;

    while ((c=*s1++) != 0) {
        for (scnp=ct;(sc = *scnp++)!=0;)
            if (sc == c)
                return (s1-1);
    }
    return(NULL);
}

#endif
