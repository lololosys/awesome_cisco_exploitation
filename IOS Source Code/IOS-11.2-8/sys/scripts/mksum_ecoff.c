/* $Id: mksum_ecoff.c,v 3.2.62.1 1996/05/10 23:53:39 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/scripts/mksum_ecoff.c,v $
 *------------------------------------------------------------------
 * mksum_ecoff.c - Generate checksum of .text segment for
 * 		   MIPS ECOFF format files
 *
 * December 1993, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mksum_ecoff.c,v $
 * Revision 3.2.62.1  1996/05/10  23:53:39  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.2  1995/11/17  18:49:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This executable can only run on an SGI box at the 
 * moment. So barf if __mips is not defined
 */
#ifndef __mips

/*
 * Error : Can only be compiled on an SGI/MIPS workstation
 */
**** Error *****;

#endif

#include <stdio.h>
#include <fcntl.h>
#include <sys/param.h>
#include <a.out.h>
#include <filehdr.h>
#ifndef GP_PAD
#include <syms.h>
#endif
#include <ldfcn.h>
#ifndef SCNHDR
#include <scnhdr.h>
#endif

#include "../os/sum_magic_number.h"
#include "../../boot/cpu.h"

#define val(x)		#x
#define macval(x)	val(x)
#define TEXTSUM_IMAGE   macval(MAGIC_VARIABLE)
#define START           "__start"
#define ETEXT		"etext"

#define START_MASK	0x01
#define CHKSUM_MASK	0x02
#define ETEXT_MASK	0x04

static int debug = 0;		/* used for development */

struct scnhdr text_hdr, data_hdr, sdata_hdr, rdata_hdr;
struct scnhdr *text_hdr_ptr = 0, *data_hdr_ptr = 0;
struct scnhdr *sdata_hdr_ptr = 0, *rdata_hdr_ptr;
int text_sec = 0, data_sec = 0, rdata_sec = 0, sdata_sec = 0;

SYMR start_symbol, chksum_symbol, etext_symbol;

#ifdef UNIX
#define	CISCOMAGIC	0x0160		/* magic number */
#else
#define	CISCOMAGIC	0x0140		/* magic number */
#endif

#define CISCOTYPE	0x02		/* machine type */

/*
 * Do the guts of the summation.  If "suspend" is true, we will
 * suspend periodically.  Note that the tests for "suspend" will
 * optimize out.  Love that compiler.
 */

unsigned long dosum (unsigned long *current, int nwords)
{
    unsigned long sum;
    int looplimit;
    
    sum = 0;
    while (nwords--) {
	ADD_AROUND(*current++, sum);
    }
    if (sum == MAGIC_VALUE) {
	sum++;
    }

    return(sum);
}

extern void *malloc(unsigned int);

