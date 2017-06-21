/* $Id: memset.c,v 3.1.10.1 1996/03/18 20:49:24 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/memset.c,v $
 *------------------------------------------------------------------
 * memset.c - set the value of a block of memory
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memset.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:39:46  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        memset---set an area of memory

ANSI_SYNOPSIS
        #include <string.h>
        void *memset(const void *dst, int c, size_t length);

DESCRIPTION
        This function converts the argument c into an unsigned char
        and fills the first length characters of the array pointed to
        by dst to the value.

RETURNS
        memset returns the value of m.

PORTABILITY
        memset is ANSI C.

        memset requires no supporting OS subroutines.
*/

#include <string.h>

/*
 * memset - set the value of a block of memory
 */
void *
memset (void *dummy, int c, size_t count)
{
    unsigned char *address = dummy;
    long lcount;
    unsigned long *lp;
    unsigned long value;

    c &= 0xff;
    value = c | (c << 8) | (c << 16) | (c << 24);

    /* First, do a few bytes so that we're word aligned. */
    for (; count && ((long)address & 0x3); count--)
        *address++ = c;

    /* Now do the longs. */
    lp = (unsigned long *) address;
    lcount = count >> 2;
    for (; lcount > 16; lcount -= 16) {
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
        *lp++ = value;
    }
    while (lcount) {
        *lp++ = value;
        lcount--;
    }
    address = (unsigned char *) lp;
    count &= 0x3;

    /* Now do what's left. */
    for (; count; address++, count--)
        *address = c;

    return dummy;
}
