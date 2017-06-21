/* $Id: rxsar.h,v 1.1.4.1 1996/05/09 15:01:35 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/src-3k-sar/rxsar.h,v $
 *-----------------------------------------------------------------
 * rxsar.h --  Header file for Receive SAR (Register defns and VCR
 *             Layout
 *
 * November 1994, rahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rxsar.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:35  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:42  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:35  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:36  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.21  1995/10/16  23:35:47  sakumar
 * Added support for:
 *     (1) Topology Change
 *     (2) LES, BUS, LECS on Catalyst 5000 ATM module with BUS implemented
 *         in fast RSAR->TSAR firmware.
 *
 * Revision 1.20  1995/10/04  01:32:05  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.19  1995/09/22  23:21:37  sakumar
 * RSAR performance changes. (Construct Synergy Header on the fly and
 * DMA it to frame buffer; DMA TBD to frame buffer instead of two 4
 * word writes).
 *
 * Revision 1.18  1995/09/12  01:10:30  sakumar
 * More RSAR performance improvements: shadow TBDs in SPRAM to reduce
 * Frame Buffer accesses during free_BIGA_buffers; data cache enabled.
 *
 * Revision 1.17  1995/08/26  18:49:57  sakumar
 * RXSAR performance enhancement:
 * 	Step thru BIGA TBDs by using the compile-time known size instead of
 *     accessing the Frame Buffer to read the LINK field.
 *
 * Revision 1.16  1995/08/23  00:01:05  sakumar
 * RXSAR Performance Enhancements.
 *
 * Revision 1.15  1995/07/25  22:08:01  sakumar
 * Added fields to track number of bytes and pointer to Small Packet.
 *
 * Revision 1.14  1995/06/10  02:07:23  fxu
 * Simplified make procedures
 *
 * Revision 1.13  1995/06/09  02:03:10  sakumar
 * Added frame counters for statistics.
 *
 * Revision 1.12  1995/06/06  17:53:08  sakumar
 * Set Re-assembly timeout to 2 seconds. A "Tick" to .2 seconds. Hence
 * you need 10 ticks (i.e 2 seconds) to timeout.
 *
 * Revision 1.11  1995/04/21  16:43:41  sakumar
 * Added support for runtime specification of active VPI/VCI bits.
 *
 * Revision 1.10  1995/03/29  22:50:35  sakumar
 * Changes to support Echo Suppression and CTRL packet in from_BUS_to_BIGA
 * VC.
 *
 * Revision 1.9  1995/03/27  23:15:17  sakumar
 * Increased mem available for VC info in VCR; added ptr to IPC Q.
 *
 * Revision 1.8  1994/12/16  02:19:33  sakumar
 * Now works on actual target.
 *
 * Revision 1.7  1994/12/06  22:03:15  sakumar
 * Added fields to allow IVCI Map Entries to live in VCR.
 *
 * Revision 1.6  1994/11/17  18:49:07  sakumar
 * Changed memory carving to static model (declared in spram.h and shram.h
 * instead of being allocated on the fly).
 *
 * Also changed producer/consumer queues to use last pointer instead of last+1
 * pointer to detect wraparound condition.
 *
 * Revision 1.5  1994/11/11  19:42:44  sakumar
 * Added BIGA code. Changed structure of test program.
 *
 * Revision 1.4  1994/11/08  23:40:01  sakumar
 * Changed RTO code to use REGs instead of VCR Variables.
 * Fixed a couple RTO bugs (16 bit field instead of 32 bit)
 *
 * Revision 1.3  1994/11/08  22:26:20  sakumar
 * Using t* temp registers instead of s* regs (in preparation for using the
 * the (thus saved) s* regs for caching more useful stuff.
 *
 * Revision 1.2  1994/11/08  18:13:02  sakumar
 * Cleaned up file header
 *
 * Revision 1.1  1994/11/08  18:05:28  rahuja
 * Added to cvs
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RXSAR_H_
#define _RXSAR_H_

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
 * INCLUDES
 */

#include    "mips.h"
#include    "atmizer.h"
#include    "basic_types.h"
#include    "buffer_if.h"
#include    "shram.h"
#include    "spram.h"
#include    "ivcimap.h"
#include    "sar_if.h"
#include    "cell.h"



/* Number of CLK ticks for one timer tick */
#define NUM_TICKS_C	10000000	/* .2 seconds for a 50 MHz clock */

/* Number of Timer Ticks for Reassembly Timeout */
#define	 RTO_INTERVAL_C	10 	/* 2 seconds */

/* Max Number of Cells to process continuously before looking at Command Q */
#define MAX_BUFS_C	8	/* Max buffers queued to BIGA */

#define ALIGN_C 	16

/*
 * Addresses
 */
#define VCR_ADDR_C          0xfff00000  /* Address of VCR */
#define ASUBR_VALUE_C       0x2       	/* Value to put into M_ASUBR CACHE EN */
 
#define VCRSIZE_C           4096        /* in bytes */


#define	SYN_HDR_LEN_C		12			/* Synergy Header Length */


