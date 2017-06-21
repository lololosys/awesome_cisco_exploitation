/* $Id: lr33000.h,v 1.1.4.1 1996/05/09 15:00:06 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/lr33000.h,v $
 *-----------------------------------------------------------------
 * lr33000.h --  defines for LSI Logic LR33000
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lr33000.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:06  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:03  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:42  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:18  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* lr33000.h - defines for LSI Logic LR33000 */

#ifndef _LR33000_
#define _LR33000_

#ifdef LANGUAGE_ASSEMBLY

#ifdef SABLE
#define C0_DCIC	$0		/* cache control */
#define C0_BPC	$2		/* breakpoint on instr */
#define C0_BDA	$4		/* breakpoint on data */
#else
#define C0_DCIC	$7		/* cache control */
#define C0_BPC	$3		/* breakpoint on instr */
#define C0_BDA	$5		/* breakpoint on data */
#endif

#endif /* LANGUAGE_ASSEMBLY */

#define DEBUG_VECT	0x80000040

/* definitions for DCIC register bits */
#define DCIC_TR	0x80000000	/* Trap enable */
#define DCIC_UD	0x40000000	/* User debug enable */
#define DCIC_KD	0x20000000	/* Kernel debug enable */
#define DCIC_TE	0x10000000	/* Trace enable */
#define DCIC_DW	0x08000000	/* Enable data breakpoints on write */
#define DCIC_DR	0x04000000	/* Enable data breakpoints on read */
#define DCIC_DAE 0x02000000	/* Enable data addresss breakpoints */
#define DCIC_PCE 0x01000000	/* Enable instruction breakpoints */
#define DCIC_DE	0x00800000	/* Debug enable */
#define DCIC_DL	0x00008000	/* Data cache line invalidate */
#define DCIC_IL	0x00004000	/* Instruction cache line invalidate */
#define DCIC_D	0x00002000	/* Data cache invalidate enable */
#define DCIC_I	0x00001000	/* Instruction cache invalidate enable */
#define DCIC_T	0x00000020	/* Trace, set by CPU */
#define DCIC_W	0x00000010	/* Write reference, set by CPU */
#define DCIC_R	0x00000008	/* Read reference, set by CPU */
#define DCIC_DA	0x00000004	/* Data address, set by CPU */
#define DCIC_PC	0x00000002	/* Program counter, set by CPU */
#define DCIC_DB	0x00000001	/* Debug, set by CPU */


/* Define counter/timer register addresses */
#define M_TIC1	0xfffe0000	/* timer 1 initial count */
#define M_TC1	0xfffe0004	/* timer 1 control	 */
#define M_TIC2	0xfffe0008	/* timer 2 initial count */
#define M_TC2	0xfffe000c	/* timer 2 control	 */
#define M_RTIC	0xfffe0010	/* refresh timer	 */
#define M_CFGREG	0xfffe0020	/* configuration reg	 */

#ifdef LANGUAGE_C
#define TIC1   (*((volatile unsigned long *)M_TIC1)) /* timer1 count */
#define TC1    (*((volatile unsigned long *)M_TC1))  /* timer1 cntrl */
#define TIC2   (*((volatile unsigned long *)M_TIC2)) /* timer2 count */
#define TC2    (*((volatile unsigned long *)M_TC2))  /* timer2 cntrl */
#define RTIC   (*((volatile unsigned long *)M_RTIC)) /* refrsh timer */
#define CFGREG (*((volatile unsigned long *)M_CFGREG)) /* config reg */
#endif

/* Definitions for counter/timer control register bits */
#define TC_CE	 0x00000004	/* count enable */
#define TC_IE	 0x00000002	/* interrupt enable */
#define TC_INT 	 0x00000001	/* interrupt request */

/* Definitions for Configuration register bits */
#define CR_XLE		0x80000000	/* LR33120 indicator bit */
#define CR_WBE		0x80000000	/* write buffer enable */
#define CR_BEN 		0x40000000	/* block write enable */
#define CR_PGSZMSK 	0x38000000	/* page size mask */
#define CR_PGSZSHFT 	27		/* page size shift amount */
#define CR_PGSZ(x)	((x)<<27)	/* set page size */
#define CR_IW8 		0x02000000	/* add 8 cycles to IOWAIT */
#define CR_PW8		0x01000000	/* add 8 cycles to PWAIT */
#define CR_ICDISABLE	0x00800000	/* Instruction cache disable */
#define CR_DCDISABLE	0x00400000	/* Data cache disable */
#define CR_IBLK_2	0x00000000	/* Instruction cache block size */
#define CR_IBLK_4	0x00100000	/* Instruction cache block size */
#define CR_IBLK_8	0x00200000	/* Instruction cache block size */
#define CR_IBLK_16	0x00300000	/* Instruction cache block size */
#define CR_IBLKMSK	0x00300000	/* Instruction cache block size */
#define CR_IBLKSHFT	20
#define CR_DBLK_2	0x00000000	/* Data cache block size */
#define CR_DBLK_4	0x00040000	/* Data cache block size */
#define CR_DBLK_8	0x00080000	/* Data cache block size */
#define CR_DBLK_16	0x000c0000	/* Data cache block size */
#define CR_DBLKMSK	0x000c0000	/* Data cache block size */
#define CR_DBLKSHFT	18
#define CR_IODIS	0x00020000	/* Disable DRDY for I/O addr's */
#define CR_IOWAITSHFT	13		/* I/O wait states */
#define CR_PDIS		0x00001000	/* Disable DRDY for PROM addr's */
#define CR_PWAITSHFT	8		/* PROM wait states */
#define CR_DPEN		0x00000040	/* Enable parity check for DRAM */
#define CR_RDYGEN	0x00000020	/* Disable DRDY for DRAM addr's */
#define CR_BLKFDIS	0x00000010	/* Disable DRAM block refill */
#define CR_RFSHEN	0x00000008	/* Enable refresh generator */
#define CR_RASPCHG	0x00000004	/* Define RAS precharge */
#define CR_CASLNTH	0x00000002	/* Define CAS active time */
#define CR_DRAMEN	0x00000001	/* Enable DRAM controller */

#endif /* _LR33000_ */
