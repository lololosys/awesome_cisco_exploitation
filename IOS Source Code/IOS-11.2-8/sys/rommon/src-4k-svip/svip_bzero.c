/* $Id: svip_bzero.c,v 1.1.6.1 1996/07/08 22:43:00 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_bzero.c,v $
 *------------------------------------------------------------------
 * svip_bzero.c
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_bzero.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:00  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:22:55  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "monitor.h"

/*
** A bzero routine that runs fast.
** This was writtten for the Sierra Monitor because executing from
** EPROM is very slow.
** WARNING: kludgy stuff here!
** If the bzero is greater than THRESHOLD, copy the routine to the 
** stack and execute it on the stack (through kseg0 (cached) if the
** stack is so configured).
*/

#define BZERO_THRESHOLD 0x10000

static void
bzero_body(long *src, int size)
{
    long *end;

    end = src + size;
    while(src < end) *src++ = 0;
}

void
bzero(char *src, int size)
{
    int wordsize;
    char buffer[(int)&bzero - (int)&bzero_body];
    void (*routine)();

    routine = &bzero_body;  /* init */

    if(size > BZERO_THRESHOLD) {
	movlword(&bzero_body, buffer, sizeof(buffer));
	cache_flush(0, -1);
	routine = (void(*)())buffer;
    }

    wordsize = size >> 2;
    (*routine)(src, wordsize);
}


#define BCOPY_THRESHOLD 0x10000

#define WORD_LEN               4
#define WORD_MASK              0xfffffffc
#define BYTE_COUNT             1028 * 100

static void 
bcopy_down_body(src, dest, numbytes)
char *src;
char *dest;
unsigned int numbytes;
{

    int ix;
    unsigned long d_offset;
    unsigned long *d_addr;
    unsigned long d_buf;
    char *cptr;
    unsigned long len;

    d_addr = (unsigned long *)((unsigned long)src & WORD_MASK);
    d_offset = (unsigned long)src % WORD_LEN;
    len = (((WORD_LEN - d_offset) < numbytes)?(WORD_LEN - d_offset) : numbytes);

    d_buf = *d_addr;                      /* read the first 4 byte from flash */

    for (ix=0; ix < len; ix++) {
	cptr = (char *)((unsigned long)&d_buf + d_offset + ix);
	*dest = *cptr;                    /* move data into destination */
	dest++;
	src++;
    } /* for */

    if ((numbytes - len) == 0) return;    /* no more bytes to move */

     for (ix=0; ix < numbytes - len; ix++) {

	d_offset = (unsigned long)src % WORD_LEN;

	if (d_offset == 0) {
	    d_addr = (unsigned long *)src;
	    d_buf = *d_addr;            /* flash 4 byte read */
	}

	cptr = (char *)((unsigned long)&d_buf + d_offset);
	*dest = *cptr;

	dest++;
	src++;

    } /* for */

    return;

} /* bcopy_down_body */



static void
bcopy_up_body(src, dest, numbytes)
char *src;
char *dest;
unsigned int numbytes;
{
    int ix;
    unsigned long d_offset;
    unsigned long *d_addr;
    unsigned long d_buf;
    char *cptr;
    unsigned long len;

    src += (numbytes - 1);     /* move the pointer to the end */
    dest+= (numbytes - 1);

    d_addr = (unsigned long *)((unsigned long)src & WORD_MASK);
    d_offset = (unsigned long)src % WORD_LEN;
    len = ((d_offset + 1) < numbytes)? (d_offset + 1) : numbytes;

    d_buf = *d_addr;                      /* read the first 4 byte from flash */

    for (ix=0; ix < len; ix++) {
	cptr = (char *)((unsigned long)&d_buf + d_offset - ix);
	*dest = *cptr;                    /* move data into destination */
	dest--;
	src--;
    } /* for */

    if ((numbytes - len) == 0) return;    /* no more bytes to move */

     for (ix=0; ix < numbytes - len; ix++) {

	d_offset = (unsigned long)src % WORD_LEN;

	if (d_offset == (WORD_LEN - 1)) {
	    d_addr = (unsigned long *)((unsigned long)src & WORD_MASK);
	    d_buf = *d_addr;            /* flash 4 byte read */
	}

	cptr = (char *)((unsigned long)&d_buf + d_offset);
	*dest = *cptr;

	dest--;
	src--;

    } /* for */

    return;

} /* body_up_body */



void
bcopy(char *src, char *dest, int size)
{
    char buffer[((int)&bcopy - (int)&bcopy_up_body) + 32];
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
