/* $Id: tohexchar.c,v 3.1.10.1 1996/03/18 20:51:35 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/tohexchar.c,v $
 *------------------------------------------------------------------
 * hexchar.c -- place for a a table of hex digits
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tohexchar.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:39  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

unsigned char const hexchars[] = "0123456789abcdef";

char
tohexchar (unsigned char c)
{
    c &= 0x0f;

    return(hexchars[c]);
}
