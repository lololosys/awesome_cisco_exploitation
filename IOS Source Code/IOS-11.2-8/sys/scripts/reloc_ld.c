/* $Id: reloc_ld.c,v 3.5 1996/03/07 19:34:23 abaindur Exp $
 * $Source: /release/111/cvs/Xsys/scripts/reloc_ld.c,v $
 *------------------------------------------------------------------
 * reloc_ld.c : Creation of relocatable image.
 *
 * Oct 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reloc_ld.c,v $
 * Revision 3.5  1996/03/07  19:34:23  abaindur
 * CSCdi50941:  Dust off platform specific stuff from relocatable image
 * tools
 *
 * Revision 3.4  1996/02/13  20:58:32  hampton
 * Use the new file for recording platform specific starting addresses of
 * relocatable images.  [CSCdi48844]
 *
 * Revision 3.3  1995/11/17  18:50:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:22:36  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:11:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:37  hampton
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

#define val(x) #x
#define macval(x) val(x)
#define TEXT_CHECKSUM_VARIABLE "_" macval(MAGIC_VARIABLE)
#define DEFAULT_RELOCATION_PROGRAM  "relocate"

/*
 * 68K specific instructions
 */
#define NOP_INSTR               0x4E71
#define LONG_BRA_INSTR          0x60ff
#define SHORT_BRA_INSTR         0x6000
#define MAX_NOP_SKIP		8

#define LONG_BRA_SIZE           6
#define SHORT_BRA_SIZE          4
#define PAD_BLOCK_SIZE          4
#define MALLOC_PADDING		1024

char debug_on = 0;
char verbose_on = 0;
long j_offset;
short j_instr;
char *family = NULL;
short pad_nop;  /*  a NOP for padding to long word boundary */
int pic_padding = 0;
int reloc_padding = 0;
char pad_block[PAD_BLOCK_SIZE];  /* for long word alignment */
char *region_types[] = { "text",
                         "data",
                         "BSS "
			 };

FILE *inpfile;
FILE *picfile;
FILE *outfile;
FILE *sunfile;
char *inpfilenm;
char *outfilenm;
char *picfilenm;
char *sunfilenm;

struct exec exec_header;        /* a.out header of file being processed */
struct exec out_header;         /* a.out header of output file */
struct reloc_image_internal_header reloc_hdr; /* spl hdr + padding */
struct special_header *spl_hdr;  /* Special header for relocatable images */
char *pic_ptr=NULL;             /* pointer to the PIC program */
char *pic_text_ptr;             /* pointer to start of text in PIC */
struct stat pic_stat;           /* stats on PIC program */
int reloc_stats[REGION_MAX][5]; /* relocation statistics */
int reloc_cnt[REGION_MAX][5];
int reference_stats[REGION_MAX][4]; /* reference statistics */
int offset_stats[REGION_MAX][4]; /* relocation offset statistics */

/*
 * Checksum storage
 */
unsigned long text_checksum;
unsigned long image_checksum;
unsigned long hdr_checksum;
struct nlist *text_checksum_symbol = NULL;
unsigned long text_checksum_offset;

/*
 * Start addresses of sections within the executable images
 */

unsigned long text_section_addr; /* Start of text section */
unsigned long data_section_addr; /* Start of data section */
unsigned long text_reloc_addr;  /* Start of text relocation */
unsigned long data_reloc_addr;  /* Start of data relocation */
unsigned long symbol_table_addr; /* Start of symbol table */
unsigned long string_table_addr; /* Start of string table */
unsigned long string_table_len; /* Length of string table */

/*
 * Pointers to our in-memory copies of image sections
 */

unsigned char *text_section;    /* Text section */
unsigned char *data_section;    /* Data section */
struct reloc_info_68k *text_reloc; /* Text relocation information */
struct reloc_info_68k *data_reloc; /* Data relocation information */
struct nlist *symbol_table;     /* Symbol table */
char *string_table;             /* String table */

/*
 * Context to allocate BSS space
 */

unsigned long next_bss_entry; /* Next BSS table entry we can allocate */
unsigned long next_bss_offset;  /* Next free offset in BSS */
struct bss_entry bss_table[MAX_BSS_ENTRY]; /* BSS allocation table */

/*
 * Context for section spaces
 */

unsigned long original_text_base; /* Text base we are relocating from */
unsigned long original_data_base; /* Data base we are relocating from */
unsigned long original_bss_base; /* BSS base we are relocating from */
unsigned long saved_original_bss_base; /* BSS base from linker */

unsigned long final_text_base;  /* Text base we are relocating to */
unsigned long actual_final_text_base; /* Text base we are relocating to */
unsigned long final_data_base;  /* Data base we are relocating to */
unsigned long final_bss_base;   /* BSS base we are relocating to */

/*
 * Special pointers to magic variables
 */

char *etext_ptr;                /* Pointer to "_etext" in symbol table */
char *edata_ptr;                /* Pointer to "_edata" in symbol table */
char *end_ptr;                  /* Pointer to "_end" in symbol table */

char *stext_ptr;                /* Pointer to "__stext" in symbol table */
char *sdata_ptr;                /* Pointer to "__sdata" in symbol table */
char *sbss_ptr;                 /* Pointer to "__sbss" in symbol table */

/*
 * Relocation context
 */

struct long_relocation_table old_relocation; /* Text section relocation */
struct relocation_table text_relocation; /* Text section relocation */
struct relocation_table data_relocation; /* Data section relocation */

/*
 * Local strings
 */

static char multi_defined_symbol[] = "Symbol %s has multiple definitions\n";
static char version_log[] = 
    "$reloc_ld: Rev: 1.2 Date: 1992/05/28 Author: kph $\n"
    "$reloc_ld: Rev: 1.3 Date: 1994/08/28 Hacker: ab $\n";
static char version[] = 
    "$reloc_ld: Rev: 1.4 Date: 1995/02/10 Hacker: ab $\n";

/*
 *----------------------- Display routines -------------------------
 */

/*
 * show_exec_header - Debugging routine to show exec header
 */
static void show_exec_header()
{
    printf("\nFile '%s' exec header :", inpfilenm);
    printf("\nMagic number  : %08x"
           "\nText start    : %08x  Data start    : %08x"
           "\nText size     : %08x  Data size     : %08x"
           "  BSS size      : %08x"
           "\nSym tbl start : %08x  Sym tbl size  : %08x"
           "\nText rel start: %08x  Data rel start: %08x"
           "\nText rel size : %08x  Data rel size : %08x",
           exec_header.a_magic,
           text_section_addr, data_section_addr,
           exec_header.a_text, exec_header.a_data,
           exec_header.a_bss,
           symbol_table_addr, exec_header.a_syms,
           text_reloc_addr, data_reloc_addr,
           exec_header.a_trsize, exec_header.a_drsize);
}

/*
 * print_symbol_info - Debugging routine to show symbol table entry
 */
void print_symbol_info (struct nlist *symbol)
{
    printf("\nString index: %x (%s)"
           "\n  Type: %x  Other: %x  Desc: %x  Value: %x",
           symbol->n_un.n_strx, &string_table[symbol->n_un.n_strx],
           symbol->n_type,
           symbol->n_other,
           symbol->n_desc,
           symbol->n_value);
}

/*
 * print_reloc_info - Debugging routine to show relocation table entry
 */
void print_reloc_info (struct reloc_info_68k *reloc)
{
    printf("\nAddr:%x  Sym no:%x  Len:%x  "
           "PCR:%x  Ext:%x  Base:%x  Jmp:%x  Rel:%x  ",
           reloc->r_address,
           reloc->r_symbolnum,
           reloc->r_length,
           reloc->r_pcrel,
           reloc->r_extern,
           reloc->r_baserel,
           reloc->r_jmptable,
           reloc->r_relative);
}

