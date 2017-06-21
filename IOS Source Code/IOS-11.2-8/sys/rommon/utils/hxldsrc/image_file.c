/* $Id: image_file.c,v 3.2 1995/11/17 18:49:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/image_file.c,v $
 *------------------------------------------------------------------
 * image_file.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Code to handle an image file.
 *------------------------------------------------------------------
 * $Log: image_file.c,v $
 * Revision 3.2  1995/11/17  18:49:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Code to handle an image file
** Send the entire file
** Address was specified by the user on the command line
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

extern char *progname, *filename, hi_unit, lo_unit;
extern unsigned long loadaddr, offset;

image_file(fptr)
     FILE *fptr;
{
    struct stat statbuf;

    if(stat(filename, &statbuf) < 0) {
	fprintf(stderr, "%s: can't stat %s\n",progname,filename);
	exit(1);
    }
	
    /* print out file size info for user */
    fprintf(stdout,"total file size 0x%x\n", statbuf.st_size);
    
    checkout();
  
    if(hi_unit != lo_unit) loadaddr >>= 1; /* halve the address */
    loadaddr += offset;
    send_data(fptr,statbuf.st_size,"\"entire file image\"");
    loadaddr = 0; /* fake entry point of program */
}
