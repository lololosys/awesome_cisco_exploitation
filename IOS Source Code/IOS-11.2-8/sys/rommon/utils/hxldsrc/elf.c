/* $Id: elf.c,v 3.2 1995/11/17 18:49:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/elf.c,v $
 *------------------------------------------------------------------
 * elf.c
 *
 * June 1993, Michael Beesley
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Handle an ELF file.
 *------------------------------------------------------------------
 * $Log: elf.c,v $
 * Revision 3.2  1995/11/17  18:49:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mon_elf.h"

extern char *progname, *filename, aflag, kflag, hi_unit, lo_unit;
extern unsigned long loadaddr, offset, kaddr;

#define size_t int

Elf32_Ehdr ehdr;
Elf32_Phdr phdr;

elf_file(fptr)
    FILE *fptr;
{
    register i;
    int filesize;
    
    if (fread((char *)(&ehdr),1,sizeof(Elf32_Ehdr),fptr) !=
       sizeof(Elf32_Ehdr)) {
	fprintf(stderr, "%s: can't read %s's ELF header\n",
		progname,filename);
	exit(1);
    }

    if (ehdr.e_phoff == 0 || ehdr.e_phnum == 0) {
	fprintf(stderr, "error, no program header\n");
	exit(1);
    }

    if (fseek(fptr, ehdr.e_phoff, 0) != 0) {
	fprintf(stderr, "error, can not seek to program header\n");
	exit(1);
    }

    /*
     * read program header until we find the one with the
     * a non-zero size
     */
    filesize = 0;
    for (i=0; i<ehdr.e_phnum; i++) {
	
	/*
	 * read program header
	 */
	if (fread((char *)(&phdr),1,sizeof(Elf32_Phdr),fptr) !=
	    sizeof(Elf32_Phdr)) {
	    fprintf(stderr, "%s: can't read %s's program header\n",
		    progname,filename);
	    exit(1);
	}
	if (phdr.p_filesz) {
	    filesize = phdr.p_memsz;
	    break;
	}
    }

    if (filesize == 0) {
	fprintf(stderr, "Error, could not find phdr with contents\n");
	exit(1);
    }
    fprintf(stdout,"total file 0x%x\n",filesize);
    
    checkout();
  
    /* 
     * download the header information if wanted (disk file image)
     * Not supported for ELF input files
     */
    if(kflag) {
	fprintf(stderr, "error, kflag not supported for ELF input files\n");
	exit(1);
    }
    
    /*
     * seek to the data area and download it
     */
    if (fseek(fptr, phdr.p_offset, 0) != 0) {
	fprintf(stderr, "error, could not seek to load block\n");
	exit(1);
    }

    if (!aflag) {
	loadaddr = phdr.p_vaddr;
	if(hi_unit != lo_unit) 
	    loadaddr >>= 1;            	/* halve the address */
    }

    loadaddr += offset;
    send_data(fptr,phdr.p_memsz,"text");
    loadaddr = phdr.p_vaddr; /* entry point of program */
}
