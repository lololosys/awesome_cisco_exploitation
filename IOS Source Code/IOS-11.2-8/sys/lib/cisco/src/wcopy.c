/* $Id: wcopy.c,v 3.1.10.1 1996/03/18 20:51:39 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/wcopy.c,v $
 *------------------------------------------------------------------
 * wcopy.c -- word copy routine
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: wcopy.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:42  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * wcopy
 * Copy a number of words.
 */
void
wcopy (unsigned short *src, unsigned short *dst, long count) 
{
    if (count <= 0)
	return;
    while (count--) {
	*dst++ = *src++;
    }
}