void print_closing_stats (void)
{
    printf("\nOriginal bases = %x/%x/%x  Final bases = %x/%x/%x",
           original_text_base, original_data_base, original_bss_base,
           reloc_hdr.spl_hdr.r_text, final_data_base, final_bss_base);
    printf("\nEntry point: 0x%x", final_text_base);
    printf("\nReloc tables: text %d+%d+%d, total %d (%dK)"
	   "\n              data %d+%d+%d, total %d (%dK)\n",
           text_relocation.region[REGION_TEXT].count,
           text_relocation.region[REGION_DATA].count,
           text_relocation.region[REGION_BSS].count,

           text_relocation.region[REGION_TEXT].count+
           text_relocation.region[REGION_DATA].count+
           text_relocation.region[REGION_BSS].count,
           (text_relocation.region[REGION_TEXT].count+
           text_relocation.region[REGION_DATA].count+
           text_relocation.region[REGION_BSS].count) / 1024,

           data_relocation.region[REGION_TEXT].count,
           data_relocation.region[REGION_DATA].count,
           data_relocation.region[REGION_BSS].count,

           data_relocation.region[REGION_TEXT].count+
           data_relocation.region[REGION_DATA].count+
           data_relocation.region[REGION_BSS].count,
           (data_relocation.region[REGION_TEXT].count+
           data_relocation.region[REGION_DATA].count+
           data_relocation.region[REGION_BSS].count) / 1024);
}


/*
 *------------------------ File reads ----------------------------------
 */

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

    /* Get a buffer */
    section = (unsigned char *)malloc(len + MALLOC_PADDING);     
    if (!section) {
        fprintf(stderr, "Unable to get memory for section buffer\n");
        exit(1);
    }

    status = fseek(inpfile, start, 0);  /* Seek to the section */

    status = fread(section, 1, len, inpfile); /* Read the section */
    if (status != len) {
        fprintf(stderr, "Section Read Error: read %d bytes instead of %d "
		" at offset %d\n",
                status, len, start);
        exit(1);
    }

    return(section);            /* Return the section pointer */
}


/*
 * read_exec_header - Read the exec header and get set up
 *
 * This routine is used to read the exec header, and calculate the
 * offsets of all of the various sections in the file.
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

    if (N_BADMAG(exec_header)) {
        fprintf(stderr, "Exec header is not valid\n");
        exit(1);
    }

    text_section_addr = N_TXTOFF(exec_header);
    data_section_addr = N_DATOFF(exec_header);

    text_reloc_addr = N_TRELOFF(exec_header);
    data_reloc_addr = N_DRELOFF(exec_header);

    symbol_table_addr = N_SYMOFF(exec_header);

    string_table_addr = N_STROFF(exec_header);

    temp = (unsigned long *)read_section(string_table_addr, 4);
                                 /* Read length */
    string_table_len = *temp;   /* Get length */
    free(temp);                 /* Free buffer */
}

void read_file_sections (void)
{
    /*
     * Read the exec header to get all the section addresses
     * and sizes.
     * Then read the sections.
     */
    read_exec_header();         /* Read the exec header */
    if (verbose_on)
        show_exec_header();

    /*
     * Read in the rest of the file
     */

    text_section = read_section(text_section_addr, exec_header.a_text);
    data_section = read_section(data_section_addr, exec_header.a_data);

    text_reloc = (struct reloc_info_68k *)
                 read_section(text_reloc_addr, exec_header.a_trsize);
    data_reloc = (struct reloc_info_68k *)
                 read_section(data_reloc_addr, exec_header.a_drsize);

    symbol_table = (struct nlist *)
                   read_section(symbol_table_addr, exec_header.a_syms);

    string_table = (char *)
                   read_section(string_table_addr, string_table_len);
}


/*
 *---------------- Search, create, resolve BSS symbols -----------------
 */

/*
 * find_bss_object - Find an object in BSS
 *
 * This routine is used to find an object in BSS. We compare name pointers
 * to look for a match; if two relocation entries point to symbols with
 * the same string pointer, they are considered the same symbol. If they
 * point to different strings (which should never happen), they are
 * actually different symbols.
 */

struct bss_entry *find_bss_object
  (char *name,
   unsigned long size)
{
    int index;
    struct bss_entry *entry;

    for (index = 0; index < next_bss_entry; index++) {
        entry = &bss_table[index]; /* Pick up entry in BSS table */
        if (entry->name != name)
          continue;
        if (entry->size != size) {
            fprintf(stderr,
                    "Multiple sizes for object %s, old = %x, new = %x\n",
                    entry->name, entry->size, size);
            exit(1);
        }
        return(entry);
    }
    return(NULL);
}


/*
 * allocate_bss_object - Allocate an object in BSS
 *
 * This routine is used to allocate or an object in BSS. The object must
 * have not already been allocated.
 */

struct bss_entry *allocate_bss_object
  (char *name,
   unsigned long size)
{
    struct bss_entry *entry;

    if (find_bss_object(name, size)) {
        fprintf(stderr, multi_defined_symbol, name);
        exit(1);
    }

    /*
     * Not found, allocate a new object
     */

    if (next_bss_entry >= MAX_BSS_ENTRY) {
        fprintf(stderr, "BSS table overflow; entries exceed %d\n",
		MAX_BSS_ENTRY);
        exit(1);
    }

    entry          = &bss_table[next_bss_entry++]; /* Get table entry */
    entry->name    = name;                         /* Save name */
    entry->address = next_bss_offset;              /* Set an offset */
    entry->size    = size;                         /* Save size */

    next_bss_offset = ((next_bss_offset + size + BSS_OBJECT_ALIGN) &
                       ~BSS_OBJECT_ALIGN); /* Calculate next BSS offset */

    return(entry);
}


/*
 * find_symbol_value - Look up a symbol value. 
 *
 * This routine is used to look up a symbol value. We handle the special
 * case symbols here, as well as BSS lookup. Symbols here must be
 * defined.
 */

unsigned long find_symbol_value
  (char  *name,
   unsigned long size,
   enum REGIONS *region)
{
    struct bss_entry *entry;

    if (!size) {                /* Must be a special symbol */
        if (name == edata_ptr) { /* _edata? */
            *region = REGION_DATA;
            return(final_data_base + exec_header.a_data);
        }

        if (name == etext_ptr) { /* _etext? */
            *region = REGION_TEXT;
            return(final_text_base + exec_header.a_text);
        }

        if (name == end_ptr) { /* _end? */
            *region = REGION_BSS;
            return(final_bss_base + next_bss_offset);
        }

        if (name == stext_ptr) { /* __stext? */
            *region = REGION_TEXT;
            return(final_text_base);
        }

        if (name == sdata_ptr) { /* __sdata? */
            *region = REGION_DATA;
            return(final_data_base);
        }

        if (name == sbss_ptr) { /* __sbss? */
            *region = REGION_BSS;
            return(final_bss_base);
        }

        fprintf(stderr,
                "Undefined special symbol %s in find_symbol_value\n",
                name);
        exit(1);
    }

    entry = find_bss_object(name, size);
    if (!entry) {
        fprintf(stderr, "Undefined symbol %s in find_symbol_value\n", name);
        exit(1);
    }
    *region = REGION_BSS;
    return(entry->address + final_bss_base);
}


/*
 *-------------------------- Relocation stuff ------------------------
 */

/* Comparison function used during qsort 
 * qsort called to sort the relocation tables
 */
int compar (int *i, int *j)
{
    return (*i - *j);
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

    old =  ((section[0] << 24) & 0xff000000);
    old |= ((section[1] << 16) & 0x00ff0000);
    old |= ((section[2] << 8) & 0x0000ff00);
    old |= ((section[3]) & 0x000000ff);

    new = old + offset; /* Perform the relocation */

    section[0] = new >> 24;
    section[1] = new >> 16;
    section[2] = new >> 8;
    section[3] = new;

    return (new);
}


