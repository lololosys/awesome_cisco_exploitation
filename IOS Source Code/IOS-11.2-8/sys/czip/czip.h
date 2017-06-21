/* $Id: czip.h,v 3.3.26.1 1996/03/18 19:11:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/czip/czip.h,v $
 *------------------------------------------------------------------
 * czip.h - Cisco Self decompressing images header file
 *
 * December 1994, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: czip.h,v $
 * Revision 3.3.26.1  1996/03/18  19:11:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  08:42:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  09:06:25  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.2  1995/11/17  08:58:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* czip.h : 
 *
 * Common defintions for all modules used in Cisco's zipped self extracting
 * router images. 
 *
 * The code in this file is derived from unzip source written
 * and put in the public domain by Mark Adler.
 */

/*
 * On r4k boxes, we do not want to use memcpy
 */
#if defined(OBJECT_4K)

#define NOMEMCPY

#endif /* OBJECT_4K */

/*
 * Magic numbers
 */
#define STACK_BYTES		(1024 * 32)	/* 32k of stack */
#define COPY_CODE_BUF		(1024 * 2)	/* 2k for copy_and_launch */
#define CZIP_BUF_BYTES		(1024 * 100)	/* 100k buffer */
#define END_BUF_BYTES		256		/* buffer after _end */

#define NULL ((void *) 0)
#define EOF     (-1)

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

/* Return codes from zip */
#define CZIP_OK      0
#define CZIP_ERROR   1
#define CZIP_WARNING 2

/* Compression methods */
#define CZIP_STORED      0
#define CZIP_COMPRESSED  1
#define CZIP_PACKED      2
#define CZIP_LZHED       3

/* methods 4 to 7 reserved */

#define CZIP_DEFLATED    8
#define MAX_METHODS 	 9

#define INBUFSIZ  	0x8000  /* input buffer size */
#define OUTBUFSIZ 	0x8000  /* output buffer size */
#define HUFT_BUFSIZE 	0x8000 	/* buffer for hufts */

#define tab_suffix window

/*
 * PKZIP header defintion
 */
#define PKZIP_MAG0	0x50		/* first magic number */
#define PKZIP_MAG1	0x4b		/* second magic number */
#define PKZIP_MAG2	0x03		/* third magic number */
#define PKZIP_MAG3	0x04		/* fourth magic number */
#define PKZIP_LOCHDR	30              /* size of local header */
#define PKZIP_LOCFIL	26              /* offset of file name field length */
#define PKZIP_LOCEXT	28              /* offset of extra field length */
#define PKZIP_LOCSIG  	0x04034b50L     /* four-byte lead-in (lsb first) */
#define PKZIP_LOCHOW	8               /* offset of compression method */
#define PKZIP_LOCFLG	6               /* offset of bit flag */

#define CRPFLG		1		/* bit for encrypted entry */

#ifndef WSIZE

#define WSIZE 0x8000     	/* window size--must be a power of two, and */
				/*  at least 32K for zip's deflate method */
#endif /* WSIZE */

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf(0))

/* put_byte is used for the compressed output, put_ubyte for the
 * uncompressed output. However unlzw() uses window for its
 * suffix table instead of its output buffer, so it does not use put_ubyte
 * (to be cleaned up).
 */
#define put_byte(c) {outbuf[outcnt++]=(uch)(c); if (outcnt==OUTBUFSIZ)\
   flush_outbuf();}

/* Output a 16 bit value, lsb first */

#define put_short(w) \
{ if (outcnt < OUTBUFSIZ-2) { \
    outbuf[outcnt++] = (uch) ((w) & 0xff); \
    outbuf[outcnt++] = (uch) ((ush)(w) >> 8); \
  } else { \
    put_byte((uch)((w) & 0xff)); \
    put_byte((uch)((ush)(w) >> 8)); \
  } \
}

/* Output a 32 bit value to the bit stream, lsb first */

#define put_long(n) { \
    put_short((n) & 0xffff); \
    put_short(((ulg)(n)) >> 16); \
}

/* Macros for getting two-byte and four-byte header values */

#define SH(p) ((ush)(uch)((p)[0]) | ((ush)(uch)((p)[1]) << 8))
#define LG(p) ((ulg)(SH(p)) | ((ulg)(SH((p)+2)) << 16))

/*
 * Globals 
 */
extern int method;         	/* compression method */
extern unsigned insize; 	/* valid bytes in inbuf */
extern unsigned inptr;  	/* index of next byte to be processed 
				   in inbuf */
extern unsigned outcnt; 	/* bytes in output buffer */

extern long bytes_in;   	/* number of input bytes */
extern long bytes_out;  	/* number of output bytes */
extern int exit_code;      	/* program exit code */

extern unsigned char inbuf[];   /* input buffer */
extern unsigned char outbuf[];  /* output buffer */
extern unsigned char window[];  /* Sliding window and suffix table */

/* for compatibility with old zip sources (to be cleaned) */

extern char *srcPtr;       	/* source from where to uncompress */
extern char *destPtr;      	/* destination to where uncompress */
extern long imageSize;     	/* size of the image to uncompress*/

/* in czip_aout_main.c */

extern void putstring(char *);
extern void error(char *s);

/* in unzip.c */

extern int unzip(void);
extern int inflate(void);

/* in czip.c */

extern int uncompress(char *, char *, long, long *);
extern int fill_inbuf (int);

/* end of file */
