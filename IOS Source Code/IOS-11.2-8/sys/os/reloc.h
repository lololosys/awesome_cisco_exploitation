/* $Id: reloc.h,v 3.3 1996/02/13 20:58:23 hampton Exp $
 * $Source: /release/111/cvs/Xsys/os/reloc.h,v $
 *------------------------------------------------------------------
 * Header file for relocatable images
 *
 * September 1992, Ashwin Baindur
 * February 1993, dit Morse - moved from 9.14 tree to lab trunk
 *
 * This file contains structures & #defines necessary for
 * modules which handle relocatable images
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reloc.h,v $
 * Revision 3.3  1996/02/13  20:58:23  hampton
 * Use the new file for recording platform specific starting addresses of
 * relocatable images.  [CSCdi48844]
 *
 * Revision 3.2  1995/11/17  18:51:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:58:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

#ifndef __RELOC_H__
#define __RELOC_H__

#if !defined(FALSE) 
#define FALSE 0
#endif /* !defined(FALSE) */

#if !defined(TRUE) 
#define TRUE 1
#endif /* !defined(TRUE) */

#define MIN_RELOC_IMAGE_VER	0x00010000 /* minimum version we'll ever see */
#define RELOC_IMAGE_VER		0x00010000

#define RELOC_IMAGE_MAJOR_VER	(RELOC_IMAGE_VER  >> 16)

#define SIMPLE_CHECKSUM_ALGORITHM	1
#define TC_ALGORITHM		SIMPLE_CHECKSUM_ALGORITHM
#define IC_ALGORITHM       	SIMPLE_CHECKSUM_ALGORITHM
#define SHC_ALGORITHM      	SIMPLE_CHECKSUM_ALGORITHM

#define BSS_OBJECT_ALIGN (4-1)	/* Align on longword boundaries */
#define MAX_BSS_ENTRY (512 * 1024) /* Assume this many BSS symbols at most */
#define MAX_LONG_RELOCATION_ENTRY (512 * 1024) /* Max 32bit relocations */
        /* Max 8bit relocations */
#define MAX_RELOCATION_ENTRY (MAX_LONG_RELOCATION_ENTRY * 4)  

/*
 * Magic type numbers in the start of the file.  This is the FIRST short
 * integer in the file.  The "real" magic number is the second short in
 * the file.  This number will distinguish Sun from HP from an already
 * compressed file.
 */
#define TYPE_COMP 0x1f9d
#define TYPE_LZHUF 0x1f9e
#define TYPE_HPUX 0x020c
#define SUN_TOOLVER 0x00
#define TYPE_SUN  0x0002
#define COMP_MAGIC 0x1f9d
#define LZHUF_MAGIC 0x1f9e
#define RELOC_MAGIC1 0xb0ee

struct sun_exec {
	unsigned short	a_machtype;	/* machine type */
	unsigned short	a_magic;	/* magic number */
	unsigned long	a_text;		/* size of text segment */
	unsigned long	a_data;		/* size of initialized data */
	unsigned long	a_bss;		/* size of uninitialized data */
	unsigned long	a_syms;		/* size of symbol table */
	unsigned long	a_entry;	/* entry point */
	unsigned long	a_trsize;	/* size of text relocation */
	unsigned long	a_drsize;	/* size of data relocation */
};

#define SUN_AOUT_HDR_SIZE 	sizeof(struct sun_exec)
#define ALIGNED_BLOCK_SIZE	512
#define RELOC_HEADER_LEN    	(ALIGNED_BLOCK_SIZE - SUN_AOUT_HDR_SIZE)

/*
 *------------  Special headers - old and new ------------
 */
/* Structure of old special header */
struct old_spl_hdr {
    unsigned short s_magic; /* MAGIC_RELOC */
    unsigned short s_hdr_len; /* total header length */
    unsigned long s_text; /* current link address for text */
    unsigned long s_data; /* current link address for data */
    unsigned long s_tsize; /* size of text */
    unsigned long s_dsize; /* size of data */
    unsigned long s_bsize; /* size of bss */
    unsigned long s_pgmsize; /* size of relocation program */
    unsigned long s_rsize; /* size of relocation tables */
};

        /*
         * Assume max 16 byte long instruction. This is called ultra
         * forward thinking ....
         */
