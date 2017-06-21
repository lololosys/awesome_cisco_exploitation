/* $Id: reloc_mksum.c,v 3.4 1996/03/07 19:34:26 abaindur Exp $
 * $Source: /release/111/cvs/Xsys/scripts/reloc_mksum.c,v $
 *------------------------------------------------------------------
 * reloc_mksum.c : Checksum relocatable image.
 *
 * Oct 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reloc_mksum.c,v $
 * Revision 3.4  1996/03/07  19:34:26  abaindur
 * CSCdi50941:  Dust off platform specific stuff from relocatable image
 * tools
 *
 * Revision 3.3  1996/02/13  20:58:34  hampton
 * Use the new file for recording platform specific starting addresses of
 * relocatable images.  [CSCdi48844]
 *
 * Revision 3.2  1995/11/17  18:50:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ASMINCLUDE
#include "../../boot/cpu.h"
#include "../machine/target_cisco.h"
#include "../h/cisco_a.out.h"
#include "reloc_addrs.h"
#include "../os/reloc.h"
#include "../os/sum_magic_number.h"
#include "../../boot/cpu.h"

#define val(x) #x
#define macval(x) val(x)

short debug_on = 0;
short verbose_on = 0;
short no_output = 0;
long j_offset;
char *family = NULL;

FILE *inpfile;
char *inpfilenm;

struct exec exec_header;        /* a.out header of file being processed */
struct reloc_image_internal_header reloc_hdr; /* spl hdr + padding */
struct special_header *spl_hdr;  /* Special header for relocatable images */

/*
 * Checksum storage
 */
unsigned long text_checksum;
unsigned long data_checksum;
unsigned long reloc_checksum;
unsigned long pic_checksum;
unsigned long image_checksum;
unsigned long hdr_checksum;
unsigned long text_checksum_offset; /* within data section */

/*
 * Start addresses of sections within the executable images
 */
unsigned long pic_code_addr;     /* Start of PIC relocation section */
unsigned long text_section_addr; /* Start of text section */
unsigned long data_section_addr; /* Start of data section */
unsigned long reloc_table_addr;  /* Start of relocation tables */

/*
 * Pointers to our in-memory copies of image sections
 */
unsigned char *pic_section;     /* PIC code section */
unsigned char *text_section;    /* Text section */
unsigned char *data_section;    /* Data section */
unsigned char *reloc_section;   /* Relocation tables section */

/*
 * Relocation context
 */
struct relocation_table *text_relocation; /* Text section relocation */
struct relocation_table *data_relocation; /* Data section relocation */

/*
 * Local strings
 */
static char version[] = 
    "$reloc_mksum: Rev: 1.1 Date: 1994/08/28 Hacker: ab $\n";



/*
 * read_section - Read an image section
 *
 * This routine is used to read an image section to a buffer. The buffer
 * is allocated by this routine (using malloc()). The caller passes the
 * start address within the file, and the length.
 */

static unsigned char *read_section 
  (unsigned long start,
   unsigned long len)
{
    unsigned char *section;
    int status;

    section = (unsigned char *)malloc(len);     /* Get a buffer */
    if (!section) {
        fprintf(stderr, "Unable to get memory for section buffer\n");
        exit(1);
    }

    status = fseek(inpfile, start, 0);  /* Seek to the section */

    status = fread(section, 1, len, inpfile); /* Read the section */
    if (status != len) {
        fprintf(stderr, "Error reading section, reading %d bytes got %d bytes\n",
                len, status);
        exit(1);
    }

    return(section);            /* Return the section pointer */
}


/*
 * read_exec_header - Read the exec header and get set up
 */
 
static void read_exec_header ()
{
    int status;
    unsigned long *temp;

    status = fseek(inpfile, 0, 0);      /* Rewind to the beginning */

    status = fread(&exec_header, sizeof(exec_header), 1, inpfile);
    if (status != 1) {
        fprintf(stderr, "Error reading exec file header\n");
        exit(1);
    }
    if (debug_on)
        printf("\nExecutable a.out header read...");

    if (N_BADMAG(exec_header)) {
        fprintf(stderr, "Exec header is not valid\n");
        exit(1);
    }
}

/*
 * show_exec_header - Debugging routine to show exec header
 */

static void show_exec_header()
{
    printf("\nMagic number  : %08x"
           "\nText size     : %08x  Data size     : %08x   "
           "BSS size      : %08x"
           "\nSym table     : %08x  Entry point   : %08x"
           "\nText reloc    : %08x  Data reloc    : %08x\n", 
           exec_header.a_magic,
           exec_header.a_text, exec_header.a_data, exec_header.a_bss,
           exec_header.a_syms, exec_header.a_entry,
           exec_header.a_trsize, exec_header.a_drsize);
}


