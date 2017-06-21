/* $Id: mksum.c,v 3.2 1995/11/17 18:49:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/scripts/mksum.c,v $
 *------------------------------------------------------------------
 * mksum.c - generate the checksum of the .text segment
 *
 * December 1993, Mark D. Baushke
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Generate the .text section checksum and write it into the
 * approparite compile time varaible in the .data section.
 * This file is to be run on the host during the compilation process.
 *------------------------------------------------------------------
 * $Log: mksum.c,v $
 * Revision 3.2  1995/11/17  18:49:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/21  19:01:15  smackie
 * Fix mksum dependancies by specifying the full include path so that the
 * compile options for mksum match the dependancy generation. (CSCdi37193)
 *
 * Revision 2.1  1995/06/07  22:43:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/param.h>

#include "../h/cisco_a.out.h"
#include "../os/sum_magic_number.h"

#define START "__start"
#define val(x) #x
#define macval(x) val(x)
#define ENDTEXT "_etext"
#define ENDDATA "_edata"
#define TEXTSUM_IMAGE "_" macval(MAGIC_VARIABLE)

static int debug = 0;		/* used for development */

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

void print_symbol(struct nlist *p)
{
    if (debug) {
	printf("name:  %s\n", p->n_un.n_name);
	printf("type:  0x%x\n", p->n_type);
	printf("other: 0x%x\n", p->n_other);
	printf("desc:  0x%x\n", p->n_desc);
	printf("value: 0x%x\n\n", p->n_value);
    }
    
}

