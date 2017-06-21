/* $Id: mfp_acp_defs.h,v 1.1.4.1 1996/05/09 14:55:42 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/mfp_acp_defs.h,v $
 *-----------------------------------------------------------------
 * mfp_acp_defs.h -- ACP 68901 Multi-Function Peripheral
 *                   target-specific definitions header file.  We
 *                   describe ACP-specific MFP registers, addresses,
 *                   and bit-patterns here.
 *
 * November 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mfp_acp_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:42  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:11  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:08  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:20  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:13  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:40  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__MFP_ACP_DEFS_H
#define	__MFP_ACP_DEFS_H

	/* ============================================================	*/
	/*	This is the ACP 68901 Multi-Function Peripheral		*/
	/*	target-specific definitions header file.  We describe	*/
	/*	ACP-specific MFP registers, addresses, and bit-patterns	*/
	/*	here.							*/
	/*								*/
	/*	By Rick Lowe, cisco, 94-11-01.				*/
	/*	This is:  %W% %G%	*/
	/* ============================================================	*/

#ifndef	__MFP_HW_DEFS_H
#include "mfp_hw_defs.h"
#endif

#define	CONSOLE			(0)		/* Ids console to SCC layer. */

	/* ============================================================	*/
	/*	Useful definitions for the MFP Vector Register.		*/
	/* ============================================================	*/

#define	MFP_TOT_VECS_USED	(16)		/* From MFP HW.  */
#define	MFP_BASE_VECTOR		(64)		/* Chosen by us. */
#define	MFP_UNINIT_VECTOR	(80)		/* Chosen by us. */
#define	MFP_CPU_INTR_LEVEL	(2)		/* From ALC HW.  */

	/* ============================================================	*/
	/*	Useful definitions for MFP GPDR/AER/DDR.		*/
	/* ============================================================	*/

#define	MFP_GPI_TSAR		(MFP_GPIP0_BIT)
#define	MFP_GPI_RSAR		(MFP_GPIP1_BIT)
#define	MFP_GPI_LCP_MP_		(MFP_GPIP2_BIT)
#define	MFP_GPI_LCP_MR_		(MFP_GPIP3_BIT)
#define	MFP_GPI_NTC_		(MFP_GPIP4_BIT)
#define	MFP_GPI_MFGJMP_		(MFP_GPIP7_BIT)

#define	MFP_TGT_DDR		(0x00)		/* All GPIO pins input. */
#define	MFP_TGT_AER		(0x03)		/* GPIO1-0 use rising edge.  */
						/* GPIO4-2 use falling edge. */
#define	MFP_TGT_GPDR		(0x00)		/* Set out pins to zero.*/

	/* ============================================================	*/
	/*	Register bit definitions for Interrupt Enable Register	*/
	/*	A/B (IERA/B), Interrupt Mask Register A/B (IMRA/B),	*/
	/*	Interrupt Pending Register A/B (IPRA/B), Interrupt	*/
	/*	In-Service Register A/B (ISRA/B).			*/
	/* ============================================================	*/

#define	MFP_IINDX_TIC_OS	(MFP_IINDX_TMRA)
#define	MFP_IINDX_TIC_BT	(MFP_IINDX_TMRB)
#define	MFP_IINDX_NTC		(MFP_IINDX_GPI4)
#define	MFP_IINDX_TIC_HWT	(MFP_IINDX_TMRC)
#define	MFP_IINDX_BRG		(MFP_IINDX_TMRD)
#define	MFP_IINDX_LCPRD		(MFP_IINDX_GPI3)
#define	MFP_IINDX_LCPTP		(MFP_IINDX_GPI2)
#define	MFP_IINDX_RSAR		(MFP_IINDX_GPI1)
#define	MFP_IINDX_TSAR		(MFP_IINDX_GPI0)

#define	MFP_IMSK_NTC		(1 << MFP_IINDX_NTC)
#define	MFP_IMSK_LCPRD		(1 << MFP_IINDX_LCPRD)
#define	MFP_IMSK_LCPTP		(1 << MFP_IINDX_LCPTP)
#define	MFP_IMSK_RSAR		(1 << MFP_IINDX_RSAR)
#define	MFP_IMSK_TSAR		(1 << MFP_IINDX_TSAR)

	/* ============================================================	*/
	/*	Useful definitions for calculating timing constants.	*/
	/* ------------------------------------------------------------	*/
	/*	Prescale is selected as					*/
	/*		ACP_HW_MFP_CLK_HZ/<period>/MFP_TMR_MAX_COUNTER,	*/
	/*	where <period> is number of 4/10 ms periods per sec,	*/
	/*	and MAX_COUNTER is 256 for max 8-bit timer counter.	*/
	/* ------------------------------------------------------------	*/
	/*	Each timer approximates desired 4/10 ms period due to	*/
	/*	integer arithmetic.  Error term is number of MFP clocks	*/
	/*	SHORT per second.  Correction term is number of timer	*/
	/*	interrupts (N) at which point an integer number of	*/
	/*	timer periods can be adjusted to fix the Error.  The	*/
	/*	fix is usually 1 period, ie. every 576 interrupts only	*/
	/*	575 periods have actually elapsed.			*/
	/* ============================================================	*/

#define	ACP_HW_MFP_CLK_HZ	(3686400)	/* Clock input freq. to MFP. */

