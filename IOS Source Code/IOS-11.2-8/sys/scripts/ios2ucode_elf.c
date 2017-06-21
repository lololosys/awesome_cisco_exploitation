/* $Id: ios2ucode_elf.c,v 3.4 1996/01/03 01:07:51 foster Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/scripts/ios2ucode_elf.c,v $
 *------------------------------------------------------------------
 * ios2ucode_elf.c - Generate UCODE file from elf format IOS image
 *
 * March 1995, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ios2ucode_elf.c,v $
 * Revision 3.4  1996/01/03  01:07:51  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 * MAX_SECTIONS defined twice. set it back to 30.
 *
 * Revision 3.3  1995/12/28  08:23:57  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 * Allow for non-compressed images (for debugging).
 *
 * Revision 3.2  1995/11/17  18:49:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:43:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Steps :
 *
 * Convert VPLD ELF file to binary format (if present)
 * Convert ELF IOS executable to binary format
 * Concatenate the two
 * Compress the resulting file
 * Add WCS initial commands
 * Make UCODE bundle
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>

#include "../h/elf.h"

/*
 * Globals
 */
int 		debug = 0;
int 		hw_type = 0;
int             comp_type = 0;

unsigned short 	req_hw_rev = 0;
unsigned short 	fw_rev = 0;

unsigned int 	vpld_ver_maj, vpld_ver_min;
unsigned int 	ios_ver_maj, ios_ver_min;
unsigned int 	entry;

char 		*vpld_version_string = (char *)0;
char 		*ios_version_string = (char *)0;
char 		*ios_input_filename = (char *)0;
char 		*ucode_output_filename = (char *)0;
char 		*vpld_filename = (char *)0;
char            *hw_string = (char *)0;
char            *compression_type = (char *)0;

/*
 * Externals
 */
extern char *optarg;
extern int optind;

/*
 * Defines
 */
#define OUTPUT_DIMAGE     1
#define OUTPUT_VPLD       2
#define OUTPUT_DBUS       3

#define DLU_HDR_SKIP	  0x85
#define DLU_HDR_VER	  1

/* compression types */
#define NONE 1
#define UNIX 2
#define ZIP  3

#define MAX_PHDRS          6
#define MAX_SECTIONS	  30

#define TRUE   1
#define FALSE  0

typedef int boolean;


/*
 * typedefs
 */
typedef struct {
    unsigned char skip;
    unsigned char hdr_ver;
    unsigned short fw_rev;
    unsigned short req_hw_rev;
    unsigned char hw_type;
    unsigned char desc[25];
} dlu_hdr_t;

typedef struct {
    unsigned short crc;
} dlu_trlr_t;

typedef struct INPUT_SEC {
    unsigned long base_addr;
    unsigned int size;
    unsigned char *ptr;
    boolean load;
} INPUT_SEC;

INPUT_SEC psec[MAX_PHDRS];

/*
 * Names
 */
static char *cardnames[] = {
    "EIP",			/*  0 */
    "Old FDDI (not used)",	/*  1 */
    "SIP",			/*  2 */
    "HIP",			/*  3 */
    "DTR (not used)",		/*  4 */
    "TRIP",			/*  5 */
    "FIP",			/*  6 */
    "FSIP",			/*  7 */
    "AIP",			/*  8 */
    "MIP",			/*  9 */
    "SSP",			/* 10 */
    "SP",			/* 11 */
    "RP (not used)",		/* 12 */
    "CIP",			/* 13 */
    "RSP1",			/* 14 */
    "RSP2",			/* 15 */
    "CI",			/* 16 */
    "CTR_SRS",			/* 17 */
    "FDDIT_SRS",		/* 18 */
    "FEIP",			/* 19 */
    "VIP",			/* 20 */
    "VIP2"			/* 21 */
};

/*
 * ipcrc
 * Return CRC checksum for the specified chunk of data
 * Used by much more than just the IP code.
 */
unsigned short longcrc (unsigned char *ptr, unsigned int count)
{
    unsigned long long crc;
    unsigned short ret, *data;
    unsigned int j;

    crc = 0;                 		/* starting checksum */
    data = (unsigned short *) ptr;   	/* point at data to check */

    /* First, check if there is an odd number of bytes.  If so, get
       the last byte, shift it left 8 and add it in */

    if(count %2) {    /* odd number to add */
	/* Add in last byte, then decrement length */
	crc = ((unsigned short)(0xff & *((char *) data + (count - 1))) << 8);
    }

    count >>=  1;  /* convert to length in shorts*/

    for(j = 0; j < count; j++) {
	crc += (unsigned long long) *data;  /* 2's complement add 
					       the next header
					       word*/
	data++;
    }

     /* Add once to get first overflow */
    crc = (crc & 0xFFFF) + ((crc >> 16) & 0xffff) +
	  ((crc >> 32) & 0xffff) +((crc >> 48) & 0xffff) ;

    /* Add again to catch overflows caused by above */

    crc += crc >> 16;
    ret = crc;
    return (~ret & 0xffff);
}

