/* $Id: mkffsi.c,v 3.2 1995/11/17 18:48:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/mkffsi.c,v $
 *------------------------------------------------------------------
 * mkffsi.c
 *
 * August 1993, Rob Clevenger
 *
 * Copyright (c) 1994 - 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Program to make a flash file system image.
 *------------------------------------------------------------------
 * $Log: mkffsi.c,v $
 * Revision 3.2  1995/11/17  18:48:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/15  15:41:57  ahh
 * CSCdi35891:  gs7-monlib-m file permissions too restrictive
 * Create output file with mode 0666 and let umask do the rest.
 *
 * Revision 2.1  1995/06/07  22:42:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** mkffsi - make a flash file system image
**
** Creation date - August 10, 1993 - clev
**
** Flash file system for system flash for all platforms,
**
** Usage: mkffsi <file1> [<file2>...] <outfile>
**
** Outfile is the output file, the flash file system image.
**
*/
#define UNIX 1  /* this program runs as a Unix native */
#ifndef __svr4__
typedef unsigned long ulong;
#endif
typedef int boolean;
typedef unsigned char uchar;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "simpfsio.h"

char filebuf[BUFSIZ];
flash_fhdr_t ffh;

main(argc, argv)
    char *argv[];
{
    int i, j, numloops;
    unsigned long chksum;
    char *biname, *nameptr, *ofname, *progname;
    int debug = 0;
    int filesiz, bufsiz, bifd, offd, ofsiz, numread;
    unsigned long magic; 
    extern int optind;
    extern char *optarg;
    char *optstring = "d";


    progname = argv[0];
    while((i = getopt(argc,argv,optstring)) != EOF) {
	switch (i) {
	case 'd':  /* debug mode on */
	    debug = 1;
	    break;
	default:
usage:
	    fprintf(stderr,"usage: %s [-%s] <file1> [<file2>...] <outfile>\n",
		    progname, optstring);
	    exit(1);
	}
    }
    if(argc - optind < 2) goto usage;
    ofname = argv[argc-1];    /* and the output file name */

    if((offd = open(ofname, O_CREAT | O_RDWR | O_TRUNC, 0666)) < 0) {
	fprintf(stderr,"\ncannot open file %s for writing\n",ofname);
	goto usage;
    }
    if(debug) fprintf(stderr,"opened output file \"%s\" for writing\n", ofname);

    ofsiz = 0;  /* init output file size */
    numloops = argc - 1;

    /*
    ** Write all files to the output with file headers.
    ** Write one additional header at the end with a file
    ** size of -1 and a name of "all F's" for a 
    ** file system terminator (EOFS).
    */
    for(; optind <= numloops; optind++) {
	/* one more loop than actual number of files */
	chksum = 0;
	filesiz = 0;
	if(optind != numloops) {  /* real file on this loop */
	    biname = argv[optind];
	    if((bifd = open(biname, O_RDONLY)) < 0) {
		fprintf(stderr,"\ncannot open file %s\n",biname);
		goto usage;
	    }
	    if(debug) fprintf(stderr,"opened input file \"%s\" for reading\n",biname);

	    /*
	    ** We need to put a Flash File System header on this image.
	    ** We also need to checksum the image.
	    */
	    bufsiz = 2;
	    while(1) {   /* calculate the checksum */
		if(debug && (filesiz & 0xfff) == 0 )
		    fprintf(stderr,"Checksumming file ... %d\r",filesiz);
		*(unsigned short *)filebuf = 0; /* init */
		if((numread = read(bifd, filebuf, bufsiz)) < 1) break;
		chksum += *(unsigned short *)filebuf;
		if(chksum & ~0xffff)
		    chksum = ++chksum & 0xFFFF;    /* end-around carry */   
		filesiz += numread;
	    }
	    lseek(bifd, 0, SEEK_SET);  /* rewind the file */
	    magic = FLASH_FHDR_MAGIC;
	} else {
	    magic = filesiz = 0xffffffff; /* terminator */
	}

	/* initialize the flash file header */
	ffh.ffh_magic = magic;
	ffh.ffh_checksum = ~chksum;  /* ones compliment */
	ffh.ffh_len = filesiz;
	ffh.ffh_flags = -1;
	ffh.ffh_addr = (unsigned char *)-1;

	if(optind != numloops) {  /* real file on this loop */
	    /* strip possible preceding directory path */
	    if(nameptr = strrchr(biname, '/')) nameptr++;
	    else nameptr = biname;
	    /* fill in name field */
	    strncpy(ffh.ffh_name, nameptr, sizeof(ffh.ffh_name));
	    /* make sure its terminated */
	    ffh.ffh_name[sizeof(ffh.ffh_name)] = '\0';
	} else {  /* terminator */
	    ffh.ffh_name[0] = '\177';
	}

	if(debug) fprintf(stderr,"\nwriting the flash file header to %s\n",
			  ofname);

#ifdef BYTESWAP
    byteswap(&ffh, sizeof(ffh));
    /* byteswap this back so its right for target machine ??? */
    byteswap(&ffh.ffh_checksum, sizeof(ffh.ffh_checksum));
#endif

	if(write(offd, &ffh, sizeof(ffh)) !=  sizeof(ffh)) {
	    fprintf(stderr, "\nbad write to file %s - aborting\n",
		    ofname);
	    exit(1);
	}

	ofsiz += sizeof(ffh);

	if(optind == numloops) break;  /* done with last loop */

	if(debug) fprintf(stderr,"writing image to %s\n", ofname);

	while(filesiz) {
	    if(filesiz < sizeof(filebuf)) bufsiz = filesiz;
	    else bufsiz = sizeof(filebuf);
	    if((numread = read(bifd, filebuf, bufsiz)) != bufsiz) {
		fprintf(stderr, "\nbad read on file %s - aborting\n",
			biname);
		fprintf(stderr, "requested %d bytes, got %d\n",
			bufsiz, numread);
		exit(1);
	    }
	    if(write(offd, filebuf, numread) != numread) {
		fprintf(stderr, "\nbad write to file %s - aborting\n",
			ofname);
		exit(1);
	    }
	    filesiz -= numread;
	    ofsiz += numread;
	}
	/*
	** Make sure the next flash file header will start on
	** a 4 byte boundary.
	*/
	filebuf[0] = 0;
	while(ofsiz & 3) {
	    if(write(offd, filebuf, sizeof(char)) != sizeof(char)) {
		fprintf(stderr, "\nbad write to file %s - aborting\n",
			ofname);
		exit(1);
	    }
	    ofsiz++;
	}
    }

    /* 
    ** make sure there is not a left over fileheader magic
    ** number here (on the target)
    */
    *(long *)&filebuf[0] = 0;
    if(write(offd, filebuf, sizeof(long)) != sizeof(long)) {
	fprintf(stderr, "\nbad write to file %s - aborting\n",
		ofname);
	exit(1);
    }
    ofsiz += sizeof(long);

    if(debug) printf("%s file size is %#x\n",ofname,ofsiz);
    close(offd);
    close(bifd);
    exit(0);
}

#ifdef DONTNEED
byteswap(ptr, numbytes)
    unsigned char *ptr;
    int numbytes;
{
    int i;
    unsigned char temp;

    if(numbytes & 1) {
	fprintf(stderr,"byteswap: numbytes must be even number");
	exit(1);
    }
    for(i=0; i<numbytes; i+=2) {
	temp = *ptr;
	*ptr = *(ptr + 1);
	*(ptr+1) = temp;
	ptr += 2;
    }
}
#endif