#define	MFP_STD_10MS_TICK	(10)		/* msec per 10 msec tick.    */
#define	MFP_NBR_4MS_PER_SEC	(1000/4)	/* Nbr 4 ms periods per sec. */
#define	MFP_NBR_10MS_PER_SEC	(1000 / MFP_STD_10MS_TICK)
						/* Nbr 10 ms periods per sec.*/

	/* ------------------------------------------------------------	*/
	/*	Timer A is 4 ms tick for IOS.  Expect:			*/
	/*		prescale	64				*/
	/*		counter		230				*/
	/*		error		6400				*/
	/*		correction	576, by -1			*/
	/* ------------------------------------------------------------	*/
#define	MFP_4MS_TMR_PSCALV	(64)
#define	MFP_4MS_TMR_PSCALE	(MFP_TMR_PSCALE_64)
#define	MFP_4MS_TMR_COUNTER	(ACP_HW_MFP_CLK_HZ/MFP_NBR_4MS_PER_SEC/MFP_4MS_TMR_PSCALV)
#define	MFP_4MS_TMR_ERROR	(ACP_HW_MFP_CLK_HZ - (MFP_4MS_TMR_PSCALV*MFP_4MS_TMR_COUNTER*MFP_NBR_4MS_PER_SEC))
#define	MFP_4MS_TMR_CORREC_CNT	(ACP_HW_MFP_CLK_HZ/MFP_4MS_TMR_ERROR)
#define	MFP_4MS_TMR_CORREC_ADJ	(1)
#define	MFP_4MS_TMR_BITMASK	(1 << MFP_IINDX_TIC_OS)
#define	MFP_4MS_TMR_C_REG	(MFP_RO_TACR)
#define	MFP_4MS_TMR_D_REG	(MFP_RO_TADR)

	/* ------------------------------------------------------------	*/
	/*	Timer B is 10 ms tick for Boot.  Expect:		*/
	/*		prescale	200				*/
	/*		counter		184				*/
	/*		error		6400				*/
	/*		correction	576, by -1			*/
	/* ------------------------------------------------------------	*/
#define	MFP_10MS_TMR_PSCALV	(200)
#define	MFP_10MS_TMR_PSCALE	(MFP_TMR_PSCALE_200)
#define	MFP_10MS_TMR_COUNTER	(ACP_HW_MFP_CLK_HZ/MFP_NBR_10MS_PER_SEC/MFP_10MS_TMR_PSCALV)
#define	MFP_10MS_TMR_ERROR	(ACP_HW_MFP_CLK_HZ - (MFP_10MS_TMR_PSCALV*MFP_10MS_TMR_COUNTER*MFP_NBR_10MS_PER_SEC))
#define	MFP_10MS_TMR_CORREC_CNT	(ACP_HW_MFP_CLK_HZ/MFP_10MS_TMR_ERROR)
#define	MFP_10MS_TMR_CORREC_ADJ	(1)
#define	MFP_10MS_TMR_BITMASK	(1 << MFP_IINDX_TIC_BT)
#define	MFP_10MS_TMR_C_REG	(MFP_RO_TBCR)
#define	MFP_10MS_TMR_D_REG	(MFP_RO_TBDR)

	/* ------------------------------------------------------------	*/
	/*	Timer C is used to check for HW (driver) timeouts.	*/
	/*	Its data are identical to that for Timer-B, 10 msec.	*/
	/* ------------------------------------------------------------	*/
#define	MFP_HWTOUT_TMR_BITMASK	(1 << MFP_IINDX_TIC_HWT)
#define	MFP_HWTOUT_TMR_C_REG	(MFP_RO_TCDCR)
#define	MFP_HWTOUT_TMR_D_REG	(MFP_RO_TCDR)

	/* ------------------------------------------------------------	*/
	/*	Timer D is 9600 Baud Rate Generator for MFP UART.	*/
	/* ------------------------------------------------------------	*/
#define	MFP_BAUD_9600		(9600)
#define	MFP_BRG_TMR_BITMASK	(1 << MFP_IINDX_BRG)

	/* ============================================================	*/
	/*	Size of UART Rx FIFO.					*/
	/* ============================================================	*/

#define	MFP_UART_RXDEPTH	(MFP_RSR_DEPTH)

	/* ============================================================	*/
	/*	Following definitions are for Catalyst portability.	*/
	/* ============================================================	*/

#define	ASYNC_1_STOP_BIT	(MFP_UCR_CHSTOP_1)
#define	CLK_16			(MFP_UCR_CLK_16X)
#define	CLK_8_BIT_CHARS		(MFP_UCR_CHLEN_8)
#define	EN_RCV_BUF_FULL		(MFP_INTR_R_FUL)
#define	EN_RCV_ERROR		(MFP_INTR_R_ERR)
#define	EN_XMIT_BUF_EMPTY	(MFP_INTR_T_EMP)
#define	EN_XMIT_ERROR		(MFP_INTR_T_ERR)
#define	RCV_BUFFER_FULL		(MFP_RSR_BUFFER_FULL)
#define	RCV_ERROR		(MFP_RSR_RX_ERR_MSK)
#define	RECEIVER_ENABLE		(MFP_RSR_RX_ENABLE)
#define	TRANSMITTER_ENABLE	(MFP_TSR_TX_ENABLE)
#define	XMIT_BUFFER_EMPTY	(MFP_TSR_BUF_EMPTY)
#define	XMIT_EXCEPTION		(MFP_TSR_TX_EXC_MSK)
#define	XMIT_HIGH		(MFP_TSR_TXSO_HIGH)

#endif	/* __MFP_ACP_DEFS_H */