/****************************************************************** 
 * add_to_cksum(new_word, old_cksum);
 * Accumulates a checksum by adding the new word the old checksum, with
 * end-around carry.  
 * Returns the new checksum value.
 ******************************************************************/
unsigned short add_to_cksum(unsigned short new_word, 
			    unsigned short old_cksum)
{
    unsigned long work;               /* work value for end-around add */
    work = old_cksum + new_word;      /* add new word to checksum */
    if (work & ~0xFFFF) 
        work++;	                      /* end-around carry */   
    return(work & 0xFFFF);
}

unsigned short update_cksum(unsigned short cksum, unsigned char *wp, 
			    unsigned long i)
{
    if ( (unsigned long)wp & 0x01 ) {
        cksum = add_to_cksum(*wp++, cksum);
        i--;
    }

    for ( ; i > 1; i -= sizeof(unsigned short)) 
	cksum = add_to_cksum(*((unsigned short *)wp)++, cksum);
    
    if (i == 1) 
	cksum = add_to_cksum(*wp << 8, cksum);
    return(cksum);
}


/*
 * set_load_flag : search for a program section that includes the
 *                 input section, if found mark the load flag.
 */
int set_load_flag (Elf32_Shdr *sh)
{
    int i;
    INPUT_SEC *ph;

    for (i=0; i<MAX_PHDRS; i++) {
	ph = &psec[i];
	if (!ph->ptr) {
	    continue;  /* empty, move on */
	}
	if (sh->sh_addr >= ph->base_addr 
	    && sh->sh_addr <= ph->base_addr+ph->size) {
	    /* mark this program header as one we want to load */
	    ph->load = TRUE;
	    return i;
	}
    }
    return -1;
}

/*
 * process_elf : Process an ELF file
 */
