/* $Id: null.c,v 3.1.10.1 1996/03/18 20:51:17 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/null.c,v $
 *------------------------------------------------------------------
 * null.c -- check for NULL or empty string
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: null.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:26  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * null
 * check for null string, 0 = non-NULL, 1 = NULL or empty string
 */

int
null (const char *s)
{
    char c;

    if (s)
	while ((c = *s++) != '\0')
	    if ((c != ' ') && (c != '\t'))
		return(0);
    return(1);
}
