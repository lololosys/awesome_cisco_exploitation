/* $Id: mksum_elf.c,v 3.2.58.2 1996/05/10 23:53:43 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/scripts/mksum_elf.c,v $
 *------------------------------------------------------------------
 * mksum_elf.c - Generate checksum of .text segment for
 * 		 MIPS ELF format files and update paddr fields
 *
 * January 1994, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mksum_elf.c,v $
 * Revision 3.2.58.2  1996/05/10  23:53:43  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.2.58.1  1996/03/21  23:45:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/11/22  21:42:38  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:50:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:43:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <fcntl.h>
#include <sys/param.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

#include "../h/elf.h"
#include "../../boot/cpu.h"
#include "../os/sum_magic_number.h"

#define Isspace(c)	((c == ' ') || (c == '\t') || (c=='\n') || (c=='\v') \
                         || (c == '\r') || (c == '\f'))

#define val(x)		#x
#define macval(x)	val(x)
#define TEXTSUM_IMAGE   macval(MAGIC_VARIABLE)
#define START           "_start"
#define ETEXT		"etext"

#define START_MASK	0x01
#define CHKSUM_MASK	0x02
#define ETEXT_MASK	0x04

#define MAX_PGM_HDR	16

static int debug = 0;		/* used for development */

Elf32_Ehdr elf_hdr;
Elf32_Phdr pgm_hdr, pgm_hdr_table[MAX_PGM_HDR];
Elf32_Shdr *sec_table;
Elf32_Shdr *sec_hdr;
char *str_table;
unsigned int start_symbol = 0;
unsigned int chksum_symbol = 0;
unsigned int etext_symbol = 0;

/*
 * CPU Type number (for e_machine)
 */
#if 	defined(SIERRA)
int cpu_type = CPU_SIERRA;
#endif

#if 	defined(RSP)
int cpu_type = CPU_RSP;
#endif

#if 	defined(RVIP) || defined(SVIP)
int cpu_type = CPU_VIP;
#endif

#if	defined(PREDATOR)
int cpu_type = CPU_C7100;
#endif

/*
 * str_to_ulong:
 * Convert ASCII string to long. The Solaris 2.X and SunOS 4.X library versions
 * of this routine (strtol()) are incompatible (Solaris 2.X truncates the
 * returned value to be a signed integer IE 0x80000000 --> 0x7fffffff).
 * Hence we use our own implementation.
 */
unsigned long
str_to_ulong (char *s, char **ptr, int base)
{
    unsigned long total = 0;
    unsigned digit;
    int radix;
    char *start = s;
    int did_conversion = 0;
    int overflow = 0;
    int minus = 0;
    unsigned long maxdiv, maxrem;

    if (s == NULL) {
	errno = ERANGE;
	if (!ptr)
	    *ptr = (char *) start;
	return 0L;
    }

    while (Isspace (*s))
	s++;
    
    if (*s == '-') {
	s++;
	minus = 1;
    } else if (*s == '+')
	s++;

    radix = base;
    if (base == 0 || base == 16) {
	/*
	 * try to infer radix from the string (assume decimal).
	 * accept leading 0x or 0X for base 16.
	 */
	if (*s == '0') {
	    did_conversion = 1;
	    if (base == 0)
		radix = 8;	/* guess it's octal */
	    s++;		/* (but check for hex) */
	    if (*s == 'X' || *s == 'x') {
		did_conversion = 0;
		s++;
		radix = 16;
	    }
	}
    }
    if (radix == 0)
	radix = 10;


    maxdiv = ULONG_MAX / radix;
    maxrem = ULONG_MAX % radix;

    while ((digit = *s) != 0) {
	if (digit >= '0' && digit <= '9' && digit < ('0' + radix))
	    digit -= '0';
	else if (radix > 10) {
	    if (digit >= 'a' && digit < ('a' + radix - 10))
		digit = digit - 'a' + 10;
	    else if (digit >= 'A' && digit < ('A' + radix - 10))
		digit = digit - 'A' + 10;
	    else
		break;
	} else
	    break;
	did_conversion = 1;
	if (total > maxdiv
	    || (total == maxdiv && digit > maxrem))
	    overflow = 1;
	total = (total * radix) + digit;
	s++;
    }

    if (overflow) {
	errno = ERANGE;
	if (ptr != NULL)
	    *ptr = (char *) s;
	return ULONG_MAX;
    }
    if (ptr != NULL)
	*ptr = (char *) ((did_conversion) ? (char *) s : start);
    return minus ? - total : total;
}

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

