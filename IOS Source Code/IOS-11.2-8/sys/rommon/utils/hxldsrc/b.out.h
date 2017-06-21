/* $Id: b.out.h,v 3.2 1995/11/17 18:48:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/b.out.h,v $
 *------------------------------------------------------------------
 * b.out.h
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for b.out files.
 *------------------------------------------------------------------
 * $Log: b.out.h,v $
 * Revision 3.2  1995/11/17  18:48:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:42:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 *	Layout of b.out file :
 *
 *	header of 8 longs	magic number 405, 407, 410, 411
 *				text size		)
 *				data size		) in bytes
 *				bss size		)
 *				symbol table size	)
 *				text relocation size	)
 *				data relocation size	)
 *				entry point
 *
 *
 *	header:			0
 *	text:			32
 *	data:			32+textsize
 *	symbol table:		32+textsize+datasize
 *	text relocation:	32+textsize+datasize+symsize
 *	data relocation:	32+textsize+datasize+symsize+rtextsize
 *
 */
/* various parameters */
#define SYMLENGTH	50		/* maximum length of a symbol */
#define SEGSIZE		32768		/* relocation boundry for 410 files */

/* types of files */
#define	ARCMAGIC 016230236041
#define OMAGIC	0405
#define	FMAGIC	0407
#define	NMAGIC	0410
#define	IMAGIC	0411
#define	RMAGIC	0700200000

/* symbol types */
#define	EXTERN	040
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

/* relocation sizes */
#define RBYTE	00
#define RWORD	01
#define RLONG	02

/* macros which define various positions in file based on a bhdr, filhdr */
#define TEXTPOS		sizeof(filhdr)
#define DATAPOS 	TEXTPOS + filhdr.tsize
#define SYMPOS		DATAPOS + filhdr.dsize
#define RTEXTPOS	SYMPOS + filhdr.ssize
#define RDATAPOS	RTEXTPOS + filhdr.rtsize
#define ENDPOS		RDATAPOS + filhdr.rdsize
/* header of b.out files */
struct bhdr {
	long	fmagic;
	long	tsize;
	long	dsize;
	long	bsize;
	long	ssize;
	long	rtsize;
	long	rdsize;
	long	entry;
};

/* symbol management */
struct sym {
	char	stype;
	char	slength;
	long	svalue;
};

/* relocation commands */
struct reloc {
	unsigned rsegment:2;	/* RTEXT, RDATA, RBSS, or REXTERN */
	unsigned rsize:2;		/* RBYTE, RWORD, or RLONG */
	unsigned rdisp:1;		/* 1 => a displacement */
	short rsymbol;		/* id of the symbol of external relocations */
	long rpos;		/* position of relocation in segment */
};

