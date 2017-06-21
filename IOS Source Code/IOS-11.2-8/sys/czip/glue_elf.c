/* $Id: glue_elf.c,v 3.2.60.1 1996/06/28 23:05:25 hampton Exp $
 * $Source: /release/112/cvs/Xsys/czip/glue_elf.c,v $
 *------------------------------------------------------------------
 * glue_elf.c - glue a zipped router image to the extractor
 *
 * May 1995, Chiung-Hsi Jonathan Lin 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: glue_elf.c,v $
 * Revision 3.2.60.1  1996/06/28  23:05:25  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:58:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/27  18:01:10  jchlin
 * It can unzip now.
 * CSCdi35299:  unable to unzip a zipped r4k executable
 *
 * Revision 2.1  1995/06/07  20:22:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files 
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../h/elf.h"
#include "../os/sum_magic_number.h"
/*
 * Definitions
 */
#define TRUE  1
#define FALSE 0

#define ZIP_COMMAND     "zip"
#define UNZIP_COMMAND	"unzip"

/*
 * ELF header macros
 */
#define ELFPHDRSZ       	sizeof(Elf32_Phdr)
#define ELF_EHDR(image) 	(*(Elf32_Ehdr *) image)

#define ELF_PHDR(image) 					\
    ((Elf32_Phdr *) (((unsigned char*) image) 			\
		     + ((Elf32_Ehdr*)image)->e_phoff))

#define ELF_SHDR(image) 					\
    ((Elf32_Shdr *) (((unsigned char*) image) 			\
		     + ((Elf32_Ehdr*)image)->e_shoff))

#define ELF_SEGMENT(image) 					\
    ((unsigned char*) image + ELF_PHDR(image)->p_offset)

#define SHDR_NOBIT(sh_src) 					\
    (sh_src.sh_type && (sh_src.sh_type != SHT_NOBITS) && sh_src.sh_size)

#define SHDR_PROGBITS(sh_src) 					\
    (sh_src.sh_type == SHT_PROGBITS && sh_src.sh_size)

#define NOT_LONGWORD_ALIGNED(value) (value & 0x03)

/*
 * Structure definitions
 */
typedef struct {
  unsigned long magic_id;
  unsigned long uncomp_size;
  unsigned long comp_size;
  unsigned long comp_checksum;
  unsigned long uncomp_checksum;
} MagicHdrType;

typedef struct {
  char *name;
  char *buffer;
  char freeBuffer;
  long size;
  MagicHdrType *magic_hdr;  
} FileInfoType;


typedef enum {
  CMD_NONE,
  CMD_COMPRESS,
  CMD_DECOMPRESS
} CommandType;
/*
 * Globals
 */
extern char *optarg;
extern int optind;

FileInfoType router_file;
FileInfoType extract_file;
FileInfoType glued_file;
MagicHdrType magic_hdr;

unsigned long magic_variable = MAGIC_VALUE;
char cmd_buf[80], router_comp_name[80];
int debug;

/*
 * usage: 
 * Print out usage message
 */ 
void usage (char *name)
{
    fprintf(stderr, "\n\n\nUsage:  %s [-arguments]\n", name);
    fprintf(stderr, "\t -r <input router image>\n");
    fprintf(stderr, "\t -z <self-extractor>\n");
    fprintf(stderr, "\t -o <output filename>\n");
    fprintf(stderr, "\t -u <unzip the image>\n");
    fprintf(stderr, "\t -d (enable debug information)\n");
    fprintf(stderr, "\n\tglue_elf -r c4500-k-m -z czip_elf -o c4500-k-mz");
    fprintf(stderr, "\n\tglue_elf -u -r c4500-k-mz -o c4500-k-mz\n\n");
    exit(1);
}

/*
 * command_line :
 * Parse command line arguments
 */
