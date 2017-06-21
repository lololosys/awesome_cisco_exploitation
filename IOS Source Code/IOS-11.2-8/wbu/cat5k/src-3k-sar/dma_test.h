/* $Id: dma_test.h,v 1.1.4.1 1996/05/09 15:01:32 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/src-3k-sar/dma_test.h,v $
 *-----------------------------------------------------------------
 * dma_test.h -- 
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dma_test.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:32  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:28  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:56  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:40  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:33  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:33  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  d m a _ t e s t 2 . h
 */

/*
 * Assembler Coding Conventions:
 *
 * <>_REG for Processor Registers (s0-s7)
 * <>_AREG for ATMIZER Registers
 * <>_s for structure fields
 * <>_C for defined constants
 * <>_L for program labels
 */

/*
 * Addresses
 */
#define	VCR_ADDR_C	  0xfff00000			/* Address of VCR */
#define ASUBR_VALUE_C     0x282       	/* Value to put into M_ASUBR */
/* must use non-cacheable space */
#define SHARED_MEM_BASE_C  (0xa0800000+0x10)  
#define SAR_2_OFFSET	   (SHARED_MEM_BASE_C + 0x100)  
#define TEST_COUNT_OFFSET  (0x50)  
#define OK_COUNT_OFFSET    (0x54)  
#define BAD_COUNT_OFFSET   (0x58)  


#define	CELL_SIZE_C	64

#ifdef LANGUAGE_C

#define STALL  (*((volatile ulong *)M_STALL))
#define CONFIG  (*((volatile ulong *)M_CONFIG))
#define PRPC8  (*((volatile ulong *)(M_PRPC+0x20)))

int do_rx_SAR_L();

typedef int Func();
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef struct shmem_rec_ {
	ulong seq_num;
	ulong dma_ok;
	ulong dma_bad;
} t_shmem_rec;

#else /* LANGUAGE_ASSEMBLY */


/* 
 * register assignments 
 */
#define	VCR_REG			s0	/* pointer to start of VCR */
#define SHMEM_REG		s1	/* pointer to start of shared memory */
#define	seq_num_REG		s2	/* counter zero means CPU Q needs processing */
#define	dma_ok_REG		s3
#define	dma_bad_REG		s4




/* 
 * ATMizer register access method 
 */
#define CREDIT_AREG	0x4000(VCR_REG)
#define ICREDIT_AREG	0x4080(VCR_REG)
#define STALL_AREG	0x4200(VCR_REG)
#define RXBUFPTR_AREG	0x4400(VCR_REG)
#define TXFIFO_AREG	0x4500(VCR_REG) # "0x4540 if CRC10 needs to be generated Pg 9-5"
#define RXACK_AREG	0x460c(VCR_REG)
#define SCR_AREG	0x4a00(VCR_REG)
#define CRC32_AREG	0x4c00(VCR_REG)
#define FCRC32_AREG	0x4c80(VCR_REG)
#define HOSTINT_AREG	0x4b00(VCR_REG)

#define	CIR_REG9_AREG	0x43a4(VCR_REG)

/* 
 * Structure Definitions
 */
	.data
	.struct	0
/* Info elements in start of Shared Memory */
seq_num_s:	.word	0
dma_ok_s:	.word	0
dma_bad_s:	.word	0
dma_dest_s: .space  CELL_SIZE_C

	.text
#endif /* END OF LANGUAGE SPECIFIC STUFF */