#define PAD_BRA_SIZE 	\
               (16 - (2*sizeof(short)) - sizeof(long))  

struct special_header {
    /*
     * j_instr will contain a jump instruction to the relocation code.
     * The pad size will be long enough to accomodate the longest
     * possible jump instruction. Pad bytes will be set to FF.
     */
    struct jump_instruct { /* To skip this header */
        /*
         * The kludgy breakup below is being done to ensure that
         * offset does not get aligned to a long-word boundary.
         * Don't forget to change PAD_BRA_SIZE if you change anything
         * below.
         */
        unsigned short j_opcode;
        unsigned short j_offset_1;  /* upper short of jump offset */
        unsigned long  j_offset_2;  /* lower portion of jump offset */
        unsigned char  pad[PAD_BRA_SIZE]; 
    } j_instr;

    /*
     * The first four fields are as per the format of the a.out
     * header. r_toolversion will be 0 to start with, and possibly
     * forever.... r_machtype indicates the 68020 platform
     * as in an a.out header. The r_magic value shown below was 
     * arbitrarily chosen around two years ago without any rhyme 
     * or reason.
     */
    unsigned char   r_dynamic:1;      /* relocatable image = 1 */
    unsigned char   r_toolversion:7;  /* ver of toolset used; initial = 0 */
    unsigned char   r_machtype;       /* machine type = 02 */
    unsigned short  r_magic;          /* 0xb0ee */
                                      /* complete magic value = 0x8002b0ee */

    /*
     * Version number for this header. The initial value will be 0x00010000.
     * The upper short will be a major version number; the lower short a 
     * minor version number. Minor version number will be bumped up for minor
     * changes that don't create backward compatibility problems. 
     * The major number will be bumped up when there is a major change that
     * affects backward compatibility. The minor version will be reset to
     * zero in such cases.
     */
    unsigned long   r_version;        /* version number for this header */

    unsigned long   r_hdr_len;        /* total length of this header */
    unsigned long   r_text;           /* current link address for text */
                                      /* modified whenever image relocated */
    unsigned long   r_data;           /* current link address for data */
                                      /* modified whenever image relocated */
    /*
     * The three fields below are moved here from the a.out header before
     * the text segment size in the a.out header is dorked with to reflect
     * all the extra baggage.
     */
    unsigned long   r_tsize;          /* size of text segment */
    unsigned long   r_dsize;          /* size of data segment */
    unsigned long   r_bsize;          /* size of bss segment */

    /*
     * The relocation program size and the relocation table size will
     * always be a multiple of a long word since the relocation
     * program and the relocation table will always be padded to a long
     * word boundary. The relocation program is padded by adding a NOP.
     * The relocation table will be padded by adding FFs.
     */
    unsigned long   r_pgmsize;        /* size of relocation program */
    unsigned long   r_rsize;          /* total size of relocation tables */

    /*
     * The default link address will be such that if an image is 
     * downloaded as the first image in flash, it should be able to 
     * execute in-place from flash.
     * This means the text segment link address will be
     *    0x3000000 + sizeof(flash-file-header) + sizeof(a.out-header)
     *              + sizeof(this-header) + sizeof(relocation-program)
     *              + sizeof(relocation-tables) + sizeof(jump-instruction)
     *    where 0x3000000 is the start of flash address space.
     * The entry point in the a.out header (a_entry) will however 
     * contain 0x3000060 by default to point to the first jump 
     * instruction in the image.
     * The data segment link address will be the starting location 
     * for images in DRAM (typically 0x1000; 0x20000 for 4000).
     */ 
    unsigned long   r_def_text;       /* default link address for text */
    unsigned long   r_def_data;       /* default link address for data */
    unsigned long   r_def_ram_start;  /* default DRAM start for images */

    /*
     * The next two fields would give the offset of the text and data
     * segments from the start of the image. These offsets could then
     * be converted into block numbers (for TFTP blocks) and offsets within
     * the blocks, and then used by the TFTP download and upload functions to
     * track TFTP blocks corresponding to the start of the text and data
     * segments.
     */
    unsigned long   r_text_offset;    /* offset of text from image start */
    unsigned long   r_data_offset;    /* offset of data from image start */

