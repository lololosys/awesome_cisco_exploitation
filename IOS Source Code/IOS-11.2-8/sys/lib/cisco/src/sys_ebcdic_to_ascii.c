/* $Id: sys_ebcdic_to_ascii.c,v 3.1.10.2 1996/09/10 03:08:20 snyder Exp $
 * $Source: /release/112/cvs/Xsys/lib/cisco/src/sys_ebcdic_to_ascii.c,v $
 *------------------------------------------------------------------
 * sys_ebcdic_to_ascii.c -- convert from EBCDIC to ASCII
 *
 * February 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sys_ebcdic_to_ascii.c,v $
 * Revision 3.1.10.2  1996/09/10  03:08:20  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.1.10.1  1996/03/18  20:51:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:36  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * e_to_a_table
 * EBCDIC to Ascii conversion table.
 */

static const char e_to_a_table[] = 
    "................"  /* 00 to 0f */
    "................"  /* 10 to 1f */
    "................"  /* 20 to 2f */
    "................"  /* 30 to 3f */
    " ...........<(+ "  /* 40 to 4f */
    "&.........!$*);^"  /* 50 to 5f */
    "-/........|,%_>?"  /* 60 to 6f */
    ".........`:#@'=\"" /* 70 to 7f */
    ".abcdefghi......"  /* 80 to 8f */
    ".jklmnopqr......"  /* 90 to 9f */
    ".~stuvwxyz......"  /* a0 to af */
    "................"  /* b0 to bf */
    "{ABCDEFGHI......"  /* c0 to cf */
    "}JKLMNOPQR......"  /* d0 to df */
   "\\.STUVWXYZ......"  /* e0 to ef */
    "0123456789......"; /* f0 to ff */

/*
 * sys_ebcdic_to_ascii
 * Convert from EBCDIC to ASCII
 */

void
sys_ebcdic_to_ascii (unsigned char *string, long length)
{
    long i;
    
    for (i = 0; i < length; i++)
	string[i] = e_to_a_table[string[i]];
}
