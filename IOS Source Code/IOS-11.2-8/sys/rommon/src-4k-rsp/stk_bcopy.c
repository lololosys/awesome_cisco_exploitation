/* $Id: stk_bcopy.c,v 3.2.58.1 1996/03/21 23:32:35 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/stk_bcopy.c,v $
 *------------------------------------------------------------------
 * stk_bcopy.c
 *
 * Mar, 1995, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A bcopy routine that runs fast (on the stack).
 * (Original concept by mbeesley.)
 *------------------------------------------------------------------
 * $Log: stk_bcopy.c,v $
 * Revision 3.2.58.1  1996/03/21  23:32:35  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:27  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:47:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:27  hampton
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
** If the bcopy is greater than BCOPY_THRESHOLD, copy the routine to the 
** stack and execute it on the stack (through kseg0 (cached) if the
** stack is so configured).
*/

#include "monitor.h"
#include "console.h"

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








