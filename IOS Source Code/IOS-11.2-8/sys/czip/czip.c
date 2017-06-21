/* $Id: czip.c,v 3.3 1996/01/16 00:56:40 vdukki Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/czip/czip.c,v $
 *------------------------------------------------------------------
 * czip.c - czip uncompress
 *
 * December 1994, Michael Beesley
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: czip.c,v $
 * Revision 3.3  1996/01/16  00:56:40  vdukki
 * CSCdi46688:  xx-j-mz.110-4.2 fails to unzip properly during boot
 * Updated the code to display a warning message if the number of bytes
 * not consumed, by the inflate() function, is less than or equal to 32.
 *
 * Revision 3.2  1995/11/17  08:58:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * The unzip code was written and put in the public domain by Mark Adler.
 * This is the interface to the unzip code. It has been modified
 * by Cisco for use in self-decompressing router images based on
 * the zip compression algorithms.
 */

#include "../czip/czip.h"

#define CZIP_MIN_REMAIN 32

extern void puthex(char *s, unsigned long hex);

/* global buffers */

unsigned char inbuf[INBUFSIZ];
unsigned char outbuf[OUTBUFSIZ];
unsigned char window[2*WSIZE];

/* local variables */

int method = CZIP_DEFLATED;	/* compression method */
int exit_code = CZIP_OK;   	/* program exit code */
long bytes_in = 0;             	/* number of input bytes */
long bytes_out = 0;            	/* number of output bytes */
long total_in = 0;         	/* input bytes for all files */
long total_out = 0;        	/* output bytes for all files */
unsigned insize = 0;           	/* valid bytes in inbuf */
unsigned inptr = 0;            	/* index of next byte to be processed 
				   in inbuf */
unsigned outcnt = 0;           	/* bytes in output buffer */
char *srcPtr = 0;       	/* source from where to uncompress */
char *destPtr = 0;      	/* destination to where uncompress */
long imageSize = 0;     	/* size of the image to uncompress*/

/*
 * fill_inbuf : 
 * Fill the input buffer. Only called when the input buffer has drained
 * to empty. eof_ok is set if an EOF is an acceptable result
 */
int fill_inbuf (int eof_ok)
{
    long i;
    
    /* Read as much as possible */
 
    insize = 0;
    for (i=0;i< INBUFSIZ;i++) {
        if (imageSize==0)
            break;
        inbuf[i] = srcPtr[i];
        imageSize--;
    } 
    insize = i;
    if (insize == 0) {
        if (eof_ok) { 
            return(EOF);
	} else {
	    error("unacceptable eof error\n");
	}
    } else {
        srcPtr = srcPtr + insize;
    }
    bytes_in += (unsigned long)insize;
    inptr = 1;
    return (inbuf[0]);
}

/*
 * get_method :
 * Check magic numbers at the start of the compressed image,
 * return the compression method. return -1 if there is
 * an error, -2 if there is a warning
 */
static int get_method (void)
{
    char magic[2];

    magic[0] = (char)get_byte();
    magic[1] = (char)get_byte();

    method = -1;      	/* unknown yet */

    if (magic[0] == PKZIP_MAG0 && magic[1] == PKZIP_MAG1 &&
	inptr == 2 && inbuf[2] == PKZIP_MAG2 && 
	inbuf[3] == PKZIP_MAG3) {

	/*
	 * Process PKZIP file
	 */
	unsigned char *h = inbuf;
	int decrypt;

	inptr += PKZIP_LOCHDR - 2 + SH(h + PKZIP_LOCFIL) + 
	    SH(h + PKZIP_LOCEXT);
	if (inptr > insize || LG(h) != PKZIP_LOCSIG) {
	    error("pkzip header - invalid entries\n");
	    exit_code = CZIP_ERROR;
	    return(-1);
	}
	method = h[PKZIP_LOCHOW];
	if (method != CZIP_DEFLATED) {
	    error("pkzip header - method not deflate\n");
	    exit_code = CZIP_ERROR;
	    return(-1);
	}
	decrypt = h[PKZIP_LOCFLG] & CRPFLG;
	if (decrypt != 0) {
	    error("pkzip header - encrypted file\n");
	    exit_code = CZIP_ERROR;
	    return(-1);
	}
    }
	
    if (method >= 0) 
	return(method);

    error("Not in zip format\n");
    exit_code = CZIP_ERROR;
    return(-1);
}

/*
 * uncompress :
 * Uncompress inputsize bytes staring at src, to dest. Record
 * the number of bytes in the decompressed image in outputsize
 */
int uncompress (
    char *src,
    char *dest,
    long inputSize,
    long *outputSize)
{
    srcPtr = src;
    destPtr = dest;
    imageSize   = inputSize;

    /*
     * Clear input and output buffers
     */
    outcnt = 0;
    insize = 0;
    inptr = 0;
    bytes_in = 0;
    bytes_out = 0;

    method = get_method();  	/* get decompress method */
    if (method < 0) {
	*outputSize = 0;
	return(exit_code);
    }

    /* 
     * Actually do the compression/decompression
     */
    unzip();
    *outputSize = bytes_out;
    return(exit_code);
}

/* end of file */
