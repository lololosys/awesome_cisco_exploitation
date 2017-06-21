/* $Id: stk_fchksum.c,v 3.2 1995/11/17 18:45:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k-c4000/stk_fchksum.c,v $
 *------------------------------------------------------------------
 * stk_fchksum.c
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A file checksum routine that runs fast (on the stack).
 * (Original concept by mbeesley.)
 *------------------------------------------------------------------
 * $Log: stk_fchksum.c,v $
 * Revision 3.2  1995/11/17  18:45:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** A file checksum routine that runs fast.
** This was writtten for the Sierra Monitor because executing from
** EPROM is very slow.
** WARNING: kludgy stuff here!
** If the size is greater than THRESHOLD, copy the routine to the 
** stack and execute it on the stack (through kseg0 (cached) if the
** stack is so configured).
*/

#define THRESHOLD 0x10000

int
fchksum_body(char *start, int size)
{
    unsigned short *ptr = (unsigned short *)start;
    unsigned short *end = (unsigned short *)(start + size);
    unsigned long chksum = 0;

    while(ptr < end) {   /* calculate the checksum */
	chksum += *ptr++;
	if(chksum & ~0xffff)
	    chksum = ++chksum & 0xFFFF;    /* end-around carry */   
    }
    return(chksum & 0xFFFF);
}

int
file_chksum(char *src, int size)
{
    char buffer[(int)&file_chksum - (int)&fchksum_body];
    void (*routine)();

    routine = &fchksum_body;  /* init */
    if(size > THRESHOLD) {
	movlword(&fchksum_body, buffer, sizeof(buffer));
	cache_flush(0, -1);
	routine = (void(*)())buffer;
    }
    return((*routine)(src, size));
}