/*
 * relocate - Relocate a section
 *
 * This routine will relocate a section. We are passed the start of the
 * relocation data and the length.
 */

void relocate 
  (struct reloc_info_68k *reloc, /* Pointer to relocation info */
   unsigned long reloc_len,     /* Length of relocation info */
   char *reloc_type,            /* Type of relocation info being done */
   unsigned char *section,      /* Pointer to start of section in memory */
   unsigned long section_len,   /* Length of section being relocated */
   long section_offset,         /* Offset being moved by */
   struct relocation_table *rtable, /* Relocation table we create */
   char first_time) 
{
    unsigned long len;
    struct nlist *symbol;
    char *symnam;               /* Symbol name */
    long address;               /* Relocated address */
    enum REGIONS region;        /* Region type for relocation info */
    struct long_relocation_table *fixup; /* Old relocation table pointer */
    int i, j, r_indx;
    long prev, diff, reference;

    /* Set pointer to relocation tables and initialize counts */
    fixup = &old_relocation;
    fixup->region[REGION_TEXT].count = 0;
    fixup->region[REGION_DATA].count = 0;
    fixup->region[REGION_BSS].count = 0;

    /* No of entries in the reloc_info_68k structure that
     * need to be processed.
     */
    len = reloc_len / sizeof(struct reloc_info_68k);

    if ((len * sizeof(struct reloc_info_68k)) != reloc_len) {
        fprintf(stderr, "Bad %s section length %d\n",
                reloc_type, reloc_len);
        exit(1);
    }
    
    for (region = 0; region < REGION_MAX; region++)
        for (i = 0; i < 4; i++) {
            reference_stats[region][i] = 0;
            offset_stats[region][i] = 0;
        }

    /*
     * Process all of the relocation records for this segment
     */

    if (first_time)
        printf("\n%d %s segment references to relocate (%dK)", 
	       len, reloc_type, (len * sizeof(long) / 1024));

    for ( r_indx = 0; len-- > 0; r_indx++, reloc++) {
        if (first_time && debug_on) {
            print_reloc_info(reloc);
            if (reloc->r_extern) {
                print_symbol_info(&symbol_table[reloc->r_symbolnum]);
            }
            printf("\n");
        }

        /*
         * Ensure that the entry is valid
         */

        if (reloc->r_length != 2) {
            fprintf(stderr, "\nRelocation %d length not longword in %s at 0x%x:\n",
                            r_indx, reloc_type, reloc);
            print_reloc_info(reloc);
            print_symbol_info(&symbol_table[reloc->r_symbolnum]);
            exit(1);
        }

        if (reloc->r_baserel || reloc->r_jmptable || reloc->r_relative) {
            fprintf(stderr,
                    "Relocation baserel = %x jmptable = %x relative = %x "
                    "at %x(%x)\n",
                    reloc->r_baserel,
                    reloc->r_jmptable,
                    reloc->r_relative,
                    reloc->r_address,
                    reloc_type);
            exit(1);
        }

        /*
         * If this points to a symbol table entry, ensure that it is valid.
         * Otherwise, ensure that the relocation type is valid.
         */

        symnam = "*Unknown*";
        address = 0;

        if (reloc->r_extern) {
            symbol = &symbol_table[reloc->r_symbolnum];
            symnam = &string_table[symbol->n_un.n_strx];

            if (symbol->n_type != (N_UNDF | N_EXT)) {
                fprintf(stderr, "Symbol %s invalid type %x at %x(%s)\n",
                        symnam,
                        symbol->n_type,
                        reloc->r_address,
                        reloc_type);
                exit(1);
            }

            if (symbol->n_other || symbol->n_desc) {
                fprintf(stderr, "Symbol %s n_other = %x "
                        "n_desc = %x at %x(%s)\n",
                        symnam,
                        symbol->n_other,
                        symbol->n_desc,
                        reloc->r_address,
                        reloc_type);
                exit(1);
            }

            address = find_symbol_value(symnam, symbol->n_value, &region);

        } else {                /* Some kind of internal relocation */
            switch (reloc->r_symbolnum) {
              case N_TEXT:
              case N_TEXT | N_EXT:
                address = final_text_base - original_text_base;
                                /* Set text base */
                symnam = "*Text*";
                region = REGION_TEXT;
                break;

              case N_DATA:
              case N_DATA | N_EXT:
                address = final_data_base - original_data_base;
                                         /* Set data base */
                symnam = "*Data*";
                region = REGION_DATA;
                break;

              case N_BSS:
              case N_BSS | N_EXT:
                address = final_bss_base - original_bss_base;
                                        /* Set BSS base */
                symnam = "*BSS*";
                region = REGION_BSS;
                break;

              default:
                fprintf(stderr, "Invalid relocation type %x at %x(%s)\n",
                        reloc->r_symbolnum,
                        reloc->r_address,
                        reloc_type);
                exit(1);
            }
        }

        /*
         * At this point, we are ready to perform the relocation.
         * The address to relocate is in reloc->r_address, and the
         * relocation offset is in address.
         */

        if ((reloc->r_address + 3) >= section_len) {
            fprintf(stderr,"Relocation address %x invalid for %s section\n",
                    reloc->r_address, reloc_type);
            exit(1);
        }

        /*
         * At this point, any PC-relative relocation should have been done.
         * If we see a relocation entry with the PC-relative flag, we
         * bias our offset by the offset of the start of the current
         * section. The result should be zero, indicating that relocation
         * is not necessary. If this is the case, we will not do anything
         * here, and not create a fixup entry.
         *
         * If the result is non-zero, we will print out a warning and
         * attempt to do the relocation. Since we do not propogate the
         * PC-relative flag to the final output file (just fixup records),
         * it is unlikely that this image will be properly relocatable.
         */

        if (reloc->r_pcrel) {
            address -= section_offset;
            if (address)
              fprintf(stderr,
                      "Warning: non-zero PC-relative relocation at %x(%s)\n",
                      reloc->r_address,
                      symnam);
            else
              continue;  
        }

        /* Zero PC-relative relocation, skip this */

        /*
         * We now have a proper relocation amount; get the old value
         * for relocation and fix it.
         * BUT, before that, check if we have exceeded our 
         * relocation table size.
         */

        if (fixup->region[region].count >= MAX_RELOCATION_ENTRY) {
            printf("\n** Relocation table size exceeded **");
            printf("\nTable number %d for %s, size %d", region+1,
                   reloc_type, MAX_RELOCATION_ENTRY);
            exit(1);
        }

        reference = fixup_location((section + reloc->r_address), address);
        switch (reference % 4) {
	  case 0:
                reference_stats[region][0]++;
                break;
	      case 1:
                reference_stats[region][1]++;
                break;
	      case 2:
                reference_stats[region][2]++;
                break;
	      case 3:
                reference_stats[region][3]++;
                break;
	    }
        switch (reloc->r_address % 4) {
	  case 0:
                offset_stats[region][0]++;
                break;
	      case 1:
                offset_stats[region][1]++;
                break;
	      case 2:
                offset_stats[region][2]++;
                break;
	      case 3:
                offset_stats[region][3]++;
                break;
	    }


        /*
         * Add the address that we relocated into the proper table.
         */

        fixup->region[region].address[fixup->region[region].count++] =
            reloc->r_address;   /* Remember relocation address */

    }
    for (region = 0; region < REGION_MAX; region++) {
        if (fixup->region[region].count) 
            qsort((char *)fixup->region[region].address,
                  fixup->region[region].count,
                  sizeof(long),
                  compar);
    }
    /*
     * Gather some statistics. Initialize the counters first.
     * The stats we gather would tell us how many table entries
     * we would need if we had 7 bit, 8 bit, 15 bit, 16 bit,
     * and 32 bit entries for the relocation references.
     * This info will be printed out only if the verbose flag
     * is specified.
     */
    for (region = 0; region < REGION_MAX; region++)
        for (i = 0; i < 5; i++) {
            reloc_stats[region][i] = 0;
            reloc_cnt[region][i] = 0;
        }

    /*
     * We store relative 8 bit offsets in our relocation tables.
     * Research indicated that 8 bit offsets would give us the
     * smallest tables. If the difference between two consecutive
     * addresses exceeds 0xFF, we divide the diff by 0xFF and store
     * FFs equal to the quotient. We store one more value equal to
     * the remainder. The remainder may be zero.
     * So to obtain address (n+1) from address (n), we go on 
     * adding table entries to address (n) until we hit a table
     * entry that's less than 0xFF.
     */
    for (region = 0; region < REGION_MAX; region++) {
        prev = 0;
        rtable->region[region].count = 0;
        if (fixup->region[region].count) {
            for (i = 0, j = 0; i < fixup->region[region].count; i++) {

                diff = fixup->region[region].address[i] - prev;
                prev = fixup->region[region].address[i];

		/* Gather stats only if we have to */
		if (first_time && verbose_on) { 

                if (diff < 0x7f) {
                    reloc_stats[region][0]++;
                    reloc_cnt[region][0]++;
                    reloc_cnt[region][1]++;
                    reloc_cnt[region][2]++;
                    reloc_cnt[region][3]++;
                    reloc_cnt[region][4]++;
                }
                else if (diff < 0xff) {
                    reloc_stats[region][1]++;
                    reloc_cnt[region][0] += 2;
                    reloc_cnt[region][1]++;
                    reloc_cnt[region][2]++;
                    reloc_cnt[region][3]++;
                    reloc_cnt[region][4]++;
                }
                else if (diff < 0x7fff) {
                    reloc_stats[region][2]++;
                    reloc_cnt[region][0] += ((diff / 0x7f) + 1);
                    reloc_cnt[region][1] += ((diff / 0xff) + 1);
                    reloc_cnt[region][2]++;
                    reloc_cnt[region][3]++;
                    reloc_cnt[region][4]++;
                }
                else if (diff < 0xffff) {
                    reloc_stats[region][3]++;
                    reloc_cnt[region][0] += ((diff / 0x7f) + 1);
                    reloc_cnt[region][1] += ((diff / 0xff) + 1);
                    reloc_cnt[region][2] += ((diff / 0x7fff) + 1);
                    reloc_cnt[region][3]++;
                    reloc_cnt[region][4]++;
                }
                else  {
                    reloc_stats[region][4]++;
                    reloc_cnt[region][0] += ((diff / 0x7f) + 1);
                    reloc_cnt[region][1] += ((diff / 0xff) + 1);
                    reloc_cnt[region][2] += ((diff / 0x7fff) + 1);
                    reloc_cnt[region][3] += ((diff / 0xffff) + 1);
                    reloc_cnt[region][4]++;
                }

	        } /* end if_verbose */

                while (diff >= 0xff) {
                    rtable->region[region].address[j++] = 0xff;
                    diff -= 0xff;
                }
                rtable->region[region].address[j++] = diff;

            }
            rtable->region[region].count = j;
        }
    }
    if (first_time && verbose_on) {
        printf("\nSome obscure statistics...");
        printf("\n%s relocation info statistics (incremental offset ranges): ", 
               reloc_type);
        printf("\n                7bit   8bit     15bit    16bit    32bit");
        printf("\nType            <128  128-255  256-32K  32K-64K   >=64K");
        for (region = 0; region < REGION_MAX; region++) {
            printf("\nDelta ranges  %06d   %06d   %06d   %06d  %06d  %s->%s",
                   reloc_stats[region][0], reloc_stats[region][1], 
                   reloc_stats[region][2],
                   reloc_stats[region][3], reloc_stats[region][4],
                   reloc_type, region_types[region]);
            printf("\n# of entries  %06d   %06d   %06d   %06d  %06d",
                   reloc_cnt[region][0], reloc_cnt[region][1], 
                   reloc_cnt[region][2], reloc_cnt[region][3],
                   reloc_cnt[region][4]);
            printf("\nTotal size    %06d   %06d   %06d   %06d  %06d",
                   reloc_cnt[region][0], reloc_cnt[region][1], 
                   sizeof(short)*reloc_cnt[region][2], 
                   sizeof(short)*reloc_cnt[region][3],
                   sizeof(long)*reloc_cnt[region][4]);
        }
        printf("\nMore obscure statistics...");
        printf("\nByte alignments of %s segment entries to be relocated:", 
               reloc_type);
        for (region = 0; region < REGION_MAX; region++) {
            printf("\n%s to %s  Mod 1=%06d, Mod 2=%06d, Mod 3=%06d, Mod 4=%06d",
                   reloc_type, region_types[region],
                   offset_stats[region][1], offset_stats[region][2],
                   offset_stats[region][3], offset_stats[region][0]);
        }
        printf("\nByte alignments of relocatable %s segment references: ", 
               reloc_type);
        for (region = 0; region < REGION_MAX; region++) {
            printf("\n%s to %s  Mod 1=%06d, Mod 2=%06d, Mod 3=%06d, Mod 4=%06d",
                   reloc_type, region_types[region],
                   reference_stats[region][1], reference_stats[region][2],
                   reference_stats[region][3], reference_stats[region][0]);
        }
        printf("\n");
    }
}


