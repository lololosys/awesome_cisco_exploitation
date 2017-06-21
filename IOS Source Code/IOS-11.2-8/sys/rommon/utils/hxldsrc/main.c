/* $Id: main.c,v 3.2.62.1 1996/06/16 21:19:14 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/utils/hxldsrc/main.c,v $
 *------------------------------------------------------------------
 * main.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 - 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Main routines for hxld.
 *------------------------------------------------------------------
 * $Log: main.c,v $
 * Revision 3.2.62.1  1996/06/16  21:19:14  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2  1995/11/17  18:49:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** hxld: allow user to hexload a file to a device or file
** default format is Moto S-rec
*/

/* options:
**  -i<addr>   send an entire file image, regardless of type
**  -k<addr>   send entire disk file image complete with file header
**             information to specified target load address
**  -o<addr>   specify offset address to be added to normal target load
**             address in every case
**  -a<addr>   specify beginning address of image regardless of address
**             in file header
**  -f<file>   send output to specified file (/dev's ok)
**  -p         direct output to /dev/promprg (default is user's tty)
**  -h<unit>   do not send lo units
**  -l<unit>   do not send hi units
**             <unit> can be 'b' (byte), 'w' (word), or 'l' (longword)
**  -x   s     end binary hex image with special handshake to console 
**	       device only
**  -b         reverse the bits within bytes
**  -t         send extended TEK hex format
**  -S         send Motorola S record format
**  -I         send Intel Hex record format
**  -B         send an addressed binary image
**  -c         send a binary coff file image (non-addressed)
**  -r         define the record size - must be multiple of 4 - 
**	       default is 128
*/

#include <stdio.h>
#include "filemagic.h"

static unsigned char buffer[256];
int bufsiz = sizeof(buffer)/2;  /* default is 128 byte buffer */
unsigned long loadaddr, kaddr;
char *filename, *progname;
unsigned checksum;
unsigned long offset = 0;
unsigned packnum = 0;
char *strmptr = "console";
char format = 'S';
char imagefile = 0;
char aflag = 0;
char kflag = 0;
char hi_unit = 1;
char lo_unit = 1;
char unit = 1;  /* default is byte units */
char shocount = 0;
char sleeptime = 0;
char bitmunge = 0;
FILE *outfile = stdout;
static char optstring[] = "i:a:o:k:pf:h:l:bBxSIr:cts:";

/*
 * Externs
 */
extern int optind;
extern char *optarg;

/*
 * The bit reversal table.
 */
unsigned char bitmungetbl[] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,    /* 00 to 07 */
    0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,    /* 08 to 0f */
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,    /* 10 to 17 */
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,    /* 18 to 1f */
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,    /* 20 to 27 */
    0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,    /* 28 to 2f */
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,    /* 30 to 37 */
    0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,    /* 38 to 3f */
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,    /* 40 to 47 */
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,    /* 48 to 4f */
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,    /* 50 to 57 */
    0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,    /* 58 to 5f */
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,    /* 60 to 67 */
    0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,    /* 68 to 6f */
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,    /* 70 to 77 */
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,    /* 78 to 7f */
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,    /* 80 to 87 */
    0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,    /* 88 to 8f */
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,    /* 90 to 97 */
    0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,    /* 98 to 9f */
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,    /* a0 to a7 */
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,    /* a8 to af */
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,    /* b0 to b7 */
    0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,    /* b8 to bf */
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,    /* c0 to c7 */
    0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,    /* c8 to cf */
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,    /* d0 to d7 */
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,    /* d8 to df */
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,    /* e0 to e7 */
    0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,    /* e8 to ef */
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,    /* f0 to f7 */
    0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF     /* f8 to ff */
};

/*
 * main
 */
