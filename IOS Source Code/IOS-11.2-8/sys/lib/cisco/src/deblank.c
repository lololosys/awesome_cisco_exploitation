/* $Id: deblank.c,v 3.1.10.1 1996/03/18 20:51:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/deblank.c,v $
 *------------------------------------------------------------------
 * deblank.c -- remove leading whitespace
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: deblank.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:19  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * deblank
 * Remove leading white space
 */

char *
deblank (const char *s)
{
    if (!s)
	return((char *)s);

    while (*s == ' ' || *s == '\t')
	++s;

    return((char *)s);
}
