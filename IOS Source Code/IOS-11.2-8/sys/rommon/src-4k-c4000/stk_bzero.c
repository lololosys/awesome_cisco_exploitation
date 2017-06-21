/* $Id: stk_bzero.c,v 3.2 1995/11/17 18:45:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k-c4000/stk_bzero.c,v $
 *------------------------------------------------------------------
 * stk_bzero.c
 *
 * December 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A bzero routine that runs fast (on the stack).
 * (Original concept by mbeesley.)
 *------------------------------------------------------------------
 * $Log: stk_bzero.c,v $
 * Revision 3.2  1995/11/17  18:45:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** A bzero routine that runs fast.
** This was writtten for the Sierra Monitor because executing from
** EPROM is very slow.
** WARNING: kludgy stuff here!
** If the bzero is greater than THRESHOLD, copy the routine to the 
** stack and execute it on the stack (through kseg0 (cached) if the
** stack is so configured).
*/

#define THRESHOLD 0x10000

static void
bzero_body(char *src, int size)
{
    char *end;

    end = src + size;
    while(src < end) *src++ = 0;
}

void
bzero(char *src, int size)
{
    char buffer[(int)&bzero - (int)&bzero_body];
    void (*routine)();

    routine = &bzero_body;  /* init */
    if(size > THRESHOLD) {
	movlword(&bzero_body, buffer, sizeof(buffer));
	cache_flush(0, -1);
	routine = (void(*)())buffer;
    }
    (*routine)(src, size);
}
