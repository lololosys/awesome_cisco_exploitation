/* $Id: cmpid.c,v 3.1.10.1 1996/03/18 20:51:01 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/cmpid.c,v $
 *------------------------------------------------------------------
 * cmpid.c -- compare two strings for len bytes
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cmpid.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:16  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * cmpid
 * Compare two strings.
 * 	If *ptr1 <  *ptr2, return < 0 
 *	If *ptr1 >  *ptr2, return > 0
 *	If *ptr1 == *ptr2, return = 0
 */

/*
 * The comparison does NOT stop when a '\0' byte is detected.
 */
int
cmpid (const unsigned char *ptr1, const unsigned char *ptr2, long len)
{
    unsigned char chr1, chr2;
    long i;
    
    for (i = 0; i < len; i++) {
	chr1 = *ptr1++;
	chr2 = *ptr2++;
	if (chr1 < chr2)
	    return(-1);
	if (chr1 > chr2)
	    return(1);
    }
    return(0);
}