CommandType command_line (int argc, char **argv) 
{
    int c;
    CommandType cmd;

    debug = FALSE;
    cmd   = CMD_NONE;
    router_file.name = extract_file.name = glued_file.name = NULL;

    while ((c = getopt(argc, argv, "ur:z:o:d")) != -1) {
	switch (c) {
	case 'u': cmd = CMD_DECOMPRESS; break;
	case 'r': router_file.name = optarg;  break;
	case 'z': extract_file.name = optarg; cmd = CMD_COMPRESS; break;
	case 'o': glued_file.name = optarg; break;
	case 'd': debug = TRUE; break;

	default : usage(argv[0]); break;
        }
    }

    if (debug) {
        if (router_file.name != NULL)
            fprintf(stderr, "\nrouter name    : %s\n", router_file.name);
        if (extract_file.name != NULL)
            fprintf(stderr, "extract name   : %s\n", extract_file.name);
        if (glued_file.name != NULL)
            fprintf(stderr, "output name    : %s\n\n", glued_file.name);
    }

    return cmd;
}
 
/*
 * read_image:
 * Open an ELF executable and read the whole image into a buffer
 */ 
void read_image (FileInfoType *image)
{
    FILE    *fd;
    struct  stat stat_buf;
    int     infd;
     
    /*
     * Open the router input file
     */
    fd = fopen(image->name, "rb");
    if (!fd) {
	fprintf(stderr, "Error : failed to open %s for reading\n", 
		image->name);
	exit(1);
    }

    /*
     * Get the router input file size
     */
    infd = fileno(fd);
    fstat(infd, &stat_buf);
    image->size = stat_buf.st_size;

    if (image->size <= 0) {
	fprintf(stderr, "Error : file size of %s is %d\n",
		image->name, image->size);
	exit(1);
    }

    if (debug) {
	fprintf(stderr, "\nReading size of image %s : 0x%x bytes\n",
		image->name, image->size);
    }

    /*
     * Allocate a buffer to store the image 
     */
    image->buffer = malloc(image->size);

    if (!image->buffer) {
	fprintf(stderr, "Error : failed to malloc space\n");
	exit(1);
    }

    if (read(infd, image->buffer, image->size) != image->size) {
	fprintf(stderr, "Error : failed to read image %s\n", 
                image->name);
	exit(1);
    }

    fclose(fd);
}

/*
 * save_image :
 *   If image2 is null, only the image1 is saved.
 *   If image2 is no null, save the image1 first, then magic header,
 *      and then the compressed router binary in image2.
 */
void save_image (FileInfoType *image1, FileInfoType *image2)
{
    FILE    *fd;
    struct  stat stat_buf;
    int     outfd, padding;
    char    pad_bytes[4] = { 0, 0, 0, 0 };

    if (!image1) {
        fprintf(stderr, "Error : image1 in save_image() has a null pointer\n");
        exit(1);
    }

    /*
     * Open the router input file
     */
    fd = fopen(image1->name, "wb");
    if (!fd) {
	fprintf(stderr, "Error : failed to open %s for reading\n", 
		image1->name);
	exit(1);
    }

    /*
     * Get the router input file size
     */
    outfd = fileno(fd);
    if (image1->size <= 0) {
	fprintf(stderr, "Error : file size of %s is %d\n",
		image1->name, image1->size);
	exit(1);
    }

    if (debug) {
	fprintf(stderr, "Writing size of image %s : 0x%x bytes\n\n",
		image1->name, image1->size + (image2? image2->size : 0));
    }

    /*
     * Write the uncompressed router image
     */
    if (!image1->buffer) {
	fprintf(stderr, "Error : buffer is null when writing %s \n", 
                image1->name);
	exit(1);
    }
    if (write(outfd, image1->buffer, image1->size) != image1->size) {
	fprintf(stderr, "Error : failed to write image %s\n", 
                image1->name);
	exit(1);
    }

    if (image2) {
        if (!image2->buffer) {
	    fprintf(stderr, "Error : buffer is null when writing %s \n", 
		    image2->name);
	    exit(1);
        }

        if (write(outfd, image2->magic_hdr, sizeof(MagicHdrType)) 
            != sizeof(MagicHdrType)) {
	    fprintf(stderr, "Error : failed to write %s's magic header\n", 
		    image2->name);
	    exit(1);
        }

        if (debug) {
            printf("\tmagic_id        = 0x%x\n", image2->magic_hdr->magic_id);
            printf("\tuncomp_size     = 0x%x\n", 
		   image2->magic_hdr->uncomp_size);
            printf("\tcomp_size       = 0x%x\n", image2->magic_hdr->comp_size);
            printf("\tcomp_checksum   = 0x%x\n", 
                   image2->magic_hdr->comp_checksum);
            printf("\tuncomp_checksum = 0x%x\n\n", 
                   image2->magic_hdr->uncomp_checksum);
        }

        if (write(outfd, image2->buffer, image2->magic_hdr->comp_size)
            != image2->magic_hdr->comp_size) {
	    fprintf(stderr, "Error : failed to write compressed image %s \n" ,
		    image2->name);
	    exit(1);
        }

        if (NOT_LONGWORD_ALIGNED(image2->magic_hdr->comp_size)) {
            padding = 4 - NOT_LONGWORD_ALIGNED(image2->magic_hdr->comp_size);
            if (write(outfd, pad_bytes, padding) != padding) {
                fprintf(stderr, "Error : failed to write the padding bytes "
                        "in the image %s\n", image2->name);
                exit(1);
            }
        }
        
        if (image2->freeBuffer) {
            free(image2->buffer);
            image2->buffer = NULL;
        }
    }

    fclose(fd);

    if (image1->freeBuffer) {
        free(image1->buffer);
        image1->buffer = NULL;
    }
}

