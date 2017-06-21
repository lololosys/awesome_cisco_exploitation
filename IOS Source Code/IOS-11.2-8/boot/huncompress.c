/* $Id: huncompress.c,v 3.2 1995/11/17 07:41:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/huncompress.c,v $
 *------------------------------------------------------------------
 * huncompress.c
 *
 * Copyright (c) 1990-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 * uncomress a system image that was compressed using tge "lzhuf" program
 * code for this module was taken from the lzhuf.c program, which is
 * copyrighted in Japan, sort of, I think.
 * lzhuf has the following history:
 *	Based on Japanese version 29-NOV-1988
 * 	LZSS coded by Haruhiko OKUMURA
 *	Adaptive Huffman Coding coded by Haruyasu YOSHIZAKI
 *	Edited and translated to English by Kenji RIKITAKE
 *------------------------------------------------------------------
 * $Log: huncompress.c,v $
 * Revision 3.2  1995/11/17  07:41:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:07  hampton
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

   unsigned char *inptr, *outptr, *instart, *outstart;
   long inlength;
#  define putc(c,file) *outptr++ = c

unsigned long int textsize;

/* LZSS Parameters */

#define N		4096	/* Size of string buffer */
#define F		60	/* Size of look-ahead buffer */
#define THRESH	2
#define NIL		N	/* End of tree's node  */

/* Huffman coding parameters */

#define N_CHAR  	(256 - THRESH + F)
				/* character code (= 0..N_CHAR-1) */
#define T 		(N_CHAR * 2 - 1)	/* Size of table */
#define R 		(T - 1)			/* root position */
#define MAX_FREQ	0x8000
					/* update when cumulative frequency */
					/* reaches to this value */

#define EVEN(n) ( (n+1) & ~1)

/*
 * Tables for encoding/decoding upper 6 bits of
 * sliding dictionary pointer
 */

/* decoder table */
uchar d_code[256] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
/*40*/	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
/*80*/	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
/*120*/	0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
	0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
	0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
	0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
/*160*/	0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
	0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
	0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
	0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
	0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
	0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
	0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
	0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
	0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
	0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

/*
 * dynamic data
 */

typedef unsigned char TEXTBUF[N + F - 1];
uchar *text_buf;

typedef unsigned short FREQTAB[T + 1];	/* cumulative freq table */
unsigned short *freq;

/*
 * pointing parent nodes.
 * area [T..(T + N_CHAR - 1)] are pointers for leaves
 */
typedef short PARENTS[T + N_CHAR];
short *prnt;

/* pointing children nodes (son[], son[] + 1)*/
typedef short  SONS[T];
short *son;

uchar bit;
uchar masks[8] = {0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1};


#define DATASIZE (sizeof(TEXTBUF)+sizeof(FREQTAB)+sizeof(PARENTS)+sizeof(SONS)+10)



static unsigned short GetBit(void)	/* get one bit */
{
	short i;

	if (inlength < 0)
	    return(0);
	i = *inptr & masks[bit];
	if (++bit >= 8) {
	    bit = 0;
	    inptr++; inlength--;
	    if (((long)inptr & 0x1FF) == 0)
		putchar('%');
	}
	return (i ? 1:0);
}


static int inline GetByte(void)	/* get a byte */
{
	unsigned short i,j;

	i = *inptr++ << 8;
	i += *inptr;
	return(0xFF & (i >> (8-bit)));
}



/* initialize freq tree */

void StartHuff()
{
	short i, j;

	for (i = 0; i < N_CHAR; i++) {
		freq[i] = 1;
		son[i] = i + T;
		prnt[i + T] = i;
	}
	i = 0; j = N_CHAR;
	while (j <= R) {
		freq[j] = freq[i] + freq[i + 1];
		son[j] = i;
		prnt[i] = prnt[i + 1] = j;
		i += 2; j++;
	}
	freq[T] = 0xffff;
	prnt[R] = 0;
	bit = 0;
}


/* reconstruct freq tree */

void reconst()
{
	short i, j, k;
	unsigned short f, l;

	/* halven cumulative freq for leaf nodes */
	j = 0;
	for (i = 0; i < T; i++) {
		if (son[i] >= T) {
			freq[j] = (freq[i] + 1) / 2;
			son[j] = son[i];
			j++;
		}
	}
	/* make a tree : first, connect children nodes */
	for (i = 0, j = N_CHAR; j < T; i += 2, j++) {
		k = i + 1;
		f = freq[j] = freq[i] + freq[k];
		for (k = j - 1; f < freq[k]; k--);
		k++;
		for (l=j; l>k; l--) {
		    freq[l] = freq[l-1];
		    son[l] = son[l-1];
		}		
		freq[k] = f;
		son[k] = i;
	}
	/* connect parent nodes */
	for (i = 0; i < T; i++) {
		if ((k = son[i]) >= T) {
			prnt[k] = i;
		} else {
			prnt[k] = prnt[k + 1] = i;
		}
	}
}


/* update freq tree */

static void inline update(short c)
{
	short i, j, k, l;

	if (freq[R] == MAX_FREQ) {
		reconst();
	}
	c = prnt[c + T];
	do {
		k = ++freq[c];

		/* swap nodes to keep the tree freq-ordered */
		if (k > freq[l = c + 1]) {
			while (k > freq[++l]);
			l--;
			freq[c] = freq[l];
			freq[l] = k;

			i = son[c];
			prnt[i] = l;
			if (i < T) prnt[i + 1] = l;

			j = son[l];
			son[l] = i;

			prnt[j] = c;
			if (j < T) prnt[j + 1] = c;
			son[c] = j;

			c = l;
		}
	} while ((c = prnt[c]) != 0);	/* do it until reaching the root */
}

