/* $Id: txsar.h,v 1.1.4.2 1996/07/02 23:45:19 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/src-3k-sar/txsar.h,v $
 *-----------------------------------------------------------------
 * txoffdiag.s -- 
 *
 * November 1994, Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: txsar.h,v $
 * Revision 1.1.4.2  1996/07/02  23:45:19  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 1.1.4.1  1996/05/09  15:01:42  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:06  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/03  21:18:30  sakumar
 * LE-ARP indication from TSAR sends SRC address (as well as DEST address).
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:13:48  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:42  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:41  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 *
 *  FILE:  tx_sar.h
 *
 *
 *  CONTENTS:
 *
 *		Tab stop = 4
 *
 *      Assembler Coding Conventions:
 *
 *          <>_REG		for Processor Registers (s0-s7)
 *          <>_AREG		for ATMIZER Registers
 *          <>_s		for structure fields
 *          <>_C 		for defined constants
 *          <>_L 		for program labels
 *			Vcr_<>_s	for varibles in internal VCR, indexed off vcr base
 *
 *	This file describes the constatns, register usage, global variables,
 *	and data structures used by the tx_sar
 *
 *
 *  OWNER:
 *	Ratinder P. S. Ahuja
 *
 *  LOG:
 *	Created:  19 Oct 1994
 *	
*/

#ifndef _TX_SAR_H_
#define _TX_SAR_H_

/*
**	INCLUDES
*/

#include	"mips.h"
#include	"atmizer.h"
#include	"basic_types.h"
#include	"param.h"
#include	"memmap.h"
#include	"shram.h"
#include	"spram.h"
#include	"ovcimap.h"
#include	"cam_if.h"
#include	"sar_if.h"
#include	"cell.h"
#include	"lec_if.h"


/*
**	EXTERNS
*/



/*
**	GLOBALS	
*/



/*
**	CONSTANTS
*/

#define	BIGA_BASE_C			SAR_BIGA_BASE_C	/* from memmap.h				*/
#define	BHEADOFF_C			(RHEAD*16)	/* from biga.h						*/
#define	BTAILOFF_C			(RTAIL*16)	/* from biga.h						*/
#define VCR_BASE_C			VCR_BASE	/* defined in atmizer.h				*/
#define VCR_ADDR_C			0xfff00000 	/* Address of VCR 					*/
#define ASUBR_VALUE_C     	0x282      	/* Value to put into M_ASUBR 		*/
#define	SHM_START_C
#define SPM_START_C
#define	VCR_CELLSZ_C		64			/* cell size in vcr is 64 bytes		*/
#define	RXBUFSZ_C			4			/* vcr has 4 cell recv buffer		*/
#define	TXBUFSZ_C			16			/* vcr has 16 cell tx buffer		*/
#define	VCRSIZE_C			4096		/* 4K vcr 							*/


/*
**	LOCAL VARIABLES IN REGISTERS s0 - s8
*/

#define VCR_REG				s0			/* pointer to start of VCR			*/
#define BIGA_BASE_REG		s1			/* base address of biga registers	*/
#define	pBHEAD_REG			s2			/* current value of head pointer	*/
#define	pBTAIL_REG			s3			/* value of tail pointer			*/
#define pTEMP_BHEAD_REG		s4			/* value of BHEAD as read in sched  */
#define	pACPCMDQ_REG		s5			/* ACP command queue desc			*/
#define	TXLENGTH_REG		s6			/* length of data to be transmitted	*/
#define	AAL5HDR_REG			s7			/* 4 byte atm header contains:		*/
										/* gfc:4, vpi:8, vci:16, pt:3, clp:1*/
#define	VCRTXCELL_REG		s8			/* current tx cell holder in VCR	*/
#define	ENDTXCELL_REG		sp			/* end of tx cell holder are in VCR	*/

#define ACTIVE_VCI_MASK_REG k0			/* used to extract the VCI from VPC */
#define NUM_ACTIVE_VCI_BITS_REG k1		/* to determine where VPI starts in VPC */

#define NUM_REG_SAVE_C		16	

/*
**	ATMIZER REGISTERS, defined as offsets from the VCR_REG
*/

#define CREDIT_AREG     0x4000(VCR_REG)
#define ICREDIT_AREG    0x4080(VCR_REG)
#define STALL_AREG      0x4200(VCR_REG)
#define RXBUFPTR_AREG   0x4400(VCR_REG)
#define TXFIFO_AREG     0x4500(VCR_REG) # "0x4540 if CRC10 needs to be 
										#generated Pg 9-5"
#define RXACK_AREG      0x460c(VCR_REG)
#define SCR_AREG        0x4a00(VCR_REG)
#define CRC32_AREG      0x4c00(VCR_REG)
#define FCRC32_AREG     0x4c80(VCR_REG)
#define HOSTINT_AREG    0x4b00(VCR_REG)
#define CIR_REG9_AREG   0x43a4(VCR_REG)

/*
**	LOCAL VARIABLES IN VCR
*/



/*
**			DATA  STRUCTURES  
*/

/*
**	VCR structure for TX sar:
**		recv cell holders (minimum 4 cells, 256 bytes)
**		transmit cell builder (4 cells, 256 bytes)
**		vc state table	(1024 bytes)
**		currently active vc map entry
**		
**		Last 64 bytes are the idle cell
**		
**
*/

#ifdef LANGUAGE_C

