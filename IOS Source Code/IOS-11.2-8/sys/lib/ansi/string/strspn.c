/* $Id: strspn.c,v 3.1.10.1 1996/03/18 20:49:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strspn.c,v $
 *------------------------------------------------------------------
 * strspn -- find initial match in string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strspn.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:13  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        strspn---find initial match

ANSI_SYNOPSIS
        #include <string.h>
        size_t strspn(const char *s1, const char *s2);

DESCRIPTION
        This function computes the length of the initial segment of
        the string pointed to by s1 which consists entirely of
        characters from the string pointed to by s2 (excluding the
        terminating null character).

RETURNS
        strspn returns the length of the segment found.

PORTABILITY
        strspn is ANSI C.

        strspn requires no supporting OS subroutines.
*/

#include <string.h>

size_t
strspn (const char *s1, const char *s2)
{
    const char *s = s1;
    const char *c;

    while (*s1) {
        for (c = s2; *c; c++) {
            if (*s1 == *c)
                break;
        }
        if (*c == '\0')
            break;
        s1++;
    }

    return s1 - s;
}

#if 0

/******************************************************************************
 * Name: strspn
 *
 * Description: ANSI strspn function
 *
 * Input: (see ANSI C Reference Manual)
 *
 * Return Value: (see ANSI C Reference Manual)
 *
 *******************************************************************************/

size_t strspn (const char *s, const char *set)
{
    uint span_length;
    const char *set_ptr;
    int exit_flag;

    span_length = 0;
    exit_flag = FALSE;
    if ((s != NULL) && (set != NULL)) {
        while ((*s != '\0') && (exit_flag == FALSE)) {
            set_ptr = set;
            while (TRUE) {
                if (*set_ptr == '\0') {
                    exit_flag = TRUE;
                    break;
                }
                if (*set_ptr == *s) {
                    span_length++;
                    break;
                }
                set_ptr++;
            }
            s++;
        }
    }
    return(span_length);
}

#endif
