/* $Id: uncompress.c,v 3.2.58.1 1996/06/01 01:10:39 clev Exp $
 * $Source: /release/112/cvs/Xboot/uncompress.c,v $
 *------------------------------------------------------------------
 * uncompress.c -- data uncompression algorithm
 *
 * August 1987, Greg Satz
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: uncompress.c,v $
 * Revision 3.2.58.1  1996/06/01  01:10:39  clev
 * CSCdi55171:  32m on 4000m breaks uncompress in the monitor
 * Branch: California_branch
 *
 * Add code to handle the discontiguous memory on a 32m 4000m.
 *
 * Revision 3.2  1995/11/17  07:42:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:05:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "stdio.h"
#include "prom2.h"
#include "reason.h"

#ifndef TEST
#include "extern.h"
#endif

#ifdef TEST
#include <stdio.h>
extern int memsize;
#endif

#define GETCODE(inp, data) 						\
    if ((data)->offset >= (data)->size || (data)->free > (data)->maxcode \
	|| (data)->clear) { 						\
	/*								\
	 * If the next entry will be too big for the current code 	\
	 * size, then we must increase the size.  This implies reading 	\
	 * a new buffer full, too. 					\
	 */ 								\
	if ((data)->free > (data)->maxcode) { 				\
	    (data)->nbits++;						\
	    if ((data)->nbits == (data)->maxbits)			\
		(data)->maxcode = (data)->maxmaxcode;			\
	    else							\
		(data)->maxcode = MAXCODE((data)->nbits);		\
	}								\
	if ((data)->clear) {						\
	    (data)->maxcode = MAXCODE((data)->nbits = INITBITS);	\
	    (data)->clear = FALSE;					\
	}								\
	(data)->size = loadbuf(bufu.bufh.buf2, inp, (data));			\
	if ((data)->size <= 0) {					\
	    code = -1;							\
	    goto done;							\
	}								\
	(data)->offset = 0;						\
	/* Round size down to integral number of codes */		\
	(data)->size = ((data)->size << 3) - ((data)->nbits - 1);	\
    }									\
    r_off = (data)->offset;						\
    bits = (data)->nbits;						\
    /*									\
     * Get to the first byte.						\
     */									\
    bp += (r_off >> 3);							\
    r_off &= 7;								\
    /* Get first part (low order bits) */				\
    code = (*bp++ >> r_off);						\
    bits -= (8 - r_off);						\
    r_off = 8 - r_off;		/* now, offset into code word */	\
    /* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */	\
    if (bits >= 8) {							\
	code |= *bp++ << r_off;						\
	r_off += 8;							\
	bits -= 8;							\
    }									\
    /* high order bits. */						\
    code |= (*bp & rmask[bits]) << r_off;				\
    (data)->offset += (data)->nbits;					\
  done:

/*
 * the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */
#define FIRST	257	/* first free entry */
#define CLEAR	256	/* table clear output code */

#define	MAXBITS		16
#define	INITBITS	9		/* initial number of bits/code */
#define	HASHSIZE	69001		/* 95% occupancy */
#define	MAXCODE(nbits)	((1 << (nbits)) - 1)

/*
 * We need this much space to run
 */

#define DATASIZE 220000

/*
 * Magic header
 */

#define	MAGIC1	0x1F			/* 0x1F */
#define	MAGIC2	0x9D			/* 0x9D */

#define	BITMASK	0x1F			/* number of bits */
#define	BLOCKMASK 0x80			/* block mode */

struct compdata {
    short clear;
    int offset;
    int size;
    long free;
    long maxcode;
    long maxmaxcode;
    int nbits;
    int maxbits;
    long count;
};

struct compinp {
    long index;
    u_char *p;
    struct cload *data;
};

/*
 * Global variables used by all of uncompress
 */

u_short *codetab;
u_char *hashtab;
u_char *destack;

u_char rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

long getcode();
boolean ram_image;		/* TRUE if decompressing from RAM */

/*
 * uncompress
 * Uncompress data given compress structure.
 */

