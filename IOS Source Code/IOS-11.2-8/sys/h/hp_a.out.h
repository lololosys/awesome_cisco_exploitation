/* $Id: hp_a.out.h,v 3.2.60.1 1996/05/17 11:15:34 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/h/hp_a.out.h,v $
 *------------------------------------------------------------------
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hp_a.out.h,v $
 * Revision 3.2.60.1  1996/05/17  11:15:34  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/03  14:25:27  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  09:12:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:58:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:35:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* @(#) $Revision: 3.2.60.1 $ */    

/*
 *	Layout of a.out file :
 *	_________________________
 *	| header		|
 *	|_______________________|
 *	| text section		|
 *	|_______________________|
 *	| data section		|
 *	|_______________________|
 *	| pascal section	|
 *	|_______________________|
 *	| symbol table		|
 *	|_______________________|
 *	| debug name table	|
 *	|_______________________|
 *	| source line table	|
 *	|_______________________|
 *	| value table		|
 *	|_______________________|
 *	| text relocation area	|
 *	|_______________________|
 *	| data relocation area	|
 *	|_______________________|
 *
 *	header:			0
 *	text:			sizeof(header)
 *	data:			sizeof(header)+textsize
 *	MIS:			sizeof(header)+textsize+datasize
 *	LEST:			sizeof(header)+textsize+datasize+ 
 *				MISsize 
 *	DNTT			sizeof(header)+textsize+datasize+
 *				MISsize+LESTsize
 *	SLT			sizeof(header)+textsize+datasize+
 *				MISsize+LESTsize+DNTTsize
 *	VT			sizeof(header)+textsize+datasize+
 *				MISsize+LESTsize+DNTTsize+SLTsize
 *	text relocation:	sizeof(header)+textsize+datasize+
 *				MISsize+LESTsize+DNTTsize+SLTsize+
 *				VTsize
 *	data relocation:	sizeof(header)+textsize+datasize+
 *				MISsize+LESTsize+DNTTsize+SLTsize+
 *				VTsize+rtextsize
 *
 *	NOTE - header, data, and text are padded to a multiple of
 *	       EXEC_PAGESIZE bytes (using EXEC_ALIGN macro) in
 *	       demand-load files.
 *
 */

/* header of a.out files */

/* @(#) $Revision: 3.2.60.1 $ */    

/*
   magic.h:  info about HP-UX "magic numbers"
*/

/* where to find the magic number in a file and what it looks like: */
#define MAGIC_OFFSET	0L

struct magic
	 {
	  unsigned short int system_id;
	  unsigned short int file_type;
	 };
typedef struct magic MAGIC;

/* predefined (required) file types: */
#define RELOC_MAGIC	0x106		/* relocatable only */
#define EXEC_MAGIC	0x107		/* normal executable */
#define SHARE_MAGIC	0x108		/* shared executable */

#define AR_MAGIC	0xFF65

/* optional (implementation-dependent) file types: */
#define DEMAND_MAGIC	0x10B		/* demand-load executable */

/* predefined HP-UX target system machine IDs */
#define HP9000_ID	0x208
#define HP98x6_ID	0x20A
#define HP9000S200_ID	0x20C


/* Nlist.h file included by hand */

/* @(#) $Revision: 3.2.60.1 $ */    
/* file nlist.h */



/* NOTE: this structure resembles, but is not the same as nlist_ defined
	 in a.out.h!
*/
struct nlist {				/* sizeof(struct nlist)=14 */
	char *		n_name;
	long	        n_value;	/* mit = svalue */
	unsigned char	n_type;		/* mit = stype */
	unsigned char	n_length;	/* length of ascii symbol name */
	short	        n_almod;        /* alignment mod */
	short	        n_unused;
};



struct exec {	
/*  0 */	MAGIC	a_magic;		/* magic number */
/*  4 */	short	a_stamp;		/* version id */
/*  6 */	short	a_unused;
/*  8 */	long	a_sparehp;
/* 12 */	long	a_text;			/* size of text segment */
/* 16 */	long	a_data;			/* size of data segment */
/* 20 */	long	a_bss;			/* size of bss segment */
/* 24 */	long	a_trsize;		/* text relocation size */
/* 28 */	long	a_drsize;		/* data relocation size */
/* 32 */	long	a_pasint;		/* pascal interface size */
/* 36 */	long	a_lesyms;		/* symbol table size */
/* 40 */	long	a_dnttsize;		/* debug name table size */
/* 44 */	long	a_entry;		/* entry point */
/* 48 */	long	a_sltsize;		/* source line table size */
/* 52 */	long	a_vtsize;		/* value table size */
/* 56 */	long	a_spare3;
/* 60 */	long	a_spare4;
};

#define	EXEC_PAGESIZE	4096	/* not always the same as the MMU page size */
#define	EXEC_PAGESHIFT	12	/* log2(EXEC_PAGESIZE) */
#define	EXEC_ALIGN(bytes)	(((bytes)+EXEC_PAGESIZE-1) & ~(EXEC_PAGESIZE-1))