/*
 * We need to do this re-relocation because of the famous 
 * chicken and egg problem. When we process the .sun file
 * relocation info to create our relocatable image, we do 
 * not know what the size of the relocation tables will be.
 * Therefore we do not know from where (relative to start
 * of image) our text section will start. So we do the
 * initial relocation with the assumption that the text
 * section will start immediately after the a.out header.
 * Then after the relocation tables have been built, we
 * compute where the text section will actually start and
 * re-relocate the text section to that base address.
 * When a user specifies the text base address, it is the
 * address based on the assumption that text follows the
 * a.out header. Since we are putting in extra baggage
 * before the text segment, we readjust the base address
 * to what it should be without expecting the user to do
 * this impossible calculation.
 */
void re_relocate_text (void)
{
    int offset;
    long cnt, cur_delta;
    struct relocation_table *r_table;
    unsigned long new;

    offset = reloc_hdr.spl_hdr.r_text - final_text_base;
    r_table = &text_relocation;

    if (verbose_on) {
        printf("\nRe-relocating text from %x to %x (by %x)",
               final_text_base, reloc_hdr.spl_hdr.r_text, offset);
    }
    cur_delta = 0;
    for (cnt = 0; cnt < r_table->region[REGION_TEXT].count; ) {
        while (r_table->region[REGION_TEXT].address[cnt] == 0xff) {
            cur_delta += 0xff;
            cnt++;
        }
        cur_delta += r_table->region[REGION_TEXT].address[cnt++];

        new = fixup_location(text_section + cur_delta, offset);

        if ((new < reloc_hdr.spl_hdr.r_text) || 
            (new > (reloc_hdr.spl_hdr.r_text + exec_header.a_text))) {
            printf("\nText ref %d/%d in text outside range ... %x at offset %x",
                   cnt, r_table->region[REGION_TEXT].count,
                   new, r_table->region[REGION_TEXT].address[cnt]);
            exit(1);
        }
    }

    r_table = &data_relocation;
    cur_delta = 0;
    for (cnt = 0; cnt < r_table->region[REGION_TEXT].count; ) {
        while (r_table->region[REGION_TEXT].address[cnt] == 0xff) {
            cur_delta += 0xff;
            cnt++;
        }
        cur_delta += r_table->region[REGION_TEXT].address[cnt++];

        new = fixup_location(data_section + cur_delta, offset);

        if ((new < reloc_hdr.spl_hdr.r_text) || 
            (new > (reloc_hdr.spl_hdr.r_text + exec_header.a_text))) {
            printf("\nText ref %d/%d in data outside range ... %x at offset %x",
                   cnt, r_table->region[REGION_TEXT].count,
                   new, r_table->region[REGION_TEXT].address[cnt]);
            exit(1);
        }
    }
}