int main(int argc, char *argv[])
{
    int desc, nchars, nwords;
    unsigned long textsize;
    struct exec header;
    unsigned int textsum_image = 0;
    unsigned long *text = NULL;
    char *program_name = argv[0];
    char filename[MAXPATHLEN];
    long int syms_offset, strs_offset;
    unsigned int syms_size, strs_size;
    int symcount;
    int totalsize;
    struct nlist *symbols_and_strings;
    struct nlist *start_entry   = NULL;
    struct nlist *etext_entry   = NULL;
    struct nlist *edata_entry   = NULL;
    struct nlist *textsum_image_entry = NULL;
    unsigned long start_addr = 0;
    unsigned int textsum_image_offset = 0;
    char *strings;

    if (argc < 2) {
	/*
	 * No filename supplied on the command line
	 */
	fprintf(stderr, "No input file specified\n");
	exit(1);
    }

    /*
     * save away program name and filename for later
     */
    program_name = argv[0];
    strcpy(filename, argv[1]);
    
    /*
     * We take one arg which is the name of the a.out file for the checksum
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
     * read in the exec struction at the head of the file
     */
    nchars = read(desc, &header, sizeof(struct exec));
    if (nchars != sizeof(struct exec)) {
	fprintf(stderr, "Unable to read %s\n", filename);
	exit(1);
    }

    /*
     * we are only interested in the stuff in the .text section
     */
    textsize = header.a_text;
    if (header.a_machtype != CISCOTYPE) {
	fprintf(stderr, "Bad machine type 0x%x\n", header.a_machtype);
	exit(1);
    }

    /*
     * verify that the magic number is correct.
     */
    if (header.a_magic != CISCOMAGIC) {
	fprintf(stderr, "Bad machine number 0x%x (should be 0x%x)\n",
		header.a_magic, CISCOMAGIC);
	exit(1);
    }

    /*
     * go fetch the .text section into memory
     */
    text = malloc(textsize);
    if (text == NULL) {
	/*
	 * no space available
	 */
	fprintf(stderr, "failed to malloc %d bytes.\n", textsize);
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
     * calculate the number of words in the .text section
     */
    nwords = textsize/sizeof(unsigned long); /* number of words to checksum */
    textsum_image = dosum(text, nwords);	     /* do the checksum */

    /*
     * cleanup .text stuff
     */
    free(text);
    text = NULL;

    /*
     * Go and fetch the symbol table
     */

    /*
     */
    syms_offset = N_SYMOFF(header);
    syms_size   = header.a_syms;
    strs_offset = N_STROFF(header);
    lseek(desc, N_STROFF(header), 0);
    if (read(desc, &strs_size, sizeof(strs_size)) != sizeof(strs_size)) {
	fprintf(stderr, "cannot read string table size in %s\n", filename);
	exit(1);
    }

    /* Number of symbol entries in the file */
    symcount = syms_size / sizeof(struct nlist);
    totalsize = strs_size + syms_size;

    /* allocate space for symbol entries and string table */
    symbols_and_strings = malloc(totalsize);
    if (symbols_and_strings == NULL) {
	fprintf(stderr,
		"unable to maloc space (%dbytes) for symbols and string table\n",
		totalsize);
	exit(1);
    }

    /*
     *  position to syms and read it in followed by the strings
     */
    lseek(desc, syms_offset, 0);
    if (syms_size != read (desc, (char *) symbols_and_strings, syms_size)) {
	fprintf(stderr, "premature end of file in symbols of %s\n", filename);
	exit(1);
    }

    /*
     * get space for strings
     */
    strings = malloc(strs_size);
    if (strings == NULL) {
	fprintf(stderr,
		"unable to maloc space (%dbytes) for strings\n", strs_size);
	exit(1);
    }
    lseek(desc, strs_offset, 0);
    if (strs_size != read (desc, (char *) strings, strs_size)) {
	fprintf(stderr, "premature end of file in strings of %s\n", filename);
	exit(1);
    }
    
    /* Modify each symbol entry to point directly at the symbol name.
     * search for the stuff we want at the same time
     */
    {	
	struct nlist *p = symbols_and_strings;
	struct nlist *end = symbols_and_strings + symcount;

	for (; p < end; p++) {
	    /* A zero index means there is no string.  */
	    if (p->n_un.n_strx != 0) {
		if (p->n_un.n_strx > 0 && p->n_un.n_strx < strs_size) {
		    p->n_un.n_name = strings + p->n_un.n_strx;
		    if (strcmp(p->n_un.n_name, START) == 0) {
			start_entry = p;	
			start_addr = start_entry->n_value;
			print_symbol(start_entry);
			if (p->n_value != header.a_entry) {
			    fprintf(stderr, "%s 0x%x != 0x%x\n",
				    START, p->n_value, header.a_entry);
			}
		    }	
		    else if (strcmp(p->n_un.n_name, ENDTEXT) == 0) {
			etext_entry = p;
			print_symbol(etext_entry);
		    }	
		    else if (strcmp(p->n_un.n_name, ENDDATA) == 0) {
			edata_entry = p;
			print_symbol(edata_entry);
		    }
		    else if (strcmp(p->n_un.n_name, TEXTSUM_IMAGE) == 0) {
			textsum_image_entry = p;
			print_symbol(textsum_image_entry);
		    }
		}
		else {
		    fprintf(stderr, "invalid string table offset in %s\n",
			    filename);
		    exit(1);
		}
	    }
	}
    }
    
    if (textsum_image_entry == NULL) {
	fprintf(stderr, "symbol '%s' not found in file:\n %s\n",
		TEXTSUM_IMAGE, filename);
	exit(1);
    }
    if (debug) {
	if (start_entry && etext_entry) {
	    unsigned long cnt = etext_entry->n_value - start_entry->n_value;
	    printf("_etext(0x%x) - __start(0x%x) => 0x%x\n",
		   etext_entry->n_value, start_entry->n_value, cnt);
	    cnt /= sizeof(unsigned int);
	    if (nwords != cnt) {
		printf("checksum on %d words not the same as checksum on %d words\n",
		       nwords, cnt);
	    }
	    else {
		printf("checksum on %d words is the same as checksum on .text\n",
		       cnt);
	    }

	}

	printf(".text range 0x%x -> 0x%x\n",
	       start_addr, start_addr + header.a_text);

	printf(".data range 0x%x -> 0x%x\n",
	       edata_entry->n_value - header.a_data,
	       edata_entry->n_value);
    }

    /*
     * Work out the offset into the file for the checksum write
     *
     * Offset is the size of the a.out header, plus the text segment...
     */
    textsum_image_offset  = N_DATOFF(header);

    /*
     * ...plus the relative offset of the symbol into the data segment.
     */
    textsum_image_offset += (textsum_image_entry->n_value -
			     (edata_entry->n_value - header.a_data));

    /*
     * the textsum_image_offset must be inside of the .data section
     */
    if ((textsum_image_offset > N_DATOFF(header)) &&
	(textsum_image_offset > (N_DATOFF(header) + header.a_data))) {
	fprintf(stderr, "%s is not in the .data section\n", TEXTSUM_IMAGE);
	exit(1);
    }

    /*
     * read the old value of the sum as a sanity check
     */
    lseek(desc, textsum_image_offset, 0);
    {
	unsigned int old_sum;

	if (read(desc, &old_sum, sizeof(old_sum)) != sizeof(old_sum)) {
	    fprintf(stderr, "failed to read old checksum from file %s\n",
		    filename);
	    exit(1);
	}
	if ((old_sum != MAGIC_VALUE) && (old_sum != textsum_image)) {
	    fprintf(stderr, "old %s 0x%x failed sanity check\n",
		    TEXTSUM_IMAGE, old_sum);
	    exit(1);
	}
    }
    
    /*
     * time to write the new value of the sum
     */
    lseek(desc, textsum_image_offset, 0);
    if (write(desc, &textsum_image, sizeof(textsum_image)) !=
	sizeof(textsum_image)) {
	fprintf(stderr, "unabel to write checksum(.text) to file %s\n",
		filename);
	exit(1);
    }

    close(desc);		/* close the file ignore the error return */

    /*
     * be clean and free the dynamic space we used
     */
    free(strings);	
    strings = NULL;
    free(symbols_and_strings);
    symbols_and_strings = NULL;
    start_entry = textsum_image_entry = etext_entry = NULL;

    printf("Writing Checksum: %s has a text segment checksum of 0x%x\n",
	   filename, textsum_image);
    exit(0);
}