void process_elf (FILE *inf, FILE *outf, int outtype, 
		  int maj_version, int min_version)
{
    int i, j, temp, sec;
    Elf32_Ehdr filehdr;
    Elf32_Phdr phdr[MAX_PHDRS], *phptr;
    Elf32_Shdr scnhdr[MAX_SECTIONS], *shptr;
    unsigned short crc;

    char buff_cmds[2] = {0x10, 0x25};
    char buffer[6]    = {0, 0, 0, 0, 0, 0};

    /* Initialize program sections */
    for (i=0; i < MAX_PHDRS; i++) {
	psec[i].ptr = 0;
	psec[i].load = FALSE;
    }

    /* Read the Elf file header */
    temp = fread(&filehdr, 1, sizeof(Elf32_Ehdr), inf);
    if (temp != sizeof(Elf32_Ehdr)) {
	fprintf(stderr, "\nError: cannot read ELF header\n");
	exit(1);
    }
    if (filehdr.e_ident[0] != ELFMAG0 ||
	filehdr.e_ident[1] != ELFMAG1 ||
	filehdr.e_ident[2] != ELFMAG2 ||
	filehdr.e_ident[3] != ELFMAG3) {
	fprintf(stderr, "\nError: Improper ELF Magic Number\n");
	exit(1);
    }

    if (debug) {
	fprintf(stderr, "Ehdr: ph_num %d, sh_num %d, entry %x\n",
		filehdr.e_phnum, filehdr.e_shnum, filehdr.e_entry);
    }

    if (filehdr.e_phnum > MAX_PHDRS) {
	fprintf(stderr, "\nError: too many program sections in input file, %d\n", 
		filehdr.e_phnum);
	exit(1);
    }
    if (filehdr.e_shnum > MAX_SECTIONS) {
	fprintf(stderr, "\nError: too many sections in input file, %d\n", 
		filehdr.e_shnum);
	exit(1);
    }
    entry = filehdr.e_entry;

    /* 
     * read in all program headers 
     */
    if (fseek(inf, filehdr.e_phoff, 0) != 0) {
	fprintf(stderr, "\nError: failed to seek to e_phoff %x\n",
		filehdr.e_phoff);
	exit(1);
    }
    for (i=0; i<filehdr.e_phnum; i++) {
	if (fread(&phdr[i], 1, ELFPHDRSZ, inf) != ELFPHDRSZ) {
	    fprintf(stderr,
		    "\nError: cannot read ELF program header, %d\n", i);
	    exit(1);
	}
	if (debug) {
	    Elf32_Phdr *p = &phdr[i];
	    fprintf(stderr, 
		    "Phdr %d: p_type %x, p_offset %x, p_vaddr %x, p_filesz %x\n",
		    i+1, p->p_type, p->p_offset, p->p_vaddr, p->p_filesz);
	}
    }
    /*
     * read in all the data for each program header
     */
    for (i=0, j=0; i < filehdr.e_phnum; i++) {
	phptr = &phdr[i];
	if (phptr->p_type == PT_LOAD) {
	    psec[j].base_addr = phptr->p_vaddr;
	    psec[j].size = phptr->p_filesz;
	    if (phptr->p_offset) {
		psec[j].ptr = (unsigned char *)malloc(phptr->p_filesz);
		if (!psec[i].ptr) {
		    fprintf(stderr, "\nError: malloc failed\n");
		    exit(1);
		}
		if (fseek(inf, phptr->p_offset, 0) != 0) {
		    fprintf(stderr,
			    "\nError: failed to seek to program section %d\n", i);
		    exit(1);
		}
		if (fread(psec[i].ptr, 1, psec[i].size, inf) != psec[i].size) {
		    fprintf(stderr, 
			    "\nError: failed to read program section %d\n", i);
		    exit(1);
		}
	    }
	    j++; 
	}
    }

    /*
     * read in each section header
     */
    if (fseek(inf, filehdr.e_shoff, 0) != 0) {
	fprintf(stderr, "\nError: failed to seek to section offset %d\n",
		filehdr.e_shoff);
	exit(1);
    }
    for (i=0; i<filehdr.e_shnum; i++) {
	if (fread(&scnhdr[i], 1, ELFSHDRSZ, inf) != ELFSHDRSZ) {
	    fprintf(stderr, "\nError: cannot read ELF section %d header\n", i);
	    exit(1);
	}
	if (debug) {
	    Elf32_Shdr *s = &scnhdr[i];
	    fprintf(stderr, 
		    "Shdr %d, sh_type %x, sh_offset %x, sh_addr %x, sh_size %x\n",
		    i+1, s->sh_type, s->sh_offset, s->sh_addr, s->sh_size);
	}
    }
    /*
     * Check the section headers looking for sections that we want to
     * download.  The reason for this extra step is that the 95q1
     * compiler adds a program header that severely confuses the vip
     * rom monitor.  This extra program header has a section whos
     * sh_type has SHT_LOPROC anded with the it.  
     * 
     * For each section that we want to keep, find the corresponding
     * program header and mark it.  Then later we'll load only the
     * program headers that are marked.  This kludge only works as
     * long as the compiler/linker keeps the SHT_LOPROC section in
     * it's own program section. 
     */
    if (debug) 
	fprintf(stderr, "Scanning sections:\n");
    for (i=0; i < filehdr.e_shnum; i++) {
	shptr = &scnhdr[i];
	if (shptr->sh_type != SHT_PROGBITS) {
	    if (debug) {
		fprintf(stderr, "   skipping section %d, sh_type %x", 
			i+1, shptr->sh_type);
		if (shptr->sh_type & SHT_LOPROC) {
		    /*
		     * this is the section that 95q1 is placing in a
		     * program head and causing greif for the rom
		     * monitor. 
		     */
		    fprintf(stderr, " (SHT_LOPROC)");
		}
		fprintf(stderr, "\n");
	    }
	    continue;
	}
	if (shptr->sh_size == 0) {
	    if (debug) {
		fprintf(stderr, "   skipping section %d, sh_size %x\n", 
			i+1, shptr->sh_size);
	    }
	    continue;
	}
	if (shptr->sh_offset) {
	    j = set_load_flag(shptr);
	    if (j < 0) {
		/* 
		 * we should never get here because this section is
		 * supposed to be loaded, but we couldn't find an
		 * appropriate program header for it.
		 */
		printf("\npanic: couldn't find section in program header\n");
		exit(1);
	    }
	    if (debug) {
		fprintf(stderr, "   keeping section %d in program header %d\n",
			i+1, j+1);
	    }
	}
    }

    /*
     * Write out the data to outf
     */
    switch (outtype) {

    case OUTPUT_VPLD:
	buffer[2] = 1; 
	break;

    case OUTPUT_DBUS:
	buffer[2] = 2; 
	break;

    default:
	buffer[2] = 0;
	break;
    }
    buffer[4] = maj_version;
    buffer[5] = min_version;
    
    if (debug) {
	fprintf(stderr, "Writing program sections:\n");
    }

    for (sec = 0; sec < MAX_PHDRS; sec++) {
	if ( !(psec[sec].ptr) || (psec[sec].size == 0)) {
	    continue;
	}
	
	if (psec[sec].load != TRUE) {
	    if (debug) { 
		fprintf(stderr, "   skipping program section %d\n", sec+1);
	    }
	    continue;
	}
	if (debug) {
	    fprintf(stderr, "   writing program section %d\n", sec+1);
	}
	    

	temp = fwrite(buff_cmds, 1, 2, outf);
	if (temp != 2) {
	    fprintf(stderr, "\nError: cannot write buff_cmds\n");
	    exit(1);
	}
	temp = fwrite(&psec[sec].base_addr, 1, 4, outf);
	if (temp != 4) {
	    fprintf(stderr, "\nError: cannot write base_addr to outfile\n");
	    exit(1);
	}
	temp = fwrite(&psec[sec].size, 1, 4, outf);
	if (temp != 4) {
	    fprintf(stderr, "\nError: cannot write size to outfile\n");
	    exit(1);
	}
	temp = fwrite(&entry, 1, 4, outf);
	if (temp != 4) {
	    fprintf(stderr, "\nError: cannot write entry to outfile\n");
	    exit(1);
	}
	temp = fwrite(buffer, 1, 6, outf);
	if (temp != 6) {
	    fprintf(stderr, "\nError: cannot write entry to outfile\n");
	    exit(1);
	}
	temp = fwrite(psec[sec].ptr, 1, psec[sec].size, outf);
	if (temp != psec[sec].size) {
	    fprintf(stderr, "\nError: cannot write data to outfile\n");
	    exit(1);
	}
	crc = longcrc((unsigned char *)psec[sec].ptr, 
		      (unsigned int)psec[sec].size);
	temp = fwrite(&crc, 1, 2, outf);
	if (temp != 2) {
	    fprintf(stderr, "\nError: cannot write crc to outfile\n");
	    exit(1);
	}
    }
}