/*
 * verify_elf_file
 *    Verify the elf header for the image
 */
void verify_elf_file (FileInfoType *image)
{
    Elf32_Ehdr *eh;

    eh = (Elf32_Ehdr *) image->buffer;

    if (eh->e_ehsize != ELFEHDRSZ) {
	fprintf(stderr, "Error : ELF header size not sizeof(Elf32_Ehdr) "
                "in file %s\n", image->name);
	exit(1);
    }
    
    if (eh->e_phentsize != ELFPHDRSZ) {
	fprintf(stderr, "Error : Pgm header size not sizeof(Elf32_Phdr) "
                "in file %s\n", image->name);
	exit(1);
    }
    
    if (eh->e_type != ET_EXEC) {
	fprintf(stderr, "Error: ELF file %s not an executable\n", image->name);
	exit(1);
    }
    
    if (eh->e_phnum != 1) {
	fprintf(stderr, "Error: More than one program header in ELF file "
                "%s\n", image->name);
	exit(1);
    }
}

/*
 * image_checksum : 
 * Run the checksummer
 */
unsigned long image_checksum (FileInfoType *image)
{
    unsigned long word_count, checksum;
    unsigned char *bufp;
    
    printf("Calculate the checksum of image %s\n", image->name);
    
    checksum = 0;
    word_count = image->size / sizeof(unsigned long);
    bufp = image->buffer;
    
    while (word_count--) {
	ADD_AROUND(*(bufp++), checksum);
    }
    printf("Checksum of %s is 0x%x\n", image->name, checksum);
    return(checksum);
}


/*
 * extract_zip_image 
 *    Extract the compressed image out of the glued image
 *    and decompress the extracted image
 */
