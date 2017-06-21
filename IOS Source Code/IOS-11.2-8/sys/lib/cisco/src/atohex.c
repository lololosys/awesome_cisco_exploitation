/* $Id: atohex.c,v 3.1.10.1 1996/03/18 20:50:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/atohex.c,v $
 *------------------------------------------------------------------
 * atohex.c -- Convert two ASCII characters into a hexadecimal byte.
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atohex.c,v $
 * Revision 3.1.10.1  1996/03/18  20:50:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:03  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>
#include <ctype.h>

/*
 * atohex
 * Convert two ASCII characters into a hexadecimal byte.
 */
int
atohex (const char *str)
{
    int result, c, i;
    
    result = 0;
    for (i = 0; i < 2; i++) {
	c = toupper(*str++);
	if (!(((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F')))) {
	    return(-1);
	}
	result = (result << 4) + 
	    ((int) c - (((c >= '0') && (c <= '9')) ? '0' : '7'));
    }
    return(result);
}