/*
 * process_addit :
 */
void process_addit (FILE *fin, FILE *fout)
{
    int length, temp, nread, infd;
    char buff_cmds[2] = {0x10, 0x25};  /* initial WCS commands */
    char buffer[6] = {0,0,0,0,0,0};
    char *image;
    struct stat stat_buf;
    unsigned short crc;

    infd = fileno(fin);
    fstat(infd, &stat_buf);
    length = stat_buf.st_size;
    
    if ((image = (char *) malloc(length)) == NULL) {
	fprintf(stderr, "\nError: malloc failed\n");
	exit(1);
    }

    if ((nread = fread(image, 1, length, fin)) != length) {
	fprintf(stderr, "\nError: failed to read input data\n");
	exit(1);
    }

    buffer[1] = 1;  /* compress flag */
    if (fwrite(buff_cmds, 1, 2, fout) != 2) {
	fprintf(stderr, "\nError: cannot write buff_cmds to fout\n");
	exit(1);
    }
    if (fwrite(&length, 1, 4, fout) != 4) {
	fprintf(stderr, "\nError: cannot write length0 to fout\n");
	exit(1);
    }
    if (fwrite(&length, 1, 4, fout) != 4) {
	fprintf(stderr, "\nError: cannot write length1 to fout\n");
	exit(1);
    }
    if (fwrite(&length, 1, 4, fout) != 4) {
	fprintf(stderr, "\nError: cannot write length2 to fout\n");
	exit(1);
    }
    if (fwrite(buffer, 1, 6, fout) != 6) {
	fprintf(stderr, "\nError: cannot write buffer to fout\n");
	exit(1);
    }
    if (fwrite(image, 1, length, fout) != length) {
	fprintf(stderr, "\nError: cannot write data to fout\n");
	exit(1);
    }
    crc = longcrc((unsigned char *)image,(unsigned int)length);
    if( fwrite(&crc, 1, 2, fout) != 2) {         /* space for cksum */
	fprintf(stderr, "\nError: cannot write buff_cmds to fout\n");
	exit(1);
    }
}