/*
 * read_spl_header - Read the special header and get set up.
 */
 
static void read_spl_header ()
{
    int status;
    unsigned long *temp;

    status = fseek(inpfile, SUN_AOUT_HDR_SIZE, 0); /* Seek to header */

    status = fread(&reloc_hdr, sizeof(reloc_hdr), 1, inpfile);
    if (status != 1) {
        fprintf(stderr, "Error reading special file header\n");
        exit(1);
    }
    if (debug_on)
        printf("\nSpecial header read ...");

    spl_hdr = &reloc_hdr.spl_hdr;  /* set global pointer to spl header */
    if (spl_hdr->r_magic != RELOC_MAGIC1) {
        printf("\nInvalid magic in special header\n");
        exit(1);
    }
}

/*
 * show_spl_header - Debugging routine to show spl header
 */

static void show_spl_header()
{
    struct special_header *spl_hdr;

    spl_hdr = &reloc_hdr.spl_hdr;
    printf("\nJump instr : %04x %04x %04x"
           "\nMagic      : %02x%02x%04x   Version    : %08x",
           spl_hdr->j_instr.j_opcode, spl_hdr->j_instr.j_offset_1,
           (spl_hdr->j_instr.j_offset_2 >> 16) & 0xffff,
           (spl_hdr->r_dynamic << 7) | spl_hdr->r_toolversion,
           spl_hdr->r_machtype, spl_hdr->r_magic,
           spl_hdr->r_version);
    printf("\nHeader len : %04x"
           "\nText at    : %08x   Data at    : %08x"
           "\nText size  : %08x   Data size  : %08x    BSS size     : %08x"
           "\nPIC size   : %04x       Reloc size : %08x",
           spl_hdr->r_hdr_len, spl_hdr->r_text, spl_hdr->r_data,
           spl_hdr->r_tsize, spl_hdr->r_dsize, spl_hdr->r_bsize,
           spl_hdr->r_pgmsize, spl_hdr->r_rsize);
    printf("\nDef text   : %08x   Def data   : %08x    Def RAM      : %08x"
           "\nText offset: %08x   Data offset: %08x"
           "\nDef text ck: %08x   Def data ck: %08x",
           spl_hdr->r_def_text, spl_hdr->r_def_data, 
           spl_hdr->r_def_ram_start,
           spl_hdr->r_text_offset, spl_hdr->r_data_offset,
           spl_hdr->r_def_text_cksum, spl_hdr->r_rfr_text_cksum);
    printf("\nTC offset  : %08x   TC in data : %08x"
           "\nIC         : %08x   Def IC     : %08x"
           "\nHdr cksum  : %08x",
           spl_hdr->r_text_cksum_offset, 
           spl_hdr->r_text_cksum_offset_in_data,
           spl_hdr->r_image_cksum, spl_hdr->r_def_image_cksum,
           spl_hdr->r_hdr_cksum);
    printf("\nChecksum algorithms :");
    printf("\nText = %08x  Image = %08x   Hdr = %08x",
           spl_hdr->r_text_cksum_algor,
           spl_hdr->r_image_cksum_algor,
           spl_hdr->r_hdr_cksum_algor);
    printf("\nPlatform family: %08x\n",
           spl_hdr->r_platform_family);
}


void store_long_value (unsigned char *location, unsigned long value)
{
    location[0] = value >> 24;
    location[1] = value >> 16;
    location[2] = value >> 8;
    location[3] = value;
}

unsigned long get_long_value (unsigned char *location)
{
    unsigned long cnt;

    cnt =  ((location[0] << 24) & 0xff000000);
    cnt |= ((location[1] << 16) & 0x00ff0000);
    cnt |= ((location[2] << 8) & 0x0000ff00);
    cnt |= ((location[3]) & 0x000000ff);

    return (cnt);
}

/*
 * Relocate a reference. Read in the old value, add the offset,
 * and write it back. We do this byte-by-byte because our
 * reference may not always start at a long word aligned
 * boundary. On the sparc m/cs (and others ?), doing a long
 * word access at anything other than a long-word boundary
 * causes bus error.
 */
unsigned long fixup_location (unsigned char *section, 
                              long offset)
{
    unsigned long old, new;

    old = get_long_value(section);

    new = old + offset; /* Perform the relocation */

    store_long_value(section, new);

    return (new);
}