/*
 * resolve_common_symbols - Resolve common symbols
 *
 * This routine resolves all of the common symbols into BSS, and looks
 * for any undefined symbols. It also handles the special case linker
 * symbols _edata, _etext, and _end.
 */

void resolve_common_symbols ()
{
    unsigned long index;
    unsigned long syms;
    struct nlist *symbol;
    char *symnam;
    int undefined = 0;

    syms = exec_header.a_syms / sizeof(struct nlist); /* Get symbol count */

    symbol = &symbol_table[0]; /* Get first symbol pointer */
    for (index = 0; index < syms; index++, symbol++) {
        symnam = &string_table[symbol->n_un.n_strx]; /* Get string pointer */

        /*
         * For now, just skip DBX records. In the future, we could get
         * module name and line number information, and give better
         * undefined symbol diagnostics.
         */

        if (symbol->n_type & N_STAB)
          continue;             /* Skip DBX records */

        /*
         * Look for our text segment checksum symbol.
         */
        if (symbol->n_un.n_strx != 0) {
            if (strcmp(symnam, TEXT_CHECKSUM_VARIABLE) == 0) {
                text_checksum_symbol = symbol;
                text_checksum_offset = symbol->n_value;
                printf("\nText checksum variable '%s' found at offset 0x%x\n",
                       symnam, symbol->n_value);
            }
        }
        /*
         * Look for symbols to allocate in BSS or undefined symbols.
         * If a value is specified, then this is a BSS allocation, otherwise
         * is is an undefined symbol.
         */

        if (symbol->n_type == (N_UNDF | N_EXT)) {
            if (symbol->n_value == 0) {

                /*
                 * If this is a special symbol, remember the pointer
                 * to it.
                 */

                if (strcmp("_edata", symnam) == 0) {
                    if (edata_ptr) {
                        fprintf(stderr, multi_defined_symbol, symnam);
                        exit(1);
                    }
                    edata_ptr = symnam; /* Remember _edata pointer */
                } else if (strcmp("_etext", symnam) == 0) {
                    if (etext_ptr) {
                        fprintf(stderr, multi_defined_symbol, symnam);
                        exit(1);
                    }
                    etext_ptr = symnam;
                } else if (strcmp("_end", symnam) == 0) {
                    if (end_ptr) {
                        fprintf(stderr, multi_defined_symbol, symnam);
                        exit(1);
                    }
                    end_ptr = symnam;
                } else if (strcmp("__stext", symnam) == 0) {
                    if (stext_ptr) {
                        fprintf(stderr, multi_defined_symbol, symnam);
                        exit(1);
                    }
                    stext_ptr = symnam;
                } else if (strcmp("__sdata", symnam) == 0) {
                    if (sdata_ptr) {
                        fprintf(stderr, multi_defined_symbol, symnam);
                        exit(1);
                    }
                    sdata_ptr = symnam;
                } else if (strcmp("__sbss", symnam) == 0) {
                    if (sbss_ptr) {
                        fprintf(stderr, multi_defined_symbol, symnam);
                        exit(1);
                    }
                    sbss_ptr = symnam;
                } else {
                    fprintf(stderr, "Undefined symbol %s\n", symnam);
                    undefined++;
                }
            } else {
                allocate_bss_object(symnam, symbol->n_value);
            }
        }
        if (debug_on)
          print_symbol_info(symbol);
    }

    if (undefined) {
        fprintf(stderr, "\n%d undefined symbols\n", undefined);
        exit(1);                        /* Fail if undefined symbols */
    }
}

add_special_symbol (struct nlist *symbol, char *symnam)
{
    /*
     * Add symbol name to string table. Bump up string table length.
     */
    symbol->n_un.n_strx = string_table_len;
    if (verbose_on)
        printf("\nNew symbol %s added to string_table", symnam);
    strcpy(&string_table[symbol->n_un.n_strx], symnam);
    string_table_len += (strlen(symnam) + 1);
    *(long *)string_table = string_table_len;
    /*
     * Bump up symbol table size in executable header
     */
    exec_header.a_syms += sizeof(struct nlist);
}

/*
 * fix_symbols - Update symbols in symbol table for creating .sun file.
 *
 * This routine updates the symbol values in the symbol table
 * as per the locations of the sections desired for the .sun file.
 */
void fix_symbols ()
{
    unsigned long index, len;
    unsigned long syms;
    struct nlist *symbol;
    char *symnam;
    struct bss_entry *entry;
    unsigned char type;

    syms = exec_header.a_syms / sizeof(struct nlist); /* Get symbol count */

    symbol = &symbol_table[0]; /* Get first symbol pointer */
    for (index = 0; index < syms; index++, symbol++) {
        symnam = &string_table[symbol->n_un.n_strx]; /* Get string pointer */

        /*
         * Look for symbols to allocate in BSS or undefined symbols.
         * If a value is specified, then this is a BSS allocation, otherwise
         * it is an undefined symbol.
         */

        type = symbol->n_type & N_TYPE;
        if (type == N_UNDF) {
            if (symbol->n_value != 0) {
                entry = find_bss_object(symnam, symbol->n_value);
		if (entry) {
		  symbol->n_value = final_bss_base + entry->address;
		  symbol->n_type  = (symbol->n_type & N_STAB) | 
		                    (N_EXT | N_BSS);
		}
            }
            else {
                if (!strcmp(symnam, "__stext")) {
                    symbol->n_value = final_text_base;
                    symbol->n_type  = (symbol->n_type & N_STAB) | 
		                      (N_TEXT | N_EXT);
                }
                else if (!strcmp(symnam, "_etext")) {
                    symbol->n_value = final_data_base;
                    symbol->n_type  = (symbol->n_type & N_STAB) | 
		                      (N_TEXT | N_EXT);
                }
                else if (!strcmp(symnam, "__sdata")) {
                    symbol->n_value = final_data_base;
                    symbol->n_type  = (symbol->n_type & N_STAB) | 
		                      (N_DATA | N_EXT);
                }
                else if (!strcmp(symnam, "_edata")) {
                    symbol->n_value = final_bss_base;
                    symbol->n_type  = (symbol->n_type & N_STAB) | 
		                      (N_DATA | N_EXT);
                }
                else if (!strcmp(symnam, "_end")) {
                    symbol->n_value = final_bss_base + next_bss_offset;
                    symbol->n_type  = (symbol->n_type & N_STAB) | 
		                      (N_BSS | N_EXT);
                }
            }
        }
        else if (type == N_TEXT) {
            ;
        }
        else if (type == N_DATA) {
            symbol->n_value += final_data_base;
	}
        else if (type == N_BSS) {
            symbol->n_value += 
	        (final_bss_base - saved_original_bss_base);
        }
        else {
            printf("\nUnfixed symbol - %s, type=%x, value=%x",
                   symnam, symbol->n_type, symbol->n_value);
        }
    }

    add_special_symbol(symbol, "__end");
    symbol->n_type = N_BSS | N_EXT;
    symbol->n_value = final_bss_base + next_bss_offset;
    symbol++;

    add_special_symbol(symbol, "__bss_start");
    symbol->n_type = N_BSS | N_EXT;
    symbol->n_value = final_bss_base;
}