uncompress(startaddr, endaddr)
    u_char *startaddr, *endaddr;
{
    register int i;
    struct cdata *cdata;
    struct cload *cload;
    long size, realsize;			/* total length of image */
    long space_needed, space_available;
    long topram, workarea;

    topram = ADRSPC_RAM + memsize;

#if defined(XX)
    /*
    ** The 4000M can have 32mb of memory but it is non-contiguous.
    ** Put the workarea in the second chunk.
    */
    if(topram > ADRSPC_PROM) {
	workarea = ADRSPC_SHAREDMEM - (DATASIZE + sizeof(struct cload));
	topram = ADRSPC_PROM;
    } else {
	workarea = topram - (DATASIZE + sizeof(struct cload));
    }
#else
    workarea = topram - (DATASIZE + sizeof(struct cload));
#endif
 
    cdata = (struct cdata *)startaddr;
    if (cdata->magic != PROM2_MAGIC2) {
	puts("Magic number mismatch in compressed data\n");
	return (FALSE);
    }
    size = 0;
    cload = (struct cload *)(cdata + 1);
    for (i = 0; i < cdata->number; i++) {
	size += cload->len;
	cload++;
    }
    cload = (struct cload *)(cdata + 1);  /* set back to first */
    realsize = size + sizeof(struct cdata) +
	cdata->number * sizeof(struct cload);
    /*
     * We assume that the incoming image won't expand more then 50%
     * of its input size.
     */

    if ((startaddr >= (u_char *)ADRSPC_RAM) && 
	(startaddr <= (u_char *)topram)) {
	ram_image = TRUE;	/* Decompressing from RAM */
	space_needed = realsize + (2 * size) + DATASIZE;
    } else {
	ram_image = FALSE;	/* Decompressing from ROM */
	space_needed = (2 * size) + DATASIZE;
    }

#if defined(XX)
#define SIXTEENMEG 0x1000000
    /*
    ** The 4000M can have 32mb of memory but it is non-contiguous.
    */
    if(memsize > SIXTEENMEG) {
	space_available = SIXTEENMEG - ((long)endaddr - ADRSPC_RAM);
    } else {
	space_available = memsize - ((long)endaddr - ADRSPC_RAM);
    }
#else
    space_available = memsize - ((long)endaddr - ADRSPC_RAM);
#endif

    if (!config.diag)
	printf("Uncompress needs %d bytes, leaving %d bytes free\n",
	       space_needed, (space_available - space_needed));
    flush();

    /*
     * If the starting address is within real memory we may have
     * to copy things around to get it all to fit.
     */
    if (ram_image) {
	/*
	** Make sure the compressed image placed in memory with enough room
	** at the end for the uncompress workarea (DATASIZE).  Also, make
	** sure the compressed image is as close to the top of RAM as
	** possible to allow for the uncompression of the largest possible
	** images.
	*/
	if (((long)startaddr + realsize >= workarea) ||
	    (long)startaddr + realsize + 64 < workarea) {
	    /*
	     * This bcopy assumes that we only have one load image
	     */
	    if (cdata->number != 1) {
		puts("Can't support relocating of multiple load images\n");
		return(FALSE);
	    }

	    /* copy entire image up under the workarea */
	    cdata = (struct cdata *)((workarea - (realsize + 20)) & ~3);
	    bcopy(startaddr, cdata, realsize);
	    cload = (struct cload *)(cdata + 1);
	    cload->addr = (u_long)(cload + 1);
	    /* now copy cload struct up to workarea */
	    ((struct cload *)workarea)->addr = cload->addr;
	    ((struct cload *)workarea)->len = cload->len;
	    cload = (struct cload *)workarea;
	}
    }
    /*
     * Set up memory for the uncompress
     */
    codetab = (u_short *)(workarea + sizeof(struct cload));
    hashtab = (u_char *)codetab + HASHSIZE * sizeof(u_short);
    destack = (u_char *)hashtab + (1 << MAXBITS);
    return decompress(cload, endaddr, size);
}

/*
 * decompress
 * Perform actual decompression
 */