/*
 * Relocate references from a section to another.
 */
void relocate (unsigned char *section, 
               RELOC_OFFSET_TYPE *reloc_ptr, 
               unsigned long cnt,
               unsigned long diff)
{ 
    unsigned char *location = section;

    while (cnt) {
        location += *reloc_ptr;
        cnt--;
        if (*reloc_ptr++ != 0xff)
	    fixup_location(location, (long)diff);
    }
}


/*
 * Checksum routines.
 * First is the basic checksum routine that calculates a new
 * checksum given the previous checksum and a data value.
 * Second is a checksum wrapper that calculates the checksum
 * for a block of data.
 */
unsigned long basic_checksum (unsigned long cksum, unsigned long val)
{
    unsigned long prev;

    prev = cksum;
    cksum += val;
    if (cksum < prev)
        cksum++;
    return (cksum);
}

unsigned long calculate_checksum (unsigned char *start,
                                  unsigned long len,
                                  unsigned long cksum)
{
    unsigned long *long_ptr, prev;

    long_ptr = (unsigned long *)start;
    while (len >= 4) {
        cksum = basic_checksum(cksum, *long_ptr++);
        len -= 4;
    }
    prev = cksum;
    switch (len) {
      case 1:
            cksum += (*long_ptr & 0xFF000000);
            break;
	  case 2:
            cksum += (*long_ptr & 0xFFFF0000);
            break;
	  case 3:
            cksum += (*long_ptr & 0xFFFFFF00);
            break;
	  case 0:
	  default:
            break;
	}
    if (cksum < prev)
        cksum++;
    return (cksum);
}


/*
 * ---------------- Checksum the various image sections --------------
 */
void checksum_pic (void)
{
    pic_section = read_section(ALIGNED_BLOCK_SIZE, spl_hdr->r_pgmsize);
    pic_checksum = calculate_checksum(pic_section, spl_hdr->r_pgmsize,
                                      pic_checksum);
    if (verbose_on) {
       printf("\nPIC checksum = %08x", pic_checksum);
   }
}

void checksum_reloc (void)
{
    reloc_section = read_section(ALIGNED_BLOCK_SIZE + spl_hdr->r_pgmsize,
                                 spl_hdr->r_rsize);
    reloc_checksum = calculate_checksum(reloc_section, spl_hdr->r_rsize,
                                      reloc_checksum);
    if (verbose_on) {
       printf("\nReloc tables checksum = %08x", reloc_checksum);
   }
}

