/* $Id: stk_fchksum.c,v 3.2 1995/11/17 18:47:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k-rsp/stk_fchksum.c,v $
 *------------------------------------------------------------------
 * stk_fchksum.c
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A file checksum routine that runs fast (on the stack).
 * (Original concept by mbeesley.)
 *------------------------------------------------------------------
 * $Log: stk_fchksum.c,v $
 * Revision 3.2  1995/11/17  18:47:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:30  hampton
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
** Use the 32bit access to load data. The RSP only supports 32bit
** access to flash. This will work for any device support 32 bit access.
*/

#define THRESHOLD 0x10000
#define WORD_LEN  4
#define WORD_MASK 0xfffffffc

int
fchksum_body(char *start, int size)
{
    unsigned short *ptr = (unsigned short *)start;
    unsigned short *end = (unsigned short *)(start + size);
    unsigned long chksum = 0;
   
    unsigned long d_offset;
    unsigned long *d_addr;
    ussigned long d_buf;


    while(ptr < end) {   /* calculate the checksum */
	d_offset = (unsigned long)ptr % WORD_LEN;
	d_addr = (unsigned long *)((ussigned long)ptr & WORD_MASK);
	d_buf = *d_addr;
	chksum += *(unsigned short *)((unsigned long)&d_buf + d_offset);
	ptr++;

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
