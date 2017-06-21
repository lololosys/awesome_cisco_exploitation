/* $Id: buserr.h,v 3.1 1995/11/09 09:09:57 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/buserr.h,v $
 *------------------------------------------------------------------
 * buserr.h -- bus and address error definitions for MC68020
 *             and MC68040
 *
 * September 1987, Greg Satz
 * April 1992, Gary Kramling
 *
 * Copyright (c) 1987-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: buserr.h,v $
 * Revision 3.1  1995/11/09  09:09:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:12  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/01/30  01:11:37  whp4
 * Add RP1 support.
 *
 *
 * $Endlog$
 */

struct buserrinfo {
        /* BEGIN - Common to 68020 and 68040 */
	u_short sr;			/* status register */
	u_long pc;			/* program counter */
	u_int type : 4;			/* stack contents type */
	u_int offset : 12;		/* vector offset */
	union {
	    struct {
		u_long instaddr;	/* instruction address */
	    } type2;
        /* END - Common to 68020 and 68040 */
            /* BEGIN - Specific to 68040 */
	    struct {
		u_long ea;		/* effective address */
	    } type3;
	    struct {
		u_long ea;		/* effective address */
		u_short ssw;		/* special status word */
		u_short wb3s;		/* writeback 3 status */
		u_short wb2s;		/* writeback 2 status */
		u_short wb1s;		/* writeback 1 status */
		u_long faddr;		/* data cycle fault address */
		u_long  wb3a;		/* writeback 3 address */
		u_long  wb3d;		/* writeback 3 data */
		u_long  wb2a;		/* writeback 2 address */
		u_long  wb2d;		/* writeback 2 data */
		u_long  wb1a;		/* writeback 1 address */
		u_long  wb1d;		/* writeback 1 data */
                u_long  pd1;		/* push data lw1 */
                u_long  pd2;		/* push data lw2 */
                u_long  pd3;		/* push data lw3 */
	    } type7;
            /* END - Specific to 68040 */
            /* BEGIN - Specific to 68020 */
	    struct {
		u_long instaddr;	/* instruction address */
		u_long ireg[4];		/* internal registers */
	    } type9;
	    struct {
		u_short ireg1;		/* internal register */
		u_short ssw;		/* special status word */
		u_short ipsc;		/* instruction pipe stage c */
		u_short ipsb;		/* instruction pipe stage b */
		u_long faddr;		/* data cycle fault address */
		u_short ireg2;		/* internal register */
		u_short ireg3;		/* internal register */
		u_long outbuf;		/* data output buffer */
		u_short ireg4;		/* internal register */
		u_short ireg5;		/* internal register */
	    } typea;
	    struct {
		u_short ireg1;		/* internal register */
		u_short ssw;		/* special status word */
		u_short ipsc;		/* instruction pipe stage c */
		u_short ipsb;		/* instruction pipe stage b */
		u_long faddr;		/* data cycle fault address */
		u_short ireg2[2];	/* internal register */
		u_long outbuf;		/* data output buffer */
		u_short ireg3[4];	/* internal register */
		u_long sbaddr;		/* stage b address */
		u_short ireg4[2];	/* internal register */
		u_long inbuf;		/* data input buffer */
		u_short ireg5[22];	/* internal register */
	    } typeb;
            /* END - Specific to 68020 */
	} stack;
};

/*
 * All #define's referenced after this point are 68020 specific...
 * They are not currently referenced anywhere in the code.
 */

/*
 * Values in type field
 */

#define	BERR_SHORT	0x0
#define BERR_THROW	0x1
#define	BERR_INSTR	0x2
#define	BERR_COPROC	0x9
#define	BERR_SHORTBUS	0xA
#define	BERR_LONGBUS	0xB

/*
 * Format of special status word
 */

#define	SSW_FC		0x8000		/* Fault on Stage C of the I pipe */
#define	SSW_FB		0x4000		/* Fault on Stage B of the I pipe */
#define	SSW_RC		0x2000		/* Rerun flag for Stage C of I pipe */
#define	SSW_RB		0x1000		/* Rerun flag for Stage B of I pipe */
#define	SSW_DF		0x0100		/* Fault/Rerun flag for Data Cycle */
#define	SSW_RM		0x0080		/* Read-Modify-Write on Data Cycle */
#define	SSW_RW		0x0040		/* R/W for Data Cycle - 1=Read */
#define	SSW_SIZ_MASK	0x0030		/* Mask for Size code */
#define	SSW_ADDR_MASK	0x0007		/* Mask of address space */

/*
 * Address space encodings
 */

#define	ADR_USERDATA	0x1		/* User Data Space */
#define	ADR_USERPROG	0x2		/* User Program Space */
#define	ADR_SUPDATA	0x5		/* Supervisor Data Space */
#define	ADR_SUPPROG	0x6		/* Supervisor Program Space */
#define	ADR_CPU		0x7		/* CPU Space */
