/* $Id: string_getnext.c,v 3.1.10.1 1996/03/18 20:51:27 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/string_getnext.c,v $
 *------------------------------------------------------------------
 * string_getnext.c -- get a buffer to write a short string into
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: string_getnext.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:34  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * string_getnext
 * Get a buffer to write a short string into. This routine should only
 * be used by display code that isn't going to keep the string very long.
 */

#define NSTRINGBUFS 10

static char string_buffer[NSTRINGBUFS][STRING_BUF_CHARS];
static int string_counter = 0;

char *
string_getnext (void)
{
    string_counter = (++string_counter) % NSTRINGBUFS;
    return(string_buffer[string_counter]);
}
