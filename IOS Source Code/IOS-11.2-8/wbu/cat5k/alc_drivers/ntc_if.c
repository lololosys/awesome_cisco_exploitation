/* $Id: ntc_if.c,v 1.1.4.1 1996/05/09 14:55:05 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/ntc_if.c,v $
 *------------------------------------------------------------------
 * ntc_if.c -- NTC initialization
 *
 * February 1995, rahuja
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntc_if.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:05  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:38  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:32  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:01  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.18  1995/10/18  23:36:33  sakumar
 * Due to the PMC erroneously counting idle cells as real cells, the TSAR
 * now keeps the output cell count. This count is then multiplied by
 * 48 to get the octet count.
 *
 * We still use the PMC's receive cell count.
 *
 * Revision 1.17  1995/10/04  01:30:55  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.16  1995/09/08  23:14:45  sakumar
 * Deleted some more printfs.
 *
 * Revision 1.15  1995/09/08  23:11:34  sakumar
 * Disabled NTC printfs.
 *
 * Revision 1.14  1995/08/29  00:38:24  sakumar
 * Took out Path Yellow indications for both Rev A and Rev B.
 *
 * Revision 1.13  1995/08/22  23:46:33  sakumar
 * Made the following changes:
 * (1) Link LED and link state based on ICD/OCD with the proviso that
 *     we check the NTC_PHY_ALARM register for B1/B2 Error conditions
 *     to determine the actual line state (i.e. if ICD and OCD came in
 *     on the same interrupt). **HACK** put in 10 ms wait before reading
 *     NTC_PHY_ALARM alarm register.
 *
 * (2) Disable DMA interrupts and LOF and SOFRX interrupts.
 *
 * Revision 1.12  1995/08/02  22:52:51  sakumar
 * The NTC interrupt handler function now calls the atmdriver directly when
 * it detects a Loss of Framing or In Framing.
 *
 * Revision 1.11  1995/07/14  23:13:37  sakumar
 * Channel 0 enable done at the end (on advice from Fujitsu).
 *
 * Revision 1.10  1995/07/05  21:00:52  sakumar
 * Changed NTC DMA scheme.
 *
 * Revision 1.9  1995/06/22  17:56:45  sakumar
 * Added ntc_get_framer_status(). Also, the bottom LED works off of the
 * Framer status for both Rev A and Rev B NTCs.
 *
 * Revision 1.8  1995/06/17  02:22:04  sakumar
 * Enabled LED for LOF.
 *
 * Revision 1.7  1995/06/16  00:32:47  sakumar
 * Changes for enabling NTC dma isr.
 *
 * Revision 1.6  1995/06/09  02:00:37  sakumar
 * ntc_dms_isr() now reads the dma stat buffer and increments the
 * appropriate statistics fields.
 *
 * Revision 1.5  1995/06/01  22:34:57  sakumar
 * Added support for Rev B NTC.
 *
 * Revision 1.4  1995/05/06  00:36:11  sakumar
 * Fixed for Generating Unassigned cells (and NOT Idle cells) for cell rate
 * decoupling. Also, for recieve, the NTC discards both Unassigned Cells and
 * Idle cells and does not pass them up to the RXSAR.
 *
 * Revision 1.3  1995/04/21  16:57:22  sakumar
 * Added support for NTC DMAing per-VC cell stats to ACP. (Does not work yet,
 * hence currently disabled).
 *
 * Revision 1.2  1995/02/16  03:48:06  rlowe
 * Changes for initial integration of ATM driver with IOS.
 *
 * Revision 1.1  1995/02/06  19:00:13  rahuja
 * Changed OK and ERR to ACP_
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 *
 *  FILE: ntc_if.c
 *
 *  CONTENTS:
 *
 *  File contains the NTC initialization 
 *
 *  OWNERS:
 *      Ratinder P. S. Ahuja
 */
 
/*
**	INCLUDES
*/

#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif

#include	"acp_basic_types.h"
#include 	"lo_wdog.h"
#include	"acp_hw_defs.h"
#include 	"acp_c_defs.h"
#include	"mfp_hw_defs.h"
#include	"mfp_acp_defs.h"
#include	"ntc_if.h"
#include	"spram.h"
#include	"led_if.h"


/*
**	GLOBALS
*/

UINT32 ntc_link_state= IN_FRAMING;

/* Channel descriptor location table */

tCHANNELDESC_LOC channel_desc_table[NUM_NTC_DMA];
tpCHANNELDESC_LOC   pDescLocTable = &channel_desc_table[0];

int ntc_hw_rev_b;		/* Hardware revision */

extern       tpRXSPRAM       acp_pRxSpram;
extern       tpTXSPRAM       acp_pTxSpram;


/* Channel descriptors */

tCHANNEL_DESC	ch0desc;

/* DMA buffers	*/
tpNTC_DMA_CH0_BLK	p_ntc_dma_ch0_blk ;

extern tpNTC_DMA_CH0_BLK NTC_InitDmaBuf (void) ;

/* Function pointer to atmdriver upcall */
void (* ntc_line_state_change_upcall) (int) ;

/*
**	STATIC
*/


/*
**	PROTOTYPES
*/

