/* $Id: remcompress.c,v 3.2.62.1 1996/03/18 21:31:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/remcompress.c,v $
 *------------------------------------------------------------------
 * remcompress.c - Simple data compression for remote GDB debugging
 *
 * 06-March-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: remcompress.c,v $
 * Revision 3.2.62.1  1996/03/18  21:31:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:10:21  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/03/07  10:19:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  18:51:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:58:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "gdb.h"

void gdb_compress (char *src, char *dest)
{
    char previous = 0;
    int repeat = 0;

    do {
	if ((*src == previous) && (repeat != 255)) {
	    repeat++;
	} else {
	    if (repeat > 3) {
		dest = dest-repeat;
		*dest++ = '*';
		*dest++ = tohexchar(repeat >> 4);
		*dest++ = tohexchar(repeat);
	    }
	    repeat = 0;
	}
	*dest++ = *src;
	previous = *src;
    } while (*src++);
}

void gdb_expand (char *src, char *dest)
{
    int i;
    int repeat;

    do {
	if (*src == '*') {
	    repeat = (chartohex(src[1]) << 4) + chartohex(src[2]);
	    for (i = 0; i < repeat; i++) {
		*dest++ = *(src-1);
	    }
	    src += 2;
	} else {
	    *dest++ = *src;
	}
    } while (*src++);
}