# define TEXT_OFFSET(hdr)	((hdr).a_magic.file_type == DEMAND_MAGIC ? \
				EXEC_ALIGN(sizeof(hdr)) : sizeof(hdr))

# define DATA_OFFSET(hdr) 	((hdr).a_magic.file_type == DEMAND_MAGIC ? \
				EXEC_ALIGN(sizeof(hdr)) +        \
					EXEC_ALIGN((hdr).a_text) : \
				sizeof(hdr) + (hdr).a_text)

# define MODCAL_OFFSET(hdr)	((hdr).a_magic.file_type == DEMAND_MAGIC ? \
				EXEC_ALIGN(sizeof(hdr)) +        \
					EXEC_ALIGN((hdr).a_text) + \
					EXEC_ALIGN((hdr).a_data) : \
				sizeof(hdr) + (hdr).a_text + (hdr).a_data)

# define LESYM_OFFSET(hdr)     MODCAL_OFFSET(hdr) + (hdr).a_pasint
# define DNTT_OFFSET(hdr)  LESYM_OFFSET(hdr) + (hdr).a_lesyms
# define SLT_OFFSET(hdr)   DNTT_OFFSET(hdr) + (hdr).a_dnttsize
# define VT_OFFSET(hdr)    SLT_OFFSET(hdr) + (hdr).a_sltsize
# define RTEXT_OFFSET(hdr) VT_OFFSET(hdr) + (hdr).a_vtsize
# define RDATA_OFFSET(hdr) RTEXT_OFFSET(hdr) + (hdr).a_trsize
# define DNTT_SIZE(hdr)  (hdr).a_dnttsize
# define VT_SIZE(hdr)    (hdr).a_vtsize
# define SLT_SIZE(hdr)   (hdr).a_sltsize


/* macros which define various positions in file based on an exec: filhdr */
#define TEXTPOS		TEXT_OFFSET(filhdr)
#define DATAPOS 	DATA_OFFSET(filhdr)
#define MODCALPOS	MODCAL_OFFSET(filhdr)
#define LESYMPOS	LESYM_OFFSET(filhdr)
#define DNTTPOS		DNTT_OFFSET(filhdr)
#define SLTPOS		SLT_OFFSET(filhdr)
#define VTPOS		VT_OFFSET(filhdr)
#define RTEXTPOS	RTEXT_OFFSET(filhdr)
#define RDATAPOS	RDATA_OFFSET(filhdr)

/* symbol management */
struct nlist_ {		
	long	n_value;
	unsigned char	n_type;
	unsigned char	n_length;	/* length of ascii symbol name */
	short	n_almod;
	short	n_unused;
};

#define	A_MAGIC1	FMAGIC       	/* normal */
#define	A_MAGIC2	NMAGIC       	/* read-only text */

/* relocation commands */
struct r_info {		/* mit= reloc{rpos,rsymbol,rsegment,rsize,rdisp} */
	long r_address;		/* position of relocation in segment */
	short r_symbolnum;	/* id of the symbol of external relocations */
	char r_segment;		/* RTEXT, RDATA, RBSS, REXTERN, or RNOOP */
	char r_length;		/* RBYTE, RWORD, or RLONG */
};

/* symbol types */
#define	EXTERN	040	/* = 0x20 */
#define ALIGN	020	/* = 0x10 */	/* special alignment symbol type */
#define	UNDEF	00
#define	ABS	01
#define	TEXT	02
#define	DATA	03
#define	BSS	04
#define	COMM	05	/* internal use only */
#define REG	06

/* relocation regions */
#define	RTEXT	00
#define	RDATA	01
#define	RBSS	02
#define	REXT	03
#define	RNOOP	077	/* no-op relocation  record, does nothing */

/* relocation sizes */
#define RBYTE	00
#define RWORD	01
#define RLONG	02
#define RALIGN	03	/* special reloc flag to support .align symbols */

	/* values for type flag */
#define	N_UNDF	0	/* undefined */
#define	N_TYPE	037
#define	N_FN	037	/* file name symbol */
#define	N_ABS	01	/* absolute */
#define	N_TEXT	02	/* text symbol */
#define	N_DATA	03	/* data symbol */
#define	N_BSS	04	/* bss symbol */
#define	N_REG	024	/* register name */
#define	N_EXT	040	/* external bit, or'ed in */
#define	FORMAT	"%06o"	/* to print a value */
#define SYMLENGTH	255		/* maximum length of a symbol */

/* These suffixes must also be maintained in the cc shell file */

#define OUT_NAME "a.out"
#define OBJ_SUFFIX ".o"
#define C_SUFFIX ".c"
#define ASM_SUFFIX ".s"

#define N_BADTYPE(x) (((x).a_magic.file_type)!=EXEC_MAGIC&&\
	((x).a_magic.file_type)!=SHARE_MAGIC&&\
	((x).a_magic.file_type)!=DEMAND_MAGIC&&\
	((x).a_magic.file_type)!=RELOC_MAGIC)

#define N_BADMACH(x) ((x).a_magic.system_id!=HP9000S200_ID&&\
	(x).a_magic.system_id!=HP98x6_ID)
#define N_BADMAG(x)  (N_BADTYPE(x) || N_BADMACH(x))