void	ntc_txframer_init();
void    ntc_cellrecv_init();
void    ntc_cellxmit_init();
void    ntc_oam_init();
void    ntc_stats_init();
void    ntc_cell_stream_init();
void    ntc_dma_setup();

UINT32	ntc_control(UINT32	opcode);


void
ntc_dump_stats()
{
        ntc_control(NTC_RXTX_FILL);
        ntc_control(NTC_OAM_ALARMS);
        ntc_control(NTC_RECV_FRAME);
		
}

void
ntc_show_status_regs()
{
	UINT32 regval;

#ifdef PRINT_DEBUG
	buginf("\n *** NTC STATUS ***");
	mREAD_NTC(NTC_RECVFRAMER_BYTE1, regval);
	buginf("\nC1: 0x%x  R1: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE2, regval);
	buginf("\nR2: 0x%x  R1: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE3, regval);
	buginf("\nF1: 0x%x  R3: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE4, regval);
	buginf("\nR4: 0x%x  D1: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE5, regval);
	buginf("\nD2: 0x%x  D3: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE6, regval);
	buginf("\nK1: 0x%x  D4: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE7, regval);
	buginf("\nD5: 0x%x  D6: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE8, regval);
	buginf("\nD7: 0x%x  D8: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE9, regval);
	buginf("\nD9: 0x%x  D10: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE10, regval);
	buginf("\nD11: 0x%x  D12: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE11, regval);
	buginf("\nZ1A: 0x%x  Z1B: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE12, regval);
	buginf("\nZ1C: 0x%x  E2: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE13, regval);
	buginf("\nR5: 0x%x  R6: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE14, regval);
	buginf("\nJ1: 0x%x  C2: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE15, regval);
	buginf("\nF2: 0x%x  Z3: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 
	mREAD_NTC(NTC_RECVFRAMER_BYTE16, regval);
	buginf("\nZ4: 0x%x  Z5: 0x%x", ((regval>>8) & 0xff), (regval & 0xff)); 

	mREAD_NTC(NTC_PHY_ALARM, regval);
	buginf("\n Physical Layer Alarms Received:");
	if (regval & 0x1) {
		buginf("\nMS-AIS");
	}
	regval >>= 1;

	if (regval & 0x1) {
		buginf("\nMS-FERF");
	}
	regval >>= 1;

	if (regval & 0x1) {
		buginf("\nB1ERR");
	}
	regval >>= 1;

	if (regval & 0x1) {
		buginf("\nB2ERR");
	}
	regval >>= 1;

	if (ntc_hw_rev_b) {
		if (regval & 0x1) {
			buginf("\nOCD");
		}
		regval >>= 1;

		if (regval & 0x1) {
			buginf("\nLOP");
		}
		regval >>= 1;

		if (regval & 0x1) {
			buginf("\nLOF");
		}
		regval >>= 1;

		if (regval & 0x1) {
			buginf("\nLOS");
		}
		regval >>= 1;
	} else {
		regval >>= 4;
	}

	if (regval & 0x1) {
		buginf("\nP-AIS");
	}
	regval >>= 1;
	
	if (regval & 0x1) {
		buginf("\nP-FERF");
	}
	regval >>= 1;

	if (regval & 0x1) {
		buginf("\nB3ERR");
	}
	regval >>= 3;

	if (regval & 0x1) {
		buginf("\nNDF");
	}
	regval >>= 1;

	if (regval & 0x1) {
		buginf("\nNJUST");
	}
	regval >>= 1;
	
	if (regval & 0x1) {
		buginf("\nPJUST");
	}
#endif /* PRINT_DEBUG */
}


/* --------------------------------------------------------------------	*/
/*	    This routine initializes the MFP for NTC interrupts.            */
/*		This interface is more general purpose than currently used,		*/
/*		to allow for future growth/usage of the driver.					*/
/* --------------------------------------------------------------------	*/
void
NTC_IntrInit (void (** VecTbl)(), u_int8 fReset, u_int8 fMaskIt)
{
	register volatile u_int8 *	pReg ;
	s_int16						Count ;


	/* RKL:  Need to groom MFP here, both for interrupt lines and timer. */

	pReg = ((volatile u_int8 *) (C_ADDR_HW_REGS (0))) ;


		/* Whenever the NTC is reset, ensure that the DMA area is */
		/* put into a known (zero) state as well.                 */
	if (fReset)
	{
		pReg [ACP_WRRG_PMD_RESET_] = ON_ ;
		pReg [ACP_WRRG_NTC_RESET_] = ON_ ;

		for (Count = 0 ; Count < 10 ; )	/* Pause to take effect. */
			{ ++ Count ; }

			/* Init the NTC DMA area, and get a pointer to it in non-cacheable ACP DRAM space. */
		p_ntc_dma_ch0_blk = NTC_InitDmaBuf () ;
	}

		/* Setup MFP IO lines for NTC DMA. */
	MFP_BindEnb (VecTbl, NULL, MFP_BASE_VECTOR, MFP_IINDX_NTC) ;

		/* At this point NTC interrupts are enabled but masked off   */
		/* at the MFP.  We can unmask them elsewhere, eg. in IOS.    */
		/* NTC uses PMD, so drop PMD from reset (with NTC) too.      */
		/* Is ordering an issue here ?                               */
	if (! fMaskIt)
	{
		MFP_UnmaskIntrW (MFP_IMSK_NTC, FALSE, FALSE) ;
	}
	pReg [ACP_WRRG_PMD_RESET_ ] = OFF_ ;
	pReg [ACP_WRRG_NTC_RESET_ ] = OFF_ ;

}	/* NTC_IntrInit */


/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           ACP_OK
*			ACP_ERR
*
*   ALGORITHMS
*
*
*******************************************************************************
*/
UINT32
ntc_init()
{
UINT32	regval;
UINT32	i;

	/*
	**	Drop NTC/PMD out of reset.  Also condition the MFP to allow interrupts for NTC.
	*/
#ifdef PRINT_DEBUG
	printf("\n\r ntc_init: dropping NTC and PMD from reset") ;
#endif
	NTC_IntrInit (NULL, TRUE, FALSE) ;

	/* Init upcall function pointer */
	ntc_line_state_change_upcall = NULL;

    /*
    ** Determine hardware revision and set global flag accordingly
    */
 
    mREAD_NTC(NTC_BUFOVERFLOW_INTR, regval);    /* Bits d8 and d9 of SR 61 show
hw rev */
    regval &= HW_REV_MASK;
 
    /* non-zero regval here indicates NTC Rev B */
    ntc_hw_rev_b = (regval ? 1 : 0);

	/*
	**	Control register 1
	*/
	
	if (ntc_hw_rev_b) {
		regval = ( XMODE_SONET_STS3C | STMSCRABLER_ENABLE | 
				  STMDESCRABLER_ENABLE | FRAME_SYNCFOUND|FRAME_SYNCLOST);
	} else {
		regval = ( FRAME_ENABLE | XMODE_SONET_STS3C | STMSCRABLER_ENABLE | 
				  STMDESCRABLER_ENABLE | FRAME_SYNCFOUND|FRAME_SYNCLOST);
	}
	
#ifdef PRINT_DEBUG
	printf("\n\r ntc_init: NTC_FRAMER_CONTROL address:%x", NTC_FRAMER_CONTROL);
	printf("\n\r ntc_init: writing NTC_FRAMER_CONTROL:%x", regval);
#endif
	mWRITE_NTC(NTC_FRAMER_CONTROL, regval);
	
#ifdef PRINT_DEBUG
	mREAD_NTC(NTC_FRAMER_CONTROL, regval);
	printf("\n\r ntc_init: read NTC_FRAMER_CONTROL:%x", regval);
#endif

	if (ntc_hw_rev_b) {
		regval = (NTC_TX_FRAMER_ENABLE | NTC_RX_FRAMER_ENABLE);
    	mWRITE_NTC(NTC_FRAMER_ENABLE_CONTROL, regval);
#ifdef PRINT_DEBUG
    	mREAD_NTC(NTC_FRAMER_ENABLE_CONTROL, regval);
    	printf("\n\r ntc_init: read NTC_FRAMER_ENABLE_CONTROL:%x", regval);
#endif
	}

	/*
	** Initialise transmit framer overhead byte block
	*/

	ntc_txframer_init();
	ntc_cellrecv_init();
	ntc_cellxmit_init();

	ntc_oam_init();
	ntc_stats_init();
	ntc_cell_stream_init();
	ntc_dma_setup();

#if	0
	ntc_control(NTC_CSI_LOOPBACK_ON);
#endif
#if	0
	ntc_control(NTC_FRAMER_LOOPBACK_ON);
#endif

/*#define PMD_EXT_LB*/
#ifdef PMD_EXT_LB

	*(UINT8 *)0x3800000e = 0;		/* turn on electrical loopback	*/

#else /* by default loopback off */

	*(UINT8 *)0x3800000e = 1;		/* turn off electrical loopback	*/

#endif


	for(i=0;i<10;i++) {

		ntc_control(NTC_RXTX_FILL);
		ntc_control(NTC_OAM_ALARMS);
		ntc_control(NTC_RECV_FRAME);
		PUNCH_WD ;
	}

	return ACP_OK;

}/*end main */



/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_txframer_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*			Set SONET section overhead: A1, A2, C1, B1
*				 line overhead:	H1, H2, H3, B2, K2, Z2
*				and path overheads: J1, B3, C2, G1
*
*******************************************************************************
*/
void
ntc_txframer_init()
{
UINT32	regval;

	/* 
	**
	**	Control register 2 : C1R1 
	**
	*/
	regval = ( (STS1_ID_C1 << SDH_SONET_C1_SHIFT) | STS1_ID_R1 );
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES1 C1R1:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES1, regval);


	/* 
	**	
	**	Control register 3 : R2E1
	**	
	*/

	regval =  (STS1_ID_R2 << SDH_SONET_R2_SHIFT); 
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES2 R2E1:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES2, regval);


    /* 
	**
	**	Control register 4 : F1R3
 	**
	*/

    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES3 F1R3:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES3, regval);


	/* 
	**
	**	Control register 5: R4D1 
	**
	*/
	
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES4 R4D1:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES4, regval);

    /* 
    **
    **  Control register 6: D2D3
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES5 D2D3:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES5, regval);

    /*
    **
    **  Control register 7: K1D4
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES6 K1D4:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES6, regval);

    /*
    **
    **  Control register 8: D5D6
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES7 D5D6:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES7, regval);
 

    /*
    **
    **  Control register 9: D7D8
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES8 D7D8:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES8, regval);
 

    /*
    **
    **  Control register 10: D9D10
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES9 D9D10:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES9, regval);
 

    /*
    **
    **  Control register 11: D11D12
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES10 D11D12:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES10, regval);
 

    /*
    **
    **  Control register 12: Z1AZ1B
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES11 Z1AZ1B:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES11, regval);
 

    /*
    **
    **  Control register 13: Z1CE2
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES12 Z1CE2:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES12, regval);
 

    /*
    **
    **  Control register 14: R5R6
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES13 R5R6:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES13, regval);
 

    /*
    **
    **  Control register 15: J1C2
	**		J1: STS path trace
	**		C2: Path signal level monitor, set to 00010011
    **
    */
 
    regval =  C2VAL;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES14 J1C2:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES14, regval);


    /*
    **
    **  Control register 16: F2Z3
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES15 F2Z3:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES15, regval);


    /*
    **
    **  Control register 17: Z4Z5
    **
    */
 
    regval =  0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_init: writing NTCTX_FRAMER_BYTES16 Z4Z5:%x", regval);
#endif
    mWRITE_NTC(NTCTX_FRAMER_BYTES16, regval);
 

	return;

}/*end ntc_txframer_init */



/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_cellrecv_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/
void
ntc_cellrecv_init()
{
UINT32	regval;

	/*
	**
	**	Control register 18: cell recv general control
	**
	*/
	regval = ( CELL_RECV_GFC| RECVOAM_CRC10GENERATE| CELL_RECV_ENABLE );
	regval = 0x0107;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_cellrecv_init: writing NTCRECV_CONTROL :%x", regval);
#endif

    mWRITE_NTC(NTCRECV_CONTROL, regval);

	/*
	**
	**	Control register 19: Cell recv HEC control
	**
	*/

	regval = (RECV_DESCRAM_ENABLE | RECV_HECMASK_ENABLE | 
				ERROR_CORRECT_ENABLE | BYTE_ALLIGN | CELL_SYNCFOUND | 
				CELL_SYNCLOST);

#ifdef PRINT_DEBUG
    printf("\n\r ntc_cellrecv_init: writing NTCRECV_HEC_CONTROL :%x", regval);
#endif
    mWRITE_NTC(NTCRECV_HEC_CONTROL, regval);


	/*
	**	
	**	Control reg 20, 21: recv xmask 1,2
	**
	*/
	
	regval = CTRL_XMASK_DISCARD_UNASS_AND_IDLE1;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_cellrecv_init: writing NTCRECV_XMASK1 :%x", regval);
#endif
    mWRITE_NTC(NTCRECV_XMASK1, regval);

	regval = CTRL_XMASK_DISCARD_UNASS_AND_IDLE2;
    mWRITE_NTC(NTCRECV_XMASK2, regval);

    /*
    ** 
    **  Control reg 22, 23: recv smask 1,2
    **
    */
    
    regval = CTRL_SMASK_DISCARD_UNASS_AND_IDLE1;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_cellrecv_init: writing NTCRECV_SMASK1 :%x", regval);
#endif
    mWRITE_NTC(NTCRECV_SMASK1, regval);

    regval = CTRL_SMASK_DISCARD_UNASS_AND_IDLE2;
    mWRITE_NTC(NTCRECV_SMASK2, regval);
 

	/*
	**	
	**	Control reg 24: Cell recv extract buffer control
	**
	*/
    regval = 0;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_cellrecv_init: writing NTCRECV_EXTRACT_CONTROL :%x", 
			regval);
#endif
    mWRITE_NTC(NTCRECV_EXTRACT_CONTROL, regval);

    /*
    ** 
    **  Control reg 25: Cell recv discard buffer control
    **
    */
    regval = RECV_UNASSIGN_DISCARD | RECV_DISCARD_MASK_DEF;
#ifdef PRINT_DEBUG
    printf("\n\r ntc_cellrecv_init: writing NTCRECV_DISCARD_CONTROL :%x", regval
);
#endif
    mWRITE_NTC(NTCRECV_DISCARD_CONTROL, regval);
 
	return;

}/* ntc_cellrecv_init */




/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_cellxmit_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/

void
ntc_cellxmit_init()
{
UINT32	regval;

	/*
	**	
	**	Control reg 26: xmit control
	**
	*/
	regval = ( TXOAM_CRC10CHECK_GENERATE | 
				CELL_TX_GFC | CELLTX_ENABLE| 
				CELL_TX_UASSIGN_GEN );

#ifdef PRINT_DEBUG
    printf("\n\r ntc_cellxmit_init: writing NTCXMIT_CONTROL :%x", regval);
#endif
    mWRITE_NTC(NTCXMIT_CONTROL, regval);
 
	
	/*
	**	
	**	Control reg 27: xmit hec control
	**
	*/
	regval = (TX_SCRAM_ENABLE | TX_HECMASK_ENABLE);
    mWRITE_NTC(NTCXMIT_HEC_CONTROL, regval);

	/*
	**	
	**	Control reg 28-31: xmit xmask1,2, smask1,2
	**
	*/
	regval = 0;
    mWRITE_NTC(NTCXMIT_XMASK1, regval);
    mWRITE_NTC(NTCXMIT_XMASK2, regval);
    mWRITE_NTC(NTCXMIT_SMASK1, regval);
    mWRITE_NTC(NTCXMIT_SMASK2, regval);

	/*
	**	
	**	Control reg 32: xmit extract buffer control
	**
	*/
	regval = 0;
    mWRITE_NTC(NTCXMIT_EXTRACT_CONTROL, regval);

    /*
    **
    **  Control reg 33: xmit discard buffer control
    **
    */
    regval = 0;
    mWRITE_NTC(NTCXMIT_DISCARD_CONTROL, regval);

	return;

}/*end ntc_cellxmit_init */



/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_oam_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/

void
ntc_oam_init()
{
UINT32  regval;
 
    /*
    **
    **  Control reg 34: OAM control
    **
    */
   	regval = (	OAM_ENABLE | NORMAL_BIP |
					TX_PATH_FEBE_ENABLED | TX_SECTION_FEBE_ENABLE );
 
#ifdef PRINT_DEBUG
    printf("\n\r ntc_cellxmit_init: writing NTC_OAM_ALARM_CONTROL :%x", regval);
#endif
    mWRITE_NTC(NTC_OAM_ALARM_CONTROL, regval);

}/*end ntc_oam_init */



/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_stats_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/

void    
ntc_stats_init()
{
UINT32	regval = 0;

    /*
    **
    **  Control reg 50: Switch statistics
    **
    */
	regval = 0;
	mWRITE_NTC(NTCSWITCH_STATS_CONTROL, regval);
	
    /*
    **
    **  Control reg 51: Network statistics general control
    **
    */
	if (ntc_hw_rev_b) {
		regval = (NETSTAT_HEADER_32VCI | RXSTAT_ENABLED | TXSTAT_ENABLED | 
				  VPVC_MASK_DISABLED | 
#ifndef SAKUMAR
				  DMA_TIMER_ENABLE | 
                  (0x1000 & DMA_TIMEOUT_MASK) | /* 2^(26-2)*40 nano-seconds */
#else
                  FORCE_DMA_TRANSFER |
#endif
                  DMA_COUNT_OVERFLOW_EN | DMA_TABLE_OVERFLOW_EN );
	} else {
		regval = (NETSTAT_HEADER_32VCI | RXSTAT_ENABLED | TXSTAT_ENABLED |
                  VPVC_MASK_DISABLED | FORCE_DMA_TRANSFER | DMA_OVERFLOW_ENABLE );
	}
	
#ifdef PRINT_DEBUG
    printf("\n\r ntc_stats_init: writing NTCNET_STATS_CONTROL :%x", regval);
#endif
	mWRITE_NTC(NTCNET_STATS_CONTROL, regval);

	return;

}/*end ntc_stats_init */



/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_cell_stream_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/

void    
ntc_cell_stream_init()
{
UINT32	regval = 0;


	/* Control register 56 */
	if (ntc_hw_rev_b) {
		 regval = (CELL_STREAM_TX_ENABLE | CELL_STREAM_RX_ENABLE |
			       RXATC_DISABLED | TXATC_DISABLED );
	} else {
		 regval = ( CELL_STREAM_ENABLE | RXATC_DISABLED | TXATC_DISABLED );
	}
   
#ifdef PRINT_DEBUG
    printf("\n\r ntc_cell_stream_init: writing NTC_CELLSTREM_CONTROL1 :%x", regval);
#endif
    mWRITE_NTC(NTC_CELLSTREM_CONTROL1, regval);

	/* Control register 57 */
	regval = (RXHEC_ENABLE | RXHECMASK_ENABLE);
#ifdef PRINT_DEBUG
    printf("\n\r ntc_cell_stream_init: writing NTC_CELLSTREM_CONTROL2 :%x", regval);
#endif
    mWRITE_NTC(NTC_CELLSTREM_CONTROL2, regval);
 
	return;

}/*end ntc_cell_stream_init */



/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_interrupt_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/
 
void    
ntc_interrupt_init()
{
UINT32	regval = 0;

	/* Control register 58: interrupt enable */
	regval  = (
#ifdef ENABLE_NTC_DMA_STATS
	DMA_INTR |
#endif /* ENABLE_NTC_DMA_STATS */
	BUFOV_INTR |
	ICD_INTR_ENABLE | OCD_INTR_ENABLE | BERR_INTR_ENABLE );
    mWRITE_NTC(NTC_INTR_ENABLE, regval);

	/* Control register 59: Clear any pending interrupts */
	regval = 0;
	mWRITE_NTC(NTC_INTR_UNDERSERVICE, regval);

	return;

}/*end ntc_interrupt_init */


/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_dma_setup: Sets up DMA, but does not ENABLE it.
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/

void    
ntc_dma_setup()
{
UINT32  regval = 0;

	/* first init the Descriptor Location Table (for Channel 0 only) */
	channel_desc_table[0].channel_loc_u.channel_loc = 0;
	channel_desc_table[0].channel_loc_u.channel_loc_bits.ccr_dec_inc = 1;
	channel_desc_table[0].channel_loc_u.channel_loc_bits.descriptor_ptr = 
											(UINT32) ((((UINT32)&ch0desc)>>2) & 0xffffff);

	/* Then the Channel 0 Descriptor itself.  The DMA buffer pointer is to non-cacheable */
	/* ACP DRAM.  We massage it to be a normal (cacheable) DRAM address before chopping  */
	/* it into a valid NTC DMA address.													 */
	ch0desc.pData = (void *) ((( (((UINT32) p_ntc_dma_ch0_blk) & (~ACP_HW_NCRAM_ADRBIT)) >>2) & 0xffffff) | DIE_BCF);
	ch0desc.count = ( (UINT32)(sizeof(tNTC_DMA_CH0_BLK)) >> 1);
	ch0desc.resvd = 0;
	ch0desc.lcc   = 0;
	ch0desc.blockstatus = 0;
	ch0desc.pNext = (void *) 0;
	
    /* Control register 60: low 16 bits of DAM descriptor pointer table */
    regval  = ( (UINT32)((UINT32)pDescLocTable>>2) & 0x0000ffff);
    mWRITE_NTC(NTC_DMADESC_PTRTABLE_LO, regval);
 
    /* Control register 61: high 7 bits of DAM descriptor pointer table */
    regval  = ( (((UINT32)pDescLocTable>>2) & 0x07f0000) >> 16);
    mWRITE_NTC(NTC_DMADESC_PTRTABLE_HI, regval);

    /* Control register 62: DMA channel activity */
	regval = CHANNEL0_ENABLE;
    mWRITE_NTC(NTC_DMA_ACTIVITY, regval);

    return;
 
}/*end ntc_dma_setup */

/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_dma_init: Actually start the statistics DMA
*			NOTE: Called only AFTER interrupts have been enabled
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/

void
ntc_dma_init()
{
	UINT32	regval;

    /* Control register 62: DMA channel activity */
	regval = CHANNEL0_ENABLE;
    mWRITE_NTC(NTC_DMA_ACTIVITY, regval);

    /* Control register 63: DMA mode */
    regval = (DMA_ENABLE | DMA_PRIORITY_0);
    mWRITE_NTC(NTC_DMA_MODE, regval);
}


/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_dma_isr: Process NTC Statistics DMA Interrupt
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/
 
void
ntc_dma_isr()
{
	int i;
	tVCI	vci;
	tVPI	vpi;
	tVPC	vpc;
	UINT32 regval;

	/* Determine which DMA channel interrupted */
	mREAD_NTC(NTC_BERRDMA_INTR, regval);

	if ((regval & CHANNEL0_ENABLE) == 0) {
		/* We are only interested in Channel 0 (NSR) data */
		return;
	}

#ifdef PRINT_DEBUG
	buginf("\nntc_dma_isr: Entered\n");
#endif /* PRINT_DEBUG */

	/* Update global statistics */
	acp_pRxSpram->rxstat_gl.rxgl_ntccell_drop +=
					p_ntc_dma_ch0_blk->nsr_hdr.nsrhdr_dcount;
	acp_pRxSpram->rxstat_gl.rxgl_ntccell_recvd +=
					p_ntc_dma_ch0_blk->nsr_hdr.nsrhdr_rcount;
	acp_pRxSpram->rxstat_gl.rxgl_ntchec_detect +=
					p_ntc_dma_ch0_blk->nsr_hdr.nsrhdr_hec_detected;
	acp_pRxSpram->rxstat_gl.rxgl_ntchec_correct +=
					p_ntc_dma_ch0_blk->nsr_hdr.nsrhdr_hec_corrected;

	acp_pTxSpram->txstat_gl.txgl_cell_xmit +=
					p_ntc_dma_ch0_blk->nsr_hdr.nsrhdr_tcount;

	/* Update statistics for each valid user block */
	for(i=0; i<16; i++) {

	   if (p_ntc_dma_ch0_blk->user_2cell_stats[i].vcivpi1_u.vcivpi_bits.active){
			vpi =
			p_ntc_dma_ch0_blk->user_2cell_stats[i].vcivpi1_u.vcivpi_bits.vpi;
			vci  = 
			p_ntc_dma_ch0_blk->user_2cell_stats[i].vcivpi1_u.vcivpi_bits.vci;

			
			/* 
			**	Increment cell stats in per vc stat record
			**	use vci/vpi to get to vpc.  Use vpc as index to stat record
			*/

			vpc = atm_get_vpc(vci, vpi);
			if (aalsap_is_vpc_active(vpc) == TRUE) {

				acp_pTxSpram->txstat_vc[vpc].txvc_ntccell_output +=
					p_ntc_dma_ch0_blk->user_2cell_stats[i].txcell_count1;

				acp_pRxSpram->rxstat_vc[vpc].rxvc_ntccell_input +=
					p_ntc_dma_ch0_blk->user_2cell_stats[i].rxcell_count1;
#ifdef COMMENTED_OUT
				buginf("vpi: %d vci: %d rxcell_count: %d txcell_count: %d\n",
					vpi, vci,
				p_ntc_dma_ch0_blk->user_2cell_stats[i].rxcell_count1,
				p_ntc_dma_ch0_blk->user_2cell_stats[i].txcell_count1);
#endif /* COMMENTED_OUT */
			}

		}/* if first vc active */

	   if (p_ntc_dma_ch0_blk->user_2cell_stats[i].vcivpi2_u.vcivpi_bits.active){
      
			vpi =
			p_ntc_dma_ch0_blk->user_2cell_stats[i].vcivpi2_u.vcivpi_bits.vpi;
			vci  = 
			p_ntc_dma_ch0_blk->user_2cell_stats[i].vcivpi2_u.vcivpi_bits.vci;

            /*
            **  Increment cell stats in per vc stat record
            **  use vci/vpi to get to vpc.  Use vpc as index to stat record
            */
 
            vpc = atm_get_vpc(vci, vpi);
            if (aalsap_is_vpc_active(vpc) == TRUE) {
 
                acp_pTxSpram->txstat_vc[vpc].txvc_ntccell_output +=   
                    p_ntc_dma_ch0_blk->user_2cell_stats[i].txcell_count2;
 
                acp_pRxSpram->rxstat_vc[vpc].rxvc_ntccell_input +=
                    p_ntc_dma_ch0_blk->user_2cell_stats[i].rxcell_count2;
#ifdef COMMENTED_OUT
            	buginf("vpi: %d vci: %d rxcell_count: %d txcell_count: %d\n",
            		vpi, vci,
            	p_ntc_dma_ch0_blk->user_2cell_stats[i].rxcell_count2,
            	p_ntc_dma_ch0_blk->user_2cell_stats[i].txcell_count2);
#endif /* COMMENTED_OUT */
            }

      }/* if second vc active */


	}/* for all 16 vc pairs in nsr record */
}


/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_isr: Process NTC Interrupt
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/
 
void
ntc_isr()
{
	UINT32 regval;
	UINT32 intr_type;
	UINT32 int_stat;

	int_stat = spl7();
	
	/* Read reason(s) for interrupt */
	mREAD_NTC(NTC_INTERRUPT_STATUS, intr_type);

	/* Clear interrupt bits; and inhibit further interrupts of associated type */
	mWRITE_NTC(NTC_INTR_UNDERSERVICE, intr_type);

	/* Handle Interrupt */
	if ( intr_type & DMA_INTR ) {
		ntc_dma_isr();
	} 
	
#ifdef PRINT_DEBUG
	if ( intr_type & BUFOV_INTR ) {
		buginf("\nNTC BUFFER OVERFLOW!!\n");
		mREAD_NTC(NTC_BUFOVERFLOW_INTR, regval);
		buginf("\nNTC Status Register 61 : 0x%x", regval);
	} 
#endif /* PRINT_DEBUG */
	if( (intr_type & OCD_INTR) ||
	   (intr_type & ICD_INTR)  ) {
		/* 
		 * Since these interrupts are unreliable (i.e. OCD instead of ICD etc.)
		 * We just use them as in indication that something has changed. To
		 * find the link state, we poll SR 59 (Physical Layer Alarms Received)
		 * to determine the actual link state.
		 */
		MFP_Poll_mSec(10);
		mREAD_NTC(NTC_PHY_ALARM, regval);
		if (regval & (B1BIP_ALARM_MASK|B2BIP_ALARM_MASK)) {
			ntc_link_state = LOSS_OF_FRAMING;
      		set_led(ACP_WRRG_PMD_BOT_LED, ACP_PMD_LEVL_LED_OFF);
		} else {
			ntc_link_state = IN_FRAMING;
			set_led(ACP_WRRG_PMD_BOT_LED, ACP_PMD_LEVL_LED_GRN);
		}
		/* Inform ATM driver of line state change */
		ntc_line_state_change_upcall(ntc_link_state);

#ifdef PRINT_DEBUG
		if (intr_type & OCD_INTR)
			buginf("\n Received OCD Intr");
		if (intr_type & ICD_INTR)
			buginf("\n Received ICD Intr");
		buginf("\nB1B2B3ERR: 0x%x", 
			    regval & (B1BIP_ALARM_MASK|B2BIP_ALARM_MASK));
#endif /* PRINT_DEBUG */
	}

	if ( intr_type & BERR_INTR ) {
#ifdef PRINT_DEBUG
		buginf("\nntc_isr: Bus Error During DMA");
#endif /* PRINT_DEBUG */
	}

	/* At this time, all other interrupts are not enabled, so ignore */

	/* Now enable further interrupts (of same type) to be generated */ 
	/* Control register 59: Clear any pending interrupts */
	regval = 0;
	mWRITE_NTC(NTC_INTR_UNDERSERVICE, regval);

	/* sakumar 7/11/95: On advice from Fujitsu, write to Reg 62 if DMA Intr */
	if ( intr_type & DMA_INTR ) {
		/* Control register 62: DMA channel activity */
    	regval = CHANNEL0_ENABLE;
    	mWRITE_NTC(NTC_DMA_ACTIVITY, regval);
	} 

	splx(int_stat);
}


 
/*
*******************************************************************************
*
*   FUNCTION:
*           ntc_control
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/

UINT32  
ntc_control(UINT32  opcode)
{
UINT32	regval = 0;

	switch(opcode) {

	case NTC_FRAMER_LOOPBACK_ON:

		if (ntc_hw_rev_b) {
			regval = (FRAMER_LOOPBACK | XMODE_SONET_STS3C | STMSCRABLER_ENABLE |
                      STMDESCRABLER_ENABLE | FRAME_SYNCFOUND |
                      FRAME_SYNCLOST);
	    } else {
			regval = (FRAME_ENABLE | FRAMER_LOOPBACK | XMODE_SONET_STS3C | 
					  STMSCRABLER_ENABLE | STMDESCRABLER_ENABLE | FRAME_SYNCFOUND |
                      FRAME_SYNCLOST);
		}

#ifdef PRINT_DEBUG 
		printf("\n\r ntc_control: ntc_loopback_on NTC_FRAMER_CONTROL:%x",
		regval);
#endif /* PRINT_DEBUG */
    	mWRITE_NTC(NTC_FRAMER_CONTROL, regval);

		if (ntc_hw_rev_b) {
			regval = (NTC_TX_FRAMER_ENABLE | NTC_RX_FRAMER_ENABLE);
            mWRITE_NTC(NTC_FRAMER_ENABLE_CONTROL, regval);
		}

		break;

	case NTC_CSI_LOOPBACK_ON:
		if (ntc_hw_rev_b) {
	    	/* Control register 57 */
        	regval = (RXHEC_ENABLE | RXHECMASK_ENABLE | CSI_LOOPBACK_ENABLE_B);
        	mWRITE_NTC(NTC_CELLSTREM_CONTROL2, regval);
		} else {
			/* Control register 56 */
        	regval = (CELL_STREAM_ENABLE | CSI_LOOPBACK_ENABLE_A | RXATC_DISABLED |
                      TXATC_DISABLED );
#ifdef PRINT_DEBUG 
        	printf("\n\r ntc_control: ntc_loopback_on NTC_CELLSTREM_CONTROL1:%x",
        	regval);
#endif /* PRINT_DEBUG */
        	mWRITE_NTC(NTC_CELLSTREM_CONTROL1, regval);
		}

		break;

	case NTC_CSI_LOOPBACK_OFF:
		if (ntc_hw_rev_b) { 
			/* Control register 57 */
        	regval = (RXHEC_ENABLE | RXHECMASK_ENABLE);
        	mWRITE_NTC(NTC_CELLSTREM_CONTROL2, regval);
		} else {
			/* Control register 56 */
        	regval = (CELL_STREAM_ENABLE |  CSI_LOOPBACK_DISABLE_A | RXATC_DISABLED |
                      TXATC_DISABLED );
			mWRITE_NTC(NTC_CELLSTREM_CONTROL1, regval);
  		}
		break;

	case NTC_RXTX_FILL:

		mREAD_NTC(NTC_CSI_RXTXC_BUFFFILL, regval);
#ifdef PRINT_DEBUG 
		printf("\n\r ntc_control: read RXTX_FILL levels=%x", regval);
#endif /* PRINT_DEBUG */
		break;

	case NTC_OAM_ALARMS:

		mREAD_NTC(NTC_PHY_ALARM, regval);
#ifdef PRINT_DEBUG 
		printf("\n\r ntc_control: read NTC_PHY_ALARM=%x", regval);
#endif /* PRINT_DEBUG */

		mREAD_NTC(NTC_INTERRUPT_STATUS, regval);
#ifdef PRINT_DEBUG 
		printf("\n\r ntc_control: read NTC_INTERRUPT_STATUS=%x", regval);
#endif /* PRINT_DEBUG */

		if(regval) {
			mWRITE_NTC(NTC_INTR_UNDERSERVICE, 0);
		}
		mREAD_NTC(NTC_OAM_ALARM_CONTROL, regval);
#ifdef PRINT_DEBUG 
		printf("\n\r ntc_control: read NTC_OAM_ALARM_CONTROL=%x", regval);
#endif /* PRINT_DEBUG */

		break;

	case NTC_RECV_FRAME:
		mREAD_NTC(NTC_RECVFRAMER_BYTE1, regval);
#ifdef PRINT_DEBUG 
		printf("\n\r ntc_control: read NTC_RECV_FRAME C1, R1=%x", regval);
#endif /* PRINT_DEBUG */
		mREAD_NTC(NTC_RECVFRAMER_BYTE2, regval);
#ifdef PRINT_DEBUG 
		printf("\n\r ntc_control: read NTC_RECV_FRAME R2=%x", ((regval&0xff00) >> 8) );
#endif /* PRINT_DEBUG */
		
	}

}/*end ntc_control */


int
ntc_get_framer_status()
{
	return (ntc_link_state);
}

void
ntc_Reg_line_state_change_upcall(void * pFunc)
{
	ntc_line_state_change_upcall = (tNtc_line_state_upcall *) pFunc;
}
