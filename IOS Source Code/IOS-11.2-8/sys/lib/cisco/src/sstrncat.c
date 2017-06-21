/* $Id: sstrncat.c,v 3.1.10.1 1996/03/18 20:51:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/sstrncat.c,v $
 *------------------------------------------------------------------
 * sstrncat.c -- cisco 'safe' strncat always nul termates strings
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sstrncat.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:29  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * This is cisco's *safe* version of strncat.  Note that it will
 * always NUL terminate the string if the source is greater than or
 * equal to max.  In these situations, the ANSI strncpy would copy the
 * string up to max and NOT NULL terminate the string.  This means
 * that using strlen to determine max will be broken if you are
 * porting code, or you are using your common understanding of ANSI
 * str libs.  For historical reference, see: /csc/Docs/strncpy.flames
 */
char *
sstrncat (char *s1, char const *s2, unsigned long max)
{
    char *orig_s1 = s1;

    while (*s1)
	s1++;
    while ((max-- > 1) && (*s2)) {
	*s1 = *s2;
	s1++;
	s2++;
    }
    *s1 = '\0';

    return(orig_s1);
}

