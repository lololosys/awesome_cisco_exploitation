/* $Id: sa_uncompress.c,v 3.2.58.1 1996/03/21 23:27:51 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/sa_uncompress.c,v $
 *------------------------------------------------------------------
 * sa_uncompress.c
 *
 * April 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Standalone data uncompression algorithm.  Derived from
 * uncompress.c by Greg Satz (August 1987).
 *------------------------------------------------------------------
 * $Log: sa_uncompress.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:51  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:04  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "mon_boot.h"

#define TRUE 1
#define FALSE 0
static int decompress(u_char *startaddr, u_char *endaddr, long count);
u_char buffer[256];


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
	(data)->size = loadbuf(buffer, inp, (data));			\
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
 * The assumption is made that the image is in main memory, placed
 * at the top with room for UNCOMPRESS_DATASIZE following it.
 * All information is passed to and from this module in the plb
 * structure.
 */
int
main(int code, struct plb *plb, struct pib *pib)
{
    long workarea;
    long size;
    u_char *startaddr, *destaddr;
    extern unsigned char *end;

    if(!plb || !plb->plb_addr || !plb->plb_size) {
	printf("\nuncompress: insufficient information to begin ... aborting\n");
	return(0);
    }
    startaddr = plb->plb_addr;
    size = plb->plb_size;
    destaddr = (u_char *)(((long)&end + 15) & ~3);
#ifdef DEBUG
    printf("\nEntering uncompress: startaddr: %#x, destaddr: %#x, size: %#x\n",
	   startaddr, destaddr, size);
#endif

    workarea = plb->plb_ipaddr;  /* monitor uses this field for workarea addr */

    /*
     * Set up memory for the uncompress
     */
    codetab = (u_short *)workarea;
    hashtab = (u_char *)codetab + HASHSIZE * sizeof(u_short);
    destack = (u_char *)hashtab + (1 << MAXBITS);
    ram_image = TRUE;

    printf("\nUncompressing file:  ");
    size = decompress(startaddr, destaddr, size);
    plb->plb_addr = destaddr;
    plb->plb_size = size;
#ifdef DEBUG
    printf("Exiting uncompress: destaddr: %#x, size: %#x\n",
	   destaddr, size);
#endif
    return(size==0 ? 0 : 1);
}

/*
 * decompress
 * Perform actual decompression
 */
static int
decompress(u_char *startaddr, u_char *endaddr, long count)
{
    register u_char *endp, *stackp;
    struct compdata data;
    struct compinp inp;
    register long code, oldcode, incode;
    register int r_off, bits;
    register u_char *bp = (u_char *)buffer;
    register char finchar;
    char block;
    long chkpt;

#ifdef DEBUG
    printf("Entering decompress: startaddr: %#x, destaddr: %#x, size: %#x\n",
	   startaddr, endaddr, count);
#endif

    endp = endaddr;
    inp.index = 0;
    inp.p = startaddr;
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
	bp = (u_char *)buffer;
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
	else chkpt = (long)codetab;

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
    register u_char *bp = (u_char *)buffer;

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
    /*
    ** Note: removed code here to handle multiple
    ** file segments (see original in the boot directory).
    */
    while (i--) {
	*dest++ = inp->p[index++];
	if ((index & 0xFFF) == 0)
	    putchar('#');
    }
    inp->index = index;
    return(cnt);
}