/*
 * process_mkdlx :
 */
void process_mkdlx (FILE *inf, FILE *outf, unsigned short req_hwrev,
		    unsigned short fwrev, int hwtype)
{
    char 		*op, *outbuf;
    struct stat		stat_buf;
    int			infd, size, byte, lastbyte, fsize;
    dlu_hdr_t		*header;
    dlu_trlr_t		*trailer;
    unsigned short	crc;

    infd = fileno(inf);
    fstat(infd, &stat_buf);
    fsize = stat_buf.st_size + 0x10000;
    outbuf = (char *)malloc(fsize);
    if (!outbuf) {
	fprintf(stderr, "\nError : malloc failed\n");
	exit(1);
    }

    op = outbuf + sizeof(dlu_hdr_t);
    size = byte = lastbyte = 0;

    size = fread(op, 1, stat_buf.st_size, inf);
    if (size != stat_buf.st_size) {
	fprintf(stderr, "\nError: failed to read all of inf\n");
	exit(1);
    }
    
    if (debug) {
	fprintf(stderr, "mkdlx: read 0x%x bytes\n", size);
    }

    if (size & 0x1) {
	outbuf[sizeof(dlu_hdr_t) + size++] = 0;	/* force padding to
				    short alignment, but make sure
				    that you add the header in */
	if (debug) {
	    fprintf(stderr, "mkdlx: size rounded up to 0x%x\n",
		    size);
	}
    }

    header = (dlu_hdr_t*)outbuf;
    header->skip = DLU_HDR_SKIP;
    header->hdr_ver = DLU_HDR_VER;
    header->fw_rev = fwrev;
    header->req_hw_rev = req_hwrev;
    header->hw_type = hwtype;

    sprintf(header->desc,"%s version %d.%d",cardnames[header->hw_type],
	    header->fw_rev >> 8, header->fw_rev & 0xff);

    trailer = (dlu_trlr_t*)(outbuf+size+sizeof(dlu_hdr_t));
    trailer->crc = 0;
    crc = 0;
    crc = update_cksum(crc,outbuf,size+sizeof(dlu_hdr_t)+sizeof(dlu_trlr_t));

    trailer->crc = ~crc;
    fwrite(outbuf, 1, size+sizeof(dlu_hdr_t)+sizeof(dlu_trlr_t), outf);
}

/*
 * usage :
 */
void usage (void)
{
    fprintf(stderr, "\nusage : \n");
    fprintf(stderr, "-v <ios version> \n");
    fprintf(stderr, "-V <VPLD version> \n");
    fprintf(stderr, "-i <ios input file> \n");
    fprintf(stderr, "-f <VPLD input file> \n");
    fprintf(stderr, "-o <output UCODE file> \n");
    fprintf(stderr, "-d - Debug \n");
    fprintf(stderr, "-m <required hw rev> \n");
    fprintf(stderr, "-t <hw type> \n");
    fprintf(stderr, "-h <string> \n");
    fprintf(stderr, "-C <[none] | [unix] | zip> \n");
    exit(1);
}

/*
 * process_args :
 */
void process_args (int argc, char *argv[])
{
    int c, maj, min;
    char argbuf[80];

    while ((c = getopt(argc, argv, "h:v:V:i:f:o:dm:t:C:")) != -1) {

	switch (c) {

	case 'v': 
	    ios_version_string = optarg; 
	    break;
	case 'V': 
	    vpld_version_string = optarg; 
	    break;
	case 'i': 
	    ios_input_filename = optarg; 
	    break;
	case 'f': 
	    vpld_filename = optarg; 
	    break;
	case 'o': 
	    ucode_output_filename = optarg;
	    break;
	case 'd': 
	    debug = 1; 
	    break;
	case 'm': 
	    strcpy(argbuf, optarg);
	    if (sscanf(argbuf, "%d.%d", &maj, &min) == 2) {
		req_hw_rev = maj<<8 | min;
	    } else {
		usage();
	    }
	    break;
	case 't':		/* set hw type */
	    strcpy(argbuf, optarg);
	    if (sscanf(optarg, "%d", &hw_type) != 1) {
		usage();
	    }
	    break;
	case 'h':
	    hw_string = optarg;
	    break;
	  case 'C':
	    compression_type = optarg;
	    if (!strcmp(compression_type, "none")){
		comp_type = NONE;
	    } else {
		if (!strcmp(compression_type, "unix")) {
		    comp_type = UNIX ;
		} else {
		    if (!strcmp(compression_type, "zip")) {
			comp_type = ZIP;
		    } else {
			usage();
		    }
		}
	    }
	    break;
	default:
	    usage();
	    break;
	}
    }
}