void extract_zip_image (FileInfoType *srcImage, FileInfoType *destImage)
{
    int temp;
    Elf32_Ehdr *eh_src;
    Elf32_Shdr *sh_src;
    Elf32_Half img_shnum;
    char       *srcp, new_buffer;
    char       tempfile[80], *name_saved;
    
    srcp  = srcImage->buffer;
    eh_src = (Elf32_Ehdr *) srcp;
    sh_src = ELF_SHDR(srcp);

    verify_elf_file (srcImage);

    /*
     * Get the index of the section header for the section that has the
     * compressed router image. The image is in the last section of the 
     * glued image.
     */
    img_shnum = eh_src->e_shnum - 1;

    /*
     * read back the magic header
     */
    memcpy ((unsigned char *) srcImage->magic_hdr, 
            (unsigned char *) (srcp + sh_src[img_shnum].sh_offset),
            sizeof(MagicHdrType));

    /*
     * restore the zip image file information
     */
    destImage->freeBuffer = FALSE;
    destImage->buffer = srcp + sh_src[img_shnum].sh_offset 
                             + sizeof(MagicHdrType);
    destImage->size = srcImage->magic_hdr->comp_size;

    if (srcImage->magic_hdr->magic_id != MAGIC_VALUE) {
        fprintf(stderr, "Error: The compressed image %s doesn't have the "
                "correct magic id.\n",
                srcImage->name);
        exit(1);
    }

    if (srcImage->magic_hdr->comp_checksum != image_checksum(destImage)) {
        fprintf(stderr, "Error: The compressed image %s doesn't checksum.\n",
                srcImage->name);
        exit(1);
    }

    /*
     * save the zip image
     */
    sprintf(tempfile, "%s.zip", destImage->name);
    name_saved = destImage->name;
    destImage->name = tempfile;
    save_image(destImage, NULL);
    destImage->name = name_saved;

    /*
     * unzip the image
     */
    sprintf(cmd_buf, "%s -p %s > %s", UNZIP_COMMAND, tempfile,
            destImage->name);
    printf("Decompressing %s using %s\n", tempfile, UNZIP_COMMAND);
    temp = system(cmd_buf);

    if (temp != 0) {
        fprintf(stderr, "Error : system call to %s failed\n",
                UNZIP_COMMAND);
        exit(1);
    }

    /*
     * Remove the temp file
     */
    temp = remove(tempfile);
    if (temp != 0) {
        fprintf(stderr, "Error : failed to remove %s\n", tempfile);
        exit(1);
    } else
        printf("Remove the zipped file %s\n", tempfile);

    free(srcImage->buffer);
    srcImage->buffer = destImage->buffer = NULL;
    destImage->freeBuffer = TRUE;

    
}

/*
 * squish_elf_file : Squish an ELF file
 */
