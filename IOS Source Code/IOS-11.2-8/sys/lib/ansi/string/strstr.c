/* $Id: strstr.c,v 3.1.10.1 1996/03/18 20:49:44 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strstr.c,v $
 *------------------------------------------------------------------
 * strstr.c -- find string segment
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strstr.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:14  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strstr---find string segment

ANSI_SYNOPSIS
        #include <string.h>
        char *strstr(const char *s1, const char *s2);

DESCRIPTION
        Locates the first occurence in the string pointed to by s1 of
        the sequence of characters in the string pointed to by s2
        (excluding the terminating null character).

RETURNS
        Returns a pointer to the located string segment, or a null
        pointer if the string s2 is not found. If s2 points to a
        string with zero length, the s1 is returned.

PORTABILITY
        strstr is ANSI C.

        strstr requires no supporting OS subroutines.
*/

#include <string.h>

/*
 * strstr
 * substring match routing
 * returns the start of the s2 in s1 if it matches, else it returns NULL.
 */
#if 0
/*
 * perhaps a more efficient implementation of strstr, but not well tested
 */
char *
strstr (const char *searchee, const char *lookfor)
{
    size_t i;

    if (searchee == lookfor)
        return((char *)searchee);

    if (*searchee == '\0') {
        if (*lookfor)
            return((char *) NULL);
        return((char *) searchee);
    }

    while (*searchee) {
        i = 0;

        while (1) {
            if (lookfor[i] == '\0') {
                return((char *) searchee);
            }

            if (lookfor[i] != searchee[i]) {
                break;
            }
            i++;
        }
        searchee++;
    }

    return((char *) NULL);
}
#else
/* old cisco implementation of strstr definition */

/*
 * strstr
 * substring match routing
 * returns the start of the s2 in s1 if it matches, else it returns NULL.
 */

char *strstr (const char *known, const char *substr)
{
    if (known == substr)
        return ((char *)known);

    while (*known) {
        /*
         * Check to see if we match on a character yet
         */
        if (*known++ == *substr) {
            /*
             * We do, so see if the rest of the substr matches
             */
            char *runner     = (char *)substr + 1;
            char *matchpoint = (char *)known - 1;

            while (*runner) {
                /*
                 * Compare our strings
                 */
                if (*known != *runner)
                    break;
                /*
                 * Step to the next comparison
                 */
                known++;
                runner++;
            }

            /*
             * Did we get to the end of the substring?
             */
            if (!(*runner))
                return(matchpoint);
        }
    }

    return(NULL);
}
#endif
