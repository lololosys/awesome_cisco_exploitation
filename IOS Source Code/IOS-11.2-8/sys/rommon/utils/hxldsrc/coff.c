/* $Id: coff.c,v 3.2 1995/11/17 18:49:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/coff.c,v $
 *------------------------------------------------------------------
 * coff.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Handle a file with a COFF header.
 *------------------------------------------------------------------
 * $Log: coff.c,v $
 * Revision 3.2  1995/11/17  18:49:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Note: This support is only limited to 
 * MIPS boxes.
 */

#include <stdio.h>
#include <ctype.h>

#ifdef __mips

#include <a.out.h>
#include <ldfcn.h>

extern char *progname, *filename, aflag, kflag, hi_unit, lo_unit;
extern unsigned long loadaddr, offset, kaddr;
static struct filehdr filehdr;
static struct aouthdr aouthdr;
static struct scnhdr scnhdr[16];  /* more than necessary */

coff_file(fptr)
    FILE *fptr;
{
    register i;
    register SCNHDR *scnptr;
    int filesize;
    
    if (fread((char *)(&filehdr),1,sizeof(struct filehdr),fptr) !=
       sizeof(struct filehdr)) {
	fprintf(stderr, "%s: can't read %s's file header\n",
		progname,filename);
	exit(1);
    }

    if (filehdr.f_opthdr != sizeof(AOUTHDR)) {
	fprintf(stderr,"%s: bad size specification for optional header\n",
		progname);
	exit(1);
    }

    if (fread((char *)(&aouthdr),1,sizeof(AOUTHDR),fptr) != sizeof(AOUTHDR)) {
	fprintf(stderr, "%s: can't read %s's a.out (optional) header\n",
		progname,filename);
	exit(1);
    }

    /* read our file's section headers */
    for(i=0; i<filehdr.f_nscns; i++) {
	if (fread((char *)(&scnhdr[i]),1,sizeof(struct scnhdr),fptr) !=
	    sizeof(struct scnhdr)) {
	    fprintf(stderr, "%s: can't read %s's section %d header\n",
		    progname,filename,i+1);
	    exit(1);
	}
    }
    
    /* print out file size info for user */
    filesize = 0;
    for(i=0; i<filehdr.f_nscns; i++) {
	scnptr = &scnhdr[i];
	fprintf(stdout,"%s 0x%x   ",scnptr->s_name,scnptr->s_size);
	filesize += scnptr->s_size;
    }
    fprintf(stdout,"total file 0x%x\n",filesize);
    
    checkout();
  
    /* download the header information if wanted (disk file image) */
    if(kflag) {
	loadaddr = kaddr + offset;
	send_formatted(sizeof(FILHDR),&filehdr);	
	send_formatted(sizeof(AOUTHDR),&aouthdr);	
	for(i=0; i<filehdr.f_nscns; i++) {
	    send_formatted(sizeof(SCNHDR),&scnhdr[i]);	
	}
    }
    
    /* download the data areas of file */
    for (i=0; i<filehdr.f_nscns; i++) {

	scnptr = &scnhdr[i];
	switch(scnptr->s_flags) {
	case STYP_TEXT:
	case STYP_RDATA:
	case STYP_DATA:
/*
        case STYP_BSS:
	case STYP_PAD:
	case STYP_REG: 
*/
	    break;
	default: 
	    continue;
	}

	if (scnptr->s_scnptr) {
	    if(fseek(fptr,scnptr->s_scnptr,0) != 0) {
		fprintf(stderr, "%s: can't seek to %s's section %d\n",
			progname,filename,i+1);
		exit(1);
	    }
	    if (kflag) {                    	/* disk file image */
		loadaddr = kaddr + scnptr->s_scnptr;
		if(hi_unit != lo_unit) 
		    loadaddr >>= 1;             /* halve the address */
	    } else if (!aflag) {             	/* core image */
		loadaddr = scnptr->s_paddr;
		if(hi_unit != lo_unit) 
		    loadaddr >>= 1;            	/* halve the address */
	    }
	    loadaddr += offset;
	    send_data(fptr,scnptr->s_size,scnptr->s_name);
	}
    }
    loadaddr = aouthdr.entry; /* entry point of program */
}

#else /* __mips */

coff_file(fptr)
    FILE *fptr;
{
    fprintf(stderr, "Error: coff input files only supported on MIPS\n");
    exit(1);
}

#endif /* __mips */