void squish_elf_file (FileInfoType *image, int append_router_image)
{
    Elf32_Ehdr *eh_src, *eh_dest;
    Elf32_Phdr *ph_src, *ph_dest;
    Elf32_Shdr *sh_src, *sh_dest;
    Elf32_Half ix, shnum, img_shnum;
    Elf32_Off  seg_dest;
    Elf32_Word padding;
    int discard;
    char *new_buffer, *srcp, *destp;
    unsigned long *long_word;

    new_buffer = malloc(image->size + 100);
    if (!new_buffer) {
	fprintf(stderr, "Error: failed to malloc for squish %s.\n",
                image->name);
	exit(1);
    }
    
    srcp  = image->buffer;
    eh_src = (Elf32_Ehdr *) srcp;
    destp = new_buffer;
    eh_dest = (Elf32_Ehdr *) destp;

    verify_elf_file (image);

    /*
     * Copy the old ELF header to the new ELF header
     */
    memcpy((unsigned char*) eh_dest, (unsigned char*) eh_src, ELFEHDRSZ);
    
    /*
     * The program header will go directly after the ELF header,
     * and the section headers will go after that.
     */
    eh_dest->e_phoff = ELFEHDRSZ;
    eh_dest->e_shoff = eh_dest->e_phoff + ELFPHDRSZ;
    
    ph_src  = ELF_PHDR(srcp);
    ph_dest = ELF_PHDR(destp);
    
    sh_src = ELF_SHDR(srcp);
    sh_dest = ELF_SHDR(destp);
    
    /*
     * Copy the old program header to the new program header
     */
    memcpy((unsigned char *) ph_dest, (unsigned char *) ph_src, ELFPHDRSZ);

    /*
     * Process the section headers and sections
     */
    for (ix=shnum=0,padding=0; ix < eh_src->e_shnum; ix++) {
        if (SHDR_PROGBITS(sh_src[ix])) {
            memcpy((unsigned char *) &sh_dest[shnum++],
                   (unsigned char *) &sh_src[ix], 
                   ELFSHDRSZ);

            if (NOT_LONGWORD_ALIGNED(sh_src[ix].sh_size)) {
              padding += (4 - NOT_LONGWORD_ALIGNED(sh_src[ix].sh_size));
            }
        }
    }

    /*
     * Create a new section header for the compressed router image
     * and make sure that the section size is in the long word boundary
     */    
    if (append_router_image) {
        /*
         * Increment the section header count and get the index of the 
         * section header for the compressed router image.
         */
        shnum++;
        img_shnum = shnum - 1;

        /*
         * Copy over header from previous section header as a template.
         */
        memcpy((unsigned char *) &sh_dest[img_shnum],
               (unsigned char *) &sh_dest[img_shnum-1],
               ELFSHDRSZ);

        sh_dest[img_shnum].sh_size = sizeof(MagicHdrType) + 
	    image->magic_hdr->comp_size;

        if (NOT_LONGWORD_ALIGNED(sh_dest[img_shnum].sh_size)) {
            sh_dest[img_shnum].sh_size += 
                       (4 - NOT_LONGWORD_ALIGNED(sh_dest[img_shnum].sh_size));
        }

        ph_dest->p_filesz += sh_dest[img_shnum].sh_size;
        ph_dest->p_memsz  += sh_dest[img_shnum].sh_size;
    }

    eh_dest->e_shnum = shnum;
    ph_dest->p_offset = eh_dest->e_shoff + (shnum * ELFSHDRSZ);
    ph_dest->p_align = ph_dest->p_offset;
    ph_dest->p_filesz += padding;
    ph_dest->p_memsz  += padding;

    if (debug) {
        printf("%s's old ELF Header : \n", image->name);
        printf("e_type : 0x%x, e_machine : 0x%x, e_version : 0x%x\n",
               eh_src->e_type, eh_src->e_machine, eh_src->e_version);
        printf("e_entry : 0x%x, e_phoff : 0x%x, e_shoff : 0x%x\n",
               eh_src->e_entry, eh_src->e_phoff, eh_src->e_shoff);
        printf("e_flags : 0x%x, e_ehsize : 0x%x, e_phentsize : 0x%x\n",
               eh_src->e_flags, eh_src->e_ehsize, eh_src->e_phentsize);
        printf("e_phnum : 0x%x, e_shentsize : 0x%x, e_shstrndx : 0x%x\n",
               eh_src->e_phnum, eh_src->e_shentsize, eh_src->e_shstrndx);
        printf("e_shnum : 0x%x\n", eh_src->e_shnum);        

        printf("%s's new ELF Header : \n", image->name);
        printf("e_type : 0x%x, e_machine : 0x%x, e_version : 0x%x\n",
               eh_dest->e_type, eh_dest->e_machine, eh_dest->e_version);
        printf("e_entry : 0x%x, e_phoff : 0x%x, e_shoff : 0x%x\n",
               eh_dest->e_entry, eh_dest->e_phoff, eh_dest->e_shoff);
        printf("e_flags : 0x%x, e_ehsize : 0x%x, e_phentsize : 0x%x\n",
               eh_dest->e_flags, eh_dest->e_ehsize, eh_dest->e_phentsize);
        printf("e_phnum : 0x%x, e_shentsize : 0x%x, e_shstrndx : 0x%x\n",
               eh_dest->e_phnum, eh_dest->e_shentsize, eh_dest->e_shstrndx);
        printf("e_shnum : 0x%x\n\n", eh_dest->e_shnum);        

        printf ("%s's old Program Header :\n", image->name);
        printf ("%s%s", "Type Offset    Vaddr     Paddr      ",
                " MemSz   FileSz  Flags\n");
        printf ("%03x  %08x  %08x  %08x  %8x  %6x  %6x\n\n",
                ph_src->p_type, ph_src->p_offset,
                ph_src->p_vaddr, ph_src->p_paddr,
                ph_src->p_memsz, ph_src->p_filesz,
                ph_src->p_flags);
	
        printf ("%s's new Program Header :\n", image->name);
        printf ("%s%s", "Type Offset    Vaddr     Paddr      ",
                " MemSz   FileSz  Flags\n");
        printf ("%03x  %08x  %08x  %08x  %8x  %6x  %6x\n\n",
                ph_dest->p_type, ph_dest->p_offset,
                ph_dest->p_vaddr, ph_dest->p_paddr,
                ph_dest->p_memsz, ph_dest->p_filesz,
                ph_dest->p_flags);
    }

    /*
     * Copy those sections containing codes into the new sections
     */
    for (ix=shnum=0, seg_dest=ph_dest->p_offset; ix < eh_src->e_shnum; ix++) {
        if (SHDR_PROGBITS(sh_src[ix])) {
            /*
             * Make sure the size of on the long word boundary
             */
            if (NOT_LONGWORD_ALIGNED(sh_src[ix].sh_size)) {
                padding = NOT_LONGWORD_ALIGNED(sh_src[ix].sh_size);
                long_word = (unsigned long *) (destp + seg_dest + 
					       sh_src[ix].sh_size - padding);
                *long_word = 0;      /* clean the padding bytes */
                sh_dest[shnum].sh_size += (4 - padding);
            }

            memcpy((destp + seg_dest), 
                   (srcp + sh_src[ix].sh_offset), 
                   sh_src[ix].sh_size);

            sh_dest[shnum].sh_offset = seg_dest;
            seg_dest += sh_dest[shnum].sh_size;
            discard = FALSE;
        } else {
            discard = TRUE;
        }
	
        if (debug) {
            printf("%s's old Section Header [0x%x]: <<<%s>>>\n", 
		   image->name, ix,
                   ((discard)? "Discarded" : "Squished"));
            printf("sh_type : 0x%x, sh_name : 0x%x, sh_flags : 0x%x\n",
                   sh_src[ix].sh_type, sh_src[ix].sh_name,
                   sh_src[ix].sh_flags);
            printf("sh_addr : 0x%x, sh_offset : 0x%x, sh_size : 0x%x\n",
                   sh_src[ix].sh_addr, sh_src[ix].sh_offset,
                   sh_src[ix].sh_size);
            printf("sh_link : 0x%x, sh_entsize : 0x%x\n",
                   sh_src[ix].sh_link, sh_src[ix].sh_entsize);

            if (!discard) {
                printf("%s's new Section Header [0x%x]:\n", image->name, 
		       shnum);
                printf("sh_type : 0x%x, sh_name : 0x%x, sh_flags : 0x%x\n",
                       sh_dest[shnum].sh_type, sh_dest[shnum].sh_name,
                       sh_dest[shnum].sh_flags);
                printf("sh_addr : 0x%x, sh_offset : 0x%x, sh_size : 0x%x\n",
                       sh_dest[shnum].sh_addr, sh_dest[shnum].sh_offset,
                       sh_dest[shnum].sh_size);
                printf("sh_link : 0x%x, sh_entsize : 0x%x\n\n",
                       sh_dest[shnum].sh_link, sh_dest[shnum].sh_entsize);
            } else {
                printf("\n");
            }
        }
        
        if (!discard)
            shnum++;

    }

    /*
     * Copy the compressed router image into the newly created section
     */
    if (append_router_image) {
        /*
         * Get the index of the section header for the section that has the
         * compressed router image. The image is in the last section of the
         * glued image.
         */
        img_shnum = eh_dest->e_shnum - 1;

        sh_dest[img_shnum].sh_offset = seg_dest;
        sh_dest[img_shnum].sh_addr = sh_dest[img_shnum-1].sh_addr 
	    + sh_dest[img_shnum-1].sh_size;

	if (debug) {
	    printf("%s's new Section Header [0x%x]: <<<Added>>>\n", 
		   image->name, img_shnum);
	    printf("sh_type : 0x%x, sh_name : 0x%x, sh_flags : 0x%x\n",
		   sh_dest[img_shnum].sh_type, sh_dest[img_shnum].sh_name,
		   sh_dest[img_shnum].sh_flags);
	    printf("sh_addr : 0x%x, sh_offset : 0x%x, sh_size : 0x%x\n",
		   sh_dest[img_shnum].sh_addr, sh_dest[img_shnum].sh_offset,
		   sh_dest[img_shnum].sh_size);
	    printf("sh_link : 0x%x, sh_entsize : 0x%x\n\n",
		   sh_dest[img_shnum].sh_link, 
		   sh_dest[img_shnum].sh_entsize);
	}
    }

    free(image->buffer);
    image->buffer = new_buffer;
    image->size   = seg_dest;
}

