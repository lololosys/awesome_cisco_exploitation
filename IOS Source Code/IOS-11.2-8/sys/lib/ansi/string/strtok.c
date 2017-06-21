/* $Id: strtok.c,v 3.1.10.1 1996/03/18 20:49:46 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strtok.c,v $
 *------------------------------------------------------------------
 * strtok.c -- get next token from a string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strtok.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:17  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strtok---get next token from a string

ANSI_SYNOPSIS
        #include <string.h>
        char *strtok(char *source, const char *delimiters)

        char *_strtok_r(void *reent,
                        const char *source, const char *delimiters)

DESCRIPTION

        A series of calls to strtok break the string starting at
        source into a sequence of tokens.  The tokens are delimited
        from one another by characters from the string at delimiters,
        at the outset.

        The first call to strtok normally has a string address as the
        first argument; subsequent calls can use NULL as the first
        argument, to continue searching the same string.  You can
        continue searching a single string with different delimiters
        by using a different delimiter string on each call.

        strtok begins by searching for any character not in the
        delimiters string: the first such character is the beginning
        of a token (and its address will be the result of the strtok
        call). strtok then continues searching until it finds another
        delimiter character; it replaces that character by NULL and
        returns.  (If strtok comes to the end of the *source string
        without finding any more delimiters, the entire remainder of
        the string is treated as the next token).

        strtok starts its search at *source, unless you pass NULL as
        the first argument; if source is NULL, strtok continues
        searching from the end of the last search. Exploiting the NULL
        first argument leads to non-reentrant code. You can easily
        circumvent this problem by saving the last delimiter address
        in your application, and always using it to pass a non-null
        source argument.

        _strtok_r performs the same function as strtok, but is
        reentrant.  The extra argument reent is a pointer to a
        reentrancy structure.

RETURNS
        strtok returns a pointer to the next token, or NULL if
        no more tokens can be found.

PORTABILITY
        strtok is ANSI C.

        strtok requires no supporting OS subroutines.

*/

#include <string.h>
#include <reent.h>
#include "../reent/reent_private.h"

char *                          /* NULL if no token left */
_strtok_r (_reent_t *rptr, char *s, const char *delim)
{
    char *scan;
    char *tok;
    const char *dscan;

    if (s != NULL)
        scan = s;
    else
        scan = rptr->_scanpoint;
    if (scan == NULL) {
	return(NULL);
    }

    /*
     * Scan leading delimiters.
     */

    for (; *scan != '\0'; scan++) {
        for (dscan = delim; *dscan != '\0'; dscan++) {
            if (*scan == *dscan)
                break;
        }

        if (*dscan == '\0')
            break;
    }
    if (*scan == '\0') {
        rptr->_scanpoint = NULL;
        return NULL;
    }

    tok = scan;

    /*
     * Scan token.
     */

    for (; *scan != '\0'; scan++) {
        for (dscan = delim; *dscan != '\0';)    /* ++ moved down. */
            if (*scan == *dscan++) {
                rptr->_scanpoint = scan + 1;
                *scan = '\0';
                return tok;
            }
    }

    /* Reached end of string. */
    rptr->_scanpoint = NULL;
    return tok;
}

char *
strtok (char *s, const char *delim)
{
    return _strtok_r(_REENT, s, delim);
}


#if 0
/* original sys/util/string.c version of strtok() */

/*
 * strtok - searches s for tokens delimited by characters from ct
 *
 * This little horror is definitely NOT reentrant.
 *
 * Do not dismiss the process between calls!
 * Do not call from interrupt!
 */
char *strtok(char *s, const char *ct)
{
    char *spnp;
    int c, sc;
    char *tok;
    static char *last;

   if ( !s && (s = last)==NULL)
      return (NULL);

cont:
   c = *s++;
   for (spnp=(char *) ct; (sc = *spnp++)!=0;)
     if (c == sc)
        goto cont;

   if (!c)
      {
      last = NULL;
      return (NULL);
      }
   tok = s - 1;

   for (;;)
      {
      c = *s++;
      spnp = (char *) ct;
      do
         {
         if ((sc = *spnp++)==c)
            {
            if (!c)
               s = NULL;
            else
               s[-1] = '\0';
            last = s;
            return (tok);
            }
         }
      while (sc);
      }
}

#endif
