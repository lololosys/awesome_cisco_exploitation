/* $Id: epromsum.c,v 3.2 1995/11/17 18:48:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/epromsum.c,v $
 *------------------------------------------------------------------
 * epromsum.c
 *
 * January 1995, Rob Clevenger
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * File checksum routine for a.out type files.
 * This utility was borrowed from the low-end DiagMon effort.
 *------------------------------------------------------------------
 * $Log: epromsum.c,v $
 * Revision 3.2  1995/11/17  18:48:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
** Calculate the checksum of the text section of an a.out file and
** stuff the sum into the variable named checksum in the file.
**
** This code makes a few assumptions:
** * The file is a.out format and has standard _etext label.
** * There is a label "__start" at the beginning of the file and is
**   also the beginning of the code to be summed.
** * There is a unsigned int variable named "_checksum" in the
**   text section for depositing the checksum into.
**
** syntax  : sum <symbols file> <a.out file>
*/

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include "fsio.h"


#define GETCHECKSUM 0x01
#define GETETEXT    0x02
#define GETSTART    0x04


main(int argc, char *argv[])
{
    FILE *fp;
    char labelbuf[50];   /* buffer to store label string */
    int labelflags;      /* 0: label found, 1: search for label */
    int i, textsize, start, chksumloc, debug;
    char symbol, *symfile, *targfile;
    int addr;
    unsigned short sum = 0;
    unsigned short tempsum = 0;

    if(argc < 3 || argc > 4) {
usage:
	fprintf(stderr,"usage: %s [-d] <symbols file> <a.out target file>\n",
		argv[0]);
	exit(1);
    }
    if(argc == 4) {
	if(strcmp(argv[1], "-d") != 0) goto usage;
	debug = 1;
	symfile = argv[2];
	targfile = argv[3];
    } else {
	debug = 0;
	symfile = argv[1];
	targfile = argv[2];
    }

    /* open symbols file to search the address for label */ 
    if((fp = fopen(symfile, "r")) == NULL) {
	fprintf(stderr, "can't open symbols file: %s \n", symfile);
	exit(2);
    }

    labelflags = GETCHECKSUM+GETETEXT+GETSTART;  /* initialize flags */

    /* look for our labels */
    while(!feof(fp) &&
	  (fscanf(fp,"%x %c %s", &addr, &symbol, labelbuf) == 3)) {
	if(labelflags & GETCHECKSUM) {
	    if(strcmp(labelbuf, "_checksum") == 0) {
		chksumloc = addr;
		if(debug) printf("_checksum = %#.8x\n", chksumloc);
		labelflags &= ~GETCHECKSUM;         /* found label1 */
	    }
	}
	if(labelflags & GETETEXT) {
	    if(strcmp(labelbuf, "_etext") == 0) {
		textsize = addr;
		if(debug) printf("_etext = %#.8x\n", textsize);
		labelflags &= ~GETETEXT;            /* found label2 */
	    }
	}
	if(labelflags & GETSTART) {
	    if(strcmp(labelbuf, "__start") == 0) {
		start = addr;
		if(debug) printf("_start = %#.8x\n", start);
		labelflags &= ~GETSTART;            /* found label3 */
	    }
	}
    }
    fclose(fp);

    if(labelflags) {
	fprintf(stderr, "error finding labels, %d\n", labelflags);
	exit(9);
    }

    chksumloc -= start;   /* calculate checksum location in file */
    chksumloc += CISCOEXEC_HDRSIZ;
    textsize  -= start;   /* and text size */
    if(debug) printf("checksum location in file is %#.8x\n"
		     "text size is %#x\n", chksumloc, textsize);

    /* open target file to perform text segment checksum */
    /* rule : the total sum will be stored in checksum (2 bytes long) */
    if((fp = fopen(targfile, "r+b")) == NULL) {
	fprintf(stderr, "can't open target file: %s\n", targfile);
	exit(3);
    }

    if(fseek(fp, (long)(CISCOEXEC_HDRSIZ), 0) != 0) {
	fprintf(stderr, "error seeking file: %s\n", targfile);
	exit(4);
    }
    for(i=0; i<textsize; i++) sum += getc(fp);

    if(fseek(fp, (long)chksumloc, 0) != 0) {
	fprintf(stderr, "error seeking file: %s\n", targfile);
	exit(5);
    }
    putw(sum, fp);  /* store checksum value (actually writes an int) */
    if(debug)
	printf("wrote checksum value of %#.4x to checksum location\n",
		     sum);
    rewind(fp);

    /* sanity check */
    if(fseek(fp, (long)(CISCOEXEC_HDRSIZ), 0) != 0) {
	fprintf(stderr, "error seeking file: %s\n", targfile);
	exit(6);
    }    
    /* total sum which included checksum */
    for(i=0; i<textsize; i++) tempsum += getc(fp);
    rewind(fp);

    if(fseek(fp, (long)chksumloc, 0) != 0) {
	fprintf(stderr, "error seeking file: %s\n", targfile);
	exit(7);
    }

    /* take out checksum value */
    for(i=0; i<sizeof(int); i++) tempsum -= (unsigned short)getc(fp);

    if(debug) printf("sanity check of sum yeilds %#x\n", tempsum);

    if(tempsum != sum) {
	fprintf(stderr,"error on sanity check\n");
	exit(8);
    }

    fclose(fp);  /* done with input file - code */ 

    exit(0);
}

/* end of module */