/*
 * Definitions for User Defined bits in BIGA Transmit Buffer Descriptor
 */

#define	BIGA_BUFFER_C	 0x01000000	/* Return buffer to BIGA buffer Queue	*/
#define	ACP_BUFFER_C	 0x02000000	/* Return buffer to ACP					*/
#define	SYN_HDR_BUFFER_C 0x03000000	/* Buffer is Synergy header do nothing	*/
#define	BUF_TYPE_MASK_C	 0x0f000000	/* Mask field						  	*/

#ifdef LANGUAGE_C

void rxsar_init();
void rxsar_work_L();

typedef void Func();
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

#else /* LANGUAGE_ASSEMBLY */


/* 
 * register assignments 
 */
#define	VCR_REG		  		s0	/* pointer to start of VCR */
#define	TBD_pkt_count_REG	s1	/* number of packets queued to BIGA */
#define VC_TAB_PTR_REG  	s2	/* pointer to VC Table Pointer Array */
#define pVC_Info_REG    	s3	/* pointer to VC tab entry for this cell */
#define	RTO_HEAD_REG		s4  /* pointer to head VC of Reassembly Timeout List */
#define	RTO_TAIL_REG		s5  /* pointer to tail VC of Reassembly Timeout List */
#define	BIGA_THEAD_REG		s6  /* pointer to head of BIGA TBD list */
#define	BIGA_TTAIL_REG		s7  /* pointer to tail of BIGA TBD list */
#define	BIGA_BASE_REG		s8	/* pointer to start of BIGA Registers */

#define	ACTIVE_VPI_VCI_BITS_REG	k0	/* to mask the active bits in Rcv Cell */
#define	NUM_VPI_SHIFT_BITS_REG	k1  /* number of bits to shift VPI right   */

#define PLP1TBD_REG	gp				/* Pointer to last + 1 TBD */
#define GL_PKT_COUNT_REG    v1  /* Counts total number of packets received */

#define SHAD_THEAD_REG a3  /* HEAD TBD Shadow SPRAM pointer */
#define SHAD_TTAIL_REG sp  /* TAIL TBD Shadow SPRAM pointer */


/* 
 * ATMizer register access method 
 */
#define CREDIT_AREG		0x4000(VCR_REG)
#define ICREDIT_AREG	0x4080(VCR_REG)
#define STALL_AREG		0x4200(VCR_REG)
#define RXBUFPTR_AREG	0x4400(VCR_REG)
#define TXFIFO_AREG		0x4500(VCR_REG) # "0x4540 if CRC10 needs to be generated Pg 9-5"
#define RXACK_AREG		0x460c(VCR_REG)
#define SCR_AREG		0x4a00(VCR_REG)
#define CRC32_AREG		0x4c00(VCR_REG)
#define FCRC32_AREG		0x4c80(VCR_REG)
#define HOSTINT_AREG	0x4b00(VCR_REG)
#define	CIR_REG9_AREG	0x43a4(VCR_REG)


/*
 * *** IMPORTANT *** 
 * If you change any of the following (including definition of the 
 * RXSAR VCR area),  make sure you also change VCR_IVCI_MAP_AREA_START_ADDR 
 * and  VCR_IVCI_MAP_AREA_END_ADDR in atmdrv_if.h
 * *** IMPORTANT *** 
 */
#define NUM_RX_CELLS_C	16 /* Number of RX Cells in VCR */

	.data
	.struct 0
/* Definition of VCR Memory */
VCR_rxbuf_s: .space vcrcell_size_s*NUM_RX_CELLS_C
VCR_bigatd_pData_s:     .word 0
VCR_bigatd_control_s:   .word 0
VCR_bigatd_EA_s:		 .word 0
VCR_sy_hdr_EA_s:		 .word 0
VCR_time_s:  .word	0			# System time -- for Reassembly Timeout
VCR_pAcpCmdq_s:  .word 	0		# pointer to ACPs command queue
VCR_pAcpRespq_s: .word	0		# pointer to ACPs response queue
VCR_pRxToAcpDataq_s: .word	0	# pointer to ACPs Data Queue (RXSAR producer)
VCR_pAcpBufPool_s:	.word	0	# pointer to ACP Buffer Pool (RXSAR consumer)
VCR_BigaBufPool_s: .space 16
VCR_pAcpToNnpIpcPool_s: .word 0	# pointer to IPC Bufs Pool (to be returned)
VCR_pGlobalStats_s:	.word 0 	# pointer to Global Statistics
VCR_pRxToTxQ_s:		.word 0		# pointer to RX to TX queue
VCR_pTxToRxQ_s:		.word 0		# pointer to TX to RX queue
#ifdef SAGE1_LOCKUP_WORKAROUND
VCR_pSmall_Packet_s: .word 0	# pointer to pkt to be sent every 20000 bytes
VCR_Byte_Count_s: .word 0		# number of bytes received
#endif /* SAGE1_LOCKUP_WORKAROUND */
VCR_heap_s:	.space 4096-VCR_heap_s
VCR_size_s:

	.text
#endif /* LANGUAGE ASSEMBLY */

#endif /* _RXSAR_H_ */
