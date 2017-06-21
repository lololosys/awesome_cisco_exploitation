/* $Id: cisco_a.out.h,v 3.2.60.1 1996/04/23 00:09:03 dcoli Exp $
 * $Source: /release/112/cvs/Xsys/h/cisco_a.out.h,v $
 *------------------------------------------------------------------
 * cisco_a.out.h - cisco specific a.out header info
 *
 * November 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_a.out.h,v $
 * Revision 3.2.60.1  1996/04/23  00:09:03  dcoli
 * CSCdi44581:  Warn user when copying enterprise image to ciscoPro router
 * flash
 * Branch: California_branch
 * Stamp a.out header with a bit identifying the image as a ciscoPro
 * image.
 *
 * Revision 3.2  1995/11/17  09:11:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/23  05:57:12  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.1  1995/06/07  20:34:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __CISCO_A_OUT_H__
#define __CISCO_A_OUT_H__

/*
 * format of the a.out exec header
 */
struct exec {
    unsigned char	a_dynamic:1;		/* must be zero */
    unsigned char	a_toolversion:7;	/* must be zero */
    unsigned char	a_machtype;		/* machine type */
    unsigned short	a_magic;		/* magic number */
    unsigned long	a_text;			/* sizeof text segment */
    unsigned long	a_data;			/* sizeof initialized data */
    unsigned long	a_bss;			/* sizeof uninitialized data */
    unsigned long	a_syms;			/* sizeof symbol table */
    unsigned long	a_entry;		/* entry point */
    unsigned long	a_trsize;		/* sizeof text relocation */
    unsigned long	a_drsize;		/* sizeof data relocation */
};

#define	CISCOMAGIC	0x0107		/* magic number */
#define CISCOTYPE	0x02		/* machine type */


/*
 * returns 1 if an object file type is invalid, i.e., if the other macros
 * defined below will not yield the correct offsets.  Note that a file may
 * have N_BADMAG(x) = 0 and may be fully linked, but still may not be
 * executable.
 */

#define	N_BADMAG(x) 	((x).a_magic!=CISCOMAGIC)

/*
 * offsets of various sections of an object file.
 */

#define N_TXTOFF(x)	sizeof(struct exec)
#define N_DATOFF(x)	(N_TXTOFF(x)  + (x).a_text)   /* data segment    */ 
#define N_TRELOFF(x)	(N_DATOFF(x)  + (x).a_data)   /* text relocation */
#define N_DRELOFF(x)	(N_TRELOFF(x) + (x).a_trsize) /* data relocation */
#define N_SYMOFF(x)	(N_DRELOFF(x) + (x).a_drsize) /* symbol table    */
#define N_STROFF(x)	(N_SYMOFF(x)  + (x).a_syms)   /* string table    */

#define FAMILY_CHECK_MAGIC	0xdade /* magic value to verify */
				       /* family/interface stamp*/
/*
 * Bit definitions for family_check.flags field.
 */
#define CISCOPRO_IMAGE_FLAG 0x0001

struct family_check {
    unsigned short	magic;	    /* magic value to validate structure */
    unsigned short 	family;	    /* CPU family for HW to which this image 
				       belongs */
    unsigned short	flags;	    /* CiscoPro image, etc. */
    unsigned short	interface;  /* interface bit defn's (les/if_table.h) */
};


/*
 * Format of a relocation datum.
 */
 
struct reloc_info_68k {
	long	r_address;	/* address which is relocated */
unsigned int	r_symbolnum:24,	/* local symbol ordinal */
		r_pcrel:1, 	/* was relocated pc relative already */
		r_length:2,	/* 0=byte, 1=word, 2=long */
		r_extern:1,	/* does not include value of sym referenced */
		r_baserel:1,	/* linkage table relative */
		r_jmptable:1,	/* pc-relative to jump table */
		r_relative:1,	/* relative relocation */
		:1;		
};

#define relocation_info reloc_info_68k	/* for backward compatibility */

/*
 * Format of a symbol table entry
 */
struct	nlist {
    union {
	char	       *n_name;		/* for use when in-core */
	struct nlist   *n_next;
	long 		n_strx;
    } n_un;
    unsigned char 	n_type;		/* type flag, i.e. N_TEXT etc */
    char		n_other;	/* unused */
    short		n_desc;		/* see <stab.h> */
    unsigned long 	n_value;	/* value of this symbol (or sdb offset) */
};

/*
 * Simple values for n_type.
 */
#define	N_UNDF	0x0		/* undefined */
#define	N_ABS	0x2		/* absolute */
#define	N_TEXT	0x4		/* text */
#define	N_DATA	0x6		/* data */
#define	N_BSS	0x8		/* bss */
#define	N_COMM	0x12		/* common (internal to ld) */
#define	N_FN	0x1e		/* file name symbol */

#define	N_EXT	01		/* external bit, or'ed in */
#define	N_TYPE	0x1e		/* mask for all the type bits */

/*
 * Dbx entries have some of the N_STAB bits set.
 * These are given in <stab.h>
 */
#define	N_STAB	0xe0		/* if any of these bits set, a dbx symbol */

#endif /*!_CISCO_A_OUT_H__*/
