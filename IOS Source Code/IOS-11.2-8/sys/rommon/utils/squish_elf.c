/* $Id: squish_elf.c,v 3.2 1995/11/17 18:48:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/squish_elf.c,v $
 *------------------------------------------------------------------
 * squish_elf.c
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Squish all of the wasted space out of the ELF file.
 *------------------------------------------------------------------
 * $Log: squish_elf.c,v $
 * Revision 3.2  1995/11/17  18:48:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** ELF file utility
** The primary purpose is to squish all of the wasted space
** out of the ELF file
** Secondarily, show what an ELF file is really comprised of.
**
** Usage: squish_elf [-hnvd:] <infile> [<outfile>]
** Options:
**     -h   dump out the headers of the infile when read
**     -n   do not squish the file (no outfile needed)
**     -v   verbose mode during squish
**     -d <num>  dump the contents of the infile data section
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "mon_elf.h"

char *scntypstr[] = {   /* array of section type strings */
    "Section header table entry unused (NULL)",
    "Program specific (private) data (PROGBITS)",
    "Link editing symbol table (SYMTAB)",
    "A string table (STRTAB)",
    "Relocation entries with addends (RELA)",
    "A symbol hash table (HASH)",
    "Information for dynamic linking (DYNAMIC)",
    "Information that marks file (NOTE)",
    "Section occupies no space in file (NOBITS)",
    "Relocation entries, no addends (REL)",
    "Reserved, unspecified semantics (SHLIB)",
    "Dynamic linking symbol table (DYNSYM)",
};
#define MAXSCNTYPSTR (sizeof(scntypstr) / sizeof(char *))
#define DUMPLINESIZ 32

extern void dismem(unsigned char *addr, int length, unsigned disaddr, int fldsize);

