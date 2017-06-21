/* $Id: bout.c,v 3.2 1995/11/17 18:49:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/bout.c,v $
 *------------------------------------------------------------------
 * bout.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Handle a file with a b.out header.
 *------------------------------------------------------------------
 * $Log: bout.c,v $
 * Revision 3.2  1995/11/17  18:49:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Handle a file with a b.out header.
*/
#include <stdio.h>
#include "b.out.h"

extern char *progname, *filename, kflag, hi_unit, lo_unit;
extern unsigned long loadaddr, offset, kaddr;

bout_file(fptr)
    FILE *fptr;
{
    struct bhdr bhdr;
    register struct bhdr *bptr = &bhdr;
    
    if(fread((char *)(&bhdr),1,sizeof(struct bhdr),fptr) !=
       sizeof(struct bhdr)) {
	fprintf(stderr, "%s: can't read %s's exec header\n",
		progname,filename);
	exit(1);
    }
    
    /* print out file size info for user */
    fprintf(stdout,".text 0x%x   .data 0x%x   .bss 0x%x   total program 0x%x\n",
	    bptr->tsize, bptr->dsize, bptr->bsize,
	    bptr->tsize + bptr->dsize + bptr->bsize);
    
    checkout();
    
    /* download the header information if wanted (disk file image) */
    if(kflag) {
	loadaddr = kaddr + offset;
	send_formatted(sizeof(struct bhdr),bptr);	
    }
    
    /* download the data areas of file */
    /* aflag means put section code at absolute offset address */
    if(hi_unit != lo_unit) loadaddr >>= 1;  /* halve the address */
    loadaddr += offset;
    send_data(fptr,bptr->tsize,".text");
    send_data(fptr,bptr->dsize,".data");
    /* do not try to send .bss section */
    loadaddr = bptr->entry; /* entry point of program */
}