main(argc,argv)
    int argc;
    char *argv[];
{
    register int i;
    int opbufsiz = 0;  /* optional buffer size */
    FILE *fptr;
    unsigned char baduse = 0;
    unsigned short magic, magic1;
    
    progname = argv[0];

    /*
     * Parse the command line parameters
     */
    while((i = getopt(argc,argv,optstring)) != EOF)

	switch (i) {

	/* absolute address */
	case 'a':
	    aflag = 1;
	    loadaddr = (unsigned long) strtol(optarg,(char **)NULL,16);
	    break;

	/* offset */
	case 'o':
	    offset = (unsigned) strtol(optarg,(char **)NULL,16);
	    break;

	/* output comm sleeptime */
	case 's':
	    sleeptime = (unsigned) strtol(optarg,(char **)NULL,10);
	    break;

	/* record size */
	case 'r':
	    opbufsiz = strtol(optarg,(char **)NULL,10);
	    if (opbufsiz > bufsiz) {
		fprintf(stderr, "Record size is out of range (max %d)\n",
			bufsiz);
		exit(1);
	    }
	    if (opbufsiz & 3) {
		fprintf(stderr, "Record size is not on longword boundary\n");
		exit(1);
	    }
	    break;

	/* send complete file header */
	case 'k':
	    kflag = 1;
	    kaddr = (unsigned) strtol(optarg,(char **)NULL,16);
	    break;

	case 'i':
	    imagefile = 1;
	    loadaddr = (unsigned long) strtol(optarg,(char **)NULL,16);
	    break;

	/* send output to /dev/propmrg */
	case 'p':
#ifdef NOTYET
	    strmptr = "/dev/promprg";
	    openstrm(strmptr);
#else
	    fprintf(stderr, "Error, 'p' not supported at this time\n");
	    exit(1);
#endif
	    break;

	/* user defined output stream */
	case 'f':
	    strmptr = optarg;
	    openstrm(strmptr);
	    break;
	    
	/* do not send lo units */
	case 'h':
	    lo_unit = 0;
	    hi_unit = 1;
getunit:
	    switch((int)*optarg) {
		case 'b': unit = 1; break;
		case 'w': unit = 2; break;
		case 'l': unit = 4; break;
		default:
		fprintf(stderr,"Illegal unit specifer %c, must be b,w, or l\n",
			optarg);
		exit(1);
	    }
	    break;

	/* do not send hi units */
	case 'l':
	    hi_unit = 0;
	    lo_unit = 1;
	    goto getunit;
	    break;
	
	/* send out binary image */
	case 'B':
	    format = 'B';
	    break;
	
	/* binary coff file image */
	case 'c':
	    format = 'C';
	    kflag = 1;
	    break;

	/* binary hex image */
	case 'x':
	    format = 'X';
	    break;

	/* extended TEK hex format */
	case 't':
	    format = 'T';
	    break;
	   
	/* Motorola S record format */
	case 'S':
	    format = 'S';
	    break;

	/* Intel Hex record format */
	case 'I':
	    format = 'I';
	    break;

	case 'b':
	    bitmunge = 1;
	    break;

	case '?':
	    baduse = 1;
	    break;
	}

    if (baduse || (optind == argc)) {
	fprintf(stderr, "%s usage: [-%s] file\n",progname,optstring);
	exit(2);
    }
    
    if (aflag && kflag) {
	fprintf(stderr,"cannot specify -k and -a at same time\n");
	exit(1);
    }
    
    if (imagefile && (aflag || kflag)) {
	fprintf(stderr,"cannot specify -i with -k or -a at same time\n");
	exit(1);
    }

    if (format == 'X' && outfile != stdout) {
	fprintf(stdout,"destination must be console for -x option\n");
	exit(1);
    }

    if (opbufsiz) bufsiz = opbufsiz;
    if (hi_unit ^ lo_unit) {
	if (!(format == 'S' || format == 'I')) {
	    fprintf(stderr,"Half units only supported with Intel Hex or Moto S-rec formats\n");
	    exit(1);
	}
	bufsiz *= 2;  /* compensate for sending only half */
    }

    filename = argv[optind];
    if ((fptr = fopen(filename,"r")) == NULL) {
	fprintf(stderr, "%s: can't open %s\n",progname,filename);
	exit(1);
    }

    if(imagefile) image_file(fptr);
    else {

	/* read our file's magic number */
	if (fread((char *)(&magic),sizeof(magic),1,fptr) != 1) {
	    fprintf(stderr, "%s: can't read %s's magic number\n",progname,filename);
	    exit(1);
	}
	if (fread((char *)(&magic1),sizeof(magic1),1,fptr) != 1) {
	    fprintf(stderr, "%s: can't read %s's magic1 number\n",progname,filename);
	    exit(1);
	}
	rewind(fptr);  /* rewind back to beginning */
    
	switch(magic) {

#ifdef TRYTHIS
	case I386MAGIC: 
	    coff_file(fptr); 
	    break;
	case ZMAGIC: 
	    exec_file(fptr); 
	    break;
#endif
	case FMAGIC:
	case MC68MAGIC: 
	    bout_file(fptr); 
	    break;
	case MIPSEBMAGIC: 
	case MIPSEBMAGIC_3: 
	    coff_file(fptr); 
	    break;
	case ELFMAGIC:
	    elf_file(fptr);
	    break;
	case SUNMAGIC1:  /* possible sun file */
	    if(magic1 == SUNMAGIC2) {
		bout_file(fptr);
		break;
	    }
	    /* fall through */
	default:
	    fprintf(stderr, "%s: bad magic number %04x on file %s\n",
		    progname, magic, filename);
	    exit(1);
	}
    }
    
    if (!kflag) {
	send_formatted(0,0); /* send eof terminator */
    }
    fclose(fptr);

#ifdef NOTYET
    if(outfile != stdout) 
	closestrm();
#endif

    fprintf(stdout,"\n");
    exit(0);
}

checkout()
{
    fprintf(stdout,"destination  %s\n",strmptr);
    if(sleeptime) {
	fprintf(stdout,"%d second pause for possible reconnect\n",sleeptime);
	sleep(sleeptime);
    }
}

send_data(fptr,size,section)
    register unsigned size;
    char *section;
    FILE *fptr;
{
    register unsigned i, oursize;
    unsigned char *cptr, *end;

    for(i=0; i<size; i += bufsiz) {
	if((oursize = (size - i)) > bufsiz) oursize = bufsiz;
	if(fread(buffer,1,oursize,fptr) != oursize) {
	    fprintf(stderr, "%s: trouble reading %s's %s section\n",
		    progname,filename,section);
	    exit(1);
	}
	if(bitmunge) {  /* reverse bits within the byte */
	    for(cptr = buffer, end = cptr + oursize; cptr < end; cptr++)
		*cptr = bitmungetbl[*cptr];
	}
	send_formatted(oursize,buffer);
    }
}

send_formatted(size,ptr) /* determine which format to use */
{
    if(shocount) {
	fprintf(stdout,"\r%04x",loadaddr);  /* show load address to user */
	fflush(stdout);
    }
    switch(format) {
    case 'I': intelhexout(size,ptr); break;
    case 'B':
    case 'C': binaryout(size,ptr); break;
#ifdef NOTYET
    case 'X': binhexout(size,ptr); break;
#endif
    case 'T': xtekhexout(size,ptr); break;
    case 'S': motsrecout(size,ptr); break;
    }
    fflush(outfile);
}