int
main(int argc, char *argv[])
{
    long magic;
    int i, infd, outfd, nscns, numread;
    int elfhdrsiz, curpos;
    char c;
    Elf32_Ehdr *ehp;
    Elf32_Phdr *php;
    Elf32_Shdr *shp;
    extern int optind;
    extern char *optarg;
    char *optstring = "hnvd:";
    char squish, showheaders, verbose;
    int dumpsection;
    char *infile, *outfile, *strdata;
    char *sectstart, *bufp, *sh_name;
    long fileptr, *offsetp, *offsetpstart;
    char errbuf[128];

    squish = 1;  /* default action */
    showheaders = verbose = 0;
    dumpsection = -1;
    strdata = (char *)0;
    while((i = getopt(argc,argv,optstring)) != EOF) {
	switch (i) {
	case 'h':
	    showheaders = 1;
	    break;
	case 'v':
	    verbose = 1;
	    break;
	case 'd':
	    dumpsection = atoi(optarg);
	    break;
	case 'n':  /* do not squish */
	    squish = 0;
	    break;
	default:
usage:
	    fprintf(stderr,"usage: %s [-%s] file [outfile]\n",
		    argv[0], optstring);
	    fprintf(stderr, "options:\n\
-h      show header information (before squish)\n\
-d <n>  dump section <n> data in hex\n\
-n      do not squish\n\
-v      verbose\n");
	    exit(1);
	}
    }
    if(argc - optind < 1) {
	fprintf(stderr, "no input file name specified\n");
	goto usage;
    }
    infile = argv[optind++];
    if(squish) {
	if(argc - optind != 1) {
	    fprintf(stderr, "need both an input and output file name\n");
	    goto usage;
	}
	outfile = argv[optind];
	if((outfd = open(outfile, O_TRUNC | O_CREAT | O_WRONLY,
			 S_IRUSR | S_IWUSR | S_IXUSR) ) < 0) {
	    sprintf(errbuf, "cannot open output file: %s", outfile);
	    syserr(errbuf);
	}
    }

    if((infd = open(infile, O_RDONLY)) < 0) {
	sprintf(errbuf, "cannot open input file: %s", infile);
	syserr(errbuf);
    }
    if((numread = read(infd, &magic, sizeof(magic))) != sizeof(magic)) {
	sprintf(errbuf, "trouble reading file magic number (numread: %d) on file: %s",
		numread, infile);
	syserr(errbuf);
    }

    if(magic != 0x7f454c46) {
	fprintf(stderr, "\nbad elf magic number (%#x) on file: %s\n",
		magic, infile);
	return(1);
    }

    elfhdrsiz = ELFEHDRSZ;  /* the default size */

    while(1) {
	if((ehp = (Elf32_Ehdr *)malloc(elfhdrsiz)) == NULL) {
	    sprintf(errbuf, "cannot malloc space for ELF header");
	    syserr(errbuf);
	}

	if(lseek(infd, 0, SEEK_SET) < 0) {  /* rewind the file to the beginning */
	    sprintf(errbuf, "cannot rewind file: %s", infile);
	    syserr(errbuf);
	}
	    
	/* Read the ELF header */
	if((numread = read(infd, ehp, elfhdrsiz)) != elfhdrsiz) {
	    sprintf(errbuf, "cannot read the ELF header");
	    syserr(errbuf);
	}

	if(ehp->e_ehsize == elfhdrsiz) break;  /* we are done */

	/* the ELF header must be bigger than expected */
	elfhdrsiz = ehp->e_ehsize;  /* get the true size */
	free(ehp);  /* release this memory */
    }


    if(ehp->e_type != ET_EXEC) {
	fprintf(stderr, "not an executable file\n");
	return(1);
    }

    nscns = ehp->e_shnum;

    if(showheaders) {
	printf("\nELF file header:\n\
ELF magic number: ");
	for(i=0; i<EI_NIDENT; i++) {
	    c = ehp->e_ident[i];
	    if(isalnum(c)) printf(" '%c'",c);
	    else printf(" %#x",c);
	}
printf("\nentry pt: %#x, program header file offset:%#x\n\
section header file offset: %#x, version: %#x, flags: %#x\n\
type: %#x, machine: %#x, ELF header size: %#x,\n\
program header table entry size: %#x\n\
program header table entry count: %d\n\
section header table entry size: %#x\n\
section header table entry count: %d\n\
section header string table index: %d\n",
ehp->e_entry, ehp->e_phoff, ehp->e_shoff, ehp->e_version, ehp->e_flags, ehp->e_type,
ehp->e_machine, ehp->e_ehsize, ehp->e_phentsize, ehp->e_phnum, ehp->e_shentsize,
ehp->e_shnum, ehp->e_shstrndx);
    }

    /* Check that we only have one program header */
    if(ehp->e_phnum > 1) {
	fprintf(stderr, "too many program headers (segments), can only handle one\n");
	return(1);
    }

    /* Seek to the the program header */
    if(lseek(infd, ehp->e_phoff, SEEK_SET) != ehp->e_phoff) {
	sprintf(errbuf, "file (program) header lseek error");
	syserr(errbuf);
    }

    if((php = (Elf32_Phdr *)malloc(ehp->e_phentsize)) == NULL) {
	sprintf(errbuf, "cannot malloc space for program header");
	syserr(errbuf);
    }

    /* Read the program header */
    if((numread = read(infd, php, ehp->e_phentsize)) != ehp->e_phentsize) {
	sprintf(errbuf, "cannot read the ELF program header");
	syserr(errbuf);
    }

    if(showheaders) {
	printf("\nELF program header:\n\
program segment type: %#x, segment flags: %#x\n\
segment file offset: %#x, segment virtual address: %#x\n\
segment physical address: %#x, segment size in file: %#x\n\
segment size in memory: %#x, segment alignment, file & memory: %#x\n",
php->p_type, php->p_flags, php->p_offset, php->p_vaddr, php->p_paddr,
php->p_filesz, php->p_memsz, php->p_align);
    }

    /* Seek to the the section headers */
    if(lseek(infd, ehp->e_shoff, SEEK_SET) != ehp->e_shoff) {
	sprintf(errbuf, "file (section) header lseek error");
	syserr(errbuf);
    }

    if((sectstart = (char *)malloc(ehp->e_shentsize * nscns)) == NULL) {
	sprintf(errbuf, "cannot malloc space for section headers");
	syserr(errbuf);
    }

    /* read in the section headers */
    shp = (Elf32_Shdr *)sectstart;
    for(i=0; i<nscns; i++, shp = (Elf32_Shdr *)((char *)shp + ehp->e_shentsize)) {
	if((numread = read(infd, shp, ELFSHDRSZ)) != ELFSHDRSZ) {
	    sprintf(errbuf, "bad read on ELF section header number %d, numread: %d",
		    i, numread);
	    syserr(errbuf);
	}

	if(shp->sh_type == SHT_STRTAB) {

	    /* 
	    ** this section contains our section name strings
	    ** read them in so we can use them
	    */

	    /* get a buffer */
	    if((strdata = (char *)malloc(shp->sh_size)) == NULL) {
		sprintf(errbuf, "cannot malloc space for section strings");
		syserr(errbuf);
	    }
	    /* remember where we are */
	    if((curpos = lseek(infd, 0L, SEEK_CUR)) < 0) {
		sprintf(errbuf, "bad lseek on ELF section header #%d", i);
		syserr(errbuf);
	    }
	    /* seek to section data */
	    if(lseek(infd, shp->sh_offset, SEEK_SET) != shp->sh_offset) {
		sprintf(errbuf, "bad lseek to strings section");
		syserr(errbuf);
	    }
	    /* and read in the strings */
	    if((numread = read(infd, strdata, shp->sh_size)) != shp->sh_size) {
		sprintf(errbuf, "bad read on strings section");
		syserr(errbuf);
	    }
	    /* seek back to where we were */
	    if(lseek(infd, curpos, SEEK_SET) != curpos) {
		sprintf(errbuf, "bad lseek from strings section");
		syserr(errbuf);
	    }
	}

	if(showheaders) {
	    if(!shp->sh_name) sh_name = "NULL";
	    else {
		if(strdata) sh_name = strdata + shp->sh_name;
		else sh_name = "????";
	    }

	    printf("\nELF section header number %d\n\
section name [%s], index in string tbl: %d\n\
miscellaneous section attributes: %#x\n\
section virtual addr at execution: %#x, size of section in bytes: %#x\n\
entry size if section holds table: %#x, (link) index of another section: %d\n\
additional section information: %#x, section file offset: %#x\n\
section alignment: %#x\n",
i, sh_name, shp->sh_name, shp->sh_flags, shp->sh_addr, shp->sh_size,
shp->sh_entsize, shp->sh_link, shp->sh_info, shp->sh_offset, shp->sh_addralign);

	    printf("type of section: ");
	    if(shp->sh_type < MAXSCNTYPSTR) printf("%s\n", scntypstr[shp->sh_type]);
	    else printf("%#x\n", shp->sh_type);

	    printf("section flags:\n");
	    if(shp->sh_flags & SHF_ALLOC) printf("    occupies mem during execution\n");
	    if(shp->sh_flags & SHF_WRITE) printf("    writable during execution\n");
	    if(shp->sh_flags & SHF_EXECINSTR) printf("    code\n");
	}
    }

    if(dumpsection >= 0) {  /* user wishes to dump a section */
	if(dumpsection >= nscns) {
	    fprintf(stderr, "cannot dump section #%d - section number out of range\n",
		    dumpsection);
	    return(1);
	}
	shp = (Elf32_Shdr *)(sectstart + (ehp->e_shentsize * dumpsection));
	if(shp->sh_type == SHT_NOBITS) {
	    sprintf(errbuf, "section #%d occupies no space in file (NOBITS)",
		    dumpsection);
	    syserr(errbuf);
	}
	if(lseek(infd, shp->sh_offset, SEEK_SET) != shp->sh_offset) {
	    sprintf(errbuf, "trouble seeking to file section #%d", dumpsection);
	    syserr(errbuf);
	}
	fileptr = 0;
	printf("\nDump of section #%d\n", dumpsection);
	while(fileptr < shp->sh_size) {  /* dump the section data */
	    if((numread = shp->sh_size - fileptr) > DUMPLINESIZ)
		numread = DUMPLINESIZ;
	    if((numread = read(infd, errbuf, numread)) <= 0) {
		sprintf(errbuf, "trouble reading input file section #%d (numread = %d)",
			dumpsection, numread);
		syserr(errbuf);
	    }
	    dismem((unsigned char *)errbuf,numread,fileptr,1);  /* display as bytes */
	    fileptr += numread;
	}
    }

    /*
    ** If we are not performing a squish operation we are done
    */
    if(!squish) {
	close(infd);
	return(0);
    }

    if(verbose) printf("Beginning squish operation ... \n");

    /* we will write the program header immediately following the ELF header */
    ehp->e_phoff = elfhdrsiz;
    /* and the section headers immediatly following that */
    ehp->e_shoff = ehp->e_phoff + ehp->e_phentsize;

    if(write(outfd, ehp, elfhdrsiz) != elfhdrsiz) {
	sprintf(errbuf, "trouble writing the ELF header");
	syserr(errbuf);
    }

    if(lseek(outfd, 0L, SEEK_CUR) != ehp->e_phoff) {
	fprintf(stderr, "file section miscalculation - start of program header\n");
	/*
	** we should probably clobber the output file here
	*/
	return(1);
    }

    /* update the fields in the program header */
    php->p_offset = php->p_align = 
	ehp->e_shoff + (nscns * ehp->e_shentsize);  /* file offset for section data */

    if(write(outfd, php, ehp->e_phentsize) != ehp->e_phentsize) {
	sprintf(errbuf, "trouble writing the ELF program header");
	syserr(errbuf);
    }

    if(lseek(outfd, 0L, SEEK_CUR) != ehp->e_shoff) {
	fprintf(stderr, "file section miscalculation - start of section headers\n");
	return(1);
    }

    /* process the section headers and write them out */
    fileptr = php->p_offset;  /* the start of section data in file */
    if((offsetpstart = (long *)malloc(sizeof(long) * nscns)) == NULL) {
	sprintf(errbuf, "cannot malloc space for offset pointers");
	syserr(errbuf);
    }
    offsetp = offsetpstart;
    for(i=0; i<nscns; i++, offsetp++) {
	shp = (Elf32_Shdr *)(sectstart + (ehp->e_shentsize * i));
	/*
	** We will copy all section headers but only the data from
	** those sections that occupy memory during execution
	*/
	if(shp->sh_type != SHT_NULL) {  /* don't touch these */
	    *offsetp = shp->sh_offset;  /* remember infile offset */
	    shp->sh_offset = fileptr;
	    if(shp->sh_flags & SHF_ALLOC ||  /* occupies memory during execution */
	       shp->sh_type & SHT_STRTAB) {  /* is a string table */
		if(shp->sh_type != SHT_NOBITS) {  /* section must occupy space in file */
		    fileptr += shp->sh_size;
		}
	    } else {
		shp->sh_size = 0;  /* clobber the size */
	    }
	} else *offsetp = 0;
	if(write(outfd, shp, ehp->e_shentsize) != ehp->e_shentsize) {
	    sprintf(errbuf, "trouble writing ELF section header #$d",i);
	    syserr(errbuf);
	}
    }

    /* now write the section data out */
    offsetp = offsetpstart;  /* reset back to beginning */
    if((bufp = (char *)malloc(BUFSIZ)) == NULL) {
	sprintf(errbuf, "cannot malloc space for file data buffer");
	syserr(errbuf);
    }
    for(i=0; i<nscns; i++, offsetp++) {
	shp = (Elf32_Shdr *)(sectstart + (ehp->e_shentsize * i));
	/*
	** Copy the data from those sections that now have a size
	*/
	if(verbose) {
	    printf("Processing section #%d of size %#x\n",
		   i, shp->sh_size);
	}
	if(shp->sh_type == SHT_NOBITS) {
	    if(verbose) {
		printf("* this section occupies no space in file [NOBITS] ... skipping\n");
	    }
	    continue;
	}
	if(shp->sh_type != SHT_NULL && shp->sh_size) {
	    /* seek to infile offset */
	    if(lseek(infd, *offsetp, SEEK_SET) != *offsetp) {
		sprintf(errbuf, "trouble seeking to file section #%d",i);
		syserr(errbuf);
	    }

	    /* check the outfile position */
	    if((curpos = lseek(outfd, 0L, SEEK_CUR)) != shp->sh_offset) {
		fprintf(stderr, "file section miscalculation - data section #%d\n",i);
		fprintf(stderr, "expected: %#x,  is: %#x\n",
			shp->sh_offset, curpos);
		return(1);
	    }

	    if(verbose) {
		printf("Moving section #%d of size %#x from offset %#x to %#x\n",
		       i, shp->sh_size, *offsetp, curpos);
	    }
		       
	    fileptr = 0;
	    while(fileptr < shp->sh_size) {  /* copy the section data */
		if((numread = shp->sh_size - fileptr) > BUFSIZ) numread = BUFSIZ;
		if((numread = read(infd, bufp, numread)) <= 0) {
		    sprintf(errbuf, "trouble reading input file section #%d (numread = %d)",
			    i, numread);
		    syserr(errbuf);
		}
		if(write(outfd, bufp, numread) != numread) {
		    sprintf(errbuf, "trouble writing output file section #%d",i);
		    syserr(errbuf);
		}
		fileptr += numread;
	    }
	}
    }
    close(infd);
    close(outfd);
    return(0);
}