/*
 *-------------------------- Write routines ----------------------------
 */

/*
 * write_relocation - Write the relocation information for a section
 *
 * This routine is used to write the relocation information for a
 * specified section.
 */
void write_relocation
  (struct relocation_table *reloc,
   FILE *outfile)
{
    enum REGIONS region;
    int status;

    /*
     * For each table, write the count, then write the relocation
     * amount.
     */
    for (region = REGION_TEXT; region < REGION_MAX; region++) {
        status = fwrite(&reloc->region[region].count, sizeof(long),
                        1, outfile);
        if (status != 1) {
            perror("Error writing output file relocation count");
            exit(1);
        }
        if (reloc->region[region].count) {
            status = fwrite(&reloc->region[region].address[0],
                            RELOC_OFFSET_SIZE,
                            reloc->region[region].count,
                            outfile);
            if (status != reloc->region[region].count) {
                perror("Error writing output file relocation data");
                exit(1);
            }
        }
    }
}


/*
 * Create our relocatable image. Do the actual file writing here.
 * Write 
 * 1. a.out header
 * 2. special header
 * 3. PIC
 * 4. relocation tables (first for text, then for data)
 * 5. text section
 * 6. data section
 */
void write_output_file (void)
{
    int status, jump_size;

    /*
     * Write out the file sections
     */

    /* Write a.out header */
    if (debug_on)
        printf("\nWriting a.out header...");
    status = fwrite(&out_header, sizeof(out_header), 1, outfile); 
    if (status != 1) {
        perror("\nError writing output file header\n");
        exit(1);
    }

    /* Write the special header */
    if (debug_on)
        printf("\nWriting special header...");
    status = fwrite(&reloc_hdr, sizeof(reloc_hdr), 1, outfile); 
    if (status != 1) {
        perror("\nError writing output file special header\n");
        exit(1);
    }

    /* 
     * Write our PIC program.
     * Subtract jump instructions from size because we will write 
     * them separately.
     */
     
    if (debug_on)
        printf("\nWriting PIC relocator...");
    jump_size = SHORT_BRA_SIZE + LONG_BRA_SIZE;
    status = fwrite(pic_text_ptr, pic_stat.st_size-jump_size, 1, outfile);
    if (status != 1) {
        perror("\nError writing output file special header\n");
        exit(1);
    }

    /*
     * Write our short branch instruction followed by any padding.
     */
    j_instr = SHORT_BRA_INSTR; /* the op code */
    status = fwrite(&j_instr, sizeof(short), 1, outfile); 
    j_instr = (pic_padding ? (pic_padding + 2) : 2); /* the offset */
    status = fwrite(&j_instr, sizeof(short), 1, outfile); 

    /*
     * Now write our padding (NULLs) that the previous jump should
     * skip over.
     */
    if (pic_padding) {
        status = fwrite(pad_block, pic_padding, 1, outfile);
    }

    /* Write the branch instruction at the end of our PIC code. */
    status = fwrite(&spl_hdr->j_instr.j_opcode, sizeof(short), 1, outfile); 
    j_offset = 4 + spl_hdr->r_rsize;
    status = fwrite(&j_offset, sizeof(long), 1, outfile); /* Write offset */

    /*
     * Write out the relocation information. There are two sections,
     * and three sub-sections in each.
     *
     *          Text section
     *                  text relocation information
     *                  data relocation information
     *                  bss relocation information
     *
     *          Data section
     *                  text relocation information
     *                  data relocation information
     *                  bss relocation information
     *
     * The relocation information consists of an initial longword length,
     * and then the data. If there is no relocation information for a given
     * type, then there is a longword of zero in the length field.
     */

    if (debug_on)
        printf("\nWriting relocation tables...");
    write_relocation(&text_relocation, outfile);
    write_relocation(&data_relocation, outfile);
     
    /*
     * Write any long-word alignment padding if we have to.
     */
    if (reloc_padding)
        status = fwrite(pad_block, reloc_padding, 1, outfile);

    /*
     * Write out the text & data sections
     */

    if (debug_on)
        printf("\nWriting text section...");
    status = fwrite(text_section, 1, exec_header.a_text, outfile);
    if (status != exec_header.a_text) {
        perror("\nError writing output file text segment\n");
        exit(1);
    }

    if (debug_on)
        printf("\nWriting data section...");
    status = fwrite(data_section, 1, exec_header.a_data, outfile);
    if (status != exec_header.a_data) {
        perror("\nError writing output file data segment\n");
        exit(1);
    }
}


/*
 * Process our PIC program.
 * Locate the NOP instruction at the end. The compiler may have
 * also added padding to align program length to a long word
 * boundary. We need to remove this padding and the NOP instr. 
 */
char locate_trailing_nop (char *pic_end)
{
    char *nop_ptr, cnt;
    short nop = 0;

    /* 
     * Search for the NOP instruction at the end of our
     * PIC program for relocation.
     */
    cnt = 2;
    nop_ptr = pic_end - cnt;  /* point to second last character */
    while (cnt < MAX_NOP_SKIP) {  /* NOP should come within 8 bytes from end */
        nop = *nop_ptr++;
        nop = ((nop << 8) & 0xFF00) | *nop_ptr;
        if (nop == NOP_INSTR)
            break;
        cnt++;
        nop_ptr -= 2;
    }
    if (nop == NOP_INSTR) {
        return (cnt);
    }
    else {
        printf("\nCould not find NOP instr at end of '%s'\n", picfile);
        exit(1);
    }
}

/*
 * Create the a.out header.
 */
void build_aout_header (void)
{
    /*
     * Build up the a.out header for our new relocatable image.
     */
    out_header.a_dynamic = 0;
    out_header.a_toolversion = 0;
    out_header.a_machtype = CISCOTYPE;
    out_header.a_magic = CISCOMAGIC;
    out_header.a_text = exec_header.a_text;
    out_header.a_data = exec_header.a_data;
    out_header.a_bss = next_bss_offset;
    out_header.a_syms = 0;
    out_header.a_entry = final_text_base; /* Set entry point */
    out_header.a_trsize = 0;    /* No text relocation info */
    out_header.a_drsize = 0;    /* No data relocation info */
}


/*
 * Create the special header.
 */