/*
 * main:
 */
void main (int argc, char *argv[])
{
    FILE *iosf, *bf, *bfz, *uf, *vf;
    int temp;
    char bundle_filename[80];
    char cmd_buf[256];

    /*
     * Process command line arguments
     */
    process_args(argc, argv);

    /*
     * Check files
     */
    if (!ios_input_filename || !ios_version_string) {
	usage();
    }
    iosf = fopen(ios_input_filename, "r");
    if (!iosf) {
	fprintf(stderr, "\nError: failed to open %s for reading\n",
		ios_input_filename);
	exit(1);
    }
    if (!ucode_output_filename) {
	usage();
    }
    sprintf(bundle_filename, "%s.bundle", ucode_output_filename);
    bf = fopen(bundle_filename, "w");
    if (!bf) {
	fprintf(stderr, "\nError: failed to open %s for writing\n",
		bundle_filename);
	exit(1);
    }

    /*
     * Check that hw_string is set
     */
    if (!hw_string) {
	usage();
    }

    /*
     * Check if we have VPLD
     */
    if (vpld_filename) {
	vf = fopen(vpld_filename, "r");
	if (!vf) {
	    fprintf(stderr, "\nError: failed to open %s for reading\n",
		    vpld_filename);
	    exit(1);
	}
	if (!vpld_version_string) {
	    usage();
	}
	if (sscanf(vpld_version_string, "%d.%d", &vpld_ver_maj, 
		   &vpld_ver_min) != 2) {
	    usage();
	}
	/* process VPLD file */
	process_elf(vf, bf, OUTPUT_VPLD, vpld_ver_maj, vpld_ver_min);
    }

    /*
     * Print out the hw we are building for
     */
    printf("Creating IOS microcode bundle, RVIP_HARDWARE = %s\n",
	   hw_string);

    /*
     * Process the IOS image
     */
    if (sscanf(ios_version_string, "%d.%d", &ios_ver_maj, &ios_ver_min) != 2) {
	usage();
    }
    /* process VPLD file */
    process_elf(iosf, bf, OUTPUT_DIMAGE, ios_ver_maj, ios_ver_min);

    fclose(bf);

    if( comp_type == 0 )
	comp_type = UNIX;         /* default to unix compress */

    switch (comp_type) {
      case UNIX:        /* compress the image */
	sprintf(cmd_buf, "compress -c -b 16 %s > %s.Z",
		bundle_filename, bundle_filename);
	printf("Compressing %s to %s.Z using compress\n",
	       bundle_filename, bundle_filename);
	temp = system(cmd_buf);
	if (temp != 0) {
	    fprintf(stderr, 
		    "\nError: system call to compress failed command was: %s\n"
		    , cmd_buf);
	    exit(1);
	}
	/* addit : add initial WCS commands (WCS header) */

	sprintf(cmd_buf, "%s.Z", bundle_filename);
	bfz = fopen(cmd_buf, "r");
	if (!bfz) {
	    fprintf(stderr, "\nError : failed to open %s for reading\n",
		    cmd_buf);
	    exit(1);
	}
	bf = fopen(bundle_filename, "w");
	if (!bf) {
	    fprintf(stderr, "\nError: failed to open %s for writing\n",
		    bundle_filename);
	    exit(1);
	}

	process_addit(bfz, bf);

	fclose(bfz);
	fclose(bf);
	break;

      case NONE:
	break;
   
      case ZIP:
	fprintf(stderr, "\nZIP not supported yet");
	exit(1);

      default:
	fprintf(stderr, "\n unknown compression type");
	exit(1);
    }

    /* mkdld : Make the download bundle */
    
    bf = fopen(bundle_filename, "r");
    if (!bf) {
	fprintf(stderr, "\nError: failed to open %s for reading\n",
		bundle_filename);
	exit(1);
    }
    uf = fopen(ucode_output_filename, "w");
    if (!uf) {
	fprintf(stderr, "\nError: failed to open %s for writing\n",
		ucode_output_filename);
	exit(1);
    }
    fw_rev = ios_ver_maj << 8 | ios_ver_min;
    process_mkdlx(bf, uf, req_hw_rev, fw_rev, hw_type);

    printf("Completed making %s\n", ucode_output_filename);
    exit(0);
}

/* end of file */
