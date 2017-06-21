/* $Id: term_char.c,v 3.1.10.1 1996/03/18 20:51:33 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/term_char.c,v $
 *------------------------------------------------------------------
 * term_char.c - similar to is isspace(), wo/ \v \f plus \0
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: term_char.c,v $
 * Revision 3.1.10.1  1996/03/18  20:51:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:47:38  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * term_char
 * Return TRUE (1) if character is a token terminator, FALSE otherwise
 */

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

int
term_char (char c)
{
    if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == 0))
	return(TRUE);
    return(FALSE);
}