void
update_machine_field(Elf32_Ehdr *e)
{
    int nchars;

    if (cpu_type != 0) {
	e->e_machine = cpu_type;
    }
}

void
update_paddr_base(unsigned long base, Elf32_Ehdr *e, Elf32_Phdr *p)
{
    int i;
    unsigned long paddr_mask;

    for (paddr_mask=0, i=0; i<8; i++) {
	if (((base >> ((7-i)*4)) & 0x0f) != 0)
	    paddr_mask <<= 4;
	else
	    paddr_mask = (paddr_mask << 4) | 0x0f;
    }
    if ((base & ~paddr_mask) != base) {
	fprintf(stderr, "Failed to find mask for 0x%x, mask= 0x%x\n",
		base, paddr_mask);
	exit(1);
    }
    
    /*
     * Do the p_paddr field and the e_entry
     */
    e->e_entry = (e->e_entry & paddr_mask) + base;
    for (i=0; i<e->e_phnum; i++) {
	p[i].p_paddr = (p[i].p_vaddr & paddr_mask) + base;
    }
    if (debug) {
	fprintf(stderr, "paddr: rewrote e->e_entry to 0x%x\n",
		e->e_entry);
	for (i=0; i<e->e_phnum; i++) {
	    fprintf(stderr, "paddr: p%d, rewrote p_paddr to 0x%x\n",
		    i, p[i].p_paddr);
	}
    }
}

void
update_vaddr_base(unsigned long base, Elf32_Ehdr *e, Elf32_Phdr *p,
		  Elf32_Shdr *s, int desc)
{
    int i, nchars;
    unsigned long vaddr_mask;

    for (vaddr_mask=0, i=0; i<8; i++) {
	if (((base >> ((7-i)*4)) & 0x0f) != 0)
	    vaddr_mask <<= 4;
	else
	    vaddr_mask = (vaddr_mask << 4) | 0x0f;
    }
    if ((base & ~vaddr_mask) != base) {
	fprintf(stderr, "Failed to find mask for 0x%x, mask= 0x%x\n",
		base, vaddr_mask);
	exit(1);
    }
    
    /*
     * Do the p_vaddr field
     */
    for (i=0; i<e->e_phnum; i++) {
	p[i].p_vaddr = (p[i].p_vaddr & vaddr_mask) + base;
    }

    if (debug) {
	for (i=0; i<e->e_phnum; i++) {
	    fprintf(stderr, "vaddr: p%d, rewrote p_vaddr to 0x%x\n",
		    i, p[i].p_vaddr);
	}
    }
    for (i=0; i<e->e_shnum; i++) {

	if ((s[i].sh_type & SHT_LOPROC)) {
	    /*
	     * This section has a sh_type indicating processor specific
	     * semantics. This really confuses the rom monitor, so set the
	     * sh_type to SHT_NOBITS and set the size to zero
	     */
	    s[i].sh_type = SHT_NOBITS;
	    s[i].sh_size = 0;
	    if (debug) {
		printf("Secton %d: set sh_type to NOBITS and size to 0\n", i);
	    }
	}
	s[i].sh_addr = (s[i].sh_addr & vaddr_mask) + base;
	if (debug) {
	    printf("Section %d: rewrote sh_addr to 0x%x\n",
		   i, s[i].sh_addr);
	}
    }
    
    /*
     * Write back the section headers
     */
    nchars = lseek(desc, e->e_shoff, 0);
    if (nchars == -1) {
	fprintf(stderr, "Error, could not seek to section headers\n");
	exit(1);
    }
    
    for (i=0; i<e->e_shnum; i++) {
	nchars = write(desc, &s[i], sizeof(Elf32_Shdr));
	if (nchars != sizeof(Elf32_Shdr)) {
	    fprintf(stderr, "Error, failed to write header %d\n", i);
	    exit(1);
	}
    }
}

