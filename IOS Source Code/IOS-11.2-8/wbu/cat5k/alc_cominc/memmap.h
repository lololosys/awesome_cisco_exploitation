/* $Id: memmap.h,v 1.1.4.1 1996/05/09 14:54:34 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/memmap.h,v $
 *-----------------------------------------------------------------
 * memmap.h -- header file for ACP/SAR Memory Map lenmpmsg.c (LEC
 *             Message handling on the NMP)
 *
 * November 1994, sakumar
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memmap.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:34  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:59  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:02  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:00  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:30  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.10  1995/10/10  16:37:10  sakumar
 * Cleaned up RSAR and atmdriver by removing canned Synergy Headers in
 * Frame Buffer. The RSAR now dynamically creates the Synergy Header
 * from VC Info. Also, in-band IPC does not use the canned Synergy
 * Header. This checkin frees up space in the Frame Buffer, which
 * is needed for SAR-to-SAR messaging queues.
 *
 * Revision 1.9  1995/10/04  01:32:10  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.8  1995/06/06  01:28:26  sakumar
 * New memory layout: (1) uses all of Frame Buffer
 *                    (2) uses SPRAM.
 *
 * Revision 1.7  1995/03/27  23:35:17  sakumar
 * Added 1 queue for ACP->NMP IPC.
 * Added 3 queues for NMP->ACP IPC.
 * Increased memory available for in VCR VC Info.
 *
 * Revision 1.6  1995/03/18  01:47:35  sakumar
 * Merged txsar and atmdriver with IOS environment
 *
 * Revision 1.5  1994/12/13  16:48:07  sakumar
 * If ATMIZER_EMULATOR is not defined, then the start of shared memory is
 * different between the ACP and the SAR.
 *
 * Revision 1.4  1994/12/08  22:52:33  rahuja
 * *** empty log message ***
 *
 * Revision 1.3  1994/12/05  02:29:39  rahuja
 * Integrated with biga.h and memory mapping for the buffer manager
 *
 * Revision 1.2  1994/11/30  21:05:55  sakumar
 * Added support for different address spaces for ACP and SARs.
 *
 * Revision 1.1  1994/11/29  23:34:33  sakumar
 * Initial Version
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _MEMMAP_H_
#define _MEMMAP_H_

#define SAR_SHARED_MEM_BASE (0xa0800000)
#define ACP_SHARED_MEM_BASE (0xb0000000)

#define	SAR_SECONDARYPORT_BASE	(0xA0C00000)
#define ACP_TXSAR_SECONDARYPORT_BASE (0xA0000000)
#define ACP_RXSAR_SECONDARYPORT_BASE (0xA8000000)

#define SHRAM_SIZE          0x100000
#define ACP_SHRAM_START_C   (ACP_SHARED_MEM_BASE)
#define SAR_SHRAM_START_C   (SAR_SHARED_MEM_BASE)

#define	SAR_BIGA_BASE_C		(SAR_SECONDARYPORT_BASE+0x200000)
 
/*
** The first 256K of each SAR's Secondary memory is for PMON/code, the 
** second  256K is for shared data structures between the SAR and ACP (SPRAM).
*/

#define RX_SAR_SPRAM_SIZE_C         0x40000
#define RX_SAR_SPRAM_START_C        (SAR_SECONDARYPORT_BASE+0x40000)	
#define ACP_RXSAR_SPRAM_START_C     (ACP_RXSAR_SECONDARYPORT_BASE+0x40000)

#define TX_SAR_SPRAM_SIZE_C 		0x40000
#define TX_SAR_SPRAM_START_C        (SAR_SECONDARYPORT_BASE+0x40000)
#define ACP_TXSAR_SPRAM_START_C 	(ACP_TXSAR_SECONDARYPORT_BASE+0x40000)

#define SHMEM_ADDR_MASK		(0xfffff)
#define SPMEM_ADDR_MASK		(0x3ffff)


#define MAP_SP_ACP_RX(x)	(void *) (  ( ((UINT32) (x)) & SPMEM_ADDR_MASK) | RX_SAR_SPRAM_START_C  )
#define MAP_SP_ACP_TX(x)	(void *) (  ( ((UINT32) (x)) & SPMEM_ADDR_MASK) | TX_SAR_SPRAM_START_C  )
#define MAP_SP_RX_ACP(x)	(void *) (  ( ((UINT32) (x)) & SPMEM_ADDR_MASK) | ACP_RXSAR_SPRAM_START_C  )
#define MAP_SP_TX_ACP(x)	(void *) (  ( ((UINT32) (x)) & SPMEM_ADDR_MASK) | ACP_TXSAR_SPRAM_START_C  )

#define MAP_SH_ACP_SAR(x)	(void *) (  ( ((UINT32) (x)) & SHMEM_ADDR_MASK) | SAR_SHRAM_START_C  )
#define MAP_SH_SAR_ACP(x)	(void *) (  ( ((UINT32) (x)) & SHMEM_ADDR_MASK) | ACP_SHRAM_START_C  )
 

/* Defines for SAR interrupt register */
#define SAR_INTR_REG_ADDR_C		(SAR_SECONDARYPORT_BASE+0x280060)
#define	INTR_OTHER_SAR_BIT_C	0x80000000
#define INTR_BY_BIGA_BIT_C		0x40000000
#define SAR_INTR_MSG_MASK_C		0x3f000000
#define INTR_BY_ACP_BIT_C		0x00800000
#define INTR_BY_ACP_BIT_C		0x00800000
#define ACP_INTR_MSG_MASK_C		0x007f0000


#endif /* _MEMMAP_H_ */
