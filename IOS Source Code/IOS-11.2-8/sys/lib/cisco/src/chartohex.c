/* $Id: chartohex.c,v 3.1.6.1 1996/03/18 20:51:00 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/src/chartohex.c,v $
 *------------------------------------------------------------------
 * chartohex -- convert char to a hex nibble
 *
 * March 1996, David Hampton
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: chartohex.c,v $
 * Revision 3.1.6.1  1996/03/18  20:51:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/09  05:28:55  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/03/08  22:07:23  hampton
 * Add placeholders for Dead King branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * chartohex
 * Convert char to hex nibble
 */

int
chartohex (unsigned char ch)
{
    if ((ch >= 'A') && (ch <= 'F')) return (ch-'A'+10);
    if ((ch >= 'a') && (ch <= 'f')) return (ch-'a'+10);
    if ((ch >= '0') && (ch <= '9')) return (ch-'0');
    return (0);
}
