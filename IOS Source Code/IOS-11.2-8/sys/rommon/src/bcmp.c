/* $Id: bcmp.c,v 3.2 1995/11/17 18:40:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/bcmp.c,v $
 *------------------------------------------------------------------
 * bcmp.c
 *
 * December 1992, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Byte by byte compare routine.
 *------------------------------------------------------------------
 * $Log: bcmp.c,v $
 * Revision 3.2  1995/11/17  18:40:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "proto.h"

/*
** Byte compare routine.
**
** Nothing has been done to speed enhance this routine.  If
** it is used for very big compares then it should be rewritten
** for speed.
*/
int
bcmp(unsigned char *bp1, unsigned char *bp2, int length)
{
    unsigned char *end;

    end = bp1 + length;
    while(bp1 < end)
	if(*bp1++ != *bp2++) return(1);
    return(0);
}
