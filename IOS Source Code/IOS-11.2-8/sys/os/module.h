/* $Id: module.h,v 3.2 1995/11/17 18:49:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/module.h,v $
 *------------------------------------------------------------------
 * module.h - Definitions for loadable modules
 *
 * November 11, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: module.h,v $
 * Revision 3.2  1995/11/17  18:49:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:57:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MODULE_H__
#define __MODULE_H__

/*
 * This module is included in both the system and in the reloc program, so
 * the definitions here should be constrained to the definitions of the
 * structure and contents of a relocatable load module.
 *
 * In particular, prototypes, data structure definitions, and external
 * references *do not* belong here.
 */

/*
 * Temporary defines
 */

#define CPU_LONG_ALIGN	2	/* Required long-word alignment */

/*
 * Define major module constants here
 */

#define MODULE_MAJVER 0		/* Module header major version number */
#define MODULE_MINVER 0		/* Module header minor version number */

#define MODULE_MAGIC (0xfeed)

#define MODULE_TYPE_RESERVED	0 	/* Reserved type */
#define MODULE_TYPE_SECTION	1	/* Section header */
#define MODULE_TYPE_RELOC	2	/* Relocation data */
#define MODULE_TYPE_DATA	3	/* Load data */

/*
 * Define sizes of fields in module header. If you change any of these,
 * you will have to change the major version number.
 */

#define MODULE_NAMEMAX (31)	/* Maximum module name length */
#define MODULE_SECTIONMAX (8)
/*
 * Define CPU types here.
 *
 * When loading a module, the loader verifies that all known instruction
 * types are supported. This means that a 68040 can load 68020 images, but
 * a 68020 can not load 68040 images.
 */

#define MODULE_68000	0x00000001 /* Contains basic 68000 instructions */
#define MODULE_CPU32	0x00000002 /* Contains CPU32 extensions */
#define MODULE_68020	0x00000004 /* Contains 68020 extensions */

/*
 * Define section flags
 */

#define SECTION_READONLY	0x00000001 /* Section is read-only */
#define SECTION_FAST		0x00000002 /* Run in fast memory */

/*
 * Define the layout of a section header
 *
 * The section header defines a section which will be found in the module.
 * The section header resides in the module header, and there is one section
 * header for each section in the file.
 */

typedef struct section_header_ {
    ulong size;			/* Size of section in bytes */
    ulong flags;		/* Section flags */
    uchar *linkaddr;		/* Base link address of section */
} section_header;

/*
 * Define the layout of a section prologue
 *
 * The section prologue preceeds the relocation information for a section
 * within the input file. Sections are numbered starting at zero and going
 * up to MODULE_SECTIONMAX - 1. All sections in the file must be included
 * sequentially by section number.
 */

typedef struct section_prologue_ {
    ulong type;			/* type code */
    ulong secnum; 		/* Section number */
    ulong relcnt;		/* Count of relocation segments to follow */
    ulong datacnt;		/* Count of data segments to follow */
} section_prologue;

/*
 * Define the layout of a relocation prologue
 *
 * There is a relocation prologue for each relocation segment within a
 * section. A relocation segment contains the relocation fixups for one
 * target section.
 *
 * The relocation items are sorted in ascending address order. This allows
 * for sequential-access relocation, which is necessary for FLASH programming.
 */

typedef struct relocation_prologue_ {
    ulong type;			/* type code */
    ulong secnum;		/* Target section number */
    ulong count;		/* Number of relocation items */
    ulong items[0];		/* Relocation items */
} relocation_prologue;

/*
 * Define the layout of a data prologue
 *
 * The data prologue preceeds the actual data being loaded into the module.
 * There may be multiple load segments - any addresses not explicitly loaded
 * are implicitly loaded with zeroes.
 *
 * Note that in order to facilitate FLASH programming, each data area must
 * start at an address greater than the end of the preceeding data area.
 */

typedef struct data_prologue_ {
    ulong type;			/* Type code */
    ulong secnum;		/* Section number */
    ulong count;		/* Length of data to follow, in bytes */
    ulong addr_offset;		/* Offset into section */
    ulong data[0];		/* Data itself */
} data_prologue;

/*
 * Define the layout of a module header
 */

typedef struct module_header_ {
    ulong hdr_magic;		/* Magic number */
    ulong hdr_majver;		/* Major version number of header structure */
    ulong hdr_minver;		/* Minor version number of header structure */
    ulong core_majver;		/* Core system major version number */
    ulong core_minver;		/* Core system minor version number */
    ulong mod_majver;		/* Major version of module */
    ulong mod_minver;		/* Minor version of module */
    ulong mod_id;		/* Canonical module ID */
    ulong mod_cpuflags;		/* Flags for CPU types */
    ulong mod_entrysec;		/* Section number containing entry point */
    ulong mod_entryoff;		/* Offset into section for entry point */
    ulong sec_cnt;		/* Section count */
    section_header sec_info[MODULE_SECTIONMAX];	/* Section info */
    uchar mod_namelen;		/* Module name length */
    char mod_name[MODULE_NAMEMAX]; /* Module name */
} module_header;

#endif __MODULE_H__