static short inline DecodeChar()
{
	unsigned short c;

	c = son[R];

	/*
	 * start searching tree from the root to leaves.
	 * choose node #(son[]) if input bit == 0
	 * else choose #(son[]+1) (input bit == 1)
	 */
	while (c < T) {
		c += GetBit();
		c = son[c];
	}
	c -= T;
	update(c);
	return(c);
}

static short inline DecodePosition()
{
	unsigned short i, j, c;

	/* decode upper 6 bits from given table */
	i = GetByte();
	c = (unsigned short)(d_code[i]) << 6;
	if (i < 32) j = 3;
	else if (i < 80) j = 4;
	else if (i < 144) j = 5;
	else if (i < 192) j = 6;
	else if (i < 240) j = 7;
	else j = 8;
	/* input lower 6 bits directly */
	j -= 2;
	while (j--) {
		i = (i << 1) + GetBit();
	}
	return(c | i & 0x3f);
}

void Decode(void)  /* Decoding/Uncompressing */
{
	short  i, j, k, r, c;
	unsigned long int  count;

	bcopy(inptr, &textsize, sizeof textsize);
	inlength -= sizeof textsize;
	inptr += sizeof textsize;

	if (textsize == 0)
		return;
	StartHuff();
	for (i = 0; i < N - F; i++)
		text_buf[i] = 0;
	r = N - F;
	for (count = 0; count < textsize; ) {
		c = DecodeChar();
		if (c < 256) {
			putc(c, outfile);
			text_buf[r++] = c;
			r &= (N - 1);
			count++;
		} else {
			i = (r - DecodePosition() - 1) & (N - 1);
			j = c - 255 + THRESH;
			for (k = 0; k < j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				putc(c, outfile);
				text_buf[r++] = c;
				r &= (N - 1);
				count++;
			}
		}
	}
	putchar('\n');
}

/*
 * Magic header
 */

#define	MAGIC	0x1F9E			/* 0x1F9E - lzhuff encoded file */

struct compinp {
    long index;
    u_char *p;
    struct cload *data;
};

struct compdata {
    u_short magic;
    u_long length;
    u_char d[1];
};

/*
 * Global variables used by all of uncompress
 */

/*
 * uncompress
 * Uncompress data given compress structure.
 */

uncompress (startaddr, endaddr)
    u_char *startaddr, *endaddr;
{
    register int i;
    struct cdata *cdata;
    struct cload *cload;
    struct compdata *compfile;
    long size, realsize;			/* total length of image */
    long space_needed, space_available;

    cdata = (struct cdata *)startaddr;
    if (cdata->magic != PROM2_MAGIC2) {
	puts("Magic number mismatch in compressed data\n");
	return(FALSE);
    }
    size = 0;
    cload = (struct cload *)(cdata + 1);
    if (cdata->number != 1) {
	puts("Huncompress only supports one segment");
	return(FALSE);
    }
    size = cload->len;
    realsize = size + sizeof(struct cdata) +
	cdata->number * sizeof(struct cload);
    compfile = (struct compdata *) cload->addr;
    /*
     * we know exactly how much space is needed for the uncompressed file
     */
    space_needed = (((long)startaddr < memsize) ? realsize : 0) +
	compfile->length + DATASIZE;

    space_available = memsize - (long)endaddr;
    if (space_needed > space_available) {
	printf("Image size of %d too large to uncompress\n", size);
	printf("space needed for uncompress = %d, space available = %d\n",
	       space_needed, space_available);
	flush();
	return(FALSE);
    }
    if (!config.diag)
	printf("Uncompress needs %d bytes, leaving %d bytes free\n",
	       space_needed, (space_available - space_needed));
    flush();

    /* Layout the scratch buffers at the top of memory */

    text_buf = (u_char *)(memsize - EVEN(sizeof(TEXTBUF)));
    freq = (u_short *)((u_char *)text_buf - EVEN(sizeof(FREQTAB)));
    prnt = (short *)((u_char *)freq - EVEN(sizeof(PARENTS)));
    son =  (short *)((u_char *)prnt - EVEN(sizeof(SONS)));

    /*
     * If we are loading something from RAM, relocate it to the top
     * of memory just before the scratch buffers.
     */

    if ((long)startaddr < memsize) {

	if (((long)endaddr + (2 * size) + realsize + DATASIZE) > memsize) {
	    puts("Image is too large to uncompress\n");
	    return(FALSE);
	}

	/*
	 * Relocate the image to the top of memory
	 */

	cdata = (struct cdata *)(son - (((realsize + 1) >>1) << 1));

	/* bcopy() does not check for overlapping starting and ending
	 * addresses, so we must do it here and decide whether to use
	 * bcopy() or a for loop.
	 */

	if ((u_char *)startaddr < (u_char *)cdata)
	  for (i = realsize - 1; i >= 0; i--)
	    ((u_char *)cdata)[i] = ((u_char *)startaddr)[i];
	else
	  bcopy(startaddr, cdata, realsize);

	cload = (struct cload *)(cdata + 1);
    }

    return(decompress(cload + 1, endaddr, size));
}

/*
 * decompress
 * Perform actual decompression
 */

decompress (compfile, endaddr, size)
    struct compdata *compfile;
    u_char *endaddr;
    int size;
{
    if (compfile->magic != MAGIC) {
	printf("Unknown compress type %x", compfile->magic);
	return(FALSE);
    }
    inlength = (u_long) size;
    inptr = (u_char *) &compfile->length;
    outptr = (u_char *)endaddr;
    Decode();
    return(compfile->length);
}