void build_spl_header (void)
{
    long reloc_size, size_upto_text;

    /*
     * Build up our special header.
     */

    spl_hdr->j_instr.j_opcode = LONG_BRA_INSTR;
    /*
     *        +----------------------+
     *        |                      | ] SUN_AOUT_HDR_SIZE = 32
     *        |    a.out hdr         | ] 
     *        +----------------------+
     *        |    bra <offset>      |   2 + 4 bytes
     *        +----------------------+
     *        |                      | ]
     *        |    spl hdr           | ] 512 - (32 + 4 + 2)
     *        |                      | ]
     *        |                      | ]
     *        +----------------------+
     */

    j_offset = (ALIGNED_BLOCK_SIZE - SUN_AOUT_HDR_SIZE - sizeof(short));

    spl_hdr->j_instr.j_offset_1 = (j_offset >> 16) & 0xffff;
    spl_hdr->j_instr.j_offset_2 = (j_offset << 16) & 0xffff0000;

    spl_hdr->r_dynamic = 1;
    spl_hdr->r_toolversion = 0;
    spl_hdr->r_machtype = CISCOTYPE;
    spl_hdr->r_magic = RELOC_MAGIC1;
    spl_hdr->r_version = RELOC_IMAGE_VER;

    spl_hdr->r_hdr_len = RELOC_HEADER_LEN;

    spl_hdr->r_text  = final_text_base;
    spl_hdr->r_data  = final_data_base;
    spl_hdr->r_tsize = exec_header.a_text;
    spl_hdr->r_dsize = exec_header.a_data;
    spl_hdr->r_bsize = next_bss_offset;

    /*
     * Extend the length of the text segment by the total size
     * of the relocation data, relocation code & special header.
     */

    reloc_size = 
      ((text_relocation.region[REGION_TEXT].count +
       text_relocation.region[REGION_DATA].count +
       text_relocation.region[REGION_BSS].count +
       
       data_relocation.region[REGION_TEXT].count +
       data_relocation.region[REGION_DATA].count +
       data_relocation.region[REGION_BSS].count ) * RELOC_OFFSET_SIZE);
    
    reloc_size += (6 * sizeof(long)); /* 6 counts, one for each table */
    reloc_padding = reloc_size % sizeof(long);
    if (reloc_padding > 0) {  /* Not long word aligned */
        reloc_padding = sizeof(long) - reloc_padding;
        if (verbose_on)
            printf("\nRelocation info size = %d, "
		   "padding %d bytes for long-word alignment",
                   reloc_size, reloc_padding);
    }
    spl_hdr->r_rsize = reloc_size + reloc_padding; /* relocation info size */

    /*
     * size_upto_text covers
     *    1. a.out header
     *    2. special header
     *    3. relocation program
     *    4. relocation tables
     * a.out header + spl header = ALIGNED_BLOCK_SIZE = 512 bytes
     */
    size_upto_text = ALIGNED_BLOCK_SIZE + spl_hdr->r_pgmsize + 
                     spl_hdr->r_rsize;

    spl_hdr->r_def_text = final_text_base;
    spl_hdr->r_def_data = final_data_base;
    spl_hdr->r_def_ram_start = final_data_base;

    spl_hdr->r_text_offset = size_upto_text;
    spl_hdr->r_data_offset = spl_hdr->r_text_offset +
                             spl_hdr->r_tsize;

    spl_hdr->r_def_text_cksum = 0xFFFFFFFF;
    spl_hdr->r_rfr_text_cksum = 0xFFFFFFFF;
    spl_hdr->r_text_cksum_offset = 0xFFFFFFFF;
    spl_hdr->r_text_cksum_offset_in_data = 0xFFFFFFFF;
    if (text_checksum_symbol != NULL) { /* variable found ! */
        spl_hdr->r_text_cksum_offset_in_data = text_checksum_offset;
        spl_hdr->r_text_cksum_offset = spl_hdr->r_data_offset +
                                       text_checksum_offset;
    }

    spl_hdr->r_image_cksum = 0xFFFFFFFF;
    spl_hdr->r_def_image_cksum = 0xFFFFFFFF;

    spl_hdr->r_hdr_cksum = 0xFFFFFFFF;

    spl_hdr->r_text_cksum_algor = TC_ALGORITHM;
    spl_hdr->r_image_cksum_algor = IC_ALGORITHM;
    spl_hdr->r_hdr_cksum_algor = SHC_ALGORITHM;

    spl_hdr->r_platform_family = PLATFORM_FAMTYPE;

    /*
     * Some final adjustments ...
     * Bump up the text segment size in the a.out header
     * AND the base link address for the text section.
     */
    size_upto_text -= SUN_AOUT_HDR_SIZE;
    out_header.a_text += size_upto_text;
    spl_hdr->r_text += size_upto_text;
}


/*
 * create_relocatable_image - 
 *     Write an a.out file. Create our relocatable image.
 *
 * In order to remain compatible with old ROMs and netbooting, we write
 * out a standard format a.out file. The relocation information appears
 * to be in the text segment for ROMs loading the system.
 */

#define MALLOC_MARGIN   16

void create_relocatable_image (char *name, char *picfile)
{
    int status;
    short padding;

    /*
     * Open output file for writing.
     */
    outfile = fopen(name, "w");
    if (!outfile) {
        perror("\nError opening output file\n");
        exit(1);
    }

    /* 
     * Find out if the relocation program exists; if yes, then
     * its size. Allocate a buffer to read in the file.
     */
    if ((picfile == NULL) || (strlen(picfile) == 0)) {
        printf("PIC file unspecified. Using 'relocate' from .");
        picfile = DEFAULT_RELOCATION_PROGRAM;
    }
    pic_stat.st_size = 0;
    status = stat(picfile, &pic_stat);
    if (pic_stat.st_size) 
        pic_ptr = (char *)malloc(pic_stat.st_size + MALLOC_MARGIN);
    else {
        fprintf(stderr, "%s program not found or of zero size", picfile);
        exit(1);
    }

    /* 
     * Open and read in our PIC file.
     */
    if (debug_on)
        printf("\nReading PIC relocator...");
    inpfile = fopen(picfile, "r");
    if (!inpfile) {
        perror("\nError opening input file\n");
        exit(1);
    }
    status = fread(pic_ptr, pic_stat.st_size, 1, inpfile);
    fclose(inpfile);

    /*
     * Set pointer to start of text section of program.
     */
    pic_text_ptr = pic_ptr + SUN_AOUT_HDR_SIZE;

    /*
     * Warning!!
     * Kludgy stuff follows. May be injurious to health.
     *
     * Locate the trailing NOP instruction. We will remove it
     * and replace it with two jump instructions.
     * The first jump will account for any long word alignment
     * padding that may be required. It will cause a jump to the
     * second jump instruction across any such padding bytes.
     * The second jump will cross the relocation tables to
     * jump to the text section.
     */

    padding = locate_trailing_nop(pic_ptr + pic_stat.st_size);
    pic_stat.st_size -= padding; /* Chop off from NOP onwards */

    /*
     * Now assume we are adding a long branch and a short branch
     * instr. The long bra is 6 bytes long, the short bra is 4.
     * So add (6 + 4 = 10) to the PIC length.
     */

    pic_stat.st_size += (SHORT_BRA_SIZE + LONG_BRA_SIZE);

    /* Reduce file size by a.out header size since we are
     * going to strip it off
     */

    pic_stat.st_size -= SUN_AOUT_HDR_SIZE; 

    /*
     * Now see if we need to pad further to make the pic program
     * length a multiple of a long word.
     */

    pic_padding = pic_stat.st_size % sizeof(long);
    if (pic_padding > 0) { /* Not long aligned */
        pic_padding = sizeof(long) - pic_padding;
        if (verbose_on)
            printf("\nPIC size = %d, padding %d bytes for long-word alignment",
                   pic_stat.st_size, pic_padding);
    }

    /*
     * Build up the a.out header for our new relocatable image.
     */
    build_aout_header();

    /*
     * Build up our special header.
     */
    reloc_hdr.spl_hdr_len = RELOC_HEADER_LEN;
    spl_hdr = &reloc_hdr.spl_hdr;  /* set pointer to struct */
    spl_hdr->r_pgmsize = pic_stat.st_size + pic_padding; 
    build_spl_header();

    /*
     * Since we do not know our relocation table size, we
     * relocate to some pre-default value. Then after we have
     * built our relocation table, we re-relocate to the 
     * desired address.
     */
    if (debug_on)
        printf("\nRe-relocating text after building relocation tables...");
    re_relocate_text();

    write_output_file();  /* Create our image file */

    free(pic_ptr);

    if (verbose_on)
        printf("\n... Relocatable image created\n");
}