decompress(cload, endaddr, count)
    struct cload *cload;
    u_char *endaddr;
    long count;
{
    register u_char *endp, *stackp;
    struct compdata data;
    struct compinp inp;
    register long code, oldcode, incode;
    register int r_off, bits;
    register u_char *bp = (u_char *)bufu.bufh.buf2;
    register char finchar;
    char block;
    long chkpt;

    endp = endaddr;
    inp.index = 0;
    inp.data = cload;
    inp.p = (u_char *)inp.data->addr;
    if (inp.p[0] != MAGIC1 || inp.p[1] != MAGIC2) {
	puts("Input is not in compressed format\n");
	return(FALSE);
    }
    data.maxbits = inp.p[2];
    inp.index += 3;
    block = data.maxbits & BLOCKMASK ? TRUE : FALSE;
    data.count = count - 3;			/* for header */
    data.maxbits &= BITMASK;
    data.maxmaxcode = 1 << data.maxbits;
    if (data.maxbits > MAXBITS) {
	printf("Can't uncompress %d bit substring codes, only %d bits supported\n",
	       data.maxbits, MAXBITS);
	return (FALSE);
    }
    data.maxcode = MAXCODE(data.nbits = INITBITS);
    for (code = 255; code >= 0; code--) {
	codetab[code] = 0;
	hashtab[code] = (u_char)code;
    }
    data.free = block ? FIRST : 256;
    data.clear = FALSE;
    data.offset = 0;
    data.size = 0;
    finchar = oldcode = getcode(&inp, &data);
    if (oldcode == -1)			/* EOF */
	return (FALSE);
    *endp++ = finchar;
    stackp = destack;
    for (;;) {
	bp = (u_char *)bufu.bufh.buf2;
	GETCODE(&inp, &data);
	if (code == -1)
	    break;
	if (code == CLEAR && block) {
	    for (code = 255; code >= 0; code--)
		codetab[code] = 0;
	    data.clear = TRUE;
	    data.free = FIRST - 1;
	    if ((code = getcode(&inp, &data)) == -1) {
		printf("\nPremature EOF when uncompressing\n");
		break;			/* premature EOF */
	    }
	}
	incode = code;
	/*
	 * Special case for KwKwK string.
	 */
	if (code >= data.free) {
	    *stackp++ = finchar;
	    code = oldcode;
	}
	/*
	 * Generate output characters in reverse order
	 */
	while (code >= 256) {
	    *stackp++ = hashtab[code];
	    code = codetab[code];
	}
	*stackp++ = finchar = hashtab[code];

	/*
	** Check to see if the destination uncompressed data will
	** overlap onto the source (compressed) data, or in the case of
	** uncompressing from ROM, overlap the decompression workarea.
	*/
	if(ram_image) chkpt = (long)(&inp.p[inp.index]);
	else chkpt = (long)codetab - sizeof(struct cload);

	if(((long)endp + ((long)stackp - (long)destack)) >= chkpt) {
	    printf("\nUncompress: decompressed data overlaps source, \
image is too large\n");
printf("dest = %#x,  chkpt = %#x\n",((long)endp + ((long)stackp - (long)destack)),chkpt);
	    return(FALSE);
	}

	/*
	 * Now put them in forward order
	 */
	do
	    *endp++ = *--stackp;
	while (stackp > destack);
	/*
	 * Generate the new entry
	 */
	if ((code = data.free) < data.maxmaxcode) {
	    codetab[code] = (u_short)oldcode;
	    hashtab[code] = finchar;
	    data.free = code + 1;
	}
	/*
	 * Remember previous code.
	 */
	oldcode = incode;
    }
    putchar('\n');
    return(endp - endaddr);
}

/*
 * getcode
 * Read a code from input compressed data
 * Return code or -1 for EOF.
 */

long getcode(inp, data)
    struct compinp *inp;
    register struct compdata *data;
{
    register long code;
    register int r_off, bits;
    register u_char *bp = (u_char *)bufu.bufh.buf2;

    GETCODE(inp, data);
    return(code);
}

loadbuf(dest, inp, data)
    register u_char *dest;
    struct compinp *inp;
    register struct compdata *data;
{
    register int i, index;
    int cnt;

    if (data->count <= 0)
	return(data->count);
    if (data->count < data->nbits)
	i = data->count;
    else
	i = data->nbits;
    cnt = i;
    data->count -= i;

    index = inp->index;
    if (index + i > inp->data->len)
	while (i--) {
	    if (index >= inp->data->len) {
		index = 0;
		inp->data++;
		inp->p = (u_char *)inp->data->addr;
	    }
	    *dest++ = inp->p[index++];
	}
    else
	while (i--) {
	    *dest++ = inp->p[index++];
	    if ((index & 0xFFF) == 0)
		putchar('#');
	}
    inp->index = index;
    return(cnt);
}