int main(int argc, char *argv[])
{
    int desc, nchars, nwords;
    unsigned long textsize, offset_to_text;
    unsigned int textsum_image = 0;
    unsigned long *text = NULL;
    char *program_name = argv[0];
    char filename[MAXPATHLEN];
    struct scnhdr scn_hdr;
    struct filehdr file_hdr;
    struct aouthdr aout_hdr;
    LDFILE *ldptr;
    char *namep;
    SYMR symbol;
    
    int symmaxlocal, symmaxextern, symmax;
    int totalsize;
    int i, symbol_flag;

    unsigned long paddr_base, paddr_mask;
    unsigned int textsum_image_offset = 0;

    if (argc < 2) {
	/*
	 * No filename supplied on the command line
	 */
usage:
	fprintf(stderr, "usage: %s file [-paddr_base 0x....]\n", argv[0]);
	exit(1);
    }

    /*
     * save away program name and filename for later
     */
    program_name = argv[0];
    strcpy(filename, argv[1]);

    paddr_base = 0;
    if (argc > 2) {
	if (strcmp(argv[2], "-paddr_base") ||
	    argc != 4) {
	    goto usage;
	}
	if (paddr_base = strtoul(argv[3], 0, 16), !paddr_base) {
	    goto usage;
	}
    }

    /*
     * We take three args; name of the unstripped ecoff file for the checksum
     * and an optional flag for physical base address and the physical
     * base address itself.
     */
    desc = open(filename, O_RDWR, 0);
    if (desc < 0) {
	/*
	 * unable to read/write filename passed as first argument
	 */
	fprintf(stderr, "Unable to open for read/write file %s\n", filename);
	perror("");
	exit(1);
    }


    /*
     * read in the exec structure at the head of the file
     */
    nchars = read(desc, &file_hdr, sizeof(struct filehdr));
    if (nchars != sizeof(struct filehdr)) {
	fprintf(stderr, "Unable to read file header%s\n", filename);
	exit(1);
    }

    if (file_hdr.f_opthdr != sizeof(AOUTHDR)) {
	fprintf(stderr, "Error: bad size spec for optional a.out header\n");
	exit(1);
    } else {
	nchars = read(desc, &aout_hdr, sizeof(AOUTHDR));
	if (nchars != sizeof(AOUTHDR)) {
	    fprintf(stderr, "Failed to read optional aout header\n");
	    exit(1);
	}
    }

    if (debug) {
	printf("Optional aout header\n");
	printf("magic : 0x%x, vstamp : 0x%x, tsize : 0x%x, dsize : 0x%x\n",
	       aout_hdr.magic, aout_hdr.vstamp, aout_hdr.tsize, 
	       aout_hdr.dsize);
	printf("bsize : 0x%x, entry : 0x%x, text_start : 0x%x\n",
	       aout_hdr.bsize, aout_hdr.entry, aout_hdr.text_start);
	printf("data_start : 0x%x, bss_start : 0x%x\n", aout_hdr.data_start,
	       aout_hdr.bss_start);
    }

    for(i=0; i<file_hdr.f_nscns; i++) {
	nchars = read(desc, &scn_hdr, sizeof(struct scnhdr));
	if (nchars != sizeof(struct scnhdr)) {
	    fprintf(stderr, "Error: cannot read section %d header\n", i);
	    exit(1);
	}
	
	if (scn_hdr.s_flags == STYP_TEXT &&
	    !strcmp(scn_hdr.s_name, ".text")) {
	    bcopy(&scn_hdr, &text_hdr, sizeof(struct scnhdr));
	    text_hdr_ptr = &text_hdr;
	    text_sec = i;
	    if (debug) {
		fprintf(stderr, "sec %s, size %d, i %d, flags %d\n",
			scn_hdr.s_name, scn_hdr.s_size, i, scn_hdr.s_flags);
	    }

	} else if (scn_hdr.s_flags == STYP_DATA &&
		   !strcmp(scn_hdr.s_name, ".data")) {
	    bcopy(&scn_hdr, &data_hdr, sizeof(struct scnhdr));
	    data_hdr_ptr = &data_hdr;
	    data_sec = i;
	    if (debug) {
		fprintf(stderr, "sec %s, size %d, i %d, flags %d\n",
			scn_hdr.s_name, scn_hdr.s_size, i, scn_hdr.s_flags);
	    }
	} else if (scn_hdr.s_flags == STYP_SDATA &&
		   !strcmp(scn_hdr.s_name, ".sdata")) {
	    bcopy(&scn_hdr, &sdata_hdr, sizeof(struct scnhdr));
	    sdata_hdr_ptr = &sdata_hdr;
	    sdata_sec = i;
	    if (debug) {
		fprintf(stderr, "sec %s, size %d, i %d, flags %d\n",
			scn_hdr.s_name, scn_hdr.s_size, i, scn_hdr.s_flags);
	    }
	} else if (scn_hdr.s_flags == STYP_RDATA &&
		   !strcmp(scn_hdr.s_name, ".rdata")) {
	    bcopy(&scn_hdr, &rdata_hdr, sizeof(struct scnhdr));
	    rdata_hdr_ptr = &rdata_hdr;
	    rdata_sec = i;
	    if (debug) {
		fprintf(stderr, "sec %s, size %d, i %d, flags %d\n",
			scn_hdr.s_name, scn_hdr.s_size, i, scn_hdr.s_flags);
	    }
	}
    }

    /*
     * check that we got a text and data section
     */
    if (!data_hdr_ptr || !text_hdr_ptr) {
	fprintf(stderr, "Failed to find text or data section header\n");
	exit(1);
    }

    /*
     * verify that the magic number is correct.
     */
    if (file_hdr.f_magic != CISCOMAGIC) {
	fprintf(stderr, "Bad magic number 0x%x (should be 0x%x)\n",
		file_hdr.f_magic, CISCOMAGIC);
	exit(1);
    }

    /*
     * go fetch the .text section into memory
     */
    textsize = text_hdr.s_size;
    text = malloc(textsize);
    if (text == NULL) {
	/*
	 * no space available
	 */
	fprintf(stderr, "failed to malloc %d bytes.\n", textsize);
	exit(1);
    }

    nchars = lseek(desc, text_hdr.s_scnptr, SEEK_SET);
    if (nchars == -1) {
	fprintf(stderr, "Failed to seek to the start of the text segment\n");
	exit(1);
    }

    nchars = read(desc, text, textsize);
    if (nchars != textsize) {
	/*
	 * oops, we were unable to read in the entire textsize for
	 * some reason
	 */
	fprintf(stderr, "Unable to read .text section");
	exit(1);
    }

    /*
     * If we have been asked to update the physical address fields in
     * the section headers, do it here
     */
    if (paddr_base) {
	
	for (paddr_mask=0, i=0; i<8; i++) {
	    if (((paddr_base >> ((7-i)*4)) & 0x0f) != 0)
		paddr_mask <<= 4;
	    else
		paddr_mask = (paddr_mask << 4) | 0x0f;
	}
	if ((paddr_base & ~paddr_mask) != paddr_base) {
	    fprintf(stderr, "Failed to find mask for 0x%x, mask= 0x%x\n",
		    paddr_base, paddr_mask);
	    exit(1);
	}
	    
	/*
	 * Do the optional aout header
	 */
	nchars = sizeof(struct filehdr);
	lseek(desc, nchars, SEEK_SET);
	if (nchars == -1) {
	    fprintf(stderr, "Error, failed to seek to optional aout header\n");
	    exit(1);
	}
	aout_hdr.entry = (aout_hdr.entry & paddr_mask) + paddr_base;
	nchars = write(desc, &aout_hdr, sizeof(AOUTHDR));
	if (nchars != sizeof(AOUTHDR)) {
	    fprintf(stderr, "Error, failed to write optional aout header\n");
	    exit(1);
	}

	/*
	 * Do the text section
	 */
	nchars = sizeof(struct filehdr) + sizeof(AOUTHDR) + 
	    (text_sec * sizeof(struct scnhdr));
	lseek(desc, nchars, SEEK_SET);
	if (nchars == -1) {
	    fprintf(stderr, "Failed to seek to beginning of text section\n");
	    exit(1);
	}
	if (!text_hdr_ptr) {
	    fprintf(stderr, "Failed to find text section\n");
	    exit(1);
	}
	text_hdr_ptr->s_paddr = (text_hdr_ptr->s_paddr & paddr_mask) + 
	    paddr_base;
	nchars = write(desc, text_hdr_ptr, sizeof(struct scnhdr));
	if (nchars != sizeof(struct scnhdr)) {
	    fprintf(stderr, "Failed to rewrite text section header\n");
	    exit(1);
	}

	/*
	 * Do the .rdata section
	 */
	if (rdata_hdr_ptr) {
	    nchars = sizeof(struct filehdr) + sizeof(AOUTHDR) + 
		(rdata_sec * sizeof(struct scnhdr));
	    lseek(desc, nchars, SEEK_SET);
	    if (nchars == -1) {
		fprintf(stderr, 
			"Failed to seek to beginning of rdata section\n");
		exit(1);
	    }
	    rdata_hdr_ptr->s_paddr = (rdata_hdr_ptr->s_paddr & paddr_mask) + 
		paddr_base;
	    nchars = write(desc, rdata_hdr_ptr, sizeof(struct scnhdr));
	    if (nchars != sizeof(struct scnhdr)) {
		fprintf(stderr, "Failed to rewrite rdata section header\n");
		exit(1);
	    }
	}

	/*
	 * Do the .sdata section
	 */
	if (sdata_hdr_ptr) {
	    nchars = sizeof(struct filehdr) + sizeof(AOUTHDR) + 
		(sdata_sec * sizeof(struct scnhdr));
	    lseek(desc, nchars, SEEK_SET);
	    if (nchars == -1) {
		fprintf(stderr, 
			"Failed to seek to beginning of sdata section\n");
		exit(1);
	    }
	    sdata_hdr_ptr->s_paddr = (sdata_hdr_ptr->s_paddr & paddr_mask) + 
		paddr_base;
	    nchars = write(desc, sdata_hdr_ptr, sizeof(struct scnhdr));
	    if (nchars != sizeof(struct scnhdr)) {
		fprintf(stderr, "Failed to rewrite sdata section header\n");
		exit(1);
	    }
	}

	/*
	 * Do the .data section
	 */
	if (data_hdr_ptr) {
	    nchars = sizeof(struct filehdr) + sizeof(AOUTHDR) + 
		(data_sec * sizeof(struct scnhdr));
	    lseek(desc, nchars, SEEK_SET);
	    if (nchars == -1) {
		fprintf(stderr, 
			"Failed to seek to beginning of data section\n");
		exit(1);
	    }
	    data_hdr_ptr->s_paddr = (data_hdr_ptr->s_paddr & paddr_mask) + 
		paddr_base;
	    nchars = write(desc, data_hdr_ptr, sizeof(struct scnhdr));
	    if (nchars != sizeof(struct scnhdr)) {
		fprintf(stderr, "Failed to rewrite data section header\n");
		exit(1);
	    }
	}
    }
    close(desc);

    /*
     * Go and fetch the symbol table
     */
    ldptr = ldopen(filename, NULL);
    if ( ldreadst(ldptr, -1) == FAILURE ) {
	fprintf(stderr, "Failed - no synbol table present\n");
	exit(1);
    }
    
    symmaxlocal = SYMHEADER(ldptr).isymMax;
    symmaxextern = SYMHEADER(ldptr).iextMax;
    symmax = symmaxlocal + symmaxextern;

    symbol_flag = START_MASK | CHKSUM_MASK | ETEXT_MASK;

    for (i=0; i<symmax; i++) {
	
	if (!symbol_flag)
	    break;
	
	if (ldtbread(ldptr, i, &symbol) == FAILURE) {
	    fprintf(stderr, "Failed - could not read symbol %d\n", i);
	    exit(1);
	}
	namep = ldgetname(ldptr, &symbol);
	if (namep == NULL) {
	    fprintf(stderr, "Failed - could not get symbol %d's name\n", i);
	    exit(1);
	}
	if ((strcmp(namep, START) == 0) && (symbol_flag & START_MASK)) {
	    symbol_flag &= ~START_MASK;
	    bcopy(&symbol, &start_symbol, sizeof(SYMR));
	    if (debug) {
		fprintf(stderr, "_start : %d\n", i);
		fprintf(stderr, "iss=0x%x value=0x%x st=%u sc=%u index=0x%x\n",
			symbol.iss, symbol.value, symbol.st,
			symbol.sc, symbol.index);
	    }
	} else if ((strcmp(namep, TEXTSUM_IMAGE) == 0) 
		   && (symbol_flag & CHKSUM_MASK)) {
	    symbol_flag &= ~CHKSUM_MASK;
	    bcopy(&symbol, &chksum_symbol, sizeof(SYMR));
	    if (debug) {
		fprintf(stderr, "chksum: %d\n", i);
		fprintf(stderr, "iss=0x%x value=0x%x st=%u sc=%u index=0x%x\n",
			symbol.iss, symbol.value, symbol.st,
			symbol.sc, symbol.index);
	    }
	} else if ((strcmp(namep, ETEXT) == 0) 
		   && (symbol_flag & ETEXT_MASK)) {
	    symbol_flag &= ~ETEXT_MASK;
	    bcopy(&symbol, &etext_symbol, sizeof(SYMR));
	    if (debug) {
		fprintf(stderr, "etext: %d\n", i);
		fprintf(stderr, "iss=0x%x value=0x%x st=%u sc=%u index=0x%x\n",
			symbol.iss, symbol.value, symbol.st,
			symbol.sc, symbol.index);
	    }
	} 
    }

    if (debug) {
	fprintf(stderr, "_start: iss=0x%x value=0x%x st=%u sc=%u index=0x%x\n",
		start_symbol.iss, start_symbol.value, start_symbol.st,
		start_symbol.sc, start_symbol.index);
	fprintf(stderr, "chksum: iss=0x%x value=0x%x st=%u sc=%u index=0x%x\n",
		chksum_symbol.iss, chksum_symbol.value, chksum_symbol.st,
		chksum_symbol.sc, chksum_symbol.index);
	fprintf(stderr, "etext: iss=0x%x value=0x%x st=%u sc=%u index=0x%x\n",
		etext_symbol.iss, etext_symbol.value, etext_symbol.st,
		etext_symbol.sc, etext_symbol.index);
    }

    if (symbol_flag) {
	fprintf(stderr, "Failed to find all required symbols\n");
	exit(1);
    }

    /*
     * calculate the number of words in the .text section
     */
    offset_to_text = (start_symbol.value - text_hdr.s_vaddr) 
	/ sizeof(unsigned long);
    nwords = (etext_symbol.value - start_symbol.value);
    nwords = nwords / sizeof(unsigned long);

    textsum_image = dosum(text + offset_to_text, nwords);

    if (debug) {
	fprintf(stderr, "textsum_image: 0x%x, text_hdr.s_vaddr: 0x%x\n", 
		textsum_image, text_hdr.s_vaddr);
	fprintf(stderr, "offset_to_text: 0x%x, nwords : 0x%x\n",
		offset_to_text, nwords);
    }

    /*
     * cleanup .text stuff
     */
    free(text);
    text = NULL;

    /*
     * open up the file and re-write the value for the magic variable
     */
    desc = open(filename, O_RDWR, 0);
    if (desc < 0) {
	/*
	 * unable to read/write filename passed as first argument
	 */
	fprintf(stderr, "Unable to open for read/write file %s\n", filename);
	perror("");
	exit(1);
    }

    /*
     * If the variable is not in the .data section, try the .sdata 
     * section if it exists
     */
    if (chksum_symbol.value >= data_hdr.s_vaddr &&
	chksum_symbol.value < (data_hdr.s_vaddr + data_hdr.s_size)) {

	textsum_image_offset = data_hdr.s_scnptr + chksum_symbol.value - 
	    data_hdr.s_vaddr;

    } else {

	if (!sdata_hdr_ptr) {
	    fprintf(stderr, "Error, .sdata section not found but checksum\n",
		    "variable is not in the .data section\n");
	    exit(1);
	} else if (chksum_symbol.value >= sdata_hdr.s_vaddr &&
		   chksum_symbol.value < (sdata_hdr.s_vaddr + 
					  sdata_hdr.s_size)) {
	    
	    textsum_image_offset = sdata_hdr.s_scnptr + chksum_symbol.value - 
		sdata_hdr.s_vaddr;

	} else {
	    fprintf(stderr, "chksum outside limit of data sections\n");
	    exit(1);
	}
    }
     
    nchars = lseek(desc, textsum_image_offset, SEEK_SET);
    if (nchars == -1) {
	fprintf(stderr, "Failed to seek to the start of the text segment\n");
	exit(1);
    }

    nchars = write(desc, &textsum_image, sizeof(unsigned long));
    if (nchars != sizeof(unsigned long)) {
	/*
	 * we were unable to re-write the magic checksum
	 * variable for some reason
	 */
	fprintf(stderr, "Unable to re-write checksum variable\n");
	exit(1);
    }
    close(desc);

    if (paddr_base)
	printf("Reassigned physical addresses using 0x%x as base\n",
	       paddr_base);

    printf("Writing Checksum: %s has a text segment checksum of 0x%x\n",
	   filename, textsum_image);
    exit(0);
}