create_sun_file (void)
{
    char *srcptr, *dstptr;
    int status;

    printf("\nCreating temporary .sun file (%s) ...", sunfilenm);
    /*
     * Update a.out header so that
     * a_trsize = a_drsize = 0
     * entry point = 0
     */
    exec_header.a_text   = spl_hdr->r_tsize;
    exec_header.a_entry  = 0; /* Set entry point */

    /*
     * Relocate so that text start = 0, data starts after
     * text, and bss follows data.
     */
    original_text_base = final_text_base;
    original_data_base = final_data_base;
    original_bss_base  = final_bss_base;
    final_text_base    = 0;
    final_data_base    = final_text_base + exec_header.a_text;
    final_bss_base     = final_data_base + exec_header.a_data;

    relocate(text_reloc, exec_header.a_trsize, "text",
             text_section, exec_header.a_text,
             final_text_base - original_text_base,
             &text_relocation, FALSE);

    relocate(data_reloc, exec_header.a_drsize, "data",
             data_section, exec_header.a_data,
             final_data_base - original_data_base,
             &data_relocation, FALSE);

    exec_header.a_bss    = next_bss_offset;
    exec_header.a_trsize = 0;    /* No text relocation info */
    exec_header.a_drsize = 0;    /* No data relocation info */

    /*
     * Fix up symbol table for global bss symbols
     */
    fix_symbols();

    /* 
     * Open the output .sun file. It's actually a .temp
     * file that will get renamed to a .sun.
     */
    sunfile = fopen(sunfilenm, "w"); 
    if (!sunfile) {
        perror("\nError opening .sun file for writing");
        exit(1);
    }

    /*
     * Write out the file sections
     */

    /* Write a.out header */
    status = fwrite(&exec_header, sizeof(exec_header), 1, sunfile); 
    if (status != 1) {
        perror("\nError writing output .sun file header\n");
        exit(1);
    }

    /*
     * Write out the text & data sections
     */

    status = fwrite(text_section, 1, exec_header.a_text, sunfile);
    if (status != exec_header.a_text) {
        perror("\nError writing output .sun file text segment\n");
        exit(1);
    }

    status = fwrite(data_section, 1, exec_header.a_data, sunfile);
    if (status != exec_header.a_data) {
        perror("\nError writing output .sun file data segment\n");
        exit(1);
    }

    status = fwrite(symbol_table, 1, exec_header.a_syms, sunfile);
    if (status != exec_header.a_syms) {
        perror("\nError writing output .sun file symbol table\n");
        exit(1);
    }

    status = fwrite(string_table, 1, string_table_len, sunfile);
    if (status != string_table_len) {
        perror("\nError writing output .sun file string table\n");
        exit(1);
    }

    fclose(sunfile);

    printf("\n... %s created\n\n", sunfilenm);
}

/*
 *-------------------------- Misc routines ------------------------------
 */

void usage (name)
     char *name;
{
    fprintf(stderr,
            "Usage:\n"
            "  %s [-text <base>] [-data <base>] [-v] [-/d]"
            " inpfile PICfile outfile .sun-file\n"
            "   <base> is specified in hexadecimal w/o a leading 0x.\n"
            "   The default text base is %x.\n"
            "   The default data base is %x.\n"
            "   -v is for verbose output\n"
            "   -/d is for detailed debug output\n",
            name,
            DEFAULT_TEXT_BASE,
            DEFAULT_DATA_BASE);
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
 * We pad the following :
 * 1. The PIC program. Padding is done by adding a NOP instruction
 *    at the 'right' place to make the program length long-word 
 *    aligned.
 * 2. The relocation tables. Padding is done by adding zeroes to make the
 *    total length a long-word multiple.
 * In addition, we set the unused bytes in the special header to FFs.
 */
void init_padding (void)
{
    int i;
    unsigned char *pad_ptr;

    pad_nop = NOP_INSTR;
    for (i = 0; i < PAD_BLOCK_SIZE; i++)
        pad_block[i] = 0;

    pad_ptr = (unsigned char *)&reloc_hdr.r_padding;
    for (i = 0; i < ALIGN_TO_512_BYTE_BOUNDARY; i++)
        *pad_ptr++ = 0xFF;
}


/*
 * main - WE START HERE.  Handle the user interface
 */
int main (int argc, char **argv)
{
    int argument;
    char *arg_key;
    int seen_data_base = FALSE; /* Flag - if seen data base */
    int seen_text_base = FALSE; /* Flag - if seen text base */

    /*
     *-------------------- Init stuff --------------------------
     */
    init_padding();
   
    /*
     * Set parameter defaults
     */

    original_text_base = 0;     /* We link it this way */
    original_data_base = 0;     /* We link it this way */

    /*
     * We will link up the image such that the text section
     * is in flash address space, and the data section starts 
     * in DRAM.
     */
    final_text_base = DEFAULT_TEXT_BASE;
    final_data_base = DEFAULT_DATA_BASE;

    /*
     *------------------- Process command line args ------------
     */
    printf("%s", version);
    for (argument = 1; argument < argc; argument++) {
        arg_key = argv[argument];
        if (*arg_key == '-') {
            arg_key++;
            if ((strcmp(arg_key, "text") == 0) ||
                (tolower(*arg_key) == 't')) {
                final_text_base = parse_hex(argv[++argument]);
                seen_text_base = TRUE;
            } else if ((strcmp(arg_key, "data") == 0) ||
                (tolower(*arg_key) == 'd')) {
                final_data_base = parse_hex(argv[++argument]);
                seen_data_base = TRUE;
            } else if (tolower(*arg_key) == 'v') {
                verbose_on = 1;
            } else if ((*arg_key == '/') && (*(arg_key + 1) == 'd')) {
                debug_on = 1;
                verbose_on = 1;
            } else break;               /* Unknown */
        }
        else
            break;
    }

    if ((argument + 4) != argc)
      usage(argv[0]);           /* Print error if unknown arguments */

    inpfilenm = argv[argument++];
    picfilenm = argv[argument++];
    outfilenm = argv[argument++];
    sunfilenm = argv[argument];

    /*
     *----------------- Open and read input file --------------------
     */
    inpfile = fopen(inpfilenm, "r"); /* Open the input file */
    if (!inpfile) {
        perror("\nError opening input file");
        exit(1);
    }

    read_file_sections();

    /*
     *------------ Calculate placement of image sections --------------
     *
     * The linker has placed the text segment at zero, the data segment
     * at zero, and BSS following that.
     *
     * We relocate the image to the user requested location. 
     * Always place the BSS after data.
     */

    original_bss_base = original_data_base + exec_header.a_data;
    saved_original_bss_base = original_bss_base;
    if (seen_text_base) {
        if (!seen_data_base) {
            final_data_base = final_text_base + exec_header.a_text;
            final_bss_base = final_data_base + exec_header.a_data;
        }
        else {
            final_bss_base = final_data_base + exec_header.a_data;
        }
    }
    else { /* text base not specified */
      final_bss_base = final_data_base + exec_header.a_data;
  }

    /*
     * Set the start of our BSS allocation. This should follow any BSS
     * allocated by the linker.
     */

    next_bss_offset = (exec_header.a_bss + BSS_OBJECT_ALIGN)
                      & ~BSS_OBJECT_ALIGN;
    next_bss_entry = 0;

    /*
     *------------ Resolve the common (BSS) symbols ------------------
     */
    if (verbose_on)
        printf("\n\nResolving common globals; creating relocatable...");
    resolve_common_symbols();

    /*
     *-------------------- Relocate the segments ---------------------
     */
    relocate(text_reloc, exec_header.a_trsize, "text",
             text_section, exec_header.a_text,
             final_text_base - original_text_base,
             &text_relocation, TRUE);

    relocate(data_reloc, exec_header.a_drsize, "data",
             data_section, exec_header.a_data,
             final_data_base - original_data_base,
             &data_relocation, TRUE);

    /*
     *----------------- Create executable output --------------------
     */
    create_relocatable_image(outfilenm, picfilenm);

    fclose(outfile);
    print_closing_stats();

    /*
     *------------------ Create .sun file ---------------------------
     */
    create_sun_file();

    exit(0);                    /* Success */
}