    /*
     * The text segment checksum will be stored within the data segment 
     * as is the case today. The r_text_cksum_offset field will give 
     * the offset of this checksum field from the start of the image. 
     * The r_text_cksum_offset_in_data will give the offset of this 
     * checksum within the data segment (just in case).
     * Apart from the current checksum, the checksum when the text segment
     * is relocated to its default state and the checksum when the
     * image is relocated to run from DRAM will also be stored. These 
     * will help avoid checksum re-calculation during upload, and easier 
     * validation during execution from DRAM.
     */
    unsigned long   r_def_text_cksum; /* default text segment checksum */
    unsigned long   r_rfr_text_cksum; /* text checksum when run from DRAM */

    /*
     * Text segment checksum offsets.
     */
    unsigned long   r_text_cksum_offset; /* from start of image */
    unsigned long   r_text_cksum_offset_in_data; /* within data segment */

    unsigned long   r_image_cksum;    /* current image checksum */
                                      /* modified whenever image relocated */
    unsigned long   r_def_image_cksum;   /* default image checksum */

    /*
     * Checksum for this header. The checksum will be calculated with this
     * field set to 0. The resulting value will be subtracted from 
     * 0xFFFFFFFF before storage in this field. As a result, during
     * checksum verification, the checksum calculated with the value
     * in this field should always be 0xFFFFFFFF.
     * Checksum should be recalculated whenever image is relocated. 
     */
    unsigned long   r_hdr_cksum;     /* Checksum for this header */

    /*
     * The checksum algorithm codes would help us modify or enhance the
     * checksum calculation method in future. It would help software determine
     * which method to use.
     */
    unsigned long   r_text_cksum_algor;  /* text checksum algorithm code */
    unsigned long   r_image_cksum_algor; /* image checksum algorithm code */
    unsigned long   r_hdr_cksum_algor;   /* header checksum algorithm code */

    /*
     * CPU family defines.
     *
    #define FAMILY_CSC2     CPU_CSC2
    #define FAMILY_RP1      CPU_RP1
    #define FAMILY_PAN      CPU_PAN
    #define FAMILY_XX       CPU_XX
    #define FAMILY_LEMONADE CPU_LEMONADE
    #define FAMILY_SIERRA   CPU_SIERRA
     *
     */

    unsigned long   r_platform_family;   /* Platform CPU family */
};

#define ALIGN_TO_512_BYTE_BOUNDARY  \
                        (512 - sizeof(struct special_header)  \
                         - SUN_AOUT_HDR_SIZE - sizeof(long))

struct reloc_image_internal_header {
    struct special_header spl_hdr;

    /*
     * The padding would be such as to align the end of this header to a 512
     * byte boundary. Therefore, the size of the pad would be
     *    512 - sizeof(a.out-header) - sizeof(special-header)
     *        - sizeof(long)
     * The jump instruction has been included in the special header.
     * The padding will be initialized to all FFs.
     */
    unsigned char   r_padding[ALIGN_TO_512_BYTE_BOUNDARY];

    /*
     * Length of this header repeated here for easy access to the
     * relocation program. The relocation program would start 
     * immediately after this length field.
     */
    unsigned long   spl_hdr_len; 
};

/*
 * Our data structure for tracking relocation entries
 */

enum REGIONS {
    REGION_TEXT, REGION_DATA, REGION_BSS,
    REGION_MAX
  };

#define SP_HDR_SIZE 		sizeof(struct reloc_image_internal_header)
typedef unsigned char           RELOC_OFFSET_TYPE;
#define RELOC_OFFSET_SIZE 	sizeof(RELOC_OFFSET_TYPE)

struct relocation_table {
    struct relocation_entry {
	unsigned long count;	/* Count of entries */
                                /* Entries themselves */
	RELOC_OFFSET_TYPE  address[MAX_RELOCATION_ENTRY]; 
      } region[REGION_MAX];	/* One for each region */
};

struct long_relocation_table {
    struct long_relocation_entry {
	unsigned long count;	/* Count of entries */
                                /* Entries themselves */
	unsigned long address[MAX_LONG_RELOCATION_ENTRY]; 
      } region[REGION_MAX];	/* One for each region */
};

struct bss_entry {
    char *name;			/* Pointer to entry in string table */
    unsigned long address;	/* Address in BSS of item */
    unsigned long size;		/* Length of object in BSS */
};

#endif  /* __RELOC_H__ */
