/* $Id: calloc.c,v 3.1.10.1 1996/03/18 20:49:06 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/calloc.c,v $
 *------------------------------------------------------------------
 * calloc.c -- alocate and zero memory
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: calloc.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:39  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
FUNCTION
        calloc---allocate space for arrays

ANSI_SYNOPSIS
        #include <stdlib.h>
        void *calloc(size_t nmemb, size_t size);

DESCRIPTION
        Use calloc to request a block of memory sufficient to hold an
        array of nmemb elements, each of which has size bytes.

        The memory allocated by calloc comes out of the same memory
        pool used by malloc, but the memory block is initialized to
        all zero bytes.  (To avoid the overhead of initializing the
        space, use malloc instead.)

RETURNS
        If successful, a pointer to the newly allocated space.

        If unsuccessful, NULL.

PORTABILITY
        calloc is ANSI.
*/

#include <string.h>
#include <stdlib.h>

void *
calloc (size_t nmemb, size_t size)
{
    void *cp;

    nmemb *= size;
    cp = malloc(nmemb);

    if (cp == 0)
        return 0;
#ifndef CISCO
    /*
     * The CISCO version of malloc already initializes the block with
     * zeros.
     */
    memset(cp, '\0', nmemb);
#endif /* CISCO */
    return cp;
}