void checksum_text (void)
{
    char *rfr_text_base, *rfr_data_base;
    unsigned char *reloc_ptr;
    unsigned long cnt, diff, i;
    /* unsigned long *long_reloc_ptr; */
 
    text_section = read_section(spl_hdr->r_text_offset, spl_hdr->r_tsize);
    text_checksum = calculate_checksum(text_section, spl_hdr->r_tsize,
                                       text_checksum);
    if (text_checksum == MAGIC_VALUE)
        text_checksum++;

    if (verbose_on) {
       printf("\nText section checksum = %08x", text_checksum);
   }

    spl_hdr->r_def_text_cksum = text_checksum;

    rfr_text_base = (char *)spl_hdr->r_def_ram_start + 
                    spl_hdr->r_hdr_len +
                    spl_hdr->r_pgmsize;

    rfr_data_base = rfr_text_base + spl_hdr->r_tsize;

    if (debug_on) {
        printf("\nRelocating text to %08x, data to %08x for TC calculation",
               rfr_text_base, rfr_data_base);
    }
    /*
     * Relocate text segment
     */
    diff = (unsigned long)rfr_text_base - (unsigned long)spl_hdr->r_text;
    reloc_ptr = (unsigned char *)reloc_section;
    cnt = get_long_value(reloc_ptr);
    reloc_ptr += sizeof(long);
    if (debug_on) {
        printf("\n%d text->text relocations. First eighty...\n", cnt);
        for (i = 0; i < 80; i++)
            printf("%02x%c", *(reloc_ptr+i), ((i % 20) == 19) ? '\n' : ' ');
    }
    relocate(text_section, reloc_ptr, cnt, diff);

    diff = (unsigned long)rfr_data_base - (unsigned long)spl_hdr->r_data;
    reloc_ptr += (RELOC_OFFSET_SIZE * cnt);
    cnt = get_long_value(reloc_ptr);
    reloc_ptr += sizeof(long);
    if (debug_on) {
        printf("\n%d text->data relocations. First eighty...\n", cnt);
        for (i = 0; i < 80; i++)
            printf("%02x%c", *(reloc_ptr+i), ((i % 20) == 19) ? '\n' : ' ');
    }
    relocate(text_section, reloc_ptr, cnt, diff);

    reloc_ptr += (RELOC_OFFSET_SIZE * cnt);
    cnt = get_long_value(reloc_ptr);
    reloc_ptr += sizeof(long);
    if (debug_on) {
        printf("\n%d text->bss relocations. First eighty...\n", cnt);
        for (i = 0; i < 80; i++)
            printf("%02x%c", *(reloc_ptr+i), ((i % 20) == 19) ? '\n' : ' ');
    }
    relocate(text_section, reloc_ptr, cnt, diff);

    spl_hdr->r_rfr_text_cksum =
        calculate_checksum(text_section, spl_hdr->r_tsize,
                           spl_hdr->r_rfr_text_cksum);
    if (spl_hdr->r_rfr_text_cksum == MAGIC_VALUE)
        spl_hdr->r_rfr_text_cksum++;

    /* Display rest of the relocation table counts */
    if (debug_on) {
        reloc_ptr += (RELOC_OFFSET_SIZE * cnt);
        cnt = get_long_value(reloc_ptr);
        reloc_ptr += sizeof(long);
        printf("\n%d data->text relocations. First eighty...\n", cnt);
        for (i = 0; i < 80; i++)
            printf("%02x%c", *(reloc_ptr+i), ((i % 20) == 19) ? '\n' : ' ');

        reloc_ptr += (RELOC_OFFSET_SIZE * cnt);
        cnt = get_long_value(reloc_ptr);
        reloc_ptr += sizeof(long);
        printf("\n%d data->data relocations. First eighty...\n", cnt);
        for (i = 0; i < 80; i++)
            printf("%02x%c", *(reloc_ptr+i), ((i % 20) == 19) ? '\n' : ' ');

        reloc_ptr += (RELOC_OFFSET_SIZE * cnt);
        cnt = get_long_value(reloc_ptr);
        reloc_ptr += sizeof(long);
        printf("\n%d data->bss relocations. First eighty...\n", cnt);
        for (i = 0; i < 80; i++)
            printf("%02x%c", *(reloc_ptr+i), ((i % 20) == 19) ? '\n' : ' ');
    }
}

void checksum_data (void)
{
    unsigned char *cksum_ptr;

    /*
     * First store the text section checksum, then calculate the
     * data section checksum.
     */

    data_section = read_section(spl_hdr->r_data_offset, spl_hdr->r_dsize);

    cksum_ptr = data_section + spl_hdr->r_text_cksum_offset_in_data;
    store_long_value(cksum_ptr, text_checksum);

    data_checksum = calculate_checksum(data_section, spl_hdr->r_dsize,
                                       data_checksum);
    if (verbose_on) {
        printf("\nData section checksum = %08x", data_checksum);
    }
}

void checksum_image (void)
{
    image_checksum = basic_checksum(image_checksum, pic_checksum);
    image_checksum = basic_checksum(image_checksum, reloc_checksum);
    image_checksum = basic_checksum(image_checksum, text_checksum);
    image_checksum = basic_checksum(image_checksum, data_checksum);

    spl_hdr->r_image_cksum = image_checksum;
    spl_hdr->r_def_image_cksum = image_checksum;
    if (verbose_on) {
        printf("\nImage checksum = %08x", image_checksum);
    }
}

void checksum_hdr (void)
{
    hdr_checksum = 0xFFFFFFFF - 
                   calculate_checksum((unsigned char *)&reloc_hdr,
                                      spl_hdr->r_hdr_len, 0);

    spl_hdr->r_hdr_cksum = hdr_checksum;
    if (verbose_on) {
        printf("\nSpl header checksum = %08x", hdr_checksum);
    }
}


/*
 * Init whatever we need to for the checksumming.
 */
void checksumming_init (void)
{
    /*
     * Initialize fields in special header.
     */
    spl_hdr->r_def_text_cksum = 0xFFFFFFFF;
    spl_hdr->r_rfr_text_cksum = 0xFFFFFFFF;
    spl_hdr->r_image_cksum = 0xFFFFFFFF;
    spl_hdr->r_def_image_cksum = 0xFFFFFFFF;
    spl_hdr->r_hdr_cksum = 0xFFFFFFFF;
    spl_hdr->r_text_cksum_algor = TC_ALGORITHM;
    spl_hdr->r_image_cksum_algor = IC_ALGORITHM;
    spl_hdr->r_hdr_cksum_algor = SHC_ALGORITHM;
    spl_hdr->r_platform_family = PLATFORM_FAMTYPE;

    /* 
     * Initialize working checksums
     */
    pic_checksum = 0;
    reloc_checksum = 0;
    text_checksum = 0;
    data_checksum = 0;
    hdr_checksum = 0;
    image_checksum = 0;
}

