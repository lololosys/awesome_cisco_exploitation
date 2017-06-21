/* $Id: lr33300.h,v 1.1.4.1 1996/05/09 15:00:08 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/lr33300.h,v $
 *-----------------------------------------------------------------
 * lr33300.h --  definitions for the LR33300 and LR33310 processors
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lr33300.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:08  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:04  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:14  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:43  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:20  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* definitions for the LR33300 and LR33310 processors */

#define M_SRAM		0xfffe0100	/* SRAM config reg */
#define M_SPEC0		0xfffe0104
#define M_SPEC1		0xfffe0108
#define	M_SPEC2		0xfffe010c
#define	M_SPEC3		0xfffe0110
#define	M_DRAM		0xfffe0120	/* DRAM config reg */
#define	M_BIU		0xfffe0130

#ifdef LANGUAGE_C

#define SRAM		(*((volatile unsigned long *)M_SRAM))
#define SPEC0		(*((volatile unsigned long *)M_SPEC0))
#define SPEC1		(*((volatile unsigned long *)M_SPEC1))
#define SPEC2		(*((volatile unsigned long *)M_SPEC2))
#define SPEC3		(*((volatile unsigned long *)M_SPEC3))
#define DRAM		(*((volatile unsigned long *)M_DRAM))
#define BIU		(*((volatile unsigned long *)M_BIU))

#define	C0_TAR		6		/* target address register */
#define	C0_BDAM		9		/* breakpoint data addr mask */
#define	C0_BPCM		11		/* breakpoint instr addr mask */
#else
#define	C0_TAR		$6		/* target address register */
#define	C0_BDAM		$9		/* breakpoint data addr mask */
#define	C0_BPCM		$11		/* breakpoint instr addr mask */
#endif

	/* cause register */
#define	CAUSE_BT	(1<<30)		/* branch taken */

	/* wait-state config registers */
#define	SPC_INHIBITMASK	(0xf<<24)
#define	SPC_INHIBITSHFT	24
#define	SPC_EXTGNT	(1<<23)
#define	SPC_16WIDE	(1<<22)
#define	SPC_8WIDE	(1<<21)
#define	SPC_PENA	(1<<20)
#define	SPC_CACHED	(1<<19)
#define	SPC_CSDLYMASK	(3<<17)
#define	SPC_CSDLYSHFT	17
#define	SPC_BLKENA	(1<<16)
#define	SPC_BLKWAITMASK	(7<<13)
#define	SPC_BLKWAITSHFT	13
#define	SPC_RECMASK	(63<<7)
#define	SPC_RECSHFT	7
#define	SPC_WAITENA	(1<<6)
#define	SPC_WAITMASK	(63<<0)
#define	SPC_WAITSHFT	0

	/* BIU */
#define	BIU_NOSTR	(1<<17)
#define	BIU_LDSCH	(1<<16)
#define	BIU_BGNT	(1<<15)
#define	BIU_NOPAD	(1<<14)
#define	BIU_RDPRI	(1<<13)
#define	BIU_INTP	(1<<12)
#define	BIU_IS1		(1<<11)
#define	BIU_IS0		(1<<10)
#define	BIU_IBLKSZMASK	(3<<8)
#define	BIU_IBLKSZSHFT	8
#define	BIU_IBLKSZ2	(0<<BIU_IBLKSZSHFT)
#define	BIU_IBLKSZ4	(1<<BIU_IBLKSZSHFT)
#define	BIU_IBLKSZ8	(2<<BIU_IBLKSZSHFT)
#define	BIU_IBLKSZ16	(3<<BIU_IBLKSZSHFT)
#define	BIU_DS		(1<<7)
#define	BIU_DS1		(1<<7)
#define	BIU_DS0		(1<<6)
#define	BIU_DBLKSZMASK	(3<<4)
#define	BIU_DBLKSZSHFT	4
#define	BIU_DBLKSZ2	(0<<BIU_DBLKSZSHFT)
#define	BIU_DBLKSZ4	(1<<BIU_DBLKSZSHFT)
#define	BIU_DBLKSZ8	(2<<BIU_DBLKSZSHFT)
#define	BIU_DBLKSZ16	(3<<BIU_DBLKSZSHFT)
#define	BIU_RAM		(1<<3)
#define	BIU_TAG		(1<<2)
#define	BIU_INV		(1<<1)
#define	BIU_LOCK 	(1<<0)

	/* DCR */ 
#define	DRAM_DLP1	(1<<28)
#define	DRAM_SYNC	(1<<27)
#define	DRAM_SCFG	(1<<26)
#define	DRAM_DMARDY	(1<<25)
#define	DRAM_DMABLKMASK	(7<<22)
#define	DRAM_DMABLKSHFT	22
#define	DRAM_DPTHMASK	(3<<20)
#define	DRAM_DPTHSHFT	20
#define	DRAM_RDYW	(1<<19)
#define	DRAM_PGSZMASK	(7<<16)
#define	DRAM_PGSZSHFT	16
#define	DRAM_PGMW	(1<<15)
#define	DRAM_RFWEMASK	(3<<13)
#define	DRAM_RFWESHFT	13
#define	DRAM_RFEN	(1<<12)
#define	DRAM_RDYEN	(1<<11)
#define	DRAM_BFD	(1<<10)
#define	DRAM_PE		(1<<9)
#define	DRAM_RPCMASK	(3<<7)
#define	DRAM_RPCSHFT	7
#define	DRAM_RCDMASK	(3<<5)
#define	DRAM_RCDSHFT	5
#define	DRAM_CS		(1<<4)
#define	DRAM_CLMASK	(7<<1)
#define	DRAM_CLSHFT	1
#define	DRAM_DCE	(1<<0)