/*
 * unzip_image : Unzip an image
 * Take router_file as input (zipped file)
 * Take glued_file as output file
 */
void unzip_image (char *argv[])
{
    if (!router_file.name || !glued_file.name) {
      usage(argv[0]);
      exit(1);
    }

    /*
     * Open compressed router image elf and extract the zip image out of it
     */
    read_image(&router_file);

    extract_zip_image(&router_file, &glued_file);

    /*
     * Open the decompressed router image and check it
     */
    read_image(&glued_file);

    if (glued_file.size != glued_file.magic_hdr->uncomp_size) {
        fprintf(stderr, "Error: The decompressed file %s has a wrong "
                "file size.\n", glued_file.name);
        exit(1);
    }

    if (glued_file.magic_hdr->uncomp_checksum != image_checksum(&glued_file)) {
        fprintf(stderr, "Error: The decompressed image %s doesn't checksum.\n",
                glued_file.name);
        exit(1);
    }

    verify_elf_file (&glued_file);

    free(glued_file.buffer);
    glued_file.buffer = NULL;

    printf("Decompressed %s out of %s successfully\n", 
           glued_file.name, router_file.name);

}

/*
 * zip_image : Zip an image
 * Take router_file as input (router file)
 * Take extract_file as input (czip file)
 * Take glued_file as output file
 */
