/* $Id: sstrncpy.c,v 3.1.10.2 1996/09/11 07:04:13 mdb Exp $
 * $Source: /release/112/cvs/Xsys/lib/cisco/src/sstrncpy.c,v $
 *------------------------------------------------------------------
 * sstrncpy.c -- cisco 'safe' strncpy always nul termates strings
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sstrncpy.c,v $
 * Revision 3.1.10.2  1996/09/11  07:04:13  mdb
 * CSCdi68717:  typo in cut-and-paste comment
 * Branch: California_branch
 *
 * Revision 3.1.10.1  1996/03/18  20:51:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:31  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * This is cisco's *safe* version of strncpy.  Note that it will always NULL
 * terminate the string if the source is greater than or equal to max.  In 
 * these situations, ANSI strncpy would copy the string up to the max and NOT 
 * terminate the string.   For historical reference, see: 
 * /csc/Docs/strncpy.flames
 */
char *
sstrncpy (char *dst, char const *src, unsigned long max)
{
    char *orig_dst = dst;

    while ((max-- > 1) && (*src)) {
	*dst = *src;
	dst++;
	src++;
    }
    *dst = '\0';

    return(orig_dst);
}