typedef struct vcr_struct {

	UINT8	vcr_rxbuf[VCR_CELLSZ_C*RXBUFSZ_C];	/* recv cell holder			*/
	UINT8	vcr_txbuf[VCR_CELLSZ_C*TXBUFSZ_C];	/* tx cell builder area		*/
	UINT8	vcr_lastcell[VCR_CELLSZ_C];			/* canned last cell			*/
	tBIGARD	vcr_bigard;							/* dma in bigard here		*/
	UINT8	vcr_txvcstate[MAX_DEFAULT_VPC_C];	/* vpc state table			*/
	UINT8	vcr_emtpycell[VCR_CELLSZ_C];		/* empty cell				*/
	UINT32	vcr_omap_tlen;						/* acutal length of data	*/
	UINT32	vcr_omap_cellhdr;					/* haeader under constructn	*/
	tSARCMD	vcr_cmd;							/* a command record for int-*/
												/* ernal use				*/
	UINT32	vcr_bigastart;						/* first biga desc			*/
	UINT32	vcr_bigaend;						/* last biga desc			*/
	UINT32	vcr_txspram_pStats;					/* pointer to stats area 	*/
	UINT32	vcr_txgl_packet_output;				/* count of output packets	*/
	UINT32	vcr_txgl_cell_output;				/* count of output cells  	*/
	UINT32	vcr_bgard_ea;						/* effective addr of bigard	*/
	UINT32	vcr_plectable;						/* address of lec table		*/
	UINT16	vcr_lecid;							/* lecid from lec table		*/
	UINT16	vcr_busvc;							/* bus vc from lec table	*/
	UINT32  vcr_pRxToTxQ;						/* Ptr to RX to TX q		*/
	UINT32  vcr_pTxToRxQ;						/* Ptr to TX to RX q		*/
	UINT32  vcr_pRSARsdatabuffer;				/* RSAR's data Buffer       */
	UINT32	vcr_save1_regt0;					/* reg save area 1 t0-t9 	*/
	UINT32	vcr_save1_regt1;					
	UINT32	vcr_save1_regt2;					
	UINT32	vcr_save1_regt3;					
	UINT32	vcr_save1_regt4;					
	UINT32	vcr_save1_regt5;					
	UINT32	vcr_save1_regt6;					
	UINT32	vcr_save1_regt7;					
	UINT32	vcr_save1_regt8;					
	UINT32	vcr_save1_regt9;					
	UINT32	vcr_save1_regRA;					

} tVCRSTRUCT, *tpVCRSTRUCT;

#else  /* assembly */

	.data
	.struct 0
vcr_rxbuf_s:		.space VCR_CELLSZ_C*RXBUFSZ_C
vcr_erxbuf_s:									/* end of rx buffer			*/
vcr_txbuf_s:		.space VCR_CELLSZ_C*TXBUFSZ_C
vcr_etxbuf_s:									/* end of tx buffer			*/
vcr_lastcell_s:		.space VCR_CELLSZ_C			/* canned last cell			*/
vcr_bigard_s:		.space 50					/* bigard+pad+syhdr+da		*/
vcr_pad_16s:		.half 0
vcr_txvcstate_s:	.space MAX_DEFAULT_VPC_C	/* num vpc bytes			*/
vcr_emtpycell_s:	.space VCR_CELLSZ_C			/* empty cell				*/
vcr_omap_tlen_s:   	.word 0						/* actual length of data	*/
vcr_omap_cellhdr_s:	.word 0						/* cell header for current	*/
												/* packet					*/
vcr_cmd_s:			.space 64					/* has to match tSARCMD as	*/
												/* defined in sar_if.h		*/
vcr_bigastart_s:	.word 0						/* first biga desc			*/
vcr_bigaend_s:		.word 0						/* last biga desc			*/
vcr_txspram_pStats_s:		.word 0				/* ptr to tx stats in spram	*/
vcr_txgl_packet_output_s:	.word 0				/* count of output packets	*/
vcr_txgl_cell_output_s:		.word 0				/* count of output cells  	*/
vcr_bigard_ea_s:	.word 0						/* effective addr of bigard	*/
vcr_plectable_s:	.word 0						/* base of lectable in spram*/
vcr_lecid_16s:		.half 0
vcr_busvc_16s:		.half 0
vcr_pRxToTxQ_s:		.word 0						/* Ptr to RX to TX q		*/
vcr_pTxToRxQ_s:		.word 0						/* Ptr to TX to RX q		*/
vcr_pRSARsdatabuffer_s:	.word 0					/* RSAR's data Buffer       */
vcr_save1_regt0_s:	.word 0						/* reg save area 1 t0-t9 	*/
vcr_save1_regt1_s:	.word 0					
vcr_save1_regt2_s:	.word 0					
vcr_save1_regt3_s:	.word 0					
vcr_save1_regt4_s:	.word 0					
vcr_save1_regt5_s:	.word 0					
vcr_save1_regt6_s:	.word 0					
vcr_save1_regt7_s:	.word 0					
vcr_save1_regt8_s:	.word 0					
vcr_save1_regt9_s:	.word 0					

vcr_save1_regRA_s:  .word 0

vcr_heap_s:       	.space VCRSIZE_C-(vcr_heap_s+VCR_CELLSZ_C)
vcr_idle_s:       	.space VCR_CELLSZ_C			/* idle cell				*/
vcr_size_s:

        .text                           /* reestablish the default      */
#endif /* language */



/*
**	PROTOTYPES
*/

#ifdef LANGUAGE_C
extern  void  txsar_init();
extern	void  txsar_work();
#endif

#endif /* _TX_SAR_H_ */