void zip_image (char *argv[])
{
    int	temp;
    char *router_name;
    Elf32_Half cpu_type;
    
    if (!router_file.name || !extract_file.name || !glued_file.name) {
      usage(argv[0]);
      exit(1);
    }

    /*
     * Open router image elf and squish it
     */
    read_image(&router_file);
    squish_elf_file(&router_file, FALSE);

    router_file.magic_hdr->uncomp_checksum = image_checksum(&router_file);
    router_file.magic_hdr->uncomp_size     = router_file.size;
    cpu_type = ELF_EHDR(router_file.buffer).e_machine;
    
    save_image(&router_file, NULL);
    
    /*
     * Call the compressor to compress up the image (always zip)
     */
    sprintf(cmd_buf, "%s -q -k %s.gz %s",
            ZIP_COMMAND, router_file.name, router_file.name);
    printf("Compressing %s using %s\n", router_file.name, ZIP_COMMAND);
    temp = system(cmd_buf);
    if (temp != 0) {
        fprintf(stderr, "Error : system call to %s failed\n",
                ZIP_COMMAND);
        exit(1);
    }
    
    /*
     * Make the compressed file name and open it
     */
    router_name = router_file.name;
    router_file.name = router_comp_name;
    sprintf(router_comp_name, "%s.gz", router_name);
    read_image(&router_file);
    router_file.magic_hdr->comp_size = router_file.size;
    
    if (router_file.magic_hdr->comp_size >= 
	router_file.magic_hdr->uncomp_size) {
        fprintf(stderr, "Error : compressed size > uncompressed size\n");
        free(router_file.buffer);
        exit(1);
    }
    router_file.magic_hdr->comp_checksum = image_checksum(&router_file);
    
    /*
     * Open the extractor elf and read it into memory
     */
    read_image(&extract_file);
    squish_elf_file(&extract_file, TRUE);

    glued_file.buffer = extract_file.buffer;
    glued_file.size   = extract_file.size;
    
    ELF_EHDR(glued_file.buffer).e_machine = cpu_type;
    save_image(&glued_file, &router_file);
    
    printf("Glued %s and %s to form %s successfully\n",
           router_file.name, extract_file.name, glued_file.name);
}

/*
 * main: 
 * zip up the router executable, checksum it and glue
 * it together with the unzip executable to make an elf image
 *
 * Or if -u is on the commmand line unzip it.
 */
int main (int argc, char *argv[])
{
    router_file.magic_hdr = extract_file.magic_hdr 
	= glued_file.magic_hdr = &magic_hdr;
    router_file.freeBuffer = extract_file.freeBuffer 
	= glued_file.freeBuffer = TRUE;;
    router_file.magic_hdr->magic_id = MAGIC_VALUE;
    

    switch (command_line(argc, argv)) {
    case CMD_COMPRESS:
         zip_image(argv);
         break;

    case CMD_DECOMPRESS:
         unzip_image(argv);
         break;

    default:
         usage(argv[0]);
    }
    
    exit(0);
}

/* end of file */