int main(int argc, char *argv[])
{
    int desc, nchars, nwords;
    unsigned long textsize, offset_to_text;
    unsigned int textsum_image = 0;
    unsigned long *text = NULL;
    char *program_name = argv[0];
    char filename[MAXPATHLEN];
    char *namep;
    int totalsize;
    int i, symbol_flag, pflag;
    int symbols_table_sec;
    unsigned long paddr_base;
    unsigned long vaddr_base;
    int do_checksum;
    int do_machine_type;
    unsigned int textsum_image_offset = 0;
    Elf32_Sym symbol;

    if (argc < 3) {
	/*
	 * No filename or command supplied on the command line
	 */
usage:
	fprintf(stderr, "usage: %s file [...]\n", argv[0]);
	fprintf(stderr, "  -sum         - checksum the file only\n");
	fprintf(stderr, "  -pbase 0x..  - relocate paddr's\n");
	fprintf(stderr, "  -machine     - rewrite e_machine\n");
	fprintf(stderr, "  -vbase 0x..  - relocate vaddr's\n");
	exit(1);
    }

    /*
     * save away program name and filename for later
     */
    program_name = argv[0];
    strcpy(filename, argv[1]);
    paddr_base = 0;
    vaddr_base = 0;
    do_checksum = 0;
    do_machine_type = 0;

    for (i=2; i<argc; i++) {
	if (!strcmp(argv[i], "-pbase")) {
	    i += 1;
	    paddr_base = str_to_ulong(argv[i], 0, 16);
	    if (i>=argc || !paddr_base) {
		goto usage;
	    }
	    continue;
	}
	if (!strcmp(argv[i], "-vbase")) {
	    i += 1;
	    vaddr_base = str_to_ulong(argv[i], 0, 16);
	    if (i>=argc || !vaddr_base) {
		goto usage;
	    }
	    continue;
	}
	if (!strcmp(argv[i], "-machine")) {
	    do_machine_type = 1;
	    continue;
	}
	if (!strcmp(argv[i], "-sum")) {
	    do_checksum = 1;
	    continue;
	}
	goto usage;
    }

    if (do_checksum) {
	if (vaddr_base || paddr_base || do_machine_type) {
	    goto usage;
	}
    }

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
    nchars = read(desc, &elf_hdr, sizeof(Elf32_Ehdr));
    if (nchars != sizeof(Elf32_Ehdr)) {
	fprintf(stderr, "Error: could not read ELF header\n");
	exit(1);
    }

    if (debug) {
	printf("ELF Header : \n");
	printf("e_type : 0x%x, e_machine : 0x%x, e_version : 0x%x\n",
	       elf_hdr.e_type, elf_hdr.e_machine, elf_hdr.e_version);
	printf("e_entry : 0x%x, e_phoff : 0x%x, e_shoff : 0x%x\n",
	       elf_hdr.e_entry, elf_hdr.e_phoff, elf_hdr.e_shoff);
	printf("e_flags : 0x%x, e_ehsize : 0x%x, e_phentsize : 0x%x\n",
	       elf_hdr.e_flags, elf_hdr.e_ehsize, elf_hdr.e_phentsize);
	printf("e_phnum : 0x%x, e_shentsize : 0x%x, e_shstrndx : 0x%x\n",
	       elf_hdr.e_phnum, elf_hdr.e_shentsize, elf_hdr.e_shstrndx);
	printf("e_shnum : 0x%x\n", elf_hdr.e_shnum);
    }

    if (elf_hdr.e_type != ET_EXEC) {
	fprintf(stderr, "Error, not an executable file\n");
	exit(1);
    }

    /*
     * Check for existance of program header
     */
    if (elf_hdr.e_phoff == 0 || elf_hdr.e_phnum == 0) {
	fprintf(stderr, "Error, no program header\n");
	exit(1);
    }
    if (elf_hdr.e_phnum >= MAX_PGM_HDR) {
	fprintf(stderr, "Error, too many program headers\n");
	exit(1);
    }

    /*
     * Read the program headers
     */
    nchars = lseek(desc, elf_hdr.e_phoff, 0);
    if (nchars == -1) {
	fprintf(stderr, "Error, failed to seek to program header\n");
	exit(1);
    }
    for (i=0, pflag = -1; i<elf_hdr.e_phnum; i++) {
	nchars = read(desc, &pgm_hdr_table[i], elf_hdr.e_phentsize);
	if (nchars != elf_hdr.e_phentsize) {
	    fprintf(stderr, "Error, failed to read program header\n");
	    exit(1);
	}
	
	if (debug) {
	    printf ("Program Header %d\n", i);
	    printf ("%s%s", "Num Type Offset    Vaddr     Paddr      ",
		    "   MemSz   FileSz  Flags\n");
	    printf ("%2d  %03x  %08x  %08x  %08x  %8x  %6x  %6x\n",
		    i, pgm_hdr_table[i].p_type, pgm_hdr_table[i].p_offset, 
		    pgm_hdr_table[i].p_vaddr, pgm_hdr_table[i].p_paddr, 
		    pgm_hdr_table[i].p_memsz, pgm_hdr_table[i].p_filesz,
		    pgm_hdr_table[i].p_flags);
	}
	if (((pgm_hdr_table[i].p_flags & (PF_X|PF_R)) == (PF_X|PF_R)) &&
	    pgm_hdr_table[i].p_vaddr == elf_hdr.e_entry) {

	    if (pflag != -1) {
		fprintf(stderr, "Error, multiple main program headers\n");
		exit(1);
	    }

	    if (debug) {
		fprintf(stderr, "Program header %d, p_flags = 0x%x\n",
			i, pgm_hdr_table[i].p_flags);
	    }
	    memcpy(&pgm_hdr, &pgm_hdr_table[i], sizeof(pgm_hdr));
	    pflag = i;
	}
    }

    if (pflag == -1) {
	fprintf(stderr, "Error, no main program header located\n");
	exit(1);
    }

    /*
     * go fetch the .text section into memory
     */
    textsize = pgm_hdr.p_filesz;
    text = malloc(textsize);
    if (text == NULL) {
	/*
	 * no space available
	 */
	fprintf(stderr, "failed to malloc %d bytes.\n", textsize);
	exit(1);
    }

    nchars = lseek(desc, pgm_hdr.p_offset, 0);
    if (nchars == -1) {
	fprintf(stderr, "Error, failed to seek to start of load segment\n");
	exit(1);
    }

    nchars = read(desc, text, textsize);
    if (nchars != textsize) {
	fprintf(stderr, "Error, failed to read load segment\n");
	exit(1);
    }

    nchars = lseek(desc, 0, 0);
    if (nchars == -1) {
	fprintf(stderr, "Error, failed to seek to file beginning\n");
	exit(1);
    }
    nchars = write(desc, &elf_hdr, sizeof(elf_hdr));
    if (nchars != sizeof(elf_hdr)) {
	fprintf(stderr, "Error, failed to rewrite ELF header\n");
	exit(1);
    }
    
    /*
     * Go and fetch the symbol table
     */
    sec_table = malloc(elf_hdr.e_shnum * sizeof(Elf32_Shdr));
    if (!sec_table) {
	fprintf(stderr, "Error, failed to malloc for section hdr table\n");
	exit(1);
    }
    nchars = lseek(desc, elf_hdr.e_shoff, 0);
    if (nchars == -1) {
	fprintf(stderr, "Error, could not seek to section headers\n");
	exit(1);
    }

    for (i=0; i<elf_hdr.e_shnum; i++) {
	nchars = read(desc, &sec_table[i], sizeof(Elf32_Shdr));
	if (nchars != sizeof(Elf32_Shdr)) {
	    fprintf(stderr, "Error, failed to read section header %d\n", i);
	    exit(1);
	}
    }

    symbols_table_sec = -1;
    for (i=0; i<elf_hdr.e_shnum; i++) {
	if (debug) {
	    printf("Section Header %d:\n", i);
	    printf("sh_type : 0x%x, sh_name : 0x%x, sh_flags : 0x%x\n",
		   sec_table[i].sh_type, sec_table[i].sh_name,
		   sec_table[i].sh_flags);
	    printf("sh_addr : 0x%x, sh_offset : 0x%x, sh_size : 0x%x\n",
		   sec_table[i].sh_addr, sec_table[i].sh_offset,
		   sec_table[i].sh_size);
	    printf("sh_link : 0x%x, sh_entsize : 0x%x\n", 
		   sec_table[i].sh_link, sec_table[i].sh_entsize);
	}
	if (sec_table[i].sh_type == SHT_SYMTAB) {
	    symbols_table_sec = i;
	}
    }

    /*
     * If we have been asked to update the physical address fields in
     * the program header, do it here
     */
    if (paddr_base) {
	update_paddr_base(paddr_base, &elf_hdr, pgm_hdr_table);
    }

    /*
     * If we have been asked to update the virtual address fields in
     * the program header, do it here
     */
    if (vaddr_base) {
	update_vaddr_base(vaddr_base, &elf_hdr, pgm_hdr_table, sec_table, 
			  desc);
    }

    /*
     * If we have been asked to update e_machine, do it here
     */
    if (do_machine_type) {
	update_machine_field(&elf_hdr);
    }

    if (paddr_base || vaddr_base || do_machine_type) {
	nchars = lseek(desc, 0, 0);
	if (nchars == -1) {
	    fprintf(stderr, "Error, failed to seek to file beginning\n");
	    exit(1);
	}
	nchars = write(desc, &elf_hdr, sizeof(elf_hdr));
	if (nchars != sizeof(elf_hdr)) {
	    fprintf(stderr, "Error, failed to rewrite ELF header\n");
	    exit(1);
	}

	nchars = lseek(desc, elf_hdr.e_phoff, 0);
	if (nchars == -1) {
	    fprintf(stderr, "Error, failed to seek to program header\n");
	    exit(1);
	}
	for (i=0; i<elf_hdr.e_phnum; i++) {
	    nchars = write(desc, &pgm_hdr_table[i], sizeof(pgm_hdr));
	    if (nchars != sizeof(pgm_hdr)) {
		fprintf(stderr, "Error, failed to rewrite program header\n");
		exit(1);
	    }
	}
    }

    if (!do_checksum) {
	if (paddr_base)
	    printf("Reassigned physical addresses using 0x%x as base\n",
		   paddr_base);
	
	if (vaddr_base)
	    printf("Reassigned virtual addresses using 0x%x as base\n",
		   vaddr_base);
	
	if (do_machine_type)
	    printf("Rewrote e_machine field in ELF header to %d\n", cpu_type);
	
	exit(0);
    }

    if (symbols_table_sec == -1) {
	fprintf(stderr, "Error, could not find symbol table section\n");
	exit(1);
    }

    /*
     * Read in the string and symbol table
     */
    symbol_flag = START_MASK | CHKSUM_MASK | ETEXT_MASK;
    sec_hdr = &(sec_table[ sec_table[symbols_table_sec].sh_link ]);
    str_table = malloc(sec_hdr->sh_size);
    if (!str_table) {
	fprintf(stderr, "Error, could not malloc string table space\n");
	exit(1);
    }
    nchars = lseek(desc, sec_hdr->sh_offset, 0);
    if (nchars == -1) {
	fprintf(stderr, "Error, could not seek to string table\n");
	exit(1);
    }
    nchars = read(desc, str_table, sec_hdr->sh_size);
    if (nchars != sec_hdr->sh_size) {
	fprintf(stderr, "Error, could not read string table\n");
	exit(1);
    }

    sec_hdr = &sec_table[symbols_table_sec];
    nchars = lseek(desc, sec_hdr->sh_offset, 0);
    if (nchars == -1) {
	fprintf(stderr, "Error, could not seek to symbol table\n");
	exit(1);
    }
    
    for (i=0; i< (sec_hdr->sh_size / sec_hdr->sh_entsize); i++) {

	if (!symbol_flag)
	    break;

	/*
	 * Read the symbol
	 */
	nchars = read(desc, &symbol, sec_hdr->sh_entsize);
	if (nchars != sec_hdr->sh_entsize) {
	    fprintf(stderr, "Error, failed to read symbol %d\n", i);
	    exit(1);
	}
	namep = str_table + symbol.st_name;
	
	if ((strcmp(namep, START) == 0) && (symbol_flag & START_MASK)) {
	    symbol_flag &= ~START_MASK;
	    start_symbol = symbol.st_value;
	    if (debug) {
		fprintf(stderr, "_start : 0x%x\n", start_symbol);
	    }
	} else if ((strcmp(namep, TEXTSUM_IMAGE) == 0) 
		   && (symbol_flag & CHKSUM_MASK)) {
	    symbol_flag &= ~CHKSUM_MASK;
	    chksum_symbol = symbol.st_value;
	    if (debug) {
		fprintf(stderr, "chksum: 0x%x\n", chksum_symbol);
	    }
	} else if ((strcmp(namep, ETEXT) == 0) 
		   && (symbol_flag & ETEXT_MASK)) {
	    symbol_flag &= ~ETEXT_MASK;
	    etext_symbol = symbol.st_value;
	    if (debug) {
		fprintf(stderr, "etext: 0x%x\n", etext_symbol);
	    }
	} 
    }	
    
    if (symbol_flag) {
	fprintf(stderr, "Failed to find all required symbols\n");
	exit(1);
    }

    /*
     * calculate the number of words in the .text section
     */
    offset_to_text = (start_symbol - pgm_hdr.p_vaddr) 
	/ sizeof(unsigned long);
    nwords = (etext_symbol - start_symbol);
    nwords = nwords / sizeof(unsigned long);

    textsum_image = dosum(text + offset_to_text, nwords);

    if (debug) {
	fprintf(stderr, "textsum_image: 0x%x, pgm_hdr.p_vaddr: 0x%x\n", 
		textsum_image, pgm_hdr.p_vaddr);
	fprintf(stderr, "offset_to_text: 0x%x, nwords : 0x%x\n",
		offset_to_text, nwords);
    }

    /*
     * cleanup .text stuff
     */
    free(text);
    text = NULL;

    /*
     * re-write the value for the magic variable
     */
    textsum_image_offset = (chksum_symbol - pgm_hdr.p_vaddr + 
			    pgm_hdr.p_offset);
    nchars = lseek(desc, textsum_image_offset, 0);
    if (nchars == -1) {
	fprintf(stderr, "Error, could not seek to checksum variable\n");
	exit(1);
    }
    nchars = write(desc, &textsum_image, sizeof(unsigned long));
    if (nchars != sizeof(unsigned long)) {
	fprintf(stderr, "Error, could not re-write checksum variable\n");
	exit(1);
    }
    close(desc);
    
    printf("Writing Checksum: %s has a text segment checksum of 0x%x\n",
	   filename, textsum_image);
    exit(0);
}