int
syserr(char *msg)
{
    extern int errno, sys_nerr;
    extern char *sys_errlist[];

    fprintf(stderr, "SYSERR: %s\nerrno: %d", msg, errno);
    if(errno > 0 && errno < sys_nerr)
	fprintf(stderr, ", %s\n", sys_errlist[errno]);
    else fprintf(stderr, "\n");
    exit(1);
}

void
dismem(unsigned char *addr, int length, unsigned disaddr, int fldsize)
{
    int value = 0;
    unsigned char i, j, c, linepos;
    unsigned char *linend, *linestart;
    unsigned char asciistart = 0;
    unsigned char *end = (addr + length);  /* the end boundary */

    while(addr < end) {
	linepos = printf("%.6x  ",disaddr);  /* display the line address */
	linestart = addr;  /* save for ASCII representation */
	linend = (addr + 16);
	for(i=0; i<2; i++) {  /* display twice, as hex and as ascii */
	    addr = linestart;
	    while(addr < linend) {
		switch(fldsize) {
		case 1:
		    value = *addr++;
		    break;
		case 2:
		    value = *(unsigned short *)addr;
		    addr += 2;
		    break;
		case 4:
		    value = *(unsigned *)addr;
		    addr += 4;
		    break;
		}
		if(i == 0) linepos += printf("%.*x ",fldsize * 2,value);  /* hex */
		else {  /* ascii representation */
#ifdef LITTLE_ENDIAN
		    for(j=0; j<(fldsize * 8); j += 8) {
#else
		    for(j=(fldsize * 8); j;) {
			j -= 8;
#endif
			c = (value >> j);
			putchar((c >= ' ') && (c < 0x7f) ? c : '.');
		    }
		}
		if(addr >= end) break;
	    }
	    if(!asciistart) asciistart = linepos;  /* record start 1st time */
	    else while(linepos++ < asciistart) putchar(' ');  /* pad w/spaces */
	}
	putchar('\n');
	disaddr += 16;
    }
}