void do_checksums (void)
{
    /*
     * Read in sections and calculate checksums
     */
    if (debug_on)
        printf("\nChecksumming PIC section ...");
    checksum_pic();

    if (debug_on)
        printf("\nChecksumming reloc tables ...");
    checksum_reloc();

    if (debug_on)
        printf("\nChecksumming text section ...");
    checksum_text();

    if (debug_on)
        printf("\nChecksumming data section ...");
    checksum_data();


    if (debug_on)
        printf("\nChecksumming image ...");
    checksum_image();


    if (debug_on)
        printf("\nChecksumming special header ...");
    checksum_hdr();

    printf("\nChecksums...");
    printf("\nHeader: 0x%x, Text: 0x%x, Image: 0x%x\n",
           spl_hdr->r_hdr_cksum, spl_hdr->r_def_text_cksum, 
           spl_hdr->r_image_cksum); 
}


/*
 * Update output file.
 * We go update the special header because it contains the
 * checksums.
 * Then we update the data section for the text checksum.
 */

void update_out (void)
{
    int status;

    /* 
     * Seek to special header and update it.
     */
    fseek(inpfile, SUN_AOUT_HDR_SIZE, 0);

    status = fwrite(&reloc_hdr, sizeof(reloc_hdr), 1, inpfile); 
    if (status != 1) {
        perror("Error updating file special header");
        exit(1);
    }
    if (debug_on)
        printf("Exec header of file '%s' updated", inpfile);


    /*
     * Seek to data section and update text checksum.
     */
    fseek(inpfile, spl_hdr->r_text_cksum_offset, 0);

    status = fwrite(&spl_hdr->r_def_text_cksum, sizeof(long), 1, inpfile);
    if (status != 1) {
        perror("Error updating text segment checksum");
        exit(1);
    }
    if (debug_on)
        printf("Text checksum of file '%s' updated", inpfile);
}


void usage (name)
     char *name;
{
    fprintf(stderr,
            "Usage:"
            "  %s [-v] [-/d] [-n] <file>\n"
            "   -v is for verbose output\n"
            "   -/d is for even more verbose output\n"
            "   -n is for file update suppression\n",
            name);
    exit(1);
}

/*
 * parse_hex - Parse a hex string
 */

unsigned long parse_hex(string)
     char *string;
{
    int i;
    unsigned long result;

    i = sscanf(string, "%lx", &result);
    if (i != 1) {
        fprintf(stderr, "Invalid hex string %s\n", string);
        exit(1);
    }
    return(result);
}


/*
 * main - WE START HERE. Handle the user interface
 */

int main (int argc, char **argv)
{
    int argument;
    char *arg_key;

    printf("%s", version);
    for (argument = 1; argument < argc; argument++) {
        arg_key = argv[argument];
        if (*arg_key == '-') {
            arg_key++;
            if (tolower(*arg_key) == 'v') {
                verbose_on = 1;
            } else if ((*arg_key == '/') && (*(arg_key + 1) == 'd')) {
                debug_on = 1;
                verbose_on = 1;
            } else if (tolower(*arg_key) == 'n') {
                no_output = 1;
            } else if ((*arg_key == 'D') || (*arg_key == 'F')) {
              family = arg_key + 1;
	  } else break;               /* Unknown */
        }
        else
            break;
    }

    if ((argument + 1) != argc)
      usage(argv[0]);           /* Print error if unknown arguments */

    inpfilenm = argv[argument];

    /* Open the input file */
    inpfile = fopen(inpfilenm, no_output ? "r" : "r+"); 
    if (!inpfile) {
        perror("\nError opening input file");
        exit(1);
    }
    if (debug_on)
        printf("\nInput file '%s' opened...", inpfilenm);

    read_exec_header();         /* Read the exec header */
    if (verbose_on)
        show_exec_header();

    read_spl_header();
    if (verbose_on) {
        show_spl_header();
    }

    checksumming_init();

    do_checksums();

    /*
     * Create output file
     */
    if (!no_output)
        update_out();

    if (verbose_on) {
        printf("\n\nFinal Special header ...");
        show_spl_header();
    }

    fclose(inpfile);
    if (debug_on)
        printf("File '%s' closed", inpfile);

    exit(0);                    /* Success */
}

