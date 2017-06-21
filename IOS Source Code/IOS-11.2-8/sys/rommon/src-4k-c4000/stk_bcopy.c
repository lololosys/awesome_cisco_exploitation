/* $Id: stk_bcopy.c,v 3.2.58.1 1996/03/21 23:31:07 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/stk_bcopy.c,v $
 *------------------------------------------------------------------
 * stk_bcopy.c
 *
 * December 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A bcopy routine that runs fast (on the stack).
 * (Original concept by mbeesley.)
 *------------------------------------------------------------------
 * $Log: stk_bcopy.c,v $
 * Revision 3.2.58.1  1996/03/21  23:31:07  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:35  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:45:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** A bcopy routine that runs fast.
** This was written for the Sierra Monitor because executing from
** EPROM is very slow.
** WARNING: kludgy stuff here!
** If the bcopy is greater than THRESHOLD, copy the routine to the 
** stack and execute it on the stack (through kseg0 (cached) if the
** stack is so configured).
*/

#include "monitor.h"

#define BCOPY_THRESHOLD 0x10000

static void
bcopy_down_body(char *src, char *dest, int size)
{
    char *end;

    end = src + size;
    while(src < end) *dest++ = *src++;
}

static void
bcopy_up_body(char *src, char *dest, int size)
{
    char *end;

    end = src;
    src += (size - 1);  /* start from the other end */
    dest += (size - 1);
    while(src >= end) *dest-- = *src--;
}

void
bcopy(char *src, char *dest, int size)
{
    char buffer[((int)&bcopy - (int)&bcopy_up_body) + 16];
    void (*routine)();

    if(PHY_ADDR((long)src) > PHY_ADDR((long)dest))
	routine = &bcopy_down_body;
    else
	routine = &bcopy_up_body;

    if(size > BCOPY_THRESHOLD) {
	movlword((char *)routine, buffer, sizeof(buffer));
	cache_flush(0, -1);
	routine = (void(*)())buffer;
    }
    (*routine)(src, dest, size);
}
